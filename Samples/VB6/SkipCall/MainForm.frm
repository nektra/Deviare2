VERSION 5.00
Begin VB.Form frmMain 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Deviare / Test SkipCall"
   ClientHeight    =   7155
   ClientLeft      =   45
   ClientTop       =   375
   ClientWidth     =   8160
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   7155
   ScaleWidth      =   8160
   StartUpPosition =   2  'CenterScreen
   Begin VB.ListBox lstEvents 
      Height          =   6690
      Left            =   120
      TabIndex        =   0
      Top             =   180
      Width           =   7935
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'
' Copyright (C) 2010-2015 Nektra S.A., Buenos Aires, Argentina.
' All rights reserved. Contact: http://www.nektra.com
'
'
' This file is part of Deviare
'
'
' Commercial License Usage
' ------------------------
' Licensees holding valid commercial Deviare licenses may use this file
' in accordance with the commercial license agreement provided with the
' Software or, alternatively, in accordance with the terms contained in
' a written agreement between you and Nektra.  For licensing terms and
' conditions see http://www.nektra.com/licensing/. Use the contact form
' at http://www.nektra.com/contact/ for further information.
'
'
' GNU General Public License Usage
' --------------------------------
' Alternatively, this file may be used under the terms of the GNU General
' Public License version 3.0 as published by the Free Software Foundation
' and appearing in the file LICENSE.GPL included in the packaging of this
' file.  Please visit http://www.gnu.org/copyleft/gpl.html and review the
' information to ensure the GNU General Public License version 3.0
' requirements will be met.
'
'
Private Declare Function GetLastErrorCode Lib "DeviareCOM.dll" () As Long

Private WithEvents spymgr As Deviare2.NktSpyMgr
Attribute spymgr.VB_VarHelpID = -1

Private Sub AddEventItem(ByVal S As String)
    If lstEvents.ListCount >= 3000 Then
        lstEvents.RemoveItem 0
    End If
    lstEvents.AddItem S
    lstEvents.ListIndex = lstEvents.ListCount - 1
End Sub

Private Sub Form_Load()
Dim notepadProc As Deviare2.NktProcess
Dim notepadHook As Deviare2.NktHook

    Set spymgr = New Deviare2.NktSpyMgr
    spymgr.DatabasePath = ""
    spymgr.AgentPath = ""
    spymgr.Initialize
    
    Set notepadProc = spymgr.CreateProcess("NOTEPAD.EXE", False, 0&)
    Call AddEventItem("Execute notepad.exe: " + Format$(notepadProc.Id))
    
    Set notepadHook = spymgr.CreateHook("kernel32.dll!CreateFileW", flgAutoHookChildProcess Or flgAutoHookActive)
    Call notepadHook.Attach(notepadProc)
    Call notepadHook.Hook(True)
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Set spymgr = Nothing
End Sub

Private Sub spymgr_OnFunctionCalled(ByVal Hook As Deviare2.INktHook, ByVal proc As Deviare2.INktProcess, ByVal callInfo As Deviare2.INktHookCallInfo)
Dim paramEnum As Deviare2.INktParamsEnum
Dim param As Deviare2.INktParam
Dim S As String

    If callInfo.IsPreCall <> False Then
        Call AddEventItem("Hooked Function Called")
        
        Set paramEnum = callInfo.Params()
        Set param = paramEnum.GetAt(0)
        S = param.ReadString
        If InStr(LCase$(S), ".txt") > 0 Then
            Call AddEventItem("Skipping function call")
            Call callInfo.SkipCall
            callInfo.Result().LongVal = -1
            callInfo.LastError = 5
        End If
    End If
End Sub

Private Sub spymgr_OnHookStateChanged(ByVal Hook As Deviare2.INktHook, ByVal proc As Deviare2.INktProcess, ByVal newState As Deviare2.eNktHookState, ByVal oldState As Deviare2.eNktHookState)
Dim S As String

    S = proc.ToString
    Call AddEventItem("Hook State Changed: " + S + "[" + Format$(newState) + "]")
End Sub
