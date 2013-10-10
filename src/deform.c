
/* FILE: deform.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*             Deformation Routines for the Cutplane Editor               */
/*                                                                        */
/* Author: LJE                                    Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define DEFORMODULE

#include <config.h>
#include <stdio.h>

#include <platform_gl.h>
#include <math.h>

#include <cutplane.h>

#include <deform.h>
#include <globals.h>
#include <grafx.h>
#include <math3d.h>

vertype ppiped[4] = {{-120.0,-120.0,-120.0}, /* stu system origin */
		     {240.0,0.0,0.0}, /* s axis unit vector */
		     {0.0,240.0,0.0}, /* t axis unit vector */
		     {0.0,0.0,240.0}}; /* u axis unit vector */

  void
bicoeff(int n, lattsizevec bc_array) /* generate binomial coefficients */
{
  register int i,k;
  int kfact,nfact,nminuskfact;
  ;
  for (i=n-1,nfact = n; i>0; --i)
    nfact *= i;
  
  bc_array[0] = 1.0;
  
  for (k=1, kfact=1, nminuskfact = nfact; k < n; ++k)
  {
    nminuskfact /= (n - k + 1);
    kfact *= k;
    bc_array[k] = (Coord) nfact/(kfact*nminuskfact);
  }
  
  bc_array[n] = 1.0;
}

powers(Coord number, int maxpower, lattsizevec numpower)
{		/* generate an array of powers from x^0 to x^maxpower */
  register int i;
  ;
  numpower[0] = 1.0;		/* x^0 = 1.0 for any x */
  for (i=1; i <= maxpower; ++i)
    numpower[i] = numpower[i-1] * number;
}

