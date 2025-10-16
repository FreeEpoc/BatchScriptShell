@echo off
echo Testing Pipe Functionality
echo ==========================

echo Creating test files...
echo Apple > test1.txt
echo Banana >> test1.txt
echo Cherry >> test1.txt
echo Date >> test1.txt

echo.
echo Orange > test2.txt
echo Pear >> test2.txt
echo Quince >> test2.txt
echo Raspberry >> test2.txt

echo.
echo 1. Testing pipe with TYPE and FIND:
echo Looking for 'Banana' in test1.txt:
TYPE test1.txt | FIND "Banana"

echo.
echo 2. Testing pipe with TYPE and SORT:
echo Sorting contents of test2.txt:
TYPE test2.txt | SORT

echo.
echo 3. Testing pipe with multiple commands (if supported):
echo Counting lines in test1.txt (using FIND /C if available):
TYPE test1.txt | FIND /C /V ""

echo.
echo 4. Testing redirection and pipes together:
echo Creating sorted output file:
TYPE test1.txt | SORT > sorted_output.txt
echo Sorted output file created:
TYPE sorted_output.txt

echo.
echo 5. Testing FOR loops with file content:
FOR /F "tokens=*" %%a IN (test1.txt) DO (
  echo Processing item: %%a
)

echo.
echo 6. Testing MORE-like functionality:
echo Long text output for pagination testing:
FOR %%i IN (1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25) DO (
  echo Line %%i
)

echo.
echo Cleaning up test files...
DEL test1.txt
DEL test2.txt
DEL sorted_output.txt

echo.
echo Pipe functionality test completed!
echo ==================================

pause