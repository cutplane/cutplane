
/* FILE: tools.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*                  Tool GLuint Generation Routines                       */
/*                                                                        */
/* Author: WAK                                   Date: September 20, 1989 */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define TOOLSMODULE

#include <config.h>

#include <stdio.h>

#include <platform_gl.h>
#include <math.h>		/* for sin, cos calls */
#ifdef IRIS
#include <values.h>		/* ??? for MAXLONG */
#ifndef INT_MAX
#define INT_MAX MAXLONG
#endif
#endif
#include <limits.h>
#include <sys/types.h>		// For int32_t
#include <string.h>		/* for strcmp */

#include <cutplane.h>		/* cutplane.h includes tools.h */
#include <tools.h>
#include <math3d.h>
#include <globals.h>
#include <init.h>
#include <deform.h>		/* for Ncontrolpts definition */
#include <update.h>
#include <grafx.h>
#include <text.h>
#include <cursor.h>		/* for arrowpad info... such a hack */


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                               GLOBAL VARIABLES                           */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

objptr stockbars[Portfoliosize][Portfoliohistory];
fveptr stocktops[Portfoliosize][Portfoliohistory];
int current_numstocks = Portfoliosize,current_stockhistory = Portfoliohistory;
int current_maxbarheight;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                               UTILITY ROUTINES                           */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
apply_front_decal(objptr decal_obj,objptr dest_obj,GLfloat decal_thickness,
		  GLfloat decal_forward_offset,GLfloat decal_margin)
{
  bbox3dptr decal_bbox;
  bbox3dtype dest_bbox;
  vertype decal_size,dest_size,to_origin,to_dest_center,scale_amt,decal_offset;
  vertype destmincorner;
  double xratio,yratio,zratio;
  GLuint newdecalobj;
  ;
  transform_obj_tree(decal_obj);
  find_branch_normals(decal_obj);
  transform_obj_tree(dest_obj);
  if (!has_property((featureptr) dest_obj,nonmanifold_prop))
    find_branch_normals(dest_obj);

  set_3d_branchbboxes(decal_obj);
  grow_group_bbox(decal_obj);
  set_3d_branchbboxes(dest_obj);
  grow_group_bbox(dest_obj);

  decal_bbox = &(decal_obj->groupbbox); 
  diffpos3d(decal_bbox->mincorner,decal_bbox->maxcorner,decal_size);
  dest_bbox = dest_obj->objbbox;
  /* make the dest bbox smaller because we want some margin... */
  dest_bbox.mincorner[vx] += decal_margin;
  dest_bbox.mincorner[vy] += decal_margin;
  dest_bbox.maxcorner[vx] -= decal_margin;
  dest_bbox.maxcorner[vy] -= decal_margin;
  setpos3d(destmincorner,dest_bbox.mincorner[vx],
	   dest_bbox.mincorner[vy],dest_bbox.maxcorner[vz]);
  diffpos3d(destmincorner,dest_bbox.maxcorner,dest_size);

  midpoint(decal_bbox->mincorner,decal_bbox->maxcorner, to_origin);
  midpoint(destmincorner,dest_bbox.maxcorner, to_dest_center);
  flip_vec(to_origin);

  setvec3d(scale_amt,
	   (dest_size[vx] / (decal_size[vx] > 0 ? decal_size[vx] : 1.0)),
	   (dest_size[vy] / (decal_size[vy] > 0 ? decal_size[vy] : 1.0)),
	   (decal_thickness / (decal_size[vz] > 0 ? decal_size[vz] : 1.0)));
  /* use the minimum of x or y scaling in both directions in order to */
  /* avoid changing proportions in x and y. */
  if (scale_amt[vx] < scale_amt[vy])
    scale_amt[vy] = scale_amt[vx];
  else
    scale_amt[vx] = scale_amt[vy];

  /* move it forward and to the center of the dest bbox... the divisions */
  /* overcome the fact that the scaling is in effect.*/

  /* hacked here because p-iris.c is not in use on windows  */
  transformpt(to_dest_center,dest_obj->invxform, decal_offset);

  decal_offset[vz] += decal_forward_offset;
  newdecalobj = create_decal_globj(decal_obj,to_origin,scale_amt,decal_offset);

  set_property(dest_obj,hasdecal_prop,(long) newdecalobj);
}

/* Could also be in storage but all the constants are here... */

  void
