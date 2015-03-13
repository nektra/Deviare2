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

using Nektra.Deviare2;

namespace CSharpConsole.Model.Visitors
{
    public interface INktParameterVisitor
    {
        void VisitResourceStringParameter(NktParam aParameter);
        void VisitAnsiCharParameter(NktParam aParameter);
        void VisitWideCharParameter(NktParam aParameter);
        void VisitStringParameter(NktParam aParameter);
        
        void VisitDoubleParameter(NktParam aParameter);
        void VisitFloatParameter(NktParam aParameter);
        
        void VisitSignedByteParameter(NktParam aParameter);
        void VisitUnsignedByteParameter(NktParam aParameter);
        
        void VisitSignedWordParameter(NktParam aParameter);
        void VisitUnsignedWordParameter(NktParam aParameter);
        
        void VisitSignedDoubleWordParameter(NktParam aParameter);
        void VisitUnsignedDoubleWordParameter(NktParam aParameter);
        
        void VisitSignedQuadWordParameter(NktParam aParameter);
        void VisitUnsignedQuadWordParameter(NktParam aParameter);
        
        void VisitNullPointerParameter(NktParam aParameter);
        void VisitNonNullPointerParameter(NktParam aParameter);

        void VisitStructureParameter(NktParam aParameter);
        void VisitGuidStringParameter(NktParam aParameter);
    }
}