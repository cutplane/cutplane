
/* FILE: split.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Face Splitting Routines for the Cutplane Editor              */
/*                             A La Mantyla !                             */
/*                                                                        */
/* Authors: WAK Only                              Date: December 20, 1989 */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define SPLITMODULE

#include <config.h>
#include <stdio.h>
#include <math.h>

#include <platform_gl.h>

#include <cutplane.h>
#include <math3d.h>
#include <update.h>

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                    Edge Subdivision Routines                               */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
splitup_edge(evfptr thisevf,int numsplits)
{
  double tinc = 1.0 / (double) numsplits;
  double a,b,c;
  vertype pt1,pt2,newpt,incvec;
  int i;
  leptr thisle;
  ;
  copypos3d(thisevf->le1->facevert->pos,pt1);
  copypos3d(thisevf->le2->facevert->pos,pt2);
  
  a = pt2[vx] - pt1[vx];
  b = pt2[vy] - pt1[vy];
  c = pt2[vz] - pt1[vz];
  incvec[vx] = a * tinc;
  incvec[vy] = b * tinc;
  incvec[vz] = c * tinc;
  incvec[vw] = 1.0;
  thisle = thisevf->le1;
  copypos3d(pt1,newpt);
  for (i = 1; i < numsplits; ++i)
  {
    addvec3d(incvec,newpt,newpt);
    make_edge_vert(thisle,Twin_le(thisle)->next,newpt);
  }
}

 void
