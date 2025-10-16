@echo off
Rem Test file for SET command

echo Testing SET command functionality
echo ==================================

Rem Test 1: Display all environment variables
echo Test 1: Displaying all environment variables
set
echo.
echo End of environment variables list
echo.

Rem Test 2: Set a new environment variable
echo Test 2: Setting a new environment variable
set TEST_VAR=Hello from SET command
echo The value of TEST_VAR is: %TEST_VAR%
echo.

Rem Test 3: Use set with a specific variable
echo Test 3: Using SET with a specific variable
set PATH
echo.
echo Above is the PATH environment variable
echo.

Rem Test 4: Set variable with spaces
echo Test 4: Setting variable with spaces
set "TEST_SPACE_VAR=This has spaces in it"
echo The value of TEST_SPACE_VAR is: "%TEST_SPACE_VAR%"
echo.

Rem Test 5: Arithmetic operations with SET /A
echo Test 5: Arithmetic operations with SET /A
set /A RESULT=5+3
echo 5+3=%RESULT%
set /A RESULT_MUL=4*6
echo 4*6=%RESULT_MUL%
set /A RESULT_DIV=20/4
echo 20/4=%RESULT_DIV%
echo.

Rem End of set tests
echo All SET command tests completed successfully