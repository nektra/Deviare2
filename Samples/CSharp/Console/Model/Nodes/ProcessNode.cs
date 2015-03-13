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
using CSharpConsole.Controls;
using CSharpConsole.Model.Collections;
using CSharpConsole.Model.Modules;
using CSharpConsole.Model.Process;
using CSharpConsole.Model.Visitors;

namespace CSharpConsole.Model.Nodes
{
    public class ProcessNode : DeviareNode
    {
        #region Fields

        private readonly EntityDictionary<Module, string, ModuleNode> _moduleNodes;
        //private int _pid;

        #endregion

        #region Properties


        public string Pid { get; private set; }

        public Image Icon { get; private set; }

        public bool IsRunning { get; private set; }

        public IProcess Process { get; private set; }

        #endregion

        #region Instance Creation

        public ProcessNode(IProcess aProcess)
        {
            Text = aProcess.Name;
            IsRunning = true;
            Icon = aProcess.Icon;
            Process = aProcess;
            Pid = string.Empty;
            aProcess.IfRunning(p => Pid = p.Id.ToString());


            _moduleNodes = new EntityDictionary<Module, string, ModuleNode>(module => module.Path.ToLower(), ModuleNode.From);
        }

        public static ProcessNode NotRunning(IProcess aProcess)
        {
            var aNode = From(aProcess);
            aNode.IsRunning = false;
            return aNode;
        }

        public static ProcessNode From(IProcess aProcess)
        {
            return new ProcessNode(aProcess);
        }

        #endregion

        public override string DeviareNodeName
        {
            get { return Process.Name; }
        }

        public override void Accept(IDeviareNodeVisitor aVisitor)
        {
            aVisitor.VisitProcessNode(this);
        }

        public bool Equals(ProcessNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return Equals(other.Process, Process);
        }

        public bool Equals(ProcessGroupNode other)
        {
            return Equals(this, other.MainNode);            
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() == typeof(ProcessNode)) return Equals((ProcessNode)obj);
            if (obj.GetType() == typeof(ProcessGroupNode)) return Equals((ProcessGroupNode)obj);
            return false;
        }

        public override int GetHashCode()
        {
            return Process.GetHashCode();
        }

        public ModuleNode ModuleNodeFor(Module aModule)
        {
            var aModuleNode = _moduleNodes.ValueOrInitialize(aModule);

            Nodes.Add(aModuleNode);

            return aModuleNode;
        }

        public bool Represents(IProcess aProcess)
        {
            return Process.Equals(aProcess);
        }
    }
}