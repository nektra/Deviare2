@ECHO OFF
IF "%1" == "" GOTO show_help

MD "%1" >NUL 2>NUL
"%~dp0\..\DbGenerator32" "%~dp0\..\..\CreateBaseHeader\%1\output\preprocessed32W.h" /x86 "/out=%~dp0\%1"
IF NOT %ERRORLEVEL% == 0 goto bad_build
GOTO end

:show_help
ECHO Use: BUILD32 namespace
ECHO Where 'namespace' should be set to "base" to compile base headers 
GOTO end

:bad_build
ECHO Errors detected while building database file
GOTO end

:end
