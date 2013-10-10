
/* FILE: list.h */

#ifndef list_includes
#define list_includes

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

#include "color.h"

#define Nil 0
#define Null Nil

#define Prev 0
#define Next 1

#define Maxnumtypes 50

#define Allsize sizeof(alltypes)
#define Listsize sizeof(listype)

#define Elem_type 0		/* generic elem type_id */
#define Feature_type Elem_type
#define Ep_type 1		/* type id of elemptrstructs (point at elems) */
#define Sort_type 5		/* type id of sort elements */

#define Elemsize sizeof(elemtype)
#define Epsize sizeof(elemptrtype)
#define Sortsize sizeof(sortype)

/* union of all simple types used in displaylist structures & sort structures */
union anydata
{
  char c;
  short s;
  int i;
  long l;
  float f;
  double d;
  char *ptr;
  struct elemstruct *elem;
};

/* head of any linked list... */

#define Normal_list 0
#define Circular_list 1

#ifdef FAST_N_SMART
#define next_elem(thislist,thiselem) \
  ((elemptr) ((thiselem)->next != Nil) ? (thiselem)->next : (thislist)->first.any)
#define prev_elem(thislist,thiselem) \
  ((elemptr) ((thiselem)->prev != Nil) ? (thiselem)->prev : (thislist)->last.any)
#endif /* FAST */


struct liststruct
{
  short kind;		/* Circular_list or Normal_list */
  union
  {
    struct elemstruct *any;	/* generic label for clist rtns */
    struct vfestruct *vfe;	/* a vertex face element in vertlist*/
    struct sketchlestruct *sketchle; /* a sketch edge element in drawlist */
    struct evfstruct *evf;	/* edge records */
    struct lestruct  *le;	/* loop's edges records */
    struct fvestruct  *fve;	/* faces records */
    struct boundstruct *bnd;
    struct shellstruct  *shell;	/* shell records in shellist */
    struct objstruct *obj;	/* object structures */
    struct worldstruct *world;	/* world structures */
    struct pickthreadstruct *picked; /* start of pick thread */
    struct segmentstruct *seg;	/* section list element */
    struct ringstruct *ring;	/* ring list element */
    struct curvestruct *curve;	/* curve list element */
    struct surfstruct *surf;	/* surf list element */
    struct sortstruct *sort;	/* used for sorting things by coordinates */
    struct veestruct *vee;	/* used in boolean preprep. */
    struct efstruct *ef;	/* used in boolean preprep. */
    struct vvstruct *vv;	/* used in boolean classif. */
    struct vfstruct *vf;	/* used in boolean classif. */
    struct sectorstruct *sector; /* used in boolean classif. */
    struct sectorpairstruct *sectorpair; /* used in boolean classif. */
    struct facepairstruct *fp;	/* used in boolean sorting */
    struct lepairstruct *lp;	/* used in boolean sorting */
    struct elemptrstruct *ep;	/* generic list of pointers to list */
    struct toolstruct *tool;	/* the tools associated with a receptacle */
    struct delemstruct *dl;	/* elements*/
    struct oelemstruct *ol;
    struct qelemstruct *ql;
    struct propstruct *prop;
  } first,last;
  int numelems;         /* a list, must be cast to whatever list */
};                      /* type you're using. */
                        /* numelems: Number of elements in the */
                        /*           list */

/* the basic element in any list. */

struct elemstruct
{
  struct elemstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  void *privatecode;		/* CPL codes for this feature */
  void *messagequeue;		   /* CPL messages for this feature */
  long featureflags;
  colorptr thecolor;
};

/* NOTE: the proplist is now a pointer to a liststruct. Make sure to call */
/* create_list on it before using it. This makes slice.c invalid!! */

struct elemptrstruct
{
  struct elemptrstruct *prev, *next;
  int type_id;
  struct liststruct *proplist;	/* pointer to property list */
  struct liststruct **global_proplists;
  void *privatecode;		/* CPL codes for this feature */
  void *messagequeue;		   /* CPL messages for this feature */
  long featureflags;		/* not really used here but has to be compat. */

  struct elemstruct *thiselem;
};

struct sortstruct
{
  struct sortstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  void *privatecode;		/* CPL codes for this feature */
  void *messagequeue;		   /* CPL messages for this feature */
  long featureflags;		/* not really used here but has to be compat */

  struct elemstruct *sortelem;
  union anydata *sortdata1,*sortdata2,*sortdata3;
};

/* typedefs to make var defs easy */

typedef union  anydata          alltypes;

typedef struct liststruct       listype;
typedef struct elemstruct       elemtype;
typedef struct elemptrstruct    elemptrtype;
typedef struct sortstruct       sortype;

typedef listype      * listptr;
typedef elemtype     * elemptr; /* these are the same for list routine */
typedef elemtype     * featureptr; /* compatibility purposes. */
typedef elemptrtype  * elemptrptr;
typedef sortype      * sortptr;

#endif
