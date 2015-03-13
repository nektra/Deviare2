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
using System.Linq;
using Aga.Controls.Tree;
using CSharpConsole.Model;
using CSharpConsole.Model.Nodes;
using CSharpConsole.Model.Process;
using CSharpConsole.Model.Visitors;
using CSharpConsole.Extensions;

namespace CSharpConsole.Controls
{
    public class ProcessCategoryNode : DeviareNode
    {
        #region Fields

        private readonly List<ProcessGroupNode> _groups = new List<ProcessGroupNode>();

        #endregion

        #region Properties

        public string CategoryTitle { get; set;}

        #endregion

        #region Instance Creation

        public ProcessCategoryNode(string categoryTitle)
        {
            CategoryTitle = categoryTitle;
            Bold = true;
        }
        #endregion

        private ProcessGroupNode GroupFor(ProcessNode aNode)
        {
            return _groups.FirstOrDefault(g => g.Groups(aNode)) ?? InitializeGroupFor(aNode);
        }

        private ProcessGroupNode InitializeGroupFor(ProcessNode aNode)
        {
            var group = new ProcessGroupNode(aNode);

            _groups.Add(group);
            AddNodeInLexigographycalOrder(group);

            return group;
        }

        private void AddNodeInLexigographycalOrder(ProcessGroupNode aNodeGroup)
        {
            var index = FindIndexOfNodeThatShouldPrecede(aNodeGroup);
            Nodes.Insert(index + 1, aNodeGroup);
        }

        private int FindIndexOfNodeThatShouldPrecede(ProcessGroupNode aNodeGroup)
        {
            Func<Node, bool> textPrecedesNodeText = node => aNodeGroup.Text.CompareTo(node.Text) > 0;

            if (!Nodes.Any(textPrecedesNodeText)) return -1;
            return Nodes.Last(textPrecedesNodeText).Index;
        }

        #region Implementation of DeviareNode

        public override string DeviareNodeName
        {
            get { return "Category:" + CategoryTitle; }
        }

        public override void Accept(IDeviareNodeVisitor aVisitor)
        {
            
        }

        #endregion

        public void Add(IEnumerable<IProcess> processes)
        {
            processes.ForEach(p => Add(p));
        }

        public ProcessGroupNode Add(IProcess aProcess)
        {
            var aNode = ProcessNode.From(aProcess);
            var group = GroupFor(aNode);
            group.Group(aNode);

            return group;
        }

        public void RemoveChildMatching(IProcess aProcess)
        {
            _groups
                .Where(g => g.Groups(aProcess))
                .ToList()
                .ForEach(g => Remove(aProcess, g));
        }

        private void Remove(IProcess aProcess, ProcessGroupNode aGroupNode)
        {
            aGroupNode.UnGroup(aProcess);
            if (aGroupNode.IsDeleted) _groups.Remove(aGroupNode);
        }
    }
}