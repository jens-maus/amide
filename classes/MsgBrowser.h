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

#ifndef _MSGBROWSER_H_
#define _MSGBROWSER_H_

#define MsgBrowserObject NewObject(CL_MsgBrowser->mcc_Class, NULL

enum{ MSG_DEFAULT=0, MSG_CMPSTART, MSG_BAR, MSG_WARNING, MSG_NOTE, MSG_WARNING_TXT, MSG_ERROR, MSG_COMPILER_P1,
      MSG_COMPILER_P2, MSG_OPTIMIZER, MSG_LINKER
    };

// Werte
#define MUIV_MsgBrowser_Closed      -100

// Attributes ID
#define MUIA_MsgBrowser_Dummy       (TAG_USER + 0x4AFE0600)

#define MUIA_MsgBrowser_Status      (MUIA_MsgBrowser_Dummy + 1)
#define MUIA_MsgBrowser_File        (MUIA_MsgBrowser_Dummy + 2)
#define MUIA_MsgBrowser_Project     (MUIA_MsgBrowser_Dummy + 3)
#define MUIA_MsgBrowser_ProjectTree (MUIA_MsgBrowser_Dummy + 4)
#define MUIA_MsgBrowser_FileType    (MUIA_MsgBrowser_Dummy + 5)
#define MUIA_MsgBrowser_Compiler    (MUIA_MsgBrowser_Dummy + 6)

// Methoden ID
#define MUIM_MsgBrowser_Dummy       (TAG_USER + 0x2FFE0600)

#define MUIM_MsgBrowser_Finish      (MUIM_MsgBrowser_Dummy + 1)
#define MUIM_MsgBrowser_Update      (MUIM_MsgBrowser_Dummy + 2)
#define MUIM_MsgBrowser_Clear       (MUIM_MsgBrowser_Dummy + 3)

/* Structures */
struct MUIP_MsgBrowser_Start_Message
{
  ULONG MethodID;
  ULONG type;
  STRPTR file;
};

struct MUIP_MsgBrowser_Compile_Message
{
  ULONG MethodID;
  STRPTR filename;
};

struct MUIP_MsgBrowser_Link_Message
{
  ULONG MethodID;
  STRPTR *file_array;
  STRPTR exe_name;
};

struct MUIP_MsgBrowser_Log_Read_Message
{
  ULONG MethodID;
  STRPTR filename;
};

struct MUIP_MsgBrowser_Msg_Parse_Message
{
  ULONG MethodID;
  STRPTR string;
};

struct MUIP_MsgBrowser_Msg_Set_Message
{
  ULONG MethodID;
  STRPTR string;
};

struct MUIP_MsgBrowser_Msg_Insert_Message
{
  ULONG MethodID;
  STRPTR string;
};

struct MsgBrowser_Data
{
  ULONG   log_position;
  LONG    max_errors;
  LONG    max_warnings;
  LONG    max_notes;
  char    status[256];
  BOOL    tmp_display;

  Object *LV_MsgBrowser;
  Object *LI_MsgBrowser;
  Object *BT_MsgBrowser_Run;
  Object *BT_MsgBrowser_Debug;
  Object *BT_MsgBrowser_Stop;
  Object *BC_MsgBrowser_Image_Warning;
  Object *BC_MsgBrowser_Image_Error;
  Object *TX_MsgBrowser_Error;
  Object *TX_MsgBrowser_Warning;
  Object *TX_MsgBrowser_Note;
  Object *BY_MsgBrowser;
  Object *TX_MsgBrowser_File;
  Object *TX_MsgBrowser_Status;
  Object *LS_MsgBrowser_Entries;

  // Die Objekte anderer Klassen
  Object *OB_Project;
  Object *OB_ProjectTree;
  Object *OB_FileType;
  Object *OB_Compiler;

  struct MsgPort *ReplyPort;
};

// Methoden-Prototypen
METHOD MsgBrowser_Clear         (Class *cl, Object *obj, Msg msg);
METHOD MsgBrowser_Finish        (Class *cl, Object *obj, Msg msg);
METHOD MsgBrowser_Insert        (Class *cl, Object *obj, struct AmIDEP_msginsert_Message *msg);
METHOD MsgBrowser_Update        (Class *cl, Object *obj, Msg msg);

// Prototypen für den Konstruktur/Destruktor der Klasse
struct MUI_CustomClass *CreateMsgBrowserClass(void);
void                    DeleteMsgBrowserClass(struct MUI_CustomClass *);

#endif
