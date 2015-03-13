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
using System.IO;
using System.Linq;
using System.Threading;
using CSharpConsole.Model;
using CSharpConsole.Model.FunctionCallHandlers;
using CSharpConsole.Model.HookStateHandlers;
using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.Modules;
using CSharpConsole.Model.Process;
using CSharpConsole.Model.ProcessHandlers;
using CSharpConsole.Views;
using Nektra.Deviare2;
using CSharpConsole.Extensions;

namespace CSharpConsole.Presenters
{
    public class ApplicationPresenter
    {
        #region Fields

        private IApplicationView _view;
        private ISpyManager _spyManager;
        private IEnumerable<IProcess> _selectedProcesses;
        public Module SelectedModule { get; private set; }
        private IEnumerable<Function> _selectedFunctions;
        private IEnumerable<Hook> _selectedHooks;
        private readonly int _platformBits;
        private bool _hooksAreBeingAdded;
        private FunctionCallDispatcher _callDispatcher;
        private readonly HookingSettings _hookingSettings;

        private HookLoader _hookLoader;

        private IHandler<FunctionCallInfo> _functionCallHandler;

        private IProcessModulesDisplayer _processModulesDisplayer;
        //static public readonly Mutex ProcessDisplayerMutex = new Mutex(false);

        #endregion

        #region Instance Creation & Initialization

        public static ApplicationPresenter For(IApplicationView aView)
        {
            return new ApplicationPresenter(aView, new SpyManager(), new HookingSettings(), (IntPtr.Size == 8 ? 64 : 32));
        }

        public ApplicationPresenter(IApplicationView aView, ISpyManager aSpyManager, HookingSettings hookingSettings, int platformBits)
        {
            _processModulesDisplayer = new ProcessModulesDisplayer(this, aView);
            _hookingSettings = hookingSettings;
            _platformBits = platformBits;
            InitializePresenter(aView, aSpyManager);
            InitializeView();
        }

        public ApplicationPresenter(IProcessModulesDisplayer processModulesDisplayer, IApplicationView aView, ISpyManager aSpyManager, HookingSettings hookingSettings, int platformBits)
        {
            _processModulesDisplayer = processModulesDisplayer;
            _hookingSettings = hookingSettings;
            _platformBits = platformBits;
            InitializePresenter(aView, aSpyManager);
            InitializeView();
        }

        private void InitializePresenter(IApplicationView aView, ISpyManager aSpyManager)
        {
            _selectedProcesses = Enumerable.Empty<IProcess>();
            _selectedFunctions = Enumerable.Empty<Function>();
            _view = aView;
            _spyManager = aSpyManager;


            _hookLoader = new HookLoader(_hookingSettings, FilterProcessesToApplyHookingSettingsOn, AddHookForRunningProcess);

            InitializeProcessHandlers();

            _spyManager.ProcessStartedHandler = ProcessStartedHandler;
            _spyManager.ProcessTerminatedHandler = ProcessTerminatedHandler;
            _spyManager.FunctionCalledHandler = FunctionCalledHandler;
            _spyManager.HookStateChangedHandler = HookStateChangedHandler;
            _spyManager.AgentLoadHandler = AgentLoadHandler;
        }

        private void InitializeView()
        {
            _view.UpdateApplicationTitle(string.Format("Deviare C# Console ({0} bits)", _platformBits));
            DisplayRunningProcesses();
            _view.AddHookEnabled = false;
            _view.RemoveHookEnabled = false;
            _view.ExportHookingSettings = false;
            _view.ClearOutputEnabled = false;
        }

        #region RunningProcess Handlers Initialization

        private void InitializeProcessHandlers()
        {
            _functionCallHandler = CreateFunctionCallHandler();
        }

        private IHandler<FunctionCallInfo> CreateFunctionCallHandler()
        {
            _callDispatcher = new FunctionCallDispatcher(_view.DisplayRootFunctionCall, _view.DisplayNestedFunctionCall, _view.UpdateRootFunctionCall);
            return new FunctionCallDispatcherHandler(_view, _callDispatcher);
        }

        #endregion

        #endregion

        #region RunningProcess Handling

