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

static LPSTR lpData = NULL;
static SIZE_T nDataSize = 0;
static SIZE_T nDataLen = 0;

//-----------------------------------------------------------

static BOOL DoProcess(SIZE_T nBits);
static BOOL EnsureDataSize(SIZE_T nSize);
static BOOL ReplaceEmptyLinesOnMacros();
static BOOL ReplaceStrings();
static BOOL FixNumbersConstants();
static BOOL IsSpaceChar(char chA);
static BOOL IsValidIdentificatorCharA(char chA, BOOL bEnd);

//-----------------------------------------------------------

int _tmain(int argc, _TCHAR* argv[])
{
  FILE *fp;
  SIZE_T k, nBits;

  if (argc != 4)
  {
    _tprintf_s(_T("Use: HeaderFix 32|64 input-file output-file.\n"));
    return 0;
  }
  nBits = _ttol(argv[1]);
  if (nBits != 32 && nBits != 64)
  {
    _tprintf_s(_T("Error: Mode must be 32 or 64.\n"));
    return 0;
  }
  //open & read source file
  if (_tfopen_s(&fp, argv[2], _T("rb")) != 0)
  {
    _tprintf_s(_T("Error: Cannot open source file.\n"));
    return 0;
  }
  fseek(fp, 0, SEEK_END);
  k = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  if (k > 0)
  {
    if (EnsureDataSize(k) == FALSE)
    {
      fclose(fp);
main_err_nomem:
      _tprintf_s(_T("Error: Not enough memory.\n"));
      return 0;
    }
    if (fread(lpData, 1, k, fp) != k)
    {
      fclose(fp);
      _tprintf_s(_T("Error: Cannot read source file.\n"));
      return 0;
    }
    fclose(fp);
    nDataLen = k;
  }
  //process
  if (DoProcess(nBits) == FALSE)
  {
    if (lpData != NULL)
      free(lpData);
    goto main_err_nomem;
  }
  //write destinantion file
  if (_tfopen_s(&fp, argv[3], _T("wb")) != 0)
  {
    _tprintf_s(_T("Error: Cannot create destination file.\n"));
    return 0;
  }
  if (nDataLen > 0 &&
      fwrite(lpData, 1, nDataLen, fp) != nDataLen)
  {
    fclose(fp);
    _tprintf_s(_T("Error: Cannot write destination file.\n"));
    return 0;
  }
  fclose(fp);
  //cleanup
  if (lpData != NULL)
    free(lpData);
	return 0;
}

//-----------------------------------------------------------

static BOOL DoProcess(SIZE_T nBits)
{
  SIZE_T nPos, c, k;

  nPos = 0;
  while (nPos < nDataLen)
  {
    //kill "__asm"
    if (nPos+5 <= nDataLen &&
        strncmp(lpData+nPos, "__asm", 5) == 0 &&
        (nPos==0 || isspace(lpData[nPos-1])!=FALSE))
    {
      for (k=nPos+5; k<nDataLen &&
                     (lpData[k]==' ' || lpData[k]=='\t' || lpData[k]=='\n' || lpData[k]=='\r'); k++);
      if (k<nDataLen && lpData[k]=='{')
      {
        //asm block with braces, find closing brace
        for (++k; k<nDataLen && lpData[k]!='}'; k++);
        if ( k<nDataLen && lpData[k]=='}')
          k++;
      }
      else
      {
        //asm block of one line, find semicolon
        for (++k; k<nDataLen && lpData[k]!=';'; k++);
      }
      //strip block
      memmove(lpData+nPos, lpData+k, nDataLen-k);
      nDataLen -= (k-nPos);
      continue;
    }
    //kill "__C_ASSERT__"
    if (nPos+13 <= nDataLen &&
        strncmp(lpData+nPos, "__C_ASSERT__[", 13) == 0 &&
        (nPos==0 || isspace(lpData[nPos-1])!=FALSE))
    {
      //go back to start of line
      while (nPos>0 && lpData[nPos-1]!='\n' && lpData[nPos-1]!='\r')
        nPos--;
      //find EOL
      for (k=nPos; k<nDataLen && lpData[k]!='\n' && lpData[k]!='\r'; k++);
      while (k<nDataLen && (lpData[k]=='\n' || lpData[k]=='\r'))
        k++;
      //strip line
      memmove(lpData+nPos, lpData+k, nDataLen-k);
      nDataLen -= (k-nPos);
      continue;
    }
    //kill "__inline"
    if (((nPos+7 <= nDataLen &&
          strncmp(lpData+nPos, "inline", 6) == 0 && isspace(lpData[nPos+6]) != FALSE) ||
         (nPos+8 <= nDataLen &&
          strncmp(lpData+nPos, "_inline", 7) == 0 && isspace(lpData[nPos+7]) != FALSE) ||
         (nPos+9 <= nDataLen &&
          strncmp(lpData+nPos, "__inline", 8) == 0 && isspace(lpData[nPos+8]) != FALSE) ||
         (nPos+14 <= nDataLen &&
          strncmp(lpData+nPos, "__forceinline", 13) == 0 && isspace(lpData[nPos+13]) != FALSE)) &&
        (nPos==0 || isspace(lpData[nPos-1])!=FALSE))
    {
      //go back to start of line or opening brace
      while (nPos>0 && lpData[nPos-1]!='\n' && lpData[nPos-1]!='\r' && lpData[nPos-1]!='{')
        nPos--;
      //advance until end of function
      for (k=nPos,c=0; k<nDataLen; k++)
      {
        if (lpData[k] == '{')
          c++;
        else if (lpData[k] == '}')
        {
          if ((--c) == 0)
          {
            k++;
            break;
          }
        }
      }
      //strip block
      memmove(lpData+nPos, lpData+k, nDataLen-k);
      nDataLen -= (k-nPos);
      continue;
    }
    //skip char
    nPos++;
  }
  /*
  //replace empty lines on macros
  if (ReplaceEmptyLinesOnMacros() == FALSE)
    return FALSE;
  */
  //replace typedefs of handles
  if (ReplaceStrings() == FALSE)
    return FALSE;
  //replace typedefs of handles
  if (FixNumbersConstants() == FALSE)
    return FALSE;
  //done
  return TRUE;
}

