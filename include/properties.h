
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

// We sometimes pass a property as if it were an address (e.g., in
// apply_to_descendants()) so make sure it's the same size as an
// address. Probably not necessary, but get's rid of a warning. -- LJE
// See similar typedefs in cpl_cutplane.h, should combine and put in
// config.h!
#ifdef __SIZEOF_POINTER__     // GCC & CLANG define __SIZEOF_POINTER__
#if __SIZEOF_POINTER__ == 8
typedef int64_t property;
#else
typedef int32_t property;
#endif
#else  // Otherwise just assume 64 bit...
typedef int32_t property;
#endif

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
