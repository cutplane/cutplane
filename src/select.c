
/* FILE: select.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Selecting Routines for the Cutplane Editor                   */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define SELECTMODULE

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <platform_gl.h>

#include <cutplane.h>		/* cutplane.h includes select.h */

#include <math3d.h>
#include <grafx.h>
#include <intersect.h>
#include <tools.h>
#include <cursor.h>

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	                       GLOBAL VARIABLES				     */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* This var is set by CPL_set_global_picksize to something other than */
/* Pkboxsize if the picking range has to be not equal to the regular */
/* Pkboxsize, as per example when we are dragging the rotate circle. */
GLfloat global_picksize = Pkboxsize;

/* This var is set by CPL_always_select make sure things are always selectable */
/* while dragging stuff. */
GLboolean always_set_selectable = FALSE;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	          CURSOR PROXIMITY TO FEATURES CODE                          */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

  static GLboolean
cursor_near_pt(segmentptr thisegment,stateptr state,selectableptr thiselect)
{
  float *vertpos;
  ;
  if (thisegment->kind == Fve_inplane)
    return (FALSE);

  /* check both end pts of segment */
  if (thisegment->pt[0] == thisegment->le[0]->facevert->pos)
  {
    vertpos = thisegment->pt[0];
    if (pt_near_pt(state->cursor,vertpos,global_picksize))
    {
      thiselect->selectkind = Vertselect;
      copypos3d(vertpos,thiselect->nearestpos);
      thiselect->thefeature = (featureptr) thisegment->le[0]->facevert;
      return (TRUE);
    }
  }
  if (thisegment->pt[1] == thisegment->le[1]->facevert->pos)
  {
    vertpos = thisegment->pt[1];
    if (pt_near_pt(state->cursor,vertpos,global_picksize))
    {
      thiselect->selectkind = Vertselect;
      copypos3d(vertpos,thiselect->nearestpos);
      thiselect->thefeature = (featureptr) thisegment->le[1]->facevert;
      return (TRUE);
    }
  }
  return (FALSE);
}

  static GLboolean
cursor_near_edge(segmentptr thisegment,stateptr state,selectableptr thiselect)
{
  vertype ptnearestedge;
  ;
  if (thisegment->kind == Fve_inplane)
    return (FALSE);

  if (thisegment->kind == Evf_inplane)
  {
    if (pt_near_line(thisegment->pt[0],thisegment->pt[1],state->cursor,
		     ptnearestedge,global_picksize))
    {
      thiselect->selectkind = Edgeselect;
      copypos3d(ptnearestedge,thiselect->nearestpos);
      thiselect->thefeature = (featureptr) thisegment->le[0]->facedge;
      thiselect->motherseg = thisegment;
      return (TRUE);
    }
  }
  else
  {
    if ((thisegment->le[0]->facevert->pos != thisegment->pt[0]) &&
	(pt_near_pt(state->cursor,thisegment->pt[0],global_picksize)))
    {
      thiselect->selectkind = Edgeselect;
      copypos3d(thisegment->pt[0],thiselect->nearestpos);
      thiselect->thefeature = (featureptr) thisegment->le[0]->facedge;
      thiselect->motherseg = thisegment;
      return (TRUE);
    }
    if ((thisegment->le[1]->facevert->pos != thisegment->pt[1]) &&
	(pt_near_pt(state->cursor,thisegment->pt[1],global_picksize)))
    {
      thiselect->selectkind = Edgeselect;
      copypos3d(thisegment->pt[1],thiselect->nearestpos);
      thiselect->thefeature = (featureptr) thisegment->le[1]->facedge;
      thiselect->motherseg = thisegment;
      return (TRUE);
    }
  }
  return (FALSE);
}

  static GLboolean
cursor_near_face(segmentptr thisegment,stateptr state,selectableptr thiselect)
{
  vertype ptnearestface;
  double t;
  ;
  if (thisegment->kind == Fve_inplane)
    return (FALSE);

  if (thisegment->kind == Fve_intersect)
  {
    if (pt_near_line(thisegment->pt[0],thisegment->pt[1],state->cursor,
		     ptnearestface,global_picksize))
    {
      thiselect->selectkind = Faceselect;
      copypos3d(ptnearestface,thiselect->nearestpos);
      thiselect->thefeature =
	                  (featureptr) thisegment->le[0]->motherbound->motherfve;
      thiselect->motherseg = thisegment;

      return (TRUE);
    }
  }

  return (FALSE);
}

  static GLboolean
