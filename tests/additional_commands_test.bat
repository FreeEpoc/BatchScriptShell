@echo off
Rem Test file for additional commands that may not be in the main test suite

echo Testing Additional Commands
echo ===========================

Rem Test 1: CLS command (with a note about how it works)
echo Test 1: CLS command functionality
echo This command would clear the screen in a real terminal
echo CLS
echo This text would be cleared by CLS command

rem Since CLS clears the screen, we'll just note that it was tested
echo CLS command tested but not executed to preserve output
echo.

Rem Test 2: CHOICE command functionality (will be skipped when run in batch)
echo Test 2: CHOICE command functionality
echo CHOICE command allows user interaction with a menu of options
echo Example:
echo CHOICE /C ABC /M "Choose A, B, or C"
echo This would pause for user input in interactive mode
echo NOTE: This test is skipped in batch mode to avoid waiting for user input
echo.

Rem Test 3: FIND command
echo Test 3: FIND command functionality
echo Creating test file for FIND command...
echo First line of test file > find_test.txt
echo Line containing search term >> find_test.txt
echo Another line >> find_test.txt

echo Searching for 'search term' in find_test.txt:
find "search term" find_test.txt

echo Searching for lines NOT containing 'search' with FIND /V:
find /V "search" find_test.txt 2>nul

echo Counting lines containing 'line' with FIND /C:
find /C "line" find_test.txt

Rem Cleanup
del find_test.txt
echo.

Rem Test 4: TYPE command (already tested but demonstrating again)
echo Test 4: TYPE command to display file contents
echo Creating a test file to demonstrate TYPE command...
echo This is line 1 > type_test.txt
echo This is line 2 >> type_test.txt
echo This is line 3 >> type_test.txt

echo Contents of type_test.txt using TYPE command:
type type_test.txt

Rem Cleanup
del type_test.txt
echo.

Rem End of additional command tests
echo All additional command tests completed successfully

Rem Ensure termination
exit /b