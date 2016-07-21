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
#include "HookCustomHandlerMgr.h"
#include "..\..\Common\ComPtr.h"
#include "..\..\Common\Tools.h"
#include "Agent.h"
#include "..\DeviareCOM\InternalCreate.h"
#include "..\DeviareCOM\disp_ids.h"
#include "HookEngine.h"

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//#define CUSTOMHANDLERS_TRY
//#define CUSTOMHANDLERS_CATCH
#define CUSTOMHANDLERS_TRY   try {
#define CUSTOMHANDLERS_CATCH } catch (...) { hRes = E_FAIL; }

#define METHODINDEX_OnLoad                  0
#define METHODINDEX_OnUnload                1
#define METHODINDEX_OnHookAdded             2
#define METHODINDEX_OnHookRemoved           3
#define METHODINDEX_OnFunctionCall          4
#define METHODINDEX_GetFunctionCallbackName 5

//-----------------------------------------------------------

typedef HRESULT (__stdcall *lpfnCorBindToRuntimeEx)(LPCWSTR pwszVersion, LPCWSTR pwszBuildFlavor,
                                                    DWORD startupFlags, REFCLSID rclsid, REFIID riid,
                                                    LPVOID FAR *ppv);
typedef HRESULT (__stdcall *lpfnCLRCreateInstance)(__in REFCLSID clsid, __in REFIID riid,
                                                   __deref_out LPVOID *ppInterface);

//-----------------------------------------------------------

static HRESULT DoLoadLibrary(__out HINSTANCE *lphDll, __inout CNktStringW &cStrDllNameW,
                             __in LPCWSTR szDllNameW);
static HRESULT __stdcall Internal_SendCustomMessage(__in LPVOID lpInternal, __out SIZE_T *lpnRetVal,
                                                    __in SIZE_T nMsgCode, __in SIZE_T nMsgParam, __in BOOL bSync);
static HRESULT __stdcall Internal_ThreadIgnore(__in LPVOID lpInternal, __in DWORD dwTid, __in BOOL bIgnore);
static HRESULT __stdcall Internal_InvalidateCache(__in LPVOID lpInternal, __in SIZE_T hDll);

//-----------------------------------------------------------

//NOTE: This IID should not change but always verify it is correct
const IID CHookCustomHandlerMgr::NKT_IID_INktHookInfo = {
  0x99DE4737, 0xC1B4, 0x49f6, { 0x80, 0x72, 0xA7, 0x64, 0x5E, 0x78, 0xBA, 0x8B }
};

CHookCustomHandlerMgr::CHookCustomHandlerMgr() : CNktDvObject()
{
  NktInterlockedExchange(&nHookInfoCounter, 0);
  hMsCoreeDll = NULL;
  hDeviareComDll = NULL;
  fnDllGetClassObject = NULL;
  fnInternal1 = NULL;
  NktInterlockedExchange(&(sDevCom.nInitialized), 0);
  return;
}

CHookCustomHandlerMgr::~CHookCustomHandlerMgr()
{
  CPlugin *lpPlugin;

  if (bAppIsExiting == FALSE)
  {
    while ((lpPlugin=cLoadedPluginsList.PopHead()) != NULL)
      lpPlugin->Release();
    //----
    if (cCorRuntimeHost != NULL && cAppDomain != NULL)
    {
      cCorRuntimeHost->UnloadDomain((mscorlib::_AppDomain*)cAppDomain);
      //NOTE: Do not call "cCorRuntimeHost->Stop();" in order to let .net runtime to be executed
      //      again on the same process
    }
    //free stuff before library unload mainly if cProc is attached to an object
    cAppDomain.Release();
    cCorRuntimeHost.Release();
    cProc.Release();
    if (hDeviareComDll != NULL)
    {
      /*
      if (fnInternal1 != NULL)
      {
        fnInternal1(2, 0, 0);
      }
      else
      {
      */
        ::FreeLibrary(hDeviareComDll);
      /*
      }
      */
      hDeviareComDll = NULL;
    }
  }
  return;
}

HRESULT CHookCustomHandlerMgr::LoadPlugin(__deref_out CPlugin **lplpPlugin, __in LPCWSTR szDllNameW)
{
  static const struct {
    LPWSTR szNameW;
    BOOL bOptional;
  } sMethodsList[6] = {
    { L"OnLoad",              FALSE },    { L"OnUnload",            FALSE },
    { L"OnHookAdded",         FALSE },    { L"OnHookRemoved",       FALSE },
    { L"OnFunctionCall",      FALSE },    { L"GetFunctionCallbackName", TRUE  }
  };
  CNktAutoFastMutex cLock(&cMtx);
  TNktComPtr<CPlugin> cNewPlugin;
  CNktComBStr cBStrAssemblyFileName, cBStrClassName;
  TNktComPtr<mscorlib::_ObjectHandle> cObjectHandle;
  TNktLnkLst<CPlugin>::Iterator it;
  CNktStringW cStrDllNameW;
  BOOL bIsDotNet;
  CPlugin *lpPlugin;
  HRESULT hRes;
  HINSTANCE hDll;
  VARIANT vtUnwrapped;
  ULONG_PTR nActCtxCookie;
  int i;

  //check parameters
  if (lplpPlugin == NULL)
    return E_POINTER;
  *lplpPlugin = NULL;
  if (szDllNameW == NULL)
    return E_POINTER;
  if (szDllNameW[0] == 0)
    return E_INVALIDARG;
  //init some stuff
  if (cProc == NULL)
  {
    hRes = CNktDvProcess::CreateFromPID(&cProc, ::GetCurrentProcessId());
    if (FAILED(hRes))
    {
      NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookCustomHandlerMgr[CreateProcessFromPID]: "
                        "hRes=%08X (%S)", ::GetTickCount(), hRes, szDllNameW));
      return hRes;
    }
  }
  //try to load the library
  hRes = DoLoadLibrary(&hDll, cStrDllNameW, szDllNameW);
  if (FAILED(hRes))
  {
    NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookCustomHandlerMgr[LoadPlugin]: hRes=%08X (%S)",
                       ::GetTickCount(), hRes, szDllNameW));
    return hRes;
  }
  hRes = IsDotNetModule(hDll);
  if (FAILED(hRes))
  {
    NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookCustomHandlerMgr[IsDotNetModule]: hRes=%08X (%S)",
                      ::GetTickCount(), hRes, (LPWSTR)cStrDllNameW));
    ::FreeLibrary(hDll);
    return hRes;
  }
  bIsDotNet = (hRes == S_OK) ? TRUE : FALSE;
  //check if the plugin is already loaded using the dll full filename obtained by DoLoadLibrary
  //DO NOT use dll handle because in AnyCPU .net dlls may vary
  for (lpPlugin=it.Begin(cLoadedPluginsList); lpPlugin!=NULL; lpPlugin=it.Next())
  {
    if (_wcsicmp(lpPlugin->szDllNameW, (LPWSTR)cStrDllNameW) == 0)
    {
      ::FreeLibrary(hDll); //dec refcount
      *lplpPlugin = lpPlugin;
      (*lplpPlugin)->AddRef();
      return S_OK;
    }
  }
  //initialize DeviareCOM for helper routines
  hRes = InitializeDeviareCOM();
  if (FAILED(hRes))
  {
    NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookCustomHandlerMgr[InitializeDeviareCOM]: "
                       "hRes=%08X", ::GetTickCount(), hRes));
    return hRes;
  }
  //create new plugin
  cNewPlugin.Attach(NKT_MEMMGR_NEW CPlugin(this));
  if (cNewPlugin == NULL)
  {
    ::FreeLibrary(hDll); //dec refcount
    return E_OUTOFMEMORY;
  }
  cNewPlugin->szDllNameW = cStrDllNameW.Detach();
  cNewPlugin->sDotNet.bIsManaged = bIsDotNet;
  cNewPlugin->hDll = hDll;
  //is a normal dll?
  if (bIsDotNet == FALSE)
  {
    //get api's
    hRes = S_OK;
    for (i=0; i<NKT_DV_ARRAYLEN(cNewPlugin->aPluginMethods) && SUCCEEDED(hRes); i++)
    {
      hRes = CNktDvTools::GetProcAddressW(&(cNewPlugin->aPluginMethods[i].lpProc), cNewPlugin->hDll,
                                          sMethodsList[i].szNameW);
      if (hRes == NKT_DVERR_NotFound && sMethodsList[i].bOptional != FALSE)
        hRes = S_OK;
    }
    if (FAILED(hRes))
    {
      NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookCustomHandlerMgr[Find methods]: hRes=%08X (%S)",
                        ::GetTickCount(), hRes, cNewPlugin->szDllNameW));
      return hRes;
    }
  }
  else
  {
    //it is a .net library
    ::FreeLibrary(cNewPlugin->hDll);
    //we are dealing with a .NET assembly
    hRes = InitializeDotNetMgr();
    if (FAILED(hRes))
      return hRes;
    hRes = cBStrAssemblyFileName.Set(cNewPlugin->szDllNameW);
    if (SUCCEEDED(hRes))
      hRes = cBStrClassName.Set(L"DeviarePlugin");
    if (SUCCEEDED(hRes))
      hRes = cAppDomain->CreateInstanceFrom(cBStrAssemblyFileName, cBStrClassName, &cObjectHandle);
    if (FAILED(hRes))
    {
      NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookCustomHandlerMgr[Find .net class]: hRes=%08X "
                        "(%S)", ::GetTickCount(), hRes, cNewPlugin->szDllNameW));
      return hRes;
    }
    //get unwrapped
    ::VariantInit(&vtUnwrapped);
    hRes = cObjectHandle->Unwrap(&vtUnwrapped);
    if (SUCCEEDED(hRes) && vtUnwrapped.vt == VT_DISPATCH)
      hRes = ::VariantChangeType(&vtUnwrapped, &vtUnwrapped, 0, VT_UNKNOWN);
    if (SUCCEEDED(hRes))
    {
      hRes = E_FAIL;
      if (vtUnwrapped.vt == VT_UNKNOWN && vtUnwrapped.punkVal != NULL)
      {
        hRes = vtUnwrapped.punkVal->QueryInterface(__uuidof(mscorlib::_Object),
                                                   (LPVOID*)&(cNewPlugin->sDotNet.cObject));
      }
    }
    ::VariantClear(&vtUnwrapped);
    if (SUCCEEDED(hRes))
      hRes = cNewPlugin->sDotNet.cObject->GetType(&(cNewPlugin->sDotNet.cType));
    //get methods
    for (i=0; i<NKT_DV_ARRAYLEN(cNewPlugin->aPluginMethods) && SUCCEEDED(hRes); i++)
    {
#ifdef USE_MARSHALLING
      hRes = cNewPlugin->FindDotNetMethod(sMethodsList[i].szNameW, &(cNewPlugin->aPluginMethods[i].cDotNetStream));
#else //USE_MARSHALLING
      hRes = cNewPlugin->FindDotNetMethod(sMethodsList[i].szNameW, &(cNewPlugin->aPluginMethods[i].cMethod));
#endif //USE_MARSHALLING
      if (hRes == NKT_DVERR_NotFound && sMethodsList[i].bOptional != FALSE)
        hRes = S_OK;
    }
