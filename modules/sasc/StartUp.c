/*
**      $VER: StartUp.c 37.31 (18.3.98)
**
**      Library startup-code and function table definition
**
**      (C) Copyright 1996-98 Andreas R. Kleinert
**      All Rights Reserved.
**
**      Modifications for the AmIDE_API v1.0
**      (C) Copyright 2001 Jens Langner
**      All Rights Reserved.
**
*/

#define __USE_SYSBASE        // perhaps only recognized by SAS/C

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/initializers.h>


#ifdef __MAXON__
#include <pragma/exec_lib.h>
#include <linkerfunc.h>
#else
#include <proto/exec.h>    // all other compilers
#endif
#include "compiler.h"
#include "sasc.module.h"

#include <amide/amide_api_base.h>

extern ULONG __saveds __stdargs L_OpenLibs(struct AmIDE_API_Base *API_Base);
extern void  __saveds __stdargs L_CloseLibs(void);

struct AmIDE_API_Base * __saveds ASM InitLib( register __a6 struct ExecBase    *sysbase GNUCREG(a6),
                                              register __a0 SEGLISTPTR          seglist GNUCREG(a0),
                                              register __d0 struct AmIDE_API_Base *exb     GNUCREG(d0));
struct AmIDE_API_Base * __saveds ASM OpenLib( register __a6 struct AmIDE_API_Base *API_Base GNUCREG(a6));
SEGLISTPTR __saveds ASM CloseLib( register __a6 struct AmIDE_API_Base *API_Base GNUCREG(a6));
SEGLISTPTR __saveds ASM ExpungeLib( register __a6 struct AmIDE_API_Base *exb GNUCREG(a6));
ULONG ASM ExtFuncLib(void);


/* ----------------------------------------------------------------------------------------
   ! LibStart:
   !
   ! If someone tries to start a library as an executable, it must return (LONG) -1
   ! as result. That's what we are doing here.
   ---------------------------------------------------------------------------------------- */

LONG ASM LibStart(void)
{
 return(-1);
}


/* ----------------------------------------------------------------------------------------
   ! Function and Data Tables:
   !
   ! The function and data tables have been placed here for traditional reasons.
   ! Placing the RomTag structure before (-> LibInit.c) would also be a good idea,
   ! but it depends on whether you would like to keep the "version" stuff separately.
   ---------------------------------------------------------------------------------------- */

extern APTR FuncTab [];
/*  extern struct MyDataInit DataTab;  */
extern DataTab; /* DICE fix */
                                  /* Instead you may place ROMTag + Datatab directly, here */
                                  /* (see LibInit.c). This may fix "Installer" version     */
                                  /* checking problems, too - try it.                      */

struct InitTable                       /* do not change */
{
 ULONG              LibBaseSize;
 APTR              *FunctionTable;
 struct MyDataInit *DataTable;
 APTR               InitLibTable;
} InitTab =
{
 (ULONG)               sizeof(struct AmIDE_API_Base),
 (APTR              *) &FuncTab[0],
 (struct MyDataInit *) &DataTab,
 (APTR)                InitLib
};

APTR FuncTab [] =
{
 OpenLib,
 CloseLib,
 ExpungeLib,
 ExtFuncLib,

 AmIDE_API_GetClass,  /* add your own functions here */

 (APTR) ((LONG)-1)
};


extern struct AmIDE_API_Base *API_Base;

/* ----------------------------------------------------------------------------------------
   ! InitLib:
   !
   ! This one is single-threaded by the Ramlib process. Theoretically you can do, what
   ! you like here, since you have full exclusive control over all the library code and data.
   ! But due to some bugs in Ramlib V37-40, you can easily cause a deadlock when opening
   ! certain libraries here (which open other libraries, that open other libraries, that...)
   !
   ---------------------------------------------------------------------------------------- */

struct AmIDE_API_Base * __saveds ASM InitLib( register __a6 struct ExecBase      *sysbase GNUCREG(a6),
                                           register __a0 SEGLISTPTR            seglist GNUCREG(a0),
                                           register __d0 struct AmIDE_API_Base   *exb     GNUCREG(d0))
{
 API_Base = exb;

 API_Base->exb_SysBase = sysbase;
 API_Base->exb_SegList = seglist;

 if(L_OpenLibs(API_Base)) return(API_Base);

 L_CloseLibs();

  {
   ULONG negsize, possize, fullsize;
   UBYTE *negptr = (UBYTE *) API_Base;

   negsize  = API_Base->exb_LibNode.lib_NegSize;
   possize  = API_Base->exb_LibNode.lib_PosSize;
   fullsize = negsize + possize;
   negptr  -= negsize;

   FreeMem(negptr, fullsize);

   #ifdef __MAXON__
   CleanupModules();
   #endif
  }

 return(NULL);
}

/* ----------------------------------------------------------------------------------------
   ! OpenLib:
   !
   ! This one is enclosed within a Forbid/Permit pair by Exec V37-40. Since a Wait() call
   ! would break this Forbid/Permit(), you are not allowed to start any operations that
   ! may cause a Wait() during their processing. It's possible, that future OS versions
   ! won't turn the multi-tasking off, but instead use semaphore protection for this
   ! function.
   !
   ! Currently you only can bypass this restriction by supplying your own semaphore
   ! mechanism.
   ---------------------------------------------------------------------------------------- */

