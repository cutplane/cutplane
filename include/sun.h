
/* FILE: list.h */

#ifndef sun_includes
#define sun_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*    All List Handling Structures and Important Constants Defined Here   */
/*    As Well as externs for all list.c routines.                         */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define Delemsize sizeof(delemtype)
#define Oelemsize sizeof(oelemtype)
#define Qelemsize sizeof(qelemtype)

#define Delem_type 2		/* type id of delemstructs */
#define Oelem_type 3		/* type id of oelemstructs */
#define Qelem_type 4		/* type id qelemstructs */


struct delemstruct
{
  struct delemstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  long featureflags;		/* not really used here but has to be compat */

  short func_id;
  union anydata args[10];
};

struct oelemstruct
{
  struct oelemstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  long featureflags;		/* not really used here but has to be compat */

  int objid;
  struct liststruct *dlisthead;
};

/* structures for queue lists */

struct qelemstruct
{
  struct qelemstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  long featureflags;		/* not really used here but has to be compat */

  unsigned short device;
  short val;
};

typedef struct delemstruct      delemtype;
typedef struct oelemstruct      oelemtype;
typedef struct qelemstruct      qelemtype;

typedef delemtype    * delemptr;
typedef oelemtype    * oelemptr;
typedef qelemtype    * qelemptr;

#ifndef geometry_includes
/* the position of a vertex in 3D space, homog. coord.'s */
typedef Coord vertype[4];
#define vx 0
#define vy 1
#define vz 2
#define vw 3
#endif

#endif
