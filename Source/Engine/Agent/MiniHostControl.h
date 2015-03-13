/*
 * Copyright (c) 2010-2012 Nektra S.A., Buenos Aires, Argentina.
 * All rights reserved.
 *
 **/

#ifndef _NKT_DV_MINI_CLR_HOSTCONTROL_H
#define _NKT_DV_MINI_CLR_HOSTCONTROL_H

#include "..\Common\EngBaseObj.h"
#include "..\Common\WaitableObjects.h"
#include "..\Common\ComPtr.h"
#include <mscoree.h>

//-----------------------------------------------------------

class CMiniHostControl : public IHostControl
{
public:
  CMiniHostControl()
    {
    NktInterlockedExchange(&nRefCount, 0);
    return;
    };

  HRESULT STDMETHODCALLTYPE QueryInterface(__in REFIID riid, __deref_out void **ppvObject)
    {
    if (ppvObject == NULL)
      return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IHostControl)
    {
      *ppvObject = static_cast<IHostControl*>(this);
      static_cast<IUnknown*>(*ppv)->AddRef();
      return S_OK;
    }
    *ppvObject = NULL;
    return E_NOINTERFACE;
    };

  ULONG STDMETHODCALLTYPE AddRef()
    {
    return (ULONG)NktInterlockedIncrement(&nRefCount);
    };

  ULONG STDMETHODCALLTYPE Release()
    {
     ULONG nNewRefCount = (ULONG)NktInterlockedDecrement(&nRefCount);
     if (nNewRefCount == 0)
       delete this;
     return nNewRefCount;
    };

  HRESULT STDMETHODCALLTYPE GetHostManager(__in REFIID riid, __deref_out void **ppObject)
    {
    if (ppObject == NULL)
      return E_POINTER;
    *ppObject = NULL;
    return E_NOINTERFACE;
    };

  HRESULT STDMETHODCALLTYPE SetAppDomainManager(__in DWORD dwAppDomainID,
                                                __in IUnknown *pUnkAppDomainManager)
    {

    HRESULT hRes


    HadesAD::IHadesVM *pDomainManager = NULL;
    
    hRes = pUnkAppDomainManager->QueryInterface(__uuidof(HadesAD::IHadesVM), 
                                      reinterpret_cast<PVOID*>(&pDomainManager));

    m_Domains[dwAppDomainID] = pDomainManager;

    // Save the pointer to the default domain for convenience. We
    // initialize m_pDefaultDomainDomainManager to NULL in the
    // class's constructor. The first time this method is called
    // is for the default application domain.
    if (!m_pDefaultDomainDomainManager)
    {
      m_pDefaultDomainDomainManager = pDomainManager;
    }

    return hr;
  }

  HadesAD::IHadesVM* GetDomainManagerForDefaultDomain()
  {
    // AddRef the pointer before returning it.
    if (m_pDefaultDomainDomainManager) 
    {
      m_pDefaultDomainDomainManager->AddRef();
    }

    return m_pDefaultDomainDomainManager;
  }

  DomainMap& GetAllDomainManagers()
  {
    return m_Domains;
  }

private:
  LONG volatile nRefCount;
  TNktComPtr<ICustomAppDomainManager> cDefDomainManager;
};