        private void DisplayRunningProcesses()
        {
            var processes = _spyManager.RunningProcesses();

            processes = ProcessFilter.ForInvalidProcesses(_platformBits).Filter(processes);

            var clasifier = new ProcessClasifier();
            clasifier.Categorize(processes.Cast<IProcess>());

            _view.DisplayRunningProcesses(
                clasifier.ToHookOnStart.ToArray(),
                clasifier.UnHooked.ToArray(),
                 clasifier.HookedInactive.ToArray(),
                clasifier.HookedActive.ToArray());
        }

        private IEnumerable<IProcess> RunningProcesses()
        {
            return _spyManager.RunningProcesses().Where(process => process.HasPlatformBits(_platformBits)).ToArray();
        }

        public void SelectedProcessesChanged(IEnumerable<IProcess> newSelectedProcesses)
        {
            if (_selectedProcesses.SequenceEqual(newSelectedProcesses))
                return;

            var lastSelectedProcesses = _selectedProcesses;

            _selectedProcesses = newSelectedProcesses.ToArray();
            
            if (lastSelectedProcesses.Any(p => _selectedProcesses.Contains(p) && _selectedProcesses.Any(pr => pr.Modules().Contains(SelectedModule))))
            {
                DisplaySelectedProcessesModulesKeepingSelection();
            }
            else
            {
                DisplaySelectedProcessesModules();
                ClearDisplayedFunctions();
            }

            if (_selectedProcesses.Any(p => p.Name.Contains("CSharpConsole")))
            {
                _view.AddHookEnabled = false;
                _view.RemoveHookEnabled = false;
            } 
            else
            {
                if (_selectedProcesses.Any(p => p.IsHooked()))
                    _view.RemoveAllHooksEnabled = true;
            }
        }

        private void ProcessStartedHandler(IRunningProcess aProcess)
        {
            if (!ProcessFilter.ForInvalidProcesses(_platformBits).IsValid(aProcess))
                return;

            _view.DisplayNewRunningProcess(aProcess);
            _view.RemoveProcessToBeHookedOnStart(aProcess);

            UpdateProcessToSelectionIfItsGroupIsSelected(aProcess);

            DisplaySelectedProcessesModulesIfWasAdded(aProcess);

            _hookLoader.LoadHooksOn(new [] {aProcess});
        }

        private IEnumerable<IRunningProcess> FilterProcessesToApplyHookingSettingsOn(IEnumerable<IRunningProcess> processes)
        {
            var clasifier = new ProcessClasifier();
            clasifier.Categorize(processes.Cast<IProcess>());

            return _view.SelectProcessesToApplyHookingRules(
                clasifier.ToHookOnStart.ToArray(),
                clasifier.UnHooked.ToArray(),
                clasifier.HookedInactive.ToArray(),
                clasifier.HookedActive.ToArray());
        }

        private void ProcessTerminatedHandler(IRunningProcess aProcess)
        {
            if (!ProcessFilter.ForInvalidProcesses(_platformBits).IsValid(aProcess))
                return;

            RemoveRunningProcess(aProcess);
            RemoveProcessHooks(aProcess);
        }

        private void RemoveProcessHooks(IProcess aProcess)
        {
            aProcess.Hooks().ForEach(h => _view.RemoveHook(h));
        }

        private void RemoveRunningProcess(IRunningProcess aProcess)
        {
            _view.RemoveRunningProcess(aProcess);

            if (!_selectedProcesses.Contains(aProcess)) return;

            RemoveRunningProcessFromSelection(aProcess);
        }

        private ProcessToBeHookedOnStart CreateProcessToBeHookedFrom(string aPath)
        {
            var processToBeHookedOnStart = new ProcessToBeHookedOnStart(aPath, _spyManager, _platformBits);

            return processToBeHookedOnStart;
        }

        public void ClearSelectedProcesses()
        {
            if (_hooksAreBeingAdded) return;

            _selectedProcesses = Enumerable.Empty<IProcess>();
            ClearDisplayedModules();
            ClearDisplayedFunctions();
        }

