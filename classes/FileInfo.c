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
#include "FileInfo.h"
#include "ProjectTree.h"
#include "Main.h"

/* Functions */
SAVEDS ASM ULONG FileInfo_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg);

/*
** FileInfo_DestFunc
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


SAVEDS ASM void FileInfo_DestFunc(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct NList_DestructMessage *ndm)
{
/*
  if(((struct FileInfo_Data *)ndm->entry)->entry->entry_filename)
  {
    free(((struct FileInfo_Data *)ndm->entry)->msg_text);
  }
*/
}

const struct Hook FileInfo_DestructHook  = { { NULL, NULL }, (void *)FileInfo_DestFunc, NULL, NULL };


/*
** FileInfo_DisplFunc
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


SAVEDS ASM void FileInfo_DisplFunc(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct NList_DisplayMessage *ndm)
{
  struct Entry_Data *entry = (struct Entry_Data *)ndm->entry;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if (entry)
  {
    ndm->preparses[0] = "\033l";
    ndm->strings[0]   = FilePart(entry->entry_file);
    ndm->preparses[1] = "\033l";
    ndm->strings[1]   = entry->entry_file;

  }
  else
  {
    ndm->preparses[0]  = "\033l\033b";
    ndm->strings[0]    = "Filename";
    ndm->preparses[1]  = "\033l\033b";
    ndm->strings[1]    = "Fullpath";
  }
}

const struct Hook FileInfo_DisplayHook  = { { NULL, NULL }, (void *)FileInfo_DisplFunc, NULL, NULL };


/*
** FileInfo_New
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

METHOD FileInfo_New(Class *cl, Object *obj, struct opSet *msg)
{
  struct FileInfo_Data *Data;
  struct FileInfo_Data Temp;

  // Text für die Cycle-Gadgets anlegen
  static const char *CYA_FileInfo_Text[] =
  {
    "LMB-DC",
    "ContextMenu",
    NULL
  };

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

//   LocalizeStringArray(CYA_Command);
//   LocalizeNewMenu(NM_Mainwin);

  obj = (Object *)DoSuperNew(cl, obj,
    MUIA_Window_Title,  "File information",
    MUIA_Window_ID,     MAKE_ID('F','I','0','1'),
    WindowContents,     VGroup,
      Child, HGroup,
        Child, ColGroup(2),
          GroupFrameT("File information"),

          Child, Label( "Name:" ),
          Child, Temp.BS_FileInfo_FileName = BetterStringObject,
            StringFrame,
            MUIA_String_Contents, "",
            MUIA_Weight, 100,
            MUIA_String_MaxLen, 80,
            MUIA_String_Format, 0,
          End,

          Child, Label( "Filesize:" ),
          Child, Temp.TX_FileInfo_FileSize  = TextObject,
          End,

          Child, Label( "Codesize:" ),
          Child, Temp.TX_FileInfo_CodeSize  = TextObject,
          End,

          Child, Label( "Filedate:" ),
          Child, Temp.TX_FileInfo_Date      = TextObject,
          End,

          Child, Label( "Edit Tool:" ),
          Child, HGroup,
            Child, Temp.PP_FileInfo_EditTool = PopaslObject,
              MUIA_Popstring_String, Temp.BS_FileInfo_EditTool = BetterStringObject,
                StringFrame,
                MUIA_String_Contents, "",
                MUIA_Weight,          80,
                MUIA_String_MaxLen,   256,
                MUIA_String_Format,   MUIV_String_Format_Left,
              End,
              MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
              ASLFR_TitleText  , "Please select Edit Tool...",
            End,
            Child, Temp.CM_FileInfo_Edit  = CheckMark(FALSE),
            Child, Temp.CYA_FileInfo_Edit = CycleObject,
              MUIA_Weight,          10,
              MUIA_Cycle_Entries, CYA_FileInfo_Text,
            End,
          End,

          Child, Label( "View Tool:" ),
          Child, HGroup,
            Child, Temp.PP_FileInfo_ViewTool = PopaslObject,
              MUIA_Popstring_String, Temp.BS_FileInfo_ViewTool = BetterStringObject,
                StringFrame,
                MUIA_String_Contents, "",
                MUIA_Weight,          80,
                MUIA_String_MaxLen,   256,
                MUIA_String_Format,   MUIV_String_Format_Left,
              End,
              MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
              ASLFR_TitleText  , "Please select View Tool...",
            End,
            Child, Temp.CM_FileInfo_View  = CheckMark(FALSE),
            Child, Temp.CYA_FileInfo_View = CycleObject,
              MUIA_Weight,          10,
              MUIA_Cycle_Entries, CYA_FileInfo_Text,
              MUIA_Cycle_Active,  2,
            End,
          End,


        End,
        Child, ColGroup(2),
          Child, Label("Compileable"),
          Child, Temp.CM_FileInfo_Compileable = CheckMark(TRUE),

          Child, Label("Linkable"),
          Child, Temp.CM_FileInfo_Linkable    = CheckMark(TRUE),

          Child, Label("Touchable"),
          Child, Temp.CM_FileInfo_Touchable   = CheckMark(TRUE),

          Child, Label("Dep-Check"),
          Child, Temp.CM_FileInfo_DepCheck    = CheckMark(TRUE),

          Child, Label("Binding"),
          Child, Temp.CM_FileInfo_Binding     = CheckMark(TRUE),
        End,
      End,
      Child, Temp.vgr = VGroup,
        GroupFrameT("Dependency Files"),

        Child, Temp.LV_FileInfo = NListviewObject,
          MUIA_CycleChain, 1,
          MUIA_NListview_NList,               Temp.LI_FileInfo = NListObject,
            MUIA_ObjectID,                    MAKE_ID('N','L','0','2'),
            MUIA_NList_DefaultObjectOnClick,  TRUE,
            MUIA_NList_MultiSelect,           MUIV_NList_MultiSelect_None,
            MUIA_NList_DisplayHook2,          &FileInfo_DisplayHook,
            MUIA_NList_DestructHook2,         &FileInfo_DestructHook,
            MUIA_NList_Format,                "COL=0 BAR, COL=1",
            MUIA_Font,                        MUIV_NList_Font_Fixed,
            MUIA_NList_AutoVisible,           TRUE,
            MUIA_NList_TitleSeparator,        TRUE,
            MUIA_NList_Title,                 TRUE,
            MUIA_NList_MinColSortable,        0,
          End,
//      MUIA_ShortHelp, "The nice multicolumn\ndraggable list\nwith char selection\npossibility :)",
        End,
      End,
    End,
  End;

  if (!obj) { return(NULL); }

  /* Nun Data instanzieren nachdem das Objekt erzeugt wurde */
  Data = INST_DATA(cl, obj);
  *Data = Temp;

