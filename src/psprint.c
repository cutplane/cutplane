 
/* FILE: psprint.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*                 Postscript Printing Routines for 3Form                 */
/*                                                                        */
/* Copyright (C) 1990,1991,1992 William Kessler and Larry Edwards.        */
/* All Rights Reserved.                                                   */
/*                                                                        */
/* Authors: LJE,WAK                              Date: August 21, 1990    */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define PSPRINTMODULE

#include <config.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <platform_gl.h>

#include <time.h>

#include <string.h>
#include <sys/types.h>

#include <cutplane.h>
#include <math3d.h>

enum
{
  Portrait = 0,
  Landscape,
  Perspective,
  Orthographic
};

static FILE *psfile = NULL;
static int stackargs = 0;
static vertype eyepoint = {0.0,0.0,1.0,1.0};
static int projtype = Orthographic;
static Coord projplanedist;
static vertype lightvector = {0.0,0.0,1.0,0.0};

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                 Low Level Postscript Support Routines                    */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

float
stacknum(void)
{
  stackargs++;
  return (0.0);
}

int
stackint(void)
{
  stackargs++;
  return (0);
}

char *
stackstr(void)
{
  stackargs++;
  return ("");
}

char *
stacksym(void)
{
  stackargs++;
  return ("");
}

void
ps_int(int number)
{
  fprintf(psfile," %d ",number);
}

void
ps_num(float number)
{
  fprintf(psfile," %f ",number);
}

void
ps_sym(char *thesymbol)
{
  fprintf(psfile," /%s ",thesymbol);
}

