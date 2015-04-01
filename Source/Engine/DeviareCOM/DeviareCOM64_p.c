

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Wed Apr 01 14:03:45 2015
 */
/* Compiler settings for DeviareCOM.idl:
    Os, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 7.00.0555 
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 450
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif /* __RPCPROXY_H_VERSION__ */


#include "DeviareCOM64_i.h"

#define TYPE_FORMAT_STRING_SIZE   3                                 
#define PROC_FORMAT_STRING_SIZE   1                                 
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _DeviareCOM_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } DeviareCOM_MIDL_TYPE_FORMAT_STRING;

typedef struct _DeviareCOM_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } DeviareCOM_MIDL_PROC_FORMAT_STRING;

typedef struct _DeviareCOM_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } DeviareCOM_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const DeviareCOM_MIDL_TYPE_FORMAT_STRING DeviareCOM__MIDL_TypeFormatString;
extern const DeviareCOM_MIDL_PROC_FORMAT_STRING DeviareCOM__MIDL_ProcFormatString;
extern const DeviareCOM_MIDL_EXPR_FORMAT_STRING DeviareCOM__MIDL_ExprFormatString;



#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const DeviareCOM_MIDL_PROC_FORMAT_STRING DeviareCOM__MIDL_ProcFormatString =
    {
        0,
        {

			0x0
        }
    };

static const DeviareCOM_MIDL_TYPE_FORMAT_STRING DeviareCOM__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };


/* Standard interface: __MIDL_itf_DeviareCOM_0000_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INktObject, ver. 0.0,
   GUID={0x3B886748,0xDC9B,0x43B0,{0xB7,0x61,0x68,0xDA,0xD9,0xE5,0x48,0x4F}} */


/* Object interface: INktProcess, ver. 0.0,
   GUID={0x32EC8909,0x95FE,0x483c,{0xA5,0xBA,0xBE,0xE8,0x64,0x70,0xC2,0x10}} */


/* Object interface: INktProcessesEnum, ver. 0.0,
   GUID={0xDF43831B,0x7A0B,0x43d4,{0xAC,0x40,0x61,0x36,0xA7,0xC9,0x5B,0x28}} */


/* Object interface: INktModule, ver. 0.0,
   GUID={0xF12AAEB4,0x3CD8,0x435F,{0xBF,0x6B,0x7D,0x2C,0x2A,0xFF,0xD4,0xEC}} */


/* Object interface: INktModulesEnum, ver. 0.0,
   GUID={0x382A6E42,0x3E59,0x40d7,{0xBA,0x40,0xAF,0xD1,0x06,0x11,0xC5,0xB8}} */


/* Object interface: INktExportedFunction, ver. 0.0,
   GUID={0x1B91884B,0xE65C,0x487d,{0xAE,0x89,0x0D,0x86,0x27,0x8C,0x93,0xC9}} */


/* Object interface: INktExportedFunctionsEnum, ver. 0.0,
   GUID={0xB110FD2E,0x174C,0x40ec,{0xB1,0x80,0xAA,0x2B,0xF9,0x8C,0x99,0x2D}} */


/* Object interface: INktHook, ver. 0.0,
   GUID={0xF69A083D,0x105F,0x417b,{0xB1,0xE3,0xEE,0x43,0x0E,0x58,0x43,0x1A}} */


/* Object interface: INktHooksEnum, ver. 0.0,
   GUID={0xA31B9BE8,0xCE9A,0x4d7c,{0xA6,0x24,0x20,0x64,0x0A,0xCA,0x53,0x2C}} */


/* Object interface: INktParam, ver. 0.0,
   GUID={0xBB0A417B,0xD0F2,0x4c69,{0x81,0x58,0x95,0x08,0x7B,0x0A,0x0C,0x8F}} */


/* Object interface: INktParamsEnum, ver. 0.0,
   GUID={0x718E4DBC,0xDFC2,0x4fa8,{0xBF,0xE6,0x0A,0x81,0x92,0xCE,0xEF,0x5D}} */


/* Object interface: INktHookCallInfo, ver. 0.0,
   GUID={0x1A2D8AE6,0x6CF9,0x49db,{0x88,0x0D,0xA8,0x06,0xD5,0x59,0xEE,0x20}} */


