@ECHO off
SET cuda_compile=nvcc cuda_pi.cu -o cuda_pi.exe
SET sec_compile=cl secuencial_pi.c /Od /Os /nologo /W4 /WL
SET omp_compile=cl autoparvec_pi.c /Ox /Oi /Ot /nologo /W4 /WL /Qvec-report:2 /Qpar /Qpar-report:2
SET set_env="C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
SET set_env_2012="D:\programs\Microsoft Visual Studio 11.0\VC\vcvarsall.bat"

ECHO ********************************
ECHO Building cuda_pi
%comspec% /c "%set_env% x64 && %cuda_compile%"
IF %errorlevel% neq 0 EXIT /b %errorlevel%
ECHO ----------------
ECHO Executing cuda_pi
CALL cuda_pi.exe
ECHO ----------------
ECHO ********************************

REM IF EXIST secuencial_pi.exe GOTO RUN_SECUENCIAL

:BUILD_SECUENCIAL
ECHO Building secuencial_pi
%comspec% /c "%set_env% x64 && %sec_compile%"
IF %errorlevel% neq 0 EXIT /b %errorlevel%

IF NOT EXIST secuencial_pi.exe GOTO ERROR_SECUENCIAL

:RUN_SECUENCIAL
ECHO ----------------
ECHO Executing secuencial_pi
CALL secuencial_pi.exe
IF %errorlevel% neq 0 EXIT /b %errorlevel%
ECHO ----------------


:BUILD_AUTOPARVEC
ECHO Building autoparvec_pi
%comspec% /c "%set_env_2012% x64 && %omp_compile%"
IF %errorlevel% neq 0 EXIT /b %errorlevel%

IF NOT EXIST autoparvec_pi.exe GOTO ERROR_OPENMP

:RUN_AUTOPARVEC
ECHO ----------------
ECHO Ejecutando version auto-paralelizada y auto-vectorizada
CALL autoparvec_pi.exe
IF %errorlevel% neq 0 EXIT /b %errorlevel%
ECHO ----------------
GOTO END

:ERROR_SECUENCIAL
ECHO "Hubo un error al compilar la version secuencial del programa"
GOTO END

:ERROR_OPENMP
ECHO "Hubo un error al compilar la version auto paralelizada y vectorizada del programa"

:END