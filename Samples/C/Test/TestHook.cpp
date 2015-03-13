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

#include "TestHook.h"
#include "msxml\msxml6.h"

//-----------------------------------------------------------

typedef TNktArrayListWithFree<LPWSTR,128> TApiList;

//-----------------------------------------------------------

//88D96A05-F192-11D4-A65F-0040963251E5
static const GUID CLSID_DOMDocument60 = {
  0x88D96A05, 0xF192, 0x11D4, { 0xA6, 0x5F, 0x00, 0x40, 0x96, 0x32, 0x51, 0xE5 }
};

//2933BF96-7B36-11D2-B20E-00C04F983E60
static const GUID IID_IXMLDOMDocument3 = {
  0x2933BF96, 0x7B36, 0x11D2, { 0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60 }
};

//-----------------------------------------------------------

static BOOL LoadHooks(TApiList &aApiList);

//-----------------------------------------------------------

void TestHookExecOrAttach(__in Deviare2::INktSpyMgr *lpSpyMgr)
{
  CComPtr<Deviare2::INktProcess> cProc;
  CComPtr<Deviare2::INktHooksEnum> cHooksEnum;
  CComPtr<Deviare2::INktHook> cHook;
  TApiList aApiList;
  HRESULT hRes;
  CComVariant cVt, CVtContinueEv;
  BSTR bstr;
  SIZE_T i, k, nCount, nCount2;
  long lTemp, nFlags;

  if (LoadHooks(aApiList) == FALSE)
    return;
  //create hooks
  nFlags = (sCmdLineParams.bAsyncCallbacks != FALSE) ? Deviare2::flgAsyncCallbacks : 0;
  nFlags |= Deviare2::flgAutoHookChildProcess;
  //nFlags |= Deviare2::flgRestrictAutoHookToSameExecutable;
  nCount = aApiList.GetCount();
  hRes = lpSpyMgr->CreateHooksCollection(&cHooksEnum);
  for (i=0; i<nCount && SUCCEEDED(hRes); i++)
  {
    cHook.Release();
    cVt.bstrVal = ::SysAllocString(aApiList[i]);
    if (cVt.bstrVal == NULL)
    {
err_nomem:
      LogPrintNoTick(L"Error: Not enough memory.\n");
      return;
    }
    cVt.vt = VT_BSTR;
    hRes = lpSpyMgr->CreateHook(cVt, nFlags, &cHook);
    cVt.Clear();
    //add custom handlers
    lTemp = Deviare2::flgChDontCallIfLoaderLocked;
    if (sCmdLineParams.bCustomHandlerDebug != FALSE)
      lTemp |= Deviare2::flgChDisableExceptionChecks;
    nCount2 = sCmdLineParams.aCustomHandlers.GetCount();
    for (k=0; k<nCount2 && SUCCEEDED(hRes); k++)
    {
      bstr = ::SysAllocString(sCmdLineParams.aCustomHandlers[k]);
      if (bstr == NULL)
        goto err_nomem;
      hRes = cHook->AddCustomHandler(bstr, lTemp, L"");
      ::SysFreeString(bstr);
    }
    if (SUCCEEDED(hRes))
      hRes = cHooksEnum->Add(cHook);
    LogPrintNoTick(L"CNktDvEngine::CreateHook (%s) => %08X\n", aApiList[i], hRes);
  }
  //load process
  if (sCmdLineParams.nTestType == ttTestHookExec &&
      SUCCEEDED(hRes))
  {
    bstr = ::SysAllocString((LPWSTR)(sCmdLineParams.sExec.cStrExeFileNameW));
    if (bstr == NULL)
      goto err_nomem;
    hRes = lpSpyMgr->__CreateProcess(bstr, VARIANT_TRUE, &CVtContinueEv, &cProc);
    ::SysFreeString(bstr);
    LogPrintNoTick(L"CNktDvProcess::Create => %08X\n", hRes);
  }
  //attach hooks to process
  if (SUCCEEDED(hRes))
  {
    switch (sCmdLineParams.nTestType)
    {
      case ttTestHookExec:
        if (sCmdLineParams.bBatchMode == FALSE)
        {
          nCount = (SUCCEEDED(cHooksEnum->get_Count(&lTemp))) ? (SIZE_T)(ULONG)lTemp : 0;
          for (i=0; i<nCount && SUCCEEDED(hRes); i++)
          {
            cHook.Release();
            hRes = cHooksEnum->GetAt((LONG)i, &cHook);
            if (SUCCEEDED(hRes))
            {
              cVt = cProc;
              hRes = cHook->Attach(cVt, VARIANT_TRUE);
              cVt.Clear();
            }
            LogPrintNoTick(L"CNktDvHook::AddProcess (%s) => %08X\n", aApiList[i], hRes);
          }
        }
        else
        {
          cVt = cProc;
          hRes = cHooksEnum->Attach(cVt, VARIANT_TRUE);
          cVt.Clear();
          LogPrintNoTick(L"CNktDvHooksEnumerator::AddProcess => %08X\n", hRes);
        }
        break;

      case ttTestHookAttach:
        //attach to running processes
        for (k=0; k<sCmdLineParams.sAttach.nCount && SUCCEEDED(hRes); k++)
        {
          cProc.Release();
          hRes = lpSpyMgr->ProcessFromPID((long)(sCmdLineParams.sAttach.dwPids[k]), &cProc);
          LogPrintNoTick(L"CNktDvProcess::CreateFromPID (%lu) => %08X\n",
                         sCmdLineParams.sAttach.dwPids[k], hRes);
          if (SUCCEEDED(hRes))
          {
            if (sCmdLineParams.bBatchMode == FALSE)
            {
              nCount = (SUCCEEDED(cHooksEnum->get_Count(&lTemp))) ? (SIZE_T)(ULONG)lTemp : 0;
              for (i=0; i<nCount && SUCCEEDED(hRes); i++)
              {
                cHook.Release();
                hRes = cHooksEnum->GetAt((LONG)i, &cHook);
                if (SUCCEEDED(hRes))
                {
                  cVt = cProc;
                  hRes = cHook->Attach(cVt, VARIANT_TRUE);
                  cVt.Clear();
                }
                LogPrintNoTick(L"CNktDvHook::AddProcess (%s) => %08X\n", aApiList[i], hRes);
              }
            }
            else
            {
              cVt = cProc;
              hRes = cHooksEnum->Attach(cVt, VARIANT_TRUE);
              cVt.Clear();
              LogPrintNoTick(L"CNktDvHooksEnumerator::AddProcess => %08X\n", hRes);
            }
          }
        }
        break;
    }
  }
  //activate hooks
  if (SUCCEEDED(hRes))
  {
    if (sCmdLineParams.bBatchMode == FALSE)
    {
      nCount = (SUCCEEDED(cHooksEnum->get_Count(&lTemp))) ? (SIZE_T)(ULONG)lTemp : 0;
      for (i=0; i<nCount && SUCCEEDED(hRes); i++)
      {
        cHook.Release();
        hRes = cHooksEnum->GetAt((LONG)i, &cHook);
        if (SUCCEEDED(hRes))
          hRes = cHook->Hook(VARIANT_TRUE);
        LogPrintNoTick(L"CNktDvHook::Hook (%s) => %08X\n", aApiList[i], hRes);
      }
    }
    else
    {
      hRes = cHooksEnum->Hook(VARIANT_TRUE);
      LogPrintNoTick(L"CNktDvHooksEnumerator::Hook => %08X\n", hRes);
    }
  }
  //continue process
  if (sCmdLineParams.nTestType == ttTestHookExec &&
      CVtContinueEv.vt != VT_EMPTY)
  {
    hRes = lpSpyMgr->ResumeProcess(cProc, CVtContinueEv);
  }
  //wait for exit key
  while (IsExitKeyPressed() == FALSE)
    ::Sleep(100);
  return;
}

