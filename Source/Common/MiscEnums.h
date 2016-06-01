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

#include "EngDefines.h"

//-----------------------------------------------------------

namespace ENktDvMiscEnums {

  typedef enum {
    asmRegNone=0,
    //----
    asmRegEax=1, asmRegEbx=2, asmRegEcx=3, asmRegEdx=4,
    asmRegEaxEdxPair=5,
    asmRegSt0=6,
    asmRegEsp=7, asmRegEbp=8, asmRegEip=9, asmRegEsi=10,
    asmRegEdi=11,
    //----
    asmRegRax=1,  asmRegRbx=2,   asmRegRcx=3,   asmRegRdx=4,
    asmRegR8=5,   asmRegR9=6,    asmRegR10=7,   asmRegR11=8,
    asmRegXmm0=9, asmRegXmm1=10, asmRegXmm2=11, asmRegXmm3=12,
    asmRegRsp=13, asmRegRbp=14,  asmRegRip=15,  asmRegRsi=16,
    asmRegRdi=17
  } eNktDvRegister;

} //ENktDvMiscEnums

//-----------------------------------------------------------

