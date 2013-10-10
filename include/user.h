
/* FILE: user.h */

#ifndef user_includes
#define user_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*     User Interface Routines Header File for the Cutplane Editor        */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <platform_gl.h>

#include "geometry.h"
#include "list.h"

#define Pkboxsize 10.0
#define Pkboxdiv 3.0

#define Fastcursormove 80.0
#define Minplanesnapdist 7.0

#define Fastrot 450
#define Slowrot 100
#define Cutsizex 1021.0
#define Cutsizey 765.0
#define Cutsizez 512.0
#define Zoombox 15
#define Minangle -1800
#define Maxangle 1800

#define Minrotx -3590
#define Maxrotx 3590
#define Minroty -3590
#define Maxroty 3590
#define Minroomrotx 0
#define Maxroomrotx 900
#define Minroomroty -900
#define Maxroomroty 900
#define Maxroomroty 900
#define Minzoom 1
#define Maxzoom 10000

#define Selection 0
#define Createsection 1
#define Sweepmode 2
#define Sketching 4

/* NOTE: this should be enum'd but p-iris can't compile |= and &= with enums!*/

#define Nocontrolbuttonsbit 0
#define Lbuttonbit 1
#define Mbuttonbit 2
#define Rbuttonbit 4
#define LMbuttonsbit  (Lbuttonbit | Mbuttonbit)
#define LRbuttonsbit  (Lbuttonbit | Rbuttonbit)
#define MRbuttonsbit  (Mbuttonbit | Rbuttonbit)
#define LMRbuttonsbit  (Lbuttonbit | Mbuttonbit | Rbuttonbit)
#define Roomrotatebuttonbit 8
#define Multipleselectbuttonbit 16
#define Zoombuttonbit 32
  
#define Leftbutton LEFTMOUSE
#define Midbutton MIDDLEMOUSE
#define Rightbutton RIGHTMOUSE

/* Object Attribute Constants */
				/* to be deleted when properties coded */
#define Noattributes 0
#define LockedGeometrytool 1
#define Deformtool 2
#define Cutool 4
#define Drawtool 8
#define Gluetool 16
#define Magnetictool 32
#define Hotspotsnap 64
#define Receptacletool 128
#define Transparentool 256

struct statestruct
{
  Icoord mousex,mousey,mousez;	/* Last polled mouse position */
  vertype cursor;		/* Position of the cross hair cursor.*/
  int roomrotx,roomroty;	/* Room rotation position. */
  int delroomrot;		/* The current room rotation increment value */
  long controlbuttonsdown;	/* Which control buttons are currently down */
  Icoord leftx,lefty,leftz;	/* Coordinates where these mouse buttons */
  Icoord midx,midy,midz;	/*  were pressed down*/
  Icoord rightx,righty,rightz;
};

typedef struct statestruct statetype;
typedef statetype *stateptr;

#endif
