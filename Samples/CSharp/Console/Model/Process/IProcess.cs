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
using System.Drawing;
using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.Modules;
using CSharpConsole.Presenters;

namespace CSharpConsole.Model.Process
{
    public interface IProcess
    {
        string Name { get; }
        Image Icon { get; }
        bool IsActive { set; }
        string Path { get; }
        
        IEnumerable<Module> Modules();
        Module ModuleByName(string moduleName);

        void Hook(Module aModule, Function aFunction, Action<Hook> onHookAdded);
        IEnumerable<Hook> Hooks();
        bool IsHooked();
        bool AtLeastOneHookIsActive();

       
        bool HasName();
        bool HasPlatformBits(int platformBits);

        void IfRunning(Action<IRunningProcess> ifTrue);
        void IfNotRunning(Action<ProcessToBeHookedOnStart> ifTrue);

        void AddHookUsing(Module aModule, Function aFunction, ApplicationPresenter applicationPresenter);
        void IsNotHooked(Action<IRunningProcess> ifTrue);
        void AlLeastOneHookIsActive(Action<IRunningProcess> ifTrue, Action<IRunningProcess> ifFalse);
        void Start(Action<IRunningProcess> onBeforeProcessStarts);
        bool IsHooked(Module aModule, IEnumerable<Function> functions);
        bool IsRunning();
        int HookCountOn(Module aModule);
        bool IsToHookOnStart();
    }
}