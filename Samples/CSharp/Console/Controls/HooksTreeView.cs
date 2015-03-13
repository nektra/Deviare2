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
using Aga.Controls.Tree;
using CSharpConsole.Model;
using CSharpConsole.Model.Collections;
using CSharpConsole.Model.HookStateHandlers;
using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.Modules;
using CSharpConsole.Model.Nodes;
using CSharpConsole.Model.Process;
using CSharpConsole.Model.Visitors;

namespace CSharpConsole.Controls
{
    public class HooksTreeView : TreeViewAdv
    {
        #region Fields

        private readonly Dictionary<Hook, HookNode> _hookNodes = new Dictionary<Hook, HookNode>();
        private readonly EntityDictionary<IRunningProcess, int, ProcessNode> _nodes;
        private readonly object _dataLock = new Object();
        private TreeModel _model;

        public IEnumerable<Hook> SelectedHooks
        {
            get
            {
                var aVisitor = new Collector();
                SelectedDeviareNode().Accept(aVisitor);
                return aVisitor.CollectedHooks;
            }
        }

        public HooksTreeView()
        {
            _nodes = new EntityDictionary<IRunningProcess, int, ProcessNode>(process => process.Id, ProcessNode.From);
            FullRowSelect = true;
        }

        public void InitializeComponent()
        {
            _model = new TreeModel();
            Model = _model;
        }

        #endregion

        #region Operations on Nodes

        private void Add(IRunningProcess aProcess, Module aModule, Function aFunction, Hook aHook)
        {
            lock (_dataLock)
            {
                if (_hookNodes.ContainsKey(aHook)) return;

                var aProcessNode = _nodes.ValueOrInitialize(aProcess);
                var aModuleNode = aProcessNode.ModuleNodeFor(aModule);
                var aHookNode = aModuleNode.AddHook(aHook, aFunction);

                _hookNodes[aHook] = aHookNode;

                _model.Nodes.Add(aProcessNode);
            }
        }

        public void Remove(Hook aHook)
        {
            lock (_dataLock)
            {
                if (!_hookNodes.ContainsKey(aHook)) return;
                var aNode = _hookNodes[aHook];
                _hookNodes.Remove(aHook);
                
                aNode.Accept(new DeepDeleteVisitor());
            }
        }

        #endregion

        private DeviareNode SelectedDeviareNode()
        {
            if (SelectedNode == null) return new NullDeviareNode();
            return (DeviareNode)SelectedNode.Tag;
        }

        public void DisplayHook(Hook hook, IRunningProcess process, HookStateChangeInfo pepe)
        {
            if (!process.IsAlive()) return;

            lock (_dataLock)
            {
                process.Do(hook, (p, m, f) =>
                                     {
                                         Add(process, m, f, hook);
                                         _hookNodes[hook].UpdateState(pepe.CurrentState);
                                     });
                
            }
        }
    }
}