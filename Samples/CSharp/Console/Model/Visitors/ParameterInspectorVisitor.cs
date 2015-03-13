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
using Nektra.Deviare2;

namespace CSharpConsole.Model.Visitors
{
    public class ParameterInspectorVisitor : INktParameterVisitor
    {
        #region Fields

        private readonly string _functionName;

        #endregion

        #region Instance Creation & Initialization

        public ParameterInspectorVisitor(string functionName)
        {
            _functionName = functionName;
            InspectedParameters = new List<InspectedParameter>();
            Depth = int.MaxValue;
        }

        #endregion
       
        #region Properties

        public List<InspectedParameter> InspectedParameters { get; private set; }

        public int Depth { get; set; }

        #endregion

        public void Inspect(IEnumerable<NktParam> parameters)
        {
            parameters.Where(param => _allowedParameterClasses.Contains(param.Class)).ForEach(param => Inspect(param));
        }

        private bool Inspect(NktParam parameter)
        {
            if (IsMaxAllowedDepthReached) return false;
            
            parameter.Accept(this);

            return true;
        }

        #region Inspection Depth Managing

        private int _currentDepthLevel;
        private readonly eNktDboClass[] _allowedParameterClasses = {
                                                                     eNktDboClass.clsFundamental, 
                                                                     eNktDboClass.clsStruct, 
                                                                     eNktDboClass.clsUnion, 
                                                                     eNktDboClass.clsArray,
                                                                     eNktDboClass.clsPointer, 
                                                                     eNktDboClass.clsReference
                                                                 };

        private bool IsMaxAllowedDepthReached
        {
            get { return _currentDepthLevel == Depth; }
        }

        private void DecreaseInspectingDepth()
        {
            _currentDepthLevel--;
        }

        private void IncreaseInspectingDepth()
        {
            _currentDepthLevel++;
        }

        #endregion
        
        #region Implementation of INktParameterVisitor

        public void VisitResourceStringParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }

        public void VisitAnsiCharParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }

        public void VisitWideCharParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }

        public void VisitStringParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }


        public void VisitDoubleParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }

        public void VisitFloatParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }


        public void VisitSignedByteParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }

        public void VisitUnsignedByteParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }


        public void VisitSignedWordParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }

        public void VisitUnsignedWordParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }


        public void VisitSignedDoubleWordParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }

        public void VisitUnsignedDoubleWordParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }


        public void VisitSignedQuadWordParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }

        public void VisitUnsignedQuadWordParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }


        public void VisitNullPointerParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }

        public void VisitNonNullPointerParameter(NktParam aParameter)
        {
            var pointee = aParameter.Evaluate();
            
            Inspect(pointee);

            // Parameter Name and Type information is provided by the pointer parameter, and not by the pointee
            UpdateLastAddedParameterInfo(aParameter.Name, aParameter.Type());
        }

        private void UpdateLastAddedParameterInfo(string newName, string newType)
        {
            InspectedParameters.Last().Name = newName;
            InspectedParameters.Last().Type = newType;
        }


        public void VisitStructureParameter(NktParam aParameter)
        {
            var inspectedStructure = RegisterInspectorFor(aParameter);
            
            foreach (var field in aParameter.Fields().CollectAllInspectables())
            {
                IncreaseInspectingDepth();

                if (Inspect(field))
                    RemoveLastCreatedParameterAndAddItAsChildOf(inspectedStructure);

                DecreaseInspectingDepth();
            }
        }

        public void VisitGuidStringParameter(NktParam aParameter)
        {
            RegisterInspectorFor(aParameter);
        }

        private void RemoveLastCreatedParameterAndAddItAsChildOf(InspectedParameter anInspectedParameter)
        {
            var lastAdded = InspectedParameters.Last();
            InspectedParameters.Remove(lastAdded);
            anInspectedParameter.Children.Add(lastAdded);
        }

        #endregion

        #region Inspectors Creation & Registration

        private InspectedParameter RegisterInspectorFor(NktParam aParameter)
        {
            var inspectedParameter = InspectedParameter.From(aParameter, _functionName);

            InspectedParameters.Add(inspectedParameter);

            return inspectedParameter;
        }

        #endregion
    }
}