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

#ifndef _AMIDE_API_H_
#define _AMIDE_API_H_

/*
** Tag macros
*/

#define AmIDEA_Dummy  (TAG_USER + 0x5EAD0000)
#define AmIDEM_Dummy  (TAG_USER + 0x3AD00000)

/*
** Method-IDs (internal)
*/

#define AmIDEM_API_Dummy      (AmIDEM_Dummy + 0x00000100)

#define AmIDEM_API_compile    (AmIDEM_API_Dummy + 1)
#define AmIDEM_API_link       (AmIDEM_API_Dummy + 2)
#define AmIDEM_API_break      (AmIDEM_API_Dummy + 3)
#define AmIDEM_API_prefs      (AmIDEM_API_Dummy + 4)

/*
** Method-IDs (external)
*/

#define AmIDEM_EXT_Dummy      (AmIDEM_Dummy + 0x00000200)

#define AmIDEM_msginsert      (AmIDEM_EXT_Dummy + 1)
#define AmIDEM_refresh        (AmIDEM_EXT_Dummy + 2)
#define AmIDEM_status         (AmIDEM_EXT_Dummy + 3)

/*
** Attribute - IDs
*/

#define AmIDEA_API_Dummy      (AmIDEA_Dummy + 0x00000100)

#define AmIDEA_API_parentObj  (AmIDEA_API_Dummy + 1)
#define AmIDEA_API_author     (AmIDEA_API_Dummy + 2)
#define AmIDEA_API_api_vers   (AmIDEA_API_Dummy + 3)
#define AmIDEA_API_lib_vers   (AmIDEA_API_Dummy + 4)
#define AmIDEA_API_compiler   (AmIDEA_API_Dummy + 5)

/*
** Message-Type IDs
*/

#define AmIDE_MSG_Dummy       (AmIDEA_Dummy + 0x00000200)

#define AmIDE_MSG_HEADER      (AmIDE_MSG_Dummy + 1)
#define AmIDE_MSG_DEFAULT     (AmIDE_MSG_Dummy + 2)
#define AmIDE_MSG_WARNING     (AmIDE_MSG_Dummy + 3)
#define AmIDE_MSG_ERROR       (AmIDE_MSG_Dummy + 4)
#define AmIDE_MSG_NOTE        (AmIDE_MSG_Dummy + 5)

/*
** Standard Styles for Messages
*/

#define AmIDE_MSG_STYLE_CMPSTART          "\033t[I7]\033r\033E\033b"
#define AmIDE_MSG_STYLE_LNKSTART          "\033t[M7]\033r\033E\033b"
#define AmIDE_MSG_STYLE_WARNING           "\033l\0338"
#define AmIDE_MSG_STYLE_WARNING_BORDER    "\033C\033t[N]"
#define AmIDE_MSG_STYLE_TXT_START         "\033b\033P[-1]"
#define AmIDE_MSG_STYLE_TXT_STOP          "\033n\033P[]"
#define AmIDE_MSG_STYLE_NOTE              "\033P[2]"
#define AmIDE_MSG_STYLE_ERROR             "\033l\033b\033P[-1]"
#define AmIDE_MSG_STYLE_PHASES            "\033u\033b"
#define AmIDE_MSG_STYLE_BAR               "\033C"

/*
** Message structures (internal)
*/

struct AmIDEP_API_compile_Message
{
  ULONG   MethodID;
  STRPTR  filename;
  STRPTR  options;
};

struct AmIDEP_API_link_Message
{
  ULONG   MethodID;
  STRPTR  *file_array;
  STRPTR  exe_name;
};

/*
** Message structure
*/

struct AmIDES_message
{
  ULONG   type;
  LONG    errnr;
  LONG    line;
  STRPTR  text;
  STRPTR  status;
};

/*
** Message structures (external)
*/

struct AmIDEP_msginsert_Message
{
  ULONG   MethodID;
  struct  AmIDES_message *message;
};

#endif /* _AMIDE_API_H_ */