set_controlpt_array(shellptr control_shell,
		    vertype controlpts[][Ncontrolpts][Ncontrolpts])
{
  static interior_filled = FALSE;
  vertype *temp;
  vertype diff;
  vertype maxpt;
  vfeptr thisvfe;
  int i,j,k;
  ;
  thisvfe = control_shell->vfelist->first.vfe;
  temp = (vertype *) controlpts;
  copypos3d(thisvfe->pos,temp[0]);  thisvfe = thisvfe->next;
  copypos3d(thisvfe->pos,temp[1]);  thisvfe = thisvfe->next;
  copypos3d(thisvfe->pos,temp[5]);  thisvfe = thisvfe->next;
  copypos3d(thisvfe->pos,temp[4]);  thisvfe = thisvfe->next;
  copypos3d(thisvfe->pos,temp[2]);  thisvfe = thisvfe->next;
  copypos3d(thisvfe->pos,temp[6]);  thisvfe = thisvfe->next;
  copypos3d(thisvfe->pos,temp[3]);  thisvfe = thisvfe->next;
  copypos3d(thisvfe->pos,temp[7]);  thisvfe = thisvfe->next;
  for (i=0; i<2; i++)
  {
    copypos3d(thisvfe->pos,temp[8+(i*4)]);  thisvfe = thisvfe->next;
    copypos3d(thisvfe->pos,temp[9+(i*4)]);  thisvfe = thisvfe->next;
    copypos3d(thisvfe->pos,temp[10+(i*4)]);  thisvfe = thisvfe->next;
    copypos3d(thisvfe->pos,temp[11+(i*4)]);  thisvfe = thisvfe->next;
  }
/*  
28 29 30 31
44 45 46 47
60 61 62 63
*/
  for (i=0; i<3; i++)
  {
    copypos3d(thisvfe->pos,temp[28+(i*16)]);  thisvfe = thisvfe->next;
    copypos3d(thisvfe->pos,temp[29+(i*16)]);  thisvfe = thisvfe->next;
    copypos3d(thisvfe->pos,temp[30+(i*16)]);  thisvfe = thisvfe->next;
    copypos3d(thisvfe->pos,temp[31+(i*16)]);  thisvfe = thisvfe->next;
  }
/*
56 57 58 59
52 53 54 55
48 49 50 51
*/
  for (i=0; i<3; i++)
  {
    copypos3d(thisvfe->pos,temp[56-(i*4)]);  thisvfe = thisvfe->next;
    copypos3d(thisvfe->pos,temp[57-(i*4)]);  thisvfe = thisvfe->next;
    copypos3d(thisvfe->pos,temp[58-(i*4)]);  thisvfe = thisvfe->next;
    copypos3d(thisvfe->pos,temp[59-(i*4)]);  thisvfe = thisvfe->next;
  }
/*
32 33 34 35
16 17 18 19
*/
  for (i=0; i<2; i++)
  {
    copypos3d(thisvfe->pos,temp[32-(i*16)]);  thisvfe = thisvfe->next;
    copypos3d(thisvfe->pos,temp[33-(i*16)]);  thisvfe = thisvfe->next;
    copypos3d(thisvfe->pos,temp[34-(i*16)]);  thisvfe = thisvfe->next;
    copypos3d(thisvfe->pos,temp[35-(i*16)]);  thisvfe = thisvfe->next;
  }
/*
20 36 24 40
*/
  copypos3d(thisvfe->pos,temp[20]);  thisvfe = thisvfe->next;
  copypos3d(thisvfe->pos,temp[36]);  thisvfe = thisvfe->next;
  copypos3d(thisvfe->pos,temp[24]);  thisvfe = thisvfe->next;
  copypos3d(thisvfe->pos,temp[40]);  thisvfe = thisvfe->next;
/*
23 27 39 43
*/
  copypos3d(thisvfe->pos,temp[23]);  thisvfe = thisvfe->next;
  copypos3d(thisvfe->pos,temp[27]);  thisvfe = thisvfe->next;
  copypos3d(thisvfe->pos,temp[39]);  thisvfe = thisvfe->next;
  copypos3d(thisvfe->pos,temp[43]);
  if (!interior_filled)		/* set interior pts */
  {
    diff[vx] = ppiped[1][vx] / (Ncontrolpts - 1);
    diff[vy] = ppiped[2][vy] / (Ncontrolpts - 1);
    diff[vz] = ppiped[3][vz] / (Ncontrolpts - 1);
    for (i=1; i < (Ncontrolpts - 1); ++i)
      for (j=1; j < (Ncontrolpts - 1); ++j)
	for (k=1; k < (Ncontrolpts - 1); ++k)
	{
	  controlpts[i][j][k][vx] = ppiped[0][vx] + i * diff[vx];
	  controlpts[i][j][k][vy] = ppiped[0][vy] + j * diff[vy];
	  controlpts[i][j][k][vz] = ppiped[0][vz] + k * diff[vz];
	}
    interior_filled = TRUE;
  }
  
  /* y-z plane throuch X0 */
/*
  printf("\ny-z plane X0:\n");
*/
  for (j=0; j < Ncontrolpts; ++j) /* step in y direction */
    for (k=0; k < Ncontrolpts; ++k) /* step in z direction */
    {
/*
      printf("controlpt[%d,%d,%d] = (%f,%f,%f)\n",0,j,k,
	     controlpts[0][j][k][vx],controlpts[0][j][k][vy],
	     controlpts[0][j][k][vz]);
*/
    }
  /* x-z plane through maxpt */
/*
  printf("\nx-z plane maxpt:\n");
*/
  for (i=1; i < Ncontrolpts; ++i) /* step in x direction */
    for (k=0; k < Ncontrolpts; ++k) /* step in z direction */
    {
/*
      printf("controlpt[%d,%d,%d] = (%f,%f,%f)\n",i,Ncontrolpts-1,k,
	     controlpts[i][Ncontrolpts-1][k][vx],
	     controlpts[i][Ncontrolpts-1][k][vy],
	     controlpts[i][Ncontrolpts-1][k][vz]);
*/
    }
  /* y-z plane throuch maxpt */
/*
  printf("\ny-z plane maxpt:\n");
*/
  for (j = Ncontrolpts-2; j >= 0; --j) /* step in y direction */
    for (k=0; k < Ncontrolpts; ++k) /* step in z direction */
    {
/*
      printf("controlpt[%d,%d,%d] = (%f,%f,%f)\n",Ncontrolpts-1,j,k,
	     controlpts[Ncontrolpts-1][j][k][vx],
	     controlpts[Ncontrolpts-1][j][k][vy],
	     controlpts[Ncontrolpts-1][j][k][vz]);
*/
    }
  /* x-z plane through X0 */
/*
  printf("\nx-z plane X0:\n");
*/
  for (i = Ncontrolpts-2; i > 0; --i) /* step in x direction */
    for (k=0; k < Ncontrolpts; ++k) /* step in z direction */
    {
/*
      printf("controlpt[%d,%d,%d] = (%f,%f,%f)\n",i,0,k,
	     controlpts[i][0][k][vx],
	     controlpts[i][0][k][vy],
	     controlpts[i][0][k][vz]);
*/
    }
  /* x-y planes */
/*
  printf("\nx-y planes:\n");
*/
  for (k=0; k < Ncontrolpts; k+=(Ncontrolpts-1)) /* step in z direction */
    for (j=1; j < (Ncontrolpts-1); ++j) /* step in y direction */
      for (i=1; i < (Ncontrolpts-1); ++i) /* step in x direction */
      {
/*
	printf("controlpt[%d,%d,%d] = (%f,%f,%f)\n",i,j,k,
	       controlpts[i][j][k][vx],
	       controlpts[i][j][k][vy],
	       controlpts[i][j][k][vz]);
*/
      }
}

  void
