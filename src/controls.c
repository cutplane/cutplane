
/* FILE: controls.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Device Control Routines for the Cutplane Editor              */
/*                                                                        */
/* Author: WAK                                    Date: April 22,1990     */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 William Kessler and Larry Edwards.   */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define CONTROLSMODULE

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <platform_gl.h>
#include <platform_glu.h>

#include "device.h"

#include <cutplane.h>		/* cutplane.h includes controls.h */
#include <globals.h>
#include <grafx.h>
#include <controls.h>
#include <update.h>
#include <math3d.h>
#include <cpl_cutplane.h>

#include <demo.h>

#define even(n) (~(n & 0x1))
#define odd(n) (n & 0x1)

#undef getvaluator
#undef setvaluator

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			GLOBAL VARIABLES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

valuatorectype valuatorecs[Maxvaluatorecs];

/* Should do tiedbuttons with records like valuatorecords, that  include */
/* mapping, a devcount field, and a set of history arrays, one for each dev */
/* associated with a tiedbutton. (e.g Pickbutton is tied to both x and y */
/* of mouse, and both need be stored, whereas Planemovebutton is tied to */
/* only one value (computed from the x and y), and Zoombutton is tied to */
/* only one value (computed from x only). */

int tiedbuttonmappings[Maxtiedbuttons] = {NULLDEV,
					    PICKBUTTON,
					    PLANEMOVEBUTTON,
					    OBJECTROTATEBUTTON,
					    ROOMROTATEBUTTON1,
					    ROOMROTATEBUTTON2,
					    ZOOMBUTTON,
					    CONSTRAINBUTTON1,
					    CONSTRAINBUTTON2,
					    MULTIPLESELECTBUTTON1,
					    MULTIPLESELECTBUTTON2};
int tiedbuttonvalcounts[Maxtiedbuttons] = {0,2,1,2,2,1,2,2,2,2};

/* this is part of a hack to test space bar as plane movement device.... */
int nonqueued_tiedbuttonmappings[Numnonqueud_tiedbuttons] = {ZOOMBUTTON};

int actual_nonqueued = 1; /* normally set to Numnonqueud_tiedbuttons */

/* These numbers are maps to the physical valuators through the valuatorecs, */
/* e.g. if tiedbutton[1] is tied to valuators 1 and 2 then we look at */
/* the "thedev" fields of valuatorecs 1 and 2 to get the actual physical dev.*/

int tiedbuttonvaldevs[Maxtiedbuttons][Maxtiedvals] = {{0},
						      {1,2},
						      {2},
						      {1,2},
						      {1,2},
						      {2},

						      {1,2},
						      {1,2},
						      {1,2},
						      {1,2}};

/* For the moment, only the Pickbutton uses the last scanned valuator */
/* values read when it it pressed down, the others still poll the valuators */
/* after the fact. */
int uselastscan[Maxtiedbuttons][Maxtiedvals] = {{FALSE,FALSE},
						{TRUE,TRUE},
						{FALSE,FALSE},
						{FALSE,FALSE},
						{FALSE,FALSE},
						{FALSE,FALSE},
						{FALSE,FALSE},
						{FALSE,FALSE},
						{FALSE,FALSE},
						{FALSE,FALSE}};

Boolean checkclickcoords = FALSE;
Boolean recordclickcoords = FALSE;
tiedbuttontype tiedbuttons[Maxtiedbuttons];
tiedbuttonkind checkbutton,recordbutton;

/* Button blocking constants. These prevent Pick/Release events from */
/* happening on some buttons when other buttons are already down. */

int enabled_buttons = ~(0x0);	/* all buttons enabled to start */
int enablebits[Maxtiedbuttons] = {0,Pickenablebit,Moveplanenablebit,
				    Rotateobjenablebit,Roomrotatenablebit,
				    Zoomenablebit,Constrainenablebit,
				    Multipleselectenablebit};
int disablebits[Maxtiedbuttons] = {0,Pickdisables,Moveplanedisables,
				     Rotateobjdisables,Roomrotatedisables,
				     Zoomdisables,Constraindisables,
				     Multipleselectdisables};


/* The ascii character map:                                                  */
/* ^@,^A,^B,^C,^D,^E,^F,^G,BS,HTAB,LF,VTAB,^L,CR,^N,^O,^P,^Q,^R,^S,^T,^U,^V, */
/* ^W,^X,^Y,^Z,^[,^\,^],^^,^_,SP,                                            */
/* !~#$%&'()*+,-./0123456789:;<=>?                                           */
/* @ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`                                         */
/*  abcdefghijklmnopqrstuvwxyz{|}~DEL                                        */

