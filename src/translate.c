
/* FILE: translate.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*          Feature Translation Routines for the Cutplane Editor          */
/*                                                                        */
/* Author: WAK                                       Date: April 30,1990  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 William Kessler and Laurence Edwards.*/
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define TRANSLATEMODULE

#include <config.h>
#include <stdio.h>

#include <platform_gl.h>
#include <math.h>

#include <cutplane.h>		/* cutplane.h includes user.h */
#include <pick.h>		/* for access to global_pickedlist */
#include <globals.h>
#include <grafx.h>
#include <update.h>

/* some stuff from vertex.c should clean this up */

  void
translatevert(register vfeptr thisvfe,Matrix the_invxform,
	      register vertype delta)
{
  vertype newdelta;
  register float *posptr;
  ;
  if (thisvfe->translated)
    return;

  thisvfe->translated = TRUE;

  delta[vw] = 0.0;	/* delta should be a vector not a position */
  posptr = thisvfe->pos;
  *(posptr++) += delta[vx];
  *(posptr++) += delta[vy];
  *(posptr++) += delta[vz];
  *posptr = 1.0;

  /* for shells might as well just change the xform matrix and then */
  /* transform shell since we are doing a transform for each vertex any how */
  /* put delta's into shell's xformed space.. */
  transformpt(delta,the_invxform,newdelta);


  posptr = thisvfe->local_pos;
  *(posptr++) += newdelta[vx];
  *(posptr++) += newdelta[vy];
  *(posptr++) += newdelta[vz];
  *posptr = 1.0;
}

  void
clear_vertranslate(vfeptr thisvfe)
{
  thisvfe->translated = FALSE;
}

  void
clear_shelltranslates(shellptr thishell)
{
  vfeptr thisvfe;
  ;
  for (thisvfe = thishell->vfelist->first.vfe;
       thisvfe != Nil; thisvfe = thisvfe->next)
    clear_vertranslate(thisvfe);
}

  void
