### Background

The background behind the creation of this template and its requirement can be found in the accompanying blog post on [Securify's blog](https://www.securify.nl/en/blog/creating-cobalt-strike-bofs-with-visual-studio).

### Usage Instructions

#### Importing The Template to Visual Studio

To use the template, download and copy the latest zip file from the [releases page](https://github.com/securifybv/Visual-Studio-BOF-template/releases) into the project templates directory under My Documents. For Visual Studio 2019, the directory would be 

%UserProfile%\Documents\Visual Studio 2019\Templates\ProjectTemplates 

The template will be automatically loaded into Visual Studio the next time you run it. After a restart, you can search for “beacon” on the new project wizard to find the template.


![Templates Location](images/TemplatesLocation.png)




![](images/NewProjectWizard.png)



#### Default Structure

The structure of the solution is shown in the image below. The *Header Files* filter contains the two headers needed to develop BOFs. The first header is the beacon header provided by the Cobalt Strike team. The bofdefs header is a modified version of the bofdefs header from TrustedSec. Next, the *Resources* filter contains a PowerShell script that will strip the debugging symbols. This PowerShell script is based on the work of Matthew Graeber (@mattifestation) on his ObjDump script. Finally, the *Source Files* filter contains the source code of the BOF.

![project_structure](images/project_structure.png)



#### Using The Template

The template makes it as easy as copying the *main* code to the *go* function for BOFs that take no arguments. The image below shows that the code in *go* is identical to the code in *main*. The ability to reuse the code without further modification increases the efficiency of porting existing projects into BOFs and eliminates the unnatural syntax of dynamic function resolution. For BOFs that take arguments, only the argument parsing part will be different. A goal for next versions is to create generic argument parsing macros or functions to eliminate the need for any modification.

![Text](images/code1.png)


When the code is ready, it can be built for x64 and x86 BOFs at one go using the predefined build configuration inside the project template. In order to do so, go to Build -> Batch Build and tick the two architectures.

![](images/building.png)


In order to build for only one architecture, select the appropriate BOF build configuration from the dropdown menu and hit build, as shown in the image below.

![](images/building2.png)

Regardless of the method used to build the BOF, the result is a stripped BOF ready to be used with Cobalt Strike. 

![](images/output.png)



#### BOFs & COM Objects

Sometimes, especially when dealing with component object model (COM) objects, it is easier to use C++ instead of C. The template supports that as well. Simply rename the Source.c to Source.cpp and compile. You will be spared from name mangling because the template will automatically add *extern “C”* if C++ is used. However, you should not get your hopes up too high with developing C++ BOFs, as this is not yet fully supported by Cobalt Strike.

#### Error Reporting

The template comes with a built-in function to print errors in a meaningful manner for operators. The purpose is to provide the operators with sufficient information to understand why the BOF failed without revisiting the source code. The error message below is a screenshot from the Cobalt Strike console and provides the following information for the operators: the error occurred 

- Name function causing the error: create\_folder
- Line number: 164
- Description by developer: “failed to create directory”
- WIN32 error code: 123

![](images/errors.png)

#### Modifying the template

Different teams have different needs, and therefore this template might not be suitable for everyone. Modifying the template is easy. Just unzip the zip file, make your modifications to the files, and zip it again!

### Example projects

To show how this template can be used, I have included two sample BOFs. The first BOF is a port of @am0nsec AppLocker enumerator, which will dump the effective AppLocker policy as beacon output. The second example is a scheduled task persistence that relies heavily on COM. 

### Feature work

This template is an initial prototype and many more features can be added to it. For example, the features on my to-do list includes:

- Adding generic argument parsing that can be used by *main* and *go* without modifications
- Internal print that takes variadic arguments

Pull requests and ideas are also most welcome!

### Acknowledgments

- Big thanks to the [TrustedSec](https://www.trustedsec.com/) team and [@FreeFirex](https://twitter.com/freefirex2) for sharing the code of the situational awareness BOFs [here](https://github.com/trustedsec/CS-Situational-Awareness-BOF). This template relied heavily on their work.
- Paul ([@am0nsec](https://twitter.com/am0nsec)) for sharing his code and experience with undocumented COM objects [here](https://ntamonsec.blogspot.com/2020/08/applocker-policy-enumeration-in-c.html).
- Matthew ([@mattifestation](https://twitter.com/mattifestation)) for sharing his work on PowerShell ObjDump [here](https://www.powershellgallery.com/packages/PowerSploit/1.0.0.0/Content/PETools%5CGet-ObjDump.ps1).

### Author

This project is created and maintained by Yasser Alhazmi [@yas_o_h](https://twitter.com/Yas_o_h) / [Securify](https://www.securify.nl/en/)

