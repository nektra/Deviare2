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
using CSharpConsole.Model.Collections;
using CSharpConsole.Model.Modules;
using CSharpConsole.Model.Process;
using Nektra.Deviare2;

namespace CSharpConsole.Model.Hooks
{
    internal class HookDictionary
    {
        #region Instance Creation

        private readonly EntityDictionary<IRunningProcess, int, HashSet<Hook>> _hooks = new EntityDictionary<IRunningProcess, int, HashSet<Hook>>(process => process.Id, process => new HashSet<Hook>());
        private readonly EntityDictionary<IRunningProcess, int, HashSet<IntPtr>> _activeHooks = new EntityDictionary<IRunningProcess, int, HashSet<IntPtr>>(process => process.Id, process => new HashSet<IntPtr>());
        private readonly EntityDictionary<IRunningProcess, int, EntityDictionary<Module, int, int>> _hookCountByModule = new EntityDictionary<IRunningProcess, int, EntityDictionary<Module, int, int>>(process => process.Id, process => new EntityDictionary<Module, int, int>(module => module.Path.GetHashCode(), module => 0));
        private readonly object _syncLock = new object();

        #endregion

        public Hook[] AllHooks()
        {
            lock (_syncLock)
            {
                return _hooks.Values.SelectMany(set => set).ToArray();
            }
        }

        public Hook[] HooksFor(IRunningProcess aProcess)
        {
            lock (_syncLock)
            {
                return _hooks.ValueOrInitialize(aProcess).ToArray();
            }
        }

        public int HooksCountFor(IRunningProcess aProcess, Module aModule)
        {
            return _hookCountByModule.ValueOrInitialize(aProcess).ValueOrInitialize(aModule);
        }

        public bool IsHooked(IRunningProcess aProcess)
        {
            lock(_syncLock)
            {
                return _hooks.ValueOrInitialize(aProcess).Count > 0;
            }
        }

        public bool AtLeastOneHookIsActiveOn(IRunningProcess aProcess)
        {
            lock (_syncLock)
            {
                return _activeHooks.ValueOrInitialize(aProcess).Count > 0;
            }
        }

        public void Update(Hook aHook, IRunningProcess aProcess, eNktHookState currentState, eNktHookState previousState)
        {
            var module = aProcess.ModuleByName(aHook.ModuleName);
            lock (_syncLock)
            {
                if (currentState == eNktHookState.stRemoved)
                {
                    _hooks.ValueOrInitialize(aProcess).Remove(aHook);
                    _activeHooks.ValueOrInitialize(aProcess).Remove(aHook.Id);
                    if (previousState == eNktHookState.stActive) 
                        _hookCountByModule.Update(aProcess, moduleDictionary =>
                                                            { 
                                                                moduleDictionary.Update(module, oldCount => --oldCount);
                                                                return moduleDictionary;
                                                            });
                }
                else
                {
                    _hooks.ValueOrInitialize(aProcess).Add(aHook);

                    if (currentState == eNktHookState.stActive)
                    {
                        _activeHooks.ValueOrInitialize(aProcess).Add(aHook.Id);
                        _hookCountByModule.Update(aProcess, moduleDictionary =>
                                                            {
                                                                moduleDictionary.Update(module, oldCount => ++oldCount);
                                                                return moduleDictionary;
                                                            });
                    }
                }
            }
        }

        public bool AtLeastOneHookWasCreated()
        {
            lock(_syncLock)
            {
                return _hooks.Values.Any(hooks => hooks.Count > 0);
            }
        }

        public void RemoveAllHooksOf(IRunningProcess aProcess)
        {
            lock (_syncLock)
            {
                _hooks.Remove(aProcess);
                _activeHooks.Remove(aProcess);
                _hookCountByModule.Update(aProcess, moduleDictionary => { 
                                                                            moduleDictionary.Clear();
                                                                            return moduleDictionary;
                                                                        });
            }
        }
    }
}