#ifdef USE_MARSHALLING
    //create interthread marshals
    if (SUCCEEDED(hRes))
    {
      hRes = ::CreateStreamOnHGlobal(NULL, TRUE, &(cNewPlugin->sDotNet.cObjectStream));
      if (SUCCEEDED(hRes))
      {
        hRes = ::CoMarshalInterface(cNewPlugin->sDotNet.cObjectStream, __uuidof(mscorlib::_Object),
                                    cNewPlugin->sDotNet.cObject, MSHCTX_INPROC, NULL, MSHLFLAGS_TABLESTRONG);
      }
    }
#endif //USE_MARSHALLING
    if (FAILED(hRes))
    {
      cNewPlugin->sDotNet.cType.Release();
      NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookCustomHandlerMgr[Load .net plugin]: "
                        "hRes=%08X (%S)", ::GetTickCount(), hRes, cNewPlugin->szDllNameW));
      return hRes;
    }
  }
  //call initialization member
  hRes = ActivateDeviareComContext(&nActCtxCookie);
  if (SUCCEEDED(hRes))
    hRes = cNewPlugin->Call_OnLoad();
  if (nActCtxCookie != 0)
    DeactivateDeviareComContext(nActCtxCookie);
  if (FAILED(hRes))
  {
    NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookCustomHandlerMgr[Call_OnLoad]: "
                      "hRes=%08X (%S)", ::GetTickCount(), hRes, cNewPlugin->szDllNameW));
    return hRes;
  }
  //add to list
  *lplpPlugin = cNewPlugin.Detach();
  (*lplpPlugin)->AddRef();
  cLoadedPluginsList.PushTail(*lplpPlugin);
  //done
  NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookCustomHandlerMgr[LoadPlugin]: hRes=%08X (%S)",
                    ::GetTickCount(), S_OK, (*lplpPlugin)->szDllNameW));
  return S_OK;
}

VOID CHookCustomHandlerMgr::FreeUnusedPlugins()
{
  CNktAutoFastMutex cLock(&cMtx);
  TNktLnkLst<CPlugin>::Iterator it;
  CPlugin *lpPlugin;

  for (lpPlugin=it.Begin(cLoadedPluginsList); lpPlugin!=NULL; lpPlugin=it.Next())
  {
    if (lpPlugin->GetRefCount() == 1)
    {
      lpPlugin->RemoveNode();
      lpPlugin->Release();
    }
  }
  return;
}

HRESULT CHookCustomHandlerMgr::CreateHookInfo(__deref_out IDispatch **ppDisp, __out DWORD *lpdwHookInfoCookie,
                                              __in CHookEntry *lpHookEntry,
                                              __in CNktDvHookEngineCallback *lpHookEngCallback)
{
  DV_AGENTCOM_HOOKINFO_CREATE_DATA sCreateData;
  LPVOID lpPtr;
  HRESULT hRes;
  TNktComPtr<IDispatch> cHookInfo;

  NKT_ASSERT(ppDisp != NULL);
  NKT_ASSERT(lpdwHookInfoCookie != NULL);
  NKT_ASSERT(lpHookEntry != NULL);
  *ppDisp = NULL;
  if (fnDllGetClassObject == NULL || fnInternal1 == NULL)
    return E_FAIL;
  sCreateData.dwId = lpHookEntry->dwSpyMgrId;
  sCreateData.nAddress = (SIZE_T)(lpHookEntry->sHookLibInfo.lpProcToHook);
  sCreateData.lpDbFunc = lpHookEntry->lpDbFunc;
  sCreateData.szFunctionNameW = (LPWSTR)(lpHookEntry->cStrFunctionNameW);
  sCreateData.sCallTrampoline.fnSendCustomMessage = &Internal_SendCustomMessage;
  sCreateData.sCallTrampoline.fnThreadIgnore = &Internal_ThreadIgnore;
  sCreateData.sCallTrampoline.fnInvalidateCache = &Internal_InvalidateCache;
  sCreateData.sCallTrampoline.lpInternal = lpHookEngCallback;
  sCreateData.lpProc = cProc;
  sCreateData.dwCookie[0] = 1;
  do
  {
    sCreateData.dwCookie[1] = (DWORD)NktInterlockedIncrement(&nHookInfoCounter);
  }
  while (sCreateData.dwCookie[1] == 0);
  //create interface using DeviareCOM's internal helper
  CUSTOMHANDLERS_TRY
    lpPtr = &sCreateData;
    hRes = fnDllGetClassObject(sInternalIid[1], sInternalIid[1], &lpPtr);
    if (SUCCEEDED(hRes))
      cHookInfo.Attach((IDispatch*)lpPtr);
  CUSTOMHANDLERS_CATCH
  if (SUCCEEDED(hRes))
  {
    *ppDisp = cHookInfo.Detach();
    *lpdwHookInfoCookie = sCreateData.dwCookie[1];
  }
  return hRes;
}

