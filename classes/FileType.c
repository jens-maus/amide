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
#include "FileType.h"
#include "FileTypeTree.h"
#include "Main.h"

/* Functions */
SAVEDS ASM ULONG FileType_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg);

/* Structures */
static struct MUI_CustomClass *CL_FileTypeTree;

/*
** FileType_New
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

METHOD FileType_New(Class *cl, Object *obj, struct opSet *msg)
{
  struct FileType_Data *Data;
  struct FileType_Data Temp;
  STRPTR tmp_str;

  enum{ MN_SETTINGS=1, MN_LOADDEFAULT, MN_SAVEDEFAULT
      };

  // Text für die Cycle-Gadgets anlegen
  static const char *CYA_FileType_Text[] =
  {
    "LMB-DC",
    "ContextMenu",
    NULL
  };

  // Menueinträge einlegen
  struct NewMenu NM_FileType[] =
  {
    /* Type     Label                       Key   Flags             MX  UserData */
    { NM_TITLE, "Settings",                 0,    0,                0,  (APTR)MN_SETTINGS,    },
    { NM_ITEM,  "Load default filetypes",   "D",  0,                0,  (APTR)MN_LOADDEFAULT  },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "Save as default...",       "S",  0,                0,  (APTR)MN_SAVEDEFAULT  },

    { NM_END, NULL,                         0,    0,                0,  (APTR)NULL,           }
  };

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

