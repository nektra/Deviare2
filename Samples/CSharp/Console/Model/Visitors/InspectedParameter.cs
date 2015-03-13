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
using CSharpConsole.Extensions;
using CSharpConsole.Model.Parsing;
using Nektra.Deviare2;
using System.Linq;

namespace CSharpConsole.Model.Visitors
{
    public class InspectedParameter
    {
        #region Properties

        public string FullDescription { get; private set; }

        public string Description { get; private set; }
        public string Type { get; set; }

        public List<InspectedParameter> Children { get; private set; }

        #endregion

        #region Instance Creation

        private InspectedParameter(string name, string description, string type)
        {
            Name = name;
            Description = @description.Replace(@"\\", @"\");
            Type = type;
            Children = new List<InspectedParameter>();
            GenerateFullDescription();
        }

        private void GenerateFullDescription()
        {
            var label = string.IsNullOrEmpty(Name) ? string.Empty : Name + ": ";
            FullDescription = string.Format("{0}{1} ({2})", label, Description, Type);
        }

        public string Name { get; set; }

        #endregion

        public override string ToString()
        {
            return string.Format("{0} ({1} children)", Description, Children.Count);
        }

        public static InspectedParameter From(NktParam aParameter, string functionName)
        {
            var description = ParameterDescriptor.Describe(aParameter, functionName);
            return From(description, aParameter.Name, aParameter.Type());
        }

        private static InspectedParameter From(string aDescription, string aParameterName, string aType)
        {
            return new InspectedParameter(aParameterName, aDescription, aType);
        }

        public static InspectedParameter Empty()
        {
            return From(string.Empty, string.Empty, string.Empty);
        }

        #region Equality Members

        private bool Equals(InspectedParameter other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return Equals(other.Name, Name) && Children.SequenceEqual(other.Children) && Equals(other.Type, Type) && Equals(other.Description, Description);
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != typeof(InspectedParameter)) return false;
            return Equals((InspectedParameter)obj);
        }

        public override int GetHashCode()
        {
            unchecked
            {
                int result = (Name != null ? Name.GetHashCode() : 0);
                result = (result * 397) ^ (Children != null ? Children.GetHashCode() : 0);
                result = (result * 397) ^ (Type != null ? Type.GetHashCode() : 0);
                result = (result * 397) ^ (Description != null ? Description.GetHashCode() : 0);
                return result;
            }
        }

        public static bool operator==(InspectedParameter left, InspectedParameter right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(InspectedParameter left, InspectedParameter right)
        {
            return !(left == right);
        }

        #endregion
    }
}