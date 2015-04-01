

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 450
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __DeviareCOM64_i_h__
#define __DeviareCOM64_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INktObject_FWD_DEFINED__
#define __INktObject_FWD_DEFINED__
typedef interface INktObject INktObject;
#endif 	/* __INktObject_FWD_DEFINED__ */


#ifndef __INktProcess_FWD_DEFINED__
#define __INktProcess_FWD_DEFINED__
typedef interface INktProcess INktProcess;
#endif 	/* __INktProcess_FWD_DEFINED__ */


#ifndef __INktProcessesEnum_FWD_DEFINED__
#define __INktProcessesEnum_FWD_DEFINED__
typedef interface INktProcessesEnum INktProcessesEnum;
#endif 	/* __INktProcessesEnum_FWD_DEFINED__ */


#ifndef __INktModule_FWD_DEFINED__
#define __INktModule_FWD_DEFINED__
typedef interface INktModule INktModule;
#endif 	/* __INktModule_FWD_DEFINED__ */


#ifndef __INktModulesEnum_FWD_DEFINED__
#define __INktModulesEnum_FWD_DEFINED__
typedef interface INktModulesEnum INktModulesEnum;
#endif 	/* __INktModulesEnum_FWD_DEFINED__ */


#ifndef __INktExportedFunction_FWD_DEFINED__
#define __INktExportedFunction_FWD_DEFINED__
typedef interface INktExportedFunction INktExportedFunction;
#endif 	/* __INktExportedFunction_FWD_DEFINED__ */


#ifndef __INktExportedFunctionsEnum_FWD_DEFINED__
#define __INktExportedFunctionsEnum_FWD_DEFINED__
typedef interface INktExportedFunctionsEnum INktExportedFunctionsEnum;
#endif 	/* __INktExportedFunctionsEnum_FWD_DEFINED__ */


#ifndef __INktHook_FWD_DEFINED__
#define __INktHook_FWD_DEFINED__
typedef interface INktHook INktHook;
#endif 	/* __INktHook_FWD_DEFINED__ */


#ifndef __INktHooksEnum_FWD_DEFINED__
#define __INktHooksEnum_FWD_DEFINED__
typedef interface INktHooksEnum INktHooksEnum;
#endif 	/* __INktHooksEnum_FWD_DEFINED__ */


#ifndef __INktParam_FWD_DEFINED__
#define __INktParam_FWD_DEFINED__
typedef interface INktParam INktParam;
#endif 	/* __INktParam_FWD_DEFINED__ */


#ifndef __INktParamsEnum_FWD_DEFINED__
#define __INktParamsEnum_FWD_DEFINED__
typedef interface INktParamsEnum INktParamsEnum;
#endif 	/* __INktParamsEnum_FWD_DEFINED__ */


#ifndef __INktHookCallInfo_FWD_DEFINED__
#define __INktHookCallInfo_FWD_DEFINED__
typedef interface INktHookCallInfo INktHookCallInfo;
#endif 	/* __INktHookCallInfo_FWD_DEFINED__ */


#ifndef __INktProcessMemory_FWD_DEFINED__
#define __INktProcessMemory_FWD_DEFINED__
typedef interface INktProcessMemory INktProcessMemory;
#endif 	/* __INktProcessMemory_FWD_DEFINED__ */


#ifndef __INktStackTrace_FWD_DEFINED__
#define __INktStackTrace_FWD_DEFINED__
typedef interface INktStackTrace INktStackTrace;
#endif 	/* __INktStackTrace_FWD_DEFINED__ */


#ifndef __INktSpyMgr_FWD_DEFINED__
#define __INktSpyMgr_FWD_DEFINED__
typedef interface INktSpyMgr INktSpyMgr;
#endif 	/* __INktSpyMgr_FWD_DEFINED__ */


#ifndef __INktHookCallInfoPlugin_FWD_DEFINED__
#define __INktHookCallInfoPlugin_FWD_DEFINED__
typedef interface INktHookCallInfoPlugin INktHookCallInfoPlugin;
#endif 	/* __INktHookCallInfoPlugin_FWD_DEFINED__ */


#ifndef __INktHookInfo_FWD_DEFINED__
#define __INktHookInfo_FWD_DEFINED__
typedef interface INktHookInfo INktHookInfo;
#endif 	/* __INktHookInfo_FWD_DEFINED__ */


#ifndef __INktDbObject_FWD_DEFINED__
#define __INktDbObject_FWD_DEFINED__
typedef interface INktDbObject INktDbObject;
#endif 	/* __INktDbObject_FWD_DEFINED__ */


#ifndef __INktDbObjectsEnum_FWD_DEFINED__
#define __INktDbObjectsEnum_FWD_DEFINED__
typedef interface INktDbObjectsEnum INktDbObjectsEnum;
#endif 	/* __INktDbObjectsEnum_FWD_DEFINED__ */


#ifndef __INktDbModule_FWD_DEFINED__
#define __INktDbModule_FWD_DEFINED__
typedef interface INktDbModule INktDbModule;
#endif 	/* __INktDbModule_FWD_DEFINED__ */


#ifndef __INktDbModulesEnum_FWD_DEFINED__
#define __INktDbModulesEnum_FWD_DEFINED__
typedef interface INktDbModulesEnum INktDbModulesEnum;
#endif 	/* __INktDbModulesEnum_FWD_DEFINED__ */


#ifndef __INktTools_FWD_DEFINED__
#define __INktTools_FWD_DEFINED__
typedef interface INktTools INktTools;
#endif 	/* __INktTools_FWD_DEFINED__ */


#ifndef __INktStructPEFileHeader_FWD_DEFINED__
#define __INktStructPEFileHeader_FWD_DEFINED__
typedef interface INktStructPEFileHeader INktStructPEFileHeader;
#endif 	/* __INktStructPEFileHeader_FWD_DEFINED__ */


#ifndef __INktStructPEOptionalHeader_FWD_DEFINED__
#define __INktStructPEOptionalHeader_FWD_DEFINED__
typedef interface INktStructPEOptionalHeader INktStructPEOptionalHeader;
#endif 	/* __INktStructPEOptionalHeader_FWD_DEFINED__ */


#ifndef __INktStructPESections_FWD_DEFINED__
#define __INktStructPESections_FWD_DEFINED__
typedef interface INktStructPESections INktStructPESections;
#endif 	/* __INktStructPESections_FWD_DEFINED__ */


#ifndef __INktPdbFunctionSymbol_FWD_DEFINED__
#define __INktPdbFunctionSymbol_FWD_DEFINED__
typedef interface INktPdbFunctionSymbol INktPdbFunctionSymbol;
#endif 	/* __INktPdbFunctionSymbol_FWD_DEFINED__ */


#ifndef __NktObject_FWD_DEFINED__
#define __NktObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktObject NktObject;
#else
typedef struct NktObject NktObject;
#endif /* __cplusplus */

#endif 	/* __NktObject_FWD_DEFINED__ */


#ifndef __DNktProcessEvents_FWD_DEFINED__
#define __DNktProcessEvents_FWD_DEFINED__
typedef interface DNktProcessEvents DNktProcessEvents;
#endif 	/* __DNktProcessEvents_FWD_DEFINED__ */


#ifndef __NktProcess_FWD_DEFINED__
#define __NktProcess_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktProcess NktProcess;
#else
typedef struct NktProcess NktProcess;
#endif /* __cplusplus */

#endif 	/* __NktProcess_FWD_DEFINED__ */


#ifndef __NktProcessesEnum_FWD_DEFINED__
#define __NktProcessesEnum_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktProcessesEnum NktProcessesEnum;
#else
typedef struct NktProcessesEnum NktProcessesEnum;
#endif /* __cplusplus */

#endif 	/* __NktProcessesEnum_FWD_DEFINED__ */


#ifndef __NktModule_FWD_DEFINED__
#define __NktModule_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktModule NktModule;
#else
typedef struct NktModule NktModule;
#endif /* __cplusplus */

#endif 	/* __NktModule_FWD_DEFINED__ */


#ifndef __NktModulesEnum_FWD_DEFINED__
#define __NktModulesEnum_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktModulesEnum NktModulesEnum;
#else
typedef struct NktModulesEnum NktModulesEnum;
#endif /* __cplusplus */

#endif 	/* __NktModulesEnum_FWD_DEFINED__ */


#ifndef __NktExportedFunction_FWD_DEFINED__
#define __NktExportedFunction_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktExportedFunction NktExportedFunction;
#else
typedef struct NktExportedFunction NktExportedFunction;
#endif /* __cplusplus */

#endif 	/* __NktExportedFunction_FWD_DEFINED__ */


#ifndef __NktExportedFunctionsEnum_FWD_DEFINED__
#define __NktExportedFunctionsEnum_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktExportedFunctionsEnum NktExportedFunctionsEnum;
#else
typedef struct NktExportedFunctionsEnum NktExportedFunctionsEnum;
#endif /* __cplusplus */

#endif 	/* __NktExportedFunctionsEnum_FWD_DEFINED__ */


#ifndef __DNktHookEvents_FWD_DEFINED__
#define __DNktHookEvents_FWD_DEFINED__
typedef interface DNktHookEvents DNktHookEvents;
#endif 	/* __DNktHookEvents_FWD_DEFINED__ */


#ifndef __NktHook_FWD_DEFINED__
#define __NktHook_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktHook NktHook;
#else
typedef struct NktHook NktHook;
#endif /* __cplusplus */

#endif 	/* __NktHook_FWD_DEFINED__ */


#ifndef __NktHooksEnum_FWD_DEFINED__
#define __NktHooksEnum_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktHooksEnum NktHooksEnum;
#else
typedef struct NktHooksEnum NktHooksEnum;
#endif /* __cplusplus */

#endif 	/* __NktHooksEnum_FWD_DEFINED__ */


#ifndef __NktParam_FWD_DEFINED__
#define __NktParam_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktParam NktParam;
#else
typedef struct NktParam NktParam;
#endif /* __cplusplus */

#endif 	/* __NktParam_FWD_DEFINED__ */


#ifndef __NktParamsEnum_FWD_DEFINED__
#define __NktParamsEnum_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktParamsEnum NktParamsEnum;
#else
typedef struct NktParamsEnum NktParamsEnum;
#endif /* __cplusplus */

#endif 	/* __NktParamsEnum_FWD_DEFINED__ */


#ifndef __NktHookCallInfo_FWD_DEFINED__
#define __NktHookCallInfo_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktHookCallInfo NktHookCallInfo;
#else
typedef struct NktHookCallInfo NktHookCallInfo;
#endif /* __cplusplus */

#endif 	/* __NktHookCallInfo_FWD_DEFINED__ */


#ifndef __NktHookInfo_FWD_DEFINED__
#define __NktHookInfo_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktHookInfo NktHookInfo;
#else
typedef struct NktHookInfo NktHookInfo;
#endif /* __cplusplus */

#endif 	/* __NktHookInfo_FWD_DEFINED__ */


#ifndef __NktProcessMemory_FWD_DEFINED__
#define __NktProcessMemory_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktProcessMemory NktProcessMemory;
#else
typedef struct NktProcessMemory NktProcessMemory;
#endif /* __cplusplus */

#endif 	/* __NktProcessMemory_FWD_DEFINED__ */


#ifndef __NktStackTrace_FWD_DEFINED__
#define __NktStackTrace_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktStackTrace NktStackTrace;
#else
typedef struct NktStackTrace NktStackTrace;
#endif /* __cplusplus */

#endif 	/* __NktStackTrace_FWD_DEFINED__ */


#ifndef __DNktSpyMgrEvents_FWD_DEFINED__
#define __DNktSpyMgrEvents_FWD_DEFINED__
typedef interface DNktSpyMgrEvents DNktSpyMgrEvents;
#endif 	/* __DNktSpyMgrEvents_FWD_DEFINED__ */


#ifndef __NktSpyMgr_FWD_DEFINED__
#define __NktSpyMgr_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktSpyMgr NktSpyMgr;
#else
typedef struct NktSpyMgr NktSpyMgr;
#endif /* __cplusplus */

#endif 	/* __NktSpyMgr_FWD_DEFINED__ */


#ifndef __NktDbObject_FWD_DEFINED__
#define __NktDbObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktDbObject NktDbObject;
#else
typedef struct NktDbObject NktDbObject;
#endif /* __cplusplus */

#endif 	/* __NktDbObject_FWD_DEFINED__ */


#ifndef __NktDbObjectsEnum_FWD_DEFINED__
#define __NktDbObjectsEnum_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktDbObjectsEnum NktDbObjectsEnum;
#else
typedef struct NktDbObjectsEnum NktDbObjectsEnum;
#endif /* __cplusplus */

#endif 	/* __NktDbObjectsEnum_FWD_DEFINED__ */


#ifndef __NktDbModule_FWD_DEFINED__
#define __NktDbModule_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktDbModule NktDbModule;
#else
typedef struct NktDbModule NktDbModule;
#endif /* __cplusplus */

#endif 	/* __NktDbModule_FWD_DEFINED__ */


#ifndef __NktDbModulesEnum_FWD_DEFINED__
#define __NktDbModulesEnum_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktDbModulesEnum NktDbModulesEnum;
#else
typedef struct NktDbModulesEnum NktDbModulesEnum;
#endif /* __cplusplus */

#endif 	/* __NktDbModulesEnum_FWD_DEFINED__ */


#ifndef __NktTools_FWD_DEFINED__
#define __NktTools_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktTools NktTools;
#else
typedef struct NktTools NktTools;
#endif /* __cplusplus */

#endif 	/* __NktTools_FWD_DEFINED__ */


#ifndef __NktHookCallInfoPlugin_FWD_DEFINED__
#define __NktHookCallInfoPlugin_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktHookCallInfoPlugin NktHookCallInfoPlugin;
#else
typedef struct NktHookCallInfoPlugin NktHookCallInfoPlugin;
#endif /* __cplusplus */

#endif 	/* __NktHookCallInfoPlugin_FWD_DEFINED__ */


#ifndef __NktStructPEFileHeader_FWD_DEFINED__
#define __NktStructPEFileHeader_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktStructPEFileHeader NktStructPEFileHeader;
#else
typedef struct NktStructPEFileHeader NktStructPEFileHeader;
#endif /* __cplusplus */

#endif 	/* __NktStructPEFileHeader_FWD_DEFINED__ */


#ifndef __NktStructPEOptionalHeader_FWD_DEFINED__
#define __NktStructPEOptionalHeader_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktStructPEOptionalHeader NktStructPEOptionalHeader;
#else
typedef struct NktStructPEOptionalHeader NktStructPEOptionalHeader;
#endif /* __cplusplus */

#endif 	/* __NktStructPEOptionalHeader_FWD_DEFINED__ */


#ifndef __NktStructPESections_FWD_DEFINED__
#define __NktStructPESections_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktStructPESections NktStructPESections;
#else
typedef struct NktStructPESections NktStructPESections;
#endif /* __cplusplus */

#endif 	/* __NktStructPESections_FWD_DEFINED__ */


#ifndef __NktPdbFunctionSymbol_FWD_DEFINED__
#define __NktPdbFunctionSymbol_FWD_DEFINED__

#ifdef __cplusplus
typedef class NktPdbFunctionSymbol NktPdbFunctionSymbol;
#else
typedef struct NktPdbFunctionSymbol NktPdbFunctionSymbol;
#endif /* __cplusplus */

#endif 	/* __NktPdbFunctionSymbol_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_DeviareCOM_0000_0000 */
/* [local] */ 

typedef hyper my_ssize_t;

typedef MIDL_uhyper my_size_t;

