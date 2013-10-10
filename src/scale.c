
/* FILE: scale.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*         GLuint Mirroring Routines for the Cutplane Editor              */
/*                                                                        */
/* Author: WAK                                    Date: December 18,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define SCALEMODULE

#include <config.h>
#include <stdio.h>

#include <platform_gl.h>
#include <math.h>

#include <cutplane.h>

void
calculate_scale_factors(vertype scale_center,vertype oldposition,
			vertype newposition, vertype scalefactor)
{
  vertype rel_oldpos,rel_newpos;
  ;
  diffpos3d(scale_center,oldposition,rel_oldpos);
  diffpos3d(scale_center,newposition,rel_newpos);
  
  if (compare(rel_oldpos[vx],0.0,1.0e-6) == 0)
    /* should only happen if newpos is same as old pos*/
    scalefactor[vx] = 1.0;
  else
    scalefactor[vx] = rel_newpos[vx]/rel_oldpos[vx];

  if (compare(rel_oldpos[vy],0.0,1.0e-6) == 0)
    /* should only happen if newpos is same as old pos*/
    scalefactor[vy] = 1.0;
  else    
    scalefactor[vy] = rel_newpos[vy]/rel_oldpos[vy];

  if (compare(rel_oldpos[vz],0.0,1.0e-6) == 0)
    /* should only happen if newpos is same as old pos*/
    scalefactor[vz] = 1.0;
  else    
    scalefactor[vz] = rel_newpos[vz]/rel_oldpos[vz];
}

  void
	/* OGLXXX XXX fix this. */
scale_vfe(vfeptr thisvfe, Matrix scalefactor)
{				/* scaling matrix is for local coords */
	/* OGLXXX XXX fix this. */
  Matrix resultmatrix;
  objptr motherobj;
  ;
  /* good new way:
  motherobj = thisvfe->startle->motherbound->motherfve->mothershell->motherobj;
  transformpt(thisvfe->local_pos,scalefactor,thisvfe->local_pos);
  transformpt(thisvfe->local_pos,motherobj->xform,thisvfe->pos);
  */
  /* bad old way */
  motherobj = thisvfe->startle->motherbound->motherfve->mothershell->motherobj;
  transformpt(thisvfe->pos,scalefactor,thisvfe->pos);
  transformpt(thisvfe->pos,motherobj->invxform,thisvfe->local_pos);
}

  void
scale_evf(evfptr thisevf,Matrix scalefactor)
{
  glPushMatrix();

  scale_vfe(thisevf->le1->facevert,scalefactor);
  scale_vfe(thisevf->le2->facevert,scalefactor);

  glPopMatrix();
}

  void
scale_fve(fveptr thisfve,Matrix scalefactor)
{
  leptr thisle;
  vfeptr thisvfe;
  boundptr thisbnd;
  ;
  glPushMatrix();

  thisbnd = thisfve->boundlist->first.bnd;
  while (thisbnd != Nil)
  {
    thisle = thisbnd->lelist->first.le;
    do
    {
      scale_vfe(thisle->facevert,scalefactor);
      thisle = thisle->next;
    } while (thisle != thisbnd->lelist->first.le);
    thisbnd = thisbnd->next;
  }

  glPopMatrix();
}

  void
scale_shell(shellptr thishell,Matrix scalefactor)
{
  vfeptr thisvfe;
  ;
  glPushMatrix();

  for (thisvfe = thishell->vfelist->first.vfe;
       thisvfe != Nil; thisvfe = thisvfe->next)
    scale_vfe(thisvfe,scalefactor);

  glPopMatrix();
}

  void
scale_obj(objptr thisobj,Matrix scalefactor,Matrix invscalefactor)
{
/* old bad way
*/
  postmult_tree_by_xform(thisobj,scalefactor,invscalefactor);

/* new good way ...not ready yet
  apply_transform_to_tree(thisobj,scalefactor,invscalefactor);
*/
}

  void
scale_feature(featureptr thisfeature, vertype scalefactor, vertype scaleorigin)
{
  objptr motherobj;
  Matrix scalematrix,invscalematrix;
  ;
  motherobj = (objptr) find_largergrain(thisfeature,Obj_type);
  log_object_erase_bbox(motherobj,Useobjbbox,Loginoldbuffer);
  /* Set up a matrix to translate to the scaleorigin, scale by scalefactor, */
  /* and then translate back.*/
  glPushMatrix();

  glLoadMatrixf((GLfloat *) imatrix);
  glTranslatef(scaleorigin[vx], scaleorigin[vy], scaleorigin[vz]);
  glScalef(scalefactor[vx], scalefactor[vy], scalefactor[vz]);
  glTranslatef(-scaleorigin[vx], -scaleorigin[vy], -scaleorigin[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) scalematrix);

  /* we want scaling to happen in local coords */
/*
  multmatrix_4x4(motherobj->xform,scalematrix,scalematrix);
*/

  glLoadMatrixf((GLfloat *) imatrix);
  glTranslatef(scaleorigin[vx], scaleorigin[vy], scaleorigin[vz]);
  glScalef(1.0/scalefactor[vx], 1.0/scalefactor[vy], 1.0/scalefactor[vz]);
  glTranslatef(-scaleorigin[vx], -scaleorigin[vy], -scaleorigin[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) invscalematrix);
  
  glPopMatrix();

  switch (thisfeature->type_id)
  {
    case Vfe_type:
      break;
    case Evf_type:
      scale_evf((evfptr) thisfeature,scalematrix);
      break;
    case Fve_type:
      scale_fve((fveptr) thisfeature,scalematrix);
      break;
    case Shell_type:
      scale_shell((shellptr) thisfeature,scalematrix);
    case Obj_type:
      scale_obj((objptr) thisfeature,scalematrix,invscalematrix);
      break;
  }
}


