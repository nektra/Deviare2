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

#ifndef _IDispEventWithTypeLibImpl_H
#define _IDispEventWithTypeLibImpl_H

#include <windows.h>
#include <atlbase.h>
#include <atlcom.h>

//-----------------------------------------------------------

template <UINT nID, class T, const IID* pdiid>
class ATL_NO_VTABLE IDispEventWithTypeLibImpl : public IDispEventSimpleImpl<nID, T, pdiid>
{
public:
  IDispEventWithTypeLibImpl(__in HINSTANCE _hDll) : IDispEventSimpleImpl<nID, T, pdiid>()
    {
    m_iid = *pdiid;
    lpMap = NULL;
    nMapCount = 0;
    hDll = _hDll;
    hResCS = cs.Init();
    return;
    };

  ~IDispEventWithTypeLibImpl()
    {
    if (lpMap != NULL)
    {
      delete [] lpMap;
      lpMap = NULL;
    }
    if (SUCCEEDED(hResCS))
      cs.Term();
    return;
    };

  STDMETHOD(GetTypeInfoCount)(__out UINT* pctinfo)
    {
    if (pctinfo == NULL)
      return E_POINTER;
    *pctinfo = 1;
    return S_OK;
    };

  STDMETHOD(GetTypeInfo)(__in UINT itinfo, __in LCID lcid, __deref_out ITypeInfo** pptinfo)
    {
    HRESULT hRes;

    hRes = EnsureTypeInfo();
    if (SUCCEEDED(hRes))
      hRes = InternalGetTypeInfo(itinfo, lcid, pptinfo);
    return hRes;
    };

  STDMETHOD(GetIDsOfNames)(__in REFIID riid, __in LPOLESTR* rgszNames, __in UINT cNames, __in LCID lcid,
                           __out DISPID* rgdispid)
    {
    HRESULT hRes;

    hRes = EnsureTypeInfo();
    if (SUCCEEDED(hRes))
      hRes = InternalGetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
    return hRes;
    };

  HRESULT GetFuncInfoFromId(__in const IID& iid, __in DISPID dispidMember, __in LCID lcid,
                            __inout _ATL_FUNC_INFO& info)
    {
    HRESULT hRes;

    hRes = EnsureTypeInfo();
    if (SUCCEEDED(hRes))
      hRes = AtlGetFuncInfoFromId(cTypeInfo, iid, dispidMember, lcid, info);
    return hRes;
    };

  VARTYPE GetUserDefinedType(__in ITypeInfo *pTI, __in HREFTYPE hrt)
    {
    return AtlGetUserDefinedType(pTI, hrt);
    };

protected:
  HRESULT EnsureTypeInfo()
    {
    HRESULT hRes = S_OK;

    if (cTypeInfo == NULL)
    {
      if (FAILED(hResCS))
      {
        ATLASSERT(FALSE);
        return hResCS;
      }
      {
        CComCritSecLock<CComCriticalSection> lock(cs);

        if (cTypeInfo == NULL)
        {
          CComPtr<ITypeLib> cTypeLib;
          ITypeInfo *lpTypeInfo;
          WCHAR szFilePathW[MAX_PATH];
          DWORD dw;

          dw = ::GetModuleFileNameW(hDll, szFilePathW, MAX_PATH);
          if (dw != 0 && dw != MAX_PATH)
            hRes = ::LoadTypeLib(szFilePathW, &cTypeLib);
          else
            hRes = E_FAIL;
          if (SUCCEEDED(hRes))
          {
            CComPtr<ITypeInfo> spTypeInfo;

            hRes = cTypeLib->GetTypeInfoOfGuid(m_iid, &spTypeInfo);
            if (SUCCEEDED(hRes))
            {
              CComPtr<ITypeInfo> spInfo(spTypeInfo);
              CComPtr<ITypeInfo2> spTypeInfo2;
              if (SUCCEEDED(spTypeInfo->QueryInterface(&spTypeInfo2)))
                spInfo = spTypeInfo2;
              lpTypeInfo = spInfo.Detach();
            }
          }
          if (SUCCEEDED(hRes))
            hRes = LoadNameCache(lpTypeInfo);
          if (SUCCEEDED(hRes))
            cTypeInfo.Attach(lpTypeInfo);
        }
      }
    }
    return hRes;
    };