typedef /* [v1_enum] */ 
enum eNktDispIds
    {	dispidNktObjectUserData	= 1000,
	dispidNktObjectInterfaceError	= ( dispidNktObjectUserData + 1 ) ,
	dispidNktObjectDeviareId	= ( dispidNktObjectInterfaceError + 1 ) ,
	dispidNktObjectToString	= ( dispidNktObjectDeviareId + 1 ) ,
	dispidNktObjectGetObjectFromDeviareId	= ( dispidNktObjectToString + 1 ) ,
	dispidNktProcessId	= 1,
	dispidNktProcessParentId	= ( dispidNktProcessId + 1 ) ,
	dispidNktProcessPlatformBits	= ( dispidNktProcessParentId + 1 ) ,
	dispidNktProcessThreadsCount	= ( dispidNktProcessPlatformBits + 1 ) ,
	dispidNktProcessPriorityBaseClass	= ( dispidNktProcessThreadsCount + 1 ) ,
	dispidNktProcessPath	= ( dispidNktProcessPriorityBaseClass + 1 ) ,
	dispidNktProcessName	= ( dispidNktProcessPath + 1 ) ,
	dispidNktProcessUserName	= ( dispidNktProcessName + 1 ) ,
	dispidNktProcessDomainName	= ( dispidNktProcessUserName + 1 ) ,
	dispidNktProcessLogonSID	= ( dispidNktProcessDomainName + 1 ) ,
	dispidNktProcessIcon	= ( dispidNktProcessLogonSID + 1 ) ,
	dispidNktProcessDefaultIcon	= ( dispidNktProcessIcon + 1 ) ,
	dispidNktProcessIsActive	= ( dispidNktProcessDefaultIcon + 1 ) ,
	dispidNktProcessTerminate	= ( dispidNktProcessIsActive + 1 ) ,
	dispidNktProcessModules	= ( dispidNktProcessTerminate + 1 ) ,
	dispidNktProcessModuleByName	= ( dispidNktProcessModules + 1 ) ,
	dispidNktProcessModuleByAddress	= ( dispidNktProcessModuleByName + 1 ) ,
	dispidNktProcessFunctionByName	= ( dispidNktProcessModuleByAddress + 1 ) ,
	dispidNktProcessFunctionByAddress	= ( dispidNktProcessFunctionByName + 1 ) ,
	dispidNktProcessHandle	= ( dispidNktProcessFunctionByAddress + 1 ) ,
	dispidNktProcessFileHeader	= ( dispidNktProcessHandle + 1 ) ,
	dispidNktProcessOptionalHeader	= ( dispidNktProcessFileHeader + 1 ) ,
	dispidNktProcessSections	= ( dispidNktProcessOptionalHeader + 1 ) ,
	dispidNktProcessPrivilege	= ( dispidNktProcessSections + 1 ) ,
	dispidNktProcessMemory	= ( dispidNktProcessPrivilege + 1 ) ,
	dispidNktProcessInvalidateCache	= ( dispidNktProcessMemory + 1 ) ,
	dispidNktProcessesEnumCount	= 1,
	dispidNktProcessesEnumGetAt	= ( dispidNktProcessesEnumCount + 1 ) ,
	dispidNktProcessesEnumFirst	= ( dispidNktProcessesEnumGetAt + 1 ) ,
	dispidNktProcessesEnumLast	= ( dispidNktProcessesEnumFirst + 1 ) ,
	dispidNktProcessesEnumNext	= ( dispidNktProcessesEnumLast + 1 ) ,
	dispidNktProcessesEnumPrevious	= ( dispidNktProcessesEnumNext + 1 ) ,
	dispidNktProcessesEnumGetByDeviareId	= ( dispidNktProcessesEnumPrevious + 1 ) ,
	dispidNktProcessesEnumGetById	= ( dispidNktProcessesEnumGetByDeviareId + 1 ) ,
	dispidNktProcessesEnumGetByName	= ( dispidNktProcessesEnumGetById + 1 ) ,
	dispidNktModuleProcessId	= 1,
	dispidNktModuleBaseAddress	= ( dispidNktModuleProcessId + 1 ) ,
	dispidNktModuleSize	= ( dispidNktModuleBaseAddress + 1 ) ,
	dispidNktModuleName	= ( dispidNktModuleSize + 1 ) ,
	dispidNktModulePath	= ( dispidNktModuleName + 1 ) ,
	dispidNktModuleFileName	= ( dispidNktModulePath + 1 ) ,
	dispidNktModuleHandle	= ( dispidNktModuleFileName + 1 ) ,
	dispidNktModuleExportedFunctions	= ( dispidNktModuleHandle + 1 ) ,
	dispidNktModuleFunctionByOrdinal	= ( dispidNktModuleExportedFunctions + 1 ) ,
	dispidNktModuleFunctionByName	= ( dispidNktModuleFunctionByOrdinal + 1 ) ,
	dispidNktModuleFunctionByAddress	= ( dispidNktModuleFunctionByName + 1 ) ,
	dispidNktModuleFileHeader	= ( dispidNktModuleFunctionByAddress + 1 ) ,
	dispidNktModuleOptionalHeader	= ( dispidNktModuleFileHeader + 1 ) ,
	dispidNktModuleSections	= ( dispidNktModuleOptionalHeader + 1 ) ,
	dispidNktModulePlatformBits	= ( dispidNktModuleSections + 1 ) ,
	dispidNktModulesEnumCount	= ( dispidNktModulePlatformBits + 1 ) ,
	dispidNktModulesEnumGetAt	= ( dispidNktModulesEnumCount + 1 ) ,
	dispidNktModulesEnumFirst	= ( dispidNktModulesEnumGetAt + 1 ) ,
	dispidNktModulesEnumLast	= ( dispidNktModulesEnumFirst + 1 ) ,
	dispidNktModulesEnumNext	= ( dispidNktModulesEnumLast + 1 ) ,
	dispidNktModulesEnumPrevious	= ( dispidNktModulesEnumNext + 1 ) ,
	dispidNktModulesEnumGetByDeviareId	= ( dispidNktModulesEnumPrevious + 1 ) ,
	dispidNktModulesEnumGetByName	= ( dispidNktModulesEnumGetByDeviareId + 1 ) ,
	dispidNktModulesEnumGetByAddress	= ( dispidNktModulesEnumGetByName + 1 ) ,
	dispidNktModulesEnumFunctionByName	= ( dispidNktModulesEnumGetByAddress + 1 ) ,
	dispidNktModulesEnumFunctionByAddress	= ( dispidNktModulesEnumFunctionByName + 1 ) ,
	dispidNktExportedFunctionOrdinal	= 1,
	dispidNktExportedFunctionName	= ( dispidNktExportedFunctionOrdinal + 1 ) ,
	dispidNktExportedFunctionModuleName	= ( dispidNktExportedFunctionName + 1 ) ,
	dispidNktExportedFunctionCompleteName	= ( dispidNktExportedFunctionModuleName + 1 ) ,
	dispidNktExportedFunctionIsForwarded	= ( dispidNktExportedFunctionCompleteName + 1 ) ,
	dispidNktExportedFunctionForwardedApiName	= ( dispidNktExportedFunctionIsForwarded + 1 ) ,
	dispidNktExportedFunctionForwardedModuleName	= ( dispidNktExportedFunctionForwardedApiName + 1 ) ,
	dispidNktExportedFunctionForwardedCompleteName	= ( dispidNktExportedFunctionForwardedModuleName + 1 ) ,
	dispidNktExportedFunctionAddrBase	= ( dispidNktExportedFunctionForwardedCompleteName + 1 ) ,
	dispidNktExportedFunctionAddrOffset	= ( dispidNktExportedFunctionAddrBase + 1 ) ,
	dispidNktExportedFunctionAddr	= ( dispidNktExportedFunctionAddrOffset + 1 ) ,
	dispidNktExportedFunctionCallingConv	= ( dispidNktExportedFunctionAddr + 1 ) ,
	dispidNktExportedFunctionDbObject	= ( dispidNktExportedFunctionCallingConv + 1 ) ,
	dispidNktExportedFunctionHasDeclaration	= ( dispidNktExportedFunctionDbObject + 1 ) ,
	dispidNktExportedFunctionDeclaration	= ( dispidNktExportedFunctionHasDeclaration + 1 ) ,
	dispidNktExportedFunctionsEnumCount	= 1,
	dispidNktExportedFunctionsEnumGetAt	= ( dispidNktExportedFunctionsEnumCount + 1 ) ,
	dispidNktExportedFunctionsEnumFirst	= ( dispidNktExportedFunctionsEnumGetAt + 1 ) ,
	dispidNktExportedFunctionsEnumLast	= ( dispidNktExportedFunctionsEnumFirst + 1 ) ,
	dispidNktExportedFunctionsEnumNext	= ( dispidNktExportedFunctionsEnumLast + 1 ) ,
	dispidNktExportedFunctionsEnumPrevious	= ( dispidNktExportedFunctionsEnumNext + 1 ) ,
	dispidNktExportedFunctionsEnumGetByDeviareId	= ( dispidNktExportedFunctionsEnumPrevious + 1 ) ,
	dispidNktExportedFunctionsEnumGetByOrdinal	= ( dispidNktExportedFunctionsEnumGetByDeviareId + 1 ) ,
	dispidNktExportedFunctionsEnumGetByName	= ( dispidNktExportedFunctionsEnumGetByOrdinal + 1 ) ,
	dispidNktExportedFunctionsEnumGetByAddress	= ( dispidNktExportedFunctionsEnumGetByName + 1 ) ,
	dispidNktHookId	= 1,
	dispidNktHookDestroy	= ( dispidNktHookId + 1 ) ,
	dispidNktHookAttach	= ( dispidNktHookDestroy + 1 ) ,
	dispidNktHookDetach	= ( dispidNktHookAttach + 1 ) ,
	dispidNktHookEnable	= ( dispidNktHookDetach + 1 ) ,
	dispidNktHookState	= ( dispidNktHookEnable + 1 ) ,
	dispidNktHookHook	= ( dispidNktHookState + 1 ) ,
	dispidNktHookUnhook	= ( dispidNktHookHook + 1 ) ,
	dispidNktHookAddCustomHandler	= ( dispidNktHookUnhook + 1 ) ,
	dispidNktHookRemoveAllHandlers	= ( dispidNktHookAddCustomHandler + 1 ) ,
	dispidNktHookFunctionName	= ( dispidNktHookRemoveAllHandlers + 1 ) ,
	dispidNktHookFlags	= ( dispidNktHookFunctionName + 1 ) ,
	dispidNktHookLastError	= ( dispidNktHookFlags + 1 ) ,
	dispidNktHookQueryOverwrite	= ( dispidNktHookLastError + 1 ) ,
	dispidNktHooksEnumCount	= 1,
	dispidNktHooksEnumGetAt	= ( dispidNktHooksEnumCount + 1 ) ,
	dispidNktHooksEnumFirst	= ( dispidNktHooksEnumGetAt + 1 ) ,
	dispidNktHooksEnumLast	= ( dispidNktHooksEnumFirst + 1 ) ,
	dispidNktHooksEnumNext	= ( dispidNktHooksEnumLast + 1 ) ,
	dispidNktHooksEnumPrevious	= ( dispidNktHooksEnumNext + 1 ) ,
	dispidNktHooksEnumGetByDeviareId	= ( dispidNktHooksEnumPrevious + 1 ) ,
	dispidNktHooksEnumGetById	= ( dispidNktHooksEnumGetByDeviareId + 1 ) ,
	dispidNktHooksEnumAdd	= ( dispidNktHooksEnumGetById + 1 ) ,
	dispidNktHooksEnumAttach	= ( dispidNktHooksEnumAdd + 1 ) ,
	dispidNktHooksEnumDetach	= ( dispidNktHooksEnumAttach + 1 ) ,
	dispidNktHooksEnumEnable	= ( dispidNktHooksEnumDetach + 1 ) ,
	dispidNktHooksEnumHook	= ( dispidNktHooksEnumEnable + 1 ) ,
	dispidNktHooksEnumUnhook	= ( dispidNktHooksEnumHook + 1 ) ,
	dispidNktHooksEnumQueryOverwrite	= ( dispidNktHooksEnumUnhook + 1 ) ,
	dispidNktParamAddress	= 1,
	dispidNktParamClass	= ( dispidNktParamAddress + 1 ) ,
	dispidNktParamBasicType	= ( dispidNktParamClass + 1 ) ,
	dispidNktParamName	= ( dispidNktParamBasicType + 1 ) ,
	dispidNktParamDeclaration	= ( dispidNktParamName + 1 ) ,
	dispidNktParamFlags	= ( dispidNktParamDeclaration + 1 ) ,
	dispidNktParamIsPointer	= ( dispidNktParamFlags + 1 ) ,
	dispidNktParamIsArray	= ( dispidNktParamIsPointer + 1 ) ,
	dispidNktParamArrayMaxHint	= ( dispidNktParamIsArray + 1 ) ,
	dispidNktParamIsAnsiString	= ( dispidNktParamArrayMaxHint + 1 ) ,
	dispidNktParamIsWideString	= ( dispidNktParamIsAnsiString + 1 ) ,
	dispidNktParamIntResourceString	= ( dispidNktParamIsWideString + 1 ) ,
	dispidNktParamGuidString	= ( dispidNktParamIntResourceString + 1 ) ,
	dispidNktParamEvaluate	= ( dispidNktParamGuidString + 1 ) ,
	dispidNktParamIndexedEvaluate	= ( dispidNktParamEvaluate + 1 ) ,
	dispidNktParamFullEvaluate	= ( dispidNktParamIndexedEvaluate + 1 ) ,
	dispidNktParamFieldsCount	= ( dispidNktParamFullEvaluate + 1 ) ,
	dispidNktParamField	= ( dispidNktParamFieldsCount + 1 ) ,
	dispidNktParamFields	= ( dispidNktParamField + 1 ) ,
	dispidNktParamValue	= ( dispidNktParamFields + 1 ) ,
	dispidNktParamValueAt	= ( dispidNktParamValue + 1 ) ,
	dispidNktParamValueArray	= ( dispidNktParamValueAt + 1 ) ,
	dispidNktParamValueArrayAt	= ( dispidNktParamValueArray + 1 ) ,
	dispidNktParamReadString	= ( dispidNktParamValueArrayAt + 1 ) ,
	dispidNktParamReadStringN	= ( dispidNktParamReadString + 1 ) ,
	dispidNktParamWriteString	= ( dispidNktParamReadStringN + 1 ) ,
	dispidNktParamStringLength	= ( dispidNktParamWriteString + 1 ) ,
	dispidNktParamCharVal	= ( dispidNktParamStringLength + 1 ) ,
	dispidNktParamByteVal	= ( dispidNktParamCharVal + 1 ) ,
	dispidNktParamShortVal	= ( dispidNktParamByteVal + 1 ) ,
	dispidNktParamUShortVal	= ( dispidNktParamShortVal + 1 ) ,
	dispidNktParamLongVal	= ( dispidNktParamUShortVal + 1 ) ,
	dispidNktParamULongVal	= ( dispidNktParamLongVal + 1 ) ,
	dispidNktParamLongLongVal	= ( dispidNktParamULongVal + 1 ) ,
	dispidNktParamULongLongVal	= ( dispidNktParamLongLongVal + 1 ) ,
	dispidNktParamSSizeTVal	= ( dispidNktParamULongLongVal + 1 ) ,
	dispidNktParamSizeTVal	= ( dispidNktParamSSizeTVal + 1 ) ,
	dispidNktParamFloatVal	= ( dispidNktParamSizeTVal + 1 ) ,
	dispidNktParamDoubleVal	= ( dispidNktParamFloatVal + 1 ) ,
	dispidNktParamCharValAt	= ( dispidNktParamDoubleVal + 1 ) ,
	dispidNktParamByteValAt	= ( dispidNktParamCharValAt + 1 ) ,
	dispidNktParamShortValAt	= ( dispidNktParamByteValAt + 1 ) ,
	dispidNktParamUShortValAt	= ( dispidNktParamShortValAt + 1 ) ,
	dispidNktParamLongValAt	= ( dispidNktParamUShortValAt + 1 ) ,
	dispidNktParamULongValAt	= ( dispidNktParamLongValAt + 1 ) ,
	dispidNktParamLongLongValAt	= ( dispidNktParamULongValAt + 1 ) ,
	dispidNktParamULongLongValAt	= ( dispidNktParamLongLongValAt + 1 ) ,
	dispidNktParamSSizeTValAt	= ( dispidNktParamULongLongValAt + 1 ) ,
	dispidNktParamSizeTValAt	= ( dispidNktParamSSizeTValAt + 1 ) ,
	dispidNktParamFloatValAt	= ( dispidNktParamSizeTValAt + 1 ) ,
	dispidNktParamDoubleValAt	= ( dispidNktParamFloatValAt + 1 ) ,
	dispidNktParamPointerVal	= ( dispidNktParamDoubleValAt + 1 ) ,
	dispidNktParamIsNullPointer	= ( dispidNktParamPointerVal + 1 ) ,
	dispidNktParamMemory	= ( dispidNktParamIsNullPointer + 1 ) ,
	dispidNktParamCastTo	= ( dispidNktParamMemory + 1 ) ,
	dispidNktParamTypeName	= ( dispidNktParamCastTo + 1 ) ,
	dispidNktParamsEnumCount	= 1,
	dispidNktParamsEnumGetAt	= ( dispidNktParamsEnumCount + 1 ) ,
	dispidNktParamsEnumFirst	= ( dispidNktParamsEnumGetAt + 1 ) ,
	dispidNktParamsEnumLast	= ( dispidNktParamsEnumFirst + 1 ) ,
	dispidNktParamsEnumNext	= ( dispidNktParamsEnumLast + 1 ) ,
	dispidNktParamsEnumPrevious	= ( dispidNktParamsEnumNext + 1 ) ,
	dispidNktParamsEnumGetByDeviareId	= ( dispidNktParamsEnumPrevious + 1 ) ,
	dispidNktHookCallInfoIsPreCall	= 1,
	dispidNktHookCallInfoCookie	= ( dispidNktHookCallInfoIsPreCall + 1 ) ,
	dispidNktHookCallInfoLastError	= ( dispidNktHookCallInfoCookie + 1 ) ,
	dispidNktHookCallInfoCurrentTimeMs	= ( dispidNktHookCallInfoLastError + 1 ) ,
	dispidNktHookCallInfoElapsedTimeMs	= ( dispidNktHookCallInfoCurrentTimeMs + 1 ) ,
	dispidNktHookCallInfoChildsElapsedTimeMs	= ( dispidNktHookCallInfoElapsedTimeMs + 1 ) ,
	dispidNktHookCallInfoHook	= ( dispidNktHookCallInfoChildsElapsedTimeMs + 1 ) ,
	dispidNktHookCallInfoProcess	= ( dispidNktHookCallInfoHook + 1 ) ,
	dispidNktHookCallInfoThreadId	= ( dispidNktHookCallInfoProcess + 1 ) ,
	dispidNktHookCallInfoStackTrace	= ( dispidNktHookCallInfoThreadId + 1 ) ,
	dispidNktHookCallInfoParams	= ( dispidNktHookCallInfoStackTrace + 1 ) ,
	dispidNktHookCallInfoResult	= ( dispidNktHookCallInfoParams + 1 ) ,
	dispidNktHookCallInfoCustomParams	= ( dispidNktHookCallInfoResult + 1 ) ,
	dispidNktHookCallInfoRegister	= ( dispidNktHookCallInfoCustomParams + 1 ) ,
	dispidNktHookCallInfoFloatingRegister	= ( dispidNktHookCallInfoRegister + 1 ) ,
	dispidNktHookCallInfoSkipCall	= ( dispidNktHookCallInfoFloatingRegister + 1 ) ,
	dispidNktHookCallInfoKernelModeTimeMs	= ( dispidNktHookCallInfoSkipCall + 1 ) ,
	dispidNktHookCallInfoUserModeTimeMs	= ( dispidNktHookCallInfoKernelModeTimeMs + 1 ) ,
	dispidNktHookCallInfoCpuClockCycles	= ( dispidNktHookCallInfoUserModeTimeMs + 1 ) ,
	dispidNktHookCallInfoReadIntercallData	= ( dispidNktHookCallInfoCpuClockCycles + 1 ) ,
	dispidNktHookCallInfoWriteIntercallData	= ( dispidNktHookCallInfoReadIntercallData + 1 ) ,
	dispidNktHookCallInfoCallDepth	= ( dispidNktHookCallInfoWriteIntercallData + 1 ) ,
	dispidNktProcessMemoryGetProtection	= 1,
	dispidNktProcessMemoryReadMem	= ( dispidNktProcessMemoryGetProtection + 1 ) ,
	dispidNktProcessMemoryWriteMem	= ( dispidNktProcessMemoryReadMem + 1 ) ,
	dispidNktProcessMemoryRead	= ( dispidNktProcessMemoryWriteMem + 1 ) ,
	dispidNktProcessMemoryReadArray	= ( dispidNktProcessMemoryRead + 1 ) ,
	dispidNktProcessMemoryWrite	= ( dispidNktProcessMemoryReadArray + 1 ) ,
	dispidNktProcessMemoryReadString	= ( dispidNktProcessMemoryWrite + 1 ) ,
	dispidNktProcessMemoryReadStringN	= ( dispidNktProcessMemoryReadString + 1 ) ,
	dispidNktProcessMemoryWriteString	= ( dispidNktProcessMemoryReadStringN + 1 ) ,
	dispidNktProcessMemoryStringLength	= ( dispidNktProcessMemoryWriteString + 1 ) ,
	dispidNktProcessMemoryCharVal	= ( dispidNktProcessMemoryStringLength + 1 ) ,
	dispidNktProcessMemoryByteVal	= ( dispidNktProcessMemoryCharVal + 1 ) ,
	dispidNktProcessMemoryShortVal	= ( dispidNktProcessMemoryByteVal + 1 ) ,
	dispidNktProcessMemoryUShortVal	= ( dispidNktProcessMemoryShortVal + 1 ) ,
	dispidNktProcessMemoryLongVal	= ( dispidNktProcessMemoryUShortVal + 1 ) ,
	dispidNktProcessMemoryULongVal	= ( dispidNktProcessMemoryLongVal + 1 ) ,
	dispidNktProcessMemoryLongLongVal	= ( dispidNktProcessMemoryULongVal + 1 ) ,
	dispidNktProcessMemoryULongLongVal	= ( dispidNktProcessMemoryLongLongVal + 1 ) ,
	dispidNktProcessMemorySSizeTVal	= ( dispidNktProcessMemoryULongLongVal + 1 ) ,
	dispidNktProcessMemorySizeTVal	= ( dispidNktProcessMemorySSizeTVal + 1 ) ,
	dispidNktProcessMemoryFloatVal	= ( dispidNktProcessMemorySizeTVal + 1 ) ,
	dispidNktProcessMemoryDoubleVal	= ( dispidNktProcessMemoryFloatVal + 1 ) ,
	dispidNktProcessMemoryAllocMem	= ( dispidNktProcessMemoryDoubleVal + 1 ) ,
	dispidNktProcessMemoryFreeMem	= ( dispidNktProcessMemoryAllocMem + 1 ) ,
	dispidNktProcessMemoryBuildParam	= ( dispidNktProcessMemoryFreeMem + 1 ) ,
	dispidNktStackTraceNearestFunction	= 1,
	dispidNktStackTraceModule	= ( dispidNktStackTraceNearestFunction + 1 ) ,
	dispidNktStackTraceOffset	= ( dispidNktStackTraceModule + 1 ) ,
	dispidNktStackTraceAddress	= ( dispidNktStackTraceOffset + 1 ) ,
	dispidNktStackTraceNearestSymbol	= ( dispidNktStackTraceAddress + 1 ) ,
	dispidNktStackTraceParams	= ( dispidNktStackTraceNearestSymbol + 1 ) ,
	dispidNktStackTraceResult	= ( dispidNktStackTraceParams + 1 ) ,
	dispidNktSpyMgrLicenseKey	= 1,
	dispidNktSpyMgrDatabasePath	= ( dispidNktSpyMgrLicenseKey + 1 ) ,
	dispidNktSpyMgrAgentPath	= ( dispidNktSpyMgrDatabasePath + 1 ) ,
	dispidNktSpyMgrInitialize	= ( dispidNktSpyMgrAgentPath + 1 ) ,
	dispidNktSpyMgrProcesses	= ( dispidNktSpyMgrInitialize + 1 ) ,
	dispidNktSpyMgrHooks	= ( dispidNktSpyMgrProcesses + 1 ) ,
	dispidNktSpyMgrCreateHook	= ( dispidNktSpyMgrHooks + 1 ) ,
	dispidNktSpyMgrCreateHookForAddress	= ( dispidNktSpyMgrCreateHook + 1 ) ,
	dispidNktSpyMgrDestroyHook	= ( dispidNktSpyMgrCreateHookForAddress + 1 ) ,
	dispidNktSpyMgrLoadCustomDll	= ( dispidNktSpyMgrDestroyHook + 1 ) ,
	dispidNktSpyMgrUnloadCustomDll	= ( dispidNktSpyMgrLoadCustomDll + 1 ) ,
	dispidNktSpyMgrCallCustomApi	= ( dispidNktSpyMgrUnloadCustomDll + 1 ) ,
	dispidNktSpyMgrProcessFromPID	= ( dispidNktSpyMgrCallCustomApi + 1 ) ,
	dispidNktSpyMgrProcessMemoryFromPID	= ( dispidNktSpyMgrProcessFromPID + 1 ) ,
	dispidNktSpyMgrCreateProcess	= ( dispidNktSpyMgrProcessMemoryFromPID + 1 ) ,
	dispidNktSpyMgrResumeProcess	= ( dispidNktSpyMgrCreateProcess + 1 ) ,
	dispidNktSpyMgrTerminateProcess	= ( dispidNktSpyMgrResumeProcess + 1 ) ,
	dispidNktSpyMgrFindProcessId	= ( dispidNktSpyMgrTerminateProcess + 1 ) ,
	dispidNktSpyMgrRealFilePath	= ( dispidNktSpyMgrFindProcessId + 1 ) ,
	dispidNktSpyMgrDbFunctions	= ( dispidNktSpyMgrRealFilePath + 1 ) ,
	dispidNktSpyMgrDbModules	= ( dispidNktSpyMgrDbFunctions + 1 ) ,
	dispidNktSpyMgrProcessHandle	= ( dispidNktSpyMgrDbModules + 1 ) ,
	dispidNktSpyMgrCheckProduct	= ( dispidNktSpyMgrProcessHandle + 1 ) ,
	dispidNktSpyMgrLicenseEMail	= ( dispidNktSpyMgrCheckProduct + 1 ) ,
	dispidNktSpyMgrGetWindowProc	= ( dispidNktSpyMgrLicenseEMail + 1 ) ,
	dispidNktSpyMgrDbObjects	= ( dispidNktSpyMgrGetWindowProc + 1 ) ,
	dispidNktSpyMgrCreateHooksCollection	= ( dispidNktSpyMgrDbObjects + 1 ) ,
	dispidNktSpyMgrLoadAgent	= ( dispidNktSpyMgrCreateHooksCollection + 1 ) ,
	dispidNktSpyMgrProcessMemory	= ( dispidNktSpyMgrLoadAgent + 1 ) ,
	dispidNktSpyMgrCreateProcessWithLogon	= ( dispidNktSpyMgrProcessMemory + 1 ) ,
	dispidNktSpyMgrSettingOverride	= ( dispidNktSpyMgrCreateProcessWithLogon + 1 ) ,
	dispidNktSpyMgrUnloadAgent	= ( dispidNktSpyMgrSettingOverride + 1 ) ,
	dispidNktSpyMgrCreateProcessWithToken	= ( dispidNktSpyMgrUnloadAgent + 1 ) ,
	dispidNktHookCallInfoPluginIsPreCall	= 1,
	dispidNktHookCallInfoPluginCookie	= ( dispidNktHookCallInfoPluginIsPreCall + 1 ) ,
	dispidNktHookCallInfoPluginLastError	= ( dispidNktHookCallInfoPluginCookie + 1 ) ,
	dispidNktHookCallInfoPluginCurrentTimeMs	= ( dispidNktHookCallInfoPluginLastError + 1 ) ,
	dispidNktHookCallInfoPluginElapsedTimeMs	= ( dispidNktHookCallInfoPluginCurrentTimeMs + 1 ) ,
	dispidNktHookCallInfoPluginChildsElapsedTimeMs	= ( dispidNktHookCallInfoPluginElapsedTimeMs + 1 ) ,
	dispidNktHookCallInfoPluginHookInfo	= ( dispidNktHookCallInfoPluginChildsElapsedTimeMs + 1 ) ,
	dispidNktHookCallInfoPluginSubIndex	= ( dispidNktHookCallInfoPluginHookInfo + 1 ) ,
	dispidNktHookCallInfoPluginThreadId	= ( dispidNktHookCallInfoPluginSubIndex + 1 ) ,
	dispidNktHookCallInfoPluginStackTrace	= ( dispidNktHookCallInfoPluginThreadId + 1 ) ,
	dispidNktHookCallInfoPluginParams	= ( dispidNktHookCallInfoPluginStackTrace + 1 ) ,
	dispidNktHookCallInfoPluginResult	= ( dispidNktHookCallInfoPluginParams + 1 ) ,
	dispidNktHookCallInfoPluginCustomParams	= ( dispidNktHookCallInfoPluginResult + 1 ) ,
	dispidNktHookCallInfoPluginRegister	= ( dispidNktHookCallInfoPluginCustomParams + 1 ) ,
	dispidNktHookCallInfoPluginFloatingRegister	= ( dispidNktHookCallInfoPluginRegister + 1 ) ,
	dispidNktHookCallInfoPluginSkipCall	= ( dispidNktHookCallInfoPluginFloatingRegister + 1 ) ,
	dispidNktHookCallInfoPluginAdd	= ( dispidNktHookCallInfoPluginSkipCall + 1 ) ,
	dispidNktHookCallInfoPluginAddString	= ( dispidNktHookCallInfoPluginAdd + 1 ) ,
	dispidNktHookCallInfoPluginAddChar	= ( dispidNktHookCallInfoPluginAddString + 1 ) ,
	dispidNktHookCallInfoPluginAddByte	= ( dispidNktHookCallInfoPluginAddChar + 1 ) ,
	dispidNktHookCallInfoPluginAddShort	= ( dispidNktHookCallInfoPluginAddByte + 1 ) ,
	dispidNktHookCallInfoPluginAddUShort	= ( dispidNktHookCallInfoPluginAddShort + 1 ) ,
	dispidNktHookCallInfoPluginAddLong	= ( dispidNktHookCallInfoPluginAddUShort + 1 ) ,
	dispidNktHookCallInfoPluginAddULong	= ( dispidNktHookCallInfoPluginAddLong + 1 ) ,
	dispidNktHookCallInfoPluginAddLongLong	= ( dispidNktHookCallInfoPluginAddULong + 1 ) ,
	dispidNktHookCallInfoPluginAddULongLong	= ( dispidNktHookCallInfoPluginAddLongLong + 1 ) ,
	dispidNktHookCallInfoPluginAddSSizeT	= ( dispidNktHookCallInfoPluginAddULongLong + 1 ) ,
	dispidNktHookCallInfoPluginAddSizeT	= ( dispidNktHookCallInfoPluginAddSSizeT + 1 ) ,
	dispidNktHookCallInfoPluginAddFloat	= ( dispidNktHookCallInfoPluginAddSizeT + 1 ) ,
	dispidNktHookCallInfoPluginAddDouble	= ( dispidNktHookCallInfoPluginAddFloat + 1 ) ,
	dispidNktHookCallInfoPluginInvalidate	= ( dispidNktHookCallInfoPluginAddDouble + 1 ) ,
	dispidNktHookCallInfoPluginFilterSpyMgrEvent	= ( dispidNktHookCallInfoPluginInvalidate + 1 ) ,
	dispidNktHookCallInfoPluginKernelModeTimeMs	= ( dispidNktHookCallInfoPluginFilterSpyMgrEvent + 1 ) ,
	dispidNktHookCallInfoPluginUserModeTimeMs	= ( dispidNktHookCallInfoPluginKernelModeTimeMs + 1 ) ,
	dispidNktHookCallInfoPluginCpuClockCycles	= ( dispidNktHookCallInfoPluginUserModeTimeMs + 1 ) ,
	dispidNktHookCallInfoPluginSendCustomMessage	= ( dispidNktHookCallInfoPluginCpuClockCycles + 1 ) ,
	dispidNktHookCallInfoPluginCurrentProcess	= ( dispidNktHookCallInfoPluginSendCustomMessage + 1 ) ,
	dispidNktHookCallInfoPluginReadIntercallData	= ( dispidNktHookCallInfoPluginCurrentProcess + 1 ) ,
	dispidNktHookCallInfoPluginWriteIntercallData	= ( dispidNktHookCallInfoPluginReadIntercallData + 1 ) ,
	dispidNktHookCallInfoPluginCallDepth	= ( dispidNktHookCallInfoPluginWriteIntercallData + 1 ) ,
	dispidNktHookInfoId	= 1,
	dispidNktHookInfoFunctionName	= ( dispidNktHookInfoId + 1 ) ,
	dispidNktHookInfoAddress	= ( dispidNktHookInfoFunctionName + 1 ) ,
	dispidNktHookInfoDbObject	= ( dispidNktHookInfoAddress + 1 ) ,
	dispidNktHookInfoSendCustomMessage	= ( dispidNktHookInfoDbObject + 1 ) ,
	dispidNktHookInfoThreadIgnore	= ( dispidNktHookInfoSendCustomMessage + 1 ) ,
	dispidNktHookInfoInvalidateCache	= ( dispidNktHookInfoThreadIgnore + 1 ) ,
	dispidNktHookInfoCurrentProcess	= ( dispidNktHookInfoInvalidateCache + 1 ) ,
	dispidNktDbObjectId	= 1,
	dispidNktDbObjectClass	= ( dispidNktDbObjectId + 1 ) ,
	dispidNktDbObjectFundamentalType	= ( dispidNktDbObjectClass + 1 ) ,
	dispidNktDbObjectDeclaration	= ( dispidNktDbObjectFundamentalType + 1 ) ,
	dispidNktDbObjectName	= ( dispidNktDbObjectDeclaration + 1 ) ,
	dispidNktDbObjectSize	= ( dispidNktDbObjectName + 1 ) ,
	dispidNktDbObjectAlign	= ( dispidNktDbObjectSize + 1 ) ,
	dispidNktDbObjectIsArray	= ( dispidNktDbObjectAlign + 1 ) ,
	dispidNktDbObjectArrayMaxHint	= ( dispidNktDbObjectIsArray + 1 ) ,
	dispidNktDbObjectFlags	= ( dispidNktDbObjectArrayMaxHint + 1 ) ,
	dispidNktDbObjectItemsCount	= ( dispidNktDbObjectFlags + 1 ) ,
	dispidNktDbObjectNameToIndex	= ( dispidNktDbObjectItemsCount + 1 ) ,
	dispidNktDbObjectItem	= ( dispidNktDbObjectNameToIndex + 1 ) ,
	dispidNktDbObjectItems	= ( dispidNktDbObjectItem + 1 ) ,
	dispidNktDbObjectItemName	= ( dispidNktDbObjectItems + 1 ) ,
	dispidNktDbObjectItemFlags	= ( dispidNktDbObjectItemName + 1 ) ,
	dispidNktDbObjectItemMemberType	= ( dispidNktDbObjectItemFlags + 1 ) ,
	dispidNktDbObjectItemStructOffset	= ( dispidNktDbObjectItemMemberType + 1 ) ,
	dispidNktDbObjectItemStructBits	= ( dispidNktDbObjectItemStructOffset + 1 ) ,
	dispidNktDbObjectItemEnumValue	= ( dispidNktDbObjectItemStructBits + 1 ) ,
	dispidNktDbObjectItemDeclaration	= ( dispidNktDbObjectItemEnumValue + 1 ) ,
	dispidNktDbObjectFuncCallingConvention	= ( dispidNktDbObjectItemDeclaration + 1 ) ,
	dispidNktDbObjectFunctionReturn	= ( dispidNktDbObjectFuncCallingConvention + 1 ) ,
	dispidNktDbObjectFunctionReturnFlags	= ( dispidNktDbObjectFunctionReturn + 1 ) ,
	dispidNktDbObjectFunctionReturnDeclaration	= ( dispidNktDbObjectFunctionReturnFlags + 1 ) ,
	dispidNktDbObjectDbModules	= ( dispidNktDbObjectFunctionReturnDeclaration + 1 ) ,
	dispidNktDbObjectIsAnsiString	= ( dispidNktDbObjectDbModules + 1 ) ,
	dispidNktDbObjectIsWideString	= ( dispidNktDbObjectIsAnsiString + 1 ) ,
	dispidNktDbObjectIsGuid	= ( dispidNktDbObjectIsWideString + 1 ) ,
	dispidNktDbObjectEvaluate	= ( dispidNktDbObjectIsGuid + 1 ) ,
	dispidNktDbObjectIndexedEvaluate	= ( dispidNktDbObjectEvaluate + 1 ) ,
	dispidNktDbObjectFullEvaluate	= ( dispidNktDbObjectIndexedEvaluate + 1 ) ,
	dispidNktDbObjectsEnumCount	= 1,
	dispidNktDbObjectsEnumGetAt	= ( dispidNktDbObjectsEnumCount + 1 ) ,
	dispidNktDbObjectsEnumFirst	= ( dispidNktDbObjectsEnumGetAt + 1 ) ,
	dispidNktDbObjectsEnumLast	= ( dispidNktDbObjectsEnumFirst + 1 ) ,
	dispidNktDbObjectsEnumNext	= ( dispidNktDbObjectsEnumLast + 1 ) ,
	dispidNktDbObjectsEnumPrevious	= ( dispidNktDbObjectsEnumNext + 1 ) ,
	dispidNktDbObjectsEnumGetById	= ( dispidNktDbObjectsEnumPrevious + 1 ) ,
	dispidNktDbObjectsEnumGetByName	= ( dispidNktDbObjectsEnumGetById + 1 ) ,
	dispidNktDbModuleId	= 1,
	dispidNktDbModuleName	= ( dispidNktDbModuleId + 1 ) ,
	dispidNktDbModuleDbFunctions	= ( dispidNktDbModuleName + 1 ) ,
	dispidNktDbModulesEnumCount	= 1,
	dispidNktDbModulesEnumGetAt	= ( dispidNktDbModulesEnumCount + 1 ) ,
	dispidNktDbModulesEnumFirst	= ( dispidNktDbModulesEnumGetAt + 1 ) ,
	dispidNktDbModulesEnumLast	= ( dispidNktDbModulesEnumFirst + 1 ) ,
	dispidNktDbModulesEnumNext	= ( dispidNktDbModulesEnumLast + 1 ) ,
	dispidNktDbModulesEnumPrevious	= ( dispidNktDbModulesEnumNext + 1 ) ,
	dispidNktDbModulesEnumGetById	= ( dispidNktDbModulesEnumPrevious + 1 ) ,
	dispidNktDbModulesEnumGetByName	= ( dispidNktDbModulesEnumGetById + 1 ) ,
	dispidNktToolsExtractIcon	= 1,
	dispidNktToolsGetFileNameFromHandle	= ( dispidNktToolsExtractIcon + 1 ) ,
	dispidNktToolsLocateFunctionSymbolInPdb	= ( dispidNktToolsGetFileNameFromHandle + 1 ) ,
	dispidNktProcessEventsOnCustomDllLoad	= 1,
	dispidNktProcessEventsOnCustomDllUnload	= ( dispidNktProcessEventsOnCustomDllLoad + 1 ) ,
	dispidNktProcessEventsOnCustomApiCall	= ( dispidNktProcessEventsOnCustomDllUnload + 1 ) ,
	dispidNktProcessEventsOnHookStateChanged	= ( dispidNktProcessEventsOnCustomApiCall + 1 ) ,
	dispidNktProcessEventsOnFunctionCalled	= ( dispidNktProcessEventsOnHookStateChanged + 1 ) ,
	dispidNktProcessEventsOnCreateProcessCall	= ( dispidNktProcessEventsOnFunctionCalled + 1 ) ,
	dispidNktProcessEventsOnLoadLibraryCall	= ( dispidNktProcessEventsOnCreateProcessCall + 1 ) ,
	dispidNktProcessEventsOnFreeLibraryCall	= ( dispidNktProcessEventsOnLoadLibraryCall + 1 ) ,
	dispidNktProcessEventsOnAgentLoad	= ( dispidNktProcessEventsOnFreeLibraryCall + 1 ) ,
	dispidNktProcessEventsOnAgentUnload	= ( dispidNktProcessEventsOnAgentLoad + 1 ) ,
	dispidNktProcessEventsOnLoadLibraryCall2	= ( dispidNktProcessEventsOnAgentUnload + 1 ) ,
	dispidNktProcessEventsOnFreeLibraryCall2	= ( dispidNktProcessEventsOnLoadLibraryCall2 + 1 ) ,
	dispidNktProcessEventsOnCustomMessage	= ( dispidNktProcessEventsOnFreeLibraryCall2 + 1 ) ,
	dispidNktProcessEventsOnCustomMessage2	= ( dispidNktProcessEventsOnCustomMessage + 1 ) ,
	dispidNktProcessEventsOnHookOverwritten	= ( dispidNktProcessEventsOnCustomMessage2 + 1 ) ,
	dispidNktHookEventsOnStateChanged	= 1,
	dispidNktHookEventsOnFunctionCalled	= ( dispidNktHookEventsOnStateChanged + 1 ) ,
	dispidNktHookEventsOnHookOverwritten	= ( dispidNktHookEventsOnFunctionCalled + 1 ) ,
	dispidNktSpyMgrEventsOnCustomDllLoad	= 1,
	dispidNktSpyMgrEventsOnCustomDllUnload	= ( dispidNktSpyMgrEventsOnCustomDllLoad + 1 ) ,
	dispidNktSpyMgrEventsOnCustomApiCall	= ( dispidNktSpyMgrEventsOnCustomDllUnload + 1 ) ,
	dispidNktSpyMgrEventsOnHookStateChanged	= ( dispidNktSpyMgrEventsOnCustomApiCall + 1 ) ,
	dispidNktSpyMgrEventsOnFunctionCalled	= ( dispidNktSpyMgrEventsOnHookStateChanged + 1 ) ,
	dispidNktSpyMgrEventsOnCreateProcessCall	= ( dispidNktSpyMgrEventsOnFunctionCalled + 1 ) ,
	dispidNktSpyMgrEventsOnLoadLibraryCall	= ( dispidNktSpyMgrEventsOnCreateProcessCall + 1 ) ,
	dispidNktSpyMgrEventsOnFreeLibraryCall	= ( dispidNktSpyMgrEventsOnLoadLibraryCall + 1 ) ,
	dispidNktSpyMgrEventsOnProcessStarted	= ( dispidNktSpyMgrEventsOnFreeLibraryCall + 1 ) ,
	dispidNktSpyMgrEventsOnProcessTerminated	= ( dispidNktSpyMgrEventsOnProcessStarted + 1 ) ,
	dispidNktSpyMgrEventsOnAgentLoad	= ( dispidNktSpyMgrEventsOnProcessTerminated + 1 ) ,
	dispidNktSpyMgrEventsOnAgentUnload	= ( dispidNktSpyMgrEventsOnAgentLoad + 1 ) ,
	dispidNktSpyMgrEventsOnLoadLibraryCall2	= ( dispidNktSpyMgrEventsOnAgentUnload + 1 ) ,
	dispidNktSpyMgrEventsOnFreeLibraryCall2	= ( dispidNktSpyMgrEventsOnLoadLibraryCall2 + 1 ) ,
	dispidNktSpyMgrEventsOnCustomMessage	= ( dispidNktSpyMgrEventsOnFreeLibraryCall2 + 1 ) ,
	dispidNktSpyMgrEventsOnCustomMessage2	= ( dispidNktSpyMgrEventsOnCustomMessage + 1 ) ,
	dispidNktSpyMgrEventsOnHookOverwritten	= ( dispidNktSpyMgrEventsOnCustomMessage2 + 1 ) ,
	dispidNktHookCallInfoPluginEventsOnSetup	= 1,
	dispidNktHookCallInfoPluginEventsOnFunctionCalled	= ( dispidNktHookCallInfoPluginEventsOnSetup + 1 ) ,
	dispidNktStructPEFileHeaderMachine	= 1,
	dispidNktStructPEFileHeaderNumberOfSections	= ( dispidNktStructPEFileHeaderMachine + 1 ) ,
	dispidNktStructPEFileHeaderTimeDateStamp	= ( dispidNktStructPEFileHeaderNumberOfSections + 1 ) ,
	dispidNktStructPEFileHeaderPointerToSymbolTable	= ( dispidNktStructPEFileHeaderTimeDateStamp + 1 ) ,
	dispidNktStructPEFileHeaderNumberOfSymbols	= ( dispidNktStructPEFileHeaderPointerToSymbolTable + 1 ) ,
	dispidNktStructPEFileHeaderSizeOfOptionalHeader	= ( dispidNktStructPEFileHeaderNumberOfSymbols + 1 ) ,
	dispidNktStructPEFileHeaderCharacteristics	= ( dispidNktStructPEFileHeaderSizeOfOptionalHeader + 1 ) ,
	dispidNktStructPEOptionalHeaderMagic	= 1,
	dispidNktStructPEOptionalHeaderMajorLinkerVersion	= ( dispidNktStructPEOptionalHeaderMagic + 1 ) ,
	dispidNktStructPEOptionalHeaderMinorLinkerVersion	= ( dispidNktStructPEOptionalHeaderMajorLinkerVersion + 1 ) ,
	dispidNktStructPEOptionalHeaderSizeOfCode	= ( dispidNktStructPEOptionalHeaderMinorLinkerVersion + 1 ) ,
	dispidNktStructPEOptionalHeaderSizeOfInitializedData	= ( dispidNktStructPEOptionalHeaderSizeOfCode + 1 ) ,
	dispidNktStructPEOptionalHeaderSizeOfUninitializedData	= ( dispidNktStructPEOptionalHeaderSizeOfInitializedData + 1 ) ,
	dispidNktStructPEOptionalHeaderAddressOfEntryPoint	= ( dispidNktStructPEOptionalHeaderSizeOfUninitializedData + 1 ) ,
	dispidNktStructPEOptionalHeaderBaseOfCode	= ( dispidNktStructPEOptionalHeaderAddressOfEntryPoint + 1 ) ,
	dispidNktStructPEOptionalHeaderImageBase	= ( dispidNktStructPEOptionalHeaderBaseOfCode + 1 ) ,
	dispidNktStructPEOptionalHeaderSectionAlignment	= ( dispidNktStructPEOptionalHeaderImageBase + 1 ) ,
	dispidNktStructPEOptionalHeaderFileAlignment	= ( dispidNktStructPEOptionalHeaderSectionAlignment + 1 ) ,
	dispidNktStructPEOptionalHeaderMajorOperatingSystemVersion	= ( dispidNktStructPEOptionalHeaderFileAlignment + 1 ) ,
	dispidNktStructPEOptionalHeaderMinorOperatingSystemVersion	= ( dispidNktStructPEOptionalHeaderMajorOperatingSystemVersion + 1 ) ,
	dispidNktStructPEOptionalHeaderMajorImageVersion	= ( dispidNktStructPEOptionalHeaderMinorOperatingSystemVersion + 1 ) ,
	dispidNktStructPEOptionalHeaderMinorImageVersion	= ( dispidNktStructPEOptionalHeaderMajorImageVersion + 1 ) ,
	dispidNktStructPEOptionalHeaderMajorSubsystemVersion	= ( dispidNktStructPEOptionalHeaderMinorImageVersion + 1 ) ,
	dispidNktStructPEOptionalHeaderMinorSubsystemVersion	= ( dispidNktStructPEOptionalHeaderMajorSubsystemVersion + 1 ) ,
	dispidNktStructPEOptionalHeaderWin32VersionValue	= ( dispidNktStructPEOptionalHeaderMinorSubsystemVersion + 1 ) ,
	dispidNktStructPEOptionalHeaderSizeOfImage	= ( dispidNktStructPEOptionalHeaderWin32VersionValue + 1 ) ,
	dispidNktStructPEOptionalHeaderSizeOfHeaders	= ( dispidNktStructPEOptionalHeaderSizeOfImage + 1 ) ,
	dispidNktStructPEOptionalHeaderCheckSum	= ( dispidNktStructPEOptionalHeaderSizeOfHeaders + 1 ) ,
	dispidNktStructPEOptionalHeaderSubsystem	= ( dispidNktStructPEOptionalHeaderCheckSum + 1 ) ,
	dispidNktStructPEOptionalHeaderDllCharacteristics	= ( dispidNktStructPEOptionalHeaderSubsystem + 1 ) ,
	dispidNktStructPEOptionalHeaderSizeOfStackReserve	= ( dispidNktStructPEOptionalHeaderDllCharacteristics + 1 ) ,
	dispidNktStructPEOptionalHeaderSizeOfStackCommit	= ( dispidNktStructPEOptionalHeaderSizeOfStackReserve + 1 ) ,
	dispidNktStructPEOptionalHeaderSizeOfHeapReserve	= ( dispidNktStructPEOptionalHeaderSizeOfStackCommit + 1 ) ,
	dispidNktStructPEOptionalHeaderSizeOfHeapCommit	= ( dispidNktStructPEOptionalHeaderSizeOfHeapReserve + 1 ) ,
	dispidNktStructPEOptionalHeaderLoaderFlags	= ( dispidNktStructPEOptionalHeaderSizeOfHeapCommit + 1 ) ,
	dispidNktStructPEOptionalHeaderNumberOfRvaAndSizes	= ( dispidNktStructPEOptionalHeaderLoaderFlags + 1 ) ,
	dispidNktStructPEOptionalHeaderDataDirectoryStart	= ( dispidNktStructPEOptionalHeaderNumberOfRvaAndSizes + 1 ) ,
	dispidNktStructPEOptionalHeaderDataDirectoryEnd	= ( dispidNktStructPEOptionalHeaderDataDirectoryStart + 1 ) ,
	dispidNktStructPESectionsCount	= 1,
	dispidNktStructPESectionsName	= ( dispidNktStructPESectionsCount + 1 ) ,
	dispidNktStructPESectionsStartAddress	= ( dispidNktStructPESectionsName + 1 ) ,
	dispidNktStructPESectionsEndAddress	= ( dispidNktStructPESectionsStartAddress + 1 ) ,
	dispidNktStructPESectionsCharacteristics	= ( dispidNktStructPESectionsEndAddress + 1 ) ,
	dispidNktPdbFunctionSymbolAddrOffset	= 1,
	dispidNktPdbFunctionSymbolPlatformBits	= ( dispidNktPdbFunctionSymbolAddrOffset + 1 ) ,
	dispidNktPdbFunctionSymbolName	= ( dispidNktPdbFunctionSymbolPlatformBits + 1 ) ,
	dispidNktPdbFunctionSymbolModuleName	= ( dispidNktPdbFunctionSymbolName + 1 ) ,
	dispidNktPdbFunctionSymbolCompleteName	= ( dispidNktPdbFunctionSymbolModuleName + 1 ) 
    } 	eNktDispIds;



























typedef /* [v1_enum] */ 
enum eNktErrorCodes
    {	errCannotLoadDatabase	= 0x8a320001L,
	errOnlyOneInstance	= 0x8a320002L,
	errProtocolVersionMismatch	= 0x8a320003L,
	errInvalidTransportData	= 0x8a320004L,
	errNotFound	= 0x80070490L,
	errArithmeticOverflow	= 0x80070216L,
	errAlreadyExists	= 0x800700b7L,
	errAlreadyInitialized	= 0x800704dfL,
	errCancelled	= 0x800704c7L,
	errExceptionRaised	= 0x80020009L,
	errHookIsInactive	= 0x8a320005L,
	errPartialCopy	= 0x8007012bL,
	errFail	= 0x80004005L,
	errOutOfMemory	= 0x8007000eL,
	errNotImpl	= 0x80004001L,
	errInvalidArg	= 0x80070057L,
	errPointer	= 0x80004003L
    } 	eNktErrorCodes;

typedef /* [v1_enum] */ 
enum eNktHookState
    {	stInactive	= 0,
	stActivating	= ( stInactive + 1 ) ,
	stActive	= ( stActivating + 1 ) ,
	stDisabled	= ( stActive + 1 ) ,
	stRemoved	= ( stDisabled + 1 ) ,
	stNotAvailable	= ( stRemoved + 1 ) ,
	stNotFound	= -1,
	stError	= -2
    } 	eNktHookState;

typedef /* [v1_enum] */ 
enum eNktHookFlags
    {	flgAutoHookChildProcess	= 0x1,
	flgRestrictAutoHookToSameExecutable	= 0x2,
	flgAutoHookActive	= 0x4,
	flgAsyncCallbacks	= 0x8,
	flgOnlyPreCall	= 0x10,
	flgOnlyPostCall	= 0x20,
	flgDontCheckAddress	= 0x40,
	flgDontCallIfLoaderLocked	= 0x80,
	flgDontCallCustomHandlersOnLdrLock	= 0x100,
	flgOnly32Bits	= 0x200,
	flgOnly64Bits	= 0x400,
	flgAddressIsOffset	= 0x800,
	flgInvalidateCache	= 0x1000,
	flgDontSkipJumps	= 0x2000,
	flgDisableStackWalk	= 0x4000
    } 	eNktHookFlags;

typedef /* [v1_enum] */ 
enum eNktHookCustomHandlerFlags
    {	flgChDontCallIfLoaderLocked	= 0x1,
	flgChDisableExceptionChecks	= 0x2
    } 	eNktHookCustomHandlerFlags;

typedef /* [v1_enum] */ 
enum eNktSearchMode
    {	smExactMatch	= 0,
	smFindContaining	= ( smExactMatch + 1 ) ,
	smGetNearest	= ( smFindContaining + 1 ) 
    } 	eNktSearchMode;

typedef /* [v1_enum] */ 
enum eNktRegister
    {	asmRegNone	= 0,
	asmRegEax	= 1,
	asmRegEbx	= 2,
	asmRegEcx	= 3,
	asmRegEdx	= 4,
	asmRegEaxEdxPair	= 5,
	asmRegSt0	= 6,
	asmRegEsp	= 7,
	asmRegEbp	= 8,
	asmRegEip	= 9,
	asmRegEsi	= 10,
	asmRegEdi	= 11,
	asmRegRax	= 1,
	asmRegRbx	= 2,
	asmRegRcx	= 3,
	asmRegRdx	= 4,
	asmRegR8	= 5,
	asmRegR9	= 6,
	asmRegR10	= 7,
	asmRegR11	= 8,
	asmRegXmm0	= 9,
	asmRegXmm1	= 10,
	asmRegXmm2	= 11,
	asmRegXmm3	= 12,
	asmRegRsp	= 13,
	asmRegRbp	= 14,
	asmRegRip	= 15,
	asmRegRsi	= 16,
	asmRegRdi	= 17
    } 	eNktRegister;

typedef /* [v1_enum] */ 
enum eNktProtection
    {	protExecute	= 0x1,
	protRead	= 0x2,
	protWrite	= 0x4,
	protIsWriteCopy	= 0x8,
	protGuard	= 0x10,
	protNoCache	= 0x20,
	protWriteCombine	= 0x40
    } 	eNktProtection;

typedef /* [v1_enum] */ 
enum eNktDboFundamentalType
    {	ftNone	= 0,
	ftSignedByte	= ( ftNone + 1 ) ,
	ftUnsignedByte	= ( ftSignedByte + 1 ) ,
	ftSignedWord	= ( ftUnsignedByte + 1 ) ,
	ftUnsignedWord	= ( ftSignedWord + 1 ) ,
	ftSignedDoubleWord	= ( ftUnsignedWord + 1 ) ,
	ftUnsignedDoubleWord	= ( ftSignedDoubleWord + 1 ) ,
	ftSignedQuadWord	= ( ftUnsignedDoubleWord + 1 ) ,
	ftUnsignedQuadWord	= ( ftSignedQuadWord + 1 ) ,
	ftFloat	= ( ftUnsignedQuadWord + 1 ) ,
	ftDouble	= ( ftFloat + 1 ) ,
	ftLongDouble	= ( ftDouble + 1 ) ,
	ftVoid	= ( ftLongDouble + 1 ) ,
	ftAnsiChar	= ( ftVoid + 1 ) ,
	ftWideChar	= ( ftAnsiChar + 1 ) 
    } 	eNktDboFundamentalType;

typedef /* [v1_enum] */ 
enum eNktDboClass
    {	clsNull	= -1,
	clsFundamental	= 0,
	clsStruct	= ( clsFundamental + 1 ) ,
	clsUnion	= ( clsStruct + 1 ) ,
	clsArray	= ( clsUnion + 1 ) ,
	clsPointer	= ( clsArray + 1 ) ,
	clsReference	= ( clsPointer + 1 ) ,
	clsEnumeration	= ( clsReference + 1 ) ,
	clsFunction	= ( clsEnumeration + 1 ) ,
	clsFunctionType	= ( clsFunction + 1 ) ,
	clsClassConstructor	= ( clsFunctionType + 1 ) ,
	clsClassDestructor	= ( clsClassConstructor + 1 ) ,
	clsClassOperatorMethod	= ( clsClassDestructor + 1 ) ,
	clsClassMethod	= ( clsClassOperatorMethod + 1 ) ,
	clsClassConverter	= ( clsClassMethod + 1 ) 
    } 	eNktDboClass;

typedef /* [v1_enum] */ 
enum eNktDboCallingConvention
    {	ccNA	= -1,
	ccStdCall	= 0,
	ccCDecl	= 1,
	ccFastCall	= 2,
	ccThisCall	= 3
    } 	eNktDboCallingConvention;

typedef /* [v1_enum] */ 
enum eNktDboFunctionFlags
    {	fflgIsPure	= 0x10,
	fflgIsThrow	= 0x20,
	fflgIsNoThrow	= 0x40,
	fflgIsNoReturn	= 0x80,
	fflgIsConst	= 0x100,
	fflgIsDeprecated	= 0x200,
	fflgIsNonNull	= 0x400,
	fflgIsMalloc	= 0x800,
	fflgIsExternal	= 0x1000,
	fflgIsDllImport	= 0x2000,
	fflgIsDllExport	= 0x4000,
	fflgIsFormat	= 0x8000
    } 	eNktDboFunctionFlags;

typedef /* [v1_enum] */ 
enum eNktDboStructUnionFlags
    {	suflgHasConstructor	= 0x10,
	suflgHasDestructor	= 0x20,
	suflgHasVirtual	= 0x40,
	suflgHasInheritance	= 0x80
    } 	eNktDboStructUnionFlags;

typedef /* [v1_enum] */ 
enum eNktDboItemFlags
    {	iflgIsVolatile	= 0x1,
	iflgIsConstant	= 0x2
    } 	eNktDboItemFlags;

typedef /* [v1_enum] */ 
enum eNktDboItemMemberType
    {	mtNA	= -1,
	mtPublic	= 0x10,
	mtPrivate	= 0x20,
	mtProtected	= 0x40
    } 	eNktDboItemMemberType;



extern RPC_IF_HANDLE __MIDL_itf_DeviareCOM_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DeviareCOM_0000_0000_v0_0_s_ifspec;

#ifndef __INktObject_INTERFACE_DEFINED__
#define __INktObject_INTERFACE_DEFINED__

