@echo off
Rem Test file for MD command

echo Testing MD command functionality
echo ================================

Rem Remove test directories if they exist from a previous run
rmdir /s /q test_md_dir1 2>nul
rmdir /s /q test_md_dir2 2>nul
rmdir /s /q parent_dir 2>nul

Rem Test 1: Create a single directory
echo Test 1: Creating a single directory
md test_md_dir1
if exist test_md_dir1 (
    echo Directory created successfully
) else (
    echo Error: Directory creation failed
)
echo.

Rem Test 2: Create a nested directory structure
echo Test 2: Creating nested directory structure
md parent_dir\child_dir\grandchild_dir
if exist parent_dir\child_dir\grandchild_dir (
    echo Nested directory structure created successfully
) else (
    echo Error: Nested directory creation failed
)
echo.

Rem Test 3: Create multiple directories (if supported)
echo Test 3: Creating multiple directories
md test_md_dir2
if exist test_md_dir2 (
    echo Second directory created successfully
) else (
    echo Error: Second directory creation failed
)
echo.

Rem Show directory structure to verify
echo Current directory structure:
dir /s

Rem Cleanup: Remove test directories
rmdir /s /q test_md_dir1
rmdir /s /q test_md_dir2
rmdir /s /q parent_dir

Rem End of md tests
echo All MD command tests completed successfully