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

#include "RegistrySettings.h"
#include "Debug.h"
#include "TransportCommon.h"
#include "MemoryManagerPool.h"
#include <crtdbg.h>

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

static LONG volatile nAccessLock = 0;
static LONG volatile nInitialized = 0;
static DWORD dwDebugLevelMask = DEFAULT_DBGLEVEL_MASK;

static DWORD dwMemMgrPoolMaxAge = NKT_MEMMGRPOOL_MAXAGE;
static DWORD dwMemMgrPoolCheckFrequency = NKT_MEMMGRPOOL_CHECK_FREQUENCY;

static DWORD dwMaxFreeBufferList = NKT_DV_TRANSPORT_MAX_FREEBUFFERLIST;
static DWORD dwMaxInUseMessageCount_SpyMgr = NKT_DV_TRANSPORT_MAX_INUSE_MESSAGES_SPYMGR;
static DWORD dwMaxInUseMessageDelay_SpyMgr = NKT_DV_TRANSPORT_MAX_INUSE_MESSAGES_DELAY;
static DWORD dwMaxInUseMessageCount_Agent = NKT_DV_TRANSPORT_MAX_INUSE_MESSAGES_AGENT;
static DWORD dwMaxInUseMessageDelay_Agent = NKT_DV_TRANSPORT_MAX_INUSE_MESSAGES_DELAY;
static DWORD dwAgentLoadTimeout = 10000;

//-----------------------------------------------------------

static VOID InitRegistrySettings();
static BOOL RegistryGetDWORD(__out DWORD *lpdw, __in HKEY hKey, __in LPCWSTR szKeyW);

#define CHECK_INITREGISTRYSETTINGS() if (nInitialized == 0) InitRegistrySettings()

//-----------------------------------------------------------

DWORD CNktDvRegistrySettings::GetDebugLevelMask()
{
  CHECK_INITREGISTRYSETTINGS();
  return dwDebugLevelMask;
}

VOID CNktDvRegistrySettings::SetDebugLevelMask(__in DWORD dwValue)
{
  CHECK_INITREGISTRYSETTINGS();
  dwDebugLevelMask = dwValue | DEFAULT_DBGLEVEL_MASK;
  return;
}

DWORD CNktDvRegistrySettings::GetMemMgrPoolMaxAge()
{
  CHECK_INITREGISTRYSETTINGS();
  return dwMemMgrPoolMaxAge;
}

VOID CNktDvRegistrySettings::SetMemMgrPoolMaxAge(__in DWORD dwValue)
{
  CHECK_INITREGISTRYSETTINGS();
  dwMemMgrPoolMaxAge = dwValue;
  return;
}

DWORD CNktDvRegistrySettings::GetMemMgrPoolCheckFrequency()
{
  CHECK_INITREGISTRYSETTINGS();
  return dwMemMgrPoolCheckFrequency;
}

VOID CNktDvRegistrySettings::SetMemMgrPoolCheckFrequency(__in DWORD dwValue)
{
  CHECK_INITREGISTRYSETTINGS();
  if (dwValue < 10)
    dwValue = 10;
  dwMemMgrPoolCheckFrequency = dwValue;
  return;
}

LONG CNktDvRegistrySettings::GetTransportMaxFreeBufferList()
{
  CHECK_INITREGISTRYSETTINGS();
  return (LONG)dwMaxFreeBufferList;
}

VOID CNktDvRegistrySettings::SetTransportMaxFreeBufferList(__in DWORD dwValue)
{
  CHECK_INITREGISTRYSETTINGS();
  if (dwValue > 1048576)
    dwValue = 1048576;
  dwMaxFreeBufferList = dwValue;
  return;
}

LONG CNktDvRegistrySettings::GetSpyMgrTransportMaxInUseMessageCount()
{
  CHECK_INITREGISTRYSETTINGS();
  return (LONG)dwMaxInUseMessageCount_SpyMgr;
}

VOID CNktDvRegistrySettings::SetSpyMgrTransportMaxInUseMessageCount(__in LONG nValue)
{
  CHECK_INITREGISTRYSETTINGS();
  if (nValue > 1048576)
    nValue = 1048576;
  else if (nValue < 0)
    nValue = 0;
  dwMaxInUseMessageCount_SpyMgr = (DWORD)nValue;
  return;
}

DWORD CNktDvRegistrySettings::GetSpyMgrTransportMessageDelay()
{
  CHECK_INITREGISTRYSETTINGS();
  return dwMaxInUseMessageDelay_SpyMgr;
}

VOID CNktDvRegistrySettings::SetSpyMgrTransportMessageDelay(__in DWORD dwValue)
{
  CHECK_INITREGISTRYSETTINGS();
  if (dwValue < 5)
    dwValue = 5;
  else if (dwValue > 30000)
    dwValue = 30000;
  dwMaxInUseMessageDelay_SpyMgr = dwValue;
  return;
}

LONG CNktDvRegistrySettings::GetAgentTransportMaxInUseMessageCount()
{
  CHECK_INITREGISTRYSETTINGS();
  return (LONG)dwMaxInUseMessageCount_Agent;
}

VOID CNktDvRegistrySettings::SetAgentTransportMaxInUseMessageCount(__in LONG nValue)
{
  CHECK_INITREGISTRYSETTINGS();
  if (nValue > 1048576)
    nValue = 1048576;
  else if (nValue < 0)
    nValue = 0;
  dwMaxInUseMessageCount_Agent = (DWORD)nValue;
  return;
}

DWORD CNktDvRegistrySettings::GetAgentTransportMessageDelay()
{
  CHECK_INITREGISTRYSETTINGS();
  return dwMaxInUseMessageDelay_Agent;
}

VOID CNktDvRegistrySettings::SetAgentTransportMessageDelay(__in DWORD dwValue)
{
  CHECK_INITREGISTRYSETTINGS();
  if (dwValue < 5)
    dwValue = 5;
  else if (dwValue > 30000)
    dwValue = 30000;
  dwMaxInUseMessageDelay_Agent = dwValue;
  return;
}

DWORD CNktDvRegistrySettings::GetAgentLoadTimeout()
{
  CHECK_INITREGISTRYSETTINGS();
  return dwAgentLoadTimeout;
}

VOID CNktDvRegistrySettings::SetAgentLoadTimeout(__in DWORD dwValue)
{
  CHECK_INITREGISTRYSETTINGS();
  if (dwValue < 500)
    dwValue = 500;
  else if (dwValue > 60*60*1000)
    dwValue = 60*60*1000;
  dwAgentLoadTimeout = dwValue;
  return;
}

//-----------------------------------------------------------

static VOID InitRegistrySettings()
{
  CNktSimpleLockNonReentrant cAccessLock(&nAccessLock);
  static const LPCWSTR szKeyW = L"Software\\Nektra\\Deviare2";
  DWORD dwValue;
  HKEY hKey;

  if (nInitialized == 0)
  {
    if (::RegOpenKeyExW(HKEY_LOCAL_MACHINE, szKeyW, 0, KEY_READ|KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS ||
        ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, szKeyW, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
      if (RegistryGetDWORD(&dwValue, hKey, L"DebugLevelMask") != FALSE)
        dwDebugLevelMask = (dwValue | DEFAULT_DBGLEVEL_MASK);
      //----
      if (RegistryGetDWORD(&dwValue, hKey, L"MemMgrPoolMaxAge") != FALSE)
        dwMemMgrPoolMaxAge = dwValue;
      if (RegistryGetDWORD(&dwValue, hKey, L"MemMgrPoolCheckFrequency") != FALSE)
        dwMemMgrPoolCheckFrequency = dwValue;
      if (dwMemMgrPoolCheckFrequency < 10)
        dwMemMgrPoolCheckFrequency = 10;
      //----
      if (RegistryGetDWORD(&dwValue, hKey, L"MaxTransportFreeListMessageCount") != FALSE)
        dwMaxFreeBufferList = dwValue;
      if (RegistryGetDWORD(&dwValue, hKey, L"SpyMgrMaxTransportMessageCount") != FALSE)
        dwMaxInUseMessageCount_SpyMgr = dwValue;
      if (RegistryGetDWORD(&dwValue, hKey, L"SpyMgrTransportMessageDelay") != FALSE)
        dwMaxInUseMessageDelay_SpyMgr = dwValue;
      if (RegistryGetDWORD(&dwValue, hKey, L"AgentMaxTransportMessageCount") != FALSE)
        dwMaxInUseMessageCount_Agent = dwValue;
      if (RegistryGetDWORD(&dwValue, hKey, L"AgentTransportMessageDelay") != FALSE)
        dwMaxInUseMessageDelay_Agent = dwValue;
      //----
      if (RegistryGetDWORD(&dwValue, hKey, L"AgentLoadTimeout") != FALSE)
        dwAgentLoadTimeout = dwValue;
      //----
      if (dwMaxFreeBufferList > 1048576)
        dwMaxFreeBufferList = 1048576;
      if (dwMaxInUseMessageCount_SpyMgr > 1048576)
        dwMaxInUseMessageCount_SpyMgr = 1048576;
      if (dwMaxInUseMessageCount_Agent > 1048576)
        dwMaxInUseMessageCount_Agent = 1048576;
      if (dwMaxInUseMessageDelay_SpyMgr < 5)
        dwMaxInUseMessageDelay_SpyMgr = 5;
      else if (dwMaxInUseMessageDelay_SpyMgr > 30000)
        dwMaxInUseMessageDelay_SpyMgr = 30000;
      if (dwMaxInUseMessageDelay_Agent < 5)
        dwMaxInUseMessageDelay_Agent = 5;
      else if (dwMaxInUseMessageDelay_Agent > 30000)
        dwMaxInUseMessageDelay_Agent = 30000;
      if (dwAgentLoadTimeout < 500)
        dwAgentLoadTimeout = 500;
      else if (dwAgentLoadTimeout > 60*60*1000)
        dwAgentLoadTimeout = 60*60*1000;
      //----
      ::RegCloseKey(hKey);
    }
    NktInterlockedExchange(&nInitialized, 1);
  }
  return;
}

static BOOL RegistryGetDWORD(__out DWORD *lpdw, __in HKEY hKey, __in LPCWSTR szKeyW)
{
  DWORD dwValue, dwType, dwSize;

  dwSize = sizeof(dwValue);
  if (::RegQueryValueExW(hKey, szKeyW, NULL, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS &&
      dwType == REG_DWORD)
  {
    *lpdw = dwValue;
    return TRUE;
  }
  return FALSE;
}
