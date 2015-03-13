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
using System.Linq;
using Aga.Controls.Tree;
using CSharpConsole.Controls;
using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.Nodes;
using CSharpConsole.Model.Process;
using CSharpConsole.Extensions;

namespace CSharpConsole.Model.Visitors
{
    public class Collector : IDeviareNodeVisitor
    {
        public List<Hook> CollectedHooks { get; private set; }
        public List<IProcess> CollectedProcesses { get; private set; }
        public List<Function> CollectedFunctions { get; private set; }

        public Collector()
        {
            CollectedHooks = new List<Hook>();
            CollectedProcesses = new List<IProcess>();
            CollectedFunctions = new List<Function>();
        }

        #region Implementation of IDeviareNodeVisitor

        public void VisitProcessNode(ProcessNode processNode)
        {
            CollectedProcesses.Add(processNode.Process);
            VisitChildNodesOf(processNode);
        }

        public void VisitProcessGroupNode(ProcessGroupNode aProcessGroupNode)
        {
            VisitChildNodesOf(aProcessGroupNode);
        }

        private void VisitChildNodesOf(DeviareNode aNode)
        {
            var childNodes = aNode.ChildNodes;
            foreach (var node in childNodes)
            {
                node.Accept(this);
            }
        }

        public void VisitModuleNode(ModuleNode aModuleNode)
        {
            VisitChildNodesOf(aModuleNode);
        }

        public void VisitHookNode(HookNode hookNode)
        {
            CollectedHooks.Add(hookNode.Hook);
        }

        public void VisitFunctionNode(FunctionNode functionNode)
        {
            CollectedFunctions.Add(functionNode.Function);
        }

        private void VisitFunctionNodes(IEnumerable<FunctionNode> functionNodes)
        {
            functionNodes.ForEach(n => CollectedFunctions.Add(n.Function));
        }

        public void VisitFunctionGroupNode(FunctionNodeGroup aFunctionNodeGroup)
        {
            VisitFunctionNodes(aFunctionNodeGroup.GrouppedNodes);
        }

        #endregion
    }
}