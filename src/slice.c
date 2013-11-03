
/* FILE: slice.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*             GLuint Slicing Routines for the Cutplane Editor            */
/*                                                                        */
/* Author: LJE                                    Date: December 18,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define SLICEMODULE

#include <config.h>

#include <platform_gl.h>
#include <stdio.h>		/* for print statements debug hack */
#include <stdlib.h>		// For abs()

#include <cutplane.h>

#include <slice.h>
#include <intersect.h>
#include <math3d.h>
#include <math.h>
#include <grafx.h>
#include <pick.h>

  static int
location(register vfeptr thisvfe, register stateptr state)
{
  register float diff;
  ;
  diff = thisvfe->pos[vz] - state->cursor[vz];

  return((diff > Tolerance) ? Infront : (diff < -Tolerance) ? Inback : Onplane);
}

  static void
transfer_faces(listptr srcfvelist, fveptr thisfve, shellptr destshell)
{
  boundptr thisbound;
  leptr thisle,firstle;
  fveptr adjacent_face;
  ;
  rem_elem(srcfvelist,(elemptr) thisfve);
  add_elem(destshell->fvelist,(elemptr) destshell->fvelist->last.fve,
	   (elemptr) thisfve);
  thisfve->mothershell = destshell;
  thisbound = thisfve->boundlist->first.bnd;
  while (thisbound)
  {
    firstle = thisle = thisbound->lelist->first.le;
    do
    {
      adjacent_face = Twin_le(thisle)->motherbound->motherfve;
      if (adjacent_face->mothershell != destshell)
	transfer_faces(srcfvelist,adjacent_face,destshell);
      thisle = thisle->next;
    } while (thisle != firstle);
    thisbound = thisbound->next;
  }
}

  static void
transfer_edges(listptr srcevflist, shellptr destshell)
{
  evfptr thisevf,nextevf;
  ;
  thisevf = srcevflist->first.evf;
  while (thisevf)
  {
    nextevf = thisevf->next;
    if (thisevf->le1->motherbound->motherfve->mothershell == destshell)
    {
      rem_elem(srcevflist,(elemptr) thisevf);
      add_elem(destshell->evflist,(elemptr) destshell->evflist->last.evf,
	       (elemptr) thisevf);
    }
    thisevf = nextevf;
  }
}

  static void
transfer_vertices(listptr srcvfelist, shellptr destshell)
{
  vfeptr thisvfe,nextvfe;
  ;
  thisvfe = srcvfelist->first.vfe;
  while (thisvfe)
  {
    nextvfe = thisvfe->next;
    if (thisvfe->startle->motherbound->motherfve->mothershell == destshell)
    {
      rem_elem(srcvfelist,(elemptr) thisvfe);
      add_elem(destshell->vfelist,(elemptr) destshell->vfelist->last.vfe,
	       (elemptr) thisvfe);
    }
    thisvfe = nextvfe;
  }
}

  listptr
separate_shell_into_pieces(shellptr thishell, listptr frontcutfaces)
{
  elemptrptr thisrearcutface,thisfrontcutface;
  listptr rearcutfaces = create_list();
  fveptr tempfve;
  ;
  /* separate the shell into separate pieces */
  thisfrontcutface = frontcutfaces->first.ep;
  while(thisfrontcutface)
  {
    thisrearcutface = (elemptrptr) append_new_blank_elem(rearcutfaces,Ep_type);
    tempfve = (fveptr) thisfrontcutface->thiselem;
    thisrearcutface->thiselem = (elemptr)
      make_face_kill_loop(tempfve->boundlist->first.bnd->next);
    thisfrontcutface = thisfrontcutface->next;
  }
  return (rearcutfaces);
}

  void
