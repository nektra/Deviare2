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

#include "EngBaseObj.h"

//-----------------------------------------------------------

class CNktDvRegistrySettings
{
public:
  static DWORD GetDebugLevelMask();
  static VOID SetDebugLevelMask(__in DWORD dwValue);

  static DWORD GetMemMgrPoolMaxAge();
  static VOID SetMemMgrPoolMaxAge(__in DWORD dwValue);

  static DWORD GetMemMgrPoolCheckFrequency();
  static VOID SetMemMgrPoolCheckFrequency(__in DWORD dwValue);

  static LONG GetTransportMaxFreeBufferList();
  static VOID SetTransportMaxFreeBufferList(__in DWORD dwValue);

  static LONG GetSpyMgrTransportMaxInUseMessageCount();
  static VOID SetSpyMgrTransportMaxInUseMessageCount(__in LONG nValue);
  static DWORD GetSpyMgrTransportMessageDelay();
  static VOID SetSpyMgrTransportMessageDelay(__in DWORD dwValue);

  static LONG GetAgentTransportMaxInUseMessageCount();
  static VOID SetAgentTransportMaxInUseMessageCount(__in LONG nValue);
  static DWORD GetAgentTransportMessageDelay();
  static VOID SetAgentTransportMessageDelay(__in DWORD dwValue);

  static DWORD GetAgentLoadTimeout();
  static VOID SetAgentLoadTimeout(__in DWORD dwValue);
};

//-----------------------------------------------------------

