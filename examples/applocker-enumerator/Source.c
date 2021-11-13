#include <windows.h>
#include <stdio.h>
#include "AppLockerPolicy.h"
#include "bofdefs.h"



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

BOOL GetAppLockerPolicies(LPBSTR pbstrPolicies) {
	BOOL bResult = FALSE;
	HRESULT hr = S_FALSE;
	CLSID xCLSID_AppIdPolicyHandler;
	IID xIID_AppIdPolicyHandler;

	wchar_t* T_IIDIAppIdPolicyHandler = L"{B6FEA19E-32DD-4367-B5B7-2F5DA140E87D}";
	wchar_t* T_CLSIDAppIdPolicyHandler = L"{F1ED7D4C-F863-4DE6-A1CA-7253EFDEE1F3}";

	hr = CLSIDFromString(T_CLSIDAppIdPolicyHandler, &xCLSID_AppIdPolicyHandler);
	if (hr != S_OK)
		return print_error("CLSIDFromString", hr);

	hr = IIDFromString(T_IIDIAppIdPolicyHandler, &xIID_AppIdPolicyHandler);
	if (hr != S_OK)
		return print_error("IIDFromString", hr);

	// Get the COM interface
	IAppIdPolicyHandler* pIAppIdPolicyHandler = (IAppIdPolicyHandler*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(IAppIdPolicyHandler));
	hr = CoCreateInstance(&xCLSID_AppIdPolicyHandler, NULL, CLSCTX_INPROC_SERVER, &xIID_AppIdPolicyHandler, (LPVOID*)&pIAppIdPolicyHandler);
	if (hr != S_OK || pIAppIdPolicyHandler == NULL) {
		print_error("CoCreateInstance", hr);
		goto failure;
	}


	hr = pIAppIdPolicyHandler->lpVtbl->GetEffectivePolicy(pIAppIdPolicyHandler, pbstrPolicies);

	// Check if an error occurred
	if (hr != S_OK || *pbstrPolicies == NULL) {
		print_error("GetEffectivePolicy", hr);
		goto failure;
	}
		
	bResult = TRUE;
failure:
	if (pIAppIdPolicyHandler) {
		pIAppIdPolicyHandler->lpVtbl->Release(pIAppIdPolicyHandler);
		pIAppIdPolicyHandler = NULL;
	}

	return bResult;
}



#ifdef BOF
void go(char* buff, int len) {

	BSTR bstrAppLockerPolicies = NULL;

	if (!com_init())
		return;

	if (!GetAppLockerPolicies(&bstrAppLockerPolicies))
		return;

	BeaconPrintf(CALLBACK_OUTPUT, "AppLocker policies: %S", bstrAppLockerPolicies);
	CoUninitialize();
}


#else

void main(int argc, char* argv[]) {

	BSTR bstrAppLockerPolicies = NULL;

	if (!com_init())
		return;

	if (!GetAppLockerPolicies(&bstrAppLockerPolicies))
		return;

	printf("AppLocker policies: \n%ws\n", bstrAppLockerPolicies);

	CoUninitialize();
}

#endif