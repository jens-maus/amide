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

#ifndef _PROJECT_H_
#define _PROJECT_H_

#define ProjectObject NewObject(CL_Project->mcc_Class, NULL

// Project-Typen
#define MUIV_Project_Type_Template		0
#define MUIV_Project_Type_C						1
#define MUIV_Project_Type_CPP					2
#define MUIV_Project_Type_SLIB				3
#define MUIV_Project_Type_LLIB				4

// Compiler ID
#define MUIV_Project_CMP_SASC					0
#define MUIV_Project_CMP_STORMC				1
#define MUIV_Project_CMP_GCC					2
#define MUIV_Project_CMP_VBCC					3

// JOB ID
#define MUIV_Project_JOB_COMPILE			0
#define MUIV_Project_JOB_LINK					1

// Attributes ID
#define MUIA_Project_Dummy						(TAG_USER + 0x4AFE0700)

#define MUIA_Project_Name							(MUIA_Project_Dummy + 1)
#define MUIA_Project_Directory				(MUIA_Project_Dummy + 2)
#define MUIA_Project_Type							(MUIA_Project_Dummy + 3)
#define MUIA_Project_Compiler					(MUIA_Project_Dummy + 4)
#define MUIA_Project_Number						(MUIA_Project_Dummy + 5)

// Methoden ID
#define MUIM_Project_Dummy						(TAG_USER + 0x2FFE0700)

#define MUIM_Project_Finish     			(MUIM_Project_Dummy + 1)
#define MUIM_Project_AddFile    			(MUIM_Project_Dummy + 2)
#define MUIM_Project_AddExe						(MUIM_Project_Dummy + 3)
#define MUIM_Project_MsgBrowser 			(MUIM_Project_Dummy + 4)
#define MUIM_Project_PM_Add						(MUIM_Project_Dummy + 5)
#define MUIM_Project_PM_Delete				(MUIM_Project_Dummy + 6)
#define MUIM_Project_PM_Touch	  			(MUIM_Project_Dummy + 7)
#define MUIM_Project_PM_Compile				(MUIM_Project_Dummy + 8)
#define MUIM_Project_File_Info  			(MUIM_Project_Dummy + 9)
#define MUIM_Project_File_Info_Open 	(MUIM_Project_Dummy + 10)
#define MUIM_Project_About_Window   	(MUIM_Project_Dummy + 11)
#define MUIM_Project_Save							(MUIM_Project_Dummy + 12)
#define MUIM_Project_Load							(MUIM_Project_Dummy + 13)
#define MUIM_Project_Update_Title			(MUIM_Project_Dummy + 14)
#define MUIM_Project_File_Touch				(MUIM_Project_Dummy + 15)
#define MUIM_Project_File_Touch_All 	(MUIM_Project_Dummy + 16)
#define MUIM_Project_FileType_Default	(MUIM_Project_Dummy + 17)
#define MUIM_Project_Scan_Dependency	(MUIM_Project_Dummy + 18)
#define MUIM_Project_Compile					(MUIM_Project_Dummy + 19)
#define MUIM_Project_Make							(MUIM_Project_Dummy + 20)
#define MUIM_Project_Make_All					(MUIM_Project_Dummy + 21)
#define MUIM_Project_Link							(MUIM_Project_Dummy + 22)
#define MUIM_Project_Add_ChildObj			(MUIM_Project_Dummy + 23)
#define MUIM_Project_Env_Set					(MUIM_Project_Dummy + 24)
#define MUIM_Project_Env_Get					(MUIM_Project_Dummy + 25)
#define MUIM_Project_File_Edit				(MUIM_Project_Dummy + 26)
#define MUIM_Project_File_View				(MUIM_Project_Dummy + 27)
#define MUIM_Project_File_DC					(MUIM_Project_Dummy + 28)
#define MUIM_Project_RunExe						(MUIM_Project_Dummy + 29)
#define MUIM_Project_AddJob						(MUIM_Project_Dummy + 30)
#define MUIM_Project_GetJob						(MUIM_Project_Dummy + 31)
#define MUIM_Project_DelJob						(MUIM_Project_Dummy + 32)
#define MUIM_Project_RunJob						(MUIM_Project_Dummy + 33)

// Strukturen
extern struct MUI_CustomClass *CL_ProjectTree;
extern struct MUI_CustomClass *CL_MsgBrowser;
extern struct MUI_CustomClass *CL_FileInfo;
extern struct MUI_CustomClass *CL_FileType;

struct MUIP_Project_File_Touch_Message
{
  ULONG MethodID;
  struct Entry_Data *entry;
};

struct MUIP_Project_File_Info_Message
{
  ULONG MethodID;
  struct Entry_Data *entry;
};

struct MUIP_Project_Load_Message
{
  ULONG MethodID;
  STRPTR filename;
};

struct MUIP_Project_Scan_Dependency_Message
{
  ULONG MethodID;
  STRPTR filename;
  struct Entry_Data *entry;
  BOOL   add_file;
};

