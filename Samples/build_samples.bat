@ECHO OFF
SETLOCAL
IF NOT "%VCINSTALLDIR%" == "" GOTO do_process
IF "%VS110COMNTOOLS%" == "" GOTO show_err

:do_process
CALL "%VS110COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
IF "%VS110COMNTOOLS%" == "" GOTO err_cantsetupvs_x86

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

ENDLOCAL

SETLOCAL
CALL "%VS110COMNTOOLS%\..\..\VC\vcvarsall.bat" x64
IF "%VS110COMNTOOLS%" == "" GOTO err_cantsetupvs_x64

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
ECHO Please execute this batch file inside a Visual Studio Command Prompt
PAUSE
GOTO end

:err_cantsetupvs_x86
ECHO Cannot initialize Visual Studio x86 Command Prompt environment
PAUSE
GOTO end

:err_cantsetupvs_x64
ECHO Cannot initialize Visual Studio x64 Command Prompt environment
PAUSE
GOTO end

:bad_compile
ECHO Errors detected while compiling samples
PAUSE
GOTO end

:end
ENDLOCAL
