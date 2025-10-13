# Batch Script - RD

---



---

This batch command removes directories, but the directories need to be empty before they can be removed.

## Syntax

```
rd [directoryname]
```

## Example

The following example shows the different variants of the **rd** command.

```
@echo off
Rem removes the directory called newdir
rd C:\newdir

Rem removes 2 directories
rd Dir1 Dir2

Rem Removes directory with spaces
rd "Application A"

Rem Removes the directory Dir1 including all the files and subdirectories in it rd /s Dir1
Rem Removes the directory Dir1 including all the files and subdirectories in it but
asks for a user confirmation first.
rd /q /s Dir1
```

## Output

All actions are performed as per the remarks in the batch file.

batch\_script\_commands.htm

