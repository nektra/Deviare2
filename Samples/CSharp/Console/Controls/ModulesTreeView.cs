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
using System.Linq;
using System.Threading;
using CSharpConsole.Model.Collections;
using CSharpConsole.Model.Modules;
using CSharpConsole.Model.Nodes;
using CSharpConsole.Model.Visitors;
using CSharpConsole.Extensions;
using CSharpConsole.Views;

namespace CSharpConsole.Controls
{
    public class ModulesTreeView : DeviareTreeView
    {
        #region Fields

        private readonly EntityDictionary<Module, string, ModuleNode> _moduleNodesCache;
        private readonly List<BackgroundWorker> _moduleDisplayers = new List<BackgroundWorker>();

        public readonly Mutex WorkerMutex = new Mutex(false);

        #endregion

        public ModulesTreeView()
        {
            _moduleNodesCache = new EntityDictionary<Module, string, ModuleNode>(module => module.Path, ModuleNode.From);
            FullRowSelect = true;
        }

        private void Add(Module aModule)
        {
            var aNode = _moduleNodesCache.ValueOrInitialize(aModule);
            AddDeviareNode(aNode);
        }

        public void HandleSelectedNode(Func<ModuleNode, bool> aModuleHandler, Action noSelection)
        {
            var aVisitor = DeviareNodeHandler.ForModules(aModuleHandler, noSelection);
            aVisitor.Visit(SelectedDeviareNode);
        }

        public void DisplaySingleProcessModules(IEnumerable<Module> modules)
        {
            Display(modules);

            SetSingleProcessModuleLayout();
        }

        private void SetSingleProcessModuleLayout()
        {
            ShowColumn("Address", true);
        }

        public void DisplayMultipleProcessesModules(IEnumerable<Module> modules)
        {
            Display(modules);

            SetMultipleProcessModuleLayout();
        }

        private void SetMultipleProcessModuleLayout()
        {
            ShowColumn("Address", false);
        }

        private void ShowColumn(string columnHeader, bool isVisible)
        {
            Columns.First(c => c.Header.Equals(columnHeader)).IsVisible = isVisible;
        }

        private void Display(IEnumerable<Module> modules)
        {   
            RemoveAllNodes();

            modules.ForEach(Add);
        }

        public void DisplayKeepingSelection(IEnumerable<Module> modules)
        {
            var lastSelectedDeviareNode = SelectedDeviareNode;

            Display(modules);

            var first =
                AllNodes.FirstOrDefault(
                    n => ((DeviareNode) n.Tag).DeviareNodeName.Equals(lastSelectedDeviareNode.DeviareNodeName));
            if (first != null) first.IsSelected = true;
        }

        private void AutoSizeColumns()
        {
            Columns.ForEach(AutoSizeColumn);
        }

        protected override void RemoveAllNodes()
        {
            _moduleNodesCache.Clear();
            base.RemoveAllNodes();
        }

        public void DisplaySingleProcessModulesKeepingSelection(IEnumerable<Module> modulesToBeDisplayed)
        {
            DisplayKeepingSelection(modulesToBeDisplayed);

            SetSingleProcessModuleLayout();
        }

        public void DisplayMultipleProcessesModulesKeepingSelection(IEnumerable<Module> modulesToBeDisplayed)
        {
            DisplayKeepingSelection(modulesToBeDisplayed);

            SetMultipleProcessModuleLayout();
        }

        public void DisplayModulesKeepingSelection(IEnumerable<Module> modules, BackgroundWorker worker)
        {
            if (WasCancelled(worker)) return;

            Module lastSelectedModule = null;

            if (SelectedNode != null)
                lastSelectedModule = ((ModuleNode)SelectedNode.Tag).Module;

                modules.ForEach(m =>
                                    {
                                        if (WasCancelled(worker)) return;
                                        Add(m);
                                    });
            if (WasCancelled(worker)) return;
            Sort(Columns.First());
            if (WasCancelled(worker)) return;
            AutoSizeColumns();

            if (lastSelectedModule != null)
                MarkAsSelected(lastSelectedModule, worker);
        }

        private bool WasCancelled(BackgroundWorker worker)
        {
            bool cancelled;
            CancelUpdateByWorker.TryGetValue(worker, out cancelled);
            return cancelled;
        }

        public void CancelUpdateFor(BackgroundWorker worker)
        {
            CancelUpdateByWorker[worker] = true;
        }

        public void RemoveWorkerCancelStatus(BackgroundWorker worker)
        {
            CancelUpdateByWorker.Remove(worker);
        }

        public void MarkAsSelected(Module module, BackgroundWorker worker)
        {
            if (WasCancelled(worker) && SelectedNode != null) return;
            SelectedNode = AllNodes.FirstOrDefault(n => ((ModuleNode) n.Tag).Module.Equals(module));
            if (WasCancelled(worker) || SelectedNode == null) return;
            EnsureVisible(SelectedNode);
        }

        public void MarkAsSelectedIfSelectionIsNull(Module module, BackgroundWorker worker)
        {
            if (SelectedNode == null)
                MarkAsSelected(module, worker);
        }

        public void SetHookingUnhookingAndUnhookingAllFlags(bool enableAddHook, bool enableRemoveHook, bool enableRemoveAllHooks, BackgroundWorker worker, int jobOrder, DeviareConsoleForm view)
        {
            if (WasCancelled(worker)) return;
            view.AddHookEnabled = enableAddHook;
            view.RemoveAllHooksEnabled = enableRemoveAllHooks;
            view.RemoveHookEnabled = enableAddHook;
        }
    }
}