cursor_in_face(segmentptr thisegment,stateptr state,selectableptr thiselect)
{
  leptr thisle;
  fveptr inplanefve;
  ;
  if (thisegment->kind == Fve_inplane)
    if (point_in_2dbbox(state->cursor,&(thisegment->segbbox)))
    {
      inplanefve = thisegment->le[0]->motherbound->motherfve;
      if (pt_in_face(inplanefve,state->cursor))
      {
	thiselect->selectkind = Inplanefaceselect;
	copypos3d(state->cursor,thiselect->nearestpos);
	thiselect->thefeature = (featureptr) inplanefve;
	thiselect->motherseg = thisegment;
	return(TRUE);
      }
    }
  return(FALSE);
}

  static GLboolean
cursor_near_feature(segmentptr thisegment,stateptr state,
		    selectableptr thiselect)
{
  if (cursor_near_pt(thisegment,state,thiselect))
    return(TRUE);
  else if (cursor_near_edge(thisegment,state,thiselect))
    return(TRUE);
  else if (cursor_near_face(thisegment,state,thiselect)) /* intersected faces*/
    return(TRUE);	       
  else if (cursor_in_face(thisegment,state,thiselect)) /* in plane faces */
    return(TRUE);
  else
    return (FALSE);
}

  static GLboolean
cursor_near_shell(shellptr thishell,stateptr state)
{
  ringptr thisring;
  bbox2dtype selectbbox;
  ;
  thisring = thishell->ringlist->first.ring;
  while (thisring != Nil)
  {
    selectbbox = thisring->ringbbox; /* requires ANSI to copy struct */
    selectbbox.mincorner[vx] -= global_picksize;
    selectbbox.maxcorner[vy] += global_picksize;
    selectbbox.maxcorner[vx] += global_picksize;
    selectbbox.mincorner[vy] -= global_picksize;
    if (point_in_2dbbox(state->cursor,&selectbbox))
      return(TRUE);		/* cursor is near one of the shell's rings */
    thisring = thisring->next;
  }
  return(FALSE);
}

  GLboolean
cursor_in_shell(shellptr thishell, stateptr state)
{
  ringptr thisring;
  segmentptr thisegment;
  vertype pt1,pt2;
  int numsegs = 0;
  int numsegschecked = 0;
  ;
  thisring = thishell->ringlist->first.ring;
  while (thisring)
  {
    if (point_in_2dbbox(state->cursor,&(thisring->ringbbox)))
    {
      thisegment = thisring->seglist->first.seg;
      while(thisegment)
      {
	numsegschecked++;
	if (thisegment->kind != Fve_inplane)
	{
	  copypos3d(thisegment->pt[0],pt1);
	  copypos3d(thisegment->pt[1],pt2);
	  pt1[vz] = state->cursor[vz];
	  pt2[vz] = state->cursor[vz];
	  /* if one pt is on the ray dont count it if edge is below ray */
	  /* this works magically */
	  if (pt1[vy] == state->cursor[vy])
	  {
	    if (pt2[vy] > state->cursor[vy])
	      numsegs += int_ray_edge(pt1,pt2,state->cursor,vx,vy);
	  }
	  else if (pt2[vy] == state->cursor[vy])
	  {
	    if (pt2[vy] > state->cursor[vy])
	      numsegs += int_ray_edge(pt1,pt2,state->cursor,vx,vy);
	  }
	  else
	    numsegs += int_ray_edge(pt1,pt2,state->cursor,vx,vy);
	}
	thisegment = thisegment->next;
      }
    }
    thisring = thisring->next;
  }
  /* int_ray_edge returns twice the num of intersections */
#ifdef Testingcursorinring
  printf ("numsegs = %d found from %d segments\n",numsegs,numsegschecked);
#endif
  numsegs = numsegs >> 1;
  return((numsegs % 2) != 0);
}


  void
set_normal_selectbox_technique(selectableptr selectable_record)
{
  switch (selectable_record->selectkind)
  {
  case Vertselect:
  case Edgeselect:
  case Faceselect:
  case Shellselect:
    selectable_record->selectboxtechnique = draw_normal_objects_select_box;
    break;
  case Inplanefaceselect:
  case Objselect:		/* should be replaced by an arrow anyways */
  case Nothingselect:
    selectable_record->selectboxtechnique = Nil;
    break;
  default:
    selectable_record->selectboxtechnique = draw_normal_objects_select_box;
    break;
  }
}

  void
set_cursor_by_selectable(selectableptr selectable_record)
{
  switch (selectable_record->selectkind)
  {
  case Vertselect:
  case Edgeselect:
  case Faceselect:
  case Shellselect:
    select_cursor(Pluscursor);
    break;
  case Objselect:		/* should be replaced by an arrow anyways */
    select_cursor(Pluscursor);
    break;
  default:
    select_cursor(Pluscursor);
    break;
  case Nothingselect:
    break;
  }
}

  
  static int