  HRESULT LoadNameCache(__in ITypeInfo* lpTypeInfo)
    {
    TYPEATTR* lpTypeAttr;
    FUNCDESC* lpFuncDesc;
    int i;

    HRESULT hRes = lpTypeInfo->GetTypeAttr(&lpTypeAttr);
    nMapCount = lpTypeAttr->cFuncs;
    lpMap = NULL;
    if (SUCCEEDED(hRes))
    {
      if (nMapCount != 0)
      {
        lpMap = new stringdispid[nMapCount];
        if (lpMap == NULL)
        {
          lpTypeInfo->ReleaseTypeAttr(lpTypeAttr);
          return E_OUTOFMEMORY;
        }
      }
      for (i=0; i<nMapCount; i++)
      {
        if (SUCCEEDED(lpTypeInfo->GetFuncDesc(i, &lpFuncDesc)))
        {
          CComBSTR bstrName;

          if (SUCCEEDED(lpTypeInfo->GetDocumentation(lpFuncDesc->memid, &bstrName, NULL, NULL, NULL)))
          {
            lpMap[i].bstr.Attach(bstrName.Detach());
            lpMap[i].nLen = (int)::SysStringLen(lpMap[i].bstr);
            lpMap[i].id = lpFuncDesc->memid;
          }
          lpTypeInfo->ReleaseFuncDesc(lpFuncDesc);
        }
      }
      lpTypeInfo->ReleaseTypeAttr(lpTypeAttr);
    }
    return S_OK;
    };

  HRESULT InternalGetTypeInfo(__in UINT itinfo, __in LCID lcid, __deref_out ITypeInfo** pptinfo)
    {
    HRESULT hRes;

    _ASSERT(pptinfo != NULL);
    if (pptinfo == NULL)
      return E_POINTER;
    *pptinfo = NULL;
    if (itinfo != 0)
      return DISP_E_BADINDEX;
    hRes = EnsureTypeInfo();
    if (SUCCEEDED(hRes))
    {
      *pptinfo = cTypeInfo;
      (*pptinfo)->AddRef();
    }
    return hRes;
    };

  HRESULT InternalGetIDsOfNames(__in REFIID /* riid */, __in  LPOLESTR* rgszNames, __in UINT cNames,
                                __in LCID lcid, __out DISPID* rgdispid)
    {
    HRESULT hRes;
    int j, n;

    hRes = EnsureTypeInfo();
    if (SUCCEEDED(hRes))
    {
      hRes = E_FAIL;
      if (lpMap != NULL && cNames == 1)
      {
        n = ocslen(rgszNames[0]);
        for (j=nMapCount-1; j>=0; j--)
        {
          if (n == lpMap[j].nLen &&
              memcmp(lpMap[j].bstr, rgszNames[0], lpMap[j].nLen*sizeof(OLECHAR)) == 0)
          {
            rgdispid[0] = lpMap[j].id;
            hRes = S_OK;
            break;
          }
        }
      }
      if (FAILED(hRes))
        hRes = cTypeInfo->GetIDsOfNames(rgszNames, cNames, rgdispid);
    }
    return hRes;
    };

protected:
  CComCriticalSection cs;
  HRESULT hResCS;
  HINSTANCE hDll;

  struct stringdispid
  {
    CComBSTR bstr;
    int nLen;
    DISPID id;
    stringdispid() : nLen(0), id(DISPID_UNKNOWN){}
  };

  stringdispid *lpMap;
  int nMapCount;
  CComPtr<ITypeInfo> cTypeInfo;
};

//-----------------------------------------------------------

#endif //_IDispEventWithTypeLibImpl_H
