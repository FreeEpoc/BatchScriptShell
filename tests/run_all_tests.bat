@echo off
Rem Batch script to run all tests

echo Running All Batch Script Tests
echo ==============================

echo Starting ECHO command test...
call echo_test.bat
echo.
echo ECHO command test completed.
echo =============================

echo Starting CD command test...
call cd_test.bat
echo.
echo CD command test completed.
echo ===========================

echo Starting COPY command test...
call copy_test.bat
echo.
echo COPY command test completed.
echo =============================

echo Starting DEL command test...
call del_test.bat
echo.
echo DEL command test completed.
echo ============================

echo Starting DIR command test...
call dir_test.bat
echo.
echo DIR command test completed.
echo ============================

echo Starting MD command test...
call md_test.bat
echo.
echo MD command test completed.
echo ==========================

echo Starting RD command test...
call rd_test.bat
echo.
echo RD command test completed.
echo ==========================

echo Starting REN command test...
call ren_test.bat
echo.
echo REN command test completed.
echo ===========================

echo Starting PAUSE command test...
call pause_test.bat
echo.
echo PAUSE command test completed.
echo ==============================

echo Starting SET command test...
call set_test.bat
echo.
echo SET command test completed.
echo ===========================

echo Starting MOVE command test...
call move_test.bat
echo.
echo MOVE command test completed.
echo ============================

echo Starting complex test...
call complex_test.bat
echo.
echo Complex test completed.
echo =======================

echo Starting additional commands test...
call additional_commands_test.bat
echo.
echo Additional commands test completed.
echo ====================================

echo Starting comprehensive documentation-based test...
call comprehensive_test.bat
echo.
echo Comprehensive documentation-based test completed.
echo =================================================

echo All tests completed successfully!