evaluate(objptr control_obj, vfeptr thisvfe, Coord s, Coord t, Coord u)
{
  register int i,j,k;
  static vertype controlpts[Ncontrolpts][Ncontrolpts][Ncontrolpts];
  lattsizevec spowers,tpowers,upowers,s1powers,t1powers,u1powers;
  lattsizevec bicoeffs;
  vertype sum;
  Coord sfactor,stfactor,stufactor,bc_i,bc_ij,bc_ijk;
  shellptr control_shell;
  ;
  control_shell = First_obj_shell(control_obj);
  set_controlpt_array(control_shell,controlpts);
  sum[vx] = sum[vy] = sum[vz] = 0.0;
  powers(s,Ncontrolpts-1,spowers);
  powers(t,Ncontrolpts-1,tpowers);
  powers(u,Ncontrolpts-1,upowers);
  powers(1-s,Ncontrolpts-1,s1powers);
  powers(1-t,Ncontrolpts-1,t1powers);
  powers(1-u,Ncontrolpts-1,u1powers);
  bicoeff(Ncontrolpts-1,bicoeffs);
  for (i = 0; i < Ncontrolpts; ++i)
  {
    bc_i = bicoeffs[i];
    sfactor = spowers[i]*s1powers[Ncontrolpts-1-i];
    for (j = 0; j < Ncontrolpts; ++j)
    {
      bc_ij = bc_i*bicoeffs[j];
      stfactor = sfactor*tpowers[j]*t1powers[Ncontrolpts-1-j];
      for (k = 0; k < Ncontrolpts; ++k)
      {
	bc_ijk = bc_ij * bicoeffs[k];
	stufactor = stfactor*upowers[k]*u1powers[Ncontrolpts-1-k];
	sum[vx] += controlpts[i][j][k][vx]*stufactor*bc_ijk;
	sum[vy] += controlpts[i][j][k][vy]*stufactor*bc_ijk;
	sum[vz] += controlpts[i][j][k][vz]*stufactor*bc_ijk;
      }
    }
  }
  setvfe(thisvfe,sum,
	  thisvfe->startle->motherbound->motherfve->mothershell->motherobj->
	  invxform);
}

