
/* FILE: update.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*                 Updating Routines for the Cutplane Editor              */
/*                                                                        */
/* Author: WAK                                       Date: April 30,1990  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 William Kessler and Laurence Edwards.*/
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define UPDATEMODULE

#include <config.h>
#include <stdio.h>
#include <stdlib.h>		// for realloc()
#include <float.h>
#include <math.h>
#include <limits.h>

#include <platform_gl.h>
#include <platform_glut.h>

#include "device.h"

#include <cutplane.h>		/* cutplane.h includes user.h */
#include <pick.h>		/* for access to global_pickedlist */
#include <globals.h>
#include <grafx.h>
#include <update.h>
#include <controls.h>
#include <cpl_cutplane.h>

#include <demo.h>

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			GLOBAL VARIABLES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

static FILE *dbgfile;

GLboolean drag_origin_set = FALSE,wait_for_good_drag = FALSE;
GLboolean already_valid_drag = FALSE;
vertype drag_origin;

int constrain_scan_count = 0;

long global_update_flags;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			LOW LEVEL FUNCTIONS				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


  double
enforce_flimits(double fvalue,double fmin,double fmax)
{
  if (fvalue > fmax)
    return (fmax);
  else if (fvalue < fmin)
    return (fmin);
  return(fvalue);
}

  int
enforce_ilimits(int ivalue,int imin,int imax)
{
  if (ivalue > imax)
    return (imax);
  else if (ivalue < imin)
    return (imin);
  return(ivalue);
}

  long
enforce_llimits(long lvalue,long lmin,long lmax)
{
  if (lvalue > lmax)
    return (lmax);
  else if (lvalue < lmin)
    return (lmin);
  return(lvalue);
}

  void
constrain_movecrosshair(vertype delta)
{
  static vertype avgdelta;
  static int maskedaxis;
  ;
  /* Look for direction of constraint by building up averages while holding */
  /* the cursor constant */
  if (compare(delta[vz],0.0,Onplanetol) != 0)
    return;			/* no normal constrains for Z movement... */
  if (constrain_scan_count < Maxconstrainscans)
  {
    if (constrain_scan_count == 0)
      copypos3d(origin,avgdelta);
    avgdelta[vx] += Fabs(delta[vx]);
    avgdelta[vy] += Fabs(delta[vy]);
    avgdelta[vz] += Fabs(delta[vz]);
    copypos3d(origin,delta);
    constrain_scan_count++;
    return;
  }
  /* Start constraining. Pick an axis of constraint. */
  else if (constrain_scan_count == Maxconstrainscans)
  {
    scalevec3d(avgdelta,(1.0 / (Maxconstrainscans)),avgdelta);
    maskedaxis = vx;
    if (Fabs(avgdelta[vx]) > Fabs(avgdelta[vy]))
      maskedaxis = vy;
    constrain_scan_count++;
  }
  /* if already constraining, then check to see if moved less than half */
  /* a Pkbox. If yes, then reset constraining so that it watches for a */
  /* new direction. */
  else if ((Fabs(delta[vx]) < Pkboxsize/2.0) &&
	   (Fabs(delta[vy]) < Pkboxsize/2.0))
  {
    constrain_scan_count = 0;
    copypos3d(origin,delta);
    return;
  }

  delta[maskedaxis] = 0.0;
}

  void
compute_crosshair_move(vertype delta,vertype actualdelta,stateptr state)
{
  vertype prevcursor,newcursor;
  ;
  if (drawflags & Drawroom)
  {
    copypos3d(state->cursor,prevcursor);
    
    /* Update cursor and enforce cursor/plane limits. */
    newcursor[vx] = enforce_flimits(state->cursor[vx] + delta[vx],
				    Mincrossx,Maxcrossx);
    newcursor[vy] = enforce_flimits(state->cursor[vy] + delta[vy],
				    Mincrossy,Maxcrossy);
    newcursor[vz] = enforce_flimits(state->cursor[vz] + delta[vz],
				    Minplanez,Maxplanez);
    
    /* Compute how much cursor/plane ACTUALLY moved */
    diffpos3d(prevcursor,newcursor,actualdelta);
  }
  else
  {
    copypos3d(state->cursor,prevcursor);
    
    /* Update cursor and enforce cursor/plane limits. */
    newcursor[vx] = state->cursor[vx] + delta[vx];
    newcursor[vy] = state->cursor[vy] + delta[vy];
    newcursor[vz] = state->cursor[vz] + delta[vz];
    
    /* Compute how much cursor/plane ACTUALLY moved */
    diffpos3d(prevcursor,newcursor,actualdelta);
  }
}
  
/* Updating the help/status windows */

#if RECODEME
  void
update_help_window(actionkind theaction)
{
  draw_help_window(TRUE,TRUE,TRUE);
}

  void
update_info_window(stateptr state,actionkind theaction)
{
  static GLfloat oldx = 10.0,oldy = 0.0, oldz = 0.0;
  ;
  if ((state->cursor[vx] != oldx) ||
      (state->cursor[vy] != oldy) || (state->cursor[vz] != oldz))
  {
    draw_info_window(state);
    oldx = state->cursor[vx];
    oldy = state->cursor[vy];
    oldz = state->cursor[vz];
  }
}
#endif