HRESULT CHookCustomHandlerMgr::InvalidateHookInfo(__in DWORD dwHookInfoCookie)
{
  HRESULT hRes;

  if (fnDllGetClassObject == NULL || fnInternal1 == NULL)
    return E_FAIL;
  CUSTOMHANDLERS_TRY
    //invalidate all the interfaces
    hRes = S_OK;
    fnInternal1(1, 1, (SIZE_T)dwHookInfoCookie);
  CUSTOMHANDLERS_CATCH
  return S_OK;
}

HRESULT CHookCustomHandlerMgr::ProcessFunctionCall(__in CHookEntry *lpHookEntry, __in IDispatch *lpHookInfo,
                                      __in CNktDvHookEngineCallback *lpHookEngCallback,
                                      __in CPluginInstanceList *lpPluginInstancesList,
                                      __deref_out CNktDvParamsEnumerator **lplpCustomParamsEnum,
                                      __inout LPVOID _lpCallInfo, __inout CNktDvParamsEnumerator *lpParamsEnum,
                                      __inout CNktDvParam *lpResult, __inout LPBYTE lpIntercallCustomData)
{
  CNktDvHookEngine::CALLINFO *lpCallInfo = (CNktDvHookEngine::CALLINFO*)_lpCallInfo;
  TNktComPtr<IDispatch> cCallInfo;
  DV_AGENTCOM_HOOKCALLINFOPLUGIN_CREATE_DATA sCreateData;
  union {
    LPVOID lpPtr;
    LPBYTE lpPtrByte;
  };
  TNktLnkLst<CHookCustomHandlerMgr::CPluginInstance>::Iterator it;
  CHookCustomHandlerMgr::CPluginInstance *lpPluginInstance;
  DWORD dwIdx;
  HRESULT hRes, hResCoInit;
  BOOL bLdrLocked;
  LONG volatile nFilterSpyMgrEvent;

  NKT_ASSERT(lpPluginInstancesList != NULL);
  if (lpPluginInstancesList->IsEmpty() != FALSE)
    return S_OK;
  if (fnDllGetClassObject == NULL || fnInternal1 == NULL)
    return E_FAIL;
  NKT_ASSERT(lplpCustomParamsEnum != NULL);
  NKT_ASSERT(lpCallInfo != NULL);
  NKT_ASSERT(lpParamsEnum != NULL);
  NKT_ASSERT(lpResult != NULL);
  NKT_ASSERT(lpIntercallCustomData != NULL);
  *lplpCustomParamsEnum = NKT_MEMMGR_NEW CNktDvParamsEnumerator;
  if ((*lplpCustomParamsEnum) == NULL)
    return E_OUTOFMEMORY;
  sCreateData.lpCustomParamsEnum = *lplpCustomParamsEnum;
  sCreateData.lpParamsEnum = lpParamsEnum;
  sCreateData.lpResult = lpResult;
  sCreateData.lpProc = cProc;
  sCreateData.lpIntercallCustomData = lpIntercallCustomData;
  sCreateData.sCallInfo.dwHookId;
  sCreateData.sCallInfo.bIsPreCall = (lpCallInfo->nPhase == CNktDvHookEngine::phPreCall) ? TRUE : FALSE;
  sCreateData.sCallInfo.dwCookie[0] = 2;
  sCreateData.sCallInfo.dwCookie[1] = (DWORD)(lpCallInfo->nCookie);
  sCreateData.sCallInfo.lpCallState = lpCallInfo->lpCallState;
  sCreateData.sCallInfo.dwThreadId = lpCallInfo->dwThreadId;
  sCreateData.sCallInfo.nCurrentTimeMs = lpCallInfo->nCurrentTimeMs;
  sCreateData.sCallInfo.nElapsedTimeMs = lpCallInfo->nElapsedTimeMs;
  sCreateData.sCallInfo.nChildsElapsedTimeMs = lpCallInfo->nChildsElapsedTimeMs;
  sCreateData.sCallInfo.nKernelTimeMs = lpCallInfo->nKernelTimeMs;
  sCreateData.sCallInfo.nUserTimeMs = lpCallInfo->nUserTimeMs;
  sCreateData.sCallInfo.nCpuClockCycles = lpCallInfo->nCpuClockCycles;
  sCreateData.sCallInfo.dwChainDepth = lpCallInfo->dwChainDepth;
  sCreateData.lpHookInfo = lpHookInfo;
  NktInterlockedExchange(&nFilterSpyMgrEvent, 0);
  sCreateData.lpnFilterSpyMgrEvent = &nFilterSpyMgrEvent;
  //create interface using DeviareCOM's internal helper
  //check if CoInitialize(Ex) was called in this thread
#if defined _M_IX86
  lpPtrByte = (LPBYTE)__readfsdword(0x18);      //get TEB
  lpPtrByte = *((LPBYTE*)(lpPtrByte+0x0F80));   //get ReservedForOle pointer
  if (lpPtrByte != NULL)
  {
    if (*((DWORD*)(lpPtrByte+0x18)) > 0)        //get oleinit count
      lpPtrByte = NULL;
  }
#elif defined _M_X64
  lpPtrByte = (LPBYTE)__readgsqword(0x30);      //get TEB
  lpPtrByte = *((LPBYTE*)(lpPtrByte+0x1758));   //get ReservedForOle pointer
  if (lpPtrByte != NULL)
  {
    if (*((DWORD*)(lpPtrByte+0x28)) > 0)        //get oleinit count
      lpPtrByte = NULL;
  }
#else
  #error Unsupported platform
#endif
  hResCoInit = E_FAIL;
  //hResCoInit = (lpPtrByte == NULL) ? (::CoInitializeEx(NULL, COINIT_MULTITHREADED)) : E_UNEXPECTED;
  //----
  CUSTOMHANDLERS_TRY
    lpPtr = &sCreateData;
    hRes = fnDllGetClassObject(sInternalIid[0], sInternalIid[0], &lpPtr);
  CUSTOMHANDLERS_CATCH
  if (SUCCEEDED(hRes))
  {
    //only check if "this" thread has the loader lock
    CUSTOMHANDLERS_TRY
      bLdrLocked = nktDvNtIsLoaderLoaderLockHeld(FALSE);
      cCallInfo.Attach((IDispatch*)lpPtr);
    CUSTOMHANDLERS_CATCH
  }
  if (SUCCEEDED(hRes))
  {
    //call plugins
    for (lpPluginInstance=it.Begin(*lpPluginInstancesList), dwIdx=1;
         lpPluginInstance!=NULL && SUCCEEDED(hRes); lpPluginInstance=it.Next(), dwIdx++)
    {
      if ((lpPluginInstance->nFlags & NKT_DV_TMSG_ADDCUSTOMHOOK_FLAG_DontCallIfLoaderLocked) == 0 ||
          bLdrLocked == FALSE)
      {
        hRes = lpPluginInstance->cPlugin->Call_OnFunctionCall(lpPluginInstance, lpHookInfo, dwIdx,
                                                              cCallInfo);
        if (FAILED(hRes))
        {
          NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookCustomHandlerMgr[Call_OnFunctionCall]: "
                            "hRes=%08X (%S) [%S @ %IXh ]", ::GetTickCount(), hRes,
                            lpPluginInstance->cPlugin->szDllNameW,
                            (lpHookEntry->lpDbFunc != NULL &&
                             lpHookEntry->lpDbFunc->GetName()[0] != 0) ?
                                              (lpHookEntry->lpDbFunc->GetName()) : L"???",
                            (SIZE_T)(lpHookEntry->sHookLibInfo.lpProcToHook)));
        }
      }
    }
  }
  else
  {
    NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookCustomHandlerMgr[ProcessFunctionCall]: "
                      "hRes=%08X [%S @ %IXh ]", ::GetTickCount(), hRes,
                      (lpHookEntry->lpDbFunc != NULL && lpHookEntry->lpDbFunc->GetName()[0] != 0) ?
                                            (lpHookEntry->lpDbFunc->GetName()) : L"???",
                      (SIZE_T)(lpHookEntry->sHookLibInfo.lpProcToHook)));
  }
  /*
  CUSTOMHANDLERS_TRY
    //invalidate all the interfaces
    fnInternal1(1, 2, (DWORD)(lpCallInfo->nCookie));
  CUSTOMHANDLERS_CATCH
  */
  if (hRes == S_OK && nFilterSpyMgrEvent != 0)
    hRes = S_FALSE;
  if (SUCCEEDED(hResCoInit))
    ::CoUninitialize();
  return hRes;
}

