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
#include "Main.h"
#include "MainBar.h"
#include "Project.h"
#include "ProjectTree.h"

/* Structures */
struct Library *MUIMasterBase;
struct Library *AmIDE_API_Base;
struct MUI_CustomClass *CL_MainBar;

// Alle Objekte definieren
Object *APP_AmIDE;
Object *WI_AmIDE_MainBar;
Object *WI_AmIDE_ProjectAss;
Object *WI_AmIDE_About;
Object *LS_AmIDE_Modules;
Object *STR_ProjectAss_Directory;
Object *STR_ProjectAss_Name;
Object *RB_ProjectAss_Type;
Object *RB_ProjectAss_Compiler;
Object *BT_ProjectAss_Okay;
Object *BT_ProjectAss_Abort;

// Globale Elemente definieren
const STRPTR CYA_Project_Type[]     = { "Template", "C Program", "C++ Program", "Shared Library", "Linker Library", NULL };
STRPTR APP_Basename;
char APP_MsgPortname[256];

// Text für die Cycle-Gadget
STRPTR *CYA_Project_Compiler;

// Text für das About-Fenster
STRPTR TX_AmIDE_About_Mod;

LONG __stack = 131070;

/*
** main Funktion
**
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** keine
**
*/

void main(void)
{
  struct MsgPort *AmIDE_port;
  ULONG AmIDE_portsig = 0UL;
  ULONG Sigs = 0UL;
  Object *BT_AmIDE_About_Close;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

   /* Program initialisation */
   if (!(MUIMasterBase = OpenLibrary(MUIMASTER_NAME, 14UL)))
   {
      Quit(NULL, "Failed to open muimaster.library V14+");
   }

   if (!(CL_MainBar = CreateMainBarClass()))
   {
      Quit(NULL, "Failed to init classes");
   }

  /*
  ** Nun die Moduleliste anlegen & laden
  */

  LS_AmIDE_Modules =  ListObject,
                      End;

  if(!LS_AmIDE_Modules) Quit(NULL, "Failed to create the module list");

  DirWalk("PROGDIR:modules", LoadModules, "#?.module");

  // Schauen ob mind. 1 modul geladen wurde
  if(xget(LS_AmIDE_Modules, MUIA_List_Entries) < 1) Quit(NULL, "No modules are installed in [PROGDIR:modules].");

  // Dann die Infos der Module holen und verschiedene Strings füllen
  if(!GetModuleInfo()) Quit(NULL, "Can`t get information about modules");

  /* Create Application */
  APP_AmIDE = ApplicationObject,
              MUIA_Application_Author,      AUTHOR,
              MUIA_Application_Base,        PROJECTNAME,
              MUIA_Application_Title,       PROJECTNAME,
              MUIA_Application_Version,     VERSTAG,
              MUIA_Application_Copyright,   COPYRIGHT,
              MUIA_Application_Description, DESCRIPTION,

              SubWindow, WI_AmIDE_About =  WindowObject,
                MUIA_Window_Borderless,   TRUE,
                MUIA_Window_CloseGadget,  FALSE,
                MUIA_Window_DepthGadget,  FALSE,
                MUIA_Window_SizeGadget,   FALSE,
                MUIA_Window_DragBar,      FALSE,
                WindowContents, VGroup,
                  GroupFrame,
                  MUIA_Background,      MUII_GroupBack,
                  Child, HGroup,
                    Child, TextObject,
                      MUIA_Text_Contents,
                        "\n\33c\33b" PROJECTNAME "\33n\n\n"
                        " " FULLVERSION_STRING "\n\n"
                        "\33bAm\33niga \33bI\33nntegrated \33bD\33nevelopment \33bE\33nnvironment\n\n"
                        "* FREEWARE *\n\n"
                        COPYRIGHT "\n"
                        "Jens Langner\n\n"
                        "Email: Jens.Langner@light-speed.de\n"
                        "http://AmIDE.light-speed.de/\n",
                    End,
                    Child, MUI_MakeObject(MUIO_VBar, 1),
                    Child, VGroup,
                      Child, TextObject,
                        MUIA_Text_Contents, "\33c\33uLoaded modules:\33n",
                      End,
                      Child, TextObject,
                        MUIA_Text_Contents, TX_AmIDE_About_Mod,
                      End,
                    End,
                  End,
                  Child, MUI_MakeObject(MUIO_HBar, 1),
                  Child, BT_AmIDE_About_Close = TextObject,
                    MUIA_Text_Contents, "\033bClose",
                    MUIA_Background,    MUII_ButtonBack,
                    MUIA_Frame,         MUIV_Frame_Button,
                    MUIA_InputMode,     MUIV_InputMode_RelVerify,
                    MUIA_Text_SetMax,   TRUE,
                    MUIA_CycleChain,    1,
                    MUIA_ControlChar,   'C',
                  End,
                End,
              End,

              SubWindow, WI_AmIDE_ProjectAss = WindowObject,
                MUIA_Window_Title,  "Project Assistant",
                MUIA_Window_ID,     MAKE_ID('P', 'A', 'S', 'S'),
                WindowContents,     VGroup,
                  Child, ColGroup(2),

                    Child, Label("Project-Directory:"),
                    Child, PopaslObject,
                      MUIA_Popstring_String, STR_ProjectAss_Directory = BetterStringObject,
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

                      Child, Label("Project-Name:"),
                      Child, STR_ProjectAss_Name = BetterStringObject,
                        StringFrame,
                          MUIA_String_Contents, "",
                          MUIA_Weight,          100,
                          MUIA_String_MaxLen,   80,
                          MUIA_String_Format,   MUIV_String_Format_Left,
                        End,
                      End,

                      Child, MUI_MakeObject(MUIO_HBar, 1),
                      Child, HGroup,
                        Child, HSpace(0),
                        Child, RB_ProjectAss_Type     = Radio("Project-Type:", CYA_Project_Type),
                        Child, HSpace(0),
                        Child, RB_ProjectAss_Compiler = Radio("Compiler:",     CYA_Project_Compiler),
                        Child, HSpace(0),
                      End,
                      Child, MUI_MakeObject(MUIO_HBar, 1),
                      Child, HGroup,
                        Child, BT_ProjectAss_Okay   = KeyButton("Okay",  "o"),
                        Child, BT_ProjectAss_Abort  = KeyButton("Abort", "a"),
                      End,
                    End,
                  End,

                  SubWindow, WI_AmIDE_MainBar = MainBarObject,
//                  MUIA_MainBar_Creator, CreatorName,
                  End,

                End;

   if (APP_AmIDE)
   {
//      STRPTR ErrorStr = NULL;

      // Wenn die das About-Close gedrückt wurde einfach das Fenster wieder schliessen
      DoMethod(BT_AmIDE_About_Close,  MUIM_Notify, MUIA_Pressed, FALSE,
               WI_AmIDE_About, 3, MUIM_Set, MUIA_Window_Open, FALSE
              );

      // Die CloseRequests an das ProjectAssistentenObjekt binden
      DoMethod(WI_AmIDE_ProjectAss, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
               WI_AmIDE_ProjectAss, 3, MUIM_Set, MUIA_Window_Open, FALSE
              );
      DoMethod(BT_ProjectAss_Abort, MUIM_Notify, MUIA_Pressed, FALSE,
               WI_AmIDE_ProjectAss, 3, MUIM_Set, MUIA_Window_Open, FALSE
              );


      // Den Okay-Button des ProjektAssistenten an die MainBar-Methode binden
      DoMethod(BT_ProjectAss_Okay, MUIM_Notify, MUIA_Pressed, FALSE,
               WI_AmIDE_MainBar, 2, MUIM_MainBar_NewProject, 1
              );


      set(WI_AmIDE_MainBar, MUIA_Window_Open, TRUE);

      if (!(xget(WI_AmIDE_MainBar, MUIA_Window_Open)))
      {
         Quit(APP_AmIDE, "Failed to open MainBar Window");
      }

      // Jetzt holen wir uns den richtigen Basename der Applikation
      // und jetzt bauen wir den MsgPortnamen zusammen
      get(APP_AmIDE, MUIA_Application_Base, &APP_Basename);
      sprintf(APP_MsgPortname, "%s_mp", APP_Basename);

      // Now we create the MsgPort to communicate with subtasks
      AmIDE_port = CreateMsgPort();
      if(!AmIDE_port) Quit(APP_AmIDE, "Failed to create MsgPort!");

      /* Fills in all field to make the port public */
      AmIDE_port->mp_Node.ln_Name = APP_MsgPortname;
      AmIDE_port->mp_Node.ln_Pri = 0;

      /* makes the port public */
      AddPort(AmIDE_port);

      /* stores the signal associate do the port */
      AmIDE_portsig = 1 << AmIDE_port->mp_SigBit;

      while (DoMethod(APP_AmIDE, MUIM_Application_NewInput, &Sigs) != MUIV_Application_ReturnID_Quit)
      {
         if (Sigs)
         {
            // Jetzt auf die Signale von MUI und CTRL_C und die am eigenen MsgPort warten
            Sigs = Wait(Sigs | SIGBREAKF_CTRL_C | AmIDE_portsig);

            // Wenn eine Msg am MsgPort ankommt
            if (Sigs & AmIDE_portsig)
            {
              struct CompAmIDE_Message *arrived_msg;

              while(arrived_msg = (struct CompAmIDE_Message *)GetMsg(AmIDE_port))
              {
                switch(arrived_msg->data)
                {
                  case MSG_CC_JOBREQUEST:
                  {
                    // Wir holen uns den nächsten Job
                    arrived_msg->data = DoMethod(arrived_msg->OB_Project, MUIM_Project_GetJob, TAG_DONE);
                  }
                  break;

                  case MSG_CC_JOBFINISHED:
                  {
                    // Jetzt löschen wir den Job aus der Liste
                    arrived_msg->data = DoMethod(arrived_msg->OB_Project, MUIM_Project_DelJob, TAG_DONE);
                  }
                  break;
                }

                ReplyMsg((struct Message *)arrived_msg);
              }
            }

            // Wenn CTRL-C dann beenden von AmIDE
            if (Sigs & SIGBREAKF_CTRL_C) break;
         }
      }

      set(WI_AmIDE_MainBar, MUIA_Window_Open, FALSE);

      if(AmIDE_port)
      {
        RemPort(AmIDE_port);
        DeleteMsgPort(AmIDE_port);
      }

      Quit(APP_AmIDE, NULL);
   }
   else
   {
      Quit(NULL, "Failed to create Application");
   }
}

