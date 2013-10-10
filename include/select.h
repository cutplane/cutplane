
/* FILE: select.h */

#ifndef select_includes
#define select_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*        Object Selection Routines Header File for the Cutplane Editor   */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <platform_gl.h>

#include "list.h"
#include "geometry.h"

/* Select Constants */
enum
{
  Nothingselect = 0,
  Vertselect,
  Edgeselect,
  Faceselect,
  Inplanefaceselect,
  Shellselect,
  Objselect
};

#define Oneselectable_permitted TRUE
#define Manyselectables_permitted FALSE

#define selectablesize sizeof(selectabletype)

typedef struct selectablestruct
{
  struct elemstruct *thefeature;      /* The feature that is selectable */
  long selectkind;		      /* Its type. */
  vertype nearestpos;		      /* Where the pick box should go */
  void (* selectboxtechnique)();    /* What routine should draw the select box */
  struct segmentstruct *motherseg;    /* The ring segment where the feature*/
} selectabletype,*selectableptr;      /* came from. */


#endif
