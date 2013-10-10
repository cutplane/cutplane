
/* FILE: transform.c */

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                                                                        */
/*        Transform Routines for the Cutplane Editor                      */
/*                                                                        */
/* Authors: LJE and WAK                           Date: November 17,1990  */
/* Version: 1.0                                                           */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#define TRANSFORMMODULE

#include <config.h>
#include <stdio.h>

#include <platform_gl.h>
#include <math.h>
#include <limits.h>

#include <cutplane.h>

#include <pick.h>
#include <math3d.h>
#include <globals.h>

void
transformpt(Coord *thept, Matrix xform, Coord * xformedpt)
{
  register Coord *xformptr;
  vertype tempxformedpt;
  ;
  xformptr = (Coord *) xform;
  if (xformedpt != thept)
  {
    xformedpt[0] = thept[0]*xformptr[0] + thept[1]*xformptr[4] +
                   thept[2]*xformptr[8] + thept[3]*xformptr[12];
    
    xformedpt[1] = thept[0]*xformptr[1] + thept[1]*xformptr[5] +
                   thept[2]*xformptr[9] + thept[3]*xformptr[13];
    
    xformedpt[2] = thept[0]*xformptr[2] + thept[1]*xformptr[6] +
                   thept[2]*xformptr[10] + thept[3]*xformptr[14];
    
    xformedpt[3] = thept[0]*xformptr[3] + thept[1]*xformptr[7] +
                   thept[2]*xformptr[11] + thept[3]*xformptr[15];
  }
  else
  {
    tempxformedpt[0] = thept[0]*xformptr[0] + thept[1]*xformptr[4] +
                       thept[2]*xformptr[8] + thept[3]*xformptr[12];
    
    tempxformedpt[1] = thept[0]*xformptr[1] + thept[1]*xformptr[5] +
                       thept[2]*xformptr[9] + thept[3]*xformptr[13];
    
    tempxformedpt[2] = thept[0]*xformptr[2] + thept[1]*xformptr[6] +
                       thept[2]*xformptr[10] + thept[3]*xformptr[14];
    
    tempxformedpt[3] = thept[0]*xformptr[3] + thept[1]*xformptr[7] +
                       thept[2]*xformptr[11] + thept[3]*xformptr[15];
    
    xformedpt[0] = tempxformedpt[0];
    xformedpt[1] = tempxformedpt[1];
    xformedpt[2] = tempxformedpt[2];
    xformedpt[3] = tempxformedpt[3];
  }
}

void
transformpts(long npts, Coord *pts, Matrix xform, Coord *xformedpts)
{
  register Coord *xformptr;
  register long i;
  vertype tempxformedpt;
  ;
  xformptr = (GLfloat *) xform;
  if (xformedpts != pts)
  {
    for (i = 0; i < npts; ++i)
    {
      *(xformedpts++) = pts[0]*xformptr[0] + pts[1]*xformptr[4] +
	                pts[2]*xformptr[8] + pts[3]*xformptr[12];

      *(xformedpts++) = pts[0]*xformptr[1] + pts[1]*xformptr[5] +
	                pts[2]*xformptr[9] + pts[3]*xformptr[13];

      *(xformedpts++) = pts[0]*xformptr[2] + pts[1]*xformptr[6] +
	                pts[2]*xformptr[10] + pts[3]*xformptr[14];

      *(xformedpts++) = pts[0]*xformptr[3] + pts[1]*xformptr[7] +
	                pts[2]*xformptr[11] + pts[3]*xformptr[15];
      pts += 4;
    }
  }
  else
  {
    for (i = 0; i < npts; ++i)
    {
      tempxformedpt[0] = pts[0]*xformptr[0] + pts[1]*xformptr[4] +
	                 pts[2]*xformptr[8] + pts[3]*xformptr[12];

      tempxformedpt[1] = pts[0]*xformptr[1] + pts[1]*xformptr[5] +
	                 pts[2]*xformptr[9] + pts[3]*xformptr[13];

      tempxformedpt[2] = pts[0]*xformptr[2] + pts[1]*xformptr[6] +
	                 pts[2]*xformptr[10] + pts[3]*xformptr[14];

      tempxformedpt[3] = pts[0]*xformptr[3] + pts[1]*xformptr[7] +
	                 pts[2]*xformptr[11] + pts[3]*xformptr[15];

      *(xformedpts++) = tempxformedpt[0];
      *(xformedpts++) = tempxformedpt[1];
      *(xformedpts++) = tempxformedpt[2];
      *(xformedpts++) = tempxformedpt[3];
      pts += 4;
    }
  }
}

