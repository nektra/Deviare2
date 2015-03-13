@ECHO OFF
IF "%1" == "" GOTO show_help

CALL build32.bat %1
CALL build64.bat %1
GOTO end

:show_help
ECHO Use: BUILDALL namespace
ECHO Where 'namespace' should be set to "base" to compile base headers 
GOTO end

:end