read_decal_obj(objptr apply_obj,char *decalobj_filename)
{
  objptr decal_obj;
  GLfloat motherwidth;
  ;
  if (read_data_file(decalobj_filename,(featureptr) apply_obj))
  {				/* apply only if found */
    decal_obj = apply_obj->children->last.obj;
    transform_obj_tree(apply_obj);
    set_3d_branchbboxes(apply_obj);
    motherwidth = apply_obj->groupbbox.maxcorner[vx] -
      apply_obj->groupbbox.mincorner[vx];
    apply_front_decal(decal_obj,apply_obj, Decalthickness*2,Decalzoffset,
		      motherwidth/10.0);
    kill_obj_branch(decal_obj);
  }
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                          TOOL OBJECT CREATION                            */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  objptr
create_hammer(worldptr world,colortype hammercolor,colortype handlecolor,
	      GLfloat hammerheadwidth,GLfloat hammerheadheight,GLfloat hammerheadepth,
	      GLfloat hammerhandleheight)
{
  objptr hammer;
  objptr hammerhead,hammerbody,hammerblade1,hammerblade2;
  objptr hammerhandletop,hammerhandle;
  vertype pos;
  leptr firstle,thisle;
  fveptr handletopface;
  int hammeresolution = 10;
  ;
  hammer = hammerbody = create_cube(world,hammerheadwidth,hammerheadheight,
				    hammerheadepth);
  set_feature_color((featureptr) hammerbody,hammercolor);
  setpos3d(pos,-hammerheadwidth/3.0,-hammerheadheight/2.0,0.0);
  translate_feature((featureptr) hammerbody,pos,FALSE);
  
  setpos3d(pos,0.0,-hammerheadheight/2.0,0.0);
  hammerblade1 = make_obj(world,pos);
  set_feature_color((featureptr) hammerblade1,hammercolor);
  firstle = thisle = First_obj_le(hammerblade1);
  setpos3d(pos,-hammerheadwidth / 6.0, -hammerheadheight/1.5,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,-hammerheadwidth, -hammerheadheight * 1.2,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,-hammerheadwidth / 6.0, hammerheadheight / 2.5, 0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,0.0, hammerheadheight / 2.0, 0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  make_edge_face(thisle,firstle);
  
  setpos3d(pos, 0.0,0.0,hammerheadepth / 2.0);
  extrude_face(First_obj_fve(hammerblade1),pos);
  hammerblade2 = duplicate_obj(hammerblade1);
  
  axis_rot_obj(hammerblade1,roty_axis,origin,100);
  
  setpos3d(pos, -hammerheadwidth/3.0,0.0,hammerheadepth/2.0);
  translate_feature((featureptr) hammerblade1,pos,FALSE);
  adopt_obj_branch(hammer,hammerblade1,FALSE);
  
  setpos3d(pos,0.0,0.0,hammerheadepth/2.0);
  axis_rot_obj(hammerblade2,roty_axis,pos,-100);
  
  setpos3d(pos, -hammerheadwidth/3.0,0.0,0.0);
  translate_feature((featureptr) hammerblade2,pos,FALSE);
  adopt_obj_branch(hammer,hammerblade2,FALSE);
  
  hammerhead = create_oval_cylinder(world,hammerheadepth,hammerheadepth,
				    hammerheadwidth/3.0,hammeresolution);
  axis_rot_obj(hammerhead,roty_axis,origin,900);
  setpos3d(pos,2.0*hammerheadwidth/3.0,0.0,hammerheadepth/2.0);
  translate_feature((featureptr) hammerhead,pos,FALSE);
  adopt_obj_branch(hammer,hammerhead,FALSE);
  
#if 0
  /* can't get this to work cause strange things seem to happen to translates */
  /* after scaling has occured. (think about this) */
  hammerhandletop = create_oval_cylinder(world,hammerheadepth/2.0,
					 hammerheadepth/2.0,
					 hammerheadepth,hammeresolution/2);
  handletopface = find_face_given_normal(hammerhandletop,z_axis);
  setpos3d(pos,0.5,0.5,0.5);
  scale_feature((featureptr) handletopface,pos,origin);
  axis_rot_obj(hammerhandletop,x_axis,origin,-900);
  setpos3d(pos,1.0,1.0,0.5);
  scale_feature((featureptr) hammerhandletop,pos,origin);
  setpos3d(pos,0.0,-hammerheadheight,hammerheadepth/2.0);
  translate_feature((featureptr) hammerhandletop,pos,FALSE);
  set_feature_color((featureptr) hammerhandletop,handlecolor);
  adopt_obj_branch(hammer,hammerhandletop,FALSE);
#endif
  
  hammerhandle = create_oval_cylinder(world,hammerheadepth/2.0,
				      hammerheadepth/2.0,hammerhandleheight,
				      hammeresolution);
  set_feature_color((featureptr) hammerhandle,handlecolor);
  axis_rot_obj(hammerhandle,x_axis,origin,900);
  setpos3d(pos,1.0,1.0,0.5);
  scale_feature((featureptr) hammerhandle,pos,origin);
  setpos3d(pos,0.0,-hammerheadheight,hammerheadepth/2.0);
  translate_feature((featureptr) hammerhandle,pos,FALSE);
  adopt_obj_branch(hammer,hammerhandle,FALSE);
  
  return(hammer);		/* mike hammer? */
}


  objptr
create_sketch_tool(worldptr world,vertype offset,char *pencilcolor,
		   char *erasercolor)
{
  objptr pencil,eraser;
  vertype newcorner;
  fveptr firstfve;
  leptr firstle,newvertle,hotspotle;
  static vertype depth = {0.0,0.0,Pencilength};
  vertype pos;
  float a,x,y;
  ;
  newcorner[vx] = Pencilwidth;
  newcorner[vy] = 0.0;
  newcorner[vz] = 0.0;
  newcorner[vw] = 1.0;
  pencil = make_obj(world,newcorner);
  set_object_name(pencil,"Pencil");
  add_property((featureptr) pencil,draw_prop);
  add_property((featureptr) pencil,tool_prop);
  firstfve = First_obj_fve(pencil);
  firstle = newvertle = firstfve->boundlist->last.bnd->lelist->last.le;

  for (a = 60.0; a < 360.0; a += 60.0)
  {
    newcorner[vx] = Pencilwidth * cos(a * Deg2rad);
    newcorner[vy] = Pencilwidth * sin(a * Deg2rad);
    newvertle = make_edge_vert(newvertle,newvertle,newcorner);
  }
  /* close face */
  make_edge_face(firstle,newvertle);
  /* push out pencil */
  extrude_face(firstfve,depth);
  /* pull out pencil tip */
  newcorner[vx] = 0.0;
  newcorner[vy] = 0.0;
  newcorner[vz] = Penciltip + Pencilength;
  hotspotle = (leptr) split_face_at_vert(firstfve,newcorner);
  add_property((featureptr) hotspotle->facevert,hotspot_prop);
  set_feature_named_color((featureptr) pencil,pencilcolor);

  eraser = create_oval_cylinder(world,Pencilwidth,Pencilwidth,
				Pencilength/7.0, 360/60);
  set_object_name(eraser,"Eraser");
  add_property((featureptr) eraser,erase_prop);
  add_property((featureptr) eraser,tool_prop);
  setpos3d(pos,0.0,0.0,-Pencilength/7.0);
  translate_feature((featureptr) eraser,pos,FALSE);
  add_property((featureptr) First_obj_vfe(eraser),hotspot_prop);
  set_feature_named_color((featureptr) eraser,erasercolor);
  adopt_obj_branch(pencil,eraser,FALSE);

  axis_rot_obj(pencil,x_axis,origin,-900);
  translate_feature((featureptr) pencil,offset,FALSE);

  log_update((featureptr) pencil, Allupdateflags,Immediateupdate);
  
  add_property_to_descendants((featureptr) pencil,lockedgeometry_prop);

  return(pencil);
}


  objptr
create_slice_tool(worldptr world,vertype offset,char *bladecolor,
		  char *handlecolor)
{
  objptr blade, handle;
  fveptr thisfve;
  leptr firstle,le1,hotspotle;
  vertype first_pos,pos;
  GLfloat xpos,ypos;
  static vertype depth = {0.0,0.0,Sawbladedepth/2.0};
  int i;
  ;
  /* Make saw blade. */
  setpos3d(first_pos,-Sawbladewidth/2.0-Sawbladewidth/10.0,0.0,0.0);
  blade = make_obj(world,first_pos);
  firstle = le1 = First_obj_le(blade);
  setpos3d(pos,Sawbladewidth/2.0,0.0,0.0);
  le1 = make_edge_vert(le1,le1,pos);
  setpos3d(pos,Sawbladewidth/2.0,-Sawbladeheight,0.0);
  le1 = make_edge_vert(le1,le1,pos);
  xpos = Sawbladewidth/2.0;
  for (i = 1; i < /*Numsawnicks*/13-1; ++i) /* Numsawnicks is 15 right now*/
  {
    if ((i % 2) != 0)
      ypos = -Sawbladeheight + Sawnickheight;
    else
      ypos = -Sawbladeheight;
    xpos -= Sawbladewidth / 13.0; /*Sawnickwidth;*/
    setpos3d(pos,xpos,ypos,0.0);
    le1 = make_edge_vert(le1,le1,pos);
  }
  make_edge_face(le1,firstle);
  extrude_face(First_obj_fve(blade),depth);
  setpos3d(pos,0.0,0.0,-Sawbladedepth/4.0);
  translate_feature((featureptr) blade,pos,FALSE);
  set_feature_named_color((featureptr) blade,bladecolor);
  
  /* Make saw handle */
  handle = create_oval_cylinder(world,Sawhandleheight/2.0,Sawhandledepth/2.0,
				Sawhandlewidth,6);
  axis_rot_obj(handle,y_axis,origin,900);
  setpos3d(pos,Sawbladewidth / 2.0,-Sawhandleheight/2.0,0.0);
  translate_feature((featureptr) handle,pos,FALSE);
  set_feature_named_color((featureptr) handle,handlecolor);

  /* Set up the tools props. */
  add_property((featureptr) handle,slice_prop);
  add_property((featureptr) handle,tool_prop);
  add_property_to_descendants((featureptr) handle,lockedgeometry_prop);
  add_property((featureptr) (First_obj_le(handle)->facevert), hotspot_prop);
  set_object_name(handle,"Saw");

  adopt_obj_branch(handle,blade,FALSE);

  translate_feature((featureptr) handle,offset,FALSE);

  log_update((featureptr) handle,Allupdateflags, Immediateupdate);
  return(handle);
}

  objptr
create_magnet(worldptr world,GLfloat magnetradius,GLfloat barthickness,
	      GLfloat magnetdepth, int magnetresolution,
	      colortype bodycolor, colortype endscolor)
{
  objptr magnet,end1,end2;
  leptr firstle,thisle;
  GLfloat angleinc,midline;
  vertype pos;
  int i;
  ;
  angleinc = (180.0 / magnetresolution) * Deg2rad;
  midline = magnetradius * 0.75;
  pos[vz] = 0.0; pos[vw] = 1.0;
  /* Make outer countour of magnet ring section */
  for (i = 0; i <= magnetresolution; ++i)
  {
    pos[vx] = cos((double) i * angleinc) * magnetradius;
    pos[vy] = sin((double) i * angleinc) * magnetradius * Magnetystretch;
    if (i == 0)
    {
      magnet = make_obj(world,pos);
      firstle =
	thisle = First_obj_le(magnet);
    }
    else
      thisle = make_edge_vert(thisle,thisle,pos);
  }
  for (; i >= 0; --i)
  {
    pos[vx] = cos((double) i * angleinc) * (magnetradius - barthickness);
    pos[vy] = sin((double) i * angleinc) * (magnetradius - barthickness) *
      Magnetystretch;
    thisle = make_edge_vert(thisle,thisle,pos);
  }  
  
  make_edge_face(firstle,thisle);
  setpos3d(pos,0.0,0.0,magnetdepth);
  extrude_face(First_obj_fve(magnet),pos);
  setpos3d(pos,0.0,midline,0.0);
  translate_feature((featureptr) magnet, pos, FALSE);
  set_feature_color((featureptr) magnet, bodycolor);
  add_property((featureptr) magnet,magnetic_prop);
  add_property((featureptr) magnet, tool_prop);
  add_property((featureptr) (First_obj_le(magnet)->facevert), hotspot_prop);

  setpos3d(pos,-magnetradius/2.0,0.0,0.0);
  end1 = make_obj(world, pos);
  firstle = thisle = First_obj_le(end1);
  setpos3d(pos,-magnetradius/2.0 - barthickness,0.0,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,-magnetradius,midline,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,-magnetradius + barthickness,midline,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);

  make_edge_face(thisle,firstle);
  setpos3d(pos,0.0,0.0,magnetdepth);
  extrude_face(First_obj_fve(end1),pos);

  set_feature_color((featureptr) end1, endscolor);
  add_property((featureptr) end1,lockedgeometry_prop);
  adopt_obj_branch(magnet,end1,FALSE);

  setpos3d(pos,magnetradius/2.0,0.0,0.0);
  end2 = make_obj(world, pos);
  firstle = thisle = First_obj_le(end2);
  setpos3d(pos,magnetradius/2.0 + barthickness,0.0,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,magnetradius,midline,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,magnetradius - barthickness,midline,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);

  make_edge_face(thisle,firstle);
  setpos3d(pos,0.0,0.0,magnetdepth);
  extrude_face(First_obj_fve(end2),pos);

  set_feature_color((featureptr) end2, endscolor);
  add_property((featureptr) end2,lockedgeometry_prop);
  adopt_obj_branch(magnet,end2,FALSE);

  add_property_to_descendants((featureptr) magnet,lockedgeometry_prop);
  return(magnet);
}

  objptr
create_magnifier(worldptr world, GLfloat lensradius, GLfloat holeradius,
		 int lensresolution, colortype lenscolor,
		 GLfloat handlewidth, GLfloat handleheight, colortype handlecolor)
{
  objptr magnifier,handle,lens, connector;
  GLfloat handledepth, connectorheight;
  fveptr lensface,othersideface,lenshole;
  leptr firstle,thisle;
  GLfloat angleinc;
  vertype pos;
  int i;
  ;
  handledepth = handlewidth;
  connectorheight = handleheight / 10.0;
  /* make a tall handle for the magnifying_glass */
  magnifier = handle = create_cube(world, handlewidth,handleheight,
				   handledepth);
  set_object_name(magnifier,"Magnifier");
  setpos3d(pos,-handlewidth/2.0,0.0,0.0);
  translate_feature((featureptr) handle, pos,FALSE);
  /* Make a connector pyramid */
  setpos3d(pos,0.0,handleheight + connectorheight,handledepth/2.0);
  split_face_at_vert(find_face_given_normal(handle,y_axis), pos);

  add_property((featureptr) magnifier, zoom_prop);
  add_property((featureptr) magnifier, tool_prop);
  
  /*  add its color and hotspot */
  set_feature_color((featureptr) handle,handlecolor);
  add_property((featureptr) (First_obj_le(handle)->facevert), hotspot_prop);

  /* Make a lens cylinder with the same thickness... */
  lens = create_oval(world, (int) lensradius, (int) lensradius, lensresolution);
  add_property((featureptr) lens,lockedgeometry_prop);
  set_feature_color((featureptr) lens, lenscolor);
  /* Cut a hole in the lens front face */
  lensface = First_obj_fve(lens);
  othersideface = Twin_le(lensface->boundlist->first.bnd->lelist->first.le)->
    motherbound->motherfve;
  angleinc = (360.0 / lensresolution) * Deg2rad;
  pos[vz] = 0.0; pos[vw] = 1.0;
  for (i = 0; i < lensresolution; ++i)
  {
    pos[vx] = cos((double) i * angleinc) * (double) holeradius;
    pos[vy] = sin((double) i * angleinc) * (double) holeradius;
    if (i == 0)
    {
      thisle = First_obj_le(lens);
      lensface = (fveptr) find_largergrain((featureptr) thisle, Fve_type);
      firstle = thisle = make_edge_vert(thisle,thisle,pos);
      kill_edge_make_ring(Twin_le(thisle),thisle);
    }
    else
      thisle = make_edge_vert(thisle,thisle,pos);
  }
  lenshole = make_edge_face(firstle,thisle);
  kill_face_make_loop(lenshole,othersideface);
  setpos3d(pos,0.0,0.0,handlewidth);
  extrude_face(lensface,pos);
  setpos3d(pos,0.0, handleheight + lensradius + connectorheight,
	  0.0);
  translate_feature((featureptr) lens, pos, FALSE);
  adopt_obj(handle,lens,FALSE);

  add_property_to_descendants((featureptr) magnifier,lockedgeometry_prop);
  return(magnifier);
}

 objptr
create_fencepart(worldptr world,GLfloat fencepartwidth,GLfloat fencepartheight,
		 GLfloat fencepartdepth,colortype fencepartcolor,int numposts)
{
  objptr fencepart,basepiece,post;
  GLfloat basetopost,fencepostwidth;
  leptr firstle,thisle;
  vertype pos;
  int i;
  ;
  basetopost = fencepartheight/8.0;
  fencepostwidth = fencepartwidth / (numposts * 2.0);
  fencepart = basepiece =
    create_cube(world,fencepartwidth,basetopost, -basetopost);
  setpos3d(pos,0.0,basetopost,-fencepartdepth);
  translate_feature((featureptr) basepiece,pos,FALSE);
  set_feature_color((featureptr) basepiece,fencepartcolor);

  basepiece = create_cube(world,fencepartwidth,basetopost, -basetopost);
  set_feature_color((featureptr) basepiece,fencepartcolor);
  setpos3d(pos,0.0,fencepartheight - basetopost - fencepostwidth*1.5,
	   -fencepartdepth);
  translate_feature((featureptr) basepiece,pos,FALSE);
  adopt_obj_branch(fencepart,basepiece,FALSE);

  for (i = 0; i < numposts; ++i)
  {
    /* make posts of fence */
    post = make_obj(world,origin);
    set_feature_color((featureptr) post,fencepartcolor);
    firstle = thisle = First_obj_le(post);
    setpos3d(pos,0.0,fencepartheight - fencepostwidth,0.0);
    thisle = make_edge_vert(thisle,thisle,pos);
    setpos3d(pos,fencepostwidth/2.0,fencepartheight,0.0);
    thisle = make_edge_vert(thisle,thisle,pos);
    setpos3d(pos,fencepostwidth,fencepartheight - fencepostwidth,0.0);
    thisle = make_edge_vert(thisle,thisle,pos);
    setpos3d(pos,fencepostwidth,0.0,0.0);
    thisle = make_edge_vert(thisle,thisle,pos);
    make_edge_face(firstle,thisle);
    setpos3d(pos,0.0,0.0,-fencepartdepth);
    extrude_face(First_obj_fve(post),pos);
    setpos3d(pos,i * (fencepostwidth * 2.0),0.0,0.0);
    translate_feature((featureptr) post,pos,FALSE);
    adopt_obj_branch(fencepart,post,FALSE);
  }      
  return(fencepart);
}

 objptr
create_fence(worldptr world,GLfloat fencewidth,GLfloat fenceheight,GLfloat fencedepth,
	     colortype fencecolor)
{
  objptr fence,fencepart;
  vertype pos;
  ;
  /* the fence is used for grouping things: it's actually two fences that */
  /* swing open to indicate ungrouping */
  fence = create_fencepart(world,fencewidth/2.0,fenceheight,fencedepth,
			   fencecolor,4);
  set_object_name(fence,"Fence");

  fencepart = create_fencepart(world,fencewidth/2.0,fenceheight,fencedepth,
			       fencecolor,4);
  setpos3d(pos,-fencewidth/2.0,0.0,0.0);
  translate_feature((featureptr) fencepart,pos,FALSE);
  adopt_obj_branch(fence,fencepart,FALSE);

  add_property_to_descendants((featureptr) fence,lockedgeometry_prop);
  add_property((featureptr) fence, group_prop);
  add_property((featureptr) fence, tool_prop);
  return(fence);
}     

/* create a little men for a group tool pad. */

  void
create_grouped_littlemen(worldptr world)
{
  vertype pos;
  objptr littleman,lman2,lman3;
  FILE *littleman_rscfile;
  GLfloat littlewidth;
  ;
  clear_world(scratch_world);
  read_decal_file(scratch_world->root,"./rsc/littleman.dcl");
  littleman = scratch_world->root->children->first.obj;
  set_3d_objbbox(littleman);
  littlewidth = littleman->objbbox.maxcorner[vx] -
    littleman->objbbox.mincorner[vx];
  
  setpos3d(pos,littlewidth,0.0,0.0);
  lman2 = duplicate_and_translate_obj(littleman,pos);
  adopt_obj_branch(littleman,lman2,FALSE);
  setpos3d(pos,littlewidth*2,0.0,0.0);
  lman3 = duplicate_and_translate_obj(littleman,pos);
  adopt_obj_branch(littleman,lman3,FALSE);
  
  adopt_obj_branch(world->root,littleman,TRUE);
  write_data_file("./rsc/group.rsc",(featureptr) littleman);

  kill_obj_branch(littleman);
}

  void
create_ungrouped_littlemen(worldptr world)
{
  vertype pos;
  objptr littleman,lman2,lman3;
  FILE *littleman_rscfile;
  GLfloat littlewidth,spacing = 1.5;
  ;
  clear_world(scratch_world);
  read_decal_file(scratch_world->root,"./rsc/littleman.dcl");
  littleman = scratch_world->root->children->first.obj;
  set_3d_objbbox(littleman);
  littlewidth = littleman->objbbox.maxcorner[vx] -
    littleman->objbbox.mincorner[vx];
  
  setpos3d(pos,littlewidth*spacing,0.0,0.0);
  lman2 = duplicate_and_translate_obj(littleman,pos);
  adopt_obj_branch(littleman,lman2,FALSE);
  setpos3d(pos,littlewidth*spacing*2,0.0,0.0);
  lman3 = duplicate_and_translate_obj(littleman,pos);
  adopt_obj_branch(littleman,lman3,FALSE);
  
  setpos3d(pos,0.0,
	   littleman->objbbox.maxcorner[vy] - littleman->objbbox.mincorner[vy],
	   0.0);
  translate_feature((featureptr) littleman,pos,FALSE);
  adopt_obj_branch(world->root,littleman,TRUE);
  write_data_file("./rsc/ungroup.rsc",(featureptr) littleman);

  kill_obj_branch(littleman);
}

  objptr
create_spotlight(worldptr world,GLfloat spotradius,GLfloat spotdepth,
		 colortype litfacecolor,colortype bodycolor)
{
  objptr spotlight,body,blind;
  int spotresolution = 8;
  fveptr litface;
  vertype pos;
  ;
  spotlight = body = create_oval_cylinder(world,spotradius/2.0,spotradius/2.0,
					  spotdepth,spotresolution);
  set_object_name(spotlight,"Spotlight");
  set_feature_color((featureptr) body, bodycolor);
  litface = find_face_given_normal(body,z_axis);
  set_feature_color((featureptr) litface,litfacecolor);
  add_property((featureptr) body,lockedgeometry_prop);

  blind = create_square(world,spotradius,spotradius);
  set_feature_color((featureptr) blind,bodycolor);
  axis_rot_obj(blind,y_axis,origin,-450);
  setpos3d(pos,spotradius/2.0,-spotradius/2.0,spotdepth);
  translate_feature((featureptr) blind,pos,FALSE);
  transform_obj_tree(blind);
  find_branch_normals(blind);
  add_property((featureptr) blind,nonmanifold_prop);
  adopt_obj_branch(spotlight,blind,FALSE);

  blind = create_square(world,spotradius,spotradius);
  set_feature_color((featureptr) blind,bodycolor);
  axis_rot_obj(blind,y_axis,origin,-1350);
  setpos3d(pos,-spotradius/2.0,-spotradius/2.0,spotdepth);
  translate_feature((featureptr) blind,pos,FALSE);
  transform_obj_tree(blind);
  find_branch_normals(blind);
  add_property((featureptr) blind,nonmanifold_prop);
  adopt_obj_branch(spotlight,blind,FALSE);

  blind = create_square(world,spotradius,spotradius);
  set_feature_color((featureptr) blind,bodycolor);
  axis_rot_obj(blind,x_axis,origin,450);
  setpos3d(pos,-spotradius/2.0,spotradius/2.0,spotdepth);
  translate_feature((featureptr) blind,pos,FALSE);
  transform_obj_tree(blind);
  find_branch_normals(blind);
  add_property((featureptr) blind,nonmanifold_prop);
  adopt_obj_branch(spotlight,blind,FALSE);

  blind = create_square(world,spotradius,spotradius);
  set_feature_color((featureptr) blind,bodycolor);
  axis_rot_obj(blind,x_axis,origin,1350);
  setpos3d(pos,-spotradius/2.0,-spotradius/2.0,spotdepth);
  translate_feature((featureptr) blind,pos,FALSE);
  transform_obj_tree(blind);
  find_branch_normals(blind);
  add_property((featureptr) blind,nonmanifold_prop);
  adopt_obj_branch(spotlight,blind,FALSE);

  axis_rot_obj(spotlight,x_axis,origin,250);
  axis_rot_obj(spotlight,y_axis,origin,350);

  add_property((featureptr) spotlight,goodies_prop);
  return(spotlight);
}

   objptr
create_lightbulb(worldptr world,colortype bulbcolor,colortype basecolor)
{
  objptr lightbulb,bulb,base;
  vertype pos;
  leptr thisle;
  static vertype bulbpos[7] = {{-2.0,0.0,0.0},
			       {-15.0,-4.0,0.0},
			       {-20.0,-9.0,0.0},
			       {-22.0,-18.0,0.0},
			       {-20.0,-25.0,0.0},
			       {-10.0,-38.0,0.0},
			       {-8.0,-52.0,0.0}};
  int i,numincrements = 11,maxbulbpos = 6;
  GLfloat scalefactor = 1.5;
  ;
  setpos3d(pos,bulbpos[0][vx]*scalefactor,bulbpos[0][vy]*scalefactor,
	     bulbpos[0][vz]*scalefactor);
  lightbulb = bulb = make_obj(world,pos);
  set_object_name(lightbulb,"Lightbulb");
  thisle = First_obj_le(bulb);
  for (i = 1; i <= maxbulbpos; ++i)
  {
    setpos3d(pos,bulbpos[i][vx]*scalefactor,bulbpos[i][vy]*scalefactor,
	     bulbpos[i][vz]*scalefactor);
    thisle = make_edge_vert(thisle,thisle,pos);
  }
  make_solid_of_rotation(bulb,y_axis,origin,numincrements);
  set_feature_color((featureptr) bulb,bulbcolor);

  setpos3d(pos,bulbpos[maxbulbpos][vx]*scalefactor,0.0,0.0);
  base = make_obj(world,pos);
  thisle = First_obj_le(base);
  
  for (i = 1; i <8; ++i)
  {
    setpos3d(pos,bulbpos[maxbulbpos][vx]*scalefactor,-i * 2.0*scalefactor,0.0);
    if (i % 2 == 0)
      pos[vx] -= 2.0*scalefactor;
    thisle = make_edge_vert(thisle,thisle,pos);
  }
  make_solid_of_rotation(base,y_axis,origin,numincrements);
  set_feature_color((featureptr) base,basecolor);
  setpos3d(pos,0.0,bulbpos[maxbulbpos][vy] * scalefactor,0.0);
  translate_feature((featureptr) base,pos,FALSE);
  adopt_obj_branch(bulb,base,FALSE);

  add_property((featureptr) lightbulb,lightbulb_prop);
  add_property_to_descendants((featureptr) lightbulb,lockedgeometry_prop);
  return(lightbulb);
}
  
    
  
  objptr
create_nikon(worldptr world,GLfloat width,GLfloat height,GLfloat depth,
	     GLfloat lensradius,int lensresolution,
	     colortype bodycolor,colortype lenscolor,
	     colortype windercolor,colortype capcolor)
{
  objptr nikon,body,lens,lenscap,viewfinder,winder;
  GLfloat lenslength,camerabevel,winderadius,winderdepth;
  vertype pos;
  leptr firstle,thisle;
  fveptr viewfinderfront;
  evfptr viewfindertopedge;
  ;
  lenslength = depth * 1.25;
  camerabevel = width / 20.0;
  setpos3d(pos,width/2.0 - camerabevel,0.0,-depth);
  nikon = body = make_obj(world,pos);
  set_feature_color((featureptr) body, bodycolor);
  set_object_name(nikon,"Nikon");
  firstle = thisle = First_obj_le(body);
  setpos3d(pos,width/2.0,0.0,-depth + camerabevel);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,width/2.0,0.0,-camerabevel);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,width/2.0 - camerabevel,0.0,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,-width/2.0+camerabevel,0.0,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,-width/2.0,0.0,-camerabevel);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,-width/2.0,0.0,-depth+camerabevel);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,-width/2.0+camerabevel,0.0,-depth);
  thisle = make_edge_vert(thisle,thisle,pos);

  make_edge_face(thisle,firstle);
  setpos3d(pos,0.0,height,0.0);
  extrude_face(First_obj_fve(body),pos);

  lens = create_oval_cylinder(world,lensradius/2.0,lensradius/2.0,lenslength,
			      lensresolution);
  set_feature_color((featureptr) lens,lenscolor);
  setpos3d(pos,0.0,height/2.0,0.0);
  translate_feature((featureptr) lens,pos,FALSE);
  adopt_obj_branch(nikon,lens,TRUE);

  winderadius = depth/4.0;
  winderdepth = depth/3.0;
  winder = create_oval_cylinder(world,winderadius,winderadius,winderdepth,
				lensresolution/2);
  set_feature_color((featureptr) winder,windercolor);
  axis_rot_obj(winder,rotx_axis,origin,900);
  setpos3d(pos,-width/2.0 + winderadius*1.5,height+winderdepth,-depth/2.0);
  add_property((featureptr) winder, nonmanifold_prop);
  translate_feature((featureptr) winder,pos,FALSE);
  adopt_obj_branch(nikon,winder,FALSE);

  viewfinder = create_cube(world, width / 4.0, height / 4.0,-depth);
  viewfinderfront = find_face_given_normal(viewfinder,x_axis);
  viewfindertopedge = First_obj_evf(viewfinder)->next->next;
  setpos3d(pos,0.0,-height/4.0 + height / 8.0,0.0);
  translate_feature((featureptr) viewfindertopedge,pos,FALSE);
  setpos3d(pos,-width/8.0,height,0.0);
  translate_feature((featureptr) viewfinder,pos,FALSE);
  set_feature_color((featureptr) viewfinder, bodycolor);
  adopt_obj_branch(nikon,viewfinder,FALSE);

  lenscap = create_oval_cylinder(world,lensradius/2.0,lensradius/2.0,
				 lenslength / 8.0,lensresolution);
  set_feature_color((featureptr) lenscap,capcolor);
  setpos3d(pos,0.0,height/2.0,lenslength);
  translate_feature((featureptr) lenscap,pos,FALSE);
  adopt_obj_branch(lens,lenscap,TRUE);

  add_property_to_descendants((featureptr) nikon,lockedgeometry_prop);
  add_property((featureptr) nikon,goodies_prop);
  read_CPL_privatecode("./cpl/nikon.cpl",(featureptr) nikon);
  read_CPL_privatecode("./cpl/lens.cpl",(featureptr) lens);
  return(nikon);
}



  objptr
