@echo off
Rem Test file that will fail and explicitly exit

echo Testing a failing test with explicit exit
echo =========================================

Rem This command should fail
invalid_command_that_does_not_exist

Rem Check ERRORLEVEL to determine if last command failed
if errorlevel 1 (
  echo Command failed, exiting with error code
  exit /b 1
)

Rem If we reach this line, the test continues
echo Test completed successfully