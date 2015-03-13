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
using Nektra.Deviare2;

namespace CSharpConsole.Model.Parsing
{
    public class BasicTypeDescriptor : ParameterDescriptor
    {
        #region Instance Creation

        private readonly Dictionary<eNktDboFundamentalType, Func<NktParam, string>> _basicTypesHandlers;

        public BasicTypeDescriptor()
        {
            _basicTypesHandlers = new Dictionary<eNktDboFundamentalType, Func<NktParam, string>>
                                      {
                                          {eNktDboFundamentalType.ftAnsiChar, param => param.ByteVal.ToString()},
                                          {eNktDboFundamentalType.ftWideChar, param => param.ShortVal.ToString()},
                                          {eNktDboFundamentalType.ftDouble, param => param.DoubleVal.ToString()},
                                          {eNktDboFundamentalType.ftFloat, param => param.FloatVal.ToString()},

                                          {eNktDboFundamentalType.ftSignedByte, ByteDescription},
                                          {eNktDboFundamentalType.ftUnsignedByte, ByteDescription},

                                          {eNktDboFundamentalType.ftSignedWord, WordDescription},
                                          {eNktDboFundamentalType.ftUnsignedWord, WordDescription},

                                          {eNktDboFundamentalType.ftSignedDoubleWord, DoubleWordDescription},
                                          {eNktDboFundamentalType.ftUnsignedDoubleWord, DoubleWordDescription},

                                          {eNktDboFundamentalType.ftSignedQuadWord, QuadWordDescription},
                                          {eNktDboFundamentalType.ftUnsignedQuadWord, QuadWordDescription},
                                          {eNktDboFundamentalType.ftVoid, param => "VOID"}
                                      };
        }

        #endregion

        #region Parameter Formatting

        protected static string FormatAsHex(object aValue)
        {
            return string.Format("0x{0:X}", aValue);
        }

        private string ByteDescription(NktParam aParameter)
        {
            return FormatAsHex(aParameter.ByteVal);
        }

        private string WordDescription(NktParam aParameter)
        {
            return FormatAsHex(aParameter.ShortVal);
        }

        private string DoubleWordDescription(NktParam aParameter)
        {
            return FormatAsHex(aParameter.LongVal);
        }

        private string QuadWordDescription(NktParam aParameter)
        {
            return FormatAsHex(aParameter.LongLongVal);
        }

        #endregion

        public override bool CanHandle(NktParam aParameter, string functionName)
        {
            var handledBasicTypes = _basicTypesHandlers.Keys;
            return handledBasicTypes.Contains(aParameter.BasicType);
        }

        public override string GenerateDescription(NktParam aParameter, string functionName)
        {
            var generateDescriptionFor = _basicTypesHandlers[aParameter.BasicType];
            return generateDescriptionFor(aParameter);
        }
    }
}