create_movie_camera(worldptr world,GLfloat width,GLfloat height,GLfloat depth,
	      int lensresolution, int reelresolution,
	      colortype bodycolor,colortype lenscolor,colortype reelscolor)
{
  objptr camera,body,lens,lenscap,reel1,reel2;
  GLfloat lenslength;
  vertype pos;
  ;
  camera = body = create_cube(world,width,height,depth);
  set_object_name(camera,"MovieCamera");
/*  add_property((featureptr) camera, camera_prop);*/
  add_property((featureptr) camera, tool_prop);
  add_property((featureptr) (First_obj_le(body)->facevert), hotspot_prop);
  set_feature_color((featureptr) body,bodycolor);

  lenslength = width/2.5;
  lens = create_oval_cylinder(world,depth/2.0,depth/2.0,lenslength,
			      lensresolution);
  set_feature_color((featureptr) lens,lenscolor);
  axis_rot_obj(lens,y_axis,origin,900);
  setpos3d(pos,width,height/2.0,depth/2.0);
  translate_feature((featureptr) lens,pos,FALSE);
  adopt_obj_branch(camera,lens,FALSE);

  lenscap = create_oval_cylinder(world,depth/1.3,depth/1.3,lenslength/3.0,
				 lensresolution);
  set_feature_color((featureptr) lenscap,lenscolor);
  axis_rot_obj(lenscap,y_axis,origin,900);
  setpos3d(pos,width + lenslength,height/2.0,depth/2.0);
  translate_feature((featureptr) lenscap,pos,FALSE);
  adopt_obj_branch(camera,lenscap,FALSE);

  reel1 = create_oval_cylinder(world,height*.75,height*.75,depth,
			       reelresolution);
  reel2 = create_oval_cylinder(world,height*.75,height*.75,depth,
			       reelresolution);
  set_feature_color((featureptr) reel1,reelscolor);
  set_feature_color((featureptr) reel2,reelscolor);
  setpos3d(pos,0.0,height * 1.75,0.0);
  translate_feature((featureptr) reel1,pos,FALSE);
  setpos3d(pos,height*1.5,height * 1.75,0.0);
  translate_feature((featureptr) reel2,pos,FALSE);
  adopt_obj_branch(camera,reel1,FALSE);
  adopt_obj_branch(camera,reel2,FALSE);

  add_property_to_descendants((featureptr) body,lockedgeometry_prop);
  add_property((featureptr) body,goodies_prop);
  return(camera);
}


  objptr
