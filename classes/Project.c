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
#include "FileInfo.h"
#include "FileType.h"
#include "FileTypeTree.h"
#include "Main.h"
#include "MainBar.h"
#include "MsgBrowser.h"
#include "Project.h"
#include "ProjectTree.h"

/* Functions */
SAVEDS ASM ULONG Project_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg);

/* Structures */
struct MUI_CustomClass *CL_ProjectTree;
struct MUI_CustomClass *CL_MsgBrowser;
struct MUI_CustomClass *CL_FileInfo;
struct MUI_CustomClass *CL_FileType;

/*
** Project_New
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

static METHOD Project_New(Class *cl, Object *obj, struct opSet *msg)
{
  struct Project_Data *Data;
  struct Project_Data Temp;
  STRPTR tmp_str;

  // Initialisieren der Toolbars
  static struct MUIP_Toolbar_Description TB_Project_L[] =
  {
    { TDT_BUTTON, 0, 0, 0,  "Add File/Drawer to list...",   0},
    { TDT_BUTTON, 0, 0, 0,  "Remove File/Drawer from list", 0},
    { TDT_BUTTON, 0, 0, 0,  "Show detailed information",    0},
    { TDT_BUTTON, 0, 0, 0,  "Touch file",                   0},
    Toolbar_End
  };

  static struct MUIP_Toolbar_Description TB_Project_R[] =
  {
    { TDT_BUTTON, 0, 0, 0,  "Compile/Make project",              0},
    Toolbar_End
  };

  enum{ MN_PROJECT=1, MN_OPEN, MN_SAVE, MN_SAVEAS, MN_SAVEASTMPL, MN_PRINT, MN_ADDFILE, MN_ADDLIB,
        MN_CHOOSEEXE, MN_RENAMEFILE, MN_DELETEFILE, MN_CLOSE, MN_ABOUT, MN_ABOUT_MUI, MN_QUIT,
        MN_ERASE, MN_MOVEUP, MN_MOVEDOWN, MN_COMPILE, MN_MAKE, MN_MAKEALL, MN_MAKEHA, MN_MAKEALLHA,
        MN_RUN, MN_DEBUG, MN_TOUCH, MN_TOUCHALL, MN_TOUCHALLHA, MN_NEXTERROR, MN_PREVERROR,
        MN_SELECTMAKE, MN_REMOVEMAKE, MN_EDITMAKE, MN_PROJENV, MN_FILETYPE, MN_SCOPTS, MN_PRGSETTINGS
      };

  struct NewMenu NM_Project[] =
  {
    /* Type     Label                       Key   Flags             MX  UserData */
    { NM_TITLE, "Project",                  0,    0,                0,  (APTR)MN_PROJECT,     },
    { NM_ITEM,  "Open...",                  "O",  0,                0,  (APTR)MN_OPEN,        },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "Save",                     "S",  0,                0,  (APTR)MN_SAVE,        },
    { NM_ITEM,  "Save As...",               0,    NM_ITEMDISABLED,  0,  (APTR)MN_SAVEAS,      },
    { NM_ITEM,  "Save As Template...",      0,    NM_ITEMDISABLED,  0,  (APTR)MN_SAVEASTMPL,  },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "Print",                    0,    NM_ITEMDISABLED,  0,  (APTR)MN_PRINT,       },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "Add File(s)...",           0,    0,                0,  (APTR)MN_ADDFILE,     },
    { NM_ITEM,  "Add Library(s)...",        0,    NM_ITEMDISABLED,  0,  (APTR)MN_ADDLIB,      },
    { NM_ITEM,  "Select Executable...",     "W",  0,                0,  (APTR)MN_CHOOSEEXE,   },
    { NM_ITEM,  "Rename File...",           "U",  NM_ITEMDISABLED,  0,  (APTR)MN_RENAMEFILE,  },
    { NM_ITEM,  "Delete File...",           0,    NM_ITEMDISABLED,  0,  (APTR)MN_DELETEFILE,  },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "Close",                    "K",  0,                0,  (APTR)MN_CLOSE,       },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "About...",                 "I",  0,                0,  (APTR)MN_ABOUT,       },
    { NM_ITEM,  "About MUI...",             0,    0,                0,  (APTR)MN_ABOUT_MUI,   },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "Quit",                     "Q",  0,                0,  (APTR)MN_QUIT,        },

    { NM_TITLE, "Edit",                     0,    0,                0,  (APTR)NULL,           },
    { NM_ITEM,  "Erase",                    0,    NM_ITEMDISABLED,  0,  (APTR)MN_ERASE,       },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "Move Up",                  0,    NM_ITEMDISABLED,  0,  (APTR)MN_MOVEUP,      },
    { NM_ITEM,  "Move Down",                0,    NM_ITEMDISABLED,  0,  (APTR)MN_MOVEDOWN,    },

    { NM_TITLE, "Compile",                  0,    0,                0,  (APTR)NULL,           },
    { NM_ITEM,  "Compile...",               0,    0,                0,  (APTR)MN_COMPILE,     },
    { NM_ITEM,  "Make...",                  "M",  0,                0,  (APTR)MN_MAKE,        },
    { NM_ITEM,  "Make All...",              0,    0,                0,  (APTR)MN_MAKEALL,     },
    { NM_ITEM,  "Run...",                   "R",  0,                0,  (APTR)MN_RUN,         },
    { NM_ITEM,  "Debug...",                 "D",  NM_ITEMDISABLED,  0,  (APTR)MN_DEBUG,       },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "Touch",                    0,    0,                0,  (APTR)MN_TOUCH,       },
    { NM_ITEM,  "Touch All",                0,    0,                0,  (APTR)MN_TOUCHALL,    },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "Next Error",               "E",  NM_ITEMDISABLED,  0,  (APTR)MN_NEXTERROR,   },
    { NM_ITEM,  "Previous Error",           0,    NM_ITEMDISABLED,  0,  (APTR)MN_PREVERROR,   },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "Select Makescript...",     0,    NM_ITEMDISABLED,  0,  (APTR)MN_SELECTMAKE,  },
    { NM_ITEM,  "Remove Makescript",        0,    NM_ITEMDISABLED,  0,  (APTR)MN_REMOVEMAKE,  },
    { NM_ITEM,  "Edit Makescript",          0,    NM_ITEMDISABLED,  0,  (APTR)MN_EDITMAKE,    },

    { NM_TITLE, "Settings",                 0,    0,                0,  (APTR)NULL,           },
    { NM_ITEM,  "Project Environment...",   0,    0,                0,  (APTR)MN_PROJENV,     },
    { NM_ITEM,  "Project Filetypes...",     0,    0,                0,  (APTR)MN_FILETYPE,    },
    { NM_ITEM,  NM_BARLABEL,                0,    0,                0,  (APTR)NULL            },
    { NM_ITEM,  "Compiler & Linker",        0,    NM_ITEMDISABLED,  0,  (APTR)MN_SCOPTS,      },
    { NM_ITEM,  "AmIDE Settings...",        0,    NM_ITEMDISABLED,  0,  (APTR)MN_PRGSETTINGS, },

    { NM_END, NULL,                         0,    0,                0,  (APTR)NULL,           }
  };

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