/* Object interface: INktProcessMemory, ver. 0.0,
   GUID={0xACE2F499,0xAA1C,0x4b36,{0x91,0x2F,0x18,0x9F,0x8F,0x8B,0x67,0x65}} */


/* Object interface: INktStackTrace, ver. 0.0,
   GUID={0x42B03B4D,0xC3DA,0x463d,{0x98,0xB7,0x73,0x40,0x57,0x54,0xA7,0x64}} */


/* Object interface: INktSpyMgr, ver. 0.0,
   GUID={0xDD71FF07,0xB1E6,0x430e,{0x84,0x39,0x53,0xB7,0x60,0xE9,0x00,0x62}} */


/* Object interface: INktHookCallInfoPlugin, ver. 0.0,
   GUID={0xD7630D7B,0x12E3,0x407a,{0x80,0x56,0xCF,0x08,0xA2,0x5B,0x3A,0x07}} */


/* Object interface: INktHookInfo, ver. 0.0,
   GUID={0x99DE4737,0xC1B4,0x49f6,{0x80,0x72,0xA7,0x64,0x5E,0x78,0xBA,0x8B}} */


/* Object interface: INktDbObject, ver. 0.0,
   GUID={0x8B4C6D62,0x05C5,0x48ee,{0xA5,0x27,0xE1,0xE1,0xC3,0x5F,0x52,0x2C}} */


/* Object interface: INktDbObjectsEnum, ver. 0.0,
   GUID={0xE36B3F2F,0xDEA5,0x4d55,{0x93,0x48,0x8B,0x67,0xA4,0x15,0x7B,0x21}} */


/* Object interface: INktDbModule, ver. 0.0,
   GUID={0x9AB36611,0xAB19,0x4346,{0x95,0x63,0xAB,0x47,0xA7,0x1A,0x86,0x3F}} */


/* Object interface: INktDbModulesEnum, ver. 0.0,
   GUID={0xD8E4F664,0x6EC1,0x4c6b,{0x87,0x91,0xD7,0x2E,0xC6,0x9E,0xBC,0xA1}} */


/* Object interface: INktTools, ver. 0.0,
   GUID={0xD9C5FF67,0x2311,0x494f,{0xBA,0x9A,0xC7,0xC5,0xAA,0xF1,0x43,0x01}} */


/* Object interface: INktStructPEFileHeader, ver. 0.0,
   GUID={0x4192DDF7,0xFB9A,0x4de5,{0x8A,0xE5,0xAD,0xCB,0x5C,0x98,0x6B,0x8F}} */


/* Object interface: INktStructPEOptionalHeader, ver. 0.0,
   GUID={0xFE1FD754,0x7630,0x44db,{0xBF,0xAA,0x06,0x50,0x5C,0x44,0x5B,0x59}} */


/* Object interface: INktStructPESections, ver. 0.0,
   GUID={0x5BF7BC52,0x7158,0x4e0f,{0x85,0x7E,0xF7,0xFD,0xB1,0x37,0x24,0x9F}} */


/* Object interface: INktPdbFunctionSymbol, ver. 0.0,
   GUID={0x21403F16,0xB370,0x4bc7,{0x9D,0x7E,0x0D,0xED,0xA5,0x4E,0xAD,0x01}} */

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    DeviareCOM__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50000, /* Ndr library version */
    0,
    0x700022b, /* MIDL Version 7.0.555 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * const _DeviareCOM_ProxyVtblList[] = 
{
    0
};

const CInterfaceStubVtbl * const _DeviareCOM_StubVtblList[] = 
{
    0
};

PCInterfaceName const _DeviareCOM_InterfaceNamesList[] = 
{
    0
};


#define _DeviareCOM_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _DeviareCOM, pIID, n)

int __stdcall _DeviareCOM_IID_Lookup( const IID * pIID, int * pIndex )
{
    return 0;
}

const ExtendedProxyFileInfo DeviareCOM_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _DeviareCOM_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _DeviareCOM_StubVtblList,
    (const PCInterfaceName * ) & _DeviareCOM_InterfaceNamesList,
    0, /* no delegation */
    & _DeviareCOM_IID_Lookup, 
    0,
    1,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* defined(_M_AMD64)*/

