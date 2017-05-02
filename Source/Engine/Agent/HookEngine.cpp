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

#include "HookEngineInternals.h"
#include "HookEngineCallDataEntry.h"
#include "HookEngineMiscHelpers.h"
#include <intrin.h>

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

#define CALC_STACK_PRESERVE(x)                             \
                 (NKT_DV_ALIGN_SIZE_T(x) + 4*sizeof(SIZE_T))

#define DUMMY_CALC_STACK_PRESERVE_SIZE      4*sizeof(SIZE_T)

#define MAX_SUSPEND_IPRANGES                              10

//-----------------------------------------------------------

#if defined _M_IX86
  EXTERN_C void __stdcall NktDvSuperHook_x86();
#elif defined _M_X64
  EXTERN_C void __stdcall NktDvSuperHook_x64();
#else
  #error Unsupported platform
#endif

static SIZE_T __stdcall PreCallCommon(__inout CNktDvHookEngine *lpEnginePtr, __in LPVOID lpHookEntry,
                                      __in SIZE_T nStackPointer);
static SIZE_T __stdcall PostCallCommon(__inout CNktDvHookEngine *lpEnginePtr, __in LPVOID lpHookEntry,
                                       __in SIZE_T nStackPointer);

#if defined _M_IX86
static __inline VOID LoadRegisters(__out LPNKT_DV_ASMREGISTERS32 lpRegisters, __out PSIZE_T lpnReturnAddr,
                                   __in SIZE_T nStackPointer, __inout CHookEntry *lpHookEntry, __in BOOL bPreCall);
static __inline VOID SaveRegisters(__in LPNKT_DV_ASMREGISTERS32 lpRegisters, __in SIZE_T nStackPointer);
#elif defined _M_X64
static __inline VOID LoadRegisters(__out LPNKT_DV_ASMREGISTERS64 lpRegisters, __out PSIZE_T lpnReturnAddr,
                                   __in SIZE_T nStackPointer, __inout CHookEntry *lpHookEntry, __in BOOL bPreCall);
static __inline VOID SaveRegisters(__in LPNKT_DV_ASMREGISTERS64 lpRegisters, __in SIZE_T nStackPointer);
#endif
static BOOL SecureMemCopy(__in LPVOID lpDest, __in LPVOID lpSrc, __in SIZE_T nCount);
static BOOL SecureMemIsDiff(__in const void *lpBuf1, __in const void *lpBuf2, __in SIZE_T nCount);
static BOOL SecureCheckStub(__in CHookEntry *lpHookEntry);

//-----------------------------------------------------------

CNktDvHookEngine::CNktDvHookEngine(__in CNktDvHookEngineCallback *_lpCallback) : CNktFastMutex(), CNktMemMgrObj()
{
  NKT_ASSERT(_lpCallback != NULL);
  lpCallback = _lpCallback;
  nBaseStubCopyMaxSize = 0;
  //----
  if (::QueryPerformanceFrequency(&liTimerFreq) == FALSE)
    liTimerFreq.QuadPart = 0;
  NktInterlockedExchange(&nCallCounter, 0);
  return;
}

CNktDvHookEngine::~CNktDvHookEngine()
{
  if (bAppIsExiting == FALSE)
  {
    Finalize();
  }
  return;
}

HRESULT CNktDvHookEngine::Initialize()
{
  CNktAutoFastMutex cLock(this);

  Finalize();
  NktInterlockedExchange(&nCallCounter, 0);
  cCustomHandlersMgr.Attach(NKT_MEMMGR_NEW CHookCustomHandlerMgr);
  if (cCustomHandlersMgr == NULL)
  {
    Finalize();
    return E_OUTOFMEMORY;
  }
  return S_OK;
}

VOID CNktDvHookEngine::Finalize()
{
  CNktAutoFastMutex cLock(this);

  UnhookAll();
  cCustomHandlersMgr.Release();
  return;
}

HRESULT CNktDvHookEngine::Hook(__in HOOKINFO aHookInfo[], __in SIZE_T nCount, __in BOOL bIsInternal)
{
  CNktDvHookEngineAutoTlsLock cAutoTls;
  CNktAutoFastMutex cLock(this);
  TNktArrayListWithRelease<CHookEntry*> cNewEntryList;
  TNktLnkLst<CHookEntry>::Iterator it;
  TNktAutoFreePtr<CNktHookLib::LPHOOK_INFO> cHookInfoList;
  CNktStringW cStrTempW[2];
  CHookEntry *lpHookEntry;
  SIZE_T nHookIdx;
  HRESULT hRes;

  if (aHookInfo == 0)
    return E_POINTER;
  if (nCount == 0)
    return E_INVALIDARG;
  if (FAILED(cAutoTls.GetError()))
    return cAutoTls.GetError();
  if ((::GetVersion() & 0x80000000) != 0)
    return E_NOTIMPL; //reject win9x
  //check parameters
  for (nHookIdx=0; nHookIdx<nCount; nHookIdx++)
  {
    if (aHookInfo[nHookIdx].lpProcToHook == NULL)
      return E_POINTER;
    if ((aHookInfo[nHookIdx].nFlags & (NKT_DV_TMSG_ADDHOOK_FLAG_OnlyPreCall|NKT_DV_TMSG_ADDHOOK_FLAG_OnlyPostCall)) ==
                  (NKT_DV_TMSG_ADDHOOK_FLAG_OnlyPreCall|NKT_DV_TMSG_ADDHOOK_FLAG_OnlyPostCall))
      return E_INVALIDARG;
    //check if already exists
    for (lpHookEntry=it.Begin(cHooksList); lpHookEntry!=NULL; lpHookEntry=it.Next())
    {
      if (lpHookEntry->dwId == aHookInfo[nHookIdx].dwHookId)
        return NKT_DVERR_AlreadyExists;
    }
  }
  cHookInfoList.Attach((CNktHookLib::LPHOOK_INFO*)nktMemMalloc(nCount * sizeof(CNktHookLib::LPHOOK_INFO)));
  if (!cHookInfoList)
    return E_OUTOFMEMORY;
  //create entries for each item
  hRes = S_OK;
  for (nHookIdx=0; nHookIdx<nCount; nHookIdx++)
  {
    LPBYTE lpBaseStub, lpPtr;
    SIZE_T k, k2, nBaseStubSize;
    DWORD dw, dwOldProt;

    //allocate new entry
    lpHookEntry = NKT_MEMMGR_NEW CHookEntry();
    if (lpHookEntry == NULL)
    {
      NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CNktDvHookEngine[New Hook Entry]: hRes=%08X",
                        ::GetTickCount(), E_OUTOFMEMORY));
      hRes = E_OUTOFMEMORY;
      break;
    }
    cHookInfoList[nHookIdx] = &(lpHookEntry->sHookLibInfo);
    lpHookEntry->cCustomHandlersMgr = cCustomHandlersMgr;
    lpHookEntry->dwId = aHookInfo[nHookIdx].dwHookId;
    lpHookEntry->dwSpyMgrId = aHookInfo[nHookIdx].dwSpyMgrHookId;
    lpHookEntry->sHookLibInfo.lpProcToHook = lpHookEntry->lpOrigProc = aHookInfo[nHookIdx].lpProcToHook;
    if ((aHookInfo[nHookIdx].nFlags & NKT_DV_TMSG_ADDHOOK_FLAG_DontSkipJumps) != 0)
    {
      lpHookEntry->sHookLibInfo.lpProcToHook =
        MiscHelpers::SkipJumpInstructions((LPBYTE)(lpHookEntry->sHookLibInfo.lpProcToHook));
    }
    lpHookEntry->lpDbFunc = aHookInfo[nHookIdx].lpDbFunc;
    if (aHookInfo[nHookIdx].szFunctionNameW != NULL && aHookInfo[nHookIdx].szFunctionNameW[0] != 0)
    {
      dw = (lpHookEntry->cStrFunctionNameW.Copy(aHookInfo[nHookIdx].szFunctionNameW) != FALSE) ? 1 : 0;
    }
    else if (lpHookEntry->lpDbFunc != NULL)
    {
      dw = (lpHookEntry->cStrFunctionNameW.Copy(lpHookEntry->lpDbFunc->GetName()) != FALSE) ? 1 : 0;
    }
    else
    {
      dw = (lpHookEntry->cStrFunctionNameW.Format(L"0x%0*Ix", (int)(sizeof(SIZE_T)>>3),
                                                  (SIZE_T)(aHookInfo[nHookIdx].lpProcToHook)) != FALSE) ? 1 : 0;
    }
    if (dw == 0)
    {
      NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CNktDvHookEngine[HookEntry Func Name]: hRes=%08X",
                        ::GetTickCount(), E_OUTOFMEMORY));
      lpHookEntry->Release();
      hRes = E_OUTOFMEMORY;
      break;
    }
    lpHookEntry->nStackReturnSize = (lpHookEntry->lpDbFunc != NULL) ? 
                    CNktDvFunctionParamsCache::GetStackUsage(lpHookEntry->lpDbFunc) : NKT_SIZE_T_MAX;
    lpHookEntry->nFlags = HOOKENG_FLAG_CallPreCall|HOOKENG_FLAG_CallPostCall;
    if ((aHookInfo[nHookIdx].nFlags & NKT_DV_TMSG_ADDHOOK_FLAG_AsyncCallbacks) != 0)
      lpHookEntry->nFlags |= HOOKENG_FLAG_AsyncCallbacks;
    if ((aHookInfo[nHookIdx].nFlags & NKT_DV_TMSG_ADDHOOK_FLAG_DontCallOnLdrLock) != 0)
      lpHookEntry->nFlags |= HOOKENG_FLAG_DontCallOnLdrLock;
    if ((aHookInfo[nHookIdx].nFlags & NKT_DV_TMSG_ADDHOOK_FLAG_DontCallCustomHandlersOnLdrLock) != 0)
      lpHookEntry->nFlags |= HOOKENG_FLAG_DontCallCustomHandlersOnLdrLock;
    if ((aHookInfo[nHookIdx].nFlags & NKT_DV_TMSG_ADDHOOK_FLAG_OnlyPreCall) != 0)
      lpHookEntry->nFlags &= ~HOOKENG_FLAG_CallPostCall;
    else if ((aHookInfo[nHookIdx].nFlags & NKT_DV_TMSG_ADDHOOK_FLAG_OnlyPostCall) != 0)
      lpHookEntry->nFlags &= ~HOOKENG_FLAG_CallPreCall;
    if ((aHookInfo[nHookIdx].nFlags & NKT_DV_TMSG_ADDHOOK_FLAG_InvalidateCache) != 0)
      lpHookEntry->nFlags |= HOOKENG_FLAG_InvalidateCache;
    if ((aHookInfo[nHookIdx].nFlags & NKT_DV_TMSG_ADDHOOK_FLAG_DisableStackWalk) != 0)
      lpHookEntry->nFlags |= HOOKENG_FLAG_DisableStackWalk;
    hRes = MiscHelpers::FindDll(&(lpHookEntry->hProcDll), aHookInfo[nHookIdx].lpProcToHook);
    if (FAILED(hRes) && hRes != NKT_DVERR_NotFound)
    {
      lpHookEntry->Release();
      break;
    }
    //initialize memory reader/writer
    if (cProcMem == NULL)
    {
      cProcMem.Attach(CNktDvProcessMemory::CreateForPID(::GetCurrentProcessId()));
      if (cProcMem == NULL)
      {
        NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CNktDvHookEngine[ProcMem assign]: hRes=%08X",
                           ::GetTickCount(), E_OUTOFMEMORY));
        lpHookEntry->Release();
        hRes = E_OUTOFMEMORY;
        break;
      }
    }
    hRes = lpHookEntry->cFuncParamCache.Initialize(aHookInfo[nHookIdx].lpDbFunc, cProcMem,
                                                   sizeof(SIZE_T)<<3);
    if (FAILED(hRes))
    {
      NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CNktDvHookEngine[FuncParam init]: hRes=%08X",
                         ::GetTickCount(), hRes));
      lpHookEntry->Release();
      break;
    }
    //attach custom handlers
    if (aHookInfo[nHookIdx].sCustomHandlers.lpData != NULL && aHookInfo[nHookIdx].sCustomHandlers.nDataSize > 0)
    {
      SIZE_T nLen[2], nSize, nCount, nFlags;
      LPBYTE d;

      if ((nSize = aHookInfo[nHookIdx].sCustomHandlers.nDataSize) < sizeof(SIZE_T))
      {
hk_badtransport:
        NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CNktDvHookEngine[Transport data]: hRes=%08X",
                           ::GetTickCount(), NKT_DVERR_InvalidTransportData));
        lpHookEntry->Release();
        hRes = NKT_DVERR_InvalidTransportData;
        break;
      }
      d = aHookInfo[nHookIdx].sCustomHandlers.lpData;
      nCount = (SIZE_T)(*((ULONG NKT_UNALIGNED *)d));
      d += sizeof(ULONG);
      nSize -= sizeof(ULONG);
      while (nCount > 0)
      {
        if (nSize < 3*sizeof(ULONG))
          goto hk_badtransport;
        nFlags = (SIZE_T)(*((ULONG NKT_UNALIGNED *)d));
        d += sizeof(ULONG);
        nLen[0] = (SIZE_T)(*((ULONG NKT_UNALIGNED *)d));
        d += sizeof(ULONG);
        nLen[1] = (SIZE_T)(*((ULONG NKT_UNALIGNED *)d));
        d += sizeof(ULONG);

        nSize -= 3*sizeof(ULONG);
        //copy dll name
        if (nSize < (nLen[0]+nLen[1])*sizeof(WCHAR))
          goto hk_badtransport;
        if (cStrTempW[0].CopyN((LPWSTR)d, nLen[0]) == FALSE ||
            cStrTempW[1].CopyN(((LPWSTR)d)+nLen[0], nLen[1]) == FALSE)
        {
          lpHookEntry->Release();
          hRes = E_OUTOFMEMORY;
          break;
        }
        nLen[0] *= sizeof(WCHAR);
        nLen[1] *= sizeof(WCHAR);
        d += nLen[0]+nLen[1];
        nSize -= nLen[0]+nLen[1];
        //initialize custom handler
        hRes = lpHookEntry->AttachCustomHandler((LPWSTR)cStrTempW[0], nFlags, (LPWSTR)cStrTempW[1], lpCallback);
        if (FAILED(hRes))
        {
          NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CNktDvHookEngine[AttachCustomHandler]: hRes=%08X",
                             ::GetTickCount(), hRes));
          lpHookEntry->Release();
          break;
        }
        nCount--;
      }
    }
    //calculate base stub code size
