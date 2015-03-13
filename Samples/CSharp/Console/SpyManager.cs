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
using System.Runtime.InteropServices;
using System.Threading;
using CSharpConsole.Extensions;
using CSharpConsole.Model;
using CSharpConsole.Model.Collections;
using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.Modules;
using CSharpConsole.Model.Process;
using CSharpConsole.Presenters;
using CSharpConsole.Properties;
using Nektra.Deviare2;

namespace CSharpConsole
{
    public class SpyManager : ISpyManager
    {
        #region Fields

        private readonly NktSpyMgr _manager;
        private readonly ManualResetEvent _shutdownDeviareEvent = new ManualResetEvent(false);
        private readonly ManualResetEvent _deviareInitializedEvent = new ManualResetEvent(false);
        private Thread _deviareWorker;

        #endregion

        #region Instance Creation & Initialization

        public SpyManager()
            : this(new NktSpyMgr())
        {
        }

        public SpyManager(NktSpyMgr aSpyMgr)
        {
            _manager = aSpyMgr;
            InitializeHooksByProcesses();
            _cachedDbModulesByPlatformBits = new EntityDictionary<int, int, Module[]>(platformBits => platformBits, RetrieveModulesFromDB);

            InitializeSpyManager();
            StartDeviareWorker();
        }

        private void InitializeSpyManager()
        {
            ProcessStartedHandler = process => { };
            ProcessTerminatedHandler = process => { };
            FunctionCalledHandler = (hook, process, hookCallInfo) => { };
            HookStateChangedHandler = (hook, process, previousState, currentState) => { };
            AgentLoadHandler = (process, errorCode) => { };
        }

        #endregion

        #region Deviare

        private void StartDeviareWorker()
        {
            _deviareWorker = new Thread(StartDeviare);
            _deviareWorker.SetApartmentState(ApartmentState.MTA);
            _deviareWorker.Name = "Deviare API Thread";
            _deviareWorker.Start();

            _deviareInitializedEvent.WaitOne();
        }

        private void StartDeviare()
        {
            InitializeDeviare();
            WaitForShutdownRequest();
            ShutdownDeviare();
        }

        private void InitializeDeviare()
        {
            _manager.Initialize();

            _manager.OnProcessStarted += HandleStartedProcess;
            _manager.OnProcessTerminated += HandleTerminatedProcess;
            _manager.OnFunctionCalled += HandleFuncionCall;
            _manager.OnHookStateChanged += HandleHookStateChanged;
            _manager.OnAgentLoad += HandleAgentLoad;

            _deviareInitializedEvent.Set();
        }

        private void WaitForShutdownRequest()
        {
            _shutdownDeviareEvent.WaitOne();
        }

        private void ShutdownDeviare()
        {
            _manager.OnProcessStarted -= HandleStartedProcess;
            _manager.OnProcessTerminated -= HandleTerminatedProcess;
            _manager.OnFunctionCalled -= HandleFuncionCall;
            _manager.OnHookStateChanged -= HandleHookStateChanged;
            _manager.OnAgentLoad -= HandleAgentLoad;

            try
            {
                Marshal.ReleaseComObject(_manager);
            }
            catch
            {

            }
        }

        #endregion

        #region Event Handlers

        private void HandleAgentLoad(NktProcess aProcess, int anErrorCode)
        {
            AgentLoadHandler(CreateConsoleProcessFrom(aProcess), anErrorCode);
        }

        private IRunningProcess CreateConsoleProcessFrom(NktProcess aProcess)
        {
            return RunningProcess.From(aProcess, this);
        }

        private void HandleHookStateChanged(NktHook aHook, NktProcess aProcess, eNktHookState currentState, eNktHookState previousState)
        {
            var consoleHook = CreateConsoleHookFrom(aHook);
            var consoleProcess = CreateConsoleProcessFrom(aProcess);

            _hooksByProcesses.Update(consoleHook, consoleProcess, currentState, previousState);

            HookStateChangedHandler(consoleHook, consoleProcess, previousState, currentState);
        }