//   LocalizeStringArray(CYA_Command);
//   LocalizeNewMenu(NM_Mainwin);

  // Nun die Attribute holen
  tmp_str                 = (STRPTR)GetTagData(MUIA_Project_Name,       NULL, msg->ops_AttrList);
  Temp.Project_Name       = calloc(strlen(tmp_str)+1, sizeof(char));
  strcpy(Temp.Project_Name, tmp_str);

  tmp_str                 = (STRPTR)GetTagData(MUIA_Project_Directory,  NULL, msg->ops_AttrList);
  Temp.Project_Directory  = calloc(strlen(tmp_str)+1, sizeof(char));
  strcpy(Temp.Project_Directory, tmp_str);

  Temp.Project_Type       = (int)GetTagData(MUIA_Project_Type,          NULL, msg->ops_AttrList);

  // Das Compilerobject holen
  DoMethod(LS_AmIDE_Modules, MUIM_List_GetEntry, (int)GetTagData(MUIA_Project_Compiler, NULL, msg->ops_AttrList), &Temp.Project_Compiler);

  Temp.Project_Number     = (int)GetTagData(MUIA_Project_Number,        NULL, msg->ops_AttrList);

  /*
  ** Anlegen der Jobliste
  */

  Temp.LS_Project_Joblist = ListObject,
                            End;

  if(!Temp.LS_Project_Joblist) return(NULL);

  // Nun erzeugen wir das Objekt
  obj = (Object *)DoSuperNew(cl, obj,
    MUIA_Window_ID,     MAKE_ID('P', 'R', 'J', Temp.Project_Number),
    MUIA_Window_Menustrip, Temp.MN_Project = MUI_MakeObject(MUIO_MenustripNM, NM_Project, 0),
    WindowContents,     VGroup,
      Child, HGroup,
        Child, Temp.TB_Project_Left_Toolbar  = ToolbarObject,
          MUIA_Toolbar_ImageType,   MUIV_Toolbar_ImageType_File,
          MUIA_Toolbar_ImageNormal, "PROGDIR:IMG/ProjectBar_Left.img",
          MUIA_Toolbar_Description, TB_Project_L,
          MUIA_Font,                MUIV_Font_Tiny,
          MUIA_ShortHelp,           FALSE,
          MUIA_Draggable,           FALSE,
        End,
      Child, HSpace(0),
        Child, Temp.TB_Project_Right_Toolbar  = ToolbarObject,
          MUIA_Toolbar_ImageType,   MUIV_Toolbar_ImageType_File,
          MUIA_Toolbar_ImageNormal, "PROGDIR:IMG/ProjectBar_Right.img",
          MUIA_Toolbar_Description, TB_Project_R,
          MUIA_Font,                MUIV_Font_Tiny,
          MUIA_ShortHelp,           FALSE,
          MUIA_Draggable,           FALSE,
        End,
      End,
      Child, Temp.TX_Project_Info = TextObject,
      End,
      Child, MUI_MakeObject(MUIO_HBar, 1),
      Child, NListviewObject,
        MUIA_CycleChain, TRUE,
        MUIA_Listview_Input, TRUE,
        MUIA_Listview_DragType,     MUIV_Listview_DragType_Immediate,
        MUIA_Listview_MultiSelect,  MUIV_Listview_MultiSelect_None,
        MUIA_NListview_NList,       Temp.LT_Project_Files = ProjectTreeObject,
        End,
      End,
    End,
  End;

  // Das FileTypeObjekt erzeugen
  Temp.WI_Project_FileType = FileTypeObject,
  End;

  // Das Environment-Fenster erzeugen
  Temp.WI_Project_Env = WindowObject,
    MUIA_Window_Title,  "Project Environment",
    MUIA_Window_ID,     MAKE_ID('E', 'N', 'V', Temp.Project_Number),
    WindowContents,     VGroup,
      Child, ColGroup(2),
        GroupFrameT("Project information"),

        Child, Label( "Name:" ),
        Child, Temp.BS_Project_Env_Name = BetterStringObject,
          StringFrame,
          MUIA_String_Contents, "",
          MUIA_Weight, 100,
          MUIA_String_MaxLen, 80,
          MUIA_String_Format, 0,
        End,

        Child, Label( "Directory:" ),
        Child, PopaslObject,
          MUIA_Popstring_String, Temp.BS_Project_Env_Dir = BetterStringObject,
            StringFrame,
            MUIA_String_Contents, "",
            MUIA_Weight,          100,
            MUIA_String_MaxLen,   256,
            MUIA_String_Format,   MUIV_String_Format_Left,
          End,
          MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
          ASLFR_TitleText  , "Please select a drawer...",
          ASLFR_DrawersOnly, TRUE,
        End,

      End,

      Child, ColGroup(3),

        Child, Temp.RB_Project_Env_PType = Radio("Project-Type:", CYA_Project_Type),

        Child, ColGroup(2),
          GroupFrameT("Standard Tools"),

          Child, Label("Edit Tool:"),
          Child, PopaslObject,
            MUIA_Popstring_String, Temp.BS_Project_Env_EditTool = BetterStringObject,
              StringFrame,
              MUIA_String_Contents, "",
              MUIA_Weight,          100,
              MUIA_String_MaxLen,   256,
              MUIA_String_Format,   MUIV_String_Format_Left,
            End,
            MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
            ASLFR_TitleText  , "Please select the edit tool...",
          End,

          Child, Label("View Tool:"),
          Child, PopaslObject,
            MUIA_Popstring_String, Temp.BS_Project_Env_ViewTool = BetterStringObject,
              StringFrame,
              MUIA_String_Contents, "",
              MUIA_Weight,          100,
              MUIA_String_MaxLen,   256,
              MUIA_String_Format,   MUIV_String_Format_Left,
            End,
            MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
            ASLFR_TitleText  , "Please select the view tool...",
          End,

        End,
      End,

      Child, ColGroup(2),
        Child, Temp.RB_Project_Env_Compiler = Radio("Compiler:",     CYA_Project_Compiler),
        Child, HSpace(0),
      End,

      Child, HGroup,
        Child, Temp.BT_Project_Env_Okay   = KeyButton("Okay",  "o"),
        Child, Temp.BT_Project_Env_Abort  = KeyButton("Abort", "a"),
      End,

    End,
  End;

  if (!obj) { return(NULL); }

  /* Nun Data instanzieren nachdem das Objekt erzeugt wurde */
  Data = INST_DATA(cl, obj);
  *Data = Temp;

  // Nun den WindowTitle erzeugen
  DoMethod(obj, MUIM_Project_Update_Title);

  // Den Creator für das Listtree setzen
  set(Data->LT_Project_Files, MUIA_ProjectTree_Creator, obj);

  // Initializieren
  Data->WI_Project_MsgBrowser   = NULL;
  Data->WI_Project_File_Info    = NULL;

  // Nun die Notifys für die Cursortasten ranhängen
  DoMethod( obj, MUIM_Notify, MUIA_Window_InputEvent, "-repeat down",
            Data->LT_Project_Files, 3, MUIM_Set, MUIA_List_Active, MUIV_List_Active_Down);

  DoMethod( obj, MUIM_Notify, MUIA_Window_InputEvent, "-repeat up",
            Data->LT_Project_Files, 3, MUIM_Set, MUIA_List_Active, MUIV_List_Active_Up);

  // Die Help-Texte der Toolbars mit dem InfoText verbinden
  DoMethod( Temp.TB_Project_Left_Toolbar, MUIM_Notify, MUIA_Toolbar_HelpString, MUIV_EveryTime,
            Temp.TX_Project_Info, 3, MUIM_Set, MUIA_Text_Contents, MUIV_TriggerValue);

  DoMethod( Temp.TB_Project_Right_Toolbar, MUIM_Notify, MUIA_Toolbar_HelpString, MUIV_EveryTime,
            Temp.TX_Project_Info, 3, MUIM_Set, MUIA_Text_Contents, MUIV_TriggerValue);

  // Notifys an die Toolbars hängen

  DoMethod( Temp.TB_Project_Left_Toolbar, MUIM_Toolbar_Notify, 0, MUIV_Toolbar_Notify_Pressed, FALSE,
            obj, 1, MUIM_Project_PM_Add);

  DoMethod( Temp.TB_Project_Left_Toolbar, MUIM_Toolbar_Notify, 1, MUIV_Toolbar_Notify_Pressed, FALSE,
            obj, 1, MUIM_Project_PM_Delete);

  DoMethod( Temp.TB_Project_Left_Toolbar, MUIM_Toolbar_Notify, 2, MUIV_Toolbar_Notify_Pressed, FALSE,
            obj, 2, MUIM_Project_File_Info_Open, 0);

  DoMethod( Temp.TB_Project_Left_Toolbar, MUIM_Toolbar_Notify, 3, MUIV_Toolbar_Notify_Pressed, FALSE,
            obj, 1, MUIM_Project_PM_Touch);

  DoMethod( Temp.TB_Project_Right_Toolbar, MUIM_Toolbar_Notify, 0, MUIV_Toolbar_Notify_Pressed, FALSE,
            obj, 1, MUIM_Project_PM_Compile);


  // Notify fuer das Listtree (DoubleClick)
  DoMethod( Data->LT_Project_Files, MUIM_Notify, MUIA_NListtree_DoubleClick, MUIV_EveryTime,
            obj,  1, MUIM_Project_File_DC
          );

  // Notify Windows
  DoMethod( obj,  MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            obj,  1,  MUIM_Project_Finish
          );

  DoMethod( Data->WI_Project_Env,  MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            Data->WI_Project_Env,  3,  MUIM_Set, MUIA_Window_Open, FALSE
          );

  DoMethod( Data->BT_Project_Env_Abort, MUIM_Notify, MUIA_Pressed, FALSE,
            Data->WI_Project_Env, 3, MUIM_Set, MUIA_Window_Open, FALSE
          );

  DoMethod( Data->BT_Project_Env_Okay, MUIM_Notify, MUIA_Pressed, FALSE,
            obj, 1, MUIM_Project_Env_Get
          );

  // Die Notifies für die Menueinträge
  DoMethod(FINDOBJ(Temp.MN_Project, MN_OPEN),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj,  1,  MUIM_Project_Load
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_SAVE),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj,  1,  MUIM_Project_Save
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_TOUCH),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj,  1,  MUIM_Project_File_Touch
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_TOUCHALL),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj,  1,  MUIM_Project_File_Touch_All
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_ADDFILE),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj,  1,  MUIM_Project_AddFile
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_CHOOSEEXE),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj,  1,  MUIM_Project_AddExe
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_COMPILE),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj,  1,  MUIM_Project_Compile
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_MAKE),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj,  1,  MUIM_Project_Make
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_MAKEALL),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj,  1,  MUIM_Project_Make_All
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_RUN),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj,  1,  MUIM_Project_RunExe
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_FILETYPE),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           Temp.WI_Project_FileType,  3,  MUIM_Set, MUIA_Window_Open, TRUE
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_PROJENV),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj, 1,  MUIM_Project_Env_Set
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_ABOUT),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           WI_AmIDE_About,  3,  MUIM_Set, MUIA_Window_Open, TRUE
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_ABOUT_MUI),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           MUIV_Notify_Application, 2, MUIM_Application_AboutMUI, obj
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_CLOSE),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj, 1, MUIM_Project_Finish
          );

  DoMethod(FINDOBJ(Temp.MN_Project, MN_QUIT),
           MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
           obj, 1, MUIM_Project_Finish
          );

  return ((ULONG)obj);
}

/*
** Project_Get
** Die OM_GET Methode des Objektes.
** Hierdurch werden die Attribute geholt
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class *        : Die Klasse des Objektes
** Object *       : Das Objekt
** struct opGet * : Pointer auf die opGet Struktur
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** 0
*/

static METHOD Project_Get(Class *cl, Object *obj, struct opGet *msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  switch (msg->opg_AttrID)
  {
    case MUIA_Project_Name      : { *(msg -> opg_Storage) = (ULONG)Data->Project_Name;       break; };
    case MUIA_Project_Directory : { *(msg -> opg_Storage) = (ULONG)Data->Project_Directory;  break; };
    default                     : { DoSuperMethodA(cl, obj, (Msg) msg); break; };
  }

  return(0);
}

