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
#include "Main.h"
#include "MainBar.h"
#include "Project.h"

/* Functions */
SAVEDS ASM ULONG MainBar_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg);

struct MUI_CustomClass *CL_Project;

/*
** MainBar_New
**
** Methode die aus der OM_NEW Message ausgerufen wird und das MainBar Objekt
** neu erzeugt
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class * : Die aufrufende Klasse
** Object *: Das Objekt
** Msg     : Die Message von OM_GET
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** METHOD: Das neue Objekt oder NULL
*/

METHOD MainBar_New(Class *cl, Object *obj, struct opSet *msg)
{
  static struct MUIP_Toolbar_Description TB_Main_Sources_desc[] =
  {
    { TDT_BUTTON, 0, TDF_GHOSTED, 0,  "Open a new source file window",          0},
    { TDT_BUTTON, 0, TDF_GHOSTED, 0,  "Load text in a new source file window",  0},
    { TDT_BUTTON, 0, TDF_GHOSTED, 0,  "Save active source file",                0},
    Toolbar_End
  };

  static struct MUIP_Toolbar_Description TB_Main_Projects_desc[] =
  {
    { TDT_BUTTON, 0, 0,           0,  "Create a new project",     0},
    { TDT_BUTTON, 0, 0,           0,  "Load Project file",        0},
    { TDT_BUTTON, 0, TDF_GHOSTED, 0,  "Save Project...",          0},
    Toolbar_End
  };

  static struct MUIP_Toolbar_Description TB_Main_Compiler_desc[] =
  {
    { TDT_BUTTON, 0, TDF_GHOSTED, 0,  "Compile project...",         0},
    { TDT_BUTTON, 0, TDF_GHOSTED, 0,  "Compile project and start",  0},
    { TDT_BUTTON, 0, TDF_GHOSTED, 0,  "Compile project and debug",  0},
    Toolbar_End
  };

  enum{ TB_BT_NEWSOURCE=1, TB_BT_LOADTEXT, TB_BT_SAVETEXT, TB_BT_NEWPROJECT, TB_BT_LOADPROJECT, TB_BT_SAVEPROJECT, TB_BT_COMPILE, TB_BT_COMPILERUN,
        TB_BT_COMPILEDEBUG, MN_PROJECT, MN_NEW, MN_OPEN, MN_SAVE, MN_SAVEAS, MN_SAVEASTMPL, MN_SAVEALL,
        MN_ABOUT, MN_ABOUT_MUI, MN_QUIT, CLOSE_WIN_ABOUT, CLOSE_WIN_PRGSETTINGS, CLOSE_WIN_PROJECT, CLOSE_WIN_PROJECT_ASS,
        BT_PRJ_CONTINUE, BT_PRJ_ABORT
      };

  struct NewMenu NM_MainBar[] =
  {
    /* Type     Label                       Key   Flags             MX  UserData */
    { NM_TITLE, "Project",                  0,    0,                0,  (APTR)MN_PROJECT,     },
    { NM_ITEM,  "New",                      "N",  0,                0,  (APTR)MN_NEW,         },
    { NM_ITEM,  "Open...",                  "O",  0,                0,  (APTR)MN_OPEN,        },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "Save",                     "S",  NM_ITEMDISABLED,  0,  (APTR)MN_SAVE,        },
    { NM_ITEM,  "Save As...",               0,    NM_ITEMDISABLED,  0,  (APTR)MN_SAVEAS,      },
    { NM_ITEM,  "Save As Template...",      0,    NM_ITEMDISABLED,  0,  (APTR)MN_SAVEASTMPL,  },
    { NM_ITEM,  "Save All",                 0,    0,                0,  (APTR)MN_SAVEALL,     },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "About...",                 "I",  0,                0,  (APTR)MN_ABOUT,       },
    { NM_ITEM,  "About MUI...",             0,    0,                0,  (APTR)MN_ABOUT_MUI,   },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "Quit",                     "Q",  0,                0,  (APTR)MN_QUIT,        },

    { NM_END, NULL,                         0,    0,                0,  (APTR)NULL,           }
  };

  struct MainBar_Data Temp, *Data;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

