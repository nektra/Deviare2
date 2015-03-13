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
using System.Xml;
using CSharpConsole.Properties;
using Nektra.Deviare2;

namespace CSharpConsole.Model.Parsing
{
    internal class MessageDescriptor : ParameterDescriptor
    {
        #region Fields

        private static XmlDocument _knownParameterDescriptions;
        private static HashSet<string> _knownParameterNames;
        private static HashSet<string> _structureMessageFunctions;
        private static HashSet<string> _plainMessageFunctions;

        #endregion

        #region Initialization

        static MessageDescriptor()
        {
            InitializeKnownParameterDescriptions();
            InitializeKnownParameterNames();
            InitializePlainMessageFunctionNames();
            InitializeStructureMessageFunctionNames();
        }

        private static void InitializeStructureMessageFunctionNames()
        {
            _structureMessageFunctions = new HashSet<string>
                                             {
                                                 "user32.dll!getmessagea", "user32.dll!getmessagew",
                                                 "user32.dll!peekmessagea", "user32.dll!peekmessagew",
                                                 "user32.dll!dispatchmessagea", "user32.dll!dispatchmessagew",
                                                 "user32.dll!translatemessage"
                                             };
        }

        private static void InitializePlainMessageFunctionNames()
        {
            _plainMessageFunctions = new HashSet<string>
                                         {
                                             "user32.dll!postmessagea", "user32.dll!postmessagew",
                                             "user32.dll!sendmessagea", "user32.dll!sendmessagew",
                                             "user32.dll!sendmessagecallbacka", "user32.dll!sendmessagecallbackw",
                                             "user32.dll!sendmessagetimeouta", "user32.dll!sendmessagetimeoutw",
                                             "user32.dll!sendnotifymessagea", "user32.dll!sendnotifymessagew",
                                             "user32.dll!postthreadmessagea", "user32.dll!postthreadmessagew"
                                         };
        }

        private static void InitializeKnownParameterNames()
        {
            _knownParameterNames = new HashSet<string> { "Msg", "Message", "uMsg", "message" };
        }

        private static void InitializeKnownParameterDescriptions()
        {
            _knownParameterDescriptions = new XmlDocument();
            _knownParameterDescriptions.LoadXml(Resources.FunctionTypes);
        }

        #endregion

        #region Overrides of ParameterDescriptor

        public override bool CanHandle(NktParam aParameter, string functionName)
        {
            return IsMessageFunction(functionName) && IsMessageParameter(aParameter.Name);
        }

        public override string GenerateDescription(NktParam aParameter, string functionName)
        {
            var description = DescriptionFor(aParameter);
            var formattedDescription = string.IsNullOrEmpty(description) ? string.Empty : " " + description;

            return string.Format("0x{0:X}{1}", (uint)aParameter.Value, formattedDescription);
        }

        #endregion

        #region Helper Methods

        private static string DescriptionFor(NktParam aParameter)
        {
            var messageCode = (uint)aParameter.Value;
            var descriptionNode = _knownParameterDescriptions.SelectSingleNode(DescriptionQueryFor(messageCode));

            if (descriptionNode == null) return string.Empty;

            if (string.IsNullOrEmpty(descriptionNode.InnerText))
            {
                return IsUserMessageCode(messageCode) ? DescriptionForUserMessage(messageCode) : string.Empty;
            }

            return DescriptionForMessage(descriptionNode);
        }

        private static string DescriptionForMessage(XmlNode descriptionNode)
        {
            return string.Format("({0})", descriptionNode.InnerText);
        }

        private static bool IsUserMessageCode(uint messageCode)
        {
            return messageCode > 0x400;
        }

        private static string DescriptionForUserMessage(uint messageCode)
        {
            return string.Format("(WM_USER + 0x{0})", messageCode - 0x400);
        }

        private static string DescriptionQueryFor(uint messageCode)
        {
            return string.Format("/messages/message[@value=\"{0:X}\"]/name", messageCode);
        }

        private bool IsMessageParameter(string parameterName)
        {
            return _knownParameterNames.Contains(parameterName);
        }

        private static bool IsMessageFunction(string functionName)
        {
            var functionNameInLowerCase = functionName.ToLower();
            return _plainMessageFunctions.Contains(functionNameInLowerCase) || _structureMessageFunctions.Contains(functionNameInLowerCase);
        }

        #endregion
    }
}