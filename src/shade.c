
/* FILE: shade.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/* Simple flat shading routines for the godly Personal Iris Zbuffer       */
/*                                                                        */
/* Author: LJE,WAK                                Date: October 18,1990   */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define SHADEMODULE

#include <config.h>

#include <platform_gl.h>
#include <platform_glu.h>
#include <math.h>

#include <cutplane.h>

#include <grafx.h>
#include <shade.h>
#include <normals.h>
#include <math3d.h>
#include <vertex.h>

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			GLOBAL VARIABLES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

static int thiszbuffcolor = 0;
double acos_table[Maxacostable+1];

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			SHADED RENDERING UTILITIES		        */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
init_acos_table(void)
{
  int i;
  double val;
  ;
  for (i = 0; i <= Maxacostable; ++i)
  {
    val = (double) i/(double) Maxacostable;
    acos_table[i] = Acos(val);
  }
}

  void
make_face_into_light(fveptr thisfve,double brightness)
{
  set_property((featureptr) thisfve,luminous_prop,brightness);
  /* need to record something about brightness of light. Color comes from */
  /* face's actual color. */
  add_property((featureptr) thisfve->mothershell,luminous_prop);
}

  void
compute_reflected_color(vertype facenorm, colortype reflectivity,
			colortype reflectedcolor)
{
  static colortype ambientlight = {80,80,80};
  static colortype directlight1 = {125,125,125};
  static colortype directlight2 = {50,50,50};
  static float fract = 1.0/255.0;
  float directfract1,directfract2;
  /* using light vector <1,1,1> */
  static vertype lightvector1 = {1.0/1.732050808,
				   1.0/1.732050808,1.0/1.732050808};
  /* using light vector <-1,1,1> */
  static vertype lightvector2 = {-1.0/1.732050808,
				   1.0/1.732050808,1.0/1.732050808};
  ;
  if (facenorm[vy] < 0.0)
  {
    reflectedcolor[0] = ambientlight[0]*reflectivity[0]*fract;
    reflectedcolor[1] = ambientlight[1]*reflectivity[1]*fract;
    reflectedcolor[2] = ambientlight[2]*reflectivity[2]*fract;
  }
  else
  {
#ifdef moronway    
    reflectedcolor[0] = (facenorm[vy] * (GLfloat) directlight[0] +
			 (GLfloat) ambientlight[0]) *
			   (fract * (GLfloat) reflectivity[0]);

    reflectedcolor[1] = (facenorm[vy] * (GLfloat) directlight[1] +
			 (GLfloat) ambientlight[1]) *
			   (fract * (GLfloat) reflectivity[1]);

    reflectedcolor[2] = (facenorm[vy] * (GLfloat) directlight[2] +
			 (GLfloat) ambientlight[2]) *
			   (fract * reflectivity[2]);
#endif
    directfract1 = enforce_flimits(dotvecs(facenorm,lightvector1),0.0,1.0);
    directfract2 = enforce_flimits(dotvecs(facenorm,lightvector2),0.0,1.0);
    reflectedcolor[0] = (directfract1 * directlight1[vx] +
			 directfract2 * directlight2[vx] +
			 (GLfloat) ambientlight[0]) *
			   (fract * (GLfloat) reflectivity[0]);

    reflectedcolor[1] = (directfract1 * directlight1[vy] +
			 directfract2 * directlight2[vy] +
			 (GLfloat) ambientlight[1]) *
			   (fract * (GLfloat) reflectivity[1]);

    reflectedcolor[2] = (directfract1 * directlight1[vz] +
			 directfract2 * directlight2[vz] +
			 (GLfloat) ambientlight[2]) *
			   (fract * reflectivity[2]);
    
  }
}