//   LocalizeStringArray(CYA_Command);
//   LocalizeNewMenu(NM_MainBar);

  obj = (Object *)DoSuperNew(cl, obj,
    MUIA_Window_Title,  VERS,
    MUIA_Window_ID,     MAKE_ID('M','A','I','N'),
    MUIA_Window_Menustrip, Temp.MN_Main = MUI_MakeObject(MUIO_MenustripNM, NM_MainBar, 0),
    WindowContents,     VGroup,
      Child,  HGroup,
        Child, Temp.TB_Main_Sources  = ToolbarObject,
          MUIA_Toolbar_ImageType,   MUIV_Toolbar_ImageType_File,
          MUIA_Toolbar_ImageNormal, "PROGDIR:IMG/Sources.img",
          MUIA_Toolbar_Description, TB_Main_Sources_desc,
          MUIA_Font,                MUIV_Font_Tiny,
          MUIA_ShortHelp,           FALSE,
          MUIA_Draggable,           FALSE,
        End,
        Child, Temp.TB_Main_Projects  = ToolbarObject,
          MUIA_Toolbar_ImageType,   MUIV_Toolbar_ImageType_File,
          MUIA_Toolbar_ImageNormal, "PROGDIR:IMG/Projects.img",
          MUIA_Toolbar_ImageGhost,  "PROGDIR:IMG/ProjectsGhost.img",
          MUIA_Toolbar_Description, TB_Main_Projects_desc,
          MUIA_Font,                MUIV_Font_Tiny,
          MUIA_ShortHelp,           FALSE,
          MUIA_Draggable,           FALSE,
        End,
        Child, Temp.TB_Main_Compiler  = ToolbarObject,
          MUIA_Toolbar_ImageType,   MUIV_Toolbar_ImageType_File,
          MUIA_Toolbar_ImageNormal, "PROGDIR:IMG/Compiler.img",
          MUIA_Toolbar_ImageGhost,  "PROGDIR:IMG/CompilerGhost.img",
          MUIA_Toolbar_Description, TB_Main_Compiler_desc,
          MUIA_Font,                MUIV_Font_Tiny,
          MUIA_ShortHelp,           FALSE,
          MUIA_Draggable,           FALSE,
        End,
      End,
      Child, MUI_MakeObject(MUIO_HBar, 1),
      Child, Temp.TX_Status = InfoTextObject,
        /*  MUIA_Text_SetVMax,  FALSE, */
      End,
    End,
  End;

  if (!obj) { return(NULL); }

  Data = INST_DATA(cl, obj);
  *Data = Temp;

  Data->WI_PrjAss = NULL;
  Data->LI_Projects = ListObject,
  End;

//  Data->CreatorName = (STRPTR)GetTagData(MUIA_MainBar_Creator, NULL, msg->ops_AttrList);

  /*
  ** Verbinde die Strings der Toolbars mit denen der InfoText.mcc Klasse
  */

  DoMethod( Temp.TB_Main_Sources, MUIM_Notify, MUIA_Toolbar_HelpString, MUIV_EveryTime,
            Temp.TX_Status, 3, MUIM_Set, MUIA_Text_Contents, MUIV_TriggerValue);

  DoMethod( Temp.TB_Main_Projects, MUIM_Notify, MUIA_Toolbar_HelpString, MUIV_EveryTime,
            Temp.TX_Status, 3, MUIM_Set, MUIA_Text_Contents, MUIV_TriggerValue);

  DoMethod( Temp.TB_Main_Compiler, MUIM_Notify, MUIA_Toolbar_HelpString, MUIV_EveryTime,
            Temp.TX_Status, 3, MUIM_Set, MUIA_Text_Contents, MUIV_TriggerValue);

  /*
  ** Kommandos für die Buttons in der ToolBar setzen
  */

  DoMethod( Temp.TB_Main_Sources, MUIM_Toolbar_Notify, 0, MUIV_Toolbar_Notify_Pressed, FALSE,
            obj, 2, MUIM_Application_ReturnID, TB_BT_NEWSOURCE);

  DoMethod( Temp.TB_Main_Sources, MUIM_Toolbar_Notify, 1, MUIV_Toolbar_Notify_Pressed, FALSE,
            obj, 2, MUIM_Application_ReturnID, TB_BT_LOADTEXT);

  DoMethod( Temp.TB_Main_Sources, MUIM_Toolbar_Notify, 2, MUIV_Toolbar_Notify_Pressed, FALSE,
            obj, 2, MUIM_Application_ReturnID, TB_BT_SAVETEXT);


  DoMethod( Temp.TB_Main_Projects, MUIM_Toolbar_Notify, 0, MUIV_Toolbar_Notify_Pressed, FALSE,
            WI_AmIDE_ProjectAss, 3, MUIM_Set, MUIA_Window_Open, TRUE);

  DoMethod( Temp.TB_Main_Projects, MUIM_Toolbar_Notify, 1, MUIV_Toolbar_Notify_Pressed, FALSE,
            obj, 1, MUIM_MainBar_LoadProject);

  DoMethod( Temp.TB_Main_Projects, MUIM_Toolbar_Notify, 2, MUIV_Toolbar_Notify_Pressed, FALSE,
            obj, 1, MUIM_MainBar_PM_Project_Save);


  DoMethod( Temp.TB_Main_Compiler, MUIM_Toolbar_Notify, 0, MUIV_Toolbar_Notify_Pressed, FALSE,
            obj, 1, MUIM_MainBar_PM_Project_Compile);

  DoMethod( Temp.TB_Main_Compiler, MUIM_Toolbar_Notify, 1, MUIV_Toolbar_Notify_Pressed, FALSE,
            obj, 1, MUIM_MainBar_PM_Project_CompileRun);

  DoMethod( Temp.TB_Main_Compiler, MUIM_Toolbar_Notify, 2, MUIV_Toolbar_Notify_Pressed, FALSE,
            obj, 1, MUIM_MainBar_PM_Project_Debug);

//   DoMethod(obj, MUIM_MainBar_Ghostings);
//   DoMethod(obj, MUIM_MainBar_CYCmdState);

   // Notify Window
  DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
           obj, 1, MUIM_MainBar_Finish
          );

  // Notify Menus
  DoMethod(FINDOBJ(Temp.MN_Main, MN_NEW),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           WI_AmIDE_ProjectAss,  3,  MUIM_Set, MUIA_Window_Open, TRUE
          );

  DoMethod(FINDOBJ(Temp.MN_Main, MN_ABOUT),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           WI_AmIDE_About,  3,  MUIM_Set, MUIA_Window_Open, TRUE
          );

  DoMethod(FINDOBJ(Temp.MN_Main, MN_ABOUT_MUI),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           MUIV_Notify_Application, 2, MUIM_Application_AboutMUI, obj
          );

  DoMethod(FINDOBJ(Temp.MN_Main, MN_QUIT),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj, 1, MUIM_MainBar_Finish
          );

  return ((ULONG)obj);
}

