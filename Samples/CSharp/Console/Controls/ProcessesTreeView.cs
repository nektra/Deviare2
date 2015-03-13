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
using System.Windows.Forms;
using Aga.Controls.Tree;
using Aga.Controls.Tree.NodeControls;
using CSharpConsole.Model.Nodes;
using CSharpConsole.Model.Process;
using CSharpConsole.Model.Visitors;
using CSharpConsole.Presenters;
using CSharpConsole.Extensions;

namespace CSharpConsole.Controls
{
    public class ProcessesTreeView : TreeViewAdv
    {
        #region Fields

        private readonly Object _lock = new Object();
        private ProcessCategoryNode _hookedActive;
        private ProcessCategoryNode _hookedInactive;
        private TreeModel _model;
        private NodeCategoryTitle _ncProcessCategory;
        private NodeSizeControl _ncProcessCategoryTitleSize;
        private NodeIcon _ncProcessIcon;
        private NodeTextBox _ncProcessName;
        private NodeTextBox _ncProcessPid;

        private bool _skipSelectionChangedEvent;
        private TreeColumn _tcProcessIcon;
        private TreeColumn _tcProcessPid;
        private NodeWithUniqueChildren _toHookOnStartGroup;
        private ProcessCategoryNode _unHooked;

        #endregion

        #region Instance Creation

        public ProcessesTreeView()
        {
            _skipSelectionChangedEvent = false;
            InitializeColumnHeaders();
            InitializeColumnControls();

            Columns.Add(_tcProcessIcon);
            Columns.Add(_tcProcessPid);

            NodeControls.Add(_ncProcessIcon);
            NodeControls.Add(_ncProcessName);
            NodeControls.Add(_ncProcessCategory);
            NodeControls.Add(_ncProcessCategoryTitleSize);
            NodeControls.Add(_ncProcessPid);

            AutoRowHeight = true;
            FullRowSelect = true;
        }

        private void InitializeColumnControls()
        {
            // 
            // ncProcessIcon
            // 
            _ncProcessIcon = new NodeIcon
                                 {
                                     DataPropertyName = "Icon",
                                     LeftMargin = 1,
                                     ParentColumn = _tcProcessIcon,
                                     ScaleMode = ImageScaleMode.Clip
                                 };
            // 
            // nodeProcessName
            // 
            _ncProcessName = new NodeTextBox
                                 {
                                     DataPropertyName = "Text",
                                     IncrementalSearchEnabled = true,
                                     LeftMargin = 3,
                                     ParentColumn = _tcProcessIcon
                                 };
            //_ncProcessName.ParentColumn = _tcProcessName;

            //
            // ncProcessGroup
            //
            _ncProcessCategory = new NodeCategoryTitle
                                     {
                                         DataPropertyName = "CategoryTitle",
                                         IncrementalSearchEnabled = true,
                                         LeftMargin = 1,
                                         ParentColumn = _tcProcessIcon,
                                         FirstRowVerticalAlign = VerticalAlignment.Top
                                     };
            //
            // ncCategoryTitleSize
            //
            _ncProcessCategoryTitleSize = new NodeSizeControl(new Size(0, 45), new Size(0, 30),
                                                              new List<Type>
                                                                  {
                                                                      typeof (ProcessCategoryNode),
                                                                      typeof (NodeWithUniqueChildren)
                                                                  });
            // 
            // nodeProcessPid
            // 
            _ncProcessPid = new NodeTextBox
                                {
                                    DataPropertyName = "Pid",
                                    IncrementalSearchEnabled = true,
                                    LeftMargin = 3,
                                    ParentColumn = _tcProcessPid
                                };
        }

        private void InitializeColumnHeaders()
        {
            // 
            // tcProcessIcon
            // 
            _tcProcessIcon = new TreeColumn
                                 {
                                     Header = "Process",
                                     SortOrder = SortOrder.None,
                                     TooltipText = null,
                                     Width = 250
                                 };

            // 
            // tcProcessPid
            // 
            _tcProcessPid = new TreeColumn
                                {
                                    Header = "Pid",
                                    SortOrder = SortOrder.None,
                                    TooltipText = null
                                };
        }

        public void InitializeComponent()
        {
            _model = new TreeModel();
            Model = _model;

            _toHookOnStartGroup = new NodeWithUniqueChildren("To Hook On Start");
            _unHooked = new ProcessCategoryNode("UnHooked");
            _hookedInactive = new ProcessCategoryNode("Hooked (inactive)");
            _hookedActive = new ProcessCategoryNode("Hooked (active)");

            _model.Nodes.Add(_toHookOnStartGroup);
            _model.Nodes.Add(_unHooked);
            _model.Nodes.Add(_hookedInactive);
            _model.Nodes.Add(_hookedActive);
        }

        #endregion

        #region Properties

        private DeviareNode SelectedDeviareNode()
        {
            if (SelectedNode == null) return new NullDeviareNode();
            return (DeviareNode) SelectedNode.Tag;
        }

        #endregion

        #region Removing Processes

        public void RemoveProcessToBeHookedOnStart(IProcess aProcess)
        {
            lock (_lock)
            {
                _toHookOnStartGroup.RemoveChildMatching(node => node.Text == aProcess.Name);
            }
        }

