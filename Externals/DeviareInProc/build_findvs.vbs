Option Explicit
Dim S

If WScript.Arguments.Count < 1 Then
	WScript.Quit 1
End If
S = GetVsFolder(WScript.Arguments.Item(0))
If Len(S) = 0 Then
	WScript.Quit 1
End If

WScript.Echo S
WScript.Quit 0

'-------------------------------------------------------------------------------

Function GetVsFolder(szVersion)
Dim oShell
Dim S, szEnv

	If szVersion = "2010" Then
		szEnv = "VS100COMNTOOLS"
		szVersion = "10.0"
	ElseIf szVersion = "2012" Then
		szEnv = "VS120COMNTOOLS"
		szVersion = "12.0"
	ElseIf szVersion = "2013" Then
		szEnv = "VS120COMNTOOLS"
		szVersion = "12.0"
	ElseIf szVersion = "2015" Then
		szEnv = "VS140COMNTOOLS"
		szVersion = "14.0"
	ElseIf szVersion = "2017" Then
		szEnv = "VS150COMNTOOLS"
		szVersion = "15.0"
	Else
		GetVsFolder = ""
		Exit Function
	End If

	Set oShell = WScript.CreateObject("WScript.Shell") 
	If Len(szEnv) > 0 Then
		S = oShell.ExpandEnvironmentStrings("%" & szEnv & "%")
		S = ValidateFolder(S)
		If Len(S) > 0 Then
			GetVsFolder = S
			Exit Function
		End If
	End If

	On Error Resume Next
	S = oShell.RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\SxS\VS7\" & szVersion)
	On Error Goto 0
	If Len(S) > 0 Then
		S = ValidateFolder(S & "\Common7\Tools")
		If Len(S) > 0 Then
			GetVsFolder = S
			Exit Function
		End If
	End If

	On Error Resume Next
	S = oShell.RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\SxS\VS7\" & szVersion)
	On Error Goto 0
	If Len(S) > 0 Then
		S = ValidateFolder(S & "\Common7\Tools")
		If Len(S) > 0 Then
			GetVsFolder = S
			Exit Function
		End If
	End If

	GetVsFolder = ""
End Function

Function ValidateFolder(szFolder)
Dim oFs

	szFolder = szFolder & "\"
	szFolder = Replace(szFolder, "/", "\", 1, -1, 1)
	szFolder = Replace(szFolder, "\\", "\", 1, -1, 1)

	Set oFs = CreateObject("Scripting.FileSystemObject")
	If oFs.FileExists(szFolder & "VsDevCmd.bat") Then
		ValidateFolder = szFolder
		Exit Function
	End If
	ValidateFolder = ""
End Function