find_select_item(stateptr state, shellptr thishell, selectableptr thiselect)
{
  listptr seglist;
  ringptr thisring;
  selectabletype testselect;
  segmentptr thisegment;
  GLboolean firstselect = TRUE;
  ;
  thisring = thishell->ringlist->first.ring;
  while (thisring)
  {
    seglist = thisring->seglist;
    for (thisegment = seglist->first.seg; thisegment != Nil;
	 thisegment = thisegment->next)
      if (cursor_near_feature(thisegment,state,&testselect))
      {
	if (firstselect)
	{
	  *thiselect = testselect;
	  firstselect = FALSE;
	}
	else if (thiselect->selectkind > testselect.selectkind)
	  *thiselect = testselect;
	
	if (testselect.selectkind == Vertselect)
	  return (Vertselect); /* if a vertex is selectable, stop search */
      }
    thisring = thisring->next;
  }

  return(thiselect->selectkind);
}

  int
determine_selectable(shellptr thishell,stateptr state,selectableptr foundselect)
{
  int foundkind = Nothingselect;
  ;
  foundselect->thefeature = Nil;
  foundselect->selectkind = foundkind;

  if (thishell->ringlist->numelems == 0)
    return(foundkind);		/* do not check me! */
  
  /* cursor inside 2D bbox around section line(s) */
  if (cursor_near_shell(thishell,state))
  {
    /* determine what feature is selectable */
    foundkind = find_select_item(state,thishell,foundselect);
    /* in all cases except inplane faces, shell select (the cursor is */
    /* inside the shell) take precedence */
/*
    if (foundkind != Inplanefaceselect)
*/
    if (foundkind == Nothingselect)
      if (cursor_in_shell(thishell,state))
      {
	foundkind = Shellselect;
	foundselect->thefeature = (featureptr) thishell;
	foundselect->selectkind = foundkind;
	copypos3d(state->cursor,foundselect->nearestpos);
      }
  }
  /* Promote the selectable kind to Shellselect if the motherobj has */
  /* the lockedgeometry property.*/
  if (has_property((featureptr) thishell->motherobj,lockedgeometry_prop) &&
      (foundkind != Nothingselect))
  {
    foundselect->thefeature = (featureptr) thishell;
    foundselect->selectkind = foundkind = Shellselect;
  }
      
  return (foundkind);
}

/* note: when/if we have pockets must be smarter about cursor in shell check */
/* to make sure we really are inside the object and not inside a hole */

/* selectechnique used ONLY by objects */
  void
set_object_selectable(objptr thisobj, stateptr state)
{
  featureptr old_selectable_feature = Nil;
  int old_selectkind,new_selectkind;
  selectabletype foundselect;
  shellptr thishell;
  objptr erase_obj;
  ;
  old_selectable_feature = thisobj->selectable_feature.thefeature;
  old_selectkind = thisobj->selectable_feature.selectkind;
  /* in here we see if selectability has changed in terms of feature kind or */
  /* status ... if it has we either add or delete or delete&add */
  /* selectability prop, if it hasnt we do nothing */
  thishell = First_obj_shell(thisobj);
  while (thishell != Nil)
  {
    new_selectkind = determine_selectable(thishell,state,&foundselect);
    /* if cursor is outside or on the shell we're done */
    if (new_selectkind != Shellselect)
      break;
    thishell = thishell->next;
  }
  if (new_selectkind == Shellselect)
  {		/* promote shells to obj since cant really select shells */
    new_selectkind = foundselect.selectkind = Objselect;
    foundselect.thefeature = (featureptr)
                                ((shellptr) foundselect.thefeature)->motherobj;
  }
  if (new_selectkind == Nothingselect)
  {
    if (old_selectkind != Nothingselect)
    {
      del_property(old_selectable_feature,selectable_prop);
      if (old_selectkind == Objselect)
      {
	erase_obj = get_unlinked_ancestor(thisobj);
	log_3d_erase_bbox((featureptr) erase_obj,&(erase_obj->groupbbox),
			  Normalbboxmargin,Loginoldbuffer);
      }
      else
	log_3d_erase_bbox((featureptr) thisobj,&(thisobj->objbbox),
			  Normalbboxmargin,Loginoldbuffer);
    }
  }
  else
  {
    if ((old_selectkind != Nothingselect) &&
	(foundselect.thefeature != old_selectable_feature))
    {
      /* delete selectable prop from old selectable feature */
      /* and add selectable property to the selectable feature found */
      del_property(old_selectable_feature,selectable_prop);
      add_property(foundselect.thefeature,selectable_prop);
      if (old_selectkind == Objselect)
      {
	erase_obj = get_unlinked_ancestor(thisobj);
	log_3d_erase_bbox((featureptr) erase_obj,&(erase_obj->groupbbox),
			  Normalbboxmargin,Loginoldbuffer);
      }
      else
	log_3d_erase_bbox((featureptr) thisobj,&(thisobj->objbbox),
			  Normalbboxmargin,Loginoldbuffer);
    }
    else if (old_selectkind == Nothingselect)
    {
      /* add selectable property to the selectable feature found */
      add_property(foundselect.thefeature,selectable_prop);
      if (old_selectkind == Objselect)
      {
	erase_obj = get_unlinked_ancestor(thisobj);
	log_3d_erase_bbox((featureptr) erase_obj,&(erase_obj->groupbbox),
			  Normalbboxmargin,Loginoldbuffer);
      }
      else
	log_3d_erase_bbox((featureptr) thisobj,&(thisobj->objbbox),
			  Normalbboxmargin,Loginoldbuffer);
    }
  }
  set_normal_selectbox_technique(&foundselect);
  set_cursor_by_selectable(&foundselect);
  thisobj->selectable_feature = foundselect;
}

  void
