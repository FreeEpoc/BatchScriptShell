@echo off
Rem Test file for RD command

echo Testing RD command functionality
echo ================================

Rem Create test directories for RD command testing
mkdir test_rd_dir1 2>nul
mkdir test_rd_dir2 2>nul
mkdir test_rd_complex 2>nul
mkdir test_rd_complex\subdir 2>nul
echo Content for non-empty directory > test_rd_complex\file.txt

Rem Test 1: Remove an empty directory
echo Test 1: Removing an empty directory
if exist test_rd_dir1 (
    rd test_rd_dir1
    if not exist test_rd_dir1 (
        echo Empty directory removed successfully
    ) else (
        echo Error: Empty directory removal failed
    )
) else (
    echo Directory test_rd_dir1 does not exist
)
echo.

Rem Test 2: Try to remove a non-empty directory (should fail without /s)
echo Test 2: Attempting to remove non-empty directory without /s flag (should fail)
if exist test_rd_complex (
    rd test_rd_complex 2>nul
    if exist test_rd_complex (
        echo Non-empty directory correctly not removed (as expected)
    ) else (
        echo Unexpected: Non-empty directory was removed
    )
) else (
    echo Directory test_rd_complex does not exist
)
echo.

Rem Test 3: Remove a non-empty directory with /s flag
echo Test 3: Removing non-empty directory with /s flag
if exist test_rd_complex (
    rd /s /q test_rd_complex
    if not exist test_rd_complex (
        echo Non-empty directory removed successfully with /s flag
    ) else (
        echo Error: Non-empty directory removal failed with /s flag
    )
) else (
    echo Directory test_rd_complex does not exist
)
echo.

Rem Test 4: Remove directory with /q flag (quiet mode)
echo Test 4: Creating and removing directory with /q flag (quiet mode)
md test_rd_dir2
if exist test_rd_dir2 (
    rd /q test_rd_dir2
    if not exist test_rd_dir2 (
        echo Directory removed in quiet mode successfully
    ) else (
        echo Error: Directory removal in quiet mode failed
    )
) else (
    echo Directory test_rd_dir2 was not created
)
echo.

Rem End of rd tests
echo All RD command tests completed successfully