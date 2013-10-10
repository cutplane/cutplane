
/* FILE: cplib.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*  The Cutplane Language (CPL) Library Routines for the Cutplane Editor  */
/*                                                                        */
/* Author: WAK                                       Date: December 3,1990*/
/* Version: 1                                                             */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990,1991 William Kessler and             */
/* Larry Edwards. All Rights Reserved.                                    */
/*                                                                        */
/* These routines interface special objects to C code currently too hard  */
/* to implement in pure CPL... (someday, maybe, I'm getting there)        */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define CPLIBMODULE

#include <config.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <platform_gl.h>

#include <cutplane.h>
#include <math3d.h>
#include <cpl_cutplane.h>
#include <update.h>
#include <tools.h>
#include <text.h>
#include <grafx.h>
#include <primitives.h>

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	                       GLOBAL VARIABLES				     */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* This is a lookup table of pointers to C routines that are to be called */
/* when the CALL_C_ROUTINE opcode is executed. The second word specifies */
/* an index into this table. (In code parsed as ASCII, a search is done */
/* on the given routine name to actually encode this index in the second */
/* word.) */

Croutinerec Croutines[MaxCroutines] = {{"slice_object",CPL_slice_object},
				       {"setup_rotatetool",
					  CPL_setup_rotatetool},
				       {"snap_rotatetool",CPL_snap_rotatetool},
				       {"xbar_rotate",CPL_xbar_rotate},
				       {"bauble_rotate",CPL_bauble_rotate},
				       {"arrange_prims",CPL_arrange_prims},
				       {"arrange_toolpads",
					  CPL_arrange_toolpads},
				       {"near_toolbar",CPL_near_toolbar},
				       {"set_picksize",CPL_set_picksize},
				       {"setup_scalebbox",
					  CPL_setup_scalebbox},
				       {"scalebbox_trans",
					  CPL_scalebbox_trans},
				       {"always_set_select",
					  CPL_always_set_select},
				       {"reset_picksize",CPL_reset_picksize},
				       {"set_light",CPL_set_light},
				       {"switch_light",CPL_switch_light},
				       {"do_color",CPL_do_color},
				       {"control_color",CPL_control_color},
				       {"set_color_bars",CPL_set_color_bars},
				       {"start_draw",CPL_start_draw},
				       {"end_draw",CPL_end_draw},
				       {"extend_draw",CPL_extend_draw},
				       {"draw_on_surface",CPL_draw_on_surface},
				       {"draw_line",CPL_draw_line},
				       {"start_marquee",CPL_start_marquee},
				       {"do_zoom",CPL_do_zoom},
				       {"world_2_inch", CPL_world_2_inch},
				       {"inch_2_world", CPL_inch_2_world},
				       {"make_decal",CPL_make_decal},
				       {"tool_off_plane",CPL_tool_off_plane},
					 /* special text input handling */
				       {"add_obj_charstream",
					CPL_add_obj_charstream},
				       {"rem_obj_charstream",
					  CPL_rem_obj_charstream},
				       {"filter_char",CPL_filter_char},
				       {"add_to_label",CPL_add_to_label},
				       {"label_fits",CPL_label_fits},
				       {"backspace_label",CPL_backspace_label},
				       {"set_insertion_pt",
					  CPL_set_insertion_pt},
				       {"move_insertion_pt",
					  CPL_move_insertion_pt},
				       {"read_from_file", CPL_read_from_file},
				       {"write_to_file",CPL_write_to_file},
					 /* SUPER HACKS A real hacker's dream */
				       {"get_feature_size",
					  CPL_get_feature_size},
				       {"set_feature_size",
					  CPL_set_feature_size},
				       {"get_feature_dist",
					  CPL_get_feature_dist},
				       {"set_feature_dist",
					  CPL_set_feature_dist},
				       {"remake_piechart",
					  CPL_remake_piechart},
				       {"toggle_slide", CPL_toggle_slide},
				       {"set_camera",CPL_set_camera},
				       {"rotate_camera",CPL_rotate_camera},
				       {"turnon_arrowcursor",
					  CPL_turnon_arrowcursor},
				       {"set_hiddenedge",
					  CPL_set_hiddenedge},
				       {"cursor_on_border",
					  CPL_cursor_on_border},
				       {"normal_constrain",
					  CPL_normal_constrain},
					 /* Primitive creation/edit routines */
				       {"remake_prim",
					  CPL_remake_prim},
				       {"get_plate_by_name",
					  CPL_get_plate_by_name},
				       {"generate_info_rec",
					  CPL_generate_info_rec},
				       {"breakup_info_rec",
					  CPL_breakup_info_rec},
				       {"breakup_dist_value",
					  CPL_breakup_dist_value}};



listptr obj_charstream_list;

/* This hack allows us to bypass borland's warning message about parameters */
/* not being used. Later, we should clean all those up if they're */
/* really of no use. FOr now, I kinda like knowing those routines can get */
/* at the world_list or state if they need to. */

#ifndef IRIS
listptr cplib_world_list;
#endif

/* Same for state. */
#ifndef IRIS
stateptr cplib_state;
#endif

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* CPL Generic List-based Stack Routines : Heavy use of malloc            */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/* These stack routines are  used for the main CPL stack used by PUSH and */
/* POP and any other stacks you care to use */
/* them for.  Their growth is limited only by malloc. This makes them */
/* kinda slow. */

  listptr
create_CPL_stack(void)
{
  return(create_list());	/* a stack is just a linked list of ep's */
}

  Boolean
CPL_stack_empty(listptr this_CPL_stack)
{
  return (this_CPL_stack->numelems == 0);
}

/* Push words onto a CPL generic linked-list stack.*/

  void
push_on_CPL_stack(listptr this_CPL_stack,CPL_word_ptr push_CPL_word)
{
  elemptrptr new_stack_word;
  CPL_word_ptr new_CPL_stack_word;
  ;
  new_stack_word = (elemptrptr)
    insert_new_blank_elem(this_CPL_stack,
			  (elemptr) this_CPL_stack->first.ep, Ep_type);
  new_stack_word->thiselem = alloc_elem(CPLwordsize);
  new_CPL_stack_word = (CPL_word_ptr) new_stack_word->thiselem;
  setup_CPL_word_blank(new_CPL_stack_word,Blank_word);
  copy_wordA_to_wordB(push_CPL_word, new_CPL_stack_word);
}

/* Pull old queued elems from top of list. */

  void
pop_off_CPL_stack(listptr this_CPL_stack,CPL_word_ptr pop_CPL_word,
		  Boolean clear_target)
{
  elemptrptr top_CPL_stack;
  CPL_word_ptr top_CPL_stack_word;
  ;
  if (this_CPL_stack->numelems == 0)
    system_error("pop_off_CPL_stack:Empty CPLstack!!!");
  top_CPL_stack =  this_CPL_stack->first.ep;
  top_CPL_stack_word = (CPL_word_ptr) top_CPL_stack->thiselem;
  if (clear_target)
    setup_CPL_word_blank(pop_CPL_word,Blank_word);
  else
    setup_CPL_word(pop_CPL_word,Blank_word);
  copy_wordA_to_wordB(top_CPL_stack_word,pop_CPL_word);
  clear_word(top_CPL_stack_word);
  rem_elem(this_CPL_stack,(elemptr) top_CPL_stack);
  free_elem((elemptr *) &top_CPL_stack);
}


  listptr
create_CPL_queue(void)
{
  return(create_list());	/* a queue is just a linked list of ep's */
}

/* These queue routines are used for message storage. */
/* Their growth is limited only by malloc. This makes them */
/* kinda slow. */


  Boolean
CPL_queue_empty(listptr this_CPL_queue)
{
  return (this_CPL_queue->numelems == 0);
}

/* Add words at the beginning/end of a CPL generic linked-list queue. */

  void
add_to_CPL_queue(listptr this_CPL_queue,CPL_word_ptr added_CPL_word,
		 Boolean prepend)
{
  elemptrptr new_queue_word;
  CPL_word_ptr new_CPL_queue_word;
  ;
  if (prepend)
    new_queue_word = (elemptrptr)
      insert_new_blank_elem(this_CPL_queue,
			    (elemptr) this_CPL_queue->first.ep, Ep_type);
  else
    new_queue_word = (elemptrptr) append_new_blank_elem(this_CPL_queue, Ep_type);
  new_queue_word->thiselem = alloc_elem(CPLwordsize);
  new_CPL_queue_word = (CPL_word_ptr) new_queue_word->thiselem;
  setup_CPL_word_blank(new_CPL_queue_word,Blank_word);
  copy_wordA_to_wordB(added_CPL_word, new_CPL_queue_word);
}


/* Unqueue old queued elems from top/bottom of CPLqueue. */

  void
remove_from_CPL_queue(listptr this_CPL_queue,CPL_word_ptr removed_CPL_word,
		      Boolean from_top)
{
  elemptrptr removed_CPL_queue;
  CPL_word_ptr removed_CPL_queue_word;
  ;
  if (this_CPL_queue->numelems == 0)
    system_error("remove_from_CPL_queue:Empty CPLqueue!!!");
  if (from_top)
    removed_CPL_queue =  this_CPL_queue->first.ep;
  else
    removed_CPL_queue =  this_CPL_queue->last.ep;
  removed_CPL_queue_word = (CPL_word_ptr) removed_CPL_queue->thiselem;
  /* This assumes the target removed word was once initialized!!!! */
  setup_CPL_word(removed_CPL_word,Blank_word);
  copy_wordA_to_wordB(removed_CPL_queue_word,removed_CPL_word);
  clear_word(removed_CPL_queue_word);
  rem_elem(this_CPL_queue,(elemptr) removed_CPL_queue);
  free_elem((elemptr *) &removed_CPL_queue);
}


/* copy old queued elems from top/bottom of CPLqueue. */

  void
copy_from_CPL_queue(listptr this_CPL_queue,CPL_word_ptr removed_CPL_word,
		    Boolean from_top)
{
  elemptrptr removed_CPL_queue;
  CPL_word_ptr removed_CPL_queue_word;
  ;
  if (this_CPL_queue->numelems == 0)
    system_error("remove_from_CPL_queue:Empty CPLqueue!!!");
  if (from_top)
    removed_CPL_queue =  this_CPL_queue->first.ep;
  else
    removed_CPL_queue =  this_CPL_queue->last.ep;
  removed_CPL_queue_word = (CPL_word_ptr) removed_CPL_queue->thiselem;
  /* This assumes the target removed word was once initialized!!!! */
  setup_CPL_word(removed_CPL_word,Blank_word);
  copy_wordA_to_wordB(removed_CPL_queue_word,removed_CPL_word);
}


  void
push_owner_stack(void)
{
  owner_stack[owner_stack_top] = current_owner;
  if (owner_stack_top < CPL_Ownerstacksize)
    owner_stack_top++;
  else
    system_error("push_owner_stack: too many CPL call levels deep!");
}

  void
pop_owner_stack(void)
{
  if (owner_stack_top > 0)
  {
    owner_stack_top--;
    current_owner = owner_stack[owner_stack_top];
  }
  else
    system_error("pop_owner_stack: empty owner stack (JSR/RTS imbalance)!");
}


  Boolean
CPL_controlstack_full(void)
{
  return (CPL_controlstack_top == (CPL_Controlstacksize - 1));
}


/* Add new stack entries at top of control stack */
  void
push_on_CPL_controlstack(CPL_word_ptr push_CPL_controlword)
{
  if (!CPL_controlstack_full())
  {
    CPL_controlstack_top++;
    CPL_controlstack[CPL_controlstack_top] = *push_CPL_controlword;
  }
}

/* Pull old queued elems from top of list. */
  void
pop_off_CPL_controlstack(CPL_word_ptr pop_CPL_controlword)
{
  if (CPL_controlstack_top == -1)
    system_error("pop_off_CPL_controlstack:Empty CPL_controlstack!!!");
  *pop_CPL_controlword = CPL_controlstack[CPL_controlstack_top];
  CPL_controlstack_top--;
}

  void
init_obj_charstream_list(void)
{
  obj_charstream_list = create_list();
}

  Boolean
objects_in_charstream(void)
{
  return(obj_charstream_list->numelems > 0);
}

  void
stream_char_to_objects(char typed_char)
{
  elemptrptr this_streamed_ep;
  CPL_word msgword;
  ;
  this_streamed_ep = obj_charstream_list->first.ep;
  setup_CPL_word_blank(&msgword,Message_data);
  while (this_streamed_ep != Nil)
  {
    msgword.word_data.CPLmessage = lookup_CPL_message("Incomingchar");
    enter_in_temp_message_block(&msgword);
    setup_CPL_auxword(&msgword,CPLaux_char_data);
    msgword.word_data.CPLauxdata.CPLchardata = typed_char;
    enter_in_temp_message_block(&msgword);
    queue_feature_message((featureptr) this_streamed_ep->thiselem, Prioritymsg);
    /* reset message system after queueing message! */
    message_block_reset();    
    this_streamed_ep = this_streamed_ep->next;
  }
}


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* Routines called by CPL directly (all names start with "CPL_")          */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/* marqueeing stuff... allows you to start a marquee with any cursor object */
/* although right now there is really only one global one... and someday this */
/* routine should be coded in CPL anyways, but today it still lacks a MAKEOBJ */
/* technique. */

  void
