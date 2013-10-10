
/* FILE: arrange.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* Der Newen Improven arranging routines                                  */
/*                                                                        */
/* Authors: WAK                                   Date: OCTOBER 12, 1990  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define ARRANGEMODULE

#include <config.h>
#include <stdio.h>

#include <platform_gl.h>

#include <cutplane.h>

#include <grafx.h>
#include <globals.h>
#include <intersect.h>


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*             AUTOMATIC OBJECT POSITIONING (CLEANUP) ROUTINES              */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


  static Coord
calc_partition(nobjsaxis,totalobjs,roomsize)
  int nobjsaxis,totalobjs;
  Coord roomsize;
{
  if (nobjsaxis > totalobjs)
    return( roomsize / (Coord) totalobjs);
  else
   return( roomsize / (Coord) nobjsaxis);
}

#ifdef WILLHASFIXEDMEUP

  void
cleanup_objs(obj_list)
  listptr obj_list;
{
  int i,nobjs,thisxpos,thisypos,thiszpos;
  /* number of objs that can be placed in each direction */
  int nobjsx,nobjsy,nobjsz;
  /* largest bbox values of all objs */
  static vertype largest = {0.0,0.0,0.0,1.0};
  Coord newval,xpartition,ypartition,zpartition;
  vertype newobjpos;
  objptr thisobj;
  featureptr thisfeature;
  property thisprop;
  ;
  if ((nobjs = obj_list->numelems) == 1)
    return;			/* If only one obj, don't worry, be happy */

  firstprop = thisprop = world->world_proplists[(int) visible_prop]->first.prop;
  while (thisprop != Nil)
  {
    thisfeature = thisprop->owner;
    if (thisfeature->type_id == Obj_type)
    {
      thisobj = (objptr) thisfeature;
      for (i = 0; i < 3; ++i)
      {
	newval = (Coord) fabs(thisobj->objbbox.mincorner[i]
			      - thisobj->objbbox.mincorner[i]);
	if (newval > largest[i])
	  largest[i] = newval;
      }
    }
    thisprop = thisprop->next;
  }
  
  nobjsx = Cutsizex / largest[vx]; /* Note that it truncates */
  nobjsy = Cutsizey / largest[vy];
  nobjsz = Cutsizez*2.0 / largest[vz];
  xpartition = calc_partition(nobjsx,nobjs,Cutsizex);
  ypartition = calc_partition(nobjsy,nobjs,Cutsizey);
  zpartition = calc_partition(nobjsz,nobjs,Cutsizez*2.0);
  if (nobjsx >= nobjs)
  {
    ypartition = Cutsizey;	/* If they all can fit on one line, center */
    zpartition = Cutsizez*2.0;	/* in y and z. */
  }
  else if (nobjsx * nobjsy >= nobjs) /* If all in one plane, center in z */
    zpartition = Cutsizez*2.0;
  
  thisobj = obj_list->first.obj;
  /* Fill in objs, objs that cannot fit end up at origin, sorry!*/
  for (thiszpos = 0;((thisobj != Nil) && (thiszpos < nobjsz)); ++thiszpos)
    for (thisypos = 0; ((thisobj != Nil) && (thisypos < nobjsy)); ++thisypos)
      for (thisxpos = 0; ((thisobj != Nil) && (thisxpos < nobjsx));
	   ++thisxpos,thisobj = thisobj->next)
      {
	newobjpos[vx] = ((Coord) thisxpos * xpartition) +
	  (xpartition/2.0) - (Cutsizex / 2.0);
	newobjpos[vy] = ((Coord) thisypos * ypartition) +
	  (ypartition/2.0) - (Cutsizey / 2.0);
	newobjpos[vz] = ((Coord) thiszpos * zpartition) +
	  (zpartition/2.0) - (Cutsizez);
	move_obj(thisobj,newobjpos);
	set_3d_objbbox(thisobj);	/* reset the bounding box */
      }
  if (thisobj)
  {
    newobjpos[vx] = newobjpos[vy] = 0.0;
    newobjpos[vz] = -Cutsizez - zpartition / 2.0;
    while (thisobj != Nil) /*move unfitted objs into the outback,mate! */
    {
      move_obj(thisobj,newobjpos);
      set_3d_objbbox(thisobj);	/* reset bounding box */
      newobjpos[vz] -= zpartition;
      thisobj = thisobj->next;
    }
  }
}

#endif /* WILLHASFIXEDMEUP */

/* Now need to decide: if filling in objs in x dir, then y dir, then zdir,*/
/* when do you begin filling, and what order, the middle outwards? Probably, */
/* if one obj, put in middle, if more, spread out in each direction, */
/* starting with middle row and moving outward in each row. This means */
/* fill an x row, move out on y and fill both x rows, if out of room, move */
/* out in z, in both dirs, and repeat process. */
/* Sounds like 3d life game to me!*/