void
ps_str(char *thestring)
{
  fprintf(psfile," (%s) ",thestring);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                     PS operator Wrapper Routines                         */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

void
ps_add(float x, float y)
{
  if (stackargs == 0)
    fprintf(psfile,"%f %f add\n",x,y);
  else
  {
    if (stackargs == 1)
      ps_num(y);		/* assume 2nd arg is num passed */
    fprintf(psfile,"add\n");
  }
  stackargs = 0;
}

void
ps_arc(float x, float y, float r, float startangle, float stopangle)
{
  if (stackargs == 0)
    fprintf(psfile,"%f %f %f %f %f arc\n",x,y,r,startangle,stopangle);
  else
  {
    switch (stackargs)
    {
    case 1: ps_num(y);
    case 2: ps_num(r);
    case 3: ps_num(startangle);
    case 4: ps_num(stopangle);
    }
    fprintf(psfile,"arc\n");
  }
  stackargs = 0;
}

void
ps_circle(float x, float y, float r) /* this isnt really a ps operator */
{				     /* ...but it should be */
  if (stackargs == 0)
  {
    fprintf(psfile,"%f %f moveto\n",x+r,y);
    fprintf(psfile,"%f %f %f 0 360 arc\n",x,y,r);
  }
  else
  {
    switch (stackargs)
    {
    case 1: ps_num(y);
    case 2: ps_num(r);
    }
    /* make a copy of args to circ and add r to x */
    fprintf(psfile,"3 copy 3 -1 roll add exch\n");
    fprintf(psfile,"moveto\n");
    fprintf(psfile,"0 360 arc\n");
  }
  fprintf(psfile,"closepath\n");
  stackargs = 0;
}

void
ps_closepath(void)
{
  fprintf(psfile,"closepath\n");
}

void
ps_currentpoint(void)
{
  fprintf(psfile,"currentpoint\n");
}

void
ps_def(char *key)
{
  /* for the value param just use whatever's on the stack */

  if (stackargs == 0)
    fprintf(psfile,"/%s exch def\n",key);
  else
    fprintf(psfile,"def\n");
  stackargs = 0;
}

void
ps_div(float x, float y)
{
  if (stackargs == 0)
    fprintf(psfile,"%f %f div\n",x,y);
  else
  {
    if (stackargs == 1)
      ps_num(y);		/* assume 2nd arg is num passed */
    fprintf(psfile,"div\n");
  }
  stackargs = 0;
}

void
ps_dup(void)
{
  fprintf(psfile,"dup\n");
}

void
ps_exch(void)
{
  fprintf(psfile,"exch\n");
}

void
ps_fill(void)
{
  fprintf(psfile,"fill\n");
}

void
ps_findfont(char *fontname)
{
  if (stackargs == 0)
    fprintf(psfile,"/%s findfont\n",fontname);
  else
    fprintf(psfile,"findfont\n");
  stackargs = 0;
}

void
ps_gsave(void)
{
  fprintf(psfile,"gsave\n");
}

void
ps_grestore(void)
{
  fprintf(psfile,"grestore\n");
}

void
ps_idiv(float x, float y)
{
  if (stackargs == 0)
    fprintf(psfile,"%f %f idiv\n",x,y);
  else
  {
    if (stackargs == 1)
      ps_num(y);		/* assume 2nd arg is num passed */
    fprintf(psfile,"idiv\n");
  }
  stackargs = 0;
}

void
ps_initgraphics(void)
{
  if (psfile == NULL)
    system_warning("ps_initgraphics: no output file specified");

  fprintf(psfile,"initgraphics\n");
}

void
ps_initmatrix(void)
{
  fprintf(psfile,"initmatrix\n");
}

void
ps_itransform(float x, float y)
{
  if (stackargs == 0)
    fprintf(psfile,"%f %f itransform\n",x,y);
  else
  {
    if (stackargs == 1)
      ps_num(y);		/* assume 2nd arg is num passed */
    fprintf(psfile,"itransform\n");
  }
  stackargs = 0;
}

void
ps_lineto(float x, float y)
{
  if (stackargs == 0)
    fprintf(psfile,"%f %f lineto\n",x,y);
  else
  {
    if (stackargs == 1)
      ps_num(y);		/* assume 2nd arg is num passed */
    fprintf(psfile,"lineto\n");
  }
  stackargs = 0;
}

void
ps_moveto(float x, float y)
{
  if (stackargs == 0)
    fprintf(psfile,"%f %f moveto\n",x,y);
  else
  {
    if (stackargs == 1)
      ps_num(y);		/* assume 2nd arg is num passed */
    fprintf(psfile,"moveto\n");
  }
  stackargs = 0;
}

void
ps_mod(int x, int y)
{
  if (stackargs == 0)
    fprintf(psfile,"%d %d mod\n",x,y);
  else
  {
    if (stackargs == 1)
      ps_int(y);		/* assume 2nd arg is num passed */
    fprintf(psfile,"mod\n");
  }
  stackargs = 0;
}

void
ps_mul(float x, float y)
{
  if (stackargs == 0)
    fprintf(psfile,"%f %f mul\n",x,y);
  else
  {
    if (stackargs == 1)
      ps_num(y);		/* assume 2nd arg is num passed */
    fprintf(psfile,"mul\n");
  }
  stackargs = 0;
}

void
ps_neg(void)
{
  fprintf(psfile,"neg\n");
}

void
ps_newpath(void)
{
  fprintf(psfile,"newpath\n");
}

void
ps_pop(void)
{
  fprintf(psfile,"pop\n");
}

void
ps_rand(void)
{
  fprintf(psfile,"rand\n");
}

void
ps_rectangle(float xmin, float ymin, float xmax, float ymax)
{ /* this isnt really a ps operator...but it should be */
  if (stackargs == 0)
  {
    fprintf(psfile,"%f %f moveto\n",xmin,ymin);
    fprintf(psfile,"%f %f lineto\n",xmax,ymin);
    fprintf(psfile,"%f %f lineto\n",xmax,ymax);
    fprintf(psfile,"%f %f lineto\n",xmin,ymax);
  }
  else
  {
    switch (stackargs)
    {
    case 1: ps_num(ymin);
    case 2: ps_num(xmax);
    case 3: ps_num(ymax);
    }
    fprintf(psfile,"4 -1 roll 2 copy\n"); /* get xmin,ymin */
    fprintf(psfile,"moveto\n");
    fprintf(psfile,"4 -1 roll dup 5 1 roll exch\n"); /* get xmax,ymin */
    fprintf(psfile,"lineto\n");
    fprintf(psfile,"3 1 roll 2 copy\n"); /* get xmax,ymax */
    fprintf(psfile,"lineto\n");
    fprintf(psfile,"exch pop\n"); /* get xmin,ymax */
    fprintf(psfile,"lineto\n");
  }
  fprintf(psfile,"closepath\n");
  stackargs = 0;
}

void
ps_rlineto(float x, float y)
{
  if (stackargs == 0)
    fprintf(psfile,"%f %f rlineto\n",x,y);
  else
  {
    if (stackargs == 1)
      ps_num(y);		/* assume 2nd arg is num passed */
    fprintf(psfile,"rlineto\n");
  }
  stackargs = 0;
}

void
ps_rmoveto(float dx, float dy)
{
  if (stackargs == 0)
    fprintf(psfile,"%f %f rmoveto\n",dx,dy);
  else
  {
    if (stackargs == 1)
      ps_num(dy);		/* assume 2nd arg is num passed */
    fprintf(psfile,"rmoveto\n");
  }
  stackargs = 0;
}

void
ps_rotate(float angle)
{
  if (stackargs == 0)
    fprintf(psfile,"%f rotate\n",angle);
  else
    fprintf(psfile,"rotate\n");
  stackargs = 0;
}

void
ps_round(float num)
{
  if (stackargs == 0)
    fprintf(psfile,"%f round\n",num);
  else
    fprintf(psfile,"round\n");
  stackargs = 0;
}

void
ps_rrectangle(float xmin, float ymin, float xmax, float ymax)
{ /* this isnt really a ps operator...but it should be */
  switch (stackargs)
  {
  case 0: ps_num(xmin);
  case 1: ps_num(ymin);
  case 2: ps_num(xmax);
  case 3: ps_num(ymax);
  }
  fprintf(psfile,"currentpoint 6 2 roll\n");
  fprintf(psfile,"4 -1 roll 2 copy\n"); /* get xmin,ymin */
  fprintf(psfile,"rmoveto\n");

  fprintf(psfile,"4 -1 roll dup 5 1 roll exch\n"); /* get xmax,ymin */
  /* add current pos to x and y */
  fprintf(psfile,"7 -2 roll 2 copy 9 2 roll\n");
  fprintf(psfile,"exch 4 1 roll add 3 1 roll add exch\n");
  fprintf(psfile,"lineto\n");

  fprintf(psfile,"3 1 roll 2 copy\n"); /* get xmax,ymax */
  fprintf(psfile,"7 -2 roll 2 copy 9 2 roll\n");
  fprintf(psfile,"exch 4 1 roll add 3 1 roll add exch\n");
  fprintf(psfile,"lineto\n");

  fprintf(psfile,"exch pop\n"); /* get xmin,ymax */
  fprintf(psfile,"4 -2 roll\n");
  fprintf(psfile,"exch 4 1 roll add 3 1 roll add exch\n");
  fprintf(psfile,"lineto\n");

  fprintf(psfile,"closepath\n");
  stackargs = 0;
}

void
ps_scale(float xscale, float yscale)
{
  if (stackargs == 0)
    fprintf(psfile,"%f %f scale\n",xscale,yscale);
  else
  {
    if (stackargs == 1)
      ps_num(yscale);		/* assume 2nd arg is num passed */
    fprintf(psfile,"scale\n");
  }
  stackargs = 0;
}

void
ps_scalefont(char *fontname, float scale)
{
  if (stackargs == 0)
    fprintf(psfile,"/%s %f scalefont\n",fontname,scale);
  else
  {
    if (stackargs == 1)
      ps_num(scale);
    fprintf(psfile,"scalefont\n");
  }
  stackargs = 0;
}

void
ps_setfont(char *fontname)
{				/* note: must include front / if dict */
  fprintf(psfile,"%s setfont\n",fontname);
}

void
ps_setgray(float graylevel)
{
  if (stackargs == 0)
    fprintf(psfile,"%f setgray\n",graylevel);
  else
    fprintf(psfile,"setgray\n");
  stackargs = 0;
}

void
ps_setlinejoin(int joinstyle)
{
  if (stackargs == 0)
    fprintf(psfile,"%d setlinejoin\n",joinstyle);
  else
    fprintf(psfile,"setlinejoin\n");
  stackargs = 0;
}

void
ps_setlinewidth(float width)
{
  if (stackargs == 0)
    fprintf(psfile,"%f setlinewidth\n",width);
  else
    fprintf(psfile,"setlinewidth\n");
  stackargs = 0;
}

void
ps_show(char *thestring)
{
  if (stackargs == 0)
    fprintf(psfile,"(%s) show\n",thestring);
  else
    fprintf(psfile,"show\n");
  stackargs = 0;
}

void
ps_showpage(void)
{
  fprintf(psfile,"showpage\n");
}

void
ps_stringwidth(char *thestring)
{
  if (stackargs == 0)
    fprintf(psfile,"(%s) stringwidth\n",thestring);
  else
    fprintf(psfile,"stringwidth\n");
  stackargs = 0;
}

void
ps_stroke(void)
{
  fprintf(psfile,"stroke\n");
}

void
ps_sub(float x, float y)
{
  if (stackargs == 0)
    fprintf(psfile,"%f %f sub\n",x,y);
  else
  {
    if (stackargs == 1)
      ps_num(y);		/* assume 2nd arg is num passed */
    fprintf(psfile,"sub\n");
  }
  stackargs = 0;
}

void
ps_transform(float x, float y)
{
  if (stackargs == 0)
    fprintf(psfile,"%f %f transform\n",x,y);
  else
  {
    if (stackargs == 1)
      ps_num(y);		/* assume 2nd arg is num passed */
    fprintf(psfile,"transform\n");
  }
  stackargs = 0;
}

void
ps_translate(float xpos, float ypos)
{
  if (stackargs == 0)
    fprintf(psfile,"%f %f translate\n",xpos,ypos);
  else
  {
    if (stackargs == 1)
      ps_num(ypos);		/* assume 2nd arg is num passed */
    fprintf(psfile,"translate\n");
  }
  stackargs = 0;
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                    Shell and Feature Plotting Routines                  */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

FILE *
open_psfile(char *filename)
{
  psfile = fopen(filename,"w");
  fprintf(psfile,"%%!\n");
  return (psfile);
}

void
set_psfile(FILE *thefile)
{
  psfile = thefile;
}

void
close_psfile(void)
{
  fclose(psfile);
}

char *
time_stamp(void)
{
  char *timestring;
  time_t thetime;
  ;
  time(&thetime);
  /*  timestring = ctime(&thetime); the way it was, pre windows port */
  timestring = ctime(&thetime);
  timestring[strlen(timestring)-1] = 0; /* get rid of the carriage return */
  return (timestring);
}

void
make_border()
{
  float top = 756.0, right = 576.0; /* 1 in = 72 units in default state */
  float left = 36.0, bottom = 36.0;
  ;
  ps_gsave();

  ps_initgraphics();
  ps_setgray(0.0);
  ps_setlinejoin(1);
  ps_setlinewidth(0.75);

  ps_newpath();
  ps_rectangle(left,bottom,right,top);
  ps_stroke();

  ps_grestore();
}

void
make_title(char *filename, int orientation)
{
  char infostring[256];
  float top = 756.0, right = 576.0; /* 1 in = 72 units in default state */
  float left = 36.0, bottom = 36.0;
  ;
  ps_gsave();

  ps_initgraphics();
  if (orientation == Landscape)
  {
    ps_rotate(90);
    ps_translate(0,-612);
    top = 576.0;
    right = 756.0;
  }
  ps_findfont("Helvetica");
  ps_scalefont(stacksym(),9);
  ps_def("fnt1");
  ps_findfont("Symbol");
  ps_scalefont(stacksym(),9);
  ps_def("fnt2");

  ps_setgray(0.0);
  ps_moveto(left,top+4.0);
  ps_setfont("fnt1");
  ps_show("Image by 3Form");
  ps_setfont("fnt2");
  ps_show("\\344");

  ps_moveto(right,bottom-10.0);
  ps_setfont("fnt1");
  sprintf(infostring,"File: %s    %s",filename,time_stamp());
  ps_str(infostring);
  ps_dup();
  ps_stringwidth(stackstr());
  ps_neg(); ps_exch();
  ps_neg(); ps_exch();
  ps_rmoveto(stacknum(),stacknum());
  ps_show(stackstr());

  ps_grestore();
}

int
page_setup(float xmin, float xmax, float ymin, float ymax,
	   float xmargin, float ymargin)
{
  float imagewidth,imageheight;
  float aspect,scale,space[2];
  float pageaspect,pagewidth,pageheight,temp;
  int orientation = Portrait;
  ;
  pagewidth = (8.5 - 2.0*xmargin)*72.0;
  pageheight = (11.0 - 2.0*ymargin)*72.0;
  pageaspect = pagewidth/pageheight;

  imagewidth = xmax-xmin;
  imageheight = ymax-ymin;
  aspect = imagewidth / imageheight;
  if (imagewidth > imageheight)
  {
    temp = pagewidth;
    pagewidth = pageheight;
    pageheight = temp;
    pageaspect = 1.0/pageaspect;
    ps_rotate(90);
    ps_translate(0,-612);
    orientation = Landscape;
  }
  if (aspect > pageaspect)	/* adjust scale to fit x */
  {
    scale = pagewidth/imagewidth;
    space[0] = (2.0*xmargin*72.0)/scale;
    space[1] = (pageheight+(2.0*ymargin*72.0))/scale - imageheight;
  }
  else
  {				/* adjust scale to fit y */
    scale = pageheight/imageheight;
    space[0] = (pagewidth+(2.0*xmargin*72.0))/scale - imagewidth;
    space[1] = (2.0*ymargin*72.0)/scale;
  }
  ps_scale(scale,scale);
  /* set the origin */
  ps_translate(-xmin,-ymin);
  /* center image */
  ps_translate(space[0]/2.0,space[1]/2.0);

  return (orientation);
}

void
set_projection(vertype eyept, Coord projplanez)
{
  eyepoint[vx] = eyept[vx];
  eyepoint[vy] = eyept[vy];
  eyepoint[vz] = eyept[vz];
  eyepoint[vw] = eyept[vw];
  /* for now we just look straight down a line parallel to z-axis */
  if (eyept[vw] == 0.0)
    projtype = Orthographic;
  else
  {
    projplanedist = projplanez - eyept[vz];
    projtype = Perspective;
  }
}

void
set_lighting(vertype lightvec)
{
  copyvec3d(lightvec,lightvector);
}

void
print_psfile(char *filename)
{
  char cmdstring[80];
  ;
  ps_showpage();
  fflush(psfile);
#ifndef IRIS
  sprintf(cmdstring,"lpr %s",filename);
  system(cmdstring);
#endif
}

void
plot_fve(fveptr thisfve)
{
  leptr firstle,thisle;
  boundptr thisbound;
  Coord costh;
  vertype eyevec, facepoint, relpos;
  colortype facecolor;
  float grayequiv;
  ;
  thisbound = thisfve->boundlist->first.bnd;

  /* construct vector from eye to point on face */
  copypos3d(thisbound->lelist->first.le->facevert->pos,facepoint);
  diffpos3d(facepoint,eyepoint,eyevec);
  normalize(eyevec,eyevec);

  costh = dotvecs(eyevec,thisfve->facenorm);

  if (costh < 0.0)		/* if face points away dont draw it */
    return;

  costh = dotvecs(lightvector,thisfve->facenorm);
  if (costh < 0.0)
    costh = 0.0;

  get_feature_color((featureptr) thisfve,facecolor);
  grayequiv = (0.3 * (float) facecolor[0] + 0.59 * (float) facecolor[1] +
              0.11 * (float) facecolor[2]) / 255.0;

  costh = grayequiv*costh + 0.5; /* add some ambient light also */
  if (costh > 1.0)
    costh = 1.0;

  ps_setgray(0.0);
  ps_newpath();
  while (thisbound)
  {
    firstle = thisbound->lelist->first.le;
    diffpos3d(eyepoint,firstle->facevert->pos,relpos);
    if (projtype == Perspective)
      ps_moveto(relpos[vx]*projplanedist/relpos[vz],
		relpos[vy]*projplanedist/relpos[vz]);
    else
      ps_moveto(relpos[vx],relpos[vy]);
    thisle = firstle->next;
    do
    {
      diffpos3d(eyepoint,thisle->facevert->pos,relpos);
      if (projtype == Perspective)
	ps_lineto(relpos[vx]*projplanedist/relpos[vz],
		  relpos[vy]*projplanedist/relpos[vz]);
      else
	ps_lineto(relpos[vx],relpos[vy]);
      thisle = thisle->next;
    } while (thisle != firstle);
    ps_closepath();
    thisbound = thisbound->next;
  }
  ps_gsave();
  ps_stroke();
  ps_grestore();
  ps_setgray(costh);
  ps_fill();
}

void
plot_fve_outline(fveptr thisfve)
{
  leptr firstle,thisle;
  boundptr thisbound;
  Coord costh;
  vertype eyevec,facepoint,relpos;
  ;
  thisbound = thisfve->boundlist->first.bnd;

  /* make vector from eye position to a point on the face */
  copypos3d(thisbound->lelist->first.le->facevert->pos,facepoint);
  diffpos3d(facepoint,eyepoint,eyevec);
  normalize(eyevec,eyevec);

  /* take dot product with face normal */
  costh = dotvecs(eyevec,thisfve->facenorm);

  if (costh < 0.0)		/* if face points away dont draw it */
    return;

  ps_setgray(0.0);
  ps_newpath();
  while (thisbound)
  {
    firstle = thisbound->lelist->first.le;
    diffpos3d(eyepoint,firstle->facevert->pos,relpos);
    if (projtype == Perspective)
      ps_moveto(relpos[vx]*projplanedist/relpos[vz],
		relpos[vy]*projplanedist/relpos[vz]);
    else
      ps_moveto(relpos[vx],relpos[vy]);
    thisle = firstle->next;
    do
    {
      diffpos3d(eyepoint,thisle->facevert->pos,relpos);
      if (projtype == Perspective)
	ps_lineto(relpos[vx]*projplanedist/relpos[vz],
		  relpos[vy]*projplanedist/relpos[vz]);
      else
	ps_lineto(relpos[vx],relpos[vy]);
      thisle = thisle->next;
    } while (thisle != firstle);
    ps_closepath();
    thisbound = thisbound->next;
  }
  ps_stroke();
}

void
plot_shell(shellptr thishell)
{
  fveptr thisfve;
  ;
  thisfve = thishell->fvelist->first.fve;
  while (thisfve)
  {
    plot_fve(thisfve);
    /*
    plot_fve_outline(thisfve);
    */
    thisfve = thisfve->next;
  }
}

void
plot_object(objptr thisobj, char *filename)
{
  shellptr thishell;
  int orientation;
  static vertype lightvec = {0.0,0.906307787,0.422618262,0.0};
  static vertype eyept = {0.0,0.0,4000.0,1.0};
  ;
  open_psfile(filename);
  ps_initgraphics();
  orientation = page_setup(-512.0,511.0,-384.0,383.0,0.5,0.5);
  set_projection(eyept,0.0);
  set_lighting(lightvec);
  make_border();
  make_title(filename,orientation);
  transform_obj(thisobj);
  find_obj_normals(thisobj);
  thishell = thisobj->shellist->first.shell;
  while (thishell)
  {
    plot_shell(thishell);
    thishell = thishell->next;
  }
  print_psfile(filename);
  close_psfile();
}

void
sort_objects(listptr objlist, int axis)
{
  listptr tempobjlist = create_list();
  objptr thisobj,sortobj;
  ;
  sortobj = objlist->first.obj;
  while (sortobj)
  {
    rem_elem(objlist, (elemptr) sortobj);
    thisobj = tempobjlist->first.obj;
    while (thisobj)
    {
      if (sortobj->objbbox.maxcorner[axis] < thisobj->objbbox.maxcorner[axis])
      {
	insert_elem(tempobjlist, (elemptr) thisobj, (elemptr) sortobj);
	break;
      }
      thisobj = thisobj->next;
    }
    if (thisobj == Nil)
      append_elem(tempobjlist, (elemptr) sortobj);
    sortobj = objlist->first.obj;
  }
  merge_lists(tempobjlist,objlist);
}

void
plot_obj_branch_engine(objptr rootobj)
{
  shellptr thishell;
  objptr thischild;
  ;
  if ((rootobj->children->numelems == 0) &&
      (!has_property((featureptr) rootobj, group_prop)))
  {
    transform_obj(rootobj);
    find_obj_normals(rootobj);
    thishell = rootobj->shellist->first.shell;
    while (thishell)
    {
      plot_shell(thishell);
      thishell = thishell->next;
    }
  }
  else
  {
    sort_objects(rootobj->children,vz);
    thischild = rootobj->children->first.obj;
    while (thischild)
    {
      plot_obj_branch_engine(thischild);
      thischild = thischild->next;
    }
  }
}

void
plot_obj_branch(objptr rootobj, char *filename)
{
  int orientation;
  static vertype lightvec = {0.0,0.906307787,0.422618262,0.0};
  static vertype eyept = {0.0,0.0,4000.0,1.0};
  ;
  open_psfile(filename);
  ps_initgraphics();
  orientation = page_setup(-512.0,511.0,-384.0,383.0,0.5,0.5);
  set_projection(eyept,0.0);
  set_lighting(lightvec);
  make_border();
  make_title(filename,orientation);
  ps_setlinejoin(1);
  plot_obj_branch_engine(rootobj);
  print_psfile(filename);
  close_psfile();
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/* The following routines are used to plot faces for debugging            */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

void
label_null_le(float x, float y, char *label1, char *label2,
		  int orientation)
{
  char combolabel[50];
  ;
  sprintf(combolabel,"%s %s",label1,label2);
  ps_transform(x,y);
  ps_gsave();
  ps_initmatrix();
  if (orientation == Landscape)
    ps_rotate(90);
  ps_itransform(stacknum(),stacknum());
  ps_newpath();
  ps_moveto(stacknum(),stacknum());
  ps_currentpoint();
  ps_rmoveto(6.0,-3.0);
  ps_show(combolabel);
  ps_moveto(stacknum(),stacknum());
  ps_rrectangle(-5.0,-5.0,5.0,5.0);
  ps_stroke();
  ps_grestore();
}

void
label_null_strut(float x, float y, char *label1, char *label2, char *label3,
		  int orientation)
{
  char combolabel[50];
  ;
  sprintf(combolabel,"%s %s %s",label1,label2,label3);
  ps_transform(x,y);
  ps_gsave();
  ps_initmatrix();
  if (orientation == Landscape)
    ps_rotate(90);
  ps_itransform(stacknum(),stacknum());
  ps_newpath();
  ps_moveto(stacknum(),stacknum());
  ps_currentpoint();
  ps_rmoveto(6.0,-3.0);
  ps_show(combolabel);
  ps_moveto(stacknum(),stacknum());
  ps_rrectangle(-5.0,-5.0,5.0,5.0);
  ps_stroke();
  ps_grestore();
}

void
label_le(float x, float y, char *thelabel, int orientation)
{
  static Boolean firstime = TRUE;
  unsigned int seed;
  int ranum;
  static char state[256];
  ;
  if (firstime)
  {
    seed = time((time_t *) 0);
    printf("\nSeed for random number generator = 0x%08X\n",seed);
#ifdef IRIS
    initstate(seed,state,256);
    setstate(state);
#endif
    firstime = FALSE;
  }
#ifdef IRIS
  ranum = -(random() & 0x1);
#endif
  ps_transform(x,y);
  ps_gsave();
  ps_initmatrix();
  if (orientation == Landscape)
    ps_rotate(90);
  ps_itransform(stacknum(),stacknum());
  ps_moveto(stacknum(),stacknum());
  ps_stringwidth(thelabel);
  ps_pop();			/* get rid of y-comp */

  ps_mul(stacknum(),ranum); /* x comp of offset */

#ifdef IRIS
  ranum = random() & 0x1;
#endif

  ps_rmoveto(stacknum(),-9.0*ranum);

  ps_show(thelabel);
  ps_grestore();
}

void
make_axes(int axis1, int axis2, int orientation)
{
  static char *axislabel[3] = {"X","Y","Z"};
  float top = 756.0, right = 576.0; /* 1 in = 72 units in default state */
  float left = 36.0, bottom = 36.0;
  ;
  ps_gsave();
  ps_initmatrix();
  ps_newpath();
  if (orientation == Landscape)
  {
    ps_rotate(90);
    ps_translate(0,-612);
    top = 576.0;
    right = 756.0;
  }
  if (axis2 == vz)
  {
    ps_moveto(left+36.0,top-36.0);
    ps_rlineto(36.0,0.0);	/* axis1 */
    ps_rmoveto(6.0,0.0);
    ps_show(axislabel[axis1]);
    ps_moveto(left+36.0,top-36.0);
    ps_rlineto(0.0,-36.0);	/* axis2 */
    ps_rmoveto(0.0,-6.0);
    ps_show(axislabel[axis2]);
  }
  else
  {
    ps_moveto(left+36.0,bottom+36.0);
    ps_rlineto(36.0,0.0);	/* axis1 */
    ps_rmoveto(6.0,0.0);
    ps_show(axislabel[axis1]);
    ps_moveto(left+36.0,bottom+36.0);
    ps_rlineto(0.0,36.0);	/* axis2 */
    ps_rmoveto(0.0,6.0);
    ps_show(axislabel[axis2]);
  }
  ps_stroke();
  ps_grestore();
}

void
itoa(int num, char *numstring)
{
  sprintf(numstring,"%d",num);
}

Boolean
le_isnull(leptr thisle, int axis1, int axis2)
{
  Coord *thispos,*nextpos;
  ;
  thispos = thisle->facevert->pos;
  nextpos = thisle->next->facevert->pos;
  return ((thispos[axis1] == nextpos[axis1]) &&
	  (thispos[axis2] == nextpos[axis2]));
}

int
get_le_vfeid(leptr thisle)
{
  shellptr thishell;
  thishell = thisle->motherbound->motherfve->mothershell;

  return (get_elem_index((elemptr) thisle->facevert,thishell->vfelist));
}

void
plotfaceles(fveptr thisfve, char *filename)
{
  leptr xmaxle,xminle,ymaxle,yminle,firstle,thisle,prevle;
  boundptr thisbound;
  vertype minpos,maxpos;
  float *thispos;
  int orientation,axis1,axis2;
  char idstring1[20],idstring2[20],idstring3[20];
  shellptr thishell;
  ;
  if (filename == Nil)
    filename = "debug.ps";
  thishell = thisfve->mothershell;
  open_psfile(filename);
  ps_initgraphics();
  ps_setlinewidth(0.0);
  ps_setgray(0.0);
  ps_setlinejoin(1);

  thisbound = thisfve->boundlist->first.bnd;
  pickplane(thisfve->facenorm, &axis1, &axis2);
  xmaxle = get_lemax(axis1,thisbound);
  xminle = get_lemin(axis1,thisbound);
  ymaxle = get_lemax(axis2,thisbound);
  yminle = get_lemin(axis2,thisbound);
  minpos[axis1] = xminle->facevert->pos[axis1] - 10.0;
  minpos[axis2] = yminle->facevert->pos[axis2] - 10.0;
  maxpos[axis1] = xmaxle->facevert->pos[axis1] + 10.0;
  maxpos[axis2] = ymaxle->facevert->pos[axis2] + 10.0;

  orientation = page_setup(minpos[axis1],maxpos[axis1],
			   minpos[axis2],maxpos[axis2],0.5,0.5);

  make_border();
  make_title(filename,orientation);

  ps_findfont("Helvetica");
  ps_scalefont(stacksym(),9);
  ps_def("labelfont");
  ps_setfont("labelfont");

  make_axes(axis1,axis2,orientation);

  ps_newpath();
  while (thisbound)
  {
    thisle = firstle = thisbound->lelist->first.le;
    do
    {
      thispos = thisle->facevert->pos;
      if (thisle == firstle)
	ps_moveto(thispos[axis1],thispos[axis2]);
      else
	ps_lineto(thispos[axis1],thispos[axis2]);
      itoa(get_le_vfeid(thisle),idstring1);
      if (le_isnull(thisle,axis1,axis2))
      {
	thisle = thisle->next;
	itoa(get_le_vfeid(thisle),idstring2);
	if ((thisle->prev != thisle->next) && le_isnull(thisle,axis1,axis2))
	{
	  thisle = thisle->next;
	  itoa(get_le_vfeid(thisle),idstring3);
	  label_null_strut(thispos[axis1],thispos[axis2],idstring1,idstring2,
			   idstring3,orientation);
	  if ((thisle == firstle) || (thisle->prev == firstle)) break;
	}
	else
	{
	  label_null_le(thispos[axis1],thispos[axis2],idstring1,idstring2,
			orientation);
	  if (thisle == firstle) break;
	}
      }
      else if ((thisle != firstle) || !le_isnull(thisle->prev,axis1,axis2))
	label_le(thispos[axis1],thispos[axis2],idstring1,orientation);
      thisle = thisle->next;
    } while (thisle != firstle);
    ps_closepath();
    thisbound = thisbound->next;
  }
  ps_stroke();

  print_psfile(filename);
  close_psfile();
}