//  Data->CreatorName = (STRPTR)GetTagData(MUIA_Mainwin_Creator, NULL, msg->ops_AttrList);

  /*
  ** Verbinden der Bodychunk Objekte mit der NList Klasse
  */

  /*
  DoMethod( Temp.BT_Project_Abort, MUIM_Notify, MUIA_Pressed, FALSE,
            obj, 2, MUIM_Application_ReturnID, BT_PRJ_ABORT);

*/
//   DoMethod(obj, MUIM_Mainwin_Ghostings);
//   DoMethod(obj, MUIM_Mainwin_CYCmdState);

  // Notifies für die Checkmarks setzen, sodaß dies gleich im Listtree geändert wird
  DoMethod( Data->CM_FileInfo_Edit, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            obj, 1, MUIM_FileInfo_Update
          );

  DoMethod( Data->CM_FileInfo_View, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            obj, 1, MUIM_FileInfo_Update
          );

  DoMethod( Data->CM_FileInfo_Compileable, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            obj, 1, MUIM_FileInfo_Update
          );

  DoMethod( Data->CM_FileInfo_Linkable, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            obj, 1, MUIM_FileInfo_Update
          );

  DoMethod( Data->CM_FileInfo_Touchable, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            obj, 1, MUIM_FileInfo_Update
          );

  DoMethod( Data->CM_FileInfo_DepCheck, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            obj, 1, MUIM_FileInfo_Update
          );

  DoMethod( Data->CM_FileInfo_Binding, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            obj, 1, MUIM_FileInfo_Update
          );

  // Notify für die Cycle-Gadget setzen
  DoMethod( Data->CYA_FileInfo_Edit, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            obj, 1, MUIM_FileInfo_Update
          );

  DoMethod( Data->CYA_FileInfo_View, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            obj, 1, MUIM_FileInfo_Update
          );


  // Notify Window
  DoMethod(  obj,  MUIM_Notify,  MUIA_Window_CloseRequest, TRUE,
             obj,  1,            MUIM_FileInfo_Close
          );

   return ((ULONG)obj);
}

