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
using CSharpConsole.Model.Visitors;

namespace CSharpConsole.Controls
{
    public class FunctionNodeGroup : GroupNode<FunctionNode>
    {
        #region Properties

        public string ReturnType
        {
            get { return IsGrouppingNodes ? string.Empty : MainNode.ReturnType; }
        }

        public string FunctionName
        {
            get { return MainNode.Text; }
        }

        public string Parameters
        {
            get { return IsGrouppingNodes ? string.Empty : MainNode.Parameters; }
        }

        public string Address
        {
            get { return IsGrouppingNodes ? string.Empty : MainNode.Address; }
        }

        public Image Icon
        {
            get
            {
                var firstOrDefault = GrouppedNodes.FirstOrDefault(node => node.FunctionIsDeclared) ?? MainNode;
                return firstOrDefault.Icon;
            }
        }

        #endregion

        public FunctionNodeGroup(FunctionNode mainNode)
            : base(mainNode)
        {
        }

        public override void Accept(IDeviareNodeVisitor aVisitor)
        {
            aVisitor.VisitFunctionGroupNode(this);
        }
    }
}