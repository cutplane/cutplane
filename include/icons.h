
/* FILE: icons.h */

#ifndef icons_includes
#define icons_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Icon Routines Header File for the Cutplane Editor            */
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

#define Rotateiconrad 180.0
#define Virtualrotateiconrad (Rotateiconrad / 2.0)
#define Freedgerotateiconrad (Rotateiconrad / 4.0)
#define Baublesize (Pkboxsize * 1.3)
#define Dragbaublesize (Pkboxsize * 1.3)
#define Rotateiconctrsize Pkboxsize
#define Rotateiconcolor white
#define Swizzlebehindcolor white
#define Swizzleinfrontcolor white
#define Minswizzlelength 50.0
#define Swaponsameaxis 1
#define Swaponotheraxis 2
#define Pausewait 10
#define Minpauseangle 200
#define Pausescapedistance 40.0

/* Plane grade icon definitions  */
#define Numplanegrades 10
#define Planegradeindentx 20
#define Planegradeindenty 20
#define Planegradecubesize 10
#define Planegradeiconheight (Planegradecubesize * Numplanegrades)

typedef enum
{
  Donotuse_rotateicon = 0,
  Rotate_inplane_axis,		/* rotate about an axis in the Cutplane */
  Rotate_outofplane_axis,	/* rotate about an axis not in the Cutplane */
  Rotate_dragging,
  Maxrotatekind
} rotatekind;

#endif
