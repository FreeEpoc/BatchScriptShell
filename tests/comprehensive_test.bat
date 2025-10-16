@echo off
Rem Comprehensive test based on various documentation topics

echo Running Comprehensive Documentation-Based Test
echo =============================================

Rem Test variables functionality
echo 1. Testing Variables functionality
set TEST_VAR=Test value
echo Variable TEST_VAR contains: %TEST_VAR%
set /A NUM_VAR=100
echo Numeric variable NUM_VAR contains: %NUM_VAR%
echo.

Rem Test string operations
echo 2. Testing String operations
set STR_VAR=Hello,World,Test
echo Original string: %STR_VAR%
echo First part: %STR_VAR:~0,5%
echo.

Rem Test command chaining and operators
echo 3. Testing command chaining
echo First command && echo Second command
echo.

Rem Test decision making with IF
echo 4. Testing IF statement decision making
set /A value=5
if %value% GTR 3 (
  echo Value is greater than 3
) else (
  echo Value is not greater than 3
)
echo.

Rem Test FOR loop functionality in more detail
echo 5. Testing FOR loop with file processing
echo Line 1 > for_test.txt
echo Line 2 >> for_test.txt
echo Line 3 >> for_test.txt

echo Processing file with FOR /F:
for /F "tokens=*" %%a in (for_test.txt) do (
  echo Processing: %%a
)
del for_test.txt
echo.

Rem Test arithmetic operations
echo 6. Testing arithmetic operations
set /A result=10+20*2
echo 10+20*2=%result%
set /A result=(10+20)*2
echo (10+20)*2=%result%
echo.

Rem Test operator functionality
echo 7. Testing operators
set /A a=10
set /A b=3
set /A result=a%%b
echo 10 mod 3 = %result% (modulo operator)
set /A result=a^b
echo 10 XOR 3 = %result% (exclusive OR operator)
echo.

Rem Test input/output redirection concept (by creating and reading files)  
echo 8. Testing input/output concepts
echo Test data > io_test.txt
set /p input_var=< io_test.txt
echo Read from file: %input_var%
del io_test.txt
echo.

Rem Test return code concept
echo 9. Testing return code concept
dir nonexistent_directory 2>nul
if errorlevel 1 echo Previous command returned an error (as expected)
echo.

Rem Test functions concept (using CALL for subroutine)
echo 10. Testing function/subroutine concept with CALL
call :print_message "This is from a subroutine"
echo.

goto :end

:print_message
echo Called subroutine with parameter: %1
goto :eof

:end
echo Comprehensive documentation-based test completed successfully!

Rem Ensure termination
exit /b