/* Put me in zoom.c */

  void
zoom_room_view(vertype center, double scale,stateptr state)
{
}

  void
rotate_room_view(int xamount,int yamount,stateptr state)
{
  state->roomrotx = enforce_ilimits(state->roomrotx + xamount,
				    Minroomrotx,Maxroomrotx);
  state->roomroty = enforce_ilimits(state->roomroty + yamount,
				    Minroomroty,Maxroomroty);
  /* remake the matrix */
  make_vanilla_matrix(state->roomrotx,state->roomroty); 
}

  
  void
set_drag_origin(stateptr state)
{
  if (!drag_origin_set)
  {
    constrain_scan_count = 0;
    drag_origin_set = TRUE;
    wait_for_good_drag = TRUE;
    copypos3d(state->cursor,drag_origin);
  }
}

  void
clear_drag_origin(void)
{
  drag_origin_set = already_valid_drag = FALSE;
}

  GLboolean
drag_valid(vertype newcursorpos)
{
  /* To be validly dragging something, you must have the drag origin set and */
  /* (the plane not within Onplanetol of the position the drag button went */
  /* down on OR the x or y coords more than Pkboxsize away from the place */
  /* where the drag button went down, unless you've already exceeded that */
  /* distance). */
  if (!already_valid_drag)
    already_valid_drag =
      ((compare(newcursorpos[vz] - drag_origin[vz],0.0,Onplanetol) != 0)
	|| (Fabs(newcursorpos[vx] - drag_origin[vx]) > Pkboxsize / 2.0) ||
	(Fabs(newcursorpos[vy] - drag_origin[vy]) > Pkboxsize/2.0));
  return (drag_origin_set && already_valid_drag);
}

  GLboolean
first_valid_drag(stateptr state,vertype extra_offset)
{
  if (wait_for_good_drag)
  {
    wait_for_good_drag = FALSE;
    diffpos3d(drag_origin,state->cursor,extra_offset);
    return(TRUE);
  }
  return(FALSE);
}

  GLboolean
currently_dragging(void)
{
  return (drag_origin_set && !wait_for_good_drag);
}
  
  void
set_constrain(void)
{
}

  void
clear_constrain(void)
{

}

  void
do_cursor_updates(void)
{
  transform_obj(cursor_obj);
  clear_feature_translates((featureptr) cursor_obj);
  set_3d_objbbox(cursor_obj);
  /* need to grow_group_bbox here, without actually translating the cursor's */
  /* children...*/
}

  void
unlog_updates(featureptr unlogged_feature)
{
  sortptr update_record,next_update_record;
  ;
  if (has_property(unlogged_feature,quickupdate_prop))
    del_property(unlogged_feature,quickupdate_prop);
  if (has_property(unlogged_feature,regularupdate_prop))
    del_property(unlogged_feature,regularupdate_prop);
}

  void
remove_killed_features(listptr world_list,stateptr state)
{
  featureptr doomed_feature;
  objptr mother_doomed;
  GLboolean deleted_one = FALSE, dorecombine;
  fveptr newfve;
  int doomedtype;
  worldptr world;
  ;
  world = world_list->first.world;
  while (world != Nil)
  {
    if (world->world_proplists[killed_prop]->numelems > 0)
      reset_nearpts_list();	/* so we don't snap plane to deleted things */
    if (world->world_proplists[killed_prop]->numelems > 0)
    {
      while ((doomed_feature = get_property_feature(world,killed_prop,1)) != Nil)
      {
	deleted_one = TRUE;
	dorecombine = TRUE;
	/* clearing the properties will also clear any update records. */
	clear_feature_properties(doomed_feature);
	doomedtype = doomed_feature->type_id;
	if (doomedtype != Obj_type)
	{			/* don't clear selectables if deleting object!*/
	  mother_doomed = (objptr) find_largergrain(doomed_feature,Obj_type);
	  ((objptr) mother_doomed)->selectable_feature.thefeature = Nil;
	  ((objptr) mother_doomed)->selectable_feature.selectkind =
	    Nothingselect;
	}
	else
	  mother_doomed = (objptr) doomed_feature;
	
	switch (doomedtype)
	{
	case Vfe_type:
	  dorecombine = (((vfeptr) doomed_feature)->startle->
			 motherbound->motherfve->
			 mothershell->evflist->numelems < 2);
	  newfve = delete_vertex(((vfeptr) doomed_feature)->startle);
	  if (newfve && (newfve->boundlist->first.bnd->lelist->numelems > 3) &&
	      !planar_face_nonormal(newfve))
	    triangularize_fve(newfve);
	  break;
	case Evf_type:
	  dorecombine = (((evfptr) doomed_feature)->le1->motherbound->motherfve->
			 mothershell->evflist->numelems < 2);
	  newfve = delete_edge(((evfptr) doomed_feature)->le1);
	  if (newfve && (newfve->boundlist->first.bnd->lelist->numelems > 3) &&
	      !planar_face_nonormal(newfve))
	    triangularize_fve(newfve);
	  break;
	case Fve_type:
	  dorecombine = (((fveptr) doomed_feature)->mothershell->
			 fvelist->numelems >
			 2);
	  newfve = delete_face((fveptr) doomed_feature);
	  if (newfve && (newfve->boundlist->first.bnd->lelist->numelems > 3) &&
	      !planar_face_nonormal(newfve))
	    triangularize_fve(newfve);
	  break;
	case Shell_type:
	  dorecombine = FALSE;
	  del_elem(mother_doomed->shellist,(elemptr) doomed_feature);
	  break;
	case Obj_type:
	  dorecombine = FALSE;
	  kill_obj(mother_doomed);
	  break;
	default:
	  dorecombine = FALSE;
	  break;
	}
	if (dorecombine)
	  recombine_coplanar_faces(mother_doomed);
      }
    }
    world = world->next;
  }
  if (deleted_one)
    global_update_flags = Allupdateflags;	/* redo them all! */
}

  void