/*
** Project_Add_ChildObj
**
** Methode um alle ChildObjekte der Applikation hinzuzufügen
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

METHOD Project_Add_ChildObj(Class *cl, Object *obj, Msg msg)
{
	struct Project_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

	if (Data->WI_Project_FileType)
	{
		DoMethod(_app(obj), OM_ADDMEMBER, Data->WI_Project_FileType);
	}

	if (Data->WI_Project_Env)
	{
		DoMethod(_app(obj), OM_ADDMEMBER, Data->WI_Project_Env);
	}

  return(0);
}

/*
** Project_AddExe
**
** Methode zum hinzufügen eines Eintrages der das Executeable file darstellt
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

METHOD Project_AddExe(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  int     i, mode=0;
  BPTR    section_fh;
  STRPTR  act_section;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if (AslBase = OpenLibrary(AslName, 36L))
  {
    struct FileRequester *req;

    if (req = MUI_AllocAslRequestTags(  ASL_FileRequest,
                                        ASLFR_TitleText,      "Choose executeable",
                                        ASLFR_InitialDrawer,  Data->Project_Directory,
                                        ASLFR_DoMultiSelect,  FALSE,
                                        TAG_DONE, 0)){

      if (MUI_AslRequestTags(req, TAG_DONE, 0))
      {
        STRPTR  filename;
        char    buffer[256];
        struct  FileType *type;

        filename = calloc(256, sizeof(char));

        strcpy(filename, req->fr_Drawer);
        AddPart(filename, req->fr_File, 255);

        DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Add_Exe, filename, "Program", 0, 0, 0, 0, 0, 1);
      }

      MUI_FreeAslRequest(req);
    }
    CloseLibrary(AslBase);
  }

  return(0);
}

/*
** Project_AddFile
** Methode die durch einen ASL-Requester es dem Benutzer ermöglicht Dateien auszuwählen
** die zum Projekt dazugehören sollen und diese dann über die MUIM_ProjectTree_Add_File
** Methode an das Listtree übergibt
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

METHOD Project_AddFile(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  int     i, mode=0;
  BPTR    section_fh;
  STRPTR  act_section;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if (AslBase = OpenLibrary(AslName, 36L))
  {
    struct FileRequester *req;

    if (req = MUI_AllocAslRequestTags(  ASL_FileRequest,
                                        ASLFR_TitleText,      "Choose file(s)...",
                                        ASLFR_InitialDrawer,  Data->Project_Directory,
                                        ASLFR_DoMultiSelect,  TRUE,
                                        ASLFR_DoPatterns,     TRUE,
                                        TAG_DONE, 0)){

      if (MUI_AslRequestTags(req, TAG_DONE, 0))
      {
        // Nun setzen wir das Listtree auf QUIET für das adden von vielen Dateien
        set(Data->LT_Project_Files, MUIA_NListtree_Quiet, TRUE);

        // Bei Multiple Selection mehrere Files Adden
        for(i=0; i < req->fr_NumArgs; i++)
        {
          STRPTR  filename;
          char    buffer[256];
          struct  FileType *type;

          filename = calloc(256, sizeof(char));

          strcpy(filename, req->fr_Drawer);
          AddPart(filename, (req->fr_ArgList[i]).wa_Name, 255);

          // Test des Identifies des FileType-Objektes
          type = (struct FileType *)DoMethod(Data->WI_Project_FileType, MUIM_FileType_Identify, filename, 0);

          // Wenn das File identifiziert wurde können wir es in die Liste adden
          if(type)
          {
            struct Entry_Data *entry;
            char buffer[256];

            sprintf(buffer, "Adding file \0338%s\0332 to folder \0338%s\0332 ....", FilePart(filename), type->name);
            set(Data->TX_Project_Info, MUIA_Text_Contents, buffer);

            entry = (struct Entry_Data *)DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Add_File_ByType, filename, type);

            if(type->scan_dep && entry)
            {
              DoMethod(obj, MUIM_Project_Scan_Dependency, filename, entry, TRUE);
            }

          }else
          {
            char buffer[256];

            sprintf(buffer, "Adding file \0338%s\0332 to folder \0338%s\0332 ....", FilePart(filename), "Others");
            set(Data->TX_Project_Info, MUIA_Text_Contents, buffer);

            DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Add_File, filename, "Others", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
          }

        }

        // Nun setzen wir das Listtree wieder zurück da wir alle Entries geadded haben
        set(Data->LT_Project_Files, MUIA_NListtree_Quiet, FALSE);
      }
      MUI_FreeAslRequest(req);
    }
    CloseLibrary(AslBase);
  }
  return(0);
}

/*
** Project_AddJob
** Funktion zum hinzufügen eines Jobs an die Jobliste
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** ULONG: Der Rückgabewert der Superklasse
*/

METHOD Project_AddJob(Class *cl, Object *obj, struct MUIP_Project_AddJob_Message *msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  struct MUIS_Project_Job *new_job;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

	// Speicher für die Struktur holen
  new_job = (struct MUIS_Project_Job *)calloc(1, sizeof(struct MUIS_Project_Job));
	if(!new_job) return(0);

	new_job->type 				= msg->type;
  new_job->OB_Compiler 	= Data->Project_Compiler;
  new_job->file 				= msg->file;
  new_job->exe 					= msg->exe;

	// Jetzt die Struktur an die Liste hängen
  return(DoMethod(Data->LS_Project_Joblist, MUIM_List_InsertSingle, new_job, MUIV_List_Insert_Bottom));
}

/*
** Project_Compile
**
** Methode um eine Datei zu kompilieren
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

METHOD Project_Compile(Class *cl, Object *obj, struct MUIP_Project_Compile_Message *msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  struct  Entry_Data *entry = msg->entry;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Wenn kein entry übergeben wurde
  if(!entry)
  {
    // Holen des aktiven Eintrages
    entry = (struct Entry_Data *)DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Get_Entry, MUIV_ProjectTree_Active, TAG_DONE);
    if(!entry) return(0);
  }

  if(entry->entry_type == ENTRY_TYPE_FILE && entry->entry_compileable)
  {
    // Das MsgBrowser-Fenster öffnen
    DoMethod(obj, MUIM_Project_MsgBrowser, TAG_DONE);

    // Den MsgBrowser nun löschen
    DoMethod(Data->WI_Project_MsgBrowser, MUIM_MsgBrowser_Clear, TAG_DONE);

    // Anhängen des COMPILE-Jobs an die Jobliste
    DoMethod(obj, MUIM_Project_AddJob, MUIV_Project_JOB_COMPILE, entry->entry_file, TAG_DONE);

    // Starten des Compilers
    DoMethod(obj, MUIM_Project_RunJob, TAG_DONE);
  }

  return(0);
}

/*
** Project_DelJob
** Funktion zum löschen des ersten Jobs in der Liste und event. das Objektfile in die
** Projektliste aufzunehmen
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** ULONG: Der Rückgabewert der Superklasse
*/

METHOD Project_DelJob(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  struct MUIS_Project_Job *job;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Erstmal den aktuellen Job holen
  DoMethod(Data->LS_Project_Joblist, MUIM_List_GetEntry, 0, &job, TAG_DONE);

  if(job)
  {
    // Jetzt schauen wir ob es ein Compile-Job war und wir ein Objekt-file hinzufügen müssen
    switch(job->type)
    {
      case MUIV_Project_JOB_COMPILE:
      {
        STRPTR tmp_str;
        STRPTR obj_file = (STRPTR)calloc(strlen(job->file)+1, sizeof(char));

        // Umkopieren
        strcpy(obj_file, job->file);

        tmp_str = strrchr(obj_file, '.');

        // Wenn noch genug Platz am String ist überschreibe ich das zeichen nach dem Punkt mit einem "o"
        if(strlen(tmp_str) > 1)
        {
          tmp_str++;
          *tmp_str = 'o';
          tmp_str++;
          *tmp_str = '\0';
        }

        // Wenn das Objektfile existiert dann fügen wir es auch hinzu
        if(FileSize(obj_file))
        {
          struct FileType *type;

          // Test des Identifies des FileType-Objektes
          type = (struct FileType *)DoMethod(Data->WI_Project_FileType, MUIM_FileType_Identify, obj_file, TAG_DONE);

          // Hinzufügen zum Project
          DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Add_File_ByType, obj_file, type, TAG_DONE);

        }else free(obj_file);
      }
      break;

      case MUIV_Project_JOB_LINK:
      {
        free(job->file);
      }
      break;
    }

    // Den Job freigeben
    free(job);

    // Nun löschen wir den Jobeintrag aus der Liste
    DoMethod(Data->LS_Project_Joblist, MUIM_List_Remove, MUIV_List_Remove_First, TAG_DONE);
  }

  return(0);
}

/*
** Project_Env_Get
**
** Methode die das Environment-Fenster ausliest und dann an die Variablen übergibt
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class *    		: Die Klasse des Objektes
** Object *   		: Das Objekt
** struct opGet *	: Pointer auf die opGet Struktur
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** 0
*/

METHOD Project_Env_Get(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

	// Setzen der Attribute
	get(Data->BS_Project_Env_Name, 			MUIA_String_Contents, &Data->Project_Name);
	get(Data->BS_Project_Env_Dir, 			MUIA_String_Contents, &Data->Project_Directory);
	get(Data->RB_Project_Env_PType, 		MUIA_Radio_Active, 		&Data->Project_Type);
	get(Data->BS_Project_Env_EditTool,	MUIA_String_Contents, &Data->Project_EditTool);
	get(Data->BS_Project_Env_ViewTool,	MUIA_String_Contents, &Data->Project_ViewTool);

	// Nun das richtige Compilermodul holen
  {
    int j;
		struct str_modules *module;

		get(Data->RB_Project_Env_Compiler,	MUIA_Radio_Active, 		&j);

		DoMethod(LS_AmIDE_Modules, MUIM_List_GetEntry, j, &module);
		if(!module) return(0);

		Data->Project_Compiler = module->lib_object;
 	}

	// Schliessen des Fensters
  set(Data->WI_Project_Env,						MUIA_Window_Open,			FALSE);

	return(0);
}

/*
** Project_Env_Set
**
** Methode die das Environment-Fenster öffnet und die Attribute setzt
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class *    		: Die Klasse des Objektes
** Object *   		: Das Objekt
** struct opGet *	: Pointer auf die opGet Struktur
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** 0
*/

METHOD Project_Env_Set(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

	// Setzen der Attribute
	set(Data->BS_Project_Env_Name, 			MUIA_String_Contents, Data->Project_Name);
	set(Data->BS_Project_Env_Dir, 			MUIA_String_Contents, Data->Project_Directory);
	set(Data->RB_Project_Env_PType, 		MUIA_Radio_Active, 		Data->Project_Type);
	set(Data->BS_Project_Env_EditTool,	MUIA_String_Contents, Data->Project_EditTool);
	set(Data->BS_Project_Env_ViewTool,	MUIA_String_Contents, Data->Project_ViewTool);
	set(Data->RB_Project_Env_Compiler,	MUIA_Radio_Active, 		GetModulePos(Data->Project_Compiler));

	// Öffnen des Fensters
  set(Data->WI_Project_Env,						MUIA_Window_Open,			TRUE);

	return(0);
}

/*
** Project_File_DC
**
** Methode die bei DoubleClick auf ein Objekt ausgeführt wird und schaut ob Edit oder
** ViewTool aufgerufen werden soll
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class *        : Die Klasse des Objektes
** Object *       : Das Objekt
** struct opGet * : Pointer auf die opGet Struktur
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** 0
*/

METHOD Project_File_DC(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);
  struct Entry_Data *entry;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Aktives Entry holen
  entry = (struct Entry_Data *)DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Get_Entry, MUIV_ProjectTree_Active);
  if(!entry) return(0);

  // Schauen ob Edit oder ViewTool
  if(entry->entry_edittool_sel == 0)
  {
    DoMethod(obj, MUIM_Project_File_Edit, entry, 0);

  }else
  {
    DoMethod(obj, MUIM_Project_File_View, entry, 0);
  }

  return(0);
}

/*
** Project_File_Edit
**
** Methode die das EditTool aufruft um eine Datei zu editieren
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class *        : Die Klasse des Objektes
** Object *       : Das Objekt
** struct opGet * : Pointer auf die opGet Struktur
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** 0
*/

