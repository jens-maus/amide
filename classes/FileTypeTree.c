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
#include "Main.h"

/* Functions */
SAVEDS ASM ULONG FileTypeTree_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg);

/*
** FileTypeTree_ConstFunc
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

SAVEDS ASM APTR FileTypeTree_ConstFunc(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct MUIP_NListtree_ConstructMessage *msg)
{
  struct FileType *type;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(msg != NULL) type = (struct FileType *)msg->UserData;

/*  STRPTR  new_filename;

  // Allokieren des Speicherbereiches für den neuen Filenamen
  new_filename = calloc((strlen(entry->entry_file)+1), sizeof(char));
  strcpy(new_filename, entry->entry_file);
  entry->entry_file = new_filename;
*/
  return(type);
}

const struct Hook FileTypeTree_ConstructHook  = { { NULL, NULL }, (void *)FileTypeTree_ConstFunc, NULL, NULL };

/*
** FileTypeTree_DisplFunc
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

SAVEDS ASM LONG FileTypeTree_DisplFunc(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct MUIP_NListtree_DisplayMessage *msg)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(msg != NULL && msg->TreeNode != NULL)
  {
    struct FileType *type = (struct FileType *)msg->TreeNode->tn_User;

    msg->Array[0] = " ";
    msg->Array[1] = " ";
    msg->Array[2] = " ";
    msg->Array[3] = " ";
    msg->Array[4] = " ";
    msg->Array[5] = " ";
    msg->Array[6] = " ";
    msg->Array[7] = " ";
    msg->Array[8] = " ";

    if(type->folder)
    {
      static char folder_name[256];

      // Dann kopieren wir die Daten zum anzeigen um
      sprintf(folder_name,      "%s%s", STYLE_LT_MAINFOLDER,    type->name);

      if(type->name)    msg->Array[0] = folder_name;
    }
    else
    {
      if(type->pattern)             msg->Array[0] = type->pattern;
      if(type->name)                msg->Array[1] = type->name;
      if(type->compileable)         msg->Array[2] = "x";
      if(type->linkable)            msg->Array[3] = "x";
      if(type->touchable)           msg->Array[4] = "x";
      if(type->scan_dep)            msg->Array[5] = "x";
      if(type->auto_binding)        msg->Array[6] = "x";

      if(type->edit_tool_standard)  msg->Array[7] = "\033bStandard";
      else                          msg->Array[7] = type->edit_tool;

      if(type->view_tool_standard)  msg->Array[8] = "\033bStandard";
      else                          msg->Array[8] = type->view_tool;

    }
  }
  else
  {
    msg->Array[0] = "Name/Pattern";
    msg->Array[1] = "Description";
    msg->Array[2] = "C";
    msg->Array[3] = "L";
    msg->Array[4] = "T";
    msg->Array[5] = "D";
    msg->Array[6] = "A";
    msg->Array[7] = "Edit Tool";
    msg->Array[8] = "View Tool";

    msg->Preparse[0] = "\033b\033l";
    msg->Preparse[1] = "\033b\033l";
    msg->Preparse[2] = "\033b\033l";
    msg->Preparse[3] = "\033b\033l";
    msg->Preparse[4] = "\033b\033l";
    msg->Preparse[5] = "\033b\033l";
    msg->Preparse[6] = "\033b\033l";
    msg->Preparse[7] = "\033b\033l";
    msg->Preparse[8] = "\033b\033l";
  }

  return(0);
}

const struct Hook FileTypeTree_DisplayHook    = { { NULL, NULL }, (void *)FileTypeTree_DisplFunc,  NULL, NULL };

/*
** FileTypeTree_DestFunc
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

SAVEDS ASM LONG FileTypeTree_DestFunc(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct MUIP_NListtree_DestructMessage *msg)
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

const struct Hook FileTypeTree_DestructHook   = { { NULL, NULL }, (void *)FileTypeTree_DestFunc,  NULL, NULL };

/*
** FileTypeTree_Get
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

METHOD FileTypeTree_Get(Class *cl, Object *obj, struct MUIP_FileTypeTree_Get_Message *msg)
{
  struct  FileTypeTree_Data *Data           = INST_DATA(cl, obj);

  struct  MUI_NListtree_TreeNode *tn;
  struct  FileType *type;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(msg->pos == MUIV_FileTypeTree_Active) msg->pos = xget(obj, MUIA_List_Active);

  DoMethod(obj, MUIM_List_GetEntry, msg->pos, &tn);
  if(!tn) return(0);

  type = (struct FileType *)tn->tn_User;

  return((ULONG)type);
}

/*
** FileTypeTree_New
** Methode zum erzeugen eines neues FileTypeTree Objektes/Fensters
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

METHOD FileTypeTree_New(Class *cl, Object *obj, struct opSet *msg)
{
  struct FileTypeTree_Data Temp;
  struct FileTypeTree_Data *Data;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

//   LocalizeStringArray(CYA_Command);
//   LocalizeNewMenu(NM_Mainwin);

  obj = (Object *)DoSuperNew(cl, obj,
    InputListFrame,
      MUIA_NListtree_DisplayHook,   &FileTypeTree_DisplayHook,
      MUIA_NListtree_ConstructHook, &FileTypeTree_ConstructHook,
      MUIA_NListtree_DestructHook,  &FileTypeTree_DestructHook,
//      MUIA_NListtree_CompareHook,   MUIV_NLt_CompareHook_LeavesMixed,
//      MUIA_NListtree_DoubleClick,   MUIV_NListtree_DoubleClick_All,
      MUIA_NListtree_EmptyNodes,    FALSE,
      MUIA_NListtree_TreeColumn,    0,
      MUIA_NListtree_DragDropSort,  TRUE,
      MUIA_NListtree_Title,         TRUE,
      MUIA_NListtree_Format,        "D=8 BAR,D=8 P=\033l BAR,P=\033c BAR,P=\033c BAR,P=\033c BAR,P=\033c BAR,P=\033l BAR,D=8 P=\033l BAR,D=8 P=\033l",
//      MUIA_NListtree_SortHook,       MUIV_NListtree_SortHook_LeavesBottom,
//      MUIA_NListtree_DisplayHook,    &ListTree_DisplayHook,
//      MUIA_NListtree_DragDropSort,   TRUE,
//      MUIA_List_DragSortable,       TRUE,
//      MUIA_NListtree_EmptyNodes,     TRUE,
//      MUIA_NListtree_Format,         "D=8 BAR,D=8 P=\033r BAR,D=8 P=\033r BAR,",
//      MUIA_NListtree_Title,          TRUE,
//      MUIA_NListtree_OpenHook,     &opnhook,
//      MUIA_NListtree_CloseHook,    &clshook,
//      MUIA_NListtree_MultiSelect,    MUIV_Listview_MultiSelect_Default,
  End;

  if (!obj) { return(NULL); }

  /* Nun Data instanzieren nachdem das Objekt erzeugt wurde */
  Data = INST_DATA(cl, obj);
  *Data = Temp;

  return ((ULONG)obj);
}