process_obj_update_engine(objptr motherobj,updateptr update_rec)
{
  stateptr updatestate,oldupdatestate;
  ;
  updatestate = &(update_rec->state);
  oldupdatestate = &(update_rec->oldstate);
  if (update_rec->update_flags & Transformflag)
    transform_obj(motherobj);

  if (update_rec->update_flags & Cleartranslatesflag)
    clear_feature_translates((featureptr) motherobj);

  if (!has_property((featureptr) motherobj,visible_prop))
    return;

  if (update_rec->update_flags & Computebboxflag)
  {
    setpos3d(motherobj->objbbox.mincorner,FLT_MAX,FLT_MAX,FLT_MAX);
    setpos3d(motherobj->objbbox.maxcorner,-FLT_MAX,-FLT_MAX,-FLT_MAX);
    set_3d_objbbox(motherobj);
  }
  
  if (update_rec->update_flags & Recomputebackfaceflag)
  {
    set_backface_flags = TRUE; /* global update but used only by grafx.c */
    /* compute the screenbbox's for drawing purposes: note that the */
    /* groupscreenbbox is set in process_feature_update and process_tree_update*/
    compute_object_screenbbox(&(motherobj->objbbox),
			      &(motherobj->objscreenbbox), Normalbboxmargin);
    if (!has_property((featureptr) motherobj, noshadow_prop))
      compute_shadow_screenbbox(&(motherobj->objbbox),
				&(motherobj->shadowscreenbbox),
				Shadowbboxmargin);
  }

  if (update_rec->update_flags & Redrawflag)
    log_feature_redraw((featureptr) motherobj);

  if ((!has_property((featureptr) motherobj,nonmanifold_prop)) &&
      (!has_property((featureptr) motherobj,group_prop)))
  {
    if (update_rec->update_flags & Findnormalsflag)
      find_obj_normals(motherobj);

    if (update_rec->update_flags & Evfeqnsflag)
      set_obj_evf_eqns(motherobj);

    if (update_rec->update_flags & Cutevfsflag)
      set_obj_evf_cutpts(motherobj,updatestate);

    if (update_rec->update_flags & Transformcutevfsflag)
      transform_obj_evf_cutpts(motherobj);

    if (update_rec->update_flags & Clearcutsectionflag)
	clear_obj_ringlist(motherobj);

    if ((update_rec->update_flags & Makecutsectionflag) &&
	must_make_obj_ringlist(motherobj,updatestate,oldupdatestate))
    {
      make_obj_ringlist(motherobj);
      log_feature_redraw((featureptr) motherobj);
    }

    if (update_rec->update_flags & Computeplanedistsflag)
      compute_obj_planedists(motherobj);

    if (update_rec->update_flags & Getselectableflag)
      set_object_selectable(motherobj,updatestate);

    /* global update but used only by user.c */
    if (update_rec->update_flags & Findplanenearptsflag)
      reset_nearpts_list();
  }
}


/* here you want to do an update on an obj containing the passed feature */

  void
process_feature_update(featureptr thisfeature,long updateflags,stateptr state,
		       stateptr oldstate)
{
  objptr motherobj;
  objptr grouproot;
  updatetype update_rec;
  ;
  if (thisfeature->type_id != Obj_type)
    motherobj = (objptr) find_largergrain(thisfeature,Obj_type);
  else
    motherobj = (objptr) thisfeature;

  update_rec.update_flags = updateflags;
  update_rec.state = *state;
  update_rec.oldstate = *oldstate;
  process_obj_update_engine(motherobj,&update_rec);

  if (updateflags & Computebboxflag)
  {
    grow_group_bbox(motherobj);
  }

  if (updateflags & Recomputebackfaceflag)
  {
    compute_object_screenbbox(&(motherobj->groupbbox),
			      &(motherobj->groupscreenbbox), Normalbboxmargin);
    compute_shadow_screenbbox(&(motherobj->groupbbox),
			      &(motherobj->shadowscreenbbox), Shadowbboxmargin);
  }

  if (updateflags & Logerasebboxflag)
    log_object_erase_bbox(motherobj,Usegroupbbox,Loginthisbuffer);
}

