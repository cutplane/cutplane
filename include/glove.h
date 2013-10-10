 
/* FILE: glove.h */

#ifndef glove_includes
#define glove_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*      Glove Reading Routines Header File for the Cutplane Editor        */
/*                                                                        */
/* Authors: LJE and WAK                           Date: September 28,1990 */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990,1991 William Kessler, Larry          */
/* Edwards (Beyond Technology, Inc.), and Jim Kramer (Virtex, Inc.)       */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <platform_gl.h>

typedef enum
{
  Noglovevent = 0,
  Glovepointevent,
  GloveconstraintoXYevent,
  GloveconstraintoZevent,
  Glovepickevent,
  GloveXYpickevent,
  GloveZpickevent,
  Gloveroomrotevent,
  Gloverotatevent,
  Maxglovevent
} gloveventkind;

typedef enum
{
  Gloverelaxstate = 0,
  Glovepointstate,
  GloveconstraintoXYstate,
  GloveconstraintoZstate,
  Glovedragstate,
  GloveXYdragstate,
  GloveZdragstate,
  Gloveroomrotstate,
  Gloverotatestate,
  Maxglovestate
} glovestatekind;

#define Resetpolhemus 0x19
#define Quietmode 0x4B
#define Noisymode 0x6D
#define Binarymode 0x66

#define Carriagereturn 0x0D
#define Linefeed 0x0A

#endif
