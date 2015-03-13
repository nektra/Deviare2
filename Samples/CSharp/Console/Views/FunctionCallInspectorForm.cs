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

﻿using System.Windows.Forms;
using Aga.Controls.Tree;
using CSharpConsole.Controls;
using CSharpConsole.Model;
using CSharpConsole.Model.Visitors;
using CSharpConsole.Presenters;

namespace CSharpConsole.Views
{
    public partial class FunctionCallInspectorForm : Form, IFunctionCallInspectorView
    {
        #region Fields

        private FunctionCallInspectorPresenter _presenter;
        private readonly TreeModel _model;

        #endregion

        #region Instance Creation & Initialization

        private FunctionCallInspectorForm()
        {
            InitializeComponent();
        }

        private FunctionCallInspectorForm(FunctionCall aFunctionCall) : this()
        {
            _model = new TreeModel();
            treeViewAdv1.Model = _model;

            _presenter = new FunctionCallInspectorPresenter(this, aFunctionCall);
        }

        public static void Inspect(FunctionCall aFunctionCall)
        {
            var aForm = new FunctionCallInspectorForm(aFunctionCall);
            aForm.ShowDialog();
        }

        #endregion

        #region Implementation of IFunctionCallInspectorView

        public void DisplayFunctionName(string moduleName, string functionName)
        {
            Text = string.Format("Function call inspector: {0} ({1})", functionName, moduleName);
        }

        public void DisplayParameter(InspectedParameter anInspectedParameter)
        {
            DisplayParameter(anInspectedParameter, _model.Root);
        }

        private void DisplayParameter(InspectedParameter anInspectedParameter, Node parent)
        {
            var parameterNode = new ParameterNode(anInspectedParameter);

            foreach (var field in anInspectedParameter.Children)
            {
                DisplayParameter(field, parameterNode);
            }

            parent.Nodes.Add(parameterNode);
            AutosizeColumns();
        }

        #endregion

        #region Event Handlers

        private void InspectedParameterExpandedHandler(object sender, TreeViewAdvEventArgs e)
        {
            AutosizeColumns();
        }

        #endregion

        private void AutosizeColumns()
        {
            treeViewAdv1.AutoSizeColumn(tcType);
            treeViewAdv1.AutoSizeColumn(tcName);
            treeViewAdv1.AutoSizeColumn(tcValue);
        }
    }
}