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
using CSharpConsole.Extensions;
using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.Process;

namespace CSharpConsole.Model.Modules
{
    public class ModuleStatus : Module, IEquatable<ModuleStatus>
    {
        private Module Module { get; set; }
        private IEnumerable<IProcess> Processes { get; set; }

        private ModuleStatus(string path, string name, IntPtr baseAddress, Module module, IEnumerable<IProcess> processes)
            : base(path, name, baseAddress)
        {
            Module = module;
            Processes = processes;
        }

        private static ModuleStatus From(Module module, IEnumerable<IProcess> processes)
        {
            return new ModuleStatus(module.Path, module.Name, module.BaseAddress, module, processes);
        }

        public static ModuleStatus[] From(IEnumerable<IProcess> processes)
        {
            
            var modules = processes.Select(p => p.Modules()).Union().Cast<RunningModule>().ToArray();

            return modules.Select(rm => From(rm, processes.Where(p => rm.BelongsToProcessNamed(p.Name)))).ToArray();
        }

        public override Function[] Functions()
        {
            return Module.Functions();
        }

        public override Hook[] Hooks()
        {
            throw new NotImplementedException();
            //return Processes.SelectMany(p => p.Hooks()).Where(h => h.ModuleName.Equals(Module.Name)).ToArray();
        }

        public override int HooksCount()
        {
            return Processes.Sum(p => p.HookCountOn(Module));
        }

        #region Equality members
        public bool Equals(ModuleStatus other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return base.Equals(other) && Equals(other.Module, Module) &&
                   Processes.All(p => other.Processes.Count(op => op.Equals(p)) == Processes.Count(op => op.Equals(p))) 
                   && other.Processes.Count().Equals(Processes.Count());
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            return Equals(obj as ModuleStatus);
        }

        public override int GetHashCode()
        {
            unchecked
            {
                int result = base.GetHashCode();
                result = (result * 397) ^ (Module != null ? Module.GetHashCode() : 0);
                result = (result * 397) ^ (Processes != null ? Processes.GetHashCode() : 0);
                return result;
            }
        }

        public static bool operator ==(ModuleStatus left, ModuleStatus right)
        {
            return Equals(left, right);
        }

        public static bool operator !=(ModuleStatus left, ModuleStatus right)
        {
            return !Equals(left, right);
        }
        #endregion
    }
}