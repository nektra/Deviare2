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
using CSharpConsole.Model.ProcessHandlers;
using CSharpConsole.Views;

namespace CSharpConsole.Model.FunctionCallHandlers
{
    internal class FunctionCallDispatcherHandler : NonTerminalHandler<FunctionCallInfo>
    {
        #region Fields

        private readonly FunctionCallDispatcher _callDispatcher;
        private readonly IApplicationView _view;

        #endregion

        #region Instance Creation

        public FunctionCallDispatcherHandler(IApplicationView aView, FunctionCallDispatcher callDispatcher)
        {
            _view = aView;
            _callDispatcher = callDispatcher;
        }

        #endregion

        #region Overrides of NonTerminalHandler<FunctionCallInfo>

        protected override bool HandleCore(FunctionCallInfo aFunctionCallInfo)
        {
            _callDispatcher.Dispatch(aFunctionCallInfo.HookCallInfo);
            _view.ClearOutputEnabled = true;

            return true;
        }

        protected override bool HandleCore(IEnumerable<FunctionCallInfo> objects)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}