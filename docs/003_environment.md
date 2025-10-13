# Batch Script - Environment

---



---

This chapter explains the environment related to Batch Script.

## Writing and Executing

Typically, to create a batch file, notepad is used. This is the simplest tool for creation of batch files. Next is the execution environment for the batch scripts. On Windows systems, this is done via the command prompt or cmd.exe. All batch files are run in this environment.

Following are the different ways to launch cmd.exe −

**Method 1** − Go to C:\Windows\System32 and double click on the cmd file.

![Writing Executing Method1](/batch_script/images/writing_executing_method1.jpg)

**Method 2** − Via the run command The following snapshot shows to find the command prompt(cmd.exe) on Windows server 2012.

![Writing Executing Method2](/batch_script/images/writing_executing_method2.jpg)

Once the cmd.exe is launched, you will be presented with the following screen. This will be your environment for executing your batch scripts.

![Batch Scripting Environment](/batch_script/images/batch_script_environment.jpg)

## Environment Variables

In order to run batch files from the command prompt, you either need to go to the location to where the batch file is stored or alternatively you can enter the file location in the path environment variable. Thus assuming that the batch file is stored in the location `C:\Application\bin`, you would need to follow these instructions for the PATH variable inclusion.

| OS | Output |
| --- | --- |
| Windows | Append the String; `C:\Application\bin` to the end of the system variable PATH. |

