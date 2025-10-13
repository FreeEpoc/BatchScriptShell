# Batch Script - DEL

---



---

This batch command deletes files and not directories.

## Syntax

```
del [filename]
```

## Example

The following example shows the different variants of the **del** command.

```
@echo off 
Rem Deletes the file lists.txt in C:\ 
del C:\lists.txt 
Rem Deletes all files recursively in all nested directories
del /s *.txt 
Rem Deletes all files recursively in all nested directories , but asks for the 
confirmation from the user first 
Del /p /s *.txt
```

## Output

All actions are performed as per the remarks in the batch file.

batch\_script\_commands.htm