        public void ProcessRightClick()
        {
            _selectedProcesses.Where(p => !p.IsRunning()).DefaultIfEmpty(RunningProcess.None()).First().IfNotRunning(proc => _view.DisplayToHookOnStartContextMenu());
        }

        //private bool SelectedProcessIsNull()
        //{
        //    return _selectedProcesses.Enumerable.Empty<IProcess>();
        //}

        public void StartSelectedProcess()
        {
            var processStarterThread = new Thread(StartSelectedProcessAndApplyHookingSettings);
            processStarterThread.Start();
        }

        private void StartSelectedProcessAndApplyHookingSettings()
        {
            _selectedProcesses.ForEach(p => p.Start(_hookLoader.LoadHooksWithoutConfirmationOn));
        }

        public void UpdateProcessToSelectionIfItsGroupIsSelected(IRunningProcess aProcess)
        {
            var firstSelectedProcess = _selectedProcesses.FirstOrDefault();
            if (firstSelectedProcess != null && firstSelectedProcess.Name.Equals(aProcess.Name))
                _selectedProcesses = _selectedProcesses.Union(new[] { aProcess });
        }

        public void RemoveRunningProcessFromSelection(IRunningProcess aProcess)
        {
            _selectedProcesses.ForEach(p => p.IfRunning(proc =>
            {
                if (proc.Id != aProcess.Id) return;
                var newProcessSelection = _selectedProcesses.ToList();
                newProcessSelection.Remove(proc);
                SelectedProcessesChanged(newProcessSelection);
            })
                                                            );
        }

        #endregion

        #region Modules Handling

        private void DisplaySelectedProcessesModules()
        {
            _processModulesDisplayer.DisplayProcessesModules(_selectedProcesses);
        }

        public void DisplaySelectedProcessesModulesKeepingSelection()
        {
            _processModulesDisplayer.DisplayProcessesModulesKeepingSelection(_selectedProcesses, SelectedModule);
        }

        //private bool ModuleShouldBeDisplayed(Module aModule)
        //{
        //    return !string.IsNullOrEmpty(aModule.Name);
        //}

        private void ClearDisplayedModules()
        {
            _view.DisplaySingleProcessModules(new RunningModule[0]);
        }

        public void SelectedModuleChanged(Module aModule)
        {
            if (SelectedModule != null && SelectedModule.Name.Equals(aModule.Name))
                return;
            SelectedModule = aModule;
            DisplaySelectedModuleFunctions();
        }

        public void ClearSelectedModule()
        {
            SelectedModule = null;
            ClearDisplayedFunctions();
        }

        public void DisplaySelectedProcessesModulesIfWasAdded(IRunningProcess aProcess)
        {
            if (_selectedProcesses.Contains(aProcess))
                DisplaySelectedProcessesModulesKeepingSelection();
        }

        #endregion

        #region Functions Handling

        public void DisplaySelectedModuleFunctions()
        {
            if (!_selectedProcesses.Any() || SelectedModule == null) return;
            _view.DisplayModuleFunctions(FunctionsToBeDisplayed());
        }

        private IEnumerable<Function> FunctionsToBeDisplayed()
        {
            var functions = SelectedModule.Functions()
                .Where(FunctionShouldBeDisplayed)
                .OrderBy(function => function.Name);

            return functions.ToArray();
        }

        private void ClearDisplayedFunctions()
        {
            _view.DisplayModuleFunctions(new Function[0]);
        }

        public void SelectedFunctionsChanged(IEnumerable<Function> functions)
        {
            _selectedFunctions = functions;
            _view.AddHookEnabled = _selectedFunctions.Any() && !_selectedProcesses.Any(p => p.Name.Contains("CSharpConsole"));
        }

        private void FunctionCalledHandler(Hook hook, IProcess aProcess, NktHookCallInfo hookCallInfo)
        {
            var aFunctionCallInfo = new FunctionCallInfo(hook, aProcess, hookCallInfo);
            _functionCallHandler.Handle(aFunctionCallInfo);
        }

        private bool FunctionShouldBeDisplayed(Function aFunction)
        {
            return !string.IsNullOrEmpty(aFunction.Name);
        }

        public void FunctionCallDoubleClicked(FunctionCall aFunctionCall)
        {
            _view.InspectFunctionCall(aFunctionCall);
        }