HRESULT CHookCustomHandlerMgr::ActivateDeviareComContext(__out ULONG_PTR *lpnCookie)
{
  return sDevCom.cActCtx.Activate(lpnCookie);
}

VOID CHookCustomHandlerMgr::DeactivateDeviareComContext(__in ULONG_PTR nCookie)
{
  sDevCom.cActCtx.Deactivate(nCookie);
  return;
}

HRESULT CHookCustomHandlerMgr::InitializeDeviareCOM()
{
  CNktStringW cStrTempW;
  HRESULT hRes;
  LPWSTR sW;
  SIZE_T k;

  if (hDeviareComDll != NULL)
    return S_OK;
  if (sDevCom.nInitialized == 0)
  {
    //NOTE: DeviareCOM(64).X.manifest will be embedded as resource "2" in post build
    hRes = sDevCom.cActCtx.Initialize(hAgentDllInstance, 2);
    if (FAILED(hRes))
      return hRes;
    NktInterlockedExchange(&(sDevCom.nInitialized), 1);
  }
  //----
  hRes = CNktDvTools::GetExecutableFileName(cStrTempW, ::GetCurrentProcess(), hAgentDllInstance);
  if (FAILED(hRes))
    return hRes;
  sW = (LPWSTR)cStrTempW;
  k = cStrTempW.GetLength();
  while (k > 0 && sW[k-1] != L'/' && sW[k-1] != L'\\')
    k--;
  sW[k] = 0;
  cStrTempW.Refresh();
#if defined _M_IX86
  if (cStrTempW.Concat(L"DeviareCOM.dll") == FALSE)
    return E_OUTOFMEMORY;
#elif defined _M_X64
  if (cStrTempW.Concat(L"DeviareCOM64.dll") == FALSE)
    return E_OUTOFMEMORY;
#endif
  hDeviareComDll = ::LoadLibraryW((LPWSTR)cStrTempW);
  if (hDeviareComDll == NULL)
    return NKT_DVERR_NotFound;
  fnDllGetClassObject = (lpfnDllGetClassObject)::GetProcAddress(hDeviareComDll, "DllGetClassObject");
  fnInternal1 = (lpfnInternal1)::GetProcAddress(hDeviareComDll, (LPCSTR)7);
  if (fnDllGetClassObject == NULL || fnInternal1 == NULL)
    return NKT_DVERR_NotFound;
  return S_OK;
}

HRESULT CHookCustomHandlerMgr::InitializeDotNetMgr()
{
  TNktComPtr<ICLRMetaHost> cMetaHost;
  TNktComPtr<ICLRRuntimeInfo> cRuntimeInfo;
  TNktComPtr<IUnknown> cUnk;
  lpfnCLRCreateInstance fnCLRCreateInstance;
  lpfnCorBindToRuntimeEx fnCorBindToRuntimeEx;
  BOOL bLoadable;
  HRESULT hRes;

  if (hMsCoreeDll != NULL)
    return S_OK;
  //load .net core library
  hMsCoreeDll = ::LoadLibraryW(L"mscoree.dll");
  if (hMsCoreeDll == NULL)
    return NKT_DVERR_NotFound;
  hRes = InstallMiniHooks();
  if (SUCCEEDED(hRes))
  {
    fnCLRCreateInstance = (lpfnCLRCreateInstance)::GetProcAddress(hMsCoreeDll, "CLRCreateInstance");
    if (fnCLRCreateInstance != NULL)
    {
      hRes = fnCLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (void**)&cMetaHost);
      if (SUCCEEDED(hRes))
        hRes = cMetaHost->GetRuntime(L"v4.0.30319", IID_ICLRRuntimeInfo, (void**)&cRuntimeInfo);
      /*
      if (SUCCEEDED(hRes))
      {
        hRes = cRuntimeInfo->SetDefaultStartupFlags(STARTUP_LOADER_OPTIMIZATION_SINGLE_DOMAIN|STARTUP_CONCURRENT_GC,
                                                    L"");
      }
      */
      if (SUCCEEDED(hRes))
        hRes = cRuntimeInfo->IsLoadable(&bLoadable);
      if (SUCCEEDED(hRes) && bLoadable == FALSE)
        hRes = E_FAIL;
      //if (SUCCEEDED(hRes))
      //  hRes = cRuntimeInfo->BindAsLegacyV2Runtime();
      if (SUCCEEDED(hRes))
        hRes = cRuntimeInfo->GetInterface(CLSID_CorRuntimeHost, IID_ICorRuntimeHost, (void**)&cCorRuntimeHost);
      if (FAILED(hRes))
      {
        cMetaHost.Release();
        cRuntimeInfo.Release();
        goto tryold;
      }
    }
    else
    {
tryold:
      fnCorBindToRuntimeEx = (lpfnCorBindToRuntimeEx)::GetProcAddress(hMsCoreeDll, "CorBindToRuntimeEx");
      hRes = (fnCorBindToRuntimeEx != NULL) ? S_OK : NKT_DVERR_NotFound;
      if (SUCCEEDED(hRes))
      {
        //bind to the runtime
        hRes = fnCorBindToRuntimeEx(NULL, L"wks", STARTUP_LOADER_OPTIMIZATION_SINGLE_DOMAIN|
                                    STARTUP_CONCURRENT_GC, CLSID_CorRuntimeHost, IID_ICorRuntimeHost,
                                    (LPVOID*)&cCorRuntimeHost);
      }
    }
  }
  if (SUCCEEDED(hRes))
    hRes = cCorRuntimeHost->Start();
  if (SUCCEEDED(hRes))
    hRes = cCorRuntimeHost->CreateDomain(L"NktDeviareAgentDomain", NULL, &cUnk);
    //hRes = cCorRuntimeHost->GetDefaultDomain(&cUnk);
  if (SUCCEEDED(hRes))
    hRes = cUnk->QueryInterface<mscorlib::_AppDomain>(&cAppDomain);
  return hRes;
}

HRESULT CHookCustomHandlerMgr::IsDotNetModule(__in HINSTANCE hDll)
{
  LPBYTE lpBaseAddr, lpNtHdr;
#if defined _M_IX86
  IMAGE_NT_HEADERS32 sNtHdr;
#elif defined _M_X64
  IMAGE_NT_HEADERS64 sNtHdr;
#else
  #error Unsupported platform
#endif
  DWORD dwTemp32;
  WORD wTemp16;

  lpBaseAddr = (LPBYTE)hDll;
  if (lpBaseAddr == NULL)
    return E_FAIL;
  //check dos signature
  nktMemCopy(&wTemp16, lpBaseAddr + FIELD_OFFSET(IMAGE_DOS_HEADER, e_magic), sizeof(WORD));
  if (wTemp16 != IMAGE_DOS_SIGNATURE)
    return E_FAIL;
  //get header offset
  nktMemCopy(&dwTemp32, lpBaseAddr + FIELD_OFFSET(IMAGE_DOS_HEADER, e_lfanew), sizeof(DWORD));
  lpNtHdr = lpBaseAddr + (SIZE_T)dwTemp32;
  //copy nt headers
  nktMemCopy(&sNtHdr, lpNtHdr, sizeof(sNtHdr));
  if (sNtHdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].VirtualAddress != NULL &&
      sNtHdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].Size > 0)
    return S_OK;
  return S_FALSE;
}

//-----------------------------------------------------------

CHookCustomHandlerMgr::CPluginMethod::CPluginMethod() : CNktMemMgrObj()
{
  lpProc = NULL;
  return;
}

CHookCustomHandlerMgr::CPluginMethod::~CPluginMethod()
{
  if (bAppIsExiting == FALSE)
  {
    CleanUp();
  }
  return;
}

VOID CHookCustomHandlerMgr::CPluginMethod::CleanUp()
{
#ifdef USE_MARSHALLING
  if (cDotNetStream != NULL)
  {
    LARGE_INTEGER li;
    HRESULT hRes;

    li.QuadPart = 0;
    cDotNetStream->Seek(li, STREAM_SEEK_SET, NULL);
    hRes = ::CoReleaseMarshalData(cDotNetStream);
    cDotNetStream.Release();
  }
#else //USE_MARSHALLING
  cMethod.Release();
#endif //USE_MARSHALLING
  return;
}

