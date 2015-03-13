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
using System.ComponentModel;
using System.Linq;
using System.Windows.Forms;
using CSharpConsole.Controls.Updating;
using CSharpConsole.Extensions;

namespace CSharpConsole.Views
{
    internal class UIUpdater
    {
        #region Fields

        private readonly Timer _updateUIWorker = new Timer();
        private readonly List<UpdateUIJob> _jobs = new List<UpdateUIJob>();
        private readonly object _jobsLock = new object();

        #endregion

        #region Instance Creation

        public UIUpdater()
        {
            _updateUIWorker.Interval = 100;
            _updateUIWorker.Tick += UpdateUI;
            _updateUIWorker.Start();
        }

        #endregion

        void UpdateUI(object sender, EventArgs e)
        {
            var jobs = JobsToBePerformed();

            var jobsByTargetControl = jobs.GroupBy(job => job.TargetControl);
            
            jobsByTargetControl.ForEach(PerformJobs);

            RefreshJobsExecutionOrderForNextUpdate();
        }

        private void PerformJobs(IGrouping<ISynchronizeInvoke, UpdateUIJob> jobGroup)
        {
            var firstJob = jobGroup.First();

            jobGroup.Where(job => job != firstJob).ForEach(firstJob.MergeWith);

            firstJob.Perform();
        }

        private IEnumerable<UpdateUIJob> JobsToBePerformed()
        {
            lock (_jobsLock)
            {
                var jobsToPerformNow = _jobs.Where(j => j.ExecutionOrder == 1).Take(200).ToArray();

                if (jobsToPerformNow.Any())
                {
                    jobsToPerformNow.ForEach(j => _jobs.Remove(j));
                    return jobsToPerformNow;
                }
                
            }

            return new UpdateUIJob[0];
        }

        private void RefreshJobsExecutionOrderForNextUpdate()
        {
            _jobs.Where(j => j.ExecutionOrder > 1).ForEach(j => j.ExecutionOrder--);
        }

        public void Add(UpdateUIJob aJob)
        {
            lock (_jobsLock)
            {
                _jobs.Add(aJob);
            }
        }

        public void RemoveAllPendingJobs()
        {
            lock(_jobsLock)
            {
                _jobs.Clear();
            }

            _updateUIWorker.Stop();
        }
    }
}