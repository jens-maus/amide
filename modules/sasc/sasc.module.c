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

#define __USE_SYSBASE        // perhaps only recognized by SAS/C

#include <clib/alib_protos.h>

#include <dos/dostags.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <utility/hooks.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <clib/debug_protos.h>
#include <string.h>

#ifdef __MAXON__
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#else
#include <proto/exec.h>
#include <proto/intuition.h>
#include <intuition/intuition.h>
#endif

#include <proto/exec.h>

#include "compiler.h"
#include "sasc.module.h"

#include "amide/amide_api.h"

/*
** Global variables
*/

struct IClass *cl = 0;

#ifdef DEBUG
static const char *id="@(#) $Id$";
#endif

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
** AmIDE_API_GetClass
**
** This is the only public function of the shared library
** It should only return the created class from the LibInit.c
**
** parameters:
** ~~~~~~~~~~~
** void
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** struct IClass *: Pointer to the created Class
*/

SAVEDS ASM struct IClass *AmIDE_API_GetClass(void)
{
  return(cl);
}

/***************************************************************************************
**
** Here follows the internal Methods and dispatcher function
**
*/

/*
** AmIDE_API_New
**
** Method to create the object
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

METHOD MODULE_New(Class *cl, Object *obj, struct opSet *msg)
{
  struct MODS_InstData *Data;

  Object *newobj;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(newobj = (Object *)DoSuperMethodA(cl, obj, (Msg) msg))
  {
    // get the InstData
    Data  = INST_DATA(cl, newobj);

    // get the attributes from the NewObject() call
    Data->parent_obj = (Object *)GetTagData(AmIDEA_API_parentObj, NULL, msg->ops_AttrList);
  }

  return((ULONG)newobj);
}

/*
** AmIDE_API_Get
**
** Method to get attributes of an object
**
** parameters:
** ~~~~~~~~~~
** Class *        : The created class
** Object *       : The object
** struct opGet * : The opGet Message
**
** return-values:
** ~~~~~~~~~~~~~~
** METHOD: 0 at sucess
*/

METHOD MODULE_Get(Class *cl, Object *obj, struct opGet *msg)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  switch (msg->opg_AttrID)
  {
    case AmIDEA_API_author:   { *(msg -> opg_Storage) = (ULONG)AUTHOR;          } break;
    case AmIDEA_API_api_vers: { *(msg -> opg_Storage) = (ULONG)API_VERS;        } break;
    case AmIDEA_API_lib_vers: { *(msg -> opg_Storage) = (ULONG)LIBNAME LIBVER;  } break;
    case AmIDEA_API_compiler: { *(msg -> opg_Storage) = (ULONG)COMPILER;        } break;

    default: { DoSuperMethodA(cl, obj, (Msg) msg); break; };
  }

  return(0);
}

/*
** AmIDE_API__Set
** The OM_SET Method of the object
** With this method all attributes will be set
**
** parameters:
** ~~~~~~~~~~
** Class *        : The created class
** Object *       : The object
** struct opSet * : The opSet Message structure
**
** return values:
** ~~~~~~~~~~~~~
** 0
*/

METHOD MODULE_Set(Class *cl, Object *obj, struct opSet *msg)
{
  struct MODS_InstData *Data = INST_DATA(cl, obj);
  struct TagItem *newdata;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // find the TagItem and set the variable
  if (newdata = FindTagItem(AmIDEA_API_parentObj, msg -> ops_AttrList))
  {
    Data->parent_obj = (Object *)(newdata->ti_Data);
  }

  // All other item are not belonging to us
  DoSuperMethodA(cl, obj, (Msg) msg);

  return(0);
}

/*
** AmIDE_API_compile
**
** This method is the MAIN Method of this shared-library.
** It handles all the compilation stuff. Will start the compiler with the correct options
** and also returns the compiler messages by calling a AmIDE Method.
** It should also preparse the messages and insert standard MUI-NList styles to the
** message
**
** parameters:
** ~~~~~~~~~~
** Class *  : The created class
** Object * : The object
** Msg      : The Message
**
** return-values:
** ~~~~~~~~~~~~~~
** METHOD: 0 at sucess
*/

