
/* FILE: error.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Error handling Routines for the Cutplane Editor              */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define ERRORMODULE

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>
#include <fcntl.h>

#include <platform_gl.h>

#ifdef IRIS
#include <extern.h>
#else
#include <cutplane.h>
#endif

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			GLOBAL VARIABLES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

Boolean enduser_system = FALSE;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                         ERROR HANDLING ROUTINES                          */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


void
reset_grafx()
{
#ifdef IRIS
#undef setvaluator
  setvaluator(MOUSEX,getgdesc(GD_XPMAX)/2,0,getgdesc(GD_XPMAX));
  setvaluator(MOUSEY,getgdesc(GD_YPMAX)/2,0,getgdesc(GD_YPMAX));
#define setvaluator gl_setvaluator
  attachcursor(MOUSEX,MOUSEY);
  setcursor(0,0,0);
  curson();
  gexit();
#endif
}


void
restart_3form(void)
{
#ifdef IRIS
  char *arg_strings[2];
  int i,numfds,thisfd,childpid;
  extern char **environ;
  static char path[82] = "./";
  ;
  /* environment save goes here */
  /*
    write_data_file("dump.cut",(featureptr) primary_world->root);
  */
  fprintf(dbgdump,
	  "\nRESTARTING 3FORM: current environment is saved in dump.cut\n");
  fprintf(dbgdump,
	  "\nTO ABORT RESTART: MOVE CURSOR INTO THIS WINDOW AND TYPE <CTRL>-C\n");
  sleep(5);
  
  /*  set close-on-exec flag of all non-standard files so that they are */
  /*  closed on a successful execve                              */
  numfds = getdtablesize();
  for (thisfd = 3; thisfd < numfds; thisfd++)
    fcntl(thisfd,F_SETFD,TRUE);
  
  /* set up argv array */
  arg_strings[0] = execfilename;
  arg_strings[1] = NULL;
  
  strcat(path,execfilename);
  execve(path,arg_strings,environ);
  
  fprintf(stderr,"FATAL ERROR: Can't execute %s.\n",execfilename);
  fflush(stderr);
  _exit(1);
#endif
}


void
system_error(char *errorstr)
{
  if (enduser_system)
  {
#ifdef IRIS
    reset_grafx();
#endif /* IRIS */

    fprintf(dbgdump, "FATAL ERROR in ");
    fprintf(dbgdump, "%s\n",errorstr);

#ifdef IRIS
    restart_3form();
    longjmp(error_return,TRUE);	/* jump back to begining of main */
#endif /* IRIS */
  }
  else
  {
#ifdef IRIS
    reset_grafx();
#endif /* IRIS */
    fprintf(dbgdump,"FATAL ERROR in ");
    fprintf(dbgdump,"%s\n",errorstr);
    fprintf(dbgdump,"press <ctrl>-C to exit\n");
#ifdef IRIS
    pause();
#else
    exit(2);
#endif
  }
}

void
system_warning(char *errorstr)
{
  fprintf(dbgdump, "WARNING in ");
  fprintf(dbgdump,"%s\n",errorstr);
}


void
#ifdef IRIS
error_handler(int thesignal, int code)
#else
error_handler(int thesignal)
#endif
{
  static char *describesignal[] = {"HANGUP","INTERRUPT","QUIT",
				   "ILLEGAL INSTRUCTION","TRACE TRAP","ABORT",
				   "EMT INSTRUCTION",
				   "FLOATING POINT EXCEPTION","KILL",
				   "BUS ERROR","SEGMENTATION VIOLATION",
				   "BAD ARGUMENT TO SYSTEM CALL",
				   "WRITE ON A PIPE WITH NO ONE TO READ IT",
				   "ALARM CLOCK",
				   "SOFTWARE TERMINATION SIGNAL",
				   "USER-DEFINED SIGNAL 1",
				   "USER-DEFINED SIGNAL 2","DEATH OF A CHILD",
				   "POWER FAIL (NOT RESET WHEN CAUGHT)","STOP",
				   "STOP SIGNAL GENERATED FROM KEYBOARD",
				   "SELECTABLE EVENT PENDING",
				   "INPUT/OUTPUT POSSIBLE",
				   "URGENT CONDITION ON IO CHANNEL",
				   "WINDOW SIZE CHANGES","VIRTUAL TIME ALARM",
				   "PROFILING ALARM","CONTINUE AFTER STOP",
				   "BACKGROUND READ FROM CONTROL TERMINAL",
				   "BACKGROUND WRITE TO CONTROL TERMINAL",
				   "CPU TIME LIMIT EXCEEDED",
				   "FILE SIZE LIMIT EXCEEDED",
				   "USER BREAKPOINT",
				   "USER BREAKPOINT",
				   "INTEGER OVERFLOW",
				   "INTEGER DIVIDE BY ZERO",
				   "INTEGER MULTIPLY OVERFLOW",
				   "INVALID VIRTUAL ADDRESS",
				   "READ-ONLY ADDRESS",
				   "READ BEYOND MAPPED OBJECT"};
  int messgindex;
  ;
  messgindex = thesignal;
#ifndef WIN32

#ifdef IRIS
  if (thesignal == SIGTRAP)
  {
    switch (code)
    {
    case BRK_USERBP: messgindex = 33;
      break;
    case BRK_SSTEPBP: messgindex = 34;
      break;
    case BRK_OVERFLOW: messgindex = 35;
      break;
    case BRK_DIVZERO: messgindex = 36;
      break;
    case BRK_MULOVF: messgindex = 37;
      break;
    }
  }
  else if (thesignal == SIGSEGV)
  {
    switch (code)
    {
    case EFAULT:  messgindex = 38;
      break;
    case EACCES:  messgindex = 39;
      break;
    case ENXIO:  messgindex = 40;
      break;
    }
  }
#endif

#endif

#ifndef WIN32
  reset_grafx();
#endif

  fprintf(dbgdump, "\n%s SIGNAL CAUGHT.\n",describesignal[messgindex-1]);
  fflush(dbgdump);
  fclose(dbgdump);

  /* reset the signal handling function */
#ifndef WIN32
  signal(SIGSEGV, error_handler);
  signal(SIGBUS, error_handler);

  signal(SIGTRAP, error_handler);
  signal(SIGFPE, error_handler);

  restart_3form();
  longjmp(error_return,TRUE);	/* jump back to begining of main */
#endif
}

#ifndef WIN32
void
init_error_handling(void)
{
  if (enduser_system)
  {
    signal(SIGSEGV, error_handler);
    signal(SIGBUS, error_handler);

    signal(SIGTRAP, error_handler);
    signal(SIGFPE, error_handler);
  }
}
#endif