#if defined _M_IX86
    lpBaseStub = (LPBYTE)MiscHelpers::SkipJumpInstructions((LPBYTE)NktDvSuperHook_x86);
#elif defined _M_X64
    lpBaseStub = (LPBYTE)MiscHelpers::SkipJumpInstructions((LPBYTE)NktDvSuperHook_x64);
#endif
    for (lpPtr=lpBaseStub; ; lpPtr++)
    {
#if defined _M_IX86
      if (*((ULONG NKT_UNALIGNED *)lpPtr) == 0xFFDDFFFF)
        break;
#elif defined _M_X64
      if (*((ULONGLONG NKT_UNALIGNED *)lpPtr) == 0xFFDDFFDDFFDDFFFF)
        break;
#endif
    }
    nBaseStubSize = (SIZE_T)(lpPtr - lpBaseStub);
    NKT_ASSERT(nBaseStubSize > 0);
    //allocate memory for inject code
    lpHookEntry->lpSuperHookStub = AllocateForBaseStubCopy();
    if (lpHookEntry->lpSuperHookStub == NULL)
    {
      NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CNktDvHookEngine[Code area alloc]: hRes=%08X",
                         ::GetTickCount(), E_OUTOFMEMORY));
      lpHookEntry->Release();
      hRes = E_OUTOFMEMORY;
      break;
    }
    //change block protection
    if (::VirtualProtect(lpHookEntry->lpSuperHookStub, nBaseStubSize, PAGE_EXECUTE_READWRITE, &dwOldProt) == FALSE)
    {
      hRes = NKT_HRESULT_FROM_LASTERROR();
      NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CNktDvHookEngine[Protect change]: hRes=%08X",
                        ::GetTickCount(), hRes));
      FreeBaseStubCopy(lpHookEntry);
      lpHookEntry->Release();
      break;
    }
    //copy code and patch
    nktMemCopy(lpHookEntry->lpSuperHookStub, lpBaseStub, nBaseStubSize);
    for (k=0; k<nBaseStubSize; )
    {
      lpPtr = lpHookEntry->lpSuperHookStub + k;
#if defined _M_IX86
      switch (*((ULONG NKT_UNALIGNED *)lpPtr))
      {
        case 0xFFDDFF01: //USAGE_COUNT
          *((ULONG NKT_UNALIGNED *)lpPtr) = (ULONG)((ULONG_PTR)&(lpHookEntry->nUsageCount));
          k += sizeof(ULONG);
          break;
        case 0xFFDDFF02: //HOOK_ENGINE_PTR
          *((ULONG NKT_UNALIGNED *)lpPtr) = (ULONG)((ULONG_PTR)this);
          k += sizeof(ULONG);
          break;
        case 0xFFDDFF03: //HOOK_ENTRY_PTR
          *((ULONG NKT_UNALIGNED *)lpPtr) = (ULONG)((ULONG_PTR)lpHookEntry);
          k += sizeof(ULONG);
          break;
        case 0xFFDDFF04: //PRECALL_ADDR
          *((ULONG NKT_UNALIGNED *)lpPtr) = (ULONG)((ULONG_PTR)PreCallCommon);
          k += sizeof(ULONG);
          break;
        case 0xFFDDFF05: //POSTCALL_ADDR
          *((ULONG NKT_UNALIGNED *)lpPtr) = (ULONG)((ULONG_PTR)PostCallCommon);
          k += sizeof(ULONG);
          break;
        case 0xFFDDFF06: //JMP_TO_ORIGINAL
          *((ULONG NKT_UNALIGNED *)lpPtr) = (ULONG)((ULONG_PTR)&(lpHookEntry->sHookLibInfo.lpCallOriginal));
          k += sizeof(ULONG);
          break;
        case 0xFFDDFF07: //CONTINUE_AFTER_CALL_MARK
          lpHookEntry->lpAfterCallMark = lpPtr + sizeof(ULONG);
          k += sizeof(ULONG);
          break;
        case 0xFFDDFF08: //AFTER_CALL_STACK_PRESERVE_SIZE
          //preserve stack size for postcall plus some extra
          if (lpHookEntry->nStackReturnSize == NKT_SIZE_T_MAX)
            k2 = CALC_STACK_PRESERVE(DUMMY_CALC_STACK_PRESERVE_SIZE);
          else
            k2 = CALC_STACK_PRESERVE(lpHookEntry->nStackReturnSize);
          *((ULONG NKT_UNALIGNED *)lpPtr) = (ULONG)((ULONG_PTR)k2);
          k += sizeof(ULONG);
          break;
        case 0xFFDDFF09: //AFTER_CALL_STACK_PRESERVE_SIZE2
          //preserve stack size for postcall plus some extra
          if (lpHookEntry->nStackReturnSize == NKT_SIZE_T_MAX)
            k2 = CALC_STACK_PRESERVE(DUMMY_CALC_STACK_PRESERVE_SIZE);
          else
            k2 = CALC_STACK_PRESERVE(lpHookEntry->nStackReturnSize);
          *((ULONG NKT_UNALIGNED *)lpPtr) = 0; //zero
          lpPtr[0] = (BYTE)( k2       & 0xFF);
          lpPtr[1] = (BYTE)((k2 >> 8) & 0xFF);
          k += sizeof(ULONG);
          break;
        default:
          k++;
          break;
      }
#elif defined _M_X64
      switch (*((ULONGLONG NKT_UNALIGNED *)lpPtr))
      {
        case 0xFFDDFFDDFFDDFF01: //USAGE_COUNT
          *((ULONGLONG NKT_UNALIGNED *)lpPtr) = (ULONGLONG)((ULONG_PTR)&(lpHookEntry->nUsageCount));
          k += sizeof(ULONGLONG);
          break;
        case 0xFFDDFFDDFFDDFF02: //HOOK_ENGINE_PTR
          *((ULONGLONG NKT_UNALIGNED *)lpPtr) = (ULONGLONG)((ULONG_PTR)this);
          k += sizeof(ULONGLONG);
          break;
        case 0xFFDDFFDDFFDDFF03: //HOOK_ENTRY_PTR
          *((ULONGLONG NKT_UNALIGNED *)lpPtr) = (ULONGLONG)((ULONG_PTR)lpHookEntry);
          k += sizeof(ULONGLONG);
          break;
        case 0xFFDDFFDDFFDDFF04: //PRECALL_ADDR
          *((ULONGLONG NKT_UNALIGNED *)lpPtr) = (ULONGLONG)((ULONG_PTR)PreCallCommon);
          k += sizeof(ULONGLONG);
          break;
        case 0xFFDDFFDDFFDDFF05: //POSTCALL_ADDR
          *((ULONGLONG NKT_UNALIGNED *)lpPtr) = (ULONGLONG)((ULONG_PTR)PostCallCommon);
          k += sizeof(ULONGLONG);
          break;
        case 0xFFDDFFDDFFDDFF06: //JMP_TO_ORIGINAL
          *((ULONGLONG NKT_UNALIGNED *)lpPtr) = (ULONGLONG)((ULONG_PTR)&(lpHookEntry->sHookLibInfo.lpCallOriginal));
          k += sizeof(ULONGLONG);
          break;
        case 0xFFDDFFDDFFDDFF07: //CONTINUE_AFTER_CALL_MARK
          lpHookEntry->lpAfterCallMark = lpPtr + sizeof(ULONGLONG);
          k += sizeof(ULONGLONG);
          break;
        case 0xFFDDFFDDFFDDFF08: //AFTER_CALL_STACK_PRESERVE_SIZE
          //preserve stack size for postcall plus some extra
          if (lpHookEntry->nStackReturnSize == NKT_SIZE_T_MAX)
            k2 = CALC_STACK_PRESERVE(DUMMY_CALC_STACK_PRESERVE_SIZE);
          else
            k2 = CALC_STACK_PRESERVE(lpHookEntry->nStackReturnSize);
          *((ULONGLONG NKT_UNALIGNED *)lpPtr) = (ULONGLONG)k2;
          k += sizeof(ULONGLONG);
          break;
        default:
          k++;
          break;
      }
#endif
    }
    NKT_ASSERT(lpHookEntry->lpAfterCallMark != NULL);
    ::VirtualProtect(lpHookEntry->lpSuperHookStub, nBaseStubSize, dwOldProt, &dw);
    lpHookEntry->sHookLibInfo.lpNewProcAddr = lpHookEntry->lpSuperHookStub;
    if (cNewEntryList.AddElement(lpHookEntry) == FALSE)
    {
      FreeBaseStubCopy(lpHookEntry);
      lpHookEntry->Release();
      hRes = E_OUTOFMEMORY;
      break;
    }
  }
  //hook new items
  if (SUCCEEDED(hRes))
  {
    hRes = NKT_HRESULT_FROM_WIN32(cHookLib.Hook(cHookInfoList.Get(), nCount,
                                                (bIsInternal != FALSE) ? NKTHOOKLIB_DisallowReentrancy : 0));
  }
  //done
  if (SUCCEEDED(hRes))
  {
    for (nHookIdx=0; nHookIdx<nCount; nHookIdx++)
    {
      lpHookEntry = (CHookEntry*)cNewEntryList[nHookIdx];
      cHooksList.PushTail(lpHookEntry);
      lpHookEntry->AddRef();
      nktMemCopy(lpHookEntry->aModifiedEntrypointCode, lpHookEntry->sHookLibInfo.lpProcToHook,
                 sizeof(lpHookEntry->aModifiedEntrypointCode));
    }
  }
  else
  {
    for (nHookIdx = 0; nHookIdx<nCount; nHookIdx++)
    {
      lpHookEntry = (CHookEntry*)cNewEntryList[nHookIdx];
      FreeBaseStubCopy(lpHookEntry);
    }
  }
  return hRes;
}

