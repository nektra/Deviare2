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

using System;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;

namespace DeviareLiteInterop
{
    public sealed class HookLib
    {
        #region Public Enums
        [Flags]
        public enum HookFlags : int
        {
            DontSkipInitialJumps     = 0x01,
            DontRemoveOnUnhook       = 0x02,
            DontSkipAnyJumps         = 0x04,
            SkipNullProcsToHook      = 0x08,
            UseAbsoluteIndirectJumps = 0x10,
            AllowReentrancy          = 0x20,
            DontEnableHooks          = 0x40
        }

        [Flags]
        public enum ProcessCreationFlags : uint
        {
            NONE = 0x00000000,
            DEBUG_PROCESS = 0x00000001,
            DEBUG_ONLY_THIS_PROCESS = 0x00000002,
            CREATE_SUSPENDED = 0x00000004,
            DETACHED_PROCESS = 0x00000008,
            CREATE_NEW_CONSOLE = 0x00000010,
            CREATE_NEW_PROCESS_GROUP = 0x00000200,
            CREATE_UNICODE_ENVIRONMENT = 0x00000400,

            CREATE_PROTECTED_PROCESS = 0x00040000,
            CREATE_BREAKAWAY_FROM_JOB = 0x01000000,
            CREATE_PRESERVE_CODE_AUTHZ_LEVEL = 0x02000000,
            CREATE_DEFAULT_ERROR_MODE = 0x04000000,
            CREATE_NO_WINDOW = 0x08000000,
            CREATE_SEPARATE_WOW_VDM = 0x00001000,
            CREATE_SHARED_WOW_VDM = 0x00001000,
            EXTENDED_STARTUPINFO_PRESENT = 0x00080000,
            INHERIT_PARENT_AFFINITY = 0x00010000
        }
        #endregion

