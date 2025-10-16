@echo off
echo Testing DOS File Commands
echo =========================

echo Current directory:
pwd

echo.
echo 1. Testing MD (mkdir) command:
MD test_folder
IF EXIST test_folder (
  echo test_folder created successfully
) ELSE (
  echo test_folder creation failed
)

echo.
echo 2. Testing file creation and TYPE command:
echo This is line 1 > test_file.txt
echo This is line 2 >> test_file.txt
echo This is line 3 >> test_file.txt

echo Content of test_file.txt:
TYPE test_file.txt

echo.
echo 3. Testing COPY command:
COPY test_file.txt backup_file.txt
IF EXIST backup_file.txt (
  echo backup_file.txt created successfully
) ELSE (
  echo backup_file.txt creation failed
)

echo.
echo 4. Testing MOVE command:
MOVE backup_file.txt moved_file.txt
IF EXIST moved_file.txt (
  echo File moved successfully
) ELSE (
  echo File move failed
)
IF NOT EXIST backup_file.txt (
  echo Original backup_file.txt no longer exists
)

echo.
echo 5. Testing DIR command:
echo Directory listing:
DIR

echo.
echo 6. Testing FIND command:
echo Finding 'line 2' in moved_file.txt:
TYPE moved_file.txt | FIND "line 2"

echo.
echo 7. Testing multiple file operations in sequence:
ECHO File A > file_a.txt
ECHO File B > file_b.txt
ECHO File C > file_c.txt

echo Files created. Combining them:
COPY file_a.txt + file_b.txt + file_c.txt combined.txt
IF EXIST combined.txt (
  echo Files combined successfully
  echo Combined file content:
  TYPE combined.txt
) ELSE (
  echo File combination failed
)

echo.
echo 8. Testing RD (rmdir) command:
echo Removing test_folder directory:
RD test_folder
IF NOT EXIST test_folder (
  echo test_folder removed successfully
) ELSE (
  echo test_folder removal failed
)

echo.
echo 9. Testing DEL command with multiple files:
DEL test_file.txt moved_file.txt combined.txt file_a.txt file_b.txt file_c.txt
echo Files deleted: test_file.txt moved_file.txt combined.txt file_a.txt file_b.txt file_c.txt

echo.
echo 10. Final check of current directory:
DIR

echo.
echo File commands test completed!
echo =============================

pause