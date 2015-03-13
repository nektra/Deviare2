#
# Copyright (C) 2010-2015 Nektra S.A., Buenos Aires, Argentina.
# All rights reserved. Contact: http://www.nektra.com
#
#
# This file is part of Deviare
#
#
# Commercial License Usage
# ------------------------
# Licensees holding valid commercial Deviare licenses may use this file
# in accordance with the commercial license agreement provided with the
# Software or, alternatively, in accordance with the terms contained in
# a written agreement between you and Nektra.  For licensing terms and
# conditions see http://www.nektra.com/licensing/. Use the contact form
# at http://www.nektra.com/contact/ for further information.
#
#
# GNU General Public License Usage
# --------------------------------
# Alternatively, this file may be used under the terms of the GNU General
# Public License version 3.0 as published by the Free Software Foundation
# and appearing in the file LICENSE.GPL included in the packaging of this
# file.  Please visit http://www.gnu.org/copyleft/gpl.html and review the
# information to ensure the GNU General Public License version 3.0
# requirements will be met.
#
#
 
# Auxiliar Functions =====================================================================

from subprocess import *
import os, sys

def GetPIDByProcessName(aProcessName):
	for proc in psutil.process_iter():
		if proc.name == aProcessName:
			return proc.pid

def OpenNotepadAndSuspend(spyManager):
	print ("Starting Notepad...")
	notepadPath = os.path.join(os.environ['WINDIR'],"syswow64\\notepad.exe")
	notepad, continueEvent = spyManager.CreateProcess(notepadPath, True)
	if notepad is None:
		notepadPath = os.path.join(os.environ['WINDIR'],"system32\\notepad.exe")
		notepad, continueEvent = spyManager.CreateProcess(notepadPath, True)
	if notepad is None:
		print ("Cannot launch Notepad")
		sys.exit(0)
	return notepad, continueEvent

def HookFunctionForProcess(spyManager, functionModuleAndName, notepadPID):
	print ("Hooking function " + functionModuleAndName + " for Notepad...")
	hook = spyManager.CreateHook(functionModuleAndName, 0)
	hook.Attach(notepadPID, True)
	hook.Hook(True)
	print ("Notepad successfully hooked")
	return hook

def StartNotepadAndHook(spyManager):
	notepad, continueEvent = OpenNotepadAndSuspend(spyManager)
	hook = HookFunctionForProcess(spyManager, "kernel32.dll!CreateFileW", notepad.Id)
	spyManager.ResumeProcess(notepad, continueEvent)
	return notepad
