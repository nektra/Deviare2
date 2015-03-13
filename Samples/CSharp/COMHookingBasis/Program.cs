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

using System;
using System.Collections.Generic;
using Nektra.Deviare2;
using System.Diagnostics;

namespace COMHookingBasis
{
    class Program
    {
        static NktSpyMgr spyMgr;
        static NktHook hookDllGetClassObj = null;
        static NktHook hookIClassFactory_CreateInstance = null;
        static NktHook hookIClassFactory2_CreateInstance = null;
        static NktHook hookIClassFactory2_CreateInstanceLic = null;
        static Object hookLock = new Object();

        static void Main(string[] args)
        {
            Int32 notepadPID;

            spyMgr = new NktSpyMgr();
            if (spyMgr.Initialize() < 0)
            {
                Console.WriteLine("Cannot initialize Deviare");
                return;
            }

            notepadPID = LaunchNotepadAndGetPid();
            if (notepadPID == 0)
            {
                Console.WriteLine("Cannot launch notepad.");
                return;
            }

            //in first place, hook DllGetClassObject of the target dll/ocx
            hookDllGetClassObj = spyMgr.CreateHook("shell32.dll!DllGetClassObject", (int)eNktHookFlags.flgOnlyPostCall);
            hookDllGetClassObj.Attach(notepadPID, true);
            hookDllGetClassObj.Hook(true);
            hookDllGetClassObj.OnFunctionCalled += OnDllGetClassObjectCalled;

            Console.WriteLine("Press ESCAPE key to quit...");
            while (Console.ReadKey(true).KeyChar != 27);
        }

        private static Int32 LaunchNotepadAndGetPid()
        {
            Process proc;
            Int32 pid = 0;

            proc = new Process();
            proc.StartInfo.FileName = "notepad.exe";
            if (proc.Start() != false)
            {
                proc.WaitForInputIdle();
                try
                {
                    pid = proc.Id;
                }
                catch (Exception)
                {
                    pid = 0;
                }
            }
            return pid;
        }

