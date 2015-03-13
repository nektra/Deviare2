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

using Aga.Controls.Tree;
using CSharpConsole.Model;

namespace CSharpConsole.Controls
{
    public class OutputNode : Node
    {
        #region Properties

        public FunctionCall FunctionCall { get; private set; }

        public int Tid { get; private set; }

        public string ModuleName { get; set; }

        public string Result { get; private set; }

        public string FunctionName { get; set; }

        public string Parameters { get; private set; }

        public int Order { get; private set; }

        #endregion

        #region Instance Creation & Initialization

        public OutputNode(FunctionCall aFunctionCall)
        {
            InitializeProperties(aFunctionCall);
        }

        private void InitializeProperties(FunctionCall aFunctionCall)
        {
            FunctionCall = aFunctionCall;
            Tid = aFunctionCall.ThreadId;
            ModuleName = aFunctionCall.ModuleName;
            FunctionName = aFunctionCall.FunctionName;
            Order = aFunctionCall.Order;
            Parameters = aFunctionCall.Summary;
            Result = aFunctionCall.Result.FullDescription;
        }

        #endregion

        public bool FunctionMatches(FunctionCall aFunctionCall)
        {
            return FunctionCall == aFunctionCall;
        }

        public void UpdateFrom(FunctionCall aFunctionCall)
        {
            InitializeProperties(aFunctionCall);
        }
    }
}