@ECHO OFF
SETLOCAL

IF "%1" == "" GOTO show_help

IF /I "%1" == "base" GOTO process_base
SET __Folder=%~dp0\%1
SET __HeaderFile=headers.h
GOTO aftervars

:process_base
SET __Folder=%~dp0\base
SET __HeaderFile=dummy.h
:aftervars

IF NOT "%VCINSTALLDIR%" == "" GOTO do_process
IF "%VS140COMNTOOLS%" == "" GOTO show_err

ECHO Setting up a Visual Studio x64 Command Prompt environment... 
CALL "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64
IF "%VS140COMNTOOLS%" == "" GOTO err_cantsetupvs

:do_process
ECHO Compiling headers...
md "%__Folder%\output\" >NUL 2>NUL
cl "%__Folder%\%__HeaderFile%" "/FI%~dp0\base\headers.h" /DWIN32 /D_WIN32 /DWIN64 /D_WIN64 /D_M_AMD64 /D_AMD64_ /D_M_X64 /DWIN32_LEAN_AND_MEAN /Zc:wchar_t /W3 /Zm1000 /EHa /GR /TP /EP                      >"%__Folder%\output\preprocessed64baseA.h" 2>"%__Folder%\output\compile64.log"
IF NOT %ERRORLEVEL% == 0 goto bad_compile
cl "%__Folder%\%__HeaderFile%" "/FI%~dp0\base\headers.h" /DWIN32 /D_WIN32 /DWIN64 /D_WIN64 /D_M_AMD64 /D_AMD64_ /D_M_X64 /DWIN32_LEAN_AND_MEAN /Zc:wchar_t /W3 /Zm1000 /EHa /GR /TP /EP /DUNICODE /D_UNICODE >"%__Folder%\output\preprocessed64baseW.h" 2>>"%__Folder%\output\compile64.log"
IF NOT %ERRORLEVEL% == 0 goto bad_compile

ECHO Fixing headers...
"%~dp0\bin\HeaderFix.exe" 64 "%__Folder%\output\preprocessed64baseA.h" "%__Folder%\output\preprocessed64A.h" >>"%__Folder%\output\compile64.log"
"%~dp0\bin\HeaderFix.exe" 64 "%__Folder%\output\preprocessed64baseW.h" "%__Folder%\output\preprocessed64W.h" >>"%__Folder%\output\compile64.log"
del /Q "%__Folder%\output\preprocessed64baseA.h"
del /Q "%__Folder%\output\preprocessed64baseW.h"
GOTO end

:show_help
ECHO Use: BUILD64 namespace
ECHO Where 'namespace' should be set to "base" to build base headers 
GOTO end

:show_err
ECHO Please execute this batch file inside a Visual Studio x64 Command Prompt
GOTO end

:err_cantsetupvs
ECHO Cannot initialize Visual Studio x64 Command Prompt environment
GOTO end

:bad_compile
ECHO Errors detected while building headers file
GOTO end

:end
ENDLOCAL
