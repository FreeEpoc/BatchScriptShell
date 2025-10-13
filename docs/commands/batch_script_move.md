# Batch Script - MOVE

---



---

This batch command moves files or directories between directories.

## Syntax

```
move [source] [destination]
```

The files will be copied from source to destination location.

## Example

The following example shows the different variants of the move command.

```
@echo off
Rem Moves the file list.txt to the directory c:\tp
move C:\lists.txt c:\tp
Rem Renames directory Dir1 to Dir2, assuming Dir1 is a directory and Dir2 does not exist. 
move Dir1 Dir2
Rem Moves the file lists.txt to the current directory.
move C:\lists.txt
```

## Output

All actions are performed as per the remarks in the batch file.

batch\_script\_commands.htm

