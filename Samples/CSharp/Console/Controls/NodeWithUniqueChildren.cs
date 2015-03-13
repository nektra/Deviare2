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
using System.Linq;
using Aga.Controls.Tree;
using CSharpConsole.Model;
using CSharpConsole.Model.Nodes;
using CSharpConsole.Model.Visitors;

namespace CSharpConsole.Controls
{
    public class NodeWithUniqueChildren : DeviareNode
    {
        public NodeWithUniqueChildren(string categoryTitle)
        {
            CategoryTitle = categoryTitle;
            Bold = true;
        }

        public string CategoryTitle { get; set; }

        public bool IsEmpty
        {
            get { return Nodes.Count == 0; }
        }

        #region Overrides of DeviareNode

        public override string DeviareNodeName
        {
            get { throw new NotImplementedException(); }
        }

        public override void Accept(IDeviareNodeVisitor aVisitor)
        {
        }

        #endregion

        public void AddChild(Node aNode)
        {
            if (ExistsNodeMatching(node => node.Text == aNode.Text)) return;
            Nodes.Add(aNode);
        }

        public void RemoveChildMatching(Func<Node, bool> aCriteria)
        {
            ProcessNodeMatching(aCriteria, node => Nodes.Remove(node));
        }

        private bool ExistsNodeMatching(Func<Node, bool> aCriteria)
        {
            return Nodes.Any(aCriteria);
        }

        private void ProcessNodeMatching(Func<Node, bool> aCriteria, Action<Node> ifFound)
        {
            var aNode = Nodes.FirstOrDefault(aCriteria);

            if (aNode != null)
                ifFound(aNode);
        }
    }
}