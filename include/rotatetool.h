
/* FILE: rotatetool.h */

#ifndef rotatetool_includes
#define rotatetool_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Rotate Tool Header File for the Cutplane Editor              */
/*                                                                        */
/* Authors: WAK                                   Date: October 26,1990   */
/*                                                                        */
/* Copyright (C) 1989,1990,1991 William Kessler and Larry Edwards.        */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <platform_gl.h>

#include "list.h"
#include "user.h"

#define Rotatetoolradius 130.0
#define Rotatedockmargin (Rotatetoolradius/4.0)
#define Rotatetooldepth 2.0
#define Rotatetool_dockx (Maxcrossx - Rotatetoolradius - Rotatedockmargin)
#define Rotatetool_docky (Maxcrossy - Rotatetoolradius - Rotatedockmargin)
#define Rotatesnapangle 15
#define Baublesize (Pkboxsize * 1.0)
#define Dragbaublesize (Pkboxsize * 1.3)
#define Showbaubles FALSE
#define Hidebaubles TRUE
#define Rotateiconctrsize (Pkboxsize * 2.0)
#define Rotateiconcolor white
#define Swizzlebehindcolor white
#define Swizzleinfrontcolor white
#define Minswizzlelength 50.0
#define Swaponsameaxis 1
#define Swaponotheraxis 2
#define Pausewait 10
#define Minpauseangle 200
#define Pausescapedistance 40.0

#endif