METHOD MODULE_Compile(Class *cl, Object *obj, struct AmIDEP_API_compile_Message *msg)
{
  struct MODS_InstData *Data = INST_DATA(cl, obj);

  char command[256];
  BPTR fh, nil;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Set log position to zero
  Data->log_position = 0;

  // create logfile-name
  sprintf(Data->logfile, "T:AmIDE-%s-compile.log", FilePart(msg->filename));

  // delete existing log-file
  DeleteFile(Data->logfile);

  fh = Open(Data->logfile, MODE_READWRITE);
  nil = Open("NIL:", MODE_OLDFILE);

  if(fh)
  {
    BOOL rc;

    // create the execute command
    sprintf(command, "sc:c/sc %s VERBOSE", msg->filename);

    rc = SystemTags(  command,
                      SYS_Input,      nil,
                      SYS_Output,     fh,
                      SYS_Asynch,     TRUE,
                      SYS_UserShell,  TRUE,
                      NP_Priority,    0,
                      NP_Error,       fh,
                      NP_WindowPtr,   NULL,
                      NP_Cli,         TRUE,
                      TAG_DONE);

    if(!rc)
    {
      BPTR lock;

      while(!(lock = Lock(Data->logfile, ACCESS_WRITE)))
      {
        // Das Logfile einlesen
        DoMethod(obj, MODM_Logfile_Read, TAG_DONE);
      }
      if(lock) UnLock(lock);

      // if there are something left in the logfile
      while(Data->log_position != FileSize(Data->logfile)) DoMethod(obj, MODM_Logfile_Read, TAG_DONE);

    }

  }

  return(0);
}

/*
** MODULE_Link
** Method to link all files passed in the message
**
** parameters:
** ~~~~~~~~~~~
** Class * : the class itself
** Object *: the object
** Msg     : the AmIDEP_API_link_Message
**
** return-values:
** ~~~~~~~~~~~~~~
** METHOD: The new object or NULL
*/

METHOD MODULE_Link(Class *cl, Object *obj, struct AmIDEP_API_link_Message *msg)
{
  struct MODS_InstData *Data = INST_DATA(cl, obj);

  STRPTR *file_array = msg->file_array;
  STRPTR exe_name    = msg->exe_name;

  char obj_list[1024] = "";
  char lib_list[1024] = "";
  char command[256];
  BOOL rc;
  BPTR lock;
  BPTR fh, nil;
  LONG i;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  // Now we have to create the temporary WITH file
  for(i=0; file_array[i]; i++)
  {
    // Now we search for .lib files so that this one will be linked together with the other object files
    if(MatchNoCase(file_array[i], "#?.lib"))
    {
      strcat(lib_list, file_array[i]);
      strcat(lib_list, " ");

    }else
    {
      strcat(obj_list, file_array[i]);
      strcat(obj_list, " ");
    }
  }

  if(fh = Open("T:AmIDE.tmp", MODE_NEWFILE))
  {
    FPrintf(fh, "VERBOSE\n");
    FPrintf(fh, "TO %s\n", exe_name);
    FPrintf(fh, "FROM %s\n", obj_list);
    FPrintf(fh, "LIBRARY %s\n", lib_list);

    Close(fh);
  }

  strcpy(command, "sc:c/slink WITH T:AmIDE.tmp");

  // first we set the log position to zero
  Data->log_position = 0;

  // create logfile-name
  strcpy(Data->logfile, "T:AmIDE-link.log");

  // delete a existing log file first
  DeleteFile(Data->logfile);

  fh = Open(Data->logfile, MODE_READWRITE);
  nil = Open("NIL:", MODE_OLDFILE);

  if(fh)
  {
    rc = SystemTags(  command,
                      SYS_Input,      nil,
                      SYS_Output,     fh,
                      SYS_Asynch,     TRUE,
                      SYS_UserShell,  TRUE,
                      NP_Priority,    0,
                      NP_Error,       fh,
                      NP_WindowPtr,   NULL,
                      NP_Cli,         TRUE,
                      TAG_DONE);

    if(!rc)
    {

      while(!(lock = Lock(Data->logfile, ACCESS_WRITE)))
      {
        // read the logfile now
        DoMethod(obj, MODM_Logfile_Read, TAG_DONE);
      }
      if(lock) UnLock(lock);

      // check if there is something left in the logfile and read the rest again
      while(Data->log_position != FileSize(Data->logfile)) DoMethod(obj, MODM_Logfile_Read, TAG_DONE);
    }

  }

  return(0);
}

