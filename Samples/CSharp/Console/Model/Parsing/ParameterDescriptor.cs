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

using System.Linq;
using Nektra.Deviare2;

namespace CSharpConsole.Model.Parsing
{
    public abstract class ParameterDescriptor
    {
        #region Description Generation

        public static string Describe(NktParam aParameter, string functionName)
        {
            return For(aParameter, functionName).GenerateDescription(aParameter, functionName);
        }

        private static ParameterDescriptor For(NktParam aParameter, string functionName)
        {
            var generators = new ParameterDescriptor[]
                                 {
                                     new NullPointerDescriptor(),
                                     new MessageDescriptor(),
                                     new ResourceStringDescriptor(),
                                     new GuidDescriptor(),
                                     new StringDescriptor(),
                                     new StructureDescriptor(),
                                     new PointerDescriptor(),
                                     new BasicTypeDescriptor()
                                 };

            return generators.First(generator => generator.CanHandle(aParameter, functionName));
        }

        #endregion
        
        public abstract bool CanHandle(NktParam aParameter, string functionName);

        public abstract string GenerateDescription(NktParam aParameter, string functionName);
    }
}
