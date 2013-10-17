
/* FILE: intersect.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*        Section Line Generation Routines for the Cutplane Editor        */
/*                                                                        */
/* Authors: LJE and WAK                           Date: September 23,1987 */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define INTERSECTMODULE

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include <platform_gl.h>
#include <float.h>

#include <cutplane.h>

#include <intersect.h>
#include <pick.h>		/* for Pkboxsize */

#define next_elem_macro(listptr,thisptr)\
((thisptr->next == Nil) ? thisptr = thisptr->next : thisptr = header->first.any)

#define prev_elem_macro(listptr,thisptr)\
((thisptr->prev == Nil) ? thisptr = thisptr->prev : thisptr = header->first.any)

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                       EDGE INTERSECTION ROUTINES                         */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  static void
set_evf_eqn(evfptr thisevf)
{				/* compute equation of line seg and store */
  Coord zdiff;
  register float *vertptr1,*vertptr2;
  ;
  vertptr1 = thisevf->le1->facevert->pos;
  vertptr2 = thisevf->le2->facevert->pos;
  thisevf->xdiff = vertptr2[vx] - vertptr1[vx];
  thisevf->ydiff = vertptr2[vy] - vertptr1[vy];

  zdiff = vertptr2[vz] - vertptr1[vz];
  if (zdiff != 0.0)
    thisevf->zc1 = 1.0 / zdiff;
  else
    thisevf->zc1 = 0.0;		/* this won't be used in cut_evf */

  thisevf->zc2 = vertptr1[vz] * thisevf->zc1;
}

  void
set_obj_evf_eqns(objptr thisobj)
{
  shellptr thishell;
  evfptr thisevf;
  ;
  for (thishell = First_obj_shell(thisobj); thishell != Nil;
       thishell = thishell->next)
    for (thisevf = thishell->evflist->first.evf; thisevf != Nil;
	 thisevf = thisevf->next)
      set_evf_eqn(thisevf);
}


  static short
set_evf_cutpt(evfptr thisevf, Coord zcut)
{
  float t,zdist1,zdist2;
  float *pt1,*pt2;
  Boolean pluscut,minuscut, endpt1cut = FALSE, endpt2cut = FALSE;
  ;
  pt1 = thisevf->le1->facevert->pos;
  pt2 = thisevf->le2->facevert->pos;
  endpt1cut = (Fabs(pt1[vz] - zcut) < Tolerance);
  endpt2cut = (Fabs(pt2[vz] - zcut) < Tolerance);
  if (endpt1cut && endpt2cut)
  {				/* intersected endpoint picked arbitrarily */
    thisevf->cutpt[vx] = pt1[vx];
    thisevf->cutpt[vy] = pt1[vy];
    thisevf->cutpt[vz] = pt1[vz];
    thisevf->cutpt[vw] = 1.0;
    transformpt(thisevf->cutpt,vanillamatrix,thisevf->local_cutpt);
    return(Oncut);     /* edge on the cutplane */
  }
  if (endpt1cut)
  {
    thisevf->cutpt[vx] = pt1[vx];
    thisevf->cutpt[vy] = pt1[vy];
    thisevf->cutpt[vz] = pt1[vz];
    thisevf->cutpt[vw] = 1.0;
    transformpt(thisevf->cutpt,vanillamatrix,thisevf->local_cutpt);
    if (pt2[vz] > zcut)		/* edge is in front of plane */
      return(Pt1cut);    /* first endpt on the cutplane */
    else
      return(-Pt1cut);    /* first endpt on the cutplane */
  }
  if (endpt2cut)
  {
    thisevf->cutpt[vx] = pt2[vx];
    thisevf->cutpt[vy] = pt2[vy];
    thisevf->cutpt[vz] = pt2[vz];
    thisevf->cutpt[vw] = 1.0;
    transformpt(thisevf->cutpt,vanillamatrix,thisevf->local_cutpt);
    if (pt1[vz] > zcut)		/* edge is in front of plane */
      return(Pt2cut);    /* second endpt on the cutplane */
    else
      return(-Pt2cut);    /* second endpt on the cutplane */
  }
  pluscut = ((pt1[vz] > zcut) && (pt2[vz] < zcut)); /* edge goes front to back */
  minuscut = ((pt2[vz] > zcut) && (pt1[vz] < zcut));
  if (pluscut || minuscut) /* it was cut */
  {                    
    t = zcut * thisevf->zc1 - thisevf->zc2;
    thisevf->cutpt[vx] = pt1[vx] + t * thisevf->xdiff;
    thisevf->cutpt[vy] = pt1[vy] + t * thisevf->ydiff;
    thisevf->cutpt[vz] = zcut;
    thisevf->cutpt[vw] = 1.0;
    transformpt(thisevf->cutpt,vanillamatrix,thisevf->local_cutpt);
    return (Wascut * (pluscut-minuscut));
  }
  if (pt1[vz] > zcut)
    return(Nocut);        /* edge does not intersect plane, is in front */
  return(-Nocut);         /* edge does not intersect plane, is in back */
}

  void