/* interface INktObject */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3B886748-DC9B-43B0-B761-68DAD9E5484F")
    INktObject : public IDispatch
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_UserData( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_UserData( 
            /* [in] */ my_ssize_t newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_InterfaceError( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DeviareId( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ToString( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetObjectFromDeviareId( 
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktObject * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktObject * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktObject * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktObject * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktObject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktObject * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        END_INTERFACE
    } INktObjectVtbl;

    interface INktObject
    {
        CONST_VTBL struct INktObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktObject_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktObject_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktObject_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktObject_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktObject_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktObject_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktObject_get_UserData_Proxy( 
    INktObject * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktObject_get_UserData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktObject_put_UserData_Proxy( 
    INktObject * This,
    /* [in] */ my_ssize_t newValue);


void __RPC_STUB INktObject_put_UserData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktObject_get_InterfaceError_Proxy( 
    INktObject * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktObject_get_InterfaceError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktObject_get_DeviareId_Proxy( 
    INktObject * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktObject_get_DeviareId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktObject_ToString_Proxy( 
    INktObject * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktObject_ToString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktObject_GetObjectFromDeviareId_Proxy( 
    INktObject * This,
    /* [in] */ my_ssize_t devId,
    /* [retval][out] */ IDispatch **ppDisp);


void __RPC_STUB INktObject_GetObjectFromDeviareId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktObject_INTERFACE_DEFINED__ */


#ifndef __INktProcess_INTERFACE_DEFINED__
#define __INktProcess_INTERFACE_DEFINED__

/* interface INktProcess */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktProcess;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("32EC8909-95FE-483c-A5BA-BEE86470C210")
    INktProcess : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Id( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ParentId( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PlatformBits( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ThreadsCount( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PriorityBaseClass( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_UserName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DomainName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LogonSID( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Icon( 
            /* [retval][out] */ IPicture **icon) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DefaultIcon( 
            /* [retval][out] */ IPicture **icon) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsActive( 
            /* [defaultvalue][in] */ LONG waitTimeMs,
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Terminate( 
            /* [defaultvalue][in] */ LONG exitCode = 0) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Modules( 
            /* [retval][out] */ INktModulesEnum **ppModEnum) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ModuleByName( 
            /* [in] */ BSTR name,
            /* [retval][out] */ INktModule **ppMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ModuleByAddress( 
            /* [in] */ my_ssize_t addr,
            /* [in] */ eNktSearchMode searchMode,
            /* [retval][out] */ INktModule **ppMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE FunctionByName( 
            /* [in] */ BSTR name,
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE FunctionByAddress( 
            /* [in] */ my_ssize_t addr,
            /* [in] */ eNktSearchMode searchMode,
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Handle( 
            /* [in] */ LONG desiredAccess,
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE FileHeader( 
            /* [retval][out] */ INktStructPEFileHeader **ppVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE OptionalHeader( 
            /* [retval][out] */ INktStructPEOptionalHeader **ppVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Sections( 
            /* [retval][out] */ INktStructPESections **ppVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Privilege( 
            /* [in] */ BSTR privilegeName,
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Privilege( 
            /* [in] */ BSTR privilegeName,
            /* [in] */ VARIANT_BOOL newValue) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Memory( 
            /* [retval][out] */ INktProcessMemory **ppProcMem) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE InvalidateCache( 
            /* [in] */ my_ssize_t hDll) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktProcessVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktProcess * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktProcess * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktProcess * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktProcess * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktProcess * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktProcess * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktProcess * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktProcess * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktProcess * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktProcess * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktProcess * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktProcess * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktProcess * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Id )( 
            INktProcess * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ParentId )( 
            INktProcess * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PlatformBits )( 
            INktProcess * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ThreadsCount )( 
            INktProcess * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PriorityBaseClass )( 
            INktProcess * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            INktProcess * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            INktProcess * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserName )( 
            INktProcess * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DomainName )( 
            INktProcess * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LogonSID )( 
            INktProcess * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Icon )( 
            INktProcess * This,
            /* [retval][out] */ IPicture **icon);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DefaultIcon )( 
            INktProcess * This,
            /* [retval][out] */ IPicture **icon);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsActive )( 
            INktProcess * This,
            /* [defaultvalue][in] */ LONG waitTimeMs,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Terminate )( 
            INktProcess * This,
            /* [defaultvalue][in] */ LONG exitCode);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Modules )( 
            INktProcess * This,
            /* [retval][out] */ INktModulesEnum **ppModEnum);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ModuleByName )( 
            INktProcess * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ INktModule **ppMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ModuleByAddress )( 
            INktProcess * This,
            /* [in] */ my_ssize_t addr,
            /* [in] */ eNktSearchMode searchMode,
            /* [retval][out] */ INktModule **ppMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FunctionByName )( 
            INktProcess * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FunctionByAddress )( 
            INktProcess * This,
            /* [in] */ my_ssize_t addr,
            /* [in] */ eNktSearchMode searchMode,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Handle )( 
            INktProcess * This,
            /* [in] */ LONG desiredAccess,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FileHeader )( 
            INktProcess * This,
            /* [retval][out] */ INktStructPEFileHeader **ppVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *OptionalHeader )( 
            INktProcess * This,
            /* [retval][out] */ INktStructPEOptionalHeader **ppVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Sections )( 
            INktProcess * This,
            /* [retval][out] */ INktStructPESections **ppVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Privilege )( 
            INktProcess * This,
            /* [in] */ BSTR privilegeName,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Privilege )( 
            INktProcess * This,
            /* [in] */ BSTR privilegeName,
            /* [in] */ VARIANT_BOOL newValue);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Memory )( 
            INktProcess * This,
            /* [retval][out] */ INktProcessMemory **ppProcMem);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *InvalidateCache )( 
            INktProcess * This,
            /* [in] */ my_ssize_t hDll);
        
        END_INTERFACE
    } INktProcessVtbl;

    interface INktProcess
    {
        CONST_VTBL struct INktProcessVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktProcess_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktProcess_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktProcess_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktProcess_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktProcess_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktProcess_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktProcess_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktProcess_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktProcess_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktProcess_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktProcess_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktProcess_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktProcess_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktProcess_get_Id(This,pVal)	\
    ( (This)->lpVtbl -> get_Id(This,pVal) ) 

#define INktProcess_get_ParentId(This,pVal)	\
    ( (This)->lpVtbl -> get_ParentId(This,pVal) ) 

#define INktProcess_get_PlatformBits(This,pVal)	\
    ( (This)->lpVtbl -> get_PlatformBits(This,pVal) ) 

#define INktProcess_get_ThreadsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_ThreadsCount(This,pVal) ) 

#define INktProcess_get_PriorityBaseClass(This,pVal)	\
    ( (This)->lpVtbl -> get_PriorityBaseClass(This,pVal) ) 

#define INktProcess_get_Path(This,pVal)	\
    ( (This)->lpVtbl -> get_Path(This,pVal) ) 

#define INktProcess_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define INktProcess_get_UserName(This,pVal)	\
    ( (This)->lpVtbl -> get_UserName(This,pVal) ) 

#define INktProcess_get_DomainName(This,pVal)	\
    ( (This)->lpVtbl -> get_DomainName(This,pVal) ) 

#define INktProcess_get_LogonSID(This,pVal)	\
    ( (This)->lpVtbl -> get_LogonSID(This,pVal) ) 

#define INktProcess_get_Icon(This,icon)	\
    ( (This)->lpVtbl -> get_Icon(This,icon) ) 

#define INktProcess_get_DefaultIcon(This,icon)	\
    ( (This)->lpVtbl -> get_DefaultIcon(This,icon) ) 

#define INktProcess_get_IsActive(This,waitTimeMs,pVal)	\
    ( (This)->lpVtbl -> get_IsActive(This,waitTimeMs,pVal) ) 

#define INktProcess_Terminate(This,exitCode)	\
    ( (This)->lpVtbl -> Terminate(This,exitCode) ) 

#define INktProcess_Modules(This,ppModEnum)	\
    ( (This)->lpVtbl -> Modules(This,ppModEnum) ) 

#define INktProcess_ModuleByName(This,name,ppMod)	\
    ( (This)->lpVtbl -> ModuleByName(This,name,ppMod) ) 

#define INktProcess_ModuleByAddress(This,addr,searchMode,ppMod)	\
    ( (This)->lpVtbl -> ModuleByAddress(This,addr,searchMode,ppMod) ) 

#define INktProcess_FunctionByName(This,name,ppExpFunc)	\
    ( (This)->lpVtbl -> FunctionByName(This,name,ppExpFunc) ) 

#define INktProcess_FunctionByAddress(This,addr,searchMode,ppExpFunc)	\
    ( (This)->lpVtbl -> FunctionByAddress(This,addr,searchMode,ppExpFunc) ) 

#define INktProcess_Handle(This,desiredAccess,pVal)	\
    ( (This)->lpVtbl -> Handle(This,desiredAccess,pVal) ) 

#define INktProcess_FileHeader(This,ppVal)	\
    ( (This)->lpVtbl -> FileHeader(This,ppVal) ) 

#define INktProcess_OptionalHeader(This,ppVal)	\
    ( (This)->lpVtbl -> OptionalHeader(This,ppVal) ) 

#define INktProcess_Sections(This,ppVal)	\
    ( (This)->lpVtbl -> Sections(This,ppVal) ) 

#define INktProcess_get_Privilege(This,privilegeName,pVal)	\
    ( (This)->lpVtbl -> get_Privilege(This,privilegeName,pVal) ) 

#define INktProcess_put_Privilege(This,privilegeName,newValue)	\
    ( (This)->lpVtbl -> put_Privilege(This,privilegeName,newValue) ) 

#define INktProcess_Memory(This,ppProcMem)	\
    ( (This)->lpVtbl -> Memory(This,ppProcMem) ) 

#define INktProcess_InvalidateCache(This,hDll)	\
    ( (This)->lpVtbl -> InvalidateCache(This,hDll) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcess_get_Id_Proxy( 
    INktProcess * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktProcess_get_Id_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcess_get_ParentId_Proxy( 
    INktProcess * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktProcess_get_ParentId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcess_get_PlatformBits_Proxy( 
    INktProcess * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktProcess_get_PlatformBits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcess_get_ThreadsCount_Proxy( 
    INktProcess * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktProcess_get_ThreadsCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcess_get_PriorityBaseClass_Proxy( 
    INktProcess * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktProcess_get_PriorityBaseClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcess_get_Path_Proxy( 
    INktProcess * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktProcess_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcess_get_Name_Proxy( 
    INktProcess * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktProcess_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcess_get_UserName_Proxy( 
    INktProcess * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktProcess_get_UserName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcess_get_DomainName_Proxy( 
    INktProcess * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktProcess_get_DomainName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcess_get_LogonSID_Proxy( 
    INktProcess * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktProcess_get_LogonSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcess_get_Icon_Proxy( 
    INktProcess * This,
    /* [retval][out] */ IPicture **icon);


void __RPC_STUB INktProcess_get_Icon_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcess_get_DefaultIcon_Proxy( 
    INktProcess * This,
    /* [retval][out] */ IPicture **icon);


void __RPC_STUB INktProcess_get_DefaultIcon_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcess_get_IsActive_Proxy( 
    INktProcess * This,
    /* [defaultvalue][in] */ LONG waitTimeMs,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktProcess_get_IsActive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcess_Terminate_Proxy( 
    INktProcess * This,
    /* [defaultvalue][in] */ LONG exitCode);


void __RPC_STUB INktProcess_Terminate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcess_Modules_Proxy( 
    INktProcess * This,
    /* [retval][out] */ INktModulesEnum **ppModEnum);


void __RPC_STUB INktProcess_Modules_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcess_ModuleByName_Proxy( 
    INktProcess * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ INktModule **ppMod);


void __RPC_STUB INktProcess_ModuleByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcess_ModuleByAddress_Proxy( 
    INktProcess * This,
    /* [in] */ my_ssize_t addr,
    /* [in] */ eNktSearchMode searchMode,
    /* [retval][out] */ INktModule **ppMod);


void __RPC_STUB INktProcess_ModuleByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcess_FunctionByName_Proxy( 
    INktProcess * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktProcess_FunctionByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcess_FunctionByAddress_Proxy( 
    INktProcess * This,
    /* [in] */ my_ssize_t addr,
    /* [in] */ eNktSearchMode searchMode,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktProcess_FunctionByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcess_Handle_Proxy( 
    INktProcess * This,
    /* [in] */ LONG desiredAccess,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktProcess_Handle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcess_FileHeader_Proxy( 
    INktProcess * This,
    /* [retval][out] */ INktStructPEFileHeader **ppVal);


void __RPC_STUB INktProcess_FileHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcess_OptionalHeader_Proxy( 
    INktProcess * This,
    /* [retval][out] */ INktStructPEOptionalHeader **ppVal);


void __RPC_STUB INktProcess_OptionalHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcess_Sections_Proxy( 
    INktProcess * This,
    /* [retval][out] */ INktStructPESections **ppVal);


void __RPC_STUB INktProcess_Sections_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcess_get_Privilege_Proxy( 
    INktProcess * This,
    /* [in] */ BSTR privilegeName,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktProcess_get_Privilege_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktProcess_put_Privilege_Proxy( 
    INktProcess * This,
    /* [in] */ BSTR privilegeName,
    /* [in] */ VARIANT_BOOL newValue);


void __RPC_STUB INktProcess_put_Privilege_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcess_Memory_Proxy( 
    INktProcess * This,
    /* [retval][out] */ INktProcessMemory **ppProcMem);


void __RPC_STUB INktProcess_Memory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcess_InvalidateCache_Proxy( 
    INktProcess * This,
    /* [in] */ my_ssize_t hDll);


void __RPC_STUB INktProcess_InvalidateCache_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktProcess_INTERFACE_DEFINED__ */


#ifndef __INktProcessesEnum_INTERFACE_DEFINED__
#define __INktProcessesEnum_INTERFACE_DEFINED__

/* interface INktProcessesEnum */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktProcessesEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DF43831B-7A0B-43d4-AC40-6136A7C95B28")
    INktProcessesEnum : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE First( 
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Last( 
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Previous( 
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetByDeviareId( 
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetById( 
            /* [in] */ LONG id,
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetByName( 
            /* [in] */ BSTR name,
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
        virtual /* [restricted][id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktProcessesEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktProcessesEnum * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktProcessesEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktProcessesEnum * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktProcessesEnum * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktProcessesEnum * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktProcessesEnum * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktProcessesEnum * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktProcessesEnum * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktProcessesEnum * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktProcessesEnum * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktProcessesEnum * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktProcessesEnum * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktProcessesEnum * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            INktProcessesEnum * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            INktProcessesEnum * This,
            /* [in] */ LONG index,
            /* [retval][out] */ INktProcess **ppProc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *First )( 
            INktProcessesEnum * This,
            /* [retval][out] */ INktProcess **ppProc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Last )( 
            INktProcessesEnum * This,
            /* [retval][out] */ INktProcess **ppProc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            INktProcessesEnum * This,
            /* [retval][out] */ INktProcess **ppProc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Previous )( 
            INktProcessesEnum * This,
            /* [retval][out] */ INktProcess **ppProc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetByDeviareId )( 
            INktProcessesEnum * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ INktProcess **ppProc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetById )( 
            INktProcessesEnum * This,
            /* [in] */ LONG id,
            /* [retval][out] */ INktProcess **ppProc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetByName )( 
            INktProcessesEnum * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ INktProcess **ppProc);
        
        /* [restricted][id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            INktProcessesEnum * This,
            /* [retval][out] */ IUnknown **ppUnk);
        
        END_INTERFACE
    } INktProcessesEnumVtbl;

    interface INktProcessesEnum
    {
        CONST_VTBL struct INktProcessesEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktProcessesEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktProcessesEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktProcessesEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktProcessesEnum_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktProcessesEnum_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktProcessesEnum_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktProcessesEnum_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktProcessesEnum_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktProcessesEnum_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktProcessesEnum_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktProcessesEnum_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktProcessesEnum_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktProcessesEnum_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktProcessesEnum_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define INktProcessesEnum_GetAt(This,index,ppProc)	\
    ( (This)->lpVtbl -> GetAt(This,index,ppProc) ) 

#define INktProcessesEnum_First(This,ppProc)	\
    ( (This)->lpVtbl -> First(This,ppProc) ) 

#define INktProcessesEnum_Last(This,ppProc)	\
    ( (This)->lpVtbl -> Last(This,ppProc) ) 

#define INktProcessesEnum_Next(This,ppProc)	\
    ( (This)->lpVtbl -> Next(This,ppProc) ) 

#define INktProcessesEnum_Previous(This,ppProc)	\
    ( (This)->lpVtbl -> Previous(This,ppProc) ) 

#define INktProcessesEnum_GetByDeviareId(This,devId,ppProc)	\
    ( (This)->lpVtbl -> GetByDeviareId(This,devId,ppProc) ) 

#define INktProcessesEnum_GetById(This,id,ppProc)	\
    ( (This)->lpVtbl -> GetById(This,id,ppProc) ) 

#define INktProcessesEnum_GetByName(This,name,ppProc)	\
    ( (This)->lpVtbl -> GetByName(This,name,ppProc) ) 

#define INktProcessesEnum_get__NewEnum(This,ppUnk)	\
    ( (This)->lpVtbl -> get__NewEnum(This,ppUnk) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcessesEnum_get_Count_Proxy( 
    INktProcessesEnum * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktProcessesEnum_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessesEnum_GetAt_Proxy( 
    INktProcessesEnum * This,
    /* [in] */ LONG index,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktProcessesEnum_GetAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessesEnum_First_Proxy( 
    INktProcessesEnum * This,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktProcessesEnum_First_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessesEnum_Last_Proxy( 
    INktProcessesEnum * This,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktProcessesEnum_Last_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessesEnum_Next_Proxy( 
    INktProcessesEnum * This,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktProcessesEnum_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessesEnum_Previous_Proxy( 
    INktProcessesEnum * This,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktProcessesEnum_Previous_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessesEnum_GetByDeviareId_Proxy( 
    INktProcessesEnum * This,
    /* [in] */ my_ssize_t devId,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktProcessesEnum_GetByDeviareId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessesEnum_GetById_Proxy( 
    INktProcessesEnum * This,
    /* [in] */ LONG id,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktProcessesEnum_GetById_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessesEnum_GetByName_Proxy( 
    INktProcessesEnum * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktProcessesEnum_GetByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcessesEnum_get__NewEnum_Proxy( 
    INktProcessesEnum * This,
    /* [retval][out] */ IUnknown **ppUnk);


void __RPC_STUB INktProcessesEnum_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktProcessesEnum_INTERFACE_DEFINED__ */


#ifndef __INktModule_INTERFACE_DEFINED__
#define __INktModule_INTERFACE_DEFINED__

/* interface INktModule */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktModule;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F12AAEB4-3CD8-435F-BF6B-7D2C2AFFD4EC")
    INktModule : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ProcessId( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PlatformBits( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BaseAddress( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FileName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Handle( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ExportedFunctions( 
            /* [retval][out] */ INktExportedFunctionsEnum **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE FunctionByOrdinal( 
            /* [in] */ LONG ordinal,
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE FunctionByName( 
            /* [in] */ BSTR name,
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE FunctionByAddress( 
            /* [in] */ my_ssize_t addr,
            /* [defaultvalue][in] */ VARIANT_BOOL searchNearest,
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE FileHeader( 
            /* [retval][out] */ INktStructPEFileHeader **ppVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE OptionalHeader( 
            /* [retval][out] */ INktStructPEOptionalHeader **ppVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Sections( 
            /* [retval][out] */ INktStructPESections **ppVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktModuleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktModule * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktModule * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktModule * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktModule * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktModule * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktModule * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktModule * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktModule * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktModule * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktModule * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktModule * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktModule * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktModule * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ProcessId )( 
            INktModule * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PlatformBits )( 
            INktModule * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BaseAddress )( 
            INktModule * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            INktModule * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            INktModule * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            INktModule * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FileName )( 
            INktModule * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Handle )( 
            INktModule * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ExportedFunctions )( 
            INktModule * This,
            /* [retval][out] */ INktExportedFunctionsEnum **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FunctionByOrdinal )( 
            INktModule * This,
            /* [in] */ LONG ordinal,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FunctionByName )( 
            INktModule * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FunctionByAddress )( 
            INktModule * This,
            /* [in] */ my_ssize_t addr,
            /* [defaultvalue][in] */ VARIANT_BOOL searchNearest,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FileHeader )( 
            INktModule * This,
            /* [retval][out] */ INktStructPEFileHeader **ppVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *OptionalHeader )( 
            INktModule * This,
            /* [retval][out] */ INktStructPEOptionalHeader **ppVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Sections )( 
            INktModule * This,
            /* [retval][out] */ INktStructPESections **ppVal);
        
        END_INTERFACE
    } INktModuleVtbl;

    interface INktModule
    {
        CONST_VTBL struct INktModuleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktModule_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktModule_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktModule_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktModule_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktModule_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktModule_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktModule_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktModule_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktModule_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktModule_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktModule_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktModule_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktModule_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktModule_get_ProcessId(This,pVal)	\
    ( (This)->lpVtbl -> get_ProcessId(This,pVal) ) 

#define INktModule_get_PlatformBits(This,pVal)	\
    ( (This)->lpVtbl -> get_PlatformBits(This,pVal) ) 

#define INktModule_get_BaseAddress(This,pVal)	\
    ( (This)->lpVtbl -> get_BaseAddress(This,pVal) ) 

#define INktModule_get_Size(This,pVal)	\
    ( (This)->lpVtbl -> get_Size(This,pVal) ) 

#define INktModule_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define INktModule_get_Path(This,pVal)	\
    ( (This)->lpVtbl -> get_Path(This,pVal) ) 

#define INktModule_get_FileName(This,pVal)	\
    ( (This)->lpVtbl -> get_FileName(This,pVal) ) 

#define INktModule_get_Handle(This,pVal)	\
    ( (This)->lpVtbl -> get_Handle(This,pVal) ) 

#define INktModule_ExportedFunctions(This,ppExpFunc)	\
    ( (This)->lpVtbl -> ExportedFunctions(This,ppExpFunc) ) 

#define INktModule_FunctionByOrdinal(This,ordinal,ppExpFunc)	\
    ( (This)->lpVtbl -> FunctionByOrdinal(This,ordinal,ppExpFunc) ) 

#define INktModule_FunctionByName(This,name,ppExpFunc)	\
    ( (This)->lpVtbl -> FunctionByName(This,name,ppExpFunc) ) 

#define INktModule_FunctionByAddress(This,addr,searchNearest,ppExpFunc)	\
    ( (This)->lpVtbl -> FunctionByAddress(This,addr,searchNearest,ppExpFunc) ) 

#define INktModule_FileHeader(This,ppVal)	\
    ( (This)->lpVtbl -> FileHeader(This,ppVal) ) 

#define INktModule_OptionalHeader(This,ppVal)	\
    ( (This)->lpVtbl -> OptionalHeader(This,ppVal) ) 

#define INktModule_Sections(This,ppVal)	\
    ( (This)->lpVtbl -> Sections(This,ppVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktModule_get_ProcessId_Proxy( 
    INktModule * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktModule_get_ProcessId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktModule_get_PlatformBits_Proxy( 
    INktModule * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktModule_get_PlatformBits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktModule_get_BaseAddress_Proxy( 
    INktModule * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktModule_get_BaseAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktModule_get_Size_Proxy( 
    INktModule * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktModule_get_Size_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktModule_get_Name_Proxy( 
    INktModule * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktModule_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktModule_get_Path_Proxy( 
    INktModule * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktModule_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktModule_get_FileName_Proxy( 
    INktModule * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktModule_get_FileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktModule_get_Handle_Proxy( 
    INktModule * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktModule_get_Handle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModule_ExportedFunctions_Proxy( 
    INktModule * This,
    /* [retval][out] */ INktExportedFunctionsEnum **ppExpFunc);


void __RPC_STUB INktModule_ExportedFunctions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModule_FunctionByOrdinal_Proxy( 
    INktModule * This,
    /* [in] */ LONG ordinal,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktModule_FunctionByOrdinal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModule_FunctionByName_Proxy( 
    INktModule * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktModule_FunctionByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModule_FunctionByAddress_Proxy( 
    INktModule * This,
    /* [in] */ my_ssize_t addr,
    /* [defaultvalue][in] */ VARIANT_BOOL searchNearest,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktModule_FunctionByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModule_FileHeader_Proxy( 
    INktModule * This,
    /* [retval][out] */ INktStructPEFileHeader **ppVal);


void __RPC_STUB INktModule_FileHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModule_OptionalHeader_Proxy( 
    INktModule * This,
    /* [retval][out] */ INktStructPEOptionalHeader **ppVal);


void __RPC_STUB INktModule_OptionalHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModule_Sections_Proxy( 
    INktModule * This,
    /* [retval][out] */ INktStructPESections **ppVal);


void __RPC_STUB INktModule_Sections_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktModule_INTERFACE_DEFINED__ */


#ifndef __INktModulesEnum_INTERFACE_DEFINED__
#define __INktModulesEnum_INTERFACE_DEFINED__

/* interface INktModulesEnum */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktModulesEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("382A6E42-3E59-40d7-BA40-AFD10611C5B8")
    INktModulesEnum : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ INktModule **ppMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE First( 
            /* [retval][out] */ INktModule **ppMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Last( 
            /* [retval][out] */ INktModule **ppMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ INktModule **ppMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Previous( 
            /* [retval][out] */ INktModule **ppMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetByDeviareId( 
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ INktModule **ppMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetByName( 
            /* [in] */ BSTR dllName,
            /* [retval][out] */ INktModule **ppMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetByAddress( 
            /* [in] */ my_ssize_t addr,
            /* [in] */ eNktSearchMode searchMode,
            /* [retval][out] */ INktModule **ppMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE FunctionByName( 
            /* [in] */ BSTR dllFunc,
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE FunctionByAddress( 
            /* [in] */ my_ssize_t addr,
            /* [in] */ eNktSearchMode searchMode,
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [restricted][id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktModulesEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktModulesEnum * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktModulesEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktModulesEnum * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktModulesEnum * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktModulesEnum * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktModulesEnum * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktModulesEnum * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktModulesEnum * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktModulesEnum * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktModulesEnum * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktModulesEnum * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktModulesEnum * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktModulesEnum * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            INktModulesEnum * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            INktModulesEnum * This,
            /* [in] */ LONG index,
            /* [retval][out] */ INktModule **ppMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *First )( 
            INktModulesEnum * This,
            /* [retval][out] */ INktModule **ppMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Last )( 
            INktModulesEnum * This,
            /* [retval][out] */ INktModule **ppMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            INktModulesEnum * This,
            /* [retval][out] */ INktModule **ppMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Previous )( 
            INktModulesEnum * This,
            /* [retval][out] */ INktModule **ppMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetByDeviareId )( 
            INktModulesEnum * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ INktModule **ppMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetByName )( 
            INktModulesEnum * This,
            /* [in] */ BSTR dllName,
            /* [retval][out] */ INktModule **ppMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetByAddress )( 
            INktModulesEnum * This,
            /* [in] */ my_ssize_t addr,
            /* [in] */ eNktSearchMode searchMode,
            /* [retval][out] */ INktModule **ppMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FunctionByName )( 
            INktModulesEnum * This,
            /* [in] */ BSTR dllFunc,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FunctionByAddress )( 
            INktModulesEnum * This,
            /* [in] */ my_ssize_t addr,
            /* [in] */ eNktSearchMode searchMode,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [restricted][id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            INktModulesEnum * This,
            /* [retval][out] */ IUnknown **ppUnk);
        
        END_INTERFACE
    } INktModulesEnumVtbl;

    interface INktModulesEnum
    {
        CONST_VTBL struct INktModulesEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktModulesEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktModulesEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktModulesEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktModulesEnum_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktModulesEnum_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktModulesEnum_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktModulesEnum_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktModulesEnum_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktModulesEnum_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktModulesEnum_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktModulesEnum_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktModulesEnum_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktModulesEnum_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktModulesEnum_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define INktModulesEnum_GetAt(This,index,ppMod)	\
    ( (This)->lpVtbl -> GetAt(This,index,ppMod) ) 

#define INktModulesEnum_First(This,ppMod)	\
    ( (This)->lpVtbl -> First(This,ppMod) ) 

#define INktModulesEnum_Last(This,ppMod)	\
    ( (This)->lpVtbl -> Last(This,ppMod) ) 

#define INktModulesEnum_Next(This,ppMod)	\
    ( (This)->lpVtbl -> Next(This,ppMod) ) 

#define INktModulesEnum_Previous(This,ppMod)	\
    ( (This)->lpVtbl -> Previous(This,ppMod) ) 

#define INktModulesEnum_GetByDeviareId(This,devId,ppMod)	\
    ( (This)->lpVtbl -> GetByDeviareId(This,devId,ppMod) ) 

#define INktModulesEnum_GetByName(This,dllName,ppMod)	\
    ( (This)->lpVtbl -> GetByName(This,dllName,ppMod) ) 

#define INktModulesEnum_GetByAddress(This,addr,searchMode,ppMod)	\
    ( (This)->lpVtbl -> GetByAddress(This,addr,searchMode,ppMod) ) 

#define INktModulesEnum_FunctionByName(This,dllFunc,ppExpFunc)	\
    ( (This)->lpVtbl -> FunctionByName(This,dllFunc,ppExpFunc) ) 

#define INktModulesEnum_FunctionByAddress(This,addr,searchMode,ppExpFunc)	\
    ( (This)->lpVtbl -> FunctionByAddress(This,addr,searchMode,ppExpFunc) ) 

#define INktModulesEnum_get__NewEnum(This,ppUnk)	\
    ( (This)->lpVtbl -> get__NewEnum(This,ppUnk) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktModulesEnum_get_Count_Proxy( 
    INktModulesEnum * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktModulesEnum_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModulesEnum_GetAt_Proxy( 
    INktModulesEnum * This,
    /* [in] */ LONG index,
    /* [retval][out] */ INktModule **ppMod);


void __RPC_STUB INktModulesEnum_GetAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModulesEnum_First_Proxy( 
    INktModulesEnum * This,
    /* [retval][out] */ INktModule **ppMod);


void __RPC_STUB INktModulesEnum_First_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModulesEnum_Last_Proxy( 
    INktModulesEnum * This,
    /* [retval][out] */ INktModule **ppMod);


void __RPC_STUB INktModulesEnum_Last_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModulesEnum_Next_Proxy( 
    INktModulesEnum * This,
    /* [retval][out] */ INktModule **ppMod);


void __RPC_STUB INktModulesEnum_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModulesEnum_Previous_Proxy( 
    INktModulesEnum * This,
    /* [retval][out] */ INktModule **ppMod);


void __RPC_STUB INktModulesEnum_Previous_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModulesEnum_GetByDeviareId_Proxy( 
    INktModulesEnum * This,
    /* [in] */ my_ssize_t devId,
    /* [retval][out] */ INktModule **ppMod);


void __RPC_STUB INktModulesEnum_GetByDeviareId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModulesEnum_GetByName_Proxy( 
    INktModulesEnum * This,
    /* [in] */ BSTR dllName,
    /* [retval][out] */ INktModule **ppMod);


void __RPC_STUB INktModulesEnum_GetByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModulesEnum_GetByAddress_Proxy( 
    INktModulesEnum * This,
    /* [in] */ my_ssize_t addr,
    /* [in] */ eNktSearchMode searchMode,
    /* [retval][out] */ INktModule **ppMod);


void __RPC_STUB INktModulesEnum_GetByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModulesEnum_FunctionByName_Proxy( 
    INktModulesEnum * This,
    /* [in] */ BSTR dllFunc,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktModulesEnum_FunctionByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktModulesEnum_FunctionByAddress_Proxy( 
    INktModulesEnum * This,
    /* [in] */ my_ssize_t addr,
    /* [in] */ eNktSearchMode searchMode,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktModulesEnum_FunctionByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktModulesEnum_get__NewEnum_Proxy( 
    INktModulesEnum * This,
    /* [retval][out] */ IUnknown **ppUnk);


void __RPC_STUB INktModulesEnum_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktModulesEnum_INTERFACE_DEFINED__ */


#ifndef __INktExportedFunction_INTERFACE_DEFINED__
#define __INktExportedFunction_INTERFACE_DEFINED__

/* interface INktExportedFunction */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktExportedFunction;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1B91884B-E65C-487d-AE89-0D86278C93C9")
    INktExportedFunction : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Ordinal( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ModuleName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CompleteName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsForwarded( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ForwardedApiName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ForwardedModuleName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ForwardedCompleteName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AddrBase( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AddrOffset( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Addr( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CallingConv( 
            /* [retval][out] */ eNktDboCallingConvention *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE DbObject( 
            /* [retval][out] */ INktDbObject **ppDbObject) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_HasDeclaration( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Declaration( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktExportedFunctionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktExportedFunction * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktExportedFunction * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktExportedFunction * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktExportedFunction * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktExportedFunction * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktExportedFunction * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktExportedFunction * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktExportedFunction * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktExportedFunction * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktExportedFunction * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktExportedFunction * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktExportedFunction * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktExportedFunction * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Ordinal )( 
            INktExportedFunction * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            INktExportedFunction * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ModuleName )( 
            INktExportedFunction * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CompleteName )( 
            INktExportedFunction * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsForwarded )( 
            INktExportedFunction * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ForwardedApiName )( 
            INktExportedFunction * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ForwardedModuleName )( 
            INktExportedFunction * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ForwardedCompleteName )( 
            INktExportedFunction * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AddrBase )( 
            INktExportedFunction * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AddrOffset )( 
            INktExportedFunction * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Addr )( 
            INktExportedFunction * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CallingConv )( 
            INktExportedFunction * This,
            /* [retval][out] */ eNktDboCallingConvention *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *DbObject )( 
            INktExportedFunction * This,
            /* [retval][out] */ INktDbObject **ppDbObject);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasDeclaration )( 
            INktExportedFunction * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Declaration )( 
            INktExportedFunction * This,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } INktExportedFunctionVtbl;

    interface INktExportedFunction
    {
        CONST_VTBL struct INktExportedFunctionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktExportedFunction_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktExportedFunction_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktExportedFunction_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktExportedFunction_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktExportedFunction_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktExportedFunction_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktExportedFunction_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktExportedFunction_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktExportedFunction_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktExportedFunction_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktExportedFunction_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktExportedFunction_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktExportedFunction_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktExportedFunction_get_Ordinal(This,pVal)	\
    ( (This)->lpVtbl -> get_Ordinal(This,pVal) ) 

#define INktExportedFunction_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define INktExportedFunction_get_ModuleName(This,pVal)	\
    ( (This)->lpVtbl -> get_ModuleName(This,pVal) ) 

#define INktExportedFunction_get_CompleteName(This,pVal)	\
    ( (This)->lpVtbl -> get_CompleteName(This,pVal) ) 

#define INktExportedFunction_get_IsForwarded(This,pVal)	\
    ( (This)->lpVtbl -> get_IsForwarded(This,pVal) ) 

#define INktExportedFunction_get_ForwardedApiName(This,pVal)	\
    ( (This)->lpVtbl -> get_ForwardedApiName(This,pVal) ) 

#define INktExportedFunction_get_ForwardedModuleName(This,pVal)	\
    ( (This)->lpVtbl -> get_ForwardedModuleName(This,pVal) ) 

#define INktExportedFunction_get_ForwardedCompleteName(This,pVal)	\
    ( (This)->lpVtbl -> get_ForwardedCompleteName(This,pVal) ) 

#define INktExportedFunction_get_AddrBase(This,pVal)	\
    ( (This)->lpVtbl -> get_AddrBase(This,pVal) ) 

#define INktExportedFunction_get_AddrOffset(This,pVal)	\
    ( (This)->lpVtbl -> get_AddrOffset(This,pVal) ) 

#define INktExportedFunction_get_Addr(This,pVal)	\
    ( (This)->lpVtbl -> get_Addr(This,pVal) ) 

#define INktExportedFunction_get_CallingConv(This,pVal)	\
    ( (This)->lpVtbl -> get_CallingConv(This,pVal) ) 

#define INktExportedFunction_DbObject(This,ppDbObject)	\
    ( (This)->lpVtbl -> DbObject(This,ppDbObject) ) 

#define INktExportedFunction_get_HasDeclaration(This,pVal)	\
    ( (This)->lpVtbl -> get_HasDeclaration(This,pVal) ) 

#define INktExportedFunction_get_Declaration(This,pVal)	\
    ( (This)->lpVtbl -> get_Declaration(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_get_Ordinal_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktExportedFunction_get_Ordinal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_get_Name_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktExportedFunction_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_get_ModuleName_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktExportedFunction_get_ModuleName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_get_CompleteName_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktExportedFunction_get_CompleteName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_get_IsForwarded_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktExportedFunction_get_IsForwarded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_get_ForwardedApiName_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktExportedFunction_get_ForwardedApiName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_get_ForwardedModuleName_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktExportedFunction_get_ForwardedModuleName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_get_ForwardedCompleteName_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktExportedFunction_get_ForwardedCompleteName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_get_AddrBase_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktExportedFunction_get_AddrBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_get_AddrOffset_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktExportedFunction_get_AddrOffset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_get_Addr_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktExportedFunction_get_Addr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_get_CallingConv_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ eNktDboCallingConvention *pVal);


void __RPC_STUB INktExportedFunction_get_CallingConv_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_DbObject_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ INktDbObject **ppDbObject);


void __RPC_STUB INktExportedFunction_DbObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_get_HasDeclaration_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktExportedFunction_get_HasDeclaration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunction_get_Declaration_Proxy( 
    INktExportedFunction * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktExportedFunction_get_Declaration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktExportedFunction_INTERFACE_DEFINED__ */


#ifndef __INktExportedFunctionsEnum_INTERFACE_DEFINED__
#define __INktExportedFunctionsEnum_INTERFACE_DEFINED__

/* interface INktExportedFunctionsEnum */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktExportedFunctionsEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B110FD2E-174C-40ec-B180-AA2BF98C992D")
    INktExportedFunctionsEnum : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE First( 
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Last( 
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Previous( 
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetByDeviareId( 
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetByOrdinal( 
            /* [in] */ LONG ordinal,
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetByName( 
            /* [in] */ BSTR funcName,
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetByAddress( 
            /* [in] */ my_ssize_t addr,
            /* [in] */ VARIANT_BOOL searchNearest,
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [restricted][id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktExportedFunctionsEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktExportedFunctionsEnum * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktExportedFunctionsEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktExportedFunctionsEnum * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktExportedFunctionsEnum * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktExportedFunctionsEnum * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktExportedFunctionsEnum * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktExportedFunctionsEnum * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktExportedFunctionsEnum * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktExportedFunctionsEnum * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktExportedFunctionsEnum * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktExportedFunctionsEnum * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktExportedFunctionsEnum * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktExportedFunctionsEnum * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            INktExportedFunctionsEnum * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            INktExportedFunctionsEnum * This,
            /* [in] */ LONG index,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *First )( 
            INktExportedFunctionsEnum * This,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Last )( 
            INktExportedFunctionsEnum * This,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            INktExportedFunctionsEnum * This,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Previous )( 
            INktExportedFunctionsEnum * This,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetByDeviareId )( 
            INktExportedFunctionsEnum * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetByOrdinal )( 
            INktExportedFunctionsEnum * This,
            /* [in] */ LONG ordinal,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetByName )( 
            INktExportedFunctionsEnum * This,
            /* [in] */ BSTR funcName,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetByAddress )( 
            INktExportedFunctionsEnum * This,
            /* [in] */ my_ssize_t addr,
            /* [in] */ VARIANT_BOOL searchNearest,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [restricted][id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            INktExportedFunctionsEnum * This,
            /* [retval][out] */ IUnknown **ppUnk);
        
        END_INTERFACE
    } INktExportedFunctionsEnumVtbl;

    interface INktExportedFunctionsEnum
    {
        CONST_VTBL struct INktExportedFunctionsEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktExportedFunctionsEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktExportedFunctionsEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktExportedFunctionsEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktExportedFunctionsEnum_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktExportedFunctionsEnum_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktExportedFunctionsEnum_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktExportedFunctionsEnum_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktExportedFunctionsEnum_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktExportedFunctionsEnum_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktExportedFunctionsEnum_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktExportedFunctionsEnum_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktExportedFunctionsEnum_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktExportedFunctionsEnum_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktExportedFunctionsEnum_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define INktExportedFunctionsEnum_GetAt(This,index,ppExpFunc)	\
    ( (This)->lpVtbl -> GetAt(This,index,ppExpFunc) ) 

#define INktExportedFunctionsEnum_First(This,ppExpFunc)	\
    ( (This)->lpVtbl -> First(This,ppExpFunc) ) 

#define INktExportedFunctionsEnum_Last(This,ppExpFunc)	\
    ( (This)->lpVtbl -> Last(This,ppExpFunc) ) 

#define INktExportedFunctionsEnum_Next(This,ppExpFunc)	\
    ( (This)->lpVtbl -> Next(This,ppExpFunc) ) 

#define INktExportedFunctionsEnum_Previous(This,ppExpFunc)	\
    ( (This)->lpVtbl -> Previous(This,ppExpFunc) ) 

#define INktExportedFunctionsEnum_GetByDeviareId(This,devId,ppExpFunc)	\
    ( (This)->lpVtbl -> GetByDeviareId(This,devId,ppExpFunc) ) 

#define INktExportedFunctionsEnum_GetByOrdinal(This,ordinal,ppExpFunc)	\
    ( (This)->lpVtbl -> GetByOrdinal(This,ordinal,ppExpFunc) ) 

#define INktExportedFunctionsEnum_GetByName(This,funcName,ppExpFunc)	\
    ( (This)->lpVtbl -> GetByName(This,funcName,ppExpFunc) ) 

#define INktExportedFunctionsEnum_GetByAddress(This,addr,searchNearest,ppExpFunc)	\
    ( (This)->lpVtbl -> GetByAddress(This,addr,searchNearest,ppExpFunc) ) 

#define INktExportedFunctionsEnum_get__NewEnum(This,ppUnk)	\
    ( (This)->lpVtbl -> get__NewEnum(This,ppUnk) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunctionsEnum_get_Count_Proxy( 
    INktExportedFunctionsEnum * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktExportedFunctionsEnum_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktExportedFunctionsEnum_GetAt_Proxy( 
    INktExportedFunctionsEnum * This,
    /* [in] */ LONG index,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktExportedFunctionsEnum_GetAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktExportedFunctionsEnum_First_Proxy( 
    INktExportedFunctionsEnum * This,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktExportedFunctionsEnum_First_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktExportedFunctionsEnum_Last_Proxy( 
    INktExportedFunctionsEnum * This,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktExportedFunctionsEnum_Last_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktExportedFunctionsEnum_Next_Proxy( 
    INktExportedFunctionsEnum * This,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktExportedFunctionsEnum_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktExportedFunctionsEnum_Previous_Proxy( 
    INktExportedFunctionsEnum * This,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktExportedFunctionsEnum_Previous_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktExportedFunctionsEnum_GetByDeviareId_Proxy( 
    INktExportedFunctionsEnum * This,
    /* [in] */ my_ssize_t devId,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktExportedFunctionsEnum_GetByDeviareId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktExportedFunctionsEnum_GetByOrdinal_Proxy( 
    INktExportedFunctionsEnum * This,
    /* [in] */ LONG ordinal,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktExportedFunctionsEnum_GetByOrdinal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktExportedFunctionsEnum_GetByName_Proxy( 
    INktExportedFunctionsEnum * This,
    /* [in] */ BSTR funcName,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktExportedFunctionsEnum_GetByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktExportedFunctionsEnum_GetByAddress_Proxy( 
    INktExportedFunctionsEnum * This,
    /* [in] */ my_ssize_t addr,
    /* [in] */ VARIANT_BOOL searchNearest,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktExportedFunctionsEnum_GetByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktExportedFunctionsEnum_get__NewEnum_Proxy( 
    INktExportedFunctionsEnum * This,
    /* [retval][out] */ IUnknown **ppUnk);


void __RPC_STUB INktExportedFunctionsEnum_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktExportedFunctionsEnum_INTERFACE_DEFINED__ */


#ifndef __INktHook_INTERFACE_DEFINED__
#define __INktHook_INTERFACE_DEFINED__

/* interface INktHook */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktHook;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F69A083D-105F-417b-B1E3-EE430E58431A")
    INktHook : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Id( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Destroy( void) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Attach( 
            /* [in] */ VARIANT procOrId,
            /* [defaultvalue][in] */ VARIANT_BOOL sync = 0) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Detach( 
            /* [in] */ VARIANT procOrId,
            /* [defaultvalue][in] */ VARIANT_BOOL sync = 0) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Enable( 
            /* [in] */ VARIANT procOrId,
            /* [defaultvalue][in] */ VARIANT_BOOL enable = 0,
            /* [defaultvalue][in] */ VARIANT_BOOL sync = 0) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE State( 
            /* [in] */ VARIANT procOrId,
            /* [retval][out] */ eNktHookState *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE QueryOverwrite( 
            /* [in] */ VARIANT procOrId,
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE LastError( 
            /* [in] */ VARIANT procOrId,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Hook( 
            /* [defaultvalue][in] */ VARIANT_BOOL sync = 0) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Unhook( 
            /* [defaultvalue][in] */ VARIANT_BOOL sync = 0) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AddCustomHandler( 
            /* [in] */ BSTR handlerDll,
            /* [in] */ LONG handlerFlags,
            /* [in] */ BSTR parameters) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE RemoveAllHandlers( void) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FunctionName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Flags( 
            /* [retval][out] */ LONG *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktHookVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktHook * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktHook * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktHook * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktHook * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktHook * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktHook * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktHook * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktHook * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktHook * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktHook * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktHook * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktHook * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktHook * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Id )( 
            INktHook * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Destroy )( 
            INktHook * This);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Attach )( 
            INktHook * This,
            /* [in] */ VARIANT procOrId,
            /* [defaultvalue][in] */ VARIANT_BOOL sync);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Detach )( 
            INktHook * This,
            /* [in] */ VARIANT procOrId,
            /* [defaultvalue][in] */ VARIANT_BOOL sync);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Enable )( 
            INktHook * This,
            /* [in] */ VARIANT procOrId,
            /* [defaultvalue][in] */ VARIANT_BOOL enable,
            /* [defaultvalue][in] */ VARIANT_BOOL sync);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *State )( 
            INktHook * This,
            /* [in] */ VARIANT procOrId,
            /* [retval][out] */ eNktHookState *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *QueryOverwrite )( 
            INktHook * This,
            /* [in] */ VARIANT procOrId,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *LastError )( 
            INktHook * This,
            /* [in] */ VARIANT procOrId,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Hook )( 
            INktHook * This,
            /* [defaultvalue][in] */ VARIANT_BOOL sync);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Unhook )( 
            INktHook * This,
            /* [defaultvalue][in] */ VARIANT_BOOL sync);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddCustomHandler )( 
            INktHook * This,
            /* [in] */ BSTR handlerDll,
            /* [in] */ LONG handlerFlags,
            /* [in] */ BSTR parameters);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *RemoveAllHandlers )( 
            INktHook * This);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FunctionName )( 
            INktHook * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Flags )( 
            INktHook * This,
            /* [retval][out] */ LONG *pVal);
        
        END_INTERFACE
    } INktHookVtbl;

    interface INktHook
    {
        CONST_VTBL struct INktHookVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktHook_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktHook_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktHook_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktHook_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktHook_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktHook_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktHook_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktHook_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktHook_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktHook_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktHook_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktHook_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktHook_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktHook_get_Id(This,pVal)	\
    ( (This)->lpVtbl -> get_Id(This,pVal) ) 

#define INktHook_Destroy(This)	\
    ( (This)->lpVtbl -> Destroy(This) ) 

#define INktHook_Attach(This,procOrId,sync)	\
    ( (This)->lpVtbl -> Attach(This,procOrId,sync) ) 

#define INktHook_Detach(This,procOrId,sync)	\
    ( (This)->lpVtbl -> Detach(This,procOrId,sync) ) 

#define INktHook_Enable(This,procOrId,enable,sync)	\
    ( (This)->lpVtbl -> Enable(This,procOrId,enable,sync) ) 

#define INktHook_State(This,procOrId,pVal)	\
    ( (This)->lpVtbl -> State(This,procOrId,pVal) ) 

#define INktHook_QueryOverwrite(This,procOrId,pVal)	\
    ( (This)->lpVtbl -> QueryOverwrite(This,procOrId,pVal) ) 

#define INktHook_LastError(This,procOrId,pVal)	\
    ( (This)->lpVtbl -> LastError(This,procOrId,pVal) ) 

#define INktHook_Hook(This,sync)	\
    ( (This)->lpVtbl -> Hook(This,sync) ) 

#define INktHook_Unhook(This,sync)	\
    ( (This)->lpVtbl -> Unhook(This,sync) ) 

#define INktHook_AddCustomHandler(This,handlerDll,handlerFlags,parameters)	\
    ( (This)->lpVtbl -> AddCustomHandler(This,handlerDll,handlerFlags,parameters) ) 

#define INktHook_RemoveAllHandlers(This)	\
    ( (This)->lpVtbl -> RemoveAllHandlers(This) ) 

#define INktHook_get_FunctionName(This,pVal)	\
    ( (This)->lpVtbl -> get_FunctionName(This,pVal) ) 

#define INktHook_get_Flags(This,pVal)	\
    ( (This)->lpVtbl -> get_Flags(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHook_get_Id_Proxy( 
    INktHook * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktHook_get_Id_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHook_Destroy_Proxy( 
    INktHook * This);


void __RPC_STUB INktHook_Destroy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHook_Attach_Proxy( 
    INktHook * This,
    /* [in] */ VARIANT procOrId,
    /* [defaultvalue][in] */ VARIANT_BOOL sync);


void __RPC_STUB INktHook_Attach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHook_Detach_Proxy( 
    INktHook * This,
    /* [in] */ VARIANT procOrId,
    /* [defaultvalue][in] */ VARIANT_BOOL sync);


void __RPC_STUB INktHook_Detach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHook_Enable_Proxy( 
    INktHook * This,
    /* [in] */ VARIANT procOrId,
    /* [defaultvalue][in] */ VARIANT_BOOL enable,
    /* [defaultvalue][in] */ VARIANT_BOOL sync);


void __RPC_STUB INktHook_Enable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHook_State_Proxy( 
    INktHook * This,
    /* [in] */ VARIANT procOrId,
    /* [retval][out] */ eNktHookState *pVal);


void __RPC_STUB INktHook_State_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHook_QueryOverwrite_Proxy( 
    INktHook * This,
    /* [in] */ VARIANT procOrId,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktHook_QueryOverwrite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHook_LastError_Proxy( 
    INktHook * This,
    /* [in] */ VARIANT procOrId,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktHook_LastError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHook_Hook_Proxy( 
    INktHook * This,
    /* [defaultvalue][in] */ VARIANT_BOOL sync);


void __RPC_STUB INktHook_Hook_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHook_Unhook_Proxy( 
    INktHook * This,
    /* [defaultvalue][in] */ VARIANT_BOOL sync);


void __RPC_STUB INktHook_Unhook_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHook_AddCustomHandler_Proxy( 
    INktHook * This,
    /* [in] */ BSTR handlerDll,
    /* [in] */ LONG handlerFlags,
    /* [in] */ BSTR parameters);


void __RPC_STUB INktHook_AddCustomHandler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHook_RemoveAllHandlers_Proxy( 
    INktHook * This);


void __RPC_STUB INktHook_RemoveAllHandlers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHook_get_FunctionName_Proxy( 
    INktHook * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktHook_get_FunctionName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHook_get_Flags_Proxy( 
    INktHook * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktHook_get_Flags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktHook_INTERFACE_DEFINED__ */


#ifndef __INktHooksEnum_INTERFACE_DEFINED__
#define __INktHooksEnum_INTERFACE_DEFINED__

/* interface INktHooksEnum */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktHooksEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A31B9BE8-CE9A-4d7c-A624-20640ACA532C")
    INktHooksEnum : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ INktHook **ppHook) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE First( 
            /* [retval][out] */ INktHook **ppHook) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Last( 
            /* [retval][out] */ INktHook **ppHook) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ INktHook **ppHook) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Previous( 
            /* [retval][out] */ INktHook **ppHook) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetByDeviareId( 
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ INktHook **ppHook) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetById( 
            /* [in] */ my_ssize_t id,
            /* [retval][out] */ INktHook **ppHook) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ INktHook *pHook) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Attach( 
            /* [in] */ VARIANT procOrId,
            /* [defaultvalue][in] */ VARIANT_BOOL sync = 0) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Detach( 
            /* [in] */ VARIANT procOrId,
            /* [defaultvalue][in] */ VARIANT_BOOL sync = 0) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Enable( 
            /* [in] */ VARIANT procOrId,
            /* [defaultvalue][in] */ VARIANT_BOOL enable = 0,
            /* [defaultvalue][in] */ VARIANT_BOOL sync = 0) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Hook( 
            /* [defaultvalue][in] */ VARIANT_BOOL sync = 0) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Unhook( 
            /* [defaultvalue][in] */ VARIANT_BOOL sync = 0) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE QueryOverwrite( 
            /* [in] */ VARIANT procOrId,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [restricted][id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktHooksEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktHooksEnum * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktHooksEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktHooksEnum * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktHooksEnum * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktHooksEnum * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktHooksEnum * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktHooksEnum * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktHooksEnum * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktHooksEnum * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktHooksEnum * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktHooksEnum * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktHooksEnum * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktHooksEnum * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            INktHooksEnum * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            INktHooksEnum * This,
            /* [in] */ LONG index,
            /* [retval][out] */ INktHook **ppHook);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *First )( 
            INktHooksEnum * This,
            /* [retval][out] */ INktHook **ppHook);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Last )( 
            INktHooksEnum * This,
            /* [retval][out] */ INktHook **ppHook);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            INktHooksEnum * This,
            /* [retval][out] */ INktHook **ppHook);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Previous )( 
            INktHooksEnum * This,
            /* [retval][out] */ INktHook **ppHook);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetByDeviareId )( 
            INktHooksEnum * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ INktHook **ppHook);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetById )( 
            INktHooksEnum * This,
            /* [in] */ my_ssize_t id,
            /* [retval][out] */ INktHook **ppHook);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            INktHooksEnum * This,
            /* [in] */ INktHook *pHook);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Attach )( 
            INktHooksEnum * This,
            /* [in] */ VARIANT procOrId,
            /* [defaultvalue][in] */ VARIANT_BOOL sync);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Detach )( 
            INktHooksEnum * This,
            /* [in] */ VARIANT procOrId,
            /* [defaultvalue][in] */ VARIANT_BOOL sync);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Enable )( 
            INktHooksEnum * This,
            /* [in] */ VARIANT procOrId,
            /* [defaultvalue][in] */ VARIANT_BOOL enable,
            /* [defaultvalue][in] */ VARIANT_BOOL sync);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Hook )( 
            INktHooksEnum * This,
            /* [defaultvalue][in] */ VARIANT_BOOL sync);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Unhook )( 
            INktHooksEnum * This,
            /* [defaultvalue][in] */ VARIANT_BOOL sync);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *QueryOverwrite )( 
            INktHooksEnum * This,
            /* [in] */ VARIANT procOrId,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [restricted][id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            INktHooksEnum * This,
            /* [retval][out] */ IUnknown **ppUnk);
        
        END_INTERFACE
    } INktHooksEnumVtbl;

    interface INktHooksEnum
    {
        CONST_VTBL struct INktHooksEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktHooksEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktHooksEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktHooksEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktHooksEnum_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktHooksEnum_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktHooksEnum_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktHooksEnum_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktHooksEnum_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktHooksEnum_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktHooksEnum_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktHooksEnum_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktHooksEnum_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktHooksEnum_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktHooksEnum_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define INktHooksEnum_GetAt(This,index,ppHook)	\
    ( (This)->lpVtbl -> GetAt(This,index,ppHook) ) 

#define INktHooksEnum_First(This,ppHook)	\
    ( (This)->lpVtbl -> First(This,ppHook) ) 

#define INktHooksEnum_Last(This,ppHook)	\
    ( (This)->lpVtbl -> Last(This,ppHook) ) 

#define INktHooksEnum_Next(This,ppHook)	\
    ( (This)->lpVtbl -> Next(This,ppHook) ) 

#define INktHooksEnum_Previous(This,ppHook)	\
    ( (This)->lpVtbl -> Previous(This,ppHook) ) 

#define INktHooksEnum_GetByDeviareId(This,devId,ppHook)	\
    ( (This)->lpVtbl -> GetByDeviareId(This,devId,ppHook) ) 

#define INktHooksEnum_GetById(This,id,ppHook)	\
    ( (This)->lpVtbl -> GetById(This,id,ppHook) ) 

#define INktHooksEnum_Add(This,pHook)	\
    ( (This)->lpVtbl -> Add(This,pHook) ) 

#define INktHooksEnum_Attach(This,procOrId,sync)	\
    ( (This)->lpVtbl -> Attach(This,procOrId,sync) ) 

#define INktHooksEnum_Detach(This,procOrId,sync)	\
    ( (This)->lpVtbl -> Detach(This,procOrId,sync) ) 

#define INktHooksEnum_Enable(This,procOrId,enable,sync)	\
    ( (This)->lpVtbl -> Enable(This,procOrId,enable,sync) ) 

#define INktHooksEnum_Hook(This,sync)	\
    ( (This)->lpVtbl -> Hook(This,sync) ) 

#define INktHooksEnum_Unhook(This,sync)	\
    ( (This)->lpVtbl -> Unhook(This,sync) ) 

#define INktHooksEnum_QueryOverwrite(This,procOrId,pVal)	\
    ( (This)->lpVtbl -> QueryOverwrite(This,procOrId,pVal) ) 

#define INktHooksEnum_get__NewEnum(This,ppUnk)	\
    ( (This)->lpVtbl -> get__NewEnum(This,ppUnk) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_get_Count_Proxy( 
    INktHooksEnum * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktHooksEnum_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_GetAt_Proxy( 
    INktHooksEnum * This,
    /* [in] */ LONG index,
    /* [retval][out] */ INktHook **ppHook);


void __RPC_STUB INktHooksEnum_GetAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_First_Proxy( 
    INktHooksEnum * This,
    /* [retval][out] */ INktHook **ppHook);


void __RPC_STUB INktHooksEnum_First_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_Last_Proxy( 
    INktHooksEnum * This,
    /* [retval][out] */ INktHook **ppHook);


void __RPC_STUB INktHooksEnum_Last_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_Next_Proxy( 
    INktHooksEnum * This,
    /* [retval][out] */ INktHook **ppHook);


void __RPC_STUB INktHooksEnum_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_Previous_Proxy( 
    INktHooksEnum * This,
    /* [retval][out] */ INktHook **ppHook);


void __RPC_STUB INktHooksEnum_Previous_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_GetByDeviareId_Proxy( 
    INktHooksEnum * This,
    /* [in] */ my_ssize_t devId,
    /* [retval][out] */ INktHook **ppHook);


void __RPC_STUB INktHooksEnum_GetByDeviareId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_GetById_Proxy( 
    INktHooksEnum * This,
    /* [in] */ my_ssize_t id,
    /* [retval][out] */ INktHook **ppHook);


void __RPC_STUB INktHooksEnum_GetById_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_Add_Proxy( 
    INktHooksEnum * This,
    /* [in] */ INktHook *pHook);


void __RPC_STUB INktHooksEnum_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_Attach_Proxy( 
    INktHooksEnum * This,
    /* [in] */ VARIANT procOrId,
    /* [defaultvalue][in] */ VARIANT_BOOL sync);


void __RPC_STUB INktHooksEnum_Attach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_Detach_Proxy( 
    INktHooksEnum * This,
    /* [in] */ VARIANT procOrId,
    /* [defaultvalue][in] */ VARIANT_BOOL sync);


void __RPC_STUB INktHooksEnum_Detach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_Enable_Proxy( 
    INktHooksEnum * This,
    /* [in] */ VARIANT procOrId,
    /* [defaultvalue][in] */ VARIANT_BOOL enable,
    /* [defaultvalue][in] */ VARIANT_BOOL sync);


void __RPC_STUB INktHooksEnum_Enable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_Hook_Proxy( 
    INktHooksEnum * This,
    /* [defaultvalue][in] */ VARIANT_BOOL sync);


void __RPC_STUB INktHooksEnum_Hook_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_Unhook_Proxy( 
    INktHooksEnum * This,
    /* [defaultvalue][in] */ VARIANT_BOOL sync);


void __RPC_STUB INktHooksEnum_Unhook_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_QueryOverwrite_Proxy( 
    INktHooksEnum * This,
    /* [in] */ VARIANT procOrId,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB INktHooksEnum_QueryOverwrite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHooksEnum_get__NewEnum_Proxy( 
    INktHooksEnum * This,
    /* [retval][out] */ IUnknown **ppUnk);


void __RPC_STUB INktHooksEnum_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktHooksEnum_INTERFACE_DEFINED__ */


#ifndef __INktParam_INTERFACE_DEFINED__
#define __INktParam_INTERFACE_DEFINED__

/* interface INktParam */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktParam;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BB0A417B-D0F2-4c69-8158-95087B0A0C8F")
    INktParam : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Address( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Class( 
            /* [retval][out] */ eNktDboClass *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BasicType( 
            /* [retval][out] */ eNktDboFundamentalType *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_TypeName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Declaration( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Flags( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsPointer( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsArray( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ArrayMaxHint( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsAnsiString( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsWideString( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IntResourceString( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_GuidString( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Evaluate( 
            /* [retval][out] */ INktParam **ppParam) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE IndexedEvaluate( 
            /* [in] */ LONG index,
            /* [retval][out] */ INktParam **ppParam) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE FullEvaluate( 
            /* [retval][out] */ INktParam **ppParam) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FieldsCount( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Field( 
            /* [in] */ VARIANT indexOrName,
            /* [retval][out] */ INktParam **ppParam) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Fields( 
            /* [retval][out] */ INktParamsEnum **ppParamsEnum) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ VARIANT var) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ValueAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ValueAt( 
            /* [in] */ LONG index,
            /* [in] */ VARIANT var) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ValueArray( 
            /* [in] */ LONG count,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ValueArrayAt( 
            /* [in] */ LONG index,
            /* [in] */ LONG count,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ReadString( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ReadStringN( 
            /* [in] */ LONG maxChars,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE WriteString( 
            /* [in] */ BSTR str,
            /* [in] */ VARIANT_BOOL isAnsi,
            /* [defaultvalue][in] */ VARIANT_BOOL writeNul = -1) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE StringLength( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CharVal( 
            /* [retval][out] */ signed char *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_CharVal( 
            /* [in] */ signed char newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ByteVal( 
            /* [retval][out] */ unsigned char *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ByteVal( 
            /* [in] */ unsigned char newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ShortVal( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ShortVal( 
            /* [in] */ short newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_UShortVal( 
            /* [retval][out] */ unsigned short *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_UShortVal( 
            /* [in] */ unsigned short newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LongVal( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_LongVal( 
            /* [in] */ long newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ULongVal( 
            /* [retval][out] */ unsigned long *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ULongVal( 
            /* [in] */ unsigned long newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LongLongVal( 
            /* [retval][out] */ __int64 *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_LongLongVal( 
            /* [in] */ __int64 newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ULongLongVal( 
            /* [retval][out] */ unsigned __int64 *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ULongLongVal( 
            /* [in] */ unsigned __int64 newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SSizeTVal( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_SSizeTVal( 
            /* [in] */ my_ssize_t newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SizeTVal( 
            /* [retval][out] */ my_size_t *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_SizeTVal( 
            /* [in] */ my_size_t newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FloatVal( 
            /* [retval][out] */ float *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_FloatVal( 
            /* [in] */ float newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DoubleVal( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DoubleVal( 
            /* [in] */ double newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CharValAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ signed char *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_CharValAt( 
            /* [in] */ LONG index,
            /* [in] */ signed char newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ByteValAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ unsigned char *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ByteValAt( 
            /* [in] */ LONG index,
            /* [in] */ unsigned char newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ShortValAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ShortValAt( 
            /* [in] */ LONG index,
            /* [in] */ short newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_UShortValAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ unsigned short *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_UShortValAt( 
            /* [in] */ LONG index,
            /* [in] */ unsigned short newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LongValAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_LongValAt( 
            /* [in] */ LONG index,
            /* [in] */ long newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ULongValAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ unsigned long *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ULongValAt( 
            /* [in] */ LONG index,
            /* [in] */ unsigned long newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LongLongValAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ __int64 *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_LongLongValAt( 
            /* [in] */ LONG index,
            /* [in] */ __int64 newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ULongLongValAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ unsigned __int64 *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ULongLongValAt( 
            /* [in] */ LONG index,
            /* [in] */ unsigned __int64 newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SSizeTValAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_SSizeTValAt( 
            /* [in] */ LONG index,
            /* [in] */ my_ssize_t newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SizeTValAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ my_size_t *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_SizeTValAt( 
            /* [in] */ LONG index,
            /* [in] */ my_size_t newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FloatValAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ float *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_FloatValAt( 
            /* [in] */ LONG index,
            /* [in] */ float newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DoubleValAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DoubleValAt( 
            /* [in] */ LONG index,
            /* [in] */ double newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PointerVal( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_PointerVal( 
            /* [in] */ my_ssize_t newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsNullPointer( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Memory( 
            /* [retval][out] */ INktProcessMemory **ppProcMem) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE CastTo( 
            /* [in] */ BSTR newType,
            /* [retval][out] */ INktParam **ppParam) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktParamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktParam * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktParam * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktParam * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktParam * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktParam * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktParam * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktParam * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktParam * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktParam * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktParam * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktParam * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktParam * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktParam * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Address )( 
            INktParam * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Class )( 
            INktParam * This,
            /* [retval][out] */ eNktDboClass *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BasicType )( 
            INktParam * This,
            /* [retval][out] */ eNktDboFundamentalType *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            INktParam * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TypeName )( 
            INktParam * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Declaration )( 
            INktParam * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Flags )( 
            INktParam * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsPointer )( 
            INktParam * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsArray )( 
            INktParam * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ArrayMaxHint )( 
            INktParam * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsAnsiString )( 
            INktParam * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsWideString )( 
            INktParam * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IntResourceString )( 
            INktParam * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GuidString )( 
            INktParam * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Evaluate )( 
            INktParam * This,
            /* [retval][out] */ INktParam **ppParam);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *IndexedEvaluate )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [retval][out] */ INktParam **ppParam);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FullEvaluate )( 
            INktParam * This,
            /* [retval][out] */ INktParam **ppParam);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FieldsCount )( 
            INktParam * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Field )( 
            INktParam * This,
            /* [in] */ VARIANT indexOrName,
            /* [retval][out] */ INktParam **ppParam);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Fields )( 
            INktParam * This,
            /* [retval][out] */ INktParamsEnum **ppParamsEnum);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            INktParam * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Value )( 
            INktParam * This,
            /* [in] */ VARIANT var);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ValueAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ValueAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [in] */ VARIANT var);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ValueArray )( 
            INktParam * This,
            /* [in] */ LONG count,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ValueArrayAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [in] */ LONG count,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReadString )( 
            INktParam * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReadStringN )( 
            INktParam * This,
            /* [in] */ LONG maxChars,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *WriteString )( 
            INktParam * This,
            /* [in] */ BSTR str,
            /* [in] */ VARIANT_BOOL isAnsi,
            /* [defaultvalue][in] */ VARIANT_BOOL writeNul);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *StringLength )( 
            INktParam * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CharVal )( 
            INktParam * This,
            /* [retval][out] */ signed char *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CharVal )( 
            INktParam * This,
            /* [in] */ signed char newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ByteVal )( 
            INktParam * This,
            /* [retval][out] */ unsigned char *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ByteVal )( 
            INktParam * This,
            /* [in] */ unsigned char newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShortVal )( 
            INktParam * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ShortVal )( 
            INktParam * This,
            /* [in] */ short newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UShortVal )( 
            INktParam * This,
            /* [retval][out] */ unsigned short *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UShortVal )( 
            INktParam * This,
            /* [in] */ unsigned short newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LongVal )( 
            INktParam * This,
            /* [retval][out] */ long *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LongVal )( 
            INktParam * This,
            /* [in] */ long newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ULongVal )( 
            INktParam * This,
            /* [retval][out] */ unsigned long *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ULongVal )( 
            INktParam * This,
            /* [in] */ unsigned long newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LongLongVal )( 
            INktParam * This,
            /* [retval][out] */ __int64 *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LongLongVal )( 
            INktParam * This,
            /* [in] */ __int64 newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ULongLongVal )( 
            INktParam * This,
            /* [retval][out] */ unsigned __int64 *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ULongLongVal )( 
            INktParam * This,
            /* [in] */ unsigned __int64 newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SSizeTVal )( 
            INktParam * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SSizeTVal )( 
            INktParam * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SizeTVal )( 
            INktParam * This,
            /* [retval][out] */ my_size_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SizeTVal )( 
            INktParam * This,
            /* [in] */ my_size_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FloatVal )( 
            INktParam * This,
            /* [retval][out] */ float *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_FloatVal )( 
            INktParam * This,
            /* [in] */ float newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DoubleVal )( 
            INktParam * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DoubleVal )( 
            INktParam * This,
            /* [in] */ double newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CharValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [retval][out] */ signed char *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CharValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [in] */ signed char newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ByteValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [retval][out] */ unsigned char *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ByteValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [in] */ unsigned char newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShortValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [retval][out] */ short *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ShortValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [in] */ short newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UShortValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [retval][out] */ unsigned short *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UShortValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [in] */ unsigned short newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LongValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [retval][out] */ long *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LongValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [in] */ long newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ULongValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [retval][out] */ unsigned long *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ULongValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [in] */ unsigned long newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LongLongValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [retval][out] */ __int64 *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LongLongValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [in] */ __int64 newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ULongLongValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [retval][out] */ unsigned __int64 *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ULongLongValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [in] */ unsigned __int64 newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SSizeTValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SSizeTValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SizeTValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [retval][out] */ my_size_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SizeTValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [in] */ my_size_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FloatValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [retval][out] */ float *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_FloatValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [in] */ float newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DoubleValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DoubleValAt )( 
            INktParam * This,
            /* [in] */ LONG index,
            /* [in] */ double newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PointerVal )( 
            INktParam * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PointerVal )( 
            INktParam * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsNullPointer )( 
            INktParam * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Memory )( 
            INktParam * This,
            /* [retval][out] */ INktProcessMemory **ppProcMem);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *CastTo )( 
            INktParam * This,
            /* [in] */ BSTR newType,
            /* [retval][out] */ INktParam **ppParam);
        
        END_INTERFACE
    } INktParamVtbl;

    interface INktParam
    {
        CONST_VTBL struct INktParamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktParam_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktParam_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktParam_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktParam_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktParam_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktParam_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktParam_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktParam_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktParam_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktParam_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktParam_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktParam_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktParam_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktParam_get_Address(This,pVal)	\
    ( (This)->lpVtbl -> get_Address(This,pVal) ) 

#define INktParam_get_Class(This,pVal)	\
    ( (This)->lpVtbl -> get_Class(This,pVal) ) 

#define INktParam_get_BasicType(This,pVal)	\
    ( (This)->lpVtbl -> get_BasicType(This,pVal) ) 

#define INktParam_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define INktParam_get_TypeName(This,pVal)	\
    ( (This)->lpVtbl -> get_TypeName(This,pVal) ) 

#define INktParam_get_Declaration(This,pVal)	\
    ( (This)->lpVtbl -> get_Declaration(This,pVal) ) 

#define INktParam_get_Flags(This,pVal)	\
    ( (This)->lpVtbl -> get_Flags(This,pVal) ) 

#define INktParam_get_IsPointer(This,pVal)	\
    ( (This)->lpVtbl -> get_IsPointer(This,pVal) ) 

#define INktParam_get_IsArray(This,pVal)	\
    ( (This)->lpVtbl -> get_IsArray(This,pVal) ) 

#define INktParam_get_ArrayMaxHint(This,pVal)	\
    ( (This)->lpVtbl -> get_ArrayMaxHint(This,pVal) ) 

#define INktParam_get_IsAnsiString(This,pVal)	\
    ( (This)->lpVtbl -> get_IsAnsiString(This,pVal) ) 

#define INktParam_get_IsWideString(This,pVal)	\
    ( (This)->lpVtbl -> get_IsWideString(This,pVal) ) 

#define INktParam_get_IntResourceString(This,pVal)	\
    ( (This)->lpVtbl -> get_IntResourceString(This,pVal) ) 

#define INktParam_get_GuidString(This,pVal)	\
    ( (This)->lpVtbl -> get_GuidString(This,pVal) ) 

#define INktParam_Evaluate(This,ppParam)	\
    ( (This)->lpVtbl -> Evaluate(This,ppParam) ) 

#define INktParam_IndexedEvaluate(This,index,ppParam)	\
    ( (This)->lpVtbl -> IndexedEvaluate(This,index,ppParam) ) 

#define INktParam_FullEvaluate(This,ppParam)	\
    ( (This)->lpVtbl -> FullEvaluate(This,ppParam) ) 

#define INktParam_get_FieldsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_FieldsCount(This,pVal) ) 

#define INktParam_Field(This,indexOrName,ppParam)	\
    ( (This)->lpVtbl -> Field(This,indexOrName,ppParam) ) 

#define INktParam_Fields(This,ppParamsEnum)	\
    ( (This)->lpVtbl -> Fields(This,ppParamsEnum) ) 

#define INktParam_get_Value(This,pVal)	\
    ( (This)->lpVtbl -> get_Value(This,pVal) ) 

#define INktParam_put_Value(This,var)	\
    ( (This)->lpVtbl -> put_Value(This,var) ) 

#define INktParam_get_ValueAt(This,index,pVal)	\
    ( (This)->lpVtbl -> get_ValueAt(This,index,pVal) ) 

#define INktParam_put_ValueAt(This,index,var)	\
    ( (This)->lpVtbl -> put_ValueAt(This,index,var) ) 

#define INktParam_get_ValueArray(This,count,pVal)	\
    ( (This)->lpVtbl -> get_ValueArray(This,count,pVal) ) 

#define INktParam_get_ValueArrayAt(This,index,count,pVal)	\
    ( (This)->lpVtbl -> get_ValueArrayAt(This,index,count,pVal) ) 

#define INktParam_ReadString(This,pVal)	\
    ( (This)->lpVtbl -> ReadString(This,pVal) ) 

#define INktParam_ReadStringN(This,maxChars,pVal)	\
    ( (This)->lpVtbl -> ReadStringN(This,maxChars,pVal) ) 

#define INktParam_WriteString(This,str,isAnsi,writeNul)	\
    ( (This)->lpVtbl -> WriteString(This,str,isAnsi,writeNul) ) 

#define INktParam_StringLength(This,pVal)	\
    ( (This)->lpVtbl -> StringLength(This,pVal) ) 

#define INktParam_get_CharVal(This,pVal)	\
    ( (This)->lpVtbl -> get_CharVal(This,pVal) ) 

#define INktParam_put_CharVal(This,newValue)	\
    ( (This)->lpVtbl -> put_CharVal(This,newValue) ) 

#define INktParam_get_ByteVal(This,pVal)	\
    ( (This)->lpVtbl -> get_ByteVal(This,pVal) ) 

#define INktParam_put_ByteVal(This,newValue)	\
    ( (This)->lpVtbl -> put_ByteVal(This,newValue) ) 

#define INktParam_get_ShortVal(This,pVal)	\
    ( (This)->lpVtbl -> get_ShortVal(This,pVal) ) 

#define INktParam_put_ShortVal(This,newValue)	\
    ( (This)->lpVtbl -> put_ShortVal(This,newValue) ) 

#define INktParam_get_UShortVal(This,pVal)	\
    ( (This)->lpVtbl -> get_UShortVal(This,pVal) ) 

#define INktParam_put_UShortVal(This,newValue)	\
    ( (This)->lpVtbl -> put_UShortVal(This,newValue) ) 

#define INktParam_get_LongVal(This,pVal)	\
    ( (This)->lpVtbl -> get_LongVal(This,pVal) ) 

#define INktParam_put_LongVal(This,newValue)	\
    ( (This)->lpVtbl -> put_LongVal(This,newValue) ) 

#define INktParam_get_ULongVal(This,pVal)	\
    ( (This)->lpVtbl -> get_ULongVal(This,pVal) ) 

#define INktParam_put_ULongVal(This,newValue)	\
    ( (This)->lpVtbl -> put_ULongVal(This,newValue) ) 

#define INktParam_get_LongLongVal(This,pVal)	\
    ( (This)->lpVtbl -> get_LongLongVal(This,pVal) ) 

#define INktParam_put_LongLongVal(This,newValue)	\
    ( (This)->lpVtbl -> put_LongLongVal(This,newValue) ) 

#define INktParam_get_ULongLongVal(This,pVal)	\
    ( (This)->lpVtbl -> get_ULongLongVal(This,pVal) ) 

#define INktParam_put_ULongLongVal(This,newValue)	\
    ( (This)->lpVtbl -> put_ULongLongVal(This,newValue) ) 

#define INktParam_get_SSizeTVal(This,pVal)	\
    ( (This)->lpVtbl -> get_SSizeTVal(This,pVal) ) 

#define INktParam_put_SSizeTVal(This,newValue)	\
    ( (This)->lpVtbl -> put_SSizeTVal(This,newValue) ) 

#define INktParam_get_SizeTVal(This,pVal)	\
    ( (This)->lpVtbl -> get_SizeTVal(This,pVal) ) 

#define INktParam_put_SizeTVal(This,newValue)	\
    ( (This)->lpVtbl -> put_SizeTVal(This,newValue) ) 

#define INktParam_get_FloatVal(This,pVal)	\
    ( (This)->lpVtbl -> get_FloatVal(This,pVal) ) 

#define INktParam_put_FloatVal(This,newValue)	\
    ( (This)->lpVtbl -> put_FloatVal(This,newValue) ) 

#define INktParam_get_DoubleVal(This,pVal)	\
    ( (This)->lpVtbl -> get_DoubleVal(This,pVal) ) 

#define INktParam_put_DoubleVal(This,newValue)	\
    ( (This)->lpVtbl -> put_DoubleVal(This,newValue) ) 

#define INktParam_get_CharValAt(This,index,pVal)	\
    ( (This)->lpVtbl -> get_CharValAt(This,index,pVal) ) 

#define INktParam_put_CharValAt(This,index,newValue)	\
    ( (This)->lpVtbl -> put_CharValAt(This,index,newValue) ) 

#define INktParam_get_ByteValAt(This,index,pVal)	\
    ( (This)->lpVtbl -> get_ByteValAt(This,index,pVal) ) 

#define INktParam_put_ByteValAt(This,index,newValue)	\
    ( (This)->lpVtbl -> put_ByteValAt(This,index,newValue) ) 

#define INktParam_get_ShortValAt(This,index,pVal)	\
    ( (This)->lpVtbl -> get_ShortValAt(This,index,pVal) ) 

#define INktParam_put_ShortValAt(This,index,newValue)	\
    ( (This)->lpVtbl -> put_ShortValAt(This,index,newValue) ) 

#define INktParam_get_UShortValAt(This,index,pVal)	\
    ( (This)->lpVtbl -> get_UShortValAt(This,index,pVal) ) 

#define INktParam_put_UShortValAt(This,index,newValue)	\
    ( (This)->lpVtbl -> put_UShortValAt(This,index,newValue) ) 

#define INktParam_get_LongValAt(This,index,pVal)	\
    ( (This)->lpVtbl -> get_LongValAt(This,index,pVal) ) 

#define INktParam_put_LongValAt(This,index,newValue)	\
    ( (This)->lpVtbl -> put_LongValAt(This,index,newValue) ) 

#define INktParam_get_ULongValAt(This,index,pVal)	\
    ( (This)->lpVtbl -> get_ULongValAt(This,index,pVal) ) 

#define INktParam_put_ULongValAt(This,index,newValue)	\
    ( (This)->lpVtbl -> put_ULongValAt(This,index,newValue) ) 

#define INktParam_get_LongLongValAt(This,index,pVal)	\
    ( (This)->lpVtbl -> get_LongLongValAt(This,index,pVal) ) 

#define INktParam_put_LongLongValAt(This,index,newValue)	\
    ( (This)->lpVtbl -> put_LongLongValAt(This,index,newValue) ) 

#define INktParam_get_ULongLongValAt(This,index,pVal)	\
    ( (This)->lpVtbl -> get_ULongLongValAt(This,index,pVal) ) 

#define INktParam_put_ULongLongValAt(This,index,newValue)	\
    ( (This)->lpVtbl -> put_ULongLongValAt(This,index,newValue) ) 

#define INktParam_get_SSizeTValAt(This,index,pVal)	\
    ( (This)->lpVtbl -> get_SSizeTValAt(This,index,pVal) ) 

#define INktParam_put_SSizeTValAt(This,index,newValue)	\
    ( (This)->lpVtbl -> put_SSizeTValAt(This,index,newValue) ) 

#define INktParam_get_SizeTValAt(This,index,pVal)	\
    ( (This)->lpVtbl -> get_SizeTValAt(This,index,pVal) ) 

#define INktParam_put_SizeTValAt(This,index,newValue)	\
    ( (This)->lpVtbl -> put_SizeTValAt(This,index,newValue) ) 

#define INktParam_get_FloatValAt(This,index,pVal)	\
    ( (This)->lpVtbl -> get_FloatValAt(This,index,pVal) ) 

#define INktParam_put_FloatValAt(This,index,newValue)	\
    ( (This)->lpVtbl -> put_FloatValAt(This,index,newValue) ) 

#define INktParam_get_DoubleValAt(This,index,pVal)	\
    ( (This)->lpVtbl -> get_DoubleValAt(This,index,pVal) ) 

#define INktParam_put_DoubleValAt(This,index,newValue)	\
    ( (This)->lpVtbl -> put_DoubleValAt(This,index,newValue) ) 

#define INktParam_get_PointerVal(This,pVal)	\
    ( (This)->lpVtbl -> get_PointerVal(This,pVal) ) 

#define INktParam_put_PointerVal(This,newValue)	\
    ( (This)->lpVtbl -> put_PointerVal(This,newValue) ) 

#define INktParam_get_IsNullPointer(This,pVal)	\
    ( (This)->lpVtbl -> get_IsNullPointer(This,pVal) ) 

#define INktParam_Memory(This,ppProcMem)	\
    ( (This)->lpVtbl -> Memory(This,ppProcMem) ) 

#define INktParam_CastTo(This,newType,ppParam)	\
    ( (This)->lpVtbl -> CastTo(This,newType,ppParam) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_Address_Proxy( 
    INktParam * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktParam_get_Address_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_Class_Proxy( 
    INktParam * This,
    /* [retval][out] */ eNktDboClass *pVal);


void __RPC_STUB INktParam_get_Class_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_BasicType_Proxy( 
    INktParam * This,
    /* [retval][out] */ eNktDboFundamentalType *pVal);


void __RPC_STUB INktParam_get_BasicType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_Name_Proxy( 
    INktParam * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktParam_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_TypeName_Proxy( 
    INktParam * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktParam_get_TypeName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_Declaration_Proxy( 
    INktParam * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktParam_get_Declaration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_Flags_Proxy( 
    INktParam * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktParam_get_Flags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_IsPointer_Proxy( 
    INktParam * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktParam_get_IsPointer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_IsArray_Proxy( 
    INktParam * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktParam_get_IsArray_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_ArrayMaxHint_Proxy( 
    INktParam * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktParam_get_ArrayMaxHint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_IsAnsiString_Proxy( 
    INktParam * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktParam_get_IsAnsiString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_IsWideString_Proxy( 
    INktParam * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktParam_get_IsWideString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_IntResourceString_Proxy( 
    INktParam * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktParam_get_IntResourceString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_GuidString_Proxy( 
    INktParam * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktParam_get_GuidString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParam_Evaluate_Proxy( 
    INktParam * This,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktParam_Evaluate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParam_IndexedEvaluate_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktParam_IndexedEvaluate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParam_FullEvaluate_Proxy( 
    INktParam * This,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktParam_FullEvaluate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_FieldsCount_Proxy( 
    INktParam * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktParam_get_FieldsCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParam_Field_Proxy( 
    INktParam * This,
    /* [in] */ VARIANT indexOrName,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktParam_Field_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParam_Fields_Proxy( 
    INktParam * This,
    /* [retval][out] */ INktParamsEnum **ppParamsEnum);


void __RPC_STUB INktParam_Fields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_Value_Proxy( 
    INktParam * This,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB INktParam_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_Value_Proxy( 
    INktParam * This,
    /* [in] */ VARIANT var);


void __RPC_STUB INktParam_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_ValueAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB INktParam_get_ValueAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_ValueAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [in] */ VARIANT var);


void __RPC_STUB INktParam_put_ValueAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_ValueArray_Proxy( 
    INktParam * This,
    /* [in] */ LONG count,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB INktParam_get_ValueArray_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_ValueArrayAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [in] */ LONG count,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB INktParam_get_ValueArrayAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParam_ReadString_Proxy( 
    INktParam * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktParam_ReadString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParam_ReadStringN_Proxy( 
    INktParam * This,
    /* [in] */ LONG maxChars,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktParam_ReadStringN_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParam_WriteString_Proxy( 
    INktParam * This,
    /* [in] */ BSTR str,
    /* [in] */ VARIANT_BOOL isAnsi,
    /* [defaultvalue][in] */ VARIANT_BOOL writeNul);


void __RPC_STUB INktParam_WriteString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParam_StringLength_Proxy( 
    INktParam * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktParam_StringLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_CharVal_Proxy( 
    INktParam * This,
    /* [retval][out] */ signed char *pVal);


void __RPC_STUB INktParam_get_CharVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_CharVal_Proxy( 
    INktParam * This,
    /* [in] */ signed char newValue);


void __RPC_STUB INktParam_put_CharVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_ByteVal_Proxy( 
    INktParam * This,
    /* [retval][out] */ unsigned char *pVal);


void __RPC_STUB INktParam_get_ByteVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_ByteVal_Proxy( 
    INktParam * This,
    /* [in] */ unsigned char newValue);


void __RPC_STUB INktParam_put_ByteVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_ShortVal_Proxy( 
    INktParam * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB INktParam_get_ShortVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_ShortVal_Proxy( 
    INktParam * This,
    /* [in] */ short newValue);


void __RPC_STUB INktParam_put_ShortVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_UShortVal_Proxy( 
    INktParam * This,
    /* [retval][out] */ unsigned short *pVal);


void __RPC_STUB INktParam_get_UShortVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_UShortVal_Proxy( 
    INktParam * This,
    /* [in] */ unsigned short newValue);


void __RPC_STUB INktParam_put_UShortVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_LongVal_Proxy( 
    INktParam * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB INktParam_get_LongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_LongVal_Proxy( 
    INktParam * This,
    /* [in] */ long newValue);


void __RPC_STUB INktParam_put_LongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_ULongVal_Proxy( 
    INktParam * This,
    /* [retval][out] */ unsigned long *pVal);


void __RPC_STUB INktParam_get_ULongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_ULongVal_Proxy( 
    INktParam * This,
    /* [in] */ unsigned long newValue);


void __RPC_STUB INktParam_put_ULongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_LongLongVal_Proxy( 
    INktParam * This,
    /* [retval][out] */ __int64 *pVal);


void __RPC_STUB INktParam_get_LongLongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_LongLongVal_Proxy( 
    INktParam * This,
    /* [in] */ __int64 newValue);


void __RPC_STUB INktParam_put_LongLongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_ULongLongVal_Proxy( 
    INktParam * This,
    /* [retval][out] */ unsigned __int64 *pVal);


void __RPC_STUB INktParam_get_ULongLongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_ULongLongVal_Proxy( 
    INktParam * This,
    /* [in] */ unsigned __int64 newValue);


void __RPC_STUB INktParam_put_ULongLongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_SSizeTVal_Proxy( 
    INktParam * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktParam_get_SSizeTVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_SSizeTVal_Proxy( 
    INktParam * This,
    /* [in] */ my_ssize_t newValue);


void __RPC_STUB INktParam_put_SSizeTVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_SizeTVal_Proxy( 
    INktParam * This,
    /* [retval][out] */ my_size_t *pVal);


void __RPC_STUB INktParam_get_SizeTVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_SizeTVal_Proxy( 
    INktParam * This,
    /* [in] */ my_size_t newValue);


void __RPC_STUB INktParam_put_SizeTVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_FloatVal_Proxy( 
    INktParam * This,
    /* [retval][out] */ float *pVal);


void __RPC_STUB INktParam_get_FloatVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_FloatVal_Proxy( 
    INktParam * This,
    /* [in] */ float newValue);


void __RPC_STUB INktParam_put_FloatVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_DoubleVal_Proxy( 
    INktParam * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktParam_get_DoubleVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_DoubleVal_Proxy( 
    INktParam * This,
    /* [in] */ double newValue);


void __RPC_STUB INktParam_put_DoubleVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_CharValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [retval][out] */ signed char *pVal);


void __RPC_STUB INktParam_get_CharValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_CharValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [in] */ signed char newValue);


void __RPC_STUB INktParam_put_CharValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_ByteValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [retval][out] */ unsigned char *pVal);


void __RPC_STUB INktParam_get_ByteValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_ByteValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [in] */ unsigned char newValue);


void __RPC_STUB INktParam_put_ByteValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_ShortValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [retval][out] */ short *pVal);


void __RPC_STUB INktParam_get_ShortValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_ShortValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [in] */ short newValue);


void __RPC_STUB INktParam_put_ShortValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_UShortValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [retval][out] */ unsigned short *pVal);


void __RPC_STUB INktParam_get_UShortValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_UShortValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [in] */ unsigned short newValue);


void __RPC_STUB INktParam_put_UShortValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_LongValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [retval][out] */ long *pVal);


void __RPC_STUB INktParam_get_LongValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_LongValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [in] */ long newValue);


void __RPC_STUB INktParam_put_LongValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_ULongValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [retval][out] */ unsigned long *pVal);


void __RPC_STUB INktParam_get_ULongValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_ULongValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [in] */ unsigned long newValue);


void __RPC_STUB INktParam_put_ULongValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_LongLongValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [retval][out] */ __int64 *pVal);


void __RPC_STUB INktParam_get_LongLongValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_LongLongValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [in] */ __int64 newValue);


void __RPC_STUB INktParam_put_LongLongValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_ULongLongValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [retval][out] */ unsigned __int64 *pVal);


void __RPC_STUB INktParam_get_ULongLongValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_ULongLongValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [in] */ unsigned __int64 newValue);


void __RPC_STUB INktParam_put_ULongLongValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_SSizeTValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktParam_get_SSizeTValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_SSizeTValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [in] */ my_ssize_t newValue);


void __RPC_STUB INktParam_put_SSizeTValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_SizeTValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [retval][out] */ my_size_t *pVal);


void __RPC_STUB INktParam_get_SizeTValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_SizeTValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [in] */ my_size_t newValue);


void __RPC_STUB INktParam_put_SizeTValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_FloatValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [retval][out] */ float *pVal);


void __RPC_STUB INktParam_get_FloatValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_FloatValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [in] */ float newValue);


void __RPC_STUB INktParam_put_FloatValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_DoubleValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktParam_get_DoubleValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_DoubleValAt_Proxy( 
    INktParam * This,
    /* [in] */ LONG index,
    /* [in] */ double newValue);


void __RPC_STUB INktParam_put_DoubleValAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_PointerVal_Proxy( 
    INktParam * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktParam_get_PointerVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktParam_put_PointerVal_Proxy( 
    INktParam * This,
    /* [in] */ my_ssize_t newValue);


void __RPC_STUB INktParam_put_PointerVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParam_get_IsNullPointer_Proxy( 
    INktParam * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktParam_get_IsNullPointer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParam_Memory_Proxy( 
    INktParam * This,
    /* [retval][out] */ INktProcessMemory **ppProcMem);


void __RPC_STUB INktParam_Memory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParam_CastTo_Proxy( 
    INktParam * This,
    /* [in] */ BSTR newType,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktParam_CastTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktParam_INTERFACE_DEFINED__ */


#ifndef __INktParamsEnum_INTERFACE_DEFINED__
#define __INktParamsEnum_INTERFACE_DEFINED__

/* interface INktParamsEnum */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktParamsEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("718E4DBC-DFC2-4fa8-BFE6-0A8192CEEF5D")
    INktParamsEnum : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ INktParam **ppParam) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE First( 
            /* [retval][out] */ INktParam **ppParam) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Last( 
            /* [retval][out] */ INktParam **ppParam) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ INktParam **ppParam) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Previous( 
            /* [retval][out] */ INktParam **ppParam) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetByDeviareId( 
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ INktParam **ppParam) = 0;
        
        virtual /* [restricted][id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktParamsEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktParamsEnum * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktParamsEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktParamsEnum * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktParamsEnum * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktParamsEnum * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktParamsEnum * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktParamsEnum * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktParamsEnum * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktParamsEnum * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktParamsEnum * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktParamsEnum * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktParamsEnum * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktParamsEnum * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            INktParamsEnum * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            INktParamsEnum * This,
            /* [in] */ LONG index,
            /* [retval][out] */ INktParam **ppParam);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *First )( 
            INktParamsEnum * This,
            /* [retval][out] */ INktParam **ppParam);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Last )( 
            INktParamsEnum * This,
            /* [retval][out] */ INktParam **ppParam);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            INktParamsEnum * This,
            /* [retval][out] */ INktParam **ppParam);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Previous )( 
            INktParamsEnum * This,
            /* [retval][out] */ INktParam **ppParam);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetByDeviareId )( 
            INktParamsEnum * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ INktParam **ppParam);
        
        /* [restricted][id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            INktParamsEnum * This,
            /* [retval][out] */ IUnknown **ppUnk);
        
        END_INTERFACE
    } INktParamsEnumVtbl;

    interface INktParamsEnum
    {
        CONST_VTBL struct INktParamsEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktParamsEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktParamsEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktParamsEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktParamsEnum_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktParamsEnum_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktParamsEnum_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktParamsEnum_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktParamsEnum_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktParamsEnum_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktParamsEnum_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktParamsEnum_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktParamsEnum_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktParamsEnum_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktParamsEnum_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define INktParamsEnum_GetAt(This,index,ppParam)	\
    ( (This)->lpVtbl -> GetAt(This,index,ppParam) ) 

#define INktParamsEnum_First(This,ppParam)	\
    ( (This)->lpVtbl -> First(This,ppParam) ) 

#define INktParamsEnum_Last(This,ppParam)	\
    ( (This)->lpVtbl -> Last(This,ppParam) ) 

#define INktParamsEnum_Next(This,ppParam)	\
    ( (This)->lpVtbl -> Next(This,ppParam) ) 

#define INktParamsEnum_Previous(This,ppParam)	\
    ( (This)->lpVtbl -> Previous(This,ppParam) ) 

#define INktParamsEnum_GetByDeviareId(This,devId,ppParam)	\
    ( (This)->lpVtbl -> GetByDeviareId(This,devId,ppParam) ) 

#define INktParamsEnum_get__NewEnum(This,ppUnk)	\
    ( (This)->lpVtbl -> get__NewEnum(This,ppUnk) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParamsEnum_get_Count_Proxy( 
    INktParamsEnum * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktParamsEnum_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParamsEnum_GetAt_Proxy( 
    INktParamsEnum * This,
    /* [in] */ LONG index,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktParamsEnum_GetAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParamsEnum_First_Proxy( 
    INktParamsEnum * This,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktParamsEnum_First_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParamsEnum_Last_Proxy( 
    INktParamsEnum * This,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktParamsEnum_Last_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParamsEnum_Next_Proxy( 
    INktParamsEnum * This,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktParamsEnum_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParamsEnum_Previous_Proxy( 
    INktParamsEnum * This,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktParamsEnum_Previous_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktParamsEnum_GetByDeviareId_Proxy( 
    INktParamsEnum * This,
    /* [in] */ my_ssize_t devId,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktParamsEnum_GetByDeviareId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktParamsEnum_get__NewEnum_Proxy( 
    INktParamsEnum * This,
    /* [retval][out] */ IUnknown **ppUnk);


void __RPC_STUB INktParamsEnum_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktParamsEnum_INTERFACE_DEFINED__ */


#ifndef __INktHookCallInfo_INTERFACE_DEFINED__
#define __INktHookCallInfo_INTERFACE_DEFINED__

/* interface INktHookCallInfo */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktHookCallInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1A2D8AE6-6CF9-49db-880D-A806D559EE20")
    INktHookCallInfo : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsPreCall( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Cookie( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LastError( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_LastError( 
            /* [in] */ LONG newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentTimeMs( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ElapsedTimeMs( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ChildsElapsedTimeMs( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_KernelModeTimeMs( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_UserModeTimeMs( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CpuClockCycles( 
            /* [retval][out] */ unsigned __int64 *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Hook( 
            /* [retval][out] */ INktHook **ppHook) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Process( 
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ThreadId( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ChainDepth( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE StackTrace( 
            /* [retval][out] */ INktStackTrace **ppStackTrace) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Params( 
            /* [retval][out] */ INktParamsEnum **ppParamsEnum) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Result( 
            /* [retval][out] */ INktParam **ppParam) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE CustomParams( 
            /* [retval][out] */ INktParamsEnum **ppParamsEnum) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Register( 
            /* [in] */ eNktRegister asmReg,
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Register( 
            /* [in] */ eNktRegister asmReg,
            /* [in] */ my_ssize_t newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FloatingRegister( 
            /* [in] */ eNktRegister asmReg,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_FloatingRegister( 
            /* [in] */ eNktRegister asmReg,
            /* [in] */ double newValue) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE SkipCall( void) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ReadIntercallData( 
            /* [in] */ my_ssize_t destAddr,
            /* [in] */ my_ssize_t offset,
            /* [in] */ my_ssize_t bytes) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE WriteIntercallData( 
            /* [in] */ my_ssize_t srcAddr,
            /* [in] */ my_ssize_t offset,
            /* [in] */ my_ssize_t bytes) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktHookCallInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktHookCallInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktHookCallInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktHookCallInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktHookCallInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktHookCallInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktHookCallInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktHookCallInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktHookCallInfo * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktHookCallInfo * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktHookCallInfo * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktHookCallInfo * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktHookCallInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktHookCallInfo * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsPreCall )( 
            INktHookCallInfo * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Cookie )( 
            INktHookCallInfo * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LastError )( 
            INktHookCallInfo * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LastError )( 
            INktHookCallInfo * This,
            /* [in] */ LONG newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CurrentTimeMs )( 
            INktHookCallInfo * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ElapsedTimeMs )( 
            INktHookCallInfo * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ChildsElapsedTimeMs )( 
            INktHookCallInfo * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_KernelModeTimeMs )( 
            INktHookCallInfo * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserModeTimeMs )( 
            INktHookCallInfo * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CpuClockCycles )( 
            INktHookCallInfo * This,
            /* [retval][out] */ unsigned __int64 *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Hook )( 
            INktHookCallInfo * This,
            /* [retval][out] */ INktHook **ppHook);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Process )( 
            INktHookCallInfo * This,
            /* [retval][out] */ INktProcess **ppProc);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ThreadId )( 
            INktHookCallInfo * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ChainDepth )( 
            INktHookCallInfo * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *StackTrace )( 
            INktHookCallInfo * This,
            /* [retval][out] */ INktStackTrace **ppStackTrace);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Params )( 
            INktHookCallInfo * This,
            /* [retval][out] */ INktParamsEnum **ppParamsEnum);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Result )( 
            INktHookCallInfo * This,
            /* [retval][out] */ INktParam **ppParam);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *CustomParams )( 
            INktHookCallInfo * This,
            /* [retval][out] */ INktParamsEnum **ppParamsEnum);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Register )( 
            INktHookCallInfo * This,
            /* [in] */ eNktRegister asmReg,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Register )( 
            INktHookCallInfo * This,
            /* [in] */ eNktRegister asmReg,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FloatingRegister )( 
            INktHookCallInfo * This,
            /* [in] */ eNktRegister asmReg,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_FloatingRegister )( 
            INktHookCallInfo * This,
            /* [in] */ eNktRegister asmReg,
            /* [in] */ double newValue);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *SkipCall )( 
            INktHookCallInfo * This);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReadIntercallData )( 
            INktHookCallInfo * This,
            /* [in] */ my_ssize_t destAddr,
            /* [in] */ my_ssize_t offset,
            /* [in] */ my_ssize_t bytes);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *WriteIntercallData )( 
            INktHookCallInfo * This,
            /* [in] */ my_ssize_t srcAddr,
            /* [in] */ my_ssize_t offset,
            /* [in] */ my_ssize_t bytes);
        
        END_INTERFACE
    } INktHookCallInfoVtbl;

    interface INktHookCallInfo
    {
        CONST_VTBL struct INktHookCallInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktHookCallInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktHookCallInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktHookCallInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktHookCallInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktHookCallInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktHookCallInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktHookCallInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktHookCallInfo_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktHookCallInfo_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktHookCallInfo_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktHookCallInfo_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktHookCallInfo_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktHookCallInfo_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktHookCallInfo_get_IsPreCall(This,pVal)	\
    ( (This)->lpVtbl -> get_IsPreCall(This,pVal) ) 

#define INktHookCallInfo_get_Cookie(This,pVal)	\
    ( (This)->lpVtbl -> get_Cookie(This,pVal) ) 

#define INktHookCallInfo_get_LastError(This,pVal)	\
    ( (This)->lpVtbl -> get_LastError(This,pVal) ) 

#define INktHookCallInfo_put_LastError(This,newValue)	\
    ( (This)->lpVtbl -> put_LastError(This,newValue) ) 

#define INktHookCallInfo_get_CurrentTimeMs(This,pVal)	\
    ( (This)->lpVtbl -> get_CurrentTimeMs(This,pVal) ) 

#define INktHookCallInfo_get_ElapsedTimeMs(This,pVal)	\
    ( (This)->lpVtbl -> get_ElapsedTimeMs(This,pVal) ) 

#define INktHookCallInfo_get_ChildsElapsedTimeMs(This,pVal)	\
    ( (This)->lpVtbl -> get_ChildsElapsedTimeMs(This,pVal) ) 

#define INktHookCallInfo_get_KernelModeTimeMs(This,pVal)	\
    ( (This)->lpVtbl -> get_KernelModeTimeMs(This,pVal) ) 

#define INktHookCallInfo_get_UserModeTimeMs(This,pVal)	\
    ( (This)->lpVtbl -> get_UserModeTimeMs(This,pVal) ) 

#define INktHookCallInfo_get_CpuClockCycles(This,pVal)	\
    ( (This)->lpVtbl -> get_CpuClockCycles(This,pVal) ) 

#define INktHookCallInfo_Hook(This,ppHook)	\
    ( (This)->lpVtbl -> Hook(This,ppHook) ) 

#define INktHookCallInfo_Process(This,ppProc)	\
    ( (This)->lpVtbl -> Process(This,ppProc) ) 

#define INktHookCallInfo_get_ThreadId(This,pVal)	\
    ( (This)->lpVtbl -> get_ThreadId(This,pVal) ) 

#define INktHookCallInfo_get_ChainDepth(This,pVal)	\
    ( (This)->lpVtbl -> get_ChainDepth(This,pVal) ) 

#define INktHookCallInfo_StackTrace(This,ppStackTrace)	\
    ( (This)->lpVtbl -> StackTrace(This,ppStackTrace) ) 

#define INktHookCallInfo_Params(This,ppParamsEnum)	\
    ( (This)->lpVtbl -> Params(This,ppParamsEnum) ) 

#define INktHookCallInfo_Result(This,ppParam)	\
    ( (This)->lpVtbl -> Result(This,ppParam) ) 

#define INktHookCallInfo_CustomParams(This,ppParamsEnum)	\
    ( (This)->lpVtbl -> CustomParams(This,ppParamsEnum) ) 

#define INktHookCallInfo_get_Register(This,asmReg,pVal)	\
    ( (This)->lpVtbl -> get_Register(This,asmReg,pVal) ) 

#define INktHookCallInfo_put_Register(This,asmReg,newValue)	\
    ( (This)->lpVtbl -> put_Register(This,asmReg,newValue) ) 

#define INktHookCallInfo_get_FloatingRegister(This,asmReg,pVal)	\
    ( (This)->lpVtbl -> get_FloatingRegister(This,asmReg,pVal) ) 

#define INktHookCallInfo_put_FloatingRegister(This,asmReg,newValue)	\
    ( (This)->lpVtbl -> put_FloatingRegister(This,asmReg,newValue) ) 

#define INktHookCallInfo_SkipCall(This)	\
    ( (This)->lpVtbl -> SkipCall(This) ) 

#define INktHookCallInfo_ReadIntercallData(This,destAddr,offset,bytes)	\
    ( (This)->lpVtbl -> ReadIntercallData(This,destAddr,offset,bytes) ) 

#define INktHookCallInfo_WriteIntercallData(This,srcAddr,offset,bytes)	\
    ( (This)->lpVtbl -> WriteIntercallData(This,srcAddr,offset,bytes) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_get_IsPreCall_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktHookCallInfo_get_IsPreCall_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_get_Cookie_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktHookCallInfo_get_Cookie_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_get_LastError_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktHookCallInfo_get_LastError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_put_LastError_Proxy( 
    INktHookCallInfo * This,
    /* [in] */ LONG newValue);


void __RPC_STUB INktHookCallInfo_put_LastError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_get_CurrentTimeMs_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktHookCallInfo_get_CurrentTimeMs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_get_ElapsedTimeMs_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktHookCallInfo_get_ElapsedTimeMs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_get_ChildsElapsedTimeMs_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktHookCallInfo_get_ChildsElapsedTimeMs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_get_KernelModeTimeMs_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktHookCallInfo_get_KernelModeTimeMs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_get_UserModeTimeMs_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktHookCallInfo_get_UserModeTimeMs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_get_CpuClockCycles_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ unsigned __int64 *pVal);


void __RPC_STUB INktHookCallInfo_get_CpuClockCycles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_Hook_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ INktHook **ppHook);


void __RPC_STUB INktHookCallInfo_Hook_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_Process_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktHookCallInfo_Process_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_get_ThreadId_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktHookCallInfo_get_ThreadId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_get_ChainDepth_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktHookCallInfo_get_ChainDepth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_StackTrace_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ INktStackTrace **ppStackTrace);


void __RPC_STUB INktHookCallInfo_StackTrace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_Params_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ INktParamsEnum **ppParamsEnum);


void __RPC_STUB INktHookCallInfo_Params_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_Result_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktHookCallInfo_Result_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_CustomParams_Proxy( 
    INktHookCallInfo * This,
    /* [retval][out] */ INktParamsEnum **ppParamsEnum);


void __RPC_STUB INktHookCallInfo_CustomParams_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_get_Register_Proxy( 
    INktHookCallInfo * This,
    /* [in] */ eNktRegister asmReg,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktHookCallInfo_get_Register_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_put_Register_Proxy( 
    INktHookCallInfo * This,
    /* [in] */ eNktRegister asmReg,
    /* [in] */ my_ssize_t newValue);


void __RPC_STUB INktHookCallInfo_put_Register_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_get_FloatingRegister_Proxy( 
    INktHookCallInfo * This,
    /* [in] */ eNktRegister asmReg,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktHookCallInfo_get_FloatingRegister_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_put_FloatingRegister_Proxy( 
    INktHookCallInfo * This,
    /* [in] */ eNktRegister asmReg,
    /* [in] */ double newValue);


void __RPC_STUB INktHookCallInfo_put_FloatingRegister_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_SkipCall_Proxy( 
    INktHookCallInfo * This);


void __RPC_STUB INktHookCallInfo_SkipCall_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_ReadIntercallData_Proxy( 
    INktHookCallInfo * This,
    /* [in] */ my_ssize_t destAddr,
    /* [in] */ my_ssize_t offset,
    /* [in] */ my_ssize_t bytes);


void __RPC_STUB INktHookCallInfo_ReadIntercallData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfo_WriteIntercallData_Proxy( 
    INktHookCallInfo * This,
    /* [in] */ my_ssize_t srcAddr,
    /* [in] */ my_ssize_t offset,
    /* [in] */ my_ssize_t bytes);


void __RPC_STUB INktHookCallInfo_WriteIntercallData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktHookCallInfo_INTERFACE_DEFINED__ */


#ifndef __INktProcessMemory_INTERFACE_DEFINED__
#define __INktProcessMemory_INTERFACE_DEFINED__

/* interface INktProcessMemory */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktProcessMemory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ACE2F499-AA1C-4b36-912F-189F8F8B6765")
    INktProcessMemory : public INktObject
    {
    public:
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetProtection( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [defaultvalue][in] */ VARIANT_BOOL allocated,
            /* [retval][out] */ eNktProtection *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ReadMem( 
            /* [in] */ my_ssize_t localAddr,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ my_ssize_t bytes,
            /* [retval][out] */ my_ssize_t *pReaded) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE WriteMem( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ my_ssize_t localAddr,
            /* [in] */ my_ssize_t bytes,
            /* [retval][out] */ my_ssize_t *pWritten) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Read( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ eNktDboFundamentalType type,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ReadArray( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ eNktDboFundamentalType type,
            /* [in] */ LONG count,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Write( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ VARIANT var) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ReadString( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ VARIANT_BOOL isAnsi,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ReadStringN( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ VARIANT_BOOL isAnsi,
            /* [in] */ LONG maxChars,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE WriteString( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ BSTR str,
            /* [in] */ VARIANT_BOOL asAnsi,
            /* [defaultvalue][in] */ VARIANT_BOOL writeNul = -1) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE StringLength( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ VARIANT_BOOL asAnsi,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CharVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ signed char *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_CharVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ signed char newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ByteVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ unsigned char *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ByteVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ unsigned char newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ShortVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ShortVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ short newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_UShortVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ unsigned short *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_UShortVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ unsigned short newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LongVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_LongVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ long newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ULongVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ unsigned long *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ULongVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ unsigned long newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LongLongVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ __int64 *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_LongLongVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ __int64 newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ULongLongVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ unsigned __int64 *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ULongLongVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ unsigned __int64 newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SSizeTVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_SSizeTVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ my_ssize_t newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SizeTVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ my_size_t *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_SizeTVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ my_size_t newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FloatVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ float *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_FloatVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ float newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DoubleVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DoubleVal( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ double newValue) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE BuildParam( 
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ BSTR type,
            /* [retval][out] */ INktParam **ppParam) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AllocMem( 
            /* [in] */ my_ssize_t bytes,
            /* [in] */ VARIANT_BOOL executeFlag,
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE FreeMem( 
            /* [in] */ my_ssize_t remoteAddr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktProcessMemoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktProcessMemory * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktProcessMemory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktProcessMemory * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktProcessMemory * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktProcessMemory * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktProcessMemory * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktProcessMemory * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktProcessMemory * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktProcessMemory * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktProcessMemory * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktProcessMemory * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetProtection )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [defaultvalue][in] */ VARIANT_BOOL allocated,
            /* [retval][out] */ eNktProtection *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReadMem )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t localAddr,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ my_ssize_t bytes,
            /* [retval][out] */ my_ssize_t *pReaded);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *WriteMem )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ my_ssize_t localAddr,
            /* [in] */ my_ssize_t bytes,
            /* [retval][out] */ my_ssize_t *pWritten);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Read )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ eNktDboFundamentalType type,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReadArray )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ eNktDboFundamentalType type,
            /* [in] */ LONG count,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Write )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ VARIANT var);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReadString )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ VARIANT_BOOL isAnsi,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReadStringN )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ VARIANT_BOOL isAnsi,
            /* [in] */ LONG maxChars,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *WriteString )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ BSTR str,
            /* [in] */ VARIANT_BOOL asAnsi,
            /* [defaultvalue][in] */ VARIANT_BOOL writeNul);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *StringLength )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ VARIANT_BOOL asAnsi,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CharVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ signed char *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CharVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ signed char newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ByteVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ unsigned char *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ByteVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ unsigned char newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShortVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ short *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ShortVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ short newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UShortVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ unsigned short *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UShortVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ unsigned short newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LongVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ long *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LongVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ long newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ULongVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ unsigned long *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ULongVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ unsigned long newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LongLongVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ __int64 *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LongLongVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ __int64 newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ULongLongVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ unsigned __int64 *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ULongLongVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ unsigned __int64 newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SSizeTVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SSizeTVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SizeTVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ my_size_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SizeTVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ my_size_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FloatVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ float *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_FloatVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ float newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DoubleVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DoubleVal )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ double newValue);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *BuildParam )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr,
            /* [in] */ BSTR type,
            /* [retval][out] */ INktParam **ppParam);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AllocMem )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t bytes,
            /* [in] */ VARIANT_BOOL executeFlag,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FreeMem )( 
            INktProcessMemory * This,
            /* [in] */ my_ssize_t remoteAddr);
        
        END_INTERFACE
    } INktProcessMemoryVtbl;

    interface INktProcessMemory
    {
        CONST_VTBL struct INktProcessMemoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktProcessMemory_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktProcessMemory_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktProcessMemory_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktProcessMemory_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktProcessMemory_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktProcessMemory_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktProcessMemory_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktProcessMemory_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktProcessMemory_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktProcessMemory_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktProcessMemory_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktProcessMemory_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktProcessMemory_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktProcessMemory_GetProtection(This,remoteAddr,allocated,pVal)	\
    ( (This)->lpVtbl -> GetProtection(This,remoteAddr,allocated,pVal) ) 

#define INktProcessMemory_ReadMem(This,localAddr,remoteAddr,bytes,pReaded)	\
    ( (This)->lpVtbl -> ReadMem(This,localAddr,remoteAddr,bytes,pReaded) ) 

#define INktProcessMemory_WriteMem(This,remoteAddr,localAddr,bytes,pWritten)	\
    ( (This)->lpVtbl -> WriteMem(This,remoteAddr,localAddr,bytes,pWritten) ) 

#define INktProcessMemory_Read(This,remoteAddr,type,pVal)	\
    ( (This)->lpVtbl -> Read(This,remoteAddr,type,pVal) ) 

#define INktProcessMemory_ReadArray(This,remoteAddr,type,count,pVal)	\
    ( (This)->lpVtbl -> ReadArray(This,remoteAddr,type,count,pVal) ) 

#define INktProcessMemory_Write(This,remoteAddr,var)	\
    ( (This)->lpVtbl -> Write(This,remoteAddr,var) ) 

#define INktProcessMemory_ReadString(This,remoteAddr,isAnsi,pVal)	\
    ( (This)->lpVtbl -> ReadString(This,remoteAddr,isAnsi,pVal) ) 

#define INktProcessMemory_ReadStringN(This,remoteAddr,isAnsi,maxChars,pVal)	\
    ( (This)->lpVtbl -> ReadStringN(This,remoteAddr,isAnsi,maxChars,pVal) ) 

#define INktProcessMemory_WriteString(This,remoteAddr,str,asAnsi,writeNul)	\
    ( (This)->lpVtbl -> WriteString(This,remoteAddr,str,asAnsi,writeNul) ) 

#define INktProcessMemory_StringLength(This,remoteAddr,asAnsi,pVal)	\
    ( (This)->lpVtbl -> StringLength(This,remoteAddr,asAnsi,pVal) ) 

#define INktProcessMemory_get_CharVal(This,remoteAddr,pVal)	\
    ( (This)->lpVtbl -> get_CharVal(This,remoteAddr,pVal) ) 

#define INktProcessMemory_put_CharVal(This,remoteAddr,newValue)	\
    ( (This)->lpVtbl -> put_CharVal(This,remoteAddr,newValue) ) 

#define INktProcessMemory_get_ByteVal(This,remoteAddr,pVal)	\
    ( (This)->lpVtbl -> get_ByteVal(This,remoteAddr,pVal) ) 

#define INktProcessMemory_put_ByteVal(This,remoteAddr,newValue)	\
    ( (This)->lpVtbl -> put_ByteVal(This,remoteAddr,newValue) ) 

#define INktProcessMemory_get_ShortVal(This,remoteAddr,pVal)	\
    ( (This)->lpVtbl -> get_ShortVal(This,remoteAddr,pVal) ) 

#define INktProcessMemory_put_ShortVal(This,remoteAddr,newValue)	\
    ( (This)->lpVtbl -> put_ShortVal(This,remoteAddr,newValue) ) 

#define INktProcessMemory_get_UShortVal(This,remoteAddr,pVal)	\
    ( (This)->lpVtbl -> get_UShortVal(This,remoteAddr,pVal) ) 

#define INktProcessMemory_put_UShortVal(This,remoteAddr,newValue)	\
    ( (This)->lpVtbl -> put_UShortVal(This,remoteAddr,newValue) ) 

#define INktProcessMemory_get_LongVal(This,remoteAddr,pVal)	\
    ( (This)->lpVtbl -> get_LongVal(This,remoteAddr,pVal) ) 

#define INktProcessMemory_put_LongVal(This,remoteAddr,newValue)	\
    ( (This)->lpVtbl -> put_LongVal(This,remoteAddr,newValue) ) 

#define INktProcessMemory_get_ULongVal(This,remoteAddr,pVal)	\
    ( (This)->lpVtbl -> get_ULongVal(This,remoteAddr,pVal) ) 

#define INktProcessMemory_put_ULongVal(This,remoteAddr,newValue)	\
    ( (This)->lpVtbl -> put_ULongVal(This,remoteAddr,newValue) ) 

#define INktProcessMemory_get_LongLongVal(This,remoteAddr,pVal)	\
    ( (This)->lpVtbl -> get_LongLongVal(This,remoteAddr,pVal) ) 

#define INktProcessMemory_put_LongLongVal(This,remoteAddr,newValue)	\
    ( (This)->lpVtbl -> put_LongLongVal(This,remoteAddr,newValue) ) 

#define INktProcessMemory_get_ULongLongVal(This,remoteAddr,pVal)	\
    ( (This)->lpVtbl -> get_ULongLongVal(This,remoteAddr,pVal) ) 

#define INktProcessMemory_put_ULongLongVal(This,remoteAddr,newValue)	\
    ( (This)->lpVtbl -> put_ULongLongVal(This,remoteAddr,newValue) ) 

#define INktProcessMemory_get_SSizeTVal(This,remoteAddr,pVal)	\
    ( (This)->lpVtbl -> get_SSizeTVal(This,remoteAddr,pVal) ) 

#define INktProcessMemory_put_SSizeTVal(This,remoteAddr,newValue)	\
    ( (This)->lpVtbl -> put_SSizeTVal(This,remoteAddr,newValue) ) 

#define INktProcessMemory_get_SizeTVal(This,remoteAddr,pVal)	\
    ( (This)->lpVtbl -> get_SizeTVal(This,remoteAddr,pVal) ) 

#define INktProcessMemory_put_SizeTVal(This,remoteAddr,newValue)	\
    ( (This)->lpVtbl -> put_SizeTVal(This,remoteAddr,newValue) ) 

#define INktProcessMemory_get_FloatVal(This,remoteAddr,pVal)	\
    ( (This)->lpVtbl -> get_FloatVal(This,remoteAddr,pVal) ) 

#define INktProcessMemory_put_FloatVal(This,remoteAddr,newValue)	\
    ( (This)->lpVtbl -> put_FloatVal(This,remoteAddr,newValue) ) 

#define INktProcessMemory_get_DoubleVal(This,remoteAddr,pVal)	\
    ( (This)->lpVtbl -> get_DoubleVal(This,remoteAddr,pVal) ) 

#define INktProcessMemory_put_DoubleVal(This,remoteAddr,newValue)	\
    ( (This)->lpVtbl -> put_DoubleVal(This,remoteAddr,newValue) ) 

#define INktProcessMemory_BuildParam(This,remoteAddr,type,ppParam)	\
    ( (This)->lpVtbl -> BuildParam(This,remoteAddr,type,ppParam) ) 

#define INktProcessMemory_AllocMem(This,bytes,executeFlag,pVal)	\
    ( (This)->lpVtbl -> AllocMem(This,bytes,executeFlag,pVal) ) 

#define INktProcessMemory_FreeMem(This,remoteAddr)	\
    ( (This)->lpVtbl -> FreeMem(This,remoteAddr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_GetProtection_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [defaultvalue][in] */ VARIANT_BOOL allocated,
    /* [retval][out] */ eNktProtection *pVal);


void __RPC_STUB INktProcessMemory_GetProtection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_ReadMem_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t localAddr,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ my_ssize_t bytes,
    /* [retval][out] */ my_ssize_t *pReaded);


void __RPC_STUB INktProcessMemory_ReadMem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_WriteMem_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ my_ssize_t localAddr,
    /* [in] */ my_ssize_t bytes,
    /* [retval][out] */ my_ssize_t *pWritten);


void __RPC_STUB INktProcessMemory_WriteMem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_Read_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ eNktDboFundamentalType type,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB INktProcessMemory_Read_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_ReadArray_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ eNktDboFundamentalType type,
    /* [in] */ LONG count,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB INktProcessMemory_ReadArray_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_Write_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ VARIANT var);


void __RPC_STUB INktProcessMemory_Write_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_ReadString_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ VARIANT_BOOL isAnsi,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktProcessMemory_ReadString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_ReadStringN_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ VARIANT_BOOL isAnsi,
    /* [in] */ LONG maxChars,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktProcessMemory_ReadStringN_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_WriteString_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ BSTR str,
    /* [in] */ VARIANT_BOOL asAnsi,
    /* [defaultvalue][in] */ VARIANT_BOOL writeNul);


void __RPC_STUB INktProcessMemory_WriteString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_StringLength_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ VARIANT_BOOL asAnsi,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktProcessMemory_StringLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_get_CharVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [retval][out] */ signed char *pVal);


void __RPC_STUB INktProcessMemory_get_CharVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_put_CharVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ signed char newValue);


void __RPC_STUB INktProcessMemory_put_CharVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_get_ByteVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [retval][out] */ unsigned char *pVal);


void __RPC_STUB INktProcessMemory_get_ByteVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_put_ByteVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ unsigned char newValue);


void __RPC_STUB INktProcessMemory_put_ByteVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_get_ShortVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [retval][out] */ short *pVal);


void __RPC_STUB INktProcessMemory_get_ShortVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_put_ShortVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ short newValue);


void __RPC_STUB INktProcessMemory_put_ShortVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_get_UShortVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [retval][out] */ unsigned short *pVal);


void __RPC_STUB INktProcessMemory_get_UShortVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_put_UShortVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ unsigned short newValue);


void __RPC_STUB INktProcessMemory_put_UShortVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_get_LongVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [retval][out] */ long *pVal);


void __RPC_STUB INktProcessMemory_get_LongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_put_LongVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ long newValue);


void __RPC_STUB INktProcessMemory_put_LongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_get_ULongVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [retval][out] */ unsigned long *pVal);


void __RPC_STUB INktProcessMemory_get_ULongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_put_ULongVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ unsigned long newValue);


void __RPC_STUB INktProcessMemory_put_ULongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_get_LongLongVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [retval][out] */ __int64 *pVal);


void __RPC_STUB INktProcessMemory_get_LongLongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_put_LongLongVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ __int64 newValue);


void __RPC_STUB INktProcessMemory_put_LongLongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_get_ULongLongVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [retval][out] */ unsigned __int64 *pVal);


void __RPC_STUB INktProcessMemory_get_ULongLongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_put_ULongLongVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ unsigned __int64 newValue);


void __RPC_STUB INktProcessMemory_put_ULongLongVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_get_SSizeTVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktProcessMemory_get_SSizeTVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_put_SSizeTVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ my_ssize_t newValue);


void __RPC_STUB INktProcessMemory_put_SSizeTVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_get_SizeTVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [retval][out] */ my_size_t *pVal);


void __RPC_STUB INktProcessMemory_get_SizeTVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_put_SizeTVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ my_size_t newValue);


void __RPC_STUB INktProcessMemory_put_SizeTVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_get_FloatVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [retval][out] */ float *pVal);


void __RPC_STUB INktProcessMemory_get_FloatVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_put_FloatVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ float newValue);


void __RPC_STUB INktProcessMemory_put_FloatVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_get_DoubleVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktProcessMemory_get_DoubleVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_put_DoubleVal_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ double newValue);


void __RPC_STUB INktProcessMemory_put_DoubleVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_BuildParam_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr,
    /* [in] */ BSTR type,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktProcessMemory_BuildParam_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_AllocMem_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t bytes,
    /* [in] */ VARIANT_BOOL executeFlag,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktProcessMemory_AllocMem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktProcessMemory_FreeMem_Proxy( 
    INktProcessMemory * This,
    /* [in] */ my_ssize_t remoteAddr);


void __RPC_STUB INktProcessMemory_FreeMem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktProcessMemory_INTERFACE_DEFINED__ */


#ifndef __INktStackTrace_INTERFACE_DEFINED__
#define __INktStackTrace_INTERFACE_DEFINED__

/* interface INktStackTrace */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktStackTrace;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("42B03B4D-C3DA-463d-98B7-73405754A764")
    INktStackTrace : public INktObject
    {
    public:
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE NearestFunction( 
            /* [in] */ LONG depth,
            /* [retval][out] */ INktExportedFunction **ppExpFunc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Module( 
            /* [in] */ LONG depth,
            /* [retval][out] */ INktModule **ppMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Offset( 
            /* [in] */ LONG depth,
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Address( 
            /* [in] */ LONG depth,
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE NearestSymbol( 
            /* [in] */ LONG depth,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Params( 
            /* [in] */ LONG depth,
            /* [retval][out] */ INktParamsEnum **ppParamsEnum) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Result( 
            /* [in] */ LONG depth,
            /* [retval][out] */ INktParam **ppParam) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktStackTraceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktStackTrace * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktStackTrace * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktStackTrace * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktStackTrace * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktStackTrace * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktStackTrace * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktStackTrace * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktStackTrace * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktStackTrace * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktStackTrace * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktStackTrace * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktStackTrace * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktStackTrace * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *NearestFunction )( 
            INktStackTrace * This,
            /* [in] */ LONG depth,
            /* [retval][out] */ INktExportedFunction **ppExpFunc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Module )( 
            INktStackTrace * This,
            /* [in] */ LONG depth,
            /* [retval][out] */ INktModule **ppMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Offset )( 
            INktStackTrace * This,
            /* [in] */ LONG depth,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Address )( 
            INktStackTrace * This,
            /* [in] */ LONG depth,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *NearestSymbol )( 
            INktStackTrace * This,
            /* [in] */ LONG depth,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Params )( 
            INktStackTrace * This,
            /* [in] */ LONG depth,
            /* [retval][out] */ INktParamsEnum **ppParamsEnum);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Result )( 
            INktStackTrace * This,
            /* [in] */ LONG depth,
            /* [retval][out] */ INktParam **ppParam);
        
        END_INTERFACE
    } INktStackTraceVtbl;

    interface INktStackTrace
    {
        CONST_VTBL struct INktStackTraceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktStackTrace_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktStackTrace_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktStackTrace_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktStackTrace_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktStackTrace_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktStackTrace_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktStackTrace_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktStackTrace_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktStackTrace_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktStackTrace_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktStackTrace_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktStackTrace_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktStackTrace_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktStackTrace_NearestFunction(This,depth,ppExpFunc)	\
    ( (This)->lpVtbl -> NearestFunction(This,depth,ppExpFunc) ) 

#define INktStackTrace_Module(This,depth,ppMod)	\
    ( (This)->lpVtbl -> Module(This,depth,ppMod) ) 

#define INktStackTrace_Offset(This,depth,pVal)	\
    ( (This)->lpVtbl -> Offset(This,depth,pVal) ) 

#define INktStackTrace_Address(This,depth,pVal)	\
    ( (This)->lpVtbl -> Address(This,depth,pVal) ) 

#define INktStackTrace_NearestSymbol(This,depth,pVal)	\
    ( (This)->lpVtbl -> NearestSymbol(This,depth,pVal) ) 

#define INktStackTrace_Params(This,depth,ppParamsEnum)	\
    ( (This)->lpVtbl -> Params(This,depth,ppParamsEnum) ) 

#define INktStackTrace_Result(This,depth,ppParam)	\
    ( (This)->lpVtbl -> Result(This,depth,ppParam) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktStackTrace_NearestFunction_Proxy( 
    INktStackTrace * This,
    /* [in] */ LONG depth,
    /* [retval][out] */ INktExportedFunction **ppExpFunc);


void __RPC_STUB INktStackTrace_NearestFunction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktStackTrace_Module_Proxy( 
    INktStackTrace * This,
    /* [in] */ LONG depth,
    /* [retval][out] */ INktModule **ppMod);


void __RPC_STUB INktStackTrace_Module_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktStackTrace_Offset_Proxy( 
    INktStackTrace * This,
    /* [in] */ LONG depth,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktStackTrace_Offset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktStackTrace_Address_Proxy( 
    INktStackTrace * This,
    /* [in] */ LONG depth,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktStackTrace_Address_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktStackTrace_NearestSymbol_Proxy( 
    INktStackTrace * This,
    /* [in] */ LONG depth,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktStackTrace_NearestSymbol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktStackTrace_Params_Proxy( 
    INktStackTrace * This,
    /* [in] */ LONG depth,
    /* [retval][out] */ INktParamsEnum **ppParamsEnum);


void __RPC_STUB INktStackTrace_Params_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktStackTrace_Result_Proxy( 
    INktStackTrace * This,
    /* [in] */ LONG depth,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktStackTrace_Result_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktStackTrace_INTERFACE_DEFINED__ */


#ifndef __INktSpyMgr_INTERFACE_DEFINED__
#define __INktSpyMgr_INTERFACE_DEFINED__

/* interface INktSpyMgr */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktSpyMgr;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DD71FF07-B1E6-430e-8439-53B760E90062")
    INktSpyMgr : public INktObject
    {
    public:
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE SettingOverride( 
            /* [in] */ BSTR setting,
            /* [in] */ LONG newValue) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DatabasePath( 
            /* [in] */ BSTR newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AgentPath( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_AgentPath( 
            /* [in] */ BSTR newValue) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Initialize( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Processes( 
            /* [retval][out] */ INktProcessesEnum **ppProcsEnum) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Hooks( 
            /* [retval][out] */ INktHooksEnum **ppHooksEnum) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE CreateHooksCollection( 
            /* [retval][out] */ INktHooksEnum **ppHooksColl) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE CreateHook( 
            /* [in] */ VARIANT func,
            /* [in] */ LONG hookFlags,
            /* [retval][out] */ INktHook **ppHook) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE CreateHookForAddress( 
            /* [in] */ my_ssize_t addr,
            /* [in] */ BSTR functionName,
            /* [in] */ LONG hookFlags,
            /* [retval][out] */ INktHook **ppHook) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE DestroyHook( 
            /* [in] */ INktHook *hook) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE LoadAgent( 
            /* [in] */ VARIANT procOrId) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE UnloadAgent( 
            /* [in] */ VARIANT procOrId) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE LoadCustomDll( 
            /* [in] */ VARIANT procOrId,
            /* [in] */ BSTR dllFileName,
            /* [defaultvalue][in] */ VARIANT_BOOL unloadOnAgentShutdown,
            /* [defaultvalue][in] */ VARIANT_BOOL sync,
            /* [retval][out] */ LONG *pActionId) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE UnloadCustomDll( 
            /* [in] */ VARIANT procOrId,
            /* [in] */ BSTR dllFileName,
            /* [defaultvalue][in] */ VARIANT_BOOL sync,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE CallCustomApi( 
            /* [in] */ VARIANT procOrId,
            /* [in] */ BSTR dllFileName,
            /* [in] */ BSTR apiToCall,
            /* [defaultvalue][in] */ VARIANT *params,
            /* [defaultvalue][in] */ VARIANT_BOOL sync,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ProcessFromPID( 
            /* [in] */ LONG pid,
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ProcessMemoryFromPID( 
            /* [in] */ LONG pid,
            /* [retval][out] */ INktProcessMemory **ppProcMem) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE CreateProcess( 
            /* [in] */ BSTR imagePath,
            /* [defaultvalue][in] */ VARIANT_BOOL suspended,
            /* [defaultvalue][out] */ VARIANT *continueEvent,
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE CreateProcessWithLogon( 
            /* [in] */ BSTR imagePath,
            /* [in] */ BSTR userName,
            /* [in] */ BSTR password,
            /* [defaultvalue][in] */ VARIANT_BOOL suspended,
            /* [defaultvalue][out] */ VARIANT *continueEvent,
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE CreateProcessWithToken( 
            /* [in] */ BSTR imagePath,
            /* [in] */ my_ssize_t token,
            /* [defaultvalue][in] */ VARIANT_BOOL suspended,
            /* [defaultvalue][out] */ VARIANT *continueEvent,
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ResumeProcess( 
            /* [in] */ INktProcess *pProc,
            /* [in] */ VARIANT continueEvent) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE TerminateProcess( 
            /* [in] */ VARIANT procOrId,
            /* [in] */ LONG exitCode) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE FindProcessId( 
            /* [in] */ BSTR name,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE RealFilePath( 
            /* [in] */ BSTR name,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE DbObjects( 
            /* [in] */ LONG platformBits,
            /* [retval][out] */ INktDbObjectsEnum **ppObjsEnum) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE DbFunctions( 
            /* [in] */ LONG platformBits,
            /* [retval][out] */ INktDbObjectsEnum **ppFuncsEnum) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE DbModules( 
            /* [in] */ LONG platformBits,
            /* [retval][out] */ INktDbModulesEnum **ppModsEnum) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ProcessHandle( 
            /* [in] */ VARIANT procOrId,
            /* [in] */ LONG desiredAccess,
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ProcessMemory( 
            /* [in] */ VARIANT procOrId,
            /* [retval][out] */ INktProcessMemory **ppProcMem) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetWindowProc( 
            /* [in] */ VARIANT procOrId,
            /* [in] */ my_ssize_t hWnd,
            /* [retval][out] */ my_ssize_t *ppWndProc) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktSpyMgrVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktSpyMgr * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktSpyMgr * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktSpyMgr * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktSpyMgr * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktSpyMgr * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktSpyMgr * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktSpyMgr * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktSpyMgr * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktSpyMgr * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktSpyMgr * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktSpyMgr * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktSpyMgr * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktSpyMgr * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *SettingOverride )( 
            INktSpyMgr * This,
            /* [in] */ BSTR setting,
            /* [in] */ LONG newValue);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DatabasePath )( 
            INktSpyMgr * This,
            /* [in] */ BSTR newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AgentPath )( 
            INktSpyMgr * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AgentPath )( 
            INktSpyMgr * This,
            /* [in] */ BSTR newValue);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            INktSpyMgr * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Processes )( 
            INktSpyMgr * This,
            /* [retval][out] */ INktProcessesEnum **ppProcsEnum);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Hooks )( 
            INktSpyMgr * This,
            /* [retval][out] */ INktHooksEnum **ppHooksEnum);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *CreateHooksCollection )( 
            INktSpyMgr * This,
            /* [retval][out] */ INktHooksEnum **ppHooksColl);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *CreateHook )( 
            INktSpyMgr * This,
            /* [in] */ VARIANT func,
            /* [in] */ LONG hookFlags,
            /* [retval][out] */ INktHook **ppHook);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *CreateHookForAddress )( 
            INktSpyMgr * This,
            /* [in] */ my_ssize_t addr,
            /* [in] */ BSTR functionName,
            /* [in] */ LONG hookFlags,
            /* [retval][out] */ INktHook **ppHook);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *DestroyHook )( 
            INktSpyMgr * This,
            /* [in] */ INktHook *hook);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *LoadAgent )( 
            INktSpyMgr * This,
            /* [in] */ VARIANT procOrId);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *UnloadAgent )( 
            INktSpyMgr * This,
            /* [in] */ VARIANT procOrId);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *LoadCustomDll )( 
            INktSpyMgr * This,
            /* [in] */ VARIANT procOrId,
            /* [in] */ BSTR dllFileName,
            /* [defaultvalue][in] */ VARIANT_BOOL unloadOnAgentShutdown,
            /* [defaultvalue][in] */ VARIANT_BOOL sync,
            /* [retval][out] */ LONG *pActionId);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *UnloadCustomDll )( 
            INktSpyMgr * This,
            /* [in] */ VARIANT procOrId,
            /* [in] */ BSTR dllFileName,
            /* [defaultvalue][in] */ VARIANT_BOOL sync,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *CallCustomApi )( 
            INktSpyMgr * This,
            /* [in] */ VARIANT procOrId,
            /* [in] */ BSTR dllFileName,
            /* [in] */ BSTR apiToCall,
            /* [defaultvalue][in] */ VARIANT *params,
            /* [defaultvalue][in] */ VARIANT_BOOL sync,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ProcessFromPID )( 
            INktSpyMgr * This,
            /* [in] */ LONG pid,
            /* [retval][out] */ INktProcess **ppProc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ProcessMemoryFromPID )( 
            INktSpyMgr * This,
            /* [in] */ LONG pid,
            /* [retval][out] */ INktProcessMemory **ppProcMem);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *CreateProcess )( 
            INktSpyMgr * This,
            /* [in] */ BSTR imagePath,
            /* [defaultvalue][in] */ VARIANT_BOOL suspended,
            /* [defaultvalue][out] */ VARIANT *continueEvent,
            /* [retval][out] */ INktProcess **ppProc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *CreateProcessWithLogon )( 
            INktSpyMgr * This,
            /* [in] */ BSTR imagePath,
            /* [in] */ BSTR userName,
            /* [in] */ BSTR password,
            /* [defaultvalue][in] */ VARIANT_BOOL suspended,
            /* [defaultvalue][out] */ VARIANT *continueEvent,
            /* [retval][out] */ INktProcess **ppProc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *CreateProcessWithToken )( 
            INktSpyMgr * This,
            /* [in] */ BSTR imagePath,
            /* [in] */ my_ssize_t token,
            /* [defaultvalue][in] */ VARIANT_BOOL suspended,
            /* [defaultvalue][out] */ VARIANT *continueEvent,
            /* [retval][out] */ INktProcess **ppProc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ResumeProcess )( 
            INktSpyMgr * This,
            /* [in] */ INktProcess *pProc,
            /* [in] */ VARIANT continueEvent);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *TerminateProcess )( 
            INktSpyMgr * This,
            /* [in] */ VARIANT procOrId,
            /* [in] */ LONG exitCode);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FindProcessId )( 
            INktSpyMgr * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *RealFilePath )( 
            INktSpyMgr * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *DbObjects )( 
            INktSpyMgr * This,
            /* [in] */ LONG platformBits,
            /* [retval][out] */ INktDbObjectsEnum **ppObjsEnum);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *DbFunctions )( 
            INktSpyMgr * This,
            /* [in] */ LONG platformBits,
            /* [retval][out] */ INktDbObjectsEnum **ppFuncsEnum);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *DbModules )( 
            INktSpyMgr * This,
            /* [in] */ LONG platformBits,
            /* [retval][out] */ INktDbModulesEnum **ppModsEnum);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ProcessHandle )( 
            INktSpyMgr * This,
            /* [in] */ VARIANT procOrId,
            /* [in] */ LONG desiredAccess,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ProcessMemory )( 
            INktSpyMgr * This,
            /* [in] */ VARIANT procOrId,
            /* [retval][out] */ INktProcessMemory **ppProcMem);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetWindowProc )( 
            INktSpyMgr * This,
            /* [in] */ VARIANT procOrId,
            /* [in] */ my_ssize_t hWnd,
            /* [retval][out] */ my_ssize_t *ppWndProc);
        
        END_INTERFACE
    } INktSpyMgrVtbl;

    interface INktSpyMgr
    {
        CONST_VTBL struct INktSpyMgrVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktSpyMgr_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktSpyMgr_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktSpyMgr_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktSpyMgr_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktSpyMgr_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktSpyMgr_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktSpyMgr_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktSpyMgr_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktSpyMgr_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktSpyMgr_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktSpyMgr_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktSpyMgr_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktSpyMgr_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktSpyMgr_SettingOverride(This,setting,newValue)	\
    ( (This)->lpVtbl -> SettingOverride(This,setting,newValue) ) 

#define INktSpyMgr_put_DatabasePath(This,newValue)	\
    ( (This)->lpVtbl -> put_DatabasePath(This,newValue) ) 

#define INktSpyMgr_get_AgentPath(This,pVal)	\
    ( (This)->lpVtbl -> get_AgentPath(This,pVal) ) 

#define INktSpyMgr_put_AgentPath(This,newValue)	\
    ( (This)->lpVtbl -> put_AgentPath(This,newValue) ) 

#define INktSpyMgr_Initialize(This,pVal)	\
    ( (This)->lpVtbl -> Initialize(This,pVal) ) 

#define INktSpyMgr_Processes(This,ppProcsEnum)	\
    ( (This)->lpVtbl -> Processes(This,ppProcsEnum) ) 

#define INktSpyMgr_Hooks(This,ppHooksEnum)	\
    ( (This)->lpVtbl -> Hooks(This,ppHooksEnum) ) 

#define INktSpyMgr_CreateHooksCollection(This,ppHooksColl)	\
    ( (This)->lpVtbl -> CreateHooksCollection(This,ppHooksColl) ) 

#define INktSpyMgr_CreateHook(This,func,hookFlags,ppHook)	\
    ( (This)->lpVtbl -> CreateHook(This,func,hookFlags,ppHook) ) 

#define INktSpyMgr_CreateHookForAddress(This,addr,functionName,hookFlags,ppHook)	\
    ( (This)->lpVtbl -> CreateHookForAddress(This,addr,functionName,hookFlags,ppHook) ) 

#define INktSpyMgr_DestroyHook(This,hook)	\
    ( (This)->lpVtbl -> DestroyHook(This,hook) ) 

#define INktSpyMgr_LoadAgent(This,procOrId)	\
    ( (This)->lpVtbl -> LoadAgent(This,procOrId) ) 

#define INktSpyMgr_UnloadAgent(This,procOrId)	\
    ( (This)->lpVtbl -> UnloadAgent(This,procOrId) ) 

#define INktSpyMgr_LoadCustomDll(This,procOrId,dllFileName,unloadOnAgentShutdown,sync,pActionId)	\
    ( (This)->lpVtbl -> LoadCustomDll(This,procOrId,dllFileName,unloadOnAgentShutdown,sync,pActionId) ) 

#define INktSpyMgr_UnloadCustomDll(This,procOrId,dllFileName,sync,pVal)	\
    ( (This)->lpVtbl -> UnloadCustomDll(This,procOrId,dllFileName,sync,pVal) ) 

#define INktSpyMgr_CallCustomApi(This,procOrId,dllFileName,apiToCall,params,sync,pVal)	\
    ( (This)->lpVtbl -> CallCustomApi(This,procOrId,dllFileName,apiToCall,params,sync,pVal) ) 

#define INktSpyMgr_ProcessFromPID(This,pid,ppProc)	\
    ( (This)->lpVtbl -> ProcessFromPID(This,pid,ppProc) ) 

#define INktSpyMgr_ProcessMemoryFromPID(This,pid,ppProcMem)	\
    ( (This)->lpVtbl -> ProcessMemoryFromPID(This,pid,ppProcMem) ) 

#define INktSpyMgr_CreateProcess(This,imagePath,suspended,continueEvent,ppProc)	\
    ( (This)->lpVtbl -> CreateProcess(This,imagePath,suspended,continueEvent,ppProc) ) 

#define INktSpyMgr_CreateProcessWithLogon(This,imagePath,userName,password,suspended,continueEvent,ppProc)	\
    ( (This)->lpVtbl -> CreateProcessWithLogon(This,imagePath,userName,password,suspended,continueEvent,ppProc) ) 

#define INktSpyMgr_CreateProcessWithToken(This,imagePath,token,suspended,continueEvent,ppProc)	\
    ( (This)->lpVtbl -> CreateProcessWithToken(This,imagePath,token,suspended,continueEvent,ppProc) ) 

#define INktSpyMgr_ResumeProcess(This,pProc,continueEvent)	\
    ( (This)->lpVtbl -> ResumeProcess(This,pProc,continueEvent) ) 

#define INktSpyMgr_TerminateProcess(This,procOrId,exitCode)	\
    ( (This)->lpVtbl -> TerminateProcess(This,procOrId,exitCode) ) 

#define INktSpyMgr_FindProcessId(This,name,pVal)	\
    ( (This)->lpVtbl -> FindProcessId(This,name,pVal) ) 

#define INktSpyMgr_RealFilePath(This,name,pVal)	\
    ( (This)->lpVtbl -> RealFilePath(This,name,pVal) ) 

#define INktSpyMgr_DbObjects(This,platformBits,ppObjsEnum)	\
    ( (This)->lpVtbl -> DbObjects(This,platformBits,ppObjsEnum) ) 

#define INktSpyMgr_DbFunctions(This,platformBits,ppFuncsEnum)	\
    ( (This)->lpVtbl -> DbFunctions(This,platformBits,ppFuncsEnum) ) 

#define INktSpyMgr_DbModules(This,platformBits,ppModsEnum)	\
    ( (This)->lpVtbl -> DbModules(This,platformBits,ppModsEnum) ) 

#define INktSpyMgr_ProcessHandle(This,procOrId,desiredAccess,pVal)	\
    ( (This)->lpVtbl -> ProcessHandle(This,procOrId,desiredAccess,pVal) ) 

#define INktSpyMgr_ProcessMemory(This,procOrId,ppProcMem)	\
    ( (This)->lpVtbl -> ProcessMemory(This,procOrId,ppProcMem) ) 

#define INktSpyMgr_GetWindowProc(This,procOrId,hWnd,ppWndProc)	\
    ( (This)->lpVtbl -> GetWindowProc(This,procOrId,hWnd,ppWndProc) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_SettingOverride_Proxy( 
    INktSpyMgr * This,
    /* [in] */ BSTR setting,
    /* [in] */ LONG newValue);


void __RPC_STUB INktSpyMgr_SettingOverride_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_put_DatabasePath_Proxy( 
    INktSpyMgr * This,
    /* [in] */ BSTR newValue);


void __RPC_STUB INktSpyMgr_put_DatabasePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_get_AgentPath_Proxy( 
    INktSpyMgr * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktSpyMgr_get_AgentPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_put_AgentPath_Proxy( 
    INktSpyMgr * This,
    /* [in] */ BSTR newValue);


void __RPC_STUB INktSpyMgr_put_AgentPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_Initialize_Proxy( 
    INktSpyMgr * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktSpyMgr_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_Processes_Proxy( 
    INktSpyMgr * This,
    /* [retval][out] */ INktProcessesEnum **ppProcsEnum);


void __RPC_STUB INktSpyMgr_Processes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_Hooks_Proxy( 
    INktSpyMgr * This,
    /* [retval][out] */ INktHooksEnum **ppHooksEnum);


void __RPC_STUB INktSpyMgr_Hooks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_CreateHooksCollection_Proxy( 
    INktSpyMgr * This,
    /* [retval][out] */ INktHooksEnum **ppHooksColl);


void __RPC_STUB INktSpyMgr_CreateHooksCollection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_CreateHook_Proxy( 
    INktSpyMgr * This,
    /* [in] */ VARIANT func,
    /* [in] */ LONG hookFlags,
    /* [retval][out] */ INktHook **ppHook);


void __RPC_STUB INktSpyMgr_CreateHook_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_CreateHookForAddress_Proxy( 
    INktSpyMgr * This,
    /* [in] */ my_ssize_t addr,
    /* [in] */ BSTR functionName,
    /* [in] */ LONG hookFlags,
    /* [retval][out] */ INktHook **ppHook);


void __RPC_STUB INktSpyMgr_CreateHookForAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_DestroyHook_Proxy( 
    INktSpyMgr * This,
    /* [in] */ INktHook *hook);


void __RPC_STUB INktSpyMgr_DestroyHook_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_LoadAgent_Proxy( 
    INktSpyMgr * This,
    /* [in] */ VARIANT procOrId);


void __RPC_STUB INktSpyMgr_LoadAgent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_UnloadAgent_Proxy( 
    INktSpyMgr * This,
    /* [in] */ VARIANT procOrId);


void __RPC_STUB INktSpyMgr_UnloadAgent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_LoadCustomDll_Proxy( 
    INktSpyMgr * This,
    /* [in] */ VARIANT procOrId,
    /* [in] */ BSTR dllFileName,
    /* [defaultvalue][in] */ VARIANT_BOOL unloadOnAgentShutdown,
    /* [defaultvalue][in] */ VARIANT_BOOL sync,
    /* [retval][out] */ LONG *pActionId);


void __RPC_STUB INktSpyMgr_LoadCustomDll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_UnloadCustomDll_Proxy( 
    INktSpyMgr * This,
    /* [in] */ VARIANT procOrId,
    /* [in] */ BSTR dllFileName,
    /* [defaultvalue][in] */ VARIANT_BOOL sync,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktSpyMgr_UnloadCustomDll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_CallCustomApi_Proxy( 
    INktSpyMgr * This,
    /* [in] */ VARIANT procOrId,
    /* [in] */ BSTR dllFileName,
    /* [in] */ BSTR apiToCall,
    /* [defaultvalue][in] */ VARIANT *params,
    /* [defaultvalue][in] */ VARIANT_BOOL sync,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktSpyMgr_CallCustomApi_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_ProcessFromPID_Proxy( 
    INktSpyMgr * This,
    /* [in] */ LONG pid,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktSpyMgr_ProcessFromPID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_ProcessMemoryFromPID_Proxy( 
    INktSpyMgr * This,
    /* [in] */ LONG pid,
    /* [retval][out] */ INktProcessMemory **ppProcMem);


void __RPC_STUB INktSpyMgr_ProcessMemoryFromPID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_CreateProcess_Proxy( 
    INktSpyMgr * This,
    /* [in] */ BSTR imagePath,
    /* [defaultvalue][in] */ VARIANT_BOOL suspended,
    /* [defaultvalue][out] */ VARIANT *continueEvent,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktSpyMgr_CreateProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_CreateProcessWithLogon_Proxy( 
    INktSpyMgr * This,
    /* [in] */ BSTR imagePath,
    /* [in] */ BSTR userName,
    /* [in] */ BSTR password,
    /* [defaultvalue][in] */ VARIANT_BOOL suspended,
    /* [defaultvalue][out] */ VARIANT *continueEvent,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktSpyMgr_CreateProcessWithLogon_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_CreateProcessWithToken_Proxy( 
    INktSpyMgr * This,
    /* [in] */ BSTR imagePath,
    /* [in] */ my_ssize_t token,
    /* [defaultvalue][in] */ VARIANT_BOOL suspended,
    /* [defaultvalue][out] */ VARIANT *continueEvent,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktSpyMgr_CreateProcessWithToken_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_ResumeProcess_Proxy( 
    INktSpyMgr * This,
    /* [in] */ INktProcess *pProc,
    /* [in] */ VARIANT continueEvent);


void __RPC_STUB INktSpyMgr_ResumeProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_TerminateProcess_Proxy( 
    INktSpyMgr * This,
    /* [in] */ VARIANT procOrId,
    /* [in] */ LONG exitCode);


void __RPC_STUB INktSpyMgr_TerminateProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_FindProcessId_Proxy( 
    INktSpyMgr * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktSpyMgr_FindProcessId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_RealFilePath_Proxy( 
    INktSpyMgr * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktSpyMgr_RealFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_DbObjects_Proxy( 
    INktSpyMgr * This,
    /* [in] */ LONG platformBits,
    /* [retval][out] */ INktDbObjectsEnum **ppObjsEnum);


void __RPC_STUB INktSpyMgr_DbObjects_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_DbFunctions_Proxy( 
    INktSpyMgr * This,
    /* [in] */ LONG platformBits,
    /* [retval][out] */ INktDbObjectsEnum **ppFuncsEnum);


void __RPC_STUB INktSpyMgr_DbFunctions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_DbModules_Proxy( 
    INktSpyMgr * This,
    /* [in] */ LONG platformBits,
    /* [retval][out] */ INktDbModulesEnum **ppModsEnum);


void __RPC_STUB INktSpyMgr_DbModules_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_ProcessHandle_Proxy( 
    INktSpyMgr * This,
    /* [in] */ VARIANT procOrId,
    /* [in] */ LONG desiredAccess,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktSpyMgr_ProcessHandle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_ProcessMemory_Proxy( 
    INktSpyMgr * This,
    /* [in] */ VARIANT procOrId,
    /* [retval][out] */ INktProcessMemory **ppProcMem);


void __RPC_STUB INktSpyMgr_ProcessMemory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktSpyMgr_GetWindowProc_Proxy( 
    INktSpyMgr * This,
    /* [in] */ VARIANT procOrId,
    /* [in] */ my_ssize_t hWnd,
    /* [retval][out] */ my_ssize_t *ppWndProc);


void __RPC_STUB INktSpyMgr_GetWindowProc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktSpyMgr_INTERFACE_DEFINED__ */


#ifndef __INktHookCallInfoPlugin_INTERFACE_DEFINED__
#define __INktHookCallInfoPlugin_INTERFACE_DEFINED__

/* interface INktHookCallInfoPlugin */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktHookCallInfoPlugin;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7630D7B-12E3-407a-8056-CF08A25B3A07")
    INktHookCallInfoPlugin : public IDispatch
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsPreCall( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Cookie( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LastError( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_LastError( 
            /* [in] */ LONG newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentTimeMs( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ElapsedTimeMs( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ChildsElapsedTimeMs( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_KernelModeTimeMs( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_UserModeTimeMs( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CpuClockCycles( 
            /* [retval][out] */ unsigned __int64 *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_HookInfo( 
            /* [retval][out] */ INktHookInfo **pppHookInfo) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ThreadId( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ChainDepth( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE CurrentProcess( 
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE StackTrace( 
            /* [retval][out] */ INktStackTrace **ppStackTrace) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Params( 
            /* [retval][out] */ INktParamsEnum **ppParamsEnum) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Result( 
            /* [retval][out] */ INktParam **ppParam) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Register( 
            /* [in] */ eNktRegister asmReg,
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Register( 
            /* [in] */ eNktRegister asmReg,
            /* [in] */ my_ssize_t newValue) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FloatingRegister( 
            /* [in] */ eNktRegister asmReg,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_FloatingRegister( 
            /* [in] */ eNktRegister asmReg,
            /* [in] */ double newValue) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE SkipCall( void) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE FilterSpyMgrEvent( void) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR name,
            /* [in] */ VARIANT value) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AddString( 
            /* [in] */ BSTR name,
            /* [in] */ BSTR value) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AddChar( 
            /* [in] */ BSTR name,
            /* [in] */ signed char value) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AddByte( 
            /* [in] */ BSTR name,
            /* [in] */ unsigned char value) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AddShort( 
            /* [in] */ BSTR name,
            /* [in] */ short value) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AddUShort( 
            /* [in] */ BSTR name,
            /* [in] */ unsigned short value) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AddLong( 
            /* [in] */ BSTR name,
            /* [in] */ long value) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AddULong( 
            /* [in] */ BSTR name,
            /* [in] */ unsigned long value) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AddLongLong( 
            /* [in] */ BSTR name,
            /* [in] */ __int64 value) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AddULongLong( 
            /* [in] */ BSTR name,
            /* [in] */ unsigned __int64 value) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AddSSizeT( 
            /* [in] */ BSTR name,
            /* [in] */ my_ssize_t value) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AddSizeT( 
            /* [in] */ BSTR name,
            /* [in] */ my_size_t value) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AddFloat( 
            /* [in] */ BSTR name,
            /* [in] */ float value) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE AddDouble( 
            /* [in] */ BSTR name,
            /* [in] */ double value) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ReadIntercallData( 
            /* [in] */ my_ssize_t destAddr,
            /* [in] */ my_ssize_t offset,
            /* [in] */ my_ssize_t bytes) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE WriteIntercallData( 
            /* [in] */ my_ssize_t srcAddr,
            /* [in] */ my_ssize_t offset,
            /* [in] */ my_ssize_t bytes) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktHookCallInfoPluginVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktHookCallInfoPlugin * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktHookCallInfoPlugin * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktHookCallInfoPlugin * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsPreCall )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Cookie )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LastError )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LastError )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ LONG newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CurrentTimeMs )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ElapsedTimeMs )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ChildsElapsedTimeMs )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_KernelModeTimeMs )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserModeTimeMs )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CpuClockCycles )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ unsigned __int64 *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HookInfo )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ INktHookInfo **pppHookInfo);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ThreadId )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ChainDepth )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *CurrentProcess )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ INktProcess **ppProc);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *StackTrace )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ INktStackTrace **ppStackTrace);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Params )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ INktParamsEnum **ppParamsEnum);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Result )( 
            INktHookCallInfoPlugin * This,
            /* [retval][out] */ INktParam **ppParam);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Register )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ eNktRegister asmReg,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Register )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ eNktRegister asmReg,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FloatingRegister )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ eNktRegister asmReg,
            /* [retval][out] */ double *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_FloatingRegister )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ eNktRegister asmReg,
            /* [in] */ double newValue);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *SkipCall )( 
            INktHookCallInfoPlugin * This);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FilterSpyMgrEvent )( 
            INktHookCallInfoPlugin * This);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ BSTR name,
            /* [in] */ VARIANT value);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddString )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ BSTR name,
            /* [in] */ BSTR value);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddChar )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ BSTR name,
            /* [in] */ signed char value);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddByte )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ BSTR name,
            /* [in] */ unsigned char value);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddShort )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ BSTR name,
            /* [in] */ short value);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddUShort )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ BSTR name,
            /* [in] */ unsigned short value);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddLong )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ BSTR name,
            /* [in] */ long value);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddULong )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ BSTR name,
            /* [in] */ unsigned long value);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddLongLong )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ BSTR name,
            /* [in] */ __int64 value);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddULongLong )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ BSTR name,
            /* [in] */ unsigned __int64 value);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddSSizeT )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ BSTR name,
            /* [in] */ my_ssize_t value);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddSizeT )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ BSTR name,
            /* [in] */ my_size_t value);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddFloat )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ BSTR name,
            /* [in] */ float value);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddDouble )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ BSTR name,
            /* [in] */ double value);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReadIntercallData )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ my_ssize_t destAddr,
            /* [in] */ my_ssize_t offset,
            /* [in] */ my_ssize_t bytes);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *WriteIntercallData )( 
            INktHookCallInfoPlugin * This,
            /* [in] */ my_ssize_t srcAddr,
            /* [in] */ my_ssize_t offset,
            /* [in] */ my_ssize_t bytes);
        
        END_INTERFACE
    } INktHookCallInfoPluginVtbl;

    interface INktHookCallInfoPlugin
    {
        CONST_VTBL struct INktHookCallInfoPluginVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktHookCallInfoPlugin_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktHookCallInfoPlugin_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktHookCallInfoPlugin_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktHookCallInfoPlugin_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktHookCallInfoPlugin_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktHookCallInfoPlugin_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktHookCallInfoPlugin_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktHookCallInfoPlugin_get_IsPreCall(This,pVal)	\
    ( (This)->lpVtbl -> get_IsPreCall(This,pVal) ) 

#define INktHookCallInfoPlugin_get_Cookie(This,pVal)	\
    ( (This)->lpVtbl -> get_Cookie(This,pVal) ) 

#define INktHookCallInfoPlugin_get_LastError(This,pVal)	\
    ( (This)->lpVtbl -> get_LastError(This,pVal) ) 

#define INktHookCallInfoPlugin_put_LastError(This,newValue)	\
    ( (This)->lpVtbl -> put_LastError(This,newValue) ) 

#define INktHookCallInfoPlugin_get_CurrentTimeMs(This,pVal)	\
    ( (This)->lpVtbl -> get_CurrentTimeMs(This,pVal) ) 

#define INktHookCallInfoPlugin_get_ElapsedTimeMs(This,pVal)	\
    ( (This)->lpVtbl -> get_ElapsedTimeMs(This,pVal) ) 

#define INktHookCallInfoPlugin_get_ChildsElapsedTimeMs(This,pVal)	\
    ( (This)->lpVtbl -> get_ChildsElapsedTimeMs(This,pVal) ) 

#define INktHookCallInfoPlugin_get_KernelModeTimeMs(This,pVal)	\
    ( (This)->lpVtbl -> get_KernelModeTimeMs(This,pVal) ) 

#define INktHookCallInfoPlugin_get_UserModeTimeMs(This,pVal)	\
    ( (This)->lpVtbl -> get_UserModeTimeMs(This,pVal) ) 

#define INktHookCallInfoPlugin_get_CpuClockCycles(This,pVal)	\
    ( (This)->lpVtbl -> get_CpuClockCycles(This,pVal) ) 

#define INktHookCallInfoPlugin_get_HookInfo(This,pppHookInfo)	\
    ( (This)->lpVtbl -> get_HookInfo(This,pppHookInfo) ) 

#define INktHookCallInfoPlugin_get_ThreadId(This,pVal)	\
    ( (This)->lpVtbl -> get_ThreadId(This,pVal) ) 

#define INktHookCallInfoPlugin_get_ChainDepth(This,pVal)	\
    ( (This)->lpVtbl -> get_ChainDepth(This,pVal) ) 

#define INktHookCallInfoPlugin_CurrentProcess(This,ppProc)	\
    ( (This)->lpVtbl -> CurrentProcess(This,ppProc) ) 

#define INktHookCallInfoPlugin_StackTrace(This,ppStackTrace)	\
    ( (This)->lpVtbl -> StackTrace(This,ppStackTrace) ) 

#define INktHookCallInfoPlugin_Params(This,ppParamsEnum)	\
    ( (This)->lpVtbl -> Params(This,ppParamsEnum) ) 

#define INktHookCallInfoPlugin_Result(This,ppParam)	\
    ( (This)->lpVtbl -> Result(This,ppParam) ) 

#define INktHookCallInfoPlugin_get_Register(This,asmReg,pVal)	\
    ( (This)->lpVtbl -> get_Register(This,asmReg,pVal) ) 

#define INktHookCallInfoPlugin_put_Register(This,asmReg,newValue)	\
    ( (This)->lpVtbl -> put_Register(This,asmReg,newValue) ) 

#define INktHookCallInfoPlugin_get_FloatingRegister(This,asmReg,pVal)	\
    ( (This)->lpVtbl -> get_FloatingRegister(This,asmReg,pVal) ) 

#define INktHookCallInfoPlugin_put_FloatingRegister(This,asmReg,newValue)	\
    ( (This)->lpVtbl -> put_FloatingRegister(This,asmReg,newValue) ) 

#define INktHookCallInfoPlugin_SkipCall(This)	\
    ( (This)->lpVtbl -> SkipCall(This) ) 

#define INktHookCallInfoPlugin_FilterSpyMgrEvent(This)	\
    ( (This)->lpVtbl -> FilterSpyMgrEvent(This) ) 

#define INktHookCallInfoPlugin_Add(This,name,value)	\
    ( (This)->lpVtbl -> Add(This,name,value) ) 

#define INktHookCallInfoPlugin_AddString(This,name,value)	\
    ( (This)->lpVtbl -> AddString(This,name,value) ) 

#define INktHookCallInfoPlugin_AddChar(This,name,value)	\
    ( (This)->lpVtbl -> AddChar(This,name,value) ) 

#define INktHookCallInfoPlugin_AddByte(This,name,value)	\
    ( (This)->lpVtbl -> AddByte(This,name,value) ) 

#define INktHookCallInfoPlugin_AddShort(This,name,value)	\
    ( (This)->lpVtbl -> AddShort(This,name,value) ) 

#define INktHookCallInfoPlugin_AddUShort(This,name,value)	\
    ( (This)->lpVtbl -> AddUShort(This,name,value) ) 

#define INktHookCallInfoPlugin_AddLong(This,name,value)	\
    ( (This)->lpVtbl -> AddLong(This,name,value) ) 

#define INktHookCallInfoPlugin_AddULong(This,name,value)	\
    ( (This)->lpVtbl -> AddULong(This,name,value) ) 

#define INktHookCallInfoPlugin_AddLongLong(This,name,value)	\
    ( (This)->lpVtbl -> AddLongLong(This,name,value) ) 

#define INktHookCallInfoPlugin_AddULongLong(This,name,value)	\
    ( (This)->lpVtbl -> AddULongLong(This,name,value) ) 

#define INktHookCallInfoPlugin_AddSSizeT(This,name,value)	\
    ( (This)->lpVtbl -> AddSSizeT(This,name,value) ) 

#define INktHookCallInfoPlugin_AddSizeT(This,name,value)	\
    ( (This)->lpVtbl -> AddSizeT(This,name,value) ) 

#define INktHookCallInfoPlugin_AddFloat(This,name,value)	\
    ( (This)->lpVtbl -> AddFloat(This,name,value) ) 

#define INktHookCallInfoPlugin_AddDouble(This,name,value)	\
    ( (This)->lpVtbl -> AddDouble(This,name,value) ) 

#define INktHookCallInfoPlugin_ReadIntercallData(This,destAddr,offset,bytes)	\
    ( (This)->lpVtbl -> ReadIntercallData(This,destAddr,offset,bytes) ) 

#define INktHookCallInfoPlugin_WriteIntercallData(This,srcAddr,offset,bytes)	\
    ( (This)->lpVtbl -> WriteIntercallData(This,srcAddr,offset,bytes) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_get_IsPreCall_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktHookCallInfoPlugin_get_IsPreCall_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_get_Cookie_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktHookCallInfoPlugin_get_Cookie_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_get_LastError_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktHookCallInfoPlugin_get_LastError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_put_LastError_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ LONG newValue);


void __RPC_STUB INktHookCallInfoPlugin_put_LastError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_get_CurrentTimeMs_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktHookCallInfoPlugin_get_CurrentTimeMs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_get_ElapsedTimeMs_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktHookCallInfoPlugin_get_ElapsedTimeMs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_get_ChildsElapsedTimeMs_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktHookCallInfoPlugin_get_ChildsElapsedTimeMs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_get_KernelModeTimeMs_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktHookCallInfoPlugin_get_KernelModeTimeMs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_get_UserModeTimeMs_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktHookCallInfoPlugin_get_UserModeTimeMs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_get_CpuClockCycles_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ unsigned __int64 *pVal);


void __RPC_STUB INktHookCallInfoPlugin_get_CpuClockCycles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_get_HookInfo_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ INktHookInfo **pppHookInfo);


void __RPC_STUB INktHookCallInfoPlugin_get_HookInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_get_ThreadId_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktHookCallInfoPlugin_get_ThreadId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_get_ChainDepth_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktHookCallInfoPlugin_get_ChainDepth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_CurrentProcess_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktHookCallInfoPlugin_CurrentProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_StackTrace_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ INktStackTrace **ppStackTrace);


void __RPC_STUB INktHookCallInfoPlugin_StackTrace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_Params_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ INktParamsEnum **ppParamsEnum);


void __RPC_STUB INktHookCallInfoPlugin_Params_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_Result_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [retval][out] */ INktParam **ppParam);


void __RPC_STUB INktHookCallInfoPlugin_Result_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_get_Register_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ eNktRegister asmReg,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktHookCallInfoPlugin_get_Register_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_put_Register_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ eNktRegister asmReg,
    /* [in] */ my_ssize_t newValue);


void __RPC_STUB INktHookCallInfoPlugin_put_Register_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_get_FloatingRegister_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ eNktRegister asmReg,
    /* [retval][out] */ double *pVal);


void __RPC_STUB INktHookCallInfoPlugin_get_FloatingRegister_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_put_FloatingRegister_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ eNktRegister asmReg,
    /* [in] */ double newValue);


void __RPC_STUB INktHookCallInfoPlugin_put_FloatingRegister_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_SkipCall_Proxy( 
    INktHookCallInfoPlugin * This);


void __RPC_STUB INktHookCallInfoPlugin_SkipCall_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_FilterSpyMgrEvent_Proxy( 
    INktHookCallInfoPlugin * This);


void __RPC_STUB INktHookCallInfoPlugin_FilterSpyMgrEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_Add_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ BSTR name,
    /* [in] */ VARIANT value);


void __RPC_STUB INktHookCallInfoPlugin_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_AddString_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ BSTR name,
    /* [in] */ BSTR value);


void __RPC_STUB INktHookCallInfoPlugin_AddString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_AddChar_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ BSTR name,
    /* [in] */ signed char value);


void __RPC_STUB INktHookCallInfoPlugin_AddChar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_AddByte_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ BSTR name,
    /* [in] */ unsigned char value);


void __RPC_STUB INktHookCallInfoPlugin_AddByte_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_AddShort_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ BSTR name,
    /* [in] */ short value);


void __RPC_STUB INktHookCallInfoPlugin_AddShort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_AddUShort_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ BSTR name,
    /* [in] */ unsigned short value);


void __RPC_STUB INktHookCallInfoPlugin_AddUShort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_AddLong_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ BSTR name,
    /* [in] */ long value);


void __RPC_STUB INktHookCallInfoPlugin_AddLong_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_AddULong_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ BSTR name,
    /* [in] */ unsigned long value);


void __RPC_STUB INktHookCallInfoPlugin_AddULong_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_AddLongLong_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ BSTR name,
    /* [in] */ __int64 value);


void __RPC_STUB INktHookCallInfoPlugin_AddLongLong_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_AddULongLong_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ BSTR name,
    /* [in] */ unsigned __int64 value);


void __RPC_STUB INktHookCallInfoPlugin_AddULongLong_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_AddSSizeT_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ BSTR name,
    /* [in] */ my_ssize_t value);


void __RPC_STUB INktHookCallInfoPlugin_AddSSizeT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_AddSizeT_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ BSTR name,
    /* [in] */ my_size_t value);


void __RPC_STUB INktHookCallInfoPlugin_AddSizeT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_AddFloat_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ BSTR name,
    /* [in] */ float value);


void __RPC_STUB INktHookCallInfoPlugin_AddFloat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_AddDouble_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ BSTR name,
    /* [in] */ double value);


void __RPC_STUB INktHookCallInfoPlugin_AddDouble_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_ReadIntercallData_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ my_ssize_t destAddr,
    /* [in] */ my_ssize_t offset,
    /* [in] */ my_ssize_t bytes);


void __RPC_STUB INktHookCallInfoPlugin_ReadIntercallData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookCallInfoPlugin_WriteIntercallData_Proxy( 
    INktHookCallInfoPlugin * This,
    /* [in] */ my_ssize_t srcAddr,
    /* [in] */ my_ssize_t offset,
    /* [in] */ my_ssize_t bytes);


void __RPC_STUB INktHookCallInfoPlugin_WriteIntercallData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktHookCallInfoPlugin_INTERFACE_DEFINED__ */


#ifndef __INktHookInfo_INTERFACE_DEFINED__
#define __INktHookInfo_INTERFACE_DEFINED__

/* interface INktHookInfo */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktHookInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("99DE4737-C1B4-49f6-8072-A7645E78BA8B")
    INktHookInfo : public IDispatch
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Id( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FunctionName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Address( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE DbObject( 
            /* [retval][out] */ INktDbObject **ppDbObject) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE SendCustomMessage( 
            /* [in] */ my_ssize_t msgCode,
            /* [in] */ my_ssize_t msgParam,
            /* [defaultvalue][in] */ VARIANT_BOOL sync,
            /* [retval][out] */ my_ssize_t *pRetVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ThreadIgnore( 
            /* [in] */ LONG theadId,
            /* [in] */ VARIANT_BOOL ignore) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE InvalidateCache( 
            /* [in] */ my_ssize_t hDll) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE CurrentProcess( 
            /* [retval][out] */ INktProcess **ppProc) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktHookInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktHookInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktHookInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktHookInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktHookInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktHookInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktHookInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktHookInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Id )( 
            INktHookInfo * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FunctionName )( 
            INktHookInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Address )( 
            INktHookInfo * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *DbObject )( 
            INktHookInfo * This,
            /* [retval][out] */ INktDbObject **ppDbObject);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *SendCustomMessage )( 
            INktHookInfo * This,
            /* [in] */ my_ssize_t msgCode,
            /* [in] */ my_ssize_t msgParam,
            /* [defaultvalue][in] */ VARIANT_BOOL sync,
            /* [retval][out] */ my_ssize_t *pRetVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ThreadIgnore )( 
            INktHookInfo * This,
            /* [in] */ LONG theadId,
            /* [in] */ VARIANT_BOOL ignore);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *InvalidateCache )( 
            INktHookInfo * This,
            /* [in] */ my_ssize_t hDll);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *CurrentProcess )( 
            INktHookInfo * This,
            /* [retval][out] */ INktProcess **ppProc);
        
        END_INTERFACE
    } INktHookInfoVtbl;

    interface INktHookInfo
    {
        CONST_VTBL struct INktHookInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktHookInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktHookInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktHookInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktHookInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktHookInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktHookInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktHookInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktHookInfo_get_Id(This,pVal)	\
    ( (This)->lpVtbl -> get_Id(This,pVal) ) 

#define INktHookInfo_get_FunctionName(This,pVal)	\
    ( (This)->lpVtbl -> get_FunctionName(This,pVal) ) 

#define INktHookInfo_get_Address(This,pVal)	\
    ( (This)->lpVtbl -> get_Address(This,pVal) ) 

#define INktHookInfo_DbObject(This,ppDbObject)	\
    ( (This)->lpVtbl -> DbObject(This,ppDbObject) ) 

#define INktHookInfo_SendCustomMessage(This,msgCode,msgParam,sync,pRetVal)	\
    ( (This)->lpVtbl -> SendCustomMessage(This,msgCode,msgParam,sync,pRetVal) ) 

#define INktHookInfo_ThreadIgnore(This,theadId,ignore)	\
    ( (This)->lpVtbl -> ThreadIgnore(This,theadId,ignore) ) 

#define INktHookInfo_InvalidateCache(This,hDll)	\
    ( (This)->lpVtbl -> InvalidateCache(This,hDll) ) 

#define INktHookInfo_CurrentProcess(This,ppProc)	\
    ( (This)->lpVtbl -> CurrentProcess(This,ppProc) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookInfo_get_Id_Proxy( 
    INktHookInfo * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktHookInfo_get_Id_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookInfo_get_FunctionName_Proxy( 
    INktHookInfo * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktHookInfo_get_FunctionName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktHookInfo_get_Address_Proxy( 
    INktHookInfo * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktHookInfo_get_Address_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookInfo_DbObject_Proxy( 
    INktHookInfo * This,
    /* [retval][out] */ INktDbObject **ppDbObject);


void __RPC_STUB INktHookInfo_DbObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookInfo_SendCustomMessage_Proxy( 
    INktHookInfo * This,
    /* [in] */ my_ssize_t msgCode,
    /* [in] */ my_ssize_t msgParam,
    /* [defaultvalue][in] */ VARIANT_BOOL sync,
    /* [retval][out] */ my_ssize_t *pRetVal);


void __RPC_STUB INktHookInfo_SendCustomMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookInfo_ThreadIgnore_Proxy( 
    INktHookInfo * This,
    /* [in] */ LONG theadId,
    /* [in] */ VARIANT_BOOL ignore);


void __RPC_STUB INktHookInfo_ThreadIgnore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookInfo_InvalidateCache_Proxy( 
    INktHookInfo * This,
    /* [in] */ my_ssize_t hDll);


void __RPC_STUB INktHookInfo_InvalidateCache_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktHookInfo_CurrentProcess_Proxy( 
    INktHookInfo * This,
    /* [retval][out] */ INktProcess **ppProc);


void __RPC_STUB INktHookInfo_CurrentProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktHookInfo_INTERFACE_DEFINED__ */


#ifndef __INktDbObject_INTERFACE_DEFINED__
#define __INktDbObject_INTERFACE_DEFINED__

/* interface INktDbObject */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktDbObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8B4C6D62-05C5-48ee-A527-E1E1C35F522C")
    INktDbObject : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Id( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Class( 
            /* [retval][out] */ eNktDboClass *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FundamentalType( 
            /* [retval][out] */ eNktDboFundamentalType *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Declaration( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Align( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsArray( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ArrayMaxHint( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsAnsiString( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsWideString( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsGuid( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Evaluate( 
            /* [retval][out] */ INktDbObject **ppDbObject) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE IndexedEvaluate( 
            /* [in] */ LONG index,
            /* [retval][out] */ INktDbObject **ppDbObject) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE FullEvaluate( 
            /* [retval][out] */ INktDbObject **ppDbObject) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Flags( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ItemsCount( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE NameToIndex( 
            /* [in] */ BSTR name,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ LONG index,
            /* [retval][out] */ INktDbObject **ppDbObject) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Items( 
            /* [retval][out] */ INktDbObjectsEnum **ppDbObjEnum) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ItemName( 
            /* [in] */ LONG index,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ItemFlags( 
            /* [in] */ LONG index,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ItemMemberType( 
            /* [in] */ LONG index,
            /* [retval][out] */ eNktDboItemMemberType *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ItemStructOffset( 
            /* [in] */ LONG index,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ItemStructBits( 
            /* [in] */ LONG index,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ItemEnumValue( 
            /* [in] */ LONG index,
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ItemDeclaration( 
            /* [in] */ LONG index,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FuncCallingConvention( 
            /* [retval][out] */ eNktDboCallingConvention *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FunctionReturn( 
            /* [retval][out] */ INktDbObject **ppDbObject) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FunctionReturnFlags( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FunctionReturnDeclaration( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE DbModules( 
            /* [retval][out] */ INktDbModulesEnum **ppDbModsEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktDbObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktDbObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktDbObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktDbObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktDbObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktDbObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktDbObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktDbObject * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktDbObject * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktDbObject * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktDbObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktDbObject * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktDbObject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktDbObject * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Id )( 
            INktDbObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Class )( 
            INktDbObject * This,
            /* [retval][out] */ eNktDboClass *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FundamentalType )( 
            INktDbObject * This,
            /* [retval][out] */ eNktDboFundamentalType *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Declaration )( 
            INktDbObject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            INktDbObject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            INktDbObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Align )( 
            INktDbObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsArray )( 
            INktDbObject * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ArrayMaxHint )( 
            INktDbObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsAnsiString )( 
            INktDbObject * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsWideString )( 
            INktDbObject * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsGuid )( 
            INktDbObject * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Evaluate )( 
            INktDbObject * This,
            /* [retval][out] */ INktDbObject **ppDbObject);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *IndexedEvaluate )( 
            INktDbObject * This,
            /* [in] */ LONG index,
            /* [retval][out] */ INktDbObject **ppDbObject);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FullEvaluate )( 
            INktDbObject * This,
            /* [retval][out] */ INktDbObject **ppDbObject);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Flags )( 
            INktDbObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ItemsCount )( 
            INktDbObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *NameToIndex )( 
            INktDbObject * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            INktDbObject * This,
            /* [in] */ LONG index,
            /* [retval][out] */ INktDbObject **ppDbObject);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Items )( 
            INktDbObject * This,
            /* [retval][out] */ INktDbObjectsEnum **ppDbObjEnum);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ItemName )( 
            INktDbObject * This,
            /* [in] */ LONG index,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ItemFlags )( 
            INktDbObject * This,
            /* [in] */ LONG index,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ItemMemberType )( 
            INktDbObject * This,
            /* [in] */ LONG index,
            /* [retval][out] */ eNktDboItemMemberType *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ItemStructOffset )( 
            INktDbObject * This,
            /* [in] */ LONG index,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ItemStructBits )( 
            INktDbObject * This,
            /* [in] */ LONG index,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ItemEnumValue )( 
            INktDbObject * This,
            /* [in] */ LONG index,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ItemDeclaration )( 
            INktDbObject * This,
            /* [in] */ LONG index,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FuncCallingConvention )( 
            INktDbObject * This,
            /* [retval][out] */ eNktDboCallingConvention *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FunctionReturn )( 
            INktDbObject * This,
            /* [retval][out] */ INktDbObject **ppDbObject);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FunctionReturnFlags )( 
            INktDbObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FunctionReturnDeclaration )( 
            INktDbObject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *DbModules )( 
            INktDbObject * This,
            /* [retval][out] */ INktDbModulesEnum **ppDbModsEnum);
        
        END_INTERFACE
    } INktDbObjectVtbl;

    interface INktDbObject
    {
        CONST_VTBL struct INktDbObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktDbObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktDbObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktDbObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktDbObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktDbObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktDbObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktDbObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktDbObject_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktDbObject_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktDbObject_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktDbObject_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktDbObject_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktDbObject_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktDbObject_get_Id(This,pVal)	\
    ( (This)->lpVtbl -> get_Id(This,pVal) ) 

#define INktDbObject_get_Class(This,pVal)	\
    ( (This)->lpVtbl -> get_Class(This,pVal) ) 

#define INktDbObject_get_FundamentalType(This,pVal)	\
    ( (This)->lpVtbl -> get_FundamentalType(This,pVal) ) 

#define INktDbObject_get_Declaration(This,pVal)	\
    ( (This)->lpVtbl -> get_Declaration(This,pVal) ) 

#define INktDbObject_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define INktDbObject_get_Size(This,pVal)	\
    ( (This)->lpVtbl -> get_Size(This,pVal) ) 

#define INktDbObject_get_Align(This,pVal)	\
    ( (This)->lpVtbl -> get_Align(This,pVal) ) 

#define INktDbObject_get_IsArray(This,pVal)	\
    ( (This)->lpVtbl -> get_IsArray(This,pVal) ) 

#define INktDbObject_get_ArrayMaxHint(This,pVal)	\
    ( (This)->lpVtbl -> get_ArrayMaxHint(This,pVal) ) 

#define INktDbObject_get_IsAnsiString(This,pVal)	\
    ( (This)->lpVtbl -> get_IsAnsiString(This,pVal) ) 

#define INktDbObject_get_IsWideString(This,pVal)	\
    ( (This)->lpVtbl -> get_IsWideString(This,pVal) ) 

#define INktDbObject_get_IsGuid(This,pVal)	\
    ( (This)->lpVtbl -> get_IsGuid(This,pVal) ) 

#define INktDbObject_Evaluate(This,ppDbObject)	\
    ( (This)->lpVtbl -> Evaluate(This,ppDbObject) ) 

#define INktDbObject_IndexedEvaluate(This,index,ppDbObject)	\
    ( (This)->lpVtbl -> IndexedEvaluate(This,index,ppDbObject) ) 

#define INktDbObject_FullEvaluate(This,ppDbObject)	\
    ( (This)->lpVtbl -> FullEvaluate(This,ppDbObject) ) 

#define INktDbObject_get_Flags(This,pVal)	\
    ( (This)->lpVtbl -> get_Flags(This,pVal) ) 

#define INktDbObject_get_ItemsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_ItemsCount(This,pVal) ) 

#define INktDbObject_NameToIndex(This,name,pVal)	\
    ( (This)->lpVtbl -> NameToIndex(This,name,pVal) ) 

#define INktDbObject_Item(This,index,ppDbObject)	\
    ( (This)->lpVtbl -> Item(This,index,ppDbObject) ) 

#define INktDbObject_Items(This,ppDbObjEnum)	\
    ( (This)->lpVtbl -> Items(This,ppDbObjEnum) ) 

#define INktDbObject_ItemName(This,index,pVal)	\
    ( (This)->lpVtbl -> ItemName(This,index,pVal) ) 

#define INktDbObject_ItemFlags(This,index,pVal)	\
    ( (This)->lpVtbl -> ItemFlags(This,index,pVal) ) 

#define INktDbObject_ItemMemberType(This,index,pVal)	\
    ( (This)->lpVtbl -> ItemMemberType(This,index,pVal) ) 

#define INktDbObject_ItemStructOffset(This,index,pVal)	\
    ( (This)->lpVtbl -> ItemStructOffset(This,index,pVal) ) 

#define INktDbObject_ItemStructBits(This,index,pVal)	\
    ( (This)->lpVtbl -> ItemStructBits(This,index,pVal) ) 

#define INktDbObject_ItemEnumValue(This,index,pVal)	\
    ( (This)->lpVtbl -> ItemEnumValue(This,index,pVal) ) 

#define INktDbObject_ItemDeclaration(This,index,pVal)	\
    ( (This)->lpVtbl -> ItemDeclaration(This,index,pVal) ) 

#define INktDbObject_get_FuncCallingConvention(This,pVal)	\
    ( (This)->lpVtbl -> get_FuncCallingConvention(This,pVal) ) 

#define INktDbObject_get_FunctionReturn(This,ppDbObject)	\
    ( (This)->lpVtbl -> get_FunctionReturn(This,ppDbObject) ) 

#define INktDbObject_get_FunctionReturnFlags(This,pVal)	\
    ( (This)->lpVtbl -> get_FunctionReturnFlags(This,pVal) ) 

#define INktDbObject_get_FunctionReturnDeclaration(This,pVal)	\
    ( (This)->lpVtbl -> get_FunctionReturnDeclaration(This,pVal) ) 

#define INktDbObject_DbModules(This,ppDbModsEnum)	\
    ( (This)->lpVtbl -> DbModules(This,ppDbModsEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_Id_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktDbObject_get_Id_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_Class_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ eNktDboClass *pVal);


void __RPC_STUB INktDbObject_get_Class_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_FundamentalType_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ eNktDboFundamentalType *pVal);


void __RPC_STUB INktDbObject_get_FundamentalType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_Declaration_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktDbObject_get_Declaration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_Name_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktDbObject_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_Size_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktDbObject_get_Size_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_Align_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktDbObject_get_Align_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_IsArray_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktDbObject_get_IsArray_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_ArrayMaxHint_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktDbObject_get_ArrayMaxHint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_IsAnsiString_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktDbObject_get_IsAnsiString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_IsWideString_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktDbObject_get_IsWideString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_IsGuid_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB INktDbObject_get_IsGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObject_Evaluate_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ INktDbObject **ppDbObject);


void __RPC_STUB INktDbObject_Evaluate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObject_IndexedEvaluate_Proxy( 
    INktDbObject * This,
    /* [in] */ LONG index,
    /* [retval][out] */ INktDbObject **ppDbObject);


void __RPC_STUB INktDbObject_IndexedEvaluate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObject_FullEvaluate_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ INktDbObject **ppDbObject);


void __RPC_STUB INktDbObject_FullEvaluate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_Flags_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktDbObject_get_Flags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_ItemsCount_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktDbObject_get_ItemsCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObject_NameToIndex_Proxy( 
    INktDbObject * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktDbObject_NameToIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObject_Item_Proxy( 
    INktDbObject * This,
    /* [in] */ LONG index,
    /* [retval][out] */ INktDbObject **ppDbObject);


void __RPC_STUB INktDbObject_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObject_Items_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ INktDbObjectsEnum **ppDbObjEnum);


void __RPC_STUB INktDbObject_Items_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObject_ItemName_Proxy( 
    INktDbObject * This,
    /* [in] */ LONG index,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktDbObject_ItemName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObject_ItemFlags_Proxy( 
    INktDbObject * This,
    /* [in] */ LONG index,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktDbObject_ItemFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObject_ItemMemberType_Proxy( 
    INktDbObject * This,
    /* [in] */ LONG index,
    /* [retval][out] */ eNktDboItemMemberType *pVal);


void __RPC_STUB INktDbObject_ItemMemberType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObject_ItemStructOffset_Proxy( 
    INktDbObject * This,
    /* [in] */ LONG index,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktDbObject_ItemStructOffset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObject_ItemStructBits_Proxy( 
    INktDbObject * This,
    /* [in] */ LONG index,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktDbObject_ItemStructBits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObject_ItemEnumValue_Proxy( 
    INktDbObject * This,
    /* [in] */ LONG index,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktDbObject_ItemEnumValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObject_ItemDeclaration_Proxy( 
    INktDbObject * This,
    /* [in] */ LONG index,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktDbObject_ItemDeclaration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_FuncCallingConvention_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ eNktDboCallingConvention *pVal);


void __RPC_STUB INktDbObject_get_FuncCallingConvention_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_FunctionReturn_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ INktDbObject **ppDbObject);


void __RPC_STUB INktDbObject_get_FunctionReturn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_FunctionReturnFlags_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktDbObject_get_FunctionReturnFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObject_get_FunctionReturnDeclaration_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktDbObject_get_FunctionReturnDeclaration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObject_DbModules_Proxy( 
    INktDbObject * This,
    /* [retval][out] */ INktDbModulesEnum **ppDbModsEnum);


void __RPC_STUB INktDbObject_DbModules_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktDbObject_INTERFACE_DEFINED__ */


#ifndef __INktDbObjectsEnum_INTERFACE_DEFINED__
#define __INktDbObjectsEnum_INTERFACE_DEFINED__

/* interface INktDbObjectsEnum */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktDbObjectsEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E36B3F2F-DEA5-4d55-9348-8B67A4157B21")
    INktDbObjectsEnum : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ INktDbObject **ppDbObject) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE First( 
            /* [retval][out] */ INktDbObject **ppDbObject) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Last( 
            /* [retval][out] */ INktDbObject **ppDbObject) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ INktDbObject **ppDbObject) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Previous( 
            /* [retval][out] */ INktDbObject **ppDbObject) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetById( 
            /* [in] */ LONG id,
            /* [retval][out] */ INktDbObject **ppDbObject) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetByName( 
            /* [in] */ BSTR name,
            /* [retval][out] */ INktDbObject **ppDbObject) = 0;
        
        virtual /* [restricted][id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktDbObjectsEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktDbObjectsEnum * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktDbObjectsEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktDbObjectsEnum * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktDbObjectsEnum * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktDbObjectsEnum * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktDbObjectsEnum * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktDbObjectsEnum * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktDbObjectsEnum * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktDbObjectsEnum * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktDbObjectsEnum * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktDbObjectsEnum * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktDbObjectsEnum * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktDbObjectsEnum * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            INktDbObjectsEnum * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            INktDbObjectsEnum * This,
            /* [in] */ LONG index,
            /* [retval][out] */ INktDbObject **ppDbObject);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *First )( 
            INktDbObjectsEnum * This,
            /* [retval][out] */ INktDbObject **ppDbObject);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Last )( 
            INktDbObjectsEnum * This,
            /* [retval][out] */ INktDbObject **ppDbObject);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            INktDbObjectsEnum * This,
            /* [retval][out] */ INktDbObject **ppDbObject);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Previous )( 
            INktDbObjectsEnum * This,
            /* [retval][out] */ INktDbObject **ppDbObject);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetById )( 
            INktDbObjectsEnum * This,
            /* [in] */ LONG id,
            /* [retval][out] */ INktDbObject **ppDbObject);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetByName )( 
            INktDbObjectsEnum * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ INktDbObject **ppDbObject);
        
        /* [restricted][id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            INktDbObjectsEnum * This,
            /* [retval][out] */ IUnknown **ppUnk);
        
        END_INTERFACE
    } INktDbObjectsEnumVtbl;

    interface INktDbObjectsEnum
    {
        CONST_VTBL struct INktDbObjectsEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktDbObjectsEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktDbObjectsEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktDbObjectsEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktDbObjectsEnum_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktDbObjectsEnum_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktDbObjectsEnum_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktDbObjectsEnum_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktDbObjectsEnum_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktDbObjectsEnum_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktDbObjectsEnum_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktDbObjectsEnum_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktDbObjectsEnum_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktDbObjectsEnum_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktDbObjectsEnum_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define INktDbObjectsEnum_GetAt(This,index,ppDbObject)	\
    ( (This)->lpVtbl -> GetAt(This,index,ppDbObject) ) 

#define INktDbObjectsEnum_First(This,ppDbObject)	\
    ( (This)->lpVtbl -> First(This,ppDbObject) ) 

#define INktDbObjectsEnum_Last(This,ppDbObject)	\
    ( (This)->lpVtbl -> Last(This,ppDbObject) ) 

#define INktDbObjectsEnum_Next(This,ppDbObject)	\
    ( (This)->lpVtbl -> Next(This,ppDbObject) ) 

#define INktDbObjectsEnum_Previous(This,ppDbObject)	\
    ( (This)->lpVtbl -> Previous(This,ppDbObject) ) 

#define INktDbObjectsEnum_GetById(This,id,ppDbObject)	\
    ( (This)->lpVtbl -> GetById(This,id,ppDbObject) ) 

#define INktDbObjectsEnum_GetByName(This,name,ppDbObject)	\
    ( (This)->lpVtbl -> GetByName(This,name,ppDbObject) ) 

#define INktDbObjectsEnum_get__NewEnum(This,ppUnk)	\
    ( (This)->lpVtbl -> get__NewEnum(This,ppUnk) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObjectsEnum_get_Count_Proxy( 
    INktDbObjectsEnum * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktDbObjectsEnum_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObjectsEnum_GetAt_Proxy( 
    INktDbObjectsEnum * This,
    /* [in] */ LONG index,
    /* [retval][out] */ INktDbObject **ppDbObject);


void __RPC_STUB INktDbObjectsEnum_GetAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObjectsEnum_First_Proxy( 
    INktDbObjectsEnum * This,
    /* [retval][out] */ INktDbObject **ppDbObject);


void __RPC_STUB INktDbObjectsEnum_First_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObjectsEnum_Last_Proxy( 
    INktDbObjectsEnum * This,
    /* [retval][out] */ INktDbObject **ppDbObject);


void __RPC_STUB INktDbObjectsEnum_Last_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObjectsEnum_Next_Proxy( 
    INktDbObjectsEnum * This,
    /* [retval][out] */ INktDbObject **ppDbObject);


void __RPC_STUB INktDbObjectsEnum_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObjectsEnum_Previous_Proxy( 
    INktDbObjectsEnum * This,
    /* [retval][out] */ INktDbObject **ppDbObject);


void __RPC_STUB INktDbObjectsEnum_Previous_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObjectsEnum_GetById_Proxy( 
    INktDbObjectsEnum * This,
    /* [in] */ LONG id,
    /* [retval][out] */ INktDbObject **ppDbObject);


void __RPC_STUB INktDbObjectsEnum_GetById_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbObjectsEnum_GetByName_Proxy( 
    INktDbObjectsEnum * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ INktDbObject **ppDbObject);


void __RPC_STUB INktDbObjectsEnum_GetByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbObjectsEnum_get__NewEnum_Proxy( 
    INktDbObjectsEnum * This,
    /* [retval][out] */ IUnknown **ppUnk);


void __RPC_STUB INktDbObjectsEnum_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktDbObjectsEnum_INTERFACE_DEFINED__ */


#ifndef __INktDbModule_INTERFACE_DEFINED__
#define __INktDbModule_INTERFACE_DEFINED__

/* interface INktDbModule */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktDbModule;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9AB36611-AB19-4346-9563-AB47A71A863F")
    INktDbModule : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Id( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE DbFunctions( 
            /* [retval][out] */ INktDbObjectsEnum **ppDbObjsEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktDbModuleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktDbModule * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktDbModule * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktDbModule * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktDbModule * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktDbModule * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktDbModule * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktDbModule * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktDbModule * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktDbModule * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktDbModule * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktDbModule * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktDbModule * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktDbModule * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Id )( 
            INktDbModule * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            INktDbModule * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *DbFunctions )( 
            INktDbModule * This,
            /* [retval][out] */ INktDbObjectsEnum **ppDbObjsEnum);
        
        END_INTERFACE
    } INktDbModuleVtbl;

    interface INktDbModule
    {
        CONST_VTBL struct INktDbModuleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktDbModule_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktDbModule_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktDbModule_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktDbModule_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktDbModule_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktDbModule_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktDbModule_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktDbModule_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktDbModule_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktDbModule_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktDbModule_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktDbModule_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktDbModule_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktDbModule_get_Id(This,pVal)	\
    ( (This)->lpVtbl -> get_Id(This,pVal) ) 

#define INktDbModule_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define INktDbModule_DbFunctions(This,ppDbObjsEnum)	\
    ( (This)->lpVtbl -> DbFunctions(This,ppDbObjsEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbModule_get_Id_Proxy( 
    INktDbModule * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktDbModule_get_Id_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbModule_get_Name_Proxy( 
    INktDbModule * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktDbModule_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbModule_DbFunctions_Proxy( 
    INktDbModule * This,
    /* [retval][out] */ INktDbObjectsEnum **ppDbObjsEnum);


void __RPC_STUB INktDbModule_DbFunctions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktDbModule_INTERFACE_DEFINED__ */


#ifndef __INktDbModulesEnum_INTERFACE_DEFINED__
#define __INktDbModulesEnum_INTERFACE_DEFINED__

/* interface INktDbModulesEnum */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktDbModulesEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D8E4F664-6EC1-4c6b-8791-D72EC69EBCA1")
    INktDbModulesEnum : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetAt( 
            /* [in] */ LONG index,
            /* [retval][out] */ INktDbModule **ppDbMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE First( 
            /* [retval][out] */ INktDbModule **ppDbMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Last( 
            /* [retval][out] */ INktDbModule **ppDbMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ INktDbModule **ppDbMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE Previous( 
            /* [retval][out] */ INktDbModule **ppDbMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetById( 
            /* [in] */ LONG id,
            /* [retval][out] */ INktDbModule **ppDbMod) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetByName( 
            /* [in] */ BSTR name,
            /* [retval][out] */ INktDbModule **ppDbMod) = 0;
        
        virtual /* [restricted][id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktDbModulesEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktDbModulesEnum * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktDbModulesEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktDbModulesEnum * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktDbModulesEnum * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktDbModulesEnum * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktDbModulesEnum * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktDbModulesEnum * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktDbModulesEnum * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktDbModulesEnum * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktDbModulesEnum * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktDbModulesEnum * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktDbModulesEnum * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktDbModulesEnum * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            INktDbModulesEnum * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            INktDbModulesEnum * This,
            /* [in] */ LONG index,
            /* [retval][out] */ INktDbModule **ppDbMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *First )( 
            INktDbModulesEnum * This,
            /* [retval][out] */ INktDbModule **ppDbMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Last )( 
            INktDbModulesEnum * This,
            /* [retval][out] */ INktDbModule **ppDbMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            INktDbModulesEnum * This,
            /* [retval][out] */ INktDbModule **ppDbMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Previous )( 
            INktDbModulesEnum * This,
            /* [retval][out] */ INktDbModule **ppDbMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetById )( 
            INktDbModulesEnum * This,
            /* [in] */ LONG id,
            /* [retval][out] */ INktDbModule **ppDbMod);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetByName )( 
            INktDbModulesEnum * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ INktDbModule **ppDbMod);
        
        /* [restricted][id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            INktDbModulesEnum * This,
            /* [retval][out] */ IUnknown **ppUnk);
        
        END_INTERFACE
    } INktDbModulesEnumVtbl;

    interface INktDbModulesEnum
    {
        CONST_VTBL struct INktDbModulesEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktDbModulesEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktDbModulesEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktDbModulesEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktDbModulesEnum_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktDbModulesEnum_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktDbModulesEnum_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktDbModulesEnum_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktDbModulesEnum_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktDbModulesEnum_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktDbModulesEnum_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktDbModulesEnum_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktDbModulesEnum_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktDbModulesEnum_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktDbModulesEnum_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define INktDbModulesEnum_GetAt(This,index,ppDbMod)	\
    ( (This)->lpVtbl -> GetAt(This,index,ppDbMod) ) 

#define INktDbModulesEnum_First(This,ppDbMod)	\
    ( (This)->lpVtbl -> First(This,ppDbMod) ) 

#define INktDbModulesEnum_Last(This,ppDbMod)	\
    ( (This)->lpVtbl -> Last(This,ppDbMod) ) 

#define INktDbModulesEnum_Next(This,ppDbMod)	\
    ( (This)->lpVtbl -> Next(This,ppDbMod) ) 

#define INktDbModulesEnum_Previous(This,ppDbMod)	\
    ( (This)->lpVtbl -> Previous(This,ppDbMod) ) 

#define INktDbModulesEnum_GetById(This,id,ppDbMod)	\
    ( (This)->lpVtbl -> GetById(This,id,ppDbMod) ) 

#define INktDbModulesEnum_GetByName(This,name,ppDbMod)	\
    ( (This)->lpVtbl -> GetByName(This,name,ppDbMod) ) 

#define INktDbModulesEnum_get__NewEnum(This,ppUnk)	\
    ( (This)->lpVtbl -> get__NewEnum(This,ppUnk) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbModulesEnum_get_Count_Proxy( 
    INktDbModulesEnum * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktDbModulesEnum_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbModulesEnum_GetAt_Proxy( 
    INktDbModulesEnum * This,
    /* [in] */ LONG index,
    /* [retval][out] */ INktDbModule **ppDbMod);


void __RPC_STUB INktDbModulesEnum_GetAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbModulesEnum_First_Proxy( 
    INktDbModulesEnum * This,
    /* [retval][out] */ INktDbModule **ppDbMod);


void __RPC_STUB INktDbModulesEnum_First_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbModulesEnum_Last_Proxy( 
    INktDbModulesEnum * This,
    /* [retval][out] */ INktDbModule **ppDbMod);


void __RPC_STUB INktDbModulesEnum_Last_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbModulesEnum_Next_Proxy( 
    INktDbModulesEnum * This,
    /* [retval][out] */ INktDbModule **ppDbMod);


void __RPC_STUB INktDbModulesEnum_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbModulesEnum_Previous_Proxy( 
    INktDbModulesEnum * This,
    /* [retval][out] */ INktDbModule **ppDbMod);


void __RPC_STUB INktDbModulesEnum_Previous_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbModulesEnum_GetById_Proxy( 
    INktDbModulesEnum * This,
    /* [in] */ LONG id,
    /* [retval][out] */ INktDbModule **ppDbMod);


void __RPC_STUB INktDbModulesEnum_GetById_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktDbModulesEnum_GetByName_Proxy( 
    INktDbModulesEnum * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ INktDbModule **ppDbMod);


void __RPC_STUB INktDbModulesEnum_GetByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktDbModulesEnum_get__NewEnum_Proxy( 
    INktDbModulesEnum * This,
    /* [retval][out] */ IUnknown **ppUnk);


void __RPC_STUB INktDbModulesEnum_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktDbModulesEnum_INTERFACE_DEFINED__ */


#ifndef __INktTools_INTERFACE_DEFINED__
#define __INktTools_INTERFACE_DEFINED__

/* interface INktTools */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktTools;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D9C5FF67-2311-494f-BA9A-C7C5AAF14301")
    INktTools : public IDispatch
    {
    public:
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE ExtractIcon( 
            /* [in] */ BSTR executablePath,
            /* [in] */ LONG index,
            /* [retval][out] */ IPicture **ppIcon) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE GetFileNameFromHandle( 
            /* [in] */ my_ssize_t _handle,
            /* [in] */ VARIANT procOrId,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE LocateFunctionSymbolInPdb( 
            /* [in] */ BSTR fileName,
            /* [in] */ BSTR functionName,
            /* [in] */ BSTR symbolServerPath,
            /* [in] */ BSTR localCachePath,
            /* [retval][out] */ INktPdbFunctionSymbol **ppSymbol) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktToolsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktTools * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktTools * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktTools * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktTools * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktTools * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktTools * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktTools * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ExtractIcon )( 
            INktTools * This,
            /* [in] */ BSTR executablePath,
            /* [in] */ LONG index,
            /* [retval][out] */ IPicture **ppIcon);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetFileNameFromHandle )( 
            INktTools * This,
            /* [in] */ my_ssize_t _handle,
            /* [in] */ VARIANT procOrId,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *LocateFunctionSymbolInPdb )( 
            INktTools * This,
            /* [in] */ BSTR fileName,
            /* [in] */ BSTR functionName,
            /* [in] */ BSTR symbolServerPath,
            /* [in] */ BSTR localCachePath,
            /* [retval][out] */ INktPdbFunctionSymbol **ppSymbol);
        
        END_INTERFACE
    } INktToolsVtbl;

    interface INktTools
    {
        CONST_VTBL struct INktToolsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktTools_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktTools_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktTools_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktTools_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktTools_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktTools_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktTools_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktTools_ExtractIcon(This,executablePath,index,ppIcon)	\
    ( (This)->lpVtbl -> ExtractIcon(This,executablePath,index,ppIcon) ) 

#define INktTools_GetFileNameFromHandle(This,_handle,procOrId,pVal)	\
    ( (This)->lpVtbl -> GetFileNameFromHandle(This,_handle,procOrId,pVal) ) 

#define INktTools_LocateFunctionSymbolInPdb(This,fileName,functionName,symbolServerPath,localCachePath,ppSymbol)	\
    ( (This)->lpVtbl -> LocateFunctionSymbolInPdb(This,fileName,functionName,symbolServerPath,localCachePath,ppSymbol) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktTools_ExtractIcon_Proxy( 
    INktTools * This,
    /* [in] */ BSTR executablePath,
    /* [in] */ LONG index,
    /* [retval][out] */ IPicture **ppIcon);


void __RPC_STUB INktTools_ExtractIcon_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktTools_GetFileNameFromHandle_Proxy( 
    INktTools * This,
    /* [in] */ my_ssize_t _handle,
    /* [in] */ VARIANT procOrId,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktTools_GetFileNameFromHandle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring] */ HRESULT STDMETHODCALLTYPE INktTools_LocateFunctionSymbolInPdb_Proxy( 
    INktTools * This,
    /* [in] */ BSTR fileName,
    /* [in] */ BSTR functionName,
    /* [in] */ BSTR symbolServerPath,
    /* [in] */ BSTR localCachePath,
    /* [retval][out] */ INktPdbFunctionSymbol **ppSymbol);


void __RPC_STUB INktTools_LocateFunctionSymbolInPdb_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktTools_INTERFACE_DEFINED__ */


#ifndef __INktStructPEFileHeader_INTERFACE_DEFINED__
#define __INktStructPEFileHeader_INTERFACE_DEFINED__

/* interface INktStructPEFileHeader */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktStructPEFileHeader;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4192DDF7-FB9A-4de5-8AE5-ADCB5C986B8F")
    INktStructPEFileHeader : public IDispatch
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Machine( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_NumberOfSections( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_TimeDateStamp( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PointerToSymbolTable( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_NumberOfSymbols( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SizeOfOptionalHeader( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Characteristics( 
            /* [retval][out] */ short *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktStructPEFileHeaderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktStructPEFileHeader * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktStructPEFileHeader * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktStructPEFileHeader * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktStructPEFileHeader * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktStructPEFileHeader * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktStructPEFileHeader * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktStructPEFileHeader * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Machine )( 
            INktStructPEFileHeader * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NumberOfSections )( 
            INktStructPEFileHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TimeDateStamp )( 
            INktStructPEFileHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PointerToSymbolTable )( 
            INktStructPEFileHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NumberOfSymbols )( 
            INktStructPEFileHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SizeOfOptionalHeader )( 
            INktStructPEFileHeader * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Characteristics )( 
            INktStructPEFileHeader * This,
            /* [retval][out] */ short *pVal);
        
        END_INTERFACE
    } INktStructPEFileHeaderVtbl;

    interface INktStructPEFileHeader
    {
        CONST_VTBL struct INktStructPEFileHeaderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktStructPEFileHeader_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktStructPEFileHeader_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktStructPEFileHeader_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktStructPEFileHeader_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktStructPEFileHeader_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktStructPEFileHeader_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktStructPEFileHeader_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktStructPEFileHeader_get_Machine(This,pVal)	\
    ( (This)->lpVtbl -> get_Machine(This,pVal) ) 

#define INktStructPEFileHeader_get_NumberOfSections(This,pVal)	\
    ( (This)->lpVtbl -> get_NumberOfSections(This,pVal) ) 

#define INktStructPEFileHeader_get_TimeDateStamp(This,pVal)	\
    ( (This)->lpVtbl -> get_TimeDateStamp(This,pVal) ) 

#define INktStructPEFileHeader_get_PointerToSymbolTable(This,pVal)	\
    ( (This)->lpVtbl -> get_PointerToSymbolTable(This,pVal) ) 

#define INktStructPEFileHeader_get_NumberOfSymbols(This,pVal)	\
    ( (This)->lpVtbl -> get_NumberOfSymbols(This,pVal) ) 

#define INktStructPEFileHeader_get_SizeOfOptionalHeader(This,pVal)	\
    ( (This)->lpVtbl -> get_SizeOfOptionalHeader(This,pVal) ) 

#define INktStructPEFileHeader_get_Characteristics(This,pVal)	\
    ( (This)->lpVtbl -> get_Characteristics(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEFileHeader_get_Machine_Proxy( 
    INktStructPEFileHeader * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB INktStructPEFileHeader_get_Machine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEFileHeader_get_NumberOfSections_Proxy( 
    INktStructPEFileHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEFileHeader_get_NumberOfSections_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEFileHeader_get_TimeDateStamp_Proxy( 
    INktStructPEFileHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEFileHeader_get_TimeDateStamp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEFileHeader_get_PointerToSymbolTable_Proxy( 
    INktStructPEFileHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEFileHeader_get_PointerToSymbolTable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEFileHeader_get_NumberOfSymbols_Proxy( 
    INktStructPEFileHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEFileHeader_get_NumberOfSymbols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEFileHeader_get_SizeOfOptionalHeader_Proxy( 
    INktStructPEFileHeader * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktStructPEFileHeader_get_SizeOfOptionalHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEFileHeader_get_Characteristics_Proxy( 
    INktStructPEFileHeader * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB INktStructPEFileHeader_get_Characteristics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktStructPEFileHeader_INTERFACE_DEFINED__ */


#ifndef __INktStructPEOptionalHeader_INTERFACE_DEFINED__
#define __INktStructPEOptionalHeader_INTERFACE_DEFINED__

/* interface INktStructPEOptionalHeader */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktStructPEOptionalHeader;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FE1FD754-7630-44db-BFAA-06505C445B59")
    INktStructPEOptionalHeader : public IDispatch
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Magic( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MajorLinkerVersion( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MinorLinkerVersion( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SizeOfCode( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SizeOfInitializedData( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SizeOfUninitializedData( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AddressOfEntryPoint( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BaseOfCode( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ImageBase( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SectionAlignment( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FileAlignment( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MajorOperatingSystemVersion( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MinorOperatingSystemVersion( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MajorImageVersion( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MinorImageVersion( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MajorSubsystemVersion( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MinorSubsystemVersion( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Win32VersionValue( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SizeOfImage( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SizeOfHeaders( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CheckSum( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Subsystem( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DllCharacteristics( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SizeOfStackReserve( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SizeOfStackCommit( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SizeOfHeapReserve( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SizeOfHeapCommit( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LoaderFlags( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_NumberOfRvaAndSizes( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DataDirectoryStart( 
            /* [in] */ LONG index,
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DataDirectoryEnd( 
            /* [in] */ LONG index,
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktStructPEOptionalHeaderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktStructPEOptionalHeader * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktStructPEOptionalHeader * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktStructPEOptionalHeader * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktStructPEOptionalHeader * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktStructPEOptionalHeader * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktStructPEOptionalHeader * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktStructPEOptionalHeader * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Magic )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MajorLinkerVersion )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MinorLinkerVersion )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SizeOfCode )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SizeOfInitializedData )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SizeOfUninitializedData )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AddressOfEntryPoint )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BaseOfCode )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ImageBase )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SectionAlignment )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FileAlignment )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MajorOperatingSystemVersion )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MinorOperatingSystemVersion )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MajorImageVersion )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MinorImageVersion )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MajorSubsystemVersion )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MinorSubsystemVersion )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Win32VersionValue )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SizeOfImage )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SizeOfHeaders )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CheckSum )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Subsystem )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DllCharacteristics )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SizeOfStackReserve )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SizeOfStackCommit )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SizeOfHeapReserve )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SizeOfHeapCommit )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LoaderFlags )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NumberOfRvaAndSizes )( 
            INktStructPEOptionalHeader * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DataDirectoryStart )( 
            INktStructPEOptionalHeader * This,
            /* [in] */ LONG index,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DataDirectoryEnd )( 
            INktStructPEOptionalHeader * This,
            /* [in] */ LONG index,
            /* [retval][out] */ my_ssize_t *pVal);
        
        END_INTERFACE
    } INktStructPEOptionalHeaderVtbl;

    interface INktStructPEOptionalHeader
    {
        CONST_VTBL struct INktStructPEOptionalHeaderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktStructPEOptionalHeader_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktStructPEOptionalHeader_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktStructPEOptionalHeader_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktStructPEOptionalHeader_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktStructPEOptionalHeader_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktStructPEOptionalHeader_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktStructPEOptionalHeader_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktStructPEOptionalHeader_get_Magic(This,pVal)	\
    ( (This)->lpVtbl -> get_Magic(This,pVal) ) 

#define INktStructPEOptionalHeader_get_MajorLinkerVersion(This,pVal)	\
    ( (This)->lpVtbl -> get_MajorLinkerVersion(This,pVal) ) 

#define INktStructPEOptionalHeader_get_MinorLinkerVersion(This,pVal)	\
    ( (This)->lpVtbl -> get_MinorLinkerVersion(This,pVal) ) 

#define INktStructPEOptionalHeader_get_SizeOfCode(This,pVal)	\
    ( (This)->lpVtbl -> get_SizeOfCode(This,pVal) ) 

#define INktStructPEOptionalHeader_get_SizeOfInitializedData(This,pVal)	\
    ( (This)->lpVtbl -> get_SizeOfInitializedData(This,pVal) ) 

#define INktStructPEOptionalHeader_get_SizeOfUninitializedData(This,pVal)	\
    ( (This)->lpVtbl -> get_SizeOfUninitializedData(This,pVal) ) 

#define INktStructPEOptionalHeader_get_AddressOfEntryPoint(This,pVal)	\
    ( (This)->lpVtbl -> get_AddressOfEntryPoint(This,pVal) ) 

#define INktStructPEOptionalHeader_get_BaseOfCode(This,pVal)	\
    ( (This)->lpVtbl -> get_BaseOfCode(This,pVal) ) 

#define INktStructPEOptionalHeader_get_ImageBase(This,pVal)	\
    ( (This)->lpVtbl -> get_ImageBase(This,pVal) ) 

#define INktStructPEOptionalHeader_get_SectionAlignment(This,pVal)	\
    ( (This)->lpVtbl -> get_SectionAlignment(This,pVal) ) 

#define INktStructPEOptionalHeader_get_FileAlignment(This,pVal)	\
    ( (This)->lpVtbl -> get_FileAlignment(This,pVal) ) 

#define INktStructPEOptionalHeader_get_MajorOperatingSystemVersion(This,pVal)	\
    ( (This)->lpVtbl -> get_MajorOperatingSystemVersion(This,pVal) ) 

#define INktStructPEOptionalHeader_get_MinorOperatingSystemVersion(This,pVal)	\
    ( (This)->lpVtbl -> get_MinorOperatingSystemVersion(This,pVal) ) 

#define INktStructPEOptionalHeader_get_MajorImageVersion(This,pVal)	\
    ( (This)->lpVtbl -> get_MajorImageVersion(This,pVal) ) 

#define INktStructPEOptionalHeader_get_MinorImageVersion(This,pVal)	\
    ( (This)->lpVtbl -> get_MinorImageVersion(This,pVal) ) 

#define INktStructPEOptionalHeader_get_MajorSubsystemVersion(This,pVal)	\
    ( (This)->lpVtbl -> get_MajorSubsystemVersion(This,pVal) ) 

#define INktStructPEOptionalHeader_get_MinorSubsystemVersion(This,pVal)	\
    ( (This)->lpVtbl -> get_MinorSubsystemVersion(This,pVal) ) 

#define INktStructPEOptionalHeader_get_Win32VersionValue(This,pVal)	\
    ( (This)->lpVtbl -> get_Win32VersionValue(This,pVal) ) 

#define INktStructPEOptionalHeader_get_SizeOfImage(This,pVal)	\
    ( (This)->lpVtbl -> get_SizeOfImage(This,pVal) ) 

#define INktStructPEOptionalHeader_get_SizeOfHeaders(This,pVal)	\
    ( (This)->lpVtbl -> get_SizeOfHeaders(This,pVal) ) 

#define INktStructPEOptionalHeader_get_CheckSum(This,pVal)	\
    ( (This)->lpVtbl -> get_CheckSum(This,pVal) ) 

#define INktStructPEOptionalHeader_get_Subsystem(This,pVal)	\
    ( (This)->lpVtbl -> get_Subsystem(This,pVal) ) 

#define INktStructPEOptionalHeader_get_DllCharacteristics(This,pVal)	\
    ( (This)->lpVtbl -> get_DllCharacteristics(This,pVal) ) 

#define INktStructPEOptionalHeader_get_SizeOfStackReserve(This,pVal)	\
    ( (This)->lpVtbl -> get_SizeOfStackReserve(This,pVal) ) 

#define INktStructPEOptionalHeader_get_SizeOfStackCommit(This,pVal)	\
    ( (This)->lpVtbl -> get_SizeOfStackCommit(This,pVal) ) 

#define INktStructPEOptionalHeader_get_SizeOfHeapReserve(This,pVal)	\
    ( (This)->lpVtbl -> get_SizeOfHeapReserve(This,pVal) ) 

#define INktStructPEOptionalHeader_get_SizeOfHeapCommit(This,pVal)	\
    ( (This)->lpVtbl -> get_SizeOfHeapCommit(This,pVal) ) 

#define INktStructPEOptionalHeader_get_LoaderFlags(This,pVal)	\
    ( (This)->lpVtbl -> get_LoaderFlags(This,pVal) ) 

#define INktStructPEOptionalHeader_get_NumberOfRvaAndSizes(This,pVal)	\
    ( (This)->lpVtbl -> get_NumberOfRvaAndSizes(This,pVal) ) 

#define INktStructPEOptionalHeader_get_DataDirectoryStart(This,index,pVal)	\
    ( (This)->lpVtbl -> get_DataDirectoryStart(This,index,pVal) ) 

#define INktStructPEOptionalHeader_get_DataDirectoryEnd(This,index,pVal)	\
    ( (This)->lpVtbl -> get_DataDirectoryEnd(This,index,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_Magic_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_Magic_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_MajorLinkerVersion_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_MajorLinkerVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_MinorLinkerVersion_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_MinorLinkerVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_SizeOfCode_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_SizeOfCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_SizeOfInitializedData_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_SizeOfInitializedData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_SizeOfUninitializedData_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_SizeOfUninitializedData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_AddressOfEntryPoint_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_AddressOfEntryPoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_BaseOfCode_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_BaseOfCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_ImageBase_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_ImageBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_SectionAlignment_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_SectionAlignment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_FileAlignment_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_FileAlignment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_MajorOperatingSystemVersion_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_MajorOperatingSystemVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_MinorOperatingSystemVersion_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_MinorOperatingSystemVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_MajorImageVersion_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_MajorImageVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_MinorImageVersion_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_MinorImageVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_MajorSubsystemVersion_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_MajorSubsystemVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_MinorSubsystemVersion_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_MinorSubsystemVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_Win32VersionValue_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_Win32VersionValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_SizeOfImage_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_SizeOfImage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_SizeOfHeaders_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_SizeOfHeaders_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_CheckSum_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_CheckSum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_Subsystem_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_Subsystem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_DllCharacteristics_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_DllCharacteristics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_SizeOfStackReserve_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_SizeOfStackReserve_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_SizeOfStackCommit_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_SizeOfStackCommit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_SizeOfHeapReserve_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_SizeOfHeapReserve_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_SizeOfHeapCommit_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_SizeOfHeapCommit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_LoaderFlags_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_LoaderFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_NumberOfRvaAndSizes_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_NumberOfRvaAndSizes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_DataDirectoryStart_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [in] */ LONG index,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_DataDirectoryStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPEOptionalHeader_get_DataDirectoryEnd_Proxy( 
    INktStructPEOptionalHeader * This,
    /* [in] */ LONG index,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktStructPEOptionalHeader_get_DataDirectoryEnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktStructPEOptionalHeader_INTERFACE_DEFINED__ */


#ifndef __INktStructPESections_INTERFACE_DEFINED__
#define __INktStructPESections_INTERFACE_DEFINED__

/* interface INktStructPESections */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktStructPESections;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5BF7BC52-7158-4e0f-857E-F7FDB137249F")
    INktStructPESections : public IDispatch
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [in] */ LONG index,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_StartAddress( 
            /* [in] */ LONG index,
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EndAddress( 
            /* [in] */ LONG index,
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Characteristics( 
            /* [in] */ LONG index,
            /* [retval][out] */ LONG *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktStructPESectionsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktStructPESections * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktStructPESections * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktStructPESections * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktStructPESections * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktStructPESections * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktStructPESections * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktStructPESections * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            INktStructPESections * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            INktStructPESections * This,
            /* [in] */ LONG index,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StartAddress )( 
            INktStructPESections * This,
            /* [in] */ LONG index,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EndAddress )( 
            INktStructPESections * This,
            /* [in] */ LONG index,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Characteristics )( 
            INktStructPESections * This,
            /* [in] */ LONG index,
            /* [retval][out] */ LONG *pVal);
        
        END_INTERFACE
    } INktStructPESectionsVtbl;

    interface INktStructPESections
    {
        CONST_VTBL struct INktStructPESectionsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktStructPESections_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktStructPESections_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktStructPESections_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktStructPESections_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktStructPESections_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktStructPESections_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktStructPESections_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktStructPESections_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define INktStructPESections_get_Name(This,index,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,index,pVal) ) 

#define INktStructPESections_get_StartAddress(This,index,pVal)	\
    ( (This)->lpVtbl -> get_StartAddress(This,index,pVal) ) 

#define INktStructPESections_get_EndAddress(This,index,pVal)	\
    ( (This)->lpVtbl -> get_EndAddress(This,index,pVal) ) 

#define INktStructPESections_get_Characteristics(This,index,pVal)	\
    ( (This)->lpVtbl -> get_Characteristics(This,index,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPESections_get_Count_Proxy( 
    INktStructPESections * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPESections_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPESections_get_Name_Proxy( 
    INktStructPESections * This,
    /* [in] */ LONG index,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktStructPESections_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPESections_get_StartAddress_Proxy( 
    INktStructPESections * This,
    /* [in] */ LONG index,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktStructPESections_get_StartAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPESections_get_EndAddress_Proxy( 
    INktStructPESections * This,
    /* [in] */ LONG index,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktStructPESections_get_EndAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktStructPESections_get_Characteristics_Proxy( 
    INktStructPESections * This,
    /* [in] */ LONG index,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktStructPESections_get_Characteristics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktStructPESections_INTERFACE_DEFINED__ */


#ifndef __INktPdbFunctionSymbol_INTERFACE_DEFINED__
#define __INktPdbFunctionSymbol_INTERFACE_DEFINED__

/* interface INktPdbFunctionSymbol */
/* [unique][helpstring][nonextensible][local][dual][uuid][object] */ 


EXTERN_C const IID IID_INktPdbFunctionSymbol;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("21403F16-B370-4bc7-9D7E-0DEDA54EAD01")
    INktPdbFunctionSymbol : public INktObject
    {
    public:
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AddrOffset( 
            /* [retval][out] */ my_ssize_t *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PlatformBits( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ModuleName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CompleteName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INktPdbFunctionSymbolVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INktPdbFunctionSymbol * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INktPdbFunctionSymbol * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INktPdbFunctionSymbol * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INktPdbFunctionSymbol * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INktPdbFunctionSymbol * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INktPdbFunctionSymbol * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INktPdbFunctionSymbol * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserData )( 
            INktPdbFunctionSymbol * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserData )( 
            INktPdbFunctionSymbol * This,
            /* [in] */ my_ssize_t newValue);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceError )( 
            INktPdbFunctionSymbol * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviareId )( 
            INktPdbFunctionSymbol * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToString )( 
            INktPdbFunctionSymbol * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetObjectFromDeviareId )( 
            INktPdbFunctionSymbol * This,
            /* [in] */ my_ssize_t devId,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AddrOffset )( 
            INktPdbFunctionSymbol * This,
            /* [retval][out] */ my_ssize_t *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PlatformBits )( 
            INktPdbFunctionSymbol * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            INktPdbFunctionSymbol * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ModuleName )( 
            INktPdbFunctionSymbol * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CompleteName )( 
            INktPdbFunctionSymbol * This,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } INktPdbFunctionSymbolVtbl;

    interface INktPdbFunctionSymbol
    {
        CONST_VTBL struct INktPdbFunctionSymbolVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INktPdbFunctionSymbol_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INktPdbFunctionSymbol_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INktPdbFunctionSymbol_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INktPdbFunctionSymbol_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INktPdbFunctionSymbol_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INktPdbFunctionSymbol_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INktPdbFunctionSymbol_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INktPdbFunctionSymbol_get_UserData(This,pVal)	\
    ( (This)->lpVtbl -> get_UserData(This,pVal) ) 

#define INktPdbFunctionSymbol_put_UserData(This,newValue)	\
    ( (This)->lpVtbl -> put_UserData(This,newValue) ) 

#define INktPdbFunctionSymbol_get_InterfaceError(This,pVal)	\
    ( (This)->lpVtbl -> get_InterfaceError(This,pVal) ) 

#define INktPdbFunctionSymbol_get_DeviareId(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviareId(This,pVal) ) 

#define INktPdbFunctionSymbol_ToString(This,pVal)	\
    ( (This)->lpVtbl -> ToString(This,pVal) ) 

#define INktPdbFunctionSymbol_GetObjectFromDeviareId(This,devId,ppDisp)	\
    ( (This)->lpVtbl -> GetObjectFromDeviareId(This,devId,ppDisp) ) 


#define INktPdbFunctionSymbol_get_AddrOffset(This,pVal)	\
    ( (This)->lpVtbl -> get_AddrOffset(This,pVal) ) 

#define INktPdbFunctionSymbol_get_PlatformBits(This,pVal)	\
    ( (This)->lpVtbl -> get_PlatformBits(This,pVal) ) 

#define INktPdbFunctionSymbol_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define INktPdbFunctionSymbol_get_ModuleName(This,pVal)	\
    ( (This)->lpVtbl -> get_ModuleName(This,pVal) ) 

#define INktPdbFunctionSymbol_get_CompleteName(This,pVal)	\
    ( (This)->lpVtbl -> get_CompleteName(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktPdbFunctionSymbol_get_AddrOffset_Proxy( 
    INktPdbFunctionSymbol * This,
    /* [retval][out] */ my_ssize_t *pVal);


void __RPC_STUB INktPdbFunctionSymbol_get_AddrOffset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktPdbFunctionSymbol_get_PlatformBits_Proxy( 
    INktPdbFunctionSymbol * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB INktPdbFunctionSymbol_get_PlatformBits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktPdbFunctionSymbol_get_Name_Proxy( 
    INktPdbFunctionSymbol * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktPdbFunctionSymbol_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktPdbFunctionSymbol_get_ModuleName_Proxy( 
    INktPdbFunctionSymbol * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktPdbFunctionSymbol_get_ModuleName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE INktPdbFunctionSymbol_get_CompleteName_Proxy( 
    INktPdbFunctionSymbol * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB INktPdbFunctionSymbol_get_CompleteName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INktPdbFunctionSymbol_INTERFACE_DEFINED__ */



#ifndef __Deviare2_LIBRARY_DEFINED__
#define __Deviare2_LIBRARY_DEFINED__

/* library Deviare2 */
/* [helpstring][version][uuid] */ 

















EXTERN_C const IID LIBID_Deviare2;

EXTERN_C const CLSID CLSID_NktObject;

#ifdef __cplusplus

class DECLSPEC_UUID("6C58E03D-ACFF-426F-95AC-56C1FA8B0C73")
NktObject;
#endif

#ifndef __DNktProcessEvents_DISPINTERFACE_DEFINED__
#define __DNktProcessEvents_DISPINTERFACE_DEFINED__

/* dispinterface DNktProcessEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID_DNktProcessEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("07F5FFF7-4801-41c5-827F-91198104D81D")
    DNktProcessEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct DNktProcessEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            DNktProcessEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            DNktProcessEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            DNktProcessEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            DNktProcessEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            DNktProcessEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            DNktProcessEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            DNktProcessEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } DNktProcessEventsVtbl;

    interface DNktProcessEvents
    {
        CONST_VTBL struct DNktProcessEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define DNktProcessEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define DNktProcessEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define DNktProcessEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define DNktProcessEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define DNktProcessEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define DNktProcessEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define DNktProcessEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __DNktProcessEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NktProcess;

#ifdef __cplusplus

class DECLSPEC_UUID("E5FD693D-4E04-4058-BBBA-8D60BAAEE16C")
NktProcess;
#endif

EXTERN_C const CLSID CLSID_NktProcessesEnum;

#ifdef __cplusplus

class DECLSPEC_UUID("6D477BF3-F521-475f-ACF5-536D160486B1")
NktProcessesEnum;
#endif

EXTERN_C const CLSID CLSID_NktModule;

#ifdef __cplusplus

class DECLSPEC_UUID("5D36944B-2FBA-4CA1-A888-C5D926593D20")
NktModule;
#endif

EXTERN_C const CLSID CLSID_NktModulesEnum;

#ifdef __cplusplus

class DECLSPEC_UUID("4207729B-B2F5-4628-9885-23C0D0B1935B")
NktModulesEnum;
#endif

EXTERN_C const CLSID CLSID_NktExportedFunction;

#ifdef __cplusplus

class DECLSPEC_UUID("D901F1D6-FC59-4dca-9DAD-E9C1EE7A3280")
NktExportedFunction;
#endif

EXTERN_C const CLSID CLSID_NktExportedFunctionsEnum;

#ifdef __cplusplus

class DECLSPEC_UUID("BD246936-6FDD-4e9c-9CD4-E3A8EF7392DC")
NktExportedFunctionsEnum;
#endif

#ifndef __DNktHookEvents_DISPINTERFACE_DEFINED__
#define __DNktHookEvents_DISPINTERFACE_DEFINED__

/* dispinterface DNktHookEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID_DNktHookEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B497652A-056C-46b1-820F-AB15D9A458C3")
    DNktHookEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct DNktHookEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            DNktHookEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            DNktHookEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            DNktHookEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            DNktHookEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            DNktHookEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            DNktHookEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            DNktHookEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } DNktHookEventsVtbl;

    interface DNktHookEvents
    {
        CONST_VTBL struct DNktHookEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define DNktHookEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define DNktHookEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define DNktHookEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define DNktHookEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define DNktHookEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define DNktHookEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define DNktHookEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __DNktHookEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NktHook;

#ifdef __cplusplus

class DECLSPEC_UUID("4C71C5D5-B510-465a-B2E4-EAC7B271C164")
NktHook;
#endif

EXTERN_C const CLSID CLSID_NktHooksEnum;

#ifdef __cplusplus

class DECLSPEC_UUID("329BE071-4C82-4106-B7B1-E4A668EAFC1E")
NktHooksEnum;
#endif

EXTERN_C const CLSID CLSID_NktParam;

#ifdef __cplusplus

class DECLSPEC_UUID("5DA31131-A1AD-40b2-A526-876F61E1DF48")
NktParam;
#endif

EXTERN_C const CLSID CLSID_NktParamsEnum;

#ifdef __cplusplus

class DECLSPEC_UUID("51BFF27D-9E5F-47e6-AAB8-76BFD2592F43")
NktParamsEnum;
#endif

EXTERN_C const CLSID CLSID_NktHookCallInfo;

#ifdef __cplusplus

class DECLSPEC_UUID("D41D1782-E887-4c10-8DCF-9A25D9DDA888")
NktHookCallInfo;
#endif

EXTERN_C const CLSID CLSID_NktHookInfo;

#ifdef __cplusplus

class DECLSPEC_UUID("B0506A96-1E21-4da4-9177-B85546756B8C")
NktHookInfo;
#endif

EXTERN_C const CLSID CLSID_NktProcessMemory;

#ifdef __cplusplus

class DECLSPEC_UUID("D9706C19-23F1-435c-97FB-1D21378ECC88")
NktProcessMemory;
#endif

EXTERN_C const CLSID CLSID_NktStackTrace;

#ifdef __cplusplus

class DECLSPEC_UUID("ABF7E938-AF08-47ab-9857-CFF16E6C3215")
NktStackTrace;
#endif

#ifndef __DNktSpyMgrEvents_DISPINTERFACE_DEFINED__
#define __DNktSpyMgrEvents_DISPINTERFACE_DEFINED__

/* dispinterface DNktSpyMgrEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID_DNktSpyMgrEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("71BF517D-533B-473C-A3B8-63FD9887C195")
    DNktSpyMgrEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct DNktSpyMgrEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            DNktSpyMgrEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            DNktSpyMgrEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            DNktSpyMgrEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            DNktSpyMgrEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            DNktSpyMgrEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            DNktSpyMgrEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            DNktSpyMgrEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } DNktSpyMgrEventsVtbl;

    interface DNktSpyMgrEvents
    {
        CONST_VTBL struct DNktSpyMgrEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define DNktSpyMgrEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define DNktSpyMgrEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define DNktSpyMgrEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define DNktSpyMgrEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define DNktSpyMgrEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define DNktSpyMgrEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define DNktSpyMgrEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __DNktSpyMgrEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NktSpyMgr;

#ifdef __cplusplus

class DECLSPEC_UUID("4C7A99AA-560C-4f75-885E-3AF6CBD3CF32")
NktSpyMgr;
#endif

EXTERN_C const CLSID CLSID_NktDbObject;

#ifdef __cplusplus

class DECLSPEC_UUID("CA025A96-CE32-45b6-BD88-14D342E15314")
NktDbObject;
#endif

EXTERN_C const CLSID CLSID_NktDbObjectsEnum;

#ifdef __cplusplus

class DECLSPEC_UUID("1505BE0D-8C51-4362-B7A1-784F2C8462D0")
NktDbObjectsEnum;
#endif

EXTERN_C const CLSID CLSID_NktDbModule;

#ifdef __cplusplus

class DECLSPEC_UUID("2D6F0D5D-0265-4a2e-8190-1788C7CAF081")
NktDbModule;
#endif

EXTERN_C const CLSID CLSID_NktDbModulesEnum;

#ifdef __cplusplus

class DECLSPEC_UUID("534B629F-0C66-4479-82CC-F7E5743FC65F")
NktDbModulesEnum;
#endif

EXTERN_C const CLSID CLSID_NktTools;

#ifdef __cplusplus

class DECLSPEC_UUID("46071676-3D4D-40b2-9088-55354732414B")
NktTools;
#endif

EXTERN_C const CLSID CLSID_NktHookCallInfoPlugin;

#ifdef __cplusplus

class DECLSPEC_UUID("10BBBE7A-A01F-45fd-BB27-92CBDCE738E4")
NktHookCallInfoPlugin;
#endif

EXTERN_C const CLSID CLSID_NktStructPEFileHeader;

#ifdef __cplusplus

class DECLSPEC_UUID("B51A9270-C6BF-47c5-A71C-E4617A4C413A")
NktStructPEFileHeader;
#endif

EXTERN_C const CLSID CLSID_NktStructPEOptionalHeader;

#ifdef __cplusplus

class DECLSPEC_UUID("EE31335B-46A8-4e52-B00E-9576E56DBB27")
NktStructPEOptionalHeader;
#endif

EXTERN_C const CLSID CLSID_NktStructPESections;

#ifdef __cplusplus

class DECLSPEC_UUID("36238075-92D6-4573-AF09-76BE12CE3369")
NktStructPESections;
#endif

EXTERN_C const CLSID CLSID_NktPdbFunctionSymbol;

#ifdef __cplusplus

class DECLSPEC_UUID("913602A6-9390-4cca-84F2-7CBB53F46689")
NktPdbFunctionSymbol;
#endif
#endif /* __Deviare2_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


