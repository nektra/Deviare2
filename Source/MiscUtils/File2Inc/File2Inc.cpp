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

#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <tchar.h>

//-----------------------------------------------------------

#define FILETYPE_Bin                                       1
#define FILETYPE_Obj                                       2

//-----------------------------------------------------------

int _tmain(int argc, _TCHAR* argv[])
{
  FILE *fp;
  union {
    LPBYTE lpFileData;
    IMAGE_FILE_HEADER *lpFileHdr;
  };
  IMAGE_SECTION_HEADER *lpSectionHdr;
  WORD wSectIdx;
  int i, j;
  long nFileSize, nFileType;
  char szVarNameA[64];
  LPBYTE lpPtr;
  DWORD dw, dwDataSize;

  nFileType = FILETYPE_Bin;
  strcpy_s(szVarNameA, 64, "data");
  if (argc < 3)
  {
    _tprintf(_T("Use: FILE2INC input-filename output-filename [/var:var-name] [/type:(bin|obj)]\n"));
    return 1;
  }
  for (j=1; j<3; j++) {
    if (argv[j][0] == '/' || argv[j][0] == '-')
    {
err_inv_param:
      _tprintf(_T("Error: Invalid command line\n"));
      return 1;
    }
  }
  for (; j<argc; j++) {
    if (argv[j][0] != '/' && argv[j][0] != '-')
      goto err_inv_param;
    if (_tcsnicmp(argv[j]+1, _T("var:"), 4) == 0)
    {
      for (i=5; i<68 && argv[j][i]!=0; i++)
      {
        if ((argv[j][i] >= _T('A') && argv[j][i] <= _T('Z')) ||
            (argv[j][i] >= _T('a') && argv[j][i] <= _T('z')) ||
            argv[j][i] == _T('_') ||
            (i > 5 && argv[j][i] >= _T('0') && argv[j][i] <= _T('9')))
        {
          szVarNameA[i-5] = (char)argv[j][i];
        }
        else
        {
err_inv_varname:
          _tprintf(_T("Error: Invalid variable name\n"));
          return 1;
        }
      }
      if (i >= 64)
        goto err_inv_varname;
      szVarNameA[i-5] = 0;
    }
    else if (_tcsnicmp(argv[j]+1, _T("type:"), 5) == 0)
    {
      if (_tcsicmp(argv[j]+6, _T("bin")) == 0)
        nFileType = FILETYPE_Bin;
      else if (_tcsicmp(argv[j]+6, _T("obj")) == 0)
        nFileType = FILETYPE_Obj;
      else
      {
        _tprintf(_T("Error: Invalid file type\n"));
        return 1;
      }
    }
    else
      goto err_inv_param;
  }
  //----
  if (_tfopen_s(&fp, argv[1], _T("rb")) != 0)
  {
    _tprintf(_T("Error: Can't open file '%s'\n"), argv[1]);
    return 1;
  }
  fseek(fp, 0, SEEK_END);
  nFileSize = ftell(fp);
  rewind(fp);
  //----
  lpFileData = (LPBYTE)malloc(nFileSize);
  if (lpFileData == NULL)
  {
    fclose(fp);
    _tprintf(_T("Error: Not enough memory\n"));
    return 1;
  }
  if (fread(lpFileData, nFileSize, 1, fp) != 1)
  {
    free(lpFileData);
    fclose(fp);
    _tprintf(_T("Error: Can't read file '%s'\n"), argv[1]);
    return 1;
  }
  fclose(fp);
  //----
  switch (nFileType)
  {
    case FILETYPE_Bin:
      lpPtr = lpFileData;
      dwDataSize = (DWORD)nFileSize;
      break;

    case FILETYPE_Obj:
      lpSectionHdr = (IMAGE_SECTION_HEADER*)(((char*)(lpFileHdr + 1)) + lpFileHdr->SizeOfOptionalHeader);
      for (wSectIdx=0; wSectIdx<lpFileHdr->NumberOfSections; wSectIdx++)
      {
        if (strncmp((char*)(lpSectionHdr[wSectIdx].Name), ".text", 5) == 0 &&
            (lpSectionHdr[wSectIdx].Name[5] == 0 || lpSectionHdr[wSectIdx].Name[5] == '$' ||
             lpSectionHdr[wSectIdx].Name[5] == '!') &&
            lpSectionHdr[wSectIdx].SizeOfRawData > 0)
          break;
      }
      if (wSectIdx >= lpFileHdr->NumberOfSections)
      {
        free(lpFileData);
        _tprintf(_T("Error: .text section not found\n"));
        return 1;
      }
      lpPtr = (LPBYTE)lpFileHdr + lpSectionHdr[wSectIdx].PointerToRawData;
      dwDataSize = lpSectionHdr[wSectIdx].SizeOfRawData;
      break;
  }
  //----
  if (_tfopen_s(&fp, argv[2], _T("wt")) != 0)
  {
    free(lpFileHdr);
    _tprintf(_T("Error: Can't create file '%s'\n"), argv[2]);
    return 1;
  }
  if (fprintf(fp, "BYTE %s[%lu] = {\n", szVarNameA, dwDataSize) == 0)
  {
err_cantwrite:
    free(lpFileData);
    fclose(fp);
    _tprintf(_T("Error: Can't write file '%s'\n"), argv[2]);
    return -2;
  }
  for (dw=0; dw<dwDataSize; dw++)
  {
    if ((dw & 3) == 0)
    {
      if (dw > 0)
      {
        if (fprintf(fp, "\n") == 0)
          goto err_cantwrite;
      }
      if (fprintf(fp, " ") == 0)
        goto err_cantwrite;
    }
    //----
    if (fprintf(fp, " X_BYTE_ENC(0x%02X,0x%04X)", lpPtr[dw], dw) == 0)
      goto err_cantwrite;
    //----
    if (dw < dwDataSize-1)
    {
      if (fprintf(fp, ",") == 0)
        goto err_cantwrite;
    }
  }
  if (fprintf(fp, "\n};\n") == 0)
    goto err_cantwrite;
  free(lpFileData);
  fclose(fp);
  _tprintf(_T("Done!\n"));
  return 0;
}
