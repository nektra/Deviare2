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
using Nektra.Deviare2;

namespace CSharpConsole.Model.Adapters
{
    public class NktDbObjectToExportedFunctionAdapter : NktExportedFunction
    {
        private readonly NktDbObject _aDbFunction;

        private NktDbObjectToExportedFunctionAdapter(NktDbObject aDbFunction)
        {
            _aDbFunction = aDbFunction;
        }

        public static NktExportedFunction[] From(NktDbObjectsEnum dbFunctions)
        {
            var adapters = new NktExportedFunction[dbFunctions.Count];

            for (var functionIndex = 0; functionIndex < dbFunctions.Count; functionIndex++)
            {
                var aDbFunction = dbFunctions.GetAt(functionIndex);
                adapters[functionIndex] = new NktDbObjectToExportedFunctionAdapter(aDbFunction);
            }

            return adapters;
        }

        #region Implementation of INktObject

        object INktObject.GetObjectFromDeviareId(IntPtr devId)
        {
            throw new NotImplementedException();
        }

        IntPtr INktExportedFunction.UserData
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        public NktDbObject DbObject()
        {
            throw new NotImplementedException();
        }

        object INktExportedFunction.GetObjectFromDeviareId(IntPtr devId)
        {
            throw new NotImplementedException();
        }

        IntPtr INktObject.UserData
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        int INktObject.InterfaceError
        {
            get { throw new NotImplementedException(); }
        }

        IntPtr INktExportedFunction.DeviareId
        {
            get { throw new NotImplementedException(); }
        }

        public int Ordinal
        {
            get { throw new NotImplementedException(); }
        }

        public string Name
        {
            get { return _aDbFunction.Name; }
        }

        public string ModuleName
        {
            get { throw new NotImplementedException(); }
        }

        public string CompleteName
        {
            get { throw new NotImplementedException(); }
        }

        public bool IsForwarded
        {
            get { return false; }
        }

        public string ForwardedApiName
        {
            get { throw new NotImplementedException(); }
        }

        public string ForwardedModuleName
        {
            get { throw new NotImplementedException(); }
        }

        public string ForwardedCompleteName
        {
            get { throw new NotImplementedException(); }
        }

        public IntPtr AddrBase
        {
            get { return IntPtr.Zero; }
        }

        public IntPtr AddrOffset
        {
            get { throw new NotImplementedException(); }
        }

        public IntPtr Addr
        {
            get { return IntPtr.Zero; }
        }

        public eNktDboCallingConvention CallingConv
        {
            get { throw new NotImplementedException(); }
        }

        public bool HasDeclaration
        {
            get { return true; }
        }

        public string Declaration
        {
            get { return _aDbFunction.Declaration; }
        }

        int INktExportedFunction.InterfaceError
        {
            get { throw new NotImplementedException(); }
        }

        IntPtr INktObject.DeviareId
        {
            get { throw new NotImplementedException(); }
        }

        #endregion
    }
}