/*
** LoadModules Funktion zum laden aller Compiler-Module
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** Immer TRUE damit DirWalk dann zum nächsten Eintrag übergeht
*/

BOOL LoadModules(struct FileInfoBlock *fib,  STRPTR pattern)
{
  char modulepath[256];
  struct IClass *mod_class;
  struct str_modules *module;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  sprintf(modulepath, "PROGDIR:modules/%s", (STRPTR) &(fib->fib_FileName));

  if(MatchNoCase(modulepath, pattern) == 1)
  {
    // Speicher für die Struktur holen
    module = (struct str_modules *)calloc(1, sizeof(struct str_modules));
    if(!module) return(TRUE);

    // Den Pfad des modules nun in die Struktur schreiben
    strcpy(module->module_path, modulepath);

    // Jetzt die Library öffnen
    module->libbase = OpenLibrary(modulepath, 0);
    if(!module->libbase) return(TRUE);

    // LibBase umkopieren
    AmIDE_API_Base = module->libbase;

    // Klasse holen
    mod_class = AmIDE_API_GetClass();
    if(!mod_class) return(TRUE);

    // Objekt erzeugen
    module->lib_object = NewObject(mod_class, NULL, TAG_DONE, NULL);
    if(!module->lib_object) return(TRUE);

    // Jetzt die Struktur an die Liste hängen
    DoMethod(LS_AmIDE_Modules, MUIM_List_InsertSingle, module, MUIV_List_Insert_Bottom, TAG_DONE);
  }

  return(TRUE);
}

