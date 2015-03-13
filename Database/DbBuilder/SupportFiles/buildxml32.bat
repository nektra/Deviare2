@ECHO OFF
IF "%1" == "" GOTO show_help

gccxml32\gccxml.EXE "%1" -fxml="%1.xml" --gccxml-config "%~dp0\gccxml32\config32"
GOTO end

:show_help
ECHO Use: BUILDXML32 header-file
ECHO Where 'header-file' should be the full path to the header to parse
GOTO end

:end