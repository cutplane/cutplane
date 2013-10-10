 
/* FILE: controls.h */

#ifndef controls_includes
#define controls_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*   User Interface Control Routines Header File for the Cutplane Editor  */
/*                                                                        */
/* Authors: LJE and WAK                           Date: March 29,1990     */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 William Kessler and Larry Edwards.   */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <platform_gl.h>

#include "geometry.h"
#include "list.h"

#define Maxvaluatorecs 5
#define Maxtiedbuttons 50
#define Maxtiedvals 3		/* how many valuators can be tied to a button */
#define Valhistorydepth 2	/* how many click positions are stored per */
				/* tied button on each valuator it is tied to */
#define Numnonqueud_tiedbuttons 2 /* hack to allow nonqueued buttons to act */
				/* like queued buttons... see */
				 /* update_nonqueued_tied_buttons()*/
enum
{
  Oneclick = 0,
  Doubleclick,
  Tripleclick
};

#if 0
/* Defines for the PC */

#define NULLDEV 0
#define LEFTMOUSE WM_LBUTTONDOWN
#define MIDDLEMOUSE WM_LBUTTONDOWN
#define RIGHTMOUSE WM_RBUTTONDOWN
#define LEFTALTKEY VK_ALT
#define RIGHTALTKEY VK_ALT
#define LEFTCTRLKEY VK_CONTROL
#define RIGHTCTRLKEY VK_CONTROL
#define LEFTSHIFTKEY VK_SHIFT
#define RIGHTSHIFTKEY VK_SHIFT
#define TABKEY VK_TAB
#define BACKSLASHKEY VK_BACK_SLASH
#define SPACEKEY VK_SPACE
#define AKEY VK_A
#define BKEY VK_B
#define CKEY VK_C
#define DKEY VK_D
#define EKEY VK_E
#define FKEY VK_F
#define GKEY VK_G
#define HKEY VK_H
#define IKEY VK_I
#define JKEY VK_J
#define KKEY VK_K
#define LKEY VK_L
#define MKEY VK_M
#define NKEY VK_N
#define OKEY VK_O
#define PKEY VK_P
#define QKEY VK_Q
#define RKEY VK_R
#define SKEY VK_S
#define TKEY VK_T
#define UKEY VK_U
#define VKEY VK_V
#define WKEY VK_W
#define XKEY VK_X
#define YKEY VK_Y
#define ZKEY VK_Z
#define F1KEY VK_F1
#define F2KEY VK_F2
#define F3KEY VK_F3
#define F4KEY VK_F4
#define F5KEY VK_F5
#define F6KEY VK_F6
#define F7KEY VK_F7
#define F8KEY VK_F8
#define F9KEY VK_F9

#endif

/* Maybe these should all be in config.h? */
/* These are really set up for the iris. */


/* #define PICKBUTTON1 LEFTMOUSE*/
/* #define PICKBUTTON2 MIDDLEMOUSE*/
#define PICKBUTTON LEFTMOUSE
#define PLANEMOVEBUTTON MIDDLEMOUSE /* SPACEKEY*/
#define OBJECTROTATEBUTTON LEFTALTKEY
#define ROOMROTATEBUTTON1 RIGHTMOUSE
#define ROOMROTATEBUTTON2 RIGHTALTKEY
#define ZOOMBUTTON TABKEY
#define ZTOOLBUTTON ZKEY
#define PANTOOLBUTTON PKEY

#define CONSTRAINBUTTON1 LEFTCTRLKEY
#define CONSTRAINBUTTON2 RIGHTCTRLKEY
#define MULTIPLESELECTBUTTON1 LEFTSHIFTKEY
#define MULTIPLESELECTBUTTON2 RIGHTSHIFTKEY

#define LOADBUTTON LKEY
#define SAVEBUTTON SKEY
#define DUPLICATEBUTTON DKEY
#define UNDOBUTTON UKEY
#define RTOOLBUTTON RKEY		/* snap the rotatetool... */
#define MIRRORBUTTON MKEY

#define QUITBUTTON ESCKEY