void
transform_shellvfes(shellptr thishell, Coord *xform)
{
  vfeptr thisvfe;
  Coord *pos,*local_pos;
  ;
  thisvfe = thishell->vfelist->first.vfe;
  while (thisvfe)
  {
    pos = thisvfe->pos;
    local_pos = thisvfe->local_pos;
    pos[0] = local_pos[0]*xform[0] + local_pos[1]*xform[4] +
	local_pos[2]*xform[8] + local_pos[3]*xform[12];
    
    pos[1] = local_pos[0]*xform[1] + local_pos[1]*xform[5] +
	local_pos[2]*xform[9] + local_pos[3]*xform[13];
    
    pos[2] = local_pos[0]*xform[2] + local_pos[1]*xform[6] +
	local_pos[2]*xform[10] + local_pos[3]*xform[14];

    pos[3] = local_pos[0]*xform[3] + local_pos[1]*xform[7] +
	local_pos[2]*xform[11] + local_pos[3]*xform[15];

    thisvfe = thisvfe->next;
  }
}

void
transform_shellnormals(shellptr thishell, Coord *xform)
{
  fveptr thisfve;
  Coord *facenorm,*local_facenorm;
  ;
  thisfve = thishell->fvelist->first.fve;
  while (thisfve)
  {
    facenorm = thisfve->facenorm;
    local_facenorm = thisfve->local_facenorm;
    facenorm[0] = local_facenorm[0]*xform[0] + local_facenorm[1]*xform[4] +
	          local_facenorm[2]*xform[8] + local_facenorm[3]*xform[12];
    
    facenorm[1] = local_facenorm[0]*xform[1] + local_facenorm[1]*xform[5] +
	          local_facenorm[2]*xform[9] + local_facenorm[3]*xform[13];
    
    facenorm[2] = local_facenorm[0]*xform[2] + local_facenorm[1]*xform[6] +
	          local_facenorm[2]*xform[10] + local_facenorm[3]*xform[14];
    
    facenorm[3] = local_facenorm[0]*xform[3] + local_facenorm[1]*xform[7] +
	          local_facenorm[2]*xform[11] + local_facenorm[3]*xform[15];

    thisfve = thisfve->next;
  }
}

void
transform_shell_cutpts(shellptr thishell, Coord *xform)
{
  evfptr thisevf;
  Coord *pos,*local_pos;
  ;
  thisevf = thishell->evflist->first.evf;
  while (thisevf)
  {
    pos = thisevf->cutpt;
    local_pos = thisevf->local_cutpt;
    pos[0] = local_pos[0]*xform[0] + local_pos[1]*xform[4] +
	     local_pos[2]*xform[8] + local_pos[3]*xform[12];
    
    pos[1] = local_pos[0]*xform[1] + local_pos[1]*xform[5] +
	     local_pos[2]*xform[9] + local_pos[3]*xform[13];
    
    pos[2] = local_pos[0]*xform[2] + local_pos[1]*xform[6] +
	     local_pos[2]*xform[10] + local_pos[3]*xform[14];

    pos[3] = local_pos[0]*xform[3] + local_pos[1]*xform[7] +
	     local_pos[2]*xform[11] + local_pos[3]*xform[15];

    thisevf = thisevf->next;
  }
}

  void
transform_shell(shellptr thishell, Matrix xformatrix)
{
  transform_shellvfes(thishell, (GLfloat *) xformatrix);
  transform_shellnormals(thishell,(GLfloat *) xformatrix);
}

