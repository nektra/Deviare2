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

//-----------------------------------------------------------

#define DEVIARECOM_MajorVersion                            2
#define DEVIARECOM_MinorVersion                            0

#ifndef STRICT
  #define STRICT
#endif

#if (defined(_WIN32) && !defined(WIN32))
  #define WIN32
#endif //_WIN32 && !WIN32

#if (defined(WIN32) && !defined(_WIN32))
  #define _WIN32
#endif //WIN32 && !_WIN32

#if (defined(_WIN64) && !defined(WIN64))
  #define WIN64
#endif //_WIN64 && !WIN64

#if (defined(WIN64) && !defined(_WIN64))
  #define _WIN64
#endif //WIN64 && !_WIN64

#include "TargetVer.h"

#define _ATL_FREE_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_SUPPORT_VT_I8
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_NO_VARIANT_THROW

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <statreg.h>

#include "..\SpyMgr\SpyMgr.h"
#include "..\..\Common\WaitableObjects.h"
#include "..\..\Common\Tools.h"

//-----------------------------------------------------------

#if defined _M_IX86
  #ifdef _DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib_Debug.lib")
    #pragma comment(lib, "..\\..\\..\\Libs\\SpyMgrLib_Debug.lib")
  #else //_DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib.lib")
    #pragma comment(lib, "..\\..\\..\\Libs\\SpyMgrLib.lib")
  #endif //_DEBUG
#elif defined _M_X64
  #ifdef _DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib64_Debug.lib")
    #pragma comment(lib, "..\\..\\..\\Libs\\SpyMgrLib64_Debug.lib")
  #else //_DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib64.lib")
    #pragma comment(lib, "..\\..\\..\\Libs\\SpyMgrLib64.lib")
  #endif //_DEBUG
#else
  #error Unsupported platform
#endif

//-----------------------------------------------------------

using namespace ATL;