/* Put back in when you have resolved conflicts between load (RKEY) and redo */
/*
#define REDOBUTTON RKEY
*/
#define DELETEBUTTON BACKSPACEKEY
#define CUTBUTTON CKEY
#define PASTEBUTTON PKEY
#define GROUPBUTTON GKEY
#define UNGROUPBUTTON YKEY		/* for lack of something better */
#define SLICEBUTTON BACKSLASHKEY
#define BOOLEANBUTTON BKEY

/* These may go away */
#define ROOMLEFTBUTTON ROOMLEFTKEY
#define ROOMRIGHTBUTTON ROOMRIGHTKEY
#define ROOMUPBUTTON ROOMUPKEY
#define ROOMDOWNBUTTON ROOMDOWNKEY
#define ZBUFFERBUTTON WKEY

#define DEMOBUTTON F1KEY
#define SCALEFCNBUTTON F2KEY 
#define PRINTBUTTON F3KEY
#define SPOOLBUTTON F4KEY
#define UNSPOOLBUTTON F5KEY
#define MEASUREBUTTON F6KEY

#define PLANEJUMPBUTTON SPACEKEY /* send the plane flying! */
				 /* pigs...in...SPAAAAACE */

#define Tiedbuttonsize sizeof(tiedbuttontype)
#define Valuatorecsize sizeof(valuatorectype)

		/* what other functions the Pickcommand disables */
#define Rotateobjenablebit 1	/* %001: Rotateobj enable bit is #0 */
#define Moveplanenablebit 2	/* %010: Moveplane enable bit is #1 */
#define Pickenablebit 4		/* %100: Pick enable bit is #2 */
#define Roomrotatenablebit 8	/* %1000: Room rotate enable bit is #3 */
#define Zoomenablebit 16	/* %10000: Zoom enable bit is #4 */
#define Constrainenablebit 32	/* %100000: Constrain enable bit is #5 */
#define Multipleselectenablebit 64 /* %1000000: Multipleselect bit is #6 */
#define Allenablebits (~0)

                                /* Pick disables rotate */
#define Pickdisables (Allenablebits & ~Rotateobjenablebit)
                                /* Moveplane disables rotate */
#define Moveplanedisables (Allenablebits /* & ~Rotateobjenablebit*/)
                                /*  Rotate object disables pick,moveplane */
#define Rotateobjdisables (Allenablebits /*& ~Moveplanenablebit*/ & ~Pickenablebit)
                                /* Room rotate disables everything else */
#define Roomrotatedisables Roomrotatenablebit
                                /* Zoom disables everything else */
#define Zoomdisables Zoomenablebit
                                /* Constrain disables nothing */
#define Constraindisables Allenablebits
                                /* Multipleselect disables nothing */
#define Multipleselectdisables Allenablebits

/* Get rid of this stuff when controls.h integrated in main system */
#ifdef Roomrotatebutton
#undef Roomrotatebutton
#endif
#ifdef Zoombutton
#undef Zoombutton
#endif

typedef enum
{
  Nobutton = 0,
  Pickbutton,
  Planemovebutton,
  Objectrotatebutton,
  Roomrotatebutton,
  Zoombutton,
  Constrainbutton,
  Multipleselectbutton,
  Maxtiedbutton
} tiedbuttonkind;

struct tiedbuttonstruct
{
  int mapping;			/* mapping to actual button device in machine */
  int valcount;			/* How many valuators are tied to this button */
  int valdevs[Maxtiedvals];	/* A list of valuators tied to this button. */
  int valhistory[Maxtiedvals][Valhistorydepth];	/* History of valuators */
  Boolean oldstatus,status;	/*whether it is down,and whether it was before*/
  int maxdepth;			/* How many multiple clicks this button had */
};

typedef struct tiedbuttonstruct tiedbuttontype;
typedef tiedbuttontype *tiedbuttonptr;

struct valuatorecstruct
{
  int devicenum;
  int minrange,maxrange,scanmin,scanmax,scanrange;
  int lastscan;
  Boolean mustuselastscan; /* whether the value came from hardware queue */
			   /* or from getvaluator calls */
			   /* If it came from hardware queue, what was it*/
};

typedef struct valuatorecstruct valuatorectype;

/* Size of PC's faked event queue */
#define PC_QUEUE_SIZE 100000

#endif
