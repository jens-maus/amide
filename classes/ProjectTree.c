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
#include "FileTypeTree.h"
#include "ProjectTree.h"
#include "Main.h"
#include "Project.h"

/* Functions */
SAVEDS ASM ULONG ProjectTree_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg);

/*
** ProjectTree_ConstFunc
** Funktion + Hook der beim Erzeugen des Objektes einer Klasse aufgerufen wird
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** struct Hook *     (a0): Der aufrufende Hook
** Object *          (a2): Das Objekt
** struct MUIP_...   (a1): Die übergebene ConstructMessage
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** METHOD: Das neue Objekt
*/

SAVEDS ASM APTR ProjectTree_ConstFunc(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct MUIP_NListtree_ConstructMessage *msg)
{
  struct Entry_Data *entry;
//  STRPTR  new_filename;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(msg != NULL) entry = (struct Entry_Data *)msg->UserData;

  // Allokieren des Speicherbereiches für den neuen Filenamen
//  new_filename = calloc((strlen(entry->entry_file)+1), sizeof(char));
//  strcpy(new_filename, entry->entry_file);
//  entry->entry_file = new_filename;

  return(entry);
}

const struct Hook ProjectTree_ConstructHook  = { { NULL, NULL }, (void *)ProjectTree_ConstFunc, NULL, NULL };

/*
** ProjectTree_DisplFunc
** Funktion + Hook der beim Darstellen des Objektes der Klasse aufgerufen wird
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** struct Hook *     (a0): Der aufrufende Hook
** Object *          (a2): Das Objekt
** struct MUIP_...   (a1): Die übergebene DisplayMessage
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** METHOD: Das neue Objekt
*/

SAVEDS ASM LONG ProjectTree_DisplFunc(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct MUIP_NListtree_DisplayMessage *msg)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(msg != NULL && msg->TreeNode != NULL)
  {
    struct Entry_Data *entry = (struct Entry_Data *)msg->TreeNode->tn_User;

    msg->Array[0] = " ";
    msg->Array[1] = " ";
    msg->Array[2] = " ";

    switch(entry->entry_type)
    {
      case ENTRY_TYPE_FOLDER:
      {
        static char folder_name[256];
        static char folder_codesize[256];
        static char folder_textsize[256];

        // Dann kopieren wir die Daten zum anzeigen um
        sprintf(folder_name,      "%s%s", STYLE_LT_MAINFOLDER,    entry->entry_file);
        sprintf(folder_codesize,  "%s%s", STYLE_LT_FOLD_CODESIZE, entry->entry_code);
        sprintf(folder_textsize,  "%s%s", STYLE_LT_FOLD_TEXTSIZE, entry->entry_text);

        if(entry->entry_file) msg->Array[0] = folder_name;
        if(entry->entry_code) msg->Array[1] = folder_codesize;
        if(entry->entry_text) msg->Array[2] = folder_textsize;
      
      }
      break;

      case ENTRY_TYPE_FILE:
      {
        if(entry->entry_file) msg->Array[0] = FilePart(entry->entry_file);
        if(entry->entry_code) msg->Array[1] = entry->entry_code;
        if(entry->entry_text) msg->Array[2] = entry->entry_text;
      }
      break;

      case ENTRY_TYPE_EXECUTEABLE:
      {
        static char file_name[256];

        // Dann den Style kopieren
        sprintf(file_name, "%s%s", "\033b", FilePart(entry->entry_file));

        if(entry->entry_file) msg->Array[0] = file_name;
        if(entry->entry_code) msg->Array[1] = entry->entry_code;
        if(entry->entry_text) msg->Array[2] = entry->entry_text;
      }
      break;
    }
  }
  else
  {
    msg->Array[0] = "File";
    msg->Array[1] = "Code";
    msg->Array[2] = "Text";

    msg->Preparse[0] = "\033b\033l";
    msg->Preparse[1] = "\033b\033c";
    msg->Preparse[2] = "\033b\033c";
  }

  return(0);
}

const struct Hook ProjectTree_DisplayHook    = { { NULL, NULL }, (void *)ProjectTree_DisplFunc,  NULL, NULL };

/*
** ProjectTree_DestFunc
** Funktion + Hook der beim Löschen des Objektes der Klasse aufgerufen wird
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** struct Hook *     (a0): Der aufrufende Hook
** Object *          (a2): Das Objekt
** struct MUIP_...   (a1): Die übergebene DisplayMessage
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** METHOD: Das neue Objekt
*/

