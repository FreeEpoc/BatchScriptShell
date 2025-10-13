# Batch Script - COPY

---



---

This batch command is used for copying files from one location to the other.

## Syntax

```
Copy [source] [destination]
```

The files will be copied from source to destination location.

## Example

The following example shows the different variants of the **copy** command.

```
@echo off
cd
Rem Copies lists.txt to the present working directory.
If there is no destination identified , it defaults to the present working directory.
copy c:\lists.txt
Rem The file lists.txt will be copied from C:\ to C:\tp location
copy C:\lists.txt c:\tp
Rem Quotation marks are required if the file name contains spaces
copy C:\My File.txt
Rem Copies all the files in F drive which have the txt file extension to the
current working directory copy
F:\*.txt
Rem Copies all files from dirA to dirB. Note that directories nested in dirA will not be copied
copy C:\dirA dirB
```

## Output

All actions are performed as per the remarks in the batch file.

batch\_script\_commands.htm

