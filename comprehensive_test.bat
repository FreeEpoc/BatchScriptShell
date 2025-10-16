@echo off
echo Comprehensive Test
echo ================
echo 1. Creating test directory
mkdir test_dir
cd test_dir
echo 2. Creating test files
echo File 1 > file1.txt
echo File 2 > file2.txt
echo 3. Checking file existence
if exist file1.txt echo File 1 exists
if not exist file1.txt echo File 1 does not exist
if exist file2.txt echo File 2 exists
if not exist file2.txt echo File 2 does not exist
echo 4. Copying files
copy file1.txt file1_copy.txt
if exist file1_copy.txt echo Copy successful
if not exist file1_copy.txt echo Copy failed
echo 5. Renaming files
ren file2.txt file2_renamed.txt
if exist file2_renamed.txt echo Rename successful
if not exist file2_renamed.txt echo Rename failed
if not exist file2.txt echo Original file deleted after rename

echo 6. Creating subdirectory
mkdir subdir
echo Content for subdirectory > subdir/subfile.txt
echo 7. Cleaning up
cd ..
rmdir /s /q test_dir
echo Cleanup completed

if not exist test_dir echo Directory successfully removed
echo 8. Test completed
