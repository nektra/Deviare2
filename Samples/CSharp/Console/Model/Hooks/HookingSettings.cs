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
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;
using CSharpConsole.Model.Process;

namespace CSharpConsole.Model.Hooks
{
    public class HookingSettings
    {
        private readonly HashSet<HookingRule> _hookingRules = new HashSet<HookingRule>();

        public bool IsEmpty
        {
            get { return !_hookingRules.Any(); }
        }

        public void Add(string aProcessName, string aModuleName, string aFunctionName)
        {
            var aHookingRule = new HookingRule(aProcessName, aModuleName, aFunctionName);
            if (_hookingRules.Contains(aHookingRule)) return;
            _hookingRules.Add(aHookingRule);
        }

        public static HookingSettings With(string aProcessName, string aModuleName, string aFunctionName)
        {
            var hookingSettings = new HookingSettings();
            hookingSettings.Add(aProcessName, aModuleName, aFunctionName);

            return hookingSettings;
        }

        #region Equality

        public bool Equals(HookingSettings other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return _hookingRules.SetEquals(other._hookingRules);
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != typeof(HookingSettings)) return false;
            return Equals((HookingSettings)obj);
        }

        public override int GetHashCode()
        {
            return (_hookingRules != null ? _hookingRules.GetHashCode() : 0);
        }

        #endregion

        public void RemoveWhere(Predicate<HookingRule> match)
        {
            _hookingRules.RemoveWhere(match);
        }

        public bool AppliesOn(IProcess aProcess)
        {
            return _hookingRules.Any(rule => rule.AppliesOn(aProcess));
        }

        public void EachRuleDo(Action<HookingRule> anAction)
        {
            foreach (var hookingRule in _hookingRules)
            {
                anAction(hookingRule);
            }
        }

        public void AddRulesFrom(HookingSettings other)
        {
            _hookingRules.UnionWith(other._hookingRules);
        }

        #region Formatting

        public string ToXml()
        {
            using (var stream = new MemoryStream())
            {
                using (var writer = new XmlTextWriter(stream, Encoding.UTF8))
                {
                    writer.Formatting = Formatting.Indented;

                    writer.WriteStartDocument();
                    writer.WriteStartElement("HookingSettings");

                    foreach (var hookingRule in _hookingRules)
                    {
                        writer.WriteStartElement("HookingRule");
                        writer.WriteAttributeString("processName", hookingRule.ProcessName);
                        writer.WriteAttributeString("moduleName", hookingRule.ModuleName);
                        writer.WriteAttributeString("functionName", hookingRule.FunctionName);
                        writer.WriteEndElement();
                    }

                    writer.WriteEndElement();
                    writer.WriteEndDocument();

                    writer.Flush();
                    stream.Position = 0;

                    using (var reader = new StreamReader(stream))
                    {
                        return reader.ReadToEnd();
                    }
                }
            }
        }

        public static HookingSettings FromXml(Stream stream)
        {
            var hookingSettings = new HookingSettings();

            using (var reader = new XmlTextReader(stream))
            {
                reader.WhitespaceHandling = WhitespaceHandling.None;
                reader.ReadStartElement("HookingSettings");

                while (!reader.EOF)
                {
                    if (reader.NodeType != XmlNodeType.Element)
                    {
                        reader.Read();
                        continue;
                    }

                    var processName = reader.GetAttribute("processName");
                    var moduleName = reader.GetAttribute("moduleName");
                    var functionName = reader.GetAttribute("functionName");
                    hookingSettings.Add(processName, moduleName, functionName);
                    reader.ReadStartElement("HookingRule");
                }
            }

            return hookingSettings;
        }

        public override string ToString()
        {
            return _hookingRules.Aggregate(string.Empty, (result, rule) => result + "[ " + rule + "] ");
        }

        #endregion

        public bool HasRuleMatching(string aProcessName, string aModuleName, string aFunctionName)
        {
            return _hookingRules.Any(rule => rule.AppliesOnProcessNamed(aProcessName) && rule.Matches(aModuleName, aFunctionName));
        }
    }
}