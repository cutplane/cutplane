
/* FILE: topology.c */

#ifndef topology_includes
#define topology_includes


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*          Topology Type Definitions for the Cutplane Editor             */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <platform_gl.h>
// #include <device.h>

#include "select.h"
#include "properties.h"
#include "grafx.h"
#include "geometry.h"
#include "user.h"

#define Outer 1
#define Inner 0

/* euler operator constants */
#define Plus 1
#define Minus 0

#ifdef FAST
/* for speed... */

#define Twin_le(thisle) \
  ((leptr) ((thisle) == Nil) ? (thisle) : ((thisle) == (thisle)->facedge->le1) ?\
   ((thisle)->facedge->le2) : ((thisle)->facedge->le1))

#endif /* FAST */

#define Forwards 0
#define Reverse 1

#define Vertintr 0
#define Edgeintr 1
#define Edgeparallel 2
#define Edgevirtual 3
#define Inplaneface 0
#define Sectionring 1

/* Draw Flags for quick refresh are defined here. They are used in */
/* conjunction with some property records, such as selectable, picked. */

#define Noflags 0
#define Visibleflag 1
#define Selectableflag 2
#define Pickedflag 4
#define Dragflag 8
#define Transparentflag 16
#define Flipflag 32
#define Nonormalflag 64
#define Msgpendingflag 128
#define Nonmanifoldflag 256
#define Noshadowflag 512
#define Lockedgeometryflag 1024
#define Neverdrawflag 2048
#define Alwaysdrawflag 4096
#define Sectioninvisibleflag 8192
#define Selectinvisibleflag 16384
#define Pickedinvisibleflag 32768

#define Allflags (~Noflags)		/* all bits on */

enum
{
  Fve_intersect = 0,
  Fve_inplane,
  Evf_inplane,
  Vfe_inplane
};

/* Hey WAK: i've changed the numbers so i can use my typesize array */

enum
{
  Vfe_type = 8,
  Evf_type,
  Le_type,
  Bound_type,
  Fve_type,
  Shell_type,
  Obj_type,
  World_type,
  Seg_type,
  Ring_type,
  Curve_type,
  Surf_type
};

#define Vfesize sizeof(vfetype)
#define Evfsize sizeof(evftype)
#define Lesize sizeof(letype)
#define Boundsize sizeof(boundtype)
#define Fvesize sizeof(fvetype)
#define Shellsize sizeof(shelltype)
#define Objsize sizeof(objtype)
#define Worldsize sizeof(worldtype)
#define Segsize sizeof(segmentype)
#define Ringsize sizeof(ringtype)
#define Curvesize sizeof(curvetype)
#define Surfsize sizeof(surftype)

#define Maxworldnamelength 25
#define Maxobjectnamelength 25

/* definition of an element in an evf list... */

typedef struct evfstruct 
{                                     /* previous and next fields in evflist */
  struct evfstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  void *privatecode;		   /* CPL codes for this feature */
  void *messagequeue;		   /* CPL messages for this feature */

  long featureflags;		   /* whether picked, selectable, etc. */
  colorptr thecolor;
  short cutype;			   /* type of intersection, if any. */
  vertype local_cutpt;		   /* the intersection point in shells' local */
				   /* coord system */
  vertype cutpt;		   /* intersection point with cutplane */
  struct lestruct *le1,*le2;	   /* loop edges going with this edge*/
  Coord xdiff,ydiff,zc1,zc2;	   /* parametric values of equation */
  /* used for new wak sketch algorithm */
  /* here, sle means sketchle, c = coming into pt1vfe, g = going away */
  /* from pt1vfe */
  struct sketchlestruct *leftsle_c,*leftsle_g,*rightsle_c,*rightsle_g;
  Boolean traversed[2];		   /* whether the left or right side has */
				   /* been walked. */
  struct vfestruct *pt1vfe,*pt2vfe; /* pointers to edge start & end pts */
  struct fvestruct *f1,*f2;
  Boolean touched;		   /* this edge has been walked upon */
  unsigned short timestamp;	   /* last time hidden edge status was updated */
  Boolean hidden_edge;		   /* TRUE if this is a hidden evf. */
  void (* selectechnique)(struct evfstruct *, struct statestruct *);
} evftype,*evfptr;


typedef struct lestruct
{
  struct lestruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  void *privatecode;		   /* CPL codes for this feature */
  void *messagequeue;		   /* CPL messages for this feature */

  long featureflags;		   /* whether picked, selectable, etc. */
  struct boundstruct *motherbound; /* pointer to containing face or hole */
  struct evfstruct *facedge;	   /* corresponding evf in the evf list */
  struct vfestruct *facevert;	   /* pointer to starting vertex of this edge*/
} letype,*leptr;