SAVEDS ASM LONG ProjectTree_DestFunc(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct MUIP_NListtree_DestructMessage *msg)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Freigeben der über den ConstuctHook allokierten Speicherbereiche
/*
  if(entry->entry_file) { free(entry->entry_file); };
  if(entry->entry_code) { free(entry->entry_code); };
  if(entry->entry_text) { free(entry->entry_text); };
  if(entry)             { free(entry); };
*/
  return(0);
}

const struct Hook ProjectTree_DestructHook   = { { NULL, NULL }, (void *)ProjectTree_DestFunc,  NULL, NULL };

/*
** ProjectTree_New
** Methode zum erzeugen eines neues Listtree Objektes/Fensters
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

METHOD ProjectTree_New(Class *cl, Object *obj, struct opSet *msg)
{
  struct ProjectTree_Data Temp;
  struct ProjectTree_Data *Data;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

//   LocalizeStringArray(CYA_Command);
//   LocalizeNewMenu(NM_Mainwin);

  obj = (Object *)DoSuperNew(cl, obj,
    InputListFrame,

      MUIA_CycleChain,              TRUE,
      MUIA_ContextMenu,             TRUE,

      MUIA_NListtree_DisplayHook,   &ProjectTree_DisplayHook,
      MUIA_NListtree_ConstructHook, &ProjectTree_ConstructHook,
      MUIA_NListtree_DestructHook,  &ProjectTree_DestructHook,
//      MUIA_NListtree_CompareHook,   MUIV_NLt_CompareHook_LeavesMixed,
      MUIA_NListtree_DoubleClick,   MUIV_NListtree_DoubleClick_All,
      MUIA_NListtree_EmptyNodes,    TRUE,
      MUIA_NListtree_TreeColumn,    0,
      MUIA_NListtree_DragDropSort,  TRUE,
      MUIA_NListtree_Title,         TRUE,
      MUIA_NListtree_Format,        "D=8 BAR,D=8 P=\033r BAR,D=8 P=\033r BAR",
//      MUIA_Listtree_SortHook,       MUIV_NListtree_SortHook_LeavesBottom,
//      MUIA_Listtree_DisplayHook,    &ListTree_DisplayHook,
//      MUIA_Listtree_DragDropSort,   TRUE,
//      MUIA_List_DragSortable,       TRUE,
//      MUIA_Listtree_EmptyNodes,     TRUE,
//      MUIA_Listtree_Format,         "D=8 BAR,D=8 P=\033r BAR,D=8 P=\033r BAR,",
//      MUIA_Listtree_Title,          TRUE,
//      MUIA_Listtree_OpenHook,     &opnhook,
//      MUIA_Listtree_CloseHook,    &clshook,
//      MUIA_Listtree_MultiSelect,    MUIV_Listview_MultiSelect_Default,
  End;

  if (!obj) { return(NULL); }

  /* Nun Data instanzieren nachdem das Objekt erzeugt wurde */
  Data = INST_DATA(cl, obj);
  *Data = Temp;

  // Initialisieren
  Data->creator_obj = NULL;

  return ((ULONG)obj);
}

