# Batch Script - Syntax

---



---

Normally, the first line in a batch file often consists of the following command.

## ECHO Command

```
@echo off
```

By default, a batch file will display its command as it runs. The purpose of this first command is to turn off this display. The command "echo off" turns off the display for the whole script, except for the "echo off" command itself. The "at" sign "@" in front makes the command apply to itself as well.

## Documentation

Very often batch files also contains lines that start with the "Rem" command. This is a way to enter comments and documentation. The computer ignores anything on a line following Rem. For batch files with increasing amount of complexity, this is often a good idea to have comments.

## First Batch Script Program

Lets construct our simple first batch script program. Open notepad and enter the following lines of code. Save the file as List.cmd.

The code does the following −

* Uses the echo off command to ensure that the commands are not shown when the code is executed.
* The Rem command is used to add a comment to say what exactly this batch file does.
* The dir command is used to take the contents of the location C:\Program Files.
* The > command is used to redirect the output to the file C:\lists.txt.
* Finally, the echo command is used to tell the user that the operation is completed.

```
@echo off 
Rem This is for listing down all the files in the directory Program files 
dir "C:\Program Files" > C:\lists.txt 
echo "The program has completed"
```

When the above command is executed, the names of the files in C:\Program Files will be sent to the file C:\Lists.txt and in the command prompt the message The program has completed will be displayed.

