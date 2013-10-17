
/* FILE: properties.h */

#ifndef properties_includes
#define properties_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*              All Property List Handling Constants Defined Here         */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "list.h"


/* This id# starts at 200 to avoid colliding with those in list.h and */
/* topology.h */

/* Hey WAK: i've changed the number so i can use my typesize array */
#define Prop_type 7
#define Propsize sizeof(proptype)
#define Maxpossibleproperty 500   /* the most possible props including */
				   /* dynamic ones. */

typedef int property;   // Hmm does this need to be 64bit? -- LJE
#define Maxproperty Maxpossibleproperty
#define Maxpropertynamelen 40


#include "cpl_cutplane.h"	/* DO NOT MOVE TO TOP!!!! */

typedef struct propinfostruct
{
  char propname[Maxpropertynamelen];
  CPL_word_ptr propertycode;
} propinfotype,*propinfoptr;


typedef struct propstruct
{
  struct propstruct *prev,*next;
  int type_id;
  struct liststruct *proplist;
  struct liststruct **global_proplists;
  elemptr owner;		/* feature that has this property */
  property propkind;
  union anydata value;
} proptype, *proptr;

#endif
