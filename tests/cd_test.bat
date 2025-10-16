@echo off
Rem Test file for CD command

echo Testing CD command functionality
echo =================================

Rem Test 1: Show current directory
echo Test 1: Display current directory
cd
echo.

Rem Test 2: Change to root directory
echo Test 2: Change to root directory
cd \
echo Current directory after 'cd \':
cd
echo.

Rem Test 3: Change to parent directory
echo Test 3: Change to parent directory
cd ..
echo Current directory after 'cd ..':
cd
echo.

Rem Test 4: Change to a specific directory using environment variable
echo Test 4: Change to USERPROFILE directory
cd %USERPROFILE%
echo Current directory after 'cd %%USERPROFILE%%':
cd
echo.

Rem Test 5: Return to original directory
echo Test 5: Return to original directory
cd %~dp0
echo Current directory after returning to script location:
cd
echo.

Rem End of cd tests
echo All CD command tests completed successfully