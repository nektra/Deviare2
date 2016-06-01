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

#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif //!WIN32_LEAN_AND_MEAN
#ifndef WINVER
  #define WINVER 0x0500
#endif //!WINVER
#ifndef _WIN32_WINNT
#ifndef _M_X64
  #define _WIN32_WINNT 0x0500
#else
  #define _WIN32_WINNT 0x0501
#endif
#endif //!_WIN32_WINN
#ifndef _WIN32_WINDOWS
  #define _WIN32_WINDOWS 0x0510
#endif //!_WIN32_WINDOWS
#ifndef _WIN32_IE
  #define _WIN32_IE 0x0700
#endif //!_WIN32_IE


#include <windows.h>
#include "MemoryManager.h"

//-----------------------------------------------------------

#ifndef NKT_SIZE_T_MAX
  #define NKT_SIZE_T_MAX  ((SIZE_T)(-1))
#endif //NKT_SIZE_T_MAX

#ifndef NKT_DV_ARRAYLEN
  #define NKT_DV_ARRAYLEN(x)        (sizeof(x)/sizeof(x[0]))
#endif //NKT_DV_ARRAYLEN

#define NKT_DV_ALIGN(x, _size)                             \
                        (((x)+((_size)-1)) & (~((_size)-1)))

#define NKT_DV_ALIGN_ULONG(x)                              \
                     NKT_DV_ALIGN((ULONG)(x), sizeof(ULONG))

#define NKT_DV_ALIGN_ULONGLONG(x)                          \
             NKT_DV_ALIGN((ULONGLONG)(x), sizeof(ULONGLONG))

#if defined _M_IX86
  #define NKT_DV_ALIGN_SIZE_T(x)       NKT_DV_ALIGN_ULONG(x)
#elif defined _M_X64
  #define NKT_DV_ALIGN_SIZE_T(x)   NKT_DV_ALIGN_ULONGLONG(x)
#else
  #error Unsupported platform
#endif //_M_X64

//-----------------------------------------------------------

#define NKT_DV_DELETE(x)                  if ((x) != NULL) \
                                 { delete (x); (x) = NULL; }
#define NKT_DV_RELEASE(x)                 if ((x) != NULL) \
                             { (x)->Release(); (x) = NULL; }

//-----------------------------------------------------------

__inline HRESULT NKT_HRESULT_FROM_WIN32(DWORD dwOsErr)
{
  if (dwOsErr == ERROR_NOT_ENOUGH_MEMORY)
    dwOsErr = ERROR_OUTOFMEMORY;
  return HRESULT_FROM_WIN32(dwOsErr);
}

__inline HRESULT NKT_HRESULT_FROM_LASTERROR()
{
  return NKT_HRESULT_FROM_WIN32(::GetLastError());
}

//-----------------------------------------------------------

#define NKT_PTR_2_ULONGLONG(ptr)  ((ULONGLONG)(SIZE_T)(ptr))

//-----------------------------------------------------------

template <typename varType>
__inline int NKT_DV_CMP(__in varType x, __in varType y)
{
  if (x < y)
    return -1;
  if (x > y)
    return 1;
  return 0;
}

//-----------------------------------------------------------

#include "EngErrorCodes.h"

//-----------------------------------------------------------

