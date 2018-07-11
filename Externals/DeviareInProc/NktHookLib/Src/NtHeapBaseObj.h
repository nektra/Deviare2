/*
 * Copyright (C) 2010-2015 Nektra S.A., Buenos Aires, Argentina.
 * All rights reserved. Contact: http://www.nektra.com
 *
 *
 * This file is part of Deviare In-Proc
 *
 *
 * Commercial License Usage
 * ------------------------
 * Licensees holding valid commercial Deviare In-Proc licenses may use this
 * file in accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Nektra.  For licensing terms and
 * conditions see http://www.nektra.com/licensing/.  For further information
 * use the contact form at http://www.nektra.com/contact/.
 *
 *
 * GNU General Public License Usage
 * --------------------------------
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 *
 **/

#ifndef _NKT_HOOKLIB_NTHEAPBASEOBJ_H
#define _NKT_HOOKLIB_NTHEAPBASEOBJ_H

#include <NktHookLib.h>

namespace NktHookLib {
namespace Internals {

//-----------------------------------------------------------

#ifndef NKT_ASSERT
  #ifdef _DEBUG
    #define NKT_ASSERT(x) if (!(x)) __debugbreak();
  #else
    #define NKT_ASSERT(x)
  #endif //_DEBUG
#endif //!NKT_ASSERT


class CNktNtHeapBaseObj
{
public:
  void* __cdecl operator new(__in size_t nSize)
    {
    return NktHookLibHelpers::MemAlloc(nSize);
    };
  void* __cdecl operator new[](__in size_t nSize)
    {
    return NktHookLibHelpers::MemAlloc(nSize);
    };
  void* __cdecl operator new(__in size_t nSize, __inout void* lpInPlace)
    {
    return lpInPlace;
    };

  void __cdecl operator delete(__inout void* p)
    {
    NktHookLibHelpers::MemFree(p);
    return;
    };
  void __cdecl operator delete[](__inout void* p)
    {
    NktHookLibHelpers::MemFree(p);
    return;
    };
#if _MSC_VER >= 1200
  void __cdecl operator delete(__inout void* p, __inout void* lpPlace)
    {
    return;
    };
#endif //_MSC_VER >= 1200
};

//-----------------------------------------------------------

} //Internals
} //NktHookLib

#endif //_NKT_HOOKLIB_NTHEAPBASEOBJ_H