/*
** ProjectTree_Set
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

METHOD ProjectTree_Set(Class *cl, Object *obj, struct opSet *msg)
{
  struct ProjectTree_Data *Data = INST_DATA(cl, obj);
  struct TagItem *newdata;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Das TagItem finden und die Variable setzen
  if (newdata = FindTagItem(MUIA_ProjectTree_Creator, msg -> ops_AttrList))
  {
    Data->creator_obj = (Object *)(newdata->ti_Data);
  }

  // Alle anderen Items gehen uns nichts an
  DoSuperMethodA(cl, obj, (Msg) msg);

  return(0);
}

/*
** ProjectTree_Add_File
** Methode zum anlegen/parsen einer neuen Datei im Listtree
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

METHOD ProjectTree_Add_File(Class *cl, Object *obj, struct MUIP_ProjectTree_Add_File_Message *msg)
{
  struct  ProjectTree_Data *Data           = INST_DATA(cl, obj);

  /* Daten aus Msg holen */
  STRPTR  filename              = msg->filename;
  STRPTR  folder_name           = msg->folder_name;
  struct  Entry_Data *dep_entry = msg->entry;
  ULONG   compileable           = msg->compileable;
  ULONG   linkable              = msg->linkable;
  ULONG   touchable             = msg->touchable;
  ULONG   scan_dep              = msg->scan_dep;
  ULONG   binding               = msg->binding;
  STRPTR  edittool              = msg->edittool;
  ULONG   edittool_std          = msg->edittool_std;
  int     edittool_sel          = msg->edittool_sel;
  STRPTR  viewtool              = msg->viewtool;
  ULONG   viewtool_std          = msg->viewtool_std;
  int     viewtool_sel          = msg->viewtool_sel;

  struct Entry_Data *entry, *entry_folder;
  struct MUI_NListtree_TreeNode *tn = NULL;
  static char empty_str[] = "";

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Nun schauen wir ob schon eine gleiche Datei im Projekt existiert und wenn ja dann den Entry zurückliefern
  tn = (struct MUI_NListtree_TreeNode *)DoMethod(   obj, MUIM_NListtree_FindName,
                                                    MUIV_NListtree_FindName_ListNode_Root, filename,
                                                    0);

  if(tn)  { return((ULONG)tn->tn_User); }

  // Füllen der entry Struktur mit den Daten
  entry = calloc(1, sizeof(struct Entry_Data));
  entry->entry_file     = filename;
  entry->entry_code     = NULL;
  entry->entry_text     = NULL;

  if(msg->MethodID == MUIM_ProjectTree_Add_File)  entry->entry_type     = ENTRY_TYPE_FILE;
  if(msg->MethodID == MUIM_ProjectTree_Add_Exe)   entry->entry_type     = ENTRY_TYPE_EXECUTEABLE;

  entry->entry_compileable  = (BOOL)compileable;
  entry->entry_linkable     = (BOOL)linkable;
  entry->entry_touchable    = (BOOL)touchable;
  entry->entry_dep_scan     = (BOOL)scan_dep;
  entry->entry_binding      = (BOOL)binding;

  // Falls der String leer ist
  if(edittool)  entry->entry_edittool = edittool;
  else          entry->entry_edittool = empty_str;

  entry->entry_edittool_std = (BOOL)edittool_std;
  entry->entry_edittool_sel = edittool_sel;

  // Falls der String leer ist
  if(viewtool)  entry->entry_viewtool = viewtool;
  else          entry->entry_viewtool = empty_str;

  entry->entry_viewtool_std = (BOOL)viewtool_std;

  // Nun schauen wir ob vorher das edittool_sel auch 0 ist und wenn ja dann setzen wir viewtool_sel auf 1
  if(edittool_sel == 0 && viewtool_sel == 0) viewtool_sel++;

  entry->entry_viewtool_sel = viewtool_sel;

  // Nun holen wir das Datum der Datei
  get_file_date(entry);

  // Die Dependency-Liste allokieren/initialisieren
  entry->entry_dep  = NListObject,
                        MUIA_NList_DefaultObjectOnClick,  TRUE,
                        MUIA_NList_MultiSelect,           MUIV_NList_MultiSelect_None,
                        MUIA_NList_Format,                "COL=0 BAR, COL=1 BAR, COL=2",
                        MUIA_NList_AutoVisible,           TRUE,
                        MUIA_NList_TitleSeparator,        TRUE,
                        MUIA_NList_Title,                 TRUE,
                        MUIA_NList_MinColSortable,        0,
                      End;

  tn = (struct MUI_NListtree_TreeNode *)DoMethod(obj, MUIM_ProjectTree_Add_Folder, folder_name, TRUE, NULL, TAG_DONE);

  // Nun müssen wir noch den entry selber of tn zeigen lassen
  entry->entry_parent = tn;

  // Das Entry zum Listtree hinzufügen
  DoMethod( obj, MUIM_NListtree_Insert, entry->entry_file, entry, tn, MUIV_NListtree_Insert_PrevNode_Tail, TAG_DONE);

  // Nun lassen wir die Filelänge berechnen
  DoMethod( obj, MUIM_ProjectTree_Refresh_Sizes, tn, entry);

  return((ULONG)entry);
}

