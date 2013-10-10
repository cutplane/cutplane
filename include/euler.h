
/* FILE: euler.h */

#ifndef euler_includes
#define euler_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*  Low-Level Euler Operator Routines Header File for the Cutplane Editor */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define Outer 1
#define Inner 0

/* euler constants */
#define Plus 1
#define Minus 0

#ifdef FAST
/* for speed... */

#define Twin_le(thisle) \
  ((leptr) ((thisle) == Nil) ? (thisle) : ((thisle) == (thisle)->facedge->le1) ?\
   ((thisle)->facedge->le2) : ((thisle)->facedge->le1))

#endif /* FAST */

#endif /* euler_includes */
