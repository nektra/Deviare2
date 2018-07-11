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

#include <atlbase.h>

//-----------------------------------------------------------

#define DECLARE_REGISTRY_RESOURCEID_EX(x, szProgId, szVersion, szDescription, rClsId, \
                                       rLibId, szThreadingModel)                      \
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)                                \
  {                                                                                   \
    struct _ATL_REGMAP_ENTRY aMapEntries[7];                                          \
    WCHAR szClsId[40], szLibId[40];                                                   \
                                                                                      \
    memset(&aMapEntries[6], 0, sizeof(aMapEntries[6]));                               \
    aMapEntries[0].szKey = L"PROGID";                                                 \
    aMapEntries[0].szData = szProgId;                                                 \
    aMapEntries[1].szKey = L"VERSION";                                                \
    aMapEntries[1].szData = szVersion;                                                \
    aMapEntries[2].szKey = L"DESCRIPTION";                                            \
    aMapEntries[2].szData = szDescription;                                            \
    aMapEntries[3].szKey = L"CLSID";                                                  \
    ::StringFromGUID2(rClsId, szClsId, 40);                                           \
    aMapEntries[3].szData = szClsId;                                                  \
    aMapEntries[4].szKey = L"LIBID";                                                  \
    ::StringFromGUID2(rLibId, szLibId, 40);                                           \
    aMapEntries[4].szData = szLibId;                                                  \
    aMapEntries[5].szKey = L"THREADING";                                              \
    aMapEntries[5].szData = szThreadingModel;                                         \
    __if_exists(_Module)                                                              \
      {                                                                               \
      return _Module.UpdateRegistryFromResource(x, bRegister, aMapEntries);           \
      }                                                                               \
      __if_not_exists(_Module)                                                        \
      {                                                                               \
      return ATL::_pAtlModule->UpdateRegistryFromResource(x, bRegister, aMapEntries); \
      }                                                                               \
  };
