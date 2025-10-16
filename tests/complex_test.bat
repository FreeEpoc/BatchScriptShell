@echo off
Rem Complex batch script test combining multiple commands

echo Running Complex Batch Script Test
echo ================================

Rem Create a test directory structure
echo 1. Creating test directory structure...
md complex_test_dir
cd complex_test_dir

Rem Create multiple test files
echo 2. Creating test files...
echo This is file 1 > file1.txt
echo This is file 2 > file2.txt
echo This is file 3 > file3.doc
md subdirectory
echo Content for subdirectory > subdirectory\subfile.txt

Rem List the current directory
echo 3. Current directory contents:
dir

Rem Set an environment variable
set TEST_DIR=%CD%
echo 4. Current test directory is: %TEST_DIR%

Rem Copy files
echo 5. Copying files...
copy file1.txt file1_backup.txt
if exist file1_backup.txt echo Backup file created successfully

Rem Rename a file
ren file2.txt file2_renamed.txt
if exist file2_renamed.txt echo File renamed successfully

Rem List files again to verify changes
echo 6. Directory contents after operations:
dir

Rem Pause for user confirmation (in a real test this might be skipped)
rem echo 7. Pausing for user confirmation...
rem pause

Rem Go back to original directory and remove test directory
cd ..
rmdir /s /q complex_test_dir

echo.
echo Complex batch script test completed successfully!

Rem Ensure termination
exit /b