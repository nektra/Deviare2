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
using CSharpConsole.Extensions;
using CSharpConsole.Model.Modules;
using CSharpConsole.Model.Process;
using CSharpConsole.Views;

namespace CSharpConsole.Presenters
{
    public interface IProcessModulesDisplayer
    {
        void DisplayProcessesModules(IEnumerable<IProcess> selectedProcesses);
        void DisplayProcessesModulesKeepingSelection(IEnumerable<IProcess> selectedProcesses, Module selectedModule);
    }

    public class ProcessModulesDisplayer : IProcessModulesDisplayer
    {
        #region Fields

        private readonly List<BackgroundWorker> _workers;
        private readonly Dictionary<BackgroundWorker, IEnumerable<Module>> _newModulesToShowByWorker;
        private readonly Dictionary<BackgroundWorker, IEnumerable<Module>> _modulesShownByWorker;
        private readonly Dictionary<BackgroundWorker, Module> _moduleToKeepSelected;
        private readonly IApplicationView _view;
        private BackgroundWorker _actualWorker;
        private ApplicationPresenter _presenter;
        private int _jobOrder;

        #endregion

        #region Instance Creation

        internal ProcessModulesDisplayer(ApplicationPresenter presenter, IApplicationView view)
        {
            _presenter = presenter;
            _view = view;

            _modulesShownByWorker = new Dictionary<BackgroundWorker, IEnumerable<Module>>();
            _newModulesToShowByWorker = new Dictionary<BackgroundWorker, IEnumerable<Module>>();
            _moduleToKeepSelected = new Dictionary<BackgroundWorker, Module>();
            _workers = new List<BackgroundWorker>();

        } 

        #endregion

        #region Worker Handlers

        private void RetrieveModulesForDisplay(object sender, DoWorkEventArgs e)
        {
            var processes = (IEnumerable<IProcess>)e.Argument;
            var worker = ((BackgroundWorker)sender);

            var processesCount = processes.Count();
            var processedProcesses = 0;

            processes.ForEach(p =>

                                  {
                                      if (worker.CancellationPending)
                                      {
                                          _view.CancelModulesViewUpdateFor(worker);
                                          e.Cancel = true;
                                          return;
                                      }
                                      _newModulesToShowByWorker[worker] = (Module[])(p.Modules().Where(m => !_modulesShownByWorker[worker].Contains(m)).ToArray().Clone());
                                      if (worker.CancellationPending)
                                      {
                                          _view.CancelModulesViewUpdateFor(worker);
                                          e.Cancel = true;
                                          return;
                                      }
                                      _modulesShownByWorker[worker] = _modulesShownByWorker[worker].Union(_newModulesToShowByWorker[worker]).ToArray();
                                      if (worker.CancellationPending)
                                      {
                                          _view.CancelModulesViewUpdateFor(worker);
                                          e.Cancel = true;
                                          return;
                                      }
                                      worker.ReportProgress(CalculateProgressPercentage(++processedProcesses, processesCount));
                                      if (worker.CancellationPending)
                                      {
                                          _view.CancelModulesViewUpdateFor(worker);
                                          e.Cancel = true;
                                          return;
                                      }
                                  });
        }

        private void UpdateGUI(object sender, ProgressChangedEventArgs progressChangedEventArgs)
        {
            var worker = (BackgroundWorker) sender;
            var modules = _newModulesToShowByWorker[worker].ToList();
            const int maxModulesProcessedPerIteration = 5;

            var first = 0;
            var count = Math.Min(maxModulesProcessedPerIteration, modules.Count);

            if (worker.CancellationPending)
            {
                _view.CancelModulesViewUpdateFor(worker);
                return;
            }

            while (count > 0)
            {
                if (worker.Equals(_actualWorker))
                {
                    if (worker.CancellationPending)
                    {
                        _view.CancelModulesViewUpdateFor(worker);
                        return;
                    }

                    _view.DisplayModulesInOrder(modules.GetRange(first, count), worker, _jobOrder);
                    first = first + count;
                    count = Math.Min(maxModulesProcessedPerIteration, (modules.Count) - first);
                    _jobOrder++;
                }
            }
        }

        private void RemoveNotLongerUsedModules(object sender, RunWorkerCompletedEventArgs e)
        {
            _view.SortModulesViewByColumn(0);

            var worker = (BackgroundWorker) sender;

            _view.MarkAsSelectedIfSelectionIsNull(_moduleToKeepSelected[worker], worker, _jobOrder);

            _view.SetHookingUnhookingAndUnhookingAllFlags(false, false, true, worker, _jobOrder);
            _moduleToKeepSelected.Remove(worker);
            _modulesShownByWorker.Remove(worker);
            _newModulesToShowByWorker.Remove(worker);
            //_workers.Remove(worker);

            _view.RemoveUpdateCancellationStatusByWorker(worker);

        }

        #endregion

        public void DisplayProcessesModules(IEnumerable<IProcess> selectedProcesses)
        {
            DisplayProcessesModulesKeepingSelection(selectedProcesses, null);
        }

        public void DisplayProcessesModulesKeepingSelection(IEnumerable<IProcess> selectedProcesses, Module selectedModule)
        {
            _workers.ForEach(w =>
                                 {
                                     _view.CancelModulesViewUpdateFor(w);
                                     w.CancelAsync();
                                 });
            _workers.Clear();

            _view.AddHookEnabled = false;
            _view.RemoveHookEnabled = false;

            if (!selectedProcesses.Any(p => p.IsToHookOnStart()))
                _view.RemoveAllHooksEnabled = false;

            var worker = NewWorker(selectedModule);

            ClearModulesView();
            worker.RunWorkerAsync(selectedProcesses.ToArray());
            _jobOrder = 1;
        }

        private BackgroundWorker NewWorker(Module aModuleToKeepSelected)
        {
            var worker = new BackgroundWorker
            {
                WorkerSupportsCancellation = true,
                WorkerReportsProgress = true
            };

            _workers.Add(worker);
            _actualWorker = worker;

            worker.DoWork += RetrieveModulesForDisplay;
            worker.ProgressChanged += UpdateGUI;
            worker.RunWorkerCompleted += RemoveNotLongerUsedModules;

            _modulesShownByWorker.Add(worker, Enumerable.Empty<Module>());
            _newModulesToShowByWorker.Add(worker, Enumerable.Empty<Module>());
            _moduleToKeepSelected.Add(worker, aModuleToKeepSelected);

            return worker;
        }

        private void ClearModulesView()
        {
            _view.DisplaySingleProcessModules(Enumerable.Empty<Module>());
        }

        private int CalculateProgressPercentage(int processedProcesses, int processesCount)
        {
            return (processedProcesses / processesCount) * 100;
        }
    }
}