HRESULT CNktDvHookEngine::Unhook(__in LPDWORD lpdwHookIds, __in SIZE_T nCount)
{
  CNktDvHookEngineAutoTlsLock cAutoTls;
  CHookEntry* aTempHookEntriesList[128];
  CNktHookLib::LPHOOK_INFO aHookLibItemsList[128];
  TNktLnkLst<CHookEntry>::Iterator it;
  CHookEntry *lpHookEntry;
  SIZE_T k, nHookIdx, nStart, nRetryCount, nTempHookEntriesCount;

  //NOTE: should this be moved to a background thread???
  if (FAILED(cAutoTls.GetError()))
    return cAutoTls.GetError();
  if (lpdwHookIds == NULL)
    return E_POINTER;
  if (nCount == 0)
    return E_INVALIDARG;
  //check parameters
  for (nHookIdx=nTempHookEntriesCount=0; nHookIdx<=nCount; nHookIdx++)
  {
    if (nHookIdx < nCount)
    {
      {
        CNktAutoFastMutex cLock(this);

        for (lpHookEntry=it.Begin(cHooksList); lpHookEntry!=NULL; lpHookEntry=it.Next())
        {
          if (lpHookEntry->dwId == lpdwHookIds[nHookIdx])
          {
            cHooksList.Remove(lpHookEntry);
            aTempHookEntriesList[nTempHookEntriesCount] = lpHookEntry;
            aHookLibItemsList[nTempHookEntriesCount] = &(lpHookEntry->sHookLibInfo);
            nTempHookEntriesCount++;
            break;
          }
        }
      }
      if (nTempHookEntriesCount < NKT_DV_ARRAYLEN(aTempHookEntriesList))
        continue;
    }
    else
    {
      if (nTempHookEntriesCount == 0)
        continue;
    }
    //disable hooks
    cHookLib.EnableHook(aHookLibItemsList, nTempHookEntriesCount, FALSE);
    //remove hooks while the stub is not in use
    nStart = 0;
    while (nStart < nTempHookEntriesCount)
    {
      k = nStart;
      for (nRetryCount=5; nRetryCount>0; nRetryCount--)
      {
        for (k=nStart; k<nTempHookEntriesCount; k++)
        {
          if (aTempHookEntriesList[k]->nUsageCount > 0)
            break;
        }
        if (k > nStart)
          break;
        if (nRetryCount > 0)
          ::Sleep(5);
      }
      if (k > nStart)
      {
        cHookLib.Unhook(aHookLibItemsList+nStart, k-nStart);
        while (nStart < k)
        {
          if (bAppIsExiting == FALSE)
            FreeBaseStubCopy(aTempHookEntriesList[nStart]);
          aTempHookEntriesList[nStart]->Release();
          nStart++;
        }
      }
      else
      {
        //hook entry at 'nStart' is still in use, just keep disable... and LEAK!!! :(
        aTempHookEntriesList[nStart]->hProcDll = NULL;
        aTempHookEntriesList[nStart]->lpSuperHookStub = NULL;
        nStart++;
      }
    }
  }
  //done
  return S_OK;
}

VOID CNktDvHookEngine::DllUnloadUnhook(__in HINSTANCE hDll)
{
  TNktLnkLst<CHookEntry>::IteratorRev it;
  CHookEntry *lpHookEntry;
  DWORD dw[64];
  SIZE_T k;

  //disable hooks first
  {
    CNktAutoFastMutex cLock(this);

    for (lpHookEntry=it.Begin(cHooksList); lpHookEntry!=NULL; lpHookEntry=it.Next())
    {
      if (lpHookEntry->hProcDll == hDll)
      {
        cHookLib.EnableHook(lpHookEntry->sHookLibInfo.nHookId, FALSE);
      }
    }
  }
  //unhook in reverse order
  do
  {
    k = 0;
    {
      CNktAutoFastMutex cLock(this);

      for (lpHookEntry=it.Begin(cHooksList); lpHookEntry!=NULL && k<NKT_DV_ARRAYLEN(dw); lpHookEntry=it.Next())
      {
        if (lpHookEntry->hProcDll == hDll)
          dw[k++] = lpHookEntry->dwId;
      }
    }
    if (k > 0)
      Unhook(dw, k);
  }
  while (k > 0);
  return;
}

