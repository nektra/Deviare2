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
using CSharpConsole.Model.Process;

namespace CSharpConsole.Model.Hooks
{
    public class HookingRule
    {
        #region Fields

        public string ProcessName { get; private set; }
        public string ModuleName { get; private set; }
        public string FunctionName { get; private set; }

        #endregion

        public HookingRule(string aProcessName, string aModuleName, string aFunctionName)
        {
            ProcessName = aProcessName.ToLower();
            ModuleName = aModuleName;
            FunctionName = aFunctionName;
        }

        #region Equality

        public bool Equals(HookingRule other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return AppliesOnProcessNamed(other.ProcessName) && Matches(other.ModuleName, other.FunctionName);
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != typeof(HookingRule)) return false;
            return Equals((HookingRule)obj);
        }

        public override int GetHashCode()
        {
            unchecked
            {
                int result = (ProcessName != null ? ProcessName.ToLower().GetHashCode() : 0);
                result = (result * 397) ^ (ModuleName != null ? ModuleName.ToLower().GetHashCode() : 0);
                result = (result * 397) ^ (FunctionName != null ? FunctionName.ToLower().GetHashCode() : 0);
                return result;
            }
        }

        #endregion

        public bool Matches(string aModuleName, string aFunctionName)
        {
            return
                ModuleName.Equals(aModuleName, StringComparison.InvariantCultureIgnoreCase) &&
                FunctionName.Equals(aFunctionName, StringComparison.InvariantCultureIgnoreCase);
        }

        public bool AppliesOn(IProcess aProcess)
        {
            return AppliesOnProcessNamed(aProcess.Name);
        }

        public override string ToString()
        {
            return string.Format("ProcessName: {0}, ModuleName: {1}, FunctionName: {2}", ProcessName, ModuleName, FunctionName);
        }

        public bool AppliesOnProcessNamed(string aProcessName)
        {
            return ProcessName.Equals(aProcessName, StringComparison.InvariantCultureIgnoreCase);
        }
    }
}