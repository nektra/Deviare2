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

#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <tchar.h>

//-----------------------------------------------------------

#define CODESECT_CHARACTERISTICS (IMAGE_SCN_CNT_CODE|IMAGE_SCN_MEM_EXECUTE|IMAGE_SCN_MEM_READ)

//-----------------------------------------------------------

int wmain(__in int argc, __in LPWSTR argv[])
{
  FILE *fp;
  union {
    IMAGE_FILE_HEADER *lpFileHdr;
    ANON_OBJECT_HEADER_BIGOBJ *lpBigFileHdr;
  };
  DWORD dw, dwDataSize, dwValue, dwSection, dwSectionsCount, dwSymbolsCount, dwSkip;
  IMAGE_SECTION_HEADER *lpSectionsHdr;
  union {
    IMAGE_SYMBOL *lpSymbols;
    IMAGE_SYMBOL_EX *lpSymbolsEx;
  };
  SIZE_T i, j, nLen, nFileSize, nRetCode, nSectCounter, nInputFileParam, nOutputFileParam;
  CHAR szVarNameA[64], *sA;
  LPBYTE lpPtr;
  BOOL bIsBigCoff;

  //parse parameters
  nInputFileParam = nOutputFileParam = 0;
  szVarNameA[0] = 0;
  if (argc < 2)
  {
    wprintf(L"Use: OBJ2INC input-filename /out output-filename [/var var-name]\n");
    return 1;
  }
  for (j=1; j<(SIZE_T)argc; j++)
  {
    if (argv[j][0] != '/' && argv[j][0] != '-')
    {
      if (nInputFileParam != 0)
      {
        wprintf(L"Error: Input filename already specified.\n");
        return 1;
      }
      nInputFileParam = j;
    }
    else
    {
      if (_wcsicmp(argv[j]+1, L"out") == 0)
      {
        if (nOutputFileParam != 0)
        {
          wprintf(L"Error: Output filename already specified.\n");
          return 1;
        }
        if ((++j) >= (SIZE_T)argc)
          goto err_invalid_cmdline;
        nOutputFileParam = j;
      }
      else if (_wcsicmp(argv[j]+1, L"var") == 0)
      {
        if (szVarNameA[0] != 0)
        {
          wprintf(L"Error: Variable name already specified.\n");
          return 1;
        }
        if ((++j) >= (SIZE_T)argc)
          goto err_invalid_cmdline;
        for (i=0; i<_countof(szVarNameA) && argv[j][i]!=0; i++)
        {
          if ((argv[j][i] >= L'A' && argv[j][i] <= L'Z') ||
              (argv[j][i] >= L'a' && argv[j][i] <= L'z') ||
              argv[j][i] == L'_' ||
              (i > 0 && argv[j][i] >= L'0' && argv[j][i] <= L'9'))
          {
            szVarNameA[i] = (CHAR)argv[j][i];
          }
          else
          {
            wprintf(L"Error: Invalid variable name.\n");
            return 1;
          }
        }
        if (i >= _countof(szVarNameA))
        {
          wprintf(L"Error: Variable name too long.\n");
          return 1;
        }
        szVarNameA[i] = 0;
      }
      else
      {
err_invalid_cmdline:
        wprintf(L"Error: Invalid command line.\n");
        return 1;
      }
    }
  }
  if (nInputFileParam == 0)
  {
    wprintf(L"Error: Input filename not specified.\n");
    return 1;
  }
  if (nOutputFileParam == 0)
  {
    wprintf(L"Error: Output filename not specified.\n");
    return 1;
  }
  if (szVarNameA[0] == 0)
    strcpy_s(szVarNameA, _countof(szVarNameA), "data");
  //begin
  fp = NULL;
  lpFileHdr = NULL;
  nRetCode = 1;
  //open and read COFF file
  if (_wfopen_s(&fp, argv[nInputFileParam], L"rb") != 0)
  {
    wprintf(L"Error: Can't open file.\n");
    goto done;
  }
  fseek(fp, 0, SEEK_END);
  nFileSize = (SIZE_T)(ULONG)ftell(fp);
  rewind(fp);
  lpFileHdr = (IMAGE_FILE_HEADER*)malloc(nFileSize);
  if (lpFileHdr == NULL)
  {
    wprintf(L"Error: Not enough memory.\n");
    goto done;
  }
  if (fread(lpFileHdr, nFileSize, 1, fp) != 1)
  {
    wprintf(L"Error: Can't read file.\n");
    goto done;
  }
  fclose(fp);
  fp = NULL;
  //create inline file
  if (_wfopen_s(&fp, argv[nOutputFileParam], L"wt") != 0)
  {
    wprintf(L"Error: Can't create output file.\n");
    goto done;
  }
  //validate COFF and check type
  if (nFileSize >= sizeof(IMAGE_FILE_HEADER) &&
      (lpFileHdr->Machine == IMAGE_FILE_MACHINE_I386 || lpFileHdr->Machine == IMAGE_FILE_MACHINE_AMD64))
  {
    bIsBigCoff = FALSE;
  }
  else if (nFileSize >= sizeof(ANON_OBJECT_HEADER_BIGOBJ) &&
          lpBigFileHdr->Sig1 == IMAGE_FILE_MACHINE_UNKNOWN && lpBigFileHdr->Sig2 == 0xFFFF &&
          lpBigFileHdr->Version >= 2 &&
          (lpBigFileHdr->Machine == IMAGE_FILE_MACHINE_I386 || lpBigFileHdr->Machine == IMAGE_FILE_MACHINE_AMD64))
  {
    bIsBigCoff = TRUE;
  }
  else
  {
err_invalidobjfile:
    wprintf(L"Error: Invalid or unsupported OBJ file.\n");
    goto done;
  }
  dwSymbolsCount = 0;
  lpSymbols = NULL;
  if (bIsBigCoff == FALSE)
  {
    dwSectionsCount = (DWORD)(lpFileHdr->NumberOfSections);
    lpSectionsHdr = (IMAGE_SECTION_HEADER*)(((LPBYTE)(lpFileHdr + 1)) + lpFileHdr->SizeOfOptionalHeader);
    if (lpFileHdr->NumberOfSymbols > 0)
    {
      if (lpFileHdr->PointerToSymbolTable == 0)
        goto err_invalidobjfile;
      dwSymbolsCount = (DWORD)(lpFileHdr->NumberOfSymbols);
      lpSymbols = (IMAGE_SYMBOL*)((LPBYTE)lpFileHdr + (SIZE_T)(lpFileHdr->PointerToSymbolTable));
      //check bounds
      if (((SIZE_T)(&lpSymbols[dwSymbolsCount+1]) - (SIZE_T)lpFileHdr) >= nFileSize)
        goto err_invalidobjfile;
    }
  }
  else
  {
    dwSectionsCount = (DWORD)(lpBigFileHdr->NumberOfSections);
    lpSectionsHdr = (IMAGE_SECTION_HEADER*)(((LPBYTE)(lpBigFileHdr + 1)) + lpBigFileHdr->SizeOfData);
    if (lpBigFileHdr->NumberOfSymbols > 0)
    {
      if (lpBigFileHdr->PointerToSymbolTable == 0)
        goto err_invalidobjfile;
      dwSymbolsCount = (DWORD)(lpBigFileHdr->NumberOfSymbols);
      lpSymbolsEx = (IMAGE_SYMBOL_EX*)((LPBYTE)lpBigFileHdr + (SIZE_T)(lpBigFileHdr->PointerToSymbolTable));
      //check bounds
      if (((SIZE_T)(&lpSymbolsEx[dwSymbolsCount+1]) - (SIZE_T)lpFileHdr) >= nFileSize)
        goto err_invalidobjfile;
    }
  }
  //dump sections
  nSectCounter = 0;
  for (dwSection=0; dwSection<dwSectionsCount; dwSection++)
  {
    //check bounds
    if (((SIZE_T)(&lpSectionsHdr[dwSection+1]) - (SIZE_T)lpFileHdr) >= nFileSize)
      goto err_invalidobjfile;
    //is a code section?
    if ((lpSectionsHdr[dwSection].Characteristics & CODESECT_CHARACTERISTICS) != CODESECT_CHARACTERISTICS ||
        lpSectionsHdr[dwSection].PointerToRawData == NULL ||
        lpSectionsHdr[dwSection].SizeOfRawData == 0)
      continue; //skip

    //dump this section
    nSectCounter++;
    //check bounds
    if ((SIZE_T)(lpSectionsHdr[dwSection].PointerToRawData) >= nFileSize ||
        (SIZE_T)(lpSectionsHdr[dwSection].PointerToRawData + lpSectionsHdr[dwSection].SizeOfRawData) > nFileSize)
      goto err_invalidobjfile;

    //dump bytecode
    lpPtr = (LPBYTE)lpFileHdr + (SIZE_T)(lpSectionsHdr[dwSection].PointerToRawData);
    dwDataSize = lpSectionsHdr[dwSection].SizeOfRawData;
    if (fprintf(fp, "BYTE %s[%lu] = {", szVarNameA, dwDataSize) == 0)
    {
err_cantwrite:
      wprintf(_T("Error: Can't write file\n"));
      goto done;
    }
    for (dw=0; dw<dwDataSize; dw++)
    {
      if ((dw & 15) == 0)
      {
        if (fprintf(fp, "\n   ") == 0)
          goto err_cantwrite;
      }
      //----
      if (fprintf(fp, " 0x%02X%s", lpPtr[dw], (dw < dwDataSize-1) ? "," : "") == 0)
        goto err_cantwrite;
    }
    if (fprintf(fp, "\n};\n") == 0)
      goto err_cantwrite;

    //dump symbols
    for (dw=0; dw<dwSymbolsCount; dw=dw+dwSkip+1)
    {
      if (bIsBigCoff == FALSE)
      {
        dwSkip = (DWORD)(lpSymbols[dw].NumberOfAuxSymbols);
        if (lpSymbols[dw].SectionNumber != dwSection+1)
          continue;
        //if (lpSymbols[dw].StorageClass != IMAGE_SYM_CLASS_EXTERNAL)
        //  continue;
        dwValue = lpSymbols[dw].Value;
        if (lpSymbols[dw].N.Name.Short != 0)
        {
          sA = (LPSTR)(&lpSymbols[dw].N.ShortName);
          for (nLen=0; nLen<8 && sA[nLen]!=0; nLen++);
        }
        else
        {
          sA = (LPSTR)(&lpSymbols[dwSymbolsCount]) + (SIZE_T)(lpSymbols[dw].N.Name.Long);
          nLen = strlen(sA);
        }
      }
      else
      {
        dwSkip = (DWORD)(lpSymbolsEx[dw].NumberOfAuxSymbols);
        if (lpSymbolsEx[dw].SectionNumber != dwSection+1)
          continue;
        //if (lpSymbolsEx[dw].StorageClass != IMAGE_SYM_CLASS_EXTERNAL)
        //  continue;
        dwValue = lpSymbolsEx[dw].Value;
        if (lpSymbolsEx[dw].N.Name.Short != 0)
        {
          sA = (LPSTR)(&lpSymbolsEx[dw].N.ShortName);
          for (nLen=0; nLen<8 && sA[nLen]!=0; nLen++);
        }
        else
        {
          sA = (LPSTR)(&lpSymbolsEx[dwSymbolsCount]) + (SIZE_T)(lpSymbolsEx[dw].N.Name.Long);
          nLen = strlen(sA);
        }
      }
      //strip underscores at the beginning
      for (i=0; i<nLen && sA[i]=='_'; i++);
      sA += i;
      nLen -= i;
      //stop on special symbols
      for (i=0; i<nLen; i++)
      {
        if (sA[i] == '.' || sA[i] == '$' || sA[i] == '@')
          break;
      }
      if ((nLen = i) == 0)
        continue;
      if (fprintf(fp, "#define %s_%.*s_OFFSET %lu\n", szVarNameA, nLen, sA, dwValue) == 0)
        goto err_cantwrite;
    }
  }
  //did we dump something?
  if (nSectCounter == 0)
  {
    wprintf(L"Error: No code sections were found.\n");
    goto done;
  }
  wprintf(L"Done!\n");
  nRetCode = 0;

done:
  if (lpFileHdr != NULL)
    free(lpFileHdr);
  if (fp != NULL)
    fclose(fp);
  return (int)nRetCode;
}
