 
/* FILE: polhemus.h */

#ifndef polhemus_includes
#define polhemus_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*      Polhemus Reading Routines Header File for the Cutplane Editor     */
/*                                                                        */
/* Authors: LJE and WAK                           Date: September 28,1990 */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990,1991 William Kessler, Larry          */
/* Edwards (Beyond Technology, Inc.), and Jim Kramer (Virtex, Inc.)       */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define conv2deg ((4.0*180.0/M_PI)/32767.0)

typedef struct
{
  vertype pos,deltapos;
  vertype orient,deltaorient;

  Matrix oldxform_world;
  Matrix xform,invxform;
  Matrix deltaxform,invdeltaxform;
  Matrix deltaxform_world,invdeltaxform_world;
} polhstatestruct,*polhstateptr;

typedef struct
{
  vertype pos;
  vertype orient;
} polhemusvalstruct,*polhemusvalptr;

typedef struct
{
  vertype posoffset,poscale;
} calibratestruct,*calibrateptr;


extern int	errno;
extern char *sys_errlist[];
extern int	sys_nerr;


#endif /* polhemus_includes */
