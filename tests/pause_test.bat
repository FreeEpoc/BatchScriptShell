@echo off
Rem Test file for PAUSE command

echo Testing PAUSE command functionality
echo ====================================

Rem Test 1: Basic pause command
echo Test 1: Basic PAUSE command
echo Before pause command - Press any key to continue...
pause
echo After pause command
echo.

Rem Test 2: Pause in a loop
echo Test 2: PAUSE command in a loop
for /L %%i in (1,1,2) do (
    echo Iteration %%i before pause
    echo Press any key to continue to next iteration...
    pause
    echo Completed iteration %%i
    echo.
)

Rem End of pause tests
echo All PAUSE command tests completed successfully