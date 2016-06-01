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

#define USE_MARSHALLING

#include "..\..\Common\EngBaseObj.h"
#include "..\..\Common\ComPtr.h"
#include "..\..\Common\LinkedList.h"
#include "..\..\Common\ParamsEnum.h"
#include "..\..\Common\Process.h"
#include "DevComActCtx.h"
#include "HookEngineInternals.h"
#include <mscoree.h>
#include <metahost.h>
class CHookEntry;
class CNktDvHookEngineCallback;

#if defined _M_IX86
  #import "DotNetHelpers\x86\mscorlib.tlb" auto_rename, raw_interfaces_only
#elif defined _M_X64
  #import "DotNetHelpers\x64\mscorlib.tlb" auto_rename, raw_interfaces_only
#else
  #error Unsupported platform
#endif

#ifndef NKT_TypeDef_PluginInterfaces
#define NKT_TypeDef_PluginInterfaces
typedef interface INktHookInfo INktHookInfo;
typedef interface INktHookCallInfoPlugin INktHookCallInfoPlugin;
#endif //NKT_TypeDef_PluginInterfaces

//-----------------------------------------------------------

class CHookCustomHandlerMgr : public CNktDvObject
{
public:
  static const IID NKT_IID_INktHookInfo;

  class CPlugin;
  class CPluginInstance;
  typedef TNktLnkLst<CPluginInstance> CPluginInstanceList;

  CHookCustomHandlerMgr();
  virtual ~CHookCustomHandlerMgr();

  virtual VOID Lock()
    {
    cMtx.Lock();
    return;
    };
  virtual VOID Unlock()
    {
    cMtx.Unlock();
    return;
    };

  virtual HRESULT LoadPlugin(__deref_out CPlugin **lplpPlugin, __in LPCWSTR szDllNameW);
  virtual VOID FreeUnusedPlugins();

  virtual HRESULT CreateHookInfo(__deref_out IDispatch **ppDisp, __out DWORD *lpdwHookInfoCookie,
                                 __in CHookEntry *lpHookEntry, __in CNktDvHookEngineCallback *lpHookEngCallback);
  virtual HRESULT InvalidateHookInfo(__in DWORD dwHookInfoCookie);

  virtual HRESULT ProcessFunctionCall(__in CHookEntry *lpHookEntry, __in IDispatch *lpHookInfo,
                             __in CNktDvHookEngineCallback *lpHookEngCallback,
                             __in CPluginInstanceList *lpPluginInstancesList,
                             __deref_out CNktDvParamsEnumerator **lplpCustomParamsEnum,
                             __inout LPVOID lpCallInfo, __inout CNktDvParamsEnumerator *lpParamsEnum,
                             __inout CNktDvParam *lpResult, __inout LPBYTE lpIntercallCustomData);

  virtual HRESULT ActivateDeviareComContext(__out ULONG_PTR *lpnCookie);
  virtual VOID DeactivateDeviareComContext(__in ULONG_PTR nCookie);

public:
  typedef HRESULT (WINAPI *lpfnOnLoad)();
  typedef VOID (WINAPI *lpfnOnUnload)();
  typedef HRESULT (WINAPI *lpfnOnHookAdded)(__in IDispatch *lpHookInfo, __in DWORD dwChainIndex,
                                            __in LPCWSTR szParametersW);
  typedef VOID (WINAPI *lpfnOnHookRemoved)(__in IDispatch *lpHookInfo, __in DWORD dwChainIndex);
  typedef HRESULT (WINAPI *lpfnOnFunctionCall)(__in IDispatch *lpHookInfo, __in DWORD dwChainIndex,
                                               __in IDispatch *lpHookCallInfoPlugin);
  typedef HRESULT (WINAPI *lpfnGetFunctionCallbackName)(__in IDispatch *lpHookInfo,
                               __in DWORD dwChainIndex, __out BSTR *lpbstrFunctionCallName);
  typedef HRESULT (WINAPI *lpfnDllGetClassObject)(__in REFCLSID rclsid, __in REFIID riid,
                                                  __out LPVOID *ppv);
  typedef HRESULT (WINAPI *lpfnInternal1)(__in DWORD dwCode, __in SIZE_T nSubCodeA, __in SIZE_T nSubCodeB);

  class CPluginMethod : public CNktMemMgrObj
  {
  public:
    CPluginMethod();
    virtual ~CPluginMethod();

    VOID CleanUp();

  public:
    LPVOID lpProc;
#ifdef USE_MARSHALLING
    TNktComPtr<IStream> cDotNetStream;
#else //USE_MARSHALLING
    TNktComPtr<mscorlib::_MethodInfo> cMethod;
#endif //USE_MARSHALLING
  };

  class CPluginVariantArray : public CNktMemMgrObj
  {
  public:
    CPluginVariantArray(__in int nParamsCount);
    virtual ~CPluginVariantArray();

    VARIANT* Lock();
    VOID Unlock();

    SAFEARRAY* Get();