/*
** FileTypeTree_Add_Folder
** Methode zum anlegen eines neuen Folders im FileTypeTree
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

METHOD FileTypeTree_Add_Folder(Class *cl, Object *obj, struct MUIP_FileTypeTree_Add_Folder_Message *msg)
{
  struct  FileTypeTree_Data *Data           = INST_DATA(cl, obj);

  struct  FileType *type;
  struct  MUI_NListtree_TreeNode *tn;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(msg->folder_name)
  {
    // Nun schauen wir ob ein Folder mit dem folder_name schon existiert
    tn = (struct MUI_NListtree_TreeNode *)DoMethod( obj, MUIM_NListtree_FindName,
                                                    MUIV_NListtree_FindName_ListNode_Root, msg->folder_name,
                                                    0);

    // Wenn der Folder schon existiert und auch ein Folder ist geben wir nur seine Treenode zurück
    if(tn)
    {
      if(((struct FileType *)(tn->tn_User))->folder == TRUE) return((ULONG)tn);
    }
  }

  // Nun legen wir eine neue FileType Struktur an und legen die als Folder an
  type = calloc(1, sizeof(struct FileType));

  if(msg->folder_name)  strcpy(type->name, msg->folder_name);
  else                  strcpy(type->name, "(Unknown)");

  type->folder        = TRUE;
  type->compileable   = FALSE;
  type->linkable      = FALSE;
  type->touchable     = FALSE;
  type->scan_dep      = FALSE;
  type->auto_binding  = FALSE;
  type->parent        = NULL;

  // Nun legen wir erstmal den neuen Folder an
  tn = (struct MUI_NListtree_TreeNode *)DoMethod( obj, MUIM_NListtree_Insert, type->name,
                                                  type, MUIV_NListtree_Insert_ListNode_Root,
                                                  MUIV_NListtree_Insert_PrevNode_Tail, TNF_LIST | TNF_OPEN, 0);

  // Dann setzen wir den Folder noch auf aktiv
  set(obj, MUIA_NListtree_Active, tn);

  if(tn) { return((ULONG)tn); }

  return(0);
}

/*
** FileTypeTree_Add_Type
** Methode zum anlegen/parsen einer neuen Datei im FileTypeTree
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

METHOD FileTypeTree_Add_Type(Class *cl, Object *obj, struct MUIP_FileTypeTree_Add_Type_Message *msg)
{
  struct FileTypeTree_Data *Data           = INST_DATA(cl, obj);

  struct FileType *type;
  struct MUI_NListtree_TreeNode *tn         = NULL;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(msg->type_pattern)
  {
    // Nun schauen wir ob schon eine gleiche Datei im Projekt existiert und wenn ja dann den Entry zurückliefern
    tn = (struct MUI_NListtree_TreeNode *)DoMethod( obj, MUIM_NListtree_FindName,
                                                    MUIV_NListtree_FindName_ListNode_Root, msg->type_pattern,
                                                    0);

    if(tn)  { return((ULONG)tn->tn_User); }

  }

  // Holen des aktiven Eintrages
  DoMethod(obj, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &tn);

  // Falls die TreeNode kein Folder ist können wir das Pattern nicht adden lassen
  if(!tn) { return(0); }

  // Allokieren eines neuen Filetypen und füllen der Struktur
  type = calloc(1, sizeof(struct FileType));

  if(msg->type_name)      strcpy(type->name,      msg->type_name);
  else                    strcpy(type->name,      "n/a"         );

  if(msg->type_pattern)   strcpy(type->pattern,   msg->type_pattern );
  else                    strcpy(type->pattern,   "#?"              );

  if(msg->type_edittool)  strcpy(type->edit_tool, msg->type_edittool);
  else                    strcpy(type->view_tool, "Ed"              );

  if(msg->type_viewtool)  strcpy(type->view_tool, msg->type_viewtool);
  else                    strcpy(type->view_tool, "Multiview"       );

  // Der Type ist kein Folder
  type->folder              = FALSE;

  type->edit_tool_standard  = msg->type_edittool_std;
  type->edit_tool_sel       = msg->type_edittool_sel;
  type->view_tool_standard  = msg->type_viewtool_std;
  type->view_tool_sel       = msg->type_viewtool_sel;
  type->compileable         = msg->type_compileable;
  type->linkable            = msg->type_linkable;
  type->touchable           = msg->type_touchable;
  type->scan_dep            = msg->type_scan_dep;
  type->auto_binding        = msg->type_auto_binding;
  type->parent              = NULL;

  // Das Entry zum Listtree hinzufügen
  if(tn)
  {
    type->parent = tn;
    DoMethod( obj, MUIM_NListtree_Insert, type->pattern, type, tn, MUIV_NListtree_Insert_PrevNode_Tail, MUIV_NListtree_Insert_Flag_Active, 0);
  }
  else DoMethod( obj, MUIM_NListtree_Insert, type->pattern, type, MUIV_NListtree_Insert_ListNode_Root,  MUIV_NListtree_Insert_PrevNode_Tail, MUIV_NListtree_Insert_Flag_Active, 0);

  return((ULONG)type);
}

/*
** FileTypeTree_Finish
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

METHOD FileTypeTree_Finish(Class *cl, Object *obj, Msg msg)
{
   struct FileTypeTree_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  DoMethod(_app(obj), OM_REMMEMBER, obj);

  return(0);
}

/*
** FileTypeTree_Dispatcher
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

SAVEDS ASM ULONG FileTypeTree_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

   switch (msg->MethodID)
   {
      case OM_NEW                       :   return(FileTypeTree_New         (cl, obj, (APTR)msg));
      case MUIM_FileTypeTree_Finish     :   return(FileTypeTree_Finish      (cl, obj, (APTR)msg));
      case MUIM_FileTypeTree_Add_Type   :   return(FileTypeTree_Add_Type    (cl, obj, (APTR)msg));
      case MUIM_FileTypeTree_Add_Folder :   return(FileTypeTree_Add_Folder  (cl, obj, (APTR)msg));
      case MUIM_FileTypeTree_Get        :   return(FileTypeTree_Get         (cl, obj, (APTR)msg));

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
          struct FileType *type = (struct FileType *)tn->tn_User;
          struct FileType *active_type;

          // now we get the active treenode we are coming from
          active_tn = (struct MUI_NListtree_TreeNode *)DoMethod(obj, MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Active, MUIV_NListtree_GetEntry_Position_Active, 0);
          if(active_tn) active_type = (struct FileType *)active_tn->tn_User;

          // if the user tries to drag a folder we don`t allow it
          // or if he tries to drag it into the root folder
          if(active_type->folder == TRUE || tn == (struct MUI_NListtree_TreeNode *)DoMethod(obj, MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Root, 0, 0))
          {
            return(MUIV_DragReport_Abort);
          }

          // otherwise we allow the dragging
          return(DoSuperMethodA(cl, obj, msg));
        }

        // to rescue the dropping we call the SuperMethod now
        return(DoSuperMethodA(cl, obj, msg));
      }
      break;
    }
    return (DoSuperMethodA(cl, obj, msg));
}

/*
** CreateFileTypeTreeClass()
** Funktion zum erstellen der FileTypeTree Klasse und zuweisen der Dispatcherfunktion
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** struct MUI_CustomClass: Die fertig erzeugte Klasse
*/

struct MUI_CustomClass *CreateFileTypeTreeClass()
{
  struct MUI_CustomClass *CL_FileTypeTree;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  CL_FileTypeTree = MUI_CreateCustomClass(NULL,
                                          MUIC_NListtree, NULL,
                                      sizeof(struct FileTypeTree_Data),
                                      FileTypeTree_Dispatcher
                                      );
  if (CL_FileTypeTree)
  {
    return CL_FileTypeTree;
  }

  DeleteFileTypeTreeClass(NULL);
  return NULL;
}

/*
** DeleteFileTypeTreeClass()
** Funktion zum löschen/freigeben der FileTypeTree Klasse
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** struct MUI_CustomClass *: Die eigentliche Klasse
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** keine
*/

void DeleteFileTypeTreeClass(struct MUI_CustomClass *CL_FileTypeTree)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

   if (CL_FileTypeTree)
   {
      MUI_DeleteCustomClass(CL_FileTypeTree);
   }
}