        private Hook CreateConsoleHookFrom(NktHook aHook)
        {
            return Hook.From(aHook, this);
        }

        private void HandleFuncionCall(NktHook aHook, NktProcess aProcess, NktHookCallInfo hookCallInfo)
        {
            FunctionCalledHandler(CreateConsoleHookFrom(aHook), CreateConsoleProcessFrom(aProcess), hookCallInfo);
        }

        private void HandleStartedProcess(NktProcess aProcess)
        {
            ProcessStartedHandler(CreateConsoleProcessFrom(aProcess));
        }

        private void HandleTerminatedProcess(NktProcess aProcess)
        {
            var consoleProcess = CreateConsoleProcessFrom(aProcess);
            ProcessTerminatedHandler(consoleProcess);
            _hooksByProcesses.RemoveAllHooksOf(consoleProcess);
            
        }

        #endregion

        #region Implementation of ISpyManager

        #region Event Handlers

        public Action<IRunningProcess> ProcessStartedHandler { get; set; }

        public Action<IRunningProcess> ProcessTerminatedHandler { get; set; }

        public Action<Hook, IRunningProcess, NktHookCallInfo> FunctionCalledHandler { get; set; }

        public Action<Hook, IRunningProcess, eNktHookState, eNktHookState> HookStateChangedHandler { get; set; }

        public Action<IRunningProcess, int> AgentLoadHandler { get; set; }

        #endregion

        public IEnumerable<IRunningProcess> RunningProcesses()
        {
            return RunningProcess.From(_manager.Processes().CollectAll(), this);
        }

        public Module ModuleByName(IRunningProcess aProcess, string aModuleName)
        {
            var nktProcess = NktProcessFrom(aProcess);
            if (nktProcess == null) return null;

            var aModule = nktProcess.ModuleByName(aModuleName);
            if (aModule == null) return null;

            return Module.From(aModule, aProcess, this);
        }

        #region Hooks

        public Hook AddHook(IRunningProcess aProcess, Module aModule, Function aFunction)
        {
            var ntkProcess = NktProcessFrom(aProcess);
            if (ntkProcess == null) return null;
            if (!aProcess.IsAlive()) return null;

            var nktModule = ntkProcess.ModuleByName(aModule.Name);
            if (nktModule == null) return null;

            var nktFunction = nktModule.FunctionByName(aFunction.Name);
            if (nktFunction == null) return null;

            var nktHook = _manager.CreateHook(nktFunction, 0);

            nktHook.Hook(true);
            nktHook.Attach(ntkProcess, true);

            return CreateConsoleHookFrom(nktHook);
        }

        public void RemoveHook(Hook aHook)
        {
            var nktHook = NktHookFor(aHook);
            if (nktHook == null) return;

            _manager.DestroyHook(nktHook);
        }

        public void RemoveAllHooks()
        {
            var hooks = Hooks();
            foreach (var hook in hooks)
            {
                RemoveHook(hook);
            }
        }

        public bool AtLeastOneHookWasCreated
        {
            get { return _hooksByProcesses.AtLeastOneHookWasCreated(); }
        }

        public bool IsHookActiveOn(Hook aHook, IRunningProcess aProcess)
        {
            var nktHook = NktHookFor(aHook);
            if (nktHook == null) return false;
            return nktHook.State(aProcess.Id) == eNktHookState.stActive;
        }

        //private EntityDictionary<IRunningProcess, int, Module[]> _modulesCache;

