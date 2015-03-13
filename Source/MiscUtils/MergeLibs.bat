@ECHO OFF
SETLOCAL
IF NOT "%VCINSTALLDIR%" == "" GOTO do_process
IF "%VS90COMNTOOLS%" == "" GOTO show_err

ECHO Setting up a Visual Studio x86 Command Prompt environment... 
CALL "%VS90COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
IF "%VS90COMNTOOLS%" == "" GOTO err_cantsetupvs

:do_process
ECHO Combining x86 library files...

LIB /out:DeviareLib.lib         NedMallocLib.lib         NktLz4Lib.lib         NktSqLiteLib.lib         SpyMgrLib.lib         UDis86Lib.lib
IF NOT %ERRORLEVEL% == 0 goto bad_combine

LIB /out:DeviareLib_Debug.lib   NedMallocLib_Debug.lib   NktLz4Lib_Debug.lib   NktSqLiteLib_Debug.lib   SpyMgrLib_Debug.lib   UDis86Lib_Debug.lib
IF NOT %ERRORLEVEL% == 0 goto bad_combine

ENDLOCAL
SETLOCAL

ECHO Setting up a Visual Studio x64 Command Prompt environment... 
CALL "%VS90COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64
IF "%VS90COMNTOOLS%" == "" GOTO err_cantsetupvs64

LIB /out:DeviareLib64.lib       NedMallocLib64.lib       NktLz4Lib64.lib       NktSqLiteLib64.lib       SpyMgrLib64.lib       UDis86Lib64.lib
IF NOT %ERRORLEVEL% == 0 goto bad_combine

LIB /out:DeviareLib64_Debug.lib NedMallocLib64_Debug.lib NktLz4Lib64_Debug.lib NktSqLiteLib64_Debug.lib SpyMgrLib64_Debug.lib UDis86Lib64_Debug.lib
IF NOT %ERRORLEVEL% == 0 goto bad_combine

GOTO end

:show_err
ECHO Please execute this batch file inside a Visual Studio x86 Command Prompt
GOTO end

:err_cantsetupvs
ECHO Cannot initialize Visual Studio x86 Command Prompt environment
GOTO end

:err_cantsetupvs64
ECHO Cannot initialize Visual Studio x64 Command Prompt environment
GOTO end

:bad_combine
ECHO Errors detected while combining library files
GOTO end

:end
ENDLOCAL