/*
** ProjectTree_Add_File_ByType
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

METHOD ProjectTree_Add_File_ByType(Class *cl, Object *obj, struct MUIP_ProjectTree_Add_File_ByType_Msg *msg)
{
  struct  ProjectTree_Data *Data            = INST_DATA(cl, obj);
  struct  FileType *type                    = msg->type;
  struct  MUI_NListtree_TreeNode *tn_parent = msg->type->parent;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  return(DoMethod(obj, MUIM_ProjectTree_Add_File,  msg->filename,
                                                ((struct FileType *)tn_parent->tn_User)->name,
                                                0,
                                                type->compileable,
                                                type->linkable,
                                                type->touchable,
                                                type->scan_dep,
                                                type->auto_binding,
                                                type->edit_tool,
                                                type->edit_tool_standard,
                                                type->edit_tool_sel,
                                                type->view_tool,
                                                type->view_tool_standard,
                                                type->view_tool_sel
        ));
}

/*
** ProjectTree_Add_Folder
** Methode zum anlegen eines neuen Folders im Listtree
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

METHOD ProjectTree_Add_Folder(Class *cl, Object *obj, struct MUIP_ProjectTree_Add_Folder_Message *msg)
{
  struct  ProjectTree_Data *Data           = INST_DATA(cl, obj);

  STRPTR  folder_name   = msg->folder_name;
  ULONG   open          = msg->open;
  STRPTR  folder_parent = msg->folder_parent;

  struct  Entry_Data *entry_folder;
  struct  MUI_NListtree_TreeNode *tn=NULL, *tn_parent=NULL;
  ULONG   TNF_flags = (TNF_LIST | TNF_OPEN);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Nun schauen wir ob ein Folder mit dem folder_name schon existiert
  tn = (struct MUI_NListtree_TreeNode *)DoMethod(   obj, MUIM_NListtree_FindName,
                                                    MUIV_NListtree_FindName_ListNode_Root, folder_name,
                                                    0);

  // Wenn der Folder schon existiert und auch ein Folder ist geben wir nur seine Treenode zurück
  if(tn != NULL)
  {
    if(((struct Entry_Data *)(tn->tn_User))->entry_type == ENTRY_TYPE_FOLDER) return((ULONG)tn);
  }

  // Schauen ob ein parent angeben wurde
  if(folder_parent)
  {
    tn_parent = (struct MUI_NListtree_TreeNode *)DoMethod(  obj, MUIM_NListtree_FindName,
                                                            MUIV_NListtree_FindName_ListNode_Root, folder_parent,
                                                            0);
  }

  // Wenn ein Parent nicht gefunden wurde dann setzen wir es Root
  if(!tn_parent)
  {
    tn_parent = MUIV_NListtree_Insert_ListNode_Root;
  }

  // Nun kopieren wir alles in die Entry_Data Struktur
  entry_folder = calloc(1, sizeof(struct Entry_Data));
  entry_folder->entry_file          = folder_name;
  entry_folder->entry_code          = NULL;
  entry_folder->entry_text          = NULL;
  if(msg->MethodID == MUIM_ProjectTree_Add_Folder) entry_folder->entry_type = ENTRY_TYPE_FOLDER;
  entry_folder->entry_compileable   = FALSE;
  entry_folder->entry_linkable      = FALSE;
  entry_folder->entry_touchable     = FALSE;
  entry_folder->entry_dep_scan      = FALSE;
  entry_folder->entry_binding       = FALSE;
  entry_folder->entry_edittool      = NULL;
  entry_folder->entry_edittool_std  = FALSE;
  entry_folder->entry_edittool_sel  = 0;
  entry_folder->entry_viewtool      = NULL;
  entry_folder->entry_viewtool_std  = FALSE;
  entry_folder->entry_viewtool_sel  = 0;

  // Wenn ein Parent übergeben wurde dann setzen wir es auch
  if(folder_parent) entry_folder->entry_parent = tn_parent;
  else              entry_folder->entry_parent = NULL;

  if(!open) TNF_flags = TNF_LIST;

  // Nun legen wir erstmal den neuen Folder an
  tn = (struct MUI_NListtree_TreeNode *)DoMethod( obj, MUIM_NListtree_Insert, entry_folder->entry_file,
                                                  entry_folder, tn_parent,
                                                  MUIV_NListtree_Insert_PrevNode_Tail, TNF_flags, 0);

  if(tn) { return((ULONG)tn); }

  return(0);
}

/*
** ProjectTree_Create_CMenu
**
** Methode um dynamisch ein ContextMenu zu erzeugen
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class *        : Die Klasse des Objektes
** Object *       : Das Objekt
** struct opGet * : Pointer auf die opGet Struktur
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** 0 - MUSS 0 zurückliefern, da ansonsten ein ContextMenu aufgebaut wird
*/