static BOOL EnsureDataSize(SIZE_T nSize)
{
  if (nSize >= nDataSize)
  {
    LPSTR lpNewData;

    nSize = (nSize+1048575) & (~1048575);
    lpNewData = (LPSTR)malloc(nSize);
    if (lpNewData == NULL)
      return FALSE;
    if (lpData != NULL)
    {
      memcpy(lpNewData, lpData, nDataLen);
      free(lpData);
    }
    lpData = lpNewData;
    nDataSize = nSize;
  }
  return TRUE;
}

static BOOL ReplaceEmptyLinesOnMacros()
{
  SIZE_T nPos, nPos2;
  BOOL bEndsWithMacro;

  bEndsWithMacro = FALSE;
  nPos = 0;
  while (nPos < nDataLen)
  {
    if (lpData[nPos] == '\\')
    {
      bEndsWithMacro = TRUE;
      nPos++;
    }
    else if (lpData[nPos] == '\n' || lpData[nPos] == '\r')
    {
      while (nPos < nDataLen && (lpData[nPos] == '\n' || lpData[nPos] == '\r'))
        nPos++;
      if (bEndsWithMacro != FALSE)
      {
        while (1)
        {
          //check if the next line is a blank
          nPos2 = nPos;
          while (nPos2 < nDataLen && lpData[nPos2] != '\n' && lpData[nPos2] != '\r' &&
                 IsSpaceChar(lpData[nPos2]) != FALSE)
            nPos2++;
          if (nPos2 >= nDataLen || (lpData[nPos2] != '\n' && lpData[nPos2] != '\r'))
            break;
          //we hit a pure blank line
          while (nPos2 < nDataLen && (lpData[nPos2] == '\n' || lpData[nPos2] == '\r'))
            nPos2++;
          //strip block from nPos & nPos2
          memmove(lpData+nPos, lpData+nPos2, nDataLen-nPos2);
          nDataLen -= (nPos2 - nPos);
        }
      }
      bEndsWithMacro = FALSE;
    }
    else if (IsSpaceChar(lpData[nPos]) == FALSE)
    {
      bEndsWithMacro = FALSE;
      nPos++;
    }
    else
    {
      nPos++;
    }
  }
  return TRUE;
}

