
/* FILE: boolean.h */

#ifndef boolean_includes
#define boolean_includes

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                                                                          */
/*     SOLID OBJECT BOOLEAN OPERATOR ROUTINES FOR THE CUTPLANE EDITOR       */
/*                                                                          */
/* Authors: WAK                                                             */
/* Date: Feb 9 1990                                                         */
/* Version: 1.0                                                             */
/*                                                                          */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland    */
/* Stanford Junior University, and William Kessler and Larry Edwards.       */
/* All Rights Reserved.                                                     */
/*                                                                          */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#define AonB 1
#define BonA 2
#define SolidA 0
#define SolidB 1

#define Null_le 0		/* for getnext_connectable */
#define Null_le_twin 1		/* for getnext_connectable */

#define BOOL_OUT -1		/* these coincide with the results returned */
#define BOOL_IN 1		/* by range(). */
#define BOOL_ON 0
#define BOOL_Union 0
#define BOOL_Subtraction 1
#define BOOL_Intersection 2

#define Onplanetol	1.0e-3	/* closeness of a dot prod between two vecs */
				/* to zero to indicate that they are normal. */
				/* (used in set_sectorpair()). */

enum
{
 EvfApt1on = 1,
 EvfApt2on,
 EvfBpt1on,
 EvfBpt2on,
 Evfsintersect
};

enum
{
  Sector_type = 20,
  Sectorpair_type,
  Vv_type,
  Vf_type,
  Vee_type,
  Ef_type,
  Facepair_type,
  Lepair_type
};

#define Sectorsize sizeof(sectortype)
#define Sectorpairsize sizeof(sectorpairtype)
#define Vvsize sizeof(vvtype)
#define Vfsize sizeof(vftype)
#define Veesize sizeof(veetype)
#define Efsize sizeof(eftype)
#define Facepairsize sizeof(facepairtype)
#define Lepairsize sizeof(lepairtype)

struct sectorstruct
{
  struct sectorstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  long featureflags;		/* Not really used here but has to be compat */

  leptr centerle;		/* The ctrle->facevert is the sector pt. */
  vertype ref1,ref2,ref1xref2;	/* Bounding vectors of sector and cross prod */
  vertype sectornorm;		/* The normal of the face the sector lies on */
  struct sectorpairstruct *sidesectorpair[2][2];
  int vfsidecodes[2];		/* Used for sector crossing a FACE of the */
};				/* other solid */

struct sectorpairstruct
{
  struct sectorpairstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  long featureflags;		/* not really used here but has to be compat */

  struct sectorstruct *sectorA,*sectorB; /* pointer to 2 intersecting sectors */
				         /* one in each object. */
  /* settings of side codes of sectors in objects A and B. */
  int sidecodesA[2];
  int sidecodesB[2];
  Boolean intersectcode;	/* Whether the two sectors intersect or not. */
};

/* This structure is head to a list of sectors contained at a vertex, which */
/* is called a neighborhood. */
				/* -->Will you be my neighbor? */

struct vvstruct
{
  struct vvstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  long featureflags;		/* not really used here but has to be compat */

  vfeptr vfeA,vfeB;		/* the vfes that are coincident. */
  listptr sectorlistA,sectorlistB; /* sectors around these vfes. */
  listptr sectorpairlist;      /* the sector pairs belonging to the neigh... */
};

struct vfstruct
{
  struct vfstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  long featureflags;		/* not really used here but has to be compat */

  elemptr touchingelem[2];	/* the vfe and face that are coincident */
  listptr sectorlist;
};

struct veestruct
{
  struct veestruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  long featureflags;		/* not really used here but has to be compat */

  struct evfstruct *evfA,*evfB;
  vertype breakpos;
  int onptcode;  
};

struct efstruct
{
  struct efstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  long featureflags;

  evfptr breakevf;
  fveptr breakfve;
  vertype breakpos;
  int evfinwhichsolid;
};

struct facepairstruct
{
  struct facepairstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  long featureflags;

  fveptr shellAfve,shellBfve;
  listptr paired_null_lelist;
};

struct lepairstruct
{
  struct lepairstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  long featureflags;

  Boolean isahole[2];
  leptr le[2];
  fveptr infaceA,outfaceA;	/* These are the faces that result when */
  fveptr infaceB,outfaceB;	/* the le's are snipped. */

  fveptr piercedface;		/* Whatever face is broken must be preserved */
				/* because this info is lost once the pierce */
				/* point is joined with neighboring null_le's*/
  listptr sectorlist;		/* if this pair resulted from a vf, */
};				/* then you need to remember the vertex's */
				/* sectorlist to do later snip_null_le's */
				/* properly. */

typedef struct sectorstruct sectortype;
typedef struct sectorpairstruct sectorpairtype;
typedef struct vvstruct vvtype;
typedef struct vfstruct vftype;
typedef struct veestruct veetype;
typedef struct efstruct eftype;
typedef struct facepairstruct facepairtype;
typedef struct lepairstruct lepairtype;

typedef sectortype * sectorptr;
typedef sectorpairtype * sectorpairptr;
typedef vvtype * vvptr;
typedef vftype * vfptr;
typedef veetype *veeptr;
typedef eftype *efptr;
typedef facepairtype * facepairptr;
typedef lepairtype * lepairptr;

typedef struct
{
  sectorpairptr sectorpair;
  int Aonside,Bonside;
} pair_oninfo_type;

#endif
