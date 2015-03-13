#ifndef _NTDLL_H
#define _NTDLL_H

typedef enum _SHUTDOWN_ACTION {
     ShutdownNoReboot,
     ShutdownReboot,
     ShutdownPowerOff
} SHUTDOWN_ACTION;

DWORD WINAPI NtShutdownSystem (SHUTDOWN_ACTION Action);

/***************************************************************************
 * NAME                                                         EXPORTED
 *      LdrLoadDll
 *
 * DESCRIPTION
 *
 * ARGUMENTS
 *
 * RETURN VALUE
 *
 * REVISIONS
 *
 * NOTE
 *
 * @implemented
 */
NTSTATUS NTAPI
LdrLoadDll (IN PWSTR SearchPath OPTIONAL,
            IN PULONG LoadFlags OPTIONAL,
            IN PUNICODE_STRING Name,
            OUT PVOID *BaseAddress /* also known as HMODULE*, and PHANDLE 'DllHandle' */);

NTSTATUS NTAPI 
LdrUnloadDll(IN HANDLE ModuleHandle );
			
NTSTATUS 
NTAPI 
LdrGetProcedureAddress(IN HMODULE ModuleHandle, IN PANSI_STRING FunctionName OPTIONAL, IN WORD Oridinal OPTIONAL, OUT PVOID *FunctionAddress );

NTSTATUS
NTAPI
LdrGetProcedureAddressEx(
    __in PVOID DllHandle,
    __in_opt PANSI_STRING ProcedureName,
    __in_opt ULONG ProcedureNumber,
    __out PVOID *ProcedureAddress,
    __in ULONG Flags
    );

NTSTATUS 
NTAPI 
LdrGetDllHandle(PWORD pwPath OPTIONAL, IN PVOID Unused OPTIONAL, IN PUNICODE_STRING ModuleFileName, OUT PHANDLE pHModule);

NTSTATUS 
NTAPI 
LdrGetDllHandleEx(IN ULONG Flags, IN PWSTR DllPath OPTIONAL, 
    IN PULONG DllCharacteristics OPTIONAL, IN PUNICODE_STRING DllName, OUT PVOID *DllHandle OPTIONAL);
	
NTSTATUS 
NTAPI 
NtDuplicateObject(HANDLE SourceProcessHandle, 
					HANDLE SourceHandle, 
					HANDLE TargetProcessHandle, 
					PHANDLE TargetHandle, 
					ACCESS_MASK DesiredAccess, 
					ULONG HandleAttributes, 
					ULONG Options);

NTSTATUS 
NTAPI 
NtNotifyChangeDirectoryFile(HANDLE FileHandle, HANDLE Event OPTIONAL, PIO_APC_ROUTINE ApcRoutine OPTIONAL, 
							PVOID ApcContext OPTIONAL, PIO_STATUS_BLOCK IoStatusBlock, PVOID Buffer, ULONG BufferSize, 
							ULONG CompletionFilter, BOOLEAN WatchTree );

NTSYSAPI 
NTSTATUS
NTAPI
NtCreateFile(
  OUT PHANDLE             FileHandle,
  IN ACCESS_MASK          DesiredAccess,
  IN POBJECT_ATTRIBUTES   ObjectAttributes,
  OUT PIO_STATUS_BLOCK    IoStatusBlock,
  IN PLARGE_INTEGER       AllocationSize OPTIONAL,
  IN ULONG                FileAttributes,
  IN ULONG                ShareAccess,
  IN ULONG                CreateDisposition,
  IN ULONG                CreateOptions,
  IN PVOID                EaBuffer OPTIONAL,
  IN ULONG                EaLength );

NTSYSAPI 
NTSTATUS
NTAPI
NtOpenFile(
  OUT PHANDLE             FileHandle,
  IN ACCESS_MASK          DesiredAccess,
  IN POBJECT_ATTRIBUTES   ObjectAttributes,
  OUT PIO_STATUS_BLOCK    IoStatusBlock,
  IN ULONG                ShareAccess,
  IN ULONG                OpenOptions );


NTSYSAPI 
NTSTATUS
NTAPI
NtReadFile(

  IN HANDLE               FileHandle,
  IN HANDLE               Event OPTIONAL,
  IN PIO_APC_ROUTINE      ApcRoutine OPTIONAL,
  IN PVOID                ApcContext OPTIONAL,
  OUT PIO_STATUS_BLOCK    IoStatusBlock,
  OUT PVOID               Buffer,
  IN ULONG                Length,
  IN PLARGE_INTEGER       ByteOffset OPTIONAL,
  IN PULONG               Key OPTIONAL );

