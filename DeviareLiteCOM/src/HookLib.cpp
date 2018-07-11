/*
 * Copyright (C) 2010-2015 Nektra S.A., Buenos Aires, Argentina.
 * All rights reserved. Contact: http://www.nektra.com
 *
 *
 * This file is part of Deviare In-Proc
 *
 *
 * Commercial License Usage
 * ------------------------
 * Licensees holding valid commercial Deviare In-Proc licenses may use this
 * file in accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Nektra.  For licensing terms and
 * conditions see http://www.nektra.com/licensing/.  For further information
 * use the contact form at http://www.nektra.com/contact/.
 *
 *
 * GNU General Public License Usage
 * --------------------------------
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 *
 **/

#include "StdAfx.h"
#include "HookLib.h"
#include "HookInfo.h"
#include "HookProcessInfo.h"

#pragma warning(disable: 4302 4311)

//-----------------------------------------------------------

__inline HRESULT NKT_HRESULT_FROM_WIN32(DWORD dwOsErr)
{
  if (dwOsErr == ERROR_NOT_ENOUGH_MEMORY)
    dwOsErr = ERROR_OUTOFMEMORY;
  return HRESULT_FROM_WIN32(dwOsErr);
}

//-----------------------------------------------------------

static LPSTR Wide2Ansi(__in_nz_opt LPCWSTR szSrcW);

//-----------------------------------------------------------

STDMETHODIMP CNktHookLibImpl::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] = { &IID_INktHookLib, NULL };
  SIZE_T i;

  for (i=0; arr[i]!=NULL; i++)
  {
    if (InlineIsEqualGUID(*arr[i], riid))
      return S_OK;
  }
  return S_FALSE;
}

STDMETHODIMP CNktHookLibImpl::Hook(__in VARIANT itemsToHook, __in LONG flags)
{
  return RemoteHook(itemsToHook, ::GetCurrentProcessId(), flags);
}

STDMETHODIMP CNktHookLibImpl::RemoteHook(__in VARIANT itemsToHook, __in LONG pid, __in LONG flags)
{
  CHookInfo cHkInfo;
  HRESULT hRes;

  hRes = cHkInfo.Init(itemsToHook);
  if (SUCCEEDED(hRes))
  {
    flags ^= NKTHOOKLIB_DisallowReentrancy;
    hRes = NKT_HRESULT_FROM_WIN32(cHookLib.RemoteHook(cHkInfo.lpInfo, cHkInfo.nCount, (DWORD)pid, (DWORD)flags));
  }
  if (SUCCEEDED(hRes))
    cHkInfo.StoreInfo();
  return hRes;
}

STDMETHODIMP CNktHookLibImpl::Unhook(__in VARIANT itemsToUnhook)
{
  CHookInfo cHkInfo;
  HRESULT hRes;

  hRes = cHkInfo.Init(itemsToUnhook);
  if (SUCCEEDED(hRes))
    hRes = NKT_HRESULT_FROM_WIN32(cHookLib.Unhook(cHkInfo.lpInfo, cHkInfo.nCount));
  return hRes;
}

STDMETHODIMP CNktHookLibImpl::UnhookProcess(__in LONG pid)
{
  cHookLib.UnhookProcess((DWORD)pid);
  return S_OK;
}

STDMETHODIMP CNktHookLibImpl::UnhookAll()
{
  cHookLib.UnhookAll();
  return S_OK;
}

STDMETHODIMP CNktHookLibImpl::EnableHook(__in VARIANT items, __in VARIANT_BOOL enable)
{
  CHookInfo cHkInfo;
  HRESULT hRes;

  hRes = cHkInfo.Init(items);
  if (SUCCEEDED(hRes))
  {
    hRes = NKT_HRESULT_FROM_WIN32(cHookLib.EnableHook(cHkInfo.lpInfo, cHkInfo.nCount,
                                                      (enable != VARIANT_FALSE) ? TRUE : FALSE));
  }
  return hRes;
}

STDMETHODIMP CNktHookLibImpl::RemoveHook(__in VARIANT items, __in VARIANT_BOOL disable)
{
  CHookInfo cHkInfo;
  HRESULT hRes;

  hRes = cHkInfo.Init(items);
  if (SUCCEEDED(hRes))
  {
    hRes = NKT_HRESULT_FROM_WIN32(cHookLib.RemoveHook(cHkInfo.lpInfo, cHkInfo.nCount,
                                                      (disable != VARIANT_FALSE) ? TRUE : FALSE));
  }
  return hRes;
}