/* here you know you are passed an object (the head of a tree). */

  void
process_tree_update(objptr thisobj,long updateflags,stateptr state,
		    stateptr oldstate)
{
  objptr grouproot;
  updatetype update_rec;
  ;
  update_rec.update_flags = updateflags;
  update_rec.state = *state;
  update_rec.oldstate = *oldstate;
  grouproot = get_unlinked_ancestor(thisobj);
  apply_to_descendants(grouproot,process_obj_update_engine,&update_rec);

  /* this is an exception since it needs to be handled outside of recursion */
  if (updateflags & Computebboxflag)
  {
    grow_group_bbox(grouproot);
  }

  if (updateflags & Recomputebackfaceflag)
  {
    compute_object_screenbbox(&(grouproot->groupbbox),
			      &(grouproot->groupscreenbbox), Normalbboxmargin);
    compute_shadow_screenbbox(&(grouproot->groupbbox),
			      &(grouproot->shadowscreenbbox), Shadowbboxmargin);
  }

  if (updateflags & Logerasebboxflag)
    log_object_erase_bbox(grouproot,Usegroupbbox,Loginthisbuffer);
}


  void
process_updates(worldptr world,stateptr state,stateptr oldstate,
		property update_prop)
{
  featureptr update_feature;
  long update_flags;
  ;
  if (world->world_proplists[update_prop]->numelems > 0)
  {
    while ((update_feature = get_property_feature(world,update_prop,1)) != Nil)
    {
      update_flags = (get_property_val(update_feature,update_prop)).l;
      if (update_feature->type_id != Obj_type)
	process_feature_update(update_feature,update_flags,state,oldstate);
      else
	process_tree_update((objptr) update_feature,update_flags,state,oldstate);
      del_property(update_feature,update_prop);
    }
  }
}

  void
process_global_updates(listptr world_list,stateptr state,stateptr oldstate)
{
  proptr thisprop;
  worldptr thisworld;
  long tempflags;
  ;
  remove_killed_features(world_list,state);
  if (global_update_flags == 0)
    return;			/* no global updates necessary */
  /* Do updates in ALL worlds. */
  thisworld = world_list->first.world;
  while (thisworld != Nil)
  {
    thisprop = thisworld->world_proplists[(int) visible_prop]->first.prop;
    while (thisprop != Nil)
    {
      tempflags = global_update_flags;
      
      process_feature_update(thisprop->owner,tempflags,state,oldstate);
      thisprop = thisprop->next;
    }
    thisworld = thisworld->next;
  }
  if (global_update_flags & Redrawflag)
    log_global_refresh();
  global_update_flags = 0;    
}

  void
log_update(featureptr updatefeature,long updateflags,GLboolean immediately)
{
  int update_prop;
  long old_updateflags = 0;
  ;
  if (immediately)
    update_prop = quickupdate_prop;
  else
    update_prop = regularupdate_prop;

  if (has_property(updatefeature,update_prop))
    old_updateflags = (get_property_val(updatefeature,update_prop)).l;

  set_property(updatefeature,update_prop,(updateflags | old_updateflags));
}

  void
log_property_update(worldptr world,property logprop,long updateflags,
		    GLboolean immediately)
{
  featureptr thisprop;
  int n = 1;
  ;
  while ((thisprop = get_property_feature(world,logprop,n++)) != Nil)
    log_update(thisprop,updateflags,immediately);
}

  void
log_global_update(long updateflags)
{
  global_update_flags |= updateflags;
}
  
/* Poll all the input devices, update whatever necessary state vars */
/* have changed due to changes in the state of those devices, and generate */
/* input events based upon those changes.  Return TRUE if any messages */
/* are logged in the input events stack. */
/* The routines that update_input_events is calling are found in controls.c. */


  GLboolean
handle_user_events(stateptr state)
{
  GLboolean events_generated = FALSE;
  int qentry = 0;
  ;
  while (qtest())		/* process ALL pending events WAK 1/29/91 */
  {
    qentry = getqentry();
    
    // Generate any input events from single presses of toggle keys or
    // event keys like Duplicate, Delete, etc.
//     if (qentry != NULLDEV)
    if (qentry != 0)
    {
      events_generated = update_queued_buttons(&qentry);

      /* Update global setting of on/off state of buttons that are tied to */
      /* the polled valuators, i.e. affect the */
      /* interpretation of the polled valuators. */

      events_generated |= update_tied_buttons(qentry,state);
    }
  }

  update_nonqueued_tied_buttons(state);

  /* Poll the valuators, and generate input events based on changes in their */
  /* state while reflecting changes in polled buttons. If tied buttons were */
  /* pressed, make sure to update their histories; if they were released, */
  /* make sure to check for multiple clicks (see update_polled_valuators()).*/
  events_generated |=  update_polled_valuators(state);

  reset_multiple_clicks();

  return(events_generated);
}


/* Main non-graphics refresh loop. */
/* Gets all the user events, and transform them into CPL RAM code to make */
/* changes in the Cutplane environment.  Then, run the CPL parser if  */
/* necessary. Finally, process whatever update calls need to */
/* be made before final drawing is done. */

  void