NTSYSAPI 
NTSTATUS
NTAPI
NtWriteFile(  IN HANDLE               FileHandle,
			IN HANDLE               Event OPTIONAL,
  IN PIO_APC_ROUTINE      ApcRoutine OPTIONAL,
  IN PVOID                ApcContext OPTIONAL,
  OUT PIO_STATUS_BLOCK    IoStatusBlock,
  IN PVOID                Buffer,
  IN ULONG                Length,
  IN PLARGE_INTEGER       ByteOffset OPTIONAL,
  IN PULONG               Key OPTIONAL );

NTSYSAPI 
NTSTATUS
NTAPI
NtDeleteFile(
  IN POBJECT_ATTRIBUTES   ObjectAttributes );

NTSYSAPI 
NTSTATUS
NTAPI
NtSetInformationFile(

  IN HANDLE               FileHandle,
  OUT PIO_STATUS_BLOCK    IoStatusBlock,
  IN PVOID                FileInformation,
  IN ULONG                Length,
  IN FILE_INFORMATION_CLASS FileInformationClass );

NTSYSAPI 
NTSTATUS
NTAPI
NtQueryInformationFile(

  IN HANDLE               FileHandle,
  OUT PIO_STATUS_BLOCK    IoStatusBlock,
  OUT PVOID               FileInformation,
  IN ULONG                Length,
  IN FILE_INFORMATION_CLASS FileInformationClass );

NTSYSAPI 
NTSTATUS
NTAPI
NtQueryDirectoryFile(
  _In_      HANDLE FileHandle,
  _In_opt_  HANDLE Event,
  _In_opt_  PIO_APC_ROUTINE ApcRoutine,
  _In_opt_  PVOID ApcContext,
  _Out_     PIO_STATUS_BLOCK IoStatusBlock,
  _Out_     PVOID FileInformation,
  _In_      ULONG Length,
  _In_      FILE_INFORMATION_CLASS FileInformationClass,
  _In_      BOOLEAN ReturnSingleEntry,
  _In_opt_  PUNICODE_STRING FileName,
  _In_      BOOLEAN RestartScan
);

typedef struct _INITIAL_TEB {
  PVOID                StackBase;
  PVOID                StackLimit;
  PVOID                StackCommit;
  PVOID                StackCommitMax;
  PVOID                StackReserved;
} INITIAL_TEB, *PINITIAL_TEB;

typedef struct _CLIENT_ID{
	DWORD					ClientID0;
	DWORD					ClientID1; // thread id
} CLIENT_ID, *PCLIENT_ID;

NTSTATUS NTAPI 
NtCreateThread(
    OUT PHANDLE ThreadHandle,
    IN  ACCESS_MASK DesiredAccess,
    IN  POBJECT_ATTRIBUTES ObjectAttributes    OPTIONAL,
    IN  HANDLE ProcessHandle,
    OUT PCLIENT_ID ClientId,
    IN  PCONTEXT ThreadContext,
    IN  PINITIAL_TEB UserStack,
    IN  BOOLEAN CreateSuspended
);

NTSTATUS NTAPI NtCreateThreadEx
(
  OUT PHANDLE hThread,
  IN ACCESS_MASK DesiredAccess,
  IN LPVOID ObjectAttributes,
  IN HANDLE ProcessHandle,
  IN LPTHREAD_START_ROUTINE lpStartAddress,
  IN LPVOID lpParameter,
  IN BOOL CreateSuspended, 
  IN ULONG StackZeroBits,
  IN ULONG SizeOfStackCommit,
  IN ULONG SizeOfStackReserve,
  OUT LPVOID lpBytesBuffer
);

NTSYSAPI 
NTSTATUS
NTAPI
RtlCreateUserThread(
  IN HANDLE               ProcessHandle,
  IN PSECURITY_DESCRIPTOR SecurityDescriptor OPTIONAL,
  IN BOOLEAN              CreateSuspended,
  IN ULONG                StackZeroBits,
  IN OUT PULONG           StackReserved,
  IN OUT PULONG           StackCommit,
  IN PVOID                StartAddress,
  IN PVOID                StartParameter OPTIONAL,
  OUT PHANDLE             ThreadHandle,
  OUT PCLIENT_ID          ClientID );
  
NTSYSAPI 
NTSTATUS
NTAPI
NtTerminateThread(HANDLE ThreadHandle, NTSTATUS ExitStatus );

NTSTATUS
NTAPI
NtTerminateProcess(IN HANDLE ProcessHandle OPTIONAL,
					IN NTSTATUS             ExitStatus );
			