STDMETHODIMP CNktHookLibImpl::put_SuspendThreadsWhileHooking(__in VARIANT_BOOL enable)
{
  cHookLib.SetSuspendThreadsWhileHooking((enable != VARIANT_FALSE) ? TRUE : FALSE);
  return S_OK;
}

STDMETHODIMP CNktHookLibImpl::get_SuspendThreadsWhileHooking(__out VARIANT_BOOL *enable)
{
  if (enable == NULL)
    return E_POINTER;
  *enable = (cHookLib.GetSuspendThreadsWhileHooking() != FALSE) ? VARIANT_TRUE : VARIANT_FALSE;
  return S_OK;
}

STDMETHODIMP CNktHookLibImpl::put_ShowDebugOutput(__in VARIANT_BOOL enable)
{
  cHookLib.SetEnableDebugOutput((enable != VARIANT_FALSE) ? TRUE : FALSE);
  return S_OK;
}

STDMETHODIMP CNktHookLibImpl::get_ShowDebugOutput(__out VARIANT_BOOL *enable)
{
  if (enable == NULL)
    return E_POINTER;
  *enable = (cHookLib.GetEnableDebugOutput() != FALSE) ? VARIANT_TRUE : VARIANT_FALSE;
  return S_OK;
}

STDMETHODIMP CNktHookLibImpl::GetModuleBaseAddress(__in BSTR moduleName, __out my_ssize_t *baseAddress)
{
  if (baseAddress == NULL)
    return E_POINTER;
  *baseAddress = 0;
  if (moduleName == NULL)
    return E_POINTER;
  *baseAddress = (my_ssize_t)NktHookLibHelpers::GetModuleBaseAddress(moduleName);
  return S_OK;
}

STDMETHODIMP CNktHookLibImpl::GetRemoteModuleBaseAddress(__in LONG pid, __in BSTR moduleName,
                                                         __in VARIANT_BOOL scanMappedImages,
                                                         __out my_ssize_t *baseAddress)
{
  HANDLE hProc;

  if (baseAddress == NULL)
    return E_POINTER;
  *baseAddress = 0;
  if (moduleName == NULL)
    return E_POINTER;
  hProc = ::OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_DUP_HANDLE|PROCESS_QUERY_INFORMATION, FALSE,
                        (DWORD)pid);
  if (hProc == NULL)
  {
    hProc = ::OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_DUP_HANDLE|PROCESS_QUERY_LIMITED_INFORMATION,
                          FALSE, (DWORD)pid);
  }
  if (hProc == NULL)
    return NKT_HRESULT_FROM_WIN32(::GetLastError());
  *baseAddress = (my_ssize_t)NktHookLibHelpers::GetRemoteModuleBaseAddress(hProc, moduleName,
                                                         (scanMappedImages != VARIANT_FALSE) ? TRUE : FALSE);
  ::CloseHandle(hProc);
  return S_OK;

}

STDMETHODIMP CNktHookLibImpl::GetProcedureAddress(__in my_ssize_t moduleBaseAddress, __in BSTR procName,
                                                  __out my_ssize_t *funcAddress)
{
  LPSTR szProcNameA;

  if (funcAddress == NULL)
    return E_POINTER;
  *funcAddress = 0;
  if (moduleBaseAddress == NULL)
    return E_INVALIDARG;
  if (procName == NULL)
    return E_POINTER;
  szProcNameA = Wide2Ansi(procName);
  if (szProcNameA == NULL)
    return E_OUTOFMEMORY;
  *funcAddress = (my_ssize_t)NktHookLibHelpers::GetProcedureAddress((HINSTANCE)moduleBaseAddress, szProcNameA);
  free(szProcNameA);
  return S_OK;
}