set_feature_selectable(featureptr thisfeature, stateptr state)
{
  objptr thisobj;
  fveptr thisfve;
  evfptr thisevf;
  vfeptr thisvfe;
  ;
  /* note: thisfeature is any visible feature at this point we are just */
  /* going through the visible list and determining if a feature is selectable */
  /* or not...in almost all cases this feature will be an object, since it is */
  /* the rare case indeed when only a subfeature of an object is visible */
  switch (thisfeature->type_id)
  {
  case Vfe_type:
    thisvfe = (vfeptr) thisfeature;
    if (thisvfe->selectechnique != Nil) {
      fprintf(dbgdump, "set_feature_selectable:vfe_select routine %p\n", (void *) thisvfe->selectechnique);
      fflush(dbgdump);
      (*(thisvfe->selectechnique))(thisvfe, state);
    }
    break;
  case Evf_type:
    thisevf = (evfptr) thisfeature;
    if (thisevf->selectechnique != Nil)
      (*(thisevf->selectechnique))(thisevf, state);
    break;
  case Fve_type:
    thisfve = (fveptr) thisfeature;
    if (thisfve->selectechnique != Nil)
      (*(thisfve->selectechnique))(thisfve, state);
    break;
  case Obj_type:
    thisobj = (objptr) thisfeature;
    if (thisobj->selectechnique != Nil)
      (*(thisobj->selectechnique))(thisobj, state);
    break;
  }
}

  void
set_oneworld_selectables(worldptr world,stateptr state)
{
  proptr thisprop;
  featureptr thisfeature;
  ;
  /* Go through all visible and not nonselectable features and set those that */
  /* are selectable */
  thisprop = world->world_proplists[(int) visible_prop]->first.prop;
  while (thisprop != Nil)
  {
    thisfeature = (featureptr) thisprop->owner;
    /* do not run on non-selectable props */
    if (!has_property(thisfeature,nonselectable_prop))
      set_feature_selectable(thisfeature,state);
    thisprop = thisprop->next;
  }
}

/* select an entire object no matter what, with no conditions... used for */
/* the cursor_obj, cutplane_obj, etc. Only called once for those, then */
/* the selectability routine used subsequently for those objects during */
/* system operation is set_always_selectable, below. */
/* Assume the selectable_feature.thefeature ptr is bogus. */

  void
select_obj(objptr selected_obj)
{
  selected_obj->selectable_feature.thefeature = (featureptr) selected_obj;
  selected_obj->selectable_feature.selectkind = Objselect;
  add_property((featureptr) selected_obj,selectable_prop);
  log_3d_erase_bbox((featureptr) selected_obj,&(selected_obj->objbbox),
		    Normalbboxmargin,Loginoldbuffer);
}

  void
unselect_obj(objptr unselected_obj)
{
  featureptr old_selected_feature;
  objptr thisobj;
  ;
  old_selected_feature = unselected_obj->selectable_feature.thefeature;
  if (old_selected_feature != Nil)
    if (has_property(old_selected_feature, selectable_prop))
    {
      del_property(old_selected_feature,selectable_prop);
      thisobj = (objptr) find_largergrain((featureptr) old_selected_feature,
					  Obj_type);
/*
      log_object_erase_bbox(thisobj,Usegroupbbox,Loginoldbuffer);
*/
      log_object_erase_bbox(thisobj,Useobjbbox,Loginoldbuffer);
    }
  
  unselected_obj->selectable_feature.thefeature = Nil;
  unselected_obj->selectable_feature.selectkind = Nothingselect;
}


  GLboolean