/*
** KillModules Funktion zum löschen aller Compiler-Module
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** ULONG: Der Rückgabewert der Superklasse
*/

BOOL KillModules(void)
{
  int i;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Die Modulliste durchgehen und alle Objekte löschen und die
  // Libraries schliessen

  for(i=0;;i++)
  {
    struct str_modules *module;

    DoMethod(LS_AmIDE_Modules, MUIM_List_GetEntry, i, &module, TAG_DONE);
    if(!module) break;

    if(module->lib_object)  DisposeObject(module->lib_object);
    if(module->libbase)     CloseLibrary(module->libbase);
    if(module)              free(module);
  }

  return(TRUE);
}

/*
** GetModuleInfo Funktion zum holen aller Moduleinformationen und füllen
** der Informationsstrings
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** BOOL: TRUE bei Erfolg
**       FALSE bei Misserfolg
*/

BOOL GetModuleInfo(void)
{
  int i;
  int len;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Wieviel Module wurden geladen?
  len = xget(LS_AmIDE_Modules, MUIA_List_Entries);

  // Den Text des Cycle-Gadgets allokieren
  CYA_Project_Compiler = calloc(len+1, sizeof(STRPTR));

  // Jetzt den Text des About-Fensters allokieren und modifizieren
  TX_AmIDE_About_Mod = calloc(len*512, sizeof(char));

  if(!CYA_Project_Compiler || !TX_AmIDE_About_Mod) Quit(NULL, "Can`t allocate enough memory");

  // Den Text des Cycle-Gadgets jetzt erzeugen
  for(i=0;;i++)
  {
    char temp[512];
    struct str_modules *module;

    DoMethod(LS_AmIDE_Modules, MUIM_List_GetEntry, i, &module, TAG_DONE);
    if(!module) break;

    sprintf(  temp,
              "\n\33b%s\33n\n"
              "Compiler: %s\n"
              "Author: %s\n\n", xget(module->lib_object, AmIDEA_API_lib_vers),
                                xget(module->lib_object, AmIDEA_API_compiler),
                                xget(module->lib_object, AmIDEA_API_author)
           );

    strcat(TX_AmIDE_About_Mod, temp);

    if(module->module_path) CYA_Project_Compiler[i] = (STRPTR)xget(module->lib_object, AmIDEA_API_compiler);
  }

  CYA_Project_Compiler[i] = NULL;

  return(TRUE);
}

