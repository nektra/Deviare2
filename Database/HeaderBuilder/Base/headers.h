#define long int

#define __ptr32
#define __ptr64
#define __unaligned
#ifndef _MT
	#define _MT
#endif
#define NULL 0
//#define _WCHAR_T_DEFINED
//#define _NATIVE_WCHAR_T_DEFINED
#define __SPECSTRINGS_STRICT_LEVEL 0
#define _USE_DECLSPECS_FOR_SAL 1
#define _CRT_NONSTDC_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_OBSOLETE_NO_WARNINGS
#define _NTTMAPI_ //DO NOT include Kernel transaction API's (bug in TRANSACTION_NOTIFICATION_RECOVERY_ARGUMENT definition)

#define _ERRCODE_DEFINED
typedef int errcode;
typedef int errno_t;

#define CINTERFACE

#define WINVER 0x0610
#define _WIN32_WINNT 0x0610
#define _WIN32_WINDOWS 0x0610
#define _WIN32_IE 0x0900
#define _MFC_VER 0x0900

#include <windows.h>

typedef DWORD FOURCC;

#define NOAVIFMT

#define DEFINE_DEVSVCGUID(name, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11)
#define DEFINE_DEVSVCPROPKEY(name, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12)

#define D3D11_NO_HELPERS
#define DEBUG_NO_IMPLEMENTATION
#define _ASSERTE(expr)
#define __FUNCTION__

#define _GDIPLUSGRAPHICS_H
#define MPR50 1
#define _ISDSCP_
#define _LPCBYTE_DEFINED

typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;

typedef signed char SBYTE;
typedef short SWORD;
typedef int SDWORD;
typedef unsigned __int64 QWORD;
typedef __int64 SQWORD;

typedef QWORD* LPQWORD;
typedef SQWORD* LPSQWORD;

typedef void** LPLPVOID;

typedef BYTE** LPLPBYTE;
typedef unsigned short** LPLPWORD;
typedef DWORD** LPLPDWORD;
typedef QWORD** LPLPQWORD;

typedef SBYTE** LPLPSBYTE;
typedef SWORD** LPLPSWORD;
typedef SDWORD** LPLPSDWORD;
typedef SQWORD** LPLPSQWORD;

//--------------------------------------------------------------------------------

#include <WinSock2.h>
#include <setupapi.h>
#include <ShellAPI.h>
#include <Imagehlp.h>
//#include <winable.h>
#include <ole2.h>
#include <OleAuto.h>
#include <commctrl.h>
#include <commdlg.h>
#include <Lm.h>
#include <Lmdfs.h>
#include <Pdh.h>
#include <Psapi.h>
#include <Mprapi.h>
#include <Tlhelp32.h>
#include <WinSock2.h>
#include <MMREG.H>
#include <Msacm.h>
#include <Vfw.h>
#include <Mswsock.h>
//#include <WinNT.h>
#include <Rasdlg.h>
#include <winternl.h>
#define _NTDEF_
#include <Wininet.h>
#include <Winineti.h>
#include <WinIoCtl.h>
#include "ntdll.h"
#include <usp10.h>
#include <Richedit.h>
#include <ws2tcpip.h>
#include <ras.h>
#include <MMSystem.h>
#include <MSAcm.h>
#include <urlmon.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#include <LZExpand.h>
#define SECURITY_WIN32
#include <sspi.h>
#include <NTSecAPI.h>
#include <NTSecPkg.h>
#include <Sddl.h>
#include <Wmistr.h>
#include <Evntrace.h>
#include <winspool.h>
#include <Wingdi.h>

typedef REPASTESPECIAL* PREPASTESPECIAL;
typedef EDITSTREAM* PEDITSTREAM;

INT WSAAPI WSARecvMsg(IN SOCKET s, IN OUT LPWSAMSG lpMsg, __out_opt LPDWORD lpdwNumberOfBytesRecvd, 
                      IN LPWSAOVERLAPPED lpOverlapped, IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
INT WSAAPI WSASendMsg(IN SOCKET s, IN LPWSAMSG lpMsg, IN DWORD dwFlags, __out_opt LPDWORD lpNumberOfBytesSent,
                      IN LPWSAOVERLAPPED lpOverlapped OPTIONAL, IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine OPTIONAL);
INT WSAAPI WSAPoll(IN OUT LPWSAPOLLFD fdarray, IN ULONG nfds, IN INT timeout);

HWND WINAPI CreateDialogIndirectParamAorW(HINSTANCE hInstance, LPCDLGTEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit, DWORD Flags);

HWND WINAPI DialogBoxIndirectParamAorW(HINSTANCE hInstance, LPCDLGTEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit, DWORD Flags);
