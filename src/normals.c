
/* FILE: normals.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*        Face Normal Generation Routines for the Cutplane Editor         */
/* aka: LENT (the Laurence Edwards Normalization Technique, name courtesy */
/*            of Scott Delp)                                              */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define NORMALSMODULE

#include <config.h>

#include <stdio.h>
#include <math.h>

#include <platform_gl.h>

#include <cutplane.h>

#include <normals.h>
#include <math3d.h>


flip_bound_orientation(boundptr thisbound)
{
  fveptr thisfve;
  leptr thisle,firstle,temple,nextle;
  ;
  thisfve = thisbound->motherfve;
  firstle = thisle = thisbound->lelist->first.le;
  do
  {
    nextle = thisle->next;
    temple = firstle->next;
    thisle->next = thisle->prev;
    thisle->prev = temple;
    thisle = nextle;
  }
  while (thisle != firstle);
}

#ifdef kesslerism
  if (vector_near_zero(vec3,Zeroveclengthtol))
  {				/* If it was a BROKEN-UP triangle, use midpt */
				/* for cross prod test (these come from bools)*/
    midpoint(nextle->prev->facevert->pos,nextle->facevert->pos,breakpos);
    makevec3d(maxle->facevert->pos,breakpos,vec2);
    cross_prod(vec1,vec2,vec3);
    if (vector_near_zero(vec3,Zeroveclengthtol))
      system_error("ccw_bound: all points collinear or face non-planar!!");
  }
#endif

  Boolean
cclockwise_bound(boundptr thisbound)
{
  vertype vec1,vec2,vec3;
  float *thenorm,*maxpos,*prevpos;
  int axis1,axis2;
  leptr maxle,nextle;
  ;
  thenorm = thisbound->motherfve->facenorm;
  if (thisbound->motherfve->featureflags & Nonormalflag)
    system_error("cclockwise_bound: this face has no normal!!!");
  pickplane(thenorm,&axis1,&axis2);
  maxle = get_lemax(axis1,thisbound);
  maxpos = maxle->facevert->pos;
  prevpos = maxle->prev->facevert->pos;
  nextle = maxle->next;

  if (prevpos[axis1] == maxpos[axis1])
  {  /* bullet proofing to avoid problems with consecutive parallel edges */
    while ((nextle->facevert->pos[axis1] == maxpos[axis1]) && (nextle != maxle))
      nextle = nextle->next;
    if (nextle == maxle)
    {
      return (TRUE);
      /* put me back some time LJE */
      /*
      system_error("cclockwise: all edges in this face are collinear!");
      */
    }
  }

  makevec3d(prevpos,maxpos,vec1);
  makevec3d(maxpos,nextle->facevert->pos,vec2);
  cross_prod(vec1,vec2,vec3);

  return (dotvecs(vec3,thenorm) > 0);
}

  Boolean
cclockwise(fveptr thisfve)
{
  vertype vec1,vec2,vec3;
  float *thenorm,*maxpos,*prevpos;
  int axis1,axis2;
  leptr maxle,nextle;
  ;
  thenorm = thisfve->facenorm;
  if (thisfve->featureflags & Nonormalflag)
    system_error("cclockwise: this face has no normal!");

  pickplane(thenorm,&axis1,&axis2);
  maxle = get_lemax(axis1,thisfve->boundlist->first.bnd);
  maxpos = maxle->facevert->pos;
  prevpos = maxle->prev->facevert->pos;
  nextle = maxle->next;

  if (prevpos[axis1] == maxpos[axis1])
  {  /* bullet proofing to avoid problems with consecutive parallel edges */
    while ((nextle->facevert->pos[axis1] == maxpos[axis1]) && (nextle != maxle))
      nextle = nextle->next;
    if (nextle == maxle)
      system_error("cclockwise: all edges in this face are collinear!");
  }

  makevec3d(prevpos,maxpos,vec1);
  makevec3d(maxpos,nextle->facevert->pos,vec2);
  cross_prod(vec1,vec2,vec3);

  return (dotvecs(vec3,thenorm) > 0);
}

/* if we know which direction is counter clockwise for thisfve then we */
/* can figure out the counter clockwise direction for the adjoining faces */
/* once we do this, find an angle we know is acute (by finding the maximum */
/* vert) then take the cross-product in the counterclockwise direction */
/* and this is the normal !! */

  void
