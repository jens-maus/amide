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

#ifndef _FILETYPE_TREE_H_
#define _FILETYPE_TREE_H_

// Variablen IDs
#define MUIV_FileTypeTree_Active          -1

// Methoden IDs
#define MUIM_FileTypeTree_Dummy       (TAG_USER + 0x2FFE0300)

#define MUIM_FileTypeTree_Finish      (MUIM_FileTypeTree_Dummy + 1)
#define MUIM_FileTypeTree_Add_Type    (MUIM_FileTypeTree_Dummy + 2)
#define MUIM_FileTypeTree_Add_Folder  (MUIM_FileTypeTree_Dummy + 3)
#define MUIM_FileTypeTree_Get         (MUIM_FileTypeTree_Dummy + 4)

#define STYLE_LT_MAINFOLDER      "\0338"
#define STYLE_LT_FOLD_CODESIZE   "\0338"
#define STYLE_LT_FOLD_TEXTSIZE   "\0338"

#define FileType_TreeObject NewObject(CL_FileTypeTree->mcc_Class, NULL

/* Structures */
struct MUIP_FileTypeTree_File
{
  ULONG  MethodID;
  STRPTR folder_name;
  STRPTR filename;
  struct MUI_NListtree_TreeNode *tn;
};

struct MUIP_FileTypeTree_Get_Message
{
  ULONG MethodID;
  LONG pos;
};

struct MUIP_FileTypeTree_Add_Folder_Message
{
  ULONG MethodID;
  STRPTR folder_name;
};

struct MUIP_FileTypeTree_Add_Type_Message
{
  ULONG   MethodID;
  STRPTR  type_name;
  STRPTR  type_pattern;
  STRPTR  type_edittool;
  int     type_edittool_std;
  int     type_edittool_sel;
  STRPTR  type_viewtool;
  int     type_viewtool_std;
  int     type_viewtool_sel;
  int     type_compileable;
  int     type_linkable;
  int     type_touchable;
  int     type_scan_dep;
  int     type_auto_binding;
};

struct FileTypeTree_Data
{
  struct MUI_NListtree_TreeNode *tn_root;
  UWORD drop_flags;
  LONG old_entry;

  struct MsgPort *ReplyPort;
};

// Die FileType-Struktur
static struct FileType
{
  char    name[256];
  char    pattern[256];
  char    view_tool[256];
  BOOL    view_tool_standard;
  int     view_tool_sel;
  char    edit_tool[256];
  BOOL    edit_tool_standard;
  int     edit_tool_sel;
  BOOL    folder;
  BOOL    compileable;
  BOOL    linkable;
  BOOL    touchable;
  BOOL    scan_dep;
  BOOL    auto_binding;
  struct  MUI_NListtree_TreeNode *parent;
};

// Methoden-Prototypen
METHOD FileTypeTree_Finish      (Class *cl, Object *obj, Msg msg);
METHOD FileTypeTree_Add_Type    (Class *cl, Object *obj, struct MUIP_FileTypeTree_Add_Type_Message *msg);
METHOD FileTypeTree_Add_Folder  (Class *cl, Object *obj, struct MUIP_FileTypeTree_Add_Folder_Message *msg);

// Prototypen für den Konstruktur/Destruktor der Klasse
struct MUI_CustomClass *CreateFileTypeTreeClass(void);
void                    DeleteFileTypeTreeClass(struct MUI_CustomClass *CL_FileTypeTree);

#endif
