
/* FILE: intersect.h */

#ifndef intersect_includes
#define intersect_includes

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

/* edge cut constants */
#define Wascut 1
#define Pt1cut 2
#define Pt2cut 3
#define Oncut 4
#define Nocut 5
#define Nearplane 6
#define Pt1near 7
#define Pt2near 8
#define Parallel 9

#define Tolerance 0.5		/* generic plane-to-object distance. */
				/* have tolerance, my son. */
#define Forward 0
#define Reverse 1

#define Cutplanebboxmargin 2.0

#endif