METHOD Project_File_Edit(Class *cl, Object *obj, struct MUIP_Project_File_Edit_Message *msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);
  struct Entry_Data *entry = msg->entry;
  STRPTR edittool;
  char   command[1024];
  BPTR   fh, nil;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(!entry) return(0);

  // Schauen ob im entry selber das Tool benutzt werden soll oder default
  if(entry->entry_edittool_std)
  {
    edittool = Data->Project_EditTool;

  }else
  {
    edittool = entry->entry_edittool;
  }

  // Das Kommando nun zusammensetzen
  sprintf(command, "%s %s", edittool, entry->entry_file);

  // Das Kommando nun ausführen
  fh = Open("CON:0/20/640/100/AmIDE Output/AUTO/CLOSE/WAIT",   MODE_READWRITE);
  nil = Open("NIL:",  MODE_OLDFILE);

  // Dann das Executeable starten
  SystemTags( command,
              SYS_Input,      nil,
              SYS_Output,     fh,
              SYS_Asynch,     TRUE,
              NP_Priority,    0,
              TAG_DONE
            );

  return(0);
}

/*
** Project_File_Info
** Methode die ein FileInfoObjekt erzeugt und das Fenster öffnet
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

METHOD Project_File_Info(Class *cl, Object *obj, struct MUIP_Project_File_Info_Message *msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);
  struct Entry_Data *entry = msg->entry;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Wenn das Objekt noch nicht erzeugt wurde machen wir das jetzt
  if (Data->WI_Project_File_Info == NULL)
  {
    Data->WI_Project_File_Info = FileInfoObject,
    End;

    if (Data->WI_Project_File_Info)
    {
      DoMethod(_app(obj), OM_ADDMEMBER, Data->WI_Project_File_Info);

      // Notify um die Daten des FileInfo Fensters zu ändern wenn ein anderer Eintrag im Listtree angewählt wird
      DoMethod( Data->LT_Project_Files,  MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime,
                obj, 1, MUIM_Project_File_Info
              );
    }
    else
    {
      DisplayBeep(_screen(obj));
      return(0);
    }
  }

  // Nur wenn MUIM_Project_File_Info_Open geschickt wurde öffnen wir auch das Fenster
  if(msg->MethodID == MUIM_Project_File_Info_Open) set(Data->WI_Project_File_Info, MUIA_Window_Open, TRUE);

  // Nur wenn das Fenster offen ist aktualisieren wir auch die Daten
  if(xget(Data->WI_Project_File_Info, MUIA_Window_Open))
  {
    // Wenn kein Entry beim Methodenaufruf übergeben wurde holen wir uns den aktiven entry
    if(!entry)
    {
      entry = (struct Entry_Data *)DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Get_Entry, MUIV_ProjectTree_Active);
      if(!entry) return(0);
    }

    // Dann das aktuelle Entry vom Listtree holen und an FileInfo schicken
    DoMethod( Data->WI_Project_File_Info, MUIM_FileInfo_Refresh, entry);
  }

  return(0);
}

/*
** Project_File_Touch
** Methode um das aktuell ausgewählte File zu touchen
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

METHOD Project_File_Touch(Class *cl, Object *obj, struct MUIP_Project_File_Touch_Message *msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);
  struct MUI_NListtree_TreeNode *tn;
  char buffer[256];

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Nur ein Element touchen
  if(msg->MethodID == MUIM_Project_File_Touch)
  {
    struct DateStamp datestamp;
    struct Entry_Data *entry = msg->entry;

    if(!entry)
    {
      // Holen des aktiven Eintrages
      entry = (struct Entry_Data *)DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Get_Entry, MUIV_ProjectTree_Active);
      if(!entry) return(0);
    }

    // Wenn es kein File ist dann touchen wir auch nicht
    if(entry->entry_type != ENTRY_TYPE_FILE) return(0);

    // Holen der aktuellen Zeit
    DateStamp(&datestamp);
    SetFileDate(entry->entry_file, &datestamp);


    sprintf(buffer, "Touched file \0338%s\0332 ....", FilePart(entry->entry_file));
    set(Data->TX_Project_Info, MUIA_Text_Contents, buffer);
  }

  // Alle Einträge des Listtree touchen
  if(msg->MethodID == MUIM_Project_File_Touch_All)
  {
    int i;

    for(i=0;;i++)
    {
      tn = (struct MUI_NListtree_TreeNode *)DoMethod(Data->LT_Project_Files, MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Root, i);

      if(tn)
      {
        struct Entry_Data *entry = (struct Entry_Data *)tn->tn_User;
        struct DateStamp datestamp;

        if(entry->entry_type == ENTRY_TYPE_FILE && entry->entry_touchable == TRUE)
        {
          // Holen der aktuellen Zeit
          DateStamp(&datestamp);
          SetFileDate(entry->entry_file, &datestamp);

          sprintf(buffer, "Touched file \0338%s\0332 ....", FilePart(entry->entry_file));
          set(Data->TX_Project_Info, MUIA_Text_Contents, buffer);
        }

      }else
      {
        break;
      }
    }
  }

  // Dann das aktuelle Entry vom Listtree holen und an FileInfo schicken
  DoMethod( Data->WI_Project_File_Info, MUIM_FileInfo_Refresh, DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Get_Entry, MUIV_ProjectTree_Active));

  return(0);
}

/*
** Project_File_View
**
** Methode die das ViewTool aufruft um eine Datei zu betrachten
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class *        : Die Klasse des Objektes
** Object *       : Das Objekt
** struct opGet * : Pointer auf die opGet Struktur
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** 0
*/

METHOD Project_File_View(Class *cl, Object *obj, struct MUIP_Project_File_View_Message *msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);
  struct Entry_Data *entry = msg->entry;
  STRPTR viewtool;
  char   command[1024];
  BPTR   fh, nil;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(!entry) return(0);

  // Schauen ob im entry selber das Tool benutzt werden soll oder default
  if(entry->entry_viewtool_std)
  {
    viewtool = Data->Project_ViewTool;

  }else
  {
    viewtool = entry->entry_viewtool;
  }

  // Das Kommando nun zusammensetzen
  sprintf(command, "%s %s", viewtool, entry->entry_file);

  // Das Kommando nun ausführen
  fh = Open("CON:0/20/640/100/AmIDE Output/AUTO/CLOSE/WAIT",   MODE_READWRITE);
  nil = Open("NIL:",  MODE_OLDFILE);

  // Dann das Executeable starten
  SystemTags( command,
              SYS_Input,      nil,
              SYS_Output,     fh,
              SYS_Asynch,     TRUE,
              NP_Priority,    0,
              TAG_DONE
            );

  return(0);
}

/*
** Project_FileType_Default
** Methode um die Default-Filetypen in das FileTyp-Objekt zu laden
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

METHOD Project_FileType_Default(Class *cl, Object *obj, Msg msg)
{
	struct Project_Data *Data = INST_DATA(cl, obj);

	return(DoMethod(Data->WI_Project_FileType, MUIM_FileType_Load, DEFAULTCFGFILE));
}

/*
** Project_Finish
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

METHOD Project_Finish(Class *cl, Object *obj, Msg msg)
{
	struct Project_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Wenn der MsgBrowser noch existiert dann killen wir es jetzt auch
  if(Data->WI_Project_MsgBrowser)
  {
  	DoMethod(_app(obj), OM_REMMEMBER, Data->WI_Project_MsgBrowser);
  }

  // Wenn das File_Info Objekt noch existiert dann killen wir es jetzt auch
  if(Data->WI_Project_File_Info)
  {
  	DoMethod(_app(obj), OM_REMMEMBER, Data->WI_Project_File_Info);
  }

  // Wenn das File_Type Objekt noch existiert dann killen wir es jetzt auch
  if(Data->WI_Project_FileType)
  {
  	DoMethod(_app(obj), OM_REMMEMBER, Data->WI_Project_FileType);
  }

  // Wenn das Environment noch existiert dann killen wir es jetzt auch
  if(Data->WI_Project_Env)
  {
  	DoMethod(_app(obj), OM_REMMEMBER, Data->WI_Project_Env);
  }

	// Nun entfernen wir noch das object aus der Projektliste
  DoMethod(WI_AmIDE_MainBar, MUIM_MainBar_DelProject, obj);

	// Nun geben wir den Allokierten Speicher frei
  free(Data->Project_Name);
  free(Data->Project_Directory);
  free(Data->Project_Title);

  // Entfernen des Objektes aus der Applikationsliste
  DoMethod(_app(obj), OM_REMMEMBER, obj);

	return(0);
}

/*
** Project_GetJob
** Funktion zum holen und entfernen des ersten Jobs in der Liste
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** ULONG: Der Rückgabewert der Superklasse
*/

METHOD Project_GetJob(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  struct MUIS_Project_Job *pend_job;

  struct AmIDES_message *message;
	char	 msg_text[256];

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

	// Jetzt die Struktur an die Liste hängen
  DoMethod(Data->LS_Project_Joblist, MUIM_List_GetEntry, 0, &pend_job, TAG_DONE);
  if(!pend_job) return((ULONG)pend_job);

	// Dann setzen wir erstmal eine Trennlinie ein
  message = calloc(1, sizeof(struct AmIDES_message));
  if(!message) return(0);

  switch(pend_job->type)
  {
    case MUIV_Project_JOB_COMPILE:
    {
  		message->text = calloc(strlen(pend_job->file)+strlen(AmIDE_MSG_STYLE_CMPSTART)+strlen(" Compiling [] ")+1, sizeof(char));
  		if(!message->text) return(0);

			message->type = AmIDE_MSG_HEADER;
	  	sprintf(message->text, "%s Compiling [%s]", AmIDE_MSG_STYLE_CMPSTART, pend_job->file);
  		message->status = "starting compile...";
    }
  	break;

  	case MUIV_Project_JOB_LINK:
    {
		  message->text = calloc(strlen(pend_job->exe)+strlen(AmIDE_MSG_STYLE_LNKSTART)+strlen(" Linking []")+1, sizeof(char));
  		if(!message->text) return(0);

			message->type = AmIDE_MSG_HEADER;
      sprintf(message->text, "%s Linking [%s]", AmIDE_MSG_STYLE_LNKSTART, pend_job->exe);
  		message->status = "linking files together...";
    }
  }

  message->errnr = -1;
  message->line = -1;

	DoMethod(Data->WI_Project_MsgBrowser, AmIDEM_msginsert, message, TAG_DONE);

	return((ULONG)pend_job);
}

