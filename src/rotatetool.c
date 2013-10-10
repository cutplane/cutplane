
/* FILE: rotatetool.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           GLuint Rotation Routines for the Cutplane Editor             */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define ROTATETOOLMODULE

#include <config.h>

#include <stdio.h>
#include <math.h>

#include <platform_gl.h>

#include <cutplane.h>
#include <rotatetool.h>
#include <rotateobj.h>

#include <update.h>
#include <globals.h>		/* for imatrix definition */
#include <math3d.h>

#define Notsnapping
#undef Notsnapping

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                               GLOBAL VARIABLES                           */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

objptr inner_circle,outer_circle,horizontal_xbar,vertical_xbar;
fveptr rotatetool_snapfve;	/* when snapping a face to the plane. */
GLboolean using_other_zaxis = FALSE; /* Indicates using an edge as the */
				   /* dialing axis. */
vertype other_zaxis;		/* Edge vector which is the dialing axis. */
vertype rotatetool_placept;


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                    ROTATE TOOL UTILITY ROUTINES                          */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


  void
give_children_property(objptr thisobj,int thisprop)
{
  objptr thischild;
  ;
  thischild = thisobj->children->first.obj;
  while (thischild != Nil)
  {
    add_property((featureptr) thischild,visible_prop);
    thischild = thischild->next;
  }
}

  void
show_xbar(objptr xbar)
{
  add_property((featureptr) First_obj_evf(xbar),visible_prop);
  add_property((featureptr) First_obj_vfe(xbar),visible_prop);
  add_property((featureptr) First_obj_vfe(xbar)->next,visible_prop);
}

  void
hide_xbar(objptr xbar,GLboolean hide_baubles)
{
  if (has_property((featureptr) First_obj_evf(xbar),visible_prop))
    del_property((featureptr) First_obj_evf(xbar),visible_prop);
  if (hide_baubles == Hidebaubles)
  {
    if (has_property((featureptr) First_obj_vfe(xbar),visible_prop))
      del_property((featureptr) First_obj_vfe(xbar),visible_prop);
    if (has_property((featureptr) First_obj_vfe(xbar)->next,visible_prop))
      del_property((featureptr) First_obj_vfe(xbar)->next,visible_prop);
  }
  else if (hide_baubles == Showbaubles)
  {
    add_property((featureptr) First_obj_vfe(xbar),visible_prop);
    add_property((featureptr) First_obj_vfe(xbar)->next,visible_prop);
  }
}

  void
show_all_xbars()
{
  show_xbar(horizontal_xbar);
  show_xbar(vertical_xbar);
}

  void