/*
** MainBar_DelProject
** Methode die das übergebene Objekt aus der Projektliste entfernt
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

METHOD MainBar_DelProject(Class *cl, Object *obj, struct MUIP_MainBar_DelProject_Message *msg)
{
  struct MainBar_Data *Data = INST_DATA(cl, obj);

  LONG entries = xget(Data->LI_Projects, MUIA_List_Entries);
  int i;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  for(i=0; i < entries; i++)
  {
    Object *tmp_obj;

    DoMethod(Data->LI_Projects, MUIM_List_GetEntry, i, &tmp_obj);

    if(msg->obj_project == tmp_obj)
    {
      DoMethod(Data->LI_Projects, MUIM_List_Remove, i);
      break;
    }
  }

  // Dann scannen wir die Projektliste neu durch und setzen die Toolbars
  DoMethod(obj, MUIM_MainBar_Scan_List);

  return(0);
}

/*
** MainBar_Finish
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

METHOD MainBar_Finish(Class *cl, Object *obj, Msg msg)
{
  struct MainBar_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  DoMethod(_app(obj), MUIM_Application_ReturnID,
                      MUIV_Application_ReturnID_Quit
          );

  return 0;
}

/*
** MainBar_LoadProject
** Methode die einen ASL-Requester öffnet, einen Dateinamen abfrägt und diesen dann
** an die MUIM_Project_Load Methode des Projektes übergibt, das Fenster öffnet und die
** Projektliste neu scannt
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

METHOD MainBar_LoadProject(Class *cl, Object *obj, Msg msg)
{
  struct MainBar_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Nun das Projekt erzeugen (ohne Assistenten)
  DoMethod(obj, MUIM_MainBar_NewProject, 0);

  if (AslBase = OpenLibrary(AslName, 36L))
  {
    struct FileRequester *req;

    if (req = MUI_AllocAslRequestTags(  ASL_FileRequest,
                                        ASLFR_TitleText,      "Choose projectfile",
                                        ASLFR_InitialDrawer,  "PROGDIR:",
                                        ASLFR_DoMultiSelect,  FALSE,
                                        ASLFR_DoPatterns,     TRUE,
                                        ASLFR_InitialPattern, "#?.¶",
                                        TAG_DONE, 0)){

      if (MUI_AslRequestTags(req, TAG_DONE, 0))
      {
        char filename[256];

        strcpy(filename,  req->fr_Drawer);
        AddPart(filename, req->fr_File, 255);

        // Dann öffnen wir das Fenster
        set(Data->WI_Act_Project, MUIA_Window_Open, TRUE);

        // Nun die Daten aus dem Projekt laden
        DoMethod(Data->WI_Act_Project, MUIM_Project_Load, filename);

        // Dann scannen wir die Projektliste neu durch und setzen die Toolbars
        DoMethod(obj, MUIM_MainBar_Scan_List);

      }else
      {
        // Wenn das Requester gecanceled wurde dann vernichten wir das Objekt wieder
        DoMethod(Data->WI_Act_Project, MUIM_Project_Finish);
      }
      MUI_FreeAslRequest(req);
    }
    CloseLibrary(AslBase);
  }

  return(0);
}

/*
** MainBar_NewProject
** Methode die aus den Daten des Projektassistenten ein neues Projekt erzeugt
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

METHOD MainBar_NewProject(Class *cl, Object *obj, struct MUIP_MainBar_NewProject_Message *msg)
{
  struct MainBar_Data *Data = INST_DATA(cl, obj);

  STRPTR Prj_Dir  = (STRPTR)xget(STR_ProjectAss_Directory,  MUIA_String_Contents);
  STRPTR Prj_Name = (STRPTR)xget(STR_ProjectAss_Name,       MUIA_String_Contents);
  int    Prj_Type = xget(RB_ProjectAss_Type,                MUIA_Radio_Active);
  int    Prj_Cmpl = xget(RB_ProjectAss_Compiler,            MUIA_Radio_Active);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(msg->use_assistent)
  {
    // Nun checken wir erstmal die Eingaben im Projekt-Assistenten
    if(strlen(Prj_Dir) == 0 || strlen(Prj_Name) == 0) return(0);

    // Jetzt das Fenster schliessen
    set(WI_AmIDE_ProjectAss, MUIA_Window_Open, FALSE);
  }

  // Erzeugen des ProjectObjects anhand der Angaben im ProjektAssistenten
  Data->WI_Act_Project =  ProjectObject,
                            MUIA_Project_Number,    (xget(Data->LI_Projects, MUIA_List_Entries)+1),
                            MUIA_Project_Directory, Prj_Dir,
                            MUIA_Project_Name,      Prj_Name,
                            MUIA_Project_Type,      Prj_Type,
                            MUIA_Project_Compiler,  Prj_Cmpl,
                          End;

  if (Data->WI_Act_Project)
  {
    DoMethod(_app(obj), OM_ADDMEMBER, Data->WI_Act_Project);

    // Nun sorgen wir noch dafür das die ChildObjekte geadded werden
    DoMethod(Data->WI_Act_Project, MUIM_Project_Add_ChildObj);

    // Das neue Projekt zur Liste hinzufügen
    DoMethod(Data->LI_Projects, MUIM_List_InsertSingle, Data->WI_Act_Project, MUIV_List_Insert_Bottom);

    if(msg->use_assistent)
    {
      // Jetzt laden wir die Default-Filetypen ein
      DoMethod(Data->WI_Act_Project, MUIM_Project_FileType_Default);

      // Dann scannen wir die Projektliste neu durch und setzen die Toolbars
      DoMethod(obj, MUIM_MainBar_Scan_List);

      // Das Fenster nun öffnen
      set(Data->WI_Act_Project, MUIA_Window_Open, TRUE);

    }

  }
  else
  {
    DisplayBeep(_screen(obj));
  }

  return(0);
}

/*
** MainBar_PM_Project_Compile
** Methode die ein PopupMenu zur Auswahl des Projektes erzeugt und darstellt
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

METHOD MainBar_PM_Project_Compile(Class *cl, Object *obj, Msg msg)
{
  struct MainBar_Data *Data = INST_DATA(cl, obj);

  struct PopupMenuBase  *PopupMenuBase;
  struct PopupMenu      *pop_menu;
  struct Window         *win;
  int    i;
  LONG   entries = xget(Data->LI_Projects, MUIA_List_Entries);
  Object *ret_obj;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Nun holen wir die Window Struktur
  get(obj, MUIA_Window_Window, &win);
  if(!win) return(0);

  // Open the popupmenu.library
  PopupMenuBase = (struct PopupMenuBase *)OpenLibrary(POPUPMENU_NAME, POPUPMENU_VERSION);
  if(PopupMenuBase)
  {
    pop_menu =  PMMenu("Compile Project"),
                End;

    // Dann scannen wir die Projektliste durch und fügen Sie an das PopUpMenu an
    for(i=0; i < entries; i++)
    {
      Object *tmp_obj;

      DoMethod(Data->LI_Projects, MUIM_List_GetEntry, i, &tmp_obj);

      PM_InsertMenuItem(  pop_menu,
                          PM_Insert_Last, TRUE,
                          PM_Insert_Item, PM_MakeItem(PM_Title, xget(tmp_obj, MUIA_Project_Name),
                                                      PM_UserData,  tmp_obj,
                                                      TAG_DONE),
                          TAG_DONE);
    }

    ret_obj = (Object *)(PM_OpenPopupMenu(  win, PM_Menu,    pop_menu, TAG_DONE));

    PM_FreePopupMenu(pop_menu);
    CloseLibrary((struct Library *)PopupMenuBase);

    if(ret_obj)
    {
      // Dann einfach die SaveMethode des Objektes aufrufen
      DoMethod(ret_obj, MUIM_Project_Make);
    }
  }

  return(0);
}

/*
** MainBar_PM_Project_CompileRun
** Methode die ein PopupMenu zur Auswahl des Projektes erzeugt und darstellt
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

METHOD MainBar_PM_Project_CompileRun(Class *cl, Object *obj, Msg msg)
{
  struct MainBar_Data *Data = INST_DATA(cl, obj);

  struct PopupMenuBase  *PopupMenuBase;
  struct PopupMenu      *pop_menu;
  struct Window         *win;
  int    i;
  LONG   entries = xget(Data->LI_Projects, MUIA_List_Entries);
  Object *ret_obj;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Nun holen wir die Window Struktur
  get(obj, MUIA_Window_Window, &win);
  if(!win) return(0);

  // Open the popupmenu.library
  PopupMenuBase = (struct PopupMenuBase *)OpenLibrary(POPUPMENU_NAME, POPUPMENU_VERSION);
  if(PopupMenuBase)
  {
    pop_menu =  PMMenu("Compile&Run Project"),
                End;

    // Dann scannen wir die Projektliste durch und fügen Sie an das PopUpMenu an
    for(i=0; i < entries; i++)
    {
      Object *tmp_obj;

      DoMethod(Data->LI_Projects, MUIM_List_GetEntry, i, &tmp_obj);

      PM_InsertMenuItem(  pop_menu,
                          PM_Insert_Last, TRUE,
                          PM_Insert_Item, PM_MakeItem(PM_Title, xget(tmp_obj, MUIA_Project_Name),
                                                      PM_UserData,  tmp_obj,
                                                      TAG_DONE),
                          TAG_DONE);
    }

    ret_obj = (Object *)(PM_OpenPopupMenu(  win, PM_Menu,    pop_menu, TAG_DONE));

    PM_FreePopupMenu(pop_menu);
    CloseLibrary((struct Library *)PopupMenuBase);

    if(ret_obj)
    {
      DoMethod(ret_obj, MUIM_Project_Make);
      DoMethod(ret_obj, MUIM_Project_RunExe);
    }
  }

  return(0);
}

/*
** MainBar_PM_Project_Debug
** Methode die ein PopupMenu zur Auswahl des Projektes erzeugt und darstellt
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

METHOD MainBar_PM_Project_Debug(Class *cl, Object *obj, Msg msg)
{
  struct MainBar_Data *Data = INST_DATA(cl, obj);

  struct PopupMenuBase  *PopupMenuBase;
  struct PopupMenu      *pop_menu;
  struct Window         *win;
  int    i;
  LONG   entries = xget(Data->LI_Projects, MUIA_List_Entries);
  ULONG  ret = 0;

  enum{ PMN_COMPILE=1, PMN_MAKE, PMN_MAKEALL };

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Nun holen wir die Window Struktur
  get(obj, MUIA_Window_Window, &win);
  if(!win) return(0);

  // Open the popupmenu.library
  PopupMenuBase = (struct PopupMenuBase *)OpenLibrary(POPUPMENU_NAME, POPUPMENU_VERSION);
  if(PopupMenuBase)
  {
    pop_menu =  PMMenu("Debug Project"),
                End;

    // Dann scannen wir die Projektliste durch und fügen Sie an das PopUpMenu an
    for(i=0; i < entries; i++)
    {
      Object *tmp_obj;

      DoMethod(Data->LI_Projects, MUIM_List_GetEntry, i, &tmp_obj);

      PM_InsertMenuItem(  pop_menu,
                          PM_Insert_Last, TRUE,
                          PM_Insert_Item, PM_MakeItem(PM_Title, xget(tmp_obj, MUIA_Project_Name), TAG_DONE),
                          TAG_DONE);
    }

    ret = (ULONG)(PM_OpenPopupMenu(  win, PM_Menu,    pop_menu, TAG_DONE));

    PM_FreePopupMenu(pop_menu);
    CloseLibrary((struct Library *)PopupMenuBase);

    switch(ret)
    {
      case PMN_COMPILE: return(0);
      case PMN_MAKE:    return(0);
      case PMN_MAKEALL: return(0);
    }
  }

  return(0);
}

/*
** MainBar_PM_Project_Save
** Methode die ein PopupMenu zur Auswahl des Projektes erzeugt und darstellt
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

METHOD MainBar_PM_Project_Save(Class *cl, Object *obj, Msg msg)
{
  struct MainBar_Data *Data = INST_DATA(cl, obj);

  struct  PopupMenuBase  *PopupMenuBase;
  struct  PopupMenu      *pop_menu;
  struct  Window         *win;
  int     i;
  LONG    entries = xget(Data->LI_Projects, MUIA_List_Entries);
  Object *ret_obj;

  enum{ PMN_COMPILE=1, PMN_MAKE, PMN_MAKEALL };

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Nun holen wir die Window Struktur
  get(obj, MUIA_Window_Window, &win);
  if(!win) return(0);

  // Open the popupmenu.library
  PopupMenuBase = (struct PopupMenuBase *)OpenLibrary(POPUPMENU_NAME, POPUPMENU_VERSION);
  if(PopupMenuBase)
  {
    pop_menu =  PMMenu("Save Project"),
                End;

    // Dann scannen wir die Projektliste durch und fügen Sie an das PopUpMenu an
    for(i=0; i < entries; i++)
    {
      Object *tmp_obj;

      DoMethod(Data->LI_Projects, MUIM_List_GetEntry, i, &tmp_obj);

      PM_InsertMenuItem(  pop_menu,
                          PM_Insert_Last, TRUE,
                          PM_Insert_Item, PM_MakeItem(  PM_Title,     xget(tmp_obj, MUIA_Project_Name),
                                                        PM_UserData,  tmp_obj,
                                                     TAG_DONE),
                          TAG_DONE);
    }

    ret_obj = (Object *)(PM_OpenPopupMenu(  win, PM_Menu,    pop_menu, TAG_DONE));

    PM_FreePopupMenu(pop_menu);
    CloseLibrary((struct Library *)PopupMenuBase);

    if(ret_obj)
    {
      // Dann einfach die SaveMethode des Objektes aufrufen
      DoMethod(ret_obj, MUIM_Project_Save);
    }
  }

  return(0);
}

/*
** MainBar_Scan_List
** Methode die die Toolbars auf ghosted/unghosted stellt falls die Projektliste
** leer/>0 ist
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

METHOD MainBar_Scan_List(Class *cl, Object *obj, Msg msg)
{
  struct MainBar_Data *Data = INST_DATA(cl, obj);

  LONG   entries = xget(Data->LI_Projects, MUIA_List_Entries);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(entries == 0)
  {
    DoMethod(Data->TB_Main_Projects, MUIM_Toolbar_Set, 2, MUIV_Toolbar_Set_Ghosted, TRUE);
    DoMethod(Data->TB_Main_Compiler, MUIM_Toolbar_Set, 0, MUIV_Toolbar_Set_Ghosted, TRUE);
    DoMethod(Data->TB_Main_Compiler, MUIM_Toolbar_Set, 1, MUIV_Toolbar_Set_Ghosted, TRUE);
    DoMethod(Data->TB_Main_Compiler, MUIM_Toolbar_Set, 2, MUIV_Toolbar_Set_Ghosted, TRUE);
  }else
  {
    DoMethod(Data->TB_Main_Projects, MUIM_Toolbar_Set, 2, MUIV_Toolbar_Set_Ghosted, FALSE);
    DoMethod(Data->TB_Main_Compiler, MUIM_Toolbar_Set, 0, MUIV_Toolbar_Set_Ghosted, FALSE);
    DoMethod(Data->TB_Main_Compiler, MUIM_Toolbar_Set, 1, MUIV_Toolbar_Set_Ghosted, FALSE);
    DoMethod(Data->TB_Main_Compiler, MUIM_Toolbar_Set, 2, MUIV_Toolbar_Set_Ghosted, TRUE);  // Funktioniert noch nicht
  }

  return(0);
}


/*
** MainBar_Dispatcher
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

SAVEDS ASM ULONG MainBar_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  switch (msg->MethodID)
  {
    case OM_NEW                             : return(MainBar_New                  (cl, obj, (APTR)msg));
    case MUIM_MainBar_Finish                : return(MainBar_Finish               (cl, obj, (APTR)msg));
    case MUIM_MainBar_NewProject            : return(MainBar_NewProject           (cl, obj, (APTR)msg));
    case MUIM_MainBar_DelProject            : return(MainBar_DelProject           (cl, obj, (APTR)msg));
    case MUIM_MainBar_Scan_List             : return(MainBar_Scan_List            (cl, obj, (APTR)msg));
    case MUIM_MainBar_PM_Project_Save       : return(MainBar_PM_Project_Save      (cl, obj, (APTR)msg));
    case MUIM_MainBar_PM_Project_Compile    : return(MainBar_PM_Project_Compile   (cl, obj, (APTR)msg));
    case MUIM_MainBar_PM_Project_CompileRun : return(MainBar_PM_Project_CompileRun(cl, obj, (APTR)msg));
    case MUIM_MainBar_PM_Project_Debug      : return(MainBar_PM_Project_Debug     (cl, obj, (APTR)msg));
    case MUIM_MainBar_LoadProject           : return(MainBar_LoadProject          (cl, obj, (APTR)msg));
  }

  return (DoSuperMethodA(cl, obj, msg));
}

/*
** CreateMainBarClass()
** Funktion zum erstellen der FileInfo Klasse und zuweisen der Dispatcherfunktion
** Vorher wird dann noch die ProjectKlasse erzeugt
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** struct MUI_CustomClass: Die fertig erzeugte Klasse
*/

struct MUI_CustomClass *CreateMainBarClass()
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

   if (CL_Project = CreateProjectClass())
   {
      struct MUI_CustomClass *CL_MainBar;

      CL_MainBar = MUI_CreateCustomClass(NULL,
                                         MUIC_Window, NULL,
                                         sizeof(struct MainBar_Data),
                                         MainBar_Dispatcher
                                        );
      if (CL_MainBar)
      {
         return CL_MainBar;
      }
   }

   DeleteMainBarClass(NULL);
   return NULL;
}

/*
** DeleteMainBarClass()
** Funktion zum löschen/beenden der MainBar Klasse
** Und vorher noch die ProjectKlasse killen
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** struct MUI_CustomClass *: Pointer auf die zu löschende Klasse
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** keine
*/

void DeleteMainBarClass(struct MUI_CustomClass *CL_MainBar)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

   if (CL_MainBar)
   {
      MUI_DeleteCustomClass(CL_MainBar);
   }

   if (CL_Project)
   {
      DeleteProjectClass(CL_Project);
      CL_Project = NULL;
   }
}

