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

#ifndef _AMIDE_API_BASE_H_
#define _AMIDE_API_BASE_H_

#ifdef   __MAXON__
#ifndef  EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif
#else
#ifndef  EXEC_LIBRARIES
#include <exec/libraries.h>
#endif /* EXEC_LIBRARIES_H */
#endif

struct AmIDE_API_Base
{
 struct Library         exb_LibNode;
 SEGLISTPTR             exb_SegList;
 struct ExecBase       *exb_SysBase;
 struct IntuitionBase  *exb_IntuitionBase;
 struct GfxBase        *exb_GfxBase;
};

#endif /* _AMIDE_API_BASE_H_ */
