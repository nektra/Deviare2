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

#include "Base64.h"
#include "AutoPtr.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

static const char cd64[] = "|$$$}rstuvwxyz{$$$=$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]"
                           "^_`abcdefghijklmnopq";

//-----------------------------------------------------------

HRESULT nktBase64Decrypt(__deref_out LPBYTE *lpBuffer, __out SIZE_T *lpnBufferSize,
                         __in_z LPCWSTR szStringW, __in SIZE_T nStrLen)
{
  SIZE_T nDecLen, nIdx, nEqualCounter;
  TNktAutoFreePtr<BYTE> aDecData;
  unsigned short k;
  BYTE v, nTempValue[4], *d;

  if (lpBuffer != NULL)
    *lpBuffer = NULL;
  if (lpnBufferSize != NULL)
    *lpnBufferSize = 0;
  if (szStringW == NULL || lpBuffer == NULL || lpnBufferSize == NULL)
    return E_POINTER;
  //calculate estimated final size and allocate memory
  if (nStrLen == NKT_SIZE_T_MAX)
    nStrLen = wcslen(szStringW);
  nDecLen = (nStrLen / 4) * 3 + 16;
  aDecData.Attach(d = (LPBYTE)nktMemMalloc(nDecLen));
  if (aDecData == NULL)
    return E_OUTOFMEMORY;
  //decode
  nIdx = nEqualCounter = 0;
  nDecLen = 0;
  while (nStrLen > 0 && szStringW[0] != 0)
  {
    k = (unsigned short)szStringW[0];
    szStringW++;
    nStrLen--;
    //decode char
    v = (k<43 || k>122) ? (BYTE)'$' : (BYTE)cd64[k-43];
    if (v == (BYTE)'=') {
      if ((++nEqualCounter) > 2)
        return E_FAIL;
      continue; //just ignore char
    }
    if (v == (BYTE)'$')
      continue; //just ignore char
    //reset 'equal signs' that where in the middle
    nEqualCounter = 0;
    nTempValue[nIdx++] = v-62;
    if (nIdx >= 4)
    {
      d[0] = ( (nTempValue[0] << 2)         | (nTempValue[1] >> 4));
      d[1] = ( (nTempValue[1] << 4)         | (nTempValue[2] >> 2));
      d[2] = (((nTempValue[2] << 6) & 0xC0) |  nTempValue[3]);
      d += 3;
      nDecLen += 3;
      nIdx = 0;
    }
  }
  if (nIdx > 1) //if only one char => ignore
  {
    nktMemSet(nTempValue+nIdx, 0, 4-nIdx);
    d[0] = ( (nTempValue[0] << 2)         | (nTempValue[1] >> 4));
    if (nIdx > 2)
      d[1] = ( (nTempValue[1] << 4)         | (nTempValue[2] >> 2));
    nDecLen += (nIdx-1);
  }
  *lpBuffer = aDecData.Detach();
  *lpnBufferSize = nDecLen;
  return S_OK;
}
