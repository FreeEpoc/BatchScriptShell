# Batch Script - ECHO

---



---

## ECHO

This batch command displays messages, or turns command echoing on or off.

## Syntax

```
ECHO string
```

## Example

The following example shows the different variants of the dir command.

```
Rem Turns the echo on so that each command will be shown as executed 
echo on 
echo "Hello World" 

Rem Turns the echo off so that each command will not be shown when executed 
@echo off 
echo "Hello World" 

Rem Displays the contents of the PATH variable 
echo %PATH%
```

## Output

The following output will be displayed in the command prompt.

```
C:\>Rem Turns the echo on so that each command will be shown as executed

C:\>echo on

C:\>echo "Hello World"
"Hello World"

C:\>Rem Turns the echo off so that each command will not be shown when executed

"Hello World"
C:\Users\ADMINI~1\AppData\Local\Temp
```

batch\_script\_commands.htm