VOID CNktDvHookEngine::UnhookAll()
{
  TNktLnkLst<CHookEntry>::IteratorRev itRev;
  DWORD dw[64];
  SIZE_T k;
  CHookEntry *lpHookEntry;

  //disable all hooks first
  {
    CNktAutoFastMutex cLock(this);

    for (lpHookEntry = itRev.Begin(cHooksList); lpHookEntry != NULL; lpHookEntry = itRev.Next())
    {
      cHookLib.EnableHook(lpHookEntry->sHookLibInfo.nHookId, FALSE);
    }
  }
  //unhook in reverse order
  do
  {
    {
      CNktAutoFastMutex cLock(this);

      for (lpHookEntry=itRev.Begin(cHooksList),k=0; lpHookEntry!=NULL && k<NKT_DV_ARRAYLEN(dw);
           lpHookEntry=itRev.Next())
      {
        dw[k++] = lpHookEntry->dwId;
      }
    }
    if (k > 0)
      Unhook(dw, k);
  }
  while (k > 0);
  return;
}

HRESULT CNktDvHookEngine::EnableHook(__in DWORD dwHookId, __in BOOL bEnable)
{
  CNktAutoFastMutex cLock(this);
  TNktLnkLst<CHookEntry>::Iterator it;
  CHookEntry *lpHookEntry;

  for (lpHookEntry=it.Begin(cHooksList); lpHookEntry!=NULL; lpHookEntry=it.Next())
  {
    if (lpHookEntry->dwId == dwHookId)
    {
      cHookLib.EnableHook(lpHookEntry->sHookLibInfo.nHookId, bEnable);
      return S_OK;
    }
  }
  return NKT_DVERR_NotFound;
}

BOOL CNktDvHookEngine::CheckIfInTrampoline(__in SIZE_T nCurrIP)
{
  CNktAutoFastMutex cLock(this);
  TNktLnkLst<CHookEntry>::Iterator it;
  CHookEntry *lpHookEntry;

  for (lpHookEntry=it.Begin(cHooksList); lpHookEntry!=NULL; lpHookEntry=it.Next())
  {
    if (nCurrIP >= (SIZE_T)(lpHookEntry->lpSuperHookStub) &&
        nCurrIP < (SIZE_T)(lpHookEntry->lpSuperHookStub) + nBaseStubCopyMaxSize)
    {
      return TRUE;
    }
  }
  return FALSE;
}

HRESULT CNktDvHookEngine::CheckOverwrittenHooks()
{
  TNktLnkLst<CHookEntry>::Iterator it;
  CHookEntry *lpHookEntry;
  DWORD aHookIdsList[1024];
  SIZE_T nCount; //,k;
  HRESULT hRes;

  hRes = S_OK;
  nCount = 0;
  {
    CNktAutoFastMutex cLock(this);

    for (lpHookEntry=it.Begin(cHooksList); lpHookEntry!=NULL && nCount<NKT_DV_ARRAYLEN(aHookIdsList);
         lpHookEntry=it.Next())
    {
      if (lpHookEntry->bChangedInformed == FALSE && (lpHookEntry->dwId & 0x80000000) == 0)
      {
        if (SecureMemIsDiff(lpHookEntry->sHookLibInfo.lpProcToHook, lpHookEntry->aModifiedEntrypointCode, 
                            sizeof(lpHookEntry->aModifiedEntrypointCode)) != FALSE)
        {
          lpHookEntry->bChangedInformed = TRUE;
          aHookIdsList[nCount++] = lpHookEntry->dwId;
        }
      }
    }
  }
  if (nCount > 0)
  {
    hRes = lpCallback->HEC_OnHookOverwritten(aHookIdsList, nCount);
  }
  //TODO: We should retry check if too many hooks were modified but if I lock the list again I can
  //      generate a deadlock, mainly in ThinApp applications
  return hRes;
}

HRESULT CNktDvHookEngine::QueryOverwrittenHooks(__in SIZE_T nCount, __in LPDWORD lpdwHookIds, __out LPBYTE lpnResults)
{
  CNktAutoFastMutex cLock(this);
  TNktLnkLst<CHookEntry>::Iterator it;
  CHookEntry *lpHookEntry;
  SIZE_T i;

  nktMemSet(lpnResults, 0, nCount);
  for (i=0; i<nCount; i++)
  {
    if (lpdwHookIds[i] > 0 && (lpdwHookIds[i] & 0x80000000) == 0)
    {
      for (lpHookEntry=it.Begin(cHooksList); lpHookEntry!=NULL; lpHookEntry=it.Next())
      {
        if (lpdwHookIds[i] == lpHookEntry->dwId)
        {
          if (SecureMemIsDiff(lpHookEntry->sHookLibInfo.lpProcToHook, lpHookEntry->aModifiedEntrypointCode,
                              sizeof(lpHookEntry->aModifiedEntrypointCode)) != FALSE)
          {
            lpnResults[i] = 1;
          }
          break;
        }
      }
    }
  }
  return S_OK;
}

LONG CNktDvHookEngine::GetNextCallCookie()
{
  LONG nCookie;

  do
  {
    nCookie = NktInterlockedIncrement(&nCallCounter);
  }
  while (nCookie == 0);
  return nCookie;
}