/*
** MODULE_Log_Read
** Method for reading the logfile and parsing it step by step
**
** parameters:
** ~~~~~~~~~~~
** Class * : the class itself
** Object *: the object
** Msg     : the Message
**
** return-values:
** ~~~~~~~~~~~~~~
** METHOD: NULL
*/

METHOD MODULE_Log_Read(Class *cl, Object *obj, Msg msg)
{
  struct MODS_InstData *Data = INST_DATA(cl, obj);

  STRPTR  buffer, endstring;
  BPTR    mfile;
  ULONG   size = Data->log_position;
  ULONG   newsize;
  APTR    tmpbuf;

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  newsize = FileSize(Data->logfile);

  if(size != newsize && newsize > 0)
  {
    if(mfile = Open(Data->logfile, MODE_OLDFILE))
    {
        buffer = tmpbuf = calloc((size_t)((newsize-size)+1), sizeof(char));

        if(!buffer || !tmpbuf)
        {
          Close(mfile);
          return(0);
        }

        Seek(mfile, size, OFFSET_BEGINNING);
        FRead(mfile, buffer, 1, (newsize-size));
        Close(mfile);

        while(endstring = strchr(buffer, '\n'))
        {
            *endstring = 0;
            Data->msg_text = buffer;
            DoMethod(obj, MODM_Msg_Parse, 0);
            buffer = endstring + 1;
        }

        // If there is already a string
        if(strlen(buffer) > 0)
        {
          Data->msg_text = buffer;
          DoMethod(obj, MODM_Msg_Parse, 0);
          newsize = newsize + 1;
        }

        // set the position to which we parsed the logfile yet
        Data->log_position = newsize;

        free(tmpbuf);
    }
  }

  return(0);
}

/*
** MODULE_Msg_Parse
** Method to parse the strings from the logfile and modify them to get it back to AmIDE
**
** parameters:
** ~~~~~~~~~~~
** Class * : the class itself
** Object *: the object
** Msg     : the Message
**
** return-values:
** ~~~~~~~~~~~~~~
** METHOD: the new object or NULL
*/

METHOD MODULE_Msg_Parse(Class *cl, Object *obj, Msg msg)
{
  struct MODS_InstData *Data = INST_DATA(cl, obj);

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  /*
  ** messages that we ignore and don`t want to be displayed in AmIDE
  */

  if(strlen(Data->msg_text) < 4)                          return(0);
  if(strstr(Data->msg_text, "Using options file"))        return(0);
  if(strstr(Data->msg_text, "Pass1 for"))                 return(0);
  if(strstr(Data->msg_text, "Pass2 for"))                 return(0);
  if(strstr(Data->msg_text, "\033[F"))                    return(0);
  if(strstr(Data->msg_text, "\033[K"))                    return(0);
  if(strstr(Data->msg_text, "Slink -"))                   return(0);

  /*
  ** The following messages are relevant ones so we insert them with MODM_Msg_Insert into the AmIDE MsgBrowser
  */

  if(strcmp(Data->msg_text, "====================") == 0) { return(DoMethod(obj, MODM_Msg_Insert, MODA_MSG_BAR,         TAG_DONE));  };
  if(strstr(Data->msg_text, " Warning "))                 { return(DoMethod(obj, MODM_Msg_Insert, MODA_MSG_WARNING,     TAG_DONE));  };
  if(strstr(Data->msg_text, " Note "))                    { return(DoMethod(obj, MODM_Msg_Insert, MODA_MSG_NOTE,        TAG_DONE));  };
  if(strstr(Data->msg_text, "\033[33;7m"))                { return(DoMethod(obj, MODM_Msg_Insert, MODA_MSG_WARNING_TXT, TAG_DONE));  };
  if(strstr(Data->msg_text, " Error "))                   { return(DoMethod(obj, MODM_Msg_Insert, MODA_MSG_ERROR,       TAG_DONE));  };
  if(strstr(Data->msg_text, "Compiler Phase 1"))          { return(DoMethod(obj, MODM_Msg_Insert, MODA_MSG_COMPILER_P1, TAG_DONE));  };
  if(strstr(Data->msg_text, "Compiler Phase 2"))          { return(DoMethod(obj, MODM_Msg_Insert, MODA_MSG_COMPILER_P2, TAG_DONE));  };
  if(strstr(Data->msg_text, "Global Optimizer"))          { return(DoMethod(obj, MODM_Msg_Insert, MODA_MSG_OPTIMIZER,   TAG_DONE));  };


  return(DoMethod(obj, MODM_Msg_Insert, MODA_MSG_DEFAULT, TAG_DONE));
}

