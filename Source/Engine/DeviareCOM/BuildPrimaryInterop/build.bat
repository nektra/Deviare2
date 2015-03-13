@ECHO OFF
ECHO Creating TLB...
COPY /Y "%~dp0\..\DeviareCOM.idl" "%~dp0\DeviareCOM.idl"
COPY /Y "%~dp0\..\disp_ids.h" "%~dp0\disp_ids.h"
COPY /Y "%~dp0\..\idl_csharp_support.h" "%~dp0\idl_csharp_support.h"
MIDL "%~dp0\DeviareCOM.idl" /D "NDEBUG" /nologo /char signed /env win32 /tlb "%~dp0\DeviareCOM.tlb" /h "%~dp0\DeviareCOM_i.h" /dlldata "%~dp0\dlldata.c" /iid "%~dp0\DeviareCOM_i.c" /proxy "%~dp0\DeviareCOM_p.c" /error all /error stub_data /Os
IF NOT %ERRORLEVEL% == 0 goto bad_compile

"%~dp0\..\..\..\MiscUtils\TlbImp2\TLBIMP2.exe" "%~dp0\DeviareCOM.tlb" /primary "/keyfile:%~dp0\keypair.snk" /transform:dispret "/out:%~dp0\..\..\..\..\bin\Nektra.Deviare2.dll"
IF NOT %ERRORLEVEL% == 0 goto bad_interopbuild

GOTO end

:bad_compile
ECHO Errors detected while compiling IDL file
GOTO end

:bad_interopbuild
ECHO Errors detected while building Primary Interop Assembly
GOTO end

:end
DEL /Q "%~dp0\DeviareCOM*.c" >NUL 2>NUL
DEL /Q "%~dp0\DeviareCOM*.h" >NUL 2>NUL
DEL /Q "%~dp0\dlldata*.c" >NUL 2>NUL