        static void OnDllGetClassObjectCalled(NktHook hook, NktProcess proc, NktHookCallInfo callInfo)
        {
            if ((callInfo.Result().ULongVal & 0x80000000) == 0)
            {
                NktParamsEnum pms;
                IntPtr addr;
                string s;

                //if the call succeeded, check if we are creating a class factory that belongs
                //to the CLSID we need, in our example, "ShellFolderView coclass"
                pms = callInfo.Params();
                if (pms.GetAt(0).GuidString == "{62112AA1-EBE4-11CF-A5FB-0020AFE7292D}")
                {
                    s = pms.GetAt(1).GuidString;
                    if (s == "{00000001-0000-0000-C000-000000000046}")
                    {
                        //we have ShellFolderView's IClassFactory object
                        if (hookIClassFactory_CreateInstance == null)
                        {
                            lock (hookLock)
                            {
                                if (hookIClassFactory_CreateInstance == null)
                                {
                                    //get the address of the newly created object
                                    addr = pms.GetAt(2).Evaluate().PointerVal;
                                    //get object's vtable address by inspecting the first pointer
                                    addr = proc.Memory().get_SSizeTVal(addr);
                                    //because the CreateInstance method is the fourth one,
                                    //get the method entrypoint by reading memory
                                    addr = (IntPtr)(addr.ToInt64() + 3 * IntPtr.Size);
                                    addr = proc.Memory().get_SSizeTVal(addr);
                                    hookIClassFactory_CreateInstance = spyMgr.CreateHookForAddress(addr, "IClassFactory::CreateInstance", (int)eNktHookFlags.flgOnlyPostCall | (int)eNktHookFlags.flgDontCheckAddress);
                                    hookIClassFactory_CreateInstance.Attach(proc.Id, true);
                                    hookIClassFactory_CreateInstance.Hook(true);
                                    hookIClassFactory_CreateInstance.OnFunctionCalled += OnIClassFactoryCreateInstanceCalled;
                                }
                            }
                        }
                    }
                    if (s == "{B196B28F-BAB4-101A-B69C-00AA00341D07}")
                    {
                        //we have ShellFolderView's IClassFactory2 object
                        if (hookIClassFactory2_CreateInstance == null)
                        {
                            lock (hookLock)
                            {
                                if (hookIClassFactory2_CreateInstance == null)
                                {
                                    //get the address of the newly created object
                                    addr = pms.GetAt(2).Evaluate().PointerVal;
                                    //get object's vtable address by inspecting the first pointer
                                    addr = proc.Memory().get_SSizeTVal(addr);
                                    //because the CreateInstance method is the fourth one,
                                    //get the method entrypoint by reading memory
                                    addr = (IntPtr)(addr.ToInt64() + 3 * IntPtr.Size);
                                    addr = proc.Memory().get_SSizeTVal(addr);
                                    hookIClassFactory2_CreateInstance = spyMgr.CreateHookForAddress(addr, "IClassFactory2::CreateInstance", (int)eNktHookFlags.flgOnlyPostCall);
                                    hookIClassFactory2_CreateInstance.Attach(proc.Id, true);
                                    hookIClassFactory2_CreateInstance.Hook(true);
                                    hookIClassFactory2_CreateInstance.OnFunctionCalled += OnIClassFactory2CreateInstanceCalled;
                                }

                                if (hookIClassFactory2_CreateInstanceLic == null)
                                {
                                    //get the address of the newly created object
                                    addr = pms.GetAt(2).Evaluate().PointerVal;
                                    //get object's vtable address by inspecting the first pointer
                                    addr = proc.Memory().get_SSizeTVal(addr);
                                    //because the CreateInstanceLic method is the eighth one,
                                    //get the method entrypoint by reading memory
                                    addr = (IntPtr)(addr.ToInt64() + 7 * IntPtr.Size);
                                    addr = proc.Memory().get_SSizeTVal(addr);
                                    hookIClassFactory2_CreateInstanceLic = spyMgr.CreateHookForAddress(addr, "IClassFactory2::CreateInstanceLic", (int)eNktHookFlags.flgOnlyPostCall);
                                    hookIClassFactory2_CreateInstanceLic.Attach(proc.Id, true);
                                    hookIClassFactory2_CreateInstanceLic.Hook(true);
                                    hookIClassFactory2_CreateInstanceLic.OnFunctionCalled += OnIClassFactory2CreateInstanceLicCalled;
                                }
                            }
                        }
                    }
                }
            }
            return;
        }

        static void OnIClassFactoryCreateInstanceCalled(NktHook hook, NktProcess proc, NktHookCallInfo callInfo)
        {
            if ((callInfo.Result().ULongVal & 0x80000000) == 0)
            {
                NktParamsEnum pms;
                IntPtr addr;
                string s;

                //if the call succeeded, check if we are creating a instance that belongs
                //to the IID we need, in our example, "IShellFolderViewDual"
                pms = callInfo.Params();
                //remember that the first parameter is the interface pointer itself
                s = pms.GetAt(2).GuidString;
                if (s == "{E7A1AF80-4D96-11CF-960C-0080C7F4EE85}" ||
                    s == "{31C147B6-0ADE-4A3C-B514-DDF932EF6D17}" ||
                    s == "{88A05C00-F000-11CE-8350-444553540000}")
                {
                    //at this point we have to apply a similar code than we used to hook
                    //IClassFactory::CreateInstance above and the other methods
                    addr = pms.GetAt(3).Evaluate().PointerVal;
                    //get object's vtable address by inspecting the first pointer
                    addr = proc.Memory().get_SSizeTVal(addr);
                    //because the CreateInstance method is the fourth one,
                    //get the method entrypoint by reading memory
                    addr = (IntPtr)(addr.ToInt64() + 3 * IntPtr.Size);
                    addr = proc.Memory().get_SSizeTVal(addr);
                    /*
                    .
                    .
                    .
                    */
                }
            }
        }

        static void OnIClassFactory2CreateInstanceCalled(NktHook hook, NktProcess proc, NktHookCallInfo callInfo)
        {
            //because interface instances can be created by IClassFactory2 too, may you have
            //to define a similar code than above
        }

        static void OnIClassFactory2CreateInstanceLicCalled(NktHook hook, NktProcess proc, NktHookCallInfo callInfo)
        {
            //because interface instances can be created by IClassFactory2 too, may you have
            //to define a similar code than above
        }
    }
}