STDMETHODIMP CNktHookLibImpl::GetRemoteProcedureAddress(__in LONG pid, __in my_ssize_t moduleBaseAddress,
                                                        __in BSTR procName, __out my_ssize_t *funcAddress)
{
  HANDLE hProc;
  LPSTR szProcNameA;

  if (funcAddress == NULL)
    return E_POINTER;
  *funcAddress = 0;
  if (moduleBaseAddress == NULL)
    return E_INVALIDARG;
  if (procName == NULL)
    return E_POINTER;
  hProc = ::OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_DUP_HANDLE|PROCESS_QUERY_INFORMATION, FALSE,
                        (DWORD)pid);
  if (hProc == NULL)
  {
    hProc = ::OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_DUP_HANDLE|PROCESS_QUERY_LIMITED_INFORMATION,
                          FALSE, (DWORD)pid);
  }
  if (hProc == NULL)
    return NKT_HRESULT_FROM_WIN32(::GetLastError());
  szProcNameA = Wide2Ansi(procName);
  if (szProcNameA == NULL)
  {
    ::CloseHandle(hProc);
    return E_OUTOFMEMORY;
  }
  *funcAddress = (my_ssize_t)NktHookLibHelpers::GetRemoteProcedureAddress(hProc, (HINSTANCE)moduleBaseAddress,
                                                                          szProcNameA);
  free(szProcNameA);
  ::CloseHandle(hProc);
  return S_OK;
}

STDMETHODIMP CNktHookLibImpl::CreateProcessWithDll(__in BSTR applicationName, __in BSTR commandLine,
                                                   __in my_ssize_t processAttributes, __in my_ssize_t threadAttributes,
                                                   __in VARIANT_BOOL inheritHandles, __in LONG creationFlags,
                                                   __in BSTR environment, __in BSTR currentDirectory,
                                                   __in my_ssize_t startupInfo, __in BSTR dllName,
                                                   __in my_ssize_t signalCompletedEvent, __in BSTR initFunctionName,
                                                   __deref_out INktHookProcessInfo **ppProcInfo)
{
  CComObject<CNktHookProcessInfoImpl>* pProcInfo = NULL;
  LPSTR szInitFunctionA = NULL;
  HRESULT hRes;

  if (ppProcInfo == NULL)
    return E_POINTER;
  *ppProcInfo = NULL;
  if (dllName == NULL)
    return E_POINTER;
  hRes = CComObject<CNktHookProcessInfoImpl>::CreateInstance(&pProcInfo);
  if (SUCCEEDED(hRes) && initFunctionName != NULL && initFunctionName[0] != 0)
  {
    szInitFunctionA = Wide2Ansi(initFunctionName);
    if (szInitFunctionA == NULL)
      hRes = E_OUTOFMEMORY;
  }
  if (SUCCEEDED(hRes))
  {
    hRes = NKT_HRESULT_FROM_WIN32(NktHookLibHelpers::CreateProcessWithDllW(applicationName, commandLine,
                       (LPSECURITY_ATTRIBUTES)processAttributes, (LPSECURITY_ATTRIBUTES)threadAttributes,
                       (inheritHandles != VARIANT_FALSE) ? TRUE : FALSE, (DWORD)creationFlags, environment,
                       currentDirectory, (LPSTARTUPINFOW)startupInfo, &(pProcInfo->sProcInfo), dllName,
                       (HANDLE)signalCompletedEvent, szInitFunctionA));
    if (SUCCEEDED(hRes))
      pProcInfo->QueryInterface<INktHookProcessInfo>(ppProcInfo);
    else
      pProcInfo->Release();
  }
  if (szInitFunctionA != NULL)
    free(szInitFunctionA);
  return hRes;
}

