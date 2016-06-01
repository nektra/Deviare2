/*
 * Copyright (C) 2010-2015 Nektra S.A., Buenos Aires, Argentina.
 * All rights reserved. Contact: http://www.nektra.com
 *
 *
 * This file is part of Deviare
 *
 *
 * Commercial License Usage
 * ------------------------
 * Licensees holding valid commercial Deviare licenses may use this file
 * in accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Nektra.  For licensing terms and
 * conditions see http://www.nektra.com/licensing/. Use the contact form
 * at http://www.nektra.com/contact/ for further information.
 *
 *
 * GNU General Public License Usage
 * --------------------------------
 * Alternatively, this file may be used under the terms of the GNU General
 * Public License version 3.0 as published by the Free Software Foundation
 * and appearing in the file LICENSE.GPL included in the packaging of this
 * file.  Please visit http://www.gnu.org/copyleft/gpl.html and review the
 * information to ensure the GNU General Public License version 3.0
 * requirements will be met.
 *
 **/

#pragma once

#include "EngBaseObj.h"
#if defined _M_X64
  #include <emmintrin.h>
#endif //_M_X64
#include "..\version.ver"

#define NKT_BUILDPROTOCOL_VERSION(x) (x)
#define NKT_BUILDPROTOCOL_VERSION_2(_major,_minor,_build,_rev)  (_major*10000+_minor*100+_build)
#define NKT_BUILDPROTOCOL_VERSION_3()                     \
      NKT_BUILDPROTOCOL_VERSION_2 NKT_BUILDPROTOCOL_VERSION(PRODUCT_VERSION)

//-----------------------------------------------------------

#define NKT_DV_TRANSPORT_PROTOCOL_VERSION                  NKT_BUILDPROTOCOL_VERSION_3()

#define NKT_DV_TRANSPORT_BUFFER_SIZE                    2048
//#define NKT_DV_TMSG_SIZE_A 1024
#define NKT_DV_TMSG_SIZE_A 512
#define NKT_DV_TMSG_SIZE_B 256

#define NKT_DV_TRANSPORT_MAX_DATA_SIZE                     \
                                NKT_DV_TRANSPORT_BUFFER_SIZE

#define NKT_DV_TRANSPORTENGINE_WORKERTHREADS_PER_CPU       3
#define NKT_DV_TRANSPORTENGINE_DISPATCHERTHREADS_PER_CPU   4

#define NKT_DV_TRANSPORTAGENT_THREADS_PER_CPU              3

#define NKT_DV_TRANSPORT_MAX_FREEBUFFERLIST              500
#define NKT_DV_TRANSPORT_MAX_INUSE_MESSAGES_SPYMGR      1000
#define NKT_DV_TRANSPORT_MAX_INUSE_MESSAGES_AGENT        500
#define NKT_DV_TRANSPORT_MAX_INUSE_MESSAGES_DELAY         50

#define NKT_DV_HOOKENG_IntercallCustomDataSize           128

//-----------------------------------------------------------
//-----------------------------------------------------------

#define NKT_DV_TMSG_CODE_CALLBACK                 0x80000000
#define NKT_DV_TMSG_ID_CALLBACK                   0x80000000

#define NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth          50

#define NKT_DV_TMSG_HOOKSTATECHANGE_Installed              1
#define NKT_DV_TMSG_HOOKSTATECHANGE_Enabled                2
#define NKT_DV_TMSG_HOOKSTATECHANGE_Disabled               3

#define NKT_DV_TMSG_ADDHOOK_FLAG_AsyncCallbacks       0x0001
#define NKT_DV_TMSG_ADDHOOK_FLAG_OnlyPreCall          0x0002
#define NKT_DV_TMSG_ADDHOOK_FLAG_OnlyPostCall         0x0004
#define NKT_DV_TMSG_ADDHOOK_FLAG_DontCallOnLdrLock    0x0008
#define NKT_DV_TMSG_ADDHOOK_FLAG_DontCallCustomHandlersOnLdrLock    0x0010
#define NKT_DV_TMSG_ADDHOOK_FLAG_InvalidateCache      0x0020
#define NKT_DV_TMSG_ADDHOOK_FLAG_DontSkipJumps        0x0040
#define NKT_DV_TMSG_ADDHOOK_FLAG_DisableStackWalk     0x0080