//   LocalizeStringArray(CYA_Command);
//   LocalizeNewMenu(NM_Mainwin);

  // Nun erzeugen wir das Objekt
  obj = (Object *)DoSuperNew(cl, obj,
    MUIA_Window_Title,  "Project FileTypes",
    MUIA_Window_ID,     MAKE_ID('F', 'I', 'T', 'Y'),
    MUIA_Window_Menustrip, Temp.MN_FileType = MUI_MakeObject(MUIO_MenustripNM, NM_FileType, 0),
    WindowContents,     VGroup,
      Child, VGroup,
        GroupFrameT("Filetypes"),

        Child, NListviewObject,
          MUIA_CycleChain,            TRUE,
          MUIA_Listview_Input,        TRUE,
          MUIA_Listview_DragType,     MUIV_Listview_DragType_Immediate,
          MUIA_Listview_MultiSelect,  MUIV_Listview_MultiSelect_None,
          MUIA_NListview_NList,       Temp.LT_FileType = FileType_TreeObject,
          End,
        End,

        Child, HGroup,
          Child, Temp.BT_FileType_Add_FileType  = KeyButton("Add filetype",  "f"),
          Child, Temp.BT_FileType_Add_Pattern   = KeyButton("Add pattern",   "p"),
          Child, Temp.BT_FileType_Delete        = KeyButton("Delete",        "D"),
        End,
      End,
      Child, HGroup,
        Child, ColGroup(2),
          GroupFrameT("Filetype information"),

          Child, Label("Name:"),
          Child, Temp.BS_FileType_Name = BetterStringObject,
            StringFrame,
            MUIA_String_Contents, "",
            MUIA_Weight,          100,
            MUIA_String_MaxLen,   256,
            MUIA_String_Format,   MUIV_String_Format_Left,
          End,

          Child, Label("Pattern:"),
          Child, Temp.BS_FileType_Pattern = BetterStringObject,
            StringFrame,
            MUIA_String_Contents, "",
            MUIA_Weight,          100,
            MUIA_String_MaxLen,   256,
            MUIA_String_Format,   MUIV_String_Format_Left,
          End,

          Child, Label("Edit Tool:"),
          Child, HGroup,
            Child, Temp.PP_FileType_EditTool = PopaslObject,
              MUIA_Popstring_String, Temp.BS_FileType_EditTool = BetterStringObject,
                StringFrame,
                MUIA_String_Contents, "",
                MUIA_Weight,          80,
                MUIA_String_MaxLen,   256,
                MUIA_String_Format,   MUIV_String_Format_Left,
              End,
              MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
              ASLFR_TitleText  , "Please select Edit Tool...",
            End,
            Child, Temp.CM_FileType_Edit  = CheckMark(FALSE),
            Child, Temp.CYA_FileType_Edit = CycleObject,
              MUIA_Weight,          10,
              MUIA_Cycle_Entries, CYA_FileType_Text,
            End,
          End,

          Child, Label("View Tool:"),
          Child, HGroup,
            Child, Temp.PP_FileType_ViewTool = PopaslObject,
              MUIA_Popstring_String, Temp.BS_FileType_ViewTool = BetterStringObject,
                StringFrame,
                MUIA_String_Contents, "",
                MUIA_Weight,          80,
                MUIA_String_MaxLen,   256,
                MUIA_String_Format,   MUIV_String_Format_Left,
              End,
              MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
              ASLFR_TitleText  , "Please select View Tool...",
            End,
            Child, Temp.CM_FileType_View  = CheckMark(FALSE),
            Child, Temp.CYA_FileType_View = CycleObject,
              MUIA_Weight,          10,
              MUIA_Cycle_Entries, CYA_FileType_Text,
              MUIA_Cycle_Active,  2,
            End,
          End,

        End,

        Child, ColGroup(2),
          Child, Label("Compileable"),
          Child, Temp.CM_FileType_Compileable = CheckMark(TRUE),

          Child, Label("Linkable"),
          Child, Temp.CM_FileType_Linkable = CheckMark(TRUE),

          Child, Label("Touchable"),
          Child, Temp.CM_FileType_Touchable = CheckMark(TRUE),

          Child, Label("Dep-Check"),
          Child, Temp.CM_FileType_DepCheck = CheckMark(TRUE),

          Child, Label("Autobinding"),
          Child, Temp.CM_FileType_AutoBind = CheckMark(TRUE),
        End,
      End,
    End,
  End;

  if (!obj) { return(NULL); }

  /* Nun Data instanzieren nachdem das Objekt erzeugt wurde */
  Data = INST_DATA(cl, obj);
  *Data = Temp;

  // Nun die Notifys für die Cursortasten ranhängen
  DoMethod( obj, MUIM_Notify, MUIA_Window_InputEvent, "-repeat down",
            Data->LT_FileType, 3, MUIM_Set, MUIA_List_Active, MUIV_List_Active_Down);

  DoMethod( obj, MUIM_Notify, MUIA_Window_InputEvent, "-repeat up",
            Data->LT_FileType, 3, MUIM_Set, MUIA_List_Active, MUIV_List_Active_Up);

  // Dann das Notify setzen der bei aktivsetzung eines Inhaltes vom Listtree die Strings neu setzt
  DoMethod( Data->LT_FileType, MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime,
            obj, 1, MUIM_FileType_Refresh
          );

  // Notifies für die Strings setzen sodaß dies gleich im Listtree geändert wird
  DoMethod( Data->BS_FileType_Name, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
            obj, 1, MUIM_FileType_Update
          );

  DoMethod( Data->BS_FileType_Pattern, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
            obj, 1, MUIM_FileType_Update
          );

  DoMethod( Data->BS_FileType_ViewTool, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
            obj, 1, MUIM_FileType_Update
          );

  // Notifies für die Checkmarks setzen, sodaß dies gleich im Listtree geändert wird
  DoMethod( Data->CM_FileType_View, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            obj, 1, MUIM_FileType_Update
          );

  DoMethod( Data->CM_FileType_Edit, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            obj, 1, MUIM_FileType_Update
          );

  DoMethod( Data->CM_FileType_Compileable, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            obj, 1, MUIM_FileType_Update
          );

  DoMethod( Data->CM_FileType_Linkable, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            obj, 1, MUIM_FileType_Update
          );

  DoMethod( Data->CM_FileType_Touchable, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            obj, 1, MUIM_FileType_Update
          );

  DoMethod( Data->CM_FileType_DepCheck, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            obj, 1, MUIM_FileType_Update
          );

  DoMethod( Data->CM_FileType_AutoBind, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            obj, 1, MUIM_FileType_Update
          );

  // Notifies für die Cycle-Gadgets setzen, sodaß dies gleich im Listtree geändert wird
  DoMethod( Data->CYA_FileType_View, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            obj, 1, MUIM_FileType_Update
          );

  DoMethod( Data->CYA_FileType_Edit, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            obj, 1, MUIM_FileType_Update
          );


  // Notifys an die Buttons hängen
  DoMethod( Data->BT_FileType_Add_FileType, MUIM_Notify, MUIA_Pressed, FALSE,
            Data->LT_FileType, 2, MUIM_FileTypeTree_Add_Folder, 0
          );

  DoMethod( Data->BT_FileType_Add_Pattern, MUIM_Notify, MUIA_Pressed, FALSE,
            Data->LT_FileType, 8, MUIM_FileTypeTree_Add_Type, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0
          );

  DoMethod( Data->BT_FileType_Delete, MUIM_Notify, MUIA_Pressed, FALSE,
            Data->LT_FileType, 4, MUIM_NListtree_Remove, MUIV_NListtree_Remove_ListNode_Root, MUIV_NListtree_Remove_TreeNode_Active, 0
          );

  // Notifies für die Menuauswahl
  DoMethod(FINDOBJ(Data->MN_FileType, MN_LOADDEFAULT),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj, 2,  MUIM_FileType_Load, DEFAULTCFGFILE
          );

  DoMethod(FINDOBJ(Data->MN_FileType, MN_SAVEDEFAULT),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj, 3,  MUIM_FileType_Save, DEFAULTCFGFILE, TRUE
          );

  // Notify Window
  DoMethod(obj,  MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
           obj,  3,  MUIM_Set, MUIA_Window_Open, FALSE
          );

  return ((ULONG)obj);
}