STDMETHODIMP CNktHookLibImpl::CreateProcessWithLogonAndDll(__in BSTR userName, __in BSTR domain, __in BSTR password,
                              __in LONG logonFlags, __in BSTR applicationName, __in BSTR commandLine,
                              __in LONG creationFlags, __in BSTR environment, __in BSTR currentDirectory,
                              __in my_ssize_t startupInfo, __in BSTR dllName, __in my_ssize_t signalCompletedEvent,
                              __in BSTR initFunctionName, __deref_out INktHookProcessInfo **ppProcInfo)
{
  CComObject<CNktHookProcessInfoImpl>* pProcInfo = NULL;
  LPSTR szInitFunctionA = NULL;
  HRESULT hRes;

  if (ppProcInfo == NULL)
    return E_POINTER;
  *ppProcInfo = NULL;
  if (dllName == NULL)
    return E_POINTER;
  hRes = CComObject<CNktHookProcessInfoImpl>::CreateInstance(&pProcInfo);
  if (SUCCEEDED(hRes) && initFunctionName != NULL && initFunctionName[0] != 0)
  {
    szInitFunctionA = Wide2Ansi(initFunctionName);
    if (szInitFunctionA == NULL)
      hRes = E_OUTOFMEMORY;
  }
  if (SUCCEEDED(hRes))
  {
    hRes = NKT_HRESULT_FROM_WIN32(NktHookLibHelpers::CreateProcessWithLogonAndDllW(userName, domain, password,
                       (DWORD)logonFlags, applicationName, commandLine, (DWORD)creationFlags, environment,
                       currentDirectory, (LPSTARTUPINFOW)startupInfo, &(pProcInfo->sProcInfo), dllName,
                       (HANDLE)signalCompletedEvent, szInitFunctionA));
    if (SUCCEEDED(hRes))
      pProcInfo->QueryInterface<INktHookProcessInfo>(ppProcInfo);
    else
      pProcInfo->Release();
  }
  if (szInitFunctionA != NULL)
    free(szInitFunctionA);
  return hRes;
}

STDMETHODIMP CNktHookLibImpl::CreateProcessWithTokenAndDll(__in my_ssize_t token, __in LONG logonFlags,
                              __in BSTR applicationName, __in BSTR commandLine, __in LONG creationFlags,
                              __in BSTR environment, __in BSTR currentDirectory, __in my_ssize_t startupInfo,
                              __in BSTR dllName, __in my_ssize_t signalCompletedEvent, __in BSTR initFunctionName,
                              __deref_out INktHookProcessInfo **ppProcInfo)
{
  CComObject<CNktHookProcessInfoImpl>* pProcInfo = NULL;
  LPSTR szInitFunctionA = NULL;
  HRESULT hRes;

  if (ppProcInfo == NULL)
    return E_POINTER;
  *ppProcInfo = NULL;
  if (dllName == NULL)
    return E_POINTER;
  hRes = CComObject<CNktHookProcessInfoImpl>::CreateInstance(&pProcInfo);
  if (SUCCEEDED(hRes) && initFunctionName != NULL && initFunctionName[0] != 0)
  {
    szInitFunctionA = Wide2Ansi(initFunctionName);
    if (szInitFunctionA == NULL)
      hRes = E_OUTOFMEMORY;
  }
  if (SUCCEEDED(hRes))
  {
    hRes = NKT_HRESULT_FROM_WIN32(NktHookLibHelpers::CreateProcessWithTokenAndDllW((HANDLE)token, (DWORD)logonFlags,
                       applicationName, commandLine, (DWORD)creationFlags, environment, currentDirectory,
                       (LPSTARTUPINFOW)startupInfo, &(pProcInfo->sProcInfo), dllName,
                       (HANDLE)signalCompletedEvent, szInitFunctionA));
    if (SUCCEEDED(hRes))
      pProcInfo->QueryInterface<INktHookProcessInfo>(ppProcInfo);
    else
      pProcInfo->Release();
  }
  if (szInitFunctionA != NULL)
    free(szInitFunctionA);
  return hRes;
}

STDMETHODIMP CNktHookLibImpl::InjectDll(__in LONG processId, __in BSTR dllName, __in_opt BSTR initFunctionName,
                                        __in_opt LONG processInitWaitTimeoutMs,
                                        __out_opt my_ssize_t *injectorThreadHandle)
{
  LPSTR szInitFunctionA = NULL;
  HRESULT hRes;

  if (dllName == NULL)
    return E_POINTER;
  if (dllName[0] == NULL || processId == 0 || processInitWaitTimeoutMs < -1)
    return E_INVALIDARG;
  if (initFunctionName != NULL && initFunctionName[0] != 0)
  {
    szInitFunctionA = Wide2Ansi(initFunctionName);
    if (szInitFunctionA == NULL)
      return E_OUTOFMEMORY;
  }
  if (processInitWaitTimeoutMs < 0)
    processInitWaitTimeoutMs = INFINITE;
  hRes = NKT_HRESULT_FROM_WIN32(NktHookLibHelpers::InjectDllByPidW((DWORD)processId, dllName, szInitFunctionA,
                                                                   (DWORD)processInitWaitTimeoutMs,
                                                                   (LPHANDLE)injectorThreadHandle));
  if (szInitFunctionA != NULL)
    free(szInitFunctionA);
  return hRes;
}