precompute_prods(xyzstruct *TxU, xyzstruct *UxS, xyzstruct *SxT,
		 Coord *ToUxS, Coord *X0oTxU, Coord *X0oUxS, Coord *X0oSxT)
{
  TxU->x = ppiped[2][vy]*ppiped[3][vz] - ppiped[2][vz]*ppiped[3][vy];
  TxU->y = ppiped[2][vz]*ppiped[3][vx] - ppiped[2][vx]*ppiped[3][vz];
  TxU->z = ppiped[2][vx]*ppiped[3][vy] - ppiped[2][vy]*ppiped[3][vx];
  
  UxS->x = ppiped[3][vy]*ppiped[1][vz] - ppiped[3][vz]*ppiped[1][vy];
  UxS->y = ppiped[3][vz]*ppiped[1][vx] - ppiped[3][vx]*ppiped[1][vz];
  UxS->z = ppiped[3][vx]*ppiped[1][vy] - ppiped[3][vy]*ppiped[1][vx];
  
  SxT->x = ppiped[1][vy]*ppiped[2][vz] - ppiped[1][vz]*ppiped[2][vy];
  SxT->y = ppiped[1][vz]*ppiped[2][vx] - ppiped[1][vx]*ppiped[2][vz];
  SxT->z = ppiped[1][vx]*ppiped[2][vy] - ppiped[1][vy]*ppiped[2][vx];
  
  *X0oTxU = ppiped[0][0]*TxU->x + ppiped[0][1]*TxU->y + ppiped[0][2]*TxU->z;
  *X0oUxS = ppiped[0][0]*UxS->x + ppiped[0][1]*UxS->y + ppiped[0][2]*UxS->z;
  *X0oSxT = ppiped[0][0]*SxT->x + ppiped[0][1]*SxT->y + ppiped[0][2]*SxT->z;
  
  *ToUxS = ppiped[2][vx]*UxS->x + ppiped[2][vy]*UxS->y + ppiped[2][vz]*UxS->z;
}


  Boolean
obj_in_lattice(objptr thisobj, objptr control_obj)
{
  return (point_in_3dbbox(thisobj->objbbox.mincorner,&(control_obj->objbbox)) ||
	  point_in_3dbbox(thisobj->objbbox.maxcorner,&(control_obj->objbbox)));
}

void
deform(objptr control_obj)
{
  xyzstruct TxU,UxS,SxT;
  Coord s,t,u,ToUxS,X0oTxU,X0oUxS,X0oSxT;
  register float *vertpos;
  objptr thisobj;
  featureptr thisfeature;
  shellptr thishell;
  vfeptr thisvfe;
  proptr thisprop;
  ;
  precompute_prods(&TxU,&UxS,&SxT,&ToUxS,&X0oTxU,&X0oUxS,&X0oSxT);
  set_3d_objbbox(control_obj);
  thisprop = working_world->world_proplists[(int) visible_prop]->first.prop;
  while (thisprop != Nil)
  {
    thisfeature = thisprop->owner;
    if (thisfeature->type_id == Obj_type)
    {
      thisobj = (objptr) thisfeature;
      set_3d_objbbox(thisobj);
      if (obj_in_lattice(thisobj,control_obj) && (thisobj != control_obj))
      {
	thishell = First_obj_shell(thisobj);
	splitup_edges(thishell,5);
	thisvfe = thishell->vfelist->first.vfe;
	while(thisvfe)
	{
	  vertpos = thisvfe->pos;
	  s = (vertpos[vx]*TxU.x+ vertpos[vy]*TxU.y + vertpos[vz]*TxU.z -
	       X0oTxU) / ToUxS;
	  t = (vertpos[vx]*UxS.x+ vertpos[vy]*UxS.y + vertpos[vz]*UxS.z -
	       X0oUxS) / ToUxS;
	  u = (vertpos[vx]*SxT.x+ vertpos[vy]*SxT.y + vertpos[vz]*SxT.z -
	       X0oSxT) / ToUxS;
	  if ( ((s <= 1.0) && (s >= 0.0)) && ((t <= 1.0) && (t >= 0.0)) &&
	      ((u <= 1.0) && (u >= 0.0)) )
	    evaluate(control_obj,thisvfe,s,t,u);
	  thisvfe = thisvfe->next;
	}
      }
    }
    thisprop = thisprop->next;
  }
}