        public void RemoveRunningProcess(IProcess aProcess)
        {
            lock (_lock)
            {
                _unHooked.RemoveChildMatching(aProcess);
                _hookedInactive.RemoveChildMatching(aProcess);
                _hookedActive.RemoveChildMatching(aProcess);
            }
        }

        private void Remove(IProcess aProcess)
        {
            RemoveProcessToBeHookedOnStart(aProcess);
            RemoveRunningProcess(aProcess);
        }

        #endregion

        #region Selection

        public void HandleSelectedNode(ApplicationPresenter presenter)
        {
            var nodeHandler = DeviareNodeHandler.ForProcesses(
                processNode =>
                    {
                        var collector = new Collector();
                        processNode.Accept(collector);
                        presenter.SelectedProcessesChanged(
                            collector.CollectedProcesses);
                        return true;
                    },
                processGroupNode =>
                    {
                        var collector = new Collector();
                        processGroupNode.Accept(collector);
                        presenter.SelectedProcessesChanged(
                            collector.CollectedProcesses);
                        return true;
                    },
                processCategoryNode => false,
                presenter.ClearSelectedProcesses);
            
            SelectedDeviareNode().Accept(nodeHandler);
        }

        public IProcess[] SelectedProcesses()
        {
            var collector = new Collector();

            foreach (DeviareNode node in SelectedDeviareNodes())
            {
                node.Accept(collector);
            }

            return collector.CollectedProcesses.ToArray();
        }

        private IEnumerable<DeviareNode> SelectedDeviareNodes()
        {
            return SelectedNodes.Select(adv => adv.Tag).Cast<DeviareNode>();
        }

        private void MarkSelected(ProcessNode aNode)
        {
            var t = AllNodes.First(adv => aNode.Equals(adv.Tag));
            t.IsSelected = true;
        }

        #endregion

        #region Displaying Processes

        public void Display(IEnumerable<IProcess> toHookOnStart, IEnumerable<IRunningProcess> unhooked,
                            IEnumerable<IRunningProcess> hookedInactive, IEnumerable<IRunningProcess> hookedActive)
        {
            lock (_lock)
            {
                toHookOnStart.ForEach(p => _toHookOnStartGroup.AddChild(ProcessNode.From(p)));

                _unHooked.Add(unhooked.Cast<IProcess>());
                _hookedInactive.Add(hookedInactive.Cast<IProcess>());
                _hookedActive.Add(hookedActive.Cast<IProcess>());
            }

            ExpandCategoryNodes();
        }

        public void DisplayProcessToBeHookedOnStart(IProcess aProcess)
        {
            lock (_lock)
            {
                _toHookOnStartGroup.AddChild(ProcessNode.NotRunning(aProcess));
                ExpandCategoryNodes();
            }
        }

        public void DisplayUnHookedProcess(IProcess aProcess)
        {
            lock (_lock)
            {
                DisplayProcess(aProcess, _unHooked);
            }
        }

        public void DisplayHookedInactiveProcess(IProcess aProcess)
        {
            lock (_lock)
            {
                DisplayProcess(aProcess, _hookedInactive);
            }
        }

        public void DisplayHookedActiveProcess(IProcess aProcess)
        {
            lock (_lock)
            {
                DisplayProcess(aProcess, _hookedActive);
            }
        }

        private ProcessGroupNode DisplayProcess(IProcess aProcess, ProcessCategoryNode aCategoryNode)
        {
            _skipSelectionChangedEvent = true;

            Remove(aProcess);

            var groupNode = aCategoryNode.Add(aProcess);
            ExpandCategoryNodes();

            _skipSelectionChangedEvent = false;

            return groupNode;
        }

        public void DisplayRecentlyHookedProcessAsInactive(IProcess aProcess)
        {
            DisplayRecentlyHookedOrUnhookedProcess(aProcess, _hookedInactive);
        }

        public void DisplayRecentlyHookedProcessAsActive(IProcess aProcess)
        {
            DisplayRecentlyHookedOrUnhookedProcess(aProcess, _hookedActive);
        }

        public void DisplayRecentlyUnhookedProcess(IProcess aProcess)
        {
            DisplayRecentlyHookedOrUnhookedProcess(aProcess, _unHooked);
        }

        private void DisplayRecentlyHookedOrUnhookedProcess(IProcess aProcess, ProcessCategoryNode aCategoryNode)
        {
            lock (_lock)
            {
                var processGroupNode = DisplayProcess(aProcess, aCategoryNode);

                var processNode = processGroupNode.GrouppedNodes.First(n => n.Process.Equals(aProcess));

                MarkSelected(processNode);

                EnsureVisible(SelectedNode);
            }
        }

        #endregion

        #region Testing

        public bool ToHookOnStartGroupIsSelected()
        {
            return SelectedNode == FindNodeByTag(_toHookOnStartGroup);
        }

        #endregion

        private void ExpandCategoryNodes()
        {
            foreach (TreeNodeAdv children in Root.Children)
            {
                children.Expand();
            }
        }
    }
}