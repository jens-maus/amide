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

#ifndef _PROJECTTREE_H_
#define _PROJECTTREE_H_

// The entry_type
enum{ ENTRY_TYPE_FOLDER=1, ENTRY_TYPE_FILE, ENTRY_TYPE_SUBFOLDER, ENTRY_TYPE_EXECUTEABLE
    };

// Variablen IDs
#define MUIV_ProjectTree_Active          -1

// Attributes ID
#define MUIA_ProjectTree_Dummy           (TAG_USER + 0x4AFE0400)

#define MUIA_ProjectTree_Creator         (MUIA_ProjectTree_Dummy + 1)

// Methoden IDs
#define MUIM_ProjectTree_Dummy           (TAG_USER + 0x2FFE0400)

#define MUIM_ProjectTree_Finish          (MUIM_ProjectTree_Dummy + 1)
#define MUIM_ProjectTree_Refresh_Sizes   (MUIM_ProjectTree_Dummy + 2)
#define MUIM_ProjectTree_Add_File        (MUIM_ProjectTree_Dummy + 3)
#define MUIM_ProjectTree_Add_File_ByType (MUIM_ProjectTree_Dummy + 4)
#define MUIM_ProjectTree_Add_Folder      (MUIM_ProjectTree_Dummy + 5)
#define MUIM_ProjectTree_Add_Exe         (MUIM_ProjectTree_Dummy + 6)
#define MUIM_ProjectTree_Get_Entry       (MUIM_ProjectTree_Dummy + 7)
#define MUIM_ProjectTree_Find_Exe        (MUIM_ProjectTree_Dummy + 8)

#define STYLE_LT_MAINFOLDER      "\0338"
#define STYLE_LT_FOLD_CODESIZE   "\0338"
#define STYLE_LT_FOLD_TEXTSIZE   "\0338"

#define ProjectTreeObject NewObject(CL_ProjectTree->mcc_Class, NULL

/* Message - Structures */

struct MUIP_ProjectTree_Refresh_Sizes_Message
{
  ULONG MethodID;
  struct  MUI_NListtree_TreeNode *tn;
  struct  Entry_Data *entry;
};

struct MUIP_ProjectTree_Add_Folder_Message
{
  ULONG   MethodID;
  STRPTR  folder_name;
  ULONG   open;
  STRPTR  folder_parent;
};

struct MUIP_ProjectTree_Add_File_Message
{
  ULONG MethodID;
  STRPTR  filename;
  STRPTR  folder_name;
  struct  Entry_Data *entry;
  ULONG   compileable;
  ULONG   linkable;
  ULONG   touchable;
  ULONG   scan_dep;
  ULONG   binding;
  STRPTR  edittool;
  ULONG   edittool_std;
  int     edittool_sel;
  STRPTR  viewtool;
  ULONG   viewtool_std;
  int     viewtool_sel;
};

struct MUIP_ProjectTree_Add_File_ByType_Msg
{
  ULONG MethodID;
  STRPTR filename;
  struct FileType *type;
};

struct MUIP_ProjectTree_GetEntry_Message
{
  ULONG MethodID;
  LONG pos;
};

struct ProjectTree_Data
{
  struct  MUIS_ProjectTree_TreeNode *tn_root;
  UWORD   drop_flags;
  LONG    old_entry;
  LONG    drag_x;
  LONG    drag_y;
  ULONG   entries;
  Object  *creator_obj;

  struct MsgPort *ReplyPort;
};


// Die Entry-Struktur
static struct Entry_Data
{
  STRPTR entry_file;
  STRPTR entry_code;
  STRPTR entry_text;
  LONG   entry_type;
  struct DateStamp  entry_date;
  BOOL   entry_compileable;
  BOOL   entry_linkable;
  BOOL   entry_touchable;
  BOOL   entry_dep_scan;
  BOOL   entry_binding;
  BOOL   entry_open;
  STRPTR entry_edittool;
  BOOL   entry_edittool_std;
  int    entry_edittool_sel;
  STRPTR entry_viewtool;
  BOOL   entry_viewtool_std;
  int    entry_viewtool_sel;
  Object *entry_dep;
  struct MUI_NListtree_TreeNode *entry_parent;
};

// Methoden-Prototypen
METHOD ProjectTree_Finish           (Class *cl, Object *obj, Msg msg);
METHOD ProjectTree_Refresh_Sizes    (Class *cl, Object *obj, struct MUIP_ProjectTree_Refresh_Sizes_Message *msg);
METHOD ProjectTree_Add_File         (Class *cl, Object *obj, struct MUIP_ProjectTree_Add_File_Message *msg);
METHOD ProjectTree_Add_File_ByType  (Class *cl, Object *obj, struct MUIP_ProjectTree_Add_File_ByType_Msg *msg);
METHOD ProjectTree_Add_Folder       (Class *cl, Object *obj, struct MUIP_ProjectTree_Add_Folder_Message *msg);
METHOD ProjectTree_Add_Exe          (Class *cl, Object *obj, Msg msg);
METHOD ProjectTree_Get_Entry        (Class *cl, Object *obj, struct MUIP_ProjectTree_GetEntry_Message *msg);
METHOD ProjectTree_Find_Exe         (Class *cl, Object *obj, Msg msg);
METHOD ProjectTree_Create_CMenu     (Class *cl, Object *obj, struct MUIP_ContextMenuBuild *msg);

// Prototypen für den Konstruktur/Destruktor der Klasse
struct MUI_CustomClass *CreateProjectTreeClass(void);
void                    DeleteProjectTreeClass(struct MUI_CustomClass *CL_ProjectTree);

#endif
