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

namespace CSharpConsole.Controls
{
    public class TreeViewOutput : TreeViewAdv
    {
        #region Fields

        private readonly object _dataLock = new object();
        private TreeModel _model;

        #endregion

        #region Properties

        protected IEnumerable<OutputNode> OutputNodes
        {
            get { return AllNodes.Select(adv => adv.Tag).Cast<OutputNode>(); }
        }

        #endregion
        
        #region Instance Creation & Initialization

        public void InitializeComponent()
        {
            _model = new TreeModel();
            Model = _model;
        }

        #endregion

        #region FunctionCall Displaying

        public void DisplayRootFunctionCall(FunctionCall aFunctionCall)
        {
            DisplayFunctionCall(functionCall => _model.Root, aFunctionCall);
        }

        public void DisplayNestedFunctionCall(FunctionCall aFunctionCall)
        {
            DisplayFunctionCall(FindParentFunctionCallFor, aFunctionCall);
        }

        private void DisplayFunctionCall(Func<FunctionCall, Node> parentNodeFor, FunctionCall aFunctionCall)
        {
            lock (_dataLock)
            {
                AddNodeFor(aFunctionCall, parentNodeFor(aFunctionCall));
            }
        }

        public void UpdateRootFunctionCall(FunctionCall aFunctionCall)
        {
            OutputNodes.First(node => node.FunctionMatches(aFunctionCall)).UpdateFrom(aFunctionCall);
        }

        #endregion

        #region helper Functions

        private Node FindParentFunctionCallFor(FunctionCall aFunctionCall)
        {
            var parentNode = _model.Nodes.Cast<OutputNode>().OrderByDescending(node => node.Order).FirstOrDefault(node => node.Tid == aFunctionCall.ThreadId);
            return parentNode ?? _model.Root;
        }

        #endregion

        private void AddNodeFor(FunctionCall aFunctionCall, Node parentNode)
        {
            var functionCallNode = new OutputNode(aFunctionCall);

            parentNode.Nodes.Add(functionCallNode);
        }

        public void ClearData()
        {
            lock (_dataLock)
            {
                _model.Nodes.Clear();
            }
        }
    }
}