static BOOL ReplaceStrings()
{
  static const struct {
    LPSTR szToSearchA;
    LPSTR szToReplaceA;
    BOOL bMultiPass;
  } aItems[] = {
    { "__declspec(deprecated)",                                                                                                                                      "",                                           TRUE  },
    { "typedef void * HGDIOBJ;",                                                                                                                                     "typedef size_t HGDIOBJ;",                    FALSE },
    { "typedef void * HANDLE;",                                                                                                                                      "typedef size_t HANDLE;",                     FALSE },
    { "typedef void * HCRYPTOIDFUNCSET;",                                                                                                                            "typedef size_t HCRYPTOIDFUNCSET;",           FALSE },
    { "typedef void * HCRYPTOIDFUNCADDR;",                                                                                                                           "typedef size_t HCRYPTOIDFUNCADDR;",          FALSE },
    { "typedef void * HCRYPTMSG;",                                                                                                                                   "typedef size_t HCRYPTMSG;",                  FALSE },
    { "typedef void * HCERTSTORE;",                                                                                                                                  "typedef size_t HCERTSTORE;",                 FALSE },
    { "typedef void * HCERTSTOREPROV;",                                                                                                                              "typedef size_t HCERTSTOREPROV;",             FALSE },
    { "typedef void * HCRYPTDEFAULTCONTEXT;",                                                                                                                        "typedef size_t HCRYPTDEFAULTCONTEXT;",       FALSE },
    { "typedef void * HCERT_SERVER_OCSP_RESPONSE;",                                                                                                                  "typedef size_t HCERT_SERVER_OCSP_RESPONSE;", FALSE },
    { "typedef LPVOID HINTERNET;",                                                                                                                                   "typedef size_t HINTERNET;",                  FALSE },
    { "struct HWND__ { int unused; }; typedef struct HWND__ *HWND;",                                                                                                 "typedef size_t HWND;", FALSE },
    { "struct HHOOK__ { int unused; }; typedef struct HHOOK__ *HHOOK;",                                                                                              "typedef size_t HHOOK;", FALSE },
    { "struct HLSURF__ { int unused; }; typedef struct HLSURF__ *HLSURF;",                                                                                           "typedef size_t HLSURF;", FALSE },
    { "struct HTOUCHINPUT__ { int unused; }; typedef struct HTOUCHINPUT__ *HTOUCHINPUT;",                                                                            "typedef size_t HTOUCHINPUT;", FALSE },
    { "struct HRAWINPUT__ { int unused; }; typedef struct HRAWINPUT__ *HRAWINPUT;",                                                                                  "typedef size_t HRAWINPUT;", FALSE },
    { "struct HGESTUREINFO__ { int unused; }; typedef struct HGESTUREINFO__ *HGESTUREINFO;",                                                                         "typedef size_t HGESTUREINFO;", FALSE },
    { "struct SC_HANDLE__ { int unused; }; typedef struct SC_HANDLE__ *SC_HANDLE;",                                                                                  "typedef size_t SC_HANDLE;", FALSE },
    { "struct SERVICE_STATUS_HANDLE__ { int unused; }; typedef struct SERVICE_STATUS_HANDLE__ *SERVICE_STATUS_HANDLE;",                                              "typedef size_t SERVICE_STATUS_HANDLE;", FALSE },
    { "struct HIMC__ { int unused; }; typedef struct HIMC__ *HIMC;",                                                                                                 "typedef size_t HIMC;", FALSE },
    { "struct HIMCC__ { int unused; }; typedef struct HIMCC__ *HIMCC;",                                                                                              "typedef size_t HIMCC;", FALSE },
    { "struct HDROP__ { int unused; }; typedef struct HDROP__ *HDROP;",                                                                                              "typedef size_t HDROP;", FALSE },
    { "struct HRASCONN__ { int unused; }; typedef struct HRASCONN__ *HRASCONN;",                                                                                     "typedef size_t HRASCONN;", FALSE },
    { "struct HACMDRIVERID__ { int unused; }; typedef struct HACMDRIVERID__ *HACMDRIVERID;",                                                                         "typedef size_t HACMDRIVERID;", FALSE },
    { "struct HACMDRIVER__ { int unused; }; typedef struct HACMDRIVER__ *HACMDRIVER;",                                                                               "typedef size_t HACMDRIVER;", FALSE },
    { "struct HACMSTREAM__ { int unused; }; typedef struct HACMSTREAM__ *HACMSTREAM;",                                                                               "typedef size_t HACMSTREAM;", FALSE },
    { "struct HACMOBJ__ { int unused; }; typedef struct HACMOBJ__ *HACMOBJ;",                                                                                        "typedef size_t HACMOBJ;", FALSE },
    { "struct HDRVR__ { int unused; }; typedef struct HDRVR__ *HDRVR;",                                                                                              "typedef size_t HDRVR;", FALSE },
    { "struct HWAVE__ { int unused; }; typedef struct HWAVE__ *HWAVE;",                                                                                              "typedef size_t HWAVE;", FALSE },
    { "struct HWAVEIN__ { int unused; }; typedef struct HWAVEIN__ *HWAVEIN;",                                                                                        "typedef size_t HWAVEIN;", FALSE },
    { "struct HWAVEOUT__ { int unused; }; typedef struct HWAVEOUT__ *HWAVEOUT;",                                                                                     "typedef size_t HWAVEOUT;", FALSE },
    { "struct HMIDI__ { int unused; }; typedef struct HMIDI__ *HMIDI;",                                                                                              "typedef size_t HMIDI;", FALSE },
    { "struct HMIDIIN__ { int unused; }; typedef struct HMIDIIN__ *HMIDIIN;",                                                                                        "typedef size_t HMIDIIN;", FALSE },
    { "struct HMIDIOUT__ { int unused; }; typedef struct HMIDIOUT__ *HMIDIOUT;",                                                                                     "typedef size_t HMIDIOUT;", FALSE },
    { "struct HMIDISTRM__ { int unused; }; typedef struct HMIDISTRM__ *HMIDISTRM;",                                                                                  "typedef size_t HMIDISTRM;", FALSE },
    { "struct HMIXEROBJ__ { int unused; }; typedef struct HMIXEROBJ__ *HMIXEROBJ;",                                                                                  "typedef size_t HMIXEROBJ;", FALSE },
    { "struct HMIXER__ { int unused; }; typedef struct HMIXER__ *HMIXER;",                                                                                           "typedef size_t HMIXER;", FALSE },
    { "struct HMMIO__ { int unused; }; typedef struct HMMIO__ *HMMIO;",                                                                                              "typedef size_t HMMIO;", FALSE },
    { "struct HIC__ { int unused; }; typedef struct HIC__ *HIC;",                                                                                                    "typedef size_t HIC;", FALSE },
    { "struct HVIDEO__ { int unused; }; typedef struct HVIDEO__ *HVIDEO;",                                                                                           "typedef size_t HVIDEO;", FALSE },
    { "struct HPSXA__ { int unused; }; typedef struct HPSXA__ *HPSXA;",                                                                                              "typedef size_t HPSXA;", FALSE },
    { "struct AUTHZ_ACCESS_CHECK_RESULTS_HANDLE__ { int unused; }; typedef struct AUTHZ_ACCESS_CHECK_RESULTS_HANDLE__ *AUTHZ_ACCESS_CHECK_RESULTS_HANDLE;",          "typedef size_t AUTHZ_ACCESS_CHECK_RESULTS_HANDLE;", FALSE },
    { "struct AUTHZ_CLIENT_CONTEXT_HANDLE__ { int unused; }; typedef struct AUTHZ_CLIENT_CONTEXT_HANDLE__ *AUTHZ_CLIENT_CONTEXT_HANDLE;",                            "typedef size_t AUTHZ_CLIENT_CONTEXT_HANDLE;", FALSE },
    { "struct AUTHZ_RESOURCE_MANAGER_HANDLE__ { int unused; }; typedef struct AUTHZ_RESOURCE_MANAGER_HANDLE__ *AUTHZ_RESOURCE_MANAGER_HANDLE;",                      "typedef size_t AUTHZ_RESOURCE_MANAGER_HANDLE;", FALSE },
    { "struct AUTHZ_AUDIT_EVENT_HANDLE__ { int unused; }; typedef struct AUTHZ_AUDIT_EVENT_HANDLE__ *AUTHZ_AUDIT_EVENT_HANDLE;",                                     "typedef size_t AUTHZ_AUDIT_EVENT_HANDLE;", FALSE },
    { "struct AUTHZ_AUDIT_EVENT_TYPE_HANDLE__ { int unused; }; typedef struct AUTHZ_AUDIT_EVENT_TYPE_HANDLE__ *AUTHZ_AUDIT_EVENT_TYPE_HANDLE;",                      "typedef size_t AUTHZ_AUDIT_EVENT_TYPE_HANDLE;", FALSE },
    { "struct AUTHZ_SECURITY_EVENT_PROVIDER_HANDLE__ { int unused; }; typedef struct AUTHZ_SECURITY_EVENT_PROVIDER_HANDLE__ *AUTHZ_SECURITY_EVENT_PROVIDER_HANDLE;", "typedef size_t AUTHZ_SECURITY_EVENT_PROVIDER_HANDLE;", FALSE },
    { "struct HWINWATCH__ { int unused; }; typedef struct HWINWATCH__ *HWINWATCH;",                                                                                  "typedef size_t HWINWATCH;", FALSE },
    { "struct HCONVLIST__ { int unused; }; typedef struct HCONVLIST__ *HCONVLIST;",                                                                                  "typedef size_t HCONVLIST;", FALSE },
    { "struct HCONV__ { int unused; }; typedef struct HCONV__ *HCONV;",                                                                                              "typedef size_t HCONV;", FALSE },
    { "struct HSZ__ { int unused; }; typedef struct HSZ__ *HSZ;",                                                                                                    "typedef size_t HSZ;", FALSE },
    { "struct HDDEDATA__ { int unused; }; typedef struct HDDEDATA__ *HDDEDATA;",                                                                                     "typedef size_t HDDEDATA;", FALSE },
    { "struct HPTPROVIDER__ { int unused; }; typedef struct HPTPROVIDER__ *HPTPROVIDER;",                                                                            "typedef size_t HPTPROVIDER;", FALSE },
    { "struct HRECOALT__ { int unused; }; typedef struct HRECOALT__ *HRECOALT;",                                                                                     "typedef size_t HRECOALT;", FALSE },
    { "struct HRECOCONTEXT__ { int unused; }; typedef struct HRECOCONTEXT__ *HRECOCONTEXT;",                                                                         "typedef size_t HRECOCONTEXT;", FALSE },
    { "struct HRECOGNIZER__ { int unused; }; typedef struct HRECOGNIZER__ *HRECOGNIZER;",                                                                            "typedef size_t HRECOGNIZER;", FALSE },
    { "struct HRECOLATTICE__ { int unused; }; typedef struct HRECOLATTICE__ *HRECOLATTICE;",                                                                         "typedef size_t HRECOLATTICE;", FALSE },
    { "struct HRECOWORDLIST__ { int unused; }; typedef struct HRECOWORDLIST__ *HRECOWORDLIST;",                                                                      "typedef size_t HRECOWORDLIST;", FALSE },
    { "struct SPSTATEHANDLE__ { int unused; }; typedef struct SPSTATEHANDLE__ *SPSTATEHANDLE;",                                                                      "typedef size_t SPSTATEHANDLE;", FALSE },
    { "struct HUIANODE__ { int unused; }; typedef struct HUIANODE__ *HUIANODE;",                                                                                     "typedef size_t HUIANODE;", FALSE },
    { "struct HUIAPATTERNOBJECT__ { int unused; }; typedef struct HUIAPATTERNOBJECT__ *HUIAPATTERNOBJECT;",                                                          "typedef size_t HUIAPATTERNOBJECT;", FALSE },
    { "struct HUIATEXTRANGE__ { int unused; }; typedef struct HUIATEXTRANGE__ *HUIATEXTRANGE;",                                                                      "typedef size_t HUIATEXTRANGE;", FALSE },
    { "struct HUIAEVENT__ { int unused; }; typedef struct HUIAEVENT__ *HUIAEVENT;",                                                                                  "typedef size_t HUIAEVENT;", FALSE },
    { "struct SAFER_LEVEL_HANDLE__ { int unused; }; typedef struct SAFER_LEVEL_HANDLE__ *SAFER_LEVEL_HANDLE;",                                                       "typedef size_t SAFER_LEVEL_HANDLE;", FALSE },
    { "struct HKEY__ { int unused; }; typedef struct HKEY__ *HKEY;",                            "typedef size_t HKEY;",                       FALSE },
    { "struct HACCEL__ { int unused; }; typedef struct HACCEL__ *HACCEL;",                      "typedef size_t HACCEL;",                     FALSE },
    { "struct HBITMAP__ { int unused; }; typedef struct HBITMAP__ *HBITMAP;",                   "typedef size_t HBITMAP;",                    FALSE },
    { "struct HBRUSH__ { int unused; }; typedef struct HBRUSH__ *HBRUSH;",                      "typedef size_t HBRUSH;",                     FALSE },
    { "struct HCOLORSPACE__ { int unused; }; typedef struct HCOLORSPACE__ *HCOLORSPACE;",       "typedef size_t HCOLORSPACE;",                FALSE },
    { "struct HDC__ { int unused; }; typedef struct HDC__ *HDC;",                               "typedef size_t HDC;",                        FALSE },
    { "struct HGLRC__ { int unused; }; typedef struct HGLRC__ *HGLRC;",                         "typedef size_t HGLRC;",                      FALSE },
    { "struct HDESK__ { int unused; }; typedef struct HDESK__ *HDESK;",                         "typedef size_t HDESK;",                      FALSE },
    { "struct HENHMETAFILE__ { int unused; }; typedef struct HENHMETAFILE__ *HENHMETAFILE;",    "typedef size_t HENHMETAFILE;",               FALSE },
    { "struct HFONT__ { int unused; }; typedef struct HFONT__ *HFONT;",                         "typedef size_t HFONT;",                      FALSE },
    { "struct HICON__ { int unused; }; typedef struct HICON__ *HICON;",                         "typedef size_t HICON;",                      FALSE },
    { "struct HMENU__ { int unused; }; typedef struct HMENU__ *HMENU;",                         "typedef size_t HMENU;",                      FALSE },
    { "struct HMETAFILE__ { int unused; }; typedef struct HMETAFILE__ *HMETAFILE;",             "typedef size_t HMETAFILE;",                  FALSE },
    { "struct HINSTANCE__ { int unused; }; typedef struct HINSTANCE__ *HINSTANCE;",             "typedef size_t HINSTANCE;",                  FALSE },
    { "struct HPALETTE__ { int unused; }; typedef struct HPALETTE__ *HPALETTE;",                "typedef size_t HPALETTE;",                   FALSE },
    { "struct HPEN__ { int unused; }; typedef struct HPEN__ *HPEN;",                            "typedef size_t HPEN;",                       FALSE },
    { "struct HRGN__ { int unused; }; typedef struct HRGN__ *HRGN;",                            "typedef size_t HRGN;",                       FALSE },
    { "struct HRSRC__ { int unused; }; typedef struct HRSRC__ *HRSRC;",                         "typedef size_t HRSRC;",                      FALSE },
    { "struct HSPRITE__ { int unused; }; typedef struct HSPRITE__ *HSPRITE;",                   "typedef size_t HSPRITE;",                    FALSE },
    { "struct HSTR__ { int unused; }; typedef struct HSTR__ *HSTR;",                            "typedef size_t HSTR;",                       FALSE },
    { "struct HTASK__ { int unused; }; typedef struct HTASK__ *HTASK;",                         "typedef size_t HTASK;",                      FALSE },
    { "struct HWINSTA__ { int unused; }; typedef struct HWINSTA__ *HWINSTA;",                   "typedef size_t HWINSTA;",                    FALSE },
    { "struct HKL__ { int unused; }; typedef struct HKL__ *HKL;",                               "typedef size_t HKL;",                        FALSE },
    { "struct HWINEVENTHOOK__ { int unused; }; typedef struct HWINEVENTHOOK__ *HWINEVENTHOOK;", "typedef size_t HWINEVENTHOOK;",              FALSE },
    { "struct HMONITOR__ { int unused; }; typedef struct HMONITOR__ *HMONITOR;",                "typedef size_t HMONITOR;",                   FALSE },
    { "struct HUMPD__ { int unused; }; typedef struct HUMPD__ *HUMPD;",                         "typedef size_t HUMPD;",                      FALSE },
    { "BTH_ADDR BTH_ADDR;",                                                                     "BTH_ADDR _BTH_ADDR;",                        FALSE },
    { "GUID GUID;",                                                                             "GUID _GUID;",                                FALSE },
    { "Network = ( Software_Fixed + 1 )",                                                       "_Network = ( Software_Fixed + 1 )",          FALSE },
    { "WLOOP WLOOP[1];",                                                                        "WLOOP _WLOOP[1];",                           FALSE },
    { "UOW UOW;",                                                                               "UOW _UOW;",                                  TRUE },
    { "((EmfPlusRecordType)((",                                                                 "(((",                                        TRUE  },
    { "__declspec(deprecated(\"swprintf has been changed to conform with the ISO C standard, adding an extra character count parameter. To use traditional Microsoft swprintf, set _CRT_NON_CONFORMING_SWPRINTFS.\"))", "", TRUE },
    { "__declspec(deprecated(\"This method is deprecated. Use the HttpGetExtendedInterface helper function instead.\"))", "", TRUE },
    { "__declspec(deprecated(\"This API has been deprecated. Refer to http://go.microsoft.com/fwlink/?LinkId=143720 for more details.\"))", "", TRUE },

    { "HRESULT (__stdcall * GetSectionBlock) ( HCEESECTION section, ULONG len, ULONG align=1, void **ppBytes=0) ;", "HRESULT (__stdcall * GetSectionBlock) ( HCEESECTION section, ULONG len, ULONG align, void **ppBytes) ;", FALSE },
    { "(IDsAdminNewObjPrimarySite * This, )", "(IDsAdminNewObjPrimarySite * This )", FALSE },
    { "(IDsAdminNotifyHandler * This, )", "(IDsAdminNotifyHandler * This )", FALSE },
    { "HRESULT (__stdcall * CountBytes)( UINT *pcb ) = 0;", "HRESULT (__stdcall * CountBytes)( UINT *pcb );", FALSE },
    { "HRESULT (__stdcall * LockBytes)( UINT cb, ULONG ulOffset, const void ** ppvBytes ) = 0;", "HRESULT (__stdcall * LockBytes)( UINT cb, ULONG ulOffset, const void ** ppvBytes );", FALSE },
    { "HRESULT (__stdcall * UnlockBytes)( const void *pvBytes, UINT cb, ULONG ulOffset ) = 0;", "HRESULT (__stdcall * UnlockBytes)( const void *pvBytes, UINT cb, ULONG ulOffset );", FALSE },
    { "PixelFormat PixelFormat;",                                                               "PixelFormat _PixelFormat;",                  FALSE },
    
    { "UINT Metafile::EmfToWmfBits", "UINT EmfToWmfBits", FALSE },
    { "StringTrimming StringFormat::GetTrimming()", "StringTrimming GetTrimming()", FALSE },
    { "struct GUID *riid", "GUID *riid", TRUE },
    { "struct DISPPARAMS *pdispparams", "DISPPARAMS *pdispparams", TRUE },
    { "struct EXCEPINFO *pexcepinfo", "EXCEPINFO *pexcepinfo", TRUE },
    { "unsigned UINT", "unsigned int", TRUE },
    { "eAll = 0x1ff", "_eAll = 0x1ff", FALSE },

    { "friend Graphics;", "friend class Graphics;", FALSE },
    { "UiInfo UiInfo;",                                                                             "UiInfo _UiInfo;",                                FALSE },

    { "int int ", "__int64 ", TRUE },

    { "__declspec( selectany ) extern const float POSITIVE_INFINITY = ((float)( 1e308 * 10 ));", "", FALSE },
    { "__declspec( selectany ) extern const float NEGATIVE_INFINITY = ((float)( -1e308 * 10 ));", "", FALSE },
    { "__declspec( selectany ) extern const float NaN = ((float)((1e308 * 10) * 0. ));", "", FALSE },

    { "extern const __declspec(selectany) GUID IID_IRowsetChangeExtInfo", "extern \"C\" const __declspec(selectany) GUID IID_IRowsetChangeExtInfo", FALSE },
    { "extern const __declspec(selectany) GUID IID_ISQLRequestDiagFields", "extern \"C\" const __declspec(selectany) GUID IID_ISQLRequestDiagFields", FALSE },
    { "extern const __declspec(selectany) GUID IID_ISQLGetDiagField", "extern \"C\" const __declspec(selectany) GUID IID_ISQLGetDiagField", FALSE },

    { "NODERESTRICTION or;", "NODERESTRICTION _or;", FALSE },

    { "__field_ecount_part(nCurPropSize, nNumberOfProperties)", "", FALSE },
    { "AddProperty(__in LPWSTR pProp)", "AddProperty(LPWSTR pProp)", FALSE },

    { "ConditionType ConditionType;", "ConditionType _ConditionType;", TRUE },
    { "StructureChangeType StructureChangeType;", "StructureChangeType _StructureChangeType;", TRUE },
    { "AsyncContentLoadedState AsyncContentLoadedState;", "AsyncContentLoadedState _AsyncContentLoadedState;", TRUE },
    { "SetInvertPolarity)(ITnef * This,", "SetInvertPolarity)(ITnef * This", FALSE },
    { "RenegotiateVPParameters)(ITnef * This,", "RenegotiateVPParameters)(ITnef * This", FALSE },

    { "union { struct _nonStructType {", "union ___dummyu1 { struct _nonStructType {", FALSE },
    { "union { struct _cp {", "union ___dummyu2 { struct _cp {", FALSE },


    { "union { struct _nonStructType {", "union ___dummyu1 { struct _nonStructType {", FALSE },
    { "union { struct _cp {", "union ___dummyu2 { struct _cp {", FALSE },

    { "namespace std { typedef decltype(__nullptr) nullptr_t; }", "", FALSE },
    { "using ::std::nullptr_t;", "", FALSE },

    { NULL, NULL, FALSE }
  };
  SIZE_T i, k, nStartOffset, nEndOffset;
  BOOL b[2];
  LPSTR s;

  for (i=0; aItems[i].szToSearchA!=NULL; i++)
  {
    //try to find the block
    nStartOffset = 0;
    while (nStartOffset < nDataLen)
    {
      if (lpData[nStartOffset] != aItems[i].szToSearchA[0])
      {
        nStartOffset++;
        continue;
      }
      //possible block start
      s = aItems[i].szToSearchA;
      nEndOffset = nStartOffset;
      while (*s != 0 && nEndOffset < nDataLen)
      {
        if (*s == ' ')
        {
          //if we are on an space, check for a separator
          if (IsSpaceChar(lpData[nEndOffset]) != FALSE)
          {
            while (nEndOffset < nDataLen && IsSpaceChar(lpData[nEndOffset]) != FALSE)
              nEndOffset++;
          }
          else
          {
            //compare the current char with the previous one, at least one of them
            //should be a non-identifier char
            b[0] = (nEndOffset > 0 && IsValidIdentificatorCharA(lpData[nEndOffset-1], TRUE) != FALSE) ? TRUE : FALSE;
            b[1] = IsValidIdentificatorCharA(lpData[nEndOffset], FALSE);
            if (b[0] != FALSE && b[1] != FALSE)
              break; //mismatch
          }
        }
        else
        {
          //else compare char
          if (lpData[nEndOffset] != *s)
            break; //mismatch
          nEndOffset++;
        }
        s++;
      }
      if (*s == 0)
        break; //got a block
      nStartOffset = nEndOffset;
    }
    if (nStartOffset >= nDataLen)
    {
      if (aItems[i].bMultiPass == FALSE)
        printf_s("Note: Block for replacement not found (%s).\n", aItems[i].szToSearchA);
      continue;
    }
    //strip block
    memmove(lpData+nStartOffset, lpData+nEndOffset, nDataLen-nEndOffset);
    nDataLen -= (nEndOffset - nStartOffset);
    //insert replacement
    k = strlen(aItems[i].szToReplaceA);
    if (EnsureDataSize(nDataLen + k) == FALSE)
      return FALSE;
    memmove(lpData+nStartOffset+k, lpData+nStartOffset, nDataLen-nStartOffset);
    memcpy(lpData+nStartOffset, aItems[i].szToReplaceA, k);
    nDataLen += k;
    if (aItems[i].bMultiPass != FALSE)
    {
      i--; //if multipass, retry
    }
  }
  return TRUE;
}

