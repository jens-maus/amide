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

#ifndef _MAINBAR_H_
#define _MAINBAR_H_

#define MainBarObject NewObject(CL_MainBar->mcc_Class, NULL

// Methoden IDs
#define MUIM_MainBar_Dummy                  (TAG_USER + 0x2FFE0500)

#define MUIM_MainBar_Finish                 (MUIM_MainBar_Dummy + 1)
#define MUIM_MainBar_NewProject             (MUIM_MainBar_Dummy + 2)
#define MUIM_MainBar_DelProject             (MUIM_MainBar_Dummy + 3)
#define MUIM_MainBar_Scan_List              (MUIM_MainBar_Dummy + 4)
#define MUIM_MainBar_PM_Project_Save        (MUIM_MainBar_Dummy + 5)
#define MUIM_MainBar_PM_Project_Compile     (MUIM_MainBar_Dummy + 6)
#define MUIM_MainBar_PM_Project_CompileRun  (MUIM_MainBar_Dummy + 7)
#define MUIM_MainBar_PM_Project_Debug       (MUIM_MainBar_Dummy + 8)
#define MUIM_MainBar_LoadProject            (MUIM_MainBar_Dummy + 9)

/* Structures */
extern struct MUI_CustomClass *CL_Project;

struct MainBar_Data
{
  Object *MN_Main;
  Object *TB_Main_Sources;
  Object *TB_Main_Projects;
  Object *TB_Main_Compiler;
  Object *TX_Status;
  Object *WI_PrjAss;
  Object *WI_Act_Project;
  Object *LI_Projects;

  struct MsgPort *ReplyPort;
};

struct MUIP_MainBar_NewProject_Message
{
  ULONG MethodID;
  ULONG use_assistent;
};

struct MUIP_MainBar_DelProject_Message
{
  ULONG MethodID;
  Object *obj_project;
};

// Methoden-Prototypen
METHOD MainBar_Finish                 (Class *cl, Object *obj, Msg msg);
METHOD MainBar_NewProject             (Class *cl, Object *obj, struct MUIP_MainBar_NewProject_Message *msg);
METHOD MainBar_DelProject             (Class *cl, Object *obj, struct MUIP_MainBar_DelProject_Message *msg);
METHOD MainBar_Scan_List              (Class *cl, Object *obj, Msg msg);
METHOD MainBar_PM_Project_Save        (Class *cl, Object *obj, Msg msg);
METHOD MainBar_PM_Project_Compile     (Class *cl, Object *obj, Msg msg);
METHOD MainBar_PM_Project_CompileRun  (Class *cl, Object *obj, Msg msg);
METHOD MainBar_PM_Project_Debug       (Class *cl, Object *obj, Msg msg);
METHOD MainBar_LoadProject            (Class *cl, Object *obj, Msg msg);

// Prototypen für den Konstruktur/Destruktor der Klasse
struct MUI_CustomClass *CreateMainBarClass(void);
void                    DeleteMainBarClass(struct MUI_CustomClass *CL_MainBar);

#endif
