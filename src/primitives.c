
/* FILE: primitives.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*       Primitive GLuint Generation Routines for the Cutplane Editor     */
/*                                                                        */
/* Author: WAK                                   Date: September 20, 1989 */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define PRIMITIVESMODULE

#include <config.h>
#include <stdio.h>

#include <platform_gl.h>
#include <math.h>  /* for sin, cos calls */

#include <cutplane.h>
#include <tools.h>
#include <math3d.h>
#include <primitives.h>

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                               GLOBAL VARIABLES                           */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

prim_create_rec prim_create_recs[Numprimtypes] = {{create_cube,
						     "Cube_Plate"},
						  {create_oval_cylinder,
						     "Cylinder_Plate"},
						  {create_oval_cone,
						     "Cone_Plate"},
						  {create_sphere,
						     "Sphere_Plate"},
						  {create_pyramid,
						     "Pyramid_Plate"}};

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                            PRIM CREATION ROUTINES                        */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  objptr
create_isosceles_triangle(worldptr world, GLfloat trianglesize)
{
  objptr triangle;
  vertype pos;
  leptr firstle,thisle;
  ;
  setpos3d(pos, -trianglesize, -trianglesize, 0.0);
  triangle = make_obj(world,pos);
  firstle = thisle = First_obj_le(triangle);
  setpos3d(pos, trianglesize,-trianglesize, 0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos, 0.0, trianglesize,  0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  make_edge_face(thisle,firstle);
  
  return(triangle);
}

  objptr
create_isosceles_prism(worldptr world, GLfloat prismsize, GLfloat prismdepth)
{
  objptr prism;
  vertype depth;
  ;
  prism = create_isosceles_triangle(world,prismsize);
  setpos3d(depth,0.0,0.0,prismdepth);
  extrude_face(First_obj_fve(prism), depth);
  
  return(prism);
}

  objptr
create_cubehole(worldptr world)
{
  static vertype corners[16] = {{-100.0,100.0,100.0},
				{100.0,100.0,100.0},
				{100.0,-100.0,100.0},
				{-100.0,-100.0,100.0},
				{100.0,-100.0,-100.0},
				{100.0,100.0,-100.0},
				{-100.0,100.0,-100.0},
				{-100.0,-100.0,-100.0},
				{-50.0,50.0,100.0},
				{50.0,50.0,100.0},
				{50.0,-50.0,100.0},
				{-50.0,-50.0,100.0},
				{50.0,-50.0,-100.0},
				{50.0,50.0,-100.0},
				{-50.0,50.0,-100.0},
				{-50.0,-50.0,-100.0}};
  objptr newobj;
  fveptr firstfve,secondfve,thirdfve;
  leptr le1,newvertle,firstle;
  boundptr newbound;
  static vertype depth = {0.0,0.0,-200.0};
  ;
  newobj = make_obj(world,corners[0]);
  firstfve = First_obj_fve(newobj);
  le1 = firstfve->boundlist->last.bnd->lelist->last.le;
  /* make the first face */
  newvertle = make_edge_vert(le1,le1,corners[1]);
  newvertle = make_edge_vert(newvertle,newvertle,corners[2]);
  newvertle = make_edge_vert(newvertle,newvertle,corners[3]);
  secondfve = make_edge_face(newvertle,le1);  /* newvertle is in the newface */
  /* start making top hole */
  newvertle = make_edge_vert(le1,le1,corners[8]);
  /* isolate the starting vert */
  newbound = kill_edge_make_ring(Twin_le(newvertle),newvertle);
  /* finish the hole */
  firstle = newvertle;
  newvertle = make_edge_vert(newvertle,newvertle,corners[9]);
  newvertle = make_edge_vert(newvertle,newvertle,corners[10]);
  newvertle = make_edge_vert(newvertle,newvertle,corners[11]);
  thirdfve = make_edge_face(firstle,newvertle);
  /* make the face filling the hole a hole in the other side of the lamina */
  kill_face_make_loop(thirdfve,secondfve);
  /* extrude that sucker*/
  extrude_face(firstfve,depth);

  return (newobj);
}

/* create a simple square lamina */

  objptr
create_square(worldptr world,GLfloat width,GLfloat height)
{
  objptr square;
  leptr firstle,thisle;
  vertype pos;
  ;
  copypos3d(origin,pos);
  square = make_obj(world,pos);
  firstle = thisle = First_obj_le(square);
  pos[vx] = width;
  pos[vy] = 0.0;
  thisle = make_edge_vert(thisle,thisle,pos);
  pos[vx] = width;
  pos[vy] = height;
  thisle = make_edge_vert(thisle,thisle,pos);
  pos[vx] = 0.0;
  pos[vy] = height;
  thisle = make_edge_vert(thisle,thisle,pos);
  make_edge_face(thisle,firstle);

  return(square);
}
  

/* create a cube with the near, lower left hand corner centered at the origin!*/

  objptr
create_cube(worldptr world, GLfloat width, GLfloat height, GLfloat length)
{
  objptr cube;
  vertype lengthvert;
  prim_rec_ptr new_prim_rec;
  ;
  cube = create_square(world,width,height);
  setpos3d(lengthvert,0.0,0.0,length);
  extrude_face(First_obj_fve(cube),lengthvert);

  new_prim_rec = (prim_rec_ptr) alloc_elem(Primrecsize);
  new_prim_rec->prim_id = Cube_id;
  new_prim_rec->prim_info = (void *) alloc_elem(Cuberecsize);
  ((cube_rec_ptr) new_prim_rec->prim_info)->length = length;
  ((cube_rec_ptr) new_prim_rec->prim_info)->width = width;
  ((cube_rec_ptr) new_prim_rec->prim_info)->height = height;
  set_property(cube,primitive_prop,new_prim_rec);
  return(cube);
}

  objptr
recreate_cube(objptr oldcube,GLfloat length,GLfloat width,GLfloat height)
{
  objptr newcube;
  ;
  newcube = create_cube(oldcube->motherworld,width,height,length);
  set_feature_color((featureptr) newcube,oldcube->thecolor);
  copy_matrix(oldcube->xform,newcube->xform);
  copy_matrix(oldcube->invxform,newcube->invxform);
  transform_obj_reverse(newcube,newcube->invxform);
  if (oldcube->obj_name != Nil)
    set_object_name(newcube,oldcube->obj_name);
  /* need it to inherit the oldcube's parent and children too... and linked... */
  return(newcube);
}

  objptr
create_pyramid(worldptr world,GLfloat width,GLfloat height,GLfloat depth)
{
  vertype depthvert,pos;
  objptr pyramid;
  leptr firstle,thisle;
  ;
  copypos3d(origin,pos);
  pyramid = make_obj(world,pos);
  firstle = thisle = First_obj_le(pyramid);
  pos[vx] = width;
  thisle = make_edge_vert(thisle,thisle,pos);
  pos[vz] = depth;
  thisle = make_edge_vert(thisle,thisle,pos);
  pos[vx] = 0.0;
  thisle = make_edge_vert(thisle,thisle,pos);
  make_edge_face(thisle,firstle);
  depthvert[vx] = width/2.0;
  depthvert[vy] = depth;
  depthvert[vz] = depth/2.0;
  depthvert[vw] = 1.0;
  split_face_at_vert(First_obj_fve(pyramid), depthvert);

  return(pyramid);
}


  objptr
create_sphere(worldptr world,double radius,int numincrements)
{
  int i;
  double angleinc;
  leptr thisle;
  vertype pos;
  objptr sphere;
  ;
  angleinc = (180.0 / numincrements) * Deg2rad;	/* half a circle only */
  pos[vx] = cos(angleinc) * radius;
  pos[vy] = sin(angleinc) * radius;
  pos[vz] = 0.0;
  pos[vw] = 1.0;
  sphere = make_obj(world,pos);
  thisle = First_obj_le(sphere);
  /* make a wire */
  for (i = 2; i < numincrements; ++i)
  {
    pos[vx] = cos(i * angleinc) * radius;
    pos[vy] = sin(i * angleinc) * radius;
    thisle = make_edge_vert(thisle,thisle,pos);
  }
  make_solid_of_rotation(sphere,x_axis,origin,numincrements);

  return(sphere);
}

  objptr
create_oval(worldptr world,GLfloat radius_a,GLfloat radius_b,int numincrements)
{
  int i;
  double angleinc;
  leptr firstle,thisle;
  vertype pos;
  objptr oval;
  ;
  angleinc = (360.0 / numincrements) * Deg2rad;
  pos[vz] = 0.0; pos[vw] = 1.0;
  for (i = 0; i < numincrements; ++i)
  {
    pos[vx] = cos((double) i * angleinc) * (double) radius_a;
    pos[vy] = sin((double) i * angleinc) * (double) radius_b;
    if (i == 0)
    {
      oval = make_obj(world,pos);
      firstle =
	thisle = First_obj_le(oval);
    }
    else
      thisle = make_edge_vert(thisle,thisle,pos);
  }
  make_edge_face(thisle,firstle);

  return(oval);
}

  objptr
create_oval_cylinder(worldptr world, GLfloat radius_a, GLfloat radius_b,
		     GLfloat depth, int numincrements)
{
  objptr cylinder;
  static vertype depthvert = {0.0,0.0,0.0,1.0};
  prim_rec_ptr new_prim_rec;
  ;
  cylinder = create_oval(world,radius_a,radius_b,numincrements);
  depthvert[vz] = depth;
  extrude_face(First_obj_fve(cylinder),depthvert);

  new_prim_rec = (prim_rec_ptr) alloc_elem(Primrecsize);
  new_prim_rec->prim_id = Cylinder_id;
  new_prim_rec->prim_info = (void *) alloc_elem(Cylinderecsize);
  /* NOTE: this is a hack, should really store both direction radii, but */
  /* nobody will ever use this this way, will instead use scaling...*/
  ((cylinder_rec_ptr) new_prim_rec->prim_info)->radius = radius_a;
  ((cylinder_rec_ptr) new_prim_rec->prim_info)->height = depth;
  ((cylinder_rec_ptr) new_prim_rec->prim_info)->num_facets = numincrements;
  set_property(cylinder,primitive_prop,new_prim_rec);

  return(cylinder);
}

/* how about create_Oval_tine()?.... just kidding */

  objptr
create_oval_cone(worldptr world,GLfloat radius_a,GLfloat radius_b,GLfloat depth,
		 int numincrements)
{
  objptr cone;
  leptr centerle;
  static vertype oval_orig = {0.0,0.0,0.0,1.0};
  static vertype delta = {0.0,0.0,0.0,1.0};
  ;
  cone = create_oval(world,radius_a,radius_b,numincrements);
  centerle = split_face_at_vert(First_obj_fve(cone), oval_orig);
  delta[vz] = depth;
  translate_vfe_core(centerle->facevert,
		     First_obj_shell(cone),delta,Donotusetranslateflags);
  return(cone);
}

  objptr
create_paraboloid(worldptr world,GLfloat width,GLfloat height,GLfloat thickness,
		  int numpoints)
{
  vertype pos;
  double xscale,yscale;
  int i;
  objptr paraboloid;
  leptr thisle;
  ;
  xscale = (width/2.0) / (double) numpoints;
  yscale = height / (double) (numpoints * numpoints);
  pos[vz] = 0.0;
  pos[vw] = 1.0;
  for (i = 1; i < numpoints; ++i)
  {
    pos[vx] = i * xscale;
    pos[vy] = i * i * yscale;
    if (i == 1)
    {
      paraboloid = make_obj(world,pos);
      thisle = First_obj_le(paraboloid);
    }
    else
      thisle = make_edge_vert(thisle,thisle,pos);
  }
  i--;
  while (i-- > 1)
  {
    pos[vx] = i * xscale;
    pos[vy] = i * i * yscale + thickness;
    thisle = make_edge_vert(thisle,thisle,pos);
  }
  make_solid_of_rotation(paraboloid,y_axis,origin,numpoints);
  axis_rot_obj(paraboloid,x_axis,origin,900);

  return(paraboloid);
}

/* coded to test combine_loops (euler.c) */

  objptr
create_donut(worldptr world,GLfloat radius,int numincrements)
{				/* hole width will equal radius */
  objptr donut;
  leptr firstle,lastle;
  fveptr firstfve;
  vertype delta;
  ;
  donut = create_oval(world,radius,radius,numincrements);
  firstle = First_obj_le(donut);
  /* open up the circle with a null edge which then gets killed */
  make_edge_vert(firstle,Twin_le(firstle)->next,firstle->facevert->pos);
  kill_edge_face(firstle->prev,Twin_le(firstle->prev));
  firstfve = First_obj_fve(donut);
  firstle = First_obj_le(donut);
  while (firstle->next->facedge != firstle->facedge)
    firstle = firstle->next;
  /* find other end of the wiresolid */
  lastle = firstle->next;
  while (lastle->next->facedge != lastle->facedge)
    lastle = lastle->next;

  delta[vx] = radius*1.5; delta[vy] = delta[vz] = 0.0; delta[vw] = 1.0;
  translate_feature((featureptr) donut,delta,Donotusetranslateflags);
  make_solid_of_rotation(donut,y_axis,origin,numincrements);

  combine_loops(Twin_le(firstle->next)->motherbound->motherfve,
		Twin_le(lastle->next)->motherbound->motherfve,FALSE);

  return (donut);
}

  objptr
create_lbeam(worldptr world,GLfloat width,GLfloat height,GLfloat thickness,
	     GLfloat depth)
{
  objptr lbeam;
  leptr firstle,thisle;
  vertype pos;
  ;
  setpos3d(pos,width,0.0,0.0);
  lbeam = make_obj(world,pos);
  firstle = thisle = First_obj_le(lbeam);
  setpos3d(pos,width,thickness,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,thickness,thickness,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,thickness,height,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,0.0,height,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,0.0,0.0,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,0.0,0.0,(GLfloat) depth);
  extrude_face(make_edge_face(thisle,firstle),pos);

  setpos3d(pos,0.0,0.0,-50.0);
  translate_obj(lbeam, pos);

  return(lbeam);
}

  objptr
create_prism(worldptr world)
{
  objptr prism;
  leptr firstle,thisle;
  static vertype pos1 = {216.5,0.0,2.3};
  static vertype pos2 = {218.1,0.0,-2.4};
  static vertype pos3 = {220.7,0.0,0.5};
  static vertype extrude_vec = {0.0,100.0,0.0};
  ;
  prism = make_obj(world,pos1);
  firstle = thisle = First_obj_le(prism);
  thisle = make_edge_vert(thisle,thisle,pos2);
  thisle = make_edge_vert(thisle,thisle,pos3);
  extrude_face(make_edge_face(thisle,firstle),extrude_vec);

  return(prism);
}

  objptr
create_tbeam(worldptr world,GLfloat width,GLfloat height,GLfloat thickness,
	     GLfloat depth)
{
  objptr tbeam;
  leptr firstle,thisle;
  vertype pos;
  ;
  setpos3d(pos,thickness / 2.0,0.0,0.0);
  tbeam = make_obj(world,pos);
  firstle = thisle = First_obj_le(tbeam);
  setpos3d(pos,thickness/2.0,height - thickness,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,width /2.0,height - thickness,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,width / 2.0,height,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,-width/2.0,height,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,-width/2.0,height-thickness,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,-thickness/2.0,height-thickness,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,-thickness/2.0,0.0,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  setpos3d(pos,0.0,0.0,(GLfloat) depth);
  extrude_face(make_edge_face(thisle,firstle),pos);

  return(tbeam);
}

#if 0
  objptr
create_round_rect(worldptr world,GLfloat rectwidth, GLfloat rectheight,
		  double corneratio, int numcornersegs)
{
  objptr newroundrect;
  vertype pos;
  double corneradius;
  double angleinc;
  leptr firstle,thisle;
  ;
  corneradius = rectwidth * corneratio;
  setpos3d(pos,rectwidth / 2.0 - corneradius,-rectheight/2.0, 0.0);
  newroundrect = make_obj(world,pos);
  firstle = thisle = First_obj_le(newroundrect);
  angleinc = (360.0 / (numcornersegs * 4)) * Deg2rad;

  /* create lower right hand corner */
  setpos3d(cornerctr,rectwidth/2.0 - corneradius,-rectheight/2.0 + corneradius,
	   0.0);
  for (i = 0; i < numcornersegs; ++i)
  {
    pos[vx] = cos((double) i * angleinc) * (double) corneradius;
    pos[vy] = sin((double) i * angleinc) * (double) corneradius;
    addpos3d(cornerctr,pos,pos);
    thisle = make_edge_vert(thisle,thisle,pos);
  }

  /* mev to top right corner */
  setpos3d(pos,rectwidth / 2.0,rectheight / 2.0 - corneradius, 0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  /* make top right corner */
  setpos3d(cornerctr,rectwidth/2.0 - corneradius,rectheight/2.0 - corneradius,
	   0.0);
  for (; i < numcornersegs * 2; ++i)
  {
    pos[vx] = cos((double) i * angleinc) * (double) corneradius;
    pos[vy] = sin((double) i * angleinc) * (double) corneradius;
    addpos3d(cornerctr,pos,pos);
    thisle = make_edge_vert(thisle,thisle,pos);
  }

  /* mev to top left corner */
  setpos3d(pos,-rectwidth / 2.0 + corneradius,rectheight / 2.0, 0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  /* make top left corner */
  setpos3d(cornerctr,-rectwidth/2.0 + corneradius,rectheight/2.0 - corneradius,
	   0.0);
  for (; i < numcornersegs * 3; ++i)
  {
    pos[vx] = cos((double) i * angleinc) * (double) corneradius;
    pos[vy] = sin((double) i * angleinc) * (double) corneradius;
    addpos3d(cornerctr,pos,pos);
    thisle = make_edge_vert(thisle,thisle,pos);
  }

  /* mev to lower left corner */
  setpos3d(pos,-rectwidth / 2.0,-rectheight / 2.0 + corneradius, 0.0);
  thisle = make_edge_vert(thisle,thisle,pos);
  /* make lower left corner */
  setpos3d(cornerctr,-rectwidth/2.0 + corneradius,-rectheight/2.0 + corneradius,
	   0.0);
  for (; i < numcornersegs * 4; ++i)
  {
    pos[vx] = cos((double) i * angleinc) * (double) corneradius;
    pos[vy] = sin((double) i * angleinc) * (double) corneradius;
    addpos3d(cornerctr,pos,pos);
    thisle = make_edge_vert(thisle,thisle,pos);
  }

  /* close the sucker */
  make_edge_face(thisle,firstle);

  return(newroundrect);
}
  
#endif
/*************************************************************************/
/* Create table demo objects                                             */
/*************************************************************************/

#ifdef extrusiondone
  objptr
extrude_pipe_along_spine(listptr shell_list,vertype pntarray[],int numpoints,
			 int pipe_radius,int pipe_resolution)
{
  objptr newobj;
  int i;
  ;
  for (i = 0; i < numpoints-1; ++i)
  {
    diffpos3d(pntarray[i],pntarray[i+1],axis1);
    if (i == 0)
    {				/* do first polygon in pipe */
      diffpos3d(pntarray[0],pntarray[1],axis);
      cross_prod
    }
    else if (i == numpoints - 1)
    {				/* do last polygon in pipe */
      diffpos3d(pntarray[numpoints-2],pntarray[numpoints-1],axis);
    }
    else
    {
    }
  }
}
#endif


  objptr
create_chair(worldptr world,GLfloat width,GLfloat height,GLfloat depth)
{
  objptr newobj,chairobj;
  static vertype delta = {0.0,0.0,0.0,1.0};
  ;
  /* create chair seat */
  chairobj = create_cube(world,width,Chairseatheight,depth);
  delta[vy] = height/2.0 - Chairseatheight/2.0;
  translate_feature((featureptr) chairobj,delta,Donotusetranslateflags);

  /* create seat cushy-wushy was a bear */
  newobj = create_oval_cylinder(world,(width - Chairlegwidth)/2.0,
				 (depth - Chairlegwidth)/2.0,
				 Chairseatheight/2.0, 30);
  axis_rot_obj(newobj,x_axis,origin,900);
  delta[vy] = height/2.0 + Chairseatheight;
  delta[vz] = -depth/2.0;
  translate_feature((featureptr) newobj,delta,Donotusetranslateflags);
  adopt_obj_branch(chairobj,newobj,FALSE);
  
  /* create chair legs */
  newobj = create_cube(world,Chairlegwidth,height/2.0 - Chairseatheight/2.0,
			   Chairlegwidth);
  delta[vx] = width/2.0 - Chairlegwidth/2.0;
  delta[vy] = 0.0;
  delta[vz] = 0.0;
  translate_feature((featureptr) newobj,delta,Donotusetranslateflags);
  adopt_obj_branch(chairobj,newobj,FALSE);

  newobj = create_cube(world,Chairlegwidth,height/2.0 - Chairseatheight/2.0,
			Chairlegwidth);
  delta[vx] = -width/2.0 + Chairlegwidth/2.0;
  translate_feature((featureptr) newobj,delta,Donotusetranslateflags);
  adopt_obj_branch(chairobj,newobj,FALSE);

  newobj = create_cube(world,Chairlegwidth,height/2.0 - Chairseatheight/2.0,
			Chairlegwidth);
  delta[vx] = width/2.0 - Chairlegwidth/2.0;
  delta[vz] = - depth + Chairlegwidth;
  translate_feature((featureptr) newobj,delta,Donotusetranslateflags);
  adopt_obj_branch(chairobj,newobj,FALSE);

  newobj = create_cube(world,Chairlegwidth,height/2.0 - Chairseatheight/2.0,
			Chairlegwidth);
  delta[vx] = -width/2.0 + Chairlegwidth/2.0;
  translate_feature((featureptr) newobj,delta,Donotusetranslateflags);
  adopt_obj_branch(chairobj,newobj,FALSE);

  /* create chair back */
  newobj = create_cube(world,Chairbacksupportwidth,
			height/4.0 - Chairseatheight/2.0,
			Chairbacksupportwidth);
  delta[vx] = width/2.0 - Chairbacksupportwidth/2.0;
  delta[vy] = height/2.0 + Chairseatheight/2.0;
  delta[vz] = - depth + Chairbacksupportwidth;
  translate_feature((featureptr) newobj,delta,Donotusetranslateflags);
  adopt_obj_branch(chairobj,newobj,FALSE);

  newobj = create_cube(world,Chairbacksupportwidth,
			height/4.0 - Chairseatheight/2.0,
			Chairbacksupportwidth);
  delta[vx] = -width/2.0 + Chairbacksupportwidth/2.0;
  translate_feature((featureptr) newobj,delta,Donotusetranslateflags);
  adopt_obj_branch(chairobj,newobj,FALSE);
  
  newobj = create_cube(world,width,
			height/4.0 - Chairseatheight,
			Chairseatheight*1.5);
  axis_rot_obj(newobj,x_axis,origin,-350);
  delta[vx] = 0.0;
  delta[vy] = 2.6*height/4;
  delta[vz] = -depth + (Chairseatheight * 3.0);
  translate_feature((featureptr) newobj,delta,Donotusetranslateflags);
  adopt_obj_branch(chairobj,newobj,FALSE);

  return(chairobj);
}


  objptr
create_tableg(worldptr world,vertype delta)
{
  int i,legsides = 10;
  objptr newobj;
  leptr thisle,firstle;
  static vertype legpts[5] = {{-5.0,0.0,0.0,1.0},
			      {-10.0,150.0,0.0,1.0},
			      {-7.0,155.0,0.0,1.0},
			      {-8.0,158.0,0.0,1.0},
			      {-Maxlegwidth,Maxlegheight,0.0,1.0}};
  ;
  newobj = make_obj(world,legpts[0]);
  firstle = thisle = First_obj_le(newobj);
  for (i = 1; i < 5; ++i)
    thisle = make_edge_vert(thisle,thisle,legpts[i]);
  make_solid_of_rotation(newobj,y_axis,origin,legsides);
  translate_feature((featureptr) newobj,delta,Donotusetranslateflags);

  return(newobj);
}

  objptr
create_table(worldptr world, GLfloat width, GLfloat height, GLfloat depth)
{
  objptr newobj,tableobj;
  vertype delta;
  GLfloat marginx, marginz;
  ;
  tableobj = create_cube(world,width,height,depth);
  delta[vx] = delta[vz] = 0.0;
  delta[vy] = 2.0; delta[vw] = 1.0;
  translate_feature((featureptr) tableobj,delta,
		    Donotusetranslateflags);
  /* make a table leg */
  marginx = Maxlegwidth * 2.0; /* see legpts[] above for details */
  marginz = Maxlegwidth * 2.0;
  delta[vy] = -Maxlegheight;

  /* make all four table legs */
  delta[vx] = width/2.0-marginx;
  delta[vz] = -marginz;
  newobj = create_tableg(world,delta);
  adopt_obj_branch(tableobj,newobj,FALSE);

  delta[vx] = -width/2.0 + marginx;
  delta[vz] = -marginz;
  newobj = create_tableg(world,delta);
  adopt_obj_branch(tableobj,newobj,FALSE);

  delta[vx] = width/2.0 - marginx;
  delta[vz] = -depth + marginz;
  newobj = create_tableg(world,delta);
  adopt_obj_branch(tableobj,newobj,FALSE);

  delta[vx] = -width/2.0 + marginx;
  delta[vz] =  -depth + marginz;
  newobj = create_tableg(world,delta);
  adopt_obj_branch(tableobj,newobj,FALSE);

  delta[vx] = 0.0; delta[vy] = 0.0; delta[vz] = depth/2.0;
  translate_feature((featureptr) tableobj,delta,
		    Donotusetranslateflags);
  return(tableobj);
}

  objptr
create_vase(worldptr world, double xscale, double yscale, int numfaces)
{
  int i;
  double x = 1.0,xinc;
  leptr thisle,firstle;
  vertype pos;
  objptr vaseobj;
  ;
  xinc = 25.0/ (double) numfaces;
  pos[vz] = 0.0; pos[vw] = 1.0;
  for (i = 0; i < numfaces; ++i)
  {
    x += xinc;
    pos[vx] = x * xscale;
    pos[vy] = (x*x*x*3.3005e-3 + x*x*(-0.16652) +
	       x*2.5245 - 1.0756) * yscale;
    if (i == 0)
    {
      vaseobj = make_obj(world,pos);
      thisle = First_obj_le(vaseobj);
    }
    else
      thisle = make_edge_vert(thisle,thisle,pos);
  }
  make_solid_of_rotation(vaseobj,x_axis,origin,numfaces);
  axis_rot_obj(vaseobj,y_axis,origin,900); /* make it upright */

  return(vaseobj);
}
