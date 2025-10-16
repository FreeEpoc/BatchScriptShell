@echo off
Rem Test file for ECHO command

echo Testing ECHO command functionality
echo =====================================

Rem Test 1: Basic echo functionality
echo Test 1: Basic echo message
echo Hello World!
echo.

Rem Test 2: Echo with variable
echo Test 2: Echo with environment variable
echo Current PATH is: %PATH%
echo.

Rem Test 3: Echo on/off
echo Test 3: Echo on/off functionality
echo This command will be displayed
@echo off
echo This command will NOT be displayed (due to @ symbol)
echo.

Rem Test 4: Echo empty line
echo Test 4: Echo empty line
echo.
echo Above is an empty line
echo.

Rem End of echo tests
echo All ECHO command tests completed successfully