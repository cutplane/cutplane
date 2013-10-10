
/* FILE: color.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* Der Newen Improven Droewen/Displayenspiel Rutinnen fur ausch Cutplagen */
/*                          Editeriat,yah yah.                            */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define COLORMODULE

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <platform_gl.h>
#include <math.h>

#ifndef IRIS
#include <string.h>
#endif

#include <cutplane.h> /* includes color.h */

#include <globals.h>

  unsigned long
packcolor(colortype thecolor)
{
  return (((unsigned long) thecolor[0]) |
	  (((unsigned long) thecolor[1]) << 8) |
	  (((unsigned long) thecolor[2]) << 16));
}

  void
unpackcolor(unsigned long packedcolor, colortype unpackedcolor)
{
  unpackedcolor[0] = packedcolor & 0xFF;
  unpackedcolor[1] = ((unsigned long) (packedcolor & 0xFF00)) >> 8;
  unpackedcolor[2] = ((unsigned long) (packedcolor & 0xFF0000)) >> 16;
}

  void
copycolor(colortype sourcecolor, colortype destcolor)
{
  destcolor[0] = sourcecolor[0];
  destcolor[1] = sourcecolor[1];
  destcolor[2] = sourcecolor[2];
}

  void
scalecolor(colortype thecolor, float scalefactor, colortype result)
{
  result[0] = enforce_ilimits(thecolor[0]*scalefactor,0,255);
  result[1] = enforce_ilimits(thecolor[1]*scalefactor,0,255);
  result[2] = enforce_ilimits(thecolor[2]*scalefactor,0,255);
}

  void
addcolors(colortype color1, colortype color2, colortype resultcolor)
{
  resultcolor[0] = color1[0]+color2[0];
  resultcolor[1] = color1[1]+color2[1];
  resultcolor[2] = color1[2]+color2[2];
}

  void
multcolors(colortype color1, colortype color2, colortype resultcolor)
{
  resultcolor[0] = color1[0]*color2[0];
  resultcolor[1] = color1[1]*color2[1];
  resultcolor[2] = color1[2]*color2[2];
}

  void
set_feature_color(featureptr thisfeature, colortype thiscolor)
{
  if (thisfeature->thecolor == Nil)
    thisfeature->thecolor = (colorptr) malloc(sizeof(colortype));
  copycolor(thiscolor,thisfeature->thecolor);
}

  void
get_feature_color(featureptr thisfeature, colortype thiscolor)
{
  featureptr motherfeature;
  ;
  if (thisfeature->thecolor == Nil)
  {
    motherfeature = find_largergrain(thisfeature,Obj_type);
    if (motherfeature->thecolor == Nil)
      copycolor(lightgrey,thiscolor);
    else
      copycolor(motherfeature->thecolor,thiscolor);
  }
  else
    copycolor(thisfeature->thecolor,thiscolor);
}

  void
copy_feature_color(featureptr sourcefeature, featureptr destfeature)
{
  if (destfeature->thecolor == Nil)
    destfeature->thecolor = (colorptr) malloc(sizeof(colortype));

  if (sourcefeature->thecolor == Nil)
    copycolor(lightgrey,destfeature->thecolor);
  else
    copycolor(sourcefeature->thecolor,destfeature->thecolor);
}

void
get_named_color(char *colorname, colortype rgbval)
{
  static char *colornames[] = {"Grey","Yellow","Purple","Green","Blue","Orange",
				 "Red"};
  static colorptr colorvals[] = {mediumgrey,yellow,magenta,green,blue,orange,
				   red};
  int i;
  ;
  for (i=0; i < sizeof(colorvals)/sizeof(colorptr); i++)
  {
#ifdef IRIS
    if (strcasecmp(colorname,colornames[i]) == 0)
    {
      copycolor(colorvals[i],rgbval);
      return;
    }
#else
#ifdef WIN32
    if (stricmp(colorname,colornames[i]) == 0)
#else
    if (strcasecmp(colorname,colornames[i]) == 0)
#endif
    {
      copycolor(colorvals[i],rgbval);
      return;
    }
#endif
  }
  copycolor(lightgrey,rgbval);
}

void
set_feature_named_color(featureptr thisfeature, char *colorname)
{
  colortype thiscolor;
  ;
  get_named_color(colorname,thiscolor);
  if (thisfeature->thecolor == Nil)
    thisfeature->thecolor = (colorptr) malloc(sizeof(colortype));
  copycolor(thiscolor,thisfeature->thecolor);
}