SIZE_T CNktDvHookEngine::PreCall(__in LPVOID lpHookEntryVoid, __inout CNktDvTlsData *lpTlsData,
                                 __in SIZE_T nStackPointer, __inout DWORD &dwOsLastError)
{
  CHookEntry *lpHookEntry = (CHookEntry*)lpHookEntryVoid;
  CHookEngCallDataEntry *lpCallDataEntry, *lpPrevCallDataEntry;
  CNktDvHookEngine::CALLINFO sCallInfo;
  CHookEngCallDataEntry::CTiming cFunctionTimingTemp;
  TNktComPtr<CNktDvFunctionParamsCache::FUNCTION_PARAMS> cFuncParams;
  TNktComPtr<CNktDvParamsEnumerator> cCustomParams;
  SIZE_T nReturnAddr, nRetCode;
  BOOL bPreCallCalled;
  HRESULT hRes;

  if (lpHookEntryVoid == NULL)
  {
    lpCallback->HEC_OnError(E_OUTOFMEMORY);
    return 0;
  }
  //get function timing (1a)
  cFunctionTimingTemp.Initialize(liTimerFreq);
  lpTlsData->ThreadTimesAccumulate(cFunctionTimingTemp.ftCreationTime, cFunctionTimingTemp.nKernelMs,
                                   cFunctionTimingTemp.nUserMs, cFunctionTimingTemp.nCpuClockCycles);
  //debug print info
  NKT_DEBUGPRINTLNA(Nektra::dlHookEnginePreCall, ("%lu) HookEngine[PreCall]: Entry=%IXh, OrigProc=%IXh, "
                    "InjCode=%IXh (%S)", ::GetTickCount(), lpHookEntry, lpHookEntry->sHookLibInfo.lpProcToHook,
                    lpHookEntry->sHookLibInfo.lpNewProcAddr, (lpHookEntry->lpDbFunc != NULL) ?
                    lpHookEntry->lpDbFunc->GetName() : L""));
  //check for system thread
  if (lpCallback->HEC_IsSystemThread() != FALSE)
  {
    nRetCode = 0; //ignore this call
    goto pc_exit;
  }
  //invalidate cache?
  if ((lpHookEntry->nFlags & HOOKENG_FLAG_InvalidateCache) != 0)
    CNktDvProcess::MarkModulesEnumeratorAsDirty(::GetCurrentProcessId(), NULL);
  //get function param item from cache
  cFuncParams.Attach(lpHookEntry->cFuncParamCache.GetFuncParam());
  if (cFuncParams == NULL)
  {
    lpCallback->HEC_OnError(E_OUTOFMEMORY);
    nRetCode = 0; //ignore this call
    goto pc_exit;
  }
  //get a calldata object from the TLS free list or allocate one
  lpCallDataEntry = (CHookEngCallDataEntry*)(lpTlsData->sHookCallData.cFreeList.PopHead());
  if (lpCallDataEntry == NULL)
  {
    lpCallDataEntry = NKT_MEMMGR_NEW CHookEngCallDataEntry;
    if (lpCallDataEntry == NULL)
    {
      lpCallback->HEC_OnError(E_OUTOFMEMORY);
      nRetCode = 0; //ignore this call
      goto pc_exit;
    }
  }
  nktMemSet(lpCallDataEntry->aIntercallCustomData, 0, NKT_DV_ARRAYLEN(lpCallDataEntry->aIntercallCustomData));
  //obtain registers data
  LoadRegisters(&(lpCallDataEntry->sCallState.sAsmRegisters), &nReturnAddr, nStackPointer, lpHookEntry, TRUE);
  //assign addresses to parameters
  hRes = CNktDvFunctionParamsCache::PreCall_AssignAddresses(cFuncParams, lpHookEntry->lpDbFunc,
                                                            &(lpCallDataEntry->sCallState.sAsmRegisters));
  if (FAILED(hRes))
  {
    lpCallDataEntry->RemoveNode();
    lpCallDataEntry->cFuncParams.Release();
    lpTlsData->sHookCallData.cFreeList.PushTail(lpCallDataEntry);
    //inform about the error
    lpCallback->HEC_OnError(hRes);
    nRetCode = 0; //ignore this call
    goto pc_exit;
  }
  //init last error and other stuff
  lpCallDataEntry->lpHookEntry = lpHookEntry;
  while ((lpCallDataEntry->nCallDataItemId = NktInterlockedIncrement(&(lpHookEntry->nCallCounter))) == 0);
  lpCallDataEntry->sCallState.dwOsLastError = dwOsLastError;
  lpCallDataEntry->sCallState.dwSkipCall = 0;
  lpPrevCallDataEntry = (CHookEngCallDataEntry*)(lpTlsData->sHookCallData.cInUseStack.GetTail());
  lpCallDataEntry->dwChainDepth = (lpPrevCallDataEntry != NULL) ? (lpPrevCallDataEntry->dwChainDepth+1) : 1;
  lpTlsData->sHookCallData.cInUseStack.PushTail(lpCallDataEntry);
  //save return address
  lpCallDataEntry->nReturnAddr = nReturnAddr;
  lpCallDataEntry->nAfterCallAddr = (SIZE_T)(lpHookEntry->lpAfterCallMark);
  //attach function parameters
  lpCallDataEntry->cFuncParams = cFuncParams;
  while ((lpCallDataEntry->nCookie = NktInterlockedIncrement(&(nCallCounter))) == 0);
  //get stack info
  nktMemSet(lpCallDataEntry->sCallState.nStackTrace, 0, sizeof(lpCallDataEntry->sCallState.nStackTrace));
  if ((lpHookEntry->nFlags & HOOKENG_FLAG_DisableStackWalk) == 0)
  {
#if defined _M_IX86
    GetStackTrace(lpCallDataEntry->sCallState.nStackTrace, NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth, 0,
                  lpCallDataEntry->sCallState.sAsmRegisters.sInteger.Esp,
                  lpCallDataEntry->sCallState.sAsmRegisters.sInteger.Ebp, 0, 0, lpTlsData);
#elif defined _M_X64
    GetStackTrace(lpCallDataEntry->sCallState.nStackTrace, NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth, 0,
                  lpCallDataEntry->sCallState.sAsmRegisters.sInteger.Rip,
                  lpCallDataEntry->sCallState.sAsmRegisters.sInteger.Rsp, 0, 0, lpTlsData);
#endif
  }
  //init call info data
  sCallInfo.dwHookId = lpHookEntry->dwId;
  sCallInfo.nCookie = lpCallDataEntry->nCookie;
  sCallInfo.nPhase = CNktDvHookEngine::phPreCall;
  sCallInfo.lpDbFunc = lpHookEntry->lpDbFunc;
  sCallInfo.bAsyncCallbacks = ((lpHookEntry->nFlags & HOOKENG_FLAG_AsyncCallbacks) != 0) ? TRUE : FALSE;
  sCallInfo.lpCallState = &(lpCallDataEntry->sCallState);
  sCallInfo.dwThreadId = ::GetCurrentThreadId();
  sCallInfo.nCurrentTimeMs = cFunctionTimingTemp.nTimeMs;
  sCallInfo.nElapsedTimeMs = sCallInfo.nChildsElapsedTimeMs = 0.0;
  sCallInfo.nKernelTimeMs = lpTlsData->sThreadTimes.nKernelTimeMs;
  sCallInfo.nUserTimeMs = lpTlsData->sThreadTimes.nUserTimeMs;
  sCallInfo.nCpuClockCycles = lpTlsData->sThreadTimes.nCpuClockCycles;
  sCallInfo.dwChainDepth = lpCallDataEntry->dwChainDepth;
  //call the callback
  bPreCallCalled = FALSE;
  hRes = S_OK; //assume all is ok if not doing a precall
  if ((lpHookEntry->nFlags & HOOKENG_FLAG_CallPreCall) != 0 &&
      ((lpHookEntry->nFlags & HOOKENG_FLAG_DontCallOnLdrLock) == 0 ||
       nktDvNtIsLoaderLoaderLockHeld(FALSE) == FALSE))
  {
    //but first call the custom handlers if they were provided
    if ((lpHookEntry->nFlags & HOOKENG_FLAG_DontCallCustomHandlersOnLdrLock) == 0 ||
        nktDvNtIsLoaderLoaderLockHeld(FALSE) == FALSE)
    {
      hRes = lpHookEntry->ProcessCustomHandler(&cCustomParams, &sCallInfo, cFuncParams->cParameters,
                                               cFuncParams->cResult, lpCallDataEntry->aIntercallCustomData, lpCallback);
    }
    //call the callback
    if (hRes == S_FALSE)
    {
      //a custom handler inform us not to send the message to spymgr
      hRes = S_OK;
    }
    else if (SUCCEEDED(hRes))
    {
      hRes = lpCallback->HEC_OnHookCalled(sCallInfo, lpCallDataEntry->aIntercallCustomData,
                                          lpCallDataEntry->cFuncParams, cCustomParams);
      bPreCallCalled = TRUE;
    }
  }
  //re-assign addresses of parameters after reporting to spymgr and save original stack values
  nktMemCopy(&(lpCallDataEntry->sCallState.sPreCallAsmRegisters),
             &(lpCallDataEntry->sCallState.sAsmRegisters),
             sizeof(lpCallDataEntry->sCallState.sAsmRegisters));
  CNktDvFunctionParamsCache::PreCall_ReassignAddressesAndSaveOriginalStackValues(cFuncParams);
  if (hRes != S_OK)
  {
    //check against S_OK
    //remove calldata from in-use list and move to the free list
    lpCallDataEntry->RemoveNode();
    lpCallDataEntry->cFuncParams.Release();
    lpTlsData->sHookCallData.cFreeList.PushTail(lpCallDataEntry);
    //inform error (if not S_FALSE)
    if (FAILED(hRes))
      lpCallback->HEC_OnError(hRes);
    //set thread times
    cFunctionTimingTemp.Initialize(liTimerFreq);
    lpTlsData->ThreadTimesSetLast(cFunctionTimingTemp.nKernelMs, cFunctionTimingTemp.nUserMs,
                                  cFunctionTimingTemp.nCpuClockCycles);
    //ignore this call
    nRetCode = 0;
    goto pc_exit;
  }
  //check skip call
  if (sCallInfo.lpCallState->dwSkipCall != 0 && lpHookEntry->nStackReturnSize != NKT_SIZE_T_MAX)
  {
    nRetCode = 0x80000000 | lpHookEntry->nStackReturnSize;
    //remove calldata from in-use list and move to the free list
    lpCallDataEntry->RemoveNode();
    lpCallDataEntry->cFuncParams.Release();
    lpTlsData->sHookCallData.cFreeList.PushTail(lpCallDataEntry);
    //update registers
    SaveRegisters(&(sCallInfo.lpCallState->sAsmRegisters), nStackPointer);
    dwOsLastError = sCallInfo.lpCallState->dwOsLastError;
    //set thread times
    cFunctionTimingTemp.Initialize(liTimerFreq);
    lpTlsData->ThreadTimesSetLast(cFunctionTimingTemp.nKernelMs, cFunctionTimingTemp.nUserMs,
                                  cFunctionTimingTemp.nCpuClockCycles);
    //skip call
    goto pc_exit;
  }
  //get function timing (1b)
  lpCallDataEntry->nCurrentTimeMs = sCallInfo.nCurrentTimeMs;
  lpCallDataEntry->cFunctionTiming[0] = cFunctionTimingTemp;
  lpCallDataEntry->cFunctionTiming[1].Initialize(liTimerFreq);
  lpCallDataEntry->nChildsElapsedTimeMs = lpCallDataEntry->nChildOverHeadMs = 0.0;
  lpTlsData->ThreadTimesSetLast(lpCallDataEntry->cFunctionTiming[1].nKernelMs,
                                lpCallDataEntry->cFunctionTiming[1].nUserMs,
                                lpCallDataEntry->cFunctionTiming[1].nCpuClockCycles);
  if (bPreCallCalled != FALSE)
  {
    //update registers
    SaveRegisters(&(lpCallDataEntry->sCallState.sAsmRegisters), nStackPointer);
    dwOsLastError = lpCallDataEntry->sCallState.dwOsLastError;
  }
  //replace return address
#if defined _M_IX86
  *((LPVOID*)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Esp)) = lpHookEntry->lpAfterCallMark;
#elif defined _M_X64
  *((LPVOID*)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Rsp)) = lpHookEntry->lpAfterCallMark;
#endif
  nRetCode = 1; //continue
pc_exit:
  NKT_DEBUGPRINTLNA(Nektra::dlHookEnginePreCall, ("%lu) HookEngine[PreCall-Exit]: Entry=%IXh, Ret=%08X",
                    ::GetTickCount(), lpHookEntry, (DWORD)nRetCode));
  return nRetCode;
}