        public void ClearDisplayedFunctionsIfModuleIsNoLongerDisplayed()
        {
            if (!_selectedProcesses.Any(p => p.Modules().Contains(SelectedModule)))
                ClearDisplayedFunctions();
        }

        #endregion

        #region Hooks Handling

        public void AddHook()
        {
            if (!_selectedProcesses.Any() || SelectedModule == null || !_selectedFunctions.Any())
                throw new InvalidOperationException();

            if (AtLeastOneSelectedFunctionIsAlreadyHooked())
            {
                if (!_view.ConfirmIfFunctionsShouldBeHookedAgain()) return;
            }

            _hooksAreBeingAdded = true;

            _selectedFunctions.ForEach(AddHook);
            
            _hooksAreBeingAdded = false;
        }

        private bool AtLeastOneSelectedFunctionIsAlreadyHooked()
        {
            var atLeastOneFunctionIsAlreadyHooked = _selectedProcesses.Any(p => p.IsHooked(SelectedModule, _selectedFunctions));
            return atLeastOneFunctionIsAlreadyHooked;
        }

        private void AddHook(Function aFunction)
        {
            var aModule = SelectedModule;
            _selectedProcesses.ForEach(p => p.AddHookUsing(aModule, aFunction, this));
        }

        public void AddHookForRunningProcess(IRunningProcess aProcess, Module aModule, Function aFunction)
        {
            aProcess.Hook(aModule, aFunction, hook => ProcessAddedHook(aProcess, aModule, aFunction, hook));
        }

        private void PerformDragAndDropFromProcessToHook(IProcess process, Hook hook)
        {
            process.IfRunning(p => _hookLoader.ApplyHookOn(hook, p));
        }

        private void PerformDragAndDropFromProcessesToHook(IEnumerable<IProcess> processes, Hook hook)
        {
            processes.ToList().ForEach(p => PerformDragAndDropFromProcessToHook(p, hook));
        }

        private void ProcessAddedHook(IRunningProcess aProcess, Module aModule, Function aFunction, Hook aHook)
        {
            _hookingSettings.Add(aProcess.Name, aModule.Name, aFunction.Name);

            DisplayHook(aProcess, aModule, aFunction, aHook);
            _view.RemoveAllHooksEnabled = true;
            _view.ExportHookingSettings = true;
            _view.RemoveHookEnabled = true;
        }

        public void AddHookForProcessToBeHooked(IProcess aProcess, Module aModule, Function aFunction)
        {
            _hookingSettings.Add(aProcess.Name, aModule.Name, aFunction.Name);
        }

        private void DisplayHook(IRunningProcess aProcess, Module aModule, Function aFunction, Hook aHook)
        {
            _view.DisplayHook(aProcess, aModule, aFunction, aHook);
        }

        public void SelectedHookChanged(Hook aHook)
        {
            SelectedHookChanged(new[] { aHook });
        }

        public void SelectedHookChanged(IEnumerable<Hook> hooks)
        {
            // ReSharper disable PossibleMultipleEnumeration
            if (!hooks.Any())

            {
                ClearSelectedHooks();
                return;
            }
            _selectedHooks = hooks;

            _view.RemoveHookEnabled = true;
            _view.ExportHookingSettings = true;
            // ReSharper restore PossibleMultipleEnumeration
        }

        public void RemoveSelectedHooks()
        {
            if (_selectedHooks == null) return;
            RemoveFromHookingSettings(_selectedHooks);
            RemoveHooksCore(_selectedHooks);
        }

        private void RemoveFromHookingSettings(IEnumerable<Hook> hooks)
        {
            foreach (var hook in hooks)
            {
                var currentHook = hook;
                _hookingSettings.RemoveWhere(rule => rule.Matches(currentHook.ModuleName, currentHook.FunctionName));
            }
        }

        private void RemoveHooksCore(IEnumerable<Hook> hooks)
        {
            hooks.ForEach(RemoveHook);
        }

        private void ClearSelectedHooks()
        {
            _view.RemoveHookEnabled = false;
            _selectedHooks = null;
        }

