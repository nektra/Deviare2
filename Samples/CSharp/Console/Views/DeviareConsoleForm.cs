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
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using Aga.Controls.Tree;
using CSharpConsole.Controls;
using CSharpConsole.Controls.Updating;
using CSharpConsole.Model;
using CSharpConsole.Model.HookStateHandlers;
using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.Modules;
using CSharpConsole.Model.Nodes;
using CSharpConsole.Model.Process;
using CSharpConsole.Model.Visitors;
using CSharpConsole.Presenters;

namespace CSharpConsole.Views
{
    public partial class DeviareConsoleForm : Form, IApplicationView
    {
        #region Fields

        private readonly UIUpdater _uiUpdater = new UIUpdater();
        private Point _lastFunctionNodeLocation;
        private Point _lastProcessNodeLocation;
        private ApplicationPresenter _presenter;

        #endregion

        #region Instance Creation & Initialization

        public DeviareConsoleForm()
        {
            InitializeComponent();
            addProcessToBeHookedDialog.InitialDirectory =
                Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);
            openHookingSettingsDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Desktop);
        }

        private void FormLoadHandler(object sender, EventArgs e)
        {
            InitializeTreeViewComponentes();
            InitializeApplicationPresenter();
        }

        private void InitializeApplicationPresenter()
        {
            _presenter = ApplicationPresenter.For(this);
        }

        private void InitializeTreeViewComponentes()
        {
            _processesTreeView.InitializeComponent();
            _processesTreeView.ExpandAll();
            _modulesTreeView.InitializeComponent();
            _functionsTreeView.InitializeComponent();
            _hooksTreeView.InitializeComponent();
            treeViewOutput.InitializeComponent();
        }

        #endregion

        #region Implementation of IApplicationView

        #region Displaying Processes

        public void DisplayRunningProcesses(IEnumerable<IProcess> toHookOnStart, IEnumerable<IRunningProcess> unhooked,
                                            IEnumerable<IRunningProcess> hookedInactive,
                                            IEnumerable<IRunningProcess> hookedActive)
        {
            AddProcessUpdateJob(() => _processesTreeView.Display(toHookOnStart, unhooked, hookedInactive, hookedActive));
        }

        public void DisplayNewRunningProcess(IProcess aProcess)
        {
            AddProcessUpdateJob(() => _processesTreeView.DisplayUnHookedProcess(aProcess));
        }

        public void RemoveRunningProcess(IProcess aProcess)
        {
            AddProcessUpdateJob(() => _processesTreeView.RemoveRunningProcess(aProcess));
        }

        public void DisplayProcessToBeHookedOnStart(IProcess aProcess)
        {
            AddProcessUpdateJob(() => _processesTreeView.DisplayProcessToBeHookedOnStart(aProcess));
        }

        public void RemoveProcessToBeHookedOnStart(IProcess aProcess)
        {
            AddProcessUpdateJob(() => _processesTreeView.RemoveProcessToBeHookedOnStart(aProcess));
        }

        public void DisplayHookedInactiveProcess(IProcess aProcess)
        {
            AddProcessUpdateJob(() => _processesTreeView.DisplayHookedInactiveProcess(aProcess));
        }

        public void DisplayRecentlyHookedProcessAsInactive(IProcess aProcess)
        {
            AddProcessUpdateJob(() => _processesTreeView.DisplayRecentlyHookedProcessAsInactive(aProcess));
        }

        public void DisplayHookedActiveProcess(IProcess aProcess)
        {
            AddProcessUpdateJob(() => _processesTreeView.DisplayHookedActiveProcess(aProcess));
        }

        public void DisplayRecentlyHookedProcessAsActive(IProcess aProcess)
        {
            AddProcessUpdateJob(() => _processesTreeView.DisplayRecentlyHookedProcessAsActive(aProcess));
        }

        public void DisplayUnhookedProcess(IProcess aProcess)
        {
            AddProcessUpdateJob(() => _processesTreeView.DisplayUnHookedProcess(aProcess));
        }

        public void DisplayRecentlyUnhookedProcess(IProcess aProcess)
        {
            AddProcessUpdateJob(() => _processesTreeView.DisplayRecentlyUnhookedProcess(aProcess));
        }

        private void AddProcessUpdateJob(Action anAction)
        {
            _uiUpdater.Add(new TreeViewAdvUpdateJob(_processesTreeView, anAction));
        }

        #endregion

        #region Displaying Modules Information

        public void DisplaySingleProcessModules(IEnumerable<Module> modules)
        {
            AddTreeViewUpdateJob(_modulesTreeView, () => _modulesTreeView.DisplaySingleProcessModules(modules));
        }

        public void DisplayMultipleProcessesModules(IEnumerable<Module> modules)
        {
            AddTreeViewUpdateJob(_modulesTreeView, () => _modulesTreeView.DisplayMultipleProcessesModules(modules));
        }

        public void DisplayModuleFunctions(IEnumerable<Function> functions)
        {
            AddTreeViewUpdateJob(_functionsTreeView, () => _functionsTreeView.Display(functions));
        }

        public void RefreshModulesView()
        {
            AddTreeViewUpdateJob(_modulesTreeView, () => _modulesTreeView.Refresh());
        }

        #endregion

        #region Displaying Hook Information

        public void DisplayHook(IRunningProcess aProcess, Module aModule, Function aFunction, Hook aHook)
        {
        }

        public void DisplayHookStatus(HookStateChangeInfo aHookStateChage)
        {
            AddTreeViewUpdateJob(_hooksTreeView,
                                 () =>
                                 _hooksTreeView.DisplayHook(aHookStateChage.Hook, aHookStateChage.Process,
                                                            aHookStateChage));
        }

        public void RemoveHook(Hook aHook)
        {
            AddTreeViewUpdateJob(_hooksTreeView, () => _hooksTreeView.Remove(aHook));
        }

        private void AddTreeViewUpdateJob(TreeViewAdv aTreeView, Action anAction)
        {
            _uiUpdater.Add(new TreeViewAdvUpdateJob(aTreeView, anAction));
        }

        private void AddTreeViewUpdateJobInOrder(TreeViewAdv aTreeView, Action anAction, int anOrder)
        {
            _uiUpdater.Add(new TreeViewAdvUpdateJob(aTreeView, anAction, anOrder));
        }

        #endregion

        #region Displaying Function Call Information

        public void ClearOutput()
        {
            AddTreeViewUpdateJob(treeViewOutput, () => treeViewOutput.ClearData());
        }

        public void DisplayRootFunctionCall(FunctionCall aFunctionCall)
        {
            AddTreeViewUpdateJob(treeViewOutput, () => treeViewOutput.DisplayRootFunctionCall(aFunctionCall));
        }

        public void DisplayNestedFunctionCall(FunctionCall aFunctionCall)
        {
            AddTreeViewUpdateJob(treeViewOutput, () => treeViewOutput.DisplayNestedFunctionCall(aFunctionCall));
        }

        public void InspectFunctionCall(FunctionCall aFunctionCall)
        {
            FunctionCallInspectorForm.Inspect(aFunctionCall);
        }

        #endregion

        #region Enabling & Disabling UI Elements

        public bool AddHookEnabled
        {
            get { return tsbtnAddHook.Enabled || addHook.Enabled; }
            set
            {
                AddUIUpdateJob(toolBar, () => tsbtnAddHook.Enabled = value);
                AddUIUpdateJob(menuMain, () => addHook.Enabled = value);
            }
        }

        public bool RemoveHookEnabled
        {
            get { return mnuRemoveHook.Enabled && cmiRemoveHook.Enabled && tsbtnRemoveAllHooks.Enabled; }
            set
            {
                AddUIUpdateJob(menuMain, () => mnuRemoveHook.Enabled = value);
                //AddUIUpdateJob(cmToHookOnStart, () => mnuRemoveHook.Enabled = value);
                AddUIUpdateJob(cmHooks, () => cmiRemoveHook.Enabled = value);
                AddUIUpdateJob(toolBar, () => tsbtnRemoveHook.Enabled = value);
            }
        }

        public bool RemoveAllHooksEnabled
        {
            get { return tsbtnRemoveAllHooks.Enabled; }
            set
            {
                AddUIUpdateJob(toolBar, () => tsbtnRemoveAllHooks.Enabled = value);
            }
        }

        public bool ExportHookingSettings
        {
            get { return tsbtnSaveHooks.Enabled; }
            set { AddUIUpdateJob(toolBar, () => tsbtnSaveHooks.Enabled = value); }
        }

        public bool ClearOutputEnabled
        {
            get { return tsbtnClearOutput.Enabled; }
            set { AddUIUpdateJob(toolBar, () => tsbtnClearOutput.Enabled = value); }
        }

        #endregion

        public void UpdateApplicationTitle(string aTitle)
        {
            AddUIUpdateJob(this, () => Text = aTitle);
        }

        public void DisplayToHookOnStartContextMenu()
        {
            Point lastProcessNodeLocation = _lastProcessNodeLocation;
            cmProcessToBeHooked.Show(_processesTreeView, lastProcessNodeLocation);
        }

        public IEnumerable<IRunningProcess> SelectProcessesToApplyHookingRules(IEnumerable<IProcess> toHookOnStart,
                                                                               IEnumerable<IRunningProcess> unhooked,
                                                                               IEnumerable<IRunningProcess>
                                                                                   hookedInactive,
                                                                               IEnumerable<IRunningProcess> hookedActive)
        {
            var selectedProcesses = new IRunningProcess[0];

            var job = new UpdateUIJob(this,
                                      () =>
                                      selectedProcesses =
                                      ProcessSelectionForm.Show(toHookOnStart, unhooked, hookedInactive, hookedActive));
            job.Perform();

            return selectedProcesses;
        }

        #region Messages

        public void ShowError(string aMessage)
        {
            AddUIUpdateJob(this, () => MessageBox.Show(aMessage, Text, MessageBoxButtons.OK, MessageBoxIcon.Warning));
        }

        public void ShowAgentLoadingError(IRunningProcess aProcess, int anErrorCode)
        {
            if (!aProcess.IsAlive(10)) return;

            string errorMessage = string.Format("Deviare Agent load error. Process: {0} ({1}). Error code: 0x{2:X}",
                                                aProcess.Name, aProcess.Id, anErrorCode);
            MessageBox.Show(errorMessage);
        }

        public void UpdateRootFunctionCall(FunctionCall aFunctionCall)
        {
            AddTreeViewUpdateJob(treeViewOutput, () => treeViewOutput.UpdateRootFunctionCall(aFunctionCall));
        }

        public void DisplayContextMenuForFunctions(bool enableRemoveHook, bool enableAddHook)
        {
            cmFunctionsRemoveHook.Enabled = enableRemoveHook;
            cmFunctionsAddHook.Enabled = enableAddHook;
            cmFunctions.Show(_functionsTreeView, _lastFunctionNodeLocation);
        }

        public void DisplayModules(IEnumerable<Module> modules, BackgroundWorker worker)
        {
            AddTreeViewUpdateJob(_modulesTreeView, () => _modulesTreeView.DisplayModulesKeepingSelection(modules, worker));
        }

        public void DisplayModulesInOrder(List<Module> modules, BackgroundWorker worker, int anOrder)
        {
            AddTreeViewUpdateJobInOrder(_modulesTreeView, () => _modulesTreeView.DisplayModulesKeepingSelection(modules, worker), anOrder);
        }

        public void SortModulesViewByColumn(int columnIndex)
        {
            AddTreeViewUpdateJob(_modulesTreeView, () => _modulesTreeView.SortByColumn(columnIndex));
        }

        public void MarkAsSelected(Module module, BackgroundWorker worker, int jobOrder)
        {
            AddTreeViewUpdateJobInOrder(_modulesTreeView, () => _modulesTreeView.MarkAsSelected(module, worker), jobOrder);
        }

        public void MarkAsSelectedIfSelectionIsNull(Module module, BackgroundWorker worker, int jobOrder)
        {
            AddTreeViewUpdateJobInOrder(_modulesTreeView, () => _modulesTreeView.MarkAsSelectedIfSelectionIsNull(module, worker), jobOrder);
        }

        public void SetHookingUnhookingAndUnhookingAllFlags(bool enableAddHook, bool enableRemoveHook, bool enableRemoveAllHooks, BackgroundWorker worker, int jobOrder)
        {
            AddTreeViewUpdateJob(_modulesTreeView, () => _modulesTreeView.SetHookingUnhookingAndUnhookingAllFlags(enableAddHook, enableRemoveHook, enableRemoveAllHooks, worker, jobOrder, this));
        }

        public void CancelModulesViewUpdateFor(BackgroundWorker worker)
        {
            _modulesTreeView.CancelUpdateFor(worker);
        }

        public void RemoveUpdateCancellationStatusByWorker(BackgroundWorker worker)
        {
            AddTreeViewUpdateJob(_modulesTreeView, () => _modulesTreeView.RemoveWorkerCancelStatus(worker));
        }

        public bool ConfirmIfFunctionsShouldBeHookedAgain()
        {
            DialogResult userChoice =
// ReSharper disable LocalizableElement
                MessageBox.Show("At least one function is already hooked. Do you want to hook it again?", Text,
// ReSharper restore LocalizableElement
                                MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            return userChoice == DialogResult.Yes;
        }

        #endregion

        private void AddUIUpdateJob(Control aControl, Action anAction)
        {
            _uiUpdater.Add(new UpdateUIJob(aControl, anAction));
        }

        #endregion

        #region Event Handlers

        private void SelectedProcessChangedHandler(object sender, EventArgs e)
        {
            _processesTreeView.HandleSelectedNode(_presenter);
        }

        private void SelectedModuleChangedHandler(object sender, EventArgs e)
        {
            _modulesTreeView.HandleSelectedNode(node =>
                                                    {
                                                        _presenter.SelectedModuleChanged(node.Module);
                                                        return true;
                                                    },
                                                () => { });
            //_presenter.ClearSelectedModule);
        }

        private void SelectedFunctionChangedHandler(object sender, EventArgs e)
        {
            _presenter.SelectedFunctionsChanged(_functionsTreeView.SelectedFunctions().ToArray());
        }

        private void SelectedHookChangedHandler(object sender, EventArgs e)
        {
            _presenter.SelectedHookChanged(_hooksTreeView.SelectedHooks);
        }

        private void SelectedFunctionOutputChangedHandler(object sender, TreeNodeAdvMouseEventArgs e)
        {
            var anOutputNode = (OutputNode) e.Node.Tag;
            _presenter.FunctionCallDoubleClicked(anOutputNode.FunctionCall);
        }

        private void ProcessNodeMouseClickHandler(object sender, TreeNodeAdvMouseEventArgs e)
        {
            if (e.Button != MouseButtons.Right) return;
            if (_processesTreeView.ToHookOnStartGroupIsSelected())
            {
                cmToHookOnStart.Show(_processesTreeView, e.Location);
                return;
            }

            _lastProcessNodeLocation = e.Location;
            _presenter.ProcessRightClick();
        }

        private void SettingsMenuClickedHandler(object sender, EventArgs e)
        {
            new SettingsForm().ShowDialog();
            _presenter.DisplaySelectedModuleFunctions();
        }

        private void AddHookHandler(object sender, EventArgs e)
        {
            _presenter.AddHook();
        }

        private void RemoveHookHandler(object sender, EventArgs e)
        {
            if (_hooksTreeView.Focused)
                _presenter.RemoveSelectedHooks();
            if (_functionsTreeView.Focused)
                _presenter.RemoveAllHooksForSelectedFunction();
        }

        private void ClearOutputHandler(object sender, EventArgs e)
        {
            _presenter.ClearOutput();
        }

        private void FormClosedHandler(object sender, FormClosedEventArgs e)
        {
            _uiUpdater.RemoveAllPendingJobs();
            _presenter.Close();
        }

        private void MiStartProcessClickedHandler(object sender, EventArgs e)
        {
            _presenter.StartSelectedProcess();
        }

        private void LoadHooksInBackground(object sender, DoWorkEventArgs e)
        {
            var path = openHookingSettingsDialog.FileName;
            using (var file = File.OpenRead(path))
            {
                _presenter.ImportHookingSettings(file);
            }
        }

        #region Drag And Drop

        private void ProcessesTreeViewItemDrag(object sender, ItemDragEventArgs e)
        {
            DoNodeDragDrop(e);
        }

        private void HooksTreeViewItemDrag(object sender, ItemDragEventArgs e)
        {
            DoNodeDragDrop(e);
        }

        private void DoNodeDragDrop(ItemDragEventArgs e)
        {
            if (!(e.Item is TreeNodeAdv[])) return;

            var node = ((TreeNodeAdv[]) e.Item)[0];
            DoDragDrop(node, DragDropEffects.Link | DragDropEffects.Scroll);
        }

        private void HooksTreeViewDragEnter(object sender, DragEventArgs e)
        {
            SetDragDropMode(e);
        }

        private void ProcessesTreeViewDragEnter(object sender, DragEventArgs e)
        {
            SetDragDropMode(e);
        }

        private static void SetDragDropMode(DragEventArgs e)
        {
            var data = new DataObject(new TreeNodeAdv(new Node()));

            if (e.Data.GetDataPresent(data.GetFormats()[0]))
            {
                e.Effect = DragDropEffects.Link;
                return;
            }

            e.Effect = DragDropEffects.None;
        }

        private void HooksTreeViewDragDrop(object sender, DragEventArgs e)
        {
            var data = new DataObject(new TreeNodeAdv(new Node()));

            if (!e.Data.GetDataPresent(data.GetFormats()[0])) return;

            var draggedTreeNode = (TreeNodeAdv)e.Data.GetData(data.GetFormats()[0]);
            var draggedNode = (DeviareNode)draggedTreeNode.Tag;

            var targetTreeNode = _hooksTreeView.DropPosition.Node;
            var targetNode = (DeviareNode)targetTreeNode.Tag;

            var processCollector = new Collector();
            draggedNode.Accept(processCollector);

            var hookCollector = new Collector();
            targetNode.Accept(hookCollector);

            _presenter.ReplicateHooks(processCollector.CollectedProcesses, hookCollector.CollectedHooks);
        }

        private void ProcessesTreeViewDragDrop(object sender, DragEventArgs e)
        {
            var data = new DataObject(new TreeNodeAdv(new Node()));

            if (!e.Data.GetDataPresent(data.GetFormats()[0])) return;

            var draggedTreeNode = (TreeNodeAdv)e.Data.GetData(data.GetFormats()[0]);
            var draggedNode = (DeviareNode)draggedTreeNode.Tag;

            var targetTreeNode = _processesTreeView.DropPosition.Node;
            var targetNode = (DeviareNode)targetTreeNode.Tag;

            var processCollector = new Collector();
            targetNode.Accept(processCollector);

            var hookCollector = new Collector();
            draggedNode.Accept(hookCollector);

            _presenter.ReplicateHooks(processCollector.CollectedProcesses, hookCollector.CollectedHooks);
        }

        #endregion

        #endregion

        private void RemoveAllHooksHandler(object sender, EventArgs e)
        {
            _presenter.RemoveAllHooks();
        }

        private void ToolStripButton1Click(object sender, EventArgs e)
        {
            if (saveFileDialog.ShowDialog() != DialogResult.OK) return;
            var exportedSettings = _presenter.ExportHookingSettings();
            File.WriteAllText(saveFileDialog.FileName, exportedSettings.ToXml());
        }

        private void TsbtnImportHooksClick(object sender, EventArgs e)
        {
            if (openHookingSettingsDialog.ShowDialog() != DialogResult.OK) return;

            hookLoaderWorker.RunWorkerAsync(openHookingSettingsDialog.FileName);
        }

        private void MiAddProcessClick(object sender, EventArgs e)
        {
            if (addProcessToBeHookedDialog.ShowDialog() != DialogResult.OK) return;
            _presenter.AddProcessToBeHooked(addProcessToBeHookedDialog.FileName);
        }

        private void FunctionsTreeViewNodeMouseClick(object sender, TreeNodeAdvMouseEventArgs e)
        {
            if (e.Button != MouseButtons.Right) return;
            _lastFunctionNodeLocation = e.Location;
            _presenter.FunctionRightClick();
        }

        private void ToolStripMenuItem2Click(object sender, EventArgs e)
        {
            _presenter.RemoveAllHooksForSelectedFunction();
        }

        private void SelectAllToolStripMenuItemClick(object sender, EventArgs e)
        {
            if (_functionsTreeView.Focused)
                _functionsTreeView.SelectAllNodes();
        }

        private void RefreshToolStripMenuItemClick(object sender, EventArgs e)
        {
            if (_modulesTreeView.Focused)
                _presenter.DisplaySelectedProcessesModulesKeepingSelection();
        }
    }
}