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

#ifndef _HEADERS_H_
#define _HEADERS_H_

#include <clib/alib_protos.h>
#include <clib/debug_protos.h>

#if defined(__SASC)
#include <clib/asl_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/intuition_protos.h>
#include <clib/muimaster_protos.h>
#include <clib/pm_protos.h>
#include <clib/utility_protos.h>
#endif

#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/pm.h>
#include <proto/utility.h>

#include <dos/dostags.h>

#include <MUI/BetterString_mcc.h>
#include <MUI/Busy_mcc.h>
#include <MUI/InfoText_mcc.h>
#include <MUI/NListtree_mcc.h>
#include <MUI/NList_mcc.h>
#include <MUI/NListview_mcc.h>
#include <MUI/NFloattext_mcc.h>
#include <MUI/Toolbar_mcc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif

