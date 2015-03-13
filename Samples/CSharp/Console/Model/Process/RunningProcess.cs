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
using CSharpConsole.Extensions;
using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.Modules;
using CSharpConsole.Presenters;
using Nektra.Deviare2;

namespace CSharpConsole.Model.Process
{
    public class RunningProcess : ProcessBase, IRunningProcess
    {
        private static NullProcess _none;
        private Dictionary<int, string> _modulePathByNameAndProcess;

        #region Instance Creation

        public RunningProcess(int id, string name, Image icon, ISpyManager spyManager, string path, int platformBits)
            : base(spyManager, platformBits, name, path, icon)
        {
            Id = id;
        }

        private RunningProcess(int id, string name, Image icon, ISpyManager spyManager, string path, int platformBits, Dictionary<int, string> modulePathByNameAndProcess)
            : base(spyManager, platformBits, name, path, icon)
        {
            Id = id;
            _modulePathByNameAndProcess = modulePathByNameAndProcess;
        }


        public static IRunningProcess From(NktProcess aProcess, ISpyManager spyManager)
        {
            var modulePathByNameAndProcess = new Dictionary<int, string>();

            if (aProcess.PlatformBits > 0 && aProcess.PlatformBits <= IntPtr.Size * 8)
            {
                var processModules = aProcess.Modules();
                if (processModules != null)
                    processModules.CollectAll().ForEach(m =>
                                                            {
                                                                var moduleKey = (m.Name + aProcess.Id).GetHashCode();
                                                                if (!modulePathByNameAndProcess.ContainsKey(moduleKey))
                                                                    modulePathByNameAndProcess.Add(moduleKey, m.Path);
                                                            });
            }

            return new RunningProcess(aProcess.Id, aProcess.Name, aProcess.Icon(), spyManager, aProcess.Path, aProcess.PlatformBits, modulePathByNameAndProcess);
        }

        public static IEnumerable<IRunningProcess> From(IEnumerable<NktProcess> expectedRunningProcesses, ISpyManager spyManager)
        {
            return expectedRunningProcesses.Where(p => p.PlatformBits > 0 && p.PlatformBits <= IntPtr.Size * 8).Select(process => From(process, spyManager)).ToArray();
        }

        public static IProcess None()
        {
            if (_none == null) _none = new NullProcess();
            return _none;
        }

        #endregion

        #region Overrides of ProcessBase

        public override Module ModuleByName(string moduleName)
        {
            try
            {
                return SpyManager.ModuleByName(this, moduleName) ??
                       Module.From(_modulePathByNameAndProcess[(moduleName + Id).GetHashCode()]);

            } catch (KeyNotFoundException)
            {
                return null;
            }
        }

        public override void Hook(Module aModule, Function aFunction, Action<Hook> onHookAdded)
        {
            Hook hook = null;
            try
            {
                hook = SpyManager.AddHook(this, aModule, aFunction);
            }
            catch 
            {
                
            }

            if (hook != null) onHookAdded(hook);
        }

        public override IEnumerable<Hook> Hooks()
        {
            return SpyManager.HooksOf(this);
        }

        public override bool IsHooked()
        {
            return SpyManager.IsHooked(this);
        }

        public override bool AtLeastOneHookIsActive()
        {
            return SpyManager.AtLeastOneHookIsActiveOn(this);
        }

        public override void IfRunning(Action<IRunningProcess> ifTrue)
        {
            ifTrue(this);
        }

        public override void IfNotRunning(Action<ProcessToBeHookedOnStart> ifTrue)
        {
            
        }

        public override void AddHookUsing(Module aModule, Function aFunction, ApplicationPresenter applicationPresenter)
        {
            applicationPresenter.AddHookForRunningProcess(this, aModule, aFunction);
        }

        public override void IsNotHooked(Action<IRunningProcess> ifTrue)
        {
            if (IsHooked()) return;
            ifTrue(this);
        }

        public override void AlLeastOneHookIsActive(Action<IRunningProcess> ifTrue, Action<IRunningProcess> ifFalse)
        {
            if (!IsHooked()) return;

            if (AtLeastOneHookIsActive())
                ifTrue(this);
            else
                ifFalse(this);
        }

        public override bool IsRunning()
        {
            return true;
        }

        public override int HookCountOn(Module aModule)
        {
            return SpyManager.HooksCountFor(this, aModule);
        }

        public override bool IsToHookOnStart()
        {
            return false;
        }

        public override IEnumerable<Module> Modules()
        {
            return SpyManager.HookableModulesFor(this);
        }

        #endregion

        #region Implementation of IRunningProcess

        public int Id { get; private set; }

        public bool IsAlive()
        {
            return IsAlive(0);
        }

        public bool IsAlive(int timeOut)
        {
            return SpyManager.IsAlive(this, timeOut);
        }

        public void Do(Hook aHook, Action<IRunningProcess, Module, Function> anAction)
        {
            if (!IsAlive()) return;

            var module = ModuleByName(aHook.ModuleName);
            if (module == null) return;

            var function = module.FunctionByName(aHook.FunctionName);
            if (function == null) return;

            anAction(this, module, function);
        }

        #endregion

        #region Equality Members

        public bool Equals(RunningProcess other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return other.Id == Id;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != typeof (RunningProcess)) return false;
            return Equals((RunningProcess) obj);
        }

        public override int GetHashCode()
        {
            return Id;
        }

        #endregion

    }
}