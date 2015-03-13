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
using CSharpConsole.Controls;
using CSharpConsole.Model.Nodes;

namespace CSharpConsole.Model.Visitors
{
    public class DeepDeleteVisitor : IDeviareNodeVisitor
    {
        #region Fields

        private readonly Queue<DeviareNode> _nodesInBranch = new Queue<DeviareNode>();

        #endregion
        
        #region IDeviareNodeVisitor Implementation

        public void VisitProcessNode(ProcessNode processNode)
        {
            EnqueueNode(processNode);
        }

        public void VisitProcessGroupNode(ProcessGroupNode aProcessGroupNode)
        {
            throw new System.NotImplementedException();
        }

        public void VisitModuleNode(ModuleNode aModuleNode)
        {
            EnqueueNode(aModuleNode);
        }

        public void VisitHookNode(HookNode hookNode)
        {
            EnqueueNode(hookNode);
        }

        public void VisitFunctionNode(FunctionNode functionNode)
        {
            EnqueueNode(functionNode);
        }

        public void VisitFunctionGroupNode(FunctionNodeGroup aFunctionNodeGroup)
        {
            EnqueueNode(aFunctionNodeGroup);
        }

        #endregion

        private void EnqueueNode(DeviareNode aNode)
        {
            _nodesInBranch.Enqueue(aNode);

            if (aNode.Parent is DeviareNode)
            {
                var parentNode = (DeviareNode) aNode.Parent;
                parentNode.Accept(this);
                return;
            }

            DeepDeleteNodes();
        }

        private void DeepDeleteNodes()
        {
            var nodeToDelete = _nodesInBranch.Dequeue();
            nodeToDelete.Parent = null;

            while (_nodesInBranch.Count > 0)
            {
                var node = _nodesInBranch.Dequeue();
                var afterDeleteNodeHasNoChildren = node.Nodes.Count == 0;

                if (afterDeleteNodeHasNoChildren)
                    node.Parent = null;
                else
                    break;
            }
        }
    }
}