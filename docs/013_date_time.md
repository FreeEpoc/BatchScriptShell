# Batch Script - DATE and TIME

---



---

The date and time in DOS Scripting have the following two basic commands for retrieving the date and time of the system.

## DATE

This command gets the system date.

### Syntax

```
DATE
```

### Example

```
@echo off 
echo %DATE%
```

### Output

The current date will be displayed in the command prompt. For example,

```
Mon 12/28/2015
```

## TIME

This command sets or displays the time.

### Syntax

```
TIME
```

### Example

```
@echo off 
echo %TIME%
```

### Output

The current system time will be displayed. For example,

```
22:06:52.87
```

Following are some implementations which can be used to get the date and time in different formats.

## Date in Format Year-Month-Day

### Example

```
@echo off 
echo/Today is: %year%-%month%-%day% 
goto :EOF 
setlocal ENABLEEXTENSIONS 
set t = 2&if "%date%z" LSS "A" set t = 1 

for /f "skip=1 tokens = 2-4 delims = (-)" %%a in ('echo/^|date') do ( 
   for /f "tokens = %t%-4 delims=.-/ " %%d in ('date/t') do ( 
      set %%a=%%d&set %%b=%%e&set %%c=%%f)) 
endlocal&set %1=%yy%&set %2=%mm%&set %3=%dd%&goto :EOF
```

### Output

The above command produces the following output.

```
Today is: 2015-12-30
```

