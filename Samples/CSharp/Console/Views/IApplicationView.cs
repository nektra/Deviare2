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

using System.Collections.Generic;
using System.ComponentModel;
using CSharpConsole.Model;
using CSharpConsole.Model.HookStateHandlers;
using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.Modules;
using CSharpConsole.Model.Process;

namespace CSharpConsole.Views
{
    public interface IApplicationView
    {
        void DisplayRunningProcesses(IEnumerable<IProcess> toHookOnStart, IEnumerable<IRunningProcess> unhooked, IEnumerable<IRunningProcess> hookedInactive, IEnumerable<IRunningProcess> hookedActive);
        void DisplayNewRunningProcess(IProcess aProcess);
        void DisplayHookedInactiveProcess(IProcess aProcess);
        void DisplayHookedActiveProcess(IProcess aProcess);
        void DisplayUnhookedProcess(IProcess aProcess);
        void RemoveProcessToBeHookedOnStart(IProcess aProcess);
        void DisplayProcessToBeHookedOnStart(IProcess aProcess);
        void RemoveRunningProcess(IProcess aProcess);

        void DisplaySingleProcessModules(IEnumerable<Module> modules);
        void DisplayMultipleProcessesModules(IEnumerable<Module> modules);
        void DisplayModuleFunctions(IEnumerable<Function> functions);

        bool AddHookEnabled { get; set; }
        void DisplayHook(IRunningProcess aProcess, Module aModule, Function aFunction, Hook aHook);
        void RemoveHook(Hook aHook);
        void DisplayHookStatus(HookStateChangeInfo aHookStateChage);

        void ClearOutput();
        bool ClearOutputEnabled { get; set; }
        
        void UpdateApplicationTitle(string aTitle);
        
        
        void DisplayRootFunctionCall(FunctionCall aFunctionCall);
        void DisplayNestedFunctionCall(FunctionCall aFunctionCall);
        void InspectFunctionCall(FunctionCall aFunctionCall);
        
        void DisplayToHookOnStartContextMenu();
        bool RemoveAllHooksEnabled { get; set; }

        IEnumerable<IRunningProcess> SelectProcessesToApplyHookingRules(IEnumerable<IProcess> toHookOnStart, IEnumerable<IRunningProcess> unhooked, IEnumerable<IRunningProcess> hookedInactive, IEnumerable<IRunningProcess> hookedActive);
        
        bool ExportHookingSettings { get; set; }
        bool RemoveHookEnabled   { get; set; }
        void ShowError(string aMessage);
        void ShowAgentLoadingError(IRunningProcess aProcess, int anErrorCode);
        void UpdateRootFunctionCall(FunctionCall aFunctionCall);
        bool ConfirmIfFunctionsShouldBeHookedAgain();
        void DisplayRecentlyUnhookedProcess(IProcess aProcess);
        void DisplayRecentlyHookedProcessAsActive(IProcess aProcess);
        void DisplayRecentlyHookedProcessAsInactive(IProcess aProcess);
        void RefreshModulesView();
        void DisplayContextMenuForFunctions(bool enableRemoveHook, bool enableAddHook);
        void DisplayModules(IEnumerable<Module> modules, BackgroundWorker worker);
        void CancelModulesViewUpdateFor(BackgroundWorker worker);
        void RemoveUpdateCancellationStatusByWorker(BackgroundWorker worker);
        void DisplayModulesInOrder(List<Module> getRange, BackgroundWorker worker, int anOrder);
        void SortModulesViewByColumn(int columnIndex);
        void MarkAsSelected(Module module, BackgroundWorker worker, int jobOrder);
        void SetHookingUnhookingAndUnhookingAllFlags(bool enableAddHook, bool enableRemoveHook, bool enableRemoveAllHooks, BackgroundWorker worker, int jobOrder);
        void MarkAsSelectedIfSelectionIsNull(Module module, BackgroundWorker worker, int jobOrder);
    }
}