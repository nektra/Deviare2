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
using System.Linq;
using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.Modules;
using CSharpConsole.Presenters;

namespace CSharpConsole.Model.Process
{
    public abstract class ProcessBase : IProcess
    {
        protected ProcessBase(ISpyManager spyManager, int platformBits, string name, string path, Image icon)
        {
            Name = name;
            Path = path;
            SpyManager = spyManager;
            PlatformBits = platformBits;
            Icon = icon;
        }

        public virtual string Name { get; protected set; }
        public Image Icon { get; protected set; }
        public bool IsActive { get; set; }
        public string Path { get; protected set; }
        public int PlatformBits { get; protected set; }
        protected ISpyManager SpyManager { get; private set; }

        public abstract Module ModuleByName(string moduleName);
        public abstract void Hook(Module aModule, Function aFunction, Action<Hook> onHookAdded);
        public abstract IEnumerable<Hook> Hooks();
        public abstract bool IsHooked();


        public abstract bool AtLeastOneHookIsActive();

        public bool HasName()
        {
            return !string.IsNullOrEmpty(Name);
        }
        public bool HasPlatformBits(int platformBits)
        {
            return PlatformBits == platformBits;
        }

        public abstract void IfRunning(Action<IRunningProcess> ifTrue);
        public abstract void IfNotRunning(Action<ProcessToBeHookedOnStart> ifTrue);
        public abstract void AddHookUsing(Module aModule, Function aFunction, ApplicationPresenter applicationPresenter);
        public abstract void IsNotHooked(Action<IRunningProcess> ifTrue);
        public abstract void AlLeastOneHookIsActive(Action<IRunningProcess> ifTrue, Action<IRunningProcess> ifFalse);

        public void Start(Action<IRunningProcess> onBeforeProcessStarts)
        {
            IfNotRunning(process => Start(process, onBeforeProcessStarts));
        }

        private void Start(ProcessToBeHookedOnStart aProcess, Action<IRunningProcess> onBeforeProcessStarts)
        {
            SpyManager.Start(aProcess, onBeforeProcessStarts);
        }

        public bool IsHooked(Module aModule, IEnumerable<Function> functions)
        {
            return Hooks().Any(hook => hook.IsAppliedOn(aModule, functions));
        }

        public abstract bool IsRunning();
        public abstract int HookCountOn(Module aModule);
        public abstract bool IsToHookOnStart();

        public abstract IEnumerable<Module> Modules();
    }
}