//sync with CNktDvHook enum
#define NKT_DV_TMSG_ADDCUSTOMHOOK_FLAG_DontCallIfLoaderLocked       0x0001
#define NKT_DV_TMSG_ADDCUSTOMHOOK_FLAG_DisableExceptionChecks       0x0002

#define NKT_DV_TMSG_CODE_LoadDll                           1
#define NKT_DV_TMSG_CODE_LoadDllCallback                   \
        (NKT_DV_TMSG_CODE_LoadDll|NKT_DV_TMSG_CODE_CALLBACK)
#define NKT_DV_TMSG_CODE_UnloadDll                         2
#define NKT_DV_TMSG_CODE_UnloadDllCallback                 \
      (NKT_DV_TMSG_CODE_UnloadDll|NKT_DV_TMSG_CODE_CALLBACK)
#define NKT_DV_TMSG_CODE_CallApi                           3
#define NKT_DV_TMSG_CODE_CallApiCallback                   \
        (NKT_DV_TMSG_CODE_CallApi|NKT_DV_TMSG_CODE_CALLBACK)
#define NKT_DV_TMSG_CODE_BigData                           4
#define NKT_DV_TMSG_CODE_OnCreateProcessCalled             5
#define NKT_DV_TMSG_CODE_OnLoadLibraryCalled               6
#define NKT_DV_TMSG_CODE_OnFreeLibraryCalled               7
#define NKT_DV_TMSG_CODE_AddHook                           8
#define NKT_DV_TMSG_CODE_RemoveHook                        9
#define NKT_DV_TMSG_CODE_EnableHook                       10
#define NKT_DV_TMSG_CODE_OnHookCalled                     11
#define NKT_DV_TMSG_CODE_HookStateChange                  12
#define NKT_DV_TMSG_CODE_SuspendX64ThreadFromX86          13
#define NKT_DV_TMSG_CODE_SuspendX64ThreadFromX86Callback   \
                 (NKT_DV_TMSG_CODE_SuspendX64ThreadFromX86|\
                                  NKT_DV_TMSG_CODE_CALLBACK)
#define NKT_DV_TMSG_CODE_InvalidateAgentCtrlDllCache      14
#define NKT_DV_TMSG_CODE_BatchHookExec                    15
#define NKT_DV_TMSG_CODE_HelperCall                       16
#define NKT_DV_TMSG_CODE_HelperCallCallback                \
                   (NKT_DV_TMSG_CODE_HelperCall|           \
                    NKT_DV_TMSG_CODE_CALLBACK)
#define NKT_DV_TMSG_CODE_CustomMessage                    17
#define NKT_DV_TMSG_CODE_HookOverwritten                  18
/*
#define NKT_DV_TMSG_CODE_InitialConnectionMessage         20
*/

#define NKT_DV_TMSG_CODE_HELPERCALL_GetWndProc             1
#define NKT_DV_TMSG_CODE_HELPERCALL_QueryHookOverwrite     2

//-----------------------------------------------------------
//-----------------------------------------------------------

#pragma pack(1)

//forward declarations
class CNktDvTransportMessageCallbackHelperBase;

typedef struct tagNKT_DV_TMSG_COMMON {
  DWORD dwMsgCode;
  DWORD dwMsgId;             //the receiver should copy the id when building a callback message or'ed
                             //with NKT_DV_TMSG_ID_CALLBACK
  ULONGLONG hProcessedEvent; //event to signal when message is processed (can be NULL)
  ULONGLONG nPtrCallback;    //pointer to CNktDvTransportMessageCallbackHelperBase
} NKT_DV_TMSG_COMMON;

typedef struct tagNKT_DV_TRANSPORT_MESSAGE {
  union {
    BYTE aBuf[NKT_DV_TRANSPORT_BUFFER_SIZE];
    NKT_DV_TMSG_COMMON sMsgCommon;
  };
} NKT_DV_TRANSPORT_MESSAGE, *LPNKT_DV_TRANSPORT_MESSAGE;