//-----------------------------------------------------------

CHookCustomHandlerMgr::CPluginVariantArray::CPluginVariantArray(__in int nParamsCount) : CNktMemMgrObj()
{
  NKT_ASSERT(nParamsCount > 0);
  lpSA = ::SafeArrayCreateVector(VT_VARIANT, 0, (ULONG)nParamsCount);
  bLocked = FALSE;
  return;
}

CHookCustomHandlerMgr::CPluginVariantArray::~CPluginVariantArray()
{
  if (bAppIsExiting == FALSE)
  {
    Unlock();
    if (lpSA != NULL)
      ::SafeArrayDestroy(lpSA);
  }
  return;
}

VARIANT* CHookCustomHandlerMgr::CPluginVariantArray::Lock()
{
  VARIANT *lpVar;

  if (lpSA == NULL ||
      bLocked != FALSE ||
      FAILED(::SafeArrayAccessData(lpSA, (void**)&lpVar)))
    return NULL;
  bLocked = TRUE;
  return lpVar;
}

VOID CHookCustomHandlerMgr::CPluginVariantArray::Unlock()
{
  if (bLocked != FALSE)
  {
    ::SafeArrayUnaccessData(lpSA);
    bLocked = FALSE;
  }
  return;
}

SAFEARRAY* CHookCustomHandlerMgr::CPluginVariantArray::Get()
{
  return lpSA;
}

//-----------------------------------------------------------

CHookCustomHandlerMgr::CPlugin::CPlugin(__in CHookCustomHandlerMgr *lpCustomHandlersMgr) :
                                              CNktDvObject(), TNktLnkLstNode<CPlugin>()
{
  NKT_ASSERT(lpCustomHandlersMgr != NULL);
  cCustomHandlersMgr = lpCustomHandlersMgr;
  szDllNameW = NULL;
  hDll = NULL;
  NktInterlockedExchange(&nNextSubIndex, 0);
  NktInterlockedExchange(&nUsageCount, 0);
  //----
  sDotNet.bIsManaged = FALSE;
  return;
}

CHookCustomHandlerMgr::CPlugin::~CPlugin()
{
  ULONG_PTR nActCtxCookie;
  int i;

  if (bAppIsExiting == FALSE)
  {
    cCustomHandlersMgr->ActivateDeviareComContext(&nActCtxCookie);
    Call_OnUnload();
    if (nActCtxCookie != 0)
      cCustomHandlersMgr->DeactivateDeviareComContext(nActCtxCookie);
    for (i=0; i<NKT_DV_ARRAYLEN(aPluginMethods); i++)
      aPluginMethods[i].CleanUp();
#ifdef USE_MARSHALLING
    if (sDotNet.cObjectStream != NULL)
    {
      LARGE_INTEGER li;

      li.QuadPart = 0;
      sDotNet.cObjectStream->Seek(li, STREAM_SEEK_SET, NULL);
      ::CoReleaseMarshalData(sDotNet.cObjectStream);
      sDotNet.cObjectStream.Release();
    }
#endif //USE_MARSHALLING
    sDotNet.cType.Release();
    //----
    if (hDll != NULL && sDotNet.bIsManaged == FALSE)
      ::FreeLibrary(hDll);
    if (szDllNameW != NULL)
      nktMemFree(szDllNameW);
  }
  return;
}

HRESULT CHookCustomHandlerMgr::CPlugin::Call_OnLoad()
{
  HRESULT hRes;

  if (sDotNet.cType != NULL)
  {
    CUSTOMHANDLERS_TRY
      hRes = InvokeDotNetMethod(METHODINDEX_OnLoad, NULL, NULL);
    CUSTOMHANDLERS_CATCH
  }
  else
  {
    CUSTOMHANDLERS_TRY
      hRes = ((lpfnOnLoad)(aPluginMethods[METHODINDEX_OnLoad].lpProc))();
    CUSTOMHANDLERS_CATCH
  }
  return hRes;
}

VOID CHookCustomHandlerMgr::CPlugin::Call_OnUnload()
{
  HRESULT hRes;

  if (sDotNet.cType != NULL)
  {
    CUSTOMHANDLERS_TRY
      hRes = InvokeDotNetMethod(METHODINDEX_OnUnload, NULL, NULL);
    CUSTOMHANDLERS_CATCH
  }
  else if (aPluginMethods[METHODINDEX_OnUnload].lpProc != NULL)
  {
    CUSTOMHANDLERS_TRY
      ((lpfnOnUnload)(aPluginMethods[METHODINDEX_OnUnload].lpProc))();
      hRes = S_OK;
    CUSTOMHANDLERS_CATCH
  }
  return;
}

HRESULT CHookCustomHandlerMgr::CPlugin::Call_OnHookAdded(__in CPluginInstance *lpPluginInst,
                                             __in IDispatch *lpHookInfo, __in DWORD dwChainIndex,
                                             __in LPCWSTR szParametersW)
{
  HRESULT hRes;

  if (szParametersW == NULL)
    szParametersW = L"";
  if (sDotNet.cType != NULL)
  {
    CHookCustomHandlerMgr::CPluginVariantArray cParams(3);
    VARIANT *lpVar;

    //initialize parameters in normal order when invoked with Invoke_3
    lpVar = cParams.Lock();
    if (lpVar != NULL)
    {
      hRes = S_OK;
      lpVar[0].vt = VT_DISPATCH;
      lpVar[0].pdispVal = lpHookInfo;
      lpHookInfo->AddRef();
      lpVar[1].vt = VT_I4;
      lpVar[1].lVal = (LONG)dwChainIndex;
      lpVar[2].vt = VT_BSTR;
      lpVar[2].bstrVal = ::SysAllocString(szParametersW);
      if (lpVar[2].bstrVal == NULL)
      {
        lpVar[2].vt = VT_EMPTY;
        hRes = E_OUTOFMEMORY;
      }
      cParams.Unlock();
    }
    else
    {
      hRes = E_OUTOFMEMORY;
    }
    //invoke
    if (SUCCEEDED(hRes))
    {
      if ((lpPluginInst->nFlags & NKT_DV_TMSG_ADDCUSTOMHOOK_FLAG_DisableExceptionChecks) == 0)
      {
        CUSTOMHANDLERS_TRY
          hRes = InvokeDotNetMethod(METHODINDEX_OnHookAdded, cParams.Get(), NULL);
        CUSTOMHANDLERS_CATCH
      }
      else
      {
        hRes = InvokeDotNetMethod(METHODINDEX_OnHookAdded, cParams.Get(), NULL);
      }
    }
  }
  else
  {
    if ((lpPluginInst->nFlags & NKT_DV_TMSG_ADDCUSTOMHOOK_FLAG_DisableExceptionChecks) == 0)
    {
      CUSTOMHANDLERS_TRY
        hRes = ((lpfnOnHookAdded)(aPluginMethods[METHODINDEX_OnHookAdded].lpProc))(lpHookInfo,
                                         dwChainIndex, szParametersW);
      CUSTOMHANDLERS_CATCH
    }
    else
    {
      hRes = ((lpfnOnHookAdded)(aPluginMethods[METHODINDEX_OnHookAdded].lpProc))(lpHookInfo,
                                         dwChainIndex, szParametersW);
    }
  }
  return hRes;
}

