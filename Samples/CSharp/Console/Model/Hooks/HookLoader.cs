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
using System.Linq;
using CSharpConsole.Model.Modules;
using CSharpConsole.Model.Process;

namespace CSharpConsole.Model.Hooks
{
    internal class HookLoader
    {
        #region Fields

        private readonly HookingSettings _hookingSettings;
        private readonly Func<IEnumerable<IRunningProcess>, IEnumerable<IRunningProcess>> _onSelectProcessToHook;
        private readonly Action<IRunningProcess, Module, Function> _addHook;

        private readonly object _processesBeingHookedLock = new object();
        private readonly HashSet<IProcess> _processesBeingHookedWithoutConfirmation = new HashSet<IProcess>();

        #endregion

        #region Instance Creation

        public HookLoader(HookingSettings hookingSettings, Func<IEnumerable<IRunningProcess>, IEnumerable<IRunningProcess>> onSelectProcessToHook, Action<IRunningProcess, Module, Function> addHook)
        {
            _hookingSettings = hookingSettings;
            _onSelectProcessToHook = onSelectProcessToHook;
            _addHook = addHook;
        }

        #endregion

        #region Hooks Loading

        public void LoadHooksOn(IEnumerable<IRunningProcess> processes)
        {
            var processesToApplySettingsOn = ProcessesToApplySettingsOn(processes);
            if (!processesToApplySettingsOn.Any()) return;

            var selectedProcesses = _onSelectProcessToHook(processesToApplySettingsOn);
            RecreateHooksFromRulesOn(selectedProcesses);
        }

        public void LoadHooksWithoutConfirmationOn(IRunningProcess aProcess)
        {
            AddProcessBeingHookedWithoutConfirmation(aProcess);

            RecreateHooksFromRulesOn(new[] {aProcess});

            RemoveProcessBeingHookedWithoutConfirmation(aProcess);
        }

        #endregion

        #region Process Filtering

        private IRunningProcess[] ProcessesToApplySettingsOn(IEnumerable<IRunningProcess> processes)
        {
            return processes
                .Where(_hookingSettings.AppliesOn)
                .Where(ProcessIsNotBeingHooked)
                .ToArray();
        }

        private bool ProcessIsNotBeingHooked(IRunningProcess aProcess)
        {
            lock (_processesBeingHookedLock)
            {
                return !_processesBeingHookedWithoutConfirmation.Contains(aProcess);
            }
        }

        #endregion

        #region Hooks Creation

        private void RecreateHooksFromRulesOn(IEnumerable<IRunningProcess> selectedProcesses)
        {
            _hookingSettings.EachRuleDo(rule => RecreateHookFromRuleOn(selectedProcesses, rule));
        }

        private void RecreateHookFromRuleOn(IEnumerable<IRunningProcess> selectedProcesses, HookingRule aRule)
        {
            var matchingProcesses = selectedProcesses.Where(aRule.AppliesOn);
            foreach (var process in matchingProcesses)
            {
                if (!process.IsAlive()) return;

                var module = process.ModuleByName(aRule.ModuleName);
                if (module == null) continue;

                var function = module.FunctionByName(aRule.FunctionName);
                if (function == null) continue;

                _addHook(process, module, function);
            }
        }

        public void ApplyHookOn(Hook hook, IRunningProcess process)
        {
            var rule = new HookingRule(process.Name, hook.ModuleName, hook.FunctionName);

            RecreateHookFromRuleOn(new List<IRunningProcess>{ process }, rule);
        }

        #endregion

        #region Process Being Hooked

        private void RemoveProcessBeingHookedWithoutConfirmation(IRunningProcess aProcess)
        {
            lock (_processesBeingHookedLock)
            {
                _processesBeingHookedWithoutConfirmation.Remove(aProcess);
            }
        }

        private void AddProcessBeingHookedWithoutConfirmation(IRunningProcess aProcess)
        {
            lock (_processesBeingHookedLock)
            {
                _processesBeingHookedWithoutConfirmation.Add(aProcess);
            }
        }

        #endregion
    }
}