static BOOL LoadHooks(TApiList &aApiList)
{
  CComPtr<IXMLDOMDocument3> cXmlDoc;
  CComPtr<IXMLDOMElement> cXmlRootElem;
  CComPtr<IXMLDOMNodeList> cXmlResultNodes;
  CComPtr<IXMLDOMNode> cXmlNode;
  CComPtr<IXMLDOMElement> cXmlElem;
  HRESULT hRes;
  VARIANT_BOOL bStatus;
  long i, nCount;
  BSTR bstr;
  LPWSTR sW;
  SIZE_T nLen;
  CComVariant cVt;

  //create xml document
  hRes = ::CoCreateInstance(CLSID_DOMDocument60, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument3,
                            (void**)&cXmlDoc);
  if (FAILED(hRes))
  {
    if (hRes == E_OUTOFMEMORY)
    {
lh_err_nomem:
      LogPrintNoTick(L"Error: Not enough memory.\n");
    }
    else
    {
      LogPrintNoTick(L"Error: Cannot create IXMLDOMDocument3 object.\n");
    }
    return FALSE;
  }
  cVt.Clear();
  hRes = cXmlDoc->put_async(VARIANT_FALSE);
  if (SUCCEEDED(hRes))
    hRes = cXmlDoc->put_validateOnParse(VARIANT_FALSE);
  if (SUCCEEDED(hRes))
    hRes = cXmlDoc->put_resolveExternals(VARIANT_FALSE);
  if (FAILED(hRes))
  {
    if (hRes == E_OUTOFMEMORY)
      goto lh_err_nomem;
    LogPrintNoTick(L"Error: Cannot initialize IXMLDOMDocument3 object.\n");
    return FALSE;
  }
  cVt.bstrVal = ::SysAllocString((LPWSTR)(sCmdLineParams.cStrXmlFileNameW));
  if (cVt.bstrVal == NULL)
    goto lh_err_nomem;
  cVt.vt = VT_BSTR;
  hRes = cXmlDoc->load(cVt, &bStatus);
  cVt.Clear();
  if (bStatus == VARIANT_FALSE)
  {
    CComPtr<IXMLDOMParseError> cObjError;

    hRes = cXmlDoc->get_parseError(&cObjError);
    bstr = NULL;
    if (SUCCEEDED(hRes) && cObjError!=NULL)
    {
      hRes = cObjError->get_reason(&bstr);
      cObjError.Release();
    }
    LogPrintNoTick(L"Error: Cannot load xml file.");
    if (bstr != NULL)
    {
      LogPrintNoTick(L" (%s)", (LPWSTR)bstr);
      ::SysFreeString(bstr);
    }
    LogPrintNoTick(L"\n");
    return FALSE;
  }
  //----
  hRes = cXmlDoc->get_documentElement(&cXmlRootElem);
  if (hRes == E_OUTOFMEMORY)
    goto lh_err_nomem;
  if (FAILED(hRes) || cXmlRootElem==NULL)
  {
    LogPrintNoTick(L"Error: Cannot get root element (0x%08X).\n", (ULONG)hRes);
    return FALSE;
  }
  hRes = cXmlRootElem->selectNodes(L"/hooks/hook" , &cXmlResultNodes);
  if (hRes == E_OUTOFMEMORY)
    goto lh_err_nomem;
  if (FAILED(hRes))
  {
    LogPrintNoTick(L"Error: Cannot select 'hook' nodes (0x%08X).\n", (ULONG)hRes);
    return FALSE;
  }
  //----
  hRes = cXmlResultNodes->get_length(&nCount);
  if (hRes == E_OUTOFMEMORY)
    goto lh_err_nomem;
  if (FAILED(hRes) || nCount == 0)
  {
    LogPrintNoTick(L"Error: Cannot get 'hook' nodes count or there are no items (0x%08X).\n", (ULONG)hRes);
    return FALSE;
  }
  for (i=0; i<nCount; i++)
  {
    cXmlNode.Release();
    hRes = cXmlResultNodes->get_item(i, &cXmlNode);
    if (hRes == E_OUTOFMEMORY)
      goto lh_err_nomem;
    if (FAILED(hRes))
    {
lh_err_cantgetfunction:
      LogPrintNoTick(L"Error: Cannot get 'hook' node #%ld (0x%08X).\n", i+1, (ULONG)hRes);
      return FALSE;
    }
    cXmlElem.Release();
    hRes = cXmlNode->QueryInterface(IID_IXMLDOMElement, (void**)&cXmlElem);
    if (hRes == E_OUTOFMEMORY)
      goto lh_err_nomem;
    if (FAILED(hRes))
      goto lh_err_cantgetfunction;
    hRes = cXmlElem->get_text(&bstr);
    if (hRes == E_OUTOFMEMORY)
      goto lh_err_nomem;
    if (FAILED(hRes))
    {
      LogPrintNoTick(L"Error: Cannot get 'hook' node #%ld value (0x%08X).\n", i+1, (ULONG)hRes);
      return FALSE;
    }
    nLen = (bstr != NULL) ? (::SysStringLen(bstr)) : 0;
    if (nLen == 0)
    {
      ::SysFreeString(bstr);
      LogPrintNoTick(L"Error: 'hook' node #%ld value is not an string or it is empty.\n", i+1);
      return FALSE;
    }
    sW = (LPWSTR)malloc((nLen+1)*sizeof(WCHAR));
    if (sW == NULL)
    {
      ::SysFreeString(bstr);
      goto lh_err_nomem;
    }
    memcpy(sW, bstr, nLen*sizeof(WCHAR));
    sW[nLen] = 0;
    ::SysFreeString(bstr);
    if (aApiList.AddElement(sW) == FALSE)
    {
      free(sW);
      goto lh_err_nomem;
    }
  }
  if (aApiList.GetCount() == 0)
  {
    LogPrintNoTick(L"Error: Api list is empty.\n");
    return FALSE;
  }
  return TRUE;
}