splitup_edges(shellptr thishell,int numsplits)
{
  evfptr thisevf,finalevf;
  ;
  thisevf = thishell->evflist->first.evf;
  finalevf = thishell->evflist->last.evf;
  do
  {
    splitup_edge(thisevf,numsplits);
    thisevf = thisevf->next;
  } while (thisevf != finalevf);
  splitup_edge(thisevf,numsplits);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                    Face Splitting Routines                                 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* Split a face radially from a central position */

  leptr
split_face_at_vert(fveptr thisfve, vertype centerpos)
{
  leptr stople,thisle,centerle;
  fveptr newfve;
  ;
  thisle = thisfve->boundlist->first.bnd->lelist->first.le;
  centerle = make_edge_vert(thisle,thisle,centerpos);
  stople = centerle->prev;
  thisle = thisle->next;
  while (thisle != stople)
  {
    newfve = make_edge_face(centerle,thisle);
    if (thisfve->thecolor != Nil)
      set_feature_color((featureptr) newfve, thisfve->thecolor);
    centerle = thisle->prev;
    thisle = thisle->next;
  }
  return(centerle);		/* return le of center */
}

/* split face in a fan pattern about an le on its outer boundary */

  void
fan_split_fve_fromle(fveptr thisfve, leptr apexle)
{
  leptr thisle;
  fveptr newfve;
  ;
  apexle = thisfve->boundlist->first.bnd->lelist->first.le;
  thisle = apexle->next->next;
  while (thisle->next != apexle)
  {
    newfve = make_edge_face(apexle,thisle);
    if (thisfve->thecolor != Nil)
      set_feature_color((featureptr) newfve,thisfve->thecolor);
    apexle = thisle->prev;
    thisle = thisle->next;
  }
}

/* split face at arbitrary vertex in a fan pattern */

  void
fan_split_fve(fveptr thisfve)
{
  leptr thisle,apexle;
  fveptr newfve;
  ;
  apexle = thisfve->boundlist->first.bnd->lelist->first.le;
  thisle = apexle->next->next;
  while (thisle->next != apexle)
  {
    make_edge_face(apexle,thisle);
    if (thisfve->thecolor != Nil)
      set_feature_color((featureptr) newfve,thisfve->thecolor);
    apexle = thisle->prev;
    thisle = thisle->next;
  }
}

/* split face in a zig zag pattern */

  void
zigzag_split_fve(fveptr thisfve)
{
  leptr firstle,thisle,apexle,longle;
  surfptr thisurf;
  fveptr newfve;
  float maxlength2,length2;
  int count = 1;
  ;
  thisurf = thisfve->mothersurf;
  /* find longest le */
  longle = thisle = firstle = thisfve->boundlist->first.bnd->lelist->first.le;
  maxlength2 = distance2(longle->facevert->pos,longle->next->facevert->pos);
  do
  {
    length2 = distance2(thisle->facevert->pos,thisle->next->facevert->pos);
    if (length2 > maxlength2)
    {
      longle = thisle;
      maxlength2 = length2;
    }
    thisle = thisle->next;
  } while (thisle != firstle);
  apexle = longle;
  thisle = apexle->next->next;
  do
  {
    newfve = make_edge_face(apexle,thisle);
    newfve->mothersurf = thisurf;
    if (thisurf != Nil)
    {
      append_new_blank_elem(thisurf->fvelist,Ep_type);
      thisurf->fvelist->last.ep->thiselem = (elemptr) newfve;
    }
    if (thisfve->thecolor != Nil)
      set_feature_color((featureptr) newfve,thisfve->thecolor);
    if (count & 1)
      apexle = thisle->prev->prev;
    else
    {
      apexle = thisle->prev;
      thisle = thisle->next;
    }
    count++;
  } while (thisle->motherbound->lelist->numelems > 3);
}

/* split off one le on the outer boundary of a face */

  void
simple_face_split(fveptr thisfve, leptr apexle)
{
  fveptr newfve;
  surfptr thisurf;
  ;
  if (apexle->motherbound->lelist->numelems <= 3)  /* dont split triangles */
    return;
  newfve = make_edge_face(apexle->next,apexle->prev);
  if (thisfve->thecolor != Nil)
    set_feature_color((featureptr) newfve, thisfve->thecolor);
  thisurf = thisfve->mothersurf;
  newfve->mothersurf = thisurf;
  if (thisurf != Nil)
  {
    append_new_blank_elem(thisurf->fvelist,Ep_type);
    thisurf->fvelist->last.ep->thiselem = (elemptr) newfve;
  }
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                Delauney Triangularization Routines                         */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
complete_triangularization(fveptr thisfve)
{
  zigzag_split_fve(thisfve);
}

  leptr
find_closest_vertex(leptr testle, fveptr thisfve)
{
  float thisdist,distance,*testpos,*thispos;
  leptr firstle,thisle, closestle = Nil;
  boundptr thisbound;
  vertype vec;
  ;
  testpos = testle->facevert->pos;
  thisbound = thisfve->boundlist->first.bnd;
  while (thisbound)
  {
    if (thisbound == testle->motherbound)
    {
      firstle = testle;
      thisle = testle->next;
    }
    else
      firstle = thisle = thisbound->lelist->first.le;
    do
    {
      thispos = thisle->facevert->pos;
      diffpos3d(testpos,thispos,vec);
      thisdist = vec[vx]*vec[vx] + vec[vy]*vec[vy] + vec[vz]*vec[vz];
      if ((thisdist < distance) || (closestle == Nil))
      {
	if ((thisle == testle->next) || (thisle == testle->prev) ||
	    (edge_in_face3d(testpos,thispos,thisfve)))
	{
	  distance = thisdist;
	  closestle = thisle;
	}
      }
      thisle = thisle->next;
    } while (thisle != firstle);
    thisbound = thisbound->next;
  }

  if ((closestle == testle->next) || (closestle == testle->prev))
    return (Nil);

  return (closestle);
}

  void
find_delauney_edge(fveptr thisfve)
{
  boundptr thisbound;
  leptr closestle,thisle,firstle,nextle;
  surfptr thisurf;
  fveptr newfve;
  ;
  thisbound = thisfve->boundlist->first.bnd;
  if ((thisfve->boundlist->numelems == 1) && (thisbound->lelist->numelems <= 3))
    return;
  thisurf = thisfve->mothersurf;
  while (thisbound)
  {
    firstle = thisle = thisbound->lelist->first.le;
    /* find a delauney edge that isn't already a side of the face */
    /* then split face */
    do
    {
      newfve = Nil;
      nextle = thisle->next;
      closestle = find_closest_vertex(thisle,thisfve);
      if (closestle != Nil)
      {
	if (thisle->motherbound == closestle->motherbound)
	{
	  /* thisle must be second arg so that it stays in thisfve */
	  newfve = make_edge_face(closestle,thisle);
	  newfve->mothersurf = thisurf;
	  if (thisurf != Nil)
	  {
	    append_new_blank_elem(thisurf->fvelist,Ep_type);
	    thisurf->fvelist->last.ep->thiselem = (elemptr) newfve;
	  }

	  /* make firstle = thisle since we might not make it back to the */
	  /* original firstle after the face is split */
	  firstle = thisle;
	}
	else
	  kill_ring_make_edge(thisle,closestle);
	/* might as well do the new face also since were here */
	if (newfve)
	  find_delauney_edge(newfve);
      }
      thisle = nextle;
    } while (thisle != firstle);
    thisbound = thisbound->next;
  }
  if (thisfve->boundlist->first.bnd->lelist->numelems > 3)
    complete_triangularization(thisfve);
}

  void
triangularize_fve(fveptr thisfve)
{
  surfptr thisurf;
  ;
  if ((thisfve->boundlist->numelems == 1) &&
      (thisfve->boundlist->first.bnd->lelist->numelems < 4))
    return;
      
  if (thisfve->mothersurf == Nil)
  {
    thisfve->mothersurf = (surfptr) append_new_blank_elem(thisfve->mothershell->
							  surflist,Surf_type);
    thisurf = thisfve->mothersurf;
    append_new_blank_elem(thisurf->fvelist,Ep_type);
    thisurf->fvelist->last.ep->thiselem = (elemptr) thisfve;
  }
  find_delauney_edge(thisfve);
}

  void
triangularize_shell(shellptr thishell)
{
  fveptr thisfve;
  ;
  thisfve = thishell->fvelist->first.fve;
  while (thisfve)
  {
    find_delauney_edge(thisfve);
    thisfve = thisfve->next;
  }
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                    Planarization Routines                                  */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  Boolean
planarize_face(fveptr thisfve, leptr thisle)
{
  vertype centroidpos;
  leptr firstle;
  int translatecount = 0;
  surfptr thisurf;
  ;
  if (thisfve->boundlist->first.bnd->lelist->numelems > 3) /* No triangles*/
  {
    if (face_is_concave(thisfve) || (thisfve->boundlist->numelems > 1))
      triangularize_fve(thisfve);
    else
    {
      if (thisfve->mothersurf == Nil)
      {
	thisfve->mothersurf =
	  (surfptr) append_new_blank_elem(thisfve->mothershell->surflist,
					  Surf_type);
	thisurf = thisfve->mothersurf;
	append_new_blank_elem(thisurf->fvelist,Ep_type);
	thisurf->fvelist->last.ep->thiselem = (elemptr) thisfve;
      }
      simple_face_split(thisfve,thisle);
    }
    return(TRUE);
  }
  return(FALSE);
}

 void
planarize_affected_fves(featureptr thisfeature, vertype deltapos)
{
  shellptr thishell = Nil;
  fveptr thisfve,motherfve;
  evfptr thisevf;
  vfeptr thisvfe;
  boundptr thisbound;
  leptr nextle,thisle,firstle,boundle,stople;
  Boolean splitting_done = FALSE;
  int i;
  ;
  switch (thisfeature->type_id)
  {
  case Vfe_type:
    thisvfe = (vfeptr) thisfeature;
    thishell = thisvfe->startle->motherbound->motherfve->mothershell;
    thisle = thisvfe->startle;
    stople = Twin_le(thisle->prev);
    do
    {
      nextle = Twin_le(thisle)->next;
      motherfve = thisle->motherbound->motherfve;
      if (compare(0.0,dotvecs(deltapos,motherfve->facenorm),1.0e-4) != 0)
	splitting_done |= planarize_face(motherfve,thisle);
      thisle = nextle;
    } while (thisle != stople);
    motherfve = thisle->motherbound->motherfve;
    if (compare(0.0,dotvecs(deltapos,motherfve->facenorm),1.0e-4) != 0)
      splitting_done |= planarize_face(motherfve,thisle);
    break;
  case Evf_type:
    thisle = ((evfptr) thisfeature)->le1;
    thishell = thisle->motherbound->motherfve->mothershell;
    for (i=0; i<2; i++)
    {
      firstle = thisle;
      stople = Twin_le(thisle->prev);
      do
      {
	nextle = Twin_le(thisle)->next;
	motherfve = thisle->motherbound->motherfve;
	if (compare(0.0,dotvecs(deltapos,motherfve->facenorm),1.0e-4) != 0)
	  splitting_done |= planarize_face(motherfve,thisle);
	thisle = nextle;
      } while (thisle != stople);
      motherfve = thisle->motherbound->motherfve;
      if (compare(0.0,dotvecs(deltapos,motherfve->facenorm),1.0e-4) != 0)
	splitting_done |= planarize_face(motherfve,thisle);
      thisle = ((evfptr) thisfeature)->le2;
    }
    break;
  case Fve_type:
    thisfve = (fveptr) thisfeature;
    thishell = thisfve->mothershell;
    thisbound = thisfve->boundlist->first.bnd; 
    while (thisbound)
    {
      boundle = firstle = thisbound->lelist->first.le;
      do
      {
	thisle = Twin_le(boundle)->next;
	do
	{
	  motherfve = thisle->motherbound->motherfve;
	  nextle = Twin_le(thisle)->next;
	  if (compare(0.0,dotvecs(deltapos,motherfve->facenorm),1.0e-4) != 0)
	    splitting_done |= planarize_face(motherfve,thisle);
	  thisle = nextle;
	} while (thisle != boundle);
	boundle = thisle->next;
      } while (boundle != firstle);
      thisbound = thisbound->next;
    }
    break;
  default:
    return;
  }
}

  void
planarize_shell(shellptr thishell)
{
  Boolean splitting_done = FALSE;
  fveptr thisfve,nextfve;
  leptr thisle;
  ;
  thisfve = thishell->fvelist->first.fve;
  while (thisfve != Nil)
  {
    nextfve = thisfve->next;
    splitting_done |= planarize_face(thisfve,thisle);
    thisfve = nextfve;
  }
}
    
  void
planarize_object(objptr thisobj)
{
  shellptr thishell;
  ;
  thishell = First_obj_shell(thisobj);
  while (thishell != Nil)
  {
    planarize_shell(thishell);
    thishell = thishell->next;
  }
}

Boolean
coplanar(fveptr fve1, fveptr fve2)
{
  return (compare(1.0,dotvecs(fve1->facenorm,fve2->facenorm),1.0e-4) == 0);
}

/* were using this version right now */
void
recombine_coplanar_faces(objptr thisobj)
{
  shellptr thishell;
  evfptr thisevf,nextevf;
  surfptr thisurf;
  fveptr fve1,fve2,thisfve;
  ;
  thishell = thisobj->shellist->first.shell;
  while (thishell)
  {
    /* first clear out fvelists in surfaces */
    thisurf = thishell->surflist->first.surf;
    while (thisurf)
    {
      clear_list(thisurf->fvelist);
      thisurf = thisurf->next;
    }
    /* 2nd kill all uneeded faces */
    thisevf = thishell->evflist->first.evf;
    while (thisevf)
    {
      nextevf = thisevf->next;
      fve1 = thisevf->le1->motherbound->motherfve;
      fve2 = thisevf->le2->motherbound->motherfve;
      if (fve1 != fve2)
      {
	if ((fve1->mothersurf != Nil) &&
	    (fve1->mothersurf == fve2->mothersurf) && coplanar(fve1,fve2))
	  kill_edge_face(thisevf->le1,thisevf->le2);
      }
      else
      {
	if (fve1->mothersurf != Nil)
	  kill_edge_make_ring(thisevf->le1,thisevf->le2);
      }
      thisevf = nextevf;
    }
    /* 3rd regenerate fvelists in surfaces */
    thisfve = thishell->fvelist->first.fve;
    while (thisfve)
    {
      if (thisfve->mothersurf != Nil)
      {
	append_new_blank_elem(thisfve->mothersurf->fvelist,Ep_type);
	thisfve->mothersurf->fvelist->last.ep->thiselem = (elemptr) thisfve;
      }
      thisfve = thisfve->next;
    }
    thishell = thishell->next;
  }
}

/* were using this version right now */
void
dumb_recombine_coplanar_faces(objptr thisobj)
{
  shellptr thishell;
  evfptr thisevf,nextevf;
  surfptr thisurf;
  fveptr fve1,fve2,thisfve;
  ;
  thishell = thisobj->shellist->first.shell;
  while (thishell)
  {
    thisevf = thishell->evflist->first.evf;
    while (thisevf)
    {
      nextevf = thisevf->next;
      fve1 = thisevf->le1->motherbound->motherfve;
      fve2 = thisevf->le2->motherbound->motherfve;
      if (fve1 != fve2)
      {
	if (coplanar(fve1, fve2))
	  kill_edge_face(thisevf->le1,thisevf->le2);
      }
      else
      {
	// If the two sides of the edge are the same boundary, we
	// split the boundary in two. For example if we have a concave
	// face that abuts itself along one edge.
	if (thisevf->le1->motherbound == thisevf->le2->motherbound)
	  kill_edge_make_ring(thisevf->le1, thisevf->le2);
      }

      thisevf = nextevf;
    }
    thishell = thishell->next;
  }
}

 fveptr
remove_coplanar_surface_facets(surfptr thisurf)
{				/* remove coplanar facets interior to the surf */
  leptr thisle,stople;
  fveptr thisfve,fve1,fve2;
  evfptr thisevf;
  ;
  if (thisurf == Nil)
    return Nil;
  if (thisurf->fvelist->numelems == 0)
    system_error("remove_surface_facets: this surface has no facets");
  else if (thisurf->fvelist->numelems == 1)
    return ((fveptr) thisurf->fvelist->first.ep->thiselem);

  thisfve = (fveptr) thisurf->fvelist->first.ep->thiselem;
  /* only look at the first bound since only these bounds could have */
  /* been created by the facet generator*/
  thisle = thisfve->boundlist->first.bnd->lelist->first.le;
  stople = thisle->prev;
  do
  {
    thisevf = thisle->facedge;
    fve1 = thisevf->le1->motherbound->motherfve;
    fve2 = thisevf->le2->motherbound->motherfve;
    /* kill edges that are interior to thisurf...dont kill struts since they */
    /* werent created by the faceting */
    if ((fve1 != fve2) && (fve1->mothersurf == fve2->mothersurf) &&
	coplanar(fve1,fve2))
    {
      thisle = thisle->prev;
      if (fve1 == thisfve)	/* order args so that thisfve remains */
	kill_edge_face(thisevf->le1,thisevf->le2);
      else
	kill_edge_face(thisevf->le2,thisevf->le1);
    }
    thisle = thisle->next;
  } while (thisle != stople);
  thisevf = thisle->facedge;
  fve1 = thisevf->le1->motherbound->motherfve;
  fve2 = thisevf->le2->motherbound->motherfve;
  if ((fve1 != fve2) && (fve1->mothersurf == fve2->mothersurf) &&
      coplanar(fve1,fve2))
  {
    thisle = thisle->prev;
    if (fve1 == thisfve)	/* order args so that thisfve remains */
      kill_edge_face(thisevf->le1,thisevf->le2);
    else
      kill_edge_face(thisevf->le2,thisevf->le1);
  }
  
  return (thisfve);
}

void
regen_surface_fvelist_engine(fveptr seedfve)
{
  boundptr thisbound;
  fveptr thisfve;
  surfptr thisurf;
  leptr thisle,firstle;
  elemptrptr newfveptr;
  ;
  thisurf = seedfve->mothersurf;
  thisbound = thisfve->boundlist->first.bnd;
  while (thisbound)
  {
    firstle = thisle = thisbound->lelist->first.le;
    do
    {
      thisfve = Twin_le(thisle)->motherbound->motherfve;
      if (thisfve->mothersurf == thisurf)
      {
	newfveptr = (elemptrptr) append_new_blank_elem(thisurf->fvelist,Ep_type);
	newfveptr->thiselem = (elemptr) thisfve;
	regen_surface_fvelist_engine(thisfve);
      }
      thisle = thisle->next;
    } while (thisle != firstle);
    thisbound = thisbound->next;
  }
}

void
regen_surface_fvelist_from_seed(fveptr seedfve)
{
  surfptr thisurf;
  elemptrptr newfveptr;
  ;
  thisurf = seedfve->mothersurf;
  if (thisurf == Nil)
    return;
  clear_list(thisurf->fvelist);
  newfveptr = (elemptrptr) append_new_blank_elem(thisurf->fvelist,Ep_type);
  newfveptr->thiselem = (elemptr) seedfve;

  regen_surface_fvelist_engine(seedfve);
}

  leptr
next_surfle_cyclical(leptr thisle)
{
  surfptr newsurf;
  leptr firstle;
  ;
  firstle = thisle;
  thisle = Twin_le(thisle)->next;
  newsurf = thisle->motherbound->motherfve->mothersurf;
  if (newsurf == Nil)
    return (thisle);
  do
  {
    if (Twin_le(thisle)->motherbound->motherfve->mothersurf != newsurf)
      return(thisle);
    thisle = Twin_le(thisle)->next;
  } while (thisle != firstle);

  return (thisle);
}

  leptr
prev_surfle_cyclical(leptr thisle)
{
  surfptr oldsurf;
  leptr firstle;
  ;
  oldsurf = thisle->motherbound->motherfve->mothersurf;
  firstle = thisle;
  thisle = Twin_le(thisle->prev);
  if (oldsurf == Nil)
    return (thisle);
  do
  {
    if (thisle->motherbound->motherfve->mothersurf != oldsurf)
      return(thisle);
    thisle = Twin_le(thisle->prev);
  } while (thisle != firstle);

  return (thisle);
}

  void
recombine_affected_coplanar_fves(featureptr thisfeature)
{
  fveptr thisfve,motherfve;
  evfptr thisevf;
  vfeptr thisvfe;
  boundptr thisbound;
  leptr nextle,thisle,firstle,boundle,stople;
  Boolean splitting_done = FALSE;
  int i;
  ;
  switch (thisfeature->type_id)
  {
  case Vfe_type:
    thisvfe = (vfeptr) thisfeature;
    stople = thisle = next_surfle_cyclical(thisvfe->startle);
    do
    {
      nextle = next_surfle_cyclical(thisle);
      remove_coplanar_surface_facets(thisle->motherbound->motherfve->
				     mothersurf);
      regen_surface_fvelist_from_seed(thisle->motherbound->motherfve);
      thisle = nextle;
    } while (thisle != stople);
    break;
  case Evf_type:
    thisle = ((evfptr) thisfeature)->le1;
    for (i=0; i<2; i++)
    {
      stople = thisle = next_surfle_cyclical(thisle);
      do
      {
	nextle = next_surfle_cyclical(thisle);
	remove_coplanar_surface_facets(thisle->motherbound->motherfve->
				       mothersurf);
	regen_surface_fvelist_from_seed(thisle->motherbound->motherfve);
	thisle = nextle;
      } while (thisle != stople);
      thisle = ((evfptr) thisfeature)->le2;
    }
    break;
  case Fve_type:
    thisfve = (fveptr) thisfeature;
    thisbound = thisfve->boundlist->first.bnd; 
    while (thisbound)
    {
      boundle = firstle = thisbound->lelist->first.le;
      do
      {
	stople = thisle = next_surfle_cyclical(boundle);
	do
	{
	  nextle = next_surfle_cyclical(thisle);
	  remove_coplanar_surface_facets(thisle->motherbound->motherfve->
					 mothersurf);
	  regen_surface_fvelist_from_seed(thisle->motherbound->motherfve);
	  thisle = nextle;
	} while (thisle != boundle);
	boundle = thisle->next;
      } while (boundle != firstle);
      thisbound = thisbound->next;
    }
    break;
  default:
    return;
  }
}