new_update_state(listptr world_list,stateptr state,stateptr oldstate)
{
  if (handle_user_events(state)) /* user events (besides cursor move) */
				 /* generated: reset selectables before */
				 /* doing any CPL processing. */
    set_selectables_definite(world_list,state,oldstate);

  if (restart_CPLparser)
  {
    restart_CPLparser = FALSE;
    restart_CPL_parse(world_list,state,oldstate);
  }

  process_updates(environment_world,state,oldstate,quickupdate_prop);
  process_updates(environment_world,state,oldstate,regularupdate_prop);
  process_updates(working_world,state,oldstate,quickupdate_prop);
  process_updates(working_world,state,oldstate,regularupdate_prop);
  process_global_updates(world_list,state,oldstate);
}

  void
swap_erase_buffers(int framebuffer)
{
  erase_record_ptr temp_erase_records;
  ;
  temp_erase_records = old_erase_records[framebuffer];
  old_erase_records[framebuffer] = new_erase_records[framebuffer];
  new_erase_records[framebuffer] = temp_erase_records;

  new_erase_records[framebuffer]->max_logged_bbox = 0;
}

  void
log_global_refresh(void)
{
  global_refresh = 2;		/* force total redraw for the next two */
}				/* swapbuffers */

  GLboolean
already_logged(featureptr log_feature,bboxscreenptr new_screen_bbox,
	       erase_record_ptr this_erase_records)
{
  int i = 0;
  bboxscreenptr checkbbox;
  ;
  return(TRUE);
  while (i < this_erase_records->max_logged_bbox)
  {
    /* if this exact same bbox is logged already return TRUE. */
    /* It's already logged if it's the same feature and EXACTLY the same */
    /* bbox. You may have logged a different bbox for this feature */
    /* in this buffer; in this case we don't report already logged. */
    checkbbox = &(this_erase_records->erase_bboxes[i]);
    if ((checkbbox->mincorner[vx] == new_screen_bbox->mincorner[vx]) &&
	(checkbbox->mincorner[vy] == new_screen_bbox->mincorner[vy]) &&
	(checkbbox->maxcorner[vx] == new_screen_bbox->maxcorner[vx]) &&
	(checkbbox->maxcorner[vy] == new_screen_bbox->maxcorner[vy]))
/*  if ((featureptr) this_erase_records->logged_features[i] == log_feature)*/
      return(TRUE);
    i++;
  }
  return(FALSE);
}

  void
do_erase_log(erase_record_ptr this_erase_records,featureptr log_feature,
	     bboxscreenptr new_screen_bbox,bbox3dptr new_3d_bbox)
{
  bboxscreenptr log_screen_bbox;
  bbox3dptr log_3d_bbox;
  int current_log_index,i;
  ;
  current_log_index = this_erase_records->max_logged_bbox;
  if (current_log_index == this_erase_records->erase_bboxes_array_size)
  {
    /* if you run out of room, double array size! */
    this_erase_records->erase_bboxes =
      (bboxscreenptr) realloc(this_erase_records->erase_bboxes,
			      this_erase_records->erase_bboxes_array_size *
			      BBoxscreensize*2);

    this_erase_records->logged_features = (featureptr *)
      realloc(this_erase_records->logged_features,
	      this_erase_records->erase_bboxes_array_size *
	      sizeof(featureptr) * 2);
    this_erase_records->bbox_erased_already = (GLboolean *)
      realloc(this_erase_records->logged_features,
	      this_erase_records->erase_bboxes_array_size *
	      sizeof(GLboolean) * 2);
    this_erase_records->erase_bboxes_array_size *= 2;
    for (i = 0; i < this_erase_records->erase_bboxes_array_size; ++i)
      this_erase_records->bbox_erased_already[i] = FALSE;
  }
  /* Record screen bbox. */
  log_screen_bbox = &(this_erase_records->
		      erase_bboxes[current_log_index]);
  setscreenpos(log_screen_bbox->mincorner,new_screen_bbox->mincorner[vx],
	       new_screen_bbox->mincorner[vy]);
  setscreenpos(log_screen_bbox->maxcorner,new_screen_bbox->maxcorner[vx],
	       new_screen_bbox->maxcorner[vy]);
  this_erase_records->logged_features[current_log_index] = log_feature;
  this_erase_records->max_logged_bbox++;
}
  
/* Log an erase bbox given a precomputed screen bbox, in either the */
/* new or old erase buffers for this framebuffer. */

  void
log_screen_erase_bbox(featureptr log_feature,bboxscreenptr new_screen_bbox,
		      bbox3dptr new_erase_bbox,GLboolean log_in_old)
{
  /* log in the other buffer with this same bbox if it is not already */
  /* logged with this log_feature identifier there. */
  if (!already_logged(log_feature,new_screen_bbox,
		      new_erase_records[1-framebuffer]))
    do_erase_log(new_erase_records[1-framebuffer],log_feature,
		 new_screen_bbox,new_erase_bbox);
  if (log_in_old)
  {
    if (!already_logged(log_feature,new_screen_bbox,
			old_erase_records[framebuffer]))
      do_erase_log(old_erase_records[framebuffer],log_feature,new_screen_bbox,
		   new_erase_bbox);
  }
  else
    if (!already_logged(log_feature,new_screen_bbox,
			new_erase_records[framebuffer]))
      do_erase_log(new_erase_records[framebuffer],log_feature,new_screen_bbox,
		   new_erase_bbox);
}