HRESULT CHookCustomHandlerMgr::CPlugin::Call_GetFunctionCallbackName(__in CPluginInstance *lpPluginInst,
                                             __in IDispatch *lpHookInfo, __in DWORD dwChainIndex,
                                             __out BSTR *lpbstrFunctionName)
{
  CNktComVariant cVarResult;
  HRESULT hRes;

  NKT_ASSERT(lpbstrFunctionName != NULL);
  *lpbstrFunctionName = NULL;
  hRes = S_OK;
  if (sDotNet.cType != NULL)
  {
#ifdef USE_MARSHALLING
    if (aPluginMethods[METHODINDEX_GetFunctionCallbackName].cDotNetStream != NULL)
#else //USE_MARSHALLING
    if (aPluginMethods[METHODINDEX_GetFunctionCallbackName].cMethod != NULL)
#endif //USE_MARSHALLING
    {
      CHookCustomHandlerMgr::CPluginVariantArray cParams(2);
      VARIANT *lpVar;

      //initialize parameters in normal order when invoked with Invoke_3
      lpVar = cParams.Lock();
      if (lpVar != NULL)
      {
        hRes = S_OK;
        lpVar[0].vt = VT_DISPATCH;
        lpVar[0].pdispVal = lpHookInfo;
        lpHookInfo->AddRef();
        lpVar[1].vt = VT_I4;
        lpVar[1].lVal = (LONG)dwChainIndex;
        cParams.Unlock();
      }
      else
      {
        hRes = E_OUTOFMEMORY;
      }
      //invoke
      if (SUCCEEDED(hRes))
      {
        if ((lpPluginInst->nFlags & NKT_DV_TMSG_ADDCUSTOMHOOK_FLAG_DisableExceptionChecks) == 0)
        {
          CUSTOMHANDLERS_TRY
            hRes = InvokeDotNetMethod(METHODINDEX_GetFunctionCallbackName, cParams.Get(), &cVarResult);
          CUSTOMHANDLERS_CATCH
        }
        else
        {
          hRes = InvokeDotNetMethod(METHODINDEX_GetFunctionCallbackName, cParams.Get(), &cVarResult);
        }
      }
      //process result
      if (SUCCEEDED(hRes))
      {
        lpVar = &cVarResult;
        while (lpVar != NULL && lpVar->vt == (VT_BYREF|VT_VARIANT))
          lpVar = lpVar->pvarVal;
        if (lpVar != NULL && lpVar->vt == VT_BSTR && lpVar->bstrVal != NULL)
        {
          //detach BSTR
          *lpbstrFunctionName = lpVar->bstrVal;
          lpVar->vt = VT_EMPTY;
        }
      }
    }
  }
  else if (aPluginMethods[METHODINDEX_GetFunctionCallbackName].lpProc != NULL)
  {
    if ((lpPluginInst->nFlags & NKT_DV_TMSG_ADDCUSTOMHOOK_FLAG_DisableExceptionChecks) == 0)
    {
      CUSTOMHANDLERS_TRY
        hRes = ((lpfnGetFunctionCallbackName)(aPluginMethods[METHODINDEX_GetFunctionCallbackName].
                                              lpProc))(lpHookInfo, dwChainIndex, lpbstrFunctionName);
      CUSTOMHANDLERS_CATCH
    }
    else
    {
      hRes = ((lpfnGetFunctionCallbackName)(aPluginMethods[METHODINDEX_GetFunctionCallbackName].
                                            lpProc))(lpHookInfo, dwChainIndex, lpbstrFunctionName);
    }
  }
  return hRes;
}

VOID CHookCustomHandlerMgr::CPlugin::Call_OnHookRemoved(__in CPluginInstance *lpPluginInst,
                                          __in IDispatch *lpHookInfo, __in DWORD dwChainIndex)
{
  HRESULT hRes;

  if (sDotNet.cType != NULL)
  {
    CHookCustomHandlerMgr::CPluginVariantArray cParams(2);
    VARIANT *lpVar;

    //initialize parameters in normal order when invoked with Invoke_3
    lpVar = cParams.Lock();
    if (lpVar != NULL)
    {
      hRes = S_OK;
      lpVar[0].vt = VT_DISPATCH;
      lpVar[0].pdispVal = lpHookInfo;
      lpHookInfo->AddRef();
      lpVar[1].vt = VT_I4;
      lpVar[1].lVal = (LONG)dwChainIndex;
    }
    else
    {
      hRes = E_OUTOFMEMORY;
    }
    //invoke
    if (SUCCEEDED(hRes))
    {
      if ((lpPluginInst->nFlags & NKT_DV_TMSG_ADDCUSTOMHOOK_FLAG_DisableExceptionChecks) == 0)
      {
        CUSTOMHANDLERS_TRY
          hRes = InvokeDotNetMethod(METHODINDEX_OnHookRemoved, cParams.Get(), NULL);
        CUSTOMHANDLERS_CATCH
      }
      else
      {
        hRes = InvokeDotNetMethod(METHODINDEX_OnHookRemoved, cParams.Get(), NULL);
      }
    }
  }
  else
  {
    if ((lpPluginInst->nFlags & NKT_DV_TMSG_ADDCUSTOMHOOK_FLAG_DisableExceptionChecks) == 0)
    {
      CUSTOMHANDLERS_TRY
        ((lpfnOnHookRemoved)(aPluginMethods[METHODINDEX_OnHookRemoved].lpProc))(lpHookInfo, dwChainIndex);
      CUSTOMHANDLERS_CATCH
    }
    else
    {
      ((lpfnOnHookRemoved)(aPluginMethods[METHODINDEX_OnHookRemoved].lpProc))(lpHookInfo, dwChainIndex);
    }
  }
  return;
}

HRESULT CHookCustomHandlerMgr::CPlugin::Call_OnFunctionCall(__in CPluginInstance *lpPluginInst,
                                            __in IDispatch *lpHookInfo, __in DWORD dwChainIndex,
                                            __in IDispatch *lpHookCallInfoPlugin)
{
  HRESULT hRes;

  if (sDotNet.cType != NULL)
  {
    CHookCustomHandlerMgr::CPluginVariantArray cParams(3);
    VARIANT *lpVar;

    //initialize parameters in normal order when invoked with Invoke_3
    lpVar = cParams.Lock();
    if (lpVar != NULL)
    {
      hRes = S_OK;
      lpVar[0].vt = VT_DISPATCH;
      lpVar[0].pdispVal = lpHookInfo;
      lpHookInfo->AddRef();
      lpVar[1].vt = VT_I4;
      lpVar[1].lVal = (LONG)dwChainIndex;
      lpVar[2].vt = VT_DISPATCH;
      lpVar[2].pdispVal = lpHookCallInfoPlugin;
      lpHookCallInfoPlugin->AddRef();
    }
    else
    {
      hRes = E_OUTOFMEMORY;
    }
    //invoke
    if (SUCCEEDED(hRes))
    {
      if ((lpPluginInst->nFlags & NKT_DV_TMSG_ADDCUSTOMHOOK_FLAG_DisableExceptionChecks) == 0)
      {
        CUSTOMHANDLERS_TRY
#ifdef USE_MARSHALLING
          if (lpPluginInst->cOnFunctionCallReRoute.cDotNetStream != NULL)
            hRes = InvokeDotNetMethod(lpPluginInst->cOnFunctionCallReRoute.cDotNetStream, cParams.Get(), NULL);
          else
            hRes = InvokeDotNetMethod(METHODINDEX_OnFunctionCall, cParams.Get(), NULL);
#else //USE_MARSHALLING
          if (lpPluginInst->cOnFunctionCallReRoute.cMethod != NULL)
            hRes = InvokeDotNetMethod(lpPluginInst->cOnFunctionCallReRoute.cMethod, cParams.Get(), NULL);
          else
            hRes = InvokeDotNetMethod(METHODINDEX_OnFunctionCall, cParams.Get(), NULL);
#endif //USE_MARSHALLING
        CUSTOMHANDLERS_CATCH
      }
      else
      {
#ifdef USE_MARSHALLING
        if (lpPluginInst->cOnFunctionCallReRoute.cDotNetStream != NULL)
          hRes = InvokeDotNetMethod(lpPluginInst->cOnFunctionCallReRoute.cDotNetStream, cParams.Get(), NULL);
        else
          hRes = InvokeDotNetMethod(METHODINDEX_OnFunctionCall, cParams.Get(), NULL);
#else //USE_MARSHALLING
        if (lpPluginInst->cOnFunctionCallReRoute.cMethod != NULL)
          hRes = InvokeDotNetMethod(lpPluginInst->cOnFunctionCallReRoute.cMethod, cParams.Get(), NULL);
        else
          hRes = InvokeDotNetMethod(METHODINDEX_OnFunctionCall, cParams.Get(), NULL);
#endif //USE_MARSHALLING
      }
    }
  }
  else
  {
    if ((lpPluginInst->nFlags & NKT_DV_TMSG_ADDCUSTOMHOOK_FLAG_DisableExceptionChecks) == 0)
    {
      CUSTOMHANDLERS_TRY
        if (lpPluginInst->cOnFunctionCallReRoute.lpProc != NULL)
        {
          hRes = ((lpfnOnFunctionCall)(lpPluginInst->cOnFunctionCallReRoute.lpProc))(lpHookInfo,
                                           dwChainIndex, lpHookCallInfoPlugin);
        }
        else
        {
          hRes = ((lpfnOnFunctionCall)(aPluginMethods[METHODINDEX_OnFunctionCall].lpProc))(lpHookInfo,
                                           dwChainIndex, lpHookCallInfoPlugin);
        }
      CUSTOMHANDLERS_CATCH
    }
    else
    {
      if (lpPluginInst->cOnFunctionCallReRoute.lpProc != NULL)
      {
        hRes = ((lpfnOnFunctionCall)(lpPluginInst->cOnFunctionCallReRoute.lpProc))(lpHookInfo,
                                         dwChainIndex, lpHookCallInfoPlugin);
      }
      else
      {
        hRes = ((lpfnOnFunctionCall)(aPluginMethods[METHODINDEX_OnFunctionCall].lpProc))(lpHookInfo,
                                         dwChainIndex, lpHookCallInfoPlugin);
      }
    }
  }
  if (hRes == DISP_E_EXCEPTION)
    hRes = S_OK;
  return hRes;
}

