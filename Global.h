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

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#ifndef _VERSION_H_
#include "AmIDE_rev.h"
#endif

#ifndef _HEADERS_H_
#include "Headers.h"
#endif

// API headers
#ifndef _API_H_
#include "modules/api/clib/amide_api_protos.h"
#include "modules/api/pragma/amide_api_pragmas.h"
#include "modules/api/amide/amide_api.h"
#endif

#define DEFAULTCFGFILE  "PROGDIR:AmIDE.default"

// Jetzt die globalen Defines setzen
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))

/* Compiler Stuff */
#define INLINE __inline
#define REG(x) register __ ## x
#define ASM    __asm
#define SAVEDS __saveds
//#define METHOD __inline ULONG
#define METHOD ULONG

/* Macros */
#define EMPTYSTR(Str) (!(*(Str)))
#define FINDOBJ(Obj, UData) ((Object *)DoMethod((Obj), MUIM_FindUData, (UData)))

// globales debug makro
#define DEBUG_MAKRO_NOLF { kprintf("0x%08lx # "__FUNC__, (ULONG)FindTask(NULL)); }
#define DEBUG_MAKRO_LF   { kprintf("\n", NULL); }
#define DEBUG_MAKRO      { kprintf("0x%08lx # "__FUNC__"\n", (ULONG)FindTask(NULL)); }

#endif