typedef struct boundstruct
{
  struct boundstruct *prev,*next;  /* Previous and next bounds in face. */
  int type_id;
  struct liststruct *proplist;	   /* Head of properties list for bound. */
  struct liststruct **global_proplists;
  void *privatecode;		   /* CPL codes for this feature */
  void *messagequeue;		   /* CPL messages for this feature */

  long featureflags;		   /* whether picked, selectable, etc. */
  struct fvestruct *motherfve;	   /* Containing face */
  struct liststruct *lelist;	   /* Start of list of loop edges */
} boundtype,*boundptr;

/* curve element... */

typedef struct curvestruct
{
  struct curvestruct *prev,*next; /* Previous and next curves in list */
  int type_id;
  struct liststruct *proplist;	   /* Head of properties list for face.  */
  struct liststruct **global_proplists;
  void *privatecode;		   /* CPL codes for this feature */
  void *messagequeue;		   /* CPL messages for this feature */

  long featureflags;		   /* whether picked, selectable, etc. */

  struct liststruct *evflist;	/* edges used to represent this curve */

  /* eventually need the equation of the curve...for now we assume its a line */
  struct vfestruct *endpoints[2]; /* endpoints of this curve */
} curvetype,*curveptr;

/* surface element... */

typedef struct surfstruct
{
  struct surfstruct *prev,*next; /* Previous and next surface in list */
  int type_id;
  struct liststruct *proplist;	/* Head of properties list for face.  */
  struct liststruct **global_proplists;
  void *privatecode;		/* CPL codes for this feature */
  void *messagequeue;		   /* CPL messages for this feature */

  long featureflags;		/* whether picked, selectable, etc. */

  struct liststruct *fvelist;	/* facets used to represent this surface */

  /* ordered list of curves bounding the surface */
  /* for now we just use a list of ptrs to evfs since all curves are straight: */
  struct liststruct *curvelist;
} surftype,*surfptr;

/* definition of an element in an fve list... */

typedef struct fvestruct
{
  struct fvestruct *prev,*next;	   /* Previous and next faces in fvelist */
  int type_id;
  struct liststruct *proplist;	   /* Head of properties list for face.  */
  struct liststruct **global_proplists;
  void *privatecode;		   /* CPL codes for this feature */
  void *messagequeue;		   /* CPL messages for this feature */

  long featureflags;		   /* whether picked, selectable, etc. */
  colorptr thecolor;

  struct shellstruct *mothershell;	   /* Containing shell */
  struct liststruct *boundlist;	   /* Head of list of boundaries */
  struct surfstruct *mothersurf; /* surface this facet belongs to */
  Boolean cutype;		   /* How the face was cut by plane. */
  vertype facenorm,local_facenorm; /* the face normal */
  Coord planedist;		   /* Distance from the face plane to origin, */
  bbox3dtype facebbox;		   /* Bounding box around face for drawing */
  unsigned short timestamp;	   /* last time backface status was updated */
  Boolean back_face;		   /* TRUE if this is a back face fve. */
  void (* selectechnique)(struct fvestruct *, struct statestruct *);
} fvetype,*fveptr;				   /* after transformation */

/* definition of an element in a vfe list... */

typedef struct vfestruct
{
  struct vfestruct *prev,*next;	   /* Previous and next vertices in vfelist */
  int type_id;
  struct liststruct *proplist;	   /* Head of properties list for vertex. */
  struct liststruct **global_proplists;
  void *privatecode;		   /* CPL codes for this feature */
  void *messagequeue;		   /* CPL messages for this feature */

  long featureflags;		   /* whether picked, selectable, etc. */
  colorptr thecolor;
  vertype pos,local_pos;
  vertype vertexnorm;		/* Averaged normal for gouraud shading */
  struct lestruct *startle;	/* Start of cycle of le's containing vert*/
  Boolean translated;		/* Avoids multiple translations/refresh. */
  unsigned short timestamp;	/* last time pos was updated */
  void (* selectechnique)(struct vfestruct *, struct statestruct *);

  struct liststruct *edgelist;	   /* currently used in sketching */
} vfetype,*vfeptr;


typedef struct shellstruct
{
  struct shellstruct *prev,*next;  /* previous and next shells in shellist */
  int type_id;
  struct liststruct *proplist;	   /* Head of properties list for shell. */
  struct liststruct **global_proplists;
  void *privatecode;		   /* CPL codes for this feature */
  void *messagequeue;		   /* CPL messages for this feature */

  long featureflags;		   /* whether picked, selectable, etc. */

  struct objstruct *motherobj;  /* Object that contains this shells. */
  struct liststruct *vfelist;	/* list of verts in the shell */
  struct liststruct *evflist;	/* list of edges in the shell */
  struct liststruct *fvelist;	/* list of faces in the shell */
  struct liststruct *surflist;	/* list of surfaces in this shell */
  struct liststruct *ringlist;	/* list of section rings */
  struct liststruct *inplanefvelist; /* list of faces in the cutplane */
  struct liststruct *inplanevflist;  /* list of edges in the cutplane */
  struct liststruct *inplanevfelist; /* list of vertices in the cutplane */
  Boolean cclockwise;		/* the shells orientation */
} shelltype,*shellptr;

/* definition of an element in an object list... */

typedef struct objstruct
{
  struct objstruct *prev,*next; /* previous and next objects in objlist */
  int type_id;
  struct liststruct *proplist;	   /* Head of properties list for object. */
  struct liststruct **global_proplists;
  void *privatecode;		   /* CPL codes for this feature */
  void *messagequeue;		   /* CPL messages for this feature */

  long featureflags;		   /* whether picked, selectable, etc. */
  colorptr thecolor;
  
  char *obj_name;
  Matrix xform,invxform;	/* transformation matrix of object. */
  Matrix scalexform,scaleinvxform;	/* transformation matrix of object. */
  unsigned short timestamp;	/* last time matrix was changed */
  struct objstruct *parent;
  struct liststruct *children;
  struct liststruct *shellist;
  bbox3dtype objbbox;		/* bounding box around object shells */
  bbox3dtype groupbbox;		/* bounding box for thisobject + descendants */
  bboxscreentype objscreenbbox;	/* bounding box of this object as rendered */
  bboxscreentype groupscreenbbox; /* bounding box of this group as rendered */
  bboxscreentype shadowscreenbbox; /* bbox of rendered shadow (if manifold) */

  struct selectablestruct selectable_feature;  /* The one feature of this */
					       /* shell that is selectable */

  /* These fields point to C routines used to draw this shell-- as a cp */
  /* shell, as a menu shell (no section line), as an icon-- and the */
  /* routines that are used to determine HOW an shell becomes selectable. */

  void (* selectechnique)(struct objstruct *, struct statestruct *);
  void (* drawtechnique)();

  /* This routine returns a pointer to a useful screen bbox for this obj, */
  /* usually the precomputed one for manifolds, but on specially rendered */
  /* tools it can be anything. */
  bboxscreenptr (* screenbboxtechnique)();

  /* Whether or not this obj or its shadow needs redrawing in the current */
  /* frame buffer. */
  short redrawcode;

  /* The world this object lives in */
  struct worldstruct *motherworld; 

  /* Whether transforms on this obj apply to its parent. */
  Boolean linked;
  
} objtype,*objptr;

typedef struct
{
  Matrix xform, invxform;
} viewertype;

typedef struct worldstruct
{
  struct worldstruct *prev,*next; /* previous and next worlds in worldlist */
  int type_id;
  struct liststruct *proplist;	  /* Head of properties list for world. */
  struct liststruct **global_proplists;
  void *privatecode;		  /* CPL codes for this feature */
  void *messagequeue;		   /* CPL messages for this feature */

  long featureflags;		  /* whether picked, selectable, etc. */
  
  char *world_name;
  viewertype viewer;
  struct objstruct *root;	/* the root of this world object tree */
  /* global property lists for this world. */
  struct liststruct *world_proplists[Maxproperty];
} worldtype,*worldptr;

typedef struct segmentstruct
{
  struct segmentstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;

  int kind;
  struct lestruct *le[2];
  Coord *pt[2];
  bbox2dtype segbbox;		/* The bbox around segment (if Fve_inplane)*/
} segmentype,*segmentptr;

typedef struct ringstruct
{
  struct ringstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;

  struct liststruct *seglist;    /* This section ring is a ring of faces. */
  bbox2dtype ringbbox;		/* The bounding box around the entire ring. */
} ringtype,*ringptr;

#define First_shell_le(x) (x->fvelist->first.fve->boundlist->first.bnd->lelist\
->first.le)
#define First_shell_bnd(x) (x->fvelist->first.fve->boundlist->first.bnd)
#define First_shell_vfe(x) (x->vfelist->first.vfe)
#define First_shell_evf(x) (x->evflist->first.evf)
#define First_shell_fve(x) (x->fvelist->first.fve)

#define First_obj_le(x) (x->shellist->first.shell->fvelist->first.fve\
->boundlist->first.bnd->lelist->first.le)
#define First_obj_bnd(x) (x->shellist->first.shell->fvelist->first.fve\
->boundlist->first.bnd)
#define First_obj_vfe(x) (x->shellist->first.shell->vfelist->first.vfe)
#define First_obj_evf(x) (x->shellist->first.shell->evflist->first.evf)
#define First_obj_fve(x) (x->shellist->first.shell->fvelist->first.fve)
#define First_obj_shell(x) (x->shellist->first.shell)

#endif /* topology_includes */
