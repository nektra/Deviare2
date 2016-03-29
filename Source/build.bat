@ECHO OFF
SETLOCAL

SET __Config=Release
IF /I [%1] == [debug] (
    SET __Config=Debug
)
IF "%VS140COMNTOOLS%" == "" (
    ECHO Visual Studio 2015 not found on this computer
    PAUSE
    GOTO end
)

:do_process
SETLOCAL
CALL "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
IF "%VS140COMNTOOLS%" == "" (
    ENDLOCAL
    ECHO Cannot initialize Visual Studio x86 Command Prompt environment
    PAUSE
    GOTO end
)
DEVENV DvEngine.sln /rebuild "%__Config%|Win32"
IF NOT %ERRORLEVEL% == 0 (
    ENDLOCAL
    ECHO Errors detected while compiling Deviare
    PAUSE
    GOTO end
)
ENDLOCAL

SETLOCAL
CALL "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x64
IF "%VS140COMNTOOLS%" == "" (
    ENDLOCAL
    ECHO Cannot initialize Visual Studio x64 Command Prompt environment
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
ENDLOCAL

:end
ENDLOCAL