        public IEnumerable<Module> HookableModulesFor(IRunningProcess aProcess)
        {

            //if (_modulesCache == null)
            //{
            //    _modulesCache = new EntityDictionary<IRunningProcess, int, Module[]>(process => process.Id,
            //    process =>
            //    {
            //        var nktProcess = NktProcessFrom(aProcess);
            //        return nktProcess.Modules().CollectAll().Select(module => Module.From(module, aProcess, this)).ToArray();
            //    }
            //);
            //}

            //return _modulesCache.ValueOrInitialize(aProcess);


            IEnumerable<Module> modules;

            try
            {
                var nktProcess = NktProcessFrom(aProcess);
                modules = nktProcess.Modules().CollectAll().Select(module => Module.From(module, aProcess, this)).ToArray();
            }
            catch
            {
                modules = Enumerable.Empty<Module>();
            }

            return modules;
        }

        public Function[] HookableFunctionsFor(Module aModule, IRunningProcess aProcess)
        {
            IEnumerable<Function> functions;

            try
            {
                var nktProcess = NktProcessFrom(aProcess);
                var nktModule = nktProcess.ModuleByName(aModule.Name);
                functions = Function.From(nktModule.ExportedFunctions().CollectAll());
            }
            catch
            {
                functions = Enumerable.Empty<Function>();
            }

            return functions.ToArray();
        }

        public bool IsAlive(RunningProcess aProcess, int timeOutInMilliseconds)
        {
            var nktProcess = NktProcessFrom(aProcess);
            if (nktProcess == null) return false;
            return nktProcess.get_IsActive(timeOutInMilliseconds);
        }

        public bool IsHooked(IRunningProcess aProcess)
        {
            return _hooksByProcesses.IsHooked(aProcess);
        }

        public bool AtLeastOneHookIsActiveOn(IRunningProcess aProcess)
        {
            return _hooksByProcesses.AtLeastOneHookIsActiveOn(aProcess);
        }

        public void Start(ProcessToBeHookedOnStart aProcess, Action<IRunningProcess> onBeforeProcessStarts)
        {
            object continueEvent;
            var newSuspendedProcess = _manager.CreateProcess(aProcess.Path, true, out continueEvent);

            var processWasCreatedSuccessfully = newSuspendedProcess != null && Convert.ToUInt64(continueEvent.ToString()) != 0;
            if (!processWasCreatedSuccessfully) return;

            var processToBeRun = CreateConsoleProcessFrom(newSuspendedProcess);
            onBeforeProcessStarts(processToBeRun);

            _manager.ResumeProcess(newSuspendedProcess, continueEvent);
        }

        public int HooksCountFor(IRunningProcess aProcess, Module aModule)
        {
            return _hooksByProcesses.HooksCountFor(aProcess, aModule);
        }

        #endregion

        #region Hooks Recording

        private HookDictionary _hooksByProcesses;

        private void InitializeHooksByProcesses()
        {
            _hooksByProcesses = new HookDictionary();
        }

        private Hook[] Hooks()
        {
            return _hooksByProcesses.AllHooks();
        }

        public Hook[] HooksOf(IRunningProcess aProcess)
        {
            return _hooksByProcesses.HooksFor(aProcess);
        }

        #endregion

        #region Deviare DB Access

        private readonly object _dbModulesLock = new object();
        private readonly EntityDictionary<int, int, Module[]> _cachedDbModulesByPlatformBits;

        private Module[] RetrieveModulesFromDB(int platformBits)
        {
            lock (_dbModulesLock)
            {
                var dbModules = _manager.DbModules(platformBits);
                return dbModules == null ? new RunningModule[0] : Module.From(dbModules);
            }
        }

        public Module[] HookableModulesFor(int platformBits)
        {
            return _cachedDbModulesByPlatformBits.ValueOrInitialize(platformBits);
        }

        #endregion

        private NktProcess NktProcessFrom(IRunningProcess aProcess)
        {
            return _manager.ProcessFromPID(aProcess.Id);
        }

        private NktHook NktHookFor(Hook aHook)
        {
            return _manager.Hooks().GetById(aHook.Id);
        }

        #endregion

        #region IDisposable Implementation

        public void Dispose()
        {
            _shutdownDeviareEvent.Set();
            _deviareWorker.Join();
        }

        #endregion
    }
}