DWORD WINAPI CreateProcessInternalW(
  __in         HANDLE hTken,
  __in_opt     LPCWSTR lpApplicationName,
  __inout_opt  LPWSTR lpCommandLine,
  __in_opt     LPSECURITY_ATTRIBUTES lpProcessAttributes,
  __in_opt     LPSECURITY_ATTRIBUTES lpThreadAttributes,
  __in         BOOL bInheritHandles,
  __in         DWORD dwCreationFlags,
  __in_opt     LPVOID lpEnvironment,
  __in_opt     LPCWSTR lpCurrentDirectory,
  __in         LPSTARTUPINFOW lpStartupInfo,
  __out        LPPROCESS_INFORMATION lpProcessInformation,
  __out        PHANDLE hNewToken
);
DWORD WINAPI CreateProcessInternalA(
  __in         HANDLE hTken,
  __in_opt     LPCSTR lpApplicationName,
  __inout_opt  LPSTR lpCommandLine,
  __in_opt     LPSECURITY_ATTRIBUTES lpProcessAttributes,
  __in_opt     LPSECURITY_ATTRIBUTES lpThreadAttributes,
  __in         BOOL bInheritHandles,
  __in         DWORD dwCreationFlags,
  __in_opt     LPVOID lpEnvironment,
  __in_opt     LPCSTR lpCurrentDirectory,
  __in         LPSTARTUPINFOA lpStartupInfo,
  __out        LPPROCESS_INFORMATION lpProcessInformation,
  __out        PHANDLE hNewToken
);

// 
// begin_winnt 
// 
 
// begin_ntddk begin_wdm begin_nthal 
// 
// Registry Specific Access Rights. 
// 
 
#define KEY_QUERY_VALUE         (0x0001) 
#define KEY_SET_VALUE           (0x0002) 
#define KEY_CREATE_SUB_KEY      (0x0004) 
#define KEY_ENUMERATE_SUB_KEYS  (0x0008) 
#define KEY_NOTIFY              (0x0010) 
#define KEY_CREATE_LINK         (0x0020) 
#define KEY_WOW64_32KEY         (0x0200) 
#define KEY_WOW64_64KEY         (0x0100) 
#define KEY_WOW64_RES           (0x0300) 
 
#define KEY_READ                ((STANDARD_RIGHTS_READ       | KEY_QUERY_VALUE            | KEY_ENUMERATE_SUB_KEYS     | KEY_NOTIFY) & (~SYNCHRONIZE)) 
 
 
#define KEY_WRITE               ((STANDARD_RIGHTS_WRITE      | KEY_SET_VALUE              | KEY_CREATE_SUB_KEY) & (~SYNCHRONIZE)) 
 
#define KEY_EXECUTE             ((KEY_READ) & (~SYNCHRONIZE)) 
 
#define KEY_ALL_ACCESS          ((STANDARD_RIGHTS_ALL        | KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_ENUMERATE_SUB_KEYS |KEY_NOTIFY |KEY_CREATE_LINK)&(~SYNCHRONIZE)) 
 
// 
// Open/Create Options 
// 
 
#define REG_OPTION_RESERVED         (0x00000000L)   // Parameter is reserved 
 
#define REG_OPTION_NON_VOLATILE     (0x00000000L)   // Key is preserved 
                                                    // when system is rebooted 
 
#define REG_OPTION_VOLATILE         (0x00000001L)   // Key is not preserved 
                                                    // when system is rebooted 
 
#define REG_OPTION_CREATE_LINK      (0x00000002L)   // Created key is a 
                                                    // symbolic link 
 
#define REG_OPTION_BACKUP_RESTORE   (0x00000004L)   // open for backup or restore 
                                                    // special access rules 
                                                    // privilege required 
 
#define REG_OPTION_OPEN_LINK        (0x00000008L)   // Open symbolic link 
 
#define REG_LEGAL_OPTION (REG_OPTION_RESERVED            | REG_OPTION_NON_VOLATILE        |REG_OPTION_VOLATILE            |REG_OPTION_CREATE_LINK         |REG_OPTION_BACKUP_RESTORE      |REG_OPTION_OPEN_LINK) 
 
// 
// Key creation/open disposition 
// 
 
#define REG_CREATED_NEW_KEY         (0x00000001L)   // New Registry Key created 
#define REG_OPENED_EXISTING_KEY     (0x00000002L)   // Existing Key opened 
 
// 
// hive format to be used by Reg(Nt)SaveKeyEx 
// 
#define REG_STANDARD_FORMAT     1 
#define REG_LATEST_FORMAT       2 
#define REG_NO_COMPRESSION      4 
 
// 
// Key restore flags 
// 
 
#define REG_WHOLE_HIVE_VOLATILE     (0x00000001L)   // Restore whole hive volatile 
#define REG_REFRESH_HIVE            (0x00000002L)   // Unwind changes to last flush 
#define REG_NO_LAZY_FLUSH           (0x00000004L)   // Never lazy flush this hive 
#define REG_FORCE_RESTORE           (0x00000008L)   // Force the restore process even when we have open handles on subkeys 
 
