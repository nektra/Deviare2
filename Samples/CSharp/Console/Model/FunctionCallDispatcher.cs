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
using System.Threading;
using CSharpConsole.Model.Collections;
using Nektra.Deviare2;

namespace CSharpConsole.Model
{
    public class FunctionCallDispatcher
    {
        #region Fields

        private readonly Action<FunctionCall> _dispatchDispatchRootCall;
        private readonly Action<FunctionCall> _dispatchDispatchChildCall;
        private readonly Action<FunctionCall> _dispatchRootCallUpdate;

        private readonly object _dataLock = new object();
        private int _functionCallsCount = -1;

        private readonly EntityDictionary<INktHookCallInfo, int, int> _functionCallCountByThread = new EntityDictionary<INktHookCallInfo, int, int>(info => info.ThreadId, info => 0);
        private readonly EntityDictionary<INktHookCallInfo, string, int> _functionCallsOrder = new EntityDictionary<INktHookCallInfo, string, int>(info => info.ThreadId + info.Hook().FunctionName, info => 0);

        #endregion

        #region Instance Creation

        public FunctionCallDispatcher(Action<FunctionCall> dispatchRootCall, Action<FunctionCall> dispatchChildCall, Action<FunctionCall> dispatchRootCallUpdate)
        {
            new HashSet<int>();
            _dispatchDispatchRootCall = dispatchRootCall;
            _dispatchDispatchChildCall = dispatchChildCall;
            _dispatchRootCallUpdate = dispatchRootCallUpdate;
        }

        #endregion

        public void Dispatch(INktHookCallInfo aCallInfo)
        {
            lock (_dataLock)
            {
                if (aCallInfo.IsPreCall)
                {
                    ProcessPreCall(aCallInfo);
                    return;
                }

                ProcessPostCall(aCallInfo);
            }
        }

        private void ProcessPreCall(INktHookCallInfo preCallInfo)
        {
            AssignFunctionCallOrderTo(preCallInfo);
            IncrementNestedFunctionsCalledByThreadOf(preCallInfo);

            var functionCall = FunctionCallFrom(preCallInfo);
            Dispatch(preCallInfo,functionCall);
        }

        private void ProcessPostCall(INktHookCallInfo postCallInfo)
        {
            UpdateFunctionCall(postCallInfo);
            DecrementNestedFunctionsCalledByThreadOf(postCallInfo);
        }

        #region Number Of Nested Function Calls By Thread

        private void IncrementNestedFunctionsCalledByThreadOf(INktHookCallInfo aCallInfo)
        {
            _functionCallCountByThread.Update(aCallInfo, i => i + 1);
        }

        private void DecrementNestedFunctionsCalledByThreadOf(INktHookCallInfo aCallInfo)
        {
            _functionCallCountByThread.Update(aCallInfo, i => i - 1);
        }

        private int NestedFunctionCallCountOf(INktHookCallInfo aCallInfo)
        {
            return _functionCallCountByThread.ValueOrInitialize(aCallInfo);
        }

        #endregion

        private void AssignFunctionCallOrderTo(INktHookCallInfo preCallInfo)
        {
            var functionCallOrder = Interlocked.Increment(ref _functionCallsCount);
            _functionCallsOrder.Update(preCallInfo, i => functionCallOrder);
        }

        private void UpdateFunctionCall(INktHookCallInfo postCallInfo)
        {
            var aFunctionCall = FunctionCallFrom(postCallInfo);
            _dispatchRootCallUpdate(aFunctionCall);
        }

        private void Dispatch(INktHookCallInfo aCallInfo, FunctionCall aFunctionCall)
        {
            if (IsRootCall(aCallInfo))
            {
                _dispatchDispatchRootCall(aFunctionCall);
                return;
            }
            
            _dispatchDispatchChildCall(aFunctionCall);
        }

        private bool IsRootCall(INktHookCallInfo aCallInfo)
        {
            var count = NestedFunctionCallCountOf(aCallInfo);

            // This is the only function call for aCallInfo.ThreadId. So, it's the root call.
            return count == 1;
        }

        private FunctionCall FunctionCallFrom(INktHookCallInfo aCallInfo)
        {
            var functionCallOrder = _functionCallsOrder.Value(aCallInfo);
            var aFunctionCall = FunctionCall.From(aCallInfo, functionCallOrder);

            return aFunctionCall;
        }

        public void Clear()
        {
            lock (_dataLock)
            {
                _functionCallCountByThread.Clear();
            }
        }
    }
}