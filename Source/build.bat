@ECHO OFF
SETLOCAL

SET __Config=Release
IF /I [%1] == [debug] (
    SET __Config=Debug
)

IF NOT "%VS150COMNTOOLS%" == "" GOTO do_process

IF NOT EXIST "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Enterprise\Common7\Tools\VsDevCmd.bat" (
    ECHO Visual Studio 2017 not found on this computer
    PAUSE
    GOTO end
)

CALL "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Enterprise\Common7\Tools\VsDevCmd.bat"

:do_process

DEVENV DvEngine.sln /rebuild "%__Config%|Win32"
IF NOT %ERRORLEVEL% == 0 (
    ENDLOCAL
    ECHO Errors detected while compiling Deviare
    PAUSE
    GOTO end
)

DEVENV DvEngine.sln /rebuild "%__Config%|x64"
IF NOT %ERRORLEVEL% == 0 (
    ENDLOCAL
    ECHO Errors detected while compiling Deviare
    PAUSE
    GOTO end
)

:end
ENDLOCAL