CPL_start_marquee(listptr world_list,stateptr state)
{
  CPL_word cursor_obj_rec,marquee_obj_rec;
  objptr this_cursor_obj,marquee;
  Coord * pos;
  ;
  pop_off_CPL_stack(CPL_stack,&cursor_obj_rec,TRUE);
  setup_CPL_word_blank(&marquee_obj_rec,Featurelist_data);
  if (cursor_obj_rec.word_type == Featurelist_data)
    if (cursor_obj_rec.word_data.CPLfeaturelist->numelems > 0)
    {
      this_cursor_obj = (objptr) cursor_obj_rec.word_data.CPLfeaturelist->
	first.ep->thiselem;
      pos = First_obj_vfe(this_cursor_obj)->pos;
      marquee = make_obj(working_world,pos);
      marquee->drawtechnique = draw_marquee_technique;
      make_edge_vert(First_obj_le(marquee),First_obj_le(marquee),pos);
      add_property((featureptr) marquee,nonmanifold_prop);
      add_property((featureptr) First_obj_vfe(marquee)->next,hotspot_prop);
      add_to_featurelist(marquee_obj_rec.word_data.CPLfeaturelist,
			 (featureptr) marquee);
    }
  if (marquee_obj_rec.word_data.CPLfeaturelist->numelems == 0)
    add_to_featurelist(marquee_obj_rec.word_data.CPLfeaturelist,Nil);
  push_on_CPL_stack(CPL_stack,&marquee_obj_rec);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}



/* pencil sketch stuff */
  void
