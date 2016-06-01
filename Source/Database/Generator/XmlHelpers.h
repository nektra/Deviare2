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

#include <windows.h>
#include <ole2.h>
#include "..\..\Common\ComPtr.h"
#include "..\..\Common\MemoryManager.h"
#include "..\..\Common\msxml\msxml6.h"
#include "..\..\Common\ObjectEnums.h"

//-----------------------------------------------------------

#define SPECIAL_DBOBJ_CLASS_Field                       1001
#define SPECIAL_DBOBJ_CLASS_Variable                    1002

//-----------------------------------------------------------

class CXmlHelper : public CNktMemMgrObj
{
public:
  CXmlHelper();
  ~CXmlHelper();

  VOID Reset();
  HRESULT Initialize(IXMLDOMElement *lpRootElem);

  LPWSTR GetIdFromFundamentalType(SIZE_T nFundType);
  IXMLDOMElement* GetElementFromFundamentalType(SIZE_T nFundType);

  IXMLDOMElement* GetElementFromIndex(SIZE_T nIndex);
  LPWSTR GetElementIdFromIndex(SIZE_T nIndex);

  IXMLDOMElement* GetElementFromId(ULONG nId, BOOL bIsConstant, BOOL bIsVolatile);
  IXMLDOMElement* GetElementFromId(LPCWSTR szIdW);

  LONG GetFundamentalTypeFromId(ULONG nId);

  static LONG GetDbObjectClass(IXMLDOMElement *lpElem, LONG nType2Check);

  static HRESULT GetFirstChildElement(IXMLDOMElement *lpElem, IXMLDOMElement **lplpFirstElem);
  static HRESULT GetNextElement(IXMLDOMElement *lpElem, IXMLDOMElement **lplpNextElem);
  static HRESULT GetElementName(IXMLDOMElement *lpElem, BSTR *lpbStrDest);
  static HRESULT GetElementText(IXMLDOMElement *lpElem, BSTR *lpbStrDest);
  static HRESULT GetAttributeValue(IXMLDOMElement *lpElem, LPCWSTR szAttrNameW, BSTR *lpbStrDest);

private:
  typedef struct {
    WCHAR szIdW[16];
    IXMLDOMElement *lpElem;
    ULONG nFundamentalType;

  } ID_ELEM_MAP_ENTRY;

  static int IdElemMapEntry_Compare(void *ctx, const void *lp1, const void *lp2);

  ID_ELEM_MAP_ENTRY *lpIdElemMapEntries;
  SIZE_T nIdElemMapEntriesCount, nIdElemMapEntriesSize;
  SIZE_T aFundTypesIndexes[NKT_DBFUNDTYPE_MAX-NKT_DBFUNDTYPE_MIN+1];
};

//-----------------------------------------------------------