void
compute_highlighted_face_color(vertype facenorm, colortype reflectivity,
			       colortype totalcolor)
{
  static colortype ambientlight = {80,80,80};
  static colortype directlight = {175,175,175};
  static colortype emittedlight = {70,70,70};
  static float fract = 1.0/255.0;
  ;
  if (facenorm[vy] < 0.0)
  {
    totalcolor[0] = ambientlight[0]*reflectivity[0]*fract;
    totalcolor[1] = ambientlight[1]*reflectivity[1]*fract;
    totalcolor[2] = ambientlight[2]*reflectivity[2]*fract;
  }
  else
  {
    totalcolor[0] = (facenorm[vy] * (GLfloat) directlight[0] +
			 (GLfloat) ambientlight[0]) *
			   (fract * (GLfloat) reflectivity[0]);

    totalcolor[1] = (facenorm[vy] * (GLfloat) directlight[1] +
			 (GLfloat) ambientlight[1]) *
			   (fract * (GLfloat) reflectivity[1]);

    totalcolor[2] = (facenorm[vy] * (GLfloat) directlight[2] +
			 (GLfloat) ambientlight[2]) *
			   (fract * reflectivity[2]);
  }
  totalcolor[0] += emittedlight[0];
  totalcolor[1] += emittedlight[1];
  totalcolor[2] += emittedlight[2];
  if (totalcolor[0] > 255)
    totalcolor[0] = 255;
  if (totalcolor[1] > 255)
    totalcolor[1] = 255;
  if (totalcolor[2] > 255)
    totalcolor[2] = 255;
}

  void
draw_flatshaded_face(fveptr thisfve)
{
  leptr firstle,thisle;
  boundptr outerbnd,thisinnerbnd;
  GLboolean arewelit;
  GLUtesselator *newTessObj;
  ;
	/* OGLXXX
	 * special cases for polygons:
	 * 	independant quads: use GL_QUADS
	 * 	independent triangles: use GL_TRIANGLES
	 */
  //  glEnable(GL_LIGHTING);
  arewelit = glIsEnabled(GL_LIGHTING);

  // newTessObj = gluNewTess();
  // gluTessBeginPolygon(newTessObj,NULL);
  //gluTessBeginContour(newTessObj);
  glBegin(GL_POLYGON);
  /* glColor3f(1.0F,1.0F,1.0F); */
  glNormal3fv(thisfve->facenorm);
  outerbnd = thisfve->boundlist->first.bnd;
  firstle = thisle = outerbnd->lelist->first.le;
  do
  {
    glVertex3fv(thisle->facevert->pos);
    // gluTessVertex(newTessObj, thisle->facevert->pos, thisle->facevert->pos);
    thisle = thisle->next;
  } while (thisle != outerbnd->lelist->first.le);
  //gluTessVertex(newTessObj, thisle->facevert->pos, thisle->facevert->pos);
  glVertex3fv(thisle->facevert->pos); /* draw back to first again */
  
  thisinnerbnd = thisfve->boundlist->first.bnd->next;
  while (thisinnerbnd != Nil) {
    //gluTessEndContour(newTessObj);
    //gluTessBeginContour(newTessObj);
    thisle = thisinnerbnd->lelist->first.le;
    do
      {
	glVertex3fv(thisle->facevert->pos);
	//gluTessVertex(newTessObj, thisle->facevert->pos,
	//thisle->facevert->pos);
	thisle = thisle->next;
      } while (thisle != thisinnerbnd->lelist->first.le);
    /* draw back to first vert of this bound */
    glVertex3fv(thisle->facevert->pos);
    //  gluTessVertex(newTessObj, thisle->facevert->pos,
    // thisle->facevert->pos);
    thisinnerbnd = thisinnerbnd->next;
  }

  glEnd();	/* this should be sufficient to draw back to first ver */
  //gluTessEndContour(newTessObj);
  //gluTessEndPolygon(newTessObj);
  //gluDeleteTess(newTessObj);
}

  void
draw_flatshaded_shell(shellptr thishell, stateptr state)
{
  colorptr objcolor,facecolor = Nil;
  fveptr thisfve;
  ;
  /* if transparent don't draw faces */
  if (thishell->featureflags & Transparentflag)
    return;
  
  objcolor = thishell->motherobj->thecolor;

	/* OGLXXX
	 * lmcolor: if LMC_NULL,  use:
	 * glDisable(GL_COLOR_MATERIAL);
	 */
  //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glColor4f((((GLfloat) objcolor[0]) / 255.0F),
	    (((GLfloat) objcolor[1]) / 255.0F),
	    (((GLfloat) objcolor[2]) / 255.0F),
	    1.0F);
  fprintf (dbgdump, "face color [%d, %d, %d]\n",
	   objcolor[0], objcolor[1], objcolor[2]);
  fflush(dbgdump);
  // glDisable(GL_COLOR_MATERIAL);
