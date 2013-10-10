
/* FILE: tools.h */

#ifndef tools_includes
#define tools_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Tool Routines Header File for the Cutplane Editor            */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/


#include <platform_gl.h>

#ifndef list_includes
  #include "list.h"
#endif
#ifndef geometry_includes
  #include "geometry.h"
#endif

#define Pencilwidth 10.0
#define Pencilength (12.0*Pencilwidth)
#define Penciltip (1.5*Pencilwidth)

#define Menublockwidth 75.0
#define Menublockheight 50.0
#define Menublockdepth 15.0
#define Menublockresolution 25.0
#define Menumargin (Menublockheight * 2)

#define Toolboxwidth 85.0
#define Toolboxheight 85.0
#define Toolboxdepth 50.0
#define Toolboxleaf 0.5
#define Toolboxmargin Toolboxheight
#define Toolboxleafrot 350

#define Magnetradius 35.0
#define Magnetbarthickness 10.0
#define Magnetdepth 10.0
#define Magnetresolution 15
#define Magnetystretch 1.5

#define Toolpadsquare 30.0
#define Numtoolpadverts 10
#define Toolpadcorner (Toolpadsquare/8.0)
#define Toolpadwidth (Toolpadsquare * 3.0)
#define Toolpadheight (Toolpadsquare * 1.5)
#define Toolpadwidthmargin (Toolpadwidth * 0.5)
#define Toolpadheightmargin (Toolpadheight * 0.5)
#define Padhandlewidth (Toolpadsquare /4.0)
#define Padhandleheight Toolpadsquare

#define Headpadsquare 30.0
#define Numheadpadverts 10
#define Headpadcorner (Headpadsquare/8.0)
#define Headpadwidth (Headpadsquare * 2.0)
#define Headpadheight (Headpadsquare * 2.0)
#define Headpadwidthmargin (Headpadwidth * 0.5)
#define Headpadheightmargin (Headpadheight * 0.5)

#define Toolpadzoffset 7.0
#define Headpadzoffset 3.0
#define Toolpadblacklinewidth 3		/* width of outer black line */
#define Decalthickness 10.0
#define Decalzoffset (Toolpadzoffset*4.0)

#define Toolbarwidth (Headpadcorner * 4.0)
#define Toolbarheight 50.0
#define Toolbardepth 10.0

#define Normalpadsquare 60.0
#define Normalpadcorner (Normalpadsquare/16.0)
#define Storagesquare Normalpadsquare

#define Floppysize 100.0
#define Floppycorner (Floppysize / 40.0)
#define Floppynotch (Floppysize / 14.0)
#define Floppydepth (Floppysize / 20.0)

#define Portfoliosize 5		/* for stocks demo */
#define Portfoliohistory 10

#define Primswidth (Toolboxwidth)
#define Primsheight (Toolboxheight)
#define Primsdepth Toolboxdepth
#define Primsmargin (Primsheight / 5.0)
#define Primsresolution 14

#define Sawbladewidth 100.0
#define Sawbladeheight 25.0
#define Sawbladedepth Sawbladeheight
#define Sawnickwidth (Sawbladewidth / (double) Numsawnicks)
#define Sawnickheight (Sawbladeheight / 2.5)
#define Numsawnicks 15

#define Sawhandlewidth (Sawbladewidth / 2.5)
#define Sawhandleheight Sawbladeheight
#define Sawhandledepth Sawhandleheight

#define Colorbasewidth 120.0
#define Colorbaseheight (Colorbasewidth / 3.0)
#define Colorbasedepth (Colorbasewidth / 2.0)
#define Chipdepth 5.0
#define Colorbarwidth ((Colorbasewidth / 3.0) - Pkboxsize)
#define Colorbarheight 100.0
#define Colorbardepth Colorbasedepth
#define Mincolorbarheight 5.0

#define Coneresolution 10	/* primitives_box stuff */
#define Cylinderesolution 15

#define Primboxoffsetx -400.0
#define Primboxoffsety -50.0
#define Primboxoffsetz -250.0
#define Chairseatheight 15.0
#define Chairlegwidth 15.0
#define Chairbacksupportwidth 10.0
#define Maxlegwidth 12.0	/* used in table leg generation only */
#define Maxlegheight 162.0


#endif
