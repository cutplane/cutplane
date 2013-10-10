
/* FILE: color.h */

#ifndef color_includes
#define color_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* Der Newen Improven Droewen/Displayenspiel Rutinnen fur ausch Cutplagen */
/*                          Editeriat,yah yah.                            */
/*                      (Der Definiciones, si si)                         */
/* Authors: LJE and WAK                           Date: November 19,1990  */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <platform_gl.h>

#define defcolor mapcolor

/* change this to color if using colormap mode...and change def of colortype */
#define setcolor glColor3sv

typedef GLshort colortype[3];
typedef short *colorptr;


#endif /* color_includes */