SIZE_T CNktDvHookEngine::PostCall(__in LPVOID lpHookEntryVoid, __inout CNktDvTlsData *lpTlsData,
                                  __in SIZE_T nStackPointer, __inout DWORD &dwOsLastError)
{
  CHookEntry *lpHookEntry = (CHookEntry*)lpHookEntryVoid;
  CHookEngCallDataEntry *lpCallDataEntry, *lpParentCallDataEntry;
  TNktComPtr<CNktDvParamsEnumerator> cCustomParams;
  CALLINFO sCallInfo;
  SIZE_T nRetAddr;
  CHookEngCallDataEntry::CTiming cFunctionTimingTemp[2];
#if defined _M_IX86
  NKT_DV_ASMREGISTERS32 sAsmRegisters;
#elif defined _M_X64
  NKT_DV_ASMREGISTERS64 sAsmRegisters;
#endif
  double nTimeDiffMs;
  HRESULT hRes;
  BOOL bPostCallCalled;

  //get function timing (2a)
  cFunctionTimingTemp[0].Initialize(liTimerFreq);
  lpTlsData->ThreadTimesAccumulate(cFunctionTimingTemp[0].ftCreationTime, cFunctionTimingTemp[0].nKernelMs,
                                   cFunctionTimingTemp[0].nUserMs, cFunctionTimingTemp[0].nCpuClockCycles);
  //debug print info
  NKT_DEBUGPRINTLNA(Nektra::dlHookEnginePostCall, ("%lu) HookEngine[PostCall]: Entry=%IXh, "
                    "OrigProc=%IXh, InjCode=%IXh (%S)", ::GetTickCount(), lpHookEntry,
                    lpHookEntry->sHookLibInfo.lpProcToHook, lpHookEntry->sHookLibInfo.lpNewProcAddr,
                    (lpHookEntry->lpDbFunc != NULL) ? lpHookEntry->lpDbFunc->GetName() : L""));
  //obtain registers data
  LoadRegisters(&sAsmRegisters, NULL, nStackPointer, lpHookEntry, FALSE);
  //find the original return address (iterate the LIFO queue because the some item may be skipped by an exception)
  while (1)
  {
    lpCallDataEntry = (CHookEngCallDataEntry*)(lpTlsData->sHookCallData.cInUseStack.GetTail());
    if (lpCallDataEntry == NULL || lpCallDataEntry->lpHookEntry == lpHookEntry)
      break;
    //remove calldata from in-use list and move to the free list
    lpCallDataEntry->RemoveNode();
    lpCallDataEntry->cFuncParams.Release();
    lpTlsData->sHookCallData.cFreeList.PushTail(lpCallDataEntry);
  }
  NKT_ASSERT(lpCallDataEntry != NULL);
  if (lpCallDataEntry == NULL)
  {
    ::MessageBoxW(NULL, L"Invalid internal state (1). Terminating application...", L"Deviare2", MB_OK|MB_ICONASTERISK);
    ::TerminateProcess(::GetCurrentProcess(), (UINT)-1);
  }
  lpParentCallDataEntry = (CHookEngCallDataEntry*)(lpCallDataEntry->GetPrevEntry());
  /*
  while (1)
  {
    lpCallDataEntry = (CHookEngCallDataEntry*)(lpTlsData->sHookCallData.cInUseStack.GetTail());
    if (lpCallDataEntry == NULL)
      break;
#if defined _M_IX86
    if (lpCallDataEntry->sCallState.sAsmRegisters.sInteger.Esp <= sAsmRegisters.sInteger.Esp)
      break;
#elif defined _M_X64
    if (lpCallDataEntry->sCallState.sAsmRegisters.sInteger.Rsp <= sAsmRegisters.sInteger.Rsp)
      break;
#endif
    //remove calldata from in-use list and move to the free list
    lpCallDataEntry->RemoveNode();
    lpCallDataEntry->cFuncParams.Release();
    lpTlsData->sHookCallData.cFreeList.PushTail(lpCallDataEntry);
  }
  //get parent entries
  while (1)
  {
    lpParentCallDataEntry = (CHookEngCallDataEntry*)(lpCallDataEntry->GetPrevEntry());
    if (lpParentCallDataEntry == NULL)
      break;
#if defined _M_IX86
    if (lpParentCallDataEntry->sCallState.sAsmRegisters.sInteger.Esp ==
            lpCallDataEntry->sCallState.sAsmRegisters.sInteger.Esp ||
        lpParentCallDataEntry->sCallState.sAsmRegisters.sInteger.Esp > sAsmRegisters.sInteger.Esp)
      break;
#elif defined _M_X64
    if (lpParentCallDataEntry->sCallState.sAsmRegisters.sInteger.Rsp ==
            lpCallDataEntry->sCallState.sAsmRegisters.sInteger.Rsp ||
        lpParentCallDataEntry->sCallState.sAsmRegisters.sInteger.Rsp > sAsmRegisters.sInteger.Rsp)
      break;
#endif
    //remove calldata from in-use list and move to the free list
    lpCallDataEntry->RemoveNode();
    lpCallDataEntry->cFuncParams.Release();
    lpTlsData->sHookCallData.cFreeList.PushTail(lpCallDataEntry);
    lpCallDataEntry = lpParentCallDataEntry;
  }
  NKT_ASSERT(lpCallDataEntry != NULL);
  if (lpCallDataEntry == NULL)
  {
    ::MessageBoxW(NULL, L"Invalid internal state (1). Terminating application...", L"Deviare2", MB_OK|MB_ICONASTERISK);
    ::TerminateProcess(::GetCurrentProcess(), (UINT)-1);
  }
  NKT_ASSERT(lpCallDataEntry->lpHookEntry == lpHookEntry);
  */
  //restore original stack values after calling the original function. some api like
  //kernelbase.dll!CreateFileW overwrites the stack with internal data
  CNktDvFunctionParamsCache::PostCall_RestoreOriginalStackValues(lpCallDataEntry->cFuncParams,
                                                                 &(lpCallDataEntry->sCallState.sPreCallAsmRegisters));
  //get function timing (2c)
  nTimeDiffMs = CHookEngCallDataEntry::CTiming::TimeDiff(cFunctionTimingTemp[0].nTimeMs,
                                                         lpCallDataEntry->cFunctionTiming[1].nTimeMs);
  nTimeDiffMs -= lpCallDataEntry->nChildOverHeadMs;
  if (nTimeDiffMs < 0.0000001)
    nTimeDiffMs = 0.0;
  //init call info data
  nRetAddr = lpCallDataEntry->nReturnAddr;
  lpCallDataEntry->sCallState.dwSkipCall = 0;
  lpCallDataEntry->sCallState.dwOsLastError = dwOsLastError;
  nktMemCopy(&(lpCallDataEntry->sCallState.sAsmRegisters), &sAsmRegisters, sizeof(sAsmRegisters));
  //call the callback
  bPostCallCalled = FALSE;
  hRes = S_OK; //assume all is ok if not doing a postcall
  if ((lpHookEntry->nFlags & HOOKENG_FLAG_CallPostCall) != 0 &&
      ((lpHookEntry->nFlags & HOOKENG_FLAG_DontCallOnLdrLock) == 0 ||
       nktDvNtIsLoaderLoaderLockHeld(FALSE) == FALSE))
  {
    sCallInfo.dwHookId = lpHookEntry->dwId;
    sCallInfo.nPhase = CNktDvHookEngine::phPostCall;
    sCallInfo.nCookie = lpCallDataEntry->nCookie;
    sCallInfo.lpDbFunc = lpHookEntry->lpDbFunc;
    sCallInfo.bAsyncCallbacks = ((lpHookEntry->nFlags & HOOKENG_FLAG_AsyncCallbacks) != 0) ? TRUE : FALSE;
    sCallInfo.dwThreadId = ::GetCurrentThreadId();
    sCallInfo.nCurrentTimeMs = lpCallDataEntry->nCurrentTimeMs;
    sCallInfo.nElapsedTimeMs = nTimeDiffMs;
    sCallInfo.nChildsElapsedTimeMs = lpCallDataEntry->nChildsElapsedTimeMs;
    sCallInfo.lpCallState = &(lpCallDataEntry->sCallState);
    sCallInfo.nKernelTimeMs = lpTlsData->sThreadTimes.nKernelTimeMs;
    sCallInfo.nUserTimeMs = lpTlsData->sThreadTimes.nUserTimeMs;
    sCallInfo.nCpuClockCycles = lpTlsData->sThreadTimes.nCpuClockCycles;
    sCallInfo.dwChainDepth = lpCallDataEntry->dwChainDepth;
    //but first call the custom handlers if they were provided
    if ((lpHookEntry->nFlags & HOOKENG_FLAG_DontCallCustomHandlersOnLdrLock) == 0 ||
        nktDvNtIsLoaderLoaderLockHeld(FALSE) == FALSE)
    {
      hRes = lpHookEntry->ProcessCustomHandler(&cCustomParams, &sCallInfo, lpCallDataEntry->cFuncParams->cParameters,
                                               lpCallDataEntry->cFuncParams->cResult,
                                               lpCallDataEntry->aIntercallCustomData, lpCallback);
    }
    //call the callback
    if (hRes == S_FALSE)
    {
      //a custom handler inform us not to send the message to spymgr
      hRes = S_OK;
    }
    else if (SUCCEEDED(hRes))
    {
      hRes = lpCallback->HEC_OnHookCalled(sCallInfo, lpCallDataEntry->aIntercallCustomData,
                                          lpCallDataEntry->cFuncParams, cCustomParams);
      bPostCallCalled = TRUE;
    }
  }
  //if an error occurs
  if (FAILED(hRes))
    lpCallback->HEC_OnError(hRes);
  //get function timing (2d)
  cFunctionTimingTemp[1].Initialize(liTimerFreq);
  if (lpParentCallDataEntry != NULL)
  {
    lpParentCallDataEntry->nChildsElapsedTimeMs += nTimeDiffMs;
    //----
    nTimeDiffMs = CHookEngCallDataEntry::CTiming::TimeDiff(cFunctionTimingTemp[1].nTimeMs,
                                                           cFunctionTimingTemp[0].nTimeMs);
    lpParentCallDataEntry->nChildOverHeadMs += nTimeDiffMs;
    nTimeDiffMs = CHookEngCallDataEntry::CTiming::TimeDiff(lpCallDataEntry->cFunctionTiming[1].nTimeMs,
                                                           lpCallDataEntry->cFunctionTiming[0].nTimeMs);
    lpParentCallDataEntry->nChildOverHeadMs += nTimeDiffMs;
  }
  //set thread last times
  lpTlsData->ThreadTimesSetLast(cFunctionTimingTemp[1].nKernelMs, cFunctionTimingTemp[1].nUserMs,
                                cFunctionTimingTemp[1].nCpuClockCycles);
  //remove calldata from in-use list and move to the free list
  lpCallDataEntry->RemoveNode();
  lpCallDataEntry->cFuncParams.Release();
  lpTlsData->sHookCallData.cFreeList.PushTail(lpCallDataEntry);
  if (bPostCallCalled != FALSE)
  {
    //update registers
    SaveRegisters(&(sCallInfo.lpCallState->sAsmRegisters), nStackPointer);
    dwOsLastError = sCallInfo.lpCallState->dwOsLastError;
  }
  //return the original return address
  NKT_DEBUGPRINTLNA(Nektra::dlHookEnginePostCall, ("%lu) HookEngine[PostCall-Exit]: Entry=%IXh, "
                    "RetAddr=%IXh", ::GetTickCount(), lpHookEntry, nRetAddr));
  return nRetAddr;
}