class CNktDvTransportOverlapped : public CNktMemMgrObj, public TNktLnkLstNode<CNktDvTransportOverlapped>
{
public:
  CNktDvTransportOverlapped() : CNktMemMgrObj(), TNktLnkLstNode<CNktDvTransportOverlapped>()
    {
    nktMemSet(&sOvr, 0, sizeof(sOvr));
    dwType = 0;
    dwId = 0;
    nOrder = 0;
    nktMemSet(&sEngineInternals, 0, sizeof(sEngineInternals));
    nktMemSet(&sMsg, 0, sizeof(sMsg));
    dwReadedBytes = 0;
    return;
    };

  static CNktDvTransportOverlapped* ClassFromOverlapped(__in LPOVERLAPPED lpOvr)
    {
    return (CNktDvTransportOverlapped*)((char*)lpOvr - (char*)&(((CNktDvTransportOverlapped*)0)->sOvr));
    };

public:
  OVERLAPPED sOvr;
  DWORD dwType;
  DWORD dwId;
  LONG nOrder;
  struct {
    LPVOID lpConn; //will store a temporary pointer to lpConnection
    DWORD dwThreadId;
  } sEngineInternals;
  NKT_DV_TRANSPORT_MESSAGE sMsg;
  union {
    DWORD dwReadedBytes;
    DWORD dwWrittenBytes;
  };
};

typedef struct tagNKT_DV_ASMREGISTERS32 {
  struct {
    DWORD dwAligment;
    double Original_St0;
  } sInternalUseOnly;
  struct tagInteger {
    __declspec(align(16)) DWORD Eax, Edx; //put Edx after Eax to make it compatible with EaxEdxPair
    __declspec(align(16)) DWORD Ecx, Ebx;
    DWORD Esi, Edi;
    DWORD Ebp;
    DWORD Esp;
    DWORD Eip;
  } sInteger;
  struct tagFloating {
    __declspec(align(16)) double St0;
  } sFloating;
} NKT_DV_ASMREGISTERS32, *LPNKT_DV_ASMREGISTERS32;

typedef struct tagNKT_DV_ASMREGISTERS64 {
  struct {
    double Original_Xmm0, Original_Xmm1, Original_Xmm2, Original_Xmm3;
  } sInternalUseOnly;
  struct tagInteger {
    __declspec(align(16)) DWORD64 Rax, Rbx, Rcx, Rdx;
    __declspec(align(16)) DWORD64 Rsi, Rdi, Rsp, Rbp;
    __declspec(align(16)) DWORD64 R8, R9, R10, R11;
    DWORD64 Rip;
  } sInteger;
  __declspec(align(16)) struct tagFloating {
    __declspec(align(16)) double Xmm0;
    __declspec(align(16)) double Xmm1;
    __declspec(align(16)) double Xmm2;
    __declspec(align(16)) double Xmm3;
  } sFloating;
} NKT_DV_ASMREGISTERS64, *LPNKT_DV_ASMREGISTERS64;

typedef struct tagNKT_DV_CURRENT_CALL_STATE32 {
  DWORD dwSkipCall;
  DWORD dwOsLastError;
  NKT_DV_ASMREGISTERS32 sAsmRegisters;
  NKT_DV_ASMREGISTERS32 sPreCallAsmRegisters;
  ULONG nStackTrace[NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth];
} NKT_DV_CURRENT_CALL_STATE32, *LPNKT_DV_CURRENT_CALL_STATE32;

typedef struct tagNKT_DV_CURRENT_CALL_STATE64 {
  DWORD dwSkipCall;
  DWORD dwOsLastError;
  NKT_DV_ASMREGISTERS64 sAsmRegisters;
  NKT_DV_ASMREGISTERS64 sPreCallAsmRegisters;
  ULONGLONG nStackTrace[NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth];
} NKT_DV_CURRENT_CALL_STATE64, *LPNKT_DV_CURRENT_CALL_STATE64;

typedef union {
  LPNKT_DV_CURRENT_CALL_STATE32 lp32;
  LPNKT_DV_CURRENT_CALL_STATE64 lp64;
} LPNKT_DV_CURRENT_CALL_STATE;
#pragma pack()

//-----------------------------------------------------------
//-----------------------------------------------------------

#pragma pack(1)
typedef struct tagNKT_DV_TMSG_LOADDLL : public NKT_DV_TMSG_COMMON {
  WCHAR szDllNameW[NKT_DV_TMSG_SIZE_A];
  DWORD dwDllNameBigPacketId;
  BOOL bUnloadOnAgentShutdown;
} NKT_DV_TMSG_LOADDLL;

