@echo off
Rem Test file that will fail

echo Testing a failing test
echo =====================================

Rem This command should fail
invalid_command_that_does_not_exist

Rem If we reach this line, the test continues, but it should have failed before
echo This should not be printed if the previous command failed