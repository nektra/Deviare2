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
using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.Process;
using CSharpConsole.Presenters;

namespace CSharpConsole.Model.Modules
{
    public class RunningModule : Module
    {
        private readonly IRunningProcess _process;
        private readonly ISpyManager _spyManager;

        public RunningModule(string path, string name, IntPtr baseAddress, IRunningProcess process, ISpyManager aSpyManager)
            : base(path, name, baseAddress)
        {
            _process = process;
            _spyManager = aSpyManager;
        }

        public RunningModule(string modulePath)
        {
            Path = modulePath;
        }

        #region Module Overrides

        public override Function[] Functions()
        {
            return _spyManager.HookableFunctionsFor(this, _process);
        }

        public override Hook[] Hooks()
        {
            throw new NotImplementedException();
            //return _spyManager.HooksOf(_process).Where(h => h.ModuleName.Equals(Name)).ToArray();
        }

        public override int HooksCount()
        {
            return _spyManager.HooksCountFor(_process, this);
        }

        #endregion

        public bool BelongsToProcessNamed(string processName)
        {
            return _process.Name.Equals(processName);
        }
    }
}