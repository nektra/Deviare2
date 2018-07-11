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

#pragma once

#include "resource.h"       // main symbols
#include "CustomRegistryMap.h"
#include "DeviareLiteCOM_h.h"
#include "DotNetCoreHooks.h"
#include <NktHookLib.h>

//-----------------------------------------------------------

class CDeviareLiteCOMModule : public CAtlDllModuleT<CDeviareLiteCOMModule>
{
public:
  CDeviareLiteCOMModule();
  ~CDeviareLiteCOMModule();

  DECLARE_LIBID(LIBID_DeviareLite)
#ifdef _WIN64
  DECLARE_REGISTRY_APPID_RESOURCEID(IDR_DEVIARELITECOM64, "{7F65AF61-32C2-4f4e-9B91-7C32910503FD}")
#else //_WIN64
  DECLARE_REGISTRY_APPID_RESOURCEID(IDR_DEVIARELITECOM, "{7F65AF62-32C2-4f4e-9B91-7C32910503FD}")
#endif //_WIN64
};

//-----------------------------------------------------------

extern class CDeviareLiteCOMModule _AtlModule;
extern HINSTANCE hDllInst;
