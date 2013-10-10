
/* FILE: slice.h */

#ifndef slice_includes
#define slice_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*        Global Variables Header File for the Cutplane Editor            */
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


#define properties_includes /* delete this when props are ready!!*/

#ifndef properties_includes
#include "properties.h"
#endif


#define Nosplit 0
#define Dosplit 1

#define Inback 0
#define Infront 1
#define Onplane 2

#define Split_prop 0


#endif