#ifdef IRIS
	/* OGLXXX
	 * lmcolor: if LMC_NULL,  use:
	 * glDisable(GL_COLOR_MATERIAL);
	 * LMC_COLOR: use glDisable(GL_COLOR_MATERIAL);
	 */
  glColorMaterial(GL_FRONT_AND_BACK, XXX lmc_COLOR);glEnable(GL_COLOR_MATERIAL);
#endif

  thisfve = thishell->fvelist->first.fve;
  while (thisfve != Nil)
  {
    if ((thisfve->featureflags & Transparentflag) ||
	(thisfve->mothershell->motherobj->featureflags & Transparentflag))
    {
      get_feature_color((featureptr) thisfve,objcolor);
      draw_wireframe_face(thisfve,objcolor);
    }
    else if (!backface_fve(thisfve))
    {
      facecolor = thisfve->thecolor;
      if ((compare(thisfve->facenorm[vz],1.0,Pttopttol) == 0) &&
	  (compare(-thisfve->planedist,state->cursor[vz],Tolerance) == 0))
      {		/* if face is in cutplane, draw it forward by 0.5 */
	glPushMatrix();
	glTranslatef(0.0, 0.0, (Fabs(state->cursor[vz]+thisfve->planedist)+1.0));
	if (facecolor != Nil)
	{
	/* OGLXXX
	 * lmcolor: if LMC_NULL,  use:
	 * glDisable(GL_COLOR_MATERIAL);
	 */
	  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	  glEnable(GL_COLOR_MATERIAL);
	  glColor4f((((GLfloat) facecolor[0]) / 255.0F),
		    (((GLfloat) facecolor[1]) / 255.0F),
		    (((GLfloat) facecolor[2]) / 255.0F),
		    1.0F);
#ifdef IRIS
	/* OGLXXX
	 * lmcolor: if LMC_NULL,  use:
	 * glDisable(GL_COLOR_MATERIAL);
	 * LMC_COLOR: use glDisable(GL_COLOR_MATERIAL);
	 */
	  glColorMaterial(GL_FRONT_AND_BACK, XXX lmc_COLOR);
	  glEnable(GL_COLOR_MATERIAL);
#endif
	  draw_flatshaded_face(thisfve);
	/* OGLXXX
	 * lmcolor: if LMC_NULL,  use:
	 * glDisable(GL_COLOR_MATERIAL);
	 */
	  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	  glEnable(GL_COLOR_MATERIAL);
#ifdef IRIS
	  glColor4f((((GLfloat) objcolor[0]) / 255.0F),
		    (((GLfloat) objcolor[1]) / 255.0F),
		    (((GLfloat) objcolor[2]) / 255.0F),
		    1.0F);
	/* OGLXXX
	 * lmcolor: if LMC_NULL,  use:
	 * glDisable(GL_COLOR_MATERIAL);
	 * LMC_COLOR: use glDisable(GL_COLOR_MATERIAL);
	 */
	  glColorMaterial(GL_FRONT_AND_BACK, XXX lmc_COLOR);
	  glEnable(GL_COLOR_MATERIAL);
#endif
	  facecolor = Nil;
	}
	else
	  draw_flatshaded_face(thisfve);
	glPopMatrix();
      }
      else
      {
	if (facecolor != Nil)
	{
	/* OGLXXX
	 * lmcolor: if LMC_NULL,  use:
	 * glDisable(GL_COLOR_MATERIAL);
	 */
	  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	  glEnable(GL_COLOR_MATERIAL);
#ifdef IRIS
	  glColor4f((((GLfloat) facecolor[0]) / 255.0F),
		    (((GLfloat) facecolor[1]) / 255.0F),
		    (((GLfloat) facecolor[2]) / 255.0F),
		    1.0F);
	/* OGLXXX
	 * lmcolor: if LMC_NULL,  use:
	 * glDisable(GL_COLOR_MATERIAL);
	 * LMC_COLOR: use glDisable(GL_COLOR_MATERIAL);
	 */
	  glColorMaterial(GL_FRONT_AND_BACK, XXX lmc_COLOR);
	  glEnable(GL_COLOR_MATERIAL);
#endif
	  draw_flatshaded_face(thisfve);
	/* OGLXXX
	 * lmcolor: if LMC_NULL,  use:
	 * glDisable(GL_COLOR_MATERIAL);
	 */
	  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	  glEnable(GL_COLOR_MATERIAL);
#ifdef IRIS
	  glColor4f((((GLfloat) objcolor[0]) / 255.0F),
		    (((GLfloat) objcolor[1]) / 255.0F),
		    (((GLfloat) objcolor[2]) / 255.0F),
		    1.0F);
	/* OGLXXX
	 * lmcolor: if LMC_NULL,  use:
	 * glDisable(GL_COLOR_MATERIAL);
	 * LMC_COLOR: use glDisable(GL_COLOR_MATERIAL);
	 */
	  glColorMaterial(GL_FRONT_AND_BACK, XXX lmc_COLOR);
	  glEnable(GL_COLOR_MATERIAL);
#endif
	  facecolor = Nil;
	}
	else
	  draw_flatshaded_face(thisfve);
      }
    }
    thisfve = thisfve->next;
  }
}

  void
