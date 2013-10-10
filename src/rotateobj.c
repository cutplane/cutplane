
/* FILE: rotateobj.c */

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

#define ROTATEOBJMODULE

#include <config.h>

#include <stdio.h>
#include <math.h>

#include <platform_gl.h>

#include <cutplane.h>

#include <rotateobj.h>
#include <update.h>
#include <globals.h>		/* for imatrix definition */
#include <math3d.h>
#include <pick.h>

Matrix rotmatrix = {{1.0,0.0,0.0,0.0},
                    {0.0,1.0,0.0,0.0},
                    {0.0,0.0,1.0,0.0},
                    {0.0,0.0,0.0,1.0}};

Matrix invmatrix = {{1.0,0.0,0.0,0.0},
                    {0.0,1.0,0.0,0.0},
                    {0.0,0.0,1.0,0.0},
                    {0.0,0.0,0.0,1.0}};


/* eventually call me from axis_rot_obj */

  void
make_axisrot_matrix(vertype rotvec,vertype rotpt,int rotangle,
		    Matrix axisrotmat,Matrix invaxisrotmat)
{
  float angle1 = 0.0, angle2 = 0.0;
  float cosangle1,cosangle2,vecmag;
  ;
  vecmag = magvec(rotvec);
  /* cosangle2 is the cos of the angle between rotvec and the y axis */
  cosangle2 = dotprod(rotvec[vx],rotvec[vy],rotvec[vz],0.0,1.0,0.0)/vecmag;
 
 if (cosangle2 <= -1.0)	/* rotating about -y axis */
    angle2 = 1800.0;
  else if (cosangle2 < 1.0)
  {
    vecmag = mag(rotvec[vx],0.0,rotvec[vz]);
    /* cosangle1 is the cos of the angle between the projection of rotvec */
    /* in the x-z plane and the x axis */
    cosangle1 = dotprod(rotvec[vx],0.0,rotvec[vz],1.0,0.0,0.0)/vecmag;
    if (cosangle1 <= -1.0)
      angle1 = 1800.0;
    else if (cosangle1 < 1.0)
      angle1 = 10.0 * Rad2deg * Acos(cosangle1);
    if (rotvec[vz] < 0.0)
      angle1 = -angle1;
    angle2 = 10.0 * Rad2deg * Acos(cosangle2);
  }

  glPushMatrix();
  if (invaxisrotmat != Nil)
  {
    glLoadMatrixf((GLfloat *) invaxisrotmat);
    glTranslatef(rotpt[vx], rotpt[vy], rotpt[vz]);
	/* OGLXXX You can do better than this. */
    glRotatef(.1*(iround(-angle1)), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
	/* OGLXXX You can do better than this. */
    glRotatef(.1*(iround(-angle2)), ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
	/* OGLXXX You can do better than this. */
    glRotatef(.1*(-rotangle), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
	/* OGLXXX You can do better than this. */
    glRotatef(.1*(iround(angle2)), ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
	/* OGLXXX You can do better than this. */
    glRotatef(.1*(iround(angle1)), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
    glTranslatef(-rotpt[vx], -rotpt[vy], -rotpt[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) invaxisrotmat);
  }
  
  if (axisrotmat != Nil)
  {
    glLoadMatrixf((GLfloat *) imatrix);
    glTranslatef(rotpt[vx], rotpt[vy], rotpt[vz]);
	/* OGLXXX You can do better than this. */
    glRotatef(.1*(iround(-angle1)), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
	/* OGLXXX You can do better than this. */
    glRotatef(.1*(iround(-angle2)), ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
	/* OGLXXX You can do better than this. */
    glRotatef(.1*(rotangle), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
	/* OGLXXX You can do better than this. */
    glRotatef(.1*(iround(angle2)), ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
	/* OGLXXX You can do better than this. */
    glRotatef(.1*(iround(angle1)), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
    glTranslatef(-rotpt[vx], -rotpt[vy], -rotpt[vz]);
    glMultMatrixf((GLfloat *) axisrotmat);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) axisrotmat);
  }
  glPopMatrix();
}

/* rotate angle in tenths of degrees, as in oldstyle gl calls */
  void
axis_rot_obj_engine(objptr rotateobj, rotateinfoptr rotateinfo)
{
  float angle1 = 0.0, angle2 = 0.0;
  float cosangle1,cosangle2,vecmag;
  GLfloat *rotvec,*rotpt;
  int rotangle;
  ;
#ifdef debug
  printf ("vector:\n");
  printvert(rotvec);
  printf ("origin:\n");
  printvert(rotpt);
  printf ("in axis_rot_obj_engine angle = %d\n\n",rotangle);
#endif
  rotvec = rotateinfo->rotvec;
  rotpt = rotateinfo->rotpt;
  rotangle = rotateinfo->rotangle;

  /* clear the object ring list so that must_make_ringlist doesn't interfere */
  clear_obj_ringlist(rotateobj);

  vecmag = magvec(rotvec);
  /* cosangle2 is the cos of the angle between rotvec and the y axis */
  cosangle2 = dotprod(rotvec[vx],rotvec[vy],rotvec[vz],0.0,1.0,0.0)/vecmag;
  if (cosangle2 <= -1.0)	/* rotating about -y axis */
    angle2 = 1800.0;
  else if (cosangle2 < 1.0)
  {
    vecmag = mag(rotvec[vx],0.0,rotvec[vz]);
    /* cosangle1 is the cos of the angle between the projection of rotvec */
    /* in the x-z plane and the x axis */
    cosangle1 = dotprod(rotvec[vx],0.0,rotvec[vz],1.0,0.0,0.0)/vecmag;
    if (cosangle1 <= -1.0)
      angle1 = 1800.0;
    else if (cosangle1 < 1.0)
      angle1 = 10.0 * Rad2deg * Acos(cosangle1);
    if (rotvec[vz] < 0.0)
      angle1 = -angle1;
    angle2 = 10.0 * Rad2deg * Acos(cosangle2);
  }
  glPushMatrix();
  glLoadMatrixf((GLfloat *) rotateobj->invxform);
  glTranslatef(rotpt[vx], rotpt[vy], rotpt[vz]);
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(iround(-angle1)), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(iround(-angle2)), ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(-rotangle), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(iround(angle2)), ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(iround(angle1)), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
  glTranslatef(-rotpt[vx], -rotpt[vy], -rotpt[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) rotateobj->invxform);
  
  glLoadMatrixf((GLfloat *) imatrix);
  glTranslatef(rotpt[vx], rotpt[vy], rotpt[vz]);
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(iround(-angle1)), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(iround(-angle2)), ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(rotangle), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(iround(angle2)), ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(iround(angle1)), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
  glTranslatef(-rotpt[vx], -rotpt[vy], -rotpt[vz]);
  glMultMatrixf((GLfloat *) rotateobj->xform);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) rotateobj->xform);
  glPopMatrix();
}

  void
axis_rot_obj(objptr rotateobj,vertype rotvec,vertype rotpt,int rotangle)
{
  objptr rootrotateobj;
  rotateinfotype rotateinfo;
  ;
  copyvec3d(rotvec,rotateinfo.rotvec);
  copypos3d(rotpt,rotateinfo.rotpt);
  rotateinfo.rotangle = rotangle;

  rootrotateobj = get_unlinked_ancestor(rotateobj);
  log_branch_erase_bbox(rootrotateobj,Useobjbbox,Loginoldbuffer);
  apply_to_descendants(rootrotateobj,axis_rot_obj_engine,&rotateinfo);
  transform_obj_tree(rotateobj);	/* update positions */
}

/* CURRENTLY NOT UP TO DATE! */
  void
inplane_axis_rot_obj_engine(objptr rotateobj, vertype rotvec, vertype rotpt,
			    int rotangle)
{
  float angle = 0.0;
  float cosangle,vecmag;
  ;
  vecmag = mag(rotvec[vx],rotvec[vy],0.0);
  cosangle = dotprod(rotvec[vx],rotvec[vy],0.0,0.0,1.0,0.0)/vecmag;
  angle = 10.0 * Rad2deg * Acos(cosangle);
  if (rotvec[vx] < 0.0)
    angle = -angle;
  glPushMatrix();
  glLoadMatrixf((GLfloat *) rotateobj->invxform);
  glTranslatef(rotpt[vx], rotpt[vy], rotpt[vz]);
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(iround(-angle)), ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(-rotangle), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(iround(angle)), ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
  glTranslatef(-rotpt[vx], -rotpt[vy], -rotpt[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) rotateobj->invxform);
  
  glLoadMatrixf((GLfloat *) imatrix);
  glTranslatef(rotpt[vx], rotpt[vy], rotpt[vz]);
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(iround(-angle)), ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(rotangle), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(iround(angle)), ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
  glTranslatef(-rotpt[vx], -rotpt[vy], -rotpt[vz]);
  glMultMatrixf((GLfloat *) rotateobj->xform);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) rotateobj->xform);
  glPopMatrix();
  transform_obj(rotateobj);	/* update positions */
}

  int
snap_face_to_plane(vertype rotaxis, vertype fnorm, int delangle)
{
  vertype fnorm_X_zaxis;
  ;
  if (Fabs(fnorm[vz]) > tolznorm)
  {
    /* angle between face normal and positive z-axis (plane normal) */
    delangle = iround((float) (10.0 * Rad2deg * Acos(fnorm[vz])));

    /* check to see if normal points in negative z direction  */
    /* if it does we want to rotate to the neg. z-axis not the pos. z-axis */
    if (fnorm[vz] < 0.0)
      delangle = delangle - 1800;
    /* axis of rotation for snapping face into plane calculated by taking */
    /* fnorm cross z-axis, this should be parallel to rotaxis */
    fnorm_X_zaxis[vx] = fnorm[vy];
    fnorm_X_zaxis[vy] = -fnorm[vx];

    /* check to see if fnorm cross z-axis points in the same direction  */
    /* as actual rotation axis, if not then angle is opposite */
    if ((rotaxis[vx]*fnorm_X_zaxis[vx] + rotaxis[vy]*fnorm_X_zaxis[vy]) < 0.0)
      delangle = -delangle;
  }

  return(delangle);
}

/* This version of snap_face_to_plane returns an angle of rotation required to */
/* make adjustvec parallel to the snapvec, rotating it about the computed */
/* rotaxis. */

  int
snap_vector_to_vector(vertype adjustvec, vertype snapvec, vertype rotaxis)
{
  int delangle;
  ;
  delangle = iround((float) (10.0 * Rad2deg * Acos(dotvecs(adjustvec,snapvec))));
  cross_prod(adjustvec,snapvec,rotaxis);
  return(delangle);
}

  int
snap_face_to_plane_perp(vertype rotaxis, vertype fnorm)
{
  int delangle = 0;
  vertype inplaneperp,newrotaxis;
  ;
  /* if normal is already in the plane dont bother... */
  if (compare(fnorm[vz],0.0,(1.0-tolznorm) == 0))
  {
    /* find a vector in the plane and perpendicular to rotaxis */
    cross_prod(rotaxis,z_axis,inplaneperp);
    /* make sure it points in the same direction as face norm */
    if (dotvecs(inplaneperp,fnorm) < 0.0)
      flip_vec(inplaneperp);
    /* find the angle */
    delangle = snap_vector_to_vector(fnorm,inplaneperp,newrotaxis);
    if (dotvecs(newrotaxis,rotaxis) < 0.0)
      delangle = -delangle;
  }

  return (delangle);
}


#if 0

/* LJE: this is not used anymore 11/15 */

  static GLboolean
just_started_rot(stateptr state,stateptr oldstate)
{
  int rightbut,oldrightbut;
  ;
  rightbut = state->controlbuttonsdown & Rbuttonbit;
  oldrightbut = oldstate->controlbuttonsdown & Rbuttonbit;
  return ((rightbut != oldrightbut) && rightbut);
}

  static int
lock_direction(stateptr state, stateptr oldstate, vertype rotvec,
	       GLboolean resetflag)
{
  int rotate_amount,delrot[2],axis;
  static int totrot[2] = {0,0}, locked_axis = vx;
  static GLboolean locked = FALSE;
  static vertype coord_axes[2] = {{1.0,0.0,0.0,0.0},{0.0,1.0,0.0,0.0}};
  ;
  if (resetflag)
  {				/* reset distance if button just pressed */
    locked = FALSE;
    totrot[vx] = 0; totrot[vy] = 0;
    return (0);
  }
  delrot[vx] = -(state->shellrotx - oldstate->shellrotx);
  delrot[vy] = (state->shellroty - oldstate->shellroty);
  if ((delrot[vx] == 0) && (delrot[vy] == 0))
    return (0);
  totrot[vx] += delrot[vx];
  totrot[vy] += delrot[vy];
  if (!locked && (totrot[vx]*totrot[vx] + totrot[vy]*totrot[vy]) >
      (4.0*Pkboxsize*Pkboxsize))
  {
    locked = TRUE;
    /* the following works because vx == false and vy == true */
    locked_axis = (abs(totrot[vy]) > abs(totrot[vx]));
    copypos3d(coord_axes[locked_axis],rotvec);
  }

  if (locked)
    return (delrot[locked_axis]);
  
  axis = abs(delrot[vy]) > abs(delrot[vx]);
  copypos3d(coord_axes[axis],rotvec);
  
  return (delrot[axis]);
}

  static int
calc_rotangle(stateptr state, stateptr oldstate, vertype rotvec)
{
  vertype mousevec;
  ;
  mousevec[vx] = (float) (state->mousex - oldstate->mousex);
  mousevec[vy] = (float) (state->mousey - oldstate->mousey);

  if ((mousevec[vx] == 0) && (mousevec[vy] == 0))
    return (0);
  
  normalize(rotvec,rotvec);
  return (iround(mousevec[vx]*rotvec[vy] - mousevec[vy]*rotvec[vx]));
}

  shellptr
establish_rotvec(listptr shell_list, vertype rotvec, vertype rotpt)
{
  shellptr rotateshell;
  featureptr rotatefeature;
  leptr rotatele;
  float *pt1,*pt2;
  vertype dummyvec;
  ;
  rotateshell = shell_list->first.shell;
  while (rotateshell != Nil)
  {
    rotatefeature = rotateshell->selectable_feature.thefeature;
    switch (rotateshell->selectable_feature.selectkind)
    {
    case Nothingselect:
    case Vertselect:
      break;			/* no free rotation around verts for now */
    case Edgeselect:
      rotatele = ((evfptr) rotatefeature)->le1;
      pt1 = rotatele->facevert->pos;
      pt2 = Twin_le(rotatele)->facevert->pos;
      diffpos3d(pt1,pt2,rotvec);
      rotateshell = rotatele->motherbound->motherfve->mothershell;
      copypos3d(pt1,rotpt);
      lock_direction(state,oldstate,dummyvec,TRUE);
      return(rotateshell);
    case Faceselect:
      get_intersect_pts(rotateshell->selectable_feature.motherseg,
			&pt1,&pt2);
      diffpos3d(pt1,pt2,rotvec);
      copypos3d(pt1,rotpt);
      return(rotateshell);
    case Inplanefaceselect:	/* currently is not permitted because */
      break;			/* this causes non-snaps to occur a lot. */
    case Shellselect:
      copypos3d(state->cursor,rotpt);
      lock_direction(state,oldstate,rotvec,TRUE);
      return(rotateshell);
    default:
      break;
    }
    rotateshell = rotateshell->next;
  }
  
  return (Nil);
}

  static void
rotate_tools(shellptr this_receptacle,vertype rotvec,vertype rotpt,int rotate_amount)
{
}

/* rotate selectable shells. */

  void
rotate_features(stateptr state,stateptr oldstate,listptr shell_list)
{
  static shellptr rotateshell = Nil;
  featureptr rotatefeature;
  int rotate_amount,axis;
  static vertype rotvec = {1.0,0.0,0.0,0.0}, rotpt = {1.0,0.0,0.0,0.0};
  static vertype fake_rotvec;
  ;
  if (just_started_rot(state,oldstate))
  {
    rotateshell = establish_rotvec(shell_list,rotvec,rotpt);
    return;
  }
  if (rotateshell == Nil)		/* this should never happen */
    return;
  rotatefeature = rotateshell->selectable_feature.thefeature;
  switch (rotateshell->selectable_feature.selectkind)
  {
  case Edgeselect:
    rotate_amount = lock_direction(state,oldstate,fake_rotvec,FALSE);
    break;
  case Faceselect:
    rotate_amount = calc_rotangle(state,oldstate,rotvec);
    rotate_amount = snap_face_to_plane(rotvec,
					 ((fveptr) rotatefeature)->facenorm,
					 rotate_amount);
    break;
  case Shellselect:
    rotate_amount = lock_direction(state,oldstate,rotvec,FALSE);
    break;
  default:
    rotate_amount = 0;
    break;
  }
  if (rotate_amount != 0)
  {
    axis_rot_shell(rotateshell,rotvec,rotpt,rotate_amount);
    if (has_property((featureptr) rotateshell, menu_prop))
      rotate_tools(rotateshell,rotvec,rotpt,rotate_amount);
  }
}

#endif
