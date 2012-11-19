@ECHO off
SET compile=cl.exe mult_matrices.c /openmp /O2 /Ot /nologo /W4 /WL
SET set_env="C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"

ECHO ********************************
ECHO Building 8x16x8
%comspec% /c "%set_env% x64 && %compile% /DAH#8 /DAW#16 /DBW#8 /o mult_8x16x8.exe"
IF %errorlevel% neq 0 EXIT /b %errorlevel%
ECHO ----------------
ECHO Executing 8x16x8
CALL mult_8x16x8.exe
ECHO ----------------
IF %errorlevel% neq 0 EXIT /b %errorlevel%

ECHO ********************************
ECHO Building 16x64x8
%comspec% /c "%set_env% x64 && %compile% /DAH#16 /DAW#64 /DBW#8 /o mult_16x64x8.exe"
IF %errorlevel% neq 0 EXIT /b %errorlevel%
ECHO ----------------
ECHO Executing 8x16x8
CALL mult_16x64x8.exe
IF %errorlevel% neq 0 EXIT /b %errorlevel%
ECHO ----------------

ECHO ********************************
ECHO Building 128x128x128
%comspec% /c "%set_env% x64 && %compile% /DAH#128 /DAW#128 /DBW#128 /o mult_128x128x128.exe"
IF %errorlevel% neq 0 EXIT /b %errorlevel%
ECHO ----------------
ECHO Executing 128x128x128
CALL mult_128x128x128.exe
ECHO ----------------
IF %errorlevel% neq 0 EXIT /b %errorlevel%

:clean
rm mult_matrices.obj