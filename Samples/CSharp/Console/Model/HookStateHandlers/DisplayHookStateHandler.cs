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

using System.Collections.Generic;
using System.Threading;
using CSharpConsole.Model.Hooks;
using CSharpConsole.Model.ProcessHandlers;
using CSharpConsole.Presenters;
using CSharpConsole.Views;
using Nektra.Deviare2;

namespace CSharpConsole.Model.HookStateHandlers
{
    internal class DisplayHookStateHandler : NonTerminalHandler<HookStateChangeInfo>
    {
        private readonly IApplicationView _view;
        private readonly ISpyManager _spyManager;

        public DisplayHookStateHandler(IApplicationView aView, ISpyManager aSpyManager)
        {
            _view = aView;
            _spyManager = aSpyManager;
        }

        #region Implementation of IHookStateChangedHandler

        protected override bool HandleCore(HookStateChangeInfo hookState)
        {
            var aHook = hookState.Hook;
            if (hookState.CurrentState == eNktHookState.stRemoved)
            {
                RemoveHook(aHook);
            }
            else
                _view.DisplayHookStatus(hookState);

            return true;
        }

        private void RemoveHook(Hook aHook)
        {
            _view.RemoveHook(aHook);
            _view.RemoveHookEnabled = false;
            _view.RemoveAllHooksEnabled = _spyManager.AtLeastOneHookWasCreated;
            _view.ExportHookingSettings = _spyManager.AtLeastOneHookWasCreated;
        }

        protected override bool HandleCore(IEnumerable<HookStateChangeInfo> objects)
        {
            throw new System.NotImplementedException();
        }

        #endregion
    }
}