LPBYTE CNktDvHookEngine::AllocateForBaseStubCopy()
{
  LPBYTE lpPtr, lpBaseStub, lpBitmap;
  SIZE_T i, nCount, nSlot, nMaxSlots;
  LPBASESTUBCOPY_BLOCK lpBlock;

  //we are inside CNktDvHookEngine's lock so this call is safe
  if (nBaseStubCopyMaxSize == 0)
  {
#if defined _M_IX86
    lpBaseStub = MiscHelpers::SkipJumpInstructions((LPBYTE)NktDvSuperHook_x86);
#elif defined _M_X64
    lpBaseStub = MiscHelpers::SkipJumpInstructions((LPBYTE)NktDvSuperHook_x64);
#endif
    for (lpPtr=lpBaseStub; ; lpPtr++)
    {
#if defined _M_IX86
      if (*((ULONG NKT_UNALIGNED *)lpPtr) == 0xFFDDFFFF)
        break;
#elif defined _M_X64
      if (*((ULONGLONG NKT_UNALIGNED *)lpPtr) == 0xFFDDFFDDFFDDFFFF)
        break;
#endif
    }
    nBaseStubCopyMaxSize = (SIZE_T)(lpPtr - lpBaseStub);
    nBaseStubCopyMaxSize = (nBaseStubCopyMaxSize+31) & (~31);
    nBaseStubCopyMaxSize += 2*sizeof(SIZE_T); //max stub size
    NKT_ASSERT(nBaseStubCopyMaxSize < 65536);
    //http://graphics.stanford.edu/~seander/bithacnBaseStubCopyMaxSizes.html#RoundUpPowerOf2
    nBaseStubCopyMaxSize--;
    nBaseStubCopyMaxSize |= nBaseStubCopyMaxSize >> 1;
    nBaseStubCopyMaxSize |= nBaseStubCopyMaxSize >> 2;
    nBaseStubCopyMaxSize |= nBaseStubCopyMaxSize >> 4;
    nBaseStubCopyMaxSize |= nBaseStubCopyMaxSize >> 8;
    nBaseStubCopyMaxSize |= nBaseStubCopyMaxSize >> 16;
    nBaseStubCopyMaxSize++;
    if (nBaseStubCopyMaxSize < sizeof(LPBYTE))
      nBaseStubCopyMaxSize = sizeof(LPBYTE); //minimum is sizeof(LPBYTE) for free-list pointers
    NKT_ASSERT(nBaseStubCopyMaxSize <= 65536);
  }
  nMaxSlots = 65536 / nBaseStubCopyMaxSize;
  //find a free slot in a previously allocated block
  nCount = cBaseStubCopyCodeBlocks.GetCount();
  for (i=0; i<nCount; i++)
  {
    lpBlock = cBaseStubCopyCodeBlocks[i];
    lpBitmap = lpBlock->aFreeSlotBitmap;
    for (nSlot=0; nSlot<nMaxSlots; nSlot++)
    {
      if ((lpBitmap[nSlot >> 3] & (1 << (nSlot & 7))) != 0)
        break; //got a free slow
    }
    if (nSlot < nMaxSlots)
      break;
  }
  if (i >= nCount)
  {
    //allocate a new block if no free slot was found
    lpBlock = (LPBASESTUBCOPY_BLOCK)malloc(sizeof(BASESTUBCOPY_BLOCK) + (nMaxSlots >> 3));
    if (lpBlock == NULL)
      return NULL;
    lpBlock->lpBaseAddress = (LPBYTE)::VirtualAlloc(NULL, 65536, MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READ);
    if (lpBlock->lpBaseAddress == NULL)
    {
      free(lpBlock);
      return NULL;
    }
    memset(lpBlock->aFreeSlotBitmap, 0xFF, (nMaxSlots >> 3) + 1);
    //add to list
    if (cBaseStubCopyCodeBlocks.AddElement(lpBlock) == FALSE)
    {
      ::VirtualFree(lpBlock->lpBaseAddress, 0, MEM_FREE);
      free(lpBlock);
      return NULL;
    }
    nSlot = 0;
  }
  //mark slot as used
  lpBlock->aFreeSlotBitmap[nSlot >> 3] &= ~(1 << (nSlot & 7));
  //done
  return lpBlock->lpBaseAddress + nBaseStubCopyMaxSize * nSlot;
}

VOID CNktDvHookEngine::FreeBaseStubCopy(__in CHookEntry *lpHookEntry)
{
  //we are inside CNktDvHookEngine's lock so this call is safe
  SIZE_T i, j, nSlot, nCount;
  LPBASESTUBCOPY_BLOCK lpBlock;
  LPBYTE lpBitmap;

  if (lpHookEntry->lpSuperHookStub != NULL)
  {
    nCount = cBaseStubCopyCodeBlocks.GetCount();
    for (i=0; i<nCount; i++)
    {
      lpBlock = cBaseStubCopyCodeBlocks[i];
      if ((LPBYTE)(lpHookEntry->lpSuperHookStub) >= lpBlock->lpBaseAddress &&
          (LPBYTE)(lpHookEntry->lpSuperHookStub) < lpBlock->lpBaseAddress+65536)
      {
        nSlot = (SIZE_T)((LPBYTE)(lpHookEntry->lpSuperHookStub) - lpBlock->lpBaseAddress) / nBaseStubCopyMaxSize;
        lpBitmap = lpBlock->aFreeSlotBitmap;

        NKT_ASSERT((lpBitmap[nSlot >> 3] & (1 << (nSlot & 7))) == 0);
        lpBitmap[nSlot >> 3] |= (1 << (nSlot & 7));
        //check if all slots are free
        for (j=(65536/nBaseStubCopyMaxSize)+1; j>0; j--)
        {
          if (*lpBitmap != 0xFF)
            break;
        }
        if (j == 0)
        {
          cBaseStubCopyCodeBlocks.RemoveElementAt(i);
          ::VirtualFree(lpBlock->lpBaseAddress, 0, MEM_FREE);
          free(lpBlock);
        }
        break;
      }
    }
  }
  return;
}

//-----------------------------------------------------------
//-----------------------------------------------------------

static SIZE_T __stdcall PreCallCommon(__inout CNktDvHookEngine *lpEnginePtr, __inout LPVOID lpHookEntry,
                                      __in SIZE_T nStackPointer)
{
  SIZE_T nRes;
  DWORD dwOsErr;

  dwOsErr = ::GetLastError();
  nRes = 0; //ignore the call
  {
    CNktDvHookEngineAutoTlsLock cAutoTls;
    HRESULT hRes;

    hRes = cAutoTls.GetError();
    if (SUCCEEDED(hRes))
    {
      if (cAutoTls.IsAlreadyLocked() == FALSE)
        nRes = lpEnginePtr->PreCall(lpHookEntry, cAutoTls.GetData(), nStackPointer, dwOsErr);
    }
    else
    {
      //if cancelled => thread is exiting so assume that it is not an error
      //else inform error
      if (hRes != NKT_DVERR_Cancelled)
        lpEnginePtr->PreCall(NULL, NULL, 0, dwOsErr);
    }
  }
  ::SetLastError(dwOsErr);
  return nRes;
}

static SIZE_T __stdcall PostCallCommon(__inout CNktDvHookEngine *lpEnginePtr, __inout LPVOID lpHookEntry,
                                       __in SIZE_T nStackPointer)
{
  SIZE_T nRes;
  DWORD dwOsErr;

  dwOsErr = ::GetLastError();
  {
    CNktDvHookEngineAutoTlsLock cAutoTls;

    nRes = lpEnginePtr->PostCall(lpHookEntry, cAutoTls.GetData(), nStackPointer, dwOsErr);
  }
  ::SetLastError(dwOsErr);
  return nRes;
}

#if defined _M_IX86
static __inline VOID LoadRegisters(__out LPNKT_DV_ASMREGISTERS32 lpRegisters, __out PSIZE_T lpnReturnAddr,
                                   __in SIZE_T nStackPointer, __inout CHookEntry *lpHookEntry, __in BOOL bPreCall)
{
  DWORD dwTemp;

  lpRegisters->sInteger.Eip = (DWORD)(lpHookEntry->lpOrigProc);
  //lpRegisters->sInteger.Eip = (DWORD)(lpHookEntry->lpInjectedCodeAddr);
  dwTemp = *((DWORD*)(nStackPointer+0x00));
  lpRegisters->sInternalUseOnly.dwAligment = dwTemp & 0x7FFFFFFFUL;
  lpRegisters->sInternalUseOnly.Original_St0 =
    lpRegisters->sFloating.St0 = *((double*)(nStackPointer+0x04));
  nStackPointer += lpRegisters->sInternalUseOnly.dwAligment;
  lpRegisters->sInteger.Esp = nStackPointer+0x28;
  if (bPreCall == FALSE)
  {
    //preserve stack size for postcall plus some extra
    if (lpHookEntry->nStackReturnSize == NKT_SIZE_T_MAX)
      lpRegisters->sInteger.Esp += CALC_STACK_PRESERVE(DUMMY_CALC_STACK_PRESERVE_SIZE);
    else
      lpRegisters->sInteger.Esp += CALC_STACK_PRESERVE(lpHookEntry->nStackReturnSize);
    //add to the Esp register the size of the dummy value that will be modified with the real return value
    lpRegisters->sInteger.Esp += sizeof(DWORD);
  }
  lpRegisters->sInteger.Eax = *((DWORD*)(nStackPointer+0x24));
  lpRegisters->sInteger.Ebx = *((DWORD*)(nStackPointer+0x20));
  lpRegisters->sInteger.Ecx = *((DWORD*)(nStackPointer+0x1C));
  lpRegisters->sInteger.Edx = *((DWORD*)(nStackPointer+0x18));
  lpRegisters->sInteger.Ebp = *((DWORD*)(nStackPointer+0x14));
  lpRegisters->sInteger.Esi = *((DWORD*)(nStackPointer+0x10));
  lpRegisters->sInteger.Edi = *((DWORD*)(nStackPointer+0x0C));
  if (lpnReturnAddr != NULL)
    *lpnReturnAddr = *((PSIZE_T)(lpRegisters->sInteger.Esp));
  return;
}

