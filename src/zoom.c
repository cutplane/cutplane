
/* FILE: zoom.c */

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                                                                        */
/*        Zooooming Routines for the Cutplane Editor                      */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 17,1990  */
/* Version: 1.0                                                           */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#define ZOOMODULE

#include <config.h>
#include <stdio.h>

#include <platform_gl.h>
#include <math.h>

#include <cutplane.h>

#include <pick.h>
#include <math3d.h>
#include <globals.h>
#include <update.h>
#include <primitives.h>

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	                       GLOBAL VARIABLES				     */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

GLfloat totalzoom = 1.0;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                            Zooming Routines                            */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

  void
fix_prim_recs(worldptr world,GLfloat scalefactor)
{
  int i = 1;
  prim_rec_ptr the_prim_rec;
  cube_rec_ptr cube_info;
  cylinder_rec_ptr cylinder_info;
  objptr thisprim;
  ;
  while ((thisprim = (objptr) get_property_feature(world,primitive_prop,i)) !=
	 Nil)
  {
    the_prim_rec = (prim_rec_ptr)
      get_property_val((featureptr) thisprim,primitive_prop).ptr;
    if (the_prim_rec == Nil)
      system_warning("Prim missing prim rec!!!\n");
    else
    {
      if (the_prim_rec->prim_id == Cube_id)
      {
	cube_info = (cube_rec_ptr) the_prim_rec->prim_info;
	cube_info->length *= scalefactor;
	cube_info->width *= scalefactor;
	cube_info->height *= scalefactor;
      }
      else if (the_prim_rec->prim_id == Cylinder_id)
	;				/* same shit */
    }
    i++;
  }
}

  void
zoom_world(worldptr world, vertype zoomcenter, GLfloat scalefactor)
{
	/* OGLXXX XXX fix this. */
  Matrix applyinvmatrix,applymatrix,tempxform;
  ;
  /* record total zoom amount so far... */
/*  totalzoom += (scalefactor - 1.0);*/
  totalzoom *= scalefactor;

  glPushMatrix();
  glLoadMatrixf((GLfloat *) imatrix);
  glTranslatef(zoomcenter[vx], zoomcenter[vy], zoomcenter[vz]);
  glScalef(scalefactor, scalefactor, scalefactor);
  glTranslatef(-zoomcenter[vx], -zoomcenter[vy], -zoomcenter[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) applymatrix);

  scalefactor = 1.0/scalefactor;

  glLoadMatrixf((GLfloat *) imatrix);
  glTranslatef(zoomcenter[vx], zoomcenter[vy], zoomcenter[vz]);
  glScalef(scalefactor, scalefactor, scalefactor);
  glTranslatef(-zoomcenter[vx], -zoomcenter[vy], -zoomcenter[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) applyinvmatrix);
  glPopMatrix();

  postmult_tree_by_xform(world->root,applymatrix,applyinvmatrix);

  multmatrix_4x4(world->viewer.xform,applymatrix,tempxform);
  copy_matrix(tempxform,world->viewer.xform);

  multmatrix_4x4(applyinvmatrix,world->viewer.invxform,tempxform);
  copy_matrix(tempxform,world->viewer.invxform);

  scalefactor = 1.0/scalefactor;
  fix_prim_recs(primary_world,scalefactor);
/*  log_branch_erase_bbox(world->root,Useobjbbox,Loginoldbuffer);*/
  log_object_erase_bbox(world->root,Usegroupbbox,Loginoldbuffer);

  log_update((featureptr) world->root,
	     Transformflag | Evfeqnsflag | Cutevfsflag | Makecutsectionflag |
	     Computeplanedistsflag | Computebboxflag |
	     Redrawflag | Recomputebackfaceflag,
	     Immediateupdate);
/*  log_property_update(world,visible_prop,Allupdateflags,Immediateupdate);*/
}

  void
totally_unzoom_world(worldptr world)
{
  totalzoom = 1.0;
  postmult_tree_by_xform(world->root,world->viewer.invxform,
			 world->viewer.xform);
  copy_matrix(imatrix, world->viewer.xform);
  copy_matrix(imatrix, world->viewer.invxform);
}

  void
	/* OGLXXX
	 * XXX fix this.
	 * XXX fix this.
	 */
totally_zoom_world(worldptr world, Matrix xform, Matrix invxform)
{
  totalzoom = xform[0][0];	/* zoom is same in all directions */
  copy_matrix(xform, world->viewer.xform);
  copy_matrix(invxform, world->viewer.invxform);
  postmult_tree_by_xform(world->root,world->viewer.xform,
			 world->viewer.invxform);
}

  GLfloat				/* zoom unit zappa? */
zoom_unit(GLfloat unzoomed_amt)
{
  return(totalzoom * unzoomed_amt);
}

  GLfloat
unzoom_unit(GLfloat zoomed_amt)
{
  return((1.0 / totalzoom) * zoomed_amt);
}

  GLfloat
fetch_system_zoom(void)
{
  return(totalzoom);
}
  
  void
set_system_zoom(GLfloat new_zoom)
{
  totalzoom = new_zoom;
}

#ifdef defunctway

  void
zoom_world(worldptr world, vertype zoomcenter, GLfloat deltacursor)
{
  GLfloat zoomfactor;
  ;
  zoomfactor = pow(1.1,deltacursor);
  glPushMatrix();
  glLoadMatrixf(world->viewer.xform);
  glScalef(zoomfactor, zoomfactor, zoomfactor);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, world->viewer.xform);
  glPopMatrix();
}

#endif
