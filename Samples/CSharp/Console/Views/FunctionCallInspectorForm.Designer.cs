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

﻿namespace CSharpConsole.Views
{
    partial class FunctionCallInspectorForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.treeViewAdv1 = new Aga.Controls.Tree.TreeViewAdv();
            this.tcType = new Aga.Controls.Tree.TreeColumn();
            this.tcName = new Aga.Controls.Tree.TreeColumn();
            this.tcValue = new Aga.Controls.Tree.TreeColumn();
            this.ncType = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.ncName = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.ncDescription = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.SuspendLayout();
            // 
            // treeViewAdv1
            // 
            this.treeViewAdv1.AutoRowHeight = true;
            this.treeViewAdv1.BackColor = System.Drawing.SystemColors.Window;
            this.treeViewAdv1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.treeViewAdv1.Columns.Add(this.tcType);
            this.treeViewAdv1.Columns.Add(this.tcName);
            this.treeViewAdv1.Columns.Add(this.tcValue);
            this.treeViewAdv1.DefaultToolTipProvider = null;
            this.treeViewAdv1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeViewAdv1.DragDropMarkColor = System.Drawing.Color.Black;
            this.treeViewAdv1.GridLineStyle = Aga.Controls.Tree.GridLineStyle.Horizontal;
            this.treeViewAdv1.LineColor = System.Drawing.SystemColors.ControlDark;
            this.treeViewAdv1.Location = new System.Drawing.Point(0, 0);
            this.treeViewAdv1.Model = null;
            this.treeViewAdv1.Name = "treeViewAdv1";
            this.treeViewAdv1.NodeControls.Add(this.ncType);
            this.treeViewAdv1.NodeControls.Add(this.ncName);
            this.treeViewAdv1.NodeControls.Add(this.ncDescription);
            this.treeViewAdv1.SelectedNode = null;
            this.treeViewAdv1.Size = new System.Drawing.Size(624, 442);
            this.treeViewAdv1.TabIndex = 0;
            this.treeViewAdv1.Text = "treeViewAdv1";
            this.treeViewAdv1.UseColumns = true;
            this.treeViewAdv1.Expanded += new System.EventHandler<Aga.Controls.Tree.TreeViewAdvEventArgs>(this.InspectedParameterExpandedHandler);
            // 
            // tcType
            // 
            this.tcType.Header = "Type";
            this.tcType.MinColumnWidth = 80;
            this.tcType.SortOrder = System.Windows.Forms.SortOrder.None;
            this.tcType.TooltipText = null;
            this.tcType.Width = 80;
            // 
            // tcName
            // 
            this.tcName.Header = "Name";
            this.tcName.MinColumnWidth = 80;
            this.tcName.SortOrder = System.Windows.Forms.SortOrder.None;
            this.tcName.TooltipText = null;
            this.tcName.Width = 80;
            // 
            // tcValue
            // 
            this.tcValue.Header = "Value";
            this.tcValue.MinColumnWidth = 80;
            this.tcValue.SortOrder = System.Windows.Forms.SortOrder.None;
            this.tcValue.TooltipText = null;
            this.tcValue.Width = 80;
            // 
            // ncType
            // 
            this.ncType.DataPropertyName = "Type";
            this.ncType.IncrementalSearchEnabled = true;
            this.ncType.LeftMargin = 3;
            this.ncType.ParentColumn = this.tcType;
            // 
            // ncName
            // 
            this.ncName.DataPropertyName = "Name";
            this.ncName.IncrementalSearchEnabled = true;
            this.ncName.LeftMargin = 3;
            this.ncName.ParentColumn = this.tcName;
            // 
            // ncDescription
            // 
            this.ncDescription.DataPropertyName = "Description";
            this.ncDescription.IncrementalSearchEnabled = true;
            this.ncDescription.LeftMargin = 3;
            this.ncDescription.ParentColumn = this.tcValue;
            // 
            // FunctionCallInspectorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(624, 442);
            this.Controls.Add(this.treeViewAdv1);
            this.Name = "FunctionCallInspectorForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Function Call Inspector";
            this.ResumeLayout(false);

        }

        #endregion

        private Aga.Controls.Tree.TreeViewAdv treeViewAdv1;
        private Aga.Controls.Tree.TreeColumn tcName;
        private Aga.Controls.Tree.NodeControls.NodeTextBox ncName;
        private Aga.Controls.Tree.TreeColumn tcType;
        private Aga.Controls.Tree.TreeColumn tcValue;
        private Aga.Controls.Tree.NodeControls.NodeTextBox ncType;
        private Aga.Controls.Tree.NodeControls.NodeTextBox ncDescription;
    }
}