/* Below stuff not needed if I use gl, as above. */

#if 0
  scalematrix[0][0] = scalefactor[vx];
  scalematrix[1][1] = scalefactor[vy];
  scalematrix[2][2] = scalefactor[vz];
  scalematrix[3][0] = -(scalefactor[vx] * scaleorigin[vx]) + scaleorigin[vx];
  scalematrix[3][1] = -(scalefactor[vy] * scaleorigin[vy]) + scaleorigin[vy];
  scalematrix[3][2] = -(scalefactor[vz] * scaleorigin[vz]) + scaleorigin[vz];

  scalematrix[0][0] = 1.0 / scalefactor[vx];
  scalematrix[1][1] = 1.0 / scalefactor[vy];
  scalematrix[2][2] = 1.0 / scalefactor[vz];
  scalematrix[3][0] = -(scalefactor[vx] * scaleorigin[vx]) + scaleorigin[vx];
  scalematrix[3][1] = -(scalefactor[vy] * scaleorigin[vy]) + scaleorigin[vy];
  scalematrix[3][2] = -(scalefactor[vz] * scaleorigin[vz]) + scaleorigin[vz];
#endif

  void
create_scalebbox(worldptr world)
{
  vfeptr thisvfe;
  evfptr thisevf;
  fveptr thisfve;
  ;
  /* scalebbox_obj is a global object that gets copied when primary */
  /* world objects get picked, so that they can be scaled.  */
  
  scalebbox_obj = create_cube(environment_world,100.0,100.0,100.0);

  scalebbox_obj->drawtechnique = draw_scalebbox_technique;
  scalebbox_obj->selectechnique = set_scalebbox_selectable;
  set_object_name(scalebbox_obj,"Scalebbox");
  add_property((featureptr) scalebbox_obj, transparent_prop);

  add_property((featureptr) scalebbox_obj, noshadow_prop);
  add_property((featureptr) scalebbox_obj, sectioninvisible_prop);
  add_property((featureptr) scalebbox_obj, selectinvisible_prop);
  add_property((featureptr) scalebbox_obj, pickedinvisible_prop);

  add_property((featureptr) scalebbox_obj, scalebbox_prop);
  del_property((featureptr) scalebbox_obj, visible_prop);
  thisvfe = First_obj_vfe(scalebbox_obj);
  while (thisvfe != Nil)
  {
    add_property((featureptr) thisvfe, scalebbox_prop);
    thisvfe = thisvfe->next;
  }
  thisevf = First_obj_evf(scalebbox_obj);
  while (thisevf != Nil)
  {
    add_property((featureptr) thisevf, scalebbox_prop);
    thisevf = thisevf->next;
  }
  thisfve = First_obj_fve(scalebbox_obj);
  while (thisfve != Nil)
  {
    add_property((featureptr) thisfve, scalebbox_prop);
    thisfve = thisfve->next;
  }
}

  void
create_sizebbox(worldptr world)
{
  vfeptr thisvfe;
  evfptr thisevf;
  fveptr thisfve;
  ;
  /* scalebbox_obj is a global object that gets copied when primary */
  /* world objects get picked, so that they can be scaled.  */
  
  scalebbox_obj = create_cube(environment_world,100.0,100.0,100.0);

  scalebbox_obj->drawtechnique = normal_draw_technique;
  scalebbox_obj->selectechnique = set_scalebbox_selectable;
  set_object_name(scalebbox_obj,"Sizebbox");
  add_property((featureptr) scalebbox_obj, transparent_prop);

  add_property((featureptr) scalebbox_obj, noshadow_prop);

  add_property((featureptr) scalebbox_obj, scalebbox_prop);
  del_property((featureptr) scalebbox_obj, visible_prop);
  thisvfe = First_obj_vfe(scalebbox_obj);
  while (thisvfe != Nil)
  {
    add_property((featureptr) thisvfe, scalebbox_prop);
    thisvfe = thisvfe->next;
  }
  thisevf = First_obj_evf(scalebbox_obj);
  while (thisevf != Nil)
  {
    add_property((featureptr) thisevf, scalebbox_prop);
    thisevf = thisevf->next;
  }
  thisfve = First_obj_fve(scalebbox_obj);
  while (thisfve != Nil)
  {
    add_property((featureptr) thisfve, scalebbox_prop);
    thisfve = thisfve->next;
  }
}

