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
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Reflection;

namespace Test
{
    class Program
    {
        //static DeviareLiteInterop.HookLib cHook = new DeviareLiteInterop.HookLib(); //<<<---- DON'T USE THIS WAY. SEE BELOW
        static DeviareLiteInterop.HookLib cHook;

        //--------

        static Program()
        {
            //USE THIS METHOD. Clarification: .Net initializes constructors and fields upon demand and NOT in
            //                                the C/C++ way. Because this constructor runs "before" Main,
            //                                DeviareLite internal hooks are properly installed before the Main
            //                                method is compiled by the JIT.
            cHook = new DeviareLiteInterop.HookLib();
        }

        static void Main(string[] args)
        {
            TestStaticMethodHook();
            TestNonStaticMethodHook();
            TestApiHook();
        }

        //--------

        static void TestStaticMethodHook()
        {
            object o = cHook.Hook(typeof(MessageBox), "Show", new Type[] { typeof(string), typeof(string), typeof(MessageBoxButtons) },
                                 typeof(Program), "Hooked_MessageBoxShow", new Type[] { typeof(string), typeof(string), typeof(MessageBoxButtons) });
            MessageBox.Show("Static Method Hook\r\rThis should be hooked", "HookTest", MessageBoxButtons.OK);
            cHook.Unhook(o);

            MessageBox.Show("Static Method Hook\r\rThis should NOT be hooked", "HookTest", MessageBoxButtons.OK);
        }

        public static DialogResult Hooked_MessageBoxShow(string text, string caption, MessageBoxButtons buttons)
        {
            return MessageBox.Show(text, "HOOKED!!!", buttons);
        }

        //--------

        static void TestNonStaticMethodHook()
        {
            SampleClass cls = new SampleClass();

            object o = cHook.Hook(typeof(SampleClass), "Call", new Type[] { typeof(string), typeof(string) },
                                  typeof(MySampleClass), "Call", new Type[] { typeof(string), typeof(string) });
            cls.Call("Non-Static Method Hook\r\rThis should be hooked", "HookTest");
            cHook.Unhook(o);

            cls.Call("Non-Static Method Hook\r\rThis should NOT be hooked", "HookTest");

            //find the method we want to hook
            //replace with our method. REMEMBER THAT FUNCTION SIGNATURE MUST MATCH!!
        }

        class MySampleClass
        {
            public DialogResult Call(string text, string caption)
            {
                SampleClass _this = (object)this as SampleClass;

                return _this.Call(text, "HOOKED!!!");
            }
        }

        //--------

        static void TestApiHook()
        {
            IntPtr user32dll, msgBoxOrigProc;

            user32dll = cHook.GetModuleBaseAddress("user32.dll");
            msgBoxOrigProc = cHook.GetProcedureAddress(user32dll, "MessageBoxW");
            if (msgBoxOrigProc == IntPtr.Zero)
            {
                MessageBox.Show("Error: Cannot Cannot get address of MessageBoxW", "HookTest", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            object o = cHook.Hook(msgBoxOrigProc, Marshal.GetFunctionPointerForDelegate(new delegMessageBoxApi(Hooked_MessageBoxApi)));
            MessageBox.Show("Win32 Api Hook\r\rThis should be hooked", "HookTest", MessageBoxButtons.OK);
            cHook.Unhook(o);

            MessageBox.Show("Win32 Api Hook\r\rThis should NOT be hooked", "HookTest", MessageBoxButtons.OK);
        }

        [DllImport("user32.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Unicode, EntryPoint = "MessageBoxW")]
        public static extern uint MessageBoxApi(IntPtr hWnd, String text, String caption, int options);

        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode, SetLastError = true)]
        delegate uint delegMessageBoxApi(IntPtr hWnd, String text, String caption, int options);

        static uint Hooked_MessageBoxApi(IntPtr hWnd, String text, String caption, int options)
        {
            return MessageBoxApi(hWnd, text, "HOOKED!!!", options);
        }


    }
}
