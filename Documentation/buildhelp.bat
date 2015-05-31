@ECHO OFF
RD /S /Q out >NUL 2>NUL
IF EXIST "%~dp0\bin\doxygen.exe" (
    "%~dp0\bin\doxygen.exe" "%~dp0\config\doxyfile"
    IF ERRORLEVEL 1 (
        ECHO Errors found while generating documentation
        GOTO :eof
    )
) ELSE (
    ECHO Please read the README.txt file located in this folder to get instructions on how to install Doxygen
) 