/*
** FileType_Find
**
** Methode die in der FileTypen liste nach dem übergebenen Type sucht und dann einen Pointer auf seinen
** Namen zurückliefert
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

METHOD FileType_Find(Class *cl, Object *obj, struct MUIP_FileType_Find_Message *msg)
{
  struct FileType_Data *Data = INST_DATA(cl, obj);

  struct MUI_NListtree_TreeNode *tn;
  struct FileType *type;
  int i;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  for(i=0;;i++)
  {
    // Den Eintrag holen
    tn = (struct MUI_NListtree_TreeNode *)DoMethod(Data->LT_FileType, MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Root, i, TAG_DONE);
    if(!tn) break;

    type = (struct FileType *)tn->tn_User;

    // Ein Folder ist ein Filetyp
    if(type->folder)
    {
      if(strcmp(msg->name, type->name) == 0)
      {
        return((ULONG)type->name);
      }
    }
  }

  return(0);
}

/*
** FileType_Finish
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

METHOD FileType_Finish(Class *cl, Object *obj, Msg msg)
{
  struct FileType_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Entfernen des Objektes aus der Applikationsliste
  DoMethod(_app(obj), OM_REMMEMBER, obj);

  return(0);
}

/*
** FileType_Identify
**
** Methode der ein string mit dem Dateinamen übergeben wird und anhand dieses Namens ein
** patterncheck stattfindet und der FileTyp zurückgeliefert wird
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

METHOD FileType_Identify(Class *cl, Object *obj, struct MUIP_FileType_Identify_Message *msg)
{
  struct FileType_Data *Data = INST_DATA(cl, obj);
  struct MUI_NListtree_TreeNode *tn, *tn2;
  struct FileType *type;
  int i;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  for(i=0;;i++)
  {
    // Den Eintrag holen
    tn = (struct MUI_NListtree_TreeNode *)DoMethod(Data->LT_FileType, MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Root, i, TAG_DONE);
    if(!tn) break;

    type = (struct FileType *)tn->tn_User;

    if(!type->folder)
    {
      // Vergleichen des Patterns mit dem string in der message
      if(MatchNoCase(FilePart(msg->name), type->pattern) == TRUE)
      {
        // Nun die Parent-Treenode holen
        tn2 = (struct MUI_NListtree_TreeNode *)DoMethod(Data->LT_FileType, MUIM_NListtree_GetEntry, tn, MUIV_NListtree_GetEntry_Position_Parent, 0);
        if(!tn2) break;

        // Dann setzen wir den type->parent pointer neu
        type->parent = tn2;

        return((ULONG)type);
      }
    }
  }

  return(0);
}

/*
** FileType_Load
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

METHOD FileType_Load(Class *cl, Object *obj, struct MUIP_FileType_Load_Message *msg)
{
  struct FileType_Data *Data = INST_DATA(cl, obj);
  BPTR fh;

  enum { MODE_FILETYPE=1, MODE_PATTERN };

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Erstmal das Listtree leer machen
  DoMethod(Data->LT_FileType, MUIM_NListtree_Remove, MUIV_NListtree_Remove_ListNode_Root, MUIV_NListtree_Remove_TreeNode_All, 0);

  // Nun setzen wir das Listtree auf QUIET für das adden von vielen Dateien
  set(Data->LT_FileType, MUIA_NListtree_Quiet, TRUE);

  if(fh = Open(msg->filename, MODE_OLDFILE))
  {
    char tmp_str[512];
    int  mode = 0;

    while(FGets(fh, tmp_str, 512))
    {
      static STRPTR filetype_name;
      static STRPTR filetype_pattern;
      static STRPTR filetype_edittool;
      static int    filetype_edittool_std;
      static int    filetype_edittool_sel;
      static STRPTR filetype_viewtool;
      static int    filetype_viewtool_std;
      static int    filetype_viewtool_sel;
      static int    filetype_compileable;
      static int    filetype_linkable;
      static int    filetype_touchable;
      static int    filetype_scan_dep;
      static int    filetype_auto_binding;

      // Dann überschreiben wir das Returnzeichen
      tmp_str[strlen(tmp_str)-1]='\0';

      // Je nachdem welcher Mode aktiv ist bearbeiten wir den String anders
      switch(mode)
      {
        // Die Projekteinstellungen
        case MODE_FILETYPE:
        {
          STRPTR tmp;

          if(tmp = get_string(tmp_str, "Name=", FALSE))
          {
            filetype_name = tmp;
            DoMethod(Data->LT_FileType, MUIM_FileTypeTree_Add_Folder, filetype_name, 0);
            mode = 0;
          }
        }
        break;

        case MODE_PATTERN:
        {
          STRPTR tmp;

          if(tmp = get_string(tmp_str, "Name=",         TRUE))  filetype_name         = tmp;
          if(tmp = get_string(tmp_str, "Pattern=",      TRUE))  filetype_pattern      = tmp;
          if(tmp = get_string(tmp_str, "EditTool=",     TRUE))  filetype_edittool     = tmp;
          if(tmp = get_string(tmp_str, "EditTool_std=", FALSE)) filetype_edittool_std = atoi(tmp);
          if(tmp = get_string(tmp_str, "EditTool_sel=", FALSE)) filetype_edittool_sel = atoi(tmp);
          if(tmp = get_string(tmp_str, "ViewTool=",     TRUE))  filetype_viewtool     = tmp;
          if(tmp = get_string(tmp_str, "ViewTool_std=", FALSE)) filetype_viewtool_std = atoi(tmp);
          if(tmp = get_string(tmp_str, "ViewTool_sel=", FALSE)) filetype_viewtool_sel = atoi(tmp);
          if(tmp = get_string(tmp_str, "Compileable=",  FALSE)) filetype_compileable  = atoi(tmp);
          if(tmp = get_string(tmp_str, "Linkable=",     FALSE)) filetype_linkable     = atoi(tmp);
          if(tmp = get_string(tmp_str, "Touchable=",    FALSE)) filetype_touchable    = atoi(tmp);
          if(tmp = get_string(tmp_str, "DepScan=",      FALSE)) filetype_scan_dep     = atoi(tmp);
          if(tmp = get_string(tmp_str, "AutoBinding=",  FALSE)) filetype_auto_binding = atoi(tmp);

          // Wenn alles bereits eingelesen wurde können wir den Type in das Listtree adden
          if( filetype_name               &&
              filetype_pattern            &&
              filetype_edittool           &&
              filetype_edittool_std >= 0  &&
              filetype_edittool_sel >= 0  &&
              filetype_viewtool           &&
              filetype_viewtool_std >= 0  &&
              filetype_viewtool_sel >= 0  &&
              filetype_compileable  >= 0  &&
              filetype_linkable     >= 0  &&
              filetype_touchable    >= 0  &&
              filetype_scan_dep     >= 0  &&
              filetype_auto_binding >= 0  )
          {
            DoMethod(Data->LT_FileType, MUIM_FileTypeTree_Add_Type, filetype_name,
                                                                    filetype_pattern,
                                                                    filetype_edittool,
                                                                    filetype_edittool_std,
                                                                    filetype_edittool_sel,
                                                                    filetype_viewtool,
                                                                    filetype_viewtool_std,
                                                                    filetype_viewtool_sel,
                                                                    filetype_compileable,
                                                                    filetype_linkable,
                                                                    filetype_touchable,
                                                                    filetype_scan_dep,
                                                                    filetype_auto_binding,
                                                                    0
                    );

            mode = 0;
          }

        }
        break;
      }

      // Nun stellen wir die Modes ein
      if(strstr(tmp_str, "[Filetype]")) mode = MODE_FILETYPE;
      if(strstr(tmp_str, "[Pattern]"))
      {
        filetype_name         = NULL;
        filetype_pattern      = NULL;
        filetype_edittool     = NULL;
        filetype_edittool_std = -1;
        filetype_edittool_sel = -1;
        filetype_compileable  = -1;
        filetype_viewtool     = NULL;
        filetype_viewtool_std = -1;
        filetype_viewtool_sel = -1;
        filetype_compileable  = -1;
        filetype_linkable     = -1;
        filetype_touchable    = -1;
        filetype_scan_dep     = -1;
        filetype_auto_binding = -1;
        mode = MODE_PATTERN;
      }
    }
    Close(fh);

    // Nun setzen wir das Listtree wieder auf NORMAL
    set(Data->LT_FileType, MUIA_NListtree_Quiet, FALSE);

  }

  return(0);
}

/*
** FileType_Refresh
**
** Methode die die Inhalte aus dem Eintrag des Listtree holt und in den Strings/CheckMarks darstellt
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

METHOD FileType_Refresh(Class *cl, Object *obj, Msg msg)
{
  struct FileType_Data *Data = INST_DATA(cl, obj);

  // Aktuellen Eintrag holen
  struct FileType *type = (struct FileType *)DoMethod(Data->LT_FileType, MUIM_FileTypeTree_Get, MUIV_FileTypeTree_Active);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(type)
  {
    // Die Attribute in die Objekte setzen (ohne MUIM_Notify)
    nnset(Data->BS_FileType_Name,         MUIA_String_Contents, type->name);
    nnset(Data->BS_FileType_Pattern,      MUIA_String_Contents, type->pattern);
    nnset(Data->BS_FileType_ViewTool,     MUIA_String_Contents, type->view_tool);
    nnset(Data->CM_FileType_View,         MUIA_Selected,        type->view_tool_standard);
    nnset(Data->CYA_FileType_View,        MUIA_Cycle_Active,    type->view_tool_sel);
    nnset(Data->BS_FileType_EditTool,     MUIA_String_Contents, type->edit_tool);
    nnset(Data->CM_FileType_Edit,         MUIA_Selected,        type->edit_tool_standard);
    nnset(Data->CYA_FileType_Edit,        MUIA_Cycle_Active,    type->edit_tool_sel);
    nnset(Data->CM_FileType_Compileable,  MUIA_Selected,        type->compileable);
    nnset(Data->CM_FileType_Linkable,     MUIA_Selected,        type->linkable);
    nnset(Data->CM_FileType_Touchable,    MUIA_Selected,        type->touchable);
    nnset(Data->CM_FileType_DepCheck,     MUIA_Selected,        type->scan_dep);
    nnset(Data->CM_FileType_AutoBind,     MUIA_Selected,        type->auto_binding);

    // Die einzelnen Objekte disablen, falls das Entry ein Folder ist
    set(Data->BS_FileType_Pattern,      MUIA_Disabled, type->folder);
    set(Data->PP_FileType_EditTool,     MUIA_Disabled, type->folder);
    set(Data->BS_FileType_EditTool,     MUIA_Disabled, type->folder);
    set(Data->CM_FileType_Edit,         MUIA_Disabled, type->folder);
    set(Data->CYA_FileType_Edit,        MUIA_Disabled, type->folder);
    set(Data->PP_FileType_ViewTool,     MUIA_Disabled, type->folder);
    set(Data->BS_FileType_ViewTool,     MUIA_Disabled, type->folder);
    set(Data->CM_FileType_View,         MUIA_Disabled, type->folder);
    set(Data->CYA_FileType_View,        MUIA_Disabled, type->folder);
    set(Data->CM_FileType_Compileable,  MUIA_Disabled, type->folder);
    set(Data->CM_FileType_Linkable,     MUIA_Disabled, type->folder);
    set(Data->CM_FileType_Touchable,    MUIA_Disabled, type->folder);
    set(Data->CM_FileType_DepCheck,     MUIA_Disabled, type->folder);
    set(Data->CM_FileType_AutoBind,     MUIA_Disabled, type->folder);

    // Schauen ob im View/Edit-Tool das Standard ausgewählt wurde, und wenn ja dann den String disablen
    if(type->view_tool_standard)
    {
      nnset(Data->BS_FileType_ViewTool, MUIA_String_Contents, "Standard Tool");
      set(Data->PP_FileType_ViewTool,   MUIA_Disabled,  TRUE);
      set(Data->BS_FileType_ViewTool,   MUIA_Disabled,  TRUE);
    }

    if(type->edit_tool_standard)
    {
      nnset(Data->BS_FileType_EditTool, MUIA_String_Contents, "Standard Tool");
      set(Data->PP_FileType_EditTool,   MUIA_Disabled,  TRUE);
      set(Data->BS_FileType_EditTool,   MUIA_Disabled,  TRUE);
    }
  }

  return(0);
}

/*
** FileType_Save
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

METHOD FileType_Save(Class *cl, Object *obj, struct MUIP_FileType_Save_Message *msg)
{
  struct  FileType_Data *Data = INST_DATA(cl, obj);
  BPTR    fh;
  ULONG   mode = MODE_READWRITE;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Wenn die Methode mit Overwrite aufgerufen wird öffnen wir auch das File so
  if(msg->overwrite) mode = MODE_NEWFILE;

  if(fh = Open(msg->filename, mode))
  {
    int i;

    // Ans Dateiende springen
    Seek(fh, 0, OFFSET_END);

    // Dann schreiben wir alle Folders aus dem Listtree
    for(i=0;;i++)
    {
      struct FileType *type = (struct FileType *)DoMethod(Data->LT_FileType, MUIM_FileTypeTree_Get, i);
      if(!type) break;

      if(type->folder)
      {
        FPrintf(fh, "\n");
        FPrintf(fh, "[Filetype]\n");
        FPrintf(fh, "Name=%s\n", type->name);

      }else
      {
        FPrintf(fh, "[Pattern]\n");
        FPrintf(fh, "Name=%s\n",          type->name);
        FPrintf(fh, "Pattern=%s\n",       type->pattern);
        FPrintf(fh, "EditTool=%s\n",      type->edit_tool);
        FPrintf(fh, "EditTool_std=%ld\n", type->edit_tool_standard);
        FPrintf(fh, "EditTool_sel=%ld\n", type->edit_tool_sel);
        FPrintf(fh, "ViewTool=%s\n",      type->view_tool);
        FPrintf(fh, "ViewTool_std=%ld\n", type->view_tool_standard);
        FPrintf(fh, "ViewTool_sel=%ld\n", type->view_tool_sel);
        FPrintf(fh, "Compileable=%ld\n",  type->compileable);
        FPrintf(fh, "Linkable=%ld\n",     type->linkable);
        FPrintf(fh, "Touchable=%ld\n",    type->touchable);
        FPrintf(fh, "DepScan=%ld\n",      type->scan_dep);
        FPrintf(fh, "AutoBinding=%ld\n",  type->auto_binding);
      }
    }
    Close(fh);
  }

  return(0);
}

/*
** FileType_Update
**
** Methode die den FileType Eintrag anhand der Angaben ändert
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

METHOD FileType_Update(Class *cl, Object *obj, Msg msg)
{
  struct FileType_Data *Data = INST_DATA(cl, obj);

  // Aktuellen Eintrag holen
  struct FileType *type = (struct FileType *)DoMethod(Data->LT_FileType, MUIM_FileTypeTree_Get, MUIV_FileTypeTree_Active);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(type)
  {
    BOOL  old_vt_std = type->view_tool_standard;
    BOOL  old_et_std = type->edit_tool_standard;

    // Die Attribute der Checkmark-Gadgets in die Variablen holen
    type->view_tool_standard  = (BOOL)xget(Data->CM_FileType_View,        MUIA_Selected);
    type->edit_tool_standard  = (BOOL)xget(Data->CM_FileType_Edit,        MUIA_Selected);
    type->compileable         = (BOOL)xget(Data->CM_FileType_Compileable, MUIA_Selected);
    type->linkable            = (BOOL)xget(Data->CM_FileType_Linkable,    MUIA_Selected);
    type->touchable           = (BOOL)xget(Data->CM_FileType_Touchable,   MUIA_Selected);
    type->scan_dep            = (BOOL)xget(Data->CM_FileType_DepCheck,    MUIA_Selected);
    type->auto_binding        = (BOOL)xget(Data->CM_FileType_AutoBind,    MUIA_Selected);

    // Die Attribute der String-Gadget in die Variablen kopieren
    strcpy(type->name,      (STRPTR)xget(Data->BS_FileType_Name,     MUIA_String_Contents));
    strcpy(type->pattern,   (STRPTR)xget(Data->BS_FileType_Pattern,  MUIA_String_Contents));

    // Wenn sich das CheckMark-Gadget der Standard für View/Edittool geändert hat ändern wir nicht das teil
    if(old_vt_std == FALSE && type->view_tool_standard == TRUE)
    {
      strcpy(type->view_tool, (STRPTR)xget(Data->BS_FileType_ViewTool, MUIA_String_Contents));
    }

    if(old_et_std == FALSE && type->edit_tool_standard == TRUE)
    {
      strcpy(type->edit_tool, (STRPTR)xget(Data->BS_FileType_EditTool, MUIA_String_Contents));
    }

    // Die Cycle-Gadget auswerten
    {
      int old_vt_sel = type->view_tool_sel;
      int old_et_sel = type->edit_tool_sel;

      type->view_tool_sel       = (int)xget(Data->CYA_FileType_View,        MUIA_Cycle_Active);
      type->edit_tool_sel       = (int)xget(Data->CYA_FileType_Edit,        MUIA_Cycle_Active);

      // Die Cycle-Gadget dürfen nicht beide auf 0 stehen
      if(type->view_tool_sel == 0 && type->edit_tool_sel == 0)
      {
        if(old_vt_sel != type->view_tool_sel) type->edit_tool_sel++;
        else type->view_tool_sel++;
      }
    }

    // Nun ein Refresh der FileType-Infos machen
    DoMethod(obj, MUIM_FileType_Refresh);

    // Nun das Listtree updaten
    DoMethod(Data->LT_FileType, MUIM_List_Redraw, MUIV_List_Redraw_Active);
  }

  return(0);
}


/*
** FileType_Dispatcher
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

SAVEDS ASM ULONG FileType_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  switch (msg->MethodID)
  {
    case OM_NEW                 : return(FileType_New     (cl, obj, (APTR)msg));
    case MUIM_FileType_Finish   : return(FileType_Finish  (cl, obj, (APTR)msg));
    case MUIM_FileType_Refresh  : return(FileType_Refresh (cl, obj, (APTR)msg));
    case MUIM_FileType_Update   : return(FileType_Update  (cl, obj, (APTR)msg));
    case MUIM_FileType_Identify : return(FileType_Identify(cl, obj, (APTR)msg));
    case MUIM_FileType_Load     : return(FileType_Load    (cl, obj, (APTR)msg));
    case MUIM_FileType_Save     : return(FileType_Save    (cl, obj, (APTR)msg));
    case MUIM_FileType_Find     : return(FileType_Find    (cl, obj, (APTR)msg));
  }

  return (DoSuperMethodA(cl, obj, msg));
}

/*
** CreateFileTypeClass()
** Funktion zum erstellen der FileType-Klasse und zuweisen der Dispatcherfunktion
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** struct MUI_CustomClass: Die fertig erzeugte Klasse
*/

struct MUI_CustomClass *CreateFileTypeClass()
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

   if (CL_FileTypeTree = CreateFileTypeTreeClass())
   {
      struct MUI_CustomClass *CL_FileType;

      CL_FileType = MUI_CreateCustomClass( NULL,
                                          MUIC_Window, NULL,
                                          sizeof(struct FileType_Data),
                                          FileType_Dispatcher
                                        );
      if (CL_FileType)
      {
         return(CL_FileType);
      }
   }

   DeleteFileTypeClass(NULL);
   return NULL;
}

/*
** DeleteFileTypeClass()
** Funktion zum löschen/beenden der FileType Klasse
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** struct MUI_CustomClass *: Pointer auf die zu löschende Klasse
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** keine
*/

void DeleteFileTypeClass(struct MUI_CustomClass *CL_FileType)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

   if (CL_FileType)
   {
      MUI_DeleteCustomClass(CL_FileType);
      CL_FileType = NULL;
   }

   if (CL_FileTypeTree)
   {
      DeleteFileTypeTreeClass(CL_FileTypeTree);
      CL_FileTypeTree = NULL;
   }
}