make_piece_into_shell(shellptr sourceshell, fveptr startfve, listptr newshellist)
{
  elemptrptr thiselemptr,newelemptr;
  shellptr destshell;
  objptr motherobj;
  int i,j;
  ;
  if (startfve->mothershell == sourceshell)
  {
    motherobj = sourceshell->motherobj;
    destshell = (shellptr) append_new_blank_elem(motherobj->shellist,Shell_type);
    transfer_faces(sourceshell->fvelist,startfve,destshell);
    transfer_edges(sourceshell->evflist,destshell);
    transfer_vertices(sourceshell->vfelist,destshell);
    destshell->motherobj = motherobj;
    destshell->cclockwise = sourceshell->cclockwise;
    if (newshellist)
    {
      newelemptr = (elemptrptr) append_new_blank_elem(newshellist,Ep_type);
      newelemptr->thiselem = (elemptr) destshell;
    }
  }
}

  static void
separate_pieces(shellptr thishell, listptr frontcutfaces,
		listptr frontshellist, listptr rearshellist)
{
  elemptrptr thisrearcutface,thisfrontcutface;
  listptr rearcutfaces = create_list();
  ;
  rearcutfaces = separate_shell_into_pieces(thishell,frontcutfaces);
  /* make front pieces into shells */
  thisfrontcutface = frontcutfaces->first.ep;
  while (thisfrontcutface)
  {
    make_piece_into_shell(thishell,(fveptr) thisfrontcutface->thiselem,
			   frontshellist);
    thisfrontcutface = thisfrontcutface->next;
  }
  /* make rear pieces into shells */
  thisrearcutface = rearcutfaces->first.ep;
  while(thisrearcutface)
  {
    make_piece_into_shell(thishell,(fveptr) thisrearcutface->thiselem,
			   rearshellist);
    thisrearcutface = thisrearcutface->next;
  }
  clear_list((listptr) rearcutfaces);  free(rearcutfaces);

  kill_shell(thishell);
}

  static void
consolidate_null_faces(listptr nulledgelist, listptr frontcutfaces)
{
  elemptrptr thiselemptr;
  leptr thisnull_le;
  elemptrptr nullfaceptr;
  ;
  thiselemptr = nulledgelist->first.ep;
  while(thiselemptr)
  {
    thisnull_le = (leptr) thiselemptr->thiselem;
    if (thisnull_le->motherbound != Twin_le(thisnull_le)->motherbound)
      kill_edge_face(thisnull_le,Twin_le(thisnull_le));
    else
    {
      nullfaceptr = (elemptrptr) append_new_blank_elem(frontcutfaces,Ep_type);
      nullfaceptr->thiselem = (elemptr) thisnull_le->motherbound->motherfve;
      kill_edge_make_ring(thisnull_le,Twin_le(thisnull_le));
    }
    thiselemptr = thiselemptr->next;
  }
}

  static GLboolean
parallel(register leptr le1, register leptr le2)
{				/* hmm maybe use tolerance here?? */
  vertype vec1,vec2;
  GLboolean itsparallel = TRUE;
  ;
  makevec3d(le1->facevert->pos,le1->next->facevert->pos,vec1);
  makevec3d(le2->facevert->pos,le2->next->facevert->pos,vec2);
  normalize(vec1,vec1);
  normalize(vec2,vec2);

  if (vectors_nearly_equal(vec1,vec2,1.0e-5))
    return (TRUE);

  flip_vec(vec1);

  return (vectors_nearly_equal(vec1,vec2,1.0e-5));
}

  static GLboolean
onplane_sector(leptr thisle, stateptr state)
{
  leptr checkle;
  int compareresult;
  ;
  /* this routine finds first "edge" not parallel to thisle and checks if */
  /* its on the plane */
  if ((location(thisle->facevert,state) == Onplane) &&
      (location(thisle->next->facevert,state) == Onplane))
  {
    compareresult = compare(1.0,
			    fabs(thisle->motherbound->motherfve->facenorm[vz]),
			    1.0e-3);
    if (compareresult != 0)
      return (FALSE);

    checkle = thisle->prev;
    while ((checkle != thisle) && parallel(checkle,thisle))
      checkle = checkle->prev;

    if (checkle == thisle)	/* this should never happen */
      return (TRUE);
    
    return (location(checkle->facevert,state) == Onplane);
  }
  return (FALSE);
}

  static void
