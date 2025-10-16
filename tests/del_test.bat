@echo off
Rem Test file for DEL command

echo Testing DEL command functionality
echo =================================

Rem Create test directories and files for DEL command testing
mkdir test_del_dir 2>nul
echo Test content for deletion > test_del_dir\file1.txt
echo Another file for deletion > test_del_dir\file2.txt
echo Temporary file > temp_file.tmp

Rem Test 1: Delete a single file
echo Test 1: Deleting a single file
if exist temp_file.tmp (
    del temp_file.tmp
    if not exist temp_file.tmp (
        echo File deleted successfully
    ) else (
        echo Error: File deletion failed
    )
) else (
    echo File temp_file.tmp does not exist
)
echo.

Rem Test 2: Delete a specific file in a directory
echo Test 2: Deleting specific file in directory
if exist test_del_dir\file1.txt (
    del test_del_dir\file1.txt
    if not exist test_del_dir\file1.txt (
        echo File deleted from directory successfully
    ) else (
        echo Error: File deletion from directory failed
    )
) else (
    echo File test_del_dir\file1.txt does not exist
)
echo.

Rem Test 3: Delete all txt files in directory (with confirmation)
echo Test 3: Deleting all txt files in directory
del /q test_del_dir\*.txt 2>nul
if not exist test_del_dir\file2.txt (
    echo All txt files deleted successfully
) else (
    echo Error: Some txt files still exist
)
echo.

Rem Cleanup: Remove test directory
rmdir /s /q test_del_dir

Rem End of del tests
echo All DEL command tests completed successfully