stationary(stateptr oldstate,stateptr state)
{
  vertype changevec;
  ;
  diffpos3d(oldstate->cursor,state->cursor,changevec);
  return (vector_near_zero(changevec,1.0));
}

  GLboolean
accelerating(stateptr oldstate,stateptr state)
{
  static vertype previouscursor = {0.0,0.0,0.0};
  vertype accelvec;
  GLfloat accelmag;
  ;
  accelvec[vx] =
    state->cursor[vx] - 2.0 * oldstate->cursor[vx] + previouscursor[vx];
  accelvec[vy] =
    state->cursor[vy] - 2.0 * oldstate->cursor[vy] + previouscursor[vy];
  accelvec[vz] =
    state->cursor[vz] - 2.0 * oldstate->cursor[vz] + previouscursor[vz];
  copypos3d(oldstate->cursor,previouscursor);
  accelmag = magvec(accelvec);

  return(accelmag >= cursor_accel_tolerance);
}

  GLboolean
accelerating_z(stateptr oldstate,stateptr state)
{
  static vertype previouscursor = {0.0,0.0,0.0};
  GLfloat accelval;
  ;
  accelval = state->cursor[vz] - 2.0 * oldstate->cursor[vz] + previouscursor[vz];
  copypos3d(oldstate->cursor,previouscursor);

  return(Fabs(accelval) >= cursor_accel_tolerance*4);
}

/* Find the unique selectable item for each shell in all worlds. */

  void
set_selectables(listptr world_list,stateptr state,stateptr oldstate)
{
  GLfloat cursor_dist;
  ;
  if (always_set_selectable)
  {
    set_oneworld_selectables(environment_world,state);
    set_oneworld_selectables(working_world,state);
    do_cursor_deselect(world_list);
    fprintf(dbgdump, "set_selectables: always_set_selectable\n");
  }
  else if (!accelerating(oldstate,state))
  {
    cursor_dist = distance2(oldstate->cursor,state->cursor);
    if (cursor_dist < cursor_move_tolerance)
    {
#if 0
      fprintf(dbgdump, "set_selectables: cursor_move_tolerance\n");
      fflush(dbgdump);
#endif

      set_oneworld_selectables(environment_world,state);
      set_oneworld_selectables(working_world,state);
      do_cursor_deselect(world_list);
    }
  }
}

/* Set all the selectables, no matter what. Called when user generates a */
/* sudden event without waiting for the system to run the above routine */
/* at least once. */

  void
set_selectables_definite(listptr world_list,stateptr state,stateptr oldstate)
{
  set_oneworld_selectables(environment_world,state);
  set_oneworld_selectables(working_world,state);
  do_cursor_deselect(world_list);
}

  void
set_global_picksize(GLfloat newpicksize)
{
  global_picksize = newpicksize;
}

  void
set_always_selectable(void)
{
  /* does nothing */
}

  void
set_cutplane_selectable(void)
{
}

  void
set_room_selectable(void)
{
}

 static GLboolean
cursor_near_innercircle(stateptr state,GLfloat *centerpt)
{
  GLfloat cursor_to_circle;
  ;
  cursor_to_circle = distance2(centerpt,state->cursor);
  return (cursor_to_circle <= (Rotateiconctrsize * Rotateiconctrsize));
}


/* NOTE: we always check to see if something is ALREADY selectable before */
/* setting it to selectable again... this reduces slowness when moving away */
/* from the thing because no extra erase_bboxes are lying around from setting */
/* it to be selectable over and over just cause the cursor's proximate. (lje */
/* word). */

  void
set_rotate_innercircle_selectable(objptr inner_circle,stateptr state)
{
  GLfloat *centerpt;
  ;
  centerpt = First_obj_vfe(inner_circle)->pos;
  if (has_property((featureptr) inner_circle,visible_prop))
  {
    if (cursor_near_innercircle(state,centerpt))
    {
      if (!has_property((featureptr) inner_circle,selectable_prop))
      {
	copypos3d(First_obj_vfe(inner_circle)->pos,
		  inner_circle->selectable_feature.nearestpos);
	inner_circle->selectable_feature.thefeature = (featureptr) inner_circle;
	inner_circle->selectable_feature.selectkind = Objselect;
	add_property((featureptr) inner_circle,selectable_prop);
	log_3d_erase_bbox((featureptr) rotatetool_obj,
			  &(rotatetool_obj->groupbbox),
			  Normalbboxmargin,Loginoldbuffer);
      }
    }
    else
      unselect_obj(inner_circle);
  }
}

  void