create_floppy(worldptr world,colortype floppycolor,colortype labelcolor,
	      colortype doorcolor)
{
  static vertype floppyarray[] = {{Floppynotch,0.0,0.0},
				  {Floppysize-Floppycorner,0.0,0.0},
				  {Floppysize,Floppycorner,0.0},
				  {Floppysize,Floppysize - Floppycorner,0.0},
				  {Floppysize-Floppycorner,Floppysize,0.0},
				  {Floppycorner,Floppysize,0.0},
				  {0.0,Floppysize-Floppycorner,0.0},
				  {0.0,Floppynotch,0.0}};
  int floppyarraysize = sizeof(floppyarray) / sizeof(vertype);
  static vertype labelarray[] = {{Floppynotch,Floppysize,Floppydepth},
				 {Floppynotch,Floppysize/3.0 + Floppynotch,
				    Floppydepth},
				 {Floppysize - Floppynotch,
				    Floppysize/3.0 + Floppynotch, Floppydepth},
				 {Floppysize - Floppynotch,Floppysize,
				    Floppydepth}};
  int labelarraysize = sizeof(labelarray) / sizeof(vertype);
  static vertype doorarray[] = {{Floppynotch*3.0,0.0,Floppydepth},
				{Floppynotch*3.0,Floppysize/3.0 - Floppycorner,
				   Floppydepth},
				{Floppynotch*3.0 + Floppycorner,Floppysize/3.0,
				   Floppydepth},
				{Floppysize - Floppynotch*3.0 - Floppycorner,
				   Floppysize/3.0, Floppydepth},
				{Floppysize - Floppynotch*3.0,
				   Floppysize/3.0 - Floppycorner, Floppydepth},
				{Floppysize - Floppynotch*3.0,
				   0.0, Floppydepth}};
  int doorarraysize = sizeof(doorarray) / sizeof(vertype);
  static vertype holearray[] = {{Floppynotch*4.0,Floppynotch,Floppydepth*2},
				{Floppynotch*4.0, Floppysize/3.0 - Floppycorner,
				   Floppydepth*2},
				{Floppynotch*6.5, Floppysize/3.0 - Floppycorner,
				   Floppydepth*2},
				{Floppynotch*6.5, Floppynotch, Floppydepth*2}};
  int holearraysize = sizeof(holearray) / sizeof(vertype);
  objptr floppy,label,door,hole;
  int i;
  leptr firstle,thisle;
  vertype pos;
  ;
  floppy = make_obj(world,floppyarray[0]);
  set_object_name(floppy,"FloppyDisk");
  add_property((featureptr) floppy,nonmanifold_prop);
  set_feature_color((featureptr) floppy,floppycolor);
  firstle = thisle = First_obj_le(floppy);
  for (i = 1; i < floppyarraysize; ++i)
    thisle = make_edge_vert(thisle,thisle,floppyarray[i]);
  make_edge_face(thisle,firstle);
  setpos3d(pos,0.0,0.0,-Floppydepth);
  extrude_face(First_obj_fve(floppy),pos);

  label = make_obj(world,labelarray[0]);
  set_feature_color((featureptr) label,labelcolor);
  firstle = thisle = First_obj_le(label);
  for (i = 1; i < labelarraysize; ++i)
    thisle = make_edge_vert(thisle,thisle,labelarray[i]);
  make_edge_face(thisle,firstle);
  adopt_obj_branch(floppy,label,FALSE);

  door = make_obj(world,doorarray[0]);
  set_feature_color((featureptr) door,doorcolor);
  firstle = thisle = First_obj_le(door);
  for (i = 1; i < doorarraysize; ++i)
    thisle = make_edge_vert(thisle,thisle,doorarray[i]);
  make_edge_face(thisle,firstle);
  adopt_obj_branch(floppy,door,FALSE);

  hole = make_obj(world,holearray[0]);
  set_feature_color((featureptr) hole,floppycolor);
  firstle = thisle = First_obj_le(hole);
  for (i = 1; i < holearraysize; ++i)
    thisle = make_edge_vert(thisle,thisle,holearray[i]);
  make_edge_face(thisle,firstle);
  adopt_obj_branch(floppy,hole,FALSE);

  return(floppy);
}
  
  objptr
create_storage_pad(worldptr world,colortype storagepadcolor)
{
  objptr floppy,storage_pad,storage_plate;
  leptr thisle;
  GLfloat diskmargin = Storagesquare / 8.0;
  vertype pos;
  static colortype labelcolor = {255,150,255};
  ;
  storage_pad = make_obj(world,origin);
  set_object_name(storage_pad, "StoragePad");
  add_property((featureptr) storage_pad,nonmanifold_prop);
  add_property((featureptr) storage_pad,noshadow_prop);
  thisle = First_obj_le(storage_pad);
  setpos3d(pos,Storagesquare,Storagesquare,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  set_feature_color((featureptr) storage_pad,storagepadcolor);
  storage_pad->selectechnique = set_normalpad_selectable;
  storage_pad->drawtechnique = draw_plate_technique;
  
  /* move pad forward so it lays over save area... */
  setpos3d(pos,0.0,0.0,10.0);
  translate_feature((featureptr) storage_pad,pos,FALSE);

  floppy = create_floppy(world,magenta,labelcolor,white);
  apply_front_decal(floppy,storage_pad,Decalthickness,Decalzoffset,diskmargin);
  kill_obj_branch(floppy);

  storage_plate = read_plate(world,storage_pad,"./rsc/storageplate.rsc");
  set_object_name(storage_plate, "StoragePlate");

  /* move pad to final position */
  setpos3d(pos,Mincrossx + Storagesquare*.5,Mincrossy + Storagesquare * 0.5,
	   Minplanez);
  translate_feature((featureptr) storage_pad,pos,FALSE);
  
  add_property((featureptr) storage_pad,dialog_prop);
  return(storage_pad);
}

  objptr
create_ruler(worldptr world, GLfloat width, GLfloat height, GLfloat depth,
	     int numnotches,colortype rulercolor)
{
  objptr ruler,body,bevel,notch;
  vertype pos;
  colortype bodycolor;
  GLfloat zincrement;
  int i;
  ;
  scalecolor(rulercolor,2.0,bodycolor);
  ruler = body = create_cube(world,depth,height,width);
  setpos3d(pos,-depth/2.0,0.0,0.0);
  translate_feature((featureptr) body,pos,FALSE);
  set_feature_color((featureptr) body, bodycolor);
  bevel = create_isosceles_prism(world, depth/2,width);
  set_feature_color((featureptr) bevel, rulercolor);
  setpos3d(pos,0.0,height+depth/2.0,0.0);
  translate_feature((featureptr) bevel,pos,FALSE);
  adopt_obj_branch(body,bevel,FALSE);
  zincrement = width / (GLfloat) numnotches;
  for (i = 0; i <= numnotches; ++i)
  {
    notch = create_square(world,zincrement/8,depth);
    set_feature_color((featureptr) notch, black);
    axis_rot_obj(notch,y_axis,origin,900);
    axis_rot_obj(notch,z_axis,origin,300);
    setpos3d(pos,depth*1.3,height,zincrement*i);
    translate_feature((featureptr) notch,pos,FALSE);
    adopt_obj_branch(bevel,notch,FALSE);
  }

  axis_rot_obj(body,y_axis,origin,-900);
  axis_rot_obj(body,x_axis,origin,-50);

  return(ruler);
}
    
  objptr
create_measure_pad(worldptr world, colortype measurepadcolor)
{
  objptr measure_pad,ruler;
  objptr measure_plate,name_plate,cube_plate,length_plate,distance_plate;
  GLfloat rulermargin = Storagesquare / 8.0;
  vertype pos;
  leptr thisle;
  ;
  measure_pad = make_obj(world,origin);
  set_object_name(measure_pad,"MeasurePad");
  add_property((featureptr) measure_pad,nonmanifold_prop);
  add_property((featureptr) measure_pad,noshadow_prop);
  thisle = First_obj_le(measure_pad);
  setpos3d(pos,Storagesquare*2,Storagesquare*.6,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  set_feature_color((featureptr) measure_pad,measurepadcolor);
  measure_pad->selectechnique = set_normalpad_selectable;
  measure_pad->drawtechnique = draw_plate_technique;
  
  /* move pad forward so it lays over input area... */
  setpos3d(pos,0.0,0.0,10.0);
  translate_feature((featureptr) measure_pad,pos,FALSE);

  ruler = create_ruler(world,120.0,20.0,10.0,7,darkyellow);
  apply_front_decal(ruler,measure_pad,Decalthickness,Decalzoffset,rulermargin);
  kill_obj_branch(ruler);

/*
  measure_plate = read_plate(world,measure_pad,"./rsc/measureplate.rsc");
  set_object_name(measure_plate, "Measure_Plate");
*/
  name_plate = read_plate(world,measure_pad,"./rsc/nameplate.rsc");
  cube_plate = read_plate(world,measure_pad,"./rsc/cubeplate.rsc");
  length_plate = read_plate(world,measure_pad,"./rsc/lengthplate.rsc");
  distance_plate = read_plate(world,measure_pad,"./rsc/distanceplate.rsc");
  
  /* move pad to final position */
  setpos3d(pos,Mincrossx + Storagesquare*2.5,
	   Mincrossy + Storagesquare / 2.0,0.0/*Minplanez*/);
  translate_feature((featureptr) measure_pad,pos,FALSE);
  
  read_CPL_privatecode("./cpl/measurepad.cpl",measure_pad);
  add_property((featureptr) measure_pad,dialog_prop);
  adopt_obj_branch(cutplane_obj,measure_pad,TRUE);

  return(measure_pad);
}


  objptr
create_drill_tool(worldptr world,GLfloat width,GLfloat height,GLfloat drillbitlength,
		  int drillresolution,colortype drillcolor,colortype bitcolor)
{
  objptr drill,bit,chuck1,chuck2,body,handle;
  vertype pos;
  fveptr bottomfve;
  int handleresolution,drillbitresolution = 6;
  GLfloat drillbitradius,chuck1length,chuck2length,bodywidth;
  ;
  bodywidth = width * 0.75;
  body = create_oval_cylinder(world,height/2.0,height/2.0,
				      bodywidth,drillresolution);
  set_object_name(body,"Body");
  setpos3d(pos,0.75,1.3,1.0);
  scale_feature((featureptr) body,pos,origin);
  set_feature_color((featureptr) body,drillcolor);
  handleresolution = drillresolution/2;
  handle = create_oval_cylinder(world,height/2.0,height/2.0,bodywidth*0.6,
				handleresolution);
  set_object_name(handle,"Handle");
  set_feature_color((featureptr) handle,drillcolor);
  bottomfve = find_face_given_normal(handle,z_axis);
  setpos3d(pos,0.0,-bodywidth*.3,0.0);
  translate_feature((featureptr) bottomfve,pos,FALSE); /* shear handle */
  setpos3d(pos,0.5,1.5,1.0);
  scale_feature((featureptr) handle,pos,origin); /* flatten it a bit */
  axis_rot_obj(handle,x_axis,origin,900);
  setpos3d(pos,0.0,0.0,height*1.5/2.0);
  translate_feature((featureptr) handle,pos,FALSE); /* slide handle fwd */
  drillbitradius = height/10.0;

  chuck1length = drillbitlength/2.5;
  chuck1 = create_oval_cylinder(world,drillbitradius*4,drillbitradius*4,
				chuck1length, drillbitresolution);
  set_feature_color((featureptr) chuck1,bitcolor);
  setpos3d(pos,0.0,0.0,bodywidth);
  translate_feature((featureptr) chuck1,pos,FALSE);

  chuck2length = chuck1length/2.0;
  chuck2 = create_oval_cone(world,drillbitradius*4,drillbitradius*4,
			    chuck2length,drillbitresolution);
  set_feature_color((featureptr) chuck2,bitcolor);
  setpos3d(pos,0.0,0.0,bodywidth+chuck1length);
  translate_feature((featureptr) chuck2,pos,FALSE);

  drill = bit =
    create_oval_cylinder(world,drillbitradius,drillbitradius,drillbitlength,
			 drillbitresolution);
  set_object_name(bit,"Bit");
  set_feature_color((featureptr) bit,bitcolor);
  setpos3d(pos,0.0,0.0,bodywidth+ chuck1length + chuck2length*.75);
  translate_feature((featureptr) bit,pos,FALSE);
  bottomfve = find_face_given_normal(bit,z_axis);
  add_property((featureptr) bottomfve->boundlist->first.bnd->lelist->
	       first.le->facevert, hotspot_prop);

  adopt_obj_branch(drill,body,FALSE);
  adopt_obj_branch(drill,handle,FALSE);
  adopt_obj_branch(drill,chuck1,FALSE);
  adopt_obj_branch(drill,chuck2,FALSE);

  axis_rot_obj(drill,y_axis,origin,900);
  
  add_property((featureptr) drill,drill_prop);
  add_property((featureptr) drill,tool_prop);
  add_property_to_descendants((featureptr) drill, lockedgeometry_prop);
  del_property_from_descendants((featureptr) drill, visible_prop);
  return(drill);
}

#if 0
  create_grouped_littlemen(world);
  create_ungrouped_littlemen(world);
  /* drill tool for bool subtraction */
  drill = create_drill_tool(primary_world,100,80.0/3.0,80.0/3.0,
			    12,blue,lightgrey);
  write_data_file("./rsc/drill.rsc",(featureptr) drill);
  kill_obj_branch(drill);

  hammer = create_hammer(world,lightgrey,brown,100.0,45.0,50.0,160.0);
  write_data_file("./rsc/hammer.rsc",(featureptr) hammer);
  apply_front_decal(hammer,headpad,Decalthickness*2,Decalzoffset,0.0);
  kill_obj_branch(hammer);

  magnet = create_magnet(world,50.0,10.0,10.0,13,lightgrey,orange);
  write_data_file("./rsc/magnet.rsc",(featureptr) magnet);
  kill_obj_branch(magnet);

  generic = create_cube(world,50.0,50.0,50.0);
  set_feature_color((featureptr) generic,green);
  write_data_file("./rsc/generic.rsc",(featureptr) generic);
  kill_obj_branch(generic);
#endif

  objptr
adjust_toolbar(objptr toolbar)
{
  objptr padhandle,newtoolbar,headpad,toolpad;
  GLfloat padhandleheight,totalpadsheight,toolbarheight;
  vertype scale_amt;
  int numpads;
  ;
  padhandle = toolbar->children->first.obj;
  toolpad = padhandle->children->first.obj;
  numpads = toolbar->children->numelems;
  if (padhandle == Nil)		/* No toolpads */
    totalpadsheight = 50.0;	/* some arbitrary, small size */
  else
  {
    set_3d_branchbboxes(padhandle);
    grow_group_bbox(padhandle);
    padhandleheight = padhandle->objbbox.maxcorner[vy] -
      padhandle->objbbox.mincorner[vy] +
	toolpad->objbbox.maxcorner[vy] - toolpad->objbbox.mincorner[vy];
    totalpadsheight = padhandleheight * 1.5 * numpads + padhandleheight / 4.0;
  }
  transform_obj(toolbar);
  set_3d_objbbox(toolbar);
  toolbarheight = toolbar->objbbox.maxcorner[vy] -
    toolbar->objbbox.mincorner[vy];
  if (toolbarheight != totalpadsheight)
  {
    newtoolbar = duplicate_obj(toolbar);
    if (!has_property((featureptr) toolbar,visible_prop))
      del_property((featureptr) newtoolbar,visible_prop);
    setpos3d(scale_amt,1.0,totalpadsheight/toolbarheight,1.0);
    scale_feature((featureptr) newtoolbar,scale_amt,toolbar->objbbox.maxcorner);
    headpad = toolbar->parent;
    /* replace the old toolbar with the new one */
    adopt_obj_branch(headpad,newtoolbar,FALSE);
    adopt_obj_branch(newtoolbar,toolbar,FALSE);
    kill_obj(toolbar);
    return(newtoolbar);
  }
  return(toolbar);
}

  objptr
create_toolbar(worldptr world,char *rscfilename)
{
  objptr toolbar,headpad,newpad,toolpad_orig,padhandle,padhandle_orig,newtool;
  vfeptr thisvfe;
  leptr thisle;
  vertype headpadpos,pos,offset;
  int i,numpads;
  colortype headpadcolor,toolpadcolor,padhandlecolor,toolbarcolor;
  FILE *toolpad_rscfile;
  char headpad_name[50],headpad_icon_name[50],toolpad_obj_filename[50];
  ;
  toolpad_rscfile = fopen(rscfilename,"r");
  fscanf(toolpad_rscfile,"Headpad Name:%s\n",headpad_name);
  fscanf(toolpad_rscfile,"Headpad Color:%hd %hd %hd\n",
	 &(headpadcolor[0]), &(headpadcolor[1]), &(headpadcolor[2]));
  fscanf(toolpad_rscfile,"Headpad Position:[%f,%f,%f]\n",
	 &(headpadpos[vx]), &(headpadpos[vy]), &(headpadpos[vz]));
  fscanf(toolpad_rscfile,"Headpad Icon File:%s\n",headpad_icon_name);
  fscanf(toolpad_rscfile,"Toolpad Color:%hd %hd %hd\n",
	 &(toolpadcolor[0]), &(toolpadcolor[1]), &(toolpadcolor[2]));
  fscanf(toolpad_rscfile,"Padhandle Color:%hd %hd %hd\n",
	 &(padhandlecolor[0]), &(padhandlecolor[1]), &(padhandlecolor[2]));
  fscanf(toolpad_rscfile,"Toolbar Color:%hd %hd %hd\n",
	 &(toolbarcolor[0]), &(toolbarcolor[1]), &(toolbarcolor[2]));

  fscanf(toolpad_rscfile,"Number of Toolpads:%d\n",&numpads);
  
  headpad = make_obj(world,origin);
  set_object_name(headpad,headpad_name);
  set_feature_color((featureptr) headpad, headpadcolor);
  setpos3d(pos,-Headpadsquare,Headpadsquare/2,10.0);
  thisle = First_obj_le(headpad);
  setpos3d(pos,Headpadsquare,Headpadsquare,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos, -Headpadsquare,-Headpadsquare,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  headpad->selectechnique = set_headpad_selectable;
  headpad->drawtechnique = draw_headpad_technique;
  add_property((featureptr) headpad, headpad_prop);
  add_property((featureptr) headpad, nonmanifold_prop);
  add_property((featureptr) headpad, noshadow_prop);
  setpos3d(pos,Headpadsquare, (Headpadsquare + 4.0 * Headpadcorner),0.0);
  translate_feature((featureptr) headpad,pos,FALSE);
  adopt_obj_branch(cutplane_obj,headpad,TRUE);

  read_data_file(headpad_icon_name,(featureptr) headpad);
  apply_front_decal(headpad->children->first.obj,
		    headpad,Decalthickness*2,Decalzoffset,0.0);
  kill_obj_branch(headpad->children->first.obj);

  read_decal_file(headpad,"./rsc/padhandle.dcl");
  padhandle_orig = headpad->children->first.obj;
  adopt_obj_branch(world->root,padhandle_orig,TRUE);

  read_decal_file(headpad,"./rsc/toolpad.dcl");
  toolpad_orig = headpad->children->first.obj;
  adopt_obj_branch(world->root,toolpad_orig,TRUE);
  
  read_decal_file(headpad,"./rsc/toolbar.dcl");
  toolbar = headpad->children->first.obj;
  adopt_obj_branch(headpad,toolbar,FALSE);
  toolbar->selectechnique = set_normalpad_selectable;
  toolbar->drawtechnique = draw_pad_technique;
  add_property((featureptr) toolbar,dragonly_prop);
  add_property((featureptr) toolbar,toolbar_prop); /* for padhandle.cpl */

  for (i = 0; i < numpads; ++i)
  {
    padhandle = duplicate_obj_branch(padhandle_orig);

    padhandle->selectechnique = set_normalpad_selectable;
    padhandle->drawtechnique = draw_pad_technique;
    add_property((featureptr) padhandle, padhandle_prop);
    add_property((featureptr) padhandle, nonmanifold_prop);
    add_property((featureptr) padhandle, noshadow_prop);
    del_property((featureptr) padhandle,visible_prop);
    adopt_obj_branch(toolbar, padhandle,TRUE);

    newpad = duplicate_obj_branch(toolpad_orig);

    newpad->selectechnique = set_normalpad_selectable;
    newpad->drawtechnique = draw_pad_technique;
    add_property((featureptr) newpad, toolpad_prop);
    add_property((featureptr) newpad, nonmanifold_prop);
    add_property((featureptr) newpad, noshadow_prop);
    del_property((featureptr) newpad, visible_prop);
    adopt_obj_branch(padhandle, newpad,FALSE);

    fscanf(toolpad_rscfile,"%s\n",toolpad_obj_filename);
    read_data_file(toolpad_obj_filename,(featureptr) newpad);
    newtool = newpad->children->first.obj;
    apply_front_decal(newtool,newpad,Decalthickness, Decalzoffset,0.0);
    del_property_from_descendants((featureptr) newtool,visible_prop);  
  }

  toolbar = adjust_toolbar(toolbar); /* replaces old toolbar with adjusted one */

  add_property_to_descendants((featureptr) headpad, lockedgeometry_prop);
  add_property((featureptr) headpad,visible_prop);
  translate_feature((featureptr) headpad,headpadpos,FALSE);

  del_property((featureptr) toolbar,visible_prop);

  kill_obj_branch(padhandle_orig);
  kill_obj_branch(toolpad_orig);

  return(headpad);
}

/* if you pass 0.0 for the thickness it does not extrude it. */

  objptr
create_arrow(worldptr world,GLfloat width,GLfloat height,GLfloat thickness,
	     GLfloat stem_width,GLfloat stem_to_arrowhead_ratio)
{
  objptr arrow;
  vertype pos;
  ;
  
  return(arrow);
}


/* This has a bug in that the CPL code that colors the chip has to get to it */
/* from any bar: with messages, we could get to the chip by name and send a */
/* message to it to color itself? or just name it to change its color. */

  objptr
create_color_control(worldptr world, vertype controlcorner,
		     GLfloat colorbasewidth)
		     
{
  objptr base,redbar,greenbar,bluebar,chip;
  fveptr barctrlface;
  vertype pos;
  GLfloat colorbaseheight, colorbasedepth;
  GLfloat colorbarwidth,colorbarheight,colorbardepth,chipdepth;
  ;
  colorbaseheight = colorbasewidth / 3.0;
  colorbasedepth = colorbasewidth / 2.0;
  colorbarwidth = (colorbasewidth / 3.0) - Pkboxsize;
  colorbarheight = colorbasewidth * 0.8;
  colorbardepth = colorbasedepth;
  chipdepth = 5.0;

  base = create_cube(world,colorbasewidth + Pkboxsize,
		     colorbaseheight,colorbasedepth);
  setpos3d(pos,-Pkboxsize,0.0,0.0);
  translate_feature((featureptr) base,pos,FALSE);

  chip = create_cube(world,colorbaseheight + Pkboxsize,
		     sqrt(colorbasedepth * colorbasedepth +
			  colorbaseheight * colorbaseheight),Chipdepth);
  axis_rot_obj(chip,x_axis,origin,-450);
  setpos3d(pos,colorbasewidth,0.0,colorbasedepth);
  translate_feature((featureptr) chip, pos, FALSE);
  set_feature_color((featureptr) chip, white);
  add_property((featureptr) chip, paint_prop);
  set_object_name(chip,"Paintchip");
  adopt_obj_branch(base,chip,TRUE);

  redbar = create_cube(world,colorbarwidth,colorbarheight,colorbardepth);
  setpos3d(pos,0.0,colorbaseheight,0.0);
  translate_feature((featureptr) redbar, pos, FALSE);
  set_feature_color((featureptr) redbar,red);
  barctrlface = find_face_given_normal(redbar,y_axis);
  add_property((featureptr) barctrlface,hotspot_prop);
  add_property((featureptr) redbar,colorbar_prop);
  adopt_obj_branch(base,redbar,TRUE);

  greenbar = create_cube(world,colorbarwidth,colorbarheight,colorbardepth);
  setpos3d(pos,colorbarwidth + Pkboxsize,colorbaseheight,0.0);
  translate_feature((featureptr) greenbar, pos, FALSE);
  set_feature_color((featureptr) greenbar,green);
  barctrlface = find_face_given_normal(greenbar,y_axis);
  add_property((featureptr) barctrlface,hotspot_prop);
  add_property((featureptr) greenbar,colorbar_prop);
  adopt_obj_branch(base,greenbar,TRUE);

  bluebar = create_cube(world,colorbarwidth,colorbarheight,colorbardepth);
  setpos3d(pos,(2 * (colorbarwidth + Pkboxsize)),colorbaseheight,0.0);
  translate_feature((featureptr) bluebar, pos, FALSE);
  set_feature_color((featureptr) bluebar,blue);
  barctrlface = find_face_given_normal(bluebar,y_axis);
  add_property((featureptr) barctrlface,hotspot_prop);
  add_property((featureptr) bluebar,colorbar_prop);
  adopt_obj_branch(base,bluebar,TRUE);
  
  add_property((featureptr) base,goodies_prop);
  add_property_to_descendants((featureptr) base,lockedgeometry_prop);
  translate_feature((featureptr) base, controlcorner,FALSE);

  return(base);
}


/*  Deformation Control Lattice Creation routines */

  void
make_lattice_face(startle,stople,numpts,pt1,pt2,pt3)
  leptr startle,stople;
  int numpts;
  vertype pt1,pt2,pt3;
{
  float *pts[3];
  leptr newle;
  int i;
  ;
  pts[0] = pt1; pts[1] = pt2; pts[2] = pt3;
  newle = startle;
  for (i=0; i<numpts; i++)
    newle = make_edge_vert(newle,newle,pts[i]);
  make_edge_face(newle,stople);
}

  void
set_controlpts(controlpts)
  vertype controlpts[];
{
  float *X0;
  vertype maxpt;
  vertype diff;
  int i,j,k,l = 0;
  ;
  X0 = ppiped[0];
  addvec3d(ppiped[1],X0,maxpt);
  addvec3d(ppiped[2],maxpt,maxpt);
  addvec3d(ppiped[3],maxpt,maxpt);
  diff[vx] = (maxpt[vx] - X0[vx]) / (Ncontrolpts - 1);
  diff[vy] = (maxpt[vy] - X0[vy]) / (Ncontrolpts - 1);
  diff[vz] = (maxpt[vz] - X0[vz]) / (Ncontrolpts - 1);
  /* y-z plane through X0 */
  for (j=0; j < Ncontrolpts; ++j) /* step in y direction */
    for (k=0; k < Ncontrolpts; ++k) /* step in z direction */
    {
      controlpts[l][vx] = X0[vx];
      controlpts[l][vy] = X0[vy] + j * diff[vy];
      controlpts[l++][vz] = X0[vz] + k * diff[vz];
    }
  /* x-z plane through maxpt */
  for (i=1; i < Ncontrolpts; ++i) /* step in x direction */
    for (k=0; k < Ncontrolpts; ++k) /* step in z direction */
    {
      controlpts[l][vx] = X0[vx] + i * diff[vx];
      controlpts[l][vy] = maxpt[vy];
      controlpts[l++][vz] = X0[vz] + k * diff[vz];
    }
  /* y-z plane throuch maxpt */
  for (j = Ncontrolpts-2; j >= 0; --j) /* step in y direction */
    for (k=0; k < Ncontrolpts; ++k) /* step in z direction */
    {
      controlpts[l][vx] = maxpt[vx];
      controlpts[l][vy] = X0[vy] + j * diff[vy];
      controlpts[l++][vz] = X0[vz] + k * diff[vz];
    }
  /* x-z plane through X0 */
  for (i = Ncontrolpts-2; i > 0; --i) /* step in x direction */
    for (k=0; k < Ncontrolpts; ++k) /* step in z direction */
    {
      controlpts[l][vx] = X0[vx] + i * diff[vx];
      controlpts[l][vy] = X0[vy];
      controlpts[l++][vz] = X0[vz] + k * diff[vz];
    }
  /* x-y planes */
  for (k=0; k < Ncontrolpts; k+=(Ncontrolpts-1)) /* step in z direction */
    for (j=1; j < (Ncontrolpts-1); ++j) /* step in y direction */
      for (i=1; i < (Ncontrolpts-1); ++i) /* step in x direction */
      {
        controlpts[l][vx] = X0[vx] + i * diff[vx];
        controlpts[l][vy] = X0[vy] + j * diff[vy];
        controlpts[l++][vz] = X0[vz] + k * diff[vz];
      }
}

  objptr
create_lattice(worldptr world)
{
  vertype controlpts[Ncontrolpts*Ncontrolpts*6];
  leptr firstle,thisle,temple,rowstartle;
  fveptr firstfve,newfve;
  objptr lattice;
  int i;
  ;
  set_controlpts(controlpts);
  lattice = make_obj(world,controlpts[0]);

  add_property((featureptr) lattice,deform_prop);
  add_property((featureptr) lattice,transparent_prop);

  firstfve = First_obj_fve(lattice);
  thisle = firstle = firstfve->boundlist->first.bnd->lelist->first.le;
  /* first row */
  make_lattice_face(firstle,firstle,3,
		    controlpts[1],controlpts[5],controlpts[4]);
  thisle = firstle->next;
  make_lattice_face(thisle,thisle->next,2,
		    controlpts[2],controlpts[6],Nil);
  thisle = firstle->next->next;
  make_lattice_face(thisle,thisle->next,2,
		    controlpts[3],controlpts[7],Nil);
  rowstartle = firstle->prev;
  for (i=0; i<10; ++i)
  {
    thisle = rowstartle;
    make_lattice_face(thisle,thisle->prev,2,
		      controlpts[8+(i*4)],controlpts[9+(i*4)],Nil);
    thisle = rowstartle->prev->prev;
    make_lattice_face(thisle,thisle->prev->prev,1,controlpts[10+(i*4)],Nil,Nil);
    thisle = rowstartle->prev->prev->prev;
    make_lattice_face(thisle,thisle->prev->prev,1,controlpts[11+(i*4)],Nil,Nil);
    rowstartle = rowstartle->prev;
  }
  /* last row */
  temple = rowstartle->prev;
  make_edge_face(rowstartle,firstle); /* rowstartle is in the new face */
  for (i=0; i<3; ++i)
  {
    thisle = temple;
    temple = thisle->prev;
    firstle = firstle->next;
    make_edge_face(thisle,firstle);
  }
  /* side face 1 */
  /* 1st row */
  rowstartle = rowstartle->prev;
  temple = rowstartle;
  rowstartle = rowstartle->prev->prev;
  thisle = temple->prev;
  temple = temple->next;
  make_lattice_face(thisle,thisle->next->next,1,controlpts[48],Nil,Nil);
  thisle = temple->prev;
  temple = temple->next;
  make_lattice_face(thisle,thisle->next->next,1,controlpts[49],Nil,Nil);
  thisle = temple->prev;
  make_edge_face(thisle,thisle->next->next->next);
  /* 2nd row */
  temple = rowstartle->next;
  rowstartle = rowstartle->prev;
  thisle = temple->prev;
  temple = temple->next;
  make_lattice_face(thisle,thisle->next->next,1,controlpts[50],Nil,Nil);
  thisle = temple->prev;
  temple = temple->next;
  make_lattice_face(thisle,thisle->next->next,1,controlpts[51],Nil,Nil);
  thisle = temple->prev;
  make_edge_face(thisle,thisle->next->next->next);
  /* 3rd row */
  temple = rowstartle->next->next->next;
  make_edge_face(rowstartle->prev,rowstartle->next->next);
  make_edge_face(temple,temple->next->next->next);

  /* side face 2 */
  /* 1st row */
  temple = firstle->next;
  firstle = firstle->prev->prev;
  thisle = temple;
  make_lattice_face(thisle,thisle->prev->prev,1,controlpts[52],Nil,Nil);
  thisle = temple->next;
  temple = temple->next->next;
  make_lattice_face(thisle,thisle->prev->prev,1,controlpts[54],Nil,Nil);
  thisle = temple->next;
  make_edge_face(thisle,thisle->prev->prev->prev);
  /* 2nd row */
  firstle = firstle->next;
  temple = firstle->next;
  firstle = firstle->prev->prev;
  thisle = temple;
  make_lattice_face(thisle,thisle->prev->prev,1,controlpts[53],Nil,Nil);
  thisle = temple->next;
  temple = temple->next->next;
  make_lattice_face(thisle,thisle->prev->prev,1,controlpts[55],Nil,Nil);
  thisle = temple->next;
  make_edge_face(thisle,thisle->prev->prev->prev);
  /* 3rd row */
  temple = firstle->prev;
  firstle = firstle->next->next;
  make_edge_face(temple,firstle);
  make_edge_face(firstle->next,firstle->prev->prev);

  return(lattice);
}



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*               toolbox routines using CPL December 16 1990            */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

  objptr
create_toolbox(worldptr world, objptr decal_obj,
	       float leafratio,GLfloat width,GLfloat height,GLfloat depth)
{
  objptr toolbox,leaf;
  vertype pos;
  static colortype toolboxcolor = {50,50,200};
  vertype leafpos[4];
  GLfloat leafwidth[4];
  int i;
  char leafname[20];
  ;
  toolbox = create_cube(world,width,height,depth);
  set_feature_color((featureptr) toolbox,toolboxcolor);


  apply_front_decal(decal_obj, toolbox, Decalthickness*2, Decalzoffset,
		    width/10.0/*0.0*/);
    
  setpos3d(leafpos[0],0.0, height - 1.0,0.0);
  setpos3d(leafpos[1],0.0,height - 1.0, depth);
  setpos3d(leafpos[2],0.0, height - 1.0,depth);
  setpos3d(leafpos[3],width, height - 1.0,depth);
  leafwidth[0] = leafwidth[1] = width;
  leafwidth[2] = leafwidth[3] = depth;

  for (i = 0; i < 4; ++i)
  {
    leaf = create_square(world,leafwidth[i],height * leafratio);

    sprintf(leafname, "Leaf%d", i);
    set_object_name(leaf,leafname);

    set_feature_color((featureptr) leaf,toolboxcolor);
    if (i == 0)
      axis_rot_obj(leaf,x_axis,origin,-Toolboxleafrot);
    else if (i == 1)
      axis_rot_obj(leaf,x_axis,origin,Toolboxleafrot);
    else if (i == 2)
    {
      axis_rot_obj(leaf,y_axis,origin,900);
      axis_rot_obj(leaf,z_axis,origin,Toolboxleafrot);
    }
    else
    {
      axis_rot_obj(leaf,y_axis,origin,900);
      axis_rot_obj(leaf,z_axis,origin,-Toolboxleafrot);
    }
    translate_feature((featureptr) leaf,leafpos[i],Usetranslateflags);
    transform_obj_tree(leaf);
    find_branch_normals(leaf);
    add_property((featureptr) leaf,nonmanifold_prop);
    add_property((featureptr) leaf,hideable_prop);
    del_property((featureptr) leaf,visible_prop);
    adopt_obj_branch(toolbox,leaf,FALSE);
  }

  add_property((featureptr) toolbox, lockedgeometry_prop);
  add_property((featureptr) toolbox, toolbox_prop);
  return(toolbox);
}

  objptr
setup_prims_toolbox(worldptr world)
{
  objptr toolbox,cube,cylinder,cone,sphere,pyramid;
  objptr lbeam_decal,triangle_decal,cube_decal;
  vertype offset,rotvec,rotpt;
  int i = 0;
  ;
  cube_decal = create_cube(world,Toolboxwidth,Toolboxheight,Toolboxdepth);
  set_feature_color((featureptr) cube_decal, orange);

  triangle_decal = create_isosceles_prism(world,Toolboxwidth/2.0,Toolboxdepth);
  set_feature_color((featureptr) triangle_decal, green);
  setpos3d(offset,3*Toolboxwidth/4.0,3*Toolboxheight/4.0,Toolboxwidth);
  translate_feature((featureptr) triangle_decal, offset,FALSE);
  adopt_obj_branch(cube_decal,triangle_decal,FALSE);

  lbeam_decal = create_lbeam(world,Toolboxwidth,Toolboxheight, Toolboxwidth/2.0,
			     Toolboxdepth);
  set_feature_color((featureptr) lbeam_decal, yellow);
  setpos3d(offset,Toolboxwidth,Toolboxheight,Toolboxwidth*2);
  translate_feature((featureptr) lbeam_decal, offset,FALSE);
  adopt_obj_branch(cube_decal,lbeam_decal,FALSE);
#if 0
  /* Tilt'em up to catch the light... not any longer me boy*/
  axis_rot_obj(cube_decal,x_axis,origin,-120);
#endif
  toolbox = create_toolbox(world, cube_decal,
			   Toolboxleaf,Toolboxwidth,
			   Toolboxheight,Toolboxdepth);
  set_object_name(toolbox,"Primsbox");

/*  write_data_file("./rsc/primdecal.cut",(featureptr) cube_decal);*/
  /* kill the decal source objects */
  kill_obj_branch(cube_decal);
  
  cube = create_cube(world,Primswidth,Primsheight * 0.75,Primsdepth);
  del_property((featureptr) cube, visible_prop);
  add_property((featureptr) cube, stamp_prop);
  add_property((featureptr) cube, hideable_prop);
  add_property((featureptr) cube, lockedgeometry_prop);
  adopt_obj_branch(toolbox,cube,TRUE);

  cylinder = create_oval_cylinder(world,Primswidth/2.0,Primsheight/2.0,
				  Primsdepth, Primsresolution);
  del_property((featureptr) cylinder, visible_prop);
  add_property((featureptr) cylinder, stamp_prop);
  add_property((featureptr) cylinder, hideable_prop);
  add_property((featureptr) cylinder, lockedgeometry_prop);
  adopt_obj_branch(toolbox,cylinder,TRUE);

  cone = create_oval_cone(world,Primswidth/2.0,Primsheight/2.0,Primsdepth,
			  Primsresolution);
  del_property((featureptr) cone, visible_prop);
  add_property((featureptr) cone, stamp_prop);
  add_property((featureptr) cone, hideable_prop);
  add_property((featureptr) cone, lockedgeometry_prop);
  adopt_obj_branch(toolbox,cone,TRUE);

  sphere = create_sphere(world,Primswidth/2,Primsresolution);
  del_property((featureptr) sphere, visible_prop);
  add_property((featureptr) sphere, stamp_prop);
  add_property((featureptr) sphere, hideable_prop);
  add_property((featureptr) sphere, lockedgeometry_prop);
  adopt_obj_branch(toolbox,sphere,TRUE);

  pyramid = create_pyramid(world,Primswidth*1.5,Primsheight*1.5,Primsdepth*1.5);
  del_property((featureptr) pyramid, visible_prop);
  add_property((featureptr) pyramid, stamp_prop);
  add_property((featureptr) pyramid, hideable_prop);
  add_property((featureptr) pyramid, lockedgeometry_prop);
  adopt_obj_branch(toolbox,pyramid,TRUE);

  setpos3d(offset,Maxcrossx - Toolboxmargin * 4.5,Mincrossy + Toolboxmargin / 3,
	  Minplanez - Toolboxmargin/4);
  translate_feature((featureptr) toolbox,offset,FALSE);

  return(toolbox);
}

  objptr
setup_goodies_toolbox(worldptr world)
{
  objptr toolbox,camera,colorctrl,lightsource;
  vertype offset;
  ;
  camera = create_nikon(world,110.0,70.0,45.0,50.0,
			13,magenta,lightgrey,orange,yellow);
  del_property_from_descendants((featureptr) camera,visible_prop);  
  add_property_to_descendants((featureptr) camera, hideable_prop);

  toolbox = create_toolbox(world, camera,
			   Toolboxleaf,Toolboxwidth,
			   Toolboxheight,Toolboxdepth);
  set_object_name(toolbox,"Goodiesbox");
  adopt_obj_branch(toolbox,camera,TRUE);

/*  write_data_file("./rsc/goodiesdecal.cut",(featureptr) camera);*/

  /*lightsource = create_spotlight(world,60.0,80.0,yellow,lightblue);*/
  lightsource = create_lightbulb(world,darkyellow,lightgrey);
  del_property_from_descendants((featureptr) lightsource,visible_prop);  
  add_property_to_descendants((featureptr) lightsource, hideable_prop);
  adopt_obj_branch(toolbox,lightsource,TRUE);

  colorctrl = create_color_control(world,origin, 120.0);
  del_property_from_descendants((featureptr) colorctrl, visible_prop);
  add_property_to_descendants((featureptr) colorctrl, hideable_prop);
  adopt_obj_branch(toolbox,colorctrl,TRUE);
  
  setpos3d(offset,Maxcrossx - Toolboxmargin * 2.5,Mincrossy + Toolboxmargin/3,
	  Minplanez/* - Toolboxmargin/4*/);
  translate_feature((featureptr) toolbox,offset,FALSE);
  return(toolbox);
}

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*          Demo for stockbrokers                                         */
/* You must call the updating stuff from main.c by defining Stocksdemo    */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

  objptr
setup_stocks_demo(worldptr world,GLfloat barwidth,GLfloat barheight,GLfloat bardepth,
		  int numstocks, int timedepth)
{
  objptr stockbase,bar,stock_plate,stocks_pad,stockbox;
  GLfloat stockbasewidth,stockbasedepth,stockmargin;
  int i,j;
  vertype offset;
  GLfloat stockiconmargin = Storagesquare / 16.0;
  leptr thisle;
  vertype pos;
  static colortype newcolor = {0,0,255};
  ;
  stocks_pad = make_obj(environment_world,origin);
  set_object_name(stocks_pad,"StocksPad");
  add_property((featureptr) stocks_pad,nonmanifold_prop);
  add_property((featureptr) stocks_pad,noshadow_prop);
  add_property((featureptr) stocks_pad,dialog_prop);
  thisle = First_obj_le(stocks_pad);
  setpos3d(pos,Storagesquare,Storagesquare,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  set_feature_color((featureptr) stocks_pad,orange);
  stocks_pad->selectechnique = set_normalpad_selectable;
  stocks_pad->drawtechnique = draw_plate_technique;


#ifdef IRIS
  srandom(32751);		/* some number i just made up ! */
#endif
  stockbasewidth = barwidth * numstocks;
  stockbasedepth = bardepth * timedepth;
  stockmargin = Pkboxsize * 2.0;
  current_numstocks = numstocks;
  current_stockhistory = timedepth;
  current_maxbarheight = barheight * 2;

  stock_plate = read_plate(world, stocks_pad, "./rsc/stocksplate.rsc");
  stockbox = stock_plate->children->first.obj;
  
  stockbase = create_cube(world,stockbasewidth * 1.5 + stockmargin * numstocks,
			  barheight/5.0, stockbasedepth * 1.5);
  setpos3d(offset,-stockbasewidth * 0.25 - stockmargin * numstocks * 0.5,
	   0.0,-stockbasewidth * 0.25 - stockmargin * numstocks * 0.25);
  translate_feature((featureptr) stockbase,offset,FALSE);
  set_feature_color((featureptr) stockbase,magenta);
  for (i = 0; i < numstocks; ++i)
  {
    for (j = 0; j < timedepth; ++j)
    {
      bar = create_cube(world,barwidth,barheight,bardepth);
      set_feature_color((featureptr) bar,newcolor);
      setpos3d(offset,i * (barwidth + stockmargin),barheight/5.0,bardepth * j);
      translate_feature((featureptr) bar,offset,FALSE);
      adopt_obj_branch(stockbase,bar,TRUE);
      stockbars[i][j] = bar;
      stocktops[i][j] = find_face_given_normal(bar,y_axis);
    }
    newcolor[vx] += 20;
    if (stockbox != Nil)
    {
      set_property((featureptr) stockbars[i][0],connection_prop,stockbox);
      stockbox = stockbox->next;
    }
  }
  
  add_property_to_descendants((featureptr) stockbase, lockedgeometry_prop);
  del_property_from_descendants((featureptr) stockbase,visible_prop);
  set_property((featureptr) stocks_pad,connection_prop,stockbase);


  axis_rot_obj(stockbase,y_axis,origin,1800);
  axis_rot_obj(stockbase,x_axis,origin,100);

  /* move pad to final position */
  setpos3d(pos,Mincrossx + Storagesquare*4.5,
	   Mincrossy + Storagesquare * 0.5,Minplanez);
  translate_feature((featureptr) stocks_pad,pos,FALSE);

  /* unadopt plate cause apply_front_decal will die trying to normalize it */
  /* if it's a child of the piechart*/
  adopt_obj_branch(world->root,stock_plate,TRUE);
  apply_front_decal(stockbase,stocks_pad,Decalthickness*3,Decalzoffset,
		    stockiconmargin);
  /* now readopt */
  adopt_obj_branch(stocks_pad,stock_plate,FALSE);
  
  return(stockbase);
}
  
  void
update_stocks_demo()
{
  bbox3dptr source_bbox,dest_bbox;
  objptr source_stock,dest_stock,showheight;
//   long scalartn;
  int32_t scalartn;		// CHECK ME!!! LJE
  GLfloat scalar,scale_amt,height;
  vertype offset;
  char outstr[50];
  colortype newcolor;
  int i,j;
  static int when = 0;
  fveptr topface1,topface2;	/* kinda a hack but scale seems to be fucked */
  vfeptr topvert1,topvert2;
  ;
  if (!has_property((featureptr) stockbars[0][0]->parent,visible_prop))
    return;
  when++;
  if (when % 10 == 0)
  {
    when = 0;
    /* update histories */
    for (j = current_stockhistory - 1; j > 0; --j)
      for (i = 0; i < current_numstocks; ++i)
      {
	source_stock = stockbars[i][j - 1];
	source_bbox = &(source_stock->objbbox);
	dest_stock = stockbars[i][j];
	dest_bbox = &(dest_stock->objbbox);
	topface1 = stocktops[i][j - 1];
	if (topface1->boundlist != Nil)
	{
	  topvert1 = topface1->boundlist->first.bnd->lelist->first.le->facevert;
	  topface2 = stocktops[i][j];
	  topvert2 = topface2->boundlist->first.bnd->lelist->first.le->facevert;
	  setpos3d(offset,0.0,topvert1->pos[vy] - topvert2->pos[vy],0.0);
	  translate_feature((featureptr) topface2,offset,FALSE);
	  unlog_updates((featureptr) topface2);
	  log_update((featureptr) topface2,(Cutevfsflag |
	     Makecutsectionflag | Computebboxflag), Nonimmediateupdate);

	  get_feature_color((featureptr) source_stock,newcolor);
	  set_feature_color((featureptr) dest_stock, newcolor);
	}
      }
    /* create a stock future */
    for (i = 0; i < current_numstocks; ++i)
    {
#ifdef IRIS
      scalartn = random();
#endif
      scalar = ((GLfloat) scalartn / INT_MAX) * 50.0 - 25; /* -25 < x < 25 */
      source_stock = stockbars[i][0];
      source_bbox = &(source_stock->objbbox);
      topface1 = stocktops[i][0];
      showheight =
	(objptr) get_property_val((featureptr) topface1->mothershell->motherobj,
			      connection_prop).l;
      if (showheight != Nil)
      {
	height = source_bbox->maxcorner[vy] - source_bbox->mincorner[vy];
	sprintf(outstr,"%2.2f",height);
	set_label_item(showheight,1,outstr);
	log_update((featureptr) showheight, Logerasebboxflag,
		   Nonimmediateupdate);
      }
      if (topface1->boundlist != Nil)
      {
	topvert1 = topface1->boundlist->first.bnd->lelist->first.le->facevert;
	if ((topvert1->pos[vy] + scalar <=
	     source_bbox->mincorner[vy] + current_maxbarheight) &&
	    (topvert1->pos[vy] + scalar >
	     source_bbox->mincorner[vy] + current_maxbarheight / 2.0))
	{
	  setpos3d(offset,0.0,scalar,0.0);
	  translate_feature((featureptr) topface1,offset,FALSE);
	  unlog_updates((featureptr) topface1);
	  log_update((featureptr) topface1,(Cutevfsflag |
	     Makecutsectionflag | Computebboxflag), Nonimmediateupdate);
	}
      }
    }  
  }
}

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*               Pie Charts Demo                                          */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/* startpercent, endpercent are the begin and end of the slice (of 360) */
/* resolution is the number of segs that are used to approx 360 degrees  */

  objptr
create_pieslice(worldptr world,GLfloat radius, GLfloat depth,
		float angleinc,int thiseg,int addsegs)
{
  objptr slice;
  vertype pos;
  int i = 0,numsegs;
  float totalpercent,angoffset;
  float startangle;
  leptr firstle,thisle;
  ;
  slice = make_obj(world,origin);
  firstle = thisle = First_obj_le(slice);
  
/*
  totalpercent = endpercent-startpercent;
  numsegs = (totalpercent * resolution);
  angoffset = ((int) ((startpercent * 2.0 * M_PI) / angleinc)) * angleinc;
  while (i <= numsegs)
  {
    pos[vx] = cos((double) i * angleinc + angoffset) * radius;
    pos[vy] = sin((double) i * angleinc + angoffset) * radius;
    thisle = make_edge_vert(thisle,thisle,pos);
    i++;
  }
*/
  startangle = angleinc * thiseg;
  pos[vz] = 0.0; pos[vw] = 1.0;
  while (i <= addsegs)
  {
    pos[vx] = cos((double) i * angleinc + startangle) * radius;
    pos[vy] = sin((double) i * angleinc + startangle) * radius;
    thisle = make_edge_vert(thisle,thisle,pos);
    i++;
  }
  make_edge_face(thisle,firstle);
  setpos3d(pos,0.0,0.0,depth);
  extrude_face(First_obj_fve(slice),pos);

  return(slice);
}

  objptr
create_piechart(worldptr world, GLfloat radius,GLfloat depth,
		    int resolution,objptr chart_plate)
{
  int i, numslices;
  objptr base,slice,piechart,this_name_box,this_percentage_box;
  float percentperslice,d1,angleinc;
  vertype pos;
  float qty;
  char *newvalstr;
  float startpercent, endpercent;
  int thiseg = 0,addsegs;
  CPL_word_ptr thepiecode;
  ;
  piechart = base = create_oval_cylinder(world,radius,radius,
					 depth/4,resolution);
  set_object_name(piechart,"Piechart");
  setpos3d(pos,0.0,0.0,-depth/4.0);
  translate_feature((featureptr) base,pos,FALSE);
  /* Assume that each slice has both a name plate and a percentage plate. */
  numslices = chart_plate->children->numelems/2;
/*
  d1 = (float) resolution / (float) numslices;
  if ((d1 * (float) numslices) !=
      (int) (d1 * numslices))
    resolution =  (int) d1;
*/

  this_name_box = chart_plate->children->first.obj;
  this_percentage_box = this_name_box->next;
  add_property((featureptr) base,lockedgeometry_prop);
  percentperslice = (100.0 / (float) numslices) / 100.0;
  angleinc = (360.0 / resolution) * Deg2rad;
  thepiecode = this_percentage_box->privatecode;
  for (i = 0; i < numslices; ++i)
  {
    if (this_name_box != Nil)
    {
      newvalstr = get_label_item(this_percentage_box,1);
      sscanf(newvalstr,"%f",&qty);
    }
    else
      qty = percentperslice;

    addsegs = iround((qty / 100.0) * (float) resolution);
    slice = create_pieslice(world,radius,depth,
			    angleinc,thiseg,addsegs);
    thiseg += addsegs;
    add_property((featureptr) slice,lockedgeometry_prop);
    adopt_obj_branch(piechart,slice,TRUE);
    if ((this_name_box == Nil) || (this_percentage_box == Nil))
      break;
    /* link a piece of pie to a text box */
    set_property((featureptr) this_percentage_box,connection_prop,slice);
    set_object_name(slice,get_label_item(this_name_box,1));
    set_feature_color((featureptr) slice, this_percentage_box->thecolor);
    this_name_box = this_name_box->next->next;
    if (this_name_box != Nil)
    {
      this_percentage_box = this_name_box->next;
      this_percentage_box->privatecode = thepiecode;
    }
  }

  setpos3d(pos,Maxcrossx - 60.0,0.0,0.0);
  translate_feature((featureptr) base,pos,FALSE);
  
  return(piechart);
}

  objptr
setup_piechart_demo(worldptr world, GLfloat radius,GLfloat depth,
		    int resolution,colortype piechartpadcolor)
{
  objptr piechart_pad,piechart,chart_plate;
  GLfloat piechartmargin = Storagesquare / 8.0;
  leptr thisle;
  vertype pos;
  ;
  piechart_pad = make_obj(world,origin);
  set_object_name(piechart_pad,"PiechartPad");
  add_property((featureptr) piechart_pad,nonmanifold_prop);
  add_property((featureptr) piechart_pad,noshadow_prop);
  thisle = First_obj_le(piechart_pad);
  setpos3d(pos,Storagesquare,Storagesquare,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  set_feature_color((featureptr) piechart_pad,piechartpadcolor);
  piechart_pad->selectechnique = set_normalpad_selectable;
  piechart_pad->drawtechnique = draw_plate_technique;

  chart_plate = read_plate(world, piechart_pad, "./rsc/piechartplate.rsc");
  piechart = create_piechart(world,radius,depth,resolution,chart_plate);

  /* unadopt plate cause apply_front_decal will die trying to normalize it */
  /* if it's a child of the piechart*/
  adopt_obj_branch(world->root,chart_plate,TRUE);
  apply_front_decal(piechart,piechart_pad,Decalthickness,Decalzoffset,
		    piechartmargin);
  /* now readopt */
  adopt_obj_branch(piechart_pad,chart_plate,FALSE);

  add_property((featureptr) piechart_pad,dialog_prop);
/*  adopt_obj_branch(cutplane_obj,piechart_pad,TRUE);*/
  del_property_from_descendants((featureptr) piechart,visible_prop);
  set_property((featureptr) piechart_pad,connection_prop,piechart);
  adopt_obj_branch(primary_world->root,piechart,TRUE);

/*  axis_rot_obj(piechart,x_axis,origin,-150);*/

  /* move pad to final position */
  setpos3d(pos,Mincrossx + Storagesquare*6.5,
	   Mincrossy + Storagesquare * 0.5,Minplanez);
  translate_feature((featureptr) piechart_pad,pos,FALSE);

  return(piechart);
}

  objptr
create_slide(worldptr world)
{
  objptr slide,film;
  vertype pos;
  static colortype filmcolor = {0,0,50};
  ;
  slide = create_cube(world,100.0,100.0,50.0);
  set_feature_color((featureptr) slide,white);
  film = create_cube(world,75.0,65.0,20.0);
  set_feature_color((featureptr) film, filmcolor);
  setpos3d(pos,12.5,17.5,60.0);
  translate_feature((featureptr) film,pos,FALSE);
  adopt_obj_branch(slide,film,FALSE);

  return(slide);
}

  objptr
setup_slide_pad(worldptr world)
{
  objptr slide_pad,slide;
  GLfloat slidemargin = Storagesquare / 8.0;
  leptr thisle;
  vertype pos;
  ;
  slide_pad = make_obj(world,origin);
  set_object_name(slide_pad,"SlidePad");
  add_property((featureptr) slide_pad,nonmanifold_prop);
  add_property((featureptr) slide_pad,noshadow_prop);
  thisle = First_obj_le(slide_pad);
  setpos3d(pos,Storagesquare,Storagesquare,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  set_feature_color((featureptr) slide_pad,orange);
  slide_pad->selectechnique = set_normalpad_selectable;
  slide_pad->drawtechnique = draw_plate_technique;

  slide = create_slide(world);

  apply_front_decal(slide,slide_pad,Decalthickness,Decalzoffset, slidemargin);
  kill_obj_branch(slide);

  read_CPL_privatecode("./cpl/slide.cpl",(featureptr) slide_pad);
  add_property((featureptr) slide_pad,dialog_prop);
/*  adopt_obj_branch(cutplane_obj,slide_pad,TRUE);*/

  /* move pad to final position */
  setpos3d(pos,Mincrossx + Storagesquare*8.5,
	   Mincrossy + Storagesquare * 0.5,Minplanez);
  translate_feature((featureptr) slide_pad,pos,FALSE);

  return(slide_pad);
}


  objptr
create_arrowtool(worldptr world)
{
  objptr arrow,film;
  vertype pos;
  static vertype arrowdata[7] = {{0.0,0.0,0.0},
			      {Arrowcursorwidth, -Arrowcursorheight*0.55,0.0},
			     {Arrowcursorstemwidth,
				-Arrowcursorheight * 0.5,0.0},
			     {Arrowcursorstemwidth, -Arrowcursorheight,0.0},
			     {-Arrowcursorstemwidth,
				-Arrowcursorheight,0.0},
			     {-Arrowcursorstemwidth,
				-Arrowcursorheight * 0.5,0.0},
			      {-Arrowcursorwidth,
				 -Arrowcursorheight*0.55,0.0}};
  static GLfloat depth = Arrowcursordepth;
  leptr firstle,thisle;
  int i;
  ;
  arrow = make_obj(world,origin);
  set_feature_color((featureptr) arrow,red);
  firstle = thisle = First_obj_le(arrow);
  i = 1;
  while (i < 7)
  {
    thisle = make_edge_vert(thisle,thisle,arrowdata[i]);
    i++;
  }
  make_edge_face(thisle,firstle);
  setpos3d(pos,0.0,0.0,depth);
  extrude_face(First_obj_fve(arrow),pos);
  axis_rot_obj(arrow,z_axis,origin,-350);

  return(arrow);
}

  objptr
setup_arrowtool_pad(worldptr world)
{
  objptr arrowtool_pad,arrowtool;
  GLfloat arrowtoolmargin = Storagesquare / 7.0;
  leptr thisle;
  vertype pos;
  ;
  arrowtool_pad = make_obj(world,origin);
  set_object_name(arrowtool_pad,"ArrowtoolPad");
  add_property((featureptr) arrowtool_pad,nonmanifold_prop);
  add_property((featureptr) arrowtool_pad,noshadow_prop);
  thisle = First_obj_le(arrowtool_pad);
  setpos3d(pos,Storagesquare,Storagesquare,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  set_feature_color((featureptr) arrowtool_pad,orange);
  arrowtool_pad->selectechnique = set_normalpad_selectable;
  arrowtool_pad->drawtechnique = draw_plate_technique;

  arrowtool = create_arrowtool(world);

  apply_front_decal(arrowtool,arrowtool_pad,Decalthickness,Decalzoffset,
		    arrowtoolmargin);
  kill_obj_branch(arrowtool);

  read_CPL_privatecode("./cpl/arrowtool.cpl",(featureptr) arrowtool_pad);
  add_property((featureptr) arrowtool_pad,dialog_prop);
/*  adopt_obj_branch(cutplane_obj,arrowtool_pad,TRUE);*/

  /* move pad to final position */
  setpos3d(pos,Mincrossx + Storagesquare*6.5,
	   Mincrossy + Storagesquare * 0.5,Minplanez);
  translate_feature((featureptr) arrowtool_pad,pos,FALSE);

  return(arrowtool_pad);
}


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*               Setup Tools Routine                                      */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/*
#define Colorcontrol
#define Buttonbar
#define Piechart
#define Slides
#define Arrowtool
#define Toolbox
#define Toolbar
*/

#define Primbox
#define Storagepad
#define Rotatetool
#define Arrangebar

  void
setup_tools(void)
{
  vertype pos;
  int use_rotatetool,use_primbox,use_goodies,use_tools,use_arrangers;
  int use_storagepad,use_measurepad,use_piechart,use_slidepad;
  int use_arrowpad;
  FILE *config_file;
  ;
  config_file = fopen("./rsc/tools.rsc","r");
  fscanf(config_file,"Rotatetool:%d\n",&use_rotatetool);
  fscanf(config_file,"Primitives Box:%d\n",&use_primbox);
  fscanf(config_file,"Goodies Box:%d\n",&use_goodies);
  fscanf(config_file,"Tool Bar:%d\n",&use_tools);
  fscanf(config_file,"Arrange Bar:%d\n",&use_arrangers);
  fscanf(config_file,"Storage Pad:%d\n",&use_storagepad);
  fscanf(config_file,"Measure Pad:%d\n",&use_measurepad);
  fscanf(config_file,"Piechart Pad:%d\n",&use_piechart);
  fscanf(config_file,"Slide Pad:%d\n",&use_slidepad);
  fscanf(config_file,"Arrow Pad:%d\n",&use_arrowpad);
  fclose(config_file);

  if (use_rotatetool)
    add_property_to_descendants((featureptr) rotatetool_obj,visible_prop);

#ifdef Colorcontrol
  create_color_control(environment_world,origin, 120.0);
#endif

  if (use_primbox)
  {
/*
  write_data_file("./rsc/primitives.cut",
		  (featureptr) setup_prims_toolbox(environment_world));
*/		  
    setup_prims_toolbox(environment_world);
  }

  if (use_goodies)
  {
/*
  write_data_file("./rsc/goodies.cut",
  (featureptr) setup_goodies_toolbox(environment_world));
*/
    setup_goodies_toolbox(environment_world);
  }

  if (use_tools)
    create_toolbar(environment_world,"./rsc/toolpads.rsc");

  if (use_arrangers)
    create_toolbar(environment_world,"./rsc/arrangepads.rsc");

#ifdef Buttonbar
  create_toolbar(environment_world,"./rsc/buttons.rsc");
#endif

#ifdef Viewbar
  create_toolbar(environment_world,"./rsc/viewpads.rsc");
#endif

  if (use_storagepad)
    create_storage_pad(environment_world,orange);

  if (use_measurepad)
    create_measure_pad(environment_world,orange);

  if (use_piechart)
    setup_piechart_demo(environment_world,100.0,50.0,36,orange);

  if (use_slidepad)
    setup_slide_pad(environment_world);

  if (use_arrowpad)
    setup_arrowtool_pad(environment_world);

  /* resolve any unknown object pointers */
  resolve_CPL_forward_object_refs();
}

