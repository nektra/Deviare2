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

//IMPORTANT: Offset and size fields are expressed in bits

#define NKT_DBFUNDTYPE_SignedByte                          1
#define NKT_DBFUNDTYPE_UnsignedByte                        2
#define NKT_DBFUNDTYPE_SignedWord                          3
#define NKT_DBFUNDTYPE_UnsignedWord                        4
#define NKT_DBFUNDTYPE_SignedDoubleWord                    5
#define NKT_DBFUNDTYPE_UnsignedDoubleWord                  6
#define NKT_DBFUNDTYPE_SignedQuadWord                      7
#define NKT_DBFUNDTYPE_UnsignedQuadWord                    8
#define NKT_DBFUNDTYPE_Float                               9
#define NKT_DBFUNDTYPE_Double                             10
#define NKT_DBFUNDTYPE_LongDouble                         11
#define NKT_DBFUNDTYPE_Void                               12
#define NKT_DBFUNDTYPE_AnsiChar                           13
#define NKT_DBFUNDTYPE_WideChar                           14

#define NKT_DBFUNDTYPE_MIN                                 1
#define NKT_DBFUNDTYPE_MAX                                14

#define NKT_DBOBJCLASS_Fundamental                         0
#define NKT_DBOBJCLASS_Struct                              1
#define NKT_DBOBJCLASS_Union                               2
#define NKT_DBOBJCLASS_Typedef                             3
#define NKT_DBOBJCLASS_Array                               4
#define NKT_DBOBJCLASS_Pointer                             5
#define NKT_DBOBJCLASS_Reference                           6
#define NKT_DBOBJCLASS_Enumeration                         7
#define NKT_DBOBJCLASS_Function                            8
#define NKT_DBOBJCLASS_FunctionType                        9
#define NKT_DBOBJCLASS_ClassConstructor                   10
#define NKT_DBOBJCLASS_ClassDestructor                    11
#define NKT_DBOBJCLASS_ClassOperatorMethod                12
#define NKT_DBOBJCLASS_ClassMethod                        13
#define NKT_DBOBJCLASS_ClassConverter                     14

#define NKT_DBOBJCLASS_MASK                       0x0000FFFF

#define NKT_DBOBJCLASSFLAG_IsConstant             0x00010000
#define NKT_DBOBJCLASSFLAG_IsVolatile             0x00020000

#define NKT_DBOBJFLAG_PubliCMember                0x00000000
#define NKT_DBOBJFLAG_ProtectedMember             0x00000001
#define NKT_DBOBJFLAG_PrivateMember               0x00000002
#define NKT_DBOBJFLAG_MEMBER_MASK                 0x00000003

#define NKT_DBOBJFLAG_StdCall                     0x00000000
#define NKT_DBOBJFLAG_CDecl                       0x00000004
#define NKT_DBOBJFLAG_FastCall                    0x00000008
#define NKT_DBOBJFLAG_ThisCall                    0x0000000C
#define NKT_DBOBJFLAG_CALLINGTYPE_MASK            0x0000000C

#define NKT_DBOBJFLAG_IsExternal                  0x00000010
#define NKT_DBOBJFLAG_IsDllImport                 0x00000020
#define NKT_DBOBJFLAG_IsPure                      0x00000040
#define NKT_DBOBJFLAG_Throw                       0x00000080
#define NKT_DBOBJFLAG_NoThrow                     0x00000100
#define NKT_DBOBJFLAG_NoReturn                    0x00000200
#define NKT_DBOBJFLAG_IsConst                     0x00000400
#define NKT_DBOBJFLAG_Deprecated                  0x00000800
#define NKT_DBOBJFLAG_NonNull                     0x00001000
#define NKT_DBOBJFLAG_Malloc                      0x00002000
#define NKT_DBOBJFLAG_IsDllExport                 0x00004000
#define NKT_DBOBJFLAG_Format                      0x00008000
#define NKT_DBOBJFLAG_FUNCTIONFLAGS_MASK          0x0000FFF0
/*
#define NKT_DBOBJFLAG_FuncClassNormal             0x00000000
#define NKT_DBOBJFLAG_FuncClassConstructor        0x00010000
#define NKT_DBOBJFLAG_FuncClassDestructor         0x00020000
#define NKT_DBOBJFLAG_FuncClassOperatorMethod     0x00030000
#define NKT_DBOBJFLAG_FuncClassMethod             0x00040000
#define NKT_DBOBJFLAG_FuncClassConverter          0x00050000
#define NKT_DBOBJFLAG_FUNCTIONCLASS_MASK          0x000F0000
*/
#define NKT_DBOBJFLAG_HasConstructor              0x00100000
#define NKT_DBOBJFLAG_HasDestructor               0x00200000
#define NKT_DBOBJFLAG_HasVirtual                  0x00400000
#define NKT_DBOBJFLAG_HasInheritance              0x00800000
#define NKT_DBOBJFLAG_STRUCTUNION_MASK            0x00F00000

//-----------------------------------------------------------