/* map from ascii code to IRIS device number */
static Device keyboardmap[128] = {0,0,0,0,0,0,0,0,BACKSPACEKEY,TABKEY,
/* ctrl keys */			    LINEFEEDKEY,0,0,RETKEY,0,0,0,0,0,0,0,0,0,0,
/* end ctrl keys */		    0,0,0,ESCKEY,0,0,0,0,SPACEKEY,
/* symbols */			    ONEKEY,ACCENTGRAVEKEY,THREEKEY,FOURKEY,
				    FIVEKEY,SEVENKEY,QUOTEKEY,NINEKEY,ZEROKEY,
				    EIGHTKEY,EQUALKEY,COMMAKEY,MINUSKEY,
/* end symbols */		    PERIODKEY,VIRGULEKEY,
/* nums */			    ZEROKEY,ONEKEY,TWOKEY,THREEKEY,FOURKEY,
/* end nums */			    FIVEKEY,SIXKEY,SEVENKEY,EIGHTKEY,NINEKEY,
/* more symbols */		    SEMICOLONKEY,SEMICOLONKEY,COMMAKEY,EQUALKEY,
/* end more symbols */		    PERIODKEY,VIRGULEKEY,TWOKEY,
/* cap chars */			    AKEY,BKEY,CKEY,DKEY,EKEY,FKEY,GKEY,HKEY,
				    IKEY,JKEY,KKEY,LKEY,MKEY,NKEY,OKEY,PKEY,
				    QKEY,RKEY,SKEY,TKEY,UKEY,VKEY,WKEY,XKEY,
/* end cap chars */		    YKEY,ZKEY,
/* more symbols */		    LEFTBRACKETKEY,BACKSLASHKEY,RIGHTBRACKETKEY,
/* end more symbols */		    SIXKEY,MINUSKEY,ACCENTGRAVEKEY,
/* chars */			    AKEY,BKEY,CKEY,DKEY,EKEY,FKEY,GKEY,HKEY,
				    IKEY,JKEY,KKEY,LKEY,MKEY,NKEY,OKEY,PKEY,
				    QKEY,RKEY,SKEY,TKEY,UKEY,VKEY,WKEY,XKEY,
/* end chars */			    YKEY,ZKEY,
/* more symbols */		    LEFTBRACKETKEY,BACKSLASHKEY,RIGHTBRACKETKEY,
/* and more symbols */		    ACCENTGRAVEKEY,DELKEY};

/* some keys with no ascii equiv: */
/* LEFTARROWKEY,DOWNARROWKEY,RIGHTARROWKEY,UPARROWKEY */
/* BREAKKEY,SETUPKEY,CTRLKEY,RIGHTCTRLKEY,CAPSLOCKKEY */
/* RIGHTSHIFTKEY,LEFTSHIFTKEY,NOSCRLKEY,LEFTALTKEY,RIGHTALTKEY */


char lowcase_asciimap[128] =
{0,0,0,0,0,0,0,27,
   '1',9,'q','a','s',0,'2','3','w','e','d','f','z','x',
   '4','5','r','t','g','h','c','v','6','7','y','u','j','k',
   'b','n','8','9','i','o','l',';','m',',','0','-','p',
   '[',
   0x27,	/* ' */
   13,		/* CR */
   '.',
   '/',
   '=',
   0x60,	/* ` */
   ']',
   0x5c,	/* \ */
   0,0,10,8,127,0,0,0,0,0,0,0,0,0,0,
   '',			/* LEFTARROWKEY */
   '',			/* DOWNARROWKEY */
   0,0,0,0,0,
   '',			/* RIGHTARROWKEY */
   '',			/* UPARROWKEY */
   0,' '};

char upcase_asciimap[128] =
{0,0,0,0,0,0,0,27,
   '!',9,'Q','A','S',0,'@','#','W','E','D','F','Z','X',
   '$','%','R','T','G','H','C','V','^','&','Y','U','J',
   'K','B','N','*','(','I','O','L',':','M','<',')','_',
   'P',
   '{',
   0x22,	/* " */
   13,		/* CR */
   '>','?',
   '+',
   '~',
   '}',
   0x7c,	/* \ */
   0,0,10,8,127,0,0,0,0,0,0,0,0,0,0,'','',0x7e,0,0,0,0,'','',0,' '};


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			USER INPUT FUNCTIONS				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  char
kbdevice_to_ascii(int thedev)	/* converts keybd device into ascii value */
{
  if (thedev < 0)
    return('\0');		/* if upstroke return NULL */
  if (getbutton(LEFTSHIFTKEY) || getbutton(RIGHTSHIFTKEY))
    return(upcase_asciimap[thedev]);
  return(lowcase_asciimap[thedev]);
}

  Boolean
is_keyboard_dev(int thedev)
{
  return((abs(thedev) > BUT0) && (abs(thedev) <= MAXKBDBUT));
}

  void
store_roomrot(int delta1,int delta2)
{
  CPL_word op_word,as_word,ad_word;
  ;
  setup_CPL_word_blank(&op_word,Opcode_data);
  op_word.word_data.CPLopcode = ROTATE;
  setup_CPL_word_blank(&as_word,Environment_data);
  as_word.word_data.CPLenvironment_part = Room_part;
  setup_CPL_auxword(&ad_word,CPLaux_rotate_data);
  setpos3d(ad_word.word_data.CPLauxdata.rotate_data.rotvec,(Coord) delta1,
	  (Coord) delta2,0.0);
  store_CPL_RAM_instruction(&op_word,&as_word,&ad_word);
}

