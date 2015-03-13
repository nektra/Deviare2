using System;
using System.Collections.Generic;
using System.Linq;
using DeviareCSharpConsole.Extensions;
using DeviareCSharpConsole.Model.Hooks;
using DeviareCSharpConsole.Model.Process;

namespace DeviareCSharpConsole.Model.Modules
{
    public class ModuleClass : Module, IEquatable<ModuleClass>
    {
        public Module Module { get; private set; }
        public IEnumerable<IProcess> Processes { get; private set; }

        public ModuleClass(string path, string name, IntPtr baseAddress, Module module, IEnumerable<IProcess> processes)
            : base(path, name, baseAddress)
        {
            Module = module;
            Processes = processes;
        }

        private static ModuleClass From(Module module, IEnumerable<IProcess> processes)
        {
            return new ModuleClass(module.Path, module.Name, module.BaseAddress, module, processes);
        }

        public static ModuleClass[] ModuleClassesFrom(IEnumerable<IProcess> processes)
        {
            // ReSharper disable PossibleMultipleEnumeration
            var modules = processes.Select(p => p.Modules()).Union().Cast<RunningModule>();

            return modules.Select(rm => From(rm, processes.Where(p => rm.BelongsToProcessNamed(p.Name)))).ToArray();

            //return modules.Select(m => From(m, processes.Where(p => p.Modules().Contains(m)))).ToArray();
        }

        public override Function[] Functions()
        {
            return Module.Functions();
        }

        public override Hook[] Hooks()
        {
            throw new NotImplementedException();
            return Processes.SelectMany(p => p.Hooks()).Where(h => h.ModuleName.Equals(Module.Name)).ToArray();
        }

        public override int HooksCount()
        {
            return Processes.Sum(p => p.HookCountOn(Module.Name));
            return Hooks().Count();
        }

        #region Equality members
        public bool Equals(ModuleClass other)
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
            return Equals(obj as ModuleClass);
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

        public static bool operator ==(ModuleClass left, ModuleClass right)
        {
            return Equals(left, right);
        }

        public static bool operator !=(ModuleClass left, ModuleClass right)
        {
            return !Equals(left, right);
        }
        #endregion
    }
}