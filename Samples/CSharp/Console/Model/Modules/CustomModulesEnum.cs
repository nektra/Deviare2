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

namespace CSharpConsole.Model.Modules
{
    public class CustomModulesEnum : NktModulesEnum
    {
        private readonly IEnumerable<NktModule> _modules;

        public CustomModulesEnum(IEnumerable<NktModule> modules)
        {
            _modules = modules;
        }

        #region Implementation Of NktModulesEnum

        object INktObject.GetObjectFromDeviareId(IntPtr devId)
        {
            return null;
        }

        private IntPtr _userData = IntPtr.Zero;
        
        IntPtr INktModulesEnum.UserData
        {
            get { return _userData; }
            set { _userData = value; }
        }

        public NktModule GetAt(int index)
        {
            return _modules.ElementAt(index);
        }

        public NktModule First()
        {
            return _modules.First();
        }

        public NktModule Last()
        {
            throw new NotImplementedException();
        }

        public NktModule Next()
        {
            throw new NotImplementedException();
        }

        public NktModule Previous()
        {
            throw new NotImplementedException();
        }

        public NktModule GetByDeviareId(IntPtr devId)
        {
            throw new NotImplementedException();
        }

        public NktModule GetByName(string dllName)
        {
            throw new NotImplementedException();
        }

        public NktModule GetByAddress(IntPtr Addr, eNktSearchMode searchMode)
        {
            throw new NotImplementedException();
        }

        public NktExportedFunction FunctionByName(string dllFunc)
        {
            throw new NotImplementedException();
        }

        public NktExportedFunction FunctionByAddress(IntPtr Addr, eNktSearchMode searchMode)
        {
            throw new NotImplementedException();
        }

        object INktModulesEnum.GetObjectFromDeviareId(IntPtr devId)
        {
            return null;
        }

        IntPtr INktObject.UserData
        {
            get { return _userData; }
            set { _userData = value; }
        }

        int INktObject.InterfaceError
        {
            get { return 0; }
        }

        IntPtr INktModulesEnum.DeviareId
        {
            get { return IntPtr.Zero; }
        }

        public int Count
        {
            get { return _modules.Count(); }
        }

        int INktModulesEnum.InterfaceError
        {
            get { return 0; }
        }

        IntPtr INktObject.DeviareId
        {
            get { return IntPtr.Zero; }
        }

        #endregion


        #region INktModulesEnum Members


        public System.Collections.IEnumerator GetEnumerator()
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}