void
transform_shellvfes_reverse(shellptr thishell, Coord *invxform)
{
  vfeptr thisvfe;
  Coord *pos,*local_pos;
  ;
  thisvfe = thishell->vfelist->first.vfe;
  while (thisvfe)
  {
    pos = thisvfe->pos;
    local_pos = thisvfe->local_pos;
    local_pos[0] = pos[0]*invxform[0] + pos[1]*invxform[4] +
	           pos[2]*invxform[8] + pos[3]*invxform[12];
    
    local_pos[1] = pos[0]*invxform[1] + pos[1]*invxform[5] +
	           pos[2]*invxform[9] + pos[3]*invxform[13];
    
    local_pos[2] = pos[0]*invxform[2] + pos[1]*invxform[6] +
	           pos[2]*invxform[10] + pos[3]*invxform[14];

    local_pos[3] = pos[0]*invxform[3] + pos[1]*invxform[7] +
	           pos[2]*invxform[11] + pos[3]*invxform[15];

    thisvfe = thisvfe->next;
  }
}

void
transform_shellnormals_reverse(shellptr thishell, Coord *invxform)
{
  fveptr thisfve;
  Coord *facenorm,*local_facenorm;
  ;
  thisfve = thishell->fvelist->first.fve;
  while (thisfve)
  {
    facenorm = thisfve->facenorm;
    local_facenorm = thisfve->local_facenorm;

    local_facenorm[0] = facenorm[0]*invxform[0] + facenorm[1]*invxform[4] +
	                facenorm[2]*invxform[8] + facenorm[3]*invxform[12];
    
    local_facenorm[1] = facenorm[0]*invxform[1] + facenorm[1]*invxform[5] +
	                facenorm[2]*invxform[9] + facenorm[3]*invxform[13];
    
    local_facenorm[2] = facenorm[0]*invxform[2] + facenorm[1]*invxform[6] +
	                facenorm[2]*invxform[10] + facenorm[3]*invxform[14];
    
    local_facenorm[3] = facenorm[0]*invxform[3] + facenorm[1]*invxform[7] +
	                facenorm[2]*invxform[11] + facenorm[3]*invxform[15];

    thisfve = thisfve->next;
  }
}

void
transform_shell_cutpts_reverse(shellptr thishell, Coord *invxform)
{
  evfptr thisevf;
  Coord *pos,*local_pos;
  ;
  thisevf = thishell->evflist->first.evf;
  while (thisevf)
  {
    pos = thisevf->cutpt;
    local_pos = thisevf->local_cutpt;
    local_pos[0] = pos[0]*invxform[0] + pos[1]*invxform[4] +
	           pos[2]*invxform[8] + pos[3]*invxform[12];
    
    local_pos[1] = pos[0]*invxform[1] + pos[1]*invxform[5] +
	           pos[2]*invxform[9] + pos[3]*invxform[13];
    
    local_pos[2] = pos[0]*invxform[2] + pos[1]*invxform[6] +
	           pos[2]*invxform[10] + pos[3]*invxform[14];

    local_pos[3] = pos[0]*invxform[3] + pos[1]*invxform[7] +
	           pos[2]*invxform[11] + pos[3]*invxform[15];

    thisevf = thisevf->next;
  }
}

  void
transform_shell_reverse(shellptr thishell, Matrix invxform)
{
  transform_shellvfes_reverse(thishell, (Coord *) invxform);
  transform_shellnormals_reverse(thishell, (Coord *) invxform);
}

  void
transform_obj_reverse(objptr thisobj, Matrix invxform)
{
  shellptr thishell;
  ;
  thishell = thisobj->shellist->first.shell;
  if (thishell != Nil)
  {
    while (thishell != Nil)
    {
      transform_shell_reverse(thishell, invxform);
      thishell = thishell->next;
    }
  }
  else
  {
    copypos3d(origin,thisobj->objbbox.mincorner);
    copypos3d(origin,thisobj->objbbox.maxcorner);
  }  
}

  void