/* Log an erase bbox, in either the new or old erase buffers */
/* for this framebuffer, given a 3d bbox and a fuzz factor margin. */

  void
log_3d_erase_bbox(featureptr log_feature,bbox3dptr new_erase_bbox,
		  GLfloat bboxmargin, GLboolean log_in_old)
{
  bbox3dtype expanded_erase_bbox;
  vertype expansion;
  bboxscreentype new_screen_bbox;
  ;
  copypos3d(new_erase_bbox->mincorner,expanded_erase_bbox.mincorner);
  copypos3d(new_erase_bbox->maxcorner,expanded_erase_bbox.maxcorner);
  setpos3d(expansion,-bboxmargin,-bboxmargin,-bboxmargin);
  addpos3d(expanded_erase_bbox.mincorner,expansion,
	   expanded_erase_bbox.mincorner);
  setpos3d(expansion,bboxmargin,bboxmargin,bboxmargin);
  addpos3d(expanded_erase_bbox.maxcorner,expansion,
	   expanded_erase_bbox.maxcorner);
  compute_screen_bbox(&expanded_erase_bbox,&new_screen_bbox);

  log_screen_erase_bbox(log_feature,&new_screen_bbox,new_erase_bbox,log_in_old);
}

/* log a an object for erasure with its shadow. */

  void
log_object_erase_bbox(objptr erase_obj,GLboolean use_objbbox, GLboolean log_in_old)
{
  bboxscreenptr special_screenbbox;
  ;
  if (erase_obj->drawtechnique == nodraw_technique)
    return;			/* no logging done for non-drawn objects, e.g */
				/* cursor, cutplane, room*/

  if (erase_obj->screenbboxtechnique != Nil)
  {				/* special screenbbox technique requires */
				/* special erase techniques */
    special_screenbbox = (erase_obj->screenbboxtechnique)();
    log_screen_erase_bbox((featureptr) erase_obj,special_screenbbox,
			  Nil, log_in_old);
  }
  else
  {
    if (use_objbbox)
      log_screen_erase_bbox((featureptr) erase_obj,&(erase_obj->objscreenbbox),
			    &(erase_obj->objbbox), log_in_old);
    else
      log_screen_erase_bbox((featureptr) erase_obj,&(erase_obj->groupscreenbbox),
			    &(erase_obj->groupbbox),log_in_old);
    if (erase_obj->children->numelems != 0)
      log_screen_erase_bbox((featureptr) erase_obj,
			    &(erase_obj->shadowscreenbbox),Nil,
			    log_in_old);
    else if (!has_property((featureptr) erase_obj,noshadow_prop))
      log_screen_erase_bbox((featureptr) erase_obj,
			    &(erase_obj->shadowscreenbbox),Nil,
			    log_in_old);
  }
}

/* Log erase bboxes on an entire branch... used primarily by zoom.c right now  */

  void
log_branch_erase_bbox(objptr erase_obj,GLboolean use_objbbox, GLboolean log_in_old)
{
  objptr thischild;
  ;
  if (erase_obj->parent != Nil)
    log_object_erase_bbox(erase_obj,use_objbbox,log_in_old);
  thischild = erase_obj->children->first.obj;
  while (thischild != Nil)
  {
    log_branch_erase_bbox(thischild,use_objbbox,log_in_old);
    thischild = thischild->next;
  }
}

/* log a feature's motherobj for redraw in the current buffer.  */

  void
log_feature_redraw(featureptr log_feature)
{
  objptr motherobj;
  ;
  if (log_feature->type_id == Obj_type)
    motherobj = (objptr) log_feature;
  else
    motherobj = (objptr) find_largergrain(log_feature,Obj_type);
  motherobj->redrawcode = Redrawbody | Redrawshadow;
}


  void
