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

﻿using System;
using CSharpConsole.Model.Visitors;
using Nektra.Deviare2;

namespace CSharpConsole.Extensions
{
    public static class NktParamExtensions
    {
        public static string Type(this NktParam aParameter)
        {
            return string.IsNullOrEmpty(aParameter.Name) ? aParameter.Declaration : aParameter.Declaration.Replace(aParameter.Name, string.Empty);
        }

        public static bool IsPointerToVoid(this NktParam aParameter)
        {
            return aParameter.IsPointer && aParameter.Evaluate().BasicType == eNktDboFundamentalType.ftVoid;
        }

        public static void Accept(this NktParam aParameter, INktParameterVisitor aVisitor)
        {
            if (aParameter.IntResourceString > 0) aVisitor.VisitResourceStringParameter(aParameter);
            else if (!string.IsNullOrEmpty(aParameter.GuidString)) aVisitor.VisitGuidStringParameter(aParameter);
            else if (aParameter.IsAnsiString || aParameter.IsWideString) aVisitor.VisitStringParameter(aParameter);
            else if (aParameter.BasicType == eNktDboFundamentalType.ftAnsiChar) aVisitor.VisitAnsiCharParameter(aParameter);
            else if (aParameter.BasicType == eNktDboFundamentalType.ftWideChar) aVisitor.VisitWideCharParameter(aParameter);

            else if (aParameter.BasicType == eNktDboFundamentalType.ftDouble) aVisitor.VisitDoubleParameter(aParameter);
            else if (aParameter.BasicType == eNktDboFundamentalType.ftFloat) aVisitor.VisitFloatParameter(aParameter);

            else if (aParameter.BasicType == eNktDboFundamentalType.ftSignedByte) aVisitor.VisitSignedByteParameter(aParameter);
            else if (aParameter.BasicType == eNktDboFundamentalType.ftUnsignedByte) aVisitor.VisitUnsignedByteParameter(aParameter);

            else if (aParameter.BasicType == eNktDboFundamentalType.ftSignedWord) aVisitor.VisitSignedWordParameter(aParameter);
            else if (aParameter.BasicType == eNktDboFundamentalType.ftUnsignedWord) aVisitor.VisitUnsignedWordParameter(aParameter);

            else if (aParameter.BasicType == eNktDboFundamentalType.ftSignedDoubleWord) aVisitor.VisitSignedDoubleWordParameter(aParameter);
            else if (aParameter.BasicType == eNktDboFundamentalType.ftUnsignedDoubleWord) aVisitor.VisitUnsignedDoubleWordParameter(aParameter);

            else if (aParameter.BasicType == eNktDboFundamentalType.ftSignedQuadWord) aVisitor.VisitSignedQuadWordParameter(aParameter);
            else if (aParameter.BasicType == eNktDboFundamentalType.ftUnsignedQuadWord) aVisitor.VisitUnsignedQuadWordParameter(aParameter);

            //else if ((aParameter.BasicType) == eNktDboFundamentalType.ftNone) aVisitor.VisitFtNone(aParameter);

            else if (aParameter.FieldsCount > 0) aVisitor.VisitStructureParameter(aParameter);

            else if (aParameter.PointerVal == IntPtr.Zero)
            {
                aVisitor.VisitNullPointerParameter(aParameter);
            }
            else if (aParameter.IsPointerToVoid())
            {
                if (aParameter.LongLongVal > 0xFFFFFFFFL)
                {
                    aVisitor.VisitUnsignedQuadWordParameter(aParameter);
                    return;
                }
                aVisitor.VisitUnsignedDoubleWordParameter(aParameter);
            }
            else if (aParameter.PointerVal != IntPtr.Zero)
            {
                aVisitor.VisitNonNullPointerParameter(aParameter);
            }
            else
            {
                throw new InvalidOperationException("Unrecognized parameter type");
            }
        }
    }
}