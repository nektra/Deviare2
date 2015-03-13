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

using System.Collections;
using System.Linq;
using System.Reflection;
using System.Windows.Forms;
using Aga.Controls.Tree;
using Aga.Controls.Tree.NodeControls;

namespace CSharpConsole.Controls
{
    internal class NodeSorter : IComparer
    {
        #region Fields

        private readonly TreeColumn _aColumn;
        private readonly TreeViewAdv _aTreeView;

        #endregion

        #region Instance Creation

        public NodeSorter(TreeColumn aColumn, TreeViewAdv aTreeView)
        {
            _aColumn = aColumn;
            _aTreeView = aTreeView;
        }

        #endregion

        #region Implementation of IComparer

        public int Compare(object x, object y)
        {
            var xValue = ValueFor(x);
            var yValue = ValueFor(y);

            return _aColumn.SortOrder == SortOrder.Ascending ? xValue.CompareTo(yValue) : yValue.CompareTo(xValue);
        }

        #region Helper Methods

        private string ValueFor(object anObject)
        {
            var node = (Node) anObject;

            var aPropertyInfo = PropertyInfoForSelectedColumnFrom(node);
            var columnValue = aPropertyInfo.GetValue(node, null).ToString();

            return columnValue;
        }

        private PropertyInfo PropertyInfoForSelectedColumnFrom(Node aNode)
        {
            var nodeControl = NodeControlForColumn();

            var prop = aNode.GetType().GetProperty(nodeControl.DataPropertyName);
            return prop;
        }

        private BindableControl NodeControlForColumn()
        {
            return (BindableControl) _aTreeView.NodeControls.First(control => control.ParentColumn == _aColumn);
        }

        #endregion


        #endregion
    }
}