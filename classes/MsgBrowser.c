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

#include "Global.h"
#include "CompClient.h"
#include "ProjectTree.h"
#include "Main.h"
#include "MsgBrowser.h"
#include "MsgBrowser_images.h"
#include "FileType.h"
#include "Project.h"

/* Functions */
SAVEDS ASM ULONG MsgBrowser_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg);

/*
** MsgBrowser_DestFunc
** Funktion + Hook der beim Entfernen des Objektes ausgeführt wird
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** struct Hook *     (a0): Der aufrufende Hook
** Object *          (a2): Das Objekt
** struct NList_...  (a1): Die übergebene DestructMessage
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** keine
*/


SAVEDS ASM void MsgBrowser_DestFunc(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct NList_DestructMessage *ndm)
{
  struct AmIDES_message *message = (struct AmIDES_message *)ndm->entry;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(message->text) free(message->text);
  if(message)       free(message);
}

const struct Hook MsgBrowser_DestructHook  = { { NULL, NULL }, (void *)MsgBrowser_DestFunc, NULL, NULL };


/*
** MsgBrowser_DisplFunc
** Funktion + Hook der beim Anzeigen des Objektes einer Klasse aufgerufen wird
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** struct Hook *     (a0): Der aufrufende Hook
** Object *          (a2): Das Objekt
** struct MUIP_...   (a1): Die übergebene ConstructMessage
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** keine
*/


SAVEDS ASM LONG MsgBrowser_DisplFunc(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct NList_DisplayMessage *ndm)
{
  struct AmIDES_message *message = (struct AmIDES_message *)ndm->entry;

  static BOOL open = FALSE;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if (message)
  {
    // Erstmal defaultwerte setzen
    ndm->preparses[0] = "\033c";
    ndm->preparses[1] = " ";
    ndm->preparses[2] = " ";
    ndm->preparses[3] = " ";
    ndm->strings[0]   = " ";
    ndm->strings[1]   = " ";
    ndm->strings[2]   = " ";
    ndm->strings[3]   = " ";

    switch(message->type)
    {
      case AmIDE_MSG_WARNING:
      {
        ndm->preparses[1] = "\033r\0338";
        ndm->preparses[2] = "\033r\0338";
        ndm->preparses[3] = AmIDE_MSG_STYLE_WARNING;
        ndm->strings[0]   = "\033o[0]";
      }
      break;

      case AmIDE_MSG_ERROR:
      {
        ndm->preparses[1] = "\033r\0338";
        ndm->preparses[2] = "\033r\0338";
        ndm->preparses[3] = AmIDE_MSG_STYLE_ERROR;
        ndm->strings[0]   = "\033o[1]";
      }
      break;

      case AmIDE_MSG_NOTE:
      {
        ndm->preparses[1] = "\033r\0338";
        ndm->preparses[2] = "\033r\0338";
        ndm->preparses[3] = AmIDE_MSG_STYLE_NOTE;
      }
      break;
    }

    // Nun schauen ob die errormsg oder line -1 ist und dann nicht darstellen
    {
      static char errnr[5];
      static char line[5];

      if(message->errnr >= 0)
      {
        sprintf(errnr, "%ld", message->errnr);
        ndm->strings[1]   = errnr;
      }

      if(message->line >= 0)
      {
        sprintf(line, "%ld", message->line);
        ndm->strings[2]   = line;
      }
    }

    ndm->strings[3] = message->text;
  }
  else
  {
    ndm->preparses[0]  = "\033c";
    ndm->preparses[1]  = "\033l\033b";
    ndm->preparses[2]  = "\033l\033b";
    ndm->preparses[3]  = "\033l\033b";
    ndm->strings[0]  = " ";
    ndm->strings[1]  = "Nr.";
    ndm->strings[2]  = "Line";
    ndm->strings[3]  = "Message";
  }

  return(0);
}

const struct Hook MsgBrowser_DisplayHook  = { { NULL, NULL }, (void *)MsgBrowser_DisplFunc, NULL, NULL };


/*
** MsgBrowser_New
** Methode die beim Erzeugen des Objektes ausgeführt wird
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class *:  Die aufrufende Klasse
** Object *: Das Objekt
** struct opSet *: Die Message
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** METHOD: Das neue Objekt
*/