/*
** GetModulePos Funktion gibt die Position des Modules innerhalb der Modulliste zurück
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** keine
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** int: die Position in der Liste
*/

int GetModulePos(Object *mod_obj)
{
  int i;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  for(i=0;;i++)
  {
    struct str_modules *module;

    DoMethod(LS_AmIDE_Modules, MUIM_List_GetEntry, i, &module, TAG_DONE);
    if(!module) break;

    if(module->lib_object == mod_obj) return(i);
  }
  return(-1);
}

/*
** DoSuperNew Funktion zum aufrufen der SuperKlasse
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Class *:  Die Klasse
** Object *: Das Object der Klasse
** Tag1:     Die einzelnen Tags die an die Superklasse übergeben werden
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** ULONG: Der Rückgabewert der Superklasse
*/

ULONG DoSuperNew(Class *cl, Object *obj, ULONG Tag1, ...)
{
   return(DoSuperMethod(cl, obj, OM_NEW, &Tag1, NULL));
}

/*
** Quit Funktion zum sauberen Beenden des Programmes
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Object *App: Das Objekt der Applikation
** STRPTR:      Die übrigen Msgs der Applikation
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** keine
*/

void Quit(Object *App, STRPTR Msg)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if (App)              MUI_DisposeObject(App);
  if (CL_MainBar)       DeleteMainBarClass(CL_MainBar);

  if (LS_AmIDE_Modules)
  {
    KillModules();
    MUI_DisposeObject(LS_AmIDE_Modules);
  }

  if (MUIMasterBase)    CloseLibrary(MUIMasterBase);

  if (Msg)
  {
    puts(Msg);
    exit(20);
  }

  exit(0);
}

/*
** xget - Hilfsfunktion um Ein bestimmtes Attribut eines Objektes
** zurückzuliefern
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** Object *: Das Objekt
** ULONG:    Das gewünschte Attribut
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** LONG: Der Wert des Attributes im Objekt
*/

LONG xget(Object *obj, ULONG Attr)
{
   LONG x;

   get(obj, Attr, &x);
   return x;
}