/*
** MODULE_Msg_Insert
** Method to send the parsed strings to AmIDE in the correct format
**
** parameters:
** ~~~~~~~~~~~
** Class * : the class itself
** Object *: the object
** Msg     : the MODS_Msg_Insert Message
**
** return-values:
** ~~~~~~~~~~~~~~
** METHOD: The new object or NULL
*/

METHOD MODULE_Msg_Insert(Class *cl, Object *obj, struct MODS_Msg_Insert *msg)
{
  struct MODS_InstData *Data = INST_DATA(cl, obj);

  // First we allocate memory for a message
  struct AmIDES_message *message = calloc(1, sizeof(struct AmIDES_message));

  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  if(!message) return(0);

  // fill it with some standard values
  message->errnr = -1;
  message->line  = -1;
  message->text  = NULL;

  // Depending on the message type we are going to modify it different and send it to AmIDE
  switch(msg->msg_type)
  {
    //
    // If this is a BAR we send a MUI BAR string
    //
    case MODA_MSG_BAR:
    {
      message->text = calloc((strlen(AmIDE_MSG_STYLE_BAR)+10), sizeof(char));
      if(!message->text) return(0);

      strcpy(message->text, AmIDE_MSG_STYLE_BAR);

      message->type = AmIDE_MSG_DEFAULT;
    }
    break;

    //
    // If this is a NOTE/EROOR or WARNING we do the following
    //
    case MODA_MSG_NOTE:
    case MODA_MSG_ERROR:
    case MODA_MSG_WARNING:
    {
      int    pos;
      char   tmp_str2[256];
      char   tmp_type[256];
      STRPTR tmp_errnr;
      STRPTR tmp_line;
      STRPTR tmp_str;

      //
      // Now we parse the string and get out error-number and line-number

      // read the message text
      tmp_str = strstr(Data->msg_text, ": ")+2;

      message->text = calloc((strlen(tmp_str)+1), sizeof(char));
      if(!message->text) return(0);
      strcpy(message->text, tmp_str);

      // now we get the error/warning number out of the string
      pos = strlen(Data->msg_text)-strlen(tmp_str)-2;
      strncpy(tmp_str2, Data->msg_text, pos);
      tmp_str2[pos] = '\0';
      tmp_errnr = strrchr(tmp_str2, ' ')+1;

      message->errnr = atol(tmp_errnr);

      // now lets get the line number
      tmp_str2[strlen(tmp_str2)-strlen(tmp_errnr)-1] = '\0';
      strcpy(tmp_type, strrchr(tmp_str2, ' ')+1);
      tmp_str2[strlen(tmp_str2)-strlen(tmp_type)-1] = '\0';
      tmp_line = strrchr(tmp_str2, ' ')+1;

      message->line = atol(tmp_line);

      switch(msg->msg_type)
      {
        case MODA_MSG_NOTE:     { message->type = AmIDE_MSG_NOTE;     } break;
        case MODA_MSG_ERROR:    { message->type = AmIDE_MSG_ERROR;    } break;
        case MODA_MSG_WARNING:  { message->type = AmIDE_MSG_WARNING;  } break;
      }
    }
    break;

    //
    // If this is the text of a warning message
    //
    case MODA_MSG_WARNING_TXT:
    {
      LONG tmp;
      STRPTR tmp_str = Data->msg_text;

      message->text = calloc(256, sizeof(char));
      if(!message->text) return(0);

      // Now we copy everything in front of the ANSI characters
      tmp = (strlen(tmp_str)-strlen(strstr(tmp_str, "\033[33;7m")));
      strncpy(message->text, tmp_str, (size_t)tmp);

      // Now insert some ESC sequences for MUI
      strcat(message->text, AmIDE_MSG_STYLE_TXT_START);

      // Now we copy the text in between of the ANSI chars.
      tmp_str += (tmp+7);

      tmp =  (strlen(tmp_str)-strlen(strstr(tmp_str, "\033[0m")));
      strncat(message->text, tmp_str, (size_t)tmp);

      // and again we append some ESC sequeneces
      strcat(message->text, AmIDE_MSG_STYLE_TXT_STOP);

      // and don`t forget the rest of the text
      tmp_str += (tmp+4);
      strcat(message->text, tmp_str);

      message->type = AmIDE_MSG_WARNING;
    }
    break;

    //
    // For Compiler-Phase1 messages of SAS/C
    //
    case MODA_MSG_COMPILER_P1:
    {
      message->status = "compiling phase1...";

      message->text = calloc((strlen(AmIDE_MSG_STYLE_PHASES "Compiler Phase 1:")+1), sizeof(char));
      strcpy(message->text, AmIDE_MSG_STYLE_PHASES "Compiler Phase 1:");

      message->type = AmIDE_MSG_DEFAULT;
    }
    break;

    //
    // For Compiler-Phase2 messages of SAS/C
    //
    case MODA_MSG_COMPILER_P2:
    {
      message->status = "compiling phase2...";

      message->text = calloc((strlen(AmIDE_MSG_STYLE_PHASES "Compiler Phase 2:")+1), sizeof(char));
      strcpy(message->text, AmIDE_MSG_STYLE_PHASES "Compiler Phase 2:");

      message->type = AmIDE_MSG_DEFAULT;
    }
    break;

    //
    // For Global-Optimizer messages
    //
    case MODA_MSG_OPTIMIZER:
    {
      message->status = "global optimizing...";

      message->text = calloc((strlen(AmIDE_MSG_STYLE_PHASES "Global Optimizer:")+1), sizeof(char));
      strcpy(message->text, AmIDE_MSG_STYLE_PHASES "Global Optimizer:");

      message->type = AmIDE_MSG_DEFAULT;
    }
    break;

    //
    // For default messages we just send the whole string
    //
    case MODA_MSG_DEFAULT:
    {
      message->text = calloc((strlen(Data->msg_text)+1), sizeof(char));
      strcpy(message->text, Data->msg_text);

      message->type = AmIDE_MSG_DEFAULT;
    }
    break;
  }

  // main method call to insert the message into AmIDE
  DoMethod(Data->parent_obj, AmIDEM_msginsert, message);

  // Now lets free the allocated stuff
  if(message->text)   free(message->text);
  if(message)         free(message);

  return(0);
}