clear_feature_translates(featureptr thisfeature)
{
  shellptr thishell;
  fveptr thisfve;
  vfeptr thisvfe;
  leptr thisle,firstle;
  boundptr thisbound;
  ;
  switch (thisfeature->type_id)
  {
  case Vfe_type:
    clear_vertranslate(((vfeptr) thisfeature));
    break;
  case Evf_type:
    clear_vertranslate(((evfptr) thisfeature)->le1->facevert);
    clear_vertranslate(((evfptr) thisfeature)->le2->facevert);
    break;
  case Fve_type:
    thisfve = (fveptr) thisfeature;
    for (thisbound = thisfve->boundlist->first.bnd;
	 thisbound != Nil; thisbound = thisbound->next)
    {
      firstle = thisbound->lelist->first.le;
      thisle = firstle;
      do
      {
	clear_vertranslate(thisle->facevert);
	thisle = thisle->next;
      } while (thisle != firstle);
    }
    break;
  case Shell_type:
    for (thisvfe = ((shellptr) thisfeature)->vfelist->first.vfe;
	 thisvfe != Nil; thisvfe = thisvfe->next)
      clear_vertranslate(thisvfe);
    break;
  case Obj_type:
    for (thishell = First_obj_shell(((objptr) thisfeature)); thishell != Nil;
	 thishell = thishell->next)
      for (thisvfe = thishell->vfelist->first.vfe;
	   thisvfe != Nil; thisvfe = thisvfe->next)
	clear_vertranslate(thisvfe);
    break;
  }
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			GLOBAL VARIABLES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
translate_vfe_core(vfeptr thevert, shellptr mothershell, vertype delta,
		   GLboolean useflags)
{
  vertype newdelta;
  register float *posptr;
  ;
  if (useflags)
  {
    if (thevert->translated)
      return;
    thevert->translated = TRUE;	/* Set as translated if not already set */
  }
  delta[vw] = 0.0;	/* delta should be a vector not a position */
  posptr = thevert->pos;
  *(posptr++) += delta[vx];
  *(posptr++) += delta[vy];
  *(posptr++) += delta[vz];
  *posptr = 1.0;
  /* for shells might as well just change the xform matrix and then */
  /* transform shell since we are doing a transform for each vertex any how */
  if (thevert->startle != Nil)	/* put delta's into shell's xformed space.. */
  {
    transformpt(delta,mothershell->motherobj->invxform,newdelta);
  }
  posptr = thevert->local_pos;
  *(posptr++) += newdelta[vx];
  *(posptr++) += newdelta[vy];
  *(posptr++) += newdelta[vz];
  *posptr = 1.0;
}

void 
translate_surface(surfptr thisurf, vertype delta, GLboolean useflags)
{
  fveptr thisfve;
  boundptr thisbound;
  elemptrptr ptr2fve;
  leptr firstle,thisle;
  shellptr mothershell;
  ;
  ptr2fve = thisurf->fvelist->first.ep;
  mothershell = ((fveptr)  ptr2fve->thiselem)->mothershell;
  while (ptr2fve)
  {
    thisfve = (fveptr) ptr2fve->thiselem;
    for (thisbound = thisfve->boundlist->first.bnd;
	 thisbound != Nil; thisbound = thisbound->next)
    {
      firstle = thisbound->lelist->first.le;
      thisle = firstle;
      do
      {
	translate_vfe_core(thisle->facevert,mothershell,delta,useflags);
	thisle = thisle->next;
      } while (thisle != firstle);
    }
    ptr2fve = ptr2fve->next;
  }
}

/* Fix collision with same name routine in vertex.c! */

  void
translate_vfe(vfeptr thisvfe,vertype delta, GLboolean useflags)
{
  shellptr mothershell;
  ;
  mothershell = (shellptr) find_largergrain((featureptr) thisvfe,Shell_type);
  translate_vfe_core(thisvfe,mothershell,delta,useflags);
  
  log_update((featureptr) mothershell,(Evfeqnsflag | Cutevfsflag |
	     Makecutsectionflag | Findnormalsflag | Computebboxflag |
	     Recomputebackfaceflag | Logerasebboxflag),
	     Immediateupdate);
}

  void
translate_evf(evfptr thisevf, vertype delta, GLboolean useflags)
{
  shellptr mothershell;
  surfptr surf1,surf2;
  ;
  mothershell = (shellptr) find_largergrain((featureptr) thisevf, Shell_type);
  surf1 = thisevf->le1->motherbound->motherfve->mothersurf;
  surf2 = thisevf->le2->motherbound->motherfve->mothersurf;
  if ((surf1 == Nil) || (surf2 != surf1))
  {
    translate_vfe_core(thisevf->le1->facevert,mothershell,delta,useflags);
    translate_vfe_core(thisevf->le2->facevert,mothershell,delta,useflags);
  }
  else
    translate_surface(surf1,delta,useflags);

  log_update((featureptr) mothershell,(Evfeqnsflag | Cutevfsflag |
	     Makecutsectionflag | Findnormalsflag | Computebboxflag |
	     Recomputebackfaceflag | Logerasebboxflag),
	     Immediateupdate);
}

  void
translate_fve(fveptr thisfve, vertype delta, GLboolean useflags)
{
  shellptr mothershell;
  leptr firstle,thisle;
  boundptr thisbound;
  ;
  mothershell = (shellptr) find_largergrain((featureptr) thisfve,Shell_type);
  if (thisfve->mothersurf == Nil)
    for (thisbound = thisfve->boundlist->first.bnd;
	 thisbound != Nil; thisbound = thisbound->next)
    {
      firstle = thisbound->lelist->first.le;
      thisle = firstle;
      do
      {
	translate_vfe_core(thisle->facevert,mothershell,delta,useflags);
	thisle = thisle->next;
      } while (thisle != firstle);
    }
  else
    translate_surface(thisfve->mothersurf,delta,useflags);

  log_update((featureptr) mothershell,(Evfeqnsflag | Cutevfsflag |
             Makecutsectionflag | Findnormalsflag |
	     Computebboxflag | Logerasebboxflag |
	     Recomputebackfaceflag), Immediateupdate);
}


  void
translate_shell(shellptr thishell,vertype delta,GLboolean useflags)
{
  vfeptr thisvfe;
  ;
  
  for (thisvfe = thishell->vfelist->first.vfe;
       thisvfe != Nil; thisvfe = thisvfe->next)
    translate_vfe_core(thisvfe,thishell,delta,useflags);
  log_update((featureptr) thishell,(Evfeqnsflag | Cutevfsflag |
				    Makecutsectionflag | Computeplanedistsflag |
				    Recomputebackfaceflag | Computebboxflag),
	     Immediateupdate);
}

  static void
apply_rotate(objptr thisobj, float angle, char axis)
{
  glPushMatrix();
  glLoadMatrixf((GLfloat *) thisobj->xform);
	/* OGLXXX You can do better than this. */
  glRotatef(angle, (axis)=='x'||(axis)=='X', (axis)=='y'||(axis)=='Y', (axis)=='z'||(axis)=='Z');
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) thisobj->xform);

  glLoadMatrixf((GLfloat *) thisobj->invxform);
	/* OGLXXX You can do better than this. */
  glRotatef(-angle, (axis)=='x'||(axis)=='X', (axis)=='y'||(axis)=='Y', (axis)=='z'||(axis)=='Z');
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) thisobj->invxform);
  glPopMatrix();
}

  static void
apply_scaling(objptr thisobj, vertype scaling)
{
  glPushMatrix();
  glLoadMatrixf((GLfloat *) thisobj->xform);
  glScalef(scaling[vx], scaling[vy], scaling[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) thisobj->xform);

  glLoadMatrixf((GLfloat *) thisobj->invxform);
  glScalef(-scaling[vx], -scaling[vy], -scaling[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) thisobj->invxform);
  glPopMatrix();
}

  static void