reset_rotatetool()
{
  GLfloat *centerpt;
  vertype hxbarpt1,hxbarpt2,vxbarpt1,vxbarpt2;
  vfeptr hxbarend1,hxbarend2,vxbarend1,vxbarend2;
  ;
  centerpt = First_obj_vfe(inner_circle)->pos;
  copypos3d(centerpt,rotatetool_placept);
  /* Now change the coords of the horizontal xbar to be normal to this vector */
  /* and the vertical xbar to be aligned w/ this vector. */
  hxbarend1 = First_obj_vfe(horizontal_xbar);
  hxbarend2 = First_obj_vfe(horizontal_xbar)->next;
  vxbarend1 = First_obj_vfe(vertical_xbar);
  vxbarend2 = First_obj_vfe(vertical_xbar)->next;
  /* set the horizontal xbar to be parallel to the x axis */
  setpos3d(hxbarpt1, centerpt[vx] + rotatetool_radius,centerpt[vy],centerpt[vz]);
  setvfe(hxbarend1,hxbarpt1,inner_circle->invxform);
  setpos3d(hxbarpt2, centerpt[vx] - rotatetool_radius,
	  centerpt[vy],centerpt[vz]);
  setvfe(hxbarend2,hxbarpt2,inner_circle->invxform);
  /* set the vertical xbar to be parallel to the y axis */
  setpos3d(vxbarpt1,centerpt[vx], centerpt[vy] + rotatetool_radius,
	  centerpt[vz]);
  setvfe(vxbarend1,vxbarpt1,inner_circle->invxform);
  setpos3d(vxbarpt2, centerpt[vx],centerpt[vy] - rotatetool_radius,
	  centerpt[vz]);
  setvfe(vxbarend2,vxbarpt2,inner_circle->invxform);

  show_all_xbars();
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                    ROTATE TOOL CREATION ROUTINES                         */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
create_rotatetool(worldptr world)
{
  vertype pos;
  leptr firstle,thisle;
  vertype rotvec,rotpt;
  GLfloat rotatetool_radius_with_bauble;
  ;
  copypos3d(origin,rotatetool_dock);
  rotatetool_obj = inner_circle = make_obj(world,rotatetool_dock);
  set_object_name(rotatetool_obj,"Rotatetool_Innercircle");

  adopt_obj_branch(cutplane_obj,rotatetool_obj,TRUE);

  add_property((featureptr) inner_circle,nonmanifold_prop);
  add_property((featureptr) inner_circle,noshadow_prop);
  inner_circle->selectechnique = set_rotate_innercircle_selectable;
  inner_circle->drawtechnique = draw_rotate_innercircle;
  inner_circle->selectable_feature.
    selectboxtechnique = draw_innercircle_select_box;
  inner_circle->screenbboxtechnique = find_rotatetool_screenbbox;

  setpos3d(pos,rotatetool_dock[vx] - rotatetool_radius,
	  rotatetool_dock[vy],rotatetool_dock[vz]);
  horizontal_xbar = make_obj(world,pos);
  set_object_name(horizontal_xbar,"Horizontal_Crossbar");
  setpos3d(pos,rotatetool_dock[vx] + rotatetool_radius,
	  rotatetool_dock[vy],rotatetool_dock[vz]);
  firstle = First_obj_le(horizontal_xbar);
  make_edge_vert(firstle,firstle,pos);
  add_property((featureptr) horizontal_xbar,nonmanifold_prop);
  add_property((featureptr) horizontal_xbar,noshadow_prop);
  add_property((featureptr) First_obj_evf(horizontal_xbar),visible_prop);
  horizontal_xbar->selectechnique = set_rotatexbar_selectable;
  horizontal_xbar->drawtechnique = draw_rotatexbar;
  horizontal_xbar->selectable_feature.
    selectboxtechnique = draw_big_circular_selectbox;
  horizontal_xbar->screenbboxtechnique = find_rotatetool_screenbbox;
  adopt_obj_branch(inner_circle,horizontal_xbar,FALSE);


  setpos3d(pos,rotatetool_dock[vx],rotatetool_dock[vy] - rotatetool_radius,
	  rotatetool_dock[vz]);
  vertical_xbar = make_obj(world,pos);
  set_object_name(vertical_xbar,"Vertical_Crossbar");
  setpos3d(pos,rotatetool_dock[vx],rotatetool_dock[vy] + rotatetool_radius,
	  rotatetool_dock[vz]);
  firstle = First_obj_le(vertical_xbar);
  make_edge_vert(firstle,firstle,pos);
  add_property((featureptr) vertical_xbar,nonmanifold_prop);
  add_property((featureptr) vertical_xbar,noshadow_prop);
  add_property((featureptr) First_obj_evf(vertical_xbar),visible_prop);
  vertical_xbar->selectechnique = set_rotatexbar_selectable;
  vertical_xbar->drawtechnique = draw_rotatexbar;
  vertical_xbar->selectable_feature.
    selectboxtechnique = draw_big_circular_selectbox;
  vertical_xbar->screenbboxtechnique = find_rotatetool_screenbbox;
  adopt_obj_branch(inner_circle,vertical_xbar,FALSE);

  outer_circle = make_obj(world,rotatetool_dock);
  set_object_name(outer_circle,"Rotatetool_Outercircle");
  add_property((featureptr) outer_circle,nonmanifold_prop);
  add_property((featureptr) outer_circle,noshadow_prop);
  outer_circle->selectechnique = set_rotate_outercircle_selectable;
  outer_circle->drawtechnique = draw_rotate_outercircle;
  outer_circle->selectable_feature.
    selectboxtechnique = draw_normal_objects_select_box;
  outer_circle->screenbboxtechnique = find_rotatetool_screenbbox;
  adopt_obj_branch(inner_circle,outer_circle,FALSE);

  reset_rotatetool();
  setpos3d(rotatetool_dock, Rotatetool_dockx, Rotatetool_docky, 0.0);
  translate_feature((featureptr) inner_circle,rotatetool_dock,FALSE);
  add_property_to_descendants((featureptr) rotatetool_obj,alwaysdraw_prop);
  add_property_to_descendants((featureptr) rotatetool_obj,selectinvisible_prop);
  add_property_to_descendants((featureptr) rotatetool_obj,pickedinvisible_prop);
}  

  bboxscreenptr
find_rotatetool_screenbbox(void)
{
  static bboxscreentype rotatetool_screenbbox;
  GLfloat *center,radius_with_bauble;
  bbox3dtype rotatetoolbbox;
  ;
  radius_with_bauble = rotatetool_radius + Baublesize*2;
  center = (First_obj_vfe(inner_circle))->pos;
  setpos3d(rotatetoolbbox.mincorner,center[vx] - radius_with_bauble,
	   center[vy] - radius_with_bauble, center[vz] - Cursormargin);
  setpos3d(rotatetoolbbox.maxcorner,center[vx] + radius_with_bauble,
	   center[vy] + radius_with_bauble, center[vz] + Cursormargin);
  compute_screen_bbox(&rotatetoolbbox,&rotatetool_screenbbox);
  return (&rotatetool_screenbbox);
}

  void
attach_rotatetool_CPLcode(void)
{
  /* here, associate the proper CPL routines with the respective */
  /* parts of this shell. */
  read_CPL_privatecode("./cpl/rotatecircle.cpl",
		       inner_circle);
  read_CPL_privatecode("./cpl/rotatecircle.cpl",
		       outer_circle);

  read_CPL_privatecode("./cpl/rotatebauble.cpl",
		       First_obj_vfe(horizontal_xbar));
  read_CPL_privatecode("./cpl/rotatebauble.cpl",
		       First_obj_vfe(horizontal_xbar)->next);
  read_CPL_privatecode("./cpl/rotatexbar.cpl",
		       First_obj_evf(horizontal_xbar));
  /* here, associate the proper CPL routines with the respective */
  /* parts of this shell. Many parts of a rotate tool are edible. */
  read_CPL_privatecode("./cpl/rotatebauble.cpl",
		       First_obj_vfe(vertical_xbar));
  read_CPL_privatecode("./cpl/rotatebauble.cpl",
		       First_obj_vfe(vertical_xbar)->next);
  read_CPL_privatecode("./cpl/rotatexbar.cpl",
		       First_obj_evf(vertical_xbar));
  /* also, attach some rotatetooldrag.cpl to the main guy so it swallows */
  /* picks and stuff. */
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                 ROTATE TOOL SELECTABILITY ROUTINES                       */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
point_vec_up(vertype pt1,vertype pt2,vertype inplane_axis)
{
  diffpos3d(pt1,pt2,inplane_axis);
  normalize(inplane_axis,inplane_axis);
  if (dotvecs(inplane_axis,x_axis) < 0.0)
  {
    if (compare(dotvecs(inplane_axis,y_axis),0.0,Ptonlinetol) < 0)
      flip_vec(inplane_axis);
  }
  else if (dotvecs(inplane_axis,y_axis) < 0.0)
    flip_vec(inplane_axis);
}

  void
create_xbar_rotrec(evfptr motherxbar,CPLauxptr rotaterec, CPLauxptr transrec,
		   CPLauxptr snappedrec,CPL_word_ptr paused_rec,
		   CPL_word_ptr inplane_record,CPL_word_ptr outofplane_record)
{
  GLfloat *pt1,*pt2,*transoffset,actualtrans_scale;
  vertype xbarvec,totalxbarvec,newpos,checkvec1,checkvec2;
  int angleinput,snapangle;
  ;
  transoffset = transrec->CPLoffsetdata;
  pt1 = motherxbar->le1->facevert->pos;
  pt2 = motherxbar->le2->facevert->pos;
  midpoint(pt1,pt2,rotaterec->rotate_data.rotpt);
  
  point_vec_up(pt1,pt2,xbarvec);
  cross_prod(z_axis,xbarvec,rotaterec->rotate_data.rotvec);
  actualtrans_scale = dotvecs(xbarvec,transoffset);
  scalevec3d(xbarvec,actualtrans_scale,transoffset);
  
  /* limit the movement to stop on baubles here. */
  diffpos3d(pt1,pt2,totalxbarvec);
  addpos3d(state->cursor,transoffset,newpos);
  diffvec3d(newpos,pt1,checkvec1);
  diffvec3d(newpos,pt2,checkvec2);
  /* if dotprod of vectors from pt that the cursor will land on */
  /* to both endpoints is the same sign, that pt is an exterior pt. */
  /* So, snap to a bauble. */
  if (fsign(dotvecs(checkvec1,totalxbarvec)) ==
      fsign(dotvecs(checkvec2,totalxbarvec)))
    /* whichever distance is longer is the wrong endpoint to snap to. */
  {
    if ((Fabs(dotvecs(transoffset,checkvec1)) <
	 Fabs(dotvecs(transoffset,checkvec2))))
    {
      diffpos3d(state->cursor,pt1,transoffset);
/*      outofplane_record->word_data.CPLfeaturelist->first.ep->thiselem =
	(elemptr) motherxbar->le1->facevert;*/
    }
    else
    {
      diffpos3d(state->cursor,pt2,transoffset);
/*      outofplane_record->word_data.CPLfeaturelist->first.ep->thiselem =
	(elemptr) motherxbar->le2->facevert;*/
    }
  }
  
  /* now use the limited transoffset to compute the rotation angle */
  angleinput = (int) (actualtrans_scale * 10.0);

  /* if rotating about a face check to see if we should snap, but only */
  /* if user's inputted angle less than the constant below. (so much for */
  /* style) */
  if ((angleinput < 75) && (rotatetool_snapfve != Nil))
  {
    /* calculate delta angle needed to snap face into plane */
    snapangle = snap_face_to_plane(rotaterec->rotate_data.rotvec,
				   rotatetool_snapfve->facenorm,0);
    /* if we are going in the same direction as snap angle see if its less */
    /* than the angle input or less than 5deg...if so we use snap angle */
    if ((snapangle*angleinput > 0) && ((abs(snapangle) < abs(angleinput)) ||
				       (abs(snapangle) < 50)))
    {
      snappedrec->CPLbooleandata = TRUE;
      angleinput = snapangle;
    }
    else
    {
      /* calculate delta angle needed to snap face perpendicular to plane */
      snapangle = snap_face_to_plane_perp(rotaterec->rotate_data.rotvec,
					  rotatetool_snapfve->facenorm);
      if ((snapangle*angleinput > 0) && ((abs(snapangle) < abs(angleinput)) ||
					 (abs(snapangle) < 50)))
      {
	paused_rec->word_data.CPLauxdata.CPLbooleandata = TRUE;
	angleinput = snapangle;
      }
    }
  }
  rotaterec->rotate_data.rotangle = angleinput;
}



  void
CPL_xbar_rotate(listptr world_list,stateptr state)
{
  evfptr motherxbar;
  CPL_word motherbar_record,translate_record,newrotate_record;
  CPL_word snappedface_record,inplane_record,outofplane_record;
  CPL_word paused_rec;
  CPLauxptr rotaterec,transrec,snappedrec;
  ;
  pop_off_CPL_stack(CPL_stack,&translate_record,TRUE);
  pop_off_CPL_stack(CPL_stack,&motherbar_record,TRUE);
  /* at this point, just bogus */
  pop_off_CPL_stack(CPL_stack,&snappedface_record,TRUE); 
  /*  pop_off_CPL_stack(&outofplane_record,TRUE); */

  setup_CPL_auxword(&newrotate_record,CPLaux_rotate_data);
  setup_CPL_auxword(&snappedface_record,CPLaux_boolean_data);
  setup_CPL_auxword(&paused_rec,CPLaux_boolean_data);
  paused_rec.word_data.CPLauxdata.CPLbooleandata = FALSE;
/*
  setup_CPL_word_blank(&outofplane_record,Featurelist_data);
  add_to_featurelist(outofplane_record.word_data.CPLfeaturelist,
		     Nil);
*/

  if (motherbar_record.word_type == Featurelist_data)
  {
    motherxbar = (evfptr) motherbar_record.word_data.CPLfeaturelist->
      first.ep->thiselem;
    /* construct rotpt and rotvec. */
    rotaterec = &(newrotate_record.word_data.CPLauxdata);
    transrec = &(translate_record.word_data.CPLauxdata);
    snappedrec = &(snappedface_record.word_data.CPLauxdata);
    snappedrec->CPLbooleandata = FALSE;
    create_xbar_rotrec(motherxbar,rotaterec,transrec,snappedrec,&paused_rec,
		       &inplane_record,&outofplane_record);
  }
/*  push_on_CPL_stack(CPL_stack,&outofplane_record);*/
  push_on_CPL_stack(CPL_stack,&snappedface_record);
  push_on_CPL_stack(CPL_stack,&paused_rec);
  push_on_CPL_stack(CPL_stack,&newrotate_record);
  push_on_CPL_stack(CPL_stack,&translate_record);
  
/*  clear_word(&outofplane_record);*/
/*  free_CPL_auxdata(newrotate_record.word_data.CPLauxdata);
  free_CPL_auxdata(translate_record.word_data.CPLauxdata);
  free_CPL_auxdata(snappedface_record.word_data.CPLauxdata);*/
}

  int
compute_snapped_rotangle(vertype normvec1,vertype normvec2,
			 vertype snapvec,int snapangle,
			 int maxsnaprange, vertype rotzaxis,
			 GLboolean *snapped_bauble)
{
  int angle1,angle2,origangle,snapdiv1,snapdiv2;
  int angle1sign,angle2sign;
  double returnangle;
  vertype acting_x_axis;
  ;
  origangle = (int) (Rad2deg * Acos(dotvecs(normvec1,normvec2)));
  copyvec3d(normvec2,snapvec);

#ifdef Notsnapping
  return(origangle);
#else
  if (origangle > maxsnaprange)
    return(origangle);		/* don't snap if greater than given amt */

  angle1 = iround((Rad2deg * atan2(normvec1[vy],normvec1[vx])));
  angle2 = iround((Rad2deg * atan2(normvec2[vy],normvec2[vx])));
  angle1sign = sign(angle1);
  angle2sign = sign(angle2);
  angle1 += (360 * (angle1 < 0)); /* make them range from 0-360 */
  angle2 += (360 * (angle2 < 0));

  if (angle1sign != angle2sign) /* crossed over atan2 boundary (neg xaxis) */
  { /* this ALWAYS snaps to X axis regardless of possible in-between snaps */
    /* when this stuff happens, always use positive Z axis as axis of rot */
    /* since i assume that's the case regardless of the way */
    /* normvec1 X normvec2 points. */
    setpos3d(rotzaxis,0.0,0.0,1.0);
    *snapped_bauble = TRUE;
    copyvec3d(rotx_axis,acting_x_axis);
    if ((angle1 > 90) && (angle1 < 270)) /* flip over negative x axis */
    {
      flip_vec(acting_x_axis);
      if (angle1 > angle2)	/* vector 1 Below negative xaxis */
	returnangle = - (Rad2deg * Acos(dotvecs(acting_x_axis,normvec1)));
      else			/* vector 1 above negative xaxis */
	returnangle = (Rad2deg * Acos(dotvecs(normvec1,acting_x_axis)));
    }
    else
    {
      if (angle1 > angle2)	/* vector 1 BELOW xaxis */
	returnangle = (Rad2deg * Acos(dotvecs(normvec1,acting_x_axis)));
      else			/* vector 2 BELOW xaxis */
	returnangle = - (Rad2deg * Acos(dotvecs(acting_x_axis,normvec1)));
    }
    return(iround(returnangle));
  }

  snapdiv1 = iround(((double) angle1 / (double) snapangle) * (double) snapangle);
  snapdiv2 = iround(((double) angle2 / (double) snapangle) * (double) snapangle);
/*  printf ("angle1 = %d,angle2 = %d,snapdiv1 = %d,snapdiv2 = %d\n",
	  angle1,angle2,snapdiv1,snapdiv2);*/
  if (snapdiv1 == angle1)
  {				/* don't run rest of code or you'll stick */
    *snapped_bauble = FALSE;
    copyvec3d(normvec2,snapvec);
    return (origangle);
  }
  if ((angle2 > angle1) && (snapdiv2 > angle1))
  {
    setpos3d(snapvec, cos(Deg2rad * (double) snapdiv2),
	     sin(Deg2rad * (double) snapdiv2),0.0);
    *snapped_bauble = TRUE;
    return(snapdiv2 - angle1);
  }
  else if ((angle1 > angle2) && (snapdiv1 > angle2))
  {
    setpos3d(snapvec, cos(Deg2rad * (double) snapdiv1),
	     sin(Deg2rad * (double) snapdiv1),0.0);
    *snapped_bauble = TRUE;
    return(angle1 - snapdiv1);
  }
  else
  {
    *snapped_bauble = FALSE;
    copyvec3d(normvec2,snapvec);
    return (origangle);
  }
#endif
}


  void
CPL_bauble_rotate(listptr world_list,stateptr state)
{
  CPL_word motherbauble_record,translate_record;
  CPL_word inplane_record,outofplane_record,snappedface_record,newrotate_record;
  CPL_word paused_rec;
  vfeptr thebauble;
  evfptr motherxbar;
  GLfloat *pt1,*pt2,*baublept,*transoffset,actualtrans_scale;
  vertype xbarvec,otherxbarvec,newpos,truenewpos,vec1,vec2,normvec1,normvec2;
  vertype outvec,normoutvec,snapvec,normsnapvec,rotzaxis,testnormvec2;
  CPLauxptr rotaterec,transrec,outofplanerec,snappedrec;
  GLfloat escaped;
  int rotangle;
  ;
  /* this method of CPL-C communications really sux!!! */
  pop_off_CPL_stack(CPL_stack,&translate_record,TRUE);
  pop_off_CPL_stack(CPL_stack,&motherbauble_record,TRUE);
  /* at this point, just bogus */
  pop_off_CPL_stack(CPL_stack,&snappedface_record,TRUE);
  pop_off_CPL_stack(CPL_stack,&inplane_record,TRUE); 
  
  setup_CPL_auxword(&newrotate_record,CPLaux_rotate_data);
  setup_CPL_word(&inplane_record,Featurelist_data);
  add_to_featurelist(inplane_record.word_data.CPLfeaturelist,Nil);
  setup_CPL_auxword(&snappedface_record,CPLaux_boolean_data);
  setup_CPL_auxword(&paused_rec,CPLaux_boolean_data);
  paused_rec.word_data.CPLauxdata.CPLbooleandata = FALSE;
  if (motherbauble_record.word_type == Featurelist_data)
  {
    thebauble = (vfeptr) motherbauble_record.word_data.CPLfeaturelist->
      first.ep->thiselem;
    motherxbar = (evfptr) find_largergrain((featureptr) thebauble, Evf_type);
    /* construct rotpt and rotvec. */
    rotaterec = &(newrotate_record.word_data.CPLauxdata);
    transrec = &(translate_record.word_data.CPLauxdata);
    snappedrec = &(snappedface_record.word_data.CPLauxdata);
    transoffset = transrec->CPLoffsetdata;
    baublept = thebauble->pos;
    pt1 = motherxbar->le1->facevert->pos;
    pt2 = motherxbar->le2->facevert->pos;
    point_vec_up(pt1,pt2,xbarvec);
    midpoint(pt1,pt2,rotaterec->rotate_data.rotpt);

    /* decide whether to allow going back along the xbar or not */
    cross_prod(xbarvec,z_axis,otherxbarvec);
    diffpos3d(rotaterec->rotate_data.rotpt,baublept,outvec);
    normalize(outvec,normoutvec);
    escaped = dotvecs(transoffset,normoutvec);

    /* go back to xbar rotation if user moves mouse more towards innercircle */
    /* than along the tangent and moves enough to escape the magnetism of the */
    /* bauble AND if the mother xbar is visible... if not, don't let him. */
    if ((Fabs(dotvecs(transoffset,otherxbarvec)) <
	 Fabs(dotvecs(transoffset,xbarvec))) &&
	(escaped < -Baublesize/2.0) &&
	(has_property((featureptr) motherxbar,visible_prop)))
    {				/* go back to xbar rotation */
      snappedrec->CPLbooleandata = FALSE;
      /* use regular xbar */
      create_xbar_rotrec(motherxbar,rotaterec,transrec, snappedrec,
			 &paused_rec,&inplane_record,&outofplane_record);
      inplane_record.word_data.CPLfeaturelist->first.ep->thiselem =
	(elemptr) motherxbar;	/* let bauble code know which xbar to activate */
    }
    else			/* do dial rotation */
    {
      diffpos3d(rotaterec->rotate_data.rotpt,baublept,vec1);
      addpos3d(state->cursor,transoffset,newpos);
      diffpos3d(rotaterec->rotate_data.rotpt,newpos,vec2);
      normalize(vec1,normvec1);
      normalize(vec2,normvec2);
      cross_prod(normvec1,normvec2,rotzaxis);
      rotangle = compute_snapped_rotangle(normvec1,normvec2,normsnapvec,45,10,
					  rotzaxis,
					  &(paused_rec.word_data.
					    CPLauxdata.CPLbooleandata));
      rotaterec->rotate_data.rotangle = rotangle * 10;

      
      if (using_other_zaxis)	/* using evf as z_axis */
      {
	if (dotvecs(rotzaxis,other_zaxis) < 0.0)
	  flip_vec(other_zaxis);
	copyvec3d(other_zaxis,rotaterec->rotate_data.rotvec);
      }
      else			/* using z/-z axis as z_axis */
	copyvec3d(rotzaxis,rotaterec->rotate_data.rotvec);
      
      normalize(rotaterec->rotate_data.rotvec,rotaterec->rotate_data.rotvec);
      /* compute new position to make cursor follow rotate bauble, with */
      /* snapping angle found above. slow but otherwise ok in this context*/
      scalevec3d(normsnapvec,rotatetool_radius,truenewpos);
      addpos3d(truenewpos,rotaterec->rotate_data.rotpt,truenewpos);
      diffpos3d(state->cursor,truenewpos,transoffset);
    }
  }
  push_on_CPL_stack(CPL_stack,&inplane_record);
  push_on_CPL_stack(CPL_stack,&snappedface_record);   /* at this point, bogus */

  push_on_CPL_stack(CPL_stack,&paused_rec);
  push_on_CPL_stack(CPL_stack,&newrotate_record);
  /* also push a rotaterecord on the stack for the rotate tool, so it doesn't */
  /* accidentally use an outofplane axis */
  copyvec3d(rotzaxis,rotaterec->rotate_data.rotvec);
  push_on_CPL_stack(CPL_stack,&newrotate_record);
  
  push_on_CPL_stack(CPL_stack,&translate_record);
  
/*  clear_word(&inplane_record);*/
/*  free_CPL_auxdata(newrotate_record.word_data.CPLauxdata);
  free_CPL_auxdata(translate_record.word_data.CPLauxdata);
  free_CPL_auxdata(snappedface_record.word_data.CPLauxdata);*/
}

  
  void
orient_rotatetool(objptr selectobj,vertype pt1, vertype pt2)
{
  vertype sectionvec;
  GLfloat *centerpt;
  vertype hxbarpt1,hxbarpt2,vxbarpt1,vxbarpt2;
  vfeptr hxbarend1,hxbarend2,vxbarend1,vxbarend2;
  ;
  point_vec_up(pt1,pt2,sectionvec);
  centerpt = First_obj_vfe(inner_circle)->pos;
  copypos3d(selectobj->selectable_feature.nearestpos,rotatetool_placept);
  /* Now change the coords of the horizontal xbar to be normal to this vector */
  /* and the vertical xbar to be aligned w/ this vector. */
  hxbarend1 = First_obj_vfe(horizontal_xbar);
  hxbarend2 = First_obj_vfe(horizontal_xbar)->next;
  vxbarend1 = First_obj_vfe(vertical_xbar);
  vxbarend2 = First_obj_vfe(vertical_xbar)->next;
  /* set the vertical xbar to be parallel to the section line/inplane evf */
  setpos3d(vxbarpt1, sectionvec[vx] * rotatetool_radius + centerpt[vx],
	  sectionvec[vy] * rotatetool_radius + centerpt[vy],
	  centerpt[vz]);
  setvfe(vxbarend1,vxbarpt1,inner_circle->invxform);
  setpos3d(vxbarpt2, (-sectionvec[vx] * rotatetool_radius) + centerpt[vx],
	  (-sectionvec[vy] * rotatetool_radius) + centerpt[vy],
	  centerpt[vz]);
  setvfe(vxbarend2,vxbarpt2,inner_circle->invxform);
  /* set the horizontal xbar to be normal to the section line/inplane evf */
  setpos3d(hxbarpt1, (sectionvec[vy] * rotatetool_radius) + centerpt[vx],
	  (-sectionvec[vx] * rotatetool_radius) + centerpt[vy],
	  centerpt[vz]);
  setvfe(hxbarend1,hxbarpt1,inner_circle->invxform);
  setpos3d(hxbarpt2, (-sectionvec[vy] * rotatetool_radius) + centerpt[vx],
	  (sectionvec[vx] * rotatetool_radius) + centerpt[vy],
	  centerpt[vz]);
  setvfe(hxbarend2,hxbarpt2,inner_circle->invxform);

  /* Make sure to show the horizontal xbar */
  show_xbar(horizontal_xbar);
  /* Hide the vertical xbar */
  hide_xbar(vertical_xbar,Hidebaubles);
}

  void
CPL_snap_rotatetool(listptr world_list,stateptr state)
{
  GLfloat *centerpt;
  vertype movevec;
  ;
  centerpt = First_obj_vfe(inner_circle)->pos;
  diffpos3d(centerpt,rotatetool_placept,movevec);
  /* snap the rotate tool to the nearest pos. */
  translate_feature((featureptr) inner_circle,movevec,FALSE);
}

  void
setup_other_zaxis(objptr motherselectable,evfptr other_zaxis_evf)
{
  /* inform system of this new rotation z_axis */
  using_other_zaxis = TRUE;
  diffpos3d(other_zaxis_evf->le1->facevert->pos,
	 other_zaxis_evf->le2->facevert->pos,other_zaxis);
  normalize(other_zaxis,other_zaxis);
  /* Hide both xbars...  */
  hide_xbar(horizontal_xbar,Hidebaubles);
  hide_xbar(vertical_xbar,Showbaubles);
  copypos3d(motherselectable->selectable_feature.nearestpos,rotatetool_placept);
}

  void
CPL_setup_rotatetool(listptr world_list,stateptr state)
{
  CPL_word selectables_record;
  featureptr chosen_selectable;
  objptr motherselectable;
  segmentptr motherseg;
  ;
  pop_off_CPL_stack(CPL_stack,&selectables_record,TRUE);
  if (selectables_record.word_type == Featurelist_data)
  {
    /* use only the first selectable found. */
    chosen_selectable = selectables_record.word_data.CPLfeaturelist->
      first.ep->thiselem;
    /* Assume no face will be used for snapping, and no evf used for zaxis */
    rotatetool_snapfve = Nil;
    using_other_zaxis = FALSE;
    
    if (chosen_selectable == Nil)
      /* nothing is selectable, so show both xbars */
      show_all_xbars();
    else
    {
      motherselectable = (objptr) find_largergrain(chosen_selectable,Obj_type);
      switch (chosen_selectable->type_id)
      {
      case Vfe_type:	/* in these cases the rotate tool can be reset */
      case Obj_type:
	reset_rotatetool();
	log_update((featureptr) rotatetool_obj,Logerasebboxflag,Immediateupdate);
	break;
      case Evf_type:
	if (has_property((featureptr) motherselectable,scalebbox_prop))
	  break;		/* HACK BIG BIG BIG HACK!!!GAG GAG GAG */
	motherseg = motherselectable->selectable_feature.motherseg;
	if (motherseg->kind == Evf_inplane) /* Rotating about inplane evfs */
	  orient_rotatetool(motherselectable,motherseg->pt[0],motherseg->pt[1]);
	else	/* Rotating about intersectevfs */
	  setup_other_zaxis(motherselectable,(evfptr) chosen_selectable);
	log_update((featureptr) rotatetool_obj,Logerasebboxflag,Immediateupdate);
	break;
      case Fve_type:
	motherseg = motherselectable->selectable_feature.motherseg;
	if (motherseg->kind == Fve_intersect)
	{			/* Rotating about intersecting fves */
	  rotatetool_snapfve = (fveptr) chosen_selectable;
	  orient_rotatetool(motherselectable,motherseg->pt[0],motherseg->pt[1]);
	}
	else			/* Rotating on inplane fves */
	  reset_rotatetool();
	log_update((featureptr) rotatetool_obj,Logerasebboxflag,Immediateupdate);
	break;
      }
    }
  }	
}