METHOD MsgBrowser_New(Class *cl, Object *obj, struct opSet *msg)
{
  struct MsgBrowser_Data *Data;
  struct MsgBrowser_Data Temp;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

//   LocalizeStringArray(CYA_Command);
//   LocalizeNewMenu(NM_Mainwin);

  obj = (Object *)DoSuperNew(cl, obj,
    MUIA_Window_Title,  " Message Browser",
    MUIA_Window_ID,     MAKE_ID('W','N','0','1'),
    WindowContents,     VGroup,
      Child, HGroup,
        Child, ColGroup(3),
          GroupFrameT("Messages"),

          Child, Temp.BC_MsgBrowser_Image_Error = BodychunkObject,
            MUIA_FixWidth             , BODY_Error_Width ,
            MUIA_FixHeight            , BODY_Error_Height,
            MUIA_Bitmap_Width         , BODY_Error_Width ,
            MUIA_Bitmap_Height        , BODY_Error_Height,
            MUIA_Bodychunk_Depth      , BODY_Error_Depth ,
            MUIA_Bodychunk_Body       , (UBYTE *) BODY_Error_Data,
            MUIA_Bodychunk_Compression, BODY_Error_Compression,
            MUIA_Bodychunk_Masking    , BODY_Error_Masking,
            MUIA_Bitmap_SourceColors  , (ULONG *) BODY_Error_Colors,
            MUIA_Bitmap_Transparent   , 0,
          End,
          Child, Label( "Errors:" ),
          Child, Temp.TX_MsgBrowser_Error = TextObject,
            MUIA_Text_Contents, "    0",
            MUIA_Text_SetMax,   TRUE,
            MUIA_Text_PreParse, "\033r\0338",
          End,

          Child, Temp.BC_MsgBrowser_Image_Warning = BodychunkObject,
            MUIA_FixWidth             , BODY_Warning_Width ,
            MUIA_FixHeight            , BODY_Warning_Height,
            MUIA_Bitmap_Width         , BODY_Warning_Width ,
            MUIA_Bitmap_Height        , BODY_Warning_Height,
            MUIA_Bodychunk_Depth      , BODY_Warning_Depth ,
            MUIA_Bodychunk_Body       , (UBYTE *) BODY_Warning_Data,
            MUIA_Bodychunk_Compression, BODY_Warning_Compression,
            MUIA_Bodychunk_Masking    , BODY_Warning_Masking,
            MUIA_Bitmap_SourceColors  , (ULONG *) BODY_Warning_Colors,
            MUIA_Bitmap_Transparent   , 0,
          End,
          Child, Label( "Warnings:" ),
          Child, Temp.TX_MsgBrowser_Warning = TextObject,
            MUIA_Text_Contents, "    0",
            MUIA_Text_SetMax,   TRUE,
            MUIA_Text_PreParse, "\033r\0338",
          End,

          Child, BodychunkObject,
            MUIA_FixWidth             , BODY_Warning_Width ,
            MUIA_FixHeight            , BODY_Warning_Height,
            MUIA_Bitmap_Width         , BODY_Warning_Width ,
            MUIA_Bitmap_Height        , BODY_Warning_Height,
            MUIA_Bodychunk_Depth      , BODY_Warning_Depth ,
            MUIA_Bodychunk_Body       , (UBYTE *) BODY_Warning_Data,
            MUIA_Bodychunk_Compression, BODY_Warning_Compression,
            MUIA_Bodychunk_Masking    , BODY_Warning_Masking,
            MUIA_Bitmap_SourceColors  , (ULONG *) BODY_Warning_Colors,
            MUIA_Bitmap_Transparent   , 0,
          End,
          Child, Label( "Notes:" ),
          Child, Temp.TX_MsgBrowser_Note = TextObject,
            MUIA_Text_Contents, "    0",
            MUIA_Text_SetMax,   TRUE,
            MUIA_Text_PreParse, "\033r\0338",
          End,

        End,
        Child, VGroup,
          GroupFrameT("Status"),

          Child, Temp.BY_MsgBrowser = BusyObject,
            MUIA_Busy_Speed, MUIV_Busy_Speed_Off,
          End,

          Child, ColGroup(2),

            Child, Label( "File:" ),
            Child, Temp.TX_MsgBrowser_File = TextObject,
              MUIA_Text_PreParse, "\0338",
            End,

            Child, Label( "Status:" ),
            Child, Temp.TX_MsgBrowser_Status = TextObject,
              MUIA_Text_PreParse, "\0338",
            End,

          End,
        End,
      End,
      Child, Temp.LV_MsgBrowser = NListviewObject,
        MUIA_CycleChain, 1,
        MUIA_NListview_NList,               Temp.LI_MsgBrowser = NListObject,
          MUIA_ObjectID,                    MAKE_ID('N','L','0','1'),
          MUIA_NList_DefaultObjectOnClick,  TRUE,
          MUIA_NList_MultiSelect,           MUIV_NList_MultiSelect_None,
          MUIA_NList_DisplayHook2,          &MsgBrowser_DisplayHook,
          MUIA_NList_SkipChars,             "\t",
//          MUIA_NList_CompareHook2, &CompareLI_TextHook,
//          MUIA_NList_ConstructHook2, &ConstructLI_TextHook,
          MUIA_NList_DestructHook2,         &MsgBrowser_DestructHook,
          MUIA_NList_Format,                "COL=0 BAR PW=24, COL=1 BAR PW=24, COL=2 BAR PW=24, COL=3 BAR",
          MUIA_Font,                        MUIV_NList_Font_Fixed,
          MUIA_NList_AutoVisible,           TRUE,
          MUIA_NList_TitleSeparator,        TRUE,
          MUIA_NList_Title,                 TRUE,
//          MUIA_NList_EntryValueDependent, TRUE,
          MUIA_NList_MinColSortable,        0,
        End,
//      MUIA_ShortHelp, "The nice multicolumn\ndraggable list\nwith char selection\npossibility :)",
      End,
      Child, HGroup,
        Child, Temp.BT_MsgBrowser_Run     = KeyButton("Run",    "R"),
        Child, Temp.BT_MsgBrowser_Debug   = KeyButton("Debug",  "D"),
        Child, Temp.BT_MsgBrowser_Stop    = KeyButton("Stop",   "S"),
      End,
    End,
  End;

  if (!obj) return(0);

  Temp.LS_MsgBrowser_Entries = ListObject,
                               End;

  if(!Temp.LS_MsgBrowser_Entries) return(0);

  /* Nun Data instanzieren nachdem das Objekt erzeugt wurde */
  Data = INST_DATA(cl, obj);
  *Data = Temp;

  // Variablen initialisieren
  Data->max_errors    = 0;
  Data->max_warnings  = 0;
  Data->max_notes     = 0;
  strcpy(Data->status, "idle...");

  // Nun die Attribute holen
  Data->OB_Project      = (Object *)GetTagData(MUIA_MsgBrowser_Project,     NULL, msg->ops_AttrList);
  Data->OB_ProjectTree  = (Object *)GetTagData(MUIA_MsgBrowser_ProjectTree, NULL, msg->ops_AttrList);
  Data->OB_FileType     = (Object *)GetTagData(MUIA_MsgBrowser_FileType,    NULL, msg->ops_AttrList);
  Data->OB_Compiler     = (Object *)GetTagData(MUIA_MsgBrowser_Compiler,    NULL, msg->ops_AttrList);

  // Temporär mal den Debug Button ausschalten, da er eh noch nicht funzt
  set(Data->BT_MsgBrowser_Debug,  MUIA_Disabled, TRUE);
  set(Data->BT_MsgBrowser_Stop,   MUIA_Disabled, TRUE);

  /*
  ** Verbinden der Bodychunk Objekte mit der NList Klasse
  */

  DoMethod( Data->LI_MsgBrowser, MUIM_NList_UseImage, Temp.BC_MsgBrowser_Image_Warning, 0, 0);
  DoMethod( Data->LI_MsgBrowser, MUIM_NList_UseImage, Temp.BC_MsgBrowser_Image_Error,   1, 0);

  DoMethod( Data->BT_MsgBrowser_Run, MUIM_Notify, MUIA_Pressed, FALSE,
            Data->OB_Project, 1, MUIM_Project_RunExe
          );

  /*
  DoMethod( Temp.BT_Project_Abort, MUIM_Notify, MUIA_Pressed, FALSE,
            obj, 2, MUIM_Application_ReturnID, BT_PRJ_ABORT);

*/
//   DoMethod(obj, MUIM_Mainwin_Ghostings);
//   DoMethod(obj, MUIM_Mainwin_CYCmdState);

   // Notify Window
   DoMethod(  obj,  MUIM_Notify,  MUIA_Window_CloseRequest, TRUE,
              obj,  3, MUIM_Set, MUIA_Window_Open, FALSE
           );

   return ((ULONG)obj);
}

/*
** MsgBrowser_Set
** Die OM_SET Methode des Objektes.
** Hierdurch werden die Attribute gesetzt
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class *        : Die Klasse des Objektes
** Object *       : Das Objekt
** struct opSet * : Pointer auf die opGet Struktur
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** 0
*/

METHOD MsgBrowser_Set(Class *cl, Object *obj, struct opSet *msg)
{
  struct MsgBrowser_Data *Data = INST_DATA(cl, obj);
  struct TagItem *newdata;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Das TagItem finden und die Variable setzen
  if (newdata = FindTagItem(MUIA_MsgBrowser_Status, msg -> ops_AttrList))
  {
    strcpy(Data->status, (STRPTR)newdata->ti_Data);

    set(Data->TX_MsgBrowser_Status, MUIA_Text_Contents, Data->status);
  }

  if (newdata = FindTagItem(MUIA_MsgBrowser_File, msg -> ops_AttrList))
  {
    set(Data->TX_MsgBrowser_File, MUIA_Text_Contents, newdata->ti_Data);
  }

  if (newdata = FindTagItem(MUIA_MsgBrowser_Project, msg -> ops_AttrList))
  {
    Data->OB_Project = (Object *)(newdata->ti_Data);
  }

  if (newdata = FindTagItem(MUIA_MsgBrowser_ProjectTree, msg -> ops_AttrList))
  {
    Data->OB_ProjectTree = (Object *)(newdata->ti_Data);
  }

  if (newdata = FindTagItem(MUIA_MsgBrowser_FileType, msg -> ops_AttrList))
  {
    Data->OB_FileType = (Object *)(newdata->ti_Data);
  }


  // Alle anderen Items gehen uns nichts an
  DoSuperMethodA(cl, obj, (Msg) msg);

  return(0);
}

/*
** MsgBrowser_Clear
**
** Methode um das MsgBrowser Objekt neu zu initialisiern und das Listview zu löschen etc.
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class * : Die aufrufende Klasse
** Object *: Das Objekt
** Msg     : Die Message
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** METHOD: Das neue Objekt oder NULL
*/

METHOD MsgBrowser_Clear(Class *cl, Object *obj, Msg msg)
{
  struct MsgBrowser_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Alle Einträge des Listview löschen
  set(Data->LI_MsgBrowser, MUIA_NList_Quiet, MUIV_NList_Quiet_Visual);
  while(DoMethod(Data->LI_MsgBrowser, MUIM_NList_Remove, MUIV_NList_Remove_First));
  set(Data->LI_MsgBrowser, MUIA_NList_Quiet, MUIV_NList_Quiet_None);

  // Dann die Anzahl der Errors/Warnings/Notes auf 0 setzen
  Data->max_errors    = 0;
  Data->max_warnings  = 0;
  Data->max_notes     = 0;

  // Dann das Display updaten
  DoMethod(obj, MUIM_MsgBrowser_Update);

  return(0);
}

/*
** MsgBrowser_Finish
** Methode zum beenden/freigeben des Objektes
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class * : Die aufrufende Klasse
** Object *: Das Objekt
** Msg     : Die Message
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** METHOD: Das neue Objekt oder NULL
*/

METHOD MsgBrowser_Finish(Class *cl, Object *obj, Msg msg)
{
  struct MsgBrowser_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  DoMethod(_app(obj), OM_REMMEMBER, obj);

  return(0);
}

/*
** MsgBrowser_Insert
** Methode um die jeweiligen Messages in das Listview einzusetzen
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class * : Die aufrufende Klasse
** Object *: Das Objekt
** Msg     : Die Message
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** METHOD: Das neue Objekt oder NULL
*/

METHOD MsgBrowser_Insert(Class *cl, Object *obj, struct AmIDEP_msginsert_Message *msg)
{
  struct MsgBrowser_Data *Data = INST_DATA(cl, obj);

  struct AmIDES_message *message = NULL;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(msg->message)
  {
    message = calloc(1, sizeof(struct AmIDES_message));
    if(!message) return(0);

    // Jetzt eine Kopie der Message anlegen
    message->errnr  = msg->message->errnr;
    message->line   = msg->message->line;

    if(msg->message->type)
    {
      message->type = msg->message->type;

      // Schauen was für ein Typ Message das ist
      switch(message->type)
      {
        case AmIDE_MSG_WARNING: { Data->max_warnings++; } break;
        case AmIDE_MSG_ERROR:   { Data->max_errors++;   } break;
        case AmIDE_MSG_NOTE:    { Data->max_notes++;    } break;
      }
    }

    if(msg->message->text)
    {
      message->text   = calloc(strlen(msg->message->text)+1, sizeof(char));
      strcpy(message->text, msg->message->text);

    }else
    {
      message->text = "\033b\0338Error in module! The text sent with the method is a NULL Pointer.";
    }

    if(msg->message->status)
    {
      strcpy(Data->status, msg->message->status);
    }

    // Das BUSY-Objekt bewegen
    DoMethod(Data->BY_MsgBrowser, MUIM_Busy_Move, TAG_DONE);

    DoMethod(Data->LI_MsgBrowser, MUIM_NList_InsertSingleWrap, message, MUIV_List_Insert_Bottom, WRAPCOL1, ALIGN_LEFT, TAG_DONE);

//    DoMethod(Data->LI_MsgBrowser, MUIM_NList_Jump, MUIV_List_Jump_Bottom, TAG_DONE);

    // Die TextObjekte noch updaten wo die max Errors/Warnings/notes drinstehen
    DoMethod(obj, MUIM_MsgBrowser_Update, TAG_DONE);
  }

  return(0);
}

/*
** MsgBrowser_Update
**
** Methode die anhand der MsgBrowser_Data struktur alle Objektes des Fenster aktualisiert
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class * : Die aufrufende Klasse
** Object *: Das Objekt
** Msg     : Die Message
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** METHOD: Das neue Objekt oder NULL
*/

METHOD MsgBrowser_Update(Class *cl, Object *obj, Msg msg)
{
  struct MsgBrowser_Data *Data = INST_DATA(cl, obj);

  char tmp_str[256];

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Die Daten Max-Errors/Warnings/Notes in die Gadgets schreiben
  sprintf(tmp_str, "%4ld", Data->max_errors);
  set(Data->TX_MsgBrowser_Error, MUIA_Text_Contents, tmp_str);
  sprintf(tmp_str, "%4ld", Data->max_warnings);
  set(Data->TX_MsgBrowser_Warning, MUIA_Text_Contents, tmp_str);
  sprintf(tmp_str, "%4ld", Data->max_notes);
  set(Data->TX_MsgBrowser_Note, MUIA_Text_Contents, tmp_str);

  // Den Status im Objekt updaten
  set(Data->TX_MsgBrowser_Status, MUIA_Text_Contents, Data->status);

  return(0);
}
/*
** MsgBrowser_Dispatcher
** Funktion(Dispatcher) die beim auftreten von Methoden-Messages aufgerufen wird
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class *    (a0): Die Klasse des Objektes
** Object *   (a2): Das Objekt
** Msg        (a1): Die Message (die METHODEN_ID)
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** ULONG: Das neue Objekt
*/

SAVEDS ASM ULONG MsgBrowser_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  switch (msg->MethodID)
  {
    case OM_NEW                     : return(MsgBrowser_New         (cl, obj, (APTR)msg));
    case OM_SET                     : return(MsgBrowser_Set         (cl, obj, (APTR)msg));
    case MUIM_MsgBrowser_Finish     : return(MsgBrowser_Finish      (cl, obj, (APTR)msg));
    case MUIM_MsgBrowser_Update     : return(MsgBrowser_Update      (cl, obj, (APTR)msg));
    case MUIM_MsgBrowser_Clear      : return(MsgBrowser_Clear       (cl, obj, (APTR)msg));
    case AmIDEM_msginsert           : return(MsgBrowser_Insert      (cl, obj, (APTR)msg));
  }

  return (DoSuperMethodA(cl, obj, msg));
}

/*
** CreateMsgBrowserClass()
** Funktion zum erstellen der MsgBrowser-Klasse und zuweisen der Dispatcherfunktion
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** struct MUI_CustomClass: Die fertig erzeugte Klasse
*/

struct MUI_CustomClass *CreateMsgBrowserClass()
{
  struct MUI_CustomClass *CL_MsgBrowser;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  CL_MsgBrowser = MUI_CreateCustomClass(NULL,
                                        MUIC_Window, NULL,
                                        sizeof(struct MsgBrowser_Data),
                                        MsgBrowser_Dispatcher
                                       );
  if (CL_MsgBrowser)
  {
     return CL_MsgBrowser;
  }

  DeleteMsgBrowserClass(NULL);
  return NULL;
}

/*
** DeleteMsgBrowserClass()
** Funktion zum löschen/beenden der MsgBrowser Klasse
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** struct MUI_CustomClass *: Pointer auf die zu löschende Klasse
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** keine
*/

void DeleteMsgBrowserClass(struct MUI_CustomClass *CL_MsgBrowser)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if (CL_MsgBrowser)
  {
    MUI_DeleteCustomClass(CL_MsgBrowser);
  }
}