/*
** MODULE_Dispatcher
**
** This is the dispatcher for the module library
**
** parameters
** ~~~~~~~~~~
** Class *    (a0): the class of the object
** Object *   (a2): the object
** Msg        (a1): the message (METHOD_IDs)
**
** return-values:
** ~~~~~~~~~~~~~~
** ULONG: the new object
*/

SAVEDS ASM ULONG MODULE_Dispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg)
{
  #ifdef DEBUG
  DEBUG_MAKRO
  #endif

  switch (msg->MethodID)
  {
    // public Methods (defined in the API)
    case OM_NEW                     : return(MODULE_New         (cl, obj, (APTR)msg));
    case OM_GET                     : return(MODULE_Get         (cl, obj, (APTR)msg));
    case OM_SET                     : return(MODULE_Set         (cl, obj, (APTR)msg));
    case AmIDEM_API_compile         : return(MODULE_Compile     (cl, obj, (APTR)msg));
    case AmIDEM_API_link            : return(MODULE_Link        (cl, obj, (APTR)msg));

// Not implemented yet:
//    case AmIDEM_API_break           : return(MODULE_break       (cl, obj, (APTR)msg));
//    case AmIDEM_API_prefs           : return(MODULE_prefs       (cl, obj, (APTR)msg));

    // private Methods (only for internal module use)
    case MODM_Logfile_Read          : return(MODULE_Log_Read    (cl, obj, (APTR)msg));
    case MODM_Msg_Parse             : return(MODULE_Msg_Parse   (cl, obj, (APTR)msg));
    case MODM_Msg_Insert            : return(MODULE_Msg_Insert  (cl, obj, (APTR)msg));
  }

  return (DoSuperMethodA(cl, obj, msg));
}
