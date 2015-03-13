@ECHO OFF
IF [%1] == [] (
    "%~dp0\bin\DbGenerator64.exe" "%~dp0\..\HeaderBuilder\Full\output\preprocessed64W.h" /x64
) ELSE (
    "%~dp0\bin\DbGenerator64.exe" "%~1" /x64
)