find_all_transitions(listptr transitionlist, vfeptr thisvfe, stateptr state)
{
  leptr startle,thisle,nextle;
  int thislocation,nextlocation;
  elemptrptr newelemptr;
  ;
  thisle = startle = thisvfe->startle;
  thislocation = location(thisle->next->facevert,state);
  /* make the initial list of all the transitions */
  do
  {
    nextle = Twin_le(thisle)->next;
    nextlocation = location(nextle->next->facevert,state);
    if (thislocation != nextlocation)
    {
      newelemptr = (elemptrptr) append_new_blank_elem(transitionlist,Ep_type);
      add_property((elemptr) newelemptr, thislocation_prop);
      add_property((elemptr) newelemptr, nextlocation_prop);
      set_property(newelemptr,thislocation_prop,thislocation);
      set_property(newelemptr, nextlocation_prop,nextlocation);
      transitionlist->last.ep->thiselem = (elemptr) thisle;
    }
    thislocation = nextlocation;
    thisle = nextle;
  } while (thisle != startle);
}

  static GLboolean
behind_plane(leptr inplane_le, fveptr thisfve)
{
  leptr nextle;
  vertype testvec,inplane_vec;
  ;
  if (fabs(thisfve->facenorm[vz]) >= 1.0)
  {
    if (thisfve->facenorm[vz] < 0.0) /* its in front */
      return (FALSE);
    return (TRUE);		/* else its behind */
  }
  else
  {
    nextle = inplane_le->next;
    makevec3d(inplane_le->facevert->pos,nextle->facevert->pos,inplane_vec);
    if (cclockwise(thisfve))
      cross_prod(thisfve->facenorm,inplane_vec,testvec);
    else
      cross_prod(inplane_vec,thisfve->facenorm,testvec);
    if (testvec[vz] > 0.0)
      return (FALSE);
    return (TRUE);
  }
}

  static void
add_pseudo_transition(vfeptr thisvfe, listptr transitionlist)
{  /* for the case where we have two onplane edges */
  elemptrptr thistransition;
  leptr thisle;
  ;
  thistransition = (elemptrptr) append_new_blank_elem(transitionlist,Ep_type);
  thisle = thisvfe->startle;
  /* find an le thats in the face behind the plane to fool the code */
  /* below into meving off an le thats in the face in front of the plane */
  if (behind_plane(thisle,thisle->motherbound->motherfve))
    thistransition->thiselem = (elemptr) thisle;
  else
    thistransition->thiselem = (elemptr) Twin_le(thisle)->next;
  add_property((elemptr) thistransition,thislocation_prop);
  add_property((elemptr) thistransition,nextlocation_prop);
  set_property(thistransition,thislocation_prop,Infront);
  set_property(thistransition,nextlocation_prop,Infront);
}

  static void
prune_plateau_transitions(listptr transitionlist, stateptr state)
{
  /* get rid of transitions to and from coplanar faces if coplanar */
  /* face is considered to be on the same side (based on face normal) */
  elemptrptr thistransition,nextransition;
  GLboolean facingfront;
  leptr thisle,sectorle;
  int nextlocation,thislocation;
  ;
  thistransition = transitionlist->first.ep;
  while (thistransition)
  {
    thisle = (leptr) thistransition->thiselem;
    nextransition = thistransition->next;
    /* doing one transition at a time to avoid mess with possible nil */
    /* lists...but it is slower since you've got to do 2 onplane_sector calls */
    /* how about an append_circular function eh? */
    if (onplane_sector(thisle,state)) /* check transition off of an onplane */
    {				      /* sector */
      facingfront = (thisle->motherbound->motherfve->facenorm[vz] > 0.0);
      nextlocation = get_property_val((elemptr) thistransition,
				  nextlocation_prop).i;
      if (nextlocation == Inback)
	if (facingfront)
	  del_elem(transitionlist,(elemptr) thistransition);
	else
	  set_property(thistransition, thislocation_prop,Infront);
      else if (nextlocation == Infront)
	if (!facingfront)
	  del_elem(transitionlist, (elemptr) thistransition);
	else
	  set_property(thistransition, thislocation_prop,Inback);
      else	/* this should never happen since on-on is not a transition */
	system_error("prune_plateau_transition: on-on transition");
    }
    else		/* check for transition on to an onplane sector */
    {
      sectorle = Twin_le(Twin_le(thisle)->next);
      if ((sectorle != thisle) && onplane_sector(sectorle,state))
      {
	facingfront = (sectorle->motherbound->motherfve->facenorm[vz] > 0.0);
	thislocation = location(thisle->next->facevert,state);
	if (thislocation == Inback)
	  if (facingfront)
	    del_elem(transitionlist,(elemptr) thistransition);
	  else
	    set_property(thistransition,nextlocation_prop,Infront);
	else if (thislocation == Infront)
	  if (!facingfront)
	    del_elem(transitionlist,(elemptr) thistransition);
	  else
	    set_property(thistransition,nextlocation_prop,Inback);
	else	/* this should never happen since on-on is not a transition */
	  system_error("prune_plateau_transition: on-on transition");
      }
    }
    thistransition = nextransition;
  }
}

  static void
