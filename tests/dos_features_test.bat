@echo off
echo Batch Script Shell (BSS) - DOS Features Test Script
echo ================================================

echo.
echo 1. Testing FOR loop functionality:
FOR %%i IN (A,B,C,D) DO echo Item %%i

echo.
echo 2. Testing MD (mkdir) and RD (rmdir):
MD testdir
IF EXIST testdir (
  echo Directory created successfully
) ELSE (
  echo Directory creation failed
)

echo.
echo 3. Testing COPY command:
echo This is a test file > original.txt
copy original.txt copied.txt
IF EXIST copied.txt (
  echo File copied successfully
) ELSE (
  echo File copy failed
)

echo.
echo 4. Testing DEL command:
DEL original.txt copied.txt
IF NOT EXIST original.txt (
  echo Original file deleted successfully
) ELSE (
  echo Original file deletion failed
)
IF NOT EXIST copied.txt (
  echo Copied file deleted successfully
) ELSE (
  echo Copied file deletion failed
)

echo.
echo 5. Testing DATE and TIME commands:
DATE
TIME

echo.
echo 6. Testing DIR command:
DIR

echo.
echo 7. Testing TYPE command:
echo This is a test line > temp.txt
TYPE temp.txt
DEL temp.txt

echo.
echo 8. Testing FIND command:
echo First line > searchtest.txt
echo Second line >> searchtest.txt
echo Third line >> searchtest.txt
FIND "Second" searchtest.txt
DEL searchtest.txt

echo.
echo 9. Testing SORT command:
echo ccc > sorttest.txt
echo aaa >> sorttest.txt
echo bbb >> sorttest.txt
echo Before sorting:
TYPE sorttest.txt
echo After sorting:
TYPE sorttest.txt | SORT
DEL sorttest.txt

echo.
echo 10. Testing CHOICE command (skipped in batch mode to avoid user input)

echo.
echo 11. Testing pipe functionality with echo command:
echo Testing pipe functionality...
echo Hello World | FIND "World"

echo.
echo 12. Testing IF statement with file existence:
IF EXIST shell.c (
  echo shell.c file exists - IF statement working
) ELSE (
  echo shell.c file not found
)

echo.
echo 13. Testing SET command with arithmetic:
SET /A result=10+5
echo 10+5=%result%

SET /A mult=4*3
echo 4*3=%mult%

SET /A div=20/4
echo 20/4=%div%

echo.
echo 14. Testing environment variables:
SET PATH
SET OS

echo.
echo 15. Testing ECHO with ON/OFF:
ECHO This should be visible
@ECHO OFF
ECHO This should NOT be visible (command is suppressed by @)
@ECHO ON
ECHO Test script completed successfully!
ECHO ===================================

pause