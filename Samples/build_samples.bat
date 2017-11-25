@ECHO OFF
SETLOCAL
IF NOT "%VS150COMNTOOLS%" == "" GOTO do_process
IF NOT EXIST "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Enterprise\Common7\Tools\VsDevCmd.bat" GOTO show_err

CALL "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Enterprise\Common7\Tools\VsDevCmd.bat"

:do_process
DEVENV C\RegistryPlugin\RegistryPlugin.sln /rebuild "Release|Win32"
IF NOT %ERRORLEVEL% == 0 goto bad_compile

DEVENV C\Test\CTest.sln /rebuild "Release|Win32"
IF NOT %ERRORLEVEL% == 0 goto bad_compile

DEVENV CSharp\COMHookingBasis\COMHookingBasis.sln /rebuild "Release|x86"
IF NOT %ERRORLEVEL% == 0 goto bad_compile

DEVENV CSharp\FilePlugin\FilePlugin.sln /rebuild "Release|AnyCPU"
IF NOT %ERRORLEVEL% == 0 goto bad_compile

DEVENV CSharp\RegistryPlugin\RegistryPlugin.sln /rebuild "Release|AnyCPU"
IF NOT %ERRORLEVEL% == 0 goto bad_compile

DEVENV CSharp\Console\CSharpConsole.sln /rebuild "Release|x86"
IF NOT %ERRORLEVEL% == 0 goto bad_compile

DEVENV CSharp\PrintLogger\PrintLogger.sln /rebuild "Release|x86"
IF NOT %ERRORLEVEL% == 0 goto bad_compile

DEVENV C\RegistryPlugin\RegistryPlugin.sln /rebuild "Release|x64"
IF NOT %ERRORLEVEL% == 0 goto bad_compile

DEVENV C\Test\CTest.sln /rebuild "Release|x64"
IF NOT %ERRORLEVEL% == 0 goto bad_compile

DEVENV CSharp\COMHookingBasis\COMHookingBasis.sln /rebuild "Release|x64"
IF NOT %ERRORLEVEL% == 0 goto bad_compile

DEVENV CSharp\Console\CSharpConsole.sln /rebuild "Release|x64"
IF NOT %ERRORLEVEL% == 0 goto bad_compile

DEVENV CSharp\PrintLogger\PrintLogger.sln /rebuild "Release|x64"
IF NOT %ERRORLEVEL% == 0 goto bad_compile

GOTO end

:show_err
ECHO Please execute this batch file inside a Visual Studio 2017 Command Prompt
PAUSE
GOTO end

:bad_compile
ECHO Errors detected while compiling samples
PAUSE
GOTO end

:end
ENDLOCAL