// end_ntddk end_wdm end_nthal 
 
// 
// Notify filter values 
// 
#define REG_NOTIFY_CHANGE_NAME          (0x00000001L) // Create or delete (child) 
#define REG_NOTIFY_CHANGE_ATTRIBUTES    (0x00000002L) 
#define REG_NOTIFY_CHANGE_LAST_SET      (0x00000004L) // time stamp 
#define REG_NOTIFY_CHANGE_SECURITY      (0x00000008L) 
 
#define REG_LEGAL_CHANGE_FILTER (REG_NOTIFY_CHANGE_NAME          |REG_NOTIFY_CHANGE_ATTRIBUTES    |REG_NOTIFY_CHANGE_LAST_SET      |REG_NOTIFY_CHANGE_SECURITY) 
 
// 
// end_winnt 
// 
 
// Boot condition flags (for NtInitializeRegistry) 
 
#define REG_INIT_BOOT_SM         (0x0000)    // Init called from SM after Autocheck, etc. 
#define REG_INIT_BOOT_SETUP      (0x0001)    // Init called from text-mode setup 
 
// 
// Values to indicate that a standard Boot has been accepted by the Service Controller. 
// The Boot information will be saved by NtInitializeRegistry to a registry ControlSet with an 
// 
//       ID = [Given Boot Condition Value] - REG_INIT_BOOT_ACCEPTED_BASE 
// 
 
#define REG_INIT_BOOT_ACCEPTED_BASE   (0x0002) 
#define REG_INIT_BOOT_ACCEPTED_MAX    REG_INIT_BOOT_ACCEPTED_BASE + 999 
 
#define REG_INIT_MAX_VALID_CONDITION  REG_INIT_BOOT_ACCEPTED_MAX 
 
// 
// registry limits for value name and key name 
// 
#define REG_MAX_KEY_VALUE_NAME_LENGTH   32767       // 32k - sanity limit for value name 
#define REG_MAX_KEY_NAME_LENGTH         512         // allow for 256 unicode, as promise 
 
 
// begin_ntddk begin_wdm begin_nthal 
// 
// Key query structures 
// 
 
typedef struct _KEY_BASIC_INFORMATION { 
    LARGE_INTEGER LastWriteTime; 
    ULONG   TitleIndex; 
    ULONG   NameLength; 
    WCHAR   Name[1];            // Variable length string 
} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION; 
 
typedef struct _KEY_NODE_INFORMATION { 
    LARGE_INTEGER LastWriteTime; 
    ULONG   TitleIndex; 
    ULONG   ClassOffset; 
    ULONG   ClassLength; 
    ULONG   NameLength; 
    WCHAR   Name[1];            // Variable length string 
//          Class[1];           // Variable length string not declared 
} KEY_NODE_INFORMATION, *PKEY_NODE_INFORMATION; 
 
typedef struct _KEY_FULL_INFORMATION { 
    LARGE_INTEGER LastWriteTime; 
    ULONG   TitleIndex; 
    ULONG   ClassOffset; 
    ULONG   ClassLength; 
    ULONG   SubKeys; 
    ULONG   MaxNameLen; 
    ULONG   MaxClassLen; 
    ULONG   Values; 
    ULONG   MaxValueNameLen; 
    ULONG   MaxValueDataLen; 
    WCHAR   Class[1];           // Variable length 
} KEY_FULL_INFORMATION, *PKEY_FULL_INFORMATION; 
 
// end_wdm 
typedef struct _KEY_NAME_INFORMATION { 
    ULONG   NameLength; 
    WCHAR   Name[1];            // Variable length string 
} KEY_NAME_INFORMATION, *PKEY_NAME_INFORMATION; 
 
typedef struct _KEY_CACHED_INFORMATION { 
    LARGE_INTEGER LastWriteTime; 
    ULONG   TitleIndex; 
    ULONG   SubKeys; 
    ULONG   MaxNameLen; 
    ULONG   Values; 
    ULONG   MaxValueNameLen; 
    ULONG   MaxValueDataLen; 
    ULONG   NameLength; 
    WCHAR   Name[1];            // Variable length string 
} KEY_CACHED_INFORMATION, *PKEY_CACHED_INFORMATION; 
 
typedef struct _KEY_FLAGS_INFORMATION { 
    ULONG   UserFlags; 
} KEY_FLAGS_INFORMATION, *PKEY_FLAGS_INFORMATION; 
 
// begin_wdm 
typedef enum _KEY_INFORMATION_CLASS { 
    KeyBasicInformation, 
    KeyNodeInformation, 
    KeyFullInformation 
// end_wdm 
    , 
    KeyNameInformation, 
    KeyCachedInformation, 
    KeyFlagsInformation 
// begin_wdm 
} KEY_INFORMATION_CLASS; 
 