log_crosshair_erase_bboxes(stateptr state,GLboolean log_in_old)
{
  bbox3dtype horizbbox,vertbbox;
  GLfloat mywidth = 27.0,ticksize = 10.0;
  ;
#if 0
  setpos3d(horizbbox.mincorner,Mincrossx,state->cursor[vy], state->cursor[vz]);
  setpos3d(horizbbox.maxcorner,Maxcrossx,state->cursor[vy],state->cursor[vz]);
  log_3d_erase_bbox((featureptr) cursor_obj,&horizbbox,Cursormargin,log_in_old);

  setpos3d(vertbbox.mincorner,state->cursor[vx],Mincrossy, state->cursor[vz]);
  setpos3d(vertbbox.maxcorner,state->cursor[vx],Maxcrossy, state->cursor[vz]);
  log_3d_erase_bbox((featureptr) cursor_obj,&vertbbox,Cursormargin,log_in_old);
#else
  setpos3d(vertbbox.mincorner,state->cursor[vx]-mywidth,
	   state->cursor[vy]-mywidth, state->cursor[vz]);
  setpos3d(vertbbox.maxcorner,state->cursor[vx]+mywidth,
	   state->cursor[vy]+mywidth, state->cursor[vz]);
  log_3d_erase_bbox((featureptr) cursor_obj,&vertbbox,Cursormargin,log_in_old);

#if 0
  /*  Do tick erase bboxes: totally lame unless there are rules on side of */
  /*  plane*/
  setpos3d(vertbbox.mincorner,Mincrossx,
	   state->cursor[vy]-ticksize, state->cursor[vz]);
  setpos3d(vertbbox.maxcorner,Mincrossx + ticksize,
	   state->cursor[vy]+ticksize, state->cursor[vz]);
  log_3d_erase_bbox((featureptr) cursor_obj,&vertbbox,Cursormargin,log_in_old);
  setpos3d(vertbbox.mincorner, state->cursor[vx]-ticksize,
	   Maxcrossy-ticksize,state->cursor[vz]);
  setpos3d(vertbbox.maxcorner, state->cursor[vx]+ticksize, Maxcrossy,
	   state->cursor[vz]);
  log_3d_erase_bbox((featureptr) cursor_obj,&vertbbox,Cursormargin,log_in_old);
#endif /* 0 */
  
#endif  
}
 
  void
log_cutplane_erase_bbox(stateptr state, GLboolean log_in_old)
{
  bbox3dtype cutplanebbox;
  ;
#if 0
  if (has_property((featureptr) cutplane_obj,picked_prop))
  {
    /* left edge of cplane */
    setpos3d(cutplanebbox.mincorner,Mincrossx - Pickedcontouroffset,
	     Mincrossy - Pickedcontouroffset, state->cursor[vz]);
    setpos3d(cutplanebbox.maxcorner,Mincrossx, Maxcrossy + Pickedcontouroffset,
	     state->cursor[vz]);
    log_3d_erase_bbox((featureptr) cutplane_obj,&cutplanebbox,Cursormargin,
		      log_in_old);
    /* right edge */
    setpos3d(cutplanebbox.mincorner,Maxcrossx,
	     Mincrossy - Pickedcontouroffset, state->cursor[vz]);
    setpos3d(cutplanebbox.maxcorner,Maxcrossx + Pickedcontouroffset,
	     Maxcrossy + Pickedcontouroffset, state->cursor[vz]);
    log_3d_erase_bbox((featureptr) cutplane_obj,&cutplanebbox,Cursormargin,
		      log_in_old);
    /* bottom edge */
    setpos3d(cutplanebbox.mincorner,Mincrossx - Pickedcontouroffset,
	     Mincrossy - Pickedcontouroffset, state->cursor[vz]);
    setpos3d(cutplanebbox.maxcorner,Maxcrossx + Pickedcontouroffset,
	     Mincrossy - Pickedcontouroffset, state->cursor[vz]);
    log_3d_erase_bbox((featureptr) cutplane_obj,&cutplanebbox,Cursormargin,
		      log_in_old);
    /* top edge */
    setpos3d(cutplanebbox.mincorner,Mincrossx - Pickedcontouroffset,
	     Maxcrossy, state->cursor[vz]);
    setpos3d(cutplanebbox.maxcorner,Maxcrossx + Pickedcontouroffset,
	     Maxcrossy + Pickedcontouroffset, state->cursor[vz]);
    log_3d_erase_bbox((featureptr) cutplane_obj,&cutplanebbox,Cursormargin,
		      log_in_old);
  }
  else
  {
    /* left edge of cplane */
    setpos3d(cutplanebbox.mincorner,Mincrossx, Mincrossy, state->cursor[vz]);
    setpos3d(cutplanebbox.maxcorner,Mincrossx, Maxcrossy, state->cursor[vz]);
    log_3d_erase_bbox((featureptr) cutplane_obj,&cutplanebbox,Cursormargin,
		      log_in_old);
    /* right edge */
    setpos3d(cutplanebbox.mincorner,Maxcrossx, Mincrossy, state->cursor[vz]);
    setpos3d(cutplanebbox.maxcorner,Maxcrossx, Maxcrossy, state->cursor[vz]);
    log_3d_erase_bbox((featureptr) cutplane_obj,&cutplanebbox,Cursormargin,
		      log_in_old);
    /* bottom edge */
    setpos3d(cutplanebbox.mincorner,Mincrossx, Mincrossy, state->cursor[vz]);
    setpos3d(cutplanebbox.maxcorner,Maxcrossx, Mincrossy, state->cursor[vz]);
    log_3d_erase_bbox((featureptr) cutplane_obj,&cutplanebbox,Cursormargin,
		      log_in_old);
    /* top edge */
    setpos3d(cutplanebbox.mincorner,Mincrossx, Maxcrossy, state->cursor[vz]);
    setpos3d(cutplanebbox.maxcorner,Maxcrossx, Maxcrossy, state->cursor[vz]);
    log_3d_erase_bbox((featureptr) cutplane_obj,&cutplanebbox,Cursormargin,
		      log_in_old);
/*
    setpos3d(cutplanebbox.mincorner,Mincrossx,Mincrossy,state->cursor[vz]);
    setpos3d(cutplanebbox.maxcorner,Maxcrossx,Maxcrossy,state->cursor[vz]);
*/
  }
#else
  /* old way, for entire picked cutplane */

  setpos3d(cutplanebbox.mincorner,Mincrossx - Pickedcontouroffset,
	   Mincrossy - Pickedcontouroffset, state->cursor[vz]);
  setpos3d(cutplanebbox.maxcorner,Maxcrossx + Pickedcontouroffset,
	   Maxcrossy + Pickedcontouroffset, state->cursor[vz]);
  log_3d_erase_bbox((featureptr) cutplane_obj,&cutplanebbox,Cursormargin,
		    log_in_old);
#endif
}

  GLboolean