typedef struct tagNKT_DV_TMSG_LOADDLL_CALLBACK : public NKT_DV_TMSG_COMMON {
  HRESULT hLoadDllRes;
} NKT_DV_TMSG_LOADDLL_CALLBACK;

typedef struct tagNKT_DV_TMSG_UNLOADDLL : public NKT_DV_TMSG_COMMON {
  WCHAR szDllNameW[NKT_DV_TMSG_SIZE_A];
  DWORD dwDllNameBigPacketId;
} NKT_DV_TMSG_UNLOADDLL;

typedef struct tagNKT_DV_TMSG_UNLOADDLL_CALLBACK : public NKT_DV_TMSG_COMMON {
  HRESULT hUnloadDllRes;
} NKT_DV_TMSG_UNLOADDLL_CALLBACK;

typedef struct tagNKT_DV_TMSG_CALLCUSTOMAPI : public NKT_DV_TMSG_COMMON {
  WCHAR szDllNameW[NKT_DV_TMSG_SIZE_A];
  DWORD dwDllNameBigPacketId;
  CHAR szApiToCallA[NKT_DV_TMSG_SIZE_B];
  ULONG nParamsCount;
  ULONGLONG nParameters[4];
} NKT_DV_TMSG_CALLCUSTOMAPI;

typedef struct tagNKT_DV_TMSG_CALLCUSTOMAPI_CALLBACK : public NKT_DV_TMSG_COMMON {
  BOOL bDllNotFound, bApiNotFound, bExceptionRaised;
  LONG nResult;
} NKT_DV_TMSG_CALLCUSTOMAPI_CALLBACK;

typedef struct tagNKT_DV_TMSG_BIGDATA : public NKT_DV_TMSG_COMMON {
  struct tagBdHeader {
    DWORD dwThreadId;
    DWORD dwId;
    DWORD dwOrderNo;
    DWORD dwDataLen;
  } sBdHeader;
  BYTE aData[NKT_DV_TRANSPORT_BUFFER_SIZE - sizeof(struct tagBdHeader) - sizeof(NKT_DV_TMSG_COMMON)];
} NKT_DV_TMSG_BIGDATA;

typedef struct tagNKT_DV_TMSG_ONCREATEPROCESS : public NKT_DV_TMSG_COMMON {
  DWORD dwPid;
  DWORD dwTid;
  ULONGLONG hProcess;
  BOOL bCanHookNow;
  BOOL bIs64BitProcess;
} NKT_DV_TMSG_ONCREATEPROCESS;

typedef struct tagNKT_DV_TMSG_ONLOADLIBRARY : public NKT_DV_TMSG_COMMON {
  DWORD dwDllNameBigPacketId;
} NKT_DV_TMSG_ONLOADLIBRARY;

typedef struct tagNKT_DV_TMSG_ONFREELIBRARY : public NKT_DV_TMSG_COMMON {
  ULONGLONG aInstances[NKT_DV_TMSG_SIZE_A / sizeof(ULONGLONG)];
} NKT_DV_TMSG_ONFREELIBRARY;

typedef struct tagNKT_DV_TMSG_ADDHOOK : public NKT_DV_TMSG_COMMON {
  DWORD dwHookId, dwSpyMgrHookId;
  DWORD dwBatchId;
  ULONGLONG lpAddress;
  LONG nFlags;
  ULONG nDbFuncId;
  WCHAR szFunctionNameW[NKT_DV_TMSG_SIZE_A];
  DWORD dwFunctionNameBigPacketId;
  DWORD dwCustomHandlersBigPacketId;
} NKT_DV_TMSG_ADDHOOK;

typedef struct tagNKT_DV_TMSG_REMOVEHOOK : public NKT_DV_TMSG_COMMON {
  DWORD dwHookId;
  DWORD dwBatchId;
} NKT_DV_TMSG_REMOVEHOOK;

typedef struct tagNKT_DV_TMSG_ENABLEHOOK : public NKT_DV_TMSG_COMMON {
  DWORD dwHookId;
  DWORD dwBatchId;
  BOOL bEnable;
} NKT_DV_TMSG_ENABLEHOOK;

