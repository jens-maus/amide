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

#ifndef _MAIN_H_
#define _MAIN_H_

// Alle Objekte von Main
extern Object *APP_AmIDE;
extern Object *WI_AmIDE_MainBar;
extern Object *WI_AmIDE_ProjectAss;
extern Object *WI_AmIDE_About;
extern Object *LS_AmIDE_Modules;
extern Object *LS_AmIDE_Joblist;
extern Object *STR_ProjectAss_Directory;
extern Object *STR_ProjectAss_Name;
extern Object *RB_ProjectAss_Type;
extern Object *RB_ProjectAss_Compiler;
extern Object *BT_ProjectAss_Okay;
extern Object *BT_ProjectAss_Abort;

extern const STRPTR CYA_Project_Type[];
extern STRPTR *CYA_Project_Compiler;
extern STRPTR TX_AmIDE_About_Mod;
extern STRPTR APP_Basename;
extern char APP_MsgPortname[256];

extern BOOL		LoadModules(struct FileInfoBlock *,  STRPTR);
extern BOOL   KillModules(void);
extern BOOL		GetModuleInfo(void);
extern int		GetModulePos(Object *);
extern LONG 	xget(Object *obj, ULONG Attr);
extern ULONG	DoSuperNew(Class *cl, Object *obj, ULONG Tag1, ...);
extern void  	Quit(Object *App, STRPTR Msg);
extern STRPTR get_string(STRPTR, STRPTR, BOOL);
extern void 	get_file_date(struct Entry_Data *);
extern ULONG  FileSize(char *filename);
extern int    MatchNoCase(char *string, char *match);
extern BOOL   DirWalk(char *dir, BOOL (*entryfun)(struct FileInfoBlock *, APTR), APTR usrdata);

/*
** Structure for the compiler-modules
*/

static struct str_modules
{
  char module_path[256];
  struct Library *libbase;
  Object *lib_object;
};

#endif
