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

#pragma once

//-----------------------------------------------------------

#define _FAC_DEVIARE                                   0xA32

#define MAKE_DEVIARE_HRESULT(code)                         \
                         MAKE_HRESULT(1, _FAC_DEVIARE, code)

#define NKT_DVERR_CannotLoadDatabase         MAKE_DEVIARE_HRESULT(1)
#define NKT_DVERR_OnlyOneInstance            MAKE_DEVIARE_HRESULT(2)
#define NKT_DVERR_ProtocolVersionMismatch    MAKE_DEVIARE_HRESULT(3)
#define NKT_DVERR_InvalidTransportData       MAKE_DEVIARE_HRESULT(4)
#define NKT_DVERR_NotFound                   HRESULT_FROM_WIN32(ERROR_NOT_FOUND)
#define NKT_DVERR_ArithmeticOverflow         HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW)
#define NKT_DVERR_AlreadyExists              HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS)
#define NKT_DVERR_AlreadyInitialized         HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED)
#define NKT_DVERR_Cancelled                  HRESULT_FROM_WIN32(ERROR_CANCELLED)
#define NKT_DVERR_NotSupported               HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED)
#define NKT_DVERR_ExceptionRaised            DISP_E_EXCEPTION
#define NKT_DVERR_HookIsInactive             MAKE_DEVIARE_HRESULT(5)
#define NKT_DVERR_PartialCopy                HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY)
#define NKT_DVERR_LicenseNotAccepted         MAKE_DEVIARE_HRESULT(6)

//-----------------------------------------------------------

