@echo off
Rem Test file for DIR command

echo Testing DIR command functionality
echo =================================

Rem Create test directory and files for DIR command testing
mkdir test_dir 2>nul
echo Test file 1 > test_dir\file1.txt
echo Test file 2 > test_dir\file2.doc
mkdir test_dir\subdir 2>nul
echo Subdir file > test_dir\subdir\subfile.txt

Rem Test 1: Basic directory listing
echo Test 1: Basic directory listing
dir test_dir
echo.

Rem Test 2: Directory listing with specific extension
echo Test 2: Directory listing with *.txt extension
dir test_dir\*.txt
echo.

Rem Test 3: Recursive directory listing
echo Test 3: Recursive directory listing
dir test_dir /s
echo.

Rem Test 4: Brief format directory listing (paths only)
echo Test 4: Brief format directory listing
dir test_dir /s /b
echo.

Rem Test 5: Directory listing with all file attributes
echo Test 5: Directory listing with all file attributes
dir test_dir /a
echo.

Rem Cleanup: Remove test directory
rmdir /s /q test_dir

Rem End of dir tests
echo All DIR command tests completed successfully