/*
** Project_Link
**
** Methode um im Listtree nachzuschauen welche Dateien zusammengelinkt werden sollen
** und dies dann auch ausführt
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

METHOD Project_Link(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  ULONG entries = xget(Data->LT_Project_Files, MUIA_List_Entries);
  STRPTR *file_array;
  STRPTR exe_name = NULL;
  struct Entry_Data *entry;
  LONG i, j = 0;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Das file_array aufbauen
  file_array = calloc(entries+1, sizeof(STRPTR));
  if(!file_array) return(0);

  // Das MsgBrowser-Fenster öffnen
  DoMethod(obj, MUIM_Project_MsgBrowser, TAG_DONE);

  // Statusnachricht
  set(Data->WI_Project_MsgBrowser, MUIA_MsgBrowser_Status,  "Checking for linkable files...");

  // Die Schleife um jeden Eintrag des Listtree zu checken
  for(i=0; i < entries; i++)
  {
    // Holen des i-ten Eintrages
    entry = (struct Entry_Data *)DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Get_Entry, i, TAG_DONE);
    if(!entry) break;

    if(entry->entry_type == ENTRY_TYPE_FILE && entry->entry_linkable)
    {
      file_array[j]    = entry->entry_file;
      file_array[++j]  = 0;

    }else
    {
      if(entry->entry_type == ENTRY_TYPE_EXECUTEABLE) exe_name = entry->entry_file;
    }

  }

  if(j>0)
  {
    if(exe_name)
    {
        // Anhängen des LINK-Jobs an die Jobliste
        DoMethod(obj, MUIM_Project_AddJob, MUIV_Project_JOB_LINK, file_array, exe_name, TAG_DONE);

        // Starten des Compilers
        DoMethod(obj, MUIM_Project_RunJob, TAG_DONE);

    }else
    {
      set(Data->WI_Project_MsgBrowser, MUIA_MsgBrowser_Status,  "No executeable destination selected - can`t link...");
    }
  }else
  {
    set(Data->WI_Project_MsgBrowser, MUIA_MsgBrowser_Status,  "No linkable files found in project!");
  }

  return(0);
}

/*
** Project_Load
** Methode die die vorher abgespeicherten Daten eines Projektes aus einer ASCII
** Datei ließt.
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

METHOD Project_Load(Class *cl, Object *obj, struct MUIP_Project_Load_Message *msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);
  BPTR fh;

  enum { MODE_PROJECT=1, MODE_FOLDER, MODE_FILE, MODE_DEPFILE, MODE_EXE};

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Erstmal das Listtree leer machen
  DoMethod(Data->LT_Project_Files, MUIM_NListtree_Remove, MUIV_NListtree_Remove_ListNode_Root, MUIV_NListtree_Remove_TreeNode_All, 0);

  // Das FileType-Objekt adden und die Daten laden
  set(Data->TX_Project_Info, MUIA_Text_Contents, "Loading filetypes...");
  DoMethod(Data->WI_Project_FileType, MUIM_FileType_Load, msg->filename);

  // Nun setzen wir das Listtree auf QUIET für das adden von vielen Dateien
  set(Data->LT_Project_Files, MUIA_NListtree_Quiet, TRUE);

  if(fh = Open(msg->filename, MODE_OLDFILE))
  {
    struct Entry_Data *tmp_entry;
    char tmp_str[512];
    int  mode = 0;

    while(FGets(fh, tmp_str, 512))
    {
      static STRPTR filename;
      static STRPTR folder_name;
      static STRPTR folder_parent;
      static int    compileable;
      static int    linkable;
      static int    touchable;
      static int    scan_dep;
      static int    binding;
      static int    open;
      static STRPTR edittool;
      static int    edittool_std;
      static int    edittool_sel;
      static STRPTR viewtool;
      static int    viewtool_std;
      static int    viewtool_sel;

      // Dann überschreiben wir das Returnzeichen
      tmp_str[strlen(tmp_str)-1]='\0';

      // Je nachdem welcher Mode aktiv ist bearbeiten wir den String anders
      switch(mode)
      {
        // Die Projekteinstellungen
        case MODE_PROJECT:
        {
          STRPTR tmp;

          if(tmp = get_string(tmp_str, "Name=",       TRUE))  Data->Project_Name      = tmp;
          if(tmp = get_string(tmp_str, "Directory=",  TRUE))  Data->Project_Directory = tmp;
          if(tmp = get_string(tmp_str, "Type=",       FALSE)) Data->Project_Type      = atoi(tmp);

          // Nun das Compilermodul finden
          if(tmp = get_string(tmp_str, "Compiler=",   TRUE))
          {
            int i;
            STRPTR mod_name = strtok(tmp, " ");

            Data->Project_Compiler = NULL;

            for(i=0;;i++)
            {
              struct str_modules *module;

              DoMethod(LS_AmIDE_Modules, MUIM_List_GetEntry, i, &module);
              if(!module) break;

              if(strcmp(strtok((STRPTR)xget(module->lib_object, AmIDEA_API_lib_vers), " "), mod_name) == 0)
              {
                Data->Project_Compiler = module->lib_object;
                break;
              }
            }

            // Falls kein passendes Modul gefunden wurde
            if(!Data->Project_Compiler)
            {
              struct str_modules *module;

              MUI_Request(APP_AmIDE, NULL, 0, "Module not found", "*Ok", "\033cModule \0338%s\0332 couldn`t be found in PROGDIR:modules .\nThe first useable module will be used instead!", mod_name);

              DoMethod(LS_AmIDE_Modules, MUIM_List_GetEntry, 0, &module);
              if(!module) break;

              Data->Project_Compiler = module->lib_object;
            }
          }

          if(tmp = get_string(tmp_str, "EditTool=",   TRUE))  Data->Project_EditTool  = tmp;
          if(tmp = get_string(tmp_str, "ViewTool=",   TRUE))  Data->Project_ViewTool  = tmp;
        }
        break;

        case MODE_FOLDER:
        {
          STRPTR tmp;

          if(tmp = get_string(tmp_str, "Name=",   TRUE))    folder_name   = tmp;
          if(tmp = get_string(tmp_str, "Open=",   FALSE))   open          = atoi(tmp);
          if(tmp = get_string(tmp_str, "Parent=", TRUE))    folder_parent = tmp;

          if(folder_name && folder_parent && open >= 0)
          {
            STRPTR tmp;

            // Jetzt suchen wir nach einen Filetypen der genauso heisst wie der Folder
            tmp = (STRPTR)DoMethod(Data->WI_Project_FileType, MUIM_FileType_Find, folder_name, 0);
            if(tmp) folder_name = tmp;

            DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Add_Folder, folder_name, open, folder_parent, 0);

            mode=0;
          }
        }
        break;

        case MODE_FILE:
        {
          STRPTR tmp;

          if(tmp = get_string(tmp_str, "Name=",         TRUE))  filename      = tmp;
          if(tmp = get_string(tmp_str, "Folder=",       TRUE))  folder_name   = tmp;
          if(tmp = get_string(tmp_str, "EditTool=",     TRUE))  edittool      = tmp;
          if(tmp = get_string(tmp_str, "EditTool_std=", FALSE)) edittool_std  = atoi(tmp);
          if(tmp = get_string(tmp_str, "EditTool_sel=", FALSE)) edittool_sel  = atoi(tmp);
          if(tmp = get_string(tmp_str, "ViewTool=",     TRUE))  viewtool      = tmp;
          if(tmp = get_string(tmp_str, "ViewTool_std=", FALSE)) viewtool_std  = atoi(tmp);
          if(tmp = get_string(tmp_str, "ViewTool_sel=", FALSE)) viewtool_sel  = atoi(tmp);
          if(tmp = get_string(tmp_str, "Compileable=",  FALSE)) compileable   = atoi(tmp);
          if(tmp = get_string(tmp_str, "Linkable=",     FALSE)) linkable      = atoi(tmp);
          if(tmp = get_string(tmp_str, "Touchable=",    FALSE)) touchable     = atoi(tmp);
          if(tmp = get_string(tmp_str, "Scan=",         FALSE)) scan_dep      = atoi(tmp);
          if(tmp = get_string(tmp_str, "Binding=",      FALSE)) binding       = atoi(tmp);

          if( filename            &&
              folder_name         &&
              edittool            &&
              edittool_std  >= 0  &&
              edittool_sel  >= 0  &&
              viewtool            &&
              viewtool_std  >= 0  &&
              viewtool_sel  >= 0  &&
              compileable   >= 0  &&
              linkable      >= 0  &&
              touchable     >= 0  &&
              scan_dep      >= 0  &&
              binding       >= 0
            )
          {
            char buffer[256];
            BPTR fh_lock;
            BOOL add_entry = TRUE;

            sprintf(buffer, "Adding file \0338%s\0332 to folder \0338%s\0332 ....", FilePart(filename), folder_name);
            set(Data->TX_Project_Info, MUIA_Text_Contents, buffer);

            // Schauen ob die Datei noch existiert
            if(!(fh_lock = Lock(filename, SHARED_LOCK)))
            {
              if(MUI_Request(APP_AmIDE, NULL, 0, "File not found", "*Remove|_Cancel", "\033cFile \0338%s\0332 doesn`t exists !\nDo you want to remove it from the list ?", FilePart(filename)))
              {
                add_entry = FALSE;
              }

            }else UnLock(fh_lock);

            if(add_entry)
            {
              tmp_entry = (struct Entry_Data *)DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Add_File, filename,
                                                                                                        folder_name,
                                                                                                        0,
                                                                                                        compileable,
                                                                                                        linkable,
                                                                                                        touchable,
                                                                                                        scan_dep,
                                                                                                        binding,
                                                                                                        edittool,
                                                                                                        edittool_std,
                                                                                                        edittool_sel,
                                                                                                        viewtool,
                                                                                                        viewtool_std,
                                                                                                        viewtool_sel );
              }

              mode = MODE_DEPFILE;
          }
        }
        break;

        case MODE_DEPFILE:
        {
          STRPTR tmp;

          // Dann die Dependencies einlesen
          if(tmp = get_string(tmp_str, "DepFile=",      TRUE))
          {
            BPTR fh_lock;
            BOOL add_entry = TRUE;
            struct Entry_Data *dep_entry;

            // Schauen ob die Datei noch existiert
            if(!(fh_lock = Lock(tmp, SHARED_LOCK)))
            {
              if(MUI_Request(APP_AmIDE, NULL, 0, filename, "*Remove|_Cancel", "\033cDependency file \0338%s\0332 doesn`t exists !\nDo you want to remove it from the list ?", FilePart(tmp)))
              {
                add_entry = FALSE;
              }

            }else UnLock(fh_lock);

            if(add_entry)
            {
              // Erzeugen eines Dependency-Entries
              dep_entry = calloc(1, sizeof(struct Entry_Data));
              if(dep_entry)
              {
                LONG entries = xget(tmp_entry->entry_dep, MUIA_NList_Entries);

                // Füllen der entry Struktur mit den Daten
                dep_entry->entry_file         = tmp;
                dep_entry->entry_code         = NULL;
                dep_entry->entry_text         = NULL;
                dep_entry->entry_type         = ENTRY_TYPE_FILE;
                dep_entry->entry_compileable  = FALSE;
                dep_entry->entry_linkable     = FALSE;
                dep_entry->entry_touchable    = FALSE;
                dep_entry->entry_dep_scan     = FALSE;
                dep_entry->entry_binding      = FALSE;
                dep_entry->entry_parent       = NULL;

                DoMethod(tmp_entry->entry_dep, MUIM_NList_InsertSingle, dep_entry, MUIV_NList_Insert_Bottom);
              }
            }
          }

        }
        break;

        case MODE_EXE:
        {
          STRPTR tmp;

          if(tmp = get_string(tmp_str, "Name=",         TRUE))  filename      = tmp;
          if(tmp = get_string(tmp_str, "Folder=",       TRUE))  folder_name   = tmp;

          if(filename && folder_name)
          {
            char buffer[256];

            sprintf(buffer, "Adding executeable \0338%s\0332 to folder \0338%s\0332 ....", FilePart(filename), folder_name);
            set(Data->TX_Project_Info, MUIA_Text_Contents, buffer);

            DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Add_Exe,  filename,
                                                                     folder_name,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     1,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0 );

            mode = 0;
          }
        }
        break;

      }

      // Nun stellen wir die Modes ein
      if(strstr(tmp_str, "[AmIDE Project]"))  mode = MODE_PROJECT;
      if(strstr(tmp_str, "[Folder]"))
      {
        folder_name   = NULL;
        open          = -1;
        folder_parent = NULL;

        mode = MODE_FOLDER;
      }
      if(strstr(tmp_str, "[File]"))
      {
        filename      = NULL;
        folder_name   = NULL;
        edittool      = NULL;
        edittool_std  = -1;
        edittool_sel  = -1;
        viewtool      = NULL;
        viewtool_std  = -1;
        viewtool_sel  = -1;
        compileable   = -1;
        linkable      = -1;
        touchable     = -1;
        scan_dep      = -1;
        binding       = -1;

        mode = MODE_FILE;
      }
      if(strstr(tmp_str, "[Executeable]"))
      {
        filename      = NULL;
        folder_name   = NULL;
        compileable   = -1;
        linkable      = -1;
        touchable     = -1;
        scan_dep      = -1;
        binding       = -1;

        mode = MODE_EXE;
      }

    }
    Close(fh);

    // Den Titel aktualisieren
    DoMethod(obj, MUIM_Project_Update_Title);

    // Nun setzen wir das Listtree wieder auf NORMAL
    set(Data->LT_Project_Files, MUIA_NListtree_Quiet, FALSE);

  }

  return(0);
}

/*
** Project_Make
**
** Methode um ein komplettes Make zu machen und dabei die Dependencies zu beachten
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

METHOD Project_Make(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  char   obj_file[256];
  struct FileInfoBlock *obj_file_fib;
  struct Entry_Data *entry;
  int    i, j;
  LONG   entries = 1;
  BOOL   compile, scan_dep;
  char   buffer[256];
  BPTR   lock;
  struct DateStamp obj_file_date;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Das MsgBrowser-Fenster öffnen
  DoMethod(obj, MUIM_Project_MsgBrowser, TAG_DONE);

  // Den MsgBrowser nun löschen
  DoMethod(Data->WI_Project_MsgBrowser, MUIM_MsgBrowser_Clear, TAG_DONE);

  entries = xget(Data->LT_Project_Files, MUIA_List_Entries);

  // Die Schleife um jeden Eintrag des Listtree zu checken
  for(i=0; i < entries; i++, compile = FALSE)
  {
    // Holen des i-ten Eintrages
    entry = (struct Entry_Data *)DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Get_Entry, i, TAG_DONE);
    if(!entry) break;

    if(entry->entry_type == ENTRY_TYPE_FILE && entry->entry_compileable)
    {
      set(Data->WI_Project_MsgBrowser, MUIA_MsgBrowser_File,    entry->entry_file);
      set(Data->WI_Project_MsgBrowser, MUIA_MsgBrowser_Status,  "Checking for changes...");

      // Wenn Make_ALL ausgeführt wird brauchen wir keine Depencies beachten
      if(msg->MethodID == MUIM_Project_Make)
      {

        // Erzeugen des Objektfilenamens
        {
          STRPTR tmp_str;

          // Umkopieren
          strcpy(obj_file, entry->entry_file);

          tmp_str = strrchr(obj_file, '.');

          // Wenn noch genug Platz am String ist überschreibe ich das zeichen nach dem Punkt mit einem "o"
          if(strlen(tmp_str) > 1)
          {
            tmp_str++;
            *tmp_str = 'o';
            tmp_str++;
            *tmp_str = '\0';
          }
        }

        // Holen des Dateidatums des Objektfiles
        // Falls es nicht möglich was das Datum zu holen, existiert das File auch nicht und wir können gleich
        // compilieren
        if((lock = Lock((STRPTR)obj_file, ACCESS_READ)))
        {
          if((obj_file_fib = AllocDosObject(DOS_FIB, NULL)))
          {
            if(Examine(lock, obj_file_fib))
            {
              memcpy(&obj_file_date, &(obj_file_fib->fib_Date), sizeof(struct DateStamp));
            }
            FreeDosObject(DOS_FIB, obj_file_fib);
          }
          UnLock(lock);
        }
        else compile = TRUE;

        // Aktualisieren des Dateidatums des Entry
        get_file_date(entry);

        // Schauen ob das Dateidatum des File neuer ist als das des Objekt files
        if(!compile) if(CompareDates(&(entry->entry_date), &(obj_file_date)) < 0) compile = TRUE;

        // Jetzt die Dependency-Files checken
        if(!compile)
        {
          struct Entry_Data *dep_entry;

          set(Data->WI_Project_MsgBrowser, MUIA_MsgBrowser_Status,  "Scanning dependency files...");

          for(j=0; j < xget(entry->entry_dep, MUIA_NList_Entries) && compile == FALSE; j++)
          {
            DoMethod(entry->entry_dep, MUIM_NList_GetEntry, j, &dep_entry, TAG_DONE);
            if(dep_entry)
            {
              // Aktualisieren des Dateidatums des Entry
              get_file_date(dep_entry);

              if(CompareDates(&(dep_entry->entry_date), &(obj_file_date)) < 0) compile = TRUE;
            }
          }
        }
      }
      else compile = TRUE;

      // Wenn alle checks compile ergeben haben compilieren wir die Datei
      if(compile)
      {
        // Die Abhängigkeiten neu einscannen
        if(entry->entry_dep_scan)
        {
          set(Data->WI_Project_MsgBrowser, MUIA_MsgBrowser_Status,  "Updating dependencies...");
          DoMethod(obj, MUIM_Project_Scan_Dependency, entry->entry_file, entry, FALSE, TAG_DONE);
        }

        // Anhängen des COMPILE-Jobs an die Jobliste
        DoMethod(obj, MUIM_Project_AddJob, MUIV_Project_JOB_COMPILE, entry->entry_file, TAG_DONE);

      }else
      {
        set(Data->WI_Project_MsgBrowser, MUIA_MsgBrowser_Status,  "No compile necessary...");
      }
    }
  }

  // Dann linken wir alle linkbaren Dateien zusammen
  DoMethod(obj, MUIM_Project_Link, TAG_DONE);

  return(0);
}

/*
** Project_MsgBrowser
** Methode die ein MessageBrowserObjekt erzeugt und das Fenster öffnet
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

METHOD Project_MsgBrowser(Class *cl, Object *obj, Msg msg)
{
	struct Project_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

	if (Data->WI_Project_MsgBrowser == NULL)
  {
  	Data->WI_Project_MsgBrowser = MsgBrowserObject,
    	MUIA_MsgBrowser_Project,			obj,
      MUIA_MsgBrowser_ProjectTree,	Data->LT_Project_Files,
      MUIA_MsgBrowser_FileType,			Data->WI_Project_FileType,
      MUIA_MsgBrowser_Compiler,			Data->Project_Compiler,
		End;

		if (Data->WI_Project_MsgBrowser)
		{
    	DoMethod(_app(obj), OM_ADDMEMBER, Data->WI_Project_MsgBrowser, TAG_DONE);
   		set(Data->WI_Project_MsgBrowser, MUIA_Window_Open, TRUE);

    }else
    {
    	DisplayBeep(_screen(obj));
      return(0);
    }
	}else
  {
		set(Data->WI_Project_MsgBrowser, MUIA_Window_Open, TRUE);
  }

  return(0);
}

/*
** Project_PM_Add
** Methode die ein PopupMenu erzeugt und darstellt und dann auf eine Auswahl wartet
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

METHOD Project_PM_Add(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

	struct PopupMenuBase	*PopupMenuBase;
	struct PopupMenu			*pop_menu;
  struct Window					*win;
	ULONG  ret = 0;

	enum{ PMN_ADDFILE=1, PMN_ADDDIR, PMN_ADDLIB, PMN_ADDFOLD, PMN_SELEXE };

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
		pop_menu = 	PMMenu("Add to list"),
									PMItem("Add file(s)..."),					PM_Disabled, FALSE, PM_UserData, PMN_ADDFILE, PMEnd,
									PMItem("Add directory..."),				PM_Disabled, TRUE, PM_UserData, PMN_ADDDIR,	 PMEnd,
									PMItem("Add library..."),					PM_Disabled, TRUE, PM_UserData, PMN_ADDLIB,	 PMEnd,
                  PMItem("Add folder..."),					PM_Disabled, TRUE, PM_UserData, PMN_ADDFOLD, PMEnd,
                  PMItem("Select executeable..."),	PM_Disabled, FALSE, PM_UserData, PMN_SELEXE, PMEnd,
								End;

		ret = (ULONG)(PM_OpenPopupMenu(	 win, PM_Menu,	  pop_menu, TAG_DONE));

		PM_FreePopupMenu(pop_menu);
		CloseLibrary((struct Library *)PopupMenuBase);

		switch(ret)
    {
      case PMN_ADDFILE		:	return(DoMethod(obj, MUIM_Project_AddFile, TAG_DONE));
      case PMN_ADDDIR			: return(0);
      case PMN_ADDLIB			: return(0);
			case PMN_ADDFOLD		: return(0);
			case PMN_SELEXE 		: return(DoMethod(obj, MUIM_Project_AddExe, TAG_DONE));
		}
  }

	return(0);
}

/*
** Project_PM_Compie
** Methode die ein PopupMenu erzeugt und darstellt und dann auf eine Auswahl wartet
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

METHOD Project_PM_Compile(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

	struct PopupMenuBase	*PopupMenuBase;
	struct PopupMenu			*pop_menu;
  struct Window					*win;
	ULONG  ret = 0;

	enum{ PMN_COMPILE=1, PMN_MAKE, PMN_MAKEALL, PMN_RUN };

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
		pop_menu = 	PMMenu("Compile & Make"),
									PMItem("Compile file..."), 	PM_Disabled, FALSE, PM_UserData, PMN_COMPILE, PMEnd,
									PMItem("Make..."),		  		PM_Disabled, FALSE, PM_UserData, PMN_MAKE,    PMEnd,
									PMItem("Make all..."),			PM_Disabled, FALSE, PM_UserData, PMN_MAKEALL, PMEnd,
                  PMItem("Run..."),						PM_Disabled, FALSE, PM_UserData, PMN_RUN,			PMEnd,
								End;

		ret = (ULONG)(PM_OpenPopupMenu(	 win, PM_Menu,	  pop_menu, TAG_DONE));

		PM_FreePopupMenu(pop_menu);
		CloseLibrary((struct Library *)PopupMenuBase);

		switch(ret)
    {
      case PMN_COMPILE:		return(DoMethod(obj, MUIM_Project_Compile, 	TAG_DONE));
      case PMN_MAKE: 			return(DoMethod(obj, MUIM_Project_Make, 		TAG_DONE));
      case PMN_MAKEALL: 	return(DoMethod(obj, MUIM_Project_Make_All,	TAG_DONE));
      case PMN_RUN:				return(DoMethod(obj, MUIM_Project_RunExe,		TAG_DONE));
		}
  }

	return(0);
}

/*
** Project_PM_Delete
** Methode die ein PopupMenu erzeugt und darstellt und dann auf eine Auswahl wartet
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

METHOD Project_PM_Delete(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  struct PopupMenuBase  *PopupMenuBase;
  struct PopupMenu      *pop_menu;
  struct Window         *win;
  ULONG  ret = 0;

  enum{ PMN_DELSEL=1, PMN_DELALL };

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
    pop_menu =  PMMenu("Remove from list"),
                  PMItem("Remove selected..."), PM_UserData, PMN_DELSEL,  PMEnd,
                  PMItem("Remove all..."),      PM_UserData, PMN_DELALL,  PMEnd,
                End;

    ret = (ULONG)(PM_OpenPopupMenu(  win, PM_Menu,    pop_menu, TAG_DONE));

    PM_FreePopupMenu(pop_menu);
    CloseLibrary((struct Library *)PopupMenuBase);

    switch(ret)
    {
      case PMN_DELSEL: return(DoMethod(Data->LT_Project_Files, MUIM_NListtree_Remove, MUIV_NListtree_Remove_ListNode_Root, MUIV_NListtree_Remove_TreeNode_Active,  TAG_DONE));
      case PMN_DELALL: return(DoMethod(Data->LT_Project_Files, MUIM_NListtree_Remove, MUIV_NListtree_Remove_ListNode_Root, MUIV_NListtree_Remove_TreeNode_All,     TAG_DONE));
    }
  }

  return(0);
}

/*
** Project_PM_Touch
** Methode die ein PopupMenu erzeugt und darstellt und dann auf eine Auswahl wartet
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

METHOD Project_PM_Touch(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

	struct PopupMenuBase	*PopupMenuBase;
	struct PopupMenu			*pop_menu;
  struct Window					*win;
	ULONG  ret = 0;

	enum{ PMN_TOUCHSEL=1, PMN_TOUCHALL };

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
		pop_menu = 	PMMenu("Touch entry"),
									PMItem("Touch selected..."), PM_Disabled, FALSE, PM_UserData, PMN_TOUCHSEL,  PMEnd,
									PMItem("Touch all..."),			 PM_Disabled, FALSE, PM_UserData, PMN_TOUCHALL,  PMEnd,
								End;

		ret = (ULONG)(PM_OpenPopupMenu(	 win, PM_Menu,	  pop_menu, TAG_DONE));

		PM_FreePopupMenu(pop_menu);
		CloseLibrary((struct Library *)PopupMenuBase);

		switch(ret)
    {
      case PMN_TOUCHSEL: return(DoMethod(obj, MUIM_Project_File_Touch, 		TAG_DONE));
      case PMN_TOUCHALL: return(DoMethod(obj, MUIM_Project_File_Touch_All,TAG_DONE));
		}
  }

	return(0);
}

/*
** Project_RunExe
**
** Methode zum starten des Executeables
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

METHOD Project_RunExe(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);
  struct Entry_Data *entry;
  BPTR   fh, nil;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  entry = (struct Entry_Data *)DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Find_Exe, 0);
  if(!entry) return(0);

  fh = Open("CON:0/20/640/100/AmIDE Output/AUTO/CLOSE/WAIT",   MODE_READWRITE);
  nil = Open("NIL:",  MODE_OLDFILE);

  // Dann das Executeable starten
  SystemTags( entry->entry_file,
              SYS_Input,      nil,
              SYS_Output,     fh,
              SYS_Asynch,     TRUE,
              NP_Priority,    0,
              TAG_DONE
            );

  return(0);
}

/*
** Project_RunJob
** Funktion die den Compiler startet und die Jobs bearbeitet
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** ULONG: Der Rückgabewert der Superklasse
*/

