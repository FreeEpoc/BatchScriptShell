@echo off
echo First line of test file > find_test.txt
find "test" find_test.txt
del find_test.txt