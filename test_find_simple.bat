@echo off
echo Hello World > test_file.txt
echo Search term here >> test_file.txt
echo Another line >> test_file.txt
find "Search term" test_file.txt
del test_file.txt