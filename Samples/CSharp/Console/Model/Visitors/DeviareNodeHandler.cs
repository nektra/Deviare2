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
using CSharpConsole.Controls;
using CSharpConsole.Extensions;
using CSharpConsole.Model.Nodes;

namespace CSharpConsole.Model.Visitors
{
    public class DeviareNodeHandler : IDeviareNodeVisitor
    {
        #region Fields

        private readonly Func<ProcessNode, bool> _processHandler = processNode => false;
        private readonly Func<ProcessGroupNode, bool> _processGroupHandler = processGroupNode => false;
        private readonly Func<ProcessCategoryNode, bool> _processCategoryHandler = processCategoryNode => false; 
        private readonly Func<ModuleNode, bool> _moduleHandler = moduleNode => false;
        private readonly Func<HookNode, bool> _hookHandler = hookNode => false;
        private readonly Func<FunctionNode, bool> _functionsHandler = functionNodes => false;
        private readonly Func<FunctionNodeGroup, bool> _functionGroupHandler = functionGroupNode => false;
        private readonly Action _noSelection;
        private bool _nodeWasHandled;

        #endregion

        #region Instance Creation

        private DeviareNodeHandler(Func<ProcessNode, bool> aProcessHandler, Func<ProcessGroupNode, bool> aProcessGroupHandler, Func<ProcessCategoryNode, bool> aProcessCategoryHandler, Action nodeNotFound)
        {
            _processHandler = aProcessHandler;
            _processGroupHandler = aProcessGroupHandler;
            _processCategoryHandler = aProcessCategoryHandler;
            _noSelection = nodeNotFound;
        }

        private DeviareNodeHandler(Func<ModuleNode, bool> aModuleHandler, Action nodeNotFound)
        {
            _moduleHandler = aModuleHandler;
            _noSelection = nodeNotFound;
        }

        private DeviareNodeHandler(Func<FunctionNode, bool> aFunctionsHandler, Func<FunctionNodeGroup, bool> aFunctionGroupHandler, Action nodeNotFound)
        {
            _functionsHandler = aFunctionsHandler;
            _functionGroupHandler = aFunctionGroupHandler;
            _noSelection = nodeNotFound;
        }

        private DeviareNodeHandler(Func<HookNode, bool> aHookHandler, Action nodeNotFound )
        {
            _hookHandler = aHookHandler;
            _noSelection = nodeNotFound;
        }

        public static DeviareNodeHandler ForProcesses(Func<ProcessNode, bool> aProcessHandler, Func<ProcessGroupNode, bool> aProcessGroupHandler, Func<ProcessCategoryNode, bool> aProcessCategoryHandler, Action nodeNotFound)
        {
            return new DeviareNodeHandler(aProcessHandler, aProcessGroupHandler, aProcessCategoryHandler, nodeNotFound);
        }

        public static DeviareNodeHandler ForModules(Func<ModuleNode, bool> aModuleHandler, Action nodeNotFound)
        {
            return new DeviareNodeHandler(aModuleHandler, nodeNotFound);
        }

        public static DeviareNodeHandler ForHooks(Func<HookNode, bool> aHookHandler)
        {
            return new DeviareNodeHandler(aHookHandler, () => { });
        }

        public static DeviareNodeHandler ForFunctions(Func<FunctionNode, bool> aFunctionHandler, Func<FunctionNodeGroup, bool> aFunctionGroupHandler, Action nodeNotFound)
        {
            return new DeviareNodeHandler(aFunctionHandler, aFunctionGroupHandler, nodeNotFound);
        }

        #endregion

        #region Implementation of IDeviareNodeVisitor

        public void VisitProcessNode(ProcessNode processNode)
        {
            _nodeWasHandled = _processHandler(processNode);
        }

        public void VisitProcessGroupNode(ProcessGroupNode aProcessGroupNode)
        {
            _nodeWasHandled = _processGroupHandler(aProcessGroupNode);
        }

        public void VisitModuleNode(ModuleNode aModuleNode)
        {
            _nodeWasHandled = _moduleHandler(aModuleNode);
        }

        public void VisitHookNode(HookNode hookNode)
        {
            _nodeWasHandled = _hookHandler(hookNode);
        }

        public void VisitFunctionNode(FunctionNode functionNode)
        {
            _nodeWasHandled = _functionsHandler(functionNode);
        }

        public void VisitFunctionGroupNode(FunctionNodeGroup aFunctionNodeGroup)
        {
            _nodeWasHandled = _functionGroupHandler(aFunctionNodeGroup);
        }

        #endregion

        public void Visit(DeviareNode aNode)
        {
            aNode.Accept(this);
            if (!_nodeWasHandled) _noSelection();
        }
    }
}