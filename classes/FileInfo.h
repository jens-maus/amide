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

#ifndef _FILEINFO_H_
#define _FILEINFO_H_

#define FileInfoObject NewObject(CL_FileInfo->mcc_Class, NULL

#define MUIM_FileInfo_Dummy         (TAG_USER + 0x2FFE0100)

#define MUIM_FileInfo_Finish        (MUIM_FileInfo_Dummy + 1)
#define MUIM_FileInfo_Close         (MUIM_FileInfo_Dummy + 2)
#define MUIM_FileInfo_Refresh       (MUIM_FileInfo_Dummy + 3)
#define MUIM_FileInfo_Update        (MUIM_FileInfo_Dummy + 4)

/* The Hooks that we can call */
extern const struct Hook FileInfo_DestructHook;
extern const struct Hook FileInfo_DisplayHook;

/* Structures */
static struct FileInfo_Data
{
  struct Entry_Data *entry;
  Object *BS_FileInfo_FileName;
  Object *TX_FileInfo_FileSize;
  Object *TX_FileInfo_CodeSize;
  Object *TX_FileInfo_Date;
  Object *PP_FileInfo_EditTool;
  Object *BS_FileInfo_EditTool;
  Object *CM_FileInfo_Edit;
  Object *CYA_FileInfo_Edit;
  Object *PP_FileInfo_ViewTool;
  Object *BS_FileInfo_ViewTool;
  Object *CM_FileInfo_View;
  Object *CYA_FileInfo_View;
  Object *CM_FileInfo_Compileable;
  Object *CM_FileInfo_Linkable;
  Object *CM_FileInfo_Touchable;
  Object *CM_FileInfo_DepCheck;
  Object *CM_FileInfo_Binding;
  Object *LV_FileInfo;
  Object *LI_FileInfo;
  Object *vgr;

  char datestring[256];

  struct MsgPort *ReplyPort;
};

static struct MUIP_FileInfo_Message
{
  ULONG MethodID;
  struct Entry_Data *entry;
};

// Methoden-Prototypen
METHOD FileInfo_Finish              (Class *cl, Object *obj, Msg msg);
METHOD FileInfo_Close               (Class *cl, Object *obj, Msg msg);
METHOD FileInfo_Refresh             (Class *cl, Object *obj, Msg msg);
METHOD FileInfo_Update              (Class *cl, Object *obj, Msg msg);

// Prototypen für den Konstruktur/Destruktor der Klasse
struct MUI_CustomClass *CreateFileInfoClass(void);
void                    DeleteFileInfoClass(struct MUI_CustomClass *);

#endif