set_shell_evf_cutpts(shellptr thishell, stateptr state)
{
  vertype dummy;
  evfptr thisevf;
  objptr motherobj;
  void *mother_invxform;
  ;
  motherobj = thishell->motherobj;
  mother_invxform = motherobj->invxform;
  glPushMatrix();
  glLoadMatrixf((GLfloat *) mother_invxform);
  thisevf = thishell->evflist->first.evf;
  while(thisevf)
  {
    thisevf->cutype = set_evf_cutpt(thisevf,state->cursor[vz]);
    thisevf = thisevf->next;
  }
  glPopMatrix();
}

  void
set_obj_evf_cutpts(objptr thisobj,stateptr state)
{
  vertype dummy;
  evfptr thisevf;
  void *mother_invxform;
  shellptr thishell;
  ;
  mother_invxform = thisobj->invxform;
  glPushMatrix();
  glLoadMatrixf((GLfloat *) mother_invxform);
  for (thishell = First_obj_shell(thisobj); thishell != Nil;
       thishell = thishell->next)
  {
    thisevf = thishell->evflist->first.evf;
    while(thisevf)
    {
      thisevf->cutype = set_evf_cutpt(thisevf,state->cursor[vz]);
      thisevf = thisevf->next;
    }
  }
  glPopMatrix();
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                      SEGMENT SORTING ROUTINES                          */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  static void
sorted_insert(listptr sortedlist, leptr thisle, float component)
{
  sortptr thisort,newsort;
  ;
  newsort = (sortptr) create_blank_elem(Sort_type);
  newsort->sortelem = (elemptr) thisle;
  newsort->sortdata1 = (alltypes *) alloc_elem(Allsize);
  newsort->sortdata1->f = component;

  thisort = sortedlist->first.sort;
  /* insert a new element before any element in the list that it is <= to */
  /* so the first element in the list will be the smallest */
  while ((thisort != Nil) && (component > thisort->sortdata1->f))
    thisort = thisort->next;
  insert_elem((listptr) sortedlist,(elemptr) thisort,(elemptr) newsort);
}

void
prune_edgelist(listptr edgelist)
{
  evfptr thisevf,nextevf;
  leptr thisle,nextle;
  sortptr thisort,nextsort;
  ;
  thisort = edgelist->first.sort;
  while (thisort != Nil)
  {
    nextsort = thisort->next;
    if (nextsort != Nil)
    {
      thisle = (leptr) thisort->sortelem;
      thisevf = thisle->facedge;
      nextle = (leptr) nextsort->sortelem;
      nextevf = nextle->facedge;
      /* remove extra Ptcut edges */
      if ((abs(thisevf->cutype) != Wascut) && (abs(nextevf->cutype) != Wascut))
	if (((thisle->next == nextle) || (thisle->prev == nextle)) &&
	    ((thisevf->cutype * nextevf->cutype) < 0))
	{  /* only remove those that cross plane ^ */
	  if (nextsort != Nil)
	    nextsort = nextsort->next;
	  if (thisevf->cutype < 0)
	    del_elem(edgelist, (elemptr) thisort);
	  else
	    del_elem(edgelist, (elemptr) thisort->next);
	}
    }
    thisort = nextsort;
  }
}
  static void
sort_cut_edges(fveptr thisfve, listptr edgelist)
{
  leptr thisle,firstle;
  boundptr thisbound;
  evfptr thisevf;
  int axis;
  ;
  if (fabs(thisfve->facenorm[vx]) > fabs(thisfve->facenorm[vy]))
    axis = vy;
  else
    axis = vx;
  thisbound = thisfve->boundlist->first.bnd;
  while(thisbound)
  {
    firstle = thisbound->lelist->first.le;
    thisle = firstle;
    do
    {
      if (thisle->motherbound != Twin_le(thisle)->motherbound)
      {				/* ignore struts */
	thisevf = thisle->facedge;
	if (abs(thisevf->cutype) < Oncut)
	  sorted_insert(edgelist,thisle,thisevf->cutpt[axis]);
      }
      thisle = thisle->next;
    } while (thisle != firstle);
    thisbound = thisbound->next;
  }
  prune_edgelist(edgelist);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                      SECTION LINE ROUTINES                             */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  static Boolean
face_in_plane(fveptr thisfve)
{
  leptr thisle,firstle;
  ;
  thisle = firstle = thisfve->boundlist->first.bnd->lelist->first.le;
  do
  {
    if (thisle->facedge->cutype != Oncut)
      return (FALSE);
    thisle = thisle->next;
  } while (thisle != firstle);
  return (TRUE);
}

  static void
expand_boundingbox(bbox2dptr thisbbox, Coord x, Coord y)
{				/* don't forget, right handed world! */
  if (thisbbox->mincorner[vx] > x)
    thisbbox->mincorner[vx] = x;
  if (thisbbox->maxcorner[vx] < x)
    thisbbox->maxcorner[vx] = x;
  if (thisbbox->maxcorner[vy] < y)
    thisbbox->maxcorner[vy] = y;
  if (thisbbox->mincorner[vy] > y)
    thisbbox->mincorner[vy] = y;
}

  static void
set_ringbbox(ringptr thisring)
{
  segmentptr thiseg;
  ;
  /* initialize bbox of ring to impossibly small values */
  thisring->ringbbox.mincorner[vx] = thisring->ringbbox.mincorner[vy] =
    FLT_MAX;
  thisring->ringbbox.maxcorner[vx] = thisring->ringbbox.maxcorner[vy] =
    -FLT_MAX;

  thiseg = thisring->seglist->first.seg;
  while (thiseg != Nil)
  {
    if (thiseg->kind == Fve_inplane)
    {
      expand_boundingbox((&(thisring->ringbbox)),
		  thiseg->segbbox.mincorner[vx],thiseg->segbbox.maxcorner[vy]);
      expand_boundingbox((&(thisring->ringbbox)),
		  thiseg->segbbox.maxcorner[vx],thiseg->segbbox.mincorner[vy]);
    }
    else
    {
      expand_boundingbox((&(thisring->ringbbox)),
			 thiseg->pt[0][vx],thiseg->pt[0][vy]);
      expand_boundingbox((&(thisring->ringbbox)),
			 thiseg->pt[1][vx],thiseg->pt[1][vy]);
    }
    thiseg = thiseg->next;
  }
}

  static void
set_inplanebbox(segmentptr newseg)
{
  leptr firstle,thisle;
  ;
  newseg->segbbox.mincorner[vx] = newseg->segbbox.mincorner[vy] = FLT_MAX;
  newseg->segbbox.maxcorner[vx] = newseg->segbbox.maxcorner[vy] = -FLT_MAX;
  firstle = thisle = newseg->le[0];
  do
  {
    expand_boundingbox((&(newseg->segbbox)),
		thisle->facevert->pos[vx],thisle->facevert->pos[vy]);
    expand_boundingbox((&(newseg->segbbox)),
		(Twin_le(thisle))->facevert->pos[vx],
		(Twin_le(thisle))->facevert->pos[vy]);
    thisle = thisle->next;
  } while (thisle != firstle);	/* note walking first bound since it's outer */
}

  void
add_segment(listptr seglist, leptr le1, leptr le2)
{
  segmentptr newseg;
  evfptr evf1,evf2;
  ;
  newseg = (segmentptr) append_new_blank_elem((listptr) seglist,Seg_type);
  if (le2 == Nil)		/* this is an inplane face */
  {
    newseg->kind = Fve_inplane;
    newseg->le[0] = le1;
    newseg->le[1] = le1->next;
    set_inplanebbox(newseg);
    newseg->pt[0] = newseg->pt[1] = Nil;
    notify_cursor_pause();
  }
  else if (le1 == le2)		/* this is an Oncut edge */
  {
    newseg->kind = Evf_inplane;
    newseg->le[0] = le1;
    newseg->le[1] = Twin_le(le1);
    newseg->pt[0] = le1->facevert->pos;
    newseg->pt[1] = newseg->le[1]->facevert->pos;
    notify_cursor_pause();
  }
  else
  {
    newseg->kind = Fve_intersect;
    newseg->le[0] = le1;
    newseg->le[1] = le2;
    newseg->pt[0] = le1->facedge->cutpt;
    newseg->pt[1] = le2->facedge->cutpt;
    evf1 = le1->facedge;
    evf2 = le2->facedge;
    if (abs(evf1->cutype) != Wascut)
      if (((le1 == evf1->le1) && (evf1->cutype == Pt2cut)) ||
	  ((le1 == evf1->le2) && (evf1->cutype == Pt1cut)))
	newseg->le[0] = le1->next;
    if (abs(evf2->cutype) != Wascut)
      if (((le2 == evf2->le1) && (evf2->cutype == Pt2cut)) ||
	  ((le2 == evf2->le2) && (evf2->cutype == Pt1cut)))
	newseg->le[1] = le2->next;
  }
}

  static Boolean
crosses_plane(leptr le1, leptr le2)
{
  evfptr evf1,evf2;
  float diff1,diff2;
  ;
  evf1 = le1->facedge;
  evf2 = le2->facedge;
  if (abs(evf1->cutype) == Pt1cut)
    diff1 = evf1->le2->facevert->pos[vz] - evf1->le1->facevert->pos[vz];
  else
    diff1 = evf1->le1->facevert->pos[vz] - evf1->le2->facevert->pos[vz];

  if (abs(evf2->cutype) == Pt1cut)
    diff2 = evf2->le2->facevert->pos[vz] - evf2->le1->facevert->pos[vz];
  else
    diff2 = evf2->le1->facevert->pos[vz] - evf2->le2->facevert->pos[vz];

  return (fsign(diff1) != fsign(diff2));
}

  static Boolean
same_cutvfe(leptr le1, leptr le2)
{
  leptr cutle1,cutle2;
  evfptr evf1,evf2;
  ;
  evf1 = le1->facedge;
  evf2 = le2->facedge;
  if (abs(evf1->cutype) == Pt1cut)
    cutle1 = evf1->le1;
  else
    cutle1 = evf1->le2;
  if (abs(evf2->cutype) == Pt1cut)
    cutle2 = evf2->le1;
  else
    cutle2 = evf2->le2;

  return (cutle1->facevert == cutle2->facevert);
}

  void
add_oncut_edges(listptr seglist, leptr thisle)
{
  leptr temple;
  evfptr thisevf;
  fveptr thisfve;
  ;
  /* this routine is not totally necessary...but it allows finding oncut edges */
  /* without searching through the entire face */
  thisevf = thisle->facedge;
  if (((abs(thisevf->cutype) == Pt1cut) && (thisle == thisevf->le1)) ||
      ((abs(thisevf->cutype) == Pt2cut) && (thisle == thisevf->le2)))
  {				/* thisle->facevert->pos is on the plane */
    temple = thisle->prev;
    while(temple->facedge->cutype == Oncut)
    {
      add_segment(seglist,temple,temple);
      temple = temple->prev;
    }
  }
  else				/* thisle->next->facevert->pos is on the plane */
  {
    temple = thisle->next;
    while(temple->facedge->cutype == Oncut)
    {
      add_segment(seglist,temple,temple);
      temple = temple->next;
    }
  }

}

  static void
make_face_segments(fveptr thisfve, listptr seglist)
  /* find all the edges in this face  that are cut and put them in a sorted */
  /* list. construct the section line segments from this list and put them in */
{				/* the segment list for later processing   */
  evfptr thisevf,nextevf;	/* into a true section line              */
  boundptr thisbound;
  leptr firstle,thisle,nextle;
  sortptr thisort;
  listptr edgelist;
  Boolean ptcut;
  ;
  edgelist = create_list();
  /* put the cut edges of this face in a sorted list */
  sort_cut_edges(thisfve,edgelist);
  if (edgelist->numelems & 1)
  {
    system_warning("make_face_sgements: face crosses plane odd number of times");
/*
    printfaceles(thisfve);
    reset_grafx();
    pause();
*/
  }
#if 0
  if (edgelist->numelems == 0)
  {
    thisbound = thisfve->boundlist->first.bnd;
    while (thisbound)
    {
      firstle = thisle = thisbound->lelist->first.le;
      do
      {
	if (thisle->facedge->cutype == Oncut)
	  add_segment(seglist,thisle,thisle);
	thisle = thisle->next;
      } while(thisle != firstle);
      thisbound = thisbound->next;
    }
    return;
  }
#endif
  /* create segments of the section line for this face */
  /* and add to the global list for this shell*/
  thisort = edgelist->first.sort;
  while ((thisort != Nil) && (thisort->next != Nil))
  {
    thisle = (leptr) thisort->sortelem;
    nextle = (leptr) (thisort->next)->sortelem;
    thisevf = thisle->facedge;
    nextevf = nextle->facedge;
    add_segment(seglist,thisle,nextle);
    /* add any Oncut edges following or preceding this edge to seglist */
    if ((abs(nextevf->cutype) == Pt1cut) || (abs(nextevf->cutype) == Pt2cut))
      add_oncut_edges(seglist,nextle);
    thisort = thisort->next->next;
  }
  del_list(edgelist);
}

/* produces ALL loop traversals of an obj's section line. */

  void
make_obj_ringlist(objptr thisobj)
{
  listptr ringlist,seglist;
  ringptr firstring;
  fveptr thisfve;
  shellptr thishell;
  ;
  thishell = First_obj_shell(thisobj);
  while (thishell != Nil)
  {
    clear_list(thishell->ringlist); /* empty out any existing section line */
    
    ringlist = thishell->ringlist;
    thisfve = thishell->fvelist->first.fve;
    seglist = create_list();
    /* first create one unsorted ring */
    while (thisfve)
    {
      if (face_in_plane(thisfve))
	add_segment(seglist,
		    thisfve->boundlist->first.bnd->lelist->first.le,Nil);
      else
	make_face_segments(thisfve,seglist);
      thisfve = thisfve->next;
    }
    if (seglist->numelems != 0)
    {
      firstring = (ringptr) append_new_blank_elem(ringlist,Ring_type);
      firstring->seglist = seglist;
      set_ringbbox(firstring);
    }
    else
      free_elem((elemptr *) &seglist);
    /* sort and break up ring...to be done */
    thishell = thishell->next;
  }
}

  void
clear_obj_ringlist(objptr thisobj)
{
  shellptr thishell;
  ;
  thishell = First_obj_shell(thisobj);
  while (thishell != Nil)
  {
    clear_list(thishell->ringlist); /* empty out any existing section line */
    thishell = thishell->next;
  }
}

  Boolean
must_make_obj_ringlist(objptr thisobj, stateptr state, stateptr oldstate)
{		/* see if we have to remake this objects section line */
  listptr ringlist,seglist;
  ringptr firstring;
  fveptr thisfve;
  shellptr thishell;
  Boolean intersects_now,intersected_last_refresh,must_do_it;
  bbox3dtype cutplane_bbox,old_cutplane_bbox;
  ;
  if (cutplane_time < 2)
    return (TRUE);

  setpos3d(cutplane_bbox.mincorner,Mincrossx,Mincrossy,
	   state->cursor[vz]-Cutplanebboxmargin);
  setpos3d(cutplane_bbox.maxcorner,Maxcrossx,Maxcrossy,
	   state->cursor[vz]+Cutplanebboxmargin);
  setpos3d(old_cutplane_bbox.mincorner,Mincrossx,Mincrossy,
	   oldstate->cursor[vz]-Cutplanebboxmargin);
  setpos3d(old_cutplane_bbox.maxcorner,Maxcrossx,Maxcrossy,
	   oldstate->cursor[vz]+Cutplanebboxmargin);

  intersects_now = bbox_intersect3d(&cutplane_bbox,&(thisobj->objbbox));
  intersected_last_refresh =
    bbox_intersect3d(&old_cutplane_bbox,&(thisobj->objbbox));

  if (!intersects_now && !intersected_last_refresh)
    return (FALSE);

  if (!intersects_now && intersected_last_refresh)
  {	/* just clear it here so we dont have to repeat bbox test later */
    clear_obj_ringlist(thisobj);
    return (FALSE);
  }

  return (TRUE);
}
