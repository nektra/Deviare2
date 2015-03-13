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

using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.Visitors;
using Nektra.Deviare2;

namespace CSharpConsole.Model.Nodes
{
    public class HookNode : DeviareNode
    {
        private readonly string _functionName;

        public HookNode(Hook aHook, Function aFunction)
        {
            _functionName = aFunction.Name;
            Text = _functionName;
            Hook = aHook;
        }

        #region Equality Members

        public bool Equals(HookNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return Equals(other.Hook, Hook);
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != typeof (HookNode)) return false;
            return Equals((HookNode) obj);
        }

        public override int GetHashCode()
        {
            return (Hook != null ? Hook.GetHashCode() : 0);
        }

        #endregion


        public Hook Hook { get; private set; }

        public override string DeviareNodeName
        {
            get { throw new System.NotImplementedException(); }
        }

        public override void Accept(IDeviareNodeVisitor aVisitor)
        {
            aVisitor.VisitHookNode(this);
        }

        public void UpdateState(eNktHookState aState)
        {
            Text = _functionName + " (" + aState + ")";
        }

        #region Overrides of Node

        public override string ToString()
        {
            return string.Format("({1}) -> {0}", Text, Parent.Text);
        }

        #endregion
    }
}