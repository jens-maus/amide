/***************************************************************************

 AmIDE - Amiga Integrated Development Environment

 Copyright (C) 1998-2002 by Jens Langner <Jens.Langner@light-speed.de>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 AmIDE Official Support Site :  http://amide.light-speed.de/
 AmIDE OpenSource project    :  http://sourceforge.net/projects/amigaIDE/

 $Id$

***************************************************************************/

#ifndef _SASC_MODULE_H_
#define _SASC_MODULE_H_

#include <clib/debug_protos.h>
#include <proto/intuition.h>

#include "compiler.h"

// globales debug makro
#define DEBUG_MAKRO_NOLF { kprintf("0x%08lx # "__FUNC__, (ULONG)FindTask(NULL)); }
#define DEBUG_MAKRO_LF   { kprintf("\n", NULL); }
#define DEBUG_MAKRO      { kprintf("0x%08lx # "__FUNC__"\n", (ULONG)FindTask(NULL)); }

/*
** Define value for the library definitions
*/

#define API_VERS  "API 1.0"
#define AUTHOR    "Jens Langner"
#define COMPILER  "SAS/C v6.58+"
#define LIBNAME   "sasc.module"
#define LIBVER    " v0.2 (23.08.2002)"

/*
** Private Dummy-IDs
*/

#define MOD_ATTR_Dummy  (TAG_USER + 0x4AFE0000)
#define MOD_METH_Dummy  (TAG_USER + 0x2FFE0000)

/*
** private Method-IDs
*/

#define MODM_Dummy          (MOD_METH_Dummy + 0x00000100)
#define MODM_Logfile_Read   (MODM_Dummy + 1)
#define MODM_Msg_Parse      (MODM_Dummy + 2)
#define MODM_Msg_Insert     (MODM_Dummy + 3)

/*
** private Attribute-IDs
*/

#define MODA_Dummy            (MOD_ATTR_Dummy + 0x00000100)
#define MODA_MSG_BAR          (MODA_Dummy + 1)
#define MODA_MSG_WARNING      (MODA_Dummy + 2)
#define MODA_MSG_NOTE         (MODA_Dummy + 3)
#define MODA_MSG_WARNING_TXT  (MODA_Dummy + 4)
#define MODA_MSG_ERROR        (MODA_Dummy + 5)
#define MODA_MSG_COMPILER_P1  (MODA_Dummy + 6)
#define MODA_MSG_COMPILER_P2  (MODA_Dummy + 7)
#define MODA_MSG_OPTIMIZER    (MODA_Dummy + 8)
#define MODA_MSG_DEFAULT      (MODA_Dummy + 9)

/*
** Message-Structures
*/

extern struct MODS_Msg_Insert
{
  ULONG MethodID;
  ULONG msg_type;
};

/*
** Prototypes
*/

extern struct IClass *cl;

extern struct MODS_InstData
{
  Object  *parent_obj;
  char    logfile[256];
  long    log_position;
  LONG    msg_type;
  STRPTR  msg_text;
};

extern SAVEDS ASM ULONG MODULE_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg);

extern SAVEDS ASM struct IClass *AmIDE_API_GetClass( void );

#endif /* _SASC_MODULE_H_ */