METHOD ProjectTree_Create_CMenu(Class *cl, Object *obj, struct MUIP_ContextMenuBuild *msg)
{
  struct ProjectTree_Data *Data = INST_DATA(cl, obj);
  struct MUI_NListtree_TestPos_Result r;
  struct MUI_NListtree_TreeNode *tn;
  struct Entry_Data *entry;
  struct PopupMenuBase  *PopupMenuBase;
  struct PopupMenu      *pop_menu;
  struct Window         *win;
  ULONG  ret = 0;
  BOOL   tmp_dis = TRUE;

  enum{ PMN_EDIT=1, PMN_VIEW, PMN_REMOVE, PMN_INFO, PMN_TOUCH, PMN_COMPILE };

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Rausfinden was unter der Maus für ein Entry ist
  DoMethod(obj, MUIM_NListtree_TestPos, msg->mx, msg->my, &r);

  tn = r.tpr_TreeNode;
  if(!tn)  return(0);

  entry = (struct Entry_Data *)tn->tn_User;
  if(!entry)  return(0);

  // Diese TreeNode aktiv setzen (ohne Notify)
  nnset(obj, MUIA_NListtree_Active, tn);

  // Nun holen wir die Window Struktur
  get(_win(obj), MUIA_Window_Window, &win);
  if(!win) return(0);

  // Open the popupmenu.library
  PopupMenuBase = (struct PopupMenuBase *)OpenLibrary(POPUPMENU_NAME, POPUPMENU_VERSION);
  if(PopupMenuBase)
  {
    pop_menu =  PMMenu(FilePart(entry->entry_file)),
                End;

    // Dann adden wir die anderen Menuitems dynamisch hinzu
    if(entry->entry_type == ENTRY_TYPE_FILE)
    {
      // Schauen wie der Benutzer das Menu will
      if(entry->entry_edittool_sel == 1)
      {
        PM_InsertMenuItem(  pop_menu, PM_Insert_Last, TRUE,
                            PM_Insert_Item, PM_MakeItem(  PM_Title, "Edit...",  PM_UserData,  PMN_EDIT, TAG_DONE),
                            PM_Insert_Item, PM_MakeItem(  PM_Title, "View...",  PM_UserData,  PMN_VIEW, TAG_DONE),
                            PM_Insert_Item, PM_MakeItem(  PM_TitleBar,  TRUE, TAG_DONE),
                            TAG_DONE
                         );
      }else
      {
        PM_InsertMenuItem(  pop_menu, PM_Insert_Last, TRUE,
                            PM_Insert_Item, PM_MakeItem(  PM_Title, "View...",  PM_UserData,  PMN_VIEW, TAG_DONE),
                            PM_Insert_Item, PM_MakeItem(  PM_Title, "Edit...",  PM_UserData,  PMN_EDIT, TAG_DONE),
                            PM_Insert_Item, PM_MakeItem(  PM_TitleBar,  TRUE, TAG_DONE),
                            TAG_DONE
                         );
      }
    }

    PM_InsertMenuItem(  pop_menu, PM_Insert_Last, TRUE,
                        PM_Insert_Item, PM_MakeItem(  PM_Title, "Remove...",      PM_UserData,  PMN_REMOVE, TAG_DONE),
                        PM_Insert_Item, PM_MakeItem(  PM_Title, "Information...", PM_UserData,  PMN_INFO,   TAG_DONE),
                        TAG_DONE
                     );

    // Wenn das entry kein Folder ist schalten wir touch/compile an
    if(entry->entry_type != ENTRY_TYPE_FOLDER)  tmp_dis = FALSE;

    PM_InsertMenuItem(  pop_menu, PM_Insert_Last, TRUE,
                        PM_Insert_Item, PM_MakeItem(  PM_Title, "Touch...",     PM_Disabled, tmp_dis, PM_UserData,  PMN_TOUCH,    TAG_DONE),
                        PM_Insert_Item, PM_MakeItem(  PM_Title, "Compile...",   PM_Disabled, tmp_dis, PM_UserData,  PMN_COMPILE,  TAG_DONE),
                        TAG_DONE
                     );

    ret = (ULONG)(PM_OpenPopupMenu(  win, PM_Menu,    pop_menu, TAG_DONE));

    PM_FreePopupMenu(pop_menu);
    CloseLibrary((struct Library *)PopupMenuBase);

    switch(ret)
    {
      case PMN_EDIT:      DoMethod(Data->creator_obj, MUIM_Project_File_Edit, entry, 0);            return(0);
      case PMN_VIEW:      DoMethod(Data->creator_obj, MUIM_Project_File_View, entry, 0);            return(0);
      case PMN_REMOVE:    DoMethod(obj, MUIM_NListtree_Remove, MUIV_NListtree_Remove_ListNode_Root, tn, 0); return(0);
      case PMN_INFO:      DoMethod(Data->creator_obj, MUIM_Project_File_Info_Open, entry, 0);       return(0);
      case PMN_TOUCH:     DoMethod(Data->creator_obj, MUIM_Project_File_Touch, entry, 0);           return(0);
      case PMN_COMPILE:   DoMethod(Data->creator_obj, MUIM_Project_Compile, entry, 0);              return(0);
    }
  }

  return(0);
}

