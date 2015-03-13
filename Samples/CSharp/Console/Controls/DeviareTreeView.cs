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
using System.ComponentModel;
using System.Windows.Forms;
using Aga.Controls.Tree;
using CSharpConsole.Model.Nodes;
using System.Linq;

namespace CSharpConsole.Controls
{
    public class DeviareTreeView : TreeViewAdv
    {
        #region Fields

        private TreeModel _model;
        private SortedTreeModel _sortedTreeModel;

        protected readonly Dictionary<BackgroundWorker, bool> CancelUpdateByWorker = new Dictionary<BackgroundWorker, bool>();

        #endregion

        #region Instance Creation

        public void InitializeComponent()
        {
            _model = new TreeModel();
            _sortedTreeModel = new SortedTreeModel(_model);
            Model = _sortedTreeModel;

            ColumnClicked += ColumnClickedHandler;

            FullRowSelect = true;
        }

        private void ColumnClickedHandler(object sender, TreeColumnEventArgs e)
        {
            var aColumn = e.Column;
            InvertSortOrderOf(aColumn);
            Sort(aColumn);
        }

        #endregion

        #region Nodes Sorting

        private void InvertSortOrderOf(TreeColumn aColun)
        {
            aColun.SortOrder = aColun.SortOrder == SortOrder.Ascending ? SortOrder.Descending : SortOrder.Ascending;
        }

        protected void Sort(TreeColumn aColumn)
        {
            _sortedTreeModel.Comparer = new NodeSorter(aColumn, this);
        }

        public void SortByColumn(int columnIndex)
        {
            _sortedTreeModel.Comparer = new NodeSorter(Columns.ElementAt(columnIndex), this);
        }

        #endregion

        protected virtual void RemoveAllNodes()
        {
            _model.Nodes.Clear();
        }

        protected DeviareNode SelectedDeviareNode
        {
            get

            {
                if (SelectedNode == null) return new NullDeviareNode();
                return (DeviareNode) SelectedNode.Tag;
            }
        }

        protected void AddDeviareNode(DeviareNode aNode)
        {
            _model.Nodes.Add(aNode);
        }
    }
}