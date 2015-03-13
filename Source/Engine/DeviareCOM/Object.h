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
// CDeviareObj

class ATL_NO_VTABLE CNktObjectImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                     public CComCoClass<CNktObjectImpl, &CLSID_NktSpyMgr>,
                                     public CNktMemMgrObj,
                                     public IObjectSafetyImpl<CNktObjectImpl,
                                                              INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                     public CNktObjectBaseImpl<CNktObjectImpl, INktObject,
                                                               CNktDvEngBaseObj, &CLSID_NktObject,
                                                               &IID_INktObject, &LIBID_Deviare2>,
                                     public IInterfaceItemImpl<CNktObjectImpl>,
                                     public INktDvEngBaseComLink
{
public:
  CNktObjectImpl(__in CNktDvEngBaseObj *lpIntObj) : CComObjectRootEx<CComMultiThreadModel>(),
                     CComCoClass<CNktObjectImpl, &CLSID_NktSpyMgr>(),
                     CNktMemMgrObj(),
                     IObjectSafetyImpl<CNktObjectImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                     CNktObjectBaseImpl<CNktObjectImpl, INktObject, CNktDvEngBaseObj, &CLSID_NktObject,
                                        &IID_INktObject, &LIBID_Deviare2>(lpIntObj),
                     IInterfaceItemImpl<CNktObjectImpl>(),
                     INktDvEngBaseComLink()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktObject",
                                 L"1", L"NktObject Class", CLSID_NktObject,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktObjectImpl)
    COM_INTERFACE_ENTRY(INktObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY(INktDvEngBaseComLink)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  OBJBASE_DECLARE_APIS_1(CNktObjectImpl,CNktDvEngBaseObj)

private:
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktObject), CNktObjectImpl)