METHOD Project_RunJob(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  struct Process *proc;
  struct AmIDEComp_Message *comp_msg;
  char 	 client_name[256];

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

	// Setzen des ParentObj für das Compiler-Modul
  set(Data->Project_Compiler, AmIDEA_API_parentObj, Data->WI_Project_MsgBrowser);

  // An das Objekt das Kommando zum compilieren schicken

	comp_msg = calloc(1, sizeof(struct AmIDEComp_Message));
	if(!comp_msg) return(0);

  comp_msg->OB_Project  = obj;
  comp_msg->OB_Compiler = Data->Project_Compiler;

	sprintf(client_name, "%s clt", APP_Basename);

	// Jetzt starten wir den SubTask
	Forbid();
	proc = CreateNewProcTags(	NP_Entry, 		Comp_Client,
  													NP_Name,			client_name,
                            NP_Cli,				TRUE,
                            NP_StackSize,	32768,
                            TAG_DONE);
	if(!proc) return(0);
  proc->pr_Task.tc_UserData = comp_msg;
	Permit();

	return(0);
}

/*
** Project_Save
** Methode die die kompletten Projektdaten in eine ASCII-Datei abspeichert
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

METHOD Project_Save(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  char filename[256];
  BPTR fh;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Nun bauen wir erstmal den Dateinamen zusammen
  sprintf(filename, "%s%s.¶", Data->Project_Directory, Data->Project_Name);

  if(fh = Open(filename, MODE_NEWFILE))
  {
    int i;

    // Erstmal den Dateikopf schreiben
    FPrintf(fh, "[AmIDE Project]\n");
    FPrintf(fh, "Version=1.0\n");
    FPrintf(fh, "Name=%s\n",      Data->Project_Name);
    FPrintf(fh, "Directory=%s\n", Data->Project_Directory);
    FPrintf(fh, "Type=%ld\n",     Data->Project_Type);
    FPrintf(fh, "Compiler=%s\n",  xget(Data->Project_Compiler, AmIDEA_API_lib_vers));
    FPrintf(fh, "EditTool=%s\n",  Data->Project_EditTool);
    FPrintf(fh, "ViewTool=%s\n",  Data->Project_ViewTool);
    FPrintf(fh, "\n");

    // Dann schreiben wir alle Folders aus dem Listtree
    for(i=0;;i++)
    {
      struct Entry_Data *entry = (struct Entry_Data *)DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Get_Entry, i);
      if(!entry) break;

      if(entry->entry_type == ENTRY_TYPE_FOLDER)
      {
        struct MUI_NListtree_TreeNode *tn_parent = (struct MUI_NListtree_TreeNode *)entry->entry_parent;

        FPrintf(fh, "[Folder]\n");
        FPrintf(fh, "Name=%s\n",    entry->entry_file);
        FPrintf(fh, "Open=%ld\n",   entry->entry_open);
        if(tn_parent)  FPrintf(fh, "Parent=%s\n",  ((struct Entry_Data *)tn_parent->tn_User)->entry_file);
        else           FPrintf(fh, "Parent=*ROOT*\n");
      }
    }
    FPrintf(fh, "\n");

    // Dann schreiben wir alle Dateien aus dem Listtree
    for(i=0;;i++)
    {
      struct Entry_Data *entry = (struct Entry_Data *)DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Get_Entry, i);
      if(!entry) break;

      if(entry->entry_type == ENTRY_TYPE_FILE)
      {
        int j;

        FPrintf(fh, "[File]\n");
        FPrintf(fh, "Name=%s\n",          entry->entry_file);
        FPrintf(fh, "Folder=%s\n",        ((struct Entry_Data *)entry->entry_parent->tn_User)->entry_file);
        FPrintf(fh, "EditTool=%s\n",      entry->entry_edittool);
        FPrintf(fh, "EditTool_std=%ld\n", entry->entry_edittool_std);
        FPrintf(fh, "EditTool_sel=%ld\n", entry->entry_edittool_sel);
        FPrintf(fh, "ViewTool=%s\n",      entry->entry_viewtool);
        FPrintf(fh, "ViewTool_std=%ld\n", entry->entry_viewtool_std);
        FPrintf(fh, "ViewTool_sel=%ld\n", entry->entry_viewtool_sel);
        FPrintf(fh, "Compileable=%ld\n",  entry->entry_compileable);
        FPrintf(fh, "Linkable=%ld\n",     entry->entry_linkable);
        FPrintf(fh, "Touchable=%ld\n",    entry->entry_touchable);
        FPrintf(fh, "Scan=%ld\n",         entry->entry_dep_scan);
        FPrintf(fh, "Binding=%ld\n",      entry->entry_binding);

        // Schreiben der Abhängigkeitsliste des jeweiligen Files
        for(j=0;;j++)
        {
          struct Entry_Data *tmp_entry;

          DoMethod(entry->entry_dep, MUIM_NList_GetEntry, j, &tmp_entry);
          if(!tmp_entry) break;

          FPrintf(fh, "DepFile=%s\n", tmp_entry->entry_file);
        }
      }

      if(entry->entry_type == ENTRY_TYPE_EXECUTEABLE)
      {
        FPrintf(fh, "[Executeable]\n");
        FPrintf(fh, "Name=%s\n",        entry->entry_file);
        FPrintf(fh, "Folder=%s\n",      ((struct Entry_Data *)entry->entry_parent->tn_User)->entry_file);
      }
      FPrintf(fh, "\n");
    }

    Close(fh);
  }

  // Und jetzt speichern wir noch die FileTypen ab
  DoMethod(Data->WI_Project_FileType, MUIM_FileType_Save, filename, FALSE);

  return(0);
}

/*
** Project_Scan_Dependency
** Methode um in einer Datei nach #includes zu suchen und die Beziehungen neu aufzustellen
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

METHOD Project_Scan_Dependency(Class *cl, Object *obj, struct MUIP_Project_Scan_Dependency_Message *msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  STRPTR filename           = msg->filename;
  struct Entry_Data *entry  = msg->entry;
  BOOL   add_file           = msg->add_file;

  BPTR fh;
  char buffer[256];
  int i,j;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  fh = Open(filename, MODE_OLDFILE);

  // Zeilenweises Einlesen
  while(FGets(fh, buffer, 256))
  {
    STRPTR p_buf = buffer;
    STRPTR tmp_str = NULL;
    struct Entry_Data *dep_entry;
    struct FileType *type;

    // Solange Zeichen für Zeichen solange es Leerzeichen oder Tabulatoren sind
    for(i=0; i < 256 && (*p_buf == ' ' || *p_buf == '\t'); i++, p_buf++);

    // Das nächste Zeichen muss nun eine Raute sein
    if(*p_buf == '#')
    {
      p_buf++;

      if(Strnicmp(p_buf, "include", 7) == 0)
      {
        int k;
        STRPTR inc_name = calloc(256, sizeof(char));
        char tmp[256];

        // Nun weiter die Leerzeichen und Tabulatoren überspringen
        for(p_buf+=7; i < 256 && (*p_buf == ' ' || *p_buf == '\t'); i++, p_buf++);

        // Danach muss ein Anführungszeichen kommen
        if(*p_buf == '"')
        {
          // Umkopieren des aktuellen Pfades
          strncpy(inc_name, filename, strlen(filename)-strlen(PathPart(filename)));

          // Nun den Dateinamen Stück für Stück lesen
          for(p_buf++, i++, k=0; i < 256 && (*p_buf != '"'); i++, k++, p_buf++)
          {
            tmp[k] = *p_buf;
          }
          tmp[k] = '\0';

          // Jetzt den Dateinamen & Pfad zusammensetzen
          AddPart(inc_name, tmp, 256-strlen(inc_name));

          if(add_file)
          {
            // Identifizieren des Dateitypes
            type = (struct FileType *)DoMethod(Data->WI_Project_FileType, MUIM_FileType_Identify, inc_name);

            // Wenn es identifiziert werden konnte dann...
            if(type)
            {
              DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Add_File_ByType, inc_name, type);

            }else
            {
              DoMethod(Data->LT_Project_Files, MUIM_ProjectTree_Add_File, inc_name, "Others", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            }
          }

          // Erzeugen eines Dependency-Entries
          dep_entry = calloc(1, sizeof(struct Entry_Data));
          if(dep_entry)
          {
            LONG entries = xget(entry->entry_dep, MUIA_NList_Entries);

            // Füllen der entry Struktur mit den Daten
            dep_entry->entry_file         = inc_name;
            dep_entry->entry_code         = NULL;
            dep_entry->entry_text         = NULL;
            dep_entry->entry_type         = ENTRY_TYPE_FILE;
            dep_entry->entry_compileable  = FALSE;
            dep_entry->entry_linkable     = FALSE;
            dep_entry->entry_touchable    = FALSE;
            dep_entry->entry_dep_scan     = FALSE;
            dep_entry->entry_binding      = FALSE;
            dep_entry->entry_parent       = NULL;

            // Dann checken wir ob der Eintrag nicht schon in der Liste existiert
            for(j=0; j < entries; j++)
            {
              struct Entry_Data *tmp_entry;

              DoMethod(entry->entry_dep, MUIM_NList_GetEntry, j, &tmp_entry);

              if(strcmp(tmp_entry->entry_file, dep_entry->entry_file) == 0) j=(entries+1);
            }

            // Wenn jetzt j <= als die kompletten Einträge der List ist dann adden wir hinzu
            if(j <= entries)
            {
              DoMethod(entry->entry_dep, MUIM_NList_InsertSingle, dep_entry, MUIV_NList_Insert_Bottom);
            }

            // Dann Scan_Dependency wieder rekursiv aufrufen
            if(type)
            {
              if(type->scan_dep) DoMethod(obj, MUIM_Project_Scan_Dependency, inc_name, entry, add_file);
            }
          }
        }
      }
    }
  }
  Close(fh);

  return(0);
}

/*
** Project_Update_Title
** Methode die den Titel des Projektfensters neu formatiert und in MUI aktualisiert
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

METHOD Project_Update_Title(Class *cl, Object *obj, Msg msg)
{
  struct Project_Data *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  sprintf(Data->Project_Title, "(%d) Project: [%s]", Data->Project_Number, Data->Project_Name);

	// Nun den Title updaten im Objekt
  set(obj, MUIA_Window_Title, Data->Project_Title);

	return(0);
}

/*
** Project_Dispatcher
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

SAVEDS ASM ULONG Project_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  switch (msg->MethodID)
  {
    case OM_NEW                         : return(Project_New              (cl, obj, (APTR)msg));
    case OM_GET                         : return(Project_Get              (cl, obj, (APTR)msg));
    case MUIM_Project_Finish            : return(Project_Finish           (cl, obj, (APTR)msg));
    case MUIM_Project_AddFile           : return(Project_AddFile          (cl, obj, (APTR)msg));
    case MUIM_Project_AddExe            : return(Project_AddExe           (cl, obj, (APTR)msg));
    case MUIM_Project_MsgBrowser        : return(Project_MsgBrowser       (cl, obj, (APTR)msg));
    case MUIM_Project_PM_Add            : return(Project_PM_Add           (cl, obj, (APTR)msg));
    case MUIM_Project_PM_Delete         : return(Project_PM_Delete        (cl, obj, (APTR)msg));
    case MUIM_Project_PM_Touch          : return(Project_PM_Touch         (cl, obj, (APTR)msg));
    case MUIM_Project_PM_Compile        : return(Project_PM_Compile       (cl, obj, (APTR)msg));
    case MUIM_Project_File_Info         :
    case MUIM_Project_File_Info_Open    : return(Project_File_Info        (cl, obj, (APTR)msg));
    case MUIM_Project_Save              : return(Project_Save             (cl, obj, (APTR)msg));
    case MUIM_Project_Load              : return(Project_Load             (cl, obj, (APTR)msg));
    case MUIM_Project_Update_Title      : return(Project_Update_Title     (cl, obj, (APTR)msg));
    case MUIM_Project_File_Touch        :
    case MUIM_Project_File_Touch_All    : return(Project_File_Touch       (cl, obj, (APTR)msg));
    case MUIM_Project_FileType_Default  : return(Project_FileType_Default (cl, obj, (APTR)msg));
    case MUIM_Project_Scan_Dependency   : return(Project_Scan_Dependency  (cl, obj, (APTR)msg));
    case MUIM_Project_Compile           : return(Project_Compile          (cl, obj, (APTR)msg));
    case MUIM_Project_Make              :
    case MUIM_Project_Make_All          : return(Project_Make             (cl, obj, (APTR)msg));
    case MUIM_Project_Link              : return(Project_Link             (cl, obj, (APTR)msg));
    case MUIM_Project_Add_ChildObj      : return(Project_Add_ChildObj     (cl, obj, (APTR)msg));
    case MUIM_Project_Env_Set           : return(Project_Env_Set          (cl, obj, (APTR)msg));
    case MUIM_Project_Env_Get           : return(Project_Env_Get          (cl, obj, (APTR)msg));
    case MUIM_Project_File_Edit         : return(Project_File_Edit        (cl, obj, (APTR)msg));
    case MUIM_Project_File_View         : return(Project_File_View        (cl, obj, (APTR)msg));
    case MUIM_Project_File_DC           : return(Project_File_DC          (cl, obj, (APTR)msg));
    case MUIM_Project_RunExe            : return(Project_RunExe           (cl, obj, (APTR)msg));
    case MUIM_Project_AddJob            : return(Project_AddJob           (cl, obj, (APTR)msg));
    case MUIM_Project_GetJob            : return(Project_GetJob           (cl, obj, (APTR)msg));
    case MUIM_Project_DelJob            : return(Project_DelJob           (cl, obj, (APTR)msg));
    case MUIM_Project_RunJob            : return(Project_RunJob           (cl, obj, (APTR)msg));
  }

  return (DoSuperMethodA(cl, obj, msg));
}

/*
** CreateProjectClass()
** Funktion zum erstellen der Project-Klasse und zuweisen der Dispatcherfunktion
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** struct MUI_CustomClass: Die fertig erzeugte Klasse
*/

