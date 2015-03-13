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

WScript.Echo "Starting SkipCall test"
Set spyMgr = WScript.CreateObject("DeviareCOM.NktSpyMgr", "spyMgr_")
spyMgr.Initialize

If StartNotepadAndHook() = False Then
	WScript.Quit 0
End If

Set wshShell = WScript.CreateObject("WScript.Shell")
wshShell.Popup "Press the OK button to exit this demo"

WScript.Echo "Exiting test"
WScript.Quit 0

'--------------------------------------------------------------------------
'Events

Sub spyMgr_OnHookStateChanged(ByVal hook, ByVal proc, ByVal newState, ByVal oldState)
	WScript.Echo "Hook State Changed: " + CStr(newState)
End Sub

Sub spyMgr_OnFunctionCalled(ByVal hook, ByVal proc, ByVal callInfo)
Dim paramEnum, param

	If callInfo.IsPreCall <> False Then
		WScript.Echo "PreCall " + CStr(callInfo.Cookie)
        Set paramEnum = callInfo.Params()
        Set param = paramEnum.GetAt(0)
        If InStr(LCase(param.ReadString), ".txt") > 0 Then
        	WScript.Echo "  Skipping function call"
            Call callInfo.SkipCall
            If proc.PlatformBits = 64 Then
            	callInfo.Result().LongLongVal = -1
            Else
            	callInfo.Result().LongVal = -1
            End If
            callInfo.LastError = 5
        End If
	Else
		WScript.Echo "PosteCall " + CStr(callInfo.Cookie)
	End If
End Sub

'--------------------------------------------------------------------------
'Helper functions

Function StartNotepadAndHook()
Dim wshShell, oNotepad, proc, myHook

	WScript.Echo "Executing Notepad..."
	Set wshShell = WScript.CreateObject("WScript.Shell")
	Set oNotepad = wshShell.Exec("notepad.exe")
	WScript.Sleep(1000) 'Little delay
	If (oNotepad Is Nothing) <> False Or oNotepad.ProcessID = 0 Then
		WScript.Echo "Error: Cannot execute notepad"
		StartNotepadAndHook = False
		Exit Function
	End If

	WScript.Echo "Finding process id #" + CStr(oNotepad.ProcessID) + "..."
	Set proc = FindProcessId(oNotepad.ProcessID)
	If (proc Is Nothing) <> False Then
		WScript.Echo "Error: Cannot find Notepad"
		StartNotepadAndHook = False
		Exit Function
	End If
	WScript.Echo "Got."      

	WScript.Echo "Hooking kernel32.dll!CreateFileW..."
    Set myHook = spyMgr.CreateHook("kernel32.dll!CreateFileW", 0)   
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