/*
** FileInfo_Close
** Methode zum schließen des Fensters des Objektes
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

METHOD FileInfo_Close(Class *cl, Object *obj, Msg msg)
{
  struct FileInfo_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  set(obj, MUIA_Window_Open, FALSE);

  return(0);
}

/*
** FileInfo_Finish
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

METHOD FileInfo_Finish(Class *cl, Object *obj, Msg msg)
{
  struct FileInfo_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  DoMethod(_app(obj), OM_REMMEMBER, obj);

  return 0;
}

/*
** FileInfo_Refresh
** Methode um die Inhalte des Fensters des Objektes neu darzustellen
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

METHOD FileInfo_Refresh(Class *cl, Object *obj, Msg msg)
{
  struct FileInfo_Data *Data = INST_DATA(cl, obj);

  int i;
  struct DateTime   date_time;
  char str_date[256];
  char str_time[256];
  struct Entry_Data *entry;
  struct Entry_Data *tmp;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Nur wenn msg->entry nicht auf NULL zeigt weisen wir entry nen wert zu.
  if(((struct MUIP_FileInfo_Message *)msg)->entry != NULL)
  {
    entry = ((struct MUIP_FileInfo_Message *)msg)->entry;

  }else return(0);

  // Nun lassen wir Data->entry auf entry zeigen
  Data->entry = entry;

  // Alle Variablen setzen
  nnset(Data->BS_FileInfo_FileName,     MUIA_String_Contents, entry->entry_file);
  nnset(Data->BS_FileInfo_EditTool,     MUIA_String_Contents, entry->entry_edittool);
  nnset(Data->BS_FileInfo_ViewTool,     MUIA_String_Contents, entry->entry_viewtool);
  set(Data->TX_FileInfo_FileSize,       MUIA_Text_Contents,   entry->entry_text);
  set(Data->TX_FileInfo_CodeSize,       MUIA_Text_Contents,   entry->entry_code);

  // Die Checkmarks ohne Notify setzen
  nnset(Data->CM_FileInfo_Edit,         MUIA_Selected,        entry->entry_edittool_std);
  nnset(Data->CM_FileInfo_View,         MUIA_Selected,        entry->entry_viewtool_std);
  nnset(Data->CM_FileInfo_Compileable,  MUIA_Selected,        entry->entry_compileable);
  nnset(Data->CM_FileInfo_Linkable,     MUIA_Selected,        entry->entry_linkable);
  nnset(Data->CM_FileInfo_Touchable,    MUIA_Selected,        entry->entry_touchable);
  nnset(Data->CM_FileInfo_DepCheck,     MUIA_Selected,        entry->entry_dep_scan);
  nnset(Data->CM_FileInfo_Binding,      MUIA_Selected,        entry->entry_binding);

  // Die Cycle-Gadget ohne Notify setzen
  nnset(Data->CYA_FileInfo_Edit,        MUIA_Cycle_Active,    entry->entry_edittool_sel);
  nnset(Data->CYA_FileInfo_View,        MUIA_Cycle_Active,    entry->entry_viewtool_sel);

  // Wenn das Entry ein Folder ist disablen wir all änderbaren Sachen
  switch(entry->entry_type)
  {
    case ENTRY_TYPE_FOLDER:
    {
      set(Data->BS_FileInfo_FileName,     MUIA_Disabled, FALSE);
      set(Data->PP_FileInfo_EditTool,     MUIA_Disabled, TRUE);
      set(Data->BS_FileInfo_EditTool,     MUIA_Disabled, TRUE);
      set(Data->CM_FileInfo_Edit,         MUIA_Disabled, TRUE);
      set(Data->CYA_FileInfo_Edit,        MUIA_Disabled, TRUE);
      set(Data->PP_FileInfo_ViewTool,     MUIA_Disabled, TRUE);
      set(Data->BS_FileInfo_ViewTool,     MUIA_Disabled, TRUE);
      set(Data->CM_FileInfo_View,         MUIA_Disabled, TRUE);
      set(Data->CYA_FileInfo_View,        MUIA_Disabled, TRUE);
      set(Data->CM_FileInfo_Compileable,  MUIA_Disabled, TRUE);
      set(Data->CM_FileInfo_Linkable,     MUIA_Disabled, TRUE);
      set(Data->CM_FileInfo_Touchable,    MUIA_Disabled, TRUE);
      set(Data->CM_FileInfo_DepCheck,     MUIA_Disabled, TRUE);
      set(Data->CM_FileInfo_Binding,      MUIA_Disabled, TRUE);
      set(Data->LV_FileInfo,              MUIA_Disabled, TRUE);

      // Ein Folder hat kein Datum
      set(Data->TX_FileInfo_Date,         MUIA_Text_Contents,   "");

      return(0);
    }
    break;

    case ENTRY_TYPE_FILE:
    {
      set(Data->BS_FileInfo_FileName,     MUIA_Disabled, FALSE);
      set(Data->PP_FileInfo_EditTool,     MUIA_Disabled, FALSE);
      set(Data->BS_FileInfo_EditTool,     MUIA_Disabled, FALSE);
      set(Data->CM_FileInfo_Edit,         MUIA_Disabled, FALSE);
      set(Data->CYA_FileInfo_Edit,        MUIA_Disabled, FALSE);
      set(Data->PP_FileInfo_ViewTool,     MUIA_Disabled, FALSE);
      set(Data->BS_FileInfo_ViewTool,     MUIA_Disabled, FALSE);
      set(Data->CM_FileInfo_View,         MUIA_Disabled, FALSE);
      set(Data->CYA_FileInfo_View,        MUIA_Disabled, FALSE);
      set(Data->CM_FileInfo_Compileable,  MUIA_Disabled, FALSE);
      set(Data->CM_FileInfo_Linkable,     MUIA_Disabled, FALSE);
      set(Data->CM_FileInfo_Touchable,    MUIA_Disabled, FALSE);
      set(Data->CM_FileInfo_DepCheck,     MUIA_Disabled, FALSE);
      set(Data->CM_FileInfo_Binding,      MUIA_Disabled, FALSE);
      set(Data->LV_FileInfo,              MUIA_Disabled, FALSE);
    }
    break;

    case ENTRY_TYPE_EXECUTEABLE:
    {
      set(Data->BS_FileInfo_FileName,     MUIA_Disabled, TRUE);
      set(Data->PP_FileInfo_EditTool,     MUIA_Disabled, TRUE);
      set(Data->BS_FileInfo_EditTool,     MUIA_Disabled, TRUE);
      set(Data->CM_FileInfo_Edit,         MUIA_Disabled, TRUE);
      set(Data->CYA_FileInfo_Edit,        MUIA_Disabled, TRUE);
      set(Data->PP_FileInfo_ViewTool,     MUIA_Disabled, TRUE);
      set(Data->BS_FileInfo_ViewTool,     MUIA_Disabled, TRUE);
      set(Data->CM_FileInfo_View,         MUIA_Disabled, TRUE);
      set(Data->CYA_FileInfo_View,        MUIA_Disabled, TRUE);
      set(Data->CM_FileInfo_Compileable,  MUIA_Disabled, TRUE);
      set(Data->CM_FileInfo_Linkable,     MUIA_Disabled, TRUE);
      set(Data->CM_FileInfo_Touchable,    MUIA_Disabled, TRUE);
      set(Data->CM_FileInfo_DepCheck,     MUIA_Disabled, TRUE);
      set(Data->CM_FileInfo_Binding,      MUIA_Disabled, TRUE);
      set(Data->LV_FileInfo,              MUIA_Disabled, TRUE);
    }
    break;
  }

  // Schauen ob im View/Edit-Tool das Standard ausgewählt wurde, und wenn ja dann den String disablen
  if(entry->entry_edittool_std)
  {
    nnset(Data->BS_FileInfo_EditTool, MUIA_String_Contents, "Standard Tool");
    set(Data->PP_FileInfo_EditTool,   MUIA_Disabled,  TRUE);
    set(Data->BS_FileInfo_EditTool,   MUIA_Disabled,  TRUE);
  }

  if(entry->entry_viewtool_std)
  {
    nnset(Data->BS_FileInfo_ViewTool, MUIA_String_Contents, "Standard Tool");
    set(Data->PP_FileInfo_ViewTool,   MUIA_Disabled,  TRUE);
    set(Data->BS_FileInfo_ViewTool,   MUIA_Disabled,  TRUE);
  }

  // Nun aktualisieren wir das Dateidatum
  get_file_date(entry);

  // Nun das Datum berechnen und in das TextObject schreiben
  date_time.dat_Stamp   = entry->entry_date;
//  date_time.dat_Format  = FORMAT_DEF;
//  date_time.dat_Flags   = DTF_SUBST;
  date_time.dat_StrDay  = NULL;
  date_time.dat_StrDate = str_date;
  date_time.dat_StrTime = str_time;
  DateToStr(&date_time);

  strcpy(Data->datestring, str_date);
  strcat(Data->datestring, " ");
  strcat(Data->datestring, str_time);

  set(Data->TX_FileInfo_Date,     MUIA_Text_Contents,   Data->datestring);

  // Wenn das File ein Executeable ist brechen wir hier schon ab
  if(entry->entry_type == ENTRY_TYPE_EXECUTEABLE) return(0);

  // Nun bauen wir das neue Listview zusammen
  if(DoMethod(Data->vgr, MUIM_Group_InitChange))
  {
    // Erst das Listview aus dem Fenster entfernen
    DoMethod(Data->vgr, OM_REMMEMBER, Data->LV_FileInfo);

    Data->LI_FileInfo = entry->entry_dep;

    Data->LV_FileInfo = NListviewObject,
      MUIA_CycleChain, 1,
      MUIA_NListview_NList,             Data->LI_FileInfo = entry->entry_dep,
      MUIA_ObjectID,                    MAKE_ID('N','L','0','2'),
    End;

    set(Data->LV_FileInfo, MUIA_NList_DisplayHook2,   &FileInfo_DisplayHook);
    set(Data->LV_FileInfo, MUIA_NList_DestructHook2,  &FileInfo_DestructHook);

    DoMethod(Data->vgr, OM_ADDMEMBER, Data->LV_FileInfo);

    DoMethod(Data->vgr, MUIM_Group_ExitChange);
  }

  return(0);
}

/*
** FileInfo_Update
**
** Methode die aus den Daten im FileInfo Fenster die Entry-Struktur neu setzt
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

METHOD FileInfo_Update(Class *cl, Object *obj, Msg msg)
{
  struct FileInfo_Data *Data = INST_DATA(cl, obj);

  // Aktuellen Eintrag holen
  struct Entry_Data *entry = Data->entry;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(entry)
  {
    BOOL  old_vt_std = entry->entry_viewtool_std;
    BOOL  old_et_std = entry->entry_edittool_std;

    // Die Attribute der Checkmark-Gadgets in die Variablen holen
    entry->entry_edittool_std = (BOOL)xget(Data->CM_FileInfo_Edit,        MUIA_Selected);
    entry->entry_viewtool_std = (BOOL)xget(Data->CM_FileInfo_View,        MUIA_Selected);
    entry->entry_compileable  = (BOOL)xget(Data->CM_FileInfo_Compileable, MUIA_Selected);
    entry->entry_linkable     = (BOOL)xget(Data->CM_FileInfo_Linkable,    MUIA_Selected);
    entry->entry_touchable    = (BOOL)xget(Data->CM_FileInfo_Touchable,   MUIA_Selected);
    entry->entry_dep_scan     = (BOOL)xget(Data->CM_FileInfo_DepCheck,    MUIA_Selected);
    entry->entry_binding      = (BOOL)xget(Data->CM_FileInfo_Binding,     MUIA_Selected);

    // Nur wenn sich nichts an den CheckMarks geändert hat holen wir auch die Daten aus den BS
    if(old_vt_std == entry->entry_viewtool_std && old_et_std == entry->entry_edittool_std)
    {
      entry->entry_viewtool   = (STRPTR)xget(Data->BS_FileInfo_ViewTool, MUIA_String_Contents);
      entry->entry_edittool   = (STRPTR)xget(Data->BS_FileInfo_EditTool, MUIA_String_Contents);
    }

    // Die Cycle-Gadget auswerten
    {
      int old_vt_sel = entry->entry_viewtool_sel;
      int old_et_sel = entry->entry_edittool_sel;

      entry->entry_edittool_sel = (int)xget(Data->CYA_FileInfo_Edit,    MUIA_Cycle_Active);
      entry->entry_viewtool_sel = (int)xget(Data->CYA_FileInfo_View,     MUIA_Cycle_Active);

      // Die Cycle-Gadget dürfen nicht beide auf 0 stehen
      if(entry->entry_viewtool_sel == 0 && entry->entry_edittool_sel == 0)
      {
        if(old_vt_sel != entry->entry_viewtool_sel) entry->entry_edittool_sel++;
        else entry->entry_viewtool_sel++;
      }
    }

    // Nun ein Refresh der FileInfo machen
    DoMethod(obj, MUIM_FileInfo_Refresh, entry);
  }

  return(0);
}

/*
** FileInfo_Dispatcher
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

SAVEDS ASM ULONG FileInfo_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  switch (msg->MethodID)
  {
    case OM_NEW                       : return(FileInfo_New           (cl, obj, (APTR)msg));
    case MUIM_FileInfo_Finish         : return(FileInfo_Finish        (cl, obj, (APTR)msg));
    case MUIM_FileInfo_Close          : return(FileInfo_Close         (cl, obj, (APTR)msg));
    case MUIM_FileInfo_Refresh        : return(FileInfo_Refresh       (cl, obj, (APTR)msg));
    case MUIM_FileInfo_Update         : return(FileInfo_Update        (cl, obj, (APTR)msg));
  }

  return (DoSuperMethodA(cl, obj, msg));
}

/*
** CreateFileInfoClass()
** Funktion zum erstellen der FileInfo Klasse und zuweisen der Dispatcherfunktion
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** struct MUI_CustomClass: Die fertig erzeugte Klasse
*/

struct MUI_CustomClass *CreateFileInfoClass()
{
  struct MUI_CustomClass *CL_FileInfo;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  CL_FileInfo = MUI_CreateCustomClass(  NULL,
                                        MUIC_Window, NULL,
                                        sizeof(struct FileInfo_Data),
                                        FileInfo_Dispatcher
                                       );
  if (CL_FileInfo)
  {
     return CL_FileInfo;
  }

  DeleteFileInfoClass(NULL);
  return NULL;
}

/*
** DeleteFileInfoClass()
** Funktion zum löschen/beenden der FileInfo Klasse
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** struct MUI_CustomClass *: Pointer auf die zu löschende Klasse
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** keine
*/

void DeleteFileInfoClass(struct MUI_CustomClass *CL_FileInfo)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if (CL_FileInfo)
  {
    MUI_DeleteCustomClass(CL_FileInfo);
  }
}