prune_consecutive_transitions(listptr transitionlist, stateptr state)
{
  /* get rid of extra transitions for front-on-back and */
  /* back-on-front. also change front-on-front to front-back-front */
  /* and change back-on-back to back-front-back */
  /* we also need to handle on-front-on and on-back-on cases! */
  elemptrptr thistransition,nextransition,prevtransition;
  int prevlocation,thislocation,nextlocation;
  leptr thisle;
  ;
  thistransition = transitionlist->first.ep;
  while (thistransition)
  {
    thisle = (leptr) thistransition->thiselem;
    thislocation = get_property_val((elemptr) thistransition, thislocation_prop).i;
    if (thislocation == Onplane)
    {
      prevtransition = (elemptrptr) prev_elem((listptr) transitionlist,
					      (elemptr) thistransition);
      prevlocation = get_property_val((elemptr) prevtransition,
					thislocation_prop).i;
      nextlocation = get_property_val((elemptr) thistransition,
					nextlocation_prop).i;
      if (prevlocation == Inback)
      {
	if (nextlocation == Inback) /* back-on-back case */
	  set_property(thistransition, thislocation_prop,Infront);
	else			/* back-on-front case */
	  set_property(thistransition, thislocation_prop,Inback);
      }
      else			/* front-on-front & front-on-back cases */
	set_property(thistransition, thislocation_prop,Inback);
      /* can do the following since we already handled onplane sectors */
      set_any_property(prevtransition, nextlocation_prop,
			 get_property_val((elemptr) thistransition,
					    thislocation_prop));
    }
    else if ((thislocation = get_property_val((elemptr) thistransition,
						nextlocation_prop).i) ==
	     Onplane)
    {
      nextransition = (elemptrptr) next_elem((listptr) transitionlist,
					     (elemptr) thistransition);
      /* is the following really right? */
      nextlocation = get_property_val((elemptr) nextransition,
					nextlocation_prop).i;
      prevlocation = get_property_val((elemptr) thistransition,
					thislocation_prop).i;
      if (prevlocation == Inback)
      {
	if (nextlocation == Inback) /* back-on-back case */
	  set_property(thistransition, nextlocation_prop,Infront);
	else			/* back-on-front case */
	  set_property(thistransition, nextlocation_prop,Inback);
      }
      else			/* front-on-front & front-on-back cases */
	set_property(thistransition, nextlocation_prop,Inback);
      /* can do the following since we already handled onplane sectors */
      set_any_property(nextransition, thislocation_prop,
			 get_property_val((elemptr) thistransition,
					    nextlocation_prop));
    }
    nextransition = thistransition->next;
    if (get_property_val((elemptr) thistransition,thislocation_prop).i ==
	get_property_val((elemptr) thistransition,nextlocation_prop).i)
      del_elem((listptr) transitionlist,(elemptr) thistransition);
    thistransition = nextransition;
  }
}

  static void
