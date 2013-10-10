
/* FILE: glove.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Glove Reading Routines for the Cutplane Editor            */
/*                                                                        */
/* Author: LJE&WAK                            Date: September 22,1990     */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990,1991 William Kessler, Larry          */
/* Edwards (Beyond Technology, Inc.), and Jim Kramer (Virtex, Inc.)       */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*             HOW TO USE THE GLOVE STUFF ON THE IRIS                     */
/*                                                                        */
/* Make sure the polhemus is 9600 baud, plugged in ttyd2.                 */
/* Make sure the glove is 9600 baud, plugged in ttyd1.                    */
/* Make sure getty is not running on ttyd1. To do this edit               */
/* the line in /etc/inittab:                                              */
/* t1:23:respawn:/etc/getty -s console ttyd1 co_9600 # for console        */
/* to be:                                                                 */
/* t1:23:off:/etc/getty -s console ttyd1 co_9600   # port 1 for glove input */
/*                                                                        */
/* DO NOT comment out the original line, it will still be parsed          */
/*                                                                        */
/* then "telinit q" to kill any getty daemons for ttyd1.                  */
/*                                                                        */
/* Also, change the permissions on /dev/ttyd1 and /dev/ttyd2 to rw for all */
/*                                                                        */
/* The software must set up the tty lines to the proper baud rates because*/
/* getty is not running on those lines anymore.  See                      */
/* init_glove_serialport() and init_polhemus_serialport() for more info,  */
/* or consult your local man pages.                                       */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/


#define GLOVEMODULE

#include <config.h>

#include <stdio.h>

#include <platform_gl.h>

#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termio.h>
#include <fcntl.h>
#include <errno.h>

#include <cutplane.h>		/* cutplane.h includes controls.h */
#include <globals.h>
#include <controls.h>
#include <math3d.h>
#include <cpl_cutplane.h>

#include <glove.h>


/*
#define Debug
*/

extern int	errno;
extern char *sys_errlist[];
extern int	sys_nerr;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			GLOBAL VARIABLES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

static struct termio ostate,nstate;
int glovefd;

glovestatekind glove_state = Gloverelaxstate;
unsigned char glove_wrist_offset;
unsigned char pendingchar;
Boolean pendingchar_read = FALSE;
unsigned char glove_eventchars[Maxglovevent+1] = {'0','p','x','z','s','w','r'};

static CPL_word op_word,as_word,at_word,ad_word;
Boolean dragging = FALSE;	/* this allows you to be dragging in */
				/* regular Glovedragstate or one of the */
				/* constrained states. */

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			LOW LEVEL ROUTINES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

void
read_glove_bytes(char buf[], int nbytes)
{
  int i;
  ;
  for (i=0; i<nbytes; i++)
    while (read(glovefd,&buf[i],1) < 1);
}

  void
init_glove_serialport(void)
{
  glovefd = open("/dev/ttyd1",O_RDWR | O_NDELAY);
  if (glovefd == -1)
  {
    perror ("Error opening glove for reading, milordship! ");
    exit(1);
  }  
  /* save old tty state */
  ioctl(glovefd,TCGETA,&ostate);
  /* set up new tty state */
  nstate = ostate;
  /* input flags */
  nstate.c_iflag &= ~(INLCR | ICRNL | IUCLC | ISTRIP | IXON | BRKINT);
  /* output flags */
  nstate.c_oflag &= ~OPOST;
  /* terminal line flags: set up punctual input and no control characters */
  nstate.c_lflag &= ~(ICANON | ISIG | ECHO);
  nstate.c_cc[VMIN] = 1;	/* incoming char buffer size */
  nstate.c_cc[VTIME] = 0;	/* length time to wait for chars */
  ioctl(glovefd,TCSETA,&nstate);
}

void
close_glove_serialport(void)
{
  /* reset to old state */
  ioctl(glovefd,TCSETA,&ostate);
  close(glovefd);
}

char
read_glove(void)
{
  char data;
  ;
  if (pendingchar_read)
  {
    pendingchar_read = FALSE;
    return(pendingchar);
  }      
  else
  {
    while (read(glovefd,&data,1) < 1);
    return (data);
  }
}

void
write_glove(unsigned char data)
{
  write(glovefd,&data,1);
}

  Boolean 
check_glove()
{
  char data;
  int numread;
  ;
  numread = read(glovefd,&data,1);
  if (numread < 1)
    return(FALSE);
  
  /* push it back in buffer */
  pendingchar_read = TRUE;
  pendingchar = data;
  return(TRUE);
}

  void
init_glove(void)
{
  FILE *gloveinfo;
  char c;
  int numcontrolchars,i = 0;
  ;
  gloveinfo = fopen("./rsc/glove.rsc","r");
  /* reset the globals via the input file */
  fscanf (gloveinfo, "Glove On:%d\n",&using_glove);
  /* more calibration constants can be added here */
  fscanf (gloveinfo, "Number of Control Characters:%d\n",&numcontrolchars);
  while (numcontrolchars-- > 0)
  { /* note that these must coincide in order with the enums in glove.h!! */
    while ((c = getc(gloveinfo)) != ':')
      ;
    fscanf(gloveinfo, "%c\n", &(glove_eventchars[i++]));
  }
  fclose(gloveinfo);

  if (using_glove)
  {
    init_glove_serialport();
    setup_CPL_word_blank(&op_word,Opcode_data);
    setup_CPL_word_blank(&as_word,Blank_word);
    setup_CPL_word_blank(&at_word,Blank_word);
    setup_CPL_word_blank(&ad_word,Blank_word);
  }
  /* make sure init_polhemus called already */
  if ((using_polhemus) && (!using_glove))
    glove_state = Glovepointstate;
  else if (using_glove)
    glove_state = Gloverelaxstate;
}

  void