set_rotatexbar_selectable(objptr rotate_xbar,stateptr state)
{
  GLfloat *pt1,*pt2;
  double dummyt;
  vertype nearpt,rotatexbarmidpt;
  selectableptr selectable_feature;
  featureptr old_selectable_feature;
  vfeptr selectvfe,endvfe1,endvfe2;
  evfptr xbarevf,selectevf;
  ;
  xbarevf = First_obj_evf(rotate_xbar);
  endvfe1 = First_obj_vfe(rotate_xbar);
  endvfe2 = endvfe1->next;
  pt1 = endvfe1->pos;
  pt2 = endvfe2->pos;
  midpoint(pt1,pt2,rotatexbarmidpt);
  selectable_feature = &(rotate_xbar->selectable_feature);
  old_selectable_feature = selectable_feature->thefeature;
  if ((pt_near_pt_3d(state->cursor,pt1,Baublesize)) &&
      (has_property((featureptr) endvfe1,visible_prop)))
  {
    selectvfe = endvfe1;
    /* only set it to selectable if not already set or required (eg */
    /* during dragging) */
    if (!has_property((featureptr) selectvfe,selectable_prop) ||
	always_set_selectable)
    {
      copypos3d(pt1,selectable_feature->nearestpos);
      selectable_feature->thefeature = (featureptr) selectvfe;
      selectable_feature->selectkind = Vertselect;
      if (old_selectable_feature != Nil) /* could have been some other feature */
	del_property(old_selectable_feature,selectable_prop);
      add_property((featureptr) selectvfe,selectable_prop);
      log_3d_erase_bbox((featureptr) rotatetool_obj,&(rotatetool_obj->groupbbox),
			Normalbboxmargin,Loginoldbuffer);
    }
    else			/* you still need to update the nearestpos */
      copypos3d(pt1,selectable_feature->nearestpos);
      
  }
  else if ((pt_near_pt_3d(state->cursor,pt2,Baublesize)) &&
	   (has_property((featureptr) endvfe2,visible_prop)))
  {
    selectvfe = endvfe2;
    /* only set it to selectable if not already set or required (eg */
    /* during dragging) */
    if (!has_property((featureptr) selectvfe, selectable_prop) ||
	always_set_selectable)
    {
      copypos3d(pt2,selectable_feature->nearestpos);
      selectable_feature->thefeature = (featureptr) selectvfe;
      selectable_feature->selectkind = Vertselect;
      if (old_selectable_feature != Nil) /* could have been some other feature */
	del_property(old_selectable_feature,selectable_prop);
      add_property((featureptr) selectvfe,selectable_prop);
      log_3d_erase_bbox((featureptr) rotatetool_obj,&(rotatetool_obj->groupbbox),
			Normalbboxmargin,Loginoldbuffer);
    }
    else			/* you still need to update the nearestpos */
      copypos3d(pt2,selectable_feature->nearestpos);
  }
  else if ((pt_near_lineseg_3d(pt1,pt2,state->cursor,nearpt,
			       &dummyt,Dragbaublesize)) &&
	   (has_property((featureptr) xbarevf,visible_prop)))
  {
    selectevf = xbarevf;
    /* only set it to selectable if not already set or required (eg */
    /* during dragging) */
    if (!has_property((featureptr) selectevf,selectable_prop) ||
	always_set_selectable)
    {
      copypos3d(nearpt,selectable_feature->nearestpos);
      selectable_feature->thefeature = (featureptr) selectevf;
      selectable_feature->selectkind = Edgeselect;
      if (old_selectable_feature != Nil) /* could have been some other feature */
	del_property(old_selectable_feature,selectable_prop);
      add_property((featureptr) selectevf,selectable_prop);
      log_3d_erase_bbox((featureptr) rotatetool_obj,&(rotatetool_obj->groupbbox),
			Normalbboxmargin,Loginoldbuffer);
    }
    else			/* you still need to update the nearestpos */
      copypos3d(nearpt,selectable_feature->nearestpos);
  }
  else if (selectable_feature->thefeature != Nil)
    unselect_obj(rotate_xbar);

}


  void
