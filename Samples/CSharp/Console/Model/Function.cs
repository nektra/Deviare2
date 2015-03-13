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
using Nektra.Deviare2;

namespace CSharpConsole.Model
{
    public class Function
    {
        public string Name { get; private set; }

        public bool HasDeclaration { get; private set; }

        public string Declaration { get; private set; }

        public bool IsForwarded { get; private set; }

        public IntPtr BaseAddress { get; private set; }

        private Function(string name, bool hasDeclaration, string declaration, bool isForwarded, IntPtr baseAddress)
        {
            Name = name;
            HasDeclaration = hasDeclaration;
            Declaration = declaration;
            IsForwarded = isForwarded;
            BaseAddress = baseAddress;
        }

        public static Function[] From(IEnumerable<NktExportedFunction> functions)
        {
            return functions.Select(f => From(f)).ToArray();
        }

        private static Function From(NktExportedFunction function)
        {
            return new Function(function.Name, function.HasDeclaration, function.Declaration, function.IsForwarded, function.AddrBase);
        }

        public bool Equals(Function other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return Equals(other.Name, Name);
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != typeof(Function)) return false;
            return Equals((Function)obj);
        }

        public override int GetHashCode()
        {
            return (Name != null ? Name.GetHashCode() : 0);
        }






        public string FunctionNameWithoutSuffix()
        {
            var endingsToBeRemoved = new[] { "ExA", "ExW", "A", "W", "2", "Ex" };
            if (!endingsToBeRemoved.Any(Name.EndsWith)) return Name;

            var matchingEnding = endingsToBeRemoved.First(Name.EndsWith);
            return Name.Remove(Name.Length - matchingEnding.Length);
        }

        public static Function[] From(NktExportedFunctionsEnum exportedFunctions)
        {
            return From(exportedFunctions.CollectAll());
        }

        public static Function[] From(NktDbObjectsEnum dbFunctions)
        {
            var functions = new Function[dbFunctions.Count];

            for (var i = 0; i < dbFunctions.Count; i++)
            {
                var dbFunction = dbFunctions.GetAt(i);
                functions[i] = new Function(dbFunction.Name, true, dbFunction.Declaration, false, IntPtr.Zero);

            }

            return functions;
        }

        public bool HasName(string aName)
        {
            return Name.Equals(aName, StringComparison.InvariantCultureIgnoreCase);
        }
    }
}