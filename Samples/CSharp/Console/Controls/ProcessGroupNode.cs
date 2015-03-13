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

using System.Drawing;
using System.Linq;
using CSharpConsole.Model;
using CSharpConsole.Model.Nodes;
using CSharpConsole.Model.Process;
using CSharpConsole.Model.Visitors;

namespace CSharpConsole.Controls
{
    public class ProcessGroupNode : GroupNode<ProcessNode>
    {
        #region Properties

        public override string DeviareNodeName
        {
            get { return "Group: " + MainNode.DeviareNodeName; }
        }

        public string Pid
        {
            get { return IsGrouppingNodes ? string.Empty : MainNode.Pid; }
        }

        public Image Icon
        {
            get { return MainNode.Icon; }
        }

        #endregion

        #region Instance Creation

        public ProcessGroupNode(ProcessNode mainNode)
            : base(mainNode)
        {
            if (mainNode.Process.Name.Contains("CSharpConsole"))
            {
                ForeColor = SystemColors.GrayText;
            }
        }

        #endregion

        public override void Accept(IDeviareNodeVisitor aVisitor)
        {
            aVisitor.VisitProcessGroupNode(this);
        }

        public bool Groups(IProcess aProcess)
        {
            return GrouppedNodes.Any(node => node.Represents(aProcess));
        }

        public void UnGroup(IProcess aProcess)
        {
            ProcessNode aNode = GrouppedNodes.FirstOrDefault(node => node.Represents(aProcess));
            UnGroup(aNode);
        }
    }
}