set_rotate_outercircle_selectable(objptr outer_circle,stateptr state)
{
  GLfloat cursor_to_circle;
  GLfloat *centerpt;
  vertype nearpt,nearvec;
  ;
  if (has_property((featureptr) outer_circle,visible_prop))
  {
    centerpt = First_obj_vfe(outer_circle)->pos;
    cursor_to_circle = distance2(centerpt,state->cursor);
    if ((cursor_to_circle >=
	 ((rotatetool_radius - Baublesize) *
	  (rotatetool_radius - Baublesize))) &&
	(cursor_to_circle <=
	 ((rotatetool_radius + Baublesize) *
	  (rotatetool_radius + Baublesize))))
    {
      diffpos3d(centerpt,state->cursor,nearvec);
      normalize(nearvec,nearvec);
      scalevec3d(nearvec,rotatetool_radius,nearvec);
      addvec3d(centerpt,nearvec,nearpt);
    }
    else
    {
      unselect_obj(outer_circle);
      return;
    }
#if 0
    copypos3d(nearpt,outer_circle->selectable_feature.nearestpos);
    outer_circle->selectable_feature.thefeature = (featureptr) outer_circle;
    outer_circle->selectable_feature.selectkind = Objselect;
    add_property((featureptr) outer_circle,selectable_prop);
#endif
  }
}

  void
set_systemcursor_selectable(void)
{
}

  void
set_toolpad_selectable(objptr toolpad, stateptr state)
{
  bbox3dtype toolpadbbox;
  static vertype cursoroffset;
  ;
  toolpadbbox = toolpad->objbbox; /* hack: depends on ansi record copy! */
  toolpadbbox.mincorner[vx] -= Toolpadsquare;
  toolpadbbox.mincorner[vy] += Toolpadsquare/2.0;
  toolpadbbox.mincorner[vz] -= Toolpadsquare;
  toolpadbbox.maxcorner[vz] += Toolpadsquare;
  if (point_in_3dbbox(state->cursor,&toolpadbbox))
  {
    /* only set it to selectable if not already set or required (eg */
    /* during dragging) */
    if (!has_property((featureptr) toolpad,selectable_prop) ||
	always_set_selectable)
    {
      copypos3d(state->cursor,toolpad->selectable_feature.nearestpos);
      toolpad->selectable_feature.thefeature = (featureptr) toolpad;
      toolpad->selectable_feature.selectkind = Objselect;
      set_normal_selectbox_technique(&(toolpad->selectable_feature));
      add_property((featureptr) toolpad,selectable_prop);
      log_3d_erase_bbox((featureptr) toolpad,&(toolpad->objbbox),
			Normalbboxmargin,Loginoldbuffer);
      diffpos3d(First_obj_vfe(cursor_obj)->pos,
		First_obj_vfe(toolpad)->pos,cursoroffset);
      cursoroffset[vx] = cursoroffset[vy] = 0.0;
      cursoroffset[vz] += Decalthickness*2+10.0;
      offset_cursor_image(cursoroffset);
      select_cursor(Arrowcursor);
    }
  }
  else
  {
    if (has_property((featureptr) toolpad,selectable_prop))
      unoffset_cursor_image(default_cursor_offset);
    unselect_obj(toolpad);
  }
}

  void
set_headpad_selectable(objptr headpad, stateptr state)
{
  bbox3dtype headpadbbox;
  static vertype cursoroffset;
  ;
  headpadbbox = headpad->objbbox; /* hack: depends on ansi record copy! */
  headpadbbox.mincorner[vx] -= Headpadcorner * 4.0;
  headpadbbox.mincorner[vz] -= Headpadsquare;
  headpadbbox.mincorner[vy] -= 4.0*Headpadcorner;
  headpadbbox.maxcorner[vz] += Headpadsquare;
  if (point_in_3dbbox(state->cursor,&headpadbbox))
  {
    /* only set it to selectable if not already set or required (eg */
    /* during dragging) */
    if (!has_property((featureptr) headpad,selectable_prop) ||
	always_set_selectable)
    {
      copypos3d(state->cursor,headpad->selectable_feature.nearestpos);
      headpad->selectable_feature.thefeature = (featureptr) headpad;
      headpad->selectable_feature.selectkind = Objselect;
      set_normal_selectbox_technique(&(headpad->selectable_feature));
      add_property((featureptr) headpad,selectable_prop);
      log_3d_erase_bbox((featureptr) headpad,&(headpad->objbbox),
			Normalbboxmargin,Loginoldbuffer);
      diffpos3d(First_obj_vfe(cursor_obj)->pos,
		First_obj_vfe(headpad)->pos,cursoroffset);
      cursoroffset[vx] = cursoroffset[vy] = 0.0;
      cursoroffset[vz] += Decalthickness*2+10.0;
      offset_cursor_image(cursoroffset);
      select_cursor(Arrowcursor);
    }
  }
  else
  {
    if (has_property((featureptr) headpad,selectable_prop))
      unoffset_cursor_image(default_cursor_offset);
    unselect_obj(headpad);
  }
}

  void