typedef struct tagNKT_DV_TMSG_ONHOOKCALLED_PARAM {
  ULONGLONG lpAddress;
  int nRegister;
} NKT_DV_TMSG_ONHOOKCALLED_PARAM;

typedef struct tagNKT_DV_TMSG_ONHOOKCALLED : public NKT_DV_TMSG_COMMON {
  DWORD dwHookId;
  BOOL bASync;
  BOOL bPreCall;
  LONG nCookie;
  ULONGLONG lpRemoteCurrCallState;
  ULONGLONG lpRemoteIntercallCustomData;
  DWORD dwOsLastErrorCopy;
  ULONGLONG nStackTraceCopy[NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth];
  DWORD dwThreadId;
  double nCurrentTimeMs, nElapsedTimeMs, nChildsElapsedTimeMs;
  double nKernelTimeMs, nUserTimeMs;
  ULONGLONG nCpuClockCycles;
  DWORD dwChainDepth;
  struct {
    ULONGLONG nParamsCount;
    NKT_DV_TMSG_ONHOOKCALLED_PARAM sParams[16];
    NKT_DV_TMSG_ONHOOKCALLED_PARAM sResult;
    ULONGLONG nCustomParamsCount;
    DWORD dwCustomParamsBigPacketId;
  } sParamsData;
} NKT_DV_TMSG_ONHOOKCALLED;

typedef struct tagNKT_DV_TMSG_HOOKSTATECHANGE : public NKT_DV_TMSG_COMMON {
  DWORD dwHookId;
  DWORD dwNewState;
} NKT_DV_TMSG_HOOKSTATECHANGE;

typedef struct tagNKT_DV_TMSG_SUSPENDX64THREADFROMX86 : public NKT_DV_TMSG_COMMON {
  ULONGLONG hProcess;
  ULONGLONG hMainThread;
} NKT_DV_TMSG_SUSPENDX64THREADFROMX86;

typedef struct tagNKT_DV_TMSG_SUSPENDX64THREADFROMX86_CALLBACK : public NKT_DV_TMSG_COMMON {
  ULONGLONG hReadyEvent;
  ULONGLONG hContinueEvent;
  HRESULT hRes;
} NKT_DV_TMSG_SUSPENDX64THREADFROMX86_CALLBACK;

typedef struct tagNKT_DV_TMSG_BATCHHOOKEXEC : public NKT_DV_TMSG_COMMON {
  DWORD dwBatchId;
} NKT_DV_TMSG_BATCHHOOKEXEC;

typedef struct tagNKT_DV_TMSG_HELPERCALL : public NKT_DV_TMSG_COMMON {
  DWORD dwCallCode;
  union {
    struct {
      ULONGLONG hWnd;
    } sGetWndProc;
    struct {
      DWORD aHookIdsList[256];
      DWORD dwCount;
    } sQueryHookOverwrite;
  };
} NKT_DV_TMSG_HELPERCALL;

typedef struct tagNKT_DV_TMSG_HELPERCALL_CALLBACK : public NKT_DV_TMSG_COMMON {
  HRESULT hRes;
  union {
    struct {
      ULONGLONG lpWndProc;
    } sGetWndProc;
    struct {
      BYTE aOverwritten[256];
    } sQueryHookOverwrite;
  };
} NKT_DV_TMSG_HELPERCALL_CALLBACK;

typedef struct tagNKT_DV_TMSG_CUSTOMMESSAGE : public NKT_DV_TMSG_COMMON {
  ULONGLONG nCode, nParam;
  DWORD dwThreadId;
  BOOL bASync;
  ULONGLONG nAddressOfReturnValue;
} NKT_DV_TMSG_CUSTOMMESSAGE;

typedef struct tagNKT_DV_TMSG_HOOKOVERWRITTEN : public NKT_DV_TMSG_COMMON {
  DWORD aHookIdsList[256];
  DWORD dwCount;
} NKT_DV_TMSG_HOOKOVERWRITTEN;

/*
typedef struct tagNKT_DV_TMSG_INITIALCONNECTIONMESSAGE : public NKT_DV_TMSG_COMMON {
  struct {
    ULONGLONG nPtr, nSize;
  } sDllInfo, sModifiedImage;
} NKT_DV_TMSG_INITIALCONNECTIONMESSAGE;
*/
#pragma pack()

