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
using System.Runtime.InteropServices;
using System.Collections.Generic;
using Nektra.Deviare2;

//IMPORTANT NOTES:
//---------------
//
// 1) Do NOT put the class inside a namespace.
//
// 2) Regardless of the functionallity of the plugin, the class name must be always "DeviarePlugin".
//
// 3) Code inside methods should try/catch exceptions to avoid possible crashes in hooked application.
//
// 4) Methods that returns an int value should return 0 if success or an HRESULT error value.
//
//    4.1) If a method returns a value less than zero, all hooks will be removed and agent will unload
//         from the process.
//
//    4.2) The recommended way to handle errors is to let the SpyMgr to decide what to do. For e.g. if
//         you hit an error in OnFunctionCall, probably, some custom parameter will not be added to the
//         CustomParams() collection. So, when in your app received the DNktSpyMgrEvents::OnFunctionCall
//         event, you will find the parameters is missing and at this point you can choose what to do.
//
// 5) This example also shows the GetFunctionCallbackName function usage.

public class DeviarePlugin
{
    public Dictionary<IntPtr, string> handleMap;

    public DeviarePlugin()
    {
        handleMap = new Dictionary<IntPtr, string>();
        return;
    }

    //called when the plugin is loaded
    public int OnLoad()
    {
        System.Diagnostics.Trace.WriteLine("MyFilePlugin::OnLoad called");
        return 0;
    }

    //called when the plugin is unloaded
    public void OnUnload()
    {
        System.Diagnostics.Trace.WriteLine("MyFilePlugin::OnUnload called");
        return;
    }

    //called when a hook is attached to this plugin
    public int OnHookAdded(INktHookInfo hookInfo, int chainIndex, string parameters)
    {
        System.Diagnostics.Trace.WriteLine("MyFilePlugin::OnHookAdded called [Hook: " + hookInfo.FunctionName + " @ 0x" + hookInfo.Address.ToString("X") + " / Chain:" + chainIndex.ToString() + "]");
        return 0;
    }

    //called when a hook is detached from this plugin
    public int OnHookRemoved(INktHookInfo hookInfo, int chainIndex)
    {
        System.Diagnostics.Trace.WriteLine("MyFilePlugin::OnHookAdded called [Hook: " + hookInfo.FunctionName + " @ 0x" + hookInfo.Address.ToString("X") + " / Chain:" + chainIndex.ToString() + "]");
        return 0;
    }

    public string GetFunctionCallbackName(INktHookInfo hookInfo, int chainIndex)
    {
        switch (hookInfo.FunctionName.ToUpper())
        {
            case "KERNEL32.DLL!CREATEFILEW":
                return "OnCreateFileW";
            case "KERNEL32.DLL!READFILE":
                return "OnReadFile";
            case "KERNEL32.DLL!WRITEFILE":
                return "OnWriteFile";
            case "KERNEL32.DLL!CLOSEHANDLE":
                return "OnCloseHandle";
        }
        return "";
    }

    //called when a hooked function is called
    public int OnFunctionCall(INktHookInfo hookInfo, int chainIndex, INktHookCallInfoPlugin callInfo)
    {
        System.Diagnostics.Trace.WriteLine("MyFilePlugin::OnFunctionCall called [Hook: " + hookInfo.FunctionName + " @ 0x" + hookInfo.Address.ToString("X") + " / Chain:" + chainIndex.ToString() + "]");
        return 0;
    }

    public int OnCreateFileW(INktHookInfo hookInfo, int chainIndex, INktHookCallInfoPlugin callInfo)
    {
        System.Diagnostics.Trace.WriteLine("MyFilePlugin::OnCreateFileW called [Hook: " + hookInfo.FunctionName + " @ 0x" + hookInfo.Address.ToString("X") + " / Chain:" + chainIndex.ToString() + "]");
        try
        {
            if (callInfo.IsPreCall == false)
            {
                IntPtr fileHandle = callInfo.Result().SizeTVal;
                System.Diagnostics.Trace.WriteLine("MyFilePlugin::OnFunctionCall OnCreateFileW " + fileHandle.ToString());
                if (fileHandle != (IntPtr)(-1) && fileHandle != IntPtr.Zero)
                {
                    string fileName = callInfo.Params().GetAt(0).ReadString();
                    System.Diagnostics.Trace.WriteLine("MyFilePlugin::OnFunctionCall OnCreateFileW " + fileName);
                    if (fileName.Length > 0)
                    {
                        lock (handleMap)
                        {
                            handleMap[fileHandle] = fileName;
                        }
                    }
                }
            }
        }
        catch (System.Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex.ToString());
        }
        return 0;
    }

    public int OnReadFile(INktHookInfo hookInfo, int chainIndex, INktHookCallInfoPlugin callInfo)
    {
        System.Diagnostics.Trace.WriteLine("MyFilePlugin::OnReadFile called [Hook: " + hookInfo.FunctionName + " @ 0x" + hookInfo.Address.ToString("X") + " / Chain:" + chainIndex.ToString() + "]");
        try
        {
            if (callInfo.IsPreCall != false)
            {
                IntPtr fileHandle = callInfo.Params().GetAt(0).SizeTVal;
                string s;

                lock (handleMap)
                {
                    if (handleMap.TryGetValue(fileHandle, out s) != false)
                    {
                        callInfo.AddString("ReadFile", s);
                    }
                }
            }
        }
        catch (System.Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex.ToString());
        }
        return 0;
    }

    public int OnWriteFile(INktHookInfo hookInfo, int chainIndex, INktHookCallInfoPlugin callInfo)
    {
        System.Diagnostics.Trace.WriteLine("MyFilePlugin::OnWriteFile called [Hook: " + hookInfo.FunctionName + " @ 0x" + hookInfo.Address.ToString("X") + " / Chain:" + chainIndex.ToString() + "]");
        try
        {
            if (callInfo.IsPreCall != false)
            {
                IntPtr fileHandle = callInfo.Params().GetAt(0).SizeTVal;
                string s;

                lock (handleMap)
                {
                    if (handleMap.TryGetValue(fileHandle, out s) != false)
                    {
                        callInfo.AddString("WriteFile", s);
                    }
                }
            }
        }
        catch (System.Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex.ToString());
        }
        return 0;
    }

    public int OnCloseHandle(INktHookInfo hookInfo, int chainIndex, INktHookCallInfoPlugin callInfo)
    {
        System.Diagnostics.Trace.WriteLine("MyFilePlugin::OnCloseHandle called [Hook: " + hookInfo.FunctionName + " @ 0x" + hookInfo.Address.ToString("X") + " / Chain:" + chainIndex.ToString() + "]");
        try
        {
            if (callInfo.IsPreCall != false)
            {
                IntPtr fileHandle = callInfo.Params().GetAt(0).SizeTVal;
                lock (handleMap)
                {
                    handleMap.Remove(fileHandle);
                }
            }
        }
        catch (System.Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex.ToString());
        }
        return 0;
    }
}