static __inline VOID SaveRegisters(__in LPNKT_DV_ASMREGISTERS32 lpRegisters, __in SIZE_T nStackPointer)
{
  if (lpRegisters->sInternalUseOnly.Original_St0 != lpRegisters->sFloating.St0)
  {
    *((double*)(nStackPointer+0x04)) = lpRegisters->sFloating.St0;
    *((DWORD*)(nStackPointer+0x00)) |= 0x80000000; //set st0 in-use flag
  }
  nStackPointer += lpRegisters->sInternalUseOnly.dwAligment;
  *((DWORD*)(nStackPointer+0x24)) = lpRegisters->sInteger.Eax;
  *((DWORD*)(nStackPointer+0x20)) = lpRegisters->sInteger.Ebx;
  *((DWORD*)(nStackPointer+0x1C)) = lpRegisters->sInteger.Ecx;
  *((DWORD*)(nStackPointer+0x18)) = lpRegisters->sInteger.Edx;
  *((DWORD*)(nStackPointer+0x14)) = lpRegisters->sInteger.Ebp;
  *((DWORD*)(nStackPointer+0x10)) = lpRegisters->sInteger.Esi;
  *((DWORD*)(nStackPointer+0x0C)) = lpRegisters->sInteger.Edi;
  return;
}

#elif defined _M_X64

static __inline VOID LoadRegisters(__out LPNKT_DV_ASMREGISTERS64 lpRegisters, __out PSIZE_T lpnReturnAddr,
                                   __in SIZE_T nStackPointer, __inout CHookEntry *lpHookEntry, __in BOOL bPreCall)
{
  lpRegisters->sInteger.Rip = (DWORD64)(lpHookEntry->lpOrigProc);
  //lpRegisters->sInteger.Rip = (DWORD64)(lpHookEntry->lpInjectedCodeAddr);
  lpRegisters->sInteger.Rax = *((DWORD64*)(nStackPointer+0x20));
  lpRegisters->sInteger.Rbx = *((DWORD64*)(nStackPointer+0x28));
  lpRegisters->sInteger.Rcx = *((DWORD64*)(nStackPointer+0x30));
  lpRegisters->sInteger.Rdx = *((DWORD64*)(nStackPointer+0x38));
  lpRegisters->sInteger.Rbp = *((DWORD64*)(nStackPointer+0x40));
  lpRegisters->sInteger.Rsp = nStackPointer+0x228;
  if (bPreCall == FALSE)
  {
    //add to the Rsp register the size of the dummy value that will be modified with the real return value
    lpRegisters->sInteger.Rsp += sizeof(DWORD64);
  }
  lpRegisters->sInteger.Rsi = *((DWORD64*)(nStackPointer+0x48));
  lpRegisters->sInteger.Rdi = *((DWORD64*)(nStackPointer+0x50));
  lpRegisters->sInteger.R8  = *((DWORD64*)(nStackPointer+0x58));
  lpRegisters->sInteger.R9  = *((DWORD64*)(nStackPointer+0x60));
  lpRegisters->sInteger.R10 = *((DWORD64*)(nStackPointer+0x68));
  lpRegisters->sInteger.R11 = *((DWORD64*)(nStackPointer+0x70));
  _mm_store_sd(&(lpRegisters->sFloating.Xmm0), *((__m128d*)(nStackPointer+0xA0)));
  _mm_store_sd(&(lpRegisters->sFloating.Xmm1), *((__m128d*)(nStackPointer+0xB0)));
  _mm_store_sd(&(lpRegisters->sFloating.Xmm2), *((__m128d*)(nStackPointer+0xC0)));
  _mm_store_sd(&(lpRegisters->sFloating.Xmm3), *((__m128d*)(nStackPointer+0xD0)));
  lpRegisters->sInternalUseOnly.Original_Xmm0 = lpRegisters->sFloating.Xmm0;
  lpRegisters->sInternalUseOnly.Original_Xmm1 = lpRegisters->sFloating.Xmm1;
  lpRegisters->sInternalUseOnly.Original_Xmm2 = lpRegisters->sFloating.Xmm2;
  lpRegisters->sInternalUseOnly.Original_Xmm3 = lpRegisters->sFloating.Xmm3;
  if (lpnReturnAddr != NULL)
    *lpnReturnAddr = *((PSIZE_T)(lpRegisters->sInteger.Rsp));
  return;
}

static __inline VOID SaveRegisters(__in LPNKT_DV_ASMREGISTERS64 lpRegisters, __in SIZE_T nStackPointer)
{
  __declspec(align(16)) __m128d nTemp128;

  *((DWORD64*)(nStackPointer+0x20)) = lpRegisters->sInteger.Rax;
  *((DWORD64*)(nStackPointer+0x28)) = lpRegisters->sInteger.Rbx;
  *((DWORD64*)(nStackPointer+0x30)) = lpRegisters->sInteger.Rcx;
  *((DWORD64*)(nStackPointer+0x38)) = lpRegisters->sInteger.Rdx;
  *((DWORD64*)(nStackPointer+0x40)) = lpRegisters->sInteger.Rbp;
  *((DWORD64*)(nStackPointer+0x48)) = lpRegisters->sInteger.Rsi;
  *((DWORD64*)(nStackPointer+0x50)) = lpRegisters->sInteger.Rdi;
  *((DWORD64*)(nStackPointer+0x58)) = lpRegisters->sInteger.R8;
  *((DWORD64*)(nStackPointer+0x60)) = lpRegisters->sInteger.R9;
  *((DWORD64*)(nStackPointer+0x68)) = lpRegisters->sInteger.R10;
  *((DWORD64*)(nStackPointer+0x70)) = lpRegisters->sInteger.R11;
  if (lpRegisters->sInternalUseOnly.Original_Xmm0 != lpRegisters->sFloating.Xmm0)
  {
    nTemp128 = _mm_set_sd(lpRegisters->sFloating.Xmm0);
    nktMemCopy((__m128d*)(nStackPointer+0xA0), &nTemp128, sizeof(nTemp128));
  }
  if (lpRegisters->sInternalUseOnly.Original_Xmm1 != lpRegisters->sFloating.Xmm1)
  {
    nTemp128 = _mm_set_sd(lpRegisters->sFloating.Xmm1);
    nktMemCopy((__m128d*)(nStackPointer+0xB0), &nTemp128, sizeof(nTemp128));
  }
  if (lpRegisters->sInternalUseOnly.Original_Xmm2 != lpRegisters->sFloating.Xmm2)
  {
    nTemp128 = _mm_set_sd(lpRegisters->sFloating.Xmm2);
    nktMemCopy((__m128d*)(nStackPointer+0xC0), &nTemp128, sizeof(nTemp128));
  }
  if (lpRegisters->sInternalUseOnly.Original_Xmm3 != lpRegisters->sFloating.Xmm3)
  {
    nTemp128 = _mm_set_sd(lpRegisters->sFloating.Xmm3);
    nktMemCopy((__m128d*)(nStackPointer+0xD0), &nTemp128, sizeof(nTemp128));
  }
  return;
}

#endif

static BOOL SecureMemCopy(__in LPVOID lpDest, __in LPVOID lpSrc, __in SIZE_T nCount)
{
  BOOL b = FALSE;

  __try
  {
    nktMemCopy(lpDest, lpSrc, nCount);
    b = TRUE;
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    b = FALSE;
  }
  return b;
}

static BOOL SecureMemIsDiff(__in const void *lpBuf1, __in const void *lpBuf2, __in SIZE_T nCount)
{
  BOOL b = FALSE;

  __try
  {
    b = (nktMemCompare(lpBuf1, lpBuf2, nCount) != 0) ? TRUE : FALSE;
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    b = FALSE;
  }
  return b;
}

static BOOL SecureCheckStub(__in CHookEntry *lpHookEntry)
{
  BOOL b = FALSE;
  SIZE_T k;

  __try
  {
    b = (nktMemCompare(lpHookEntry->sHookLibInfo.lpProcToHook, lpHookEntry->aModifiedEntrypointCode,
                       sizeof(lpHookEntry->aModifiedEntrypointCode)) == 0) ? TRUE : FALSE;
    if (b != FALSE)
    {
      //check double hook
      for (k=0; k<8; k++)
      {
        if (((LPBYTE)(lpHookEntry->sHookLibInfo.lpNewProcAddr))[k] != 0x90)
        {
          b = FALSE;
          break;
        }
      }
    }
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    b = FALSE;
  }
  return b;
}