STDMETHODIMP CNktHookLibImpl::InjectDllH(__in my_ssize_t processHandle, __in BSTR dllName,
                                         __in_opt BSTR initFunctionName, __in_opt LONG processInitWaitTimeoutMs,
                                         __out_opt my_ssize_t *injectorThreadHandle)
{
  LPSTR szInitFunctionA = NULL;
  HRESULT hRes;

  if (dllName == NULL)
    return E_POINTER;
  if (dllName[0] == NULL || processHandle == 0 || processInitWaitTimeoutMs < -1)
    return E_INVALIDARG;
  if (initFunctionName != NULL && initFunctionName[0] != 0)
  {
    szInitFunctionA = Wide2Ansi(initFunctionName);
    if (szInitFunctionA == NULL)
      return E_OUTOFMEMORY;
  }
  if (processInitWaitTimeoutMs < 0)
    processInitWaitTimeoutMs = INFINITE;
  hRes = NKT_HRESULT_FROM_WIN32(NktHookLibHelpers::InjectDllByHandleW((HANDLE)processHandle, dllName, szInitFunctionA,
                                                                      (DWORD)processInitWaitTimeoutMs,
                                                                      (LPHANDLE)injectorThreadHandle));
  if (szInitFunctionA != NULL)
    free(szInitFunctionA);
  return hRes;
}

//-----------------------------------------------------------

CNktHookLibImpl::CHookInfo::CHookInfo()
{
  lpInfo = NULL;
  lplpHookInfoPtr = NULL;
  nCount = 0;
  return;
}

CNktHookLibImpl::CHookInfo::~CHookInfo()
{
  SIZE_T i;

  if (lplpHookInfoPtr != NULL)
  {
    for (i=0; i<nCount; i++)
    {
      if (lplpHookInfoPtr[i] != NULL)
        lplpHookInfoPtr[i]->Release();
    }
    free(lplpHookInfoPtr);
  }
  if (lpInfo != NULL)
    free(lpInfo);
  return;
}