transform_obj_evf_cutpts(objptr thisobj)
{
  shellptr thishell;
  ;
  thishell = First_obj_shell(thisobj);
  while (thishell != Nil)
  {
    transform_shell_cutpts(thishell,(GLfloat *) thisobj->xform);
    thishell = thishell->next;
  }
}

  void
transform_obj(objptr thisobj)
{
  shellptr thishell;
  ;
  thishell = First_obj_shell(thisobj);
  if (thishell != Nil)
  {
    while (thishell != Nil)
    {
      transform_shellvfes(thishell,(GLfloat *) thisobj->xform);
      transform_shellnormals(thishell,(GLfloat *) thisobj->xform);
      thishell = thishell->next;
    }
  }
  else
  {
    copypos3d(origin,thisobj->objbbox.mincorner);
    copypos3d(origin,thisobj->objbbox.maxcorner);
  }  
}

void
transform_obj_tree(objptr thisobj)
{
  objptr grouproot;
  ;
  grouproot = get_unlinked_ancestor(thisobj);
  apply_to_descendants(grouproot,transform_obj,Nil);
}

/* actually change the local coordinates according to the transform and */
/* set the transforms to identity. */

  void
permanent_transform_obj(objptr thisobj)
{
  vfeptr thisvfe;
  fveptr thisfve;
  shellptr thishell;
  ;
  thishell = thisobj->shellist->first.shell;
  while (thishell)
  {
    thisvfe = thishell->vfelist->first.vfe;
    while(thisvfe)
    {
      transformpt(thisvfe->local_pos,thisobj->xform,thisvfe->pos);
      copypos3d(thisvfe->pos,thisvfe->local_pos);
      thisvfe = thisvfe->next;
    }
    
    thisfve = thishell->fvelist->first.fve;
    while (thisfve != Nil)
    {
      transformpt(thisfve->local_facenorm,thisobj->xform,
			 thisfve->facenorm);
      copyvec3d(thisfve->facenorm,thisfve->local_facenorm);
      thisfve = thisfve->next;
    }
    thishell = thishell->next;
  }

  copy_matrix(imatrix,thisobj->xform);
  copy_matrix(imatrix,thisobj->invxform);
  set_3d_objbbox(thisobj);
}


  void
apply_invtransform(objptr thisobj,Matrix invtransform)
{
  glPushMatrix();
  glLoadMatrixf((GLfloat *) invtransform);
  glMultMatrixf((GLfloat *) thisobj->invxform);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) thisobj->invxform);
  glPopMatrix();
}

  void
apply_transform(objptr thisobj,Matrix newtransform)
{
  glPushMatrix();
  glLoadMatrixf((GLfloat *) thisobj->xform);
  glMultMatrixf((GLfloat *) newtransform);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) thisobj->xform);
  glPopMatrix();  
}

  void
apply_transform_to_tree(objptr thisobj,Matrix newtransform,Matrix invtransform)
{
  objptr grouproot;
  ;
  grouproot = get_unlinked_ancestor(thisobj);
  apply_to_descendants(grouproot,apply_transform,newtransform);
  apply_to_descendants(grouproot,apply_invtransform,invtransform);
}

  void
apply_postmultinv(objptr thisobj, Matrix invxform)
{
  glPushMatrix();
  glLoadMatrixf((GLfloat *) thisobj->invxform);
  glMultMatrixf((GLfloat *) invxform);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) thisobj->invxform);
  glPopMatrix();
}

  void
apply_postmult(objptr thisobj, Matrix xform)
{
  glPushMatrix();
  glLoadMatrixf((GLfloat *) xform);
  glMultMatrixf((GLfloat *) thisobj->xform);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) thisobj->xform);
  glPopMatrix();
}

  void
postmult_tree_by_xform(objptr thisobj,Matrix newtransform,Matrix invtransform)
{
  objptr grouproot;
  ;
  grouproot = get_unlinked_ancestor(thisobj);
  apply_to_descendants(grouproot,apply_postmult,newtransform);
  apply_to_descendants(grouproot,apply_postmultinv,invtransform);
}