HRESULT CHookCustomHandlerMgr::CPlugin::InvokeDotNetMethod(__in int nMethodIndex, __inout SAFEARRAY *lpParameters,
                                                           __out_opt VARIANT *lpVarResult)
{
#ifdef USE_MARSHALLING
  return InvokeDotNetMethod(aPluginMethods[nMethodIndex].cDotNetStream, lpParameters, lpVarResult);
#else //USE_MARSHALLING
  return InvokeDotNetMethod(aPluginMethods[nMethodIndex].cMethod, lpParameters, lpVarResult);
#endif //USE_MARSHALLING
}

#ifdef USE_MARSHALLING

HRESULT CHookCustomHandlerMgr::CPlugin::InvokeDotNetMethod(__in IStream *lpMethodStream,
                                                           __inout SAFEARRAY *lpParameters,
                                                           __out_opt VARIANT *lpVarResult)
{
  TNktComPtr<IStream> cObjectStream, cMthdStream;
  TNktComPtr<mscorlib::_MethodInfo> cMethod;
  TNktComPtr<mscorlib::_Object> cObject;
  CNktComVariant cVarResult;
  VARIANT vtObj, *lpVar;
  LARGE_INTEGER li;
  HRESULT hRes;

  NKT_ASSERT(lpMethodStream != NULL);
  //unmarshal interfaces
  {
    CNktAutoFastMutex cLock(&(sDotNet.cMarshalMtx));

    hRes = lpMethodStream->Clone(&cMthdStream);
    if (SUCCEEDED(hRes))
      hRes = sDotNet.cObjectStream->Clone(&cObjectStream);
  }
  if (SUCCEEDED(hRes))
  {
    li.QuadPart = 0;
    cObjectStream->Seek(li, STREAM_SEEK_SET, NULL);
    li.QuadPart = 0;
    cMthdStream->Seek(li, STREAM_SEEK_SET, NULL);
    hRes = ::CoUnmarshalInterface(cMthdStream, IID_NULL, (LPVOID*)&cMethod);
  }
  if (SUCCEEDED(hRes))
    hRes = ::CoUnmarshalInterface(cObjectStream, IID_NULL, (LPVOID*)&cObject);
  //make the call
  if (SUCCEEDED(hRes))
  {
    vtObj.vt = VT_UNKNOWN;
    vtObj.punkVal = (IUnknown*)cObject;
    hRes = cMethod->Invoke_3(vtObj, lpParameters, (lpVarResult != NULL) ? lpVarResult : &cVarResult);
  }
  //process the result
  if (SUCCEEDED(hRes) && lpVarResult == NULL)
  {
    lpVar = &cVarResult;
    while (lpVar != NULL && lpVar->vt == (VT_BYREF|VT_VARIANT))
      lpVar = lpVar->pvarVal;
    if (lpVar != NULL)
    {
      switch (lpVar->vt)
      {
        case VT_I1:
          hRes = (HRESULT)(lpVar->cVal);
          break;
        case VT_I2:
          hRes = (HRESULT)(lpVar->iVal);
          break;
        case VT_I4:
          hRes = (HRESULT)(lpVar->lVal);
          break;
        case VT_I8:
          hRes = (HRESULT)(lpVar->llVal);
          break;
        case VT_INT:
          hRes = (HRESULT)(lpVar->intVal);
          break;
        case VT_UI1:
          hRes = (HRESULT)(ULONGLONG)(lpVar->bVal);
          break;
        case VT_UI2:
          hRes = (HRESULT)(ULONGLONG)(lpVar->uiVal);
          break;
        case VT_UI4:
          hRes = (HRESULT)(ULONGLONG)(lpVar->ulVal);
          break;
        case VT_UI8:
          hRes = (HRESULT)(lpVar->ullVal);
          break;
        case VT_UINT:
          hRes = (HRESULT)(ULONGLONG)(lpVar->uintVal);
          break;
        case VT_EMPTY:
        case VT_NULL:
          hRes = S_OK;
          break;
        default:
          hRes = E_INVALIDARG;
          break;
      }
    }
    else
    {
      hRes = E_POINTER;
    }
  }
  /*
  else if (hRes == DISP_E_EXCEPTION)
  {
    //exception error code
    // make sure sExcepInfo is filled in
    if (sExcepInfo.pfnDeferredFillIn != NULL)
      sExcepInfo.pfnDeferredFillIn(&sExcepInfo);

    NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) HookEngine[InvokeDotNetMethod]: Exception Code:%08X",
      ::GetTickCount(), (HRESULT)sExcepInfo.scode));
    if (sExcepInfo.bstrSource != NULL)
    {
      NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu)         Source:%S", ::GetTickCount(),
        sExcepInfo.bstrSource));
    }
    if (sExcepInfo.bstrSource != NULL)
    {
      NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu)    Description:%S", ::GetTickCount(),
        sExcepInfo.bstrDescription));
    }
    ::SysFreeString(sExcepInfo.bstrSource);
    ::SysFreeString(sExcepInfo.bstrDescription);
  }
  */
  return hRes;
}

HRESULT CHookCustomHandlerMgr::CPlugin::FindDotNetMethod(__in LPCWSTR szMethodNameW, __out IStream **lplpStream)
{
  TNktComPtr<mscorlib::_MethodInfo> cMethod;
  TNktComPtr<IStream> cIStream;
  BSTR bstr;
  HRESULT hRes;

  NKT_ASSERT(lplpStream != NULL);
  bstr = ::SysAllocString(szMethodNameW);
  if (bstr != NULL)
  {
    hRes = sDotNet.cType->GetMethod_2(bstr, (mscorlib::BindingFlags)(mscorlib::BindingFlags_InvokeMethod|
                              mscorlib::BindingFlags_Public|mscorlib::BindingFlags_Instance), &cMethod);
    //hRes = sDotNet.cType->GetMethod_6(bstr, &cMethod);
    ::SysFreeString(bstr);
    if (SUCCEEDED(hRes) && cMethod == NULL)
      hRes = NKT_DVERR_NotFound;
  }
  else
  {
    hRes = E_OUTOFMEMORY;
  }
  if (SUCCEEDED(hRes))
    hRes = ::CreateStreamOnHGlobal(NULL, TRUE, &cIStream);
  if (SUCCEEDED(hRes))
  {
    hRes = ::CoMarshalInterface(cIStream, __uuidof(mscorlib::_MethodInfo), cMethod, MSHCTX_INPROC,
                                NULL, MSHLFLAGS_TABLESTRONG);
  }
  if (SUCCEEDED(hRes))
  {
    *lplpStream = cIStream.Detach();
  }
  else
  {
    *lplpStream = NULL;
    NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookCustomHandlerMgr[Find .net method]: "
                       "hRes=%08X (%S @ %S)", ::GetTickCount(), hRes, szMethodNameW, szDllNameW));
  }
  return hRes;
}

