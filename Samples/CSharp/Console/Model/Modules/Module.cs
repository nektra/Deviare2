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
using System.Linq;
using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.Process;
using CSharpConsole.Presenters;
using Nektra.Deviare2;

namespace CSharpConsole.Model.Modules
{
    public abstract class Module
    {
        #region Properties

        public string Path { get; protected set; }
        public string Name { get; private set; }
        public IntPtr BaseAddress { get; private set; }

        #endregion

        #region Instance Creation

        protected Module() { }

        protected Module(string path, string name, IntPtr baseAddress)
        {
            Path = path;
            Name = name;
            BaseAddress = baseAddress;
        }

        public static Module From(NktModule module, IRunningProcess aProcess, ISpyManager aSpyManager)
        {
            return new RunningModule(module.Path, module.Name, module.BaseAddress, aProcess, aSpyManager);
        }

        public static Module[] From(NktDbModulesEnum dbModules)
        {
            var modules = new Module[dbModules.Count];

            for (var i = 0; i < dbModules.Count; i++)
            {
                var dbModule = dbModules.GetAt(i);
                var functions = Function.From(dbModule.DbFunctions());
                modules[i] = new DataBaseModule(@"<not available>\" + dbModule.Name, dbModule.Name, IntPtr.Zero, functions);
            }

            return modules;
        }

        #endregion

        public abstract Function[] Functions();

        public abstract Hook[] Hooks();

        public abstract int HooksCount();

        public Function FunctionByName(string functionName)
        {
            return Functions().FirstOrDefault(f => f.Name == functionName);
        }

        #region Equality Members

        public bool Equals(Module other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return Equals(other.Name, Name);
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != typeof (RunningModule)) return false;
            return Equals((((Module) obj)));
        }

        public override int GetHashCode()
        {
            return (Name != null ? Name.GetHashCode() : 0);
        }

        #endregion

        public bool HasName(string aName)
        {
            return Name.Equals(aName, StringComparison.InvariantCultureIgnoreCase);
        }

        #region Formatting Members

        public override string ToString()
        {
            return string.Format("Name: {0}", Name);
        }

        #endregion

        public static Module From(string modulePath)
        {
            return new RunningModule(modulePath);
        }
    }
}