  private:
    SAFEARRAY *lpSA;
    BOOL bLocked;
  };

  class CPlugin : public CNktDvObject, public TNktLnkLstNode<CPlugin>
  {
  public:
    CPlugin(__in CHookCustomHandlerMgr *lpCustomHandlersMgr);
    virtual ~CPlugin();

    virtual VOID Lock()
      {
      cMtx.Lock();
      return;
      };
    virtual VOID Unlock()
      {
      cMtx.Unlock();
      return;
      };

    virtual VOID IncUsageCount()
      {
      NktInterlockedIncrement(&nUsageCount);
      return;
      };
    virtual VOID DecUsageCount()
      {
      NktInterlockedDecrement(&nUsageCount);
      return;
      };

    virtual HRESULT Call_OnLoad();
    virtual VOID Call_OnUnload();
    virtual HRESULT Call_OnHookAdded(__in CPluginInstance *lpPluginInst, __in IDispatch *lpHookInfo,
                                     __in DWORD dwChainIndex, __in LPCWSTR szParametersW);
    virtual HRESULT Call_GetFunctionCallbackName(__in CPluginInstance *lpPluginInst,
                                                 __in IDispatch *lpHookInfo, __in DWORD dwChainIndex,
                                                 __out BSTR *lpbstrFunctionName);
    virtual VOID Call_OnHookRemoved(__in CPluginInstance *lpPluginInst, __in IDispatch *lpHookInfo,
                                    __in DWORD dwChainIndex);
    virtual HRESULT Call_OnFunctionCall(__in CPluginInstance *lpPluginInst, __in IDispatch *lpHookInfo,
                                        __in DWORD dwChainIndex, __in IDispatch *lpHookCallInfoPlugin);

  public:
    TNktComPtr<CHookCustomHandlerMgr> cCustomHandlersMgr;
    CNktFastMutex cMtx;
    //----
    LPWSTR szDllNameW;
    HINSTANCE hDll;
    //----
    struct {
      BOOL bIsManaged;
#ifdef USE_MARSHALLING
      CNktFastMutex cMarshalMtx;
      TNktComPtr<IStream> cObjectStream;
#endif //USE_MARSHALLING
      TNktComPtr<mscorlib::_Object> cObject;
      TNktComPtr<mscorlib::_Type> cType;
    } sDotNet;
    CPluginMethod aPluginMethods[6];
    //----
    LONG volatile nNextSubIndex;
    LONG volatile nUsageCount;

  private:
    friend class CHookEntry;
    friend class CHookCustomHandlerMgr;

    virtual HRESULT InvokeDotNetMethod(__in int nMethodIndex, __inout SAFEARRAY *lpParameters,
                                       __out_opt VARIANT *lpVarResult);
#ifdef USE_MARSHALLING
    virtual HRESULT InvokeDotNetMethod(__in IStream *lpMethodStream, __inout SAFEARRAY *lpParameters,
                                       __out_opt VARIANT *lpVarResult);
    virtual HRESULT FindDotNetMethod(__in LPCWSTR szMethodNameW, __out IStream **lplpStream);
#else //USE_MARSHALLING
    virtual HRESULT InvokeDotNetMethod(__in mscorlib::_MethodInfo *lpMethod, __inout SAFEARRAY *lpParameters,
                                       __out_opt VARIANT *lpVarResult);
    virtual HRESULT FindDotNetMethod(__in LPCWSTR szMethodNameW, __out mscorlib::_MethodInfo **lplpMethodInfo);
#endif //USE_MARSHALLING
  };

  class CPluginInstance : public CNktDvObject, public TNktLnkLstNode<CPluginInstance>
  {
  public:
    CPluginInstance() : CNktDvObject()
      {
      nFlags = 0;
      return;
      };

  public:
    friend class CHookEntry;
    friend class CHookCustomHandlerMgr;

    ULONG nFlags;
    TNktComPtr<CPlugin> cPlugin;
    CPluginMethod cOnFunctionCallReRoute;
  };

private:
  virtual HRESULT InitializeDeviareCOM();
  virtual HRESULT InitializeDotNetMgr();
  virtual HRESULT IsDotNetModule(__in HINSTANCE hDll);

private:
  TNktComPtr<CNktDvProcess> cProc;
  TNktLnkLst<CPlugin> cLoadedPluginsList;
  CNktFastMutex cMtx;
  struct {
    CNktDeviareComActCtx cActCtx;
    LONG volatile nInitialized;
  } sDevCom;
  //----
  HINSTANCE hMsCoreeDll;
  TNktComPtr<ICorRuntimeHost> cCorRuntimeHost;
  TNktComPtr<mscorlib::_AppDomain> cAppDomain;
  //----
  LONG volatile nHookInfoCounter;
  HINSTANCE hDeviareComDll;
  lpfnDllGetClassObject fnDllGetClassObject;
  lpfnInternal1 fnInternal1;
};

//-----------------------------------------------------------

