
/* FILE: geometry.h */

#ifndef geometry_includes
#define geometry_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Geometry Types Header File for the Cutplane Editor           */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                            Tolerances                                  */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


#define BBox2dsize sizeof(bbox2dtype)
#define BBox3dsize sizeof(bbox3dtype)

#define Pttopttol 1.0e-3	/* tolerance for closeness of a pt to a pt */
#define Ptonlinetol 1.0e-3	/* tolerance for closeness of pt on line/seg*/
#define Ptonplanetol 1.0e-3	/* tolerance for closeness of point on plane */
#define Linetolinetol 1.0e-3	/* tolerance for closeness of line intersect*/
#define Planetoplanetol 1.0e-3	/* tolerance for intersection of two planes */
#define Divzerorange 1.0e-308   /* smallest near div/zero on sun machines */
#define DivzerorangeIRIS 1.0e-5 /* smallest near div/zero on persiris machines*/
#define Floatol 1.0e-6

#define vx 0
#define vy 1
#define vz 2
#define vw 3


#ifndef IRIS
#define divide_by_zero(a) ((1.0 / a) == (1.0/0.0)) /* Note: Inf result */
				/* is handled by unix machines */
#else				/* Note: Iris compiler brain dead */
#define divide_by_zero(a) (Fabs(a) < DivzerorangeIRIS)
#endif

#define less_than_tol(a,b) (compare(a,0.0,b) == 0)

enum
{
  Lineonplane,
  Lineparallelsplane,
  Lineintersectsplane
};

enum
{
  Linesegonplane,
  Linesegendpt1onplane,
  Linesegendpt2onplane,
  Linesegintersectsplane,
  Linesegoffplane
};

enum
{
  Lineseginface,		/* 0 */
  Lineseginfacept1,		/* 1 */
  Lineseginfacept2,		/* 2 */
  /* line seg on plane of face, may multiply*/
  /* intersect. */
  Linesegintspossible,		/* 3 */
  Linesegpt1onboundvfe,		/* 4 */
  Linesegpt1onboundle,		/* 5 */
  Linesegpt1inface,		/* 6 */
  Linesegpt2onboundvfe,		/* 7 */
  Linesegpt2onboundle,		/* 8 */
  Linesegpt2inface,		/* 9 */
  Linesegintersectsface,	/* 10 */
  Linesegintersectsboundvfe,	/* 11 */
  Linesegintersectsboundle,     /* 12 */
  Linesegoutsideface		/* 13 */
};

enum
{
  Linesequal,
  Linesdonotintersect,
  Linesintersect
};

enum
{
  Linesegsequal,
  Linesegsdonotintersect,
  Linesegsintersect
};

enum
{
  Ptoutsidebound,
  Ptinsidebound,
  Ptonboundvfe,
  Ptonboundle,
  Ptnotonbound
};

enum
{
  Ptoutsideface,
  Ptonfaceboundvfe,
  Ptonfaceboundle,
  Ptinsideface
};


/* the position of a vertex in 3D space, homog. coord.'s */

typedef Coord vertype[4];
typedef Coord pos2d[2];
typedef Coord pos3d[3];
typedef Coord pos4d[4];
typedef Screencoord screenpos[2];

typedef struct
{
  screenpos mincorner,maxcorner;
} bboxscreentype,*bboxscreenptr;

typedef struct
{
  pos2d mincorner,maxcorner;
} bbox2dtype,*bbox2dptr;

typedef struct
{
  vertype mincorner,maxcorner;
} bbox3dtype,*bbox3dptr;

#define Leftofbbox 1
#define Rightofbbox 2
#define Bottomofbbox 4
#define Topofbbox 8
#define Frontofbbox 16
#define Backofbbox 32

/* for Liang-Barsky clipping routines */

#define Notclipped 0
#define Pt0clipped 1
#define Pt1clipped 2
#define Bothclipped 3
#define Maybeclipped 4

#endif
