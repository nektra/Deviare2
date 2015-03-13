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
using CSharpConsole.Model.Process;

namespace CSharpConsole.Model
{
    public class ProcessClasifier
    {
        #region Fields

        #endregion

        #region Properties

        public List<IProcess> ToHookOnStart { get; private set; }
        public List<IRunningProcess> UnHooked { get; private set; }
        public List<IRunningProcess> HookedInactive { get; private set; }
        public List<IRunningProcess> HookedActive { get; private set; }

        #endregion

        public ProcessClasifier()
        {
            ToHookOnStart = new List<IProcess>();
            UnHooked = new List<IRunningProcess>();
            HookedInactive = new List<IRunningProcess>();
            HookedActive = new List<IRunningProcess>();
        }

        public void Categorize(IProcess aProcess)
        {
            Categorize(aProcess, p => ToHookOnStart.Add(p), p => UnHooked.Add(p), p => HookedInactive.Add(p), p => HookedActive.Add(p));
        }

        public void Categorize(IProcess aProcess, Action<ProcessToBeHookedOnStart> toHookOnStart, Action<IRunningProcess> unhooked, Action<IRunningProcess> hookedInactive, Action<IRunningProcess> hookedActive)
        {
            aProcess.IfNotRunning(toHookOnStart);
            aProcess.IsNotHooked(unhooked);
            aProcess.AlLeastOneHookIsActive(hookedActive, hookedInactive);
        }

        public void Categorize(IEnumerable<IProcess> processesToBeDisplayed)
        {
            foreach (var process in processesToBeDisplayed)
            {
                Categorize(process);
            }
        }
    }
}