translate_obj_engine(objptr thisobj, vertype delta,
		     GLboolean *a_visible_translated)
{
  shellptr thishell;
  objptr recurse_obj;
  ;
#if 0
oldway
  glPushMatrix();
  glLoadMatrixf((GLfloat *) thisobj->xform);
  glTranslatef(delta[vx], delta[vy], delta[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) thisobj->xform);
  glLoadMatrixf((GLfloat *) thisobj->invxform);
  glTranslatef(-delta[vx], -delta[vy], -delta[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) thisobj->invxform);
  glPopMatrix();
#endif


  /* we need to do this in here because if an object has drawtechnique == */
  /* nodraw_technique then none of its children (which get translated) */
  /* will be erased/redrawn even if they have normaldraw_technique... */
  if (has_property((featureptr) thisobj,visible_prop))
    log_object_erase_bbox(thisobj,Useobjbbox,Loginoldbuffer);

  glPushMatrix();
  glLoadMatrixf((GLfloat *) imatrix);
  glTranslatef(delta[vx], delta[vy], delta[vz]);
  glMultMatrixf((GLfloat *) thisobj->xform);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) thisobj->xform);



  glLoadMatrixf((GLfloat *) thisobj->invxform);
  glTranslatef(-delta[vx], -delta[vy], -delta[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) thisobj->invxform);

  glPopMatrix();

  thisobj->timestamp = cutplane_time;

  /* now recursively do the same thing on all the children */
  recurse_obj = thisobj->children->first.obj;
  while (recurse_obj != Nil)
  {
    translate_obj_engine(recurse_obj,delta, a_visible_translated);
    recurse_obj = recurse_obj->next;
  }
  if (has_property((featureptr) thisobj,visible_prop))
    *a_visible_translated = TRUE;
}

  void
translate_obj(objptr thisobj, vertype delta)
{
  objptr grouproot;
  GLboolean a_visible_translated = FALSE;
  ;
  grouproot = get_unlinked_ancestor(thisobj);
  translate_obj_engine(grouproot,delta, &a_visible_translated);
  if (a_visible_translated)
    log_update((featureptr) grouproot, Transformflag | Evfeqnsflag |
	       Cutevfsflag | Makecutsectionflag | Computeplanedistsflag |
	       Computebboxflag | Redrawflag | Recomputebackfaceflag,
	       Immediateupdate);
}

  void
translate_obj_quick(objptr thisobj,vertype delta)
{
  objptr grouproot;
  GLboolean a_visible_translated = FALSE;
  ;
  grouproot = get_unlinked_ancestor(thisobj);
  translate_obj_engine(grouproot,delta, &a_visible_translated);
  if (has_property((featureptr) grouproot,visible_prop))
    log_update((featureptr) grouproot, Transformflag | Evfeqnsflag |
	       Transformcutevfsflag | Computeplanedistsflag |
	       Computebboxflag | Redrawflag | Recomputebackfaceflag,
	       Immediateupdate);
}

  void
translate_feature(featureptr thisfeature,vertype delta, GLboolean useflags)
{
  objptr motherobj;
  ;
  motherobj = (objptr) find_largergrain(thisfeature,Obj_type);
  switch (thisfeature->type_id)
    {
    case Vfe_type:
      log_object_erase_bbox(motherobj,Usegroupbbox,Loginoldbuffer);
      translate_vfe((vfeptr) thisfeature,delta,useflags);
      break;
    case Evf_type:
      log_object_erase_bbox(motherobj,Usegroupbbox,Loginoldbuffer);
      translate_evf((evfptr) thisfeature,delta,useflags);
      break;
    case Fve_type:
      log_object_erase_bbox(motherobj,Usegroupbbox,Loginoldbuffer);
      translate_fve((fveptr) thisfeature,delta,useflags);
      break;
    case Shell_type:
      log_object_erase_bbox(motherobj,Usegroupbbox,Loginoldbuffer);
      translate_obj(((shellptr) thisfeature)->motherobj,delta);
      break;
    case Obj_type:
      translate_obj((objptr) thisfeature,delta);
      break;
  }

  if ((thisfeature->type_id != Shell_type) &&
      (thisfeature->type_id !=  Obj_type))
    log_update(thisfeature,Transformflag | Evfeqnsflag | Cutevfsflag |
	       Makecutsectionflag | Findnormalsflag | Computeplanedistsflag |
	       Computebboxflag | Logerasebboxflag | Cleartranslatesflag |
	       Recomputebackfaceflag, Immediateupdate);
    
}

  void
clear_property_translates(property thisproperty)
{
  proptr thisproptr;
  ;
  thisproptr = (working_world->world_proplists[(int) thisproperty])->first.prop;
  while (thisproptr != Nil)
  {
    clear_feature_translates(thisproptr->owner);
    thisproptr = thisproptr->next;
  }
}

