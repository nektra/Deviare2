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
using System.Drawing;
using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.Modules;
using CSharpConsole.Presenters;

namespace CSharpConsole.Model.Process
{
    public class NullProcess : IProcess
    {
        #region Implementation of IProcess

        public string Name
        {
            get { throw new NotImplementedException(); }
        }

        public Image Icon
        {
            get { throw new NotImplementedException(); }
        }

        public bool IsActive
        {
            set { throw new NotImplementedException(); }
        }

        public string Path
        {
            get { throw new NotImplementedException(); }
        }

        public int PlatformBits
        {
            get { throw new NotImplementedException(); }
        }

        public IEnumerable<Module> Modules()
        {
            throw new NotImplementedException();
        }

        public Module ModuleByName(string moduleName)
        {
            throw new NotImplementedException();
        }

        public void Hook(Module aModule, Function aFunction, Action<Hook> onHookAdded)
        {
            throw new NotImplementedException();
        }

        public IEnumerable<Hook> Hooks()
        {
            throw new NotImplementedException();
        }

        public bool IsHooked()
        {
            throw new NotImplementedException();
        }

        public bool AtLeastOneHookIsActive()
        {
            throw new NotImplementedException();
        }

        public bool HasName()
        {
            throw new NotImplementedException();
        }

        public bool HasPlatformBits(int platformBits)
        {
            throw new NotImplementedException();
        }

        public void IfRunning(Action<IRunningProcess> ifTrue)
        {
            
        }

        public void IfNotRunning(Action<ProcessToBeHookedOnStart> ifTrue)
        {
            
        }

        public void AddHookUsing(Module aModule, Function aFunction, ApplicationPresenter applicationPresenter)
        {
            throw new NotImplementedException();
        }

        public void IsNotHooked(Action<IRunningProcess> ifTrue)
        {
            throw new NotImplementedException();
        }

        public void AlLeastOneHookIsActive(Action<IRunningProcess> ifTrue, Action<IRunningProcess> ifFalse)
        {
            throw new NotImplementedException();
        }

        public void Start(Action<IRunningProcess> onBeforeProcessStarts)
        {
            
        }

        public bool IsHooked(Module aModule, IEnumerable<Function> functions)
        {
            return false;
        }

        public bool IsRunning()
        {
            return false;
        }

        public int HookCountOn(Module aModule)
        {
            throw new NotImplementedException();
        }

        public bool IsToHookOnStart()
        {
            throw new NotImplementedException();
        }

        public int HookCountOn(string name)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}