tiedbuttonkind
map_qentry2tiedbutton(int qentry, int actual_qentry)
{
  switch(qentry)
  {
  case PICKBUTTON:
    return(Pickbutton);
  case PLANEMOVEBUTTON:
    return(Planemovebutton);
  case OBJECTROTATEBUTTON:
    return(Objectrotatebutton);
  case ROOMROTATEBUTTON1:
  case ROOMROTATEBUTTON2:
    return(Roomrotatebutton);
  case ZOOMBUTTON:
    return(Zoombutton);
  case MULTIPLESELECTBUTTON1:
  case MULTIPLESELECTBUTTON2:
    return(Multipleselectbutton);
  case CONSTRAINBUTTON1:
  case CONSTRAINBUTTON2:
    return(Constrainbutton);
  default:
    return(Nobutton);
  }
}

  Boolean
button_clear(tiedbuttonkind thistied)
{				/* coercion to int to run on 3.2 irislame */
  return ((enabled_buttons & enablebits[(int) thistied]) > 0);
}

  Boolean
button_pressed(tiedbuttonkind thistied)
{
  return(tiedbuttons[(int) thistied].status);
}

  Boolean
encode_statechange(tiedbuttonkind thistied,stateptr state)
{
  CPL_word op_word, as_word,ad_word;
  Boolean events_generated = FALSE;
  ;
  if ((tiedbuttons[(int) thistied].status &&
       !tiedbuttons[(int) thistied].oldstatus) && button_clear(thistied))
  {				/* tied button pressed down */
    recordclickcoords = TRUE;
    recordbutton = thistied;
    events_generated = TRUE;
    enabled_buttons &= disablebits[(int) thistied]; /* block proper buttons */
    switch (thistied)
    {
    case Pickbutton:
      setup_CPL_word_blank(&op_word,Opcode_data);
      op_word.word_data.CPLopcode = GLUPDATE;
      setup_CPL_auxword(&ad_word,CPLaux_update_data);
      ad_word.word_data.CPLauxdata.update_data.update_flags = Getselectableflag;
      ad_word.word_data.CPLauxdata.update_data.update_now = TRUE;
/*      store_CPL_RAM_instruction(&op_word,&ad_word);*/
      
      /* If user had multiple select button down, make sure to code */
      /* MPICK instruction instead of PICK instruction.*/
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
      break;
    case Planemovebutton:
      setup_CPL_word_blank(&op_word,Opcode_data);
      op_word.word_data.CPLopcode = PICK;
      setup_CPL_word_blank(&as_word,Environment_data);
      as_word.word_data.CPLenvironment_part = Cutplane_part;
      store_CPL_RAM_instruction(&op_word,&as_word);
      /* tell cursor to cancel any jumps that might be in progress */
      encode_CPL_RAM_instruction(" MAKEMSG Msg:Canceljump");
      encode_CPL_RAM_instruction(" QMSG Obj:Crosshair_Cursor,Const:Noprioritymsg");
      encode_CPL_RAM_instruction(" PUSH Const:TRUE");
      encode_CPL_RAM_instruction(" CRTN Crtn:set_hiddenedge");
      break;
    case Roomrotatebutton:
      encode_CPL_RAM_instruction(" PUSH Const:TRUE");
      encode_CPL_RAM_instruction(" CRTN Crtn:set_hiddenedge");
      break;
    case Objectrotatebutton:
    case Constrainbutton:
      encode_CPL_RAM_instruction(" WORLD Const:WEnvironment_world");
      encode_CPL_RAM_instruction(" MAKEMSG Msg:Constraining");
      encode_CPL_RAM_instruction(" MAKEMSG Const:TRUE");
      encode_CPL_RAM_instruction(" QMSG Obj:Crosshair_Cursor,Const:Prioritymsg");
      encode_CPL_RAM_instruction(" WORLD Const:WPrimary_world");
      break;
    default:
      break;
    }
  }
  else if ((!tiedbuttons[(int) thistied].status &&
	    tiedbuttons[(int) thistied].oldstatus))
  {				/* tied button released */
    events_generated = TRUE;
    checkclickcoords = TRUE;
    checkbutton = thistied;
    enabled_buttons |= ~disablebits[(int) thistied];
    switch (thistied)
    {
    case Pickbutton:
      /* If user had multiple select button down, make sure to code */
      /* MRELEAS instruction instead of RELEAS instruction.*/
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
#ifdef IRIS
      clear_drag_origin(state);
#else
      clear_drag_origin();
#endif
      break;
    case Planemovebutton:
      setup_CPL_word_blank(&op_word,Opcode_data);
      op_word.word_data.CPLopcode = RELEAS;
      setup_CPL_word_blank(&as_word,Environment_data);
      as_word.word_data.CPLenvironment_part = Cutplane_part;
      store_CPL_RAM_instruction(&op_word,&as_word);
      encode_CPL_RAM_instruction(" PUSH Const:FALSE");
      encode_CPL_RAM_instruction(" CRTN Crtn:set_hiddenedge");
      encode_CPL_RAM_instruction(" GLUPDATE Const:URedrawflag/when:Now");
      break;
    case Roomrotatebutton:
      encode_CPL_RAM_instruction(" PUSH Const:FALSE");
      encode_CPL_RAM_instruction(" CRTN Crtn:set_hiddenedge");
      encode_CPL_RAM_instruction(" GLUPDATE Const:URedrawflag/when:Now");
      break;
    case Objectrotatebutton:
    case Constrainbutton:
      encode_CPL_RAM_instruction(" WORLD Const:WEnvironment_world");
      encode_CPL_RAM_instruction(" MAKEMSG Msg:Constraining");
      encode_CPL_RAM_instruction(" MAKEMSG Const:FALSE");
      encode_CPL_RAM_instruction(" QMSG Obj:Crosshair_Cursor,Const:Prioritymsg");
      encode_CPL_RAM_instruction(" WORLD Const:WPrimary_world");
      break;
    default:
      break;
    }
  }

  return(events_generated);
}
  

  void