HRESULT CNktHookLibImpl::CHookInfo::Init(__in VARIANT items)
{
  VARTYPE nVarType;
  union {
    LPVOID pv;
    IUnknown **lpUnk;
    IDispatch **lpDisp;
  };
  CNktHookInfoImpl *lpHookInfoImpl;
  SIZE_T i;
  HRESULT hRes;

  switch (V_VT(&items))
  {
    case VT_DISPATCH:
      if (items.pdispVal == NULL)
        return E_POINTER;
      lpDisp = &(items.pdispVal);
      nCount = 1;
      break;

    case VT_BYREF|VT_DISPATCH:
      if (items.ppdispVal == NULL)
        return E_POINTER;
      lpDisp = items.ppdispVal;
      nCount = 1;
      break;

    case VT_UNKNOWN:
      if (items.punkVal == NULL)
        return E_POINTER;
      lpUnk = &(items.punkVal);
      nCount = 1;
      break;

    case VT_BYREF|VT_UNKNOWN:
      if (items.ppunkVal == NULL)
        return E_POINTER;
      lpUnk = items.ppunkVal;
      nCount = 1;
      break;

    case VT_ARRAY|VT_DISPATCH:
    case VT_ARRAY|VT_UNKNOWN:
      if (items.parray == NULL)
        return E_POINTER;
      //check for vector
      if (::SafeArrayGetDim(items.parray) != 1)
        return E_INVALIDARG;
      //check count
      nCount = (SIZE_T)(items.parray->rgsabound[0].cElements);
      if (nCount < 1)
        return E_INVALIDARG;
      //check type
      hRes = ::SafeArrayGetVartype(items.parray, &nVarType);
      if (FAILED(hRes))
        return hRes;
      if (nVarType != VT_UNKNOWN && nVarType != VT_DISPATCH)
        return E_INVALIDARG;
      break;

    default:
      return E_INVALIDARG;
  }
  //allocate memory
  lpInfo = (CNktHookLib::HOOK_INFO*)malloc(nCount * sizeof(CNktHookLib::HOOK_INFO));
  if (lpInfo == NULL)
    return E_POINTER;
  memset(lpInfo, 0, nCount * sizeof(CNktHookLib::HOOK_INFO));
  lplpHookInfoPtr = (INktHookInfo**)malloc(nCount * sizeof(INktHookInfo*));
  if (lplpHookInfoPtr == NULL)
    return E_POINTER;
  memset(lplpHookInfoPtr, 0, nCount * sizeof(INktHookInfo*));
  //get items
  switch (V_VT(&items))
  {
    case VT_DISPATCH:
    case VT_BYREF|VT_DISPATCH:
      if (lpDisp == NULL)
        return E_POINTER;
      hRes = (*lpDisp)->QueryInterface(IID_INktHookInfo, (LPVOID*)&lplpHookInfoPtr[0]);
      if (FAILED(hRes))
        return hRes;
      lpHookInfoImpl = static_cast<CNktHookInfoImpl*>(lplpHookInfoPtr[0]);
      lpInfo[0] = lpHookInfoImpl->sInfo;
      break;

    case VT_UNKNOWN:
    case VT_BYREF|VT_UNKNOWN:
      if (lpUnk == NULL)
        return E_POINTER;
      hRes = (*lpUnk)->QueryInterface(IID_INktHookInfo, (LPVOID*)&lplpHookInfoPtr[0]);
      if (FAILED(hRes))
        return hRes;
      lpHookInfoImpl = static_cast<CNktHookInfoImpl*>(lplpHookInfoPtr[0]);
      lpInfo[0] = lpHookInfoImpl->sInfo;
      break;

    case VT_ARRAY|VT_DISPATCH:
    case VT_ARRAY|VT_UNKNOWN:
      hRes = ::SafeArrayAccessData(items.parray, &pv);
      if (SUCCEEDED(hRes))
      {
        for (i=0; i<nCount && SUCCEEDED(hRes); i++)
        {
          switch (nVarType)
          {
            case VT_UNKNOWN:
              if (lpUnk[i] != NULL)
                hRes = lpUnk[i]->QueryInterface(IID_INktHookInfo, (LPVOID*)&lplpHookInfoPtr[i]);
              else
                hRes = E_POINTER;
              break;
            case VT_DISPATCH:
              if (lpDisp[i] != NULL)
                hRes = lpDisp[i]->QueryInterface(IID_INktHookInfo, (LPVOID*)&lplpHookInfoPtr[i]);
              else
                hRes = E_POINTER;
              break;
          }
          if (SUCCEEDED(hRes))
          {
            lpHookInfoImpl = static_cast<CNktHookInfoImpl*>(lplpHookInfoPtr[i]);
            lpInfo[i] = lpHookInfoImpl->sInfo;
          }
        }
        ::SafeArrayUnaccessData(items.parray);
      }
      if (FAILED(hRes))
        return hRes;
      break;
  }
  return S_OK;
}

VOID CNktHookLibImpl::CHookInfo::StoreInfo()
{
  CNktHookInfoImpl *lpHookInfoImpl;
  SIZE_T i;

  for (i=0; i<nCount; i++)
  {
    lpHookInfoImpl = static_cast<CNktHookInfoImpl*>(lplpHookInfoPtr[i]);
    lpHookInfoImpl->sInfo = lpInfo[i];
  }
  return;
}

//-----------------------------------------------------------

static LPSTR Wide2Ansi(__in_nz_opt LPCWSTR szSrcW)
{
  SIZE_T nSrcLen, nDestLen;
  LPSTR szDestA;

  nSrcLen = wcslen(szSrcW);
  nDestLen = (SIZE_T)::WideCharToMultiByte(CP_ACP, 0, szSrcW, (int)nSrcLen, NULL, 0, NULL, NULL);
  szDestA = (LPSTR)malloc((nDestLen+1)*sizeof(CHAR));
  if (szDestA == NULL)
    return NULL;
  nDestLen = (SIZE_T)::WideCharToMultiByte(CP_ACP, 0, szSrcW, (int)nSrcLen, szDestA, (int)(nDestLen+1), NULL, NULL);
  szDestA[nDestLen] = 0;
  return szDestA;
}
