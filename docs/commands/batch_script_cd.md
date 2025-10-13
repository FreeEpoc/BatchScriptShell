# Batch Script - CD

---



---

This batch command helps in making changes to a different directory, or displays the current directory.

## Syntax

```
cd
```

## Example

The following example shows how the cd command can be used in a variety of ways.

```
@echo off
Rem The cd without any parameters is used to display the current working directory
cd
Rem Changing the path to Program Files
cd\Program Files
cd
Rem Changing the path to Program Files
cd %USERPROFILE%
cd
Rem Changing to the parent directory
cd..
cd
Rem Changing to the parent directory two levels up
cd..\..
cd
```

## Output

The above command will display the following output after changing to the various folder locations.

```
C:\Users\Administrator
C:\Program Files
C:\Users\Administrator
C:\Users
C:\
```

batch\_script\_commands.htm

