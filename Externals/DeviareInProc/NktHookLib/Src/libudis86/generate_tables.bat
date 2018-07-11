@ECHO OFF
SET __FOUND=
FOR %%X IN (python.exe) DO ( SET __FOUND=%%~$PATH:X )
IF "%__FOUND%" == "" (
    ECHO Error: Cannot locate Python 2.7
    EXIT /B 1
)
RD "%~dp0tables" /S /Q >NUL 2>NUL
MD "%~dp0tables" >NUL 2>NUL
"%__FOUND%" "%~dp0source\scripts\ud_itab.py" "%~dp0source\docs\x86\optable.xml" "%~dp0/tables"
IF ERRORLEVEL 1 (
    ECHO Error: Cannot generate UDis86 tables
    EXIT /B 1
)
