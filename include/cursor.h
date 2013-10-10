
/* FILE: cursor.h */

#ifndef cursor_includes
#define cursor_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*              Cursor Display Definitions for 3Form                      */
/*                                                                        */
/* Author: WAK                                     Date: February 26,1991 */
/* Version: 3                                                             */
/* Copyright (C) 1990,1991 William Kessler and Laurence Edwards.          */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <platform_gl.h>
// #include <device.h>

#include "cutplane.h"
#include "grafx.h"

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                           CONSTANT DEFINITIONS                       */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#define Basicursordepth 10.0
#define Hiddencursorwidth 1.0
#define Hiddencursorheight 1.0
#define Hiddencursordepth 1.0
#define Pluscursorwidth 35.0
#define Pluscursorheight Pluscursorwidth
#define Pluscursordepth Basicursordepth
#define Arrowcursorsquare 18.0
#define Arrowcursorwidth Arrowcursorsquare
#define Arrowcursorstemwidth (Arrowcursorwidth * 0.15)
#define Arrowcursorheight (Arrowcursorwidth * 2.45)
#define Arrowcursordepth Basicursordepth
#define Ibeamcursorwidth 10.0
#define Ibeamcursorheight 25.0
#define Ibeamcursordepth Basicursordepth
#define Xhaircursorwidth Roomwidth /* size of the plane */
#define Xhaircursorheight Roomheight
#define Xhaircursordepth Basicursordepth

typedef enum
{
  Hiddencursor = 0,	/* used when some other object is entire cursor */
  Pluscursor,
  Arrowcursor,
  Ibeamcursor,
  Xhaircursor,
  Maxcursors			/* should there be a precursor?? */
} cursorkind;

typedef struct
{
  void (*cursor_drawptr)(objptr);
  /* routine to save what will be underneath */
  void (*cursor_saveptr)(objptr,int,stateptr);
  /* routine to restore what was underneath */
  void (*cursor_restoreptr)(int);
} cursorecordtype,*cursorecordptr;

#endif /* cursor_includes */
