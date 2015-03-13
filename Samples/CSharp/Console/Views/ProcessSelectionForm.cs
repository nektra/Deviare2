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
using System.Collections.Generic;
using System.Windows.Forms;
using CSharpConsole.Model.Process;

namespace CSharpConsole.Views
{
    public partial class ProcessSelectionForm : Form
    {
        #region Properties

        private List<IRunningProcess> SelectedProcesses { get; set; }

        #endregion

        #region Instance Creation

        public ProcessSelectionForm()
        {
            InitializeComponent();
            SelectedProcesses = new List<IRunningProcess>();
        }

        private ProcessSelectionForm(IEnumerable<IProcess> toHookOnStart, IEnumerable<IRunningProcess> unhooked, IEnumerable<IRunningProcess> hookedInactive, IEnumerable<IRunningProcess> hookedActive)
            : this()
        {
            processesTreeView1.InitializeComponent();
            processesTreeView1.Display(toHookOnStart, unhooked, hookedInactive, hookedActive);
            processesTreeView1.ExpandAll();
        }

        public static IRunningProcess[] Show(IEnumerable<IProcess> toHookOnStart, IEnumerable<IRunningProcess> unhooked, IEnumerable<IRunningProcess> hookedInactive, IEnumerable<IRunningProcess> hookedActive)
        {
            var aSelectionForm = new ProcessSelectionForm(toHookOnStart, unhooked, hookedInactive, hookedActive);
            aSelectionForm.ShowDialog();
            return aSelectionForm.SelectedProcesses.ToArray();
        }

        #endregion

        #region Event Handlers

        private void btnAccept_Click(object sender, EventArgs e)
        {
            foreach (var process in processesTreeView1.SelectedProcesses())
            {
                process.IfRunning(p => SelectedProcesses.Add(p));
            }
            
            DialogResult = DialogResult.OK;
        }

        #endregion
    }
}
