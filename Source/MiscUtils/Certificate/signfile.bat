@ECHO OFF
IF EXIST "%~dp0\nektra\realsignfile.bat" (
    IF EXIST "%~dp0\nektra\nektra_code_certificate.p12" (
        CALL "%~dp0\nektra\realsignfile.bat" "%~1"
    )
)
