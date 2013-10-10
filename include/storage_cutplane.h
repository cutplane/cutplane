
/* FILE: storage.h */

#ifndef storage_includes
#define storage_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*   Read/Save Objects from/to Disk: Header File for the Cutplane Editor  */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define NAMELENGTH 50
#define Numspecialfiles 20

#define Techniquenamelen 50
#define Numtechniques 50

#define Pathnamelength 200
#define Filenamelength 55

typedef struct techniquestruct
{
  char technique_name[Techniquenamelen];
  void (*technique_rtn)();
} techniquerec,*techniquerecptr;

typedef char filename_type[Pathnamelength + Filenamelength];

#endif