draw_flatshaded_obj(objptr thisobj,stateptr state)
{
	/* OGLXXX
	 * use gluBeginPolygon(tobj) to draw concave polys
	 * concave(TRUE)
	 */
  /*DELETED*/;
  /* only draw outer most shell */

  draw_flatshaded_shell(First_obj_shell(thisobj),state);

	/* OGLXXX
	 * use gluBeginPolygon(tobj) to draw concave polys
	 * concave(FALSE)
	 */
  /*DELETED*/;
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			SOFTWARE LIGHTING UTILITIES		        */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
draw_software_shaded_face(fveptr thisfve)
{
  leptr firstle,thisle;
  boundptr outerbnd,thisinnerbnd;
  colortype reflectedcolor,reflectivity;
  ;
  if (backface_fve(thisfve))
    return;

  get_feature_color((featureptr) thisfve,reflectivity);
  compute_reflected_color(thisfve->facenorm,reflectivity,reflectedcolor);

	/* OGLXXX
	 * special cases for polygons:
	 * 	independant quads: use GL_QUADS
	 * 	independent triangles: use GL_TRIANGLES
	 */
  glBegin(GL_POLYGON);
  glColor4f((((GLfloat) reflectedcolor[0]) / 255.0F),
	    (((GLfloat) reflectedcolor[1]) / 255.0F),
	    (((GLfloat) reflectedcolor[2]) / 255.0F),
	    1.0F);
  outerbnd = thisfve->boundlist->first.bnd;
  firstle = thisle = outerbnd->lelist->first.le;
  do
  {
    glVertex3fv(thisle->facevert->pos);
    thisle = thisle->next;
  } while (thisle != outerbnd->lelist->first.le);
  glVertex3fv(thisle->facevert->pos); /* draw back to first again */
  
  thisinnerbnd = thisfve->boundlist->first.bnd->next;
  while (thisinnerbnd != Nil)
  {
    thisle = thisinnerbnd->lelist->first.le;
    do
    {
      glVertex3fv(thisle->facevert->pos);
      thisle = thisle->next;
    } while (thisle != thisinnerbnd->lelist->first.le);
    glVertex3fv(thisle->facevert->pos);	/* draw back to first vert of this bound */
    thisinnerbnd = thisinnerbnd->next;
  }
  glEnd();	/* this should be sufficient to draw back to first ver */
  
}

  void
draw_software_shaded_shell(shellptr thishell)
{
  fveptr thisfve;
  colortype objscolor;
  ;
  /* If transparent don't draw faces */
  if (thishell->featureflags & Transparentflag)
    return;
  thisfve = thishell->fvelist->first.fve;
  while (thisfve != Nil)
  {
    if ((thisfve->featureflags & Transparentflag) == 0)
      draw_software_shaded_face(thisfve);
    else
    {
      get_feature_color((featureptr) thisfve,objscolor);
      draw_wireframe_face(thisfve,objscolor);
    }
    thisfve = thisfve->next;
  }
}

  void
draw_software_shaded_obj(objptr thisobj)
{
	/* OGLXXX
	 * use gluBeginPolygon(tobj) to draw concave polys
	 * concave(TRUE)
	 */
  /*DELETED*/;
  /* only draw outer most shell */
  draw_software_shaded_shell(First_obj_shell(thisobj));
	/* OGLXXX
	 * use gluBeginPolygon(tobj) to draw concave polys
	 * concave(FALSE)
	 */
  /*DELETED*/;
}
