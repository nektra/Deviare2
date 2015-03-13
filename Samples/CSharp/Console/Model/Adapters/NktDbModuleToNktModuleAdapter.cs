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
using Nektra.Deviare2;

namespace CSharpConsole.Model.Adapters
{
    public class NktDbModuleToNktModuleAdapter : NktModule
    {
        private readonly NktDbModule _anAdaptee;
        private readonly NktExportedFunctionsEnum _exportedFunctions;

        public NktDbModuleToNktModuleAdapter(NktDbModule anAdaptee)
        {
            _anAdaptee = anAdaptee;
            var functions = NktDbObjectToExportedFunctionAdapter.From(anAdaptee.DbFunctions());
            _exportedFunctions = new CustomNktExportedFunctionsEnum(functions);
        }

        #region Implementation of INktObject

        object INktObject.GetObjectFromDeviareId(IntPtr devId)
        {
            throw new NotImplementedException();
        }

        public NktStructPESections Sections()
        {
            throw new NotImplementedException();
        }

        IntPtr INktModule.UserData
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        public NktExportedFunctionsEnum ExportedFunctions()
        {
            return _exportedFunctions;
        }

        public NktExportedFunction FunctionByOrdinal(int Ordinal)
        {
            throw new NotImplementedException();
        }

        public NktExportedFunction FunctionByName(string Name)
        {
            throw new NotImplementedException();
        }

        public NktExportedFunction FunctionByAddress(IntPtr Addr, bool searchNearest)
        {
            throw new NotImplementedException();
        }

        public NktStructPEFileHeader FileHeader()
        {
            throw new NotImplementedException();
        }

        public NktStructPEOptionalHeader OptionalHeader()
        {
            throw new NotImplementedException();
        }

        object INktModule.GetObjectFromDeviareId(IntPtr devId)
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

        IntPtr INktModule.DeviareId
        {
            get { throw new NotImplementedException(); }
        }

        public int ProcessId
        {
            get { throw new NotImplementedException(); }
        }

        public int PlatformBits
        {
            get { throw new NotImplementedException(); }
        }

        public IntPtr BaseAddress
        {
            get { return IntPtr.Zero; }
        }

        public IntPtr Size
        {
            get { throw new NotImplementedException(); }
        }

        public string Name
        {
            get { return _anAdaptee.Name; }
        }

        public string Path
        {
            get { return @"<not available>\" + Name; }
        }

        public string FileName
        {
            get { throw new NotImplementedException(); }
        }

        public IntPtr Handle
        {
            get { throw new NotImplementedException(); }
        }

        int INktModule.InterfaceError
        {
            get { throw new NotImplementedException(); }
        }

        IntPtr INktObject.DeviareId
        {
            get { throw new NotImplementedException(); }
        }

        #endregion

        public static IEnumerable<NktModule> Adapt(NktDbModulesEnum dbModules)
        {
            var adapters = new List<NktModule>();

            for (var moduleIndex = 0; moduleIndex < dbModules.Count; moduleIndex++)
            {
                var aDbModule = dbModules.GetAt(moduleIndex);
                adapters.Add(new NktDbModuleToNktModuleAdapter(aDbModule));

            }

            return adapters;
        }
    }
}
