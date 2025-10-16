@echo off
echo Testing Interactive and Advanced Commands
echo =========================================

echo.
echo 1. Testing DATE and TIME commands:
echo Current date:
DATE
echo Current time:
TIME

echo.
echo 2. Testing FOR loops with different patterns:
echo Looping through numbers:
FOR %%n IN (1,2,3,4,5) DO (
  echo Number: %%n
)

echo.
echo Looping through directories/files:
FOR %%f IN (*.c, *.h, Makefile) DO (
  IF EXIST %%f echo Found: %%f
)

echo.
echo 3. Testing advanced SET commands with arithmetic:
SET /A var1=10
SET /A var2=20
SET /A sum=var1+var2
echo %var1% + %var2% = %sum%

SET /A product=var1*var2
echo %var1% * %var2% = %product%

SET /A diff=var2-var1
echo %var2% - %var1% = %diff%

SET /A quotient=var2/var1
echo %var2% / %var1% = %quotient%

echo.
echo 4. Testing assignment operators:
SET /A counter=0
SET /A counter+=5
echo Counter after +=5: %counter%

SET /A counter*=2
echo Counter after *=2: %counter%

SET /A counter-=3
echo Counter after -=3: %counter%

SET /A counter/=2
echo Counter after /=2: %counter%

echo.
echo 5. Testing IF statements with numeric comparisons:
IF %counter% EQU 7 (
  echo Counter is equal to 7
) ELSE (
  echo Counter is not equal to 7 (it's %counter%)
)

IF %counter% GTR 5 (
  echo Counter (%counter%) is greater than 5
)

IF %counter% LSS 10 (
  echo Counter (%counter%) is less than 10
)

echo.
echo 6. Testing IF with logical operators:
IF %var1% LSS %var2% (
  IF %var2% GTR 5 (
    echo Both conditions are true: %var1% < %var2% and %var2% > 5
  )
)

echo.
echo 7. Testing nested IF statements:
IF %counter% GTR 0 (
  echo Counter is positive
  IF %counter% LEQ 10 (
    echo Counter is between 1 and 10
  )
)

echo.
echo 8. Testing string operations:
SET test_string=Hello World
echo Test string: %test_string%

SET first_word=Hello
SET second_word=World
SET combined=%first_word% %second_word%
echo Combined string: %combined%

echo.
echo 9. Testing environment variable operations:
echo OS: %OS%
echo Current working directory: %PWD%

echo.
echo 10. Testing ERRORLEVEL with commands:
DIR nonexistent_file.txt >nul 2>&1
IF ERRORLEVEL 1 (
  echo Command failed as expected (nonexistent file)
) ELSE (
  echo Command unexpectedly succeeded
)

echo.
echo Interactive commands test completed!
echo ====================================

pause