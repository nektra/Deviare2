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

using Aga.Controls.Tree;
using CSharpConsole.Controls;
using CSharpConsole.Model.Process;

namespace CSharpConsole.Views
{
    partial  class DeviareConsoleForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DeviareConsoleForm));
            this.splitContainerMain = new System.Windows.Forms.SplitContainer();
            this.splitContainerTop = new System.Windows.Forms.SplitContainer();
            this._processesTreeView = new CSharpConsole.Controls.ProcessesTreeView();
            this.splitContainerRight = new System.Windows.Forms.SplitContainer();
            this._modulesTreeView = new CSharpConsole.Controls.ModulesTreeView();
            this.treeModuleName = new Aga.Controls.Tree.TreeColumn();
            this.treeModuleAddress = new Aga.Controls.Tree.TreeColumn();
            this.treeModuleHooksCount = new Aga.Controls.Tree.TreeColumn();
            this.nodeModuleName = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.nodeModuleAddress = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.nodeModuleHooksCount = new Aga.Controls.Tree.NodeControls.NodeIntegerTextBox();
            this._functionsTreeView = new CSharpConsole.Controls.FunctionsTreeView();
            this.tcFunctionIcon = new Aga.Controls.Tree.TreeColumn();
            this.tcFunctionReturnType = new Aga.Controls.Tree.TreeColumn();
            this.treeFunctionName = new Aga.Controls.Tree.TreeColumn();
            this.tcFunctionParameters = new Aga.Controls.Tree.TreeColumn();
            this.treeFunctionAddress = new Aga.Controls.Tree.TreeColumn();
            this.ncFunctionIcon = new Aga.Controls.Tree.NodeControls.NodeIcon();
            this.ncFunctionReturnType = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.nodeFunctionName = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.ncFunctionParameters = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.nodeFunctionAddress = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.splitContainerBottom = new System.Windows.Forms.SplitContainer();
            this._hooksTreeView = new CSharpConsole.Controls.HooksTreeView();
            this.treeHook = new Aga.Controls.Tree.TreeColumn();
            this.cmHooks = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.cmiRemoveHook = new System.Windows.Forms.ToolStripMenuItem();
            this.nodeHook = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.treeViewOutput = new CSharpConsole.Controls.TreeViewOutput();
            this.tcOutputOrder = new Aga.Controls.Tree.TreeColumn();
            this.tcOutputTid = new Aga.Controls.Tree.TreeColumn();
            this.tcOutputModuleName = new Aga.Controls.Tree.TreeColumn();
            this.tcOutputFunctionResult = new Aga.Controls.Tree.TreeColumn();
            this.tcOutputFunctionName = new Aga.Controls.Tree.TreeColumn();
            this.tcOutputParameters = new Aga.Controls.Tree.TreeColumn();
            this.ncOutputOrder = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.ncOutputTid = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.ncOutputModuleName = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.ncOutputFunctionResult = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.ncOutputFunctionName = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.ncOutputParameters = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.cmProcessToBeHooked = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.miStartProcess = new System.Windows.Forms.ToolStripMenuItem();
            this.menuMain = new System.Windows.Forms.MenuStrip();
            this.hookToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addHook = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuRemoveHook = new System.Windows.Forms.ToolStripMenuItem();
            this.settingsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.selectAllToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.refreshToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolBar = new System.Windows.Forms.ToolStrip();
            this.tsbtnAddHook = new System.Windows.Forms.ToolStripButton();
            this.tsbtnRemoveHook = new System.Windows.Forms.ToolStripButton();
            this.tsbtnRemoveAllHooks = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.tsbtnClearOutput = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.tsbtnImportHooks = new System.Windows.Forms.ToolStripButton();
            this.tsbtnSaveHooks = new System.Windows.Forms.ToolStripButton();
            this.saveFileDialog = new System.Windows.Forms.SaveFileDialog();
            this.openHookingSettingsDialog = new System.Windows.Forms.OpenFileDialog();
            this.cmToHookOnStart = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.miAddProcess = new System.Windows.Forms.ToolStripMenuItem();
            this.hookLoaderWorker = new System.ComponentModel.BackgroundWorker();
            this.cmFunctions = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.cmFunctionsAddHook = new System.Windows.Forms.ToolStripMenuItem();
            this.cmFunctionsRemoveHook = new System.Windows.Forms.ToolStripMenuItem();
            this.addProcessToBeHookedDialog = new System.Windows.Forms.OpenFileDialog();
            this.splitContainerMain.Panel1.SuspendLayout();
            this.splitContainerMain.Panel2.SuspendLayout();
            this.splitContainerMain.SuspendLayout();
            this.splitContainerTop.Panel1.SuspendLayout();
            this.splitContainerTop.Panel2.SuspendLayout();
            this.splitContainerTop.SuspendLayout();
            this.splitContainerRight.Panel1.SuspendLayout();
            this.splitContainerRight.Panel2.SuspendLayout();
            this.splitContainerRight.SuspendLayout();
            this.splitContainerBottom.Panel1.SuspendLayout();
            this.splitContainerBottom.Panel2.SuspendLayout();
            this.splitContainerBottom.SuspendLayout();
            this.cmHooks.SuspendLayout();
            this.cmProcessToBeHooked.SuspendLayout();
            this.menuMain.SuspendLayout();
            this.toolBar.SuspendLayout();
            this.cmToHookOnStart.SuspendLayout();
            this.cmFunctions.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainerMain
            // 
            this.splitContainerMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainerMain.Location = new System.Drawing.Point(0, 49);
            this.splitContainerMain.Name = "splitContainerMain";
            this.splitContainerMain.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainerMain.Panel1
            // 
            this.splitContainerMain.Panel1.Controls.Add(this.splitContainerTop);
            // 
            // splitContainerMain.Panel2
            // 
            this.splitContainerMain.Panel2.Controls.Add(this.splitContainerBottom);
            this.splitContainerMain.Size = new System.Drawing.Size(1007, 530);
            this.splitContainerMain.SplitterDistance = 270;
            this.splitContainerMain.TabIndex = 3;
            // 
            // splitContainerTop
            // 
            this.splitContainerTop.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainerTop.Location = new System.Drawing.Point(0, 0);
            this.splitContainerTop.Name = "splitContainerTop";
            // 
            // splitContainerTop.Panel1
            // 
            this.splitContainerTop.Panel1.Controls.Add(this._processesTreeView);
            // 
            // splitContainerTop.Panel2
            // 
            this.splitContainerTop.Panel2.Controls.Add(this.splitContainerRight);
            this.splitContainerTop.Size = new System.Drawing.Size(1007, 270);
            this.splitContainerTop.SplitterDistance = 271;
            this.splitContainerTop.TabIndex = 0;
            // 
            // _processesTreeView
            // 
            this._processesTreeView.AllowDrop = true;
            this._processesTreeView.AutoRowHeight = true;
            this._processesTreeView.BackColor = System.Drawing.SystemColors.Window;
            this._processesTreeView.DefaultToolTipProvider = null;
            this._processesTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this._processesTreeView.DragDropMarkColor = System.Drawing.Color.Black;
            this._processesTreeView.FullRowSelect = true;
            this._processesTreeView.LineColor = System.Drawing.SystemColors.ControlDark;
            this._processesTreeView.Location = new System.Drawing.Point(0, 0);
            this._processesTreeView.Model = null;
            this._processesTreeView.Name = "_processesTreeView";
            this._processesTreeView.SelectedNode = null;
            this._processesTreeView.Size = new System.Drawing.Size(271, 270);
            this._processesTreeView.TabIndex = 0;
            this._processesTreeView.UseColumns = true;
            this._processesTreeView.SelectionChanged += new System.EventHandler(this.SelectedProcessChangedHandler);
            this._processesTreeView.DragDrop += new System.Windows.Forms.DragEventHandler(this.ProcessesTreeViewDragDrop);
            this._processesTreeView.DragEnter += new System.Windows.Forms.DragEventHandler(this.ProcessesTreeViewDragEnter);
            this._processesTreeView.NodeMouseClick += new System.EventHandler<Aga.Controls.Tree.TreeNodeAdvMouseEventArgs>(this.ProcessNodeMouseClickHandler);
            this._processesTreeView.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.ProcessesTreeViewItemDrag);
            // 
            // splitContainerRight
            // 
            this.splitContainerRight.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainerRight.Location = new System.Drawing.Point(0, 0);
            this.splitContainerRight.Name = "splitContainerRight";
            // 
            // splitContainerRight.Panel1
            // 
            this.splitContainerRight.Panel1.Controls.Add(this._modulesTreeView);
            // 
            // splitContainerRight.Panel2
            // 
            this.splitContainerRight.Panel2.Controls.Add(this._functionsTreeView);
            this.splitContainerRight.Size = new System.Drawing.Size(732, 270);
            this.splitContainerRight.SplitterDistance = 338;
            this.splitContainerRight.TabIndex = 0;
            // 
            // _modulesTreeView
            // 
            this._modulesTreeView.BackColor = System.Drawing.SystemColors.Window;
            this._modulesTreeView.Columns.Add(this.treeModuleName);
            this._modulesTreeView.Columns.Add(this.treeModuleAddress);
            this._modulesTreeView.Columns.Add(this.treeModuleHooksCount);
            this._modulesTreeView.DefaultToolTipProvider = null;
            this._modulesTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this._modulesTreeView.DragDropMarkColor = System.Drawing.Color.Black;
            this._modulesTreeView.FullRowSelect = true;
            this._modulesTreeView.LineColor = System.Drawing.SystemColors.ControlDark;
            this._modulesTreeView.Location = new System.Drawing.Point(0, 0);
            this._modulesTreeView.Model = null;
            this._modulesTreeView.Name = "_modulesTreeView";
            this._modulesTreeView.NodeControls.Add(this.nodeModuleName);
            this._modulesTreeView.NodeControls.Add(this.nodeModuleAddress);
            this._modulesTreeView.NodeControls.Add(this.nodeModuleHooksCount);
            this._modulesTreeView.SelectedNode = null;
            this._modulesTreeView.Size = new System.Drawing.Size(338, 270);
            this._modulesTreeView.TabIndex = 0;
            this._modulesTreeView.UseColumns = true;
            this._modulesTreeView.SelectionChanged += new System.EventHandler(this.SelectedModuleChangedHandler);
            // 
            // treeModuleName
            // 
            this.treeModuleName.Header = "Name";
            this.treeModuleName.MinColumnWidth = 200;
            this.treeModuleName.Sortable = true;
            this.treeModuleName.SortOrder = System.Windows.Forms.SortOrder.Ascending;
            this.treeModuleName.TooltipText = null;
            this.treeModuleName.Width = 200;
            // 
            // treeModuleAddress
            // 
            this.treeModuleAddress.Header = "Address";
            this.treeModuleAddress.Sortable = true;
            this.treeModuleAddress.SortOrder = System.Windows.Forms.SortOrder.None;
            this.treeModuleAddress.TooltipText = null;
            this.treeModuleAddress.Width = 80;
            // 
            // treeModuleHooksCount
            // 
            this.treeModuleHooksCount.Header = "Hooks";
            this.treeModuleHooksCount.Sortable = true;
            this.treeModuleHooksCount.SortOrder = System.Windows.Forms.SortOrder.None;
            this.treeModuleHooksCount.TooltipText = null;
            // 
            // nodeModuleName
            // 
            this.nodeModuleName.DataPropertyName = "Text";
            this.nodeModuleName.IncrementalSearchEnabled = true;
            this.nodeModuleName.LeftMargin = 3;
            this.nodeModuleName.ParentColumn = this.treeModuleName;
            // 
            // nodeModuleAddress
            // 
            this.nodeModuleAddress.DataPropertyName = "Address";
            this.nodeModuleAddress.IncrementalSearchEnabled = true;
            this.nodeModuleAddress.LeftMargin = 3;
            this.nodeModuleAddress.ParentColumn = this.treeModuleAddress;
            this.nodeModuleAddress.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // nodeModuleHooksCount
            // 
            this.nodeModuleHooksCount.AllowNegativeSign = false;
            this.nodeModuleHooksCount.DataPropertyName = "HooksCount";
            this.nodeModuleHooksCount.IncrementalSearchEnabled = true;
            this.nodeModuleHooksCount.LeftMargin = 3;
            this.nodeModuleHooksCount.ParentColumn = this.treeModuleHooksCount;
            this.nodeModuleHooksCount.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // _functionsTreeView
            // 
            this._functionsTreeView.BackColor = System.Drawing.SystemColors.Window;
            this._functionsTreeView.Columns.Add(this.tcFunctionIcon);
            this._functionsTreeView.Columns.Add(this.tcFunctionReturnType);
            this._functionsTreeView.Columns.Add(this.treeFunctionName);
            this._functionsTreeView.Columns.Add(this.tcFunctionParameters);
            this._functionsTreeView.Columns.Add(this.treeFunctionAddress);
            this._functionsTreeView.DefaultToolTipProvider = null;
            this._functionsTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this._functionsTreeView.DragDropMarkColor = System.Drawing.Color.Black;
            this._functionsTreeView.LineColor = System.Drawing.SystemColors.ControlDark;
            this._functionsTreeView.Location = new System.Drawing.Point(0, 0);
            this._functionsTreeView.Model = null;
            this._functionsTreeView.Name = "_functionsTreeView";
            this._functionsTreeView.NodeControls.Add(this.ncFunctionIcon);
            this._functionsTreeView.NodeControls.Add(this.ncFunctionReturnType);
            this._functionsTreeView.NodeControls.Add(this.nodeFunctionName);
            this._functionsTreeView.NodeControls.Add(this.ncFunctionParameters);
            this._functionsTreeView.NodeControls.Add(this.nodeFunctionAddress);
            this._functionsTreeView.SelectedNode = null;
            this._functionsTreeView.SelectionMode = Aga.Controls.Tree.TreeSelectionMode.Multi;
            this._functionsTreeView.Size = new System.Drawing.Size(390, 270);
            this._functionsTreeView.TabIndex = 0;
            this._functionsTreeView.UseColumns = true;
            this._functionsTreeView.SelectionChanged += new System.EventHandler(this.SelectedFunctionChangedHandler);
            this._functionsTreeView.NodeMouseClick += new System.EventHandler<Aga.Controls.Tree.TreeNodeAdvMouseEventArgs>(this.FunctionsTreeViewNodeMouseClick);
            // 
            // tcFunctionIcon
            // 
            this.tcFunctionIcon.Header = "";
            this.tcFunctionIcon.SortOrder = System.Windows.Forms.SortOrder.None;
            this.tcFunctionIcon.TooltipText = null;
            this.tcFunctionIcon.Width = 64;
            // 
            // tcFunctionReturnType
            // 
            this.tcFunctionReturnType.Header = "Return Type";
            this.tcFunctionReturnType.MinColumnWidth = 80;
            this.tcFunctionReturnType.SortOrder = System.Windows.Forms.SortOrder.None;
            this.tcFunctionReturnType.TooltipText = null;
            this.tcFunctionReturnType.Width = 81;
            // 
            // treeFunctionName
            // 
            this.treeFunctionName.Header = "Name";
            this.treeFunctionName.MinColumnWidth = 200;
            this.treeFunctionName.SortOrder = System.Windows.Forms.SortOrder.None;
            this.treeFunctionName.TooltipText = null;
            this.treeFunctionName.Width = 200;
            // 
            // tcFunctionParameters
            // 
            this.tcFunctionParameters.Header = "Parameters";
            this.tcFunctionParameters.MinColumnWidth = 300;
            this.tcFunctionParameters.SortOrder = System.Windows.Forms.SortOrder.None;
            this.tcFunctionParameters.TooltipText = null;
            this.tcFunctionParameters.Width = 300;
            // 
            // treeFunctionAddress
            // 
            this.treeFunctionAddress.Header = "Address";
            this.treeFunctionAddress.MinColumnWidth = 80;
            this.treeFunctionAddress.SortOrder = System.Windows.Forms.SortOrder.None;
            this.treeFunctionAddress.TooltipText = null;
            this.treeFunctionAddress.Width = 80;
            // 
            // ncFunctionIcon
            // 
            this.ncFunctionIcon.DataPropertyName = "Icon";
            this.ncFunctionIcon.LeftMargin = 1;
            this.ncFunctionIcon.ParentColumn = this.tcFunctionIcon;
            this.ncFunctionIcon.ScaleMode = Aga.Controls.Tree.ImageScaleMode.Clip;
            // 
            // ncFunctionReturnType
            // 
            this.ncFunctionReturnType.DataPropertyName = "ReturnType";
            this.ncFunctionReturnType.IncrementalSearchEnabled = true;
            this.ncFunctionReturnType.LeftMargin = 3;
            this.ncFunctionReturnType.ParentColumn = this.tcFunctionReturnType;
            // 
            // nodeFunctionName
            // 
            this.nodeFunctionName.DataPropertyName = "FunctionName";
            this.nodeFunctionName.IncrementalSearchEnabled = true;
            this.nodeFunctionName.LeftMargin = 3;
            this.nodeFunctionName.ParentColumn = this.treeFunctionName;
            // 
            // ncFunctionParameters
            // 
            this.ncFunctionParameters.DataPropertyName = "Parameters";
            this.ncFunctionParameters.IncrementalSearchEnabled = true;
            this.ncFunctionParameters.LeftMargin = 3;
            this.ncFunctionParameters.ParentColumn = this.tcFunctionParameters;
            // 
            // nodeFunctionAddress
            // 
            this.nodeFunctionAddress.DataPropertyName = "Address";
            this.nodeFunctionAddress.IncrementalSearchEnabled = true;
            this.nodeFunctionAddress.LeftMargin = 3;
            this.nodeFunctionAddress.ParentColumn = this.treeFunctionAddress;
            // 
            // splitContainerBottom
            // 
            this.splitContainerBottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainerBottom.Location = new System.Drawing.Point(0, 0);
            this.splitContainerBottom.Name = "splitContainerBottom";
            // 
            // splitContainerBottom.Panel1
            // 
            this.splitContainerBottom.Panel1.Controls.Add(this._hooksTreeView);
            // 
            // splitContainerBottom.Panel2
            // 
            this.splitContainerBottom.Panel2.Controls.Add(this.treeViewOutput);
            this.splitContainerBottom.Size = new System.Drawing.Size(1007, 256);
            this.splitContainerBottom.SplitterDistance = 415;
            this.splitContainerBottom.TabIndex = 0;
            // 
            // _hooksTreeView
            // 
            this._hooksTreeView.AllowDrop = true;
            this._hooksTreeView.BackColor = System.Drawing.SystemColors.Window;
            this._hooksTreeView.Columns.Add(this.treeHook);
            this._hooksTreeView.ContextMenuStrip = this.cmHooks;
            this._hooksTreeView.DefaultToolTipProvider = null;
            this._hooksTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this._hooksTreeView.DragDropMarkColor = System.Drawing.Color.Black;
            this._hooksTreeView.FullRowSelect = true;
            this._hooksTreeView.LineColor = System.Drawing.SystemColors.ControlDark;
            this._hooksTreeView.Location = new System.Drawing.Point(0, 0);
            this._hooksTreeView.Model = null;
            this._hooksTreeView.Name = "_hooksTreeView";
            this._hooksTreeView.NodeControls.Add(this.nodeHook);
            this._hooksTreeView.SelectedNode = null;
            this._hooksTreeView.Size = new System.Drawing.Size(415, 256);
            this._hooksTreeView.TabIndex = 0;
            this._hooksTreeView.UseColumns = true;
            this._hooksTreeView.SelectionChanged += new System.EventHandler(this.SelectedHookChangedHandler);
            this._hooksTreeView.DragDrop += new System.Windows.Forms.DragEventHandler(this.HooksTreeViewDragDrop);
            this._hooksTreeView.DragEnter += new System.Windows.Forms.DragEventHandler(this.HooksTreeViewDragEnter);
            this._hooksTreeView.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.HooksTreeViewItemDrag);
            // 
            // treeHook
            // 
            this.treeHook.Header = "Hook";
            this.treeHook.SortOrder = System.Windows.Forms.SortOrder.None;
            this.treeHook.TooltipText = null;
            this.treeHook.Width = 350;
            // 
            // cmHooks
            // 
            this.cmHooks.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.cmiRemoveHook});
            this.cmHooks.Name = "cmHooks";
            this.cmHooks.Size = new System.Drawing.Size(118, 26);
            this.cmHooks.Text = "Remove";
            // 
            // cmiRemoveHook
            // 
            this.cmiRemoveHook.Image = global::CSharpConsole.Properties.Resources._112_Minus_Orange_16x16_72;
            this.cmiRemoveHook.Name = "cmiRemoveHook";
            this.cmiRemoveHook.Size = new System.Drawing.Size(117, 22);
            this.cmiRemoveHook.Text = "Remove";
            this.cmiRemoveHook.Click += new System.EventHandler(this.RemoveHookHandler);
            // 
            // nodeHook
            // 
            this.nodeHook.DataPropertyName = "Text";
            this.nodeHook.IncrementalSearchEnabled = true;
            this.nodeHook.LeftMargin = 3;
            this.nodeHook.ParentColumn = this.treeHook;
            // 
            // treeViewOutput
            // 
            this.treeViewOutput.AutoRowHeight = true;
            this.treeViewOutput.BackColor = System.Drawing.SystemColors.Window;
            this.treeViewOutput.Columns.Add(this.tcOutputOrder);
            this.treeViewOutput.Columns.Add(this.tcOutputTid);
            this.treeViewOutput.Columns.Add(this.tcOutputModuleName);
            this.treeViewOutput.Columns.Add(this.tcOutputFunctionResult);
            this.treeViewOutput.Columns.Add(this.tcOutputFunctionName);
            this.treeViewOutput.Columns.Add(this.tcOutputParameters);
            this.treeViewOutput.DefaultToolTipProvider = null;
            this.treeViewOutput.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeViewOutput.DragDropMarkColor = System.Drawing.Color.Black;
            this.treeViewOutput.LineColor = System.Drawing.SystemColors.ControlDark;
            this.treeViewOutput.Location = new System.Drawing.Point(0, 0);
            this.treeViewOutput.Model = null;
            this.treeViewOutput.Name = "treeViewOutput";
            this.treeViewOutput.NodeControls.Add(this.ncOutputOrder);
            this.treeViewOutput.NodeControls.Add(this.ncOutputTid);
            this.treeViewOutput.NodeControls.Add(this.ncOutputModuleName);
            this.treeViewOutput.NodeControls.Add(this.ncOutputFunctionResult);
            this.treeViewOutput.NodeControls.Add(this.ncOutputFunctionName);
            this.treeViewOutput.NodeControls.Add(this.ncOutputParameters);
            this.treeViewOutput.SelectedNode = null;
            this.treeViewOutput.Size = new System.Drawing.Size(588, 256);
            this.treeViewOutput.TabIndex = 0;
            this.treeViewOutput.UseColumns = true;
            this.treeViewOutput.NodeMouseDoubleClick += new System.EventHandler<Aga.Controls.Tree.TreeNodeAdvMouseEventArgs>(this.SelectedFunctionOutputChangedHandler);
            // 
            // tcOutputOrder
            // 
            this.tcOutputOrder.Header = "Order";
            this.tcOutputOrder.SortOrder = System.Windows.Forms.SortOrder.None;
            this.tcOutputOrder.TooltipText = null;
            this.tcOutputOrder.Width = 100;
            // 
            // tcOutputTid
            // 
            this.tcOutputTid.Header = "TID";
            this.tcOutputTid.MinColumnWidth = 80;
            this.tcOutputTid.SortOrder = System.Windows.Forms.SortOrder.None;
            this.tcOutputTid.TooltipText = null;
            this.tcOutputTid.Width = 100;
            // 
            // tcOutputModuleName
            // 
            this.tcOutputModuleName.Header = "Module";
            this.tcOutputModuleName.MinColumnWidth = 80;
            this.tcOutputModuleName.SortOrder = System.Windows.Forms.SortOrder.None;
            this.tcOutputModuleName.TooltipText = null;
            this.tcOutputModuleName.Width = 200;
            // 
            // tcOutputFunctionResult
            // 
            this.tcOutputFunctionResult.Header = "Result";
            this.tcOutputFunctionResult.MinColumnWidth = 80;
            this.tcOutputFunctionResult.SortOrder = System.Windows.Forms.SortOrder.None;
            this.tcOutputFunctionResult.TooltipText = null;
            this.tcOutputFunctionResult.Width = 80;
            // 
            // tcOutputFunctionName
            // 
            this.tcOutputFunctionName.Header = "Function Name";
            this.tcOutputFunctionName.MinColumnWidth = 80;
            this.tcOutputFunctionName.SortOrder = System.Windows.Forms.SortOrder.None;
            this.tcOutputFunctionName.TooltipText = null;
            this.tcOutputFunctionName.Width = 200;
            // 
            // tcOutputParameters
            // 
            this.tcOutputParameters.Header = "Arguments";
            this.tcOutputParameters.MinColumnWidth = 80;
            this.tcOutputParameters.SortOrder = System.Windows.Forms.SortOrder.None;
            this.tcOutputParameters.TooltipText = null;
            this.tcOutputParameters.Width = 300;
            // 
            // ncOutputOrder
            // 
            this.ncOutputOrder.DataPropertyName = "Order";
            this.ncOutputOrder.IncrementalSearchEnabled = true;
            this.ncOutputOrder.LeftMargin = 3;
            this.ncOutputOrder.ParentColumn = this.tcOutputOrder;
            // 
            // ncOutputTid
            // 
            this.ncOutputTid.DataPropertyName = "Tid";
            this.ncOutputTid.IncrementalSearchEnabled = true;
            this.ncOutputTid.LeftMargin = 3;
            this.ncOutputTid.ParentColumn = this.tcOutputTid;
            // 
            // ncOutputModuleName
            // 
            this.ncOutputModuleName.DataPropertyName = "ModuleName";
            this.ncOutputModuleName.IncrementalSearchEnabled = true;
            this.ncOutputModuleName.LeftMargin = 3;
            this.ncOutputModuleName.ParentColumn = this.tcOutputModuleName;
            // 
            // ncOutputFunctionResult
            // 
            this.ncOutputFunctionResult.DataPropertyName = "Result";
            this.ncOutputFunctionResult.IncrementalSearchEnabled = true;
            this.ncOutputFunctionResult.LeftMargin = 3;
            this.ncOutputFunctionResult.ParentColumn = this.tcOutputFunctionResult;
            // 
            // ncOutputFunctionName
            // 
            this.ncOutputFunctionName.DataPropertyName = "FunctionName";
            this.ncOutputFunctionName.IncrementalSearchEnabled = true;
            this.ncOutputFunctionName.LeftMargin = 3;
            this.ncOutputFunctionName.ParentColumn = this.tcOutputFunctionName;
            // 
            // ncOutputParameters
            // 
            this.ncOutputParameters.DataPropertyName = "Parameters";
            this.ncOutputParameters.IncrementalSearchEnabled = true;
            this.ncOutputParameters.LeftMargin = 3;
            this.ncOutputParameters.ParentColumn = this.tcOutputParameters;
            // 
            // cmProcessToBeHooked
            // 
            this.cmProcessToBeHooked.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.miStartProcess});
            this.cmProcessToBeHooked.Name = "cmToHookOnStart";
            this.cmProcessToBeHooked.Size = new System.Drawing.Size(142, 26);
            this.cmProcessToBeHooked.Text = "Start";
            // 
            // miStartProcess
            // 
            this.miStartProcess.Image = global::CSharpConsole.Properties.Resources.Run;
            this.miStartProcess.Name = "miStartProcess";
            this.miStartProcess.Size = new System.Drawing.Size(141, 22);
            this.miStartProcess.Text = "Start Process";
            this.miStartProcess.Click += new System.EventHandler(this.MiStartProcessClickedHandler);
            // 
            // menuMain
            // 
            this.menuMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.hookToolStripMenuItem,
            this.settingsToolStripMenuItem,
            this.editToolStripMenuItem,
            this.viewToolStripMenuItem});
            this.menuMain.Location = new System.Drawing.Point(0, 0);
            this.menuMain.Name = "menuMain";
            this.menuMain.Size = new System.Drawing.Size(1007, 24);
            this.menuMain.TabIndex = 1;
            this.menuMain.Text = "menuStrip1";
            // 
            // hookToolStripMenuItem
            // 
            this.hookToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addHook,
            this.mnuRemoveHook});
            this.hookToolStripMenuItem.Name = "hookToolStripMenuItem";
            this.hookToolStripMenuItem.Size = new System.Drawing.Size(48, 20);
            this.hookToolStripMenuItem.Text = "Hook";
            // 
            // addHook
            // 
            this.addHook.Name = "addHook";
            this.addHook.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.H)));
            this.addHook.Size = new System.Drawing.Size(191, 22);
            this.addHook.Text = "Add Hook";
            this.addHook.Click += new System.EventHandler(this.AddHookHandler);
            // 
            // mnuRemoveHook
            // 
            this.mnuRemoveHook.Name = "mnuRemoveHook";
            this.mnuRemoveHook.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.U)));
            this.mnuRemoveHook.Size = new System.Drawing.Size(191, 22);
            this.mnuRemoveHook.Text = "Remove Hook";
            this.mnuRemoveHook.Click += new System.EventHandler(this.RemoveHookHandler);
            // 
            // settingsToolStripMenuItem
            // 
            this.settingsToolStripMenuItem.Name = "settingsToolStripMenuItem";
            this.settingsToolStripMenuItem.Size = new System.Drawing.Size(61, 20);
            this.settingsToolStripMenuItem.Text = "Settings";
            this.settingsToolStripMenuItem.Click += new System.EventHandler(this.SettingsMenuClickedHandler);
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.selectAllToolStripMenuItem});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.Size = new System.Drawing.Size(39, 20);
            this.editToolStripMenuItem.Text = "Edit";
            // 
            // selectAllToolStripMenuItem
            // 
            this.selectAllToolStripMenuItem.Name = "selectAllToolStripMenuItem";
            this.selectAllToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.A)));
            this.selectAllToolStripMenuItem.Size = new System.Drawing.Size(162, 22);
            this.selectAllToolStripMenuItem.Text = "Select all";
            this.selectAllToolStripMenuItem.Click += new System.EventHandler(this.SelectAllToolStripMenuItemClick);
            // 
            // viewToolStripMenuItem
            // 
            this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.refreshToolStripMenuItem});
            this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
            this.viewToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.viewToolStripMenuItem.Text = "View";
            // 
            // refreshToolStripMenuItem
            // 
            this.refreshToolStripMenuItem.Name = "refreshToolStripMenuItem";
            this.refreshToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F5;
            this.refreshToolStripMenuItem.Size = new System.Drawing.Size(132, 22);
            this.refreshToolStripMenuItem.Text = "Refresh";
            this.refreshToolStripMenuItem.Click += new System.EventHandler(this.RefreshToolStripMenuItemClick);
            // 
            // toolBar
            // 
            this.toolBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsbtnAddHook,
            this.tsbtnRemoveHook,
            this.tsbtnRemoveAllHooks,
            this.toolStripSeparator2,
            this.tsbtnClearOutput,
            this.toolStripSeparator1,
            this.tsbtnImportHooks,
            this.tsbtnSaveHooks});
            this.toolBar.Location = new System.Drawing.Point(0, 24);
            this.toolBar.Name = "toolBar";
            this.toolBar.Size = new System.Drawing.Size(1007, 25);
            this.toolBar.TabIndex = 4;
            this.toolBar.Text = "toolBar";
            // 
            // tsbtnAddHook
            // 
            this.tsbtnAddHook.Image = global::CSharpConsole.Properties.Resources._112_Plus_Green_16x16_72;
            this.tsbtnAddHook.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbtnAddHook.Name = "tsbtnAddHook";
            this.tsbtnAddHook.Size = new System.Drawing.Size(81, 22);
            this.tsbtnAddHook.Text = "Add Hook";
            this.tsbtnAddHook.Click += new System.EventHandler(this.AddHookHandler);
            // 
            // tsbtnRemoveHook
            // 
            this.tsbtnRemoveHook.Image = global::CSharpConsole.Properties.Resources._112_Minus_Orange_16x16_72;
            this.tsbtnRemoveHook.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbtnRemoveHook.Name = "tsbtnRemoveHook";
            this.tsbtnRemoveHook.Size = new System.Drawing.Size(102, 22);
            this.tsbtnRemoveHook.Text = "Remove Hook";
            this.tsbtnRemoveHook.Click += new System.EventHandler(this.RemoveHookHandler);
            // 
            // tsbtnRemoveAllHooks
            // 
            this.tsbtnRemoveAllHooks.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbtnRemoveAllHooks.Name = "tsbtnRemoveAllHooks";
            this.tsbtnRemoveAllHooks.Size = new System.Drawing.Size(108, 22);
            this.tsbtnRemoveAllHooks.Text = "Remove All Hooks";
            this.tsbtnRemoveAllHooks.Click += new System.EventHandler(this.RemoveAllHooksHandler);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // tsbtnClearOutput
            // 
            this.tsbtnClearOutput.Image = global::CSharpConsole.Properties.Resources.Delete;
            this.tsbtnClearOutput.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbtnClearOutput.Name = "tsbtnClearOutput";
            this.tsbtnClearOutput.Size = new System.Drawing.Size(95, 22);
            this.tsbtnClearOutput.Text = "Clear Output";
            this.tsbtnClearOutput.Click += new System.EventHandler(this.ClearOutputHandler);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // tsbtnImportHooks
            // 
            this.tsbtnImportHooks.Image = global::CSharpConsole.Properties.Resources.Import;
            this.tsbtnImportHooks.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbtnImportHooks.Name = "tsbtnImportHooks";
            this.tsbtnImportHooks.Size = new System.Drawing.Size(90, 22);
            this.tsbtnImportHooks.Text = "Load Hooks";
            this.tsbtnImportHooks.Click += new System.EventHandler(this.TsbtnImportHooksClick);
            // 
            // tsbtnSaveHooks
            // 
            this.tsbtnSaveHooks.Image = global::CSharpConsole.Properties.Resources.Export;
            this.tsbtnSaveHooks.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbtnSaveHooks.Name = "tsbtnSaveHooks";
            this.tsbtnSaveHooks.Size = new System.Drawing.Size(88, 22);
            this.tsbtnSaveHooks.Text = "Save Hooks";
            this.tsbtnSaveHooks.Click += new System.EventHandler(this.ToolStripButton1Click);
            // 
            // saveFileDialog
            // 
            this.saveFileDialog.Filter = "Hooking Setting Files (*.xml)|*.xml";
            // 
            // openHookingSettingsDialog
            // 
            this.openHookingSettingsDialog.Filter = "Hooking Setting Files (*.xml)|*.xml";
            // 
            // cmToHookOnStart
            // 
            this.cmToHookOnStart.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.miAddProcess});
            this.cmToHookOnStart.Name = "contextMenuStrip1";
            this.cmToHookOnStart.Size = new System.Drawing.Size(140, 26);
            // 
            // miAddProcess
            // 
            this.miAddProcess.Name = "miAddProcess";
            this.miAddProcess.Size = new System.Drawing.Size(139, 22);
            this.miAddProcess.Text = "Add Process";
            this.miAddProcess.Click += new System.EventHandler(this.MiAddProcessClick);
            // 
            // hookLoaderWorker
            // 
            this.hookLoaderWorker.DoWork += new System.ComponentModel.DoWorkEventHandler(this.LoadHooksInBackground);
            // 
            // cmFunctions
            // 
            this.cmFunctions.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.cmFunctionsAddHook,
            this.cmFunctionsRemoveHook});
            this.cmFunctions.Name = "cmFunctions";
            this.cmFunctions.Size = new System.Drawing.Size(150, 48);
            // 
            // cmFunctionsAddHook
            // 
            this.cmFunctionsAddHook.Image = global::CSharpConsole.Properties.Resources._112_Plus_Green_16x16_72;
            this.cmFunctionsAddHook.Name = "cmFunctionsAddHook";
            this.cmFunctionsAddHook.Size = new System.Drawing.Size(149, 22);
            this.cmFunctionsAddHook.Text = "Add Hook";
            this.cmFunctionsAddHook.Click += new System.EventHandler(this.AddHookHandler);
            // 
            // cmFunctionsRemoveHook
            // 
            this.cmFunctionsRemoveHook.Image = global::CSharpConsole.Properties.Resources._112_Minus_Orange_16x16_72;
            this.cmFunctionsRemoveHook.Name = "cmFunctionsRemoveHook";
            this.cmFunctionsRemoveHook.Size = new System.Drawing.Size(149, 22);
            this.cmFunctionsRemoveHook.Text = "Remove Hook";
            this.cmFunctionsRemoveHook.Click += new System.EventHandler(this.ToolStripMenuItem2Click);
            // 
            // addProcessToBeHookedDialog
            // 
            this.addProcessToBeHookedDialog.Filter = "Executable Files (*.exe)|*.exe";
            // 
            // DeviareConsoleForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1007, 579);
            this.Controls.Add(this.splitContainerMain);
            this.Controls.Add(this.toolBar);
            this.Controls.Add(this.menuMain);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuMain;
            this.Name = "DeviareConsoleForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "FormMain";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.Load += new System.EventHandler(this.FormLoadHandler);
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.FormClosedHandler);
            this.splitContainerMain.Panel1.ResumeLayout(false);
            this.splitContainerMain.Panel2.ResumeLayout(false);
            this.splitContainerMain.ResumeLayout(false);
            this.splitContainerTop.Panel1.ResumeLayout(false);
            this.splitContainerTop.Panel2.ResumeLayout(false);
            this.splitContainerTop.ResumeLayout(false);
            this.splitContainerRight.Panel1.ResumeLayout(false);
            this.splitContainerRight.Panel2.ResumeLayout(false);
            this.splitContainerRight.ResumeLayout(false);
            this.splitContainerBottom.Panel1.ResumeLayout(false);
            this.splitContainerBottom.Panel2.ResumeLayout(false);
            this.splitContainerBottom.ResumeLayout(false);
            this.cmHooks.ResumeLayout(false);
            this.cmProcessToBeHooked.ResumeLayout(false);
            this.menuMain.ResumeLayout(false);
            this.menuMain.PerformLayout();
            this.toolBar.ResumeLayout(false);
            this.toolBar.PerformLayout();
            this.cmToHookOnStart.ResumeLayout(false);
            this.cmFunctions.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainerMain;
        private System.Windows.Forms.SplitContainer splitContainerTop;
        private System.Windows.Forms.SplitContainer splitContainerRight;
        private FunctionsTreeView _functionsTreeView;
        private System.Windows.Forms.SplitContainer splitContainerBottom;
        private HooksTreeView _hooksTreeView;
        private TreeViewOutput treeViewOutput;
        private ProcessesTreeView _processesTreeView;
        private TreeColumn treeModuleName;
        private TreeColumn treeModuleAddress;
        private ModulesTreeView _modulesTreeView;
        private Aga.Controls.Tree.NodeControls.NodeTextBox nodeModuleName;
        private Aga.Controls.Tree.NodeControls.NodeTextBox nodeModuleAddress;
        private TreeColumn treeFunctionName;
        private TreeColumn treeFunctionAddress;
        private Aga.Controls.Tree.NodeControls.NodeTextBox nodeFunctionName;
        private Aga.Controls.Tree.NodeControls.NodeTextBox nodeFunctionAddress;
        private System.Windows.Forms.MenuStrip menuMain;
        private System.Windows.Forms.ToolStripMenuItem hookToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addHook;
        private TreeColumn treeHook;
        private Aga.Controls.Tree.NodeControls.NodeTextBox nodeHook;
        private System.Windows.Forms.ToolStripMenuItem mnuRemoveHook;
        private System.Windows.Forms.ToolStrip toolBar;
        private System.Windows.Forms.ToolStripButton tsbtnAddHook;
        private System.Windows.Forms.ToolStripButton tsbtnRemoveHook;
        private System.Windows.Forms.ToolStripButton tsbtnClearOutput;
        private TreeColumn tcFunctionIcon;
        private Aga.Controls.Tree.NodeControls.NodeIcon ncFunctionIcon;
        
        private TreeColumn tcFunctionReturnType;
        private TreeColumn tcFunctionParameters;
        private Aga.Controls.Tree.NodeControls.NodeTextBox ncFunctionReturnType;
        private Aga.Controls.Tree.NodeControls.NodeTextBox ncFunctionParameters;
        private System.Windows.Forms.ToolStripMenuItem settingsToolStripMenuItem;
        private TreeColumn tcOutputFunctionName;
        private TreeColumn tcOutputParameters;
        private Aga.Controls.Tree.NodeControls.NodeTextBox ncOutputFunctionName;
        private Aga.Controls.Tree.NodeControls.NodeTextBox ncOutputParameters;
        private TreeColumn tcOutputModuleName;
        private Aga.Controls.Tree.NodeControls.NodeTextBox ncOutputModuleName;
        private TreeColumn tcOutputTid;
        private Aga.Controls.Tree.NodeControls.NodeTextBox ncOutputTid;
        private TreeColumn tcOutputOrder;
        private Aga.Controls.Tree.NodeControls.NodeTextBox ncOutputOrder;
        private System.Windows.Forms.ContextMenuStrip cmProcessToBeHooked;
        private System.Windows.Forms.ToolStripMenuItem miStartProcess;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ContextMenuStrip cmHooks;
        private System.Windows.Forms.ToolStripMenuItem cmiRemoveHook;
        private System.Windows.Forms.ToolStripButton tsbtnRemoveAllHooks;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton tsbtnSaveHooks;
        private System.Windows.Forms.SaveFileDialog saveFileDialog;
        private System.Windows.Forms.ToolStripButton tsbtnImportHooks;
        private System.Windows.Forms.OpenFileDialog openHookingSettingsDialog;
        private System.Windows.Forms.ContextMenuStrip cmToHookOnStart;
        private System.Windows.Forms.ToolStripMenuItem miAddProcess;
        private System.ComponentModel.BackgroundWorker hookLoaderWorker;
        private TreeColumn tcOutputFunctionResult;
        private Aga.Controls.Tree.NodeControls.NodeTextBox ncOutputFunctionResult;
        private System.Windows.Forms.ContextMenuStrip cmFunctions;
        private System.Windows.Forms.ToolStripMenuItem cmFunctionsAddHook;
        private System.Windows.Forms.ToolStripMenuItem cmFunctionsRemoveHook;
        private System.Windows.Forms.OpenFileDialog addProcessToBeHookedDialog;
        private TreeColumn treeModuleHooksCount;
        private Aga.Controls.Tree.NodeControls.NodeIntegerTextBox nodeModuleHooksCount;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem selectAllToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem refreshToolStripMenuItem;
    }
}