init_tied_buttons(stateptr state)
{
  tiedbuttonkind thistied;
  int i,j,k;
  ;
  for (i = (int) Nobutton + 1; i < (int) Maxtiedbutton; i++)
  {
    thistied = (tiedbuttonkind) i; /* brain dead iris 3.2 again */
    tiedbuttons[i].mapping = tiedbuttonmappings[i];
    tiedbuttons[i].valcount = tiedbuttonvalcounts[i];
    for (j = 0; j < tiedbuttons[i].valcount; ++j)
      tiedbuttons[i].valdevs[j] = tiedbuttonvaldevs[i][j];
						
    for (j = 0; j < tiedbuttons[i].valcount; ++j)
      for (k = 0; k < Valhistorydepth; ++k)
	tiedbuttons[i].valhistory[j][k] = 0;
    tiedbuttons[i].oldstatus = FALSE;
    tiedbuttons[i].status = getbutton(tiedbuttonmappings[i]);
    tiedbuttons[i].maxdepth = 1;
    encode_statechange(thistied,state);
  }
}
    

  void
init_valuatorec(int thevaluator,Device thedev,int initialvalue,
		int minrange,int maxrange,
		int scanrangemin,int scanrangemax)
{
  /* i think scan range is a subset of maxrange */
  /* to leave a margin for wrapping the valuator */
  valuatorecs[thevaluator].devicenum = thedev;
  valuatorecs[thevaluator].lastscan = initialvalue;
  valuatorecs[thevaluator].minrange = minrange;
  valuatorecs[thevaluator].maxrange = maxrange;
  valuatorecs[thevaluator].scanmin = scanrangemin;
  valuatorecs[thevaluator].scanmax = scanrangemax;
  valuatorecs[thevaluator].scanrange = scanrangemax - scanrangemin;
  valuatorecs[thevaluator].mustuselastscan = FALSE;
  setvaluator(thedev,initialvalue,minrange,maxrange);
}

/* To use different valuators rather than the mouse, you just change */
/* the two constants MOUSEX and MOUSEY below to what you want. If you */
/* need to use more valuators, add to these calls.  The minrange and */
/* maxrange values are what is passed to setvaluator.  The scanrange */
/* values however are the values that this valuator's inputs will be wrapped */
/* at by lowlevel_controldev_update().  E.g., MOUSEX can range from */
/* 0-Screensize of the computer screen; so it's mapped one-to-one onto */
/* pixel size.  However, when it goes less than mainwindowleft it gets */
/* wrapped to mainwindowright, and if it goes greater than mainwindowright */
/* it gets wrapped to mainwindowleft, with GL library calls to set_valuator. */

  void
init_valuatorecs()
{
  init_valuatorec(1,MOUSEX,mainwindowleft+screensizex/2,
		  0,screensizex-1,mainwindowleft,mainwindowright);
  init_valuatorec(2,MOUSEY,mainwindowbottom+screensizey/2,
		  0,screensizey-1,mainwindowbottom,mainwindowtop);
#if 0
  /* this was just for testing */
  init_valuatorec(1,MOUSEX,mainwindowleft+screensizex/2,
		  0,XMAXSCREEN-1,0,XMAXSCREEN);
  init_valuatorec(2,MOUSEY,mainwindowbottom+screensizey/2,
		  0,YMAXSCREEN-1,0,YMAXSCREEN);
#endif
  /* Add more here if more valuators desired. */
}

  void
clear_record(char *therec,int recsize)
{
  int i;
  char *ptr;
  ;
  for (i = 0; i < recsize; ++i,++therec)
    *therec = 0;
}

  void
init_controls(stateptr state)
{
  init_valuatorecs();
  init_tied_buttons(state);
}

  Boolean
