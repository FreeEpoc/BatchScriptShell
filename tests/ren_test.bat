@echo off
Rem Test file for REN command

echo Testing REN command functionality
echo =================================

Rem Create test directories and files for REN command testing
mkdir test_ren_dir1 2>nul
echo Test content for renaming > test_ren_file1.txt

Rem Test 1: Rename a file
echo Test 1: Renaming a file
if exist test_ren_file1.txt (
    ren test_ren_file1.txt test_ren_file2.txt
    if exist test_ren_file2.txt (
        echo File renamed successfully
    ) else (
        echo Error: File rename failed
    )
) else (
    echo File test_ren_file1.txt does not exist
)
echo.

Rem Test 2: Rename a directory
echo Test 2: Renaming a directory
if exist test_ren_dir1 (
    ren test_ren_dir1 test_ren_dir2
    if exist test_ren_dir2 (
        echo Directory renamed successfully
    ) else (
        echo Error: Directory rename failed
    )
) else (
    echo Directory test_ren_dir1 does not exist
)
echo.

Rem Test 3: Rename file with spaces in name
echo Test 3: Renaming a file with spaces in name
echo Another test file > "test file with spaces.txt"
if exist "test file with spaces.txt" (
    ren "test file with spaces.txt" "renamed file with spaces.txt"
    if exist "renamed file with spaces.txt" (
        echo File with spaces renamed successfully
    ) else (
        echo Error: File with spaces rename failed
    )
) else (
    echo File "test file with spaces.txt" does not exist
)
echo.

Rem Cleanup: Remove test directories and files
rmdir /s /q test_ren_dir2 2>nul
del test_ren_file2.txt 2>nul
del "renamed file with spaces.txt" 2>nul

Rem End of ren tests
echo All REN command tests completed successfully