struct MUI_CustomClass *CreateProjectClass()
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

   if (CL_ProjectTree = CreateProjectTreeClass()   )
   if (CL_MsgBrowser  = CreateMsgBrowserClass() )
   if (CL_FileInfo    = CreateFileInfoClass()   )
   if (CL_FileType    = CreateFileTypeClass()   )
   {
      struct MUI_CustomClass *CL_Project;

      CL_Project = MUI_CreateCustomClass( NULL,
                                          MUIC_Window, NULL,
                                          sizeof(struct Project_Data),
                                          Project_Dispatcher
                                        );
      if (CL_Project)
      {
         return(CL_Project);
      }
   }

   DeleteProjectClass(NULL);
   return NULL;
}

/*
** DeleteProjectClass()
** Funktion zum löschen/beenden der Project Klasse
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** struct MUI_CustomClass *: Pointer auf die zu löschende Klasse
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** keine
*/

void DeleteProjectClass(struct MUI_CustomClass *CL_Project)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

   if (CL_Project)
   {
      MUI_DeleteCustomClass(CL_Project);
      CL_Project = NULL;
   }

   if (CL_ProjectTree)
   {
      DeleteProjectTreeClass(CL_ProjectTree);
      CL_ProjectTree = NULL;
   }

   if (CL_MsgBrowser)
   {
      DeleteMsgBrowserClass(CL_MsgBrowser);
      CL_MsgBrowser = NULL;
   }

   if (CL_FileInfo)
   {
      DeleteFileInfoClass(CL_FileInfo);
      CL_FileInfo = NULL;
   }

   if (CL_FileType)
   {
      DeleteFileTypeClass(CL_FileType);
      CL_FileType = NULL;
   }
}