add_null_edges(vfeptr thisvfe, listptr transitionlist, stateptr state,
	       listptr nulledgelist)
{
  elemptrptr thistransition,firstransition,nextransition;
  int nextlocation;
  leptr thisle,nextle,null_le;
  ;
  firstransition = transitionlist->first.ep;
  thistransition = firstransition;
  do
  {
    nextlocation = get_property_val((elemptr) thistransition,nextlocation_prop).i;
    nextransition = (elemptrptr) next_elem((listptr) transitionlist,
					   (elemptr) thistransition);
    if (nextlocation == Infront)
    {
      if (get_property_val((elemptr) nextransition,thislocation_prop).i != Infront)
	printf("YoooHoooo, somethings wrong in add_null_edges\n");

      thisle = (leptr) thistransition->thiselem;
      thisle = Twin_le(thisle)->next;
      nextle = (leptr) nextransition->thiselem;
      nextle = Twin_le(nextle)->next;
      null_le = make_edge_vert(thisle,nextle,nextle->facevert->pos);
      append_new_blank_elem(nulledgelist,Ep_type);
      nulledgelist->last.ep->thiselem = (elemptr) null_le;
    }
    thistransition = nextransition;
  } while (thistransition != firstransition);
}

  static void
zero_shell_normals(shellptr thishell)
{
  fveptr thisfve;
  static vertype zerovec = {0.0,0.0,0.0,0.0};
  ;
  thisfve = thishell->fvelist->first.fve;
  while (thisfve)
  {
    setfnorm(thisfve,zerovec);
    thisfve = thisfve->next;
  }
}

  static void
finish_null_faces(listptr nulledgelist)
{
  leptr firstle,nextle,thisle;
  elemptrptr thiselemptr;
  ;
  thiselemptr = nulledgelist->first.ep;
  while (thiselemptr)
  {
    thisle = (leptr) thiselemptr->thiselem;
    nextle = thisle->next->next->next;
    make_edge_face(thisle,nextle);
    thiselemptr = thiselemptr->next;
  }
}

  static listptr
make_null_faces(listptr splitvertlist, shellptr thishell, stateptr state)
{				/* mantylas splitclassify */
  /* classify verts connected to splitverts according to position */
  /* reclassify verts connected to splitverts according to face normals */
  /* reclassify on-plane verts connected to splitverts according to:  */
  /* front-on-front -> front-back-front */
  /* front-on-back  -> front-back-back */
  /* back-on-back   -> back-front-back */
  /* back-on-front  -> back-back-front */
  elemptrptr thisplitvert;
  listptr nulledgelist = create_list();
  listptr transitionlist = create_list();
  ;
  thisplitvert = splitvertlist->first.ep;
  while (thisplitvert)
  {
    find_all_transitions(transitionlist,(vfeptr) thisplitvert->thiselem,state);
    if (transitionlist->numelems != 0)
    {
      prune_plateau_transitions(transitionlist,state);
      prune_consecutive_transitions(transitionlist,state);
    }
    else
      add_pseudo_transition((vfeptr) thisplitvert->thiselem,transitionlist);

    if (transitionlist->numelems != 0)
      add_null_edges((vfeptr) thisplitvert->thiselem,transitionlist,
		     state,nulledgelist);

    clear_list((listptr) transitionlist);
    thisplitvert = thisplitvert->next;
  }
  free_elem((elemptr *) &transitionlist);
  finish_null_faces(nulledgelist);
  return (nulledgelist);
}

  static void
add_split_vert(register listptr splitvertlist, register vfeptr thisvfe)
{
  register elemptrptr thiselemptrptr;
  ;
  /* Search splitvertlist to see if thisvfe is already there. */
  /* Hmm...this is n^2. Should really create a used flag field in vfe's or */
  /* create a boolean list that points to the vfes and make splitvertlist */
  /* point at the boolean list...actually that won't work */
  thiselemptrptr = splitvertlist->first.ep;
  while (thiselemptrptr)
  {
    if (((vfeptr) thiselemptrptr->thiselem) == thisvfe)
      return;
    thiselemptrptr = thiselemptrptr->next;
  }
  thiselemptrptr = (elemptrptr) append_new_blank_elem(splitvertlist,Ep_type);
  thiselemptrptr->thiselem = (elemptr) thisvfe;
}

  static listptr
