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

using System.Linq;
using CSharpConsole.Model;
using CSharpConsole.Model.Nodes;
using CSharpConsole.Model.Visitors;

namespace CSharpConsole.Controls
{
    public class GroupNode<T> : DeviareNode where T : DeviareNode
    {
        public override string DeviareNodeName
        {
            get { throw new System.NotImplementedException(); }
        }

        public override System.Collections.Generic.IEnumerable<DeviareNode> ChildNodes
        {
            get { return GrouppedNodes; }
        }

        public GroupNode(T mainNode)
        {
            MainNode = mainNode;
        }

        public T[] GrouppedNodes
        {
            get { return IsGrouppingNodes ? Nodes.Cast<T>().ToArray() : new[] {MainNode}; }
        }

        public override string Text
        {
            get { return MainNode.Text; }
            set { }
        }

        public override void Accept(IDeviareNodeVisitor aVisitor)
        {
            if (IsGrouppingNodes) return;
            MainNode.Accept(aVisitor);
        }

        public T MainNode { get; private set; }

        protected bool IsGrouppingNodes
        {
            get { return Nodes.Count > 0; }
        }

        public bool IsDeleted
        {
            get { return Parent == null; }
        }

        public bool Groups(T aNode)
        {
            return MainNode.Text == aNode.Text;
        }

        public void Group(T aNode)
        {
            if (aNode == MainNode) return;
            if (GrouppedNodes.Contains(aNode)) return;

            Nodes.Add(MainNode);
            Nodes.Add(aNode);
        }

        public void UnGroup(T aNode)
        {
            if (!IsGrouppingNodes)
            {
                DeleteWholeNodeGroup();
                return;
            }

            Nodes.Remove(aNode);

            if (aNode == MainNode)
            {
                MainNode = (T) Nodes.First();
            }

            if (Nodes.Count != 1) return;
            Nodes.Clear();
        }

        private void DeleteWholeNodeGroup()
        {
            Parent = null;
        }
    }
}