update_queued_buttons(int *qentry)
{
  CPL_word op_word;
  Boolean code_new_words = TRUE;
  char transinstr[MaxCPLinelength],typed_char;
  char inoutfilename[100];	/* hackccccck */
  objptr objA,objB;		/* HACKGAGAGAGAGAGGGGGGGGGGG */
  static int newonflag = 1;
  ;
  if ((is_keyboard_dev(*qentry)) && (objects_in_charstream()))
  {
    typed_char = kbdevice_to_ascii(*qentry);
    /* if an object(s) have flagged the system */
    /* that they want typed characters, */
    /* send them the inputted character instead */
    /* of translating it into a command (unless it's NULL, i.e. upstroke).*/
    if (typed_char != '\0')
      stream_char_to_objects(typed_char);
    code_new_words = FALSE;
  }
  else
  {
    switch (*qentry)
    {
    case LOADBUTTON:
      /* this should send a message to the disk object... */
      encode_CPL_RAM_instruction(" MAKEMSG Msg:Loadfromfile");
      encode_CPL_RAM_instruction(" QMSG Obj:LoadObject,Const:Prioritymsg");
#if 0
      inquire_filename(inoutfilename,
		       "Please type the name of the world load file:","");
      if (strlen(inoutfilename) > 1)
      {
	read_data_file(inoutfilename,(featureptr) working_world->root);
      }
      log_global_update(Redrawflag);
#endif    
      break;
    case SAVEBUTTON:
      /* this should send a message to the disk object... */
      encode_CPL_RAM_instruction(" MAKEMSG Msg:Savetofile");
      encode_CPL_RAM_instruction(" QMSG Obj:SaveObject,Const:Prioritymsg");
#if 0
      inquire_filename(inoutfilename,
		       "Please type the name of the file to save world:","");
      if (strlen(inoutfilename) > 1)
      {
	write_data_file(inoutfilename,(featureptr) working_world);
      }
      log_global_update(Redrawflag);
#endif
      break;
    case ROOMUPBUTTON:
      store_roomrot(-Slowrot,0);
      break;
    case ROOMDOWNBUTTON:
      store_roomrot(Slowrot,0);
      break;
    case ROOMLEFTBUTTON:
      store_roomrot(0,-Slowrot);
      break;
    case ROOMRIGHTBUTTON:
      store_roomrot(0,Slowrot);
      break;
    case DELETEBUTTON:
      encode_CPL_RAM_instruction(" MAKEMSG Msg:Delete");
      encode_CPL_RAM_instruction(" QMSG Prop:Picked*,Const:Noprioritymsg");
      break;
    case CUTBUTTON:
      code_new_words = FALSE;
      break;
/* temporarily replaced by the PANTOOLBUTTON...
    case PASTEBUTTON:
      code_new_words = FALSE;
      break;
*/      
    case DUPLICATEBUTTON:
      encode_CPL_RAM_instruction(" DUP Prop:Picked*,A:R3");
      sprintf (transinstr," TRANS A:R3*,Const:Normaltranslate,\
Const:V[%1.1f,%1.1f,%1.1f]", 100.0,100.0,0.0);
      encode_CPL_RAM_instruction(transinstr);
      encode_CPL_RAM_instruction(" MAKEMSG Msg:Unpick");
      encode_CPL_RAM_instruction(" QMSG Prop:Picked*,Const:Noprioritymsg");
      encode_CPL_RAM_instruction(" MAKEMSG Msg:Pick");
      encode_CPL_RAM_instruction(" QMSG A:R3*,Const:Noprioritymsg");
      break;
    case MIRRORBUTTON:
      code_new_words = FALSE;
      break;
    case SLICEBUTTON:
      encode_CPL_RAM_instruction(" PUSH Prop:Picked*");
      encode_CPL_RAM_instruction(" CRTN Crtn:slice_object");
      encode_CPL_RAM_instruction(" POP  A:R2");
      encode_CPL_RAM_instruction(" TRANS A:R2*,Const:Normaltranslate,\
Const:V[0.0,0.0,-10.0]");
      encode_CPL_RAM_instruction(" POP  A:R3");
      encode_CPL_RAM_instruction(" TRANS A:R3*,Const:Normaltranslate,\
Const:V[0.0,0.0,10.0]");
      encode_CPL_RAM_instruction(" UPDATE	A:R2*,\
Const:UClearcutsectionflag/when:Now");
      encode_CPL_RAM_instruction(" UPDATE	A:R3*,\
Const:UClearcutsectionflag/when:Now");
      code_new_words = FALSE;
      break;
    case BOOLEANBUTTON:
      /* HACK to test booleans only */
      objA = (objptr) get_property_feature(primary_world,picked_prop,1);
      objB = (objptr) get_property_feature(primary_world,picked_prop,2);
      if ((objA != Nil) && (objB != Nil))
	do_boolean(objA,objB,BOOL_Subtraction);
      code_new_words = FALSE;
      break;
    case GROUPBUTTON:
      encode_CPL_RAM_instruction(" MAKEMSG Msg:Activate");
      encode_CPL_RAM_instruction(" QMSG Obj:Grouptool,Const:Prioritymsg"); 
      code_new_words = FALSE;
      break;
    case UNDOBUTTON:
      code_new_words = FALSE;
      break;
      /*
	case REDOBUTTON:
	code_new_words = FALSE;
	break;
	*/
    case RTOOLBUTTON:
      encode_CPL_RAM_instruction(" MAKEMSG Msg:Snaprotatetool");
      encode_CPL_RAM_instruction(" QMSG Obj:Rotatetool_Innercircle,Const:\
Prioritymsg");
      code_new_words = FALSE;
      break;
    case ZTOOLBUTTON:
      encode_CPL_RAM_instruction(" MAKEMSG Msg:Activatetool");
      encode_CPL_RAM_instruction(" QMSG Obj:Magnifier,Const:Prioritymsg");
      code_new_words = FALSE;
      break;
    case PANTOOLBUTTON:
      encode_CPL_RAM_instruction(" MAKEMSG Msg:Activatetool");
      encode_CPL_RAM_instruction(" QMSG Obj:Pantool,Const:Prioritymsg");
      code_new_words = FALSE;
      break;
    case ZBUFFERBUTTON:
      encode_CPL_RAM_instruction(" CPSHADE Const:WPrimary_world");
      encode_CPL_RAM_instruction(" GLUPDATE Const:URedrawflag/when:Now");
      break;
    case DEMOBUTTON:
      code_new_words = FALSE;
      if (newonflag > Maxdrawflag) /* from grafx.h demo shit 3/20/91 */
      {
	drawflags = 0;
	newonflag = 1;
      }
      else
      {
	drawflags |= newonflag;
	newonflag = newonflag << 1;
      }
      log_global_update(Redrawflag);
      break;
    case MEASUREBUTTON:
      code_new_words = FALSE;
      break;
    case SPOOLBUTTON:		/* serious HACK to allow me to turn debuggin */
      toggle_cpldebug();	/* on and off interactively */
      code_new_words = FALSE;
      break;
    case UNSPOOLBUTTON:
      code_new_words = FALSE;
      break;
    case SCALEFCNBUTTON:
      code_new_words = FALSE;
      encode_CPL_RAM_instruction(" CRTN Crtn:turnon_arrowcursor");      
      break;
    case PRINTBUTTON:
      code_new_words = FALSE;
//       dump_scene();		/* abusive hack */
      break;
      break;
    case QUITBUTTON:
      setup_CPL_word_blank(&op_word,Opcode_data);
      op_word.word_data.CPLopcode = QUIT;
      store_CPL_RAM_instruction(&op_word);
      break;
    case PLANEJUMPBUTTON:
      encode_CPL_RAM_instruction(" MAKEMSG Msg:Jumplane");
      encode_CPL_RAM_instruction(" QMSG Obj:Crosshair_Cursor,Const:Prioritymsg");
      code_new_words = FALSE;
      break;
    default:
      code_new_words = FALSE;
      break;
    }
  }
  
  if (code_new_words)
    *qentry = NULLDEV;

  return(code_new_words);
} 

  Boolean