typedef struct _KEY_WRITE_TIME_INFORMATION { 
    LARGE_INTEGER LastWriteTime; 
} KEY_WRITE_TIME_INFORMATION, *PKEY_WRITE_TIME_INFORMATION; 
 
typedef struct _KEY_USER_FLAGS_INFORMATION { 
    ULONG   UserFlags; 
} KEY_USER_FLAGS_INFORMATION, *PKEY_USER_FLAGS_INFORMATION; 

/*
typedef enum _KEY_SET_INFORMATION_CLASS { 
    KeyWriteTimeInformation, 
    KeyUserFlagsInformation 
} KEY_SET_INFORMATION_CLASS;
*/ 
 
// 
// Value entry query structures 
// 
 
typedef struct _KEY_VALUE_BASIC_INFORMATION { 
    ULONG   TitleIndex; 
    ULONG   Type; 
    ULONG   NameLength; 
    WCHAR   Name[1];            // Variable size 
} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION; 
 
typedef struct _KEY_VALUE_FULL_INFORMATION { 
    ULONG   TitleIndex; 
    ULONG   Type; 
    ULONG   DataOffset; 
    ULONG   DataLength; 
    ULONG   NameLength; 
    WCHAR   Name[1];            // Variable size 
//          Data[1];            // Variable size data not declared 
} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION; 
 
typedef struct _KEY_VALUE_PARTIAL_INFORMATION { 
    ULONG   TitleIndex; 
    ULONG   Type; 
    ULONG   DataLength; 
    UCHAR   Data[1];            // Variable size 
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION; 
 
typedef struct _KEY_VALUE_PARTIAL_INFORMATION_ALIGN64 { 
    ULONG   Type; 
    ULONG   DataLength; 
    UCHAR   Data[1];            // Variable size 
} KEY_VALUE_PARTIAL_INFORMATION_ALIGN64, *PKEY_VALUE_PARTIAL_INFORMATION_ALIGN64; 
 
/*
typedef struct _KEY_VALUE_ENTRY { 
    PUNICODE_STRING ValueName; 
    ULONG           DataLength; 
    ULONG           DataOffset; 
    ULONG           Type; 
} KEY_VALUE_ENTRY, *PKEY_VALUE_ENTRY; 
*/

typedef enum _KEY_VALUE_INFORMATION_CLASS { 
    KeyValueBasicInformation, 
    KeyValueFullInformation, 
    KeyValuePartialInformation, 
    KeyValueFullInformationAlign64, 
    KeyValuePartialInformationAlign64 
} KEY_VALUE_INFORMATION_CLASS; 
 
 
// end_ntddk end_wdm end_nthal 
// 
// Notify return structures 
// 
 
typedef enum _REG_ACTION { 
    KeyAdded, 
    KeyRemoved, 
    KeyModified 
} REG_ACTION; 
 
typedef struct _REG_NOTIFY_INFORMATION { 
    ULONG           NextEntryOffset; 
    REG_ACTION      Action; 
    ULONG           KeyLength; 
    WCHAR           Key[1];     // Variable size 
} REG_NOTIFY_INFORMATION, *PREG_NOTIFY_INFORMATION; 

// NtQueryDirectoryFile
typedef struct _FILE_DIRECTORY_INFORMATION {
  ULONG         NextEntryOffset;
  ULONG         FileIndex;
  LARGE_INTEGER CreationTime;
  LARGE_INTEGER LastAccessTime;
  LARGE_INTEGER LastWriteTime;
  LARGE_INTEGER ChangeTime;
  LARGE_INTEGER EndOfFile;
  LARGE_INTEGER AllocationSize;
  ULONG         FileAttributes;
  ULONG         FileNameLength;
  WCHAR         FileName[1];
} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;

typedef struct _FILE_FULL_DIR_INFORMATION {
  ULONG         NextEntryOffset;
  ULONG         FileIndex;
  LARGE_INTEGER CreationTime;
  LARGE_INTEGER LastAccessTime;
  LARGE_INTEGER LastWriteTime;
  LARGE_INTEGER ChangeTime;
  LARGE_INTEGER EndOfFile;
  LARGE_INTEGER AllocationSize;
  ULONG         FileAttributes;
  ULONG         FileNameLength;
  ULONG         EaSize;
  WCHAR         FileName[1];
} FILE_FULL_DIR_INFORMATION, *PFILE_FULL_DIR_INFORMATION;

typedef struct _FILE_ID_BOTH_DIR_INFORMATION {
  ULONG         NextEntryOffset;
  ULONG         FileIndex;
  LARGE_INTEGER CreationTime;
  LARGE_INTEGER LastAccessTime;
  LARGE_INTEGER LastWriteTime;
  LARGE_INTEGER ChangeTime;
  LARGE_INTEGER EndOfFile;
  LARGE_INTEGER AllocationSize;
  ULONG         FileAttributes;
  ULONG         FileNameLength;
  ULONG         EaSize;
  CCHAR         ShortNameLength;
  WCHAR         ShortName[12];
  LARGE_INTEGER FileId;
  WCHAR         FileName[1];
} FILE_ID_BOTH_DIR_INFORMATION, *PFILE_ID_BOTH_DIR_INFORMATION;

typedef struct _FILE_ID_FULL_DIR_INFORMATION {
  ULONG         NextEntryOffset;
  ULONG         FileIndex;
  LARGE_INTEGER CreationTime;
  LARGE_INTEGER LastAccessTime;
  LARGE_INTEGER LastWriteTime;
  LARGE_INTEGER ChangeTime;
  LARGE_INTEGER EndOfFile;
  LARGE_INTEGER AllocationSize;
  ULONG         FileAttributes;
  ULONG         FileNameLength;
  ULONG         EaSize;
  LARGE_INTEGER FileId;
  WCHAR         FileName[1];
} FILE_ID_FULL_DIR_INFORMATION, *PFILE_ID_FULL_DIR_INFORMATION;

typedef struct _FILE_BASIC_INFORMATION {
  LARGE_INTEGER CreationTime;
  LARGE_INTEGER LastAccessTime;
  LARGE_INTEGER LastWriteTime;
  LARGE_INTEGER ChangeTime;
  ULONG         FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;
 
typedef struct _FILE_NAMES_INFORMATION {
  ULONG NextEntryOffset;
  ULONG FileIndex;
  ULONG FileNameLength;
  WCHAR FileName[1];
} FILE_NAMES_INFORMATION, *PFILE_NAMES_INFORMATION;

typedef struct _FILE_OBJECTID_INFORMATION {
  LONGLONG FileReference;
  UCHAR    ObjectId[16];
  union {
    struct {
      UCHAR BirthVolumeId[16];
      UCHAR BirthObjectId[16];
      UCHAR DomainId[16];
    };
    UCHAR  ExtendedInfo[48];
  };
} FILE_OBJECTID_INFORMATION, *PFILE_OBJECTID_INFORMATION;

typedef struct _FILE_REPARSE_POINT_INFORMATION {
  LONGLONG FileReference;
  ULONG    Tag;
} FILE_REPARSE_POINT_INFORMATION, *PFILE_REPARSE_POINT_INFORMATION;

typedef struct _FILE_BOTH_DIR_INFORMATION {
  ULONG         NextEntryOffset;
  ULONG         FileIndex;
  LARGE_INTEGER CreationTime;
  LARGE_INTEGER LastAccessTime;
  LARGE_INTEGER LastWriteTime;
  LARGE_INTEGER ChangeTime;
  LARGE_INTEGER EndOfFile;
  LARGE_INTEGER AllocationSize;
  ULONG         FileAttributes;
  ULONG         FileNameLength;
  ULONG         EaSize;
  CCHAR         ShortNameLength;
  WCHAR         ShortName[12];
  WCHAR         FileName[1];
} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;


// 
// Nt level registry API calls 
// 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtCreateKey( 
    OUT PHANDLE KeyHandle, 
    IN ACCESS_MASK DesiredAccess, 
    IN POBJECT_ATTRIBUTES ObjectAttributes, 
    IN ULONG TitleIndex, 
    IN PUNICODE_STRING Class OPTIONAL, 
    IN ULONG CreateOptions, 
    OUT PULONG Disposition OPTIONAL 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtDeleteKey( 
    IN HANDLE KeyHandle 
    ); 
 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtDeleteValueKey( 
    IN HANDLE KeyHandle, 
    IN PUNICODE_STRING ValueName 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtEnumerateKey( 
    IN HANDLE KeyHandle, 
    IN ULONG Index, 
    IN KEY_INFORMATION_CLASS KeyInformationClass, 
    OUT PVOID KeyInformation, 
    IN ULONG Length, 
    OUT PULONG ResultLength 
    ); 
 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtEnumerateValueKey( 
    IN HANDLE KeyHandle, 
    IN ULONG Index, 
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass, 
    OUT PVOID KeyValueInformation, 
    IN ULONG Length, 
    OUT PULONG ResultLength 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtFlushKey( 
    IN HANDLE KeyHandle 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtInitializeRegistry( 
    IN USHORT BootCondition 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtNotifyChangeKey( 
    IN HANDLE KeyHandle, 
    IN HANDLE Event OPTIONAL, 
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL, 
    IN PVOID ApcContext OPTIONAL, 
    OUT PIO_STATUS_BLOCK IoStatusBlock, 
    IN ULONG CompletionFilter, 
    IN BOOLEAN WatchTree, 
    OUT PVOID Buffer, 
    IN ULONG BufferSize, 
    IN BOOLEAN Asynchronous 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtNotifyChangeMultipleKeys( 
    IN HANDLE MasterKeyHandle,  		 
    IN ULONG Count, 
    IN OBJECT_ATTRIBUTES SlaveObjects[], 
    IN HANDLE Event OPTIONAL, 
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL, 
    IN PVOID ApcContext OPTIONAL, 
    OUT PIO_STATUS_BLOCK IoStatusBlock, 
    IN ULONG CompletionFilter, 
    IN BOOLEAN WatchTree, 
    OUT PVOID Buffer, 
    IN ULONG BufferSize, 
    IN BOOLEAN Asynchronous 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtLoadKey( 
    IN POBJECT_ATTRIBUTES TargetKey, 
    IN POBJECT_ATTRIBUTES SourceFile 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtLoadKey2( 
    IN POBJECT_ATTRIBUTES TargetKey, 
    IN POBJECT_ATTRIBUTES SourceFile, 
    IN ULONG Flags 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtOpenKey( 
    OUT PHANDLE KeyHandle, 
    IN ACCESS_MASK DesiredAccess, 
    IN POBJECT_ATTRIBUTES ObjectAttributes 
    ); 
 
NTSTATUS NtOpenKeyEx(
  _Out_  PHANDLE KeyHandle,
  _In_   ACCESS_MASK DesiredAccess,
  _In_   POBJECT_ATTRIBUTES ObjectAttributes,
  _In_   ULONG OpenOptions
);


NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtQueryKey( 
    IN HANDLE KeyHandle, 
    IN KEY_INFORMATION_CLASS KeyInformationClass, 
    OUT PVOID KeyInformation, 
    IN ULONG Length, 
    OUT PULONG ResultLength 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtQueryValueKey( 
    IN HANDLE KeyHandle, 
    IN PUNICODE_STRING ValueName, 
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass, 
    OUT PVOID KeyValueInformation, 
    IN ULONG Length, 
    OUT PULONG ResultLength 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtQueryMultipleValueKey( 
    IN HANDLE KeyHandle, 
    IN OUT PKEY_VALUE_ENTRY ValueEntries, 
    IN ULONG EntryCount, 
    OUT PVOID ValueBuffer, 
    IN OUT PULONG BufferLength, 
    OUT OPTIONAL PULONG RequiredBufferLength 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtReplaceKey( 
    IN POBJECT_ATTRIBUTES NewFile, 
    IN HANDLE             TargetHandle, 
    IN POBJECT_ATTRIBUTES OldFile 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtRenameKey( 
    IN HANDLE           KeyHandle, 
    IN PUNICODE_STRING  NewName 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtCompactKeys( 
    IN ULONG Count, 
    IN HANDLE KeyArray[] 
            ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtCompressKey( 
    IN HANDLE Key 
            ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtRestoreKey( 
    IN HANDLE KeyHandle, 
    IN HANDLE FileHandle, 
    IN ULONG  Flags 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtSaveKey( 
    IN HANDLE KeyHandle, 
    IN HANDLE FileHandle 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtSaveKeyEx( 
    IN HANDLE KeyHandle, 
    IN HANDLE FileHandle, 
    IN ULONG  Format 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtSaveMergedKeys( 
    IN HANDLE HighPrecedenceKeyHandle, 
    IN HANDLE LowPrecedenceKeyHandle, 
    IN HANDLE FileHandle 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtSetValueKey( 
    IN HANDLE KeyHandle, 
    IN PUNICODE_STRING ValueName, 
    IN ULONG TitleIndex OPTIONAL, 
    IN ULONG Type, 
    IN PVOID Data, 
    IN ULONG DataSize 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtUnloadKey( 
    IN POBJECT_ATTRIBUTES TargetKey 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtUnloadKeyEx( 
    IN POBJECT_ATTRIBUTES TargetKey, 
    IN HANDLE Event OPTIONAL 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtSetInformationKey( 
    IN HANDLE KeyHandle, 
    IN KEY_SET_INFORMATION_CLASS KeySetInformationClass, 
    IN PVOID KeySetInformation, 
    IN ULONG KeySetInformationLength 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtQueryOpenSubKeys( 
    IN POBJECT_ATTRIBUTES TargetKey, 
    OUT PULONG  HandleCount 
    ); 
 
NTSYSCALLAPI 
NTSTATUS 
NTAPI 
NtLockRegistryKey( 
    IN HANDLE           KeyHandle 
    ); 
 
NTSYSCALLAPI NTSTATUS NTAPI NtLockProductActivationKeys(ULONG   *pPrivateVer, ULONG   *pIsSafeMode); 

NTSYSAPI NTSTATUS NTAPI NtQueryAttributesFile(IN POBJECT_ATTRIBUTES ObjectAttributes, OUT PFILE_BASIC_INFORMATION FileAttributes);
  
typedef struct _LARGE_UNICODE_STRING {
  ULONG Length;
  ULONG MaximumLength:31;
  ULONG bAnsi:1;
  PWSTR Buffer;
} LARGE_UNICODE_STRING, *PLARGE_UNICODE_STRING;

typedef struct _LARGE_STRING {
  ULONG Length;
  ULONG MaximumLength:31;
  ULONG bAnsi:1;
  PVOID Buffer;
} LARGE_STRING, *PLARGE_STRING;

typedef struct _LARGE_ANSI_STRING {
  ULONG Length;
  ULONG MaximumLength:31;
  ULONG bAnsi:1;
  PCHAR Buffer;
} LARGE_ANSI_STRING, *PLARGE_ANSI_STRING;

HWND NTAPI NtUserCreateWindowEx(DWORD dwExStyle, PLARGE_STRING plstrClassName, PLARGE_STRING plstrClsVersion,
                                PLARGE_STRING plstrWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight,
                                HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, DWORD dwFlags,
                                PVOID acbiBuffer);


NTSYSAPI NTSTATUS NTAPI NtSetSystemEnvironmentValue(PUNICODE_STRING VariableName, PUNICODE_STRING Value);
NTSYSAPI NTSTATUS NTAPI RtlExpandEnvironmentStrings_U(IN PVOID Environment OPTIONAL, IN PUNICODE_STRING SourceString, OUT PUNICODE_STRING DestinationString, OUT PULONG DestinationBufferLength OPTIONAL );
NTSYSAPI NTSTATUS NTAPI RtlQueryEnvironmentVariable_U(IN PVOID Environment OPTIONAL, IN PUNICODE_STRING VariableName, OUT PUNICODE_STRING VariableValue);
NTSYSAPI NTSTATUS NTAPI RtlSetEnvironmentVariable(IN OUT PVOID *Environment OPTIONAL, IN PUNICODE_STRING VariableName, IN PUNICODE_STRING VariableValue);

NTSYSCALLAPI NTSTATUS NTAPI NtContinue(__in PCONTEXT ContextRecord, __in BOOLEAN TestAlert);                              
NTSYSCALLAPI NTSTATUS NTAPI NtRaiseException(__in PEXCEPTION_RECORD ExceptionRecord, __in PCONTEXT ContextRecord, __in BOOLEAN FirstChance);
NTSYSAPI BOOLEAN NTAPI RtlDispatchException(__in PEXCEPTION_RECORD ExceptionRecord, __in PCONTEXT ContextRecord);                              
NTSYSAPI VOID NTAPI RtlRaiseException(__in PEXCEPTION_RECORD ExceptionRecord);              
NTSYSAPI VOID NTAPI RtlRaiseStatus(__in NTSTATUS Status);

NTSYSCALLAPI NTSTATUS NTAPI NtRaiseHardError(_In_ NTSTATUS 	ErrorStatus,
_In_ ULONG 	NumberOfParameters,
_In_ ULONG 	UnicodeStringParameterMask,
_In_ PULONG_PTR 	Parameters,
_In_ ULONG 	ValidResponseOptions,
_Out_ PULONG 	Response 
);

NTSTATUS NtCreateDirectoryObject(OUT PHANDLE DirectoryHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes);
  
VOID NTAPI RtlInitAnsiString(
  _Out_     PANSI_STRING DestinationString,
  _In_opt_  PCSZ SourceString
);

NTSTATUS NTAPI RtlInitAnsiStringEx
(
  PANSI_STRING target,
  PCSZ         source
);

VOID WINAPI RtlInitUnicodeString(
  _Inout_   PUNICODE_STRING DestinationString,
  _In_opt_  PCWSTR SourceString
);

NTSTATUS WINAPI RtlInitUnicodeStringEx(
  _Inout_   PUNICODE_STRING DestinationString,
  _In_opt_  PCWSTR SourceString
);

NTSTATUS NTAPI NtDeviceIoControlFile(
  IN HANDLE               FileHandle,
  IN HANDLE               Event OPTIONAL,
  IN PIO_APC_ROUTINE      ApcRoutine OPTIONAL,
  IN PVOID                ApcContext OPTIONAL,
  OUT PIO_STATUS_BLOCK    IoStatusBlock,
  IN ULONG                IoControlCode,
  IN PVOID                InputBuffer OPTIONAL,
  IN ULONG                InputBufferLength,
  OUT PVOID               OutputBuffer OPTIONAL,
  IN ULONG                OutputBufferLength );
  
#endif /* _NTDLL_H */
