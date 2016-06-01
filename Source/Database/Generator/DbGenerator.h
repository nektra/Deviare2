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

#ifndef NKT_DV_DBGENERATOR
  #error NKT_DV_DBGENERATOR not defined
#endif //!NKT_DV_DBGENERATOR

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "XmlHelpers.h"
#include "..\..\SupportLibs\SqLite\SQLiteConnector.h"
#include "..\..\Common\StringLiteW.h"
#include "..\..\Common\ArrayList.h"
#include "..\..\Common\ObjectEnums.h"

//-----------------------------------------------------------

#define ARRAY_LEN(x)                (sizeof(x)/sizeof(x[0]))

//-----------------------------------------------------------

//88D96A05-F192-11D4-A65F-0040963251E5
extern const GUID CLSID_DOMDocument60;
extern const GUID IID_IXMLDOMDocument3;
extern ULONG nProcessingBits;
extern CNktSQLiteConnector cDB;
extern CXmlHelper cXmlHelper;
extern TNktComPtr<IXMLDOMDocument3> cXmlDoc;
extern TNktComPtr<IXMLDOMElement> cXmlRootElem;
extern TNktArrayListWithFree<LPWSTR,512> aFunctionsList;

//-----------------------------------------------------------

IXMLDOMDocument3* LoadXML(LPCWSTR szSrcFileNameW);

VOID PrintAndLogNoMemoryError();
VOID PrintAndLog(LPCWSTR szFormatW, ...);
VOID PrintAndLogSeparator();
VOID LogOnly(LPCWSTR szFormatW, ...);

BOOL IsAbortKeyPressed();

//-----------------------------------------------------------