static BOOL IsSpaceChar(char chA)
{
  return ((unsigned char)chA <= 32) ? TRUE : FALSE;
}

static BOOL IsValidIdentificatorCharA(char chA, BOOL bEnd)
{
  if ((chA >= 'A' && chA <= 'Z') || (chA >= 'a' && chA <= 'z') || chA == '_')
    return TRUE;
  if (bEnd != FALSE && (chA >= '0' && chA <= '9'))
    return TRUE;
  return FALSE;
}

static BOOL FixNumbersConstants()
{
  SIZE_T k, nPos, nStart;
  BOOL bIsNeg;
  ULONGLONG nValue;

  nPos = 0;
  while (nPos < nDataLen)
  {
    if (lpData[nPos] == '\'' || lpData[nPos] == '\"')
    {
      CHAR chA = lpData[nPos++];
      while (nPos < nDataLen && lpData[nPos] != chA)
      {
        if (lpData[nPos] == '\\' && nPos < nDataLen-1)
          nPos++;
        nPos++;
      }
      if (nPos < nDataLen)
        nPos++; //skip closing quote
    }
    else if (lpData[nPos] >= '0' && lpData[nPos] <= '9')
    {
      bIsNeg = FALSE;
fnc_gotnumber:
      nValue = 0ui64;
      nStart = nPos;
      if (lpData[nPos] == '0')
      {
        if (nPos < nDataLen-1 && lpData[nPos+1] == 'x')
        {
          //hexa number
          nPos += 2;
          while (nPos < nDataLen)
          {
            if (lpData[nPos] >= '0' && lpData[nPos] <= '9')
              nValue = nValue*16ui64 + (ULONGLONG)(lpData[nPos] - '0');
            else if (lpData[nPos] >= 'A' && lpData[nPos] <= 'F')
              nValue = nValue*16ui64 + (ULONGLONG)(lpData[nPos] - 'A') + 10ui64;
            else if (lpData[nPos] >= 'a' && lpData[nPos] <= 'f')
              nValue = nValue*16ui64 + (ULONGLONG)(lpData[nPos] - 'a') + 10ui64;
            else
              break;
            nPos++;
          }
        }
        else
        {
          //octal number
          while (nPos < nDataLen && lpData[nPos] >= '0' && lpData[nPos] <= '7')
          {
            nValue = nValue*8ui64 + (ULONGLONG)(lpData[nPos] - '0');
            nPos++;
          }
        }
      }
      else
      {
        //decimal number
        while (nPos < nDataLen && lpData[nPos] >= '0' && lpData[nPos] <= '9')
        {
          nValue = nValue*10 + (ULONGLONG)(lpData[nPos] - '0');
          nPos++;
        }
      }
      if (nPos >= nDataLen || lpData[nPos] != '.') //ensure not a decimal point number
      {
        if (nValue > 0xFFFFFFFFui64)
        {
          if (nPos >= nDataLen-4 ||
              (lpData[nPos] == 'U' || lpData[nPos] == 'u') &&
              (lpData[nPos+1] == 'I' || lpData[nPos+1] == 'i') &&
              lpData[nPos+2] == '6' && lpData[nPos+3] == '4')
          {
            bIsNeg = FALSE;
            memmove(lpData+nPos, lpData+nPos+4, nDataLen-nPos-4);
            nDataLen -= 4;
          }
          else if (nPos >= nDataLen-3 ||
                   (lpData[nPos] == 'I' || lpData[nPos] == 'i') &&
                    lpData[nPos+1] == '6' && lpData[nPos+2] == '4')
          {
            bIsNeg = TRUE;
            memmove(lpData+nPos, lpData+nPos+3, nDataLen-nPos-3);
            nDataLen -= 3;
          }
          if (nPos >= nDataLen-3 ||
              (lpData[nPos] != 'U' && lpData[nPos] != 'u') ||
              (lpData[nPos+1] != 'L' && lpData[nPos+1] != 'l') ||
              (lpData[nPos+2] != 'L' && lpData[nPos+2] != 'l'))
          {
            //insert sufix
            k = (bIsNeg == FALSE) ? 3 : 2;
            if (EnsureDataSize(nDataLen + k) == FALSE)
              return FALSE;
            memmove(lpData+nPos+k, lpData+nPos, nDataLen-nPos);
            memcpy(lpData+nPos, (bIsNeg == FALSE) ? "ULL" : "LL", k);
            nDataLen += k;
            nPos += k;
          }
        }
      }
    }
    else if (lpData[nPos] == '-' && nPos < nDataLen-1 &&
             (lpData[nPos+1] >= '0' && lpData[nPos+1] <= '9'))
    {
      bIsNeg = TRUE;
      nPos++;
      goto fnc_gotnumber;
    }
    else if (IsValidIdentificatorCharA(lpData[nPos], FALSE) != FALSE)
    {
      nPos++;
      while (nPos < nDataLen && IsValidIdentificatorCharA(lpData[nPos], TRUE) != FALSE)
        nPos++;
    }
    else
      nPos++;
  }
  return TRUE;
}