CPL_start_draw(listptr world_list,stateptr state)
{
  objptr scribble;
#ifdef IRIS
  CPL_word scribble_rec,selectable_rec;
#else
  CPL_word scribble_rec;
#endif
  ;
  /* passed on stack is selectable objects, should be used here when draw on */
  /* surface is coded.*/
  scribble = make_obj(working_world,state->cursor);
  scribble->selectechnique = set_always_selectable;
  scribble->drawtechnique = draw_scribble_technique;
  setup_CPL_word_blank(&scribble_rec,Featurelist_data);
  add_to_featurelist(scribble_rec.word_data.CPLfeaturelist,
		     (featureptr) scribble);
  del_property((featureptr) scribble, quickupdate_prop);
  push_on_CPL_stack(CPL_stack,&scribble_rec);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_end_draw(listptr world_list,stateptr state)
{
#ifdef IRIS
  objptr scribble,surfacedraw;
#else
  objptr scribble;
#endif
  CPL_word scribble_rec,surface_rec;
  leptr firstle,lastle;
  vfeptr thisvfe;
  static Coord zpos;
  static vertype depth = {0.0,0.0,50.0};
  Boolean flatpoly = TRUE;
  ;
  pop_off_CPL_stack(CPL_stack,&scribble_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&surface_rec,TRUE);
  
  if (scribble_rec.word_type == Featurelist_data)
  {
    scribble = (objptr)
      scribble_rec.word_data.CPLfeaturelist->first.ep->thiselem;
    if (scribble != Nil)
    {
      /* check for planarity first... */
      thisvfe = First_obj_vfe(scribble);
      zpos = thisvfe->pos[vz];
      while (thisvfe != Nil)
      {
	if (thisvfe->pos[vz] != zpos)
	  flatpoly = FALSE;
	thisvfe = thisvfe->next;
      }
      if (flatpoly)		/* only finish if flat.. real ugly but */
      {				/* TRNSFRM not working anymore due to glove */
	firstle = First_obj_vfe(scribble)->startle;
	lastle = scribble->shellist->first.shell->vfelist->last.vfe->startle;
	make_edge_face(lastle,firstle);
      }
    }
    if (flatpoly)
    {
      /* else check for surface draw */
      scribble->selectechnique = set_object_selectable; /* the usual way */
      scribble->drawtechnique = normal_draw_technique;
      extrude_face(First_obj_fve(scribble),depth);
    }
    else
    {
      kill_obj(scribble);
      scribble_rec.word_data.CPLfeaturelist->first.ep->thiselem = Nil;
    }
  }
  push_on_CPL_stack(CPL_stack,&scribble_rec);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_extend_draw(listptr world_list,stateptr state)
{
#ifdef IRIS
  objptr scribble,surfacedraw;
#else
  objptr scribble;
#endif
  CPL_word scribble_rec,trans_rec;
  vfeptr lastvfe;
  leptr lastle;
  Coord *cursormove;
  vertype newpos;
  ;
  /* needs the surface rec also? */
  pop_off_CPL_stack(CPL_stack,&trans_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&scribble_rec,TRUE);
  
  if (scribble_rec.word_type == Featurelist_data)
  {
    scribble = (objptr)
      scribble_rec.word_data.CPLfeaturelist->first.ep->thiselem;
    if (scribble != Nil)
    {
      if (trans_rec.word_type == Auxiliary_data)
      {
	cursormove = trans_rec.word_data.CPLauxdata.CPLoffsetdata;
	lastvfe = scribble->shellist->first.shell->vfelist->last.vfe;
	lastle = lastvfe->startle;
	addpos3d(lastvfe->pos,cursormove,newpos);
	make_edge_vert(lastle,lastle,newpos);
      }
    }
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_draw_on_surface(listptr world_list, stateptr state)
{
  CPL_word draw_feature,connect_to_it,cutpt1rec;
  static vfeptr vfe1,vfe2;
  static evfptr evf1,evf2;
  featureptr thefeature;
  static vertype cutpt1;
  vertype cutpt2;
  double dummyt;
  leptr firstle,secondle;
  objptr motherobj;
  ;
  /* cases: draws along same edge. */
  /* draws from one edge to edge not in same face (should restart no breaking) */
  /* draws from vfe->edge, edge->vfe, vfe->vfe; checks for same faces...*/
  pop_off_CPL_stack(CPL_stack,&connect_to_it,TRUE);  
  pop_off_CPL_stack(CPL_stack,&draw_feature,TRUE);
  if (connect_to_it.word_data.CPLauxdata.CPLbooleandata == TRUE)
  {
    thefeature = draw_feature.word_data.CPLfeaturelist->first.ep->thiselem;
    motherobj = (objptr) find_largergrain(thefeature,Obj_type);
    if (thefeature->type_id == Vfe_type)
    {
      vfe2 = (vfeptr) thefeature;
      copypos3d(vfe2->pos,cutpt2);
      secondle = vfe2->startle;
    }
    else
    {
      evf2 = (evfptr) thefeature;
      if (motherobj->selectable_feature.motherseg->kind == Evf_inplane)
	pt_near_lineseg_3d(evf2->le1->facevert->pos,evf2->le2->facevert->pos,
			   state->cursor, cutpt2, &dummyt,global_picksize);
      else
	copypos3d(evf2->cutpt,cutpt2);
      secondle = break_edge_at_pos(evf2->le1,cutpt2);
    }
    if (vfe1 == Nil)
      firstle = break_edge_at_pos(evf1->le1,cutpt1);
    else
      firstle = vfe1->startle;
    if (find_les_same_face_walking(&firstle,&secondle))
      make_edge_face(firstle,secondle);
  }
  else
  {
    thefeature = draw_feature.word_data.CPLfeaturelist->first.ep->thiselem;
    motherobj = (objptr) find_largergrain(thefeature,Obj_type);
    setup_CPL_auxword(&cutpt1rec,CPLaux_offset_data);
    if (thefeature->type_id == Vfe_type)
    {
      vfe1 = (vfeptr) thefeature;
      copypos3d(vfe1->pos,cutpt1rec.word_data.CPLauxdata.CPLoffsetdata);
    }
    else
    {
      evf1 = (evfptr) thefeature;
      vfe1 = Nil;
      if (motherobj->selectable_feature.motherseg->kind == Evf_inplane)
	pt_near_lineseg_3d(evf1->le1->facevert->pos,evf1->le2->facevert->pos,
			   state->cursor,
			   cutpt1rec.word_data.CPLauxdata.CPLoffsetdata,
			   &dummyt,global_picksize);
      else
	copypos3d(evf1->cutpt,cutpt1rec.word_data.CPLauxdata.CPLoffsetdata);
    }
    copypos3d(cutpt1rec.word_data.CPLauxdata.CPLoffsetdata,cutpt1);
    push_on_CPL_stack(CPL_stack,&cutpt1rec);
  }

#ifndef IRIS
  cplib_world_list = world_list;
#endif
}

  void
CPL_draw_line(listptr world_list,stateptr state)
{
  CPL_word pt1rec,pt2rec;
  float *pt1,*pt2;
  ;
  pop_off_CPL_stack(CPL_stack,&pt2rec,TRUE);  
  pop_off_CPL_stack(CPL_stack,&pt1rec,TRUE);
  if ( ((pt1rec.word_type == Auxiliary_data) &&
	(pt2rec.word_type == Auxiliary_data)) ||
      ((pt1rec.word_type == Auxiliary_data) &&
       (pt2rec.word_type == Environment_data)) )
  {
    pt1 = pt1rec.word_data.CPLauxdata.CPLoffsetdata;
    if (pt2rec.word_type == Environment_data)
      if (pt2rec.word_data.CPLenvironment_part == Crosshair_part)
	pt2 = state->cursor;
      else
	pt2 = origin;
    else
      pt2 = pt2rec.word_data.CPLauxdata.CPLoffsetdata;
    copypos3d(pt1,First_obj_vfe(line_obj)->pos);
    copypos3d(pt2,First_obj_vfe(line_obj)->next->pos);
  }

#ifndef IRIS
  cplib_world_list = world_list;
#endif

}
  
  void
CPL_do_zoom(listptr world_list,stateptr state)
{
  CPL_word zoom_qty;
  Coord zoom_amt;
  ;
  pop_off_CPL_stack(CPL_stack,&zoom_qty,TRUE);

  zoom_amt = pow(1.01,zoom_qty.word_data.CPLauxdata.CPLoffsetdata[vy]);
  zoom_world(working_world,state->cursor,zoom_amt);
  push_on_CPL_stack(CPL_stack,&zoom_qty);

#ifndef IRIS
  cplib_world_list = world_list;
#endif

}



#define Inchesperworldunits (1.0/10.0)
#define Worldunitsperinch (1.0 / Inchesperworldunits)

  void
CPL_world_2_inch(listptr world_list, stateptr state)
{
  CPL_word value_rec;
  Coord unzoomed,inch_amt;
  ;
  pop_off_CPL_stack(CPL_stack,&value_rec,TRUE);
  unzoomed = unzoom_unit(value_rec.word_data.CPLauxdata.CPLfloatdata);
  inch_amt = unzoomed * Inchesperworldunits;
  value_rec.word_data.CPLauxdata.CPLfloatdata = inch_amt;
  push_on_CPL_stack(CPL_stack,&value_rec);  

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}



  void
CPL_inch_2_world(listptr world_list, stateptr state)
{
  CPL_word value_rec;
  Coord zoomed,world_amt;
  ;
  pop_off_CPL_stack(CPL_stack,&value_rec,TRUE);
  world_amt = value_rec.word_data.CPLauxdata.CPLfloatdata * Worldunitsperinch;
  zoomed = zoom_unit(world_amt);
  value_rec.word_data.CPLauxdata.CPLfloatdata = zoomed;
  push_on_CPL_stack(CPL_stack,&value_rec);  

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}


  void
CPL_make_decal(listptr world_list,stateptr state)
{
  CPL_word decal_word;
  featureptr motherfeature;
  objptr mother_obj,decal_obj;
  ;
  pop_off_CPL_stack(CPL_stack,&decal_word,TRUE);

  if (decal_word.word_type != Featurelist_data)
  {    
    push_on_CPL_stack(CPL_stack,&decal_word);
    return;
  }
  motherfeature = (featureptr) decal_word.word_data.CPLfeaturelist->
    first.ep->thiselem;
  mother_obj = (objptr) find_largergrain(motherfeature,Obj_type);
  decal_obj = mother_obj->children->first.obj;
  apply_front_decal(decal_obj,mother_obj,Decalthickness,Decalzoffset,0.0);
  push_on_CPL_stack(CPL_stack,&decal_word);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_tool_off_plane(listptr world_list,stateptr state)
{
  CPL_word tool_word,offset_word;
  featureptr motherfeature;
  objptr tool_obj;
  ;
  pop_off_CPL_stack(CPL_stack,&tool_word,TRUE);
  setup_CPL_auxword(&offset_word,CPLaux_offset_data);
  if (tool_word.word_type != Featurelist_data)
  {    
    copypos3d(origin,offset_word.word_data.CPLauxdata.CPLoffsetdata);
    push_on_CPL_stack(CPL_stack,&offset_word);
    return;
  }
  motherfeature = (featureptr) tool_word.word_data.CPLfeaturelist->
    first.ep->thiselem;
  tool_obj = (objptr) find_largergrain(motherfeature,Obj_type);
  transform_obj_tree(tool_obj);
  find_branch_normals(tool_obj);
  set_3d_branchbboxes(tool_obj);
  grow_group_bbox(tool_obj);
  diffpos3d(tool_obj->groupbbox.mincorner,state->cursor,
	    offset_word.word_data.CPLauxdata.CPLoffsetdata);
  offset_word.word_data.CPLauxdata.CPLoffsetdata[vx] = 
    offset_word.word_data.CPLauxdata.CPLoffsetdata[vy] = 0.0;
  offset_word.word_data.CPLauxdata.CPLoffsetdata[vz] += 10.0; /* just off it */

  push_on_CPL_stack(CPL_stack,&offset_word);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}


  void
CPL_slice_object(listptr world_list,stateptr state)
{
  featureptr thefeature;
#ifdef IRIS
  objptr newfrontobj,newrearobj,sliced_obj;
#else
  objptr sliced_obj;
#endif
  CPL_word sliced_object_set,resulting_front,resulting_rear;
  elemptrptr thefeature_ep;
  listptr frontobjlist,rearobjlist;
  ;
  pop_off_CPL_stack(CPL_stack,&sliced_object_set,TRUE);
  setup_CPL_word_blank(&resulting_front,Featurelist_data);
  setup_CPL_word_blank(&resulting_rear,Featurelist_data);
  if (sliced_object_set.word_type == Featurelist_data)
  {
    thefeature_ep = sliced_object_set.word_data.CPLfeaturelist->first.ep;
    if (thefeature_ep != Nil)
    {
      while (thefeature_ep != Nil)
      {
	thefeature = thefeature_ep->thiselem;
	if (thefeature == Nil)
	{
	  add_to_featurelist(resulting_front.word_data.CPLfeaturelist,Nil);
	  add_to_featurelist(resulting_rear.word_data.CPLfeaturelist,Nil);
	  push_on_CPL_stack(CPL_stack,&resulting_rear);
	  push_on_CPL_stack(CPL_stack,&resulting_front);
	  return;	/* wasteful! reached end of list prematurely */
	}
	sliced_obj = (objptr) find_largergrain(thefeature,Obj_type);
	
	frontobjlist = create_list();
	rearobjlist = create_list();
	slice_object(sliced_obj,state,frontobjlist,rearobjlist);

	merge_lists(frontobjlist,resulting_front.word_data.CPLfeaturelist);
	merge_lists(rearobjlist,resulting_rear.word_data.CPLfeaturelist);
	thefeature_ep = thefeature_ep->next;
      }
    }
  }
  if (resulting_front.word_data.CPLfeaturelist->numelems == 0)
    add_to_featurelist(resulting_front.word_data.CPLfeaturelist,Nil);
  if (resulting_rear.word_data.CPLfeaturelist->numelems == 0)
    add_to_featurelist(resulting_rear.word_data.CPLfeaturelist,Nil);
  push_on_CPL_stack(CPL_stack,&resulting_rear);
  push_on_CPL_stack(CPL_stack,&resulting_front);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

/* Arrange a toolbox's children in a neat row, vertically */
/* Eventually should do it along normal to the opening of the box, but */
/* this is pretty set right now anyhow */


  void
CPL_arrange_prims(listptr world_list,stateptr state)
{
  objptr thistoolbox,thisprim;
  vertype top,primtop,toolboxcenter,primcenter,translatevec;
  Coord primheight;
  CPL_word arranged_toolbox;
  elemptrptr thistoolbox_ep;
  ;
  pop_off_CPL_stack(CPL_stack,&arranged_toolbox,TRUE);
  if (arranged_toolbox.word_type != Featurelist_data)
  {
    push_on_CPL_stack(CPL_stack,&arranged_toolbox);
    return;
  }
  else
  {
    thistoolbox_ep = arranged_toolbox.word_data.CPLfeaturelist->first.ep;
    if (thistoolbox_ep != Nil)
    {
      while (thistoolbox_ep != Nil)
      {
	thistoolbox = (objptr) thistoolbox_ep->thiselem;

	midpoint(thistoolbox->objbbox.mincorner,
		 thistoolbox->objbbox.maxcorner,toolboxcenter);
	copypos3d(toolboxcenter,top);
	top[vy] = thistoolbox->objbbox.maxcorner[vy];
	top[vy] += Primsmargin;
	
	thisprim = thistoolbox->children->first.obj;
	while (thisprim != Nil)
	{
	  if (!has_property((featureptr) thisprim,
			    nonmanifold_prop))
	  {
	    primheight = thisprim->groupbbox.maxcorner[vy] -
	      thisprim->groupbbox.mincorner[vy] + Primsmargin;
	    top[vy] += primheight;

	    midpoint(thisprim->groupbbox.maxcorner,
		     thisprim->groupbbox.mincorner,primcenter);
	    copypos3d(primcenter,primtop);
	    primtop[vy] = thisprim->groupbbox.maxcorner[vy];

	    diffpos3d(primtop,top,translatevec);

	    translate_feature((featureptr) thisprim,translatevec,FALSE);
	  }
	  thisprim = thisprim->next;
	}
	thistoolbox_ep = thistoolbox_ep->next;
      }
    }
    push_on_CPL_stack(CPL_stack,&arranged_toolbox);
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

/* Arrange a toolbar's children in a neat row, vertically. Assume toolbar is */
/* a nice vertical things. */
/* Eventually should do it along normal to the opening of the box, but */
/* this is pretty set right now anyhow */



  void
CPL_arrange_toolpads(listptr world_list,stateptr state)
{
  objptr toolbar,padhandle,toolpad;
  vertype targetpos,offset;
  Coord barleft,bartop,baright,barfront,padhandlewidth,padhandleheight;
  CPL_word arranged_toolbar_rec;
  elemptrptr thistoolbar_ep;
  int i;
  ;
  pop_off_CPL_stack(CPL_stack,&arranged_toolbar_rec,TRUE);
  if (arranged_toolbar_rec.word_type != Featurelist_data)
    push_on_CPL_stack(CPL_stack,&arranged_toolbar_rec);
  else
  {
    thistoolbar_ep = arranged_toolbar_rec.word_data.CPLfeaturelist->first.ep;
    while (thistoolbar_ep != Nil)
    {
      toolbar = (objptr) thistoolbar_ep->thiselem;
      set_3d_branchbboxes(toolbar);
      grow_group_bbox(toolbar);
      barleft = toolbar->objbbox.mincorner[vx];
      baright = toolbar->objbbox.maxcorner[vx];
      bartop = toolbar->objbbox.maxcorner[vy];
      barfront = toolbar->objbbox.maxcorner[vz];
      i = 0;
      
      padhandle = toolbar->children->first.obj;
      if (padhandle != Nil)	/* there are actually pads on this toolbar */
      {
	padhandlewidth = padhandle->objbbox.maxcorner[vx] -
	  padhandle->objbbox.mincorner[vx];
	while (padhandle != Nil)
	{
	  toolpad = padhandle->children->first.obj;
	  /* the padhandleheight includes the height of its child, */
	  /* the toolpad. The groupbbox for the padhandle unfortunately */
	  /* includes untranslated tools (children of the pads) which you */
	  /* don't want to include for these calculations. */
	  padhandleheight = padhandle->objbbox.maxcorner[vy] -
	    padhandle->objbbox.mincorner[vy] +
	      toolpad->objbbox.maxcorner[vy] - toolpad->objbbox.mincorner[vy];
	  setpos3d(targetpos,
		   barleft + (baright - barleft) / 2.0 + padhandlewidth,
		   bartop - (padhandleheight * 1.5 * i + padhandleheight / 4.0),
		   padhandle->objbbox.maxcorner[vz]);
	  diffpos3d(padhandle->objbbox.maxcorner,targetpos,offset);
	  translate_feature((featureptr) padhandle,offset,FALSE);
	  padhandle = padhandle->next;
	  i++;
	}
	adjust_toolbar(toolbar);
      }
      thistoolbar_ep = thistoolbar_ep->next;
    }
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

#if 0
  void
CPL_arrange_toolpads(listptr world_list,stateptr state)
{
  objptr toolbar,padhandle,toolpad;
  vertype targetpos,offset;
  Coord barleft,bartop,baright,barfront,padhandlewidth,padhandleheight;
  CPL_word arranged_toolbar_rec;
  elemptrptr thistoolbar_ep;
  int i;
  ;
  pop_off_CPL_stack(CPL_stack,&arranged_toolbar_rec,TRUE);
  if (arranged_toolbar_rec.word_type != Featurelist_data)
    push_on_CPL_stack(CPL_stack,&arranged_toolbar_rec);
  else
  {
    thistoolbar_ep = arranged_toolbar_rec.word_data.CPLfeaturelist->first.ep;
    while (thistoolbar_ep != Nil)
    {
      toolbar = (objptr) thistoolbar_ep->thiselem;
      barleft = toolbar->objbbox.mincorner[vx];
      baright = toolbar->objbbox.maxcorner[vx];
      bartop = toolbar->objbbox.maxcorner[vy];
      barfront = toolbar->objbbox.maxcorner[vz];
      i = 0;
      
      padhandle = toolbar->children->first.obj;
      if (padhandle != Nil)	/* there are actually pads on this toolbar */
      {
	set_3d_branchbboxes(padhandle);
	grow_group_bbox(padhandle);
	padhandlewidth = padhandle->groupbbox.maxcorner[vx] -
	  padhandle->groupbbox.mincorner[vx];
	padhandleheight = padhandle->groupbbox.maxcorner[vy] -
	  padhandle->groupbbox.mincorner[vy];
	while (padhandle != Nil)
	{
	  toolpad = padhandle->children->first.obj;
	  setpos3d(targetpos,
		   barleft + (baright - barleft) / 2.0 + padhandlewidth,
		   bartop - (padhandleheight * 1.5 * i + padhandleheight / 4.0),
		   padhandle->groupbbox.maxcorner[vz]);
	  diffpos3d(padhandle->groupbbox.maxcorner,targetpos,offset);
	  translate_feature((featureptr) padhandle,offset,FALSE);
	  padhandle = padhandle->next;
	  i++;
	}
	adjust_toolbar(toolbar);
      }
      thistoolbar_ep = thistoolbar_ep->next;
    }
  }
}

#endif

  void
CPL_near_toolbar(listptr world_list,stateptr state)
{
  CPL_word toolbar_rec,padhandle_rec,nearby_rec;
  elemptrptr thistoolbar_ep,thispadhandle_ep;
  objptr toolbar,padhandle;
  bbox3dtype toolbarbbox,padhandlebbox;
  Coord toolbardepth
  ;
  pop_off_CPL_stack(CPL_stack,&toolbar_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&padhandle_rec,TRUE);
  setup_CPL_word_blank(&nearby_rec,Featurelist_data);
  if ((toolbar_rec.word_type = Featurelist_data) &&
      (padhandle_rec.word_type == Featurelist_data))
  {
    thistoolbar_ep = toolbar_rec.word_data.CPLfeaturelist->first.ep;
    thispadhandle_ep = padhandle_rec.word_data.CPLfeaturelist->first.ep;
    if ((thistoolbar_ep != Nil) && (thispadhandle_ep != Nil))
    {
      while (thistoolbar_ep != Nil)
      {
	toolbar = (objptr) thistoolbar_ep->thiselem;
	toolbarbbox = toolbar->objbbox;
	/* extend it a bit so that things always hit */
	toolbardepth = toolbarbbox.maxcorner[vz] - toolbarbbox.mincorner[vz];
	toolbarbbox.mincorner[vz] -= toolbardepth;
	toolbarbbox.maxcorner[vz] += toolbardepth;
	padhandle = (objptr) thispadhandle_ep->thiselem;
	padhandlebbox = padhandle->groupbbox;
	
	if (bbox_intersect3d(&toolbarbbox,&padhandlebbox))
	{
	  add_to_featurelist(nearby_rec.word_data.CPLfeaturelist,
			     (featureptr) toolbar);
	  break;
	}
	thistoolbar_ep = thistoolbar_ep->next;
      }
    }
  }
  if (nearby_rec.word_data.CPLfeaturelist->numelems == 0)
    add_to_featurelist(nearby_rec.word_data.CPLfeaturelist,Nil);
  push_on_CPL_stack(CPL_stack,&nearby_rec);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}


  void
CPL_do_color(listptr world_list,stateptr state)
{
  CPL_word painter, paintee;
  featureptr paintoolfeature,painted;
  objptr paintool;
  fveptr paintface;
#ifdef IRIS
  int i = 0,colorval;
#endif
  leptr firstle,thisle;
  Coord *testpt,*redpos,*greenpos,*bluepos;
  Coord a,b,c,d,e,x1,x2,y1,y2,redratio,greenratio,blueratio;
  colortype newcolor;
#ifdef IRIS
  vertype cursorfacepos;
#endif
  ;
  pop_off_CPL_stack(CPL_stack,&paintee,TRUE);
  pop_off_CPL_stack(CPL_stack,&painter,TRUE);
  if ((painter.word_type != Featurelist_data) ||
      (paintee.word_type != Featurelist_data))
    return;

  paintoolfeature = (featureptr) painter.word_data.CPLfeaturelist->
    first.ep->thiselem;
  paintool = (objptr) find_largergrain(paintoolfeature,Obj_type);
  painted = (featureptr) paintee.word_data.CPLfeaturelist->first.ep->thiselem;


  paintface = find_face_given_normal(paintool,z_axis);
  firstle = thisle = paintface->boundlist->first.bnd->lelist->first.le;

  redpos = thisle->facevert->pos;
  greenpos = thisle->next->facevert->pos;
  bluepos = thisle->next->next->facevert->pos;

  testpt = state->cursor;
  
  if ((testpt[vx] < greenpos[vx]) || (testpt[vx] > bluepos[vx]) ||
      (testpt[vy] < greenpos[vy]) || (testpt[vy] > redpos[vy]))
    return;			/* dont do anything if outside triangle */
  /* use similar triangles to find the hypotenuse of the little triangle */
  /* created in the redcorner by the intersection of a horizontal line */
  /* through the testpt and the RG line */
  
  x2 = redpos[vx] - greenpos[vx];
  y2 = redpos[vy] = greenpos[vy];
  y1 = testpt[vy] - greenpos[vy];
  x1 = y1 * (x2 / y2);
  a = sqrt((x1 * x1) + (y1 * y1));
  b = sqrt((x2 * x2) + (y2 * y2));
  c = (bluepos[vx] - greenpos[vx]) - (x1*2);
  d = x2 - (redpos[vx] - testpt[vx] + x1);
  e = c - d;
  redratio =  a / b;
  greenratio = (e / c) * (1.0 - redratio);
  blueratio = (d / c) * (1.0 - redratio);

  newcolor[0] = redratio * 255;
  newcolor[1] = greenratio * 255;
  newcolor[2] = blueratio * 255;
  
  set_feature_color((featureptr) painted,newcolor);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_set_light(listptr world_list,stateptr state)
{
  CPL_word lightbulb_rec;
  featureptr thefeature;
  objptr lightbulb;
  colortype bulbcolor;
  vertype bulbcenter;
  ;
  pop_off_CPL_stack(CPL_stack,&lightbulb_rec,TRUE);
  if (lightbulb_rec.word_type == Featurelist_data)
  {
    thefeature = (featureptr) lightbulb_rec.word_data.CPLfeaturelist->first.ep->
      thiselem;
    if (thefeature != Nil)
    {
      lightbulb = (objptr) find_largergrain(thefeature,Obj_type);
      get_feature_color((featureptr) lightbulb,bulbcolor);
      midpoint(lightbulb->objbbox.mincorner,lightbulb->objbbox.maxcorner,
	       bulbcenter);
      setup_light_source(2,bulbcenter,bulbcolor); 
    }
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_switch_light(listptr world_list,stateptr state)
{
  CPL_word lightbulb_rec,lightbulb_status_rec,lightbulb_color_rec;
  featureptr thefeature;
#ifdef IRIS
  objptr lightbulb;
#endif
  Boolean turn_it_on;
  double scalefactor;
  colortype newcolor;
  ;
  pop_off_CPL_stack(CPL_stack,&lightbulb_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&lightbulb_status_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&lightbulb_color_rec,TRUE);

  if ((lightbulb_rec.word_type == Featurelist_data) &&
      (lightbulb_status_rec.word_type == Auxiliary_data))
  {
    thefeature = (featureptr) lightbulb_rec.word_data.CPLfeaturelist->first.ep->
      thiselem;
    if (thefeature != Nil)
    {			/* supposedly get the lightnumber from thefeature */
      turn_it_on = lightbulb_status_rec.word_data.CPLauxdata.CPLbooleandata;
      switch_light_source(2,turn_it_on);
      if (turn_it_on)
	scalefactor = 2.0;
      else
	scalefactor = 0.5;
      scalecolor(lightbulb_color_rec.word_data.CPLauxdata.CPLcolordata,
		 scalefactor,newcolor);
      copycolor(newcolor,lightbulb_color_rec.word_data.CPLauxdata.CPLcolordata);
    }
  }
  /* pass back the new lightbulb color on the stack */
  push_on_CPL_stack(CPL_stack,&lightbulb_color_rec);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}


  short
make_colorbar_value(objptr colorbar)
{
  Coord height;
  double ratio;
  ;
  height = (colorbar->objbbox.maxcorner[vy] - colorbar->objbbox.mincorner[vy]) -
    Mincolorbarheight;		/* allow for a small minimum size... */
  ratio = height / (Colorbarheight - Mincolorbarheight);
  return((short) (255.0 * ratio));
}
  
  void
CPL_control_color(listptr world_list,stateptr state)
{
  CPL_word colorbar_rec, translation_rec, color_rec;
  featureptr colorbarfeature;
  objptr colorbar,colorbase,thischild;
  colortype assigncolor;
  Coord newcolorbarheight,basetop,bartop,minimum_yposition,cursormove;
  Coord projected_position;
  fveptr ctrlface;
  int i;
  ;
  pop_off_CPL_stack(CPL_stack,&translation_rec,TRUE);
  cursormove = translation_rec.word_data.CPLauxdata.CPLoffsetdata[vy];
  pop_off_CPL_stack(CPL_stack,&colorbar_rec,TRUE);
  if ((translation_rec.word_type != Auxiliary_data) ||
      (colorbar_rec.word_type != Featurelist_data))
    return;
  colorbarfeature = colorbar_rec.word_data.CPLfeaturelist->
    first.ep->thiselem;
  colorbar = (objptr) find_largergrain(colorbarfeature, Obj_type);
  ctrlface = (fveptr) find_subfeature(colorbar,hotspot_prop);
  colorbase = colorbar->parent;
  basetop = colorbase->objbbox.maxcorner[vy];
  bartop = ctrlface->boundlist->first.bnd->lelist->first.le->facevert->pos[vy];
  projected_position = bartop + cursormove;
  minimum_yposition = colorbase->objbbox.maxcorner[vy] + Mincolorbarheight;
  /* make sure to slam the ctrlface if it goes beyond the min/max limits */
  if (projected_position < minimum_yposition)
    cursormove = minimum_yposition - bartop; /* slam at minimum */
  else
    if (projected_position > basetop + Colorbarheight)
      cursormove = basetop + Colorbarheight - bartop; /* slam at maximum */


  /* compute what the new height of the bar will be */
  newcolorbarheight = (bartop + cursormove) - basetop;
  /* compute the color as it is now */
  i = 0;
  thischild = colorbase->children->first.obj;
  while (thischild != Nil)
  {
    if (thischild->obj_name == Nil) /* don't use chip as one of the color bars!*/
    {				
      assigncolor[i] = make_colorbar_value(thischild);
      if (thischild == colorbar)
	assigncolor[i] = (short)
	  ((newcolorbarheight / (Colorbarheight - Mincolorbarheight)) * 255.0);
      i++;
    }
    thischild = thischild->next;
  }
  
  setup_CPL_word_blank(&color_rec,Auxiliary_data);
  copycolor(assigncolor,color_rec.word_data.CPLauxdata.CPLcolordata);
  translation_rec.word_data.CPLauxdata.CPLoffsetdata[vy] = cursormove;

  push_on_CPL_stack(CPL_stack,&color_rec);
  push_on_CPL_stack(CPL_stack,&translation_rec);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}
 
  void
set_colorbar_height(objptr colorbar,short value)
{
  fveptr ctrlface;
  Coord top,base, correctheight,heightdifference;
  vertype offset;
  ;
  ctrlface = find_face_given_normal(colorbar,y_axis); /* find control face */
  top = colorbar->objbbox.maxcorner[vy];
  base = colorbar->objbbox.mincorner[vy];
  correctheight = ((double) value / 255.0) * Colorbarheight;
  heightdifference = -1 * ((top - base) - correctheight);
  setpos3d(offset,0.0,heightdifference,0.0);
  translate_feature((featureptr) ctrlface,offset,FALSE);
}

/* the best routine in the environment_world bar none... heh heh  */

  void
CPL_set_color_bars(listptr world_list,stateptr state)
{
  CPL_word colorbars_rec, painted_set_rec;
  objptr colorbar;
#ifdef IRIS
  colortype painted_set_color,assigncolor;
#else
  colortype painted_set_color;
#endif
  elemptrptr this_ep;
  int i;
  ;
  /* record of all three color bars */
  pop_off_CPL_stack(CPL_stack,&colorbars_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&painted_set_rec,TRUE);
  if ((painted_set_rec.word_type != Featurelist_data) ||
      (colorbars_rec.word_type != Featurelist_data))
    return;
  /* Take the color of the first object in the painted_set_rec only. */
  get_feature_color((featureptr) painted_set_rec.word_data.CPLfeaturelist->
		    first.ep->thiselem,painted_set_color);
  
  i = 0;
  this_ep = colorbars_rec.word_data.CPLfeaturelist->first.ep;
  while (this_ep != Nil)
  {
    colorbar = (objptr) this_ep->thiselem;
    if (colorbar != Nil)
      set_colorbar_height(colorbar,painted_set_color[i++]);
    this_ep = this_ep->next;
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_set_picksize(listptr world_list,stateptr state)
{
  CPL_word newsizerec;
  double newsize;
  ;
  pop_off_CPL_stack(CPL_stack,&newsizerec,TRUE);
  if (newsizerec.word_type != Auxiliary_data)
    return;
  newsize = newsizerec.word_data.CPLauxdata.CPLfloatdata;
  set_global_picksize(newsize);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_setup_scalebbox(listptr world_list, stateptr state)
{
  CPL_word scaled_rec,scalebbox_rec;
  objptr grouproot,thiscalebbox_obj;
  bbox3dptr grouprootbbox,scalebbox;
  /*  Coord diag1,diag2;*/
  vertype mincorner,maxcorner,newmaxcorner,bboxoffset,scale_factor;
  ;
  pop_off_CPL_stack(CPL_stack,&scaled_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&scalebbox_rec,TRUE);
  if ((scaled_rec.word_type == Featurelist_data) &&
      (scalebbox_rec.word_type == Featurelist_data))
  {
    grouproot = (objptr) scaled_rec.word_data.CPLfeaturelist->first.ep->thiselem;
    if (grouproot != Nil)
    {
      thiscalebbox_obj = (objptr) scalebbox_rec.word_data.CPLfeaturelist->
	first.ep->thiselem;
      if (thiscalebbox_obj != Nil)
      {
	if (has_property((featureptr) thiscalebbox_obj,scalebbox_prop))
	{
	  /* hmmm why dont we just set the corners of the scale bbox equal to */
	  /* the group root bbox plus BBoxoutdent...*/
	  set_3d_branchbboxes(grouproot);
	  grow_group_bbox(grouproot);
	  grouprootbbox = &(grouproot->groupbbox);
	  scalebbox = &(thiscalebbox_obj->objbbox);
	  setpos3d(bboxoffset,-BBoxoutdent,-BBoxoutdent,-BBoxoutdent);
	  addpos3d(grouprootbbox->mincorner,bboxoffset,bboxoffset);
	  translate_feature((featureptr) thiscalebbox_obj,bboxoffset,FALSE);
	  addpos3d(scalebbox->mincorner,bboxoffset,mincorner);
	  addpos3d(scalebbox->maxcorner,bboxoffset,maxcorner);
	  setpos3d(bboxoffset,BBoxoutdent,BBoxoutdent,BBoxoutdent);
	  addpos3d(bboxoffset,grouprootbbox->maxcorner,newmaxcorner);
/* for the new scaling method:
	  transformpt(mincorner,thiscalebbox_obj->invxform,mincorner);
	  transformpt(maxcorner,thiscalebbox_obj->invxform,maxcorner);
	  transformpt(newmaxcorner,thiscalebbox_obj->invxform,newmaxcorner);
*/
	  calculate_scale_factors(mincorner,maxcorner,newmaxcorner,scale_factor);
	  scale_feature((featureptr) thiscalebbox_obj,scale_factor,mincorner);
	  log_update((featureptr) thiscalebbox_obj, Transformflag |
		     Evfeqnsflag | Cutevfsflag | Makecutsectionflag |
		     Computebboxflag | Findnormalsflag |
		     Recomputebackfaceflag | Logerasebboxflag,
		     Immediateupdate);
	}
      }
    }
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif

}

  void
find_other_bboxcorner(bbox3dptr thisbbox,vertype thiscorner,vertype othercorner)
{
  if (thiscorner[vx] == thisbbox->mincorner[vx])
    othercorner[vx] = thisbbox->maxcorner[vx];
  else
    othercorner[vx] = thisbbox->mincorner[vx];
  if (thiscorner[vy] == thisbbox->mincorner[vy])
    othercorner[vy] = thisbbox->maxcorner[vy];
  else
    othercorner[vy] = thisbbox->mincorner[vy];
  if (thiscorner[vz] == thisbbox->mincorner[vz])
    othercorner[vz] = thisbbox->maxcorner[vz];
  else
    othercorner[vz] = thisbbox->mincorner[vz];
}  

  void
CPL_scalebbox_trans(listptr world_list,stateptr state)
{
  CPL_word scalebbox_rec,trans_rec,snapped_rec,constrain_rec,switched_rec;
  bbox3dptr motherbbox;
  featureptr scalefeature;
  Boolean constraining_now;
  objptr motherscale,grouproot;
  vertype translation,othercorner,newcorner,newpos,intrpt;
  vertype scalefactor,scale_origin,diagonalvec,edgevec,plane_normal;
  static Boolean switched_to_vfe = FALSE;
  static vertype stored_edgevec;
  Coord *thiscorner,*diagonalcorner;
  double dummyt;
  evfptr thisevf;
  ;
  pop_off_CPL_stack(CPL_stack,&switched_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&constrain_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&trans_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&scalebbox_rec,TRUE);
  if ((scalebbox_rec.word_type == Featurelist_data) &&
      (trans_rec.word_type == Auxiliary_data))
  {
    scalefeature = scalebbox_rec.word_data.CPLfeaturelist->first.ep->thiselem;
    copyvec3d(trans_rec.word_data.CPLauxdata.CPLoffsetdata,translation);
    /* if constrained, make cursor move along some version of XY line. */
    constraining_now = constrain_rec.word_data.CPLauxdata.CPLbooleandata;
    motherscale = (objptr) find_largergrain(scalefeature,Obj_type);
    grouproot = get_unlinked_ancestor(motherscale);
    motherbbox = &(motherscale->objbbox);
    setup_CPL_word_blank(&snapped_rec,Featurelist_data);

    switch (scalefeature->type_id)
    {
    case Vfe_type:
      thiscorner = ((vfeptr) scalefeature)->pos;
      find_other_bboxcorner(motherbbox,thiscorner,othercorner);

      if (constraining_now)
      {
	/* find center of bbox...this will be center of scaling */
	midpoint(othercorner,thiscorner,scale_origin);
	/* find component of translation along the vector from the center of */
	/* scaling to the handle */
	diffvec3d(scale_origin,thiscorner,diagonalvec);
	normalize(diagonalvec,diagonalvec);
	scalevec3d(diagonalvec,dotvecs(translation,diagonalvec),translation);
      }
      else
      {
	/* center of scaling will be the diagonally opposite corner of bbox */
	copypos3d(othercorner,scale_origin);
	/* keep it on the last used edge if there was one... if not, just */
	/* use ztranslation for now... */
	if (switched_rec.word_data.CPLauxdata.CPLbooleandata == TRUE)
	{
	  if (stored_edgevec[vz]*translation[vz] < 0.0)
	    flip_vec(stored_edgevec);
	  scalevec3d(stored_edgevec,
		     magvec(translation)/Fabs(stored_edgevec[vz]),translation);
	}
      }

      addpos3d(thiscorner,translation,newcorner);
/* new scaling:
      transformpt(scale_origin,motherscale->invxform,scale_origin);
      transformpt(thiscorner,motherscale->invxform,thiscorner);
      transformpt(newcorner,motherscale->invxform,newcorner);
*/
      calculate_scale_factors(scale_origin,thiscorner,newcorner,scalefactor);
      scale_feature((featureptr) motherscale,scalefactor,scale_origin);

      copypos3d(translation,trans_rec.word_data.CPLauxdata.CPLoffsetdata);
      break;
    case Evf_type:
      thisevf = (evfptr) scalefeature;
      if (translation[vz] != 0.0) /* moving in z...just make cursor track edge */
      {				/* till we get to the corners */
	/* determine translation along edge which will result in same */
	/* change in Z as actual translation, magnitude of this vector */
	/* will be magnitude(translation)/cos(theta) where theta is angle */
	/* between edge vec and z axis (translation is along z here) */
	diffvec3d(thisevf->le1->facevert->pos,thisevf->le2->facevert->pos,
		  edgevec);
	if (edgevec[vz]*translation[vz] < 0.0)
	  flip_vec(edgevec);
	normalize(edgevec,edgevec);
	scalevec3d(edgevec,magvec(translation)/Fabs(edgevec[vz]),translation);
	addpos3d(thisevf->cutpt,translation,newpos);
	/* see if we translate to or past a bbox corner... */
	if (pt_near_lineseg_3d(thisevf->cutpt,newpos,
			       thisevf->le1->facevert->pos,
			       intrpt,&dummyt,Pkboxsize))
	{
	  /* limit cursor movement to bbox corner */
	  diffpos3d(state->cursor,thisevf->le1->facevert->pos,
		    trans_rec.word_data.CPLauxdata.CPLoffsetdata);
	  /* tell scale CPL that we snapped to bbox corner */
	  add_to_featurelist(snapped_rec.word_data.CPLfeaturelist,
			     (featureptr) thisevf->le1->facevert);
	  switched_rec.word_data.CPLauxdata.CPLbooleandata = TRUE;
	  /* save the edgevec for crossovers into vfe-translations so that */
	  /* on rotated bboxes, the cursor continues to follow the edge vector.*/
	  copypos3d(edgevec,stored_edgevec);
	}
	else if (pt_near_lineseg_3d(thisevf->cutpt,newpos,
				    thisevf->le2->facevert->pos,
				    intrpt,&dummyt,Pkboxsize))
	{
	  /* limit cursor movement to bbox corner */
	  diffpos3d(state->cursor,thisevf->le2->facevert->pos,
		    trans_rec.word_data.CPLauxdata.CPLoffsetdata);
	  /* tell scale CPL that we snapped to bbox corner */
	  add_to_featurelist(snapped_rec.word_data.CPLfeaturelist,
			     (featureptr) thisevf->le2->facevert);
	  switched_rec.word_data.CPLauxdata.CPLbooleandata = TRUE;
	  /* save the edgevec for crossovers into vfe-translations so that */
	  /* on rotated bboxes, the cursor continues to follow the edge vector.*/
	  copypos3d(edgevec,stored_edgevec);
	}
	else
	  copypos3d(translation,trans_rec.word_data.CPLauxdata.CPLoffsetdata);
      }
      else
      {
	thiscorner = thisevf->le1->facevert->pos;  /* arbitrarily use le1 here */
	if (constraining_now)
	{
	  find_other_bboxcorner(motherbbox,thiscorner,othercorner);
	  /* find center of bbox...this will be center of scaling */
	  midpoint(othercorner,thiscorner,scale_origin);
	  /* find component of translation along the vector from the center of */
	  /* scaling to the handle */
	  diffvec3d(scale_origin,thisevf->cutpt,diagonalvec);
	  normalize(diagonalvec,diagonalvec);
	  scalevec3d(diagonalvec,dotvecs(translation,diagonalvec),translation);
	}
	else
	{
	  diagonalcorner =
	                 Twin_le(thisevf->le1->prev)->next->next->facevert->pos;
	  /* center of scaling will be the pt diagonally opposite to cutpt */
	  diffpos3d(thiscorner,diagonalcorner,diagonalvec);
	  addpos3d(thisevf->cutpt,diagonalvec,scale_origin);
	  /* find projection of trans. vec. in the plane perp. to this edge */
	  diffvec3d(thiscorner,thisevf->le2->facevert->pos,plane_normal);
	  normalize(plane_normal,plane_normal);
	  scalevec3d(plane_normal,dotvecs(translation,plane_normal),
		     plane_normal);
	  diffvec3d(plane_normal,translation,translation);
	}
	addpos3d(thisevf->cutpt,translation,newpos);
/* new scale:
	transformpt(scale_origin,motherscale->invxform,scale_origin);
	transformpt(newpos,motherscale->invxform,newpos);
	calculate_scale_factors(scale_origin,thisevf->local_cutpt,newpos,scalefactor);
*/
	calculate_scale_factors(scale_origin,thisevf->cutpt,newpos,scalefactor);

	scale_feature((featureptr) motherscale,scalefactor,scale_origin);
	copypos3d(translation,trans_rec.word_data.CPLauxdata.CPLoffsetdata);
      }
      break;
    default:
      break;
    }
    log_update((featureptr) grouproot,
	       Transformflag | Evfeqnsflag | Cutevfsflag | Makecutsectionflag |
	       Computeplanedistsflag | Computebboxflag |
	       Redrawflag | Recomputebackfaceflag,
	       Immediateupdate);
    if (snapped_rec.word_data.CPLfeaturelist->numelems == 0)
      add_to_featurelist(snapped_rec.word_data.CPLfeaturelist,Nil);
  }
  else
    setup_CPL_word_blank(&snapped_rec,Blank_word);

  push_on_CPL_stack(CPL_stack,&trans_rec);
  push_on_CPL_stack(CPL_stack,&snapped_rec);
  push_on_CPL_stack(CPL_stack,&switched_rec);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_always_set_select(listptr world_list,stateptr state)
{
  CPL_word newsettingrec;
  ;
  pop_off_CPL_stack(CPL_stack,&newsettingrec,TRUE);
  if (newsettingrec.word_type != Auxiliary_data)
    return;
  /* set the global var that determines if selectability routines are always */
  /* run or not. */
  always_set_selectable = newsettingrec.word_data.CPLauxdata.CPLbooleandata;

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_reset_picksize(listptr world_list,stateptr state)
{
#ifdef IRIS
  CPL_word newsizerec;
#endif
  ;
  set_global_picksize(Pkboxsize);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}


/* add an object to the list of objects that are supposed to receive typed */
/* characters (e.g. text input boxes). */

  void
CPL_add_obj_charstream(listptr world_list,stateptr state)
{
  CPL_word add_obj_rec;
  featureptr thefeature;
  ;
  pop_off_CPL_stack(CPL_stack,&add_obj_rec,TRUE);
  if (add_obj_rec.word_type == Featurelist_data)
  {
    thefeature = (featureptr) add_obj_rec.word_data.
      CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
      add_to_featurelist(obj_charstream_list,thefeature);
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

/* remove an object from the list of objects that are supposed to receive */
/* typed characters. */

  void
CPL_rem_obj_charstream(listptr world_list,stateptr state)
{
  CPL_word rem_obj_rec;
  featureptr thefeature;
  ;
  pop_off_CPL_stack(CPL_stack,&rem_obj_rec,TRUE);
  if (rem_obj_rec.word_type == Featurelist_data)
  {
    thefeature = (featureptr) rem_obj_rec.word_data.
      CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
      remove_from_featurelist(obj_charstream_list,thefeature);
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

/* Test an inputted character for inclusion in a given character set... if it */
/* is not in that set, push FALSE on the CPL_stack... */

  void
CPL_filter_char(listptr world_list,stateptr state)
{
  CPL_word filter_set,given_char_rec,return_word;
  char thechar, *filterstr;
  ;
  setup_CPL_auxword(&return_word,CPLaux_boolean_data);
  return_word.word_data.CPLauxdata.CPLbooleandata = FALSE;
  pop_off_CPL_stack(CPL_stack,&given_char_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&filter_set,TRUE);
  if ((given_char_rec.word_type == Auxiliary_data) &&
      (filter_set.word_type == Auxiliary_data))
  {
    thechar = given_char_rec.word_data.CPLauxdata.CPLchardata;
    filterstr = filter_set.word_data.CPLauxdata.CPLstringdata;
    if (strrchr(filterstr,thechar) != NULL) /* relies on strrchar UNIX hack */
      return_word.word_data.CPLauxdata.CPLbooleandata = TRUE;      
  }
  push_on_CPL_stack(CPL_stack,&return_word);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

/* This specialized routine makes sure adding the specified character will */
/* not make the first label of an object exceed its bounding box.  This is */
/* used to clip text labels to input boxes... */

  void
CPL_label_fits(listptr world_list,stateptr state)
{
  CPL_word set_label_rec,given_char_rec,return_word;
  char thechar;
  textstringptr text_string;
  elemptrptr thefeature_ep;
  featureptr thefeature;
  objptr motherobj;
  Coord width1,width2,actual_width;
  static char extra[2] = "X";
  static char newstring[500];	/* better not have a string bigger 'n this!!! */
  ;
  pop_off_CPL_stack(CPL_stack,&set_label_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&given_char_rec,TRUE);
  setup_CPL_auxword(&return_word,CPLaux_boolean_data);
  return_word.word_data.CPLauxdata.CPLbooleandata = TRUE;
  
  if ((set_label_rec.word_type == Featurelist_data) &&
      (given_char_rec.word_type == Auxiliary_data))
  {
    thefeature_ep = set_label_rec.word_data.CPLfeaturelist->first.ep;
    thechar = given_char_rec.word_data.CPLauxdata.CPLchardata;
    extra[0] = thechar;
    while (thefeature_ep != Nil)
    {
      thefeature = thefeature_ep->thiselem;
      if (thefeature != Nil)
      {
	motherobj = (objptr) find_largergrain(thefeature,Obj_type);
	/* right now examine label #1 only although this is bogus */
	text_string = find_label_item(motherobj,1);
	if (text_string->string != Nil)	/* has to have some text already */
	{			/* hardwired for Hershey... oh well */
	  width1 = text_string_width(text_string->string, Hershey,
				     text_string->string_x_scale);
	  width2 = text_string_width(extra, Hershey,
				     text_string->string_x_scale);
	  actual_width = width1 + width2 + text_string->string_offset[vx];
	  if (actual_width > (motherobj->objbbox.maxcorner[vx] -
			      motherobj->objbbox.mincorner[vx]))
	    return_word.word_data.CPLauxdata.CPLbooleandata = FALSE;
	}
      }
      thefeature_ep = thefeature_ep->next;
    }
  }
  push_on_CPL_stack(CPL_stack,&return_word);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}



  void
CPL_add_to_label(listptr world_list,stateptr state)
{
  CPL_word set_label_rec,given_char_rec;
  char thechar;
  textstringptr text_string;
  elemptrptr thefeature_ep;
  featureptr thefeature;
  objptr motherobj;
#ifdef IRIS
  int lastlen;
  char *new_string;
#endif
  static char extra[2] = "X";
  ;
  pop_off_CPL_stack(CPL_stack,&set_label_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&given_char_rec,TRUE);
  if ((set_label_rec.word_type == Featurelist_data) &&
      (given_char_rec.word_type == Auxiliary_data))
  {
    thefeature_ep = set_label_rec.word_data.CPLfeaturelist->first.ep;
    thechar = given_char_rec.word_data.CPLauxdata.CPLchardata;
    extra[0] = thechar;
    while (thefeature_ep != Nil)
    {
      thefeature = thefeature_ep->thiselem;
      if (thefeature != Nil)
      {
	motherobj = (objptr) find_largergrain(thefeature,Obj_type);
	/* right now set label #1 only although this is bogus */
	text_string = find_label_item(motherobj,1);
	if (text_string->string == Nil)
	{
	  set_label_item(motherobj,1,extra);
	  set_label_insertion_point(motherobj,1,strlen(extra));
	}
	else
	{
	  insert_string_into_label(motherobj,1,extra);
	  move_label_insertion_point(motherobj,1,1);
	}
      }
      thefeature_ep = thefeature_ep->next;
    }
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_backspace_label(listptr world_list,stateptr state)
{
#ifdef IRIS
  CPL_word set_label_rec;
#else
  CPL_word set_label_rec,given_char_rec;
  char thechar;
#endif
  textstringptr text_string;
  elemptrptr thefeature_ep;
  featureptr thefeature;
  objptr motherobj;
  /* int lastlen; */
  /* char *new_string; */
  ;
  pop_off_CPL_stack(CPL_stack,&set_label_rec,TRUE);
  if (set_label_rec.word_type == Featurelist_data)
  {
    thefeature_ep = set_label_rec.word_data.CPLfeaturelist->first.ep;
    while (thefeature_ep != Nil)
    {
      thefeature = thefeature_ep->thiselem;
      if (thefeature != Nil)
      {
	motherobj = (objptr) find_largergrain(thefeature,Obj_type);
	/* right now backspace label #1 only although this is bogus */
	text_string = find_label_item(motherobj,1);
	if (text_string->string != Nil)
	{	  /* remove from end but should remove from ibeam point */
	  if (strlen(text_string->string) == 1)	/* end case: erase it totally */
	    free_elem((elemptr *) &(text_string->string));
	  else
	    remove_chars_from_label(motherobj,1,text_string->insertion_point-1,
				    text_string->insertion_point);
	  move_label_insertion_point(motherobj,1,-1);
	}
      }
      thefeature_ep = thefeature_ep->next;
    }
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_set_insertion_pt(listptr world_list,stateptr state)
{
  CPL_word set_label_rec,new_insertpt_rec;
#ifdef IRIS
  textstringptr text_string;*/
#endif
  elemptrptr thefeature_ep;
  featureptr thefeature;
  objptr motherobj;
  int new_insertpt_pos;
  ;
  pop_off_CPL_stack(CPL_stack,&set_label_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&new_insertpt_rec,TRUE);
  if ((set_label_rec.word_type == Featurelist_data) &&
      (new_insertpt_rec.word_type == Auxiliary_data))
  {
    thefeature_ep = set_label_rec.word_data.CPLfeaturelist->first.ep;
    /* The insertion points -1 and 10000 mean something special... */
    /* CPL passes -1 if it wants the insert point to go away, and */
    /* 10000 if it wants the insert pt to be at the end... assuming */
    /* you don't have any displayed strings 10000 characters long! */
    
    new_insertpt_pos = new_insertpt_rec.word_data.CPLauxdata.CPLintdata;
    
    while (thefeature_ep != Nil)
    {
      thefeature = thefeature_ep->thiselem;
      if (thefeature != Nil)
      {
	motherobj = (objptr) find_largergrain(thefeature,Obj_type);
	/* right now set point in  label #1 only although this is bogus */
	set_label_insertion_point(motherobj,1,new_insertpt_pos);
      }
      thefeature_ep = thefeature_ep->next;
    }
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_move_insertion_pt(listptr world_list,stateptr state)
{
  CPL_word set_label_rec,move_rec;
  textstringptr text_string;
  elemptrptr thefeature_ep;
  featureptr thefeature;
  objptr motherobj;
  ;
  pop_off_CPL_stack(CPL_stack,&set_label_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&move_rec,TRUE);
  if ((set_label_rec.word_type == Featurelist_data) &&
      (move_rec.word_type == Auxiliary_data))
  {
    thefeature_ep = set_label_rec.word_data.CPLfeaturelist->first.ep;
    while (thefeature_ep != Nil)
    {
      thefeature = thefeature_ep->thiselem;
      if (thefeature != Nil)
      {
	motherobj = (objptr) find_largergrain(thefeature,Obj_type);
	move_label_insertion_point(motherobj,1,
				   move_rec.word_data.CPLauxdata.CPLintdata);
      }
      thefeature_ep = thefeature_ep->next;
    }
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}


  void
CPL_read_from_file(listptr world_list,stateptr state)
{
  CPL_word read_filename_rec;
  char *givenfilename;
  ;
  pop_off_CPL_stack(CPL_stack,&read_filename_rec,TRUE);
  if (read_filename_rec.word_type == Auxiliary_data)
  {
    givenfilename = read_filename_rec.word_data.CPLauxdata.CPLstringdata;
    read_data_file(givenfilename,Nil); /* right now just read as child of root */
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}


  void
CPL_write_to_file(listptr world_list,stateptr state)
{
  CPL_word save_filename_rec,saved_feature_rec;
  char *givenfilename;
  featureptr saved_feature;
  ;
  pop_off_CPL_stack(CPL_stack,&save_filename_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&saved_feature_rec,TRUE);
  if ((save_filename_rec.word_type == Auxiliary_data) &&
      ((saved_feature_rec.word_type == Featurelist_data) ||
      (saved_feature_rec.word_type == Auxiliary_data)))
  {
    givenfilename = save_filename_rec.word_data.CPLauxdata.CPLstringdata;
    /* right now if you're saving picked features, it assumes INDEX was used */
    /* first and that only one feature will be saved into a file (one object */
    /* per branch file... kinda limiting but there really needs an file */
    /* append for better usage). */
    if (saved_feature_rec.word_type == Auxiliary_data)
      saved_feature = (featureptr) saved_feature_rec.word_data.CPLauxdata.
	new_world;
    else
      saved_feature = (featureptr)
	saved_feature_rec.word_data.CPLfeaturelist->first.ep->thiselem;
    if (strstr(givenfilename,".ps") != NULL)
    {
      printf ("Writing Postscript file %s...",givenfilename);
#ifdef IRIS
      plot_obj_branch(((worldptr) saved_feature)->root,givenfilename);
#endif
      printf ("Done writing Postscript file..\n");
    }
    else
      write_data_file(givenfilename,saved_feature);
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

/* Set the size of a given feature passed on the stack to the amount */
/* passed on the stack... right now only meaningful for edges... also */
/* right now units in inches approx.. */

  void
CPL_get_feature_size(listptr world_list,stateptr state)
{
  CPL_word sized_feature_rec,size_amt_rec;
  /*  float size_amt; */
  featureptr sized_feature;
  /* borrowed from old shit in user.c (cyberthon) */
  Coord *pt1,*pt2;
  vertype pt3;
  Coord length;
  evfptr thisevf;
  ;
  pop_off_CPL_stack(CPL_stack,&sized_feature_rec,TRUE);
  setup_CPL_auxword(&size_amt_rec,CPLaux_float_data);
  if (sized_feature_rec.word_type == Featurelist_data)
  {
    sized_feature = (featureptr) sized_feature_rec.word_data.
      CPLfeaturelist->first.ep->thiselem;
    if (sized_feature != Nil)
    {
      switch (sized_feature->type_id)
      {
      case Vfe_type:
      case Le_type:
      case Bound_type:
      case Fve_type:
      case Shell_type:
      case Obj_type:
      case World_type:
      case Seg_type:
      case Ring_type:
      case Curve_type:
      case Surf_type:
	break;
      case Evf_type:
	thisevf = (evfptr) sized_feature;
	pt1 = thisevf->le1->facevert->pos;
	pt2 = thisevf->le2->facevert->pos;
	diffpos3d(pt1,pt2,pt3);
	length = magvec(pt3);
	if (has_property(sized_feature,scalebbox_prop))
	  length -= BBoxoutdent * 2; /* remove bbox outdenting from reported */
				     /* value!!! hackereedeedee */
	size_amt_rec.word_data.CPLauxdata.CPLfloatdata = length;
	break;
      default:
	break;
      }
    }
  }
  push_on_CPL_stack(CPL_stack,&size_amt_rec);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

/* Set the size of a given feature passed on the stack to the amount */
/* passed on the stack... right now only meaningful for edges... also */
/* right now units in inches approx.. */

  void
CPL_set_feature_size(listptr world_list,stateptr state)
{
  CPL_word sized_feature_rec,size_amt_rec;
  double size_amt;
  featureptr sized_feature;
  Coord *pt1,*pt2;
  vertype evfmidpt,evfvec,unit_evfvec,half_newevfvec,half_oldevfvec;
  vertype scalevec,newpt1,newpt2,oldpt1;
  Coord length;
  evfptr thisevf;
  objptr motherbox;
  ;
  pop_off_CPL_stack(CPL_stack,&size_amt_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&sized_feature_rec,TRUE);
  if ((sized_feature_rec.word_type == Featurelist_data) &&
      (size_amt_rec.word_type == Auxiliary_data))
  {
    sized_feature = (featureptr) sized_feature_rec.word_data.
      CPLfeaturelist->first.ep->thiselem;
    if ((size_amt_rec.aux_kind == CPLaux_float_data) && (sized_feature != Nil))
    {
      size_amt = size_amt_rec.word_data.CPLauxdata.CPLfloatdata;
      switch (sized_feature->type_id)
      {
      case Vfe_type:
      case Le_type:
      case Bound_type:
      case Fve_type:
      case Shell_type:
      case Obj_type:
      case World_type:
      case Seg_type:
      case Ring_type:
      case Curve_type:
      case Surf_type:
	break;
      case Evf_type:
	thisevf = (evfptr) sized_feature;
	pt1 = thisevf->le1->facevert->pos;
	pt2 = thisevf->le2->facevert->pos;
	diffpos3d(pt1,pt2,evfvec);
	length = magvec(evfvec);
	normalize(evfvec,unit_evfvec);
	midpoint(pt1,pt2,evfmidpt);
	scalevec3d(unit_evfvec,size_amt/2.0,half_newevfvec);
	addvec3d(evfmidpt,half_newevfvec,newpt1);
	if (has_property(sized_feature,scalebbox_prop))
	{
	  /* we add Bboxoutdent * 2 to the given size to make the resulting */
	  /* size be the length the user really wants...*/
	  /* hmmm the above comment doesnt agree with the code...is this right */
	  motherbox = (objptr) find_largergrain(sized_feature,Obj_type);
	  scalevec3d(unit_evfvec,length/2.0 - BBoxoutdent,half_oldevfvec);
	  addpos3d(evfmidpt,half_oldevfvec,oldpt1);
/* new scaling
	  transformpt(evfmidpt,motherbox->invxform,evfmidpt);
	  transformpt(oldpt1,motherbox->invxform,oldpt1);
	  transformpt(newpt1,motherbox->invxform,newpt1);
*/
	  calculate_scale_factors(evfmidpt,oldpt1,newpt1,scalevec);
	  scale_feature((featureptr) motherbox,scalevec,evfmidpt);
	}
	else
	{
	  flip_vec(half_newevfvec);
	  addvec3d(evfmidpt,half_newevfvec,newpt2);
	  setvfe(thisevf->le1->facevert,newpt2,
		 thisevf->le1->motherbound->motherfve->mothershell->motherobj->
		 invxform);
	  setvfe(thisevf->le2->facevert,newpt1,
		 thisevf->le2->motherbound->motherfve->mothershell->motherobj->
		 invxform);
	}
	break;
      default:
	break;
      }
    }
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_get_feature_dist(listptr world_list, stateptr state)
{
  CPL_word feature1_rec,feature2_rec,dist_rec;
  Coord *pt1,*pt2,*pt3,*pt4,hypot = 0.0,xdist = 0.0,ydist = 0.0,zdist = 0.0;
  double dummyt;
  fveptr fve1,fve2;
  featureptr feature1,feature2;
  vertype distvec,intrpt,intrvec;
  ;
  pop_off_CPL_stack(CPL_stack,&feature2_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&feature1_rec,TRUE);
  setup_CPL_auxword(&dist_rec,CPLaux_float_data);
  dist_rec.word_data.CPLauxdata.CPLfloatdata = 0.0;
  if ((feature1_rec.word_type == Featurelist_data) &&
      (feature2_rec.word_type == Featurelist_data))
  {
    feature1 = (featureptr) feature1_rec.word_data.CPLfeaturelist->
      first.ep->thiselem;
    feature2 = (featureptr) feature2_rec.word_data.CPLfeaturelist->
      first.ep->thiselem;
    /* this handles vfe-vfe,vfe-evf,vfe-fve, evf-evf, evf-fve, and fve-fve */
    /* dists only. "Not bad... for a human..." */
    if (feature1->type_id == Vfe_type)
    {
      pt1 = ((vfeptr) feature1)->pos;
      if (feature2->type_id == Vfe_type)
      {				/* vfe-vfe */
	pt2 = ((vfeptr) feature2)->pos;
	diffpos3d(pt1,pt2,distvec);
	hypot = magvec(distvec);
	xdist = distvec[vx]; ydist = distvec[vy]; zdist = distvec[vz];
      }
      else if (feature2->type_id == Evf_type)
      {				/* vfe-evf */
	pt3 = ((evfptr) feature2)->le1->facevert->pos;
	pt4 = ((evfptr) feature2)->le2->facevert->pos;
	hypot = pt_to_line_dist_3d(pt1,pt3,pt4);
      }
      else if (feature2->type_id == Fve_type)
      {				/* vfe-fve */
	fve1 = ((fveptr) feature2);
	hypot = pt_to_planedist_3d(pt1,fve1->facenorm,fve1->planedist);
      }
    }
    else if (feature1->type_id == Evf_type)
    {
      pt1 = ((evfptr) feature1)->le1->facevert->pos;
      pt2 = ((evfptr) feature1)->le2->facevert->pos;
      if (feature2->type_id == Vfe_type) /* evf-vfe */
	hypot = pt_to_line_dist_3d(((vfeptr) feature2)->pos,pt1,pt2);
      else if (feature2->type_id == Evf_type) /* evf-evf */
      {
	pt3 = ((evfptr) feature2)->le1->facevert->pos;
	pt4 = ((evfptr) feature2)->le2->facevert->pos;
	xdist = ydist = zdist = 0.0;
	hypot = line_to_line_dist_3d(pt1,pt2,pt3,pt4);
	push_on_CPL_stack(CPL_stack,&dist_rec);
      }
      else if (feature2->type_id == Fve_type) /* evf-fve */
      {
	fve1 = ((fveptr) feature2);
	if ((line_intersect_plane_3d(pt1,pt2,fve1->facenorm,fve1->planedist,
				     intrpt,&dummyt)) == Lineparallelsplane)
	  hypot = pt_to_planedist_3d(pt1,fve1->facenorm,fve1->planedist);
      }
    }
    else if (feature1->type_id == Fve_type) 
    {
      if (feature2->type_id == Vfe_type) /* fve-vfe */
      {
	pt3 = ((vfeptr) feature2)->pos;
	fve1 = (fveptr) feature1;
	hypot = pt_to_planedist_3d(pt3,fve1->facenorm,fve1->planedist);
      }
      else if (feature2->type_id == Evf_type) /* fve-evf */
      {
	pt3 = ((evfptr) feature2)->le1->facevert->pos;
	pt4 = ((evfptr) feature2)->le2->facevert->pos;
	fve1 = ((fveptr) feature1);
	if ((line_intersect_plane_3d(pt3,pt4,fve1->facenorm,fve1->planedist,
				     intrpt,&dummyt)) == Lineparallelsplane)
	  hypot = pt_to_planedist_3d(pt3,fve1->facenorm,fve1->planedist);
      }
      else if (feature2->type_id == Fve_type) /* fve-fve */
      {
	fve1 = (fveptr) feature1;
	fve2 = (fveptr) feature2;
	if (!planes_intersect(fve1->facenorm,fve1->planedist,
			      fve2->facenorm,fve2->planedist,
			      intrvec,intrpt,Planetoplanetol))
	  /* planes parallel */
	{
	  if (dotvecs(fve1->facenorm,fve2->facenorm) < 0)
	    /* if normals opposite, reverse sign. */
	    hypot = Fabs(fve2->planedist + fve1->planedist);
	  else
	    hypot = Fabs(fve2->planedist - fve1->planedist);
	}
      }
    }
  }
  dist_rec.word_data.CPLauxdata.CPLfloatdata = zdist;
  push_on_CPL_stack(CPL_stack,&dist_rec);
  dist_rec.word_data.CPLauxdata.CPLfloatdata = ydist;
  push_on_CPL_stack(CPL_stack,&dist_rec);
  dist_rec.word_data.CPLauxdata.CPLfloatdata = xdist;
  push_on_CPL_stack(CPL_stack,&dist_rec);
  dist_rec.word_data.CPLauxdata.CPLfloatdata = hypot;
  push_on_CPL_stack(CPL_stack,&dist_rec);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_set_feature_dist(listptr world_list, stateptr state)
{
  CPL_word feature1_rec,feature2_rec,hypot_rec;
  CPL_word xdist_rec,ydist_rec,zdist_rec;
  Coord *pt1,*pt2,*pt3,*pt4,hypot,requested_hypot,*gonorm1,*gonorm2;
  vertype requested_dist_vec;
  double dummyt,ratio;
  featureptr feature1,feature2;
  fveptr fve1,fve2;
  objptr mother1,mother2;
  vertype distvec,intrpt,intrvec,newoffsetvec;
  ;
  pop_off_CPL_stack(CPL_stack,&feature2_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&feature1_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&zdist_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&ydist_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&xdist_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&hypot_rec,TRUE);
  requested_dist_vec[vx] = xdist_rec.word_data.CPLauxdata.CPLfloatdata;
  requested_dist_vec[vy] = ydist_rec.word_data.CPLauxdata.CPLfloatdata;
  requested_dist_vec[vz] = zdist_rec.word_data.CPLauxdata.CPLfloatdata;
  requested_hypot = hypot_rec.word_data.CPLauxdata.CPLfloatdata;
  if ((feature1_rec.word_type == Featurelist_data) &&
      (feature2_rec.word_type == Featurelist_data))
  {
    feature1 = (featureptr) feature1_rec.word_data.CPLfeaturelist->
      first.ep->thiselem;
    feature2 = (featureptr) feature2_rec.word_data.CPLfeaturelist->
      first.ep->thiselem;
    mother1 = (objptr) find_largergrain(feature1,Obj_type);
    mother2 = (objptr) find_largergrain(feature2,Obj_type);
    /* this handles vfe-vfe,vfe-evf,vfe-fve, evf-evf, evf-fve, and fve-fve */
    /* dists only. "Not bad... for a human..." */
    if (feature1->type_id == Vfe_type)
    {
      pt1 = ((vfeptr) feature1)->pos;
      if (feature2->type_id == Vfe_type)
      {				/* vfe-vfe */
	pt2 = ((vfeptr) feature2)->pos;
	diffpos3d(pt2,pt1,distvec);
	addvec3d(distvec,requested_dist_vec,newoffsetvec);
	translate_feature((featureptr) mother2,newoffsetvec,FALSE);
      }
      else if (feature2->type_id == Evf_type)
      {				/* vfe-evf */
      }
      else if (feature2->type_id == Fve_type)
      {				/* vfe-fve */
	fve1 = ((fveptr) feature2);
	hypot = pt_to_planedist_3d(pt1,fve1->facenorm,fve1->planedist);
	ratio = requested_hypot / hypot;
	/* snap face to vfe, then translate face to requested distance. */
	scalevec3d(fve1->facenorm,-hypot,newoffsetvec);
	translate_feature((featureptr) mother2,newoffsetvec,FALSE);
	scalevec3d(fve1->facenorm,requested_hypot,newoffsetvec);
	translate_feature((featureptr) mother2,newoffsetvec,FALSE);
      }
    }
    else if (feature1->type_id == Evf_type)
    {
      pt1 = ((evfptr) feature1)->le1->facevert->pos;
      pt2 = ((evfptr) feature1)->le2->facevert->pos;
      if (feature2->type_id == Vfe_type) /* evf-vfe */
	hypot = pt_to_line_dist_3d(((vfeptr) feature2)->pos,pt1,pt2);
      else if (feature2->type_id == Evf_type) /* evf-evf */
      {
	pt3 = ((evfptr) feature2)->le1->facevert->pos;
	pt4 = ((evfptr) feature2)->le2->facevert->pos;
	hypot = line_to_line_dist_3d(pt1,pt2,pt3,pt4);
      }
      else if (feature2->type_id == Fve_type) /* evf-fve */
      {
	fve1 = ((fveptr) feature2);
	if ((line_intersect_plane_3d(pt1,pt2,fve1->facenorm,fve1->planedist,
				     intrpt,&dummyt)) == Lineparallelsplane)
	  hypot = pt_to_planedist_3d(pt1,fve1->facenorm,fve1->planedist);
      }
    }
    else if (feature1->type_id == Fve_type) 
    {
      if (feature2->type_id == Vfe_type) /* fve-vfe */
      {
	pt3 = ((vfeptr) feature2)->pos;
	fve1 = (fveptr) feature1;
	hypot = pt_to_planedist_3d(pt3,fve1->facenorm,fve1->planedist);
      }
      else if (feature2->type_id == Evf_type) /* fve-evf */
      {
	pt3 = ((evfptr) feature2)->le1->facevert->pos;
	pt4 = ((evfptr) feature2)->le2->facevert->pos;
	fve1 = ((fveptr) feature1);
	if ((line_intersect_plane_3d(pt3,pt4,fve1->facenorm,fve1->planedist,
				     intrpt,&dummyt)) == Lineparallelsplane)
	  hypot = pt_to_planedist_3d(pt3,fve1->facenorm,fve1->planedist);
      }
      else if (feature2->type_id == Fve_type) /* fve-fve */
      {
	fve1 = (fveptr) feature1;
	fve2 = (fveptr) feature2;
	if (!planes_intersect(fve1->facenorm,fve1->planedist,
			      fve2->facenorm,fve2->planedist,
			      intrvec,intrpt,Planetoplanetol))
	  /* planes parallel */
	{
	  if (dotvecs(fve1->facenorm,fve2->facenorm) < 0)
	    /* if normals opposite, reverse sign. */
	  {
	    hypot = Fabs(fve2->planedist + fve1->planedist);
	    /* are faces both pointing towards each other or both away from */
	    /* each other? */
	    if (((Fabs(fve1->planedist) > Fabs(fve2->planedist)) &&
		 (fve1->planedist < 0)) ||
		((Fabs(fve1->planedist) < Fabs(fve2->planedist)) &&
		 (fve1->planedist > 0)))
	    {			/* normals point away from each other*/
	      gonorm1 = fve2->facenorm;
	      gonorm2 = fve1->facenorm;
	    }
	    else
	    { /* normals point towards each other*/
	      gonorm1 = fve1->facenorm;
	      gonorm2 = fve2->facenorm;
	    }
	    /* snap face to face, then translate face to requested distance. */
	    scalevec3d(gonorm1,hypot,newoffsetvec);
	    translate_feature((featureptr) mother2, newoffsetvec,FALSE);
	    scalevec3d(gonorm2,requested_hypot,newoffsetvec);
	    translate_feature((featureptr) mother2, newoffsetvec,FALSE);
	  }
	  else			/* normals not opposite */
	  {
	    hypot = Fabs(fve2->planedist - fve1->planedist);
	    scalevec3d(fve1->facenorm,-hypot,newoffsetvec);
	    translate_feature((featureptr) mother2, newoffsetvec,FALSE);
	    scalevec3d(fve1->facenorm,requested_hypot,newoffsetvec);
	    translate_feature((featureptr) mother2, newoffsetvec,FALSE);
	  } 
	}
      }
    }
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}


  void
CPL_remake_piechart(listptr world_list, stateptr state)
{
  CPL_word chart_plate_rec;
  objptr old_chart,chart_plate,piechart,chart_pad;
  ;
  pop_off_CPL_stack(CPL_stack,&chart_plate_rec,TRUE);
  
  if (chart_plate_rec.word_type == Featurelist_data)
  {
    old_chart = (objptr) find_object_in_any_world("Piechart");
    chart_pad = (objptr) find_object_in_any_world("PiechartPad");
    kill_obj_branch(old_chart);
    chart_plate = (objptr)
      chart_plate_rec.word_data.CPLfeaturelist->first.ep->thiselem;
    piechart = create_piechart(primary_world,100.0,50.0,34,chart_plate);
    set_property((featureptr) chart_pad,connection_prop,piechart);
    log_global_update(Redrawflag);
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_toggle_slide(listptr world_list, stateptr state)
{
  static Boolean shown = FALSE;
  ;
  shown = !shown;
  pushpop_slide(shown);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_set_camera(listptr world_list, stateptr state)
{
  CPL_word camera_rec;
  featureptr camera_feature;
  objptr thecamera,camera_lens;
  fveptr camera_lens_face;
  vertype centerpt;
  ;
  pop_off_CPL_stack(CPL_stack,&camera_rec,TRUE);
  if (camera_rec.word_type == Featurelist_data)
  {
    camera_feature = camera_rec.word_data.CPLfeaturelist->first.ep->thiselem;
    if (camera_feature != Nil)
    {
      thecamera = (objptr) camera_feature;
      camera_lens = thecamera->children->first.obj;
      camera_lens_face = camera_lens->shellist->first.shell->fvelist->first.fve;
      compute_face_centroid(camera_lens_face,centerpt);
      set_camera_view(camera_lens_face->facenorm,centerpt);
    }
  }

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_rotate_camera(listptr world_list, stateptr state)
{
  CPL_word camera_rec,lens_rec,trans_rec,rotate_rec;
  featureptr camera_feature;
  objptr thecamera,camera_lens;
  fveptr camera_lens_face;
  vertype camera_centerpt,lens_centerpt,new_centerpt,v1,v2;
  Coord *translation;
  CPLauxptr rotrec;
  ;
  pop_off_CPL_stack(CPL_stack,&trans_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&camera_rec,TRUE);
  pop_off_CPL_stack(CPL_stack,&lens_rec,TRUE);
  setup_CPL_auxword(&rotate_rec,CPLaux_rotate_data);
  if ((camera_rec.word_type == Featurelist_data) &&
      (lens_rec.word_type == Featurelist_data) &&
      (trans_rec.word_type == Auxiliary_data))
  {
    camera_feature = camera_rec.word_data.CPLfeaturelist->first.ep->thiselem;
    if (camera_feature != Nil)
    {
      translation = trans_rec.word_data.CPLauxdata.CPLoffsetdata;
      thecamera = (objptr) camera_feature;
      camera_lens = (objptr)
	lens_rec.word_data.CPLfeaturelist->first.ep->thiselem;
      camera_lens_face = camera_lens->shellist->first.shell->fvelist->first.fve;
      compute_face_centroid(camera_lens_face,lens_centerpt);
      compute_obj_centroid(thecamera,camera_centerpt);
      copypos3d(state->cursor,lens_centerpt);
      addpos3d(lens_centerpt,translation,new_centerpt);
      diffpos3d(camera_centerpt,lens_centerpt,v1);
      diffpos3d(camera_centerpt,new_centerpt,v2);
      normalize(v1,v1);
      normalize(v2,v2);
      rotrec = &(rotate_rec.word_data.CPLauxdata);
      cross_prod(v1,v2,rotrec->rotate_data.rotvec);
      rotrec->rotate_data.rotangle =
	(int) (Rad2deg * (Acos(dotvecs(v1,v2)))) * 10;
      copypos3d(camera_centerpt,rotrec->rotate_data.rotpt);
    }
  }
  push_on_CPL_stack(CPL_stack,&rotate_rec);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_turnon_arrowcursor(listptr world_list, stateptr state)
{
#ifdef IRIS

  long thedevice,width,height;
  short thedata;
  Boolean leftmousewentdown, stillwaiting = TRUE;
  ;
  setcursor(0,0,0);
  curson();
#undef setvaluator
#undef getvaluator
  setvaluator(MOUSEX,getvaluator(MOUSEX),0,XMAXSCREEN);
  setvaluator(MOUSEY,getvaluator(MOUSEY),0,YMAXSCREEN);
#define setvaluator gl_setvaluator
#define getvaluator gl_getvaluator
  qreset();

  do
  {
    thedevice = qread(&thedata);
    switch (thedevice)
    {
    case REDRAW:
      winset(mainwindow);
      getsize(&width,&height);
      getorigin((long *) &mainwindowleft,(long *) &mainwindowbottom);
      mainwindowright = mainwindowleft + width - 1;
      mainwindowtop = mainwindowbottom + height - 1;
      reshapeviewport();

      init_projmatrix();
      make_vanilla_matrix(state->roomrotx,state->roomroty);

      clear_view();
      draw_underlying_objects(state,oldstate);
      /* Object drawing routines. */
      draw_worlds(Draweverything); /* draw all objects no matter what */
      /* framebuffer comes round */
      swapbuffers();  /* show it! */

      clear_view();
      draw_underlying_objects(state,oldstate);
      /* Object drawing routines. */
      draw_worlds(Draweverything); /* draw all objects no matter what */
      /* framebuffer comes round */
      log_global_update(Allupdateflags);
      break;
    case LEFTMOUSE:
      leftmousewentdown = (thedata == 0);
    case MIDDLEMOUSE:
    case RIGHTMOUSE:
      stillwaiting = FALSE;
      break;
    }
  } while (stillwaiting);

  qread(&thedata);
  setcursor(0,0,0);
  cursoff();
  init_controls(state);

#endif /* IRIS */


#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_set_hiddenedge(listptr world_list, stateptr state)
{
  CPL_word hidden_rec;
  ;
  pop_off_CPL_stack(CPL_stack, &hidden_rec,TRUE);
  if (hidden_rec.word_data.CPLauxdata.CPLbooleandata == TRUE)
    use_hidden_line_technique = use_hidden_line_technique + 1;
  else if (use_hidden_line_technique > 0)
    use_hidden_line_technique = use_hidden_line_technique - 1;

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}


  void
CPL_cursor_on_border(listptr world_list, stateptr state)
{
  CPL_word returnval;
  ;
  setup_CPL_auxword(&returnval,CPLaux_boolean_data);
  returnval.word_data.CPLauxdata.CPLbooleandata =
    ((compare(Fabs(state->cursor[vx]),Maxcrossx,5.0) == 0) ||
      (compare(Fabs(state->cursor[vy]),Maxcrossy,5.0) == 0));
  push_on_CPL_stack(CPL_stack,&returnval);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}


  void
CPL_normal_constrain(listptr world_list, stateptr state)
{
  CPL_word cursor_delta_rec;
  Coord *delta;
  ;
  pop_off_CPL_stack(CPL_stack,&cursor_delta_rec,TRUE);
  delta = cursor_delta_rec.word_data.CPLauxdata.CPLoffsetdata;
  constrain_movecrosshair(delta);
  push_on_CPL_stack(CPL_stack,&cursor_delta_rec);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_remake_prim(listptr world_list, stateptr state)
{
  CPL_word oldobj_rec,newobj_rec,primrecptr_word;
  prim_rec_ptr new_prim_rec;
  cube_rec_ptr cube_info;
  cylinder_rec_ptr cylinder_info;
  objptr oldcube,newobj = Nil;
  ;
  pop_off_CPL_stack(CPL_stack,&primrecptr_word,TRUE);
  pop_off_CPL_stack(CPL_stack,&oldobj_rec,TRUE);
  new_prim_rec = (prim_rec_ptr) primrecptr_word.word_data.
    CPLauxdata.CPLaddressdata;
  switch (new_prim_rec->prim_id)
  {
  case Cube_id:
    cube_info = (cube_rec_ptr) new_prim_rec->prim_info;
    printf("You wish to make a cube with the following dimensions great sir:\n");
    printf("L:%f,W:%f,H:%f\n",cube_info->length,cube_info->width,
	   cube_info->height);
    oldcube = (objptr) oldobj_rec.word_data.CPLfeaturelist->first.ep->thiselem;
    newobj = recreate_cube(oldcube,cube_info->length,cube_info->width,
			   cube_info->height);
    break;
  case Cylinder_id:
  case Cone_id:
  case Sphere_id:
  case Pyramid_id:
  default:
    break;
  }  

  setup_CPL_word_blank(&newobj_rec,Featurelist_data);
  if (newobj != Nil)
    add_to_featurelist(newobj_rec.word_data.CPLfeaturelist,(featureptr) newobj);
  else
    clear_word(&newobj_rec);
  push_on_CPL_stack(CPL_stack,&newobj_rec);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_get_plate_by_name(listptr world_list, stateptr state)
{
  CPL_word primrecptr_word;
  prim_rec_ptr old_prim_rec;
  char *plate_name;
  objptr the_plate;
  ;
  pop_off_CPL_stack(CPL_stack,&primrecptr_word,TRUE);
  old_prim_rec = (prim_rec_ptr) primrecptr_word.word_data.
    CPLauxdata.CPLaddressdata;
  plate_name = prim_create_recs[old_prim_rec->prim_id].prim_info_platename;
  the_plate = find_object_in_any_world(plate_name);
  if (the_plate != Nil)
  {
    setup_CPL_word(&primrecptr_word,Featurelist_data);
    add_to_featurelist(primrecptr_word.word_data.CPLfeaturelist,(featureptr)
		       the_plate);
  }
  else
    clear_word(&primrecptr_word);
  push_on_CPL_stack(CPL_stack,&primrecptr_word);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  
  void
CPL_generate_info_rec(listptr world_list, stateptr state)
{
  CPL_word primrecptr_word, oldprim_word,new_data;
  prim_rec_ptr old_prim_rec,new_prim_rec;
  char *plate_name;
  cube_rec_ptr cube_info;
  cylinder_rec_ptr cylinder_info;
  objptr oldprim;
  ;
  pop_off_CPL_stack(CPL_stack,&oldprim_word,TRUE);
  if (oldprim_word.word_type != Featurelist_data)
  {
    system_warning("Incorrect previous primitive in CPL_generate_info_rec!");
    return;
  }
  oldprim = (objptr) oldprim_word.word_data.CPLfeaturelist->first.ep->thiselem;
  if (!has_property((featureptr) oldprim, primitive_prop))
  {
    system_warning("Previous obj not a primitive in CPL_generate_info_rec!");
    return;
  }
  old_prim_rec = (prim_rec_ptr) get_property_val((featureptr) oldprim,
					     primitive_prop).ptr;
  if (old_prim_rec == Nil)
  {
    system_warning("Prim missing prim rec!!!\n");
    new_prim_rec = (prim_rec_ptr) alloc_elem(Primrecsize);
    new_prim_rec->prim_id = Cube_id;
    new_prim_rec->prim_info = (void *) alloc_elem(Cuberecsize);
    ((cube_rec_ptr) new_prim_rec->prim_info)->height =
      oldprim->objbbox.maxcorner[vy] - oldprim->objbbox.mincorner[vy];
    ((cube_rec_ptr) new_prim_rec->prim_info)->width = 
      oldprim->objbbox.maxcorner[vx] - oldprim->objbbox.mincorner[vx];
    ((cube_rec_ptr) new_prim_rec->prim_info)->length =
      oldprim->objbbox.maxcorner[vz] - oldprim->objbbox.mincorner[vz];

    primrecptr_word.word_data.CPLauxdata.CPLaddressdata = new_prim_rec;
    push_on_CPL_stack(CPL_stack,&primrecptr_word);
    return;
  }

  setup_CPL_auxword(&primrecptr_word,CPLaux_address_data);
  switch (old_prim_rec->prim_id)
  {
  case Cube_id:
    new_prim_rec = (prim_rec_ptr) alloc_elem(Primrecsize);
    new_prim_rec->prim_id = Cube_id;
    new_prim_rec->prim_info = (void *) alloc_elem(Cuberecsize);

    /* These values are left on the stack by retrieval from fields just before */
    /* this routine is called.  Retrieval from the fields is accomplished by */
    /* each individual field so that appropriate conversions etc take place */
    /* on a field by field basis... I've used the oldprim_rec to determine */
    /* how many arguments there should be on the stack... this is hairy... */
    pop_off_CPL_stack(CPL_stack,&new_data,TRUE);
    ((cube_rec_ptr) new_prim_rec->prim_info)->height = 
      new_data.word_data.CPLauxdata.CPLfloatdata;
    pop_off_CPL_stack(CPL_stack,&new_data,TRUE);
    ((cube_rec_ptr) new_prim_rec->prim_info)->width = 
      new_data.word_data.CPLauxdata.CPLfloatdata;
    pop_off_CPL_stack(CPL_stack,&new_data,TRUE);
    ((cube_rec_ptr) new_prim_rec->prim_info)->length =
      new_data.word_data.CPLauxdata.CPLfloatdata;

    primrecptr_word.word_data.CPLauxdata.CPLaddressdata = new_prim_rec;
    break;
  case Cylinder_id:
  case Cone_id:
  case Sphere_id:
  case Pyramid_id:
    break;    
  }
  push_on_CPL_stack(CPL_stack,&primrecptr_word);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_breakup_info_rec(listptr world_list, stateptr state)
{
  CPL_word primrecptr_word,new_data;
  prim_rec_ptr old_prim_rec;
  char *plate_name;
  cube_rec_ptr cube_info;
  cylinder_rec_ptr cylinder_info;
  ;
  pop_off_CPL_stack(CPL_stack,&primrecptr_word,TRUE);
  old_prim_rec = (prim_rec_ptr) primrecptr_word.word_data.
    CPLauxdata.CPLaddressdata;
  switch (old_prim_rec->prim_id)
  {
  case Cube_id:
    cube_info = (cube_rec_ptr) old_prim_rec->prim_info;
    setup_CPL_auxword(&new_data,CPLaux_float_data);
    new_data.word_data.CPLauxdata.CPLfloatdata = cube_info->length;
    enter_in_temp_message_block(&new_data);
    new_data.word_data.CPLauxdata.CPLfloatdata = cube_info->width;
    enter_in_temp_message_block(&new_data);
    new_data.word_data.CPLauxdata.CPLfloatdata = cube_info->height;
    enter_in_temp_message_block(&new_data);
    break;
  case Cylinder_id:
    cylinder_info = (cylinder_rec_ptr) old_prim_rec->prim_info;
    setup_CPL_auxword(&new_data,CPLaux_float_data);
    new_data.word_data.CPLauxdata.CPLfloatdata = cylinder_info->radius;
    enter_in_temp_message_block(&new_data);
    new_data.word_data.CPLauxdata.CPLfloatdata = cylinder_info->height;
    enter_in_temp_message_block(&new_data);
    setup_CPL_auxword(&new_data,CPLaux_int_data);
    new_data.word_data.CPLauxdata.CPLintdata = cylinder_info->num_facets;
    enter_in_temp_message_block(&new_data);
    break;
  case Cone_id:
  case Sphere_id:
  case Pyramid_id:
  default:
    break;
  }  

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

  void
CPL_breakup_dist_value(listptr world_list, stateptr state)
{
  CPL_word dist_word,new_data;
  Coord *dist_vec;
  ;
  pop_off_CPL_stack(CPL_stack,&dist_word,TRUE);
  dist_vec = dist_word.word_data.CPLauxdata.CPLoffsetdata;
  setup_CPL_auxword(&new_data,CPLaux_float_data);
  new_data.word_data.CPLauxdata.CPLfloatdata = magvec(dist_vec);
  push_on_CPL_stack(CPL_stack,&new_data);
  new_data.word_data.CPLauxdata.CPLfloatdata = dist_vec[vx];
  push_on_CPL_stack(CPL_stack,&new_data);
  new_data.word_data.CPLauxdata.CPLfloatdata = dist_vec[vy];
  push_on_CPL_stack(CPL_stack,&new_data);
  new_data.word_data.CPLauxdata.CPLfloatdata = dist_vec[vz];
  push_on_CPL_stack(CPL_stack,&new_data);

#ifndef IRIS
  cplib_world_list = world_list;
  cplib_state = state;
#endif
}