//-----------------------------------------------------------

class CNktDvTransportMessageCallbackHelperBase
{
public:
  virtual HRESULT Initialize()=0;
  virtual HRESULT CopyResponseAndSignal(__in NKT_DV_TMSG_COMMON *lpMsg, __in SIZE_T nMsgSize)=0;
  virtual HRESULT Wait(__in HANDLE hGlobalShutdownEvent, __in HANDLE hConnShutdownEvent,
                       __in HANDLE hExtraShutdownEvent=NULL)=0;
};

template <typename RESP>
class TNktDvTransportMessageCallbackHelper : public CNktDvTransportMessageCallbackHelperBase
{
public:
  virtual HRESULT Initialize()
    {
    return (cEv.Create(TRUE, FALSE) != FALSE) ? S_OK : E_OUTOFMEMORY;
    }

  virtual HRESULT CopyResponseAndSignal(__in NKT_DV_TMSG_COMMON *lpMsg, __in SIZE_T nMsgSize)
    {
    NKT_ASSERT(nMsgSize <= NKT_DV_TRANSPORT_MAX_DATA_SIZE);
    if (nMsgSize != sizeof(RESP))
      return NKT_DVERR_InvalidTransportData;
    _ReadWriteBarrier();
    nktMemCopy(aBuf, lpMsg, nMsgSize);
    _ReadWriteBarrier();
    return (cEv.Set() != FALSE) ? S_OK : E_FAIL;
    }

  virtual RESP* operator->()
    {
    return &sMsg;
    };

  virtual HRESULT Wait(__in HANDLE hGlobalShutdownEvent, __in HANDLE hConnShutdownEvent,
                       __in HANDLE hExtraShutdownEvent=NULL)
    {
    HANDLE hEvents[4];

    hEvents[0] = cEv.GetEventHandle();
    hEvents[1] = hGlobalShutdownEvent;
    hEvents[2] = hConnShutdownEvent;
    hEvents[3] = hExtraShutdownEvent;
    switch (::WaitForMultipleObjects((hExtraShutdownEvent == NULL) ? 3 : 4, hEvents, FALSE, INFINITE))
    {
      case WAIT_OBJECT_0:
        break;
      case WAIT_OBJECT_0+1:
      case WAIT_OBJECT_0+2:
      case WAIT_OBJECT_0+3:
        return NKT_DVERR_Cancelled;
      default:
        return E_FAIL;
    }
    return S_OK;
    };

private:
  CNktEvent cEv;
  union {
    RESP sMsg;
    BYTE aBuf[NKT_DV_TRANSPORT_MAX_DATA_SIZE];
  };
};

//-----------------------------------------------------------

__inline HRESULT NktDvTransportInitMessage(__inout NKT_DV_TMSG_COMMON *lpMsg, __in DWORD dwCode,
                                           __in DWORD dwId, __in HANDLE hProcessedEvent,
                                           __in CNktDvTransportMessageCallbackHelperBase *lpCallback=NULL)
{
  lpMsg->dwMsgCode = dwCode;
  lpMsg->dwMsgId = dwId;
  lpMsg->hProcessedEvent = NKT_PTR_2_ULONGLONG(hProcessedEvent);
  lpMsg->nPtrCallback = NKT_PTR_2_ULONGLONG(lpCallback);
  if (lpCallback != NULL)
  {
    HRESULT hRes = lpCallback->Initialize();
    if (FAILED(hRes))
      return hRes;
  }
  return S_OK;
}

__inline VOID NktDvTransportInitMessage4Callback(__inout NKT_DV_TMSG_COMMON *lpMsg, __in DWORD dwCode,
                                                 __in NKT_DV_TMSG_COMMON *lpOrigMsg)
{
  lpMsg->dwMsgCode = dwCode;
  lpMsg->dwMsgId = lpOrigMsg->dwMsgId | NKT_DV_TMSG_ID_CALLBACK;
  lpMsg->hProcessedEvent = (ULONGLONG)NULL;
  lpMsg->nPtrCallback = lpOrigMsg->nPtrCallback;
  return;
}

//-----------------------------------------------------------

