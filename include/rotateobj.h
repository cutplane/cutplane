
/* FILE: rotateobj.h */

#ifndef rotateobj_includes
#define rotateobj_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Rotate Object Routines Header File for the Cutplane Editor   */
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


#define rx 0
#define ry 1
#define tolznorm 0.996

#ifndef ROTATEOBJMODULE

extern Matrix rotmatrix;
extern Matrix invmatrix;

#endif

typedef struct rotateinfostruct
{
  vertype rotvec,rotpt;
  int rotangle;
} rotateinfotype,*rotateinfoptr;

#endif