get_split_verts(shellptr thishell, listptr splitedgelist)
{
  elemptrptr thisedgeptr;
  leptr thisle,startle;
  listptr splitvertlist = create_list();
  ;
  thisedgeptr = splitedgelist->first.ep;
  while (thisedgeptr)
  {
    add_split_vert(splitvertlist,
		   ((evfptr) (thisedgeptr->thiselem))->le1->facevert);
    add_split_vert(splitvertlist,
		   ((evfptr) (thisedgeptr->thiselem))->le2->facevert);
    thisedgeptr = thisedgeptr->next;
  }

  return (splitvertlist);
}

  static leptr
find_le_thisface(fveptr thisfve, leptr thisle)
{
  leptr startle;
  ;
  startle = thisle;
  do
  {
    if (thisle->motherbound->motherfve == thisfve)
      return (thisle);
    thisle = Twin_le(thisle)->next;
  } while (thisle != startle);

  return (Nil);
}

  static void
add_split_edge(listptr splitedgelist, evfptr thisevf)
{
  elemptrptr thiselemptrptr;
  ;
  thiselemptrptr = (elemptrptr) append_new_blank_elem(splitedgelist,Ep_type);
  thiselemptrptr->thiselem = (elemptr) thisevf;
}

GLboolean
already_connected(leptr le1, leptr le2)
{		/* see if two segment bounding edges are already connected */
  leptr thisle,firstle,stople;
  evfptr evf1,evf2;
  ;
  evf1 = le1->facedge;
  evf2 = le2->facedge;
  if ((evf1->cutype == Oncut) && ((abs(evf2->cutype) == Pt1cut) ||
				  (abs(evf2->cutype) == Pt2cut)))
  {
    firstle = thisle = le1;
    stople = le2;
  }
  else if ((evf2->cutype == Oncut) && ((abs(evf1->cutype) == Pt1cut) ||
				       (abs(evf1->cutype) == Pt2cut)))
  {
    firstle = thisle = le2;
    stople = le1;
  }
  else
    return (FALSE);

  thisle = thisle->next;
  while (thisle != stople)
  {
    if (thisle->facedge->cutype != Oncut)
      return (FALSE);
    thisle = thisle->next;
  }

  return (TRUE);   
}

  static listptr
get_split_edges(shellptr thishell)
{ /* this splits all the faces intersected by the plane, and puts the */
  /* new inplane edges into a list along with all the old inplane edges */
  fveptr newfve;
  segmentptr thiseg;
  leptr le1,le2;
  listptr seglist, splitedgelist = create_list();
  ;
  seglist = thishell->ringlist->first.ring->seglist;
  thiseg = seglist->first.seg;
  while (thiseg)
  {
    if (thiseg->kind != Fve_inplane)
    {
      if (thiseg->kind != Evf_inplane)
      {
	le1 = thiseg->le[0];
	le2 = thiseg->le[1];
	if (!already_connected(le1,le2))
	{
	  if (le1->motherbound != le2->motherbound)
	    kill_ring_make_edge(le1,le2);	/* in case face has hole!!! */
	  else
	  {
	    newfve = make_edge_face(le1,le2);
	    /* fix segment intersect fve in case mefing makes there */
	    /* intersectfve wrong */
	    /*
	      fix_concave_face_segs(newfve, thiseg);
	      */
	  }
	  add_split_edge(splitedgelist,le1->prev->facedge);
	}
      }
      else
	add_split_edge(splitedgelist,thiseg->le[0]->facedge);
    }
    thiseg = thiseg->next;
  }
  
  return (splitedgelist);
}

  static void
