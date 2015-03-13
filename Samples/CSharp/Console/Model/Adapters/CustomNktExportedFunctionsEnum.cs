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
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Nektra.Deviare2;

namespace CSharpConsole.Model.Adapters
{
    public class CustomNktExportedFunctionsEnum : NktExportedFunctionsEnum
    {
        private readonly IEnumerable<NktExportedFunction> _functions;

        public CustomNktExportedFunctionsEnum(IEnumerable<NktExportedFunction> functions)
        {
            _functions = functions;
        }

        #region Implementation of INktObject

        object INktObject.GetObjectFromDeviareId(IntPtr devId)
        {
            throw new NotImplementedException();
        }

        IEnumerator INktExportedFunctionsEnum.GetEnumerator()
        {
            throw new NotImplementedException();
        }

        IntPtr INktExportedFunctionsEnum.UserData
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        public NktExportedFunction GetAt(int index)
        {
            return _functions.ElementAt(index);
        }

        public NktExportedFunction First()
        {
            return _functions.First();
        }

        public NktExportedFunction Last()
        {
            throw new NotImplementedException();
        }

        public NktExportedFunction Next()
        {
            throw new NotImplementedException();
        }

        public NktExportedFunction Previous()
        {
            throw new NotImplementedException();
        }

        public NktExportedFunction GetByDeviareId(IntPtr devId)
        {
            throw new NotImplementedException();
        }

        public NktExportedFunction GetByOrdinal(int Ordinal)
        {
            throw new NotImplementedException();
        }

        public NktExportedFunction GetByName(string funcName)
        {
            throw new NotImplementedException();
        }

        public NktExportedFunction GetByAddress(IntPtr Addr, bool searchNearest)
        {
            throw new NotImplementedException();
        }

        object INktExportedFunctionsEnum.GetObjectFromDeviareId(IntPtr devId)
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

        IntPtr INktExportedFunctionsEnum.DeviareId
        {
            get { throw new NotImplementedException(); }
        }

        public int Count
        {
            get { return _functions.Count(); }
        }

        int INktExportedFunctionsEnum.InterfaceError
        {
            get { throw new NotImplementedException(); }
        }

        IntPtr INktObject.DeviareId
        {
            get { throw new NotImplementedException(); }
        }

        #endregion

        IEnumerator IEnumerable.GetEnumerator()
        {
            throw new NotImplementedException();
        }
    }
}