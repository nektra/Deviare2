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

#include "ObjectBase.h"

//-----------------------------------------------------------
// CNktEnumeratorImpl

class ATL_NO_VTABLE CNktEnumeratorImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                         public CNktMemMgrObj,
                                         public IEnumVARIANT
{
public:
  CNktEnumeratorImpl() : CComObjectRootEx<CComMultiThreadModel>(),
                         CNktMemMgrObj(),
                         IEnumVARIANT()
    {
    nIndex = 0;
    return;
    };

  HRESULT AddItem(__in IUnknown *lpUnk);

  BEGIN_COM_MAP(CNktEnumeratorImpl)
    COM_INTERFACE_ENTRY(IEnumVARIANT)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct()
    {
    return S_OK;
    };

  void FinalRelease()
    {
    return;
    };

public:
  STDMETHODIMP Next(__in ULONG celt, __out VARIANT *rgVar, __out_opt ULONG *pCeltFetched);
  STDMETHODIMP Skip(__in ULONG celt);
  STDMETHODIMP Reset();
  STDMETHODIMP Clone(__deref_out IEnumVARIANT **ppEnum);

private:
  TNktArrayListWithRelease<IDispatch*> aItemsList;
  SIZE_T nIndex;
};
