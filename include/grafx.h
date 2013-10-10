
/* FILE: grafx.h */

#ifndef grafx_includes
#define grafx_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* Der Newen Improven Droewen/Displayenspiel Rutinnen fur ausch Cutplagen */
/*                          Editeriat,yah yah.                            */
/*                      (Der Definiciones, si si)                         */
/* Authors: LJE and WAK                           Date: August 24,1988    */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <platform_gl.h>

#include "list.h"

/* Quick redrawing defs */
#define Selectivedraw FALSE
#define Draweverything TRUE
#define Starterasebboxnum 250	/* Initially start with room for 500 */
				/* erase bboxes */


/* Redraw codes for use with quick redrawing techniques. */
enum
{
  Redrawbody = 1,		/* redraw object body only */
  Redrawfaces = 2,		/* redraw some faces as they are tagged */
  Redrawshadow = 4,		/* redraw object shadow only */
  Redrawsectionline = 8,	/* not yet in use */
  Redrawselectables = 16,	/* not yet in use */
  Redrawpicked = 32,		/* not yet in use */
  Redrawselectbox = 64,    	/* not yet in use */
  Alwaysdraw = 128,		/* Always draw entire object. */
  Neverdraw = 256		/* Never draw the object, ever. */
};

/* Extra space around cursor to make erase strip big enough */
#define Cursormargin 10.0

/* Extra space around objects' bboxes to make */
/* erase area large enough if they are picked */
#define Normalbboxmargin (BBoxoutdent + Pkboxsize + 2.0)

/* Extra space around objects' shadow bboxes to make */
/* erase area large enough */
#define Shadowbboxmargin 2.0

#define BBoxscreensize sizeof(bboxscreentype)
#define Eraserecordsize sizeof(erase_record_type)
  
/* Pick feedback constants */
#define Pickedcontouroffset 12.0

#define eyepos lookat

#define Leftbutton LEFTMOUSE
#define Midbutton MIDDLEMOUSE
#define Rightbutton RIGHTMOUSE

#define Roomwidth 1022.0
#define Roomheight 766.0
#define Roomdepth 1000.0
#define Maxcrossx 510
#define Mincrossx -511
#define Maxcrossy 383
#define Mincrossy -382
#define Maxplanez 500
#define Minplanez -499

#define Filledpkbox TRUE
#define Hollowpkbox FALSE

#define BBoxoffset -3.0
#define BBoxoutdent (Pkboxsize * 2)
#define Crosshairoffsetz 2.0	/* hack for zbuffer */
#define Crosshairshadowoffset 3.0	/* hack for Hughes demo 8/9/90 */
#define Sectionlineoffset 1.0
#define Sectionlinewidth 3

/* Help Window graphics defs */

#define Mousemargin 20.0
#define Mousebody_x Mousemargin
#define Mousebody_y (helpwindowtop - helpwindowbottom - Mousemargin)
#define Mousebodywidth ((helpwindowright - helpwindowleft) / 2.0)
#define Mousebodyheight (Mousebodywidth * 2.0)
#define Mousethickness (Mousebodywidth / 10.0)
#define Mousecorneradius (Mousebodywidth / 10.0)
#define Buttonwidth (Mousebodywidth / 4)
#define Buttonheight (Mousebodyheight / 3.0)
#define Buttonmargin (Buttonwidth / 5.0)
#define Buttoncorneradius (Buttonwidth / 10.0)
#define Buttonthickness (Buttonwidth / 10.0)
#define Pickbuttonx (Mousebody_x+Buttonmargin)
#define Pickbuttony (Mousebody_y-Buttonmargin)
#define Planebuttonx (Pickbuttonx + Buttonwidth + Buttonmargin)
#define Planebuttony Mousebody_y-Buttonmargin
#define Roomrotbuttonx (Planebuttonx + Buttonwidth + Buttonmargin)
#define Roomrotbuttony Mousebody_y-Buttonmargin
#define Statuslinex 20
#define Statusliney 20

/* pattern defs for lines */

#define Dotpattern 0xAAAA
#define Largedotpattern 0xCCCC
#define Dashpattern 0xF0F0
#define Largedashpattern 0xFCFC
#define Inversedotpattern ((GLuint) ~0xAAAA)
#define Inverselargedotpattern ~0xCCCC
#define Inversedashpattern ~0xF0F0
#define Inverselargedashpattern ~0xFCFC

/* Dimensioning constants */
#define Edgedimensionmargin 5.0
#define Edgedimensionoffset 30.0
#define Edgedimensionindent 10.0
#define Dimensionoffplane 5.0

/* Added 12/30/01 for new calls to opengl list routines */
#define MAX_GL_LISTS 100000

typedef struct
{
  int x1,y1,x2,y2;		/* extents of saved blits */
  unsigned long *pix_save_area;	/* space for storing pixels */
  GLboolean validblit;		/* whether it should be restored or not */
} blitsavetype,*blitsaveptr;


enum 		/* enums for GL display list objects */
{
  Room,
  Blank,
  Vanillaview,
  Cutplane,
  Cutplaneborder,
  Crosshair,
  Planepickedrect
};

enum
{
  Dotline = 1,
  Inversedotline,
  Largedotline,
  Inverselargedotline,
  Dashline,
  Inversedashline,
  Largedashline,
  Inverselargedashline
};

typedef struct
{
  bboxscreenptr erase_bboxes;
  featureptr *logged_features;
  GLboolean *bbox_erased_already;
  int max_logged_bbox;
  int erase_bboxes_array_size;
} erase_record_type,*erase_record_ptr;

#endif /* grafx_includes */
