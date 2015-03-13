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
using System.Text.RegularExpressions;
using CSharpConsole.Model.Visitors;
using CSharpConsole.Properties;

namespace CSharpConsole.Model.Nodes
{
    public class FunctionNode : DeviareNode
    {
        #region Properties

        public string Address { get; set; }

        public Function Function { get; private set; }

        public Image Icon { get; private set; }

        public bool FunctionIsDeclared { get; private set; }

        #endregion

        #region Instance Creation

        public FunctionNode(Function aFunction, bool displayFunctionsSignatureAndReturnType)
        {
            Function = aFunction;
            FunctionIsDeclared = aFunction.HasDeclaration;

            Text = aFunction.FunctionNameWithoutSuffix();

            if (displayFunctionsSignatureAndReturnType && FunctionIsDeclared)
            {
                PopulateFunctionInfoFromDeclaration(aFunction);
            }
            else
            {
                PopulateFunctionInfoFromName(aFunction);
            }

            if (aFunction.IsForwarded)
                FunctionName += " -> Forwarded";

            Address = "0x" + aFunction.BaseAddress.ToString("X8");

            AssignNodeIcon(FunctionIsDeclared);
        }

        private void PopulateFunctionInfoFromName(Function aFunction)
        {
            FunctionName = aFunction.Name;
            ReturnType = string.Empty;
            Parameters = string.Empty;
        }

        private void PopulateFunctionInfoFromDeclaration(Function aFunction)
        {
            var functionSections = Regex.Match(aFunction.Declaration, @"(\w+)( \w+)? (\w+)\((.+?)\)");
            ReturnType = functionSections.Groups[1].Value;
            Parameters = functionSections.Groups[4].Value;
            FunctionName = functionSections.Groups[3].Value;
        }

        public string Parameters { get; set; }

        public string ReturnType { get; set; }

       

        public string FunctionName { get; private set; }

        #endregion

        public override string DeviareNodeName
        {
            get { throw new System.NotImplementedException(); }
        }

        public override void Accept(IDeviareNodeVisitor aVisitor)
        {
            aVisitor.VisitFunctionNode(this);
        }

        private void AssignNodeIcon(bool isDocumented)
        {
            Icon = isDocumented ? Resources.Book_16x16 : Resources.BlankImage;
        }
    }
}