update_tied_buttons(int qentry,stateptr state)
{
  int buttonvaluator;
  tiedbuttonkind thistied;
  Boolean events_generated;
  ;
  thistied = map_qentry2tiedbutton(abs(qentry),qentry);
  if (qentry > 0)
  {
    tiedbuttons[(int) thistied].oldstatus = FALSE;
    tiedbuttons[(int) thistied].status = TRUE;
  } else if (qentry < 0)
  {
    tiedbuttons[(int) thistied].oldstatus = TRUE;
    tiedbuttons[(int) thistied].status = FALSE;
  } 
  events_generated = encode_statechange(thistied,state);
  /* since this button was tied to a valuator make sure to use */
  /* the last scanned value and not the one gotten later by calls */
  /* to getvaluator since they'll be late. */
  /* Note:hardwired to assume only two valuators queued by the hardware with */
  /* this tied button, since that's all the IRIS can do. */
  if (uselastscan[(int) thistied][0])
  {
    buttonvaluator = tiedbuttons[(int) thistied].valdevs[0];
    valuatorecs[buttonvaluator].mustuselastscan = TRUE;
  }
  if (uselastscan[(int) thistied][1])
  {
    buttonvaluator = tiedbuttons[(int) thistied].valdevs[1];
    valuatorecs[buttonvaluator].mustuselastscan = TRUE;
  }

  return(events_generated);
}

  void
update_nonqueued_tied_buttons(stateptr state)
{
  int i;
  int fakeqentry;
  tiedbuttonkind thistied;
  ;
  for (i = 0; i < actual_nonqueued; ++i)
  {
    fakeqentry = nonqueued_tiedbuttonmappings[i];
    thistied = map_qentry2tiedbutton(abs(fakeqentry),fakeqentry);
/*    printf ("%d\n", getbutton(fakeqentry));*/
    if (getbutton(fakeqentry) &&
	(tiedbuttons[(int) thistied].status != TRUE))
      update_tied_buttons(fakeqentry,state);
    else if (!getbutton(fakeqentry) &&
	     (tiedbuttons[(int) thistied].status == TRUE))
      update_tied_buttons(-fakeqentry,state);
  }
}
	

  Boolean
lowlevel_controldev_update(int *delta,int thevaluator)
{
  int newvalue;
  Device thedev;
  ;
  thedev = valuatorecs[thevaluator].devicenum;
  /* if the valuator had its position queued by tie(), use that value. */
  if (valuatorecs[thevaluator].mustuselastscan)
  {
    newvalue = valuatorecs[thevaluator].lastscan;
    valuatorecs[thevaluator].mustuselastscan = FALSE;
  }
  else				/* otherwise, read the valuator directly. */
    newvalue = getvaluator(thedev);

  *delta = newvalue - valuatorecs[thevaluator].lastscan;
  valuatorecs[thevaluator].lastscan = newvalue;
  if (*delta != 0)
  {
    if (newvalue < valuatorecs[thevaluator].scanmin)
    {
      newvalue += valuatorecs[thevaluator].scanrange;
      setvaluator(thedev,(short) newvalue,
		  (short) valuatorecs[thevaluator].minrange,
		  (short) valuatorecs[thevaluator].maxrange);
    }
    else if (newvalue > valuatorecs[thevaluator].scanmax)
    {
      newvalue -= valuatorecs[thevaluator].scanrange;
      setvaluator(thedev,(short) newvalue,
		  (short) valuatorecs[thevaluator].minrange,
		  (short) valuatorecs[thevaluator].maxrange);
    }
    valuatorecs[thevaluator].lastscan = newvalue;
    return(TRUE);
  }
  else
    return(FALSE);		/* That valuator did not change values at all */
}

