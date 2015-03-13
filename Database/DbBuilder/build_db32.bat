@ECHO OFF
IF [%1] == [] (
    "%~dp0\bin\DbGenerator.exe" "%~dp0\..\HeaderBuilder\Full\output\preprocessed32W.h" /x86
) ELSE (
    "%~dp0\bin\DbGenerator.exe" "%~1" /x86
)
