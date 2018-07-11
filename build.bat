@ECHO OFF
SETLOCAL

IF [%~1] == [] (
	ECHO Use: BUILD.BAT ^(2013^|2015^|2017^)
	ENDLOCAL
	PAUSE
	EXIT /B 1
)

IF [%~1%] == [] (
	ECHO Error: Missing argument for VS version parameter.
	ENDLOCAL
	PAUSE
	EXIT /B 1
)

FOR /f delims^=^"^ tokens^=* %%a IN ('CSCRIPT.EXE //NoLogo "build_findvs.vbs" %~1') DO SET __ComnTools=%%a
IF "%__ComnTools%" == "" (
	ECHO Error: Cannot locate selected Visual Studio. Ensure it is properly installed.
	ENDLOCAL
	PAUSE
	EXIT /B 1
)


CALL "%__ComnTools%\VsDevCmd.bat" >NUL 2>NUL

DEVENV "%~dp0NktHookLib_%~1.sln" /rebuild "Debug|Win32"
IF NOT %ERRORLEVEL% == 0 GOTO bad_compile
REM DeviareLiteInterop depends on DeviareLiteCOM
REM DeviareLiteCOM depends on NktHookLib
DEVENV "%~dp0NktHookLib_%~1.sln" /rebuild "Release|Win32"
IF NOT %ERRORLEVEL% == 0 GOTO bad_compile

DEVENV "%~dp0NktHookLib_%~1.sln" /rebuild "Debug|x64"
IF NOT %ERRORLEVEL% == 0 GOTO bad_compile
REM DeviareLiteInterop depends on DeviareLiteCOM
REM DeviareLiteCOM depends on NktHookLib
DEVENV "%~dp0NktHookLib_%~1.sln" /rebuild "Release|x64"
IF NOT %ERRORLEVEL% == 0 GOTO bad_compile

ENDLOCAL
EXIT /B 0

:bad_compile
ENDLOCAL

ECHO Error: Unable to complete compilation stage.
ENDLOCAL
PAUSE
EXIT /B 1