set_normalpad_selectable(objptr normalpad,stateptr state)
{
  bbox3dtype normalpadbbox;
  static vertype cursoroffset;
  ;
  normalpadbbox = normalpad->objbbox; /* hack: depends on ansi record copy! */
  normalpadbbox.mincorner[vz] -= Normalpadsquare;
  normalpadbbox.maxcorner[vz] += Normalpadsquare;
  if (point_in_3dbbox(state->cursor,&normalpadbbox))
  {
    /* only set it to selectable if not already set or required (eg */
    /* during dragging) */
    if (!has_property((featureptr) normalpad,selectable_prop) ||
	always_set_selectable)
    {
      copypos3d(state->cursor,normalpad->selectable_feature.nearestpos);
      normalpad->selectable_feature.thefeature = (featureptr) normalpad;
      normalpad->selectable_feature.selectkind = Objselect;
      set_normal_selectbox_technique(&(normalpad->selectable_feature));
      add_property((featureptr) normalpad,selectable_prop);
      log_3d_erase_bbox((featureptr) normalpad,&(normalpad->objbbox),
			Normalbboxmargin,Loginoldbuffer);
      diffpos3d(First_obj_vfe(cursor_obj)->pos,
		First_obj_vfe(normalpad)->pos,cursoroffset);
      cursoroffset[vx] = cursoroffset[vy] = 0.0;
      cursoroffset[vz] += Decalthickness*2+10.0;
      offset_cursor_image(cursoroffset);
      if (has_property((featureptr) normalpad,textinput_prop))
	select_cursor(Ibeamcursor);
      else
	select_cursor(Arrowcursor);
    }
  }
  else
  {
    if (has_property((featureptr) normalpad,selectable_prop))
      unoffset_cursor_image(default_cursor_offset);
    unselect_obj(normalpad);
  }
}

  void
set_scalebbox_selectable(objptr scalebbox, stateptr state)
{
  vfeptr thisvfe;
  evfptr thisevf;
  int old_selectkind;
  selectabletype foundselect;
  GLboolean logging = FALSE;
  ;
  thisevf = First_obj_evf(scalebbox);
  foundselect.selectkind = Nothingselect;
  thisvfe = First_obj_vfe(scalebbox);
  while (thisvfe != Nil)
  {
    fprintf(dbgdump, "set_scalebbox_selectable:thisvfe=%p\n", (void *) thisvfe);
    fflush(dbgdump);
    if (pt_near_pt(thisvfe->pos,state->cursor,Pkboxsize))
    {
      foundselect.thefeature = (featureptr) thisvfe;
      foundselect.selectkind = Vertselect;
      break;
    }
    thisvfe = thisvfe->next;
  }      
  if (foundselect.selectkind == Nothingselect)
  {
    while (thisevf != Nil)
    {
      if (abs(thisevf->cutype) == Wascut)
      {
	if (pt_near_pt(thisevf->cutpt,state->cursor,Pkboxsize))
	{
	  foundselect.thefeature = (featureptr) thisevf;
	  foundselect.selectkind = Edgeselect;
	  break;
	}
      }
      thisevf = thisevf->next;
    }
  }
  /* clear the old selectable record if changed. */
  old_selectkind = scalebbox->selectable_feature.selectkind;
  if (old_selectkind != Nothingselect)
  {
    if ((foundselect.selectkind == Nothingselect) ||
	(foundselect.thefeature != scalebbox->selectable_feature.thefeature))
    {
      unselect_obj(scalebbox);
      logging = TRUE;
    }
  }
  /* set the new select if different and not nothing */
  if ((foundselect.selectkind != Nothingselect) &&
      (foundselect.selectkind != old_selectkind))
  {
				/* Added 12/5/2001 by WAK to fix weird bug */
				/* that only seems to crop up in borland, */
				/* ie, foundselect's selectboxtechnique */
				/* routine pointer is not getting set. */

    set_normal_selectbox_technique(&foundselect);

    scalebbox->selectable_feature = foundselect;
    add_property(foundselect.thefeature,selectable_prop);
    select_cursor(Arrowcursor);
    logging = TRUE;
  }

  if (logging)
    /* change to log_object_erase_bbox, and use the screenbboxtechnique */
    /* to only erase the squares... */
    log_3d_erase_bbox((featureptr) scalebbox,&(scalebbox->groupbbox),
		      Normalbboxmargin,Loginoldbuffer);
}

