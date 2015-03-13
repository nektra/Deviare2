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
using Nektra.Deviare2;

//IMPORTANT NOTES:
//---------------
//
//1) Do NOT put the class inside a namespace.
//
//2) Regardless of the functionallity of the plugin, the class name must be always "DeviarePlugin".
//
//3) Code inside methods should try/catch exceptions to avoid possible crashes in hooked application.
//
//4) Methods that returns an int value should return 0 if success or an HRESULT error value.
//
//   4.1) If a method returns a value less than zero, all hooks will be removed and agent will unload
//        from the process.
//
//   4.2) The recommended way to handle errors is to let the SpyMgr to decide what to do. For e.g. if
//        you hit an error in OnFunctionCall, probably, some custom parameter will not be added to the
//        CustomParams() collection. So, when in your app received the DNktSpyMgrEvents::OnFunctionCall
//        event, you will find the parameters is missing and at this point you can choose what to do.

public class DeviarePlugin
{
    public DeviarePlugin()
    {
        return;
    }

    //called when the plugin is loaded
    public int OnLoad()
    {
        System.Diagnostics.Trace.WriteLine("MyRegistryPlugin::OnLoad called");
        return 0;
    }

    //called when the plugin is unloaded
    public void OnUnload()
    {
        System.Diagnostics.Trace.WriteLine("MyRegistryPlugin::OnUnload called");
        return;
    }

    //called when a hook is attached to this plugin
    public int OnHookAdded(INktHookInfo hookInfo, int chainIndex, string parameters)
    {
        System.Diagnostics.Trace.WriteLine("MyRegistryPlugin::OnHookAdded called [Hook: " + hookInfo.FunctionName + " @ 0x" + hookInfo.Address.ToString("X") + " / Chain:" + chainIndex.ToString() + "]");
        return 0;
    }

    //called when a hook is detached from this plugin
    public int OnHookRemoved(INktHookInfo hookInfo, int chainIndex)
    {
        System.Diagnostics.Trace.WriteLine("MyRegistryPlugin::OnHookAdded called [Hook: " + hookInfo.FunctionName + " @ 0x" + hookInfo.Address.ToString("X") + " / Chain:" + chainIndex.ToString() + "]");
        return 0;
    }

    //called when a hooked function is called
    public int OnFunctionCall(INktHookInfo hookInfo, int chainIndex, INktHookCallInfoPlugin callInfo)
    {
        System.Diagnostics.Trace.WriteLine("MyRegistryPlugin::OnFunctionCall called [Hook: " + hookInfo.FunctionName + " @ 0x" + hookInfo.Address.ToString("X") + " / Chain:" + chainIndex.ToString() + "]");
        INktParamsEnum pms;

        callInfo.AddString("sample name", "HKEY extractor sample");
        pms = callInfo.Params();
        for (int i = 0; i < pms.Count; i++)
        {
            INktParam p = pms.GetAt(i);
            if (p.IsPointer)
                p = p.Evaluate();
            if (p != null && p.TypeName == "HKEY")
            {
                callInfo.AddSizeT("param#" + i.ToString(), p.SizeTVal);
            }
        }
        return 0;
    }

    /*
     * The following functions are optional.
     * 
     * If GetFunctionCallbackName is defined, it will be called and you must return a string containing
     * the name of the function that will be called instead of OnFunctionCall.
     * 
     * In this example we return "MyFuncion" and you can see the definition of MyFuncion below.
     * MyFuncion's parameters are the same than OnFunctionCall.
     * 
     * 
     * 
     * public string GetFunctionCallbackName(INktHookInfo hookInfo, int chainIndex)
     * {
     *      System.Diagnostics.Trace.WriteLine("MyRegistryPlugin::GetFunctionCallbackName called [Hook: " + hookInfo.FunctionName + " @ 0x" + hookInfo.Address.ToString("X") + " / Chain:" + chainIndex.ToString() + "]");
     *      return "MyFuncion";
     * }
     
     * public int MyFuncion(INktHookInfo hookInfo, int chainIndex, INktHookCallInfoPlugin callInfo)
     * {
     *     callInfo.AddString("sample name 2", "HKEY extractor sample 2");
     *     return 0;
     * }
     * 
     * */
}
