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
using CSharpConsole.Model.Visitors;
using CSharpConsole.Views;
using CSharpConsole.Extensions;

namespace CSharpConsole.Controls
{
    public class FunctionsTreeView : TreeViewAdv
    {
        #region Fields

        private TreeModel _model;
        private readonly List<FunctionNodeGroup> _functionsByName = new List<FunctionNodeGroup>();

        #endregion

        #region Properties

        public IEnumerable<Function> SelectedFunctions()
        {
            if (SelectedNode == null)
                return Enumerable.Empty<Function>();
            var collector = new Collector();
            
            SelectedNodes.Select(n => n.Tag).Cast<DeviareNode>().ForEach(n => n.Accept(collector));

            return collector.CollectedFunctions;
        }

        #endregion

        #region Instance Creation & Initialization

        public void InitializeComponent()
        {
            _model = new TreeModel();
            Model = _model;
            FullRowSelect = true;
        }

        #endregion

        #region Operation on Nodes

        private void Add(Function aFunction)
        {
            var settings = new ApplicationSettings();
            var aNode = new FunctionNode(aFunction, settings.DisplayFunctionsSignatureAndReturnType);

            Func<FunctionNodeGroup, bool> nodeThatGroupsTheFunction = node => node.Groups(aNode);
            if (_functionsByName.Any(nodeThatGroupsTheFunction))
            {
                _functionsByName.First(nodeThatGroupsTheFunction).Group(aNode);
            }
            else
            {
                var aFunctionGroup = new FunctionNodeGroup(aNode);
                _functionsByName.Add(aFunctionGroup);

                _model.Nodes.Add(aFunctionGroup);
            }
        }

        #endregion

        private void RemoveAllNodes()
        {
            _functionsByName.Clear();
            _model.Nodes.Clear();
        }

        public void Display(IEnumerable<Function> functions)
        {
            RemoveAllNodes();

            foreach (var function in functions)
            {
                Add(function);
            }

            //AutoSizeColumn(tcFunctionReturnType);
            //AutoSizeColumn(treeFunctionName);
            //AutoSizeColumn(tcFunctionParameters);
            //AutoSizeColumn(treeFunctionAddress);
        }

        public IEnumerable<Function> AllFunctions()
        {
            var collector = new Collector();

            AllNodes.ForEach(n => ((DeviareNode)n.Tag).Accept(collector));

            return collector.CollectedFunctions;
        }
    }
}