/*
** get_string
** Funktion die als Parameter einen String in dem gesucht werden soll, einen String
** nach dem Gesucht werden soll bekommt und die Speicher allokiert und dann als
** Returnwert den Pointer auf einen String bekommt der den Inhalt nach dem suchstring hat
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** STRPTR: Der String der geprüft werden soll
** STRPTR: Der String nach dem im anderen gesucht werden soll
** BOOL  : Wenn Speicher allokiert werden soll TRUE
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** STRPTR: Ein Pointer auf einen String der nach dem Suchstring liegt
*/

STRPTR get_string(STRPTR string, STRPTR search_str, BOOL alloc_mem)
{
  STRPTR tmp_str;
  STRPTR new_str;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(tmp_str = strstr(string, search_str))
  {
    // Nun setzen wir den Pointer um die Länge des search_str hoch
    tmp_str += strlen(search_str);

    if(alloc_mem)
    {
      new_str = calloc(strlen(tmp_str)+1, sizeof(char));
      strcpy(new_str, tmp_str);

    }else return(tmp_str);

  }else return(NULL);

  return(new_str);
}

/*
** get_file_date
**
** Hilfsfunktion um das Dateidatum eines entry neu zu setzen
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** STRPTR: Der komplette Dateiname
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** keine
*/

void get_file_date(struct Entry_Data *entry)
{
  struct FileInfoBlock *fib;
  BPTR lock;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if((lock = Lock((STRPTR)entry->entry_file, ACCESS_READ)))
  {
    if((fib = AllocDosObject(DOS_FIB, NULL)))
    {
      if(Examine(lock, fib))
      {
        entry->entry_date = fib->fib_Date;
      }
      FreeDosObject(DOS_FIB, fib);
    }
    UnLock(lock);
  }
}

/*
** FileSize
**
** function that returns the size of a file
**
** parameters:
** ~~~~~~~~~~~
** char *: the path+filename of the file which we want to examine
**
** return value:
** ~~~~~~~~~~~~~
** the size of the file
*/

ULONG FileSize(char *filename)
{
  struct FileInfoBlock *fib;
  BPTR lock;
  ULONG size = 0;

  if((lock = Lock((STRPTR)filename, ACCESS_READ)))
  {
    if((fib = AllocDosObject(DOS_FIB, NULL)))
    {
      if(Examine(lock, fib))
      {
        size = fib->fib_Size;
      }
      FreeDosObject(DOS_FIB, fib);
    }
    UnLock(lock);
  }

  return size;
}

/*
** MatchNoCase
**
** Case insensitive pattern matching
**
** parameters:
** ~~~~~~~~~~~
** char *string: the string that should be searched
** char *match : the pattern we use for the match
**
** return value:
** ~~~~~~~~~~~~~
** return of the match
*/

int MatchNoCase(char *string, char *match)
{
  int result = -1;
  char *pattern;

  if(!(pattern = calloc(strlen(match)*2+2, sizeof(char)))) return -1;

  if(ParsePatternNoCase(match, pattern, (LONG)strlen(match)*2+2) > -1)
  {
     result = MatchPatternNoCase(pattern, string);
  }

  free(pattern);

  return result;
}

/*
** DirWalk
**
** call entryfun(fib, usrdata) with every entry of dir
**
** parameters:
** ~~~~~~~~~~~
** char *dir      : path of the directory to work on
** BOOL *entryfun : pointer to the function to call for every file in the dir
** APTR usrdata   : the userdata to call the function with
**
** return value:
** ~~~~~~~~~~~~~
** returns TRUE on success
*/

BOOL DirWalk(char *dir, BOOL (*entryfun)(struct FileInfoBlock *, APTR), APTR usrdata)
{
  struct FileInfoBlock *fib;
  BPTR lock;
  BOOL success = FALSE;

  if((lock = Lock((STRPTR)dir, ACCESS_READ)))
  {
    if((fib = AllocDosObject(DOS_FIB, NULL)))
    {
      if(Examine(lock, fib) != DOSFALSE)
      {
        while(1)
        {
          if(ExNext(lock, fib) == DOSFALSE)
          {
            if(ERROR_NO_MORE_ENTRIES == IoErr()) success = TRUE;

            break;
          }

          if(!(entryfun(fib, usrdata))) break;
        }
      }

      FreeDosObject(DOS_FIB, fib);
    }
    UnLock(lock);
  }

  return success;
}