        private void RemoveHook(Hook aHook)
        {
            _spyManager.RemoveHook(aHook);
            _hookingSettings.RemoveWhere(rule => rule.Matches(aHook.ModuleName, aHook.FunctionName));
        }

        private void HookStateChangedHandler(Hook hook, IRunningProcess aProcess, eNktHookState previousState, eNktHookState currentState)
        {
            var aStateChangeInfo = new HookStateChangeInfo(hook, aProcess, previousState, currentState);

            var aHook = aStateChangeInfo.Hook;
            if (aStateChangeInfo.CurrentState == eNktHookState.stRemoved)
            {
                _view.RemoveHook(aHook);
                _view.RemoveHookEnabled = false;
                _view.RemoveAllHooksEnabled = _spyManager.AtLeastOneHookWasCreated;
                _view.ExportHookingSettings = _spyManager.AtLeastOneHookWasCreated;
            }
            else
                _view.DisplayHookStatus(aStateChangeInfo);

            if (!aProcess.IsHooked())
                _view.DisplayRecentlyUnhookedProcess(aProcess);
            else if (aProcess.AtLeastOneHookIsActive())
                _view.DisplayRecentlyHookedProcessAsActive(aProcess);
            else
                _view.DisplayRecentlyHookedProcessAsInactive(aProcess);

            _view.RefreshModulesView();
        }

        public void ReplicateHooks(IEnumerable<IProcess> collectedProcesses, List<Hook> collectedHooks)
        {
            collectedHooks.ForEach(h => PerformDragAndDropFromProcessesToHook(collectedProcesses, h));
        }

        public void RemoveAllHooks()
        {
            _hookingSettings.RemoveWhere(rule => true);
            _spyManager.RemoveAllHooks();
        }

        public void RemoveAllHooksForSelectedFunction()
        {
            var hooksToRemove = _selectedProcesses.SelectMany(p => p.Hooks().Where(hook => hook.IsAppliedOn(SelectedModule, _selectedFunctions)));

            hooksToRemove.ForEach(RemoveHook);
        }

        #endregion

        private void AgentLoadHandler(IRunningProcess aProcess, int anErrorCode)
        {
            if (anErrorCode == 0) return;
            _view.ShowAgentLoadingError(aProcess, anErrorCode);
        }

        public void ClearOutput()
        {
            _callDispatcher.Clear();
            _view.ClearOutput();
            _view.ClearOutputEnabled = false;
        }

        public void Close()
        {
            _spyManager.Dispose();
        }

        public HookingSettings ExportHookingSettings()
        {
            return _hookingSettings;
        }

        public void ImportHookingSettings(Stream exportedHookingSettings)
        {
            try
            {
                var importedSettings = HookingSettings.FromXml(exportedHookingSettings);
                _hookingSettings.AddRulesFrom(importedSettings);
            }
            catch
            {
                _view.ShowError("Cannot import hooking settings. The file is invalid.");
                return;
            }

            var processes = ProcessFilter.ForInvalidProcesses(_platformBits).Filter(_spyManager.RunningProcesses());

            _hookLoader.LoadHooksOn(processes);
        }

        public void AddProcessToBeHooked(string aPath)
        {
            if (ThereIsARunningProcessMatching(aPath))
            {
                var aMessage = string.Format("The process '{0}' is already running.", aPath);
                _view.ShowError(aMessage);
                return;
            }
            _view.DisplayProcessToBeHookedOnStart(CreateProcessToBeHookedFrom(aPath));
        }

        private bool ThereIsARunningProcessMatching(string aPath)
        {
            return RunningProcesses().Any(process => process.Path.Equals(aPath, StringComparison.InvariantCultureIgnoreCase));
        }

        public void FunctionRightClick()
        {
            var enableRemoveHook = AtLeastOneSelectedFunctionIsAlreadyHooked();
            var enableAddHook = SelectedProcessIsHookable();
            _view.DisplayContextMenuForFunctions(enableRemoveHook, enableAddHook);
        }

        private bool SelectedProcessIsHookable()
        {
            return !_selectedProcesses.Any(p => p.Name.Contains("CSharpConsole"));
        }
    }
}