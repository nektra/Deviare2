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
using CSharpConsole.Model.Modules;
using CSharpConsole.Presenters;

namespace CSharpConsole.Model.Process
{
    public class ProcessToBeHookedOnStart : ProcessBase
    {
        #region Fields

        //private IEnumerable<Module> _modules;

        #endregion

        #region Instance Creation

        public ProcessToBeHookedOnStart(string aPath, ISpyManager aSpyManager, int platformBits)
            : base(aSpyManager, platformBits, System.IO.Path.GetFileName(aPath), aPath, ProcessIcon.IconFrom(aPath))
        {
        }

        #endregion

        #region Equality Members

        public bool Equals(ProcessToBeHookedOnStart other)
        {
            return Path.Equals(other.Path, StringComparison.InvariantCulture);
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != typeof(ProcessToBeHookedOnStart)) return false;
            return Equals((ProcessToBeHookedOnStart)obj);
        }

        public override int GetHashCode()
        {
            return Path.GetHashCode();
        }

        #endregion

        #region Overrides of ProcessBase

        public override Module ModuleByName(string moduleName)
        {
            return Modules().FirstOrDefault(module => module.Name.Equals(moduleName, StringComparison.InvariantCultureIgnoreCase));
        }

        public override void Hook(Module aModule, Function aFunction, Action<Hook> onHookAdded)
        {
            throw new InvalidOperationException();
        }

        public override IEnumerable<Hook> Hooks()
        {
            return Enumerable.Empty<Hook>();
        }

        public override bool IsHooked()
        {
            return false;
        }

        public override bool AtLeastOneHookIsActive()
        {
            return false;
        }

        public override void IfRunning(Action<IRunningProcess> ifTrue)
        {
        }

        public override void IfNotRunning(Action<ProcessToBeHookedOnStart> ifTrue)
        {
            ifTrue(this);
        }

        public override void AddHookUsing(Module aModule, Function aFunction, ApplicationPresenter applicationPresenter)
        {
            applicationPresenter.AddHookForProcessToBeHooked(this, aModule, aFunction);
        }

        public override void IsNotHooked(Action<IRunningProcess> ifTrue)
        {
        }

        public override void AlLeastOneHookIsActive(Action<IRunningProcess> ifTrue, Action<IRunningProcess> ifFalse)
        {
        }

        public override bool IsRunning()
        {
            return false;
        }

        public override int HookCountOn(Module aModule)
        {
            throw new NotImplementedException();
        }

        public override bool IsToHookOnStart()
        {
            return true;
        }

        public override IEnumerable<Module> Modules()
        {
            return SpyManager.HookableModulesFor(PlatformBits);
        }

        #endregion
    }
}