/*
** ProjectTree_Find_Exe
** Methode zum finden des Executeables
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class * : Die aufrufende Klasse
** Object *: Das Objekt
** Msg     : Die Message
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** METHOD: Das entry des Executeables
*/

METHOD ProjectTree_Find_Exe(Class *cl, Object *obj, Msg msg)
{
  struct  ProjectTree_Data *Data           = INST_DATA(cl, obj);
  struct  Entry_Data *entry;
  int i;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif


  for(i=0; (entry = (struct Entry_Data *)DoMethod(obj, MUIM_ProjectTree_Get_Entry, i, 0)); i++)
  {
    if(entry->entry_type == ENTRY_TYPE_EXECUTEABLE) return((ULONG)entry);
  }

  return(0);
}

/*
** ProjectTree_Finish
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

METHOD ProjectTree_Finish(Class *cl, Object *obj, Msg msg)
{
  struct ProjectTree_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  DoMethod(_app(obj), OM_REMMEMBER, obj);

  return(0);
}

/*
** ProjectTree_Get_Entry
** Methode zum holen des aktiven Entries
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class * : Die aufrufende Klasse
** Object *: Das Objekt
** Msg     : Die Message
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** METHOD: Das neue Objekt
*/

METHOD ProjectTree_Get_Entry(Class *cl, Object *obj, struct MUIP_ProjectTree_GetEntry_Message *msg)
{
  struct  ProjectTree_Data *Data           = INST_DATA(cl, obj);

  struct  MUI_NListtree_TreeNode *tn;
  struct  Entry_Data *entry;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(msg->pos == MUIV_ProjectTree_Active) msg->pos = MUIV_NListtree_GetEntry_Position_Active;

  tn = (struct MUI_NListtree_TreeNode *)DoMethod(obj, MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Root, msg->pos, 0);

  if(!tn) return(0);

  entry = (struct Entry_Data *)tn->tn_User;

  if(tn->tn_Flags & TNF_OPEN) entry->entry_open = TRUE;
  else                        entry->entry_open = FALSE;

  return((ULONG)entry);
}

