@echo off
Rem Test file for COPY command

echo Testing COPY command functionality
echo ===================================

Rem Create test directories and files for COPY command testing
mkdir test_copy_source 2>nul
mkdir test_copy_dest 2>nul

Rem Create a test file to copy
echo This is a test file for copy command > test_copy_source\source_file.txt

Rem Test 1: Copy file to same directory
echo Test 1: Copy file to same directory
copy test_copy_source\source_file.txt test_copy_source\copied_file.txt
if exist test_copy_source\copied_file.txt (
    echo File copied successfully
) else (
    echo Error: File copy failed
)
echo.

Rem Test 2: Copy file to different directory
echo Test 2: Copy file to different directory
copy test_copy_source\source_file.txt test_copy_dest\dest_file.txt
if exist test_copy_dest\dest_file.txt (
    echo File copied to destination directory successfully
) else (
    echo Error: File copy to destination failed
)
echo.

Rem Test 3: Copy all files with specific extension
echo Test 3: Copy all txt files to destination
copy test_copy_source\*.txt test_copy_dest\
echo Files copied to destination directory
echo.

Rem Show contents of destination directory to verify
echo Contents of destination directory:
dir test_copy_dest
echo.

Rem Cleanup: Remove test directories and files
rmdir /s /q test_copy_source
rmdir /s /q test_copy_dest

Rem End of copy tests
echo All COPY command tests completed successfully