void
calc_z_direction(stateptr state, vertype z_dir)
{
  float minangle,fraction,z_dir_angle;
  ;
  /* need room x-axis rot angle & room y-axis rot angle */
  /* y angle rot tells you the spread for mouse moves */
  if (state->roomroty == 0)
  {
    z_dir[vx] = 0.0;
    z_dir[vy] = -1.0;
    z_dir[vz] = 0.0;
    z_dir[vw] = 0.0;
    if (state->roomrotx < 0)
      z_dir[vy] = 1.0;
  }
  else
  {
    /* minangle is the angle if rommrotx == 0 */
    /* +-90 is the angle if roomrotx == 90 */
    /* we interpolate between those two extremes for the actual roomrotx */
    minangle = -sign(state->roomrotx)*90.0 + (float) state->roomroty/10.0;
    fraction = -((float) state->roomrotx)/900.0;
    z_dir_angle = minangle + fraction*(90.0-fabs(minangle));

    z_dir[vx] = cos(Deg2rad*z_dir_angle);
    z_dir[vy] = sin(Deg2rad*z_dir_angle);
    z_dir[vz] = 0.0;
    z_dir[vw] = 0.0;
  }
}

  

  int
planemove_interpolate(stateptr state, int mouse_deltax, int mouse_deltay)
{
  float z_comp;
  vertype z_dir; /* vector in mouse space that should map to +plane movements */
  ;
  calc_z_direction(state,z_dir);	/* z_dir is unit vector */
  z_comp = (mouse_deltax*z_dir[vx] + mouse_deltay*z_dir[vy]);

  return(iround(z_comp));
}

void
store_cursor_move(vertype cursor_move, Boolean dragging)
{
  CPL_word op_word,as_word,at_word,ad_word;
  CPL_word drag_as_word,drag_at_word,drag_ad_word;
  vertype extra_cursor_move,extra_offset;
  ;
  setup_CPL_word_blank(&op_word,Opcode_data);
  op_word.word_data.CPLopcode = TRANS;
  setup_CPL_word_blank(&as_word,Environment_data);
  as_word.word_data.CPLenvironment_part = Crosshair_part;
  setup_CPL_auxword(&at_word,CPLaux_offset_data);
  at_word.word_data.CPLauxdata.CPLbooleandata = FALSE; /* == Normaltranslate */
  setup_CPL_auxword(&ad_word,CPLaux_offset_data);

  /* You could also be dragging during crosshair move. */
  if (dragging)
  {
    setup_CPL_auxword(&drag_at_word,CPLaux_offset_data);
    /* Assume Quicktranslate for drag... if it turns out not to be dragging */
    /* an entire object, execute_TRANS will just ignore it. */
    drag_at_word.word_data.CPLauxdata.CPLbooleandata = TRUE; /* Quicktranslate */
    setup_CPL_auxword(&drag_ad_word,CPLaux_offset_data);
    if (first_valid_drag(state,extra_offset))
      /* Need to add the difference between the drag_origin and the current */
      /* crosshair move, if this is the first drag. */
      addpos3d(cursor_move,extra_offset,
							 extra_cursor_move);
    else
      copypos3d(cursor_move,extra_cursor_move);
    compute_crosshair_move(extra_cursor_move,
													 drag_ad_word.word_data.CPLauxdata.CPLoffsetdata,
													 state);
    setup_CPL_word_blank(&drag_as_word,Proplist_data);
    drag_as_word.word_data.CPLproplist->first.sort->sortdata1->i = picked_prop;
    drag_as_word.contents_of = TRUE;
    store_CPL_RAM_instruction(&op_word,&drag_as_word,&drag_at_word,
															&drag_ad_word);
  }

  /* This HACK makes cutplane move in 1 unit increments ONLY while constrain */
  /* button is down... i'd rather do it in CPL but there's no SIGN operator */
  /* in CPL yet, and it's impossible to do with pure math... so here it is.*/
  if (button_pressed(Constrainbutton))
    cursor_move[vz] = 1.0 * fsign(cursor_move[vz]);
  
	printf("storing cursor move: (%f,%f)\n",
				 cursor_move[0],cursor_move[1]);

  compute_crosshair_move(cursor_move,
												 ad_word.word_data.CPLauxdata.CPLoffsetdata,
												 state);
	printf("CPLoffsetdata = (%f,%f)\n",
				 ad_word.word_data.CPLauxdata.CPLoffsetdata[0],
				 ad_word.word_data.CPLauxdata.CPLoffsetdata[1]);
  store_CPL_RAM_instruction(&op_word,&as_word,&at_word,&ad_word);
  /* hack for debuggin */
  copypos3d(cursor_move,cursor_delta);
}
  
  void
