@echo off
echo Creating test file with redirection...
echo This is a test line > redirection_test.txt
echo Checking if file was created:
if exist redirection_test.txt (
  echo File redirection_test.txt exists
  type redirection_test.txt
) else (
  echo File redirection_test.txt does not exist
)