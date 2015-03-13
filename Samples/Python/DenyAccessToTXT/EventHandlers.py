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
 
# Event Handlers ======================================================================

import win32com.client

class NktSpyMgrEvents:

	def OnAgentLoad(self, proc, errorCode):
		if not errorCode == 0:
			print ("OnAgenLoad error code: %d" % (errorCode,))

	def OnProcessStarted(self, nktProcessAsPyIDispatch):
		nktProcess = win32com.client.Dispatch(nktProcessAsPyIDispatch)
		if (nktProcess.Name == "notepad.exe"):
			print ("Notepad was started.")

	def OnProcessTerminated(self, nktProcessAsPyIDispatch):
		nktProcess = win32com.client.Dispatch(nktProcessAsPyIDispatch)
		if (nktProcess.Name == "notepad.exe"):
			print ("Notepad was terminated.")

	def OnFunctionCalled(self, nktHookAsPyIDispatch, nktProcessAsPyIDispatch, nktHookCallInfoAsPyIDispatch):
		nktHookCallInfo = win32com.client.Dispatch(nktHookCallInfoAsPyIDispatch)
		nktProcess = win32com.client.Dispatch(nktProcessAsPyIDispatch)
		if (nktHookCallInfo.IsPreCall):
			fileName = self.GetFileNameParam(nktHookCallInfo.Params())
			if (fileName.endswith(".txt")):
				self.SkipCall(nktHookCallInfo, nktProcess)

# Aux Functions =========================================================================

	def SkipCall(self, nktHookCallInfo, nktProcess):
		nktHookCallInfo.SkipCall()
		if (nktProcess.PlatformBits == 64):
			nktHookCallInfo.Result().LongLongVal = -1
		else:
			nktHookCallInfo.Result().LongVal = -1
		nktHookCallInfo.LastError = 5

	def GetFileNameParam(self, nktParamsEnum):
		nktParam = nktParamsEnum.First()
		return nktParam.Value
