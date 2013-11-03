
/* FILE: shade.h */

#ifndef shade_includes
#define shade_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*        Shading Routines Header File for the Cutplane Editor            */
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


#define ZbufferRangeMin 12
#define ZbufferRangeMax 250
#define ZbufferRange (ZbufferRangeMax - ZbufferRangeMin)
#define Flashmargin (ZbufferRange * .15)
#define NonFlashRange (ZbufferRange - Flashmargin)
#define FlashRangeStart (zbuffershellcolor + NonFlashRange)

#define Newflashmargin 0.25
#define Edgeselectcoloroffset 30
#ifndef Pi
#define Pi 3.141592654
#endif
#define Maxacostable 1000

#define Selectableoffsetscale 3.0

#endif
