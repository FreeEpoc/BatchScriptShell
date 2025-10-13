# Batch Script - Files

---



---

In this chapter, we will learn how to create, save, execute, and modify batch files.

## Creating Batch Files

Batch files are normally created in notepad. Hence the simplest way is to open notepad and enter the commands required for the script. For this exercise, open notepad and enter the following statements.

```
:: Deletes All files in the Current Directory With Prompts and Warnings 
::(Hidden, System, and Read-Only Files are Not Affected) 
:: 
@ECHO OFF 
DEL . 
DR
```

## Saving Batch Files

After your batch file is created, the next step is to save your batch file. Batch files have the extension of either .bat or .cmd. Some general rules to keep in mind when naming batch files −

* Try to avoid spaces when naming batch files, it sometime creates issues when they are called from other scripts.
* Dont name them after common batch files which are available in the system such as ping.cmd.

![Saving Batch Files](imgs/saving_batch_files.jpg)

The above screenshot shows how to save the batch file. When saving your batch file a few points to keep in mind.

* Remember to put the .bat or .cmd at the end of the file name.
* Choose the Save as type option as All Files.
* Put the entire file name in quotes .

## Executing Batch Files

Following are the steps to execute a batch file −

* **Step 1** − Open the command prompt (cmd.exe).
* **Step 2** − Go to the location where the .bat or .cmd file is stored.
* **Step 3** − Write the name of the file as shown in the following image and press the Enter button to execute the batch file.

![Executing Batch Files](imgs/executing_batch_files.jpg)

## Modifying Batch Files

Following are the steps for modifying an existing batch file.

* **Step 1** − Open windows explorer.
* **Step 2** − Go to the location where the .bat or .cmd file is stored.
* **Step 3** − Right-click the file and choose the Edit option from the context menu. The file will open in Notepad for further editing.

![Modifying Batch Files](imgs/modifying_batch_files.jpg)

