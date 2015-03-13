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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Nektra.Deviare2;

namespace PrintLogger
{
    public partial class PrintLogger : Form
    {
        private NktSpyMgr _spyMgr;
        private NktProcess _process;

        public PrintLogger()
        {
            InitializeComponent();

            _spyMgr = new NktSpyMgr();
            _spyMgr.Initialize();
            _spyMgr.OnFunctionCalled += new DNktSpyMgrEvents_OnFunctionCalledEventHandler(OnFunctionCalled);

            GetProcess("spoolsv.exe");
            if (_process == null)
            {
                MessageBox.Show("Please start \"spoolsv.exe\" before!", "Error");
                Environment.Exit(0);
            }
        }

        private void PrintLogger_Load(object sender, EventArgs e)
        {
            NktHook hook = _spyMgr.CreateHook("spoolsv.exe!PrvStartDocPrinterW", (int)(eNktHookFlags.flgRestrictAutoHookToSameExecutable | eNktHookFlags.flgOnlyPreCall));
            hook.Hook(true);
            hook.Attach(_process, true);
        }

        private bool GetProcess(string proccessName)
        {
            NktProcessesEnum enumProcess = _spyMgr.Processes();
            NktProcess tempProcess = enumProcess.First();
            while (tempProcess != null)
            {
                if (tempProcess.Name.Equals(proccessName, StringComparison.InvariantCultureIgnoreCase) && tempProcess.PlatformBits > 0 && tempProcess.PlatformBits <= IntPtr.Size * 8)
                {
                    _process = tempProcess;
                    return true;
                }
                tempProcess = enumProcess.Next();
            }

            _process = null;
            return false;
        }

        private void OnFunctionCalled(NktHook hook, NktProcess process, NktHookCallInfo hookCallInfo)
        {
            string strDocument = "Document: ";

            INktParamsEnum paramsEnum = hookCallInfo.Params();

            INktParam param = paramsEnum.First();

            param = paramsEnum.Next();

            param = paramsEnum.Next();
            if (param.PointerVal != IntPtr.Zero)
            {
                INktParamsEnum paramsEnumStruct = param.Evaluate().Fields();
                INktParam paramStruct = paramsEnumStruct.First();

                strDocument += paramStruct.ReadString();
                strDocument += "\n";
            }

            Output(strDocument);
        }

        public delegate void OutputDelegate(string strOutput);

        private void Output(string strOutput)
        {
            if (InvokeRequired)
                BeginInvoke(new OutputDelegate(Output), strOutput);
            else
                textOutput.AppendText(strOutput);
        }
    }
}