struct AmIDE_API_Base * __saveds ASM OpenLib( register __a6 struct AmIDE_API_Base *API_Base GNUCREG(a6))
{
 #ifdef __MAXON__
 GetBaseReg();
 InitModules();
 #endif

 API_Base->exb_LibNode.lib_OpenCnt++;

 API_Base->exb_LibNode.lib_Flags &= ~LIBF_DELEXP;

 return(API_Base);
}

/* ----------------------------------------------------------------------------------------
   ! CloseLib:
   !
   ! This one is enclosed within a Forbid/Permit pair by Exec V37-40. Since a Wait() call
   ! would break this Forbid/Permit(), you are not allowed to start any operations that
   ! may cause a Wait() during their processing. It's possible, that future OS versions
   ! won't turn the multi-tasking off, but instead use semaphore protection for this
   ! function.
   !
   ! Currently you only can bypass this restriction by supplying your own semaphore
   ! mechanism.
   ---------------------------------------------------------------------------------------- */

SEGLISTPTR __saveds ASM CloseLib( register __a6 struct AmIDE_API_Base *API_Base GNUCREG(a6))
{
 API_Base->exb_LibNode.lib_OpenCnt--;

 if(!API_Base->exb_LibNode.lib_OpenCnt)
  {
   if(API_Base->exb_LibNode.lib_Flags & LIBF_DELEXP)
    {
     return( ExpungeLib(API_Base) );
    }
  }

 return(NULL);
}

/* ----------------------------------------------------------------------------------------
   ! ExpungeLib:
   !
   ! This one is enclosed within a Forbid/Permit pair by Exec V37-40. Since a Wait() call
   ! would break this Forbid/Permit(), you are not allowed to start any operations that
   ! may cause a Wait() during their processing. It's possible, that future OS versions
   ! won't turn the multi-tasking off, but instead use semaphore protection for this
   ! function.
   !
   ! Currently you only could bypass this restriction by supplying your own semaphore
   ! mechanism - but since expunging can't be done twice, one should avoid it here.
   ---------------------------------------------------------------------------------------- */

SEGLISTPTR __saveds ASM ExpungeLib( register __a6 struct AmIDE_API_Base *exb GNUCREG(a6))
{
 struct AmIDE_API_Base *API_Base = exb;
 SEGLISTPTR seglist;

 if(!API_Base->exb_LibNode.lib_OpenCnt)
  {
   ULONG negsize, possize, fullsize;
   UBYTE *negptr = (UBYTE *) API_Base;

   seglist = API_Base->exb_SegList;

   Remove((struct Node *)API_Base);

   L_CloseLibs();

   negsize  = API_Base->exb_LibNode.lib_NegSize;
   possize  = API_Base->exb_LibNode.lib_PosSize;
   fullsize = negsize + possize;
   negptr  -= negsize;

   FreeMem(negptr, fullsize);

   #ifdef __MAXON__
   CleanupModules();
   #endif

   return(seglist);
  }

 API_Base->exb_LibNode.lib_Flags |= LIBF_DELEXP;

 return(NULL);
}

/* ----------------------------------------------------------------------------------------
   ! ExtFunct:
   !
   ! This one is enclosed within a Forbid/Permit pair by Exec V37-40. Since a Wait() call
   ! would break this Forbid/Permit(), you are not allowed to start any operations that
   ! may cause a Wait() during their processing. It's possible, that future OS versions
   ! won't turn the multi-tasking off, but instead use semaphore protection for this
   ! function.
   !
   ! Currently you only can bypass this restriction by supplying your own semaphore
   ! mechanism - but since this function currently is unused, you should not touch
   ! it, either.
   ---------------------------------------------------------------------------------------- */

ULONG ASM ExtFuncLib(void)
{
 return(NULL);
}

struct AmIDE_API_Base *API_Base = NULL;


/* ----------------------------------------------------------------------------------------
   ! __SASC stuff:
   !
   ! This is only for SAS/C - its intention is to turn off internal CTRL-C handling
   ! for standard C functions and to avoid calls to exit() et al.
   ---------------------------------------------------------------------------------------- */

#ifdef __SASC

#ifdef ARK_OLD_STDIO_FIX

ULONG XCEXIT       = NULL; /* These symbols may be referenced by    */
ULONG _XCEXIT      = NULL; /* some functions of sc.lib, but should  */
ULONG ONBREAK      = NULL; /* never be used inside a shared library */
ULONG _ONBREAK     = NULL;
ULONG base         = NULL; /* Note, that XCEXIT/ONBREAK actually    */
ULONG _base        = NULL; /* should have been defined as functions */
ULONG ProgramName  = NULL; /* and not as ULONGs...                  */
ULONG _ProgramName = NULL;
ULONG StackPtr     = NULL;
ULONG _StackPtr    = NULL;
ULONG oserr        = NULL;
ULONG _oserr       = NULL;
ULONG OSERR        = NULL;
ULONG _OSERR       = NULL;

#endif /* ARK_OLD_STDIO_FIX */

void __regargs __chkabort(void) { }  /* a shared library cannot be    */
void __regargs _CXBRK(void)     { }  /* CTRL-C aborted when doing I/O */

#endif /* __SASC */