split_cut_edges(shellptr thishell)
{
  evfptr thisevf,lastevf;
  ;
  thisevf = thishell->evflist->first.evf;
  lastevf = thishell->evflist->last.evf;
  if (thisevf == Nil)
    return;
  while (thisevf != lastevf)
  {
    if (abs(thisevf->cutype) == Wascut)
    {
      make_edge_vert(thisevf->le1,thisevf->le2->next,thisevf->cutpt);
/*
      printf("cutpt = [%f,%f,%f]\n",
	     thisevf->cutpt[vx],thisevf->cutpt[vy],thisevf->cutpt[vz]);
*/
    }
    thisevf = thisevf->next;
  }
  if (abs(thisevf->cutype) == Wascut)
  {
    make_edge_vert(thisevf->le1,thisevf->le2->next,thisevf->cutpt);
/*
    printf("cutpt = [%f,%f,%f]\n",
	   thisevf->cutpt[vx],thisevf->cutpt[vy],thisevf->cutpt[vz]);
*/
  }
}

  void
slice_shell(shellptr thishell, stateptr state,
	     listptr rearshellist, listptr frontshellist)
{
  listptr splitvertlist = Nil, splitedgelist = Nil, nulledgelist = Nil;
  listptr frontcutfaces = create_list();
  sortptr thisort;
  ;
  if (thishell->ringlist->numelems == 0)
    return;			/* not cut at all, don't try to slice? */

  split_cut_edges(thishell);

  set_shell_evf_cutpts(thishell,state);
  make_obj_ringlist(thishell->motherobj);

  splitedgelist = get_split_edges(thishell);
  splitvertlist = get_split_verts(thishell,splitedgelist);

  /* make sure all the normals are correct */
  /* must zero out normals before you do this */
  zero_shell_normals(thishell);
  find_obj_normals(thishell->motherobj);

  nulledgelist = make_null_faces(splitvertlist,thishell,state);
  if (nulledgelist->numelems == 0)
    return;
  consolidate_null_faces(nulledgelist,frontcutfaces);

  separate_pieces(thishell,frontcutfaces,frontshellist,rearshellist);

  del_list((listptr) splitvertlist); 
  del_list((listptr) splitedgelist); 
  del_list((listptr) nulledgelist); 
  del_list((listptr) frontcutfaces);

}


  void
slice_object(objptr thisobj, stateptr state,
	     listptr frontobjlist, listptr rearobjlist)
{
  shellptr thishell,nextshell,frontshell,rearshell;
  listptr frontshellist,rearshellist;
  elemptrptr front_ep,rear_ep;
  objptr newfrontobj,newrearobj;
  ;
  thishell = thisobj->shellist->first.shell;
  while (thishell != Nil) 
  {
    nextshell = thishell->next;
    frontshellist = create_list();
    rearshellist = create_list();
    slice_shell(thishell,state,frontshellist,rearshellist);
    front_ep = frontshellist->first.ep;
    while (front_ep != Nil)
    {
      append_new_blank_elem(frontobjlist,Ep_type);

      /* create a new obj at the same level as the object of the shell */
      /* you are splitting */
      newfrontobj = make_childobj_noshell(thisobj->parent);
      copy_matrix(thisobj->xform,newfrontobj->xform);
      copy_matrix(thisobj->invxform,newfrontobj->invxform);
      copy_feature_color((featureptr) thisobj,(featureptr) newfrontobj);

      frontshell = (shellptr) front_ep->thiselem;
      move_shell_to_obj(newfrontobj,frontshell);
      frontobjlist->last.ep->thiselem = (elemptr) newfrontobj;

      front_ep = front_ep->next;
    }
    rear_ep = rearshellist->first.ep;
    while (rear_ep != Nil)
    {
      append_new_blank_elem(rearobjlist,Ep_type);

      /* create a new obj at the same level as the object of the shell */
      /* you are splitting */
      newrearobj = make_childobj_noshell(thisobj->parent);
      copy_matrix(thisobj->xform,newrearobj->xform);
      copy_matrix(thisobj->invxform,newrearobj->invxform);
      copy_feature_color((featureptr) thisobj,(featureptr) newrearobj);

      rearshell = (shellptr) rear_ep->thiselem;
      move_shell_to_obj(newrearobj,rearshell);
      rearobjlist->last.ep->thiselem = (elemptr) newrearobj;

      rear_ep = rear_ep->next;
    }
    thishell = nextshell;
  }
  kill_obj(thisobj);		/* the old object is dead meat */
}
