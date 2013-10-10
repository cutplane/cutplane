
/* FILE: cutplane.h */

#ifndef cutplane_includes
#define cutplane_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*    This file includes definitions and declarations common to most      */
/*    files as well as ALL the externs (see extern.h)                     */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <platform_gl.h>

/* Added by WAK to support GL's types, 10/18/01 */
typedef GLfloat Coord;
typedef Coord Matrix[4][4];
typedef GLint Screencoord;
typedef GLboolean Boolean;
typedef long Icoord;

#ifndef TRUE
#define TRUE GL_TRUE
#endif
#ifndef FALSE
#define FALSE GL_FALSE
#endif

/* end additions */

#include "geometry.h"
#include "color.h"
#include "list.h"
#include "select.h"
#include "properties.h"
#include "topology.h"
#include "user.h"
#include "boolean.h"

#include "extern.h"

#endif /* cutplane_includes */
