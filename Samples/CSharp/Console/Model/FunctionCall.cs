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
using System.Linq;
using CSharpConsole.Extensions;
using CSharpConsole.Model.Visitors;
using CSharpConsole.Views;
using Nektra.Deviare2;

namespace CSharpConsole.Model
{
    public class FunctionCall
    {
        #region Properties

        protected int CallId { get; private set; }
        public int ThreadId { get; private set; }
        public int Order { get; private set; }
        public string ModuleName { get; private set; }
        public string FunctionName { get; private set; }
        public string Summary { get; private set; }
        public List<InspectedParameter> Parameters { get; private set; }
        public InspectedParameter Result { get; set; }

        #endregion

        #region Instance Creation

        private FunctionCall(int callId, int threadId, int order, string moduleName, string functionName, string summary, List<InspectedParameter> parameters, InspectedParameter result)
        {
            CallId = callId;
            ThreadId = threadId;
            Order = order;
            ModuleName = moduleName;
            FunctionName = functionName;
            Summary = summary;
            Parameters = parameters;
            Result = result;
        }

        public static FunctionCall From(INktHookCallInfo aCallInfo, int order)
        {
            return aCallInfo.IsPreCall ? FromPreCall(aCallInfo, order) : FromPostCall(aCallInfo, order);
        }

        private static FunctionCall FromPostCall(INktHookCallInfo aCallInfo, int order)
        {
            var inspectedParameters = InspectParametersFrom(aCallInfo);
            var inspectedResult = InspectResultFrom(aCallInfo);
            var summary = CreateSummaryFrom(inspectedParameters);

            return From(aCallInfo, order, inspectedResult, inspectedParameters, summary);
        }

        public static FunctionCall FromPreCall(INktHookCallInfo aPreCallInfo, int order)
        {
            return From(aPreCallInfo, order, InspectedParameter.Empty(), new List<InspectedParameter>(), string.Empty);
        }

        private static FunctionCall From(INktHookCallInfo aCallInfo, int order, InspectedParameter inspectedResult, List<InspectedParameter> inspectedParameters, string summary)
        {
            return new FunctionCall(aCallInfo.GetHashCode(), aCallInfo.ThreadId, order, aCallInfo.Hook().ModuleName(), aCallInfo.Hook().FunctionNameWithoutModule(), summary, inspectedParameters, inspectedResult);
        }

        private static string CreateSummaryFrom(List<InspectedParameter> inspectedParameters)
        {
            var summaryBuilder = new InspectedParameterSummaryBuilder();
            return summaryBuilder.BuildSummaryFor(inspectedParameters);
        }

        private static List<InspectedParameter> InspectParametersFrom(INktHookCallInfo callInfo)
        {
            if (callInfo.IsPreCall) return new List<InspectedParameter>();

            var functionParameters = callInfo.Params().CollectAll();

            var parameterInspector = CreateParameterInspectorFor(callInfo);
            parameterInspector.Inspect(functionParameters);

            var fn = callInfo.Hook().FunctionName;
            return parameterInspector.InspectedParameters;
        }

        private static ParameterInspectorVisitor CreateParameterInspectorFor(INktHookCallInfo callInfo)
        {
            var parameterInspector = new ParameterInspectorVisitor(callInfo.Hook().FunctionName) { Depth = new ApplicationSettings().MaximumParameterInspectionDepth };
            return parameterInspector;
        }

        private static InspectedParameter InspectResultFrom(INktHookCallInfo aCallInfo)
        {
            if (aCallInfo.IsPreCall) return InspectedParameter.Empty();

            var result = aCallInfo.Result();
            var inspector = CreateParameterInspectorFor(aCallInfo);

            inspector.Inspect(new[] { result });

            return inspector.InspectedParameters.First();
        }

        #endregion

        #region Equality Members

        public bool Equals(FunctionCall other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return other.Order == Order && other.ThreadId == ThreadId && other.ModuleName == ModuleName && other.FunctionName == FunctionName;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != typeof(FunctionCall)) return false;
            return Equals((FunctionCall)obj);
        }

        public override int GetHashCode()
        {
            unchecked
            {
                return (Order * 397) ^ CallId;
            }
        }

        public static bool operator ==(FunctionCall left, FunctionCall right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(FunctionCall left, FunctionCall right)
        {
            return !(left == right);
        }

        #endregion

        public override string ToString()
        {
            return string.Format("CallId: {0}, ThreadId: {1}, Order: {2}, ModuleName: {3}, FunctionName: {4}, Summary: {5}, Result: {6}", CallId, ThreadId, Order, ModuleName, FunctionName, Summary, Result);
        }
    }
}