/*
 * Copyright (C) 2015 Nektra S.A., Buenos Aires, Argentina.
 * All rights reserved. Contact: http://www.nektra.com
 *
 *
 * This file is part of Deviare Process Monitor
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
#ifndef _DRIVER_INTERFACE_H_
#define _DRIVER_INTERFACE_H_

//-----------------------------------------------------------

#define DEVIAREPD_SERVICE_NAME              L"DeviarePD"
#define DEVIAREPD_DEVICE_NAME_STRING        L"\\\\.\\DeviareProcessNotification"
#define DEVIAREPD_NTDEVICE_NAME_STRING      L"\\Device\\DeviareProcessNotification"
#define DEVIAREPD_SYMBOLIC_NAME_STRING      L"\\DosDevices\\DeviareProcessNotification"

#define DEVIAREPD_DEVICE_TYPE              0x9999

#define DEVIAREPD_IOCTL_SET_NOTIFICATION_EVENT  CTL_CODE(DEVIAREPD_DEVICE_TYPE, 0x800, METHOD_BUFFERED, FILE_WRITE_ACCESS)

typedef struct _DEVIAREPD_READ_REQUEST {
  DWORD dwPid;
  ULONG Created;
} DEVIAREPD_READ_REQUEST, *LPDEVIAREPD_READ_REQUEST;

//-----------------------------------------------------------

#endif //_DRIVER_INTERFACE_H_