/*
** ProjectTree_Refresh_Sizes
** Methode die die Dateilänge des übergebenen Eintrages neu berechnet und in die
** Entry_Data Struktur einträgt.
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

METHOD ProjectTree_Refresh_Sizes(Class *cl, Object *obj, struct MUIP_ProjectTree_Refresh_Sizes_Message *msg)
{
  struct  ProjectTree_Data *Data           = INST_DATA(cl, obj);

  struct  MUI_NListtree_TreeNode *node  = msg->tn;
  struct  Entry_Data *entry             = msg->entry;

  char    buffer[255];
  STRPTR  new_textsize;
  STRPTR  new_codesize;
  struct  MUI_NListtree_TreeNode *newnode;
  struct  Entry_Data *newentry;
  LONG    foldertextsize = 0;
  LONG    foldercodesize = 0;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(entry->entry_type == ENTRY_TYPE_FOLDER)
  {
    newnode = (struct MUI_NListtree_TreeNode *)DoMethod(  obj, MUIM_NListtree_GetEntry,
                                                          node, 0, MUIV_NListtree_GetEntry_Flag_SameLevel);

    if(newnode) newentry = (struct Entry_Data *)newnode->tn_User;

    while(newnode)
    {
      foldertextsize += atol(newentry->entry_text);
      foldercodesize += atol(newentry->entry_code);

      newnode = (struct MUI_NListtree_TreeNode *)DoMethod(  obj, MUIM_NListtree_GetEntry,
                                                            newnode, MUIV_NListtree_GetEntry_Position_Next,
                                                            MUIV_NListtree_GetEntry_Flag_SameLevel);

      if(newnode) newentry = (struct Entry_Data *)newnode->tn_User;
    }

    // Holen der Filegröße und konvertieren in einen String
    if(foldertextsize == 0) strcpy(buffer, " ");
    else sprintf(buffer, "%ld", foldertextsize);
    new_textsize = calloc(strlen(buffer)+1, sizeof(char));
    strcpy(new_textsize, buffer);

    if(foldercodesize == 0) strcpy(buffer, " ");
    else sprintf(buffer, "%ld", foldercodesize);
    new_codesize = calloc(strlen(buffer)+1, sizeof(char));
    strcpy(new_codesize, buffer);

    // Schauen ob schon ein Textsize existiert und wenn ja, dann vorher freigeben.
    if(entry->entry_text) { free(entry->entry_text); };
    // Schauen ob schon ein Codesize existiert und wenn ja, dann vorher freigeben.
    if(entry->entry_code) { free(entry->entry_code); };

    entry->entry_text = new_textsize;
    entry->entry_code = new_codesize;

    return(0);
  }

  if(entry->entry_type == ENTRY_TYPE_FILE || entry->entry_type == ENTRY_TYPE_EXECUTEABLE)
  {
    // Holen der Filegröße und konvertieren in einen String
    sprintf(buffer, "%ld", FileSize(entry->entry_file));

    new_textsize = calloc(strlen(buffer)+1, sizeof(char));
    strcpy(new_textsize, buffer);

    // Schauen ob schon ein Textsize existiert und wenn ja, dann vorher freigeben.
    if(entry->entry_text) { free(entry->entry_text); };

    entry->entry_text = new_textsize;

    // Nun checken wir ob es ein compileable file ist
    if(entry->entry_compileable)
    {
      // Erzeugen des Objektfilenamens
      {
        STRPTR tmp_str;

        // Umkopieren
        strcpy(buffer, entry->entry_file);

        tmp_str = strrchr(buffer, '.');

        // Wenn noch genug Platz am String ist überschreibe ich das zeichen nach dem Punkt mit einem "o"
        if(strlen(tmp_str) > 1)
        {
          tmp_str++;
          *tmp_str = 'o';
          tmp_str++;
          *tmp_str = '\0';
        }
      }

      // Holen der Filegröße und konvertieren in einen String
      sprintf(buffer, "%ld", FileSize(buffer));

      new_codesize = calloc(strlen(buffer)+1, sizeof(char));
      strcpy(new_codesize, buffer);

      // Schauen ob schon ein Codesize existiert und wenn ja, dann vorher freigeben.
      if(entry->entry_code) { free(entry->entry_code); };

      entry->entry_code = new_codesize;
    }

    // Am Schluss muss sich der Folder neu kalkulieren
    DoMethod(obj, MUIM_ProjectTree_Refresh_Sizes, entry->entry_parent, (struct Entry_Data *)entry->entry_parent->tn_User);
  }

  return(0);
}

/*
** ProjectTree_Dispatcher
** Funktion die beim auftreten von Methoden-Messages aufgerufen wird
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

SAVEDS ASM ULONG ProjectTree_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg)
{
  #ifdef DEBUG
//  DEBUG_MAKRO
  #endif

   switch (msg->MethodID)
   {
      case OM_NEW                           :   return(ProjectTree_New             (cl, obj, (APTR)msg));
      case OM_SET                           :   return(ProjectTree_Set             (cl, obj, (APTR)msg));
      case MUIM_ProjectTree_Finish          :   return(ProjectTree_Finish          (cl, obj, (APTR)msg));
      case MUIM_ProjectTree_Refresh_Sizes   :   return(ProjectTree_Refresh_Sizes   (cl, obj, (APTR)msg));
      case MUIM_ProjectTree_Add_Exe         :
      case MUIM_ProjectTree_Add_File        :   return(ProjectTree_Add_File        (cl, obj, (APTR)msg));
      case MUIM_ProjectTree_Add_File_ByType :   return(ProjectTree_Add_File_ByType (cl, obj, (APTR)msg));
      case MUIM_ProjectTree_Add_Folder      :   return(ProjectTree_Add_Folder      (cl, obj, (APTR)msg));
      case MUIM_ProjectTree_Get_Entry       :   return(ProjectTree_Get_Entry       (cl, obj, (APTR)msg));
      case MUIM_ProjectTree_Find_Exe        :   return(ProjectTree_Find_Exe        (cl, obj, (APTR)msg));

      // Wenn ein ContextMenu gemacht werden soll
      case MUIM_ContextMenuBuild            :   return(ProjectTree_Create_CMenu    (cl, obj, (APTR)msg));

      // Drag&Drop Methoden
      case MUIM_DragDrop:
      {
        struct ProjectTree_Data *Data = INST_DATA(cl, obj);

        struct MUIP_DragDrop *dd = (struct MUIP_DragDrop *)msg;
        struct MUI_NListtree_TestPos_Result r;
        struct MUI_NListtree_TreeNode *tn, *tn_parent;
        struct Entry_Data *entry;

        DoMethod(obj, MUIM_NListtree_TestPos, Data->drag_x, Data->drag_y, &r);

        tn = (struct MUI_NListtree_TreeNode *)DoMethod(obj, MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Active, MUIV_NListtree_GetEntry_Position_Active, 0);
        if(tn) entry = (struct Entry_Data *)tn->tn_User;

        // Rausfinden der Parent-TreeNode des Objektes wo wir es hingedragt haben
        tn_parent = (struct MUI_NListtree_TreeNode *)DoMethod(obj, MUIM_NListtree_GetEntry, r.tpr_TreeNode, MUIV_NListtree_GetEntry_Position_Parent, 0);
        if(tn_parent) entry->entry_parent = tn_parent;
      }
      break;

      // we catch MUIM_DragReport because we want to restrict some dragging for some special objects
      case MUIM_DragReport:
      {
        struct ProjectTree_Data *Data = INST_DATA(cl, obj);

        struct MUIP_DragReport *drag_report = (struct MUIP_DragReport *)msg;
        struct MUI_NListtree_TestPos_Result res;
        struct MUI_NListtree_TreeNode *tn, *active_tn;

        DoMethod(obj, MUIM_NListtree_TestPos, drag_report->x, drag_report->y, &res);

        if(tn = res.tpr_TreeNode)
        {
          struct Entry_Data *entry = (struct Entry_Data *)tn->tn_User;
          struct Entry_Data *active_entry;

          // now we get the active treenode we are coming from
          active_tn = (struct MUI_NListtree_TreeNode *)DoMethod(obj, MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Active, MUIV_NListtree_GetEntry_Position_Active, 0);
          if(active_tn) active_entry = (struct Entry_Data *)active_tn->tn_User;

          // if the parent treenode is the same one as the active tree node or both parent`s are the same we
          // allow dropping or if the entry_binding is switched of for this entry
          if(active_entry->entry_parent == tn || active_entry->entry_parent == entry->entry_parent || active_entry->entry_binding == FALSE)
          {
            // if the entry_type is a FILE and the droptype is ONTO we change the droptype
            if( entry->entry_type == ENTRY_TYPE_FILE && res.tpr_Flags == MUIV_NListtree_DropType_Onto)
            {
              res.tpr_Flags = MUIV_NListtree_DropType_None;
            }

            // Dann die Koordinaten des D&D speichern
            Data->drag_x = drag_report->x;
            Data->drag_y = drag_report->y;

            // now we allow dragging by calling the SuperMethod()
            return(DoSuperMethodA(cl, obj, msg));
          }

          // if we reach here we don`t allow dropping
          return(MUIV_DragReport_Abort);

        }

        // to rescue the dropping we call the SuperMethod now
        return(DoSuperMethodA(cl, obj, msg));
      }
      break;
   }

   return (DoSuperMethodA(cl, obj, msg));
}

/*
** CreateProjectTreeClass()
** Funktion zum erstellen der Listtree Klasse und zuweisen der Dispatcherfunktion
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** struct MUI_CustomClass: Die fertig erzeugte Klasse
*/

struct MUI_CustomClass *CreateProjectTreeClass()
{
  struct MUI_CustomClass *CL_ProjectTree;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  CL_ProjectTree = MUI_CreateCustomClass( NULL,
                                          MUIC_NListtree, NULL,
                                          sizeof(struct ProjectTree_Data),
                                          ProjectTree_Dispatcher
                                        );
  if (CL_ProjectTree)
  {
    return(CL_ProjectTree);
  }

  DeleteProjectTreeClass(NULL);
  return NULL;
}

/*
** DeleteProjectTreeClass()
** Funktion zum löschen/freigeben der Listtree Klasse
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** struct MUI_CustomClass *: Die eigentliche Klasse
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** keine
*/

void DeleteProjectTreeClass(struct MUI_CustomClass *CL_ProjectTree)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if (CL_ProjectTree)
  {
    MUI_DeleteCustomClass(CL_ProjectTree);
  }
}
