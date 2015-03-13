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
using CSharpConsole.Model.Modules;
using CSharpConsole.Model.Visitors;

namespace CSharpConsole.Model.Nodes
{
    public class ModuleNode : DeviareNode
    {
        public int HooksCount { get; private set; }

        public string Address { get; set; }

        public Module Module { get; private set; }

        public ModuleNode(Module aModule)
        {
            Module = aModule;
            Text = aModule.Name;
            HooksCount =  Module.HooksCount(); 
            Address = "0x" + aModule.BaseAddress.ToString("X8");
        }

        public override string DeviareNodeName
        {
            get { return Module.Name; }
        }

        public override void Accept(IDeviareNodeVisitor aVisitor)
        {
            aVisitor.VisitModuleNode(this);
        }

        public HookNode AddHook(Hook aHook, Function aFunction)
        {
            var aHookNode = new HookNode(aHook, aFunction);

            //var node = Nodes.FirstOrDefault(node1 => node1 .Equals(aHookNode));
            //if (node != null) return (HookNode)node;
            
            Nodes.Add(aHookNode);

            return aHookNode;
        }

        public static ModuleNode From(Module module)
        {
            return new ModuleNode(module);
        }

        public void RefreshHooksCount()
        {
            HooksCount = Module.HooksCount();
        }
    }
}