#else //USE_MARSHALLING

HRESULT CHookCustomHandlerMgr::CPlugin::InvokeDotNetMethod(__in mscorlib::_MethodInfo *lpMethod,
                                                           __inout SAFEARRAY *lpParameters,
                                                           __out_opt VARIANT *lpVarResult)
{
  CNktComVariant cVarResult;
  VARIANT *lpVar;
  HRESULT hRes;

  NKT_ASSERT(lpMethod != NULL);
  //make the call
  hRes = lpMethod->Invoke_3(sDotNet.vtUnwrapped, lpParameters, (lpVarResult != NULL) ? lpVarResult : &cVarResult);
  //process the result
  if (SUCCEEDED(hRes) && lpVarResult == NULL)
  {
    lpVar = &cVarResult;
    while (lpVar != NULL && lpVar->vt == (VT_BYREF|VT_VARIANT))
      lpVar = lpVar->pvarVal;
    if (lpVar != NULL)
    {
      switch (lpVar->vt)
      {
        case VT_I1:
          hRes = (HRESULT)(lpVar->cVal);
          break;
        case VT_I2:
          hRes = (HRESULT)(lpVar->iVal);
          break;
        case VT_I4:
          hRes = (HRESULT)(lpVar->lVal);
          break;
        case VT_I8:
          hRes = (HRESULT)(lpVar->llVal);
          break;
        case VT_INT:
          hRes = (HRESULT)(lpVar->intVal);
          break;
        case VT_UI1:
          hRes = (HRESULT)(ULONGLONG)(lpVar->bVal);
          break;
        case VT_UI2:
          hRes = (HRESULT)(ULONGLONG)(lpVar->uiVal);
          break;
        case VT_UI4:
          hRes = (HRESULT)(ULONGLONG)(lpVar->ulVal);
          break;
        case VT_UI8:
          hRes = (HRESULT)(lpVar->ullVal);
          break;
        case VT_UINT:
          hRes = (HRESULT)(ULONGLONG)(lpVar->uintVal);
          break;
        case VT_EMPTY:
        case VT_NULL:
          hRes = S_OK;
          break;
        default:
          hRes = E_INVALIDARG;
          break;
      }
    }
    else
    {
      hRes = E_POINTER;
    }
  }
  return hRes;
}

HRESULT CHookCustomHandlerMgr::CPlugin::FindDotNetMethod(__in LPCWSTR szMethodNameW,
                                                         __out mscorlib::_MethodInfo **lplpMethodInfo)
{
  TNktComPtr<mscorlib::_MethodInfo> cMethod;
  BSTR bstr;
  HRESULT hRes;

  NKT_ASSERT(lplpMethodInfo != NULL);
  bstr = ::SysAllocString(szMethodNameW);
  if (bstr != NULL)
  {
    hRes = sDotNet.cType->GetMethod_2(bstr, (mscorlib::BindingFlags)(mscorlib::BindingFlags_InvokeMethod|
                              mscorlib::BindingFlags_Public|mscorlib::BindingFlags_Instance), &cMethod);
    //hRes = sDotNet.cType->GetMethod_6(bstr, &cMethod);
    ::SysFreeString(bstr);
    if (FAILED(hRes) || cMethod == NULL)
      hRes = NKT_DVERR_NotFound;
  }
  else
  {
    hRes = E_OUTOFMEMORY;
  }
  if (SUCCEEDED(hRes))
  {
    *lplpMethodInfo = cMethod.Detach();
  }
  else
  {
    *lplpMethodInfo = NULL;
    NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookCustomHandlerMgr[Find .net method]: "
                       "hRes=%08X (%S @ %S)", ::GetTickCount(), hRes, szMethodNameW, szDllNameW));
  }
  return hRes;
}

#endif //USE_MARSHALLING

static HRESULT DoLoadLibrary(__out HINSTANCE *lphDll, __inout CNktStringW &cStrDllNameW, __in LPCWSTR szDllNameW)
{
  HRESULT hRes;
  LPWSTR sW;
  SIZE_T k;

  NKT_ASSERT(lphDll != NULL);
  *lphDll = NULL;
  if (szDllNameW == NULL)
    return E_POINTER;
  if (szDllNameW[0] == 0)
    return E_INVALIDARG;
  if (szDllNameW[0] != L'\\' && szDllNameW[1] != L':')
  {
    //if path is relative, use agent module path as a base
    hRes = CNktDvTools::GetModulePath(cStrDllNameW, hAgentDllInstance);
    if (FAILED(hRes))
      return hRes;
    if (cStrDllNameW.Concat(szDllNameW) == FALSE)
      return E_OUTOFMEMORY;
  }
  else
  {
    if (cStrDllNameW.Copy(szDllNameW) == FALSE)
      return E_OUTOFMEMORY;
  }
  *lphDll = ::LoadLibraryW((LPWSTR)cStrDllNameW);
  if ((*lphDll) == NULL)
  {
    //if the dll was not found or cannot be loaded, try adding/removing the "64" prefix as needed
    hRes = NKT_HRESULT_FROM_LASTERROR();
    sW = (LPWSTR)cStrDllNameW;
    k = wcslen(sW);
    while (k > 0 && sW[k-1] != L'.' && sW[k-1] != L'/' && sW[k-1] != L'\\')
      k--;
    if (sW[k-1] != L'.')
    {
      //because an extension was not found in the filename, move the pointer to the end
      k = wcslen(sW);
    }
    else
    {
      k--; //move just before the dot
    }
    //"k" points to the character after the filename (shoul be a dot or the end of the string)
#if defined _M_IX86
    //because dealing with x86 code, remove the "64" string if found, else error
    if (k < 2 || sW[k-2] != L'6' || sW[k-1] != L'4')
      return hRes; //"64" string not found, return original error
    cStrDllNameW.Delete(k-2, 2);
#elif defined _M_X64
    //because dealing with x64 code, add the "64" string if not found, else error
    if (k >= 2 && sW[k-2] == L'6' && sW[k-1] == L'4')
      return hRes; //"64" string found, return original error
    if (cStrDllNameW.Insert(L"64", k) == FALSE)
      return E_OUTOFMEMORY;
#endif
    //try again
    *lphDll = ::LoadLibraryW((LPWSTR)cStrDllNameW);
    if ((*lphDll) == NULL)
      return NKT_HRESULT_FROM_LASTERROR();
  }
  //get the full path of the dll to compare with an already loaded one
  hRes = CNktDvTools::GetModuleFullName(cStrDllNameW, *lphDll);
  if (FAILED(hRes))
  {
    ::FreeLibrary(*lphDll);
    return hRes;
  }
  return S_OK;
}

static HRESULT __stdcall Internal_SendCustomMessage(__in LPVOID lpInternal, __out SIZE_T *lpnRetVal,
                                                    __in SIZE_T nMsgCode, __in SIZE_T nMsgParam, __in BOOL bSync)
{
  CNktDvHookEngineCallback *lpCallback = reinterpret_cast<CNktDvHookEngineCallback*>(lpInternal);

  NKT_ASSERT(lpCallback != NULL);
  return lpCallback->HEC_OnSendCustomMessage(lpnRetVal, nMsgCode, nMsgParam, bSync);
}

static HRESULT __stdcall Internal_ThreadIgnore(__in LPVOID lpInternal, __in DWORD dwTid, __in BOOL bIgnore)
{
  CNktDvHookEngineCallback *lpCallback = reinterpret_cast<CNktDvHookEngineCallback*>(lpInternal);

  NKT_ASSERT(lpCallback != NULL);
  return lpCallback->HEC_OnThreadIgnore(dwTid, bIgnore);
}

static HRESULT __stdcall Internal_InvalidateCache(__in LPVOID lpInternal, __in SIZE_T hDll)
{
  CNktDvHookEngineCallback *lpCallback = reinterpret_cast<CNktDvHookEngineCallback*>(lpInternal);

  NKT_ASSERT(lpCallback != NULL);
  CNktDvProcess::MarkModulesEnumeratorAsDirty(::GetCurrentProcessId(), (HINSTANCE)hDll);
  return lpCallback->HEC_OnInvalidateCache((HINSTANCE)hDll);
}