store_mouse_cursor_move(int delta1, int delta2, stateptr state)
{
  vertype cursor_move,projected_cursor_pos;
  Boolean dragging;
  ;
  /* Plane move gets top priority */
  if ((tiedbuttons[(int) Planemovebutton].status) &&
      button_clear(Planemovebutton))
    /* Encode word for crosshair move in Z only. */
    setpos3d(cursor_move,0.0,0.0,
	    (Coord) planemove_interpolate(state, delta1,delta2));
  else
  {
    /* Encode x-y move only. */
    setpos3d(cursor_move,(Coord) delta1, (Coord) delta2,0.0);
    /* ...constraining now handled in CPL... see message passed about it above.*/
  }
  addpos3d(state->cursor,cursor_move,projected_cursor_pos);
  dragging = ((tiedbuttons[(int) Pickbutton].status)
	      && button_clear(Pickbutton) && drag_valid(projected_cursor_pos));
  store_cursor_move(cursor_move,dragging);
}

  void
reset_multiple_clicks()
{
  checkclickcoords = recordclickcoords = FALSE;
}

  int
check_for_multiple_click(tiedbuttonkind checkbutton)
{
  Boolean valuatormoved = FALSE;
  int valcnt, *maxdepthptr;
  ;
  for (valcnt = 0; valcnt < tiedbuttons[(int) checkbutton].valcount; valcnt++)
  {
    if (tiedbuttons[(int) checkbutton].
	valhistory[valcnt][Valhistorydepth - 1] !=
	tiedbuttons[(int) checkbutton].
	valhistory[valcnt][Valhistorydepth - 2])
    {
      valuatormoved = TRUE;
      break;
    }
  }
  /* Currently only checks for double and triple picks but you can add to */
  /* this if'n you wanna by copying the top if construct and increasing the */
  /* constant multiplier. */
  maxdepthptr = &(tiedbuttons[(int) checkbutton].maxdepth);
  if ((*maxdepthptr == 1) || (valuatormoved))
  {				/* This was the first pick after triple pick */
				/* or after movement of valuators. */
    *maxdepthptr = 2;		/* Get ready for double clicks */
    return(Oneclick);
  }
  else
  {
    if (*maxdepthptr == 2)
    {
      ++(*maxdepthptr);		/* Get ready for triple clicks */
      return(Doubleclick);
    }
    else if (*maxdepthptr == 3)
    {
      *maxdepthptr = 1;		/* reset so no repeated triple clicks. */
      return(Tripleclick);
    }
  }
  // CHECK ME!! Should we define "Noclick" and return here?! -- LJE
}

  void
record_devhistory(tiedbuttonkind recordbutton)
{
  int thisval,valcnt,i;
  Device thedev;
  ;
  for (valcnt = 0; valcnt < tiedbuttons[(int) recordbutton].valcount; ++valcnt)
  {
    for (i = 0; i < Valhistorydepth-1; ++i) /* shift values down in history */
      tiedbuttons[(int) recordbutton].valhistory[valcnt][i] =
	tiedbuttons[(int) recordbutton].valhistory[valcnt][i+1];
    thedev = valuatorecs[tiedbuttons[(int) recordbutton].valdevs[valcnt]].
      devicenum;
    tiedbuttons[(int) recordbutton].valhistory[valcnt][i] =
      getvaluator(thedev);
#ifdef debug
    printf ("pos recorded:%d\n",
	    tiedbuttons[(int) recordbutton].valhistory[valcnt][i]);
#endif
  }
}


/* The following routine implements the control heirarchies to */
/* generate the proper events, by order of hardwired priority that */
/* we have decided is the BEST. */

  Boolean
update_polled_valuators(stateptr state)
{
  CPL_word op_word;
  int delta1,delta2;
  Boolean update;
  ;
  /* These calls get two valuators' values, and they return false if those */
  /* polled valuators showed no change */
  /* If more polled valuators need to be added, add them here and use */
  /* them as you wish below. The first two for v1.0 are the mouse x and y.*/
  update = lowlevel_controldev_update(&delta1,1);
  update |= lowlevel_controldev_update(&delta2,2);
  if (!update)
  {				/* devices' values did not change */
    if (checkclickcoords)
    {				/* if the Pickbutton released, */
				/* check for multiple clicking */
      /* currently out of service... check later. */
      check_for_multiple_click(checkbutton);
    }
    else if (recordclickcoords)
      record_devhistory(recordbutton);
    return(FALSE);		/* no events coded, since no change in vals.*/
  }
  /* NOTE: we enter this code only if the valuator(s) have moved! */
  if (recordclickcoords)
    record_devhistory(recordbutton);

  if (tiedbuttons[(int) Roomrotatebutton].status)
  {				/* Room rotate has the highest priority */
    store_roomrot(-delta2,delta1);
  }
  else if (tiedbuttons[(int) Zoombutton].status)
  {				/* Zoom has the next highest priority */
				/* delta1 is amount to zzzzooom */
				/* If > 0 zoom in, if < 0 zoom out */
  }
  else				/* Move the plane or crosshair. */
    store_mouse_cursor_move(delta1,delta2,state);

  /* Right now, nothing polled is considered an "event" (2/2/91) */  
  return(FALSE); 
}

  Boolean
pick_button_down()
{
  return(tiedbuttons[(int) Pickbutton].status);
}

  Boolean
cutplane_button_down()
{
  return(tiedbuttons[(int) Planemovebutton].status);
}

  Boolean
roomrot_button_down()
{
  return(tiedbuttons[(int) Roomrotatebutton].status);
}