struct MUIP_Project_Compile_Message
{
  ULONG MethodID;
  struct Entry_Data *entry;
};

struct MUIP_Project_File_Edit_Message
{
  ULONG MethodID;
  struct Entry_Data *entry;
};

struct MUIP_Project_File_View_Message
{
  ULONG MethodID;
  struct Entry_Data *entry;
};

struct MUIP_Project_AddJob_Message
{
  ULONG MethodID;
  ULONG type;
	APTR  file;
  APTR  exe;
};

struct Project_Data
{
  Object *MN_Project;
  Object *LT_Project_Files;
  Object *TB_Project_Left_Toolbar;
  Object *TB_Project_Right_Toolbar;
  Object *TX_Project_Info;
  Object *WI_Project_MsgBrowser;
  Object *WI_Project_File_Info;
  Object *WI_Project_FileType;
  Object *LS_Project_Joblist;

  // Objekte des Environment Fensters
  Object *WI_Project_Env;
  Object *BS_Project_Env_Name;
  Object *BS_Project_Env_Dir;
  Object *RB_Project_Env_PType;
  Object *BS_Project_Env_EditTool;
  Object *BS_Project_Env_ViewTool;
  Object *RB_Project_Env_Compiler;
	Object *BT_Project_Env_Okay;
	Object *BT_Project_Env_Abort;

  char		Project_Title[256];
  STRPTR	Project_Name;
  STRPTR	Project_Directory;
  STRPTR	Project_EditTool;
  STRPTR	Project_ViewTool;
  int			Project_Type;
  Object	*Project_Compiler;
  int			Project_Number;

  struct MsgPort *ReplyPort;
};

struct MUIS_Project_Job
{
  ULONG type;
  Object *OB_Compiler;
  APTR file;
  APTR exe;
};

// Methoden-Prototypen
METHOD Project_Finish						(Class *cl, Object *obj, Msg msg);
METHOD Project_AddFile					(Class *cl, Object *obj, Msg msg);
METHOD Project_AddExe						(Class *cl, Object *obj, Msg msg);
METHOD Project_RunExe						(Class *cl, Object *obj, Msg msg);
METHOD Project_MsgBrowser				(Class *cl, Object *obj, Msg msg);
METHOD Project_Add_ChildObj			(Class *cl, Object *obj, Msg msg);
METHOD Project_AddFile					(Class *cl, Object *obj, Msg msg);
METHOD Project_FileType_Default	(Class *cl, Object *obj, Msg msg);
METHOD Project_PM_Add						(Class *cl, Object *obj, Msg msg);
METHOD Project_PM_Delete				(Class *cl, Object *obj, Msg msg);
METHOD Project_PM_Touch					(Class *cl, Object *obj, Msg msg);
METHOD Project_PM_Compile				(Class *cl, Object *obj, Msg msg);
METHOD Project_File_Touch				(Class *cl, Object *obj, struct MUIP_Project_File_Touch_Message *msg);
METHOD Project_File_Info				(Class *cl, Object *obj, struct MUIP_Project_File_Info_Message *msg);
METHOD Project_Load							(Class *cl, Object *obj, struct MUIP_Project_Load_Message *msg);
METHOD Project_Save							(Class *cl, Object *obj, Msg msg);
METHOD Project_Scan_Dependency	(Class *cl, Object *obj, struct MUIP_Project_Scan_Dependency_Message *msg);
METHOD Project_Update_Title			(Class *cl, Object *obj, Msg msg);
METHOD Project_Compile					(Class *cl, Object *obj, struct MUIP_Project_Compile_Message *msg);
METHOD Project_Make							(Class *cl, Object *obj, Msg msg);
METHOD Project_Link							(Class *cl, Object *obj, Msg msg);
METHOD Project_Env_Set					(Class *cl, Object *obj, Msg msg);
METHOD Project_Env_Get					(Class *cl, Object *obj, Msg msg);
METHOD Project_File_Edit				(Class *cl, Object *obj, struct MUIP_Project_File_Edit_Message *msg);
METHOD Project_File_View				(Class *cl, Object *obj, struct MUIP_Project_File_View_Message *msg);
METHOD Project_File_DC					(Class *cl, Object *obj, Msg msg);
METHOD Project_File_RunExe			(Class *cl, Object *obj, Msg msg);
METHOD Project_AddJob						(Class *cl, Object *obj, struct MUIP_Project_AddJob_Message *msg);
METHOD Project_GetJob						(Class *cl, Object *obj, Msg msg);
METHOD Project_DelJob						(Class *cl, Object *obj, Msg msg);
METHOD Project_RunJob						(Class *cl, Object *obj, Msg msg);

// Prototypen für den Konstruktur/Destruktor der Klasse
struct MUI_CustomClass *CreateProjectClass(void);
void                    DeleteProjectClass(struct MUI_CustomClass *CL_Project);

#endif
