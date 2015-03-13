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
Dim spyMgr, wshShell

WScript.Echo "Starting Sample VBScript App"
Set spyMgr = WScript.CreateObject("DeviareCOM.NktSpyMgr", "spyMgr_")
spyMgr.Initialize

If StartNotepadAndHook() = False Then
	WScript.Quit 0
End If

Set wshShell = WScript.CreateObject("WScript.Shell")
wshShell.Popup "Press the OK button to exit this demo"

WScript.Echo "Exiting Sample VBScript App"
WScript.Quit 0

'--------------------------------------------------------------------------
'Events

Sub spyMgr_OnProcessStarted(ByVal proc)
	WScript.Echo "OnProcessStarted: " + proc.Name
	'MsgBox "OnProcessStarted: " + proc.Name
End Sub

Sub spyMgr_OnProcessTerminated(ByVal proc)
	WScript.Echo "OnProcessTerminated: " + proc.Name
	'MsgBox "OnProcessTerminated: " + proc.Name
End Sub

Sub spyMgr_OnFunctionCalled(ByVal hook, ByVal proc, ByVal callInfo)
Dim S

	If callInfo.IsPreCall <> False Then S = "PreCall" Else S = "PostCall"
	WScript.Echo "OnFunctionCalled (" + S + "): " + proc.Name
End Sub

'--------------------------------------------------------------------------
'Helper functions

Function StartNotepadAndHook()
Dim wshShell, oCalc, proc, myHook

	WScript.Echo "Executing Notepad..."
	Set wshShell = WScript.CreateObject("WScript.Shell")
	Set oCalc = wshShell.Exec("notepad.exe")
	WScript.Sleep(1000) 'Little delay
	If (oCalc Is Nothing) <> False Or oCalc.ProcessID = 0 Then
		WScript.Echo "Error: Cannot execute notepad"
		StartNotepadAndHook = False
		Exit Function
	End If

	WScript.Echo "Finding process id #" + CStr(oCalc.ProcessID) + "..."
	Set proc = FindProcessId(oCalc.ProcessID)
	If (proc Is Nothing) <> False Then
		WScript.Echo "Error: Cannot find Notepad"
		StartNotepadAndHook = False
		Exit Function
	End If
	WScript.Echo "Got."      

	WScript.Echo "Hooking user32.dll!BeginPaint..."
    Set myHook = spyMgr.CreateHook("user32.dll!BeginPaint", 0)   
    myHook.Attach proc, True   
    myHook.Hook True
	WScript.Echo "Done"
	
	StartNotepadAndHook = True
End Function

Function FindProcessId(ByVal pid) 
Dim proc, enumProcs
    
    Set enumProcs = spyMgr.Processes
    Set proc = enumProcs.First
    Do While (proc Is Nothing) = False
    	If proc.Id = pid Then
            Set FindProcessId = proc
            Exit Function
        End If
        Set proc = enumProcs.Next
    Loop
    Set FindProcessId = Nothing
End Function