        #region Public Structs
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct STARTUPINFO
        {
            public Int32 cb;
            public string lpReserved;
            public string lpDesktop;
            public string lpTitle;
            public Int32 dwX;
            public Int32 dwY;
            public Int32 dwXSize;
            public Int32 dwYSize;
            public Int32 dwXCountChars;
            public Int32 dwYCountChars;
            public Int32 dwFillAttribute;
            public Int32 dwFlags;
            public Int16 wShowWindow;
            public Int16 cbReserved2;
            public IntPtr lpReserved2;
            public IntPtr hStdInput;
            public IntPtr hStdOutput;
            public IntPtr hStdError;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SECURITY_ATTRIBUTES
        {
            public int length;
            public IntPtr lpSecurityDescriptor;
            public int inheritHandle;
        }

        public struct ProcessInfo
        {
            public SafeWaitHandle procHandle;
            public SafeWaitHandle threadHandle;
            public int procId;
            public int threadId;
        }
        #endregion

        public HookLib()
        {
            Type hookLibType = Type.GetTypeFromCLSID(new Guid("CC78D151-6AA0-42d9-B9F1-C434CFAE695E"));
            this.hookLib = Activator.CreateInstance(hookLibType);
            this.hookInfoType = Type.GetTypeFromCLSID(new Guid("74A66D3C-D6D5-4991-90FC-59F312BCD624"));

            this.DotNetInit();
        }

        public object Hook(IntPtr origAddr, IntPtr newAddr)
        {
            return this.Hook(origAddr, newAddr, 0);
        }

        public object Hook(IntPtr origAddr, IntPtr newAddr, int flags)
        {
            Object hookInfo = Activator.CreateInstance(hookInfoType);

            this.SetProperty(hookInfo, "OrigProcAddr", IntPtr2Obj(origAddr));
            this.SetProperty(hookInfo, "NewProcAddr", IntPtr2Obj(newAddr));
            this.Invoke(hookLib, "Hook", new object[] { hookInfo, flags });
            return hookInfo;
        }

        public object Hook(Type origClassType, string origMethodName, Type[] origParams,
                           Type newClassType, string newMethodName, Type[] newParams)
        {
            return this.Hook(origClassType, origMethodName, origParams, newClassType, newMethodName, newParams, 0);
        }

        public object Hook(Type origClassType, string origMethodName, Type[] origParams,
                           Type newClassType, string newMethodName, Type[] newParams, int flags)
        {
            IntPtr origAddr = this.GetMethodAddress(origClassType, origMethodName, origParams);
            IntPtr newAddr = this.GetMethodAddress(newClassType, newMethodName, newParams);
            return this.Hook(origAddr, newAddr, flags);
        }

        public object RemoteHook(IntPtr origAddr, IntPtr newAddr, int pid)
        {
            return this.RemoteHook(origAddr, newAddr, pid, 0);
        }

        public object RemoteHook(IntPtr origAddr, IntPtr newAddr, int pid, int flags)
        {
            Object hookInfo = Activator.CreateInstance(hookInfoType);

            this.SetProperty(hookInfo, "OrigProcAddr", IntPtr2Obj(origAddr));
            this.SetProperty(hookInfo, "NewProcAddr", IntPtr2Obj(newAddr));
            this.Invoke(this.hookLib, "RemoteHook", new object[] { hookInfo, pid, flags });
            return hookInfo;
        }

        public void Unhook(object o)
        {
            this.Invoke(this.hookLib, "Unhook", new object[] { o });
        }

        public void UnhookProcess(int pid)
        {
            this.Invoke(this.hookLib, "UnhookProcess", new object[] { pid });
        }

        public void UnhookAll()
        {
            this.Invoke(this.hookLib, "UnhookAll");
        }

        public void EnableHook(object o, bool enable)
        {
            int e = (enable) ? -1 : 0;
            this.Invoke(this.hookLib, "EnableHook", new object[] { o, e });
        }

        public void RemoveHook(object o, bool disable)
        {
            int d = (disable) ? -1 : 0;
            this.Invoke(this.hookLib, "RemoveHook", new object[] { o, d });
        }

        public bool SuspendThreadsWhileHooking
        {
            get {
                int e = (int)GetProperty(this.hookLib, "SuspendThreadsWhileHooking");
                return (e != 0) ? true : false;
            }

            set
            {
                int e = (value) ? -1 : 0;
                this.SetProperty(this.hookLib, "SuspendThreadsWhileHooking", e);
            }
        }

        public bool ShowDebugOutput
        {
            get {
                int e = (int)GetProperty(this.hookLib, "ShowDebugOutput");
                return (e != 0) ? true : false;
            }

            set
            {
                int e = (value) ? -1 : 0;
                this.SetProperty(this.hookLib, "ShowDebugOutput", e);
            }
        }

        public IntPtr GetModuleBaseAddress(string moduleName)
        {
            object o = this.Invoke(this.hookLib, "GetModuleBaseAddress", new object[] { moduleName });
            return Obj2IntPtr(o);
        }

        public IntPtr GetRemoteModuleBaseAddress(int pid, string moduleName, bool scanMappedImages)
        {
            int scanMI = (scanMappedImages) ? -1 : 0;
            object o = this.Invoke(this.hookLib, "GetRemoteModuleBaseAddress",
                                   new object[] { pid, moduleName, scanMI });
            return Obj2IntPtr(o);
        }

        public IntPtr GetProcedureAddress(IntPtr moduleBaseAddress, string procName)
        {
            object o = this.Invoke(this.hookLib, "GetProcedureAddress",
                                   new object[] { IntPtr2Obj(moduleBaseAddress), procName });
            return Obj2IntPtr(o);
        }

        public IntPtr GetRemoteProcedureAddress(int pid, IntPtr moduleBaseAddress, string procName)
        {
            object o = this.Invoke(this.hookLib, "GetRemoteProcedureAddress",
                                   new object[] { pid, IntPtr2Obj(moduleBaseAddress), procName });
            return Obj2IntPtr(o);
        }

        public ProcessInfo CreateProcess(string applicationName, string commandLine,
                                         Nullable<SECURITY_ATTRIBUTES> processAttributes,
                                         Nullable<SECURITY_ATTRIBUTES> threadAttributes,
                                         bool inheritHandles, ProcessCreationFlags creationFlags,
                                         string environment, string currentDirectory,
                                         Nullable<STARTUPINFO> startupInfo)
        {
            PROCESS_INFORMATION pi = new PROCESS_INFORMATION();
            IntPtr procAttr = IntPtr.Zero;
            IntPtr threadAttr = IntPtr.Zero;
            IntPtr stInfo = IntPtr.Zero;

            if (processAttributes.HasValue)
            {
                procAttr = Marshal.AllocHGlobal(Marshal.SizeOf(processAttributes.Value));
                Marshal.StructureToPtr(processAttributes.Value, procAttr, false);
            }
            if (threadAttributes.HasValue)
            {
                threadAttr = Marshal.AllocHGlobal(Marshal.SizeOf(threadAttributes.Value));
                Marshal.StructureToPtr(threadAttributes.Value, threadAttr, false);
            }
            if (startupInfo.HasValue)
            {
                stInfo = Marshal.AllocHGlobal(Marshal.SizeOf(startupInfo.Value));
                Marshal.StructureToPtr(startupInfo.Value, stInfo, false);
            }
            if (NativeCreateProcess(applicationName, commandLine, procAttr, threadAttr, inheritHandles,
                                    (uint)creationFlags, environment, currentDirectory, stInfo, out pi) == false)
            {
                Marshal.ThrowExceptionForHR(Marshal.GetHRForLastWin32Error());
            }

            ProcessInfo _pi = new ProcessInfo();
            _pi.procHandle = new SafeWaitHandle(pi.procHandle, true);
            _pi.threadHandle = new SafeWaitHandle(pi.threadHandle, true);
            _pi.procId = pi.procId;
            _pi.threadId = pi.threadId;
            return _pi;
        }

        public void ResumeThread(SafeWaitHandle threadHandle)
        {
            ResumeThread(threadHandle.DangerousGetHandle());
        }

        public void ResumeThread(IntPtr threadHandle)
        {
            NativeResumeThread(threadHandle);
        }

        public ProcessInfo CreateProcessWithDll(string applicationName, string commandLine,
                                                Nullable<SECURITY_ATTRIBUTES> processAttributes,
                                                Nullable<SECURITY_ATTRIBUTES> threadAttributes,
                                                bool inheritHandles, ProcessCreationFlags creationFlags,
                                                string environment, string currentDirectory,
                                                Nullable<STARTUPINFO> startupInfo, string dllName,
                                                IntPtr signalCompletedEvent, string initFunctionName)
        {
            ProcessInfo pi = new ProcessInfo();
            IntPtr procAttr = IntPtr.Zero;
            IntPtr threadAttr = IntPtr.Zero;
            IntPtr stInfo = IntPtr.Zero;
            int ih = (inheritHandles) ? -1 : 0;

            if (processAttributes.HasValue)
            {
                procAttr = Marshal.AllocHGlobal(Marshal.SizeOf(processAttributes.Value));
                Marshal.StructureToPtr(processAttributes.Value, procAttr, false);
            }
            if (threadAttributes.HasValue)
            {
                threadAttr = Marshal.AllocHGlobal(Marshal.SizeOf(threadAttributes.Value));
                Marshal.StructureToPtr(threadAttributes.Value, threadAttr, false);
            }
            if (startupInfo.HasValue)
            {
                stInfo = Marshal.AllocHGlobal(Marshal.SizeOf(startupInfo.Value));
                Marshal.StructureToPtr(startupInfo.Value, stInfo, false);
            }
            if (applicationName == null)
                applicationName = "";
            if (commandLine == null)
                commandLine = "";
            if (environment == null)
                environment = "";
            if (currentDirectory == null)
                currentDirectory = "";
            try
            {
                object o = this.Invoke(this.hookLib, "CreateProcessWithDll",
                                       new object[] { applicationName, commandLine, IntPtr2Obj(procAttr),
                                                      IntPtr2Obj(threadAttr), ih, creationFlags, environment,
                                                      currentDirectory, IntPtr2Obj(stInfo), dllName,
                                                      IntPtr2Obj(signalCompletedEvent), initFunctionName });
                pi.procHandle = DuplicateAndConvertToSafeWaitHandle(Obj2IntPtr(this.GetProperty(o, "ProcessHandle")));
                pi.threadHandle = DuplicateAndConvertToSafeWaitHandle(Obj2IntPtr(this.GetProperty(o, "ThreadHandle")));
                pi.procId = (int)(this.GetProperty(o, "ProcessId"));
                pi.threadId = (int)(this.GetProperty(o, "ThreadId"));
            }
            catch (Exception)
            {
                throw;
            }
            finally
            {
                if (stInfo != IntPtr.Zero)
                    Marshal.FreeHGlobal(stInfo);
                if (threadAttr != IntPtr.Zero)
                    Marshal.FreeHGlobal(threadAttr);
                if (procAttr != IntPtr.Zero)
                    Marshal.FreeHGlobal(procAttr);
            }
            return pi;
        }

        public ProcessInfo CreateProcessWithLogonAndDll(string userName, string domain, string password, int logonFlags,
                                                        string applicationName, string commandLine,
                                                        ProcessCreationFlags creationFlags, string environment,
                                                        string currentDirectory, Nullable<STARTUPINFO> startupInfo,
                                                        string dllName, IntPtr signalCompletedEvent,
                                                        string initFunctionName)
        {
            ProcessInfo pi = new ProcessInfo();
            IntPtr stInfo = IntPtr.Zero;

            if (startupInfo.HasValue)
            {
                stInfo = Marshal.AllocHGlobal(Marshal.SizeOf(startupInfo.Value));
                Marshal.StructureToPtr(startupInfo.Value, stInfo, false);
            }
            if (userName == null)
                userName = "";
            if (domain == null)
                domain = "";
            if (password == null)
                password = "";
            if (applicationName == null)
                applicationName = "";
            if (commandLine == null)
                commandLine = "";
            if (environment == null)
                environment = "";
            if (currentDirectory == null)
                currentDirectory = "";
            try
            {
                object o = this.Invoke(this.hookLib, "CreateProcessWithLogonAndDll",
                                       new object[] { userName, domain, password, logonFlags, applicationName,
                                                      commandLine, creationFlags, environment, currentDirectory,
                                                      IntPtr2Obj(stInfo), dllName, IntPtr2Obj(signalCompletedEvent),
                                                      initFunctionName });
                pi.procHandle = DuplicateAndConvertToSafeWaitHandle(Obj2IntPtr(this.GetProperty(o, "ProcessHandle")));
                pi.threadHandle = DuplicateAndConvertToSafeWaitHandle(Obj2IntPtr(this.GetProperty(o, "ThreadHandle")));
                pi.procId = (int)(this.GetProperty(o, "ProcessId"));
                pi.threadId = (int)(this.GetProperty(o, "ThreadId"));
            }
            catch (Exception)
            {
                throw;
            }
            finally
            {
                if (stInfo != IntPtr.Zero)
                    Marshal.FreeHGlobal(stInfo);
            }
            return pi;
        }

        public ProcessInfo CreateProcessWithTokenAndDll(IntPtr token, int logonFlags, string applicationName,
                                                        string commandLine, ProcessCreationFlags creationFlags,
                                                        string environment, string currentDirectory,
                                                        Nullable<STARTUPINFO> startupInfo, string dllName,
                                                        IntPtr signalCompletedEvent, string initFunctionName)
        {
            ProcessInfo pi = new ProcessInfo();
            IntPtr stInfo = IntPtr.Zero;

            if (startupInfo.HasValue)
            {
                stInfo = Marshal.AllocHGlobal(Marshal.SizeOf(startupInfo.Value));
                Marshal.StructureToPtr(startupInfo.Value, stInfo, false);
            }
            if (applicationName == null)
                applicationName = "";
            if (commandLine == null)
                commandLine = "";
            if (environment == null)
                environment = "";
            if (currentDirectory == null)
                currentDirectory = "";
            try
            {
                object o = this.Invoke(this.hookLib, "CreateProcessWithTokenAndDll",
                                       new object[] { IntPtr2Obj(token), logonFlags, applicationName, commandLine,
                                                      creationFlags, environment, currentDirectory, IntPtr2Obj(stInfo),
                                                      dllName, IntPtr2Obj(signalCompletedEvent), initFunctionName });
                pi.procHandle = DuplicateAndConvertToSafeWaitHandle(Obj2IntPtr(this.GetProperty(o, "ProcessHandle")));
                pi.threadHandle = DuplicateAndConvertToSafeWaitHandle(Obj2IntPtr(this.GetProperty(o, "ThreadHandle")));
                pi.procId = (int)(this.GetProperty(o, "ProcessId"));
                pi.threadId = (int)(this.GetProperty(o, "ThreadId"));
            }
            catch (Exception)
            {
                throw;
            }
            finally
            {
                if (stInfo != IntPtr.Zero)
                    Marshal.FreeHGlobal(stInfo);
            }
            return pi;
        }

        public void InjectDll(int procId, string dllName, string initFunctionName, uint processInitWaitTimeoutMs )
        {
            IntPtr injectorThreadHandle;

            InjectDll(procId, dllName, initFunctionName, processInitWaitTimeoutMs, out injectorThreadHandle);
            if (injectorThreadHandle != IntPtr.Zero)
                NativeCloseHandle(injectorThreadHandle);
        }

        public void InjectDll(int procId, string dllName, string initFunctionName, uint processInitWaitTimeoutMs, out IntPtr injectorThreadHandle)
        {
            ParameterModifier argsMod = new ParameterModifier(5);
            argsMod[4] = true;
            object[] args = new object[] { procId, dllName, initFunctionName, (int)processInitWaitTimeoutMs, IntPtr2Obj(new IntPtr()) };
            this.Invoke(this.hookLib, "InjectDll", args, argsMod);
            injectorThreadHandle = Obj2IntPtr(args[4]);
        }

        public void InjectDllH(IntPtr procHandle, string dllName, string initFunctionName, uint processInitWaitTimeoutMs)
        {
            IntPtr injectorThreadHandle;

            InjectDllH(procHandle, dllName, initFunctionName, processInitWaitTimeoutMs, out injectorThreadHandle);
            if (injectorThreadHandle != IntPtr.Zero)
                NativeCloseHandle(injectorThreadHandle);
        }

        public void InjectDllH(IntPtr procHandle, string dllName, string initFunctionName, uint processInitWaitTimeoutMs, out IntPtr injectorThreadHandle)
        {
            ParameterModifier argsMod = new ParameterModifier(5);
            argsMod[4] = true;
            object[] args = new object[] { IntPtr2Obj(procHandle), dllName, initFunctionName, (int)processInitWaitTimeoutMs, IntPtr2Obj(new IntPtr()) };
            this.Invoke(this.hookLib, "InjectDllH", args, argsMod);
            injectorThreadHandle = Obj2IntPtr(args[4]);
        }

        public uint WaitForInjectorThread(IntPtr injectorThreadHandle, uint timeoutMs)
        {
            uint exitCode = uint.MaxValue;

            if (injectorThreadHandle != IntPtr.Zero)
            {
                switch (NativeWaitForSingleObject(injectorThreadHandle, timeoutMs))
                {
                    case 0:
                        NativeGetExitCodeThread(injectorThreadHandle, out exitCode);
                        break;
                    case 0x00000102: //timeout
                        exitCode = 1460; //ERROR_TIMEOUT
                        break;
                }
                NativeCloseHandle(injectorThreadHandle);
            }
            return exitCode;
        }

        #region Private Vars
        private Object hookLib;
        private Type hookInfoType;
        private IntPtr dummy;
        #endregion

        #region Private Structs
        [StructLayout(LayoutKind.Sequential)]
        public struct PROCESS_INFORMATION
        {
            public IntPtr procHandle;
            public IntPtr threadHandle;
            public int procId;
            public int threadId;
        }
        #endregion

        #region Internal Helpers
        private Object GetProperty(Object obj, string propName)
        {
            return obj.GetType().InvokeMember(propName, BindingFlags.GetProperty | BindingFlags.Instance |
                                                        BindingFlags.Public, null, obj, new object[] { });
        }

        private void SetProperty(Object obj, string propName, object propValue)
        {

            obj.GetType().InvokeMember(propName, BindingFlags.SetProperty | BindingFlags.Instance |
                                                 BindingFlags.Public,
                                       null, obj, new object[] { propValue });
        }

        private Object Invoke(Object obj, string methodName)
        {
            return this.Invoke(obj, methodName, new object[] { });
        }

        private Object Invoke(Object obj, string methodName, object[] parameters)
        {
            return obj.GetType().InvokeMember(methodName, BindingFlags.InvokeMethod | BindingFlags.Instance |
                                                          BindingFlags.Public,
                                              null, obj, parameters);
        }

        private Object Invoke(Object obj, string methodName, object[] parameters, ParameterModifier modifiers)
        {
            return obj.GetType().InvokeMember(methodName, BindingFlags.InvokeMethod | BindingFlags.Instance |
                                                          BindingFlags.Public,
                                              null, obj, parameters, new ParameterModifier[] { modifiers },
                                              null, null);
        }

        private IntPtr GetMethodAddress(Type classType, string methodName, Type[] parameters)
        {
            MethodInfo mi = classType.GetMethod(methodName, BindingFlags.Public | BindingFlags.NonPublic |
                                                            BindingFlags.Instance | BindingFlags.Static, null,
                                                CallingConventions.Any, parameters, null);
            if (mi == null)
                return IntPtr.Zero;
            System.Runtime.CompilerServices.RuntimeHelpers.PrepareMethod(mi.MethodHandle);
            return mi.MethodHandle.GetFunctionPointer();
        }

        [MethodImpl(MethodImplOptions.NoOptimization)]
        private void DotNetInit()
        {
            //NOTE: This will enforce a call to compileMethod in JIT compiler so DeviareLite.dll internal
            //      data can be initialized
            IntPtr mod = GetModuleBaseAddress("kernel32.dll");
            dummy = GetProcedureAddress(mod, "WaitForSingleObject");
            return;
        }

        private object IntPtr2Obj(IntPtr val)
        {
            if (IntPtr.Size == 4)
                return (object)(val.ToInt32());
            return (object)(val.ToInt64());
        }

        private IntPtr Obj2IntPtr(object o)
        {
            if (o is int)
                return new IntPtr((int)o);
            if (o is long)
                return new IntPtr((long)o);
            return IntPtr.Zero;
        }

        private SafeWaitHandle DuplicateAndConvertToSafeWaitHandle(IntPtr h)
        {
            IntPtr hDup;

            if (NativeDuplicateHandle(NativeGetCurrentProcess(), h, NativeGetCurrentProcess(), out hDup, 0,
                                      false, 2) == false)
            {
                Marshal.ThrowExceptionForHR(Marshal.GetHRForLastWin32Error());
            }
            return new SafeWaitHandle(hDup, true);
        }
        #endregion

        #region P/Invoke
        [DllImport("kernel32.dll", EntryPoint = "CreateProcess", SetLastError = true, CharSet = CharSet.Auto)]
        private static extern bool NativeCreateProcess(string lpApplicationName, string lpCommandLine,
                                                       IntPtr lpProcessAttributes, IntPtr lpThreadAttributes,
                                                       [MarshalAs(UnmanagedType.Bool)] bool bInheritHandles,
                                                       uint dwCreationFlags, string lpEnvironment,
                                                       string lpCurrentDirectory, IntPtr lpStartupInfo,
                                                       out PROCESS_INFORMATION lpProcessInformation);

        [DllImport("kernel32.dll", EntryPoint = "ResumeThread")]
        private static extern uint NativeResumeThread(IntPtr hThread);

        [DllImport("kernel32.dll", EntryPoint = "CloseHandle")]
        private static extern uint NativeCloseHandle(IntPtr hThread);

        [DllImport("kernel32.dll", EntryPoint = "DuplicateHandle", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool NativeDuplicateHandle(IntPtr hSourceProcessHandle, IntPtr hSourceHandle,
                                                         IntPtr hTargetProcessHandle, out IntPtr lpTargetHandle,
                                                         uint dwDesiredAccess,
                                                         [MarshalAs(UnmanagedType.Bool)] bool bInheritHandle,
                                                         uint dwOptions);

        [DllImport("kernel32.dll", EntryPoint = "GetCurrentProcess")]
        static extern IntPtr NativeGetCurrentProcess();

        [DllImport("kernel32.dll", EntryPoint = "GetExitCodeThread")]
        static extern bool NativeGetExitCodeThread(IntPtr hThread, out uint lpExitCode);

        [DllImport("kernel32.dll", EntryPoint = "WaitForSingleObject", SetLastError = true)]
        static extern UInt32 NativeWaitForSingleObject(IntPtr hHandle, UInt32 dwMilliseconds);
        #endregion
    }
}
