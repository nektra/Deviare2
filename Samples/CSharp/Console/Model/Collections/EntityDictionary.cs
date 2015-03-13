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

namespace CSharpConsole.Model.Collections
{
    public class EntityDictionary<TEntity, TKey, TValue> : Dictionary<TKey, TValue>
    {
        #region Fields

        private readonly Func<TEntity, TKey> _generateKeyFor;
        private readonly Func<TEntity, TValue> _initializeValueFrom;

        #endregion

        #region Instance Creation

        public EntityDictionary(Func<TEntity, TKey> generateKeyFor, Func<TEntity, TValue> initializeValueFrom)
        {
            _generateKeyFor = generateKeyFor;
            _initializeValueFrom = initializeValueFrom;
        }

        #endregion
        
        /// <summary>
        /// Retrieves the value associated with the key for <paramref name="anElement"/>. If missing, adds a new dictionary entry generating a key for <paramref name="anElement"/> and initializing its value.
        /// </summary>
        /// <param name="anElement"></param>
        /// <returns></returns>
        public TValue ValueOrInitialize(TEntity anElement)
        {
            var aKey = _generateKeyFor(anElement);

            if (!Contains(anElement))
                Initialize(anElement);

            return this[aKey];
        }

        private void Initialize(TEntity anElement)
        {
            Add(_generateKeyFor(anElement), _initializeValueFrom(anElement));
        }

        public bool Contains(TEntity anElement)
        {
            return ContainsKey(_generateKeyFor(anElement));
        }

        public void Remove(TEntity anElement)
        {
            if (!Contains(anElement)) return;
            Remove(_generateKeyFor(anElement));
        }

        public void Update(TEntity anElement, Func<TValue, TValue> updateValue)
        {
            var currentValue = ValueOrInitialize(anElement);
            var newValue = updateValue(currentValue);

            this[_generateKeyFor(anElement)] = newValue;
        }

        public TValue Value(TEntity anElement)
        {
            return this[_generateKeyFor(anElement)];
        }
    }
}
