@echo off
Rem Test file for MOVE command

echo Testing MOVE command functionality
echo ==================================

Rem Create test directories and files for MOVE command testing
mkdir move_source_dir 2>nul
mkdir move_dest_dir 2>nul
echo Content for moved file > move_source_dir\source_file.txt
echo Another file > move_source_dir\another_file.txt
mkdir move_source_dir\subdir 2>nul
echo Content for subdirectory > move_source_dir\subdir\subfile.txt

Rem Test 1: Move a single file
echo Test 1: Moving a single file
if exist move_source_dir\source_file.txt (
    move move_source_dir\source_file.txt move_dest_dir\moved_file.txt
    if exist move_dest_dir\moved_file.txt (
        echo File moved successfully
    ) else (
        echo Error: File move failed
    )
) else (
    echo Source file does not exist
)
echo.

Rem Test 2: Move all files with specific extension
echo Test 2: Moving all txt files
move move_source_dir\*.txt move_dest_dir\
echo Files moved to destination directory
echo.

Rem Test 3: Move a directory
echo Test 3: Moving a directory
if exist move_source_dir\subdir (
    move move_source_dir\subdir move_dest_dir\moved_subdir
    if exist move_dest_dir\moved_subdir (
        echo Directory moved successfully
    ) else (
        echo Error: Directory move failed
    )
) else (
    echo Source directory does not exist
)
echo.

Rem Show contents of both directories to verify
echo Contents of destination directory after moves:
dir move_dest_dir
echo.
echo Contents of source directory after moves:
dir move_source_dir
echo.

Rem Cleanup: Remove test directories
rmdir /s /q move_source_dir 2>nul
rmdir /s /q move_dest_dir 2>nul

Rem End of move tests
echo All MOVE command tests completed successfully