calc_face_norm(fveptr thisfve, vertype facenorm)
{
  leptr firstle,thisle,nextle;
  vertype firstvec,thisvec;
  static vertype stupidnorm = {1.0, 0.0, 0.0, 0.0};
  vfeptr firstvfe;
  ;
  firstle = thisfve->boundlist->first.bnd->lelist->first.le;
  nextle = firstle->next;
  firstvfe = firstle->facevert;
  do
  {
    makevec3d(firstvfe->pos,nextle->facevert->pos,firstvec);
    if (!vec_eq_zero(firstvec))
      break;
    nextle = nextle->next;
  } while (nextle != firstle);
  if (nextle == firstle)
  {
    copyvec3d(stupidnorm,facenorm);
    return;
    /* put me back some time LJE */
    system_error("calc_face_norm: all points in this face are coincident!");
  }
  thisle = nextle->next;
  if (thisle == firstle)
  {
    copyvec3d(stupidnorm,facenorm);
    return;
    /* put me back some time LJE */
    system_error("calc_face_norm: only 2 points in this face do "
		 "not coincide!");
  }
  normalize(firstvec,firstvec);
  do
  {
    makevec3d(firstvfe->pos,thisle->facevert->pos,thisvec);
    normalize(thisvec,thisvec);
    cross_prod(firstvec,thisvec,facenorm);
    if (!vector_near_zero(facenorm,1.0e-6))
    {
      normalize(facenorm,facenorm);
      facenorm[vw] = 0.0;
      return;
    }
    thisle = thisle->next;
  } while (thisle != firstle);

  copyvec3d(stupidnorm,facenorm);
  return;
  /* put me back some time LJE */
  system_error("calc_face_norm: all edges in this face are colinear!");
}

  void
find_normals_guts(shellptr this_shell, fveptr thisfve)
{
  leptr firstle,thisle;
  evfptr thisevf;
  fveptr otherfve;
  boundptr firstbound,thisbound;
  vertype othernorm;
  ;
  if (thisfve->featureflags & Nonormalflag)
    system_error("find_normals_guts: thisfve->facenorm has mag = 0.");
  /* find all faces adjoining thisfve that don't have normals */
  firstbound = thisbound = thisfve->boundlist->first.bnd;
  while (thisbound)
  {
    if (thisbound != firstbound)
      if (cclockwise_bound(thisbound) == this_shell->cclockwise)
      {
	flip_hole_orientation(thisbound);
	if (cclockwise(thisfve) != this_shell->cclockwise)
	  system_error("find_normals_guts: shell isn't consistently oriented");
      }
    thisle = firstle = thisbound->lelist->first.le;
    do
    {
      otherfve = Twin_le(thisle)->motherbound->motherfve;
      if (otherfve->featureflags & Nonormalflag)
      {
	calc_face_norm(otherfve,othernorm);
	setfnorm(otherfve,othernorm);
	if (cclockwise(otherfve) != this_shell->cclockwise)
	{
	  flip_vec(othernorm);
	  setfnorm(otherfve,othernorm);
	}
	find_normals_guts(this_shell,otherfve);
      }
      thisle = thisle->next;
    } while (thisle != firstle);
    thisbound = thisbound->next;
  }
}

  leptr
find_top_edge(shellptr the_shell, vfeptr vfemax)
{
  float slope,deltax,deltay,deltaz,minslope = -1.0;
  float *vertex1,*vertex2;
  leptr thisle,tople;
  ;
  thisle = tople = vfemax->startle;  /* vfe with maximum y value */
  /* of the edges that contain vfemax find the one with the smallest slope */
  /* maybe change this to find the minslope to the x-z plane if more than */
  /* one le has minslope */
  do
  {
    vertex1 = thisle->facevert->pos;
    vertex2 = Twin_le(thisle)->facevert->pos;
    deltax = vertex1[vx] - vertex2[vx];
    deltay = vertex1[vy] - vertex2[vy];
    deltaz = vertex1[vz] - vertex2[vz];
    if ((deltax != 0.0) || (deltaz != 0.0))
    {
      if (deltay == 0.0)
	return (thisle);
      slope = (deltay*deltay)/(deltax*deltax + deltaz*deltaz);
      if ((slope < minslope) || (minslope < 0.0))
      {
	minslope = slope;
	tople = thisle;
      }
    }
    thisle = Twin_le(thisle)->next;
  }  while (thisle != vfemax->startle);
  if (minslope < 0.0)
    system_error("find_top_edge: all edges are vertical");
  /* a hack around this problem might be to perturb bottom points of the */
  /* vertical edges radially find face normals then perturb them back and */
  /* set vy of the normals for those faces = 0.0 */
  return (tople);
}

  Boolean
sameside(vertype othervec[], vertype norm)
{
  float dotprod1,dotprod2;
  ;
  /* norm is the normal to the plane containing y-vec and topvec */
  /* this works because othervec contains vfemax, so the angle between */
  /* topvec and othervec is always acute and the interior is always on */
  /* the same side as othervec */
  dotprod1 = dotvecs(othervec[0],norm);
  dotprod2 = dotvecs(othervec[1],norm);
  if ((dotprod1 != 0.0) && (dotprod2 != 0.0))
    return(fsign(dotprod1) == fsign(dotprod2));
  return(1);
}

fix_direction(vertype facenorm[], vertype othervec[])
{
  static vertype y_vec = {0.0,1.0,0.0,0.0};
  float costh1,costh2;
  ;
  /* if both faces are on the same side of the plane containing the */
  /* top edge and a y-axis vector then find the vector in the faces */
  /* and perpendicular to that plane (actually just need the normal and */
  /* make sure it points upwards)...then the vector that makes the  */
  /* smallest angle with the y-axis vector is the top face          */

  costh1 = dotvecs(facenorm[0],y_vec);
  costh2 = dotvecs(facenorm[1],y_vec);
  
  if (costh1 == 0.0) /* probably put a tolerance here */
  { /* face 1 is parallel to the y-axis */
    if (dotvecs(othervec[1],facenorm[0]) > 0.0)
    {  /* angle is < 90 deg so face normal 1 must be wrong */
      facenorm[0][vx] = -facenorm[0][vx];
      facenorm[0][vy] = -facenorm[0][vy];
      facenorm[0][vz] = -facenorm[0][vz];
    }
  }
  else if (costh2 == 0.0) /* face 2 is parallel to the y-axis */
  {
    if (dotvecs(othervec[0],facenorm[1]) > 0.0) /* face normal 2 is wrong */
    {
      facenorm[1][vx] = -facenorm[1][vx];
      facenorm[1][vy] = -facenorm[1][vy];
      facenorm[1][vz] = -facenorm[1][vz];
    }
  }
  else if (costh1 > costh2) /* face 1 points up and face 2 points down */
  {
    facenorm[1][vx] = -facenorm[1][vx];
    facenorm[1][vy] = -facenorm[1][vy];
    facenorm[1][vz] = -facenorm[1][vz];
  }
  else if (costh1 < costh2) /* face 2 points up and face 1 points down */
  {
    facenorm[0][vx] = -facenorm[0][vx];
    facenorm[0][vy] = -facenorm[0][vy];
    facenorm[0][vz] = -facenorm[0][vz];
  }
}

  leptr
find_first_norms(shellptr thishell)
{
  fveptr thisfve[2];
  leptr tople,tople_twin,tople_prev,twinle_next;
  static vertype y_vec = {0.0,1.0,0.0,0.0}, topvec = {0.0,0.0,0.0,0.0};
  vertype facevec[2],facenorm[2],norm;
  int i;
  ;
  /* get the edge with the min slope that contains vfe with max y-coord */
  tople = find_top_edge(thishell,get_3axis_vfemax(vy,vz,vx,thishell->vfelist));
  tople_twin = Twin_le(tople);
  thisfve[0] = tople->motherbound->motherfve;
  thisfve[1] = tople_twin->motherbound->motherfve;
  calc_face_norm(thisfve[0],facenorm[0]);
  calc_face_norm(thisfve[1],facenorm[1]);

  for (i=0; i<2; ++i)    /* make sure both normals point upwards */
    if (facenorm[i][vy] < 0.0)
      flip_vec(facenorm[i]);
  
  tople_prev = find_noncollinear_le(tople,Prev);
  twinle_next = find_noncollinear_le(tople_twin,Next);

  /* find vector normal to the plane containing the y-vector and topevf */
  /* note the normalizations below arent required in theory, but they */
  /* are done so that numerical innacuracies will be at the same scale */

  makevec3d(tople->facevert->pos,tople_twin->facevert->pos,topvec);
  normalize(topvec,topvec);
  cross_prod(topvec,y_vec,norm);
  normalize(norm,norm);
  makevec3d(tople->facevert->pos,tople_prev->facevert->pos,facevec[0]);
  makevec3d(tople->facevert->pos,twinle_next->facevert->pos,facevec[1]);
  normalize(facevec[0],facevec[0]);
  normalize(facevec[1],facevec[1]);

  /* in a degenerate case both compares below could be true, fix this later */

  /* if one face's normal is perp. to y_vec other face's normal is right */
  if (compare(facenorm[0][vy],0.0,1.0e-6) == 0)
  {
    setfnorm(thisfve[0],facenorm[0]);
    setfnorm(thisfve[1],facenorm[1]);
    if (cclockwise(thisfve[0]) != cclockwise(thisfve[1]))
      flip_vec(facenorm[0]);
  }
  else if (compare(facenorm[1][vy],0.0,1.0e-6) == 0)
  {
    setfnorm(thisfve[0],facenorm[0]);
    setfnorm(thisfve[1],facenorm[1]);
    if (cclockwise(thisfve[1]) != cclockwise(thisfve[0]))
      flip_vec(facenorm[1]);
  }
  else if (sameside(facevec,norm))
    fix_direction(facenorm,facevec);

  setfnorm(thisfve[0],facenorm[0]);
  setfnorm(thisfve[1],facenorm[1]);

  /* might as well set the shells cclockwise flag here */
  thishell->cclockwise = cclockwise(thisfve[0]);

  /* we want all outer shells to have cclockwise orientation */
  if (thishell == thishell->motherobj->shellist->first.shell)
  {
    if (!thishell->cclockwise)
    {
      flip_shell_orientation(thishell);
      thishell->cclockwise = TRUE;
    }
  }
  else  /* we want all inner shells to have clockwise orientation */
    if (thishell->cclockwise)
    {
      flip_shell_orientation(thishell);
      thishell->cclockwise = FALSE;
    }

  return(tople);
}

  void
clear_normals(shellptr thishell)
{
  fveptr thisfve;
  ;
  thisfve = thishell->fvelist->first.fve;
  while(thisfve != Nil)
  {
    thisfve->featureflags |= Nonormalflag;
    thisfve = thisfve->next;
  }
}

  void
find_obj_normals(objptr thisobj)
{
  leptr tople;
  shellptr thishell;
  ;
  for (thishell = First_obj_shell(thisobj); thishell != Nil;
       thishell = thishell->next)
  {
    clear_normals(thishell);
    tople = find_first_norms(thishell);
/*
    printvert(tople->motherbound->motherfve->facenorm);
    printvert(Twin_le(tople)->motherbound->motherfve->facenorm);
*/
    if (tople != Nil)
      find_normals_guts(thishell,tople->motherbound->motherfve);
    /*  printfnorms(thishell->fvelist);*/
    /* HACK here because the normalizer has a bug dealing with lamina */
    /* and always makes both norms point the same way*/
    if (thishell->fvelist->numelems == 2)
    {
      flip_vec(thishell->fvelist->first.fve->facenorm);      
      flip_vec(thishell->fvelist->first.fve->local_facenorm);      
    }
  }
}

  void
find_branch_normals(objptr thisobj)
{
  apply_to_descendants(thisobj,find_obj_normals, Nil);
}

/* NEEDS UPDATING AND/OR AXING! */

  void
find_normals(listptr shell_list)
{
  leptr tople;
  shellptr thishell;
  ;
  /* pick either one of the 2 faces attached to topevf to start with */
  thishell = shell_list->first.shell;
  while(thishell)
  {
    clear_normals(thishell);	/* LJE improve this with "normalized" flag */
				/* of some kind (bitfield?) */
    tople = find_first_norms(thishell);
    find_normals_guts(thishell,tople->motherbound->motherfve);
    thishell = thishell->next;
  }
}

  void
compute_planedist(fveptr thisfve)
{
  float *facept;
  ;
  facept = thisfve->boundlist->first.bnd->lelist->first.le->facevert->pos;
  thisfve->planedist = - (thisfve->facenorm[vx] * facept[vx] +
			  thisfve->facenorm[vy] * facept[vy] +
			  thisfve->facenorm[vz] * facept[vz]);
}

  void
compute_shell_planedists(shellptr thishell)
{
  fveptr thisfve;
  ;
  for (thisfve = thishell->fvelist->first.fve; thisfve != Nil;
       thisfve = thisfve->next)
    compute_planedist(thisfve);
}

  void
compute_obj_planedists(objptr thisobj)
{
  shellptr thishell;
  fveptr thisfve;
  ;
  for (thishell = First_obj_shell(thisobj); thishell != Nil;
       thishell = thishell->next)
    for (thisfve = thishell->fvelist->first.fve; thisfve != Nil;
	 thisfve = thisfve->next)
      compute_planedist(thisfve);
}

  void
invert_shell(shellptr thishell)
{
  fveptr thisfve;
  ;
  thisfve = thishell->fvelist->first.fve;
  while (thisfve != Nil)
  {
    flip_vec(thisfve->facenorm);
    compute_planedist(thisfve);	/* recompute face plane distance (sign change)*/
    thisfve = thisfve->next;
  }
  thishell->cclockwise = !thishell->cclockwise;
  flip_shell_orientation(thishell);
}
