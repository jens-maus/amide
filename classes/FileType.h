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

#ifndef _FILETYPE_H_
#define _FILETYPE_H_

#define FileTypeObject NewObject(CL_FileType->mcc_Class, NULL

// Methoden ID
#define MUIM_FileType_Dummy         (TAG_USER + 0x2FFE0200)

#define MUIM_FileType_Finish        (MUIM_FileType_Dummy + 1)
#define MUIM_FileType_Refresh       (MUIM_FileType_Dummy + 2)
#define MUIM_FileType_Update        (MUIM_FileType_Dummy + 3)
#define MUIM_FileType_Identify      (MUIM_FileType_Dummy + 4)
#define MUIM_FileType_Load          (MUIM_FileType_Dummy + 5)
#define MUIM_FileType_Save          (MUIM_FileType_Dummy + 6)
#define MUIM_FileType_Find          (MUIM_FileType_Dummy + 7)

/*
** Message-structures
*/

struct MUIP_FileType_Identify_Message
{
  ULONG MethodID;
  STRPTR name;
};

struct MUIP_FileType_Load_Message
{
  ULONG MethodID;
  STRPTR filename;
};

struct MUIP_FileType_Save_Message
{
  ULONG   MethodID;
  STRPTR  filename;
  ULONG   overwrite;
};

struct MUIP_FileType_Find_Message
{
  ULONG MethodID;
  STRPTR name;
};

/*
** general class data structure
*/

struct FileType_Data
{
  Object *MN_FileType;
  Object *LT_FileType;
  Object *BT_FileType_Add_FileType;
  Object *BT_FileType_Add_Pattern;
  Object *BT_FileType_Delete;
  Object *BS_FileType_Name;
  Object *BS_FileType_Pattern;
  Object *PP_FileType_EditTool;
  Object *BS_FileType_EditTool;
  Object *CM_FileType_Edit;
  Object *CYA_FileType_Edit;
  Object *PP_FileType_ViewTool;
  Object *BS_FileType_ViewTool;
  Object *CM_FileType_View;
  Object *CYA_FileType_View;
  Object *CM_FileType_Compileable;
  Object *CM_FileType_Linkable;
  Object *CM_FileType_Touchable;
  Object *CM_FileType_DepCheck;
  Object *CM_FileType_AutoBind;

  struct MsgPort *ReplyPort;
};

// Methoden-Prototypen
METHOD FileType_Finish              (Class *cl, Object *obj, Msg msg);
METHOD FileType_Refresh             (Class *cl, Object *obj, Msg msg);
METHOD FileType_Update              (Class *cl, Object *obj, Msg msg);
METHOD FileType_Identify            (Class *cl, Object *obj, struct MUIP_FileType_Identify_Message *msg);
METHOD FileType_Load                (Class *cl, Object *obj, struct MUIP_FileType_Load_Message *msg);
METHOD FileType_Save                (Class *cl, Object *obj, struct MUIP_FileType_Save_Message *msg);
METHOD FileType_Find                (Class *cl, Object *obj, struct MUIP_FileType_Find_Message *msg);

// Prototypen für den Konstruktur/Destruktor der Klasse
struct MUI_CustomClass *CreateFileTypeClass(void);
void                    DeleteFileTypeClass(struct MUI_CustomClass *CL_FileType);

#endif