quit_glove(void)
{
  close_glove_serialport();
}

  gloveventkind
get_glove_event()
{
  char glove_eventchar;
  gloveventkind search_event = Maxglovevent;
  Boolean glove_pending = Noglovevent;
  ;
  glove_pending = check_glove();
  if (glove_pending)
  {
    glove_eventchar = read_glove();
    for (search_event = Noglovevent; search_event < Maxglovevent; ++search_event)
      if (glove_eventchars[search_event] == glove_eventchar)
	break;
  }
  return(search_event);
}

  void
get_glove_wrist()
{
  unsigned char dataprompt = 4;
  ;
#if 0
  fflush(stdout);
  write_glove(dataprompt);
  glove_wrist_offset = read_glove();
#endif
}

  
  void
glove_pick(stateptr state)
{
  /* DONT PICK IF ALREADY IN SOME DRAGGING MODE.. LOVE THOSE ALL CAPS */
  if (!dragging)
  {
    /* If user had multiple select button down, make sure to code */
    /* MPICK instruction instead of PICK instruction.*/
    /* NB Uses illicit access to stuff local to controls.c but hey. */
    setup_CPL_word_blank(&op_word,Opcode_data);
    if (tiedbuttons[(int) Multipleselectbutton].status)
      op_word.word_data.CPLopcode = MPICK;
    else
      op_word.word_data.CPLopcode = PICK;
    setup_CPL_word_blank(&as_word,Proplist_data);
    as_word.word_data.CPLproplist->first.sort->sortdata1->i =
      (int) selectable_prop;
    as_word.contents_of = TRUE;
    store_CPL_RAM_instruction(&op_word,&as_word);
    
    set_drag_origin(state);
    dragging = TRUE;
  }
}

  void
glove_release(stateptr state)
{
  if (dragging)      /* encode a release event if we were dragging */
  {
    /* If user had multiple select button down, make sure to code */
    /* MRELEAS instruction instead of RELEAS instruction.*/
    /* NB Uses illicit access to stuff local to controls.c but hey. */
    setup_CPL_word_blank(&op_word,Opcode_data);
    if (tiedbuttons[(int) Multipleselectbutton].status) 
      op_word.word_data.CPLopcode = MRELEAS;
    else
      op_word.word_data.CPLopcode = RELEAS;
    setup_CPL_word_blank(&as_word,Proplist_data);
    as_word.word_data.CPLproplist->first.sort->sortdata1->i =
      (int) selectable_prop;
    as_word.contents_of = TRUE;
    store_CPL_RAM_instruction(&op_word,&as_word);

    clear_drag_origin(state);
    dragging = FALSE;
  }
}

  Boolean
glove_statechange(stateptr state)
{
  gloveventkind glove_event;
  ;
  glove_event = get_glove_event();
  switch (glove_event)
  {
  case Noglovevent:
    /*    glove_release(state);*/
    glove_state = Gloverelaxstate;
    break;
  case Glovepointevent:
    glove_state = Glovepointstate;
    glove_release(state); /* in case dragging unpick when you re-enter pointing*/
    break;
  case GloveconstraintoXYevent:
    glove_state = GloveconstraintoXYstate;
    glove_release(state); /* in case dragging unpick when you re-enter pointing*/
    break; /* dont change dragging status here... */
  case GloveconstraintoZevent:
    glove_state = GloveconstraintoZstate;
    glove_release(state); /* in case dragging unpick when you re-enter pointing*/
    break; /* dont change dragging status here... */

  case Glovepickevent:
    glove_pick(state);
    glove_state = Glovedragstate;
    break;
  case GloveXYpickevent:
    glove_pick(state);
    glove_state = GloveXYdragstate;
    break;
  case GloveZpickevent:
    glove_pick(state);
    glove_state = GloveZdragstate;
    break;
  case Gloveroomrotevent:
    glove_state = Gloveroomrotstate;
    break;
  case Gloverotatevent:
    glove_pick(state);
    glove_state = Gloverotatestate;
    /* BUT we are NOT dragging in this case */
    dragging = FALSE;
    break;
  case Maxglovevent:
    break;			/* do nothing if no data there at all */
  }
}


  Boolean
update_glove_events(stateptr state)
{
  Boolean events_generated;
  Matrix transformatrix;
  ;
  events_generated = glove_statechange(state);
  if (using_polhemus)			/* do nada unless polhemus in use! */
  {
    update_polhemus_state();

    switch (glove_state)
    {
    case Gloverelaxstate:
      events_generated = FALSE;
      break;
    case Glovepointstate:
    case GloveconstraintoXYstate:
    case GloveconstraintoZstate:
    case Glovedragstate:
    case GloveXYdragstate:
    case GloveZdragstate:
      get_glove_wrist();
      /* generate a crosshair move */
      encode_polhemus_cursor_move(state,glove_state,dragging); 
      break;
    case Gloveroomrotstate:
      get_glove_wrist();
      encode_polhemus_roomrot(state);
      break;
    case Gloverotatestate:
      get_glove_wrist();
      encode_polhemus_transform(state);
      break;
    }
  }
  /* if it isn't in use, things just fall back on mouse input */
  return(events_generated);	/* we return it but it's ignored!! c'estlavie */
}

  Boolean
glove_in_use()
{
  return(glove_state != Gloverelaxstate);
}