screen_bbox_collision(objptr thisobj,bboxscreenptr test_bbox)
{
  bboxscreenptr special_screenbbox;
  ;
  if (thisobj->screenbboxtechnique != Nil)
  {
    special_screenbbox = (thisobj->screenbboxtechnique)();
    /* this is where weird objects like rotatetool, etc get tested for redraw */
    if (bbox_intersect_screen(test_bbox, special_screenbbox))
      thisobj->redrawcode |= Redrawbody;
  }
  else
  {
    if (bbox_intersect_screen(test_bbox, &(thisobj->objscreenbbox)))
      thisobj->redrawcode |= Redrawbody;
    if (!has_property((featureptr) thisobj, noshadow_prop))
      if (bbox_intersect_screen(test_bbox, &(thisobj->shadowscreenbbox)))
	thisobj->redrawcode |= Redrawshadow;
  }
  /* stop if all possible redraws logged on this object. */
  return(thisobj->redrawcode == (Redrawbody | Redrawshadow));
}

/* Flag any objects that need to be redraw if their bboxes intersect any */
/* erasure bboxes. */

  void
log_erasebbox_intersections(listptr world_list,int framebuffer)
{
  worldptr world;
  proptr thisprop;
  int check_erase_bbox;
  featureptr thisfeature;
  objptr thisobj;
  int max_logged_bbox;
  ;
  max_logged_bbox = old_erase_records[framebuffer]->max_logged_bbox;
  if (max_logged_bbox == 0)
    return;			/* no bbox erasures will be done */
  world = world_list->first.world;
  while (world != Nil)
  {
    thisprop = world->world_proplists[(int) visible_prop]->first.prop;
    while (thisprop != Nil)
    {
      thisfeature = thisprop->owner;
      if (thisfeature->type_id == Obj_type)
      {
	thisobj = (objptr) thisfeature;
	if ((thisobj->drawtechnique != nodraw_technique) &&
	    (thisobj->redrawcode != (Redrawbody | Redrawshadow)))
	{
	  /* Concerned with objs that are actually drawn but not */
	  /* yet totally flagged. */
	  check_erase_bbox = 0;
	  while (check_erase_bbox < max_logged_bbox)
	  {
	    if (screen_bbox_collision(thisobj,
				      (&(old_erase_records[framebuffer]->
					 erase_bboxes[check_erase_bbox]))))
	      break;
	    check_erase_bbox++;
	  }
	}
      }
      thisprop = thisprop->next;
    }
    world = world->next;
  }
}

void
update_cutplanetime(listptr world_list)
{
  cutplane_time++;

  if (cutplane_time == USHRT_MAX) /* from /usr/include/limits.h */
    reset_cutplanetime(world_list);
}

/* Main refresh loop. */
GLboolean
new_user_input(listptr world_list, stateptr state, stateptr oldstate)
{
  int fudge = 20;		// for viewport clip stuff
  static vertype lastverthair[2][2], lasthorizhair[2][2];

  static Matrix lastviewmatrix[2];
  vertype verthair[2],horizhair[2];
  GLuint left,right,bot,top;

  Matrix debugmatrix;
  vertype pos;
  statetype previoustate;
  static int just_lifted_blit = 0;
  ;
  previoustate = *oldstate;
  *oldstate = *state;

  new_update_state(world_list, state, oldstate);

  update_world(state, oldstate);
  
  /* Get selections based on the last refresh. */
  set_selectables(world_list, state, oldstate);

  clear_view();

  draw_underlying_objects(state, oldstate);

  draw_worlds(Draweverything); // draw all objects no matter what

  // draw cursor, select box
  draw_gl_objects(state);

#ifdef IRIS
  zbuffer_writing_on();		/* if zbuffer writing was off, restore it. */
#endif /* IRIS */
  
  glutSwapBuffers();

  framebuffer = 1 - framebuffer;
  

  if (cutplane_time == 0)
  {
    global_update_flags = Allupdateflags; /* need to update all pos's */
    process_global_updates(world_list, state, oldstate);
  }

  cutplane_time++;

  if (cutplane_time > 2)
    set_backface_flags = FALSE;	/* don't reset them unless roomrot, see grafx*/

  return (exit_system != TRUE);
}  

void
update_display()
{
  new_user_input(world_list, state, oldstate);
}
