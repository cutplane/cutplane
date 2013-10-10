
/* FILE: pencilsketch.h */

#ifndef pencilsketch_includes
#define pencilsketch_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*    Pencil Sketching Routines Header File for the Cutplane Editor       */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <platform_gl.h>

#include "list.h"
#include "user.h"
#include "properties.h"

/* This id# is for sketch elements (sles) only.  It's at 300 to avoid */
/* collision with those in list.h,topology.h, and properties.h. */

/* Hey WAK: i've changed the number so i can use my typesize array */
#define Sketchle_type 6
#define Sketchlesize sizeof(sketchletype)


#define Accy 0.001		/* used by intersect_segments() */
#define Line1End1 1		/* Bits turned on by intersect_segments() */
#define Line1End2 2		/* indicating intersection of line segs */
#define Line2End1 4		/* at or near endpoints, or not near them. */
#define Line2End2 8

#define End1 4
#define End2 8
#define Bothends 16
#define Center_intersect 32

#define Coming 1		/* whether a sketchle  is pointing at a vfe */
#define Going 2			/* or towards it. */
#define Left 0			/* left side of an evf */
#define Right 1			/* right side of an evf */
#define Outside 0		/* whether a point is inside or outside */
#define Inside 1		/* a face */
#define Notraversed 0		/* whether  a sketchle has been walked in */
#define Traversed 1		/* the face culling pass of the algorithm. */
#define Clockwise 0
#define Counterclockwise 1

/* for the macpaintlike sketching routine */
#define Nonewpoint 0
#define Movetopoint 1
#define Drawtopoint 2
#define Autoclose 3

#define Pi 3.1415976 /* for lack of existing def somrewhere */


/* sketching structure, similar to lestruct but for new wak sketch algo. */
struct sketchlestruct
{
  struct sketchlestruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;

  struct evfstruct *sketchevf;
  struct vfestruct *sketchvert;	/* the vert this sketchle starts at */
};

typedef struct sketchlestruct   sketchletype;
typedef sketchletype * sketchleptr;


#endif
