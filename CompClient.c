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
#include "MsgBrowser.h"
#include "Project.h"

/*
** Comp_Client
** Funktion die von AmIDE aus per CreateNewProc als subtask aufgerufen wird.
** Da dies ein subtask ist muss er auch alle Libraries öffnen.
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** void
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** BOOL - TRUE falls sich der Subtask korrekt beendet hat
**        FALSE falls ein Fehler aufgetreten ist
*/

BOOL Comp_Client(void)
{
	struct CompAmIDE_Message *to_send;
  struct AmIDEComp_Message *msg;
	struct MsgPort *reply_port;
	struct MsgPort *dest_port=NULL;
	struct Task *me;
	char mport_name[32];
	char base_name[32];
	char pipe_name[32];
  struct MUIS_Project_Job *job;

	geta4();

	if((to_send = (struct CompAmIDE_Message *)calloc(1, sizeof(struct CompAmIDE_Message))))
	{
  	if (reply_port = CreatePort(0, 0))
		{
			me = FindTask(NULL);
      if(!me) return(FALSE);

      // Den MsgPortnamen des Servers zusammenbauen
			sscanf(me->tc_Node.ln_Name, "%s ", &base_name);
			sprintf(mport_name,"%s_mp", base_name);

			dest_port=FindPort(mport_name);
      if(!dest_port) return(FALSE);
			//KPrintf("CLIENT[%lx]: |%s| - %lx - %lx\n", me, mport_name, dest_port, dest_port->mp_SigTask);

      // Die Message zusammenbauen
      to_send->msg.mn_Node.ln_Type = NT_MESSAGE;
      to_send->msg.mn_Length = sizeof(struct CompAmIDE_Message);
      to_send->msg.mn_ReplyPort = reply_port;

      // Jetzt holen wir die Daten aus der Taskstruktur
      // Die Message auf der UserData holen
      msg = (struct AmIDEComp_Message *)me->tc_UserData;
      to_send->OB_Project = msg->OB_Project;

      // Jetzt schauen wir ob etwas in der Jobliste steht und gehen diese durch
      while(job = (struct MUIS_Project_Job *)SafeSend(MSG_CC_JOBREQUEST, to_send, mport_name))
      {
        switch(job->type)
        {
          case MUIV_Project_JOB_COMPILE:
          {
            DoMethod(msg->OB_Compiler, AmIDEM_API_compile, job->file, TAG_DONE);
          }
        	break;

        	case MUIV_Project_JOB_LINK:
          {
            DoMethod(msg->OB_Compiler, AmIDEM_API_link, job->file, job->exe, TAG_DONE);
          }
        	break;
        }

      	// AmIDE sagen das der Job beendet wurde
		    SafeSend(MSG_CC_JOBFINISHED, to_send, mport_name);
			}
		}
    free(to_send);
	}

  return(TRUE);
}

/*
** SafeSend
** Funktion die mit dem MessagePort von AmIDE kommuniziert und auf eine Antwort wartet
**
** Übergabeparameter:
** ~~~~~~~~~~~~~~~~~
** void
**
** Rückgabewerte:
** ~~~~~~~~~~~~~
** ULONG - Bei Erfolg die Antwort von AmIDE
**         0 bei Misserfolg
*/

ULONG SafeSend(ULONG data, struct CompAmIDE_Message *to_send, STRPTR dest)
{
    struct MsgPort *ans_port;
    struct MsgPort *send_port;
    struct CompAmIDE_Message *answer;

    ans_port = to_send->msg.mn_ReplyPort;
    to_send->data = data;

    if (dest)
    {
        // lets put the message in the MessagePort
        Forbid();
        send_port = FindPort(dest);
        if(send_port) PutMsg(send_port, (struct Message *)to_send);
        Permit();

        /* waits for the answering message */
        WaitPort(ans_port);
        answer = (struct CompAmIDE_Message *)GetMsg(ans_port);

        return(answer->data);
    }
    else return(0);
}
