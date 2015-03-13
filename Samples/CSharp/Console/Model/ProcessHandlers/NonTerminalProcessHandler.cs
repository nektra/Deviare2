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

namespace CSharpConsole.Model.ProcessHandlers
{
    public abstract class NonTerminalHandler<T> : IHandler<T>
    {
        private IHandler<T> _nextHandler;

        public IHandler<T> NextHandler
        {
            get { return _nextHandler ?? new NullHandler<T>(); }
            set
            {
                if (value == this)
                    throw new ArgumentException("NextHandler cannot be assigned to himself");

                _nextHandler = value;
            }
        }

        public void Handle(T anObject)
        {
            if (!HandleCore(anObject)) return;

            ProcessWithNextHandler(anObject);
        }

        public void Handle(IEnumerable<T> objects)
        {
            if (!HandleCore(objects)) return;

            ProcessWithNextHandler(objects);
        }

        protected virtual void ProcessWithNextHandler(T anObject)
        {
            NextHandler.Handle(anObject);
        }

        protected virtual void ProcessWithNextHandler(IEnumerable<T> objects)
        {
            NextHandler.Handle(objects);
        }

        protected abstract bool HandleCore(T aProcess);
        protected abstract bool HandleCore(IEnumerable<T> objects);

        public override string ToString()
        {
            return string.Format("{0} -> {1}", GetType().Name, NextHandler);
        }
    }
}