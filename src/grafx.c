
/* FILE: grafx.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* Der Newen Improven Droewen/Displayenspiel Rutinnen fur ausch Cutplagen */
/*                          Editeriat,yah yah.                            */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1990  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990,1991 The Board of Trustees of The    */
/* Leland Stanford Junior University, and William Kessler and             */
/* Larry Edwards. All Rights Reserved.                                    */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define GRAFXMODULE

#include <config.h>
#include <stdio.h>
#include <math.h>

#include <platform_gl.h>
#include <platform_glu.h>

#ifdef IRIS
#include <fmclient.h>
#endif

#include <cutplane.h>

#include <grafx.h>
#include <globals.h>
#include <select.h>
#include <intersect.h>
#include <shade.h>
#include <rotatetool.h>
#include <tools.h>

#include <demo.h>

/* Used to define cutplane edges */
static vertype corners[4] = {{Mincrossx,Mincrossy,0.0},
			     {Maxcrossx,Mincrossy,0.0},
			     {Maxcrossx,Maxcrossy,0.0},
			     {Mincrossx,Maxcrossy,0.0}};


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                               GLOBAL VARIABLES                           */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#ifdef IRIS
Object Oldhatch;		/* LJE: temp hack test dude */
#endif

/* Plane hatching colors */
colortype myplanecolor = {0,0,0};
colortype patcol1,patcol2;

vertype contourpt1,contourpt2;	/* bngline hack */

int numplanehatches = 80;

vertype viewpt = { 0.0, 0.0, 4000.0 };
	/* OGLXXX XXX fix this. */
Matrix vanillamatrix, invanillamatrix;/* harks back to the Old Ones... */

void (*global_selectroutine)();
objptr global_selectobj = Nil;
/*
long zbuffermin = -8388608, zbuffermax = 8388607; 
long zbuffershift = 65535;
*/
long zbuffermin = -8388608, zbuffermax = 8388607;
long zbuffershift = 65535, max_zbuffershift = 3*65535;
int framebuffer = 0;  /* which is the current framebuffer we're drawing in */

GLboolean doslideshow = FALSE;

/* An array of old and new erase records, one for each frame buffer. */
erase_record_type erase_records[2][2];
/* An array of pointers to which of the two rows in the above array */
/* constitutes the old and new erase records, one for each frame buffer. */
erase_record_ptr old_erase_records[2],new_erase_records[2];

/* Added for new use of opengl lists, 12/30/01 */
GLuint first_gl_list_id, current_gl_list_id;

/* Hack for opengl lights */
GLfloat light1Pos[4] = { 0.0F, 0.0F, 0.0F, 1.0F };

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                        LOW LEVEL SUPPORT ROUTINES                        */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
toggle_zbuffering(void)
{
  use_zbuffer = !use_zbuffer;
  /* if(use_zbuffer) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST); */ /* save for a rainy day */
}

void
normal_zbuffer(void)
{
  /* OGLXXX glDepthRange params must be scaled to [0, 1] */
#ifdef IRIS
  glDepthRange(zbuffermin + max_zbuffershift, zbuffermax);
#else
  glDepthRange(0.0,1.0);
  /*  fprintf (dbgdump, "zbuffer-range set to 0.0 - 1.0\n"); */
#endif
}

void
shifted_zbuffer(int numtimes)
{
  /* OGLXXX glDepthRange params must be scaled to [0, 1] */
#ifdef IRIS
  glDepthRange(zbuffermin + max_zbuffershift - numtimes*zbuffershift,
	       zbuffermax - numtimes*zbuffershift);
#else
  glDepthRange((zbuffermin + max_zbuffershift) /
	       (zbuffermin + max_zbuffershift - numtimes*zbuffershift),
	       zbuffermax / (zbuffermax - numtimes*zbuffershift));
  glDepthRange(0.0,1.0);
#endif

}

  GLboolean
backface_fve(fveptr thisfve)
{
  static vertype xformedpos,xformednorm;
  ;
  if (set_backface_flags)
  {
    if (thisfve->timestamp < cutplane_time)
    {
      /* this works!! yehaah! */
      transformpt(thisfve->boundlist->first.bnd->lelist->
			 first.le->facevert->pos, vanillamatrix,xformedpos);
      transformpt(thisfve->facenorm,vanillamatrix,xformednorm);

      thisfve->timestamp = cutplane_time;
      thisfve->back_face = (dotvecs(xformedpos,xformednorm) > 0.0);
    }
  }
  return(thisfve->back_face);
}

  GLboolean
edge_is_hidden(evfptr thisevf, GLboolean fve1_facesback, GLboolean fve2_facesback)
{
  vertype norm1xnorm2,edgevec;
  fveptr fve1;
  ;
  if (set_backface_flags)
  {
    if (thisevf->timestamp < cutplane_time)
    {
      /* both faces point towards viewer...edge is visible */
      if (!fve1_facesback && !fve2_facesback)
      {
	thisevf->hidden_edge = FALSE;
	return (FALSE);
      }
      
      /* both faces point away from viewer...edge is hidden */
      if (fve1_facesback && fve2_facesback)
      {
	thisevf->hidden_edge = TRUE;
	return (TRUE);
      }
      
      /* one face points towards viewer the other faces away... */
      /* check to see if shell is concave at the edge...if so edge is hidden */
      makevec3d(thisevf->le1->facevert->pos,thisevf->le2->facevert->pos,edgevec);
      
      fve1 = thisevf->le1->motherbound->motherfve;
      cross_prod(fve1->facenorm,thisevf->le2->motherbound->motherfve->facenorm,
		 norm1xnorm2);
      if (fve1->mothershell->cclockwise)
	thisevf->hidden_edge = (dotvecs(norm1xnorm2,edgevec) < 0.0);
      else
	thisevf->hidden_edge = (dotvecs(norm1xnorm2,edgevec) > 0.0);
    }
  }
  return (thisevf->hidden_edge);
}

  GLboolean
no_containing_feature_picked(elemptr thisfeature)
{
  leptr thisle,startle;
  vfeptr thisvfe;
  evfptr thisevf;
  fveptr fve1,fve2;
  ;
  switch (thisfeature->type_id)
  {
  case Vfe_type:
    thisvfe = (vfeptr) thisfeature;
    startle = thisvfe->startle;
    if (startle->motherbound->motherfve->mothershell->motherobj->featureflags &
	Pickedflag)
      return (FALSE);
    thisle = startle;
    do
    {
      if ((thisle->facedge->featureflags & Pickedflag) ||
	  (thisle->motherbound->motherfve->featureflags & Pickedflag))
	return (FALSE);
      thisle = Twin_le(thisle)->next;
    } while (thisle != startle);
    return (TRUE);
  case Evf_type:
    thisevf = (evfptr) thisfeature;
    fve1 = thisevf->le1->motherbound->motherfve;
    if (fve1->mothershell->motherobj->featureflags &	Pickedflag)
      return (FALSE);
    fve2 = thisevf->le2->motherbound->motherfve;
    if ((fve1->featureflags & Pickedflag) || (fve2->featureflags & Pickedflag))
      return (FALSE);
    return (TRUE);
  case Fve_type:
    if (((fveptr) thisfeature)->mothershell->motherobj->featureflags &
	Pickedflag)
      return (FALSE);
    return (TRUE);
  case Shell_type:
    return (TRUE);
  case Obj_type:
    return (TRUE);
  }

#ifndef IRIS
  return(TRUE);
#endif
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                            DEFINING ROUTINES                             */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
def_environment_colors(void)
{
  FILE *colorfile;
  static char colorfilename[] = {"rsc/cpcolors.rsc"};
  ;
  colorfile = fopen(colorfilename,"r");
  fscanf(colorfile, "*** Cutplane Color Definition File\n\n");
  fscanf(colorfile,"Background Color:%hd,%hd,%hd\n",&backgroundcolor[0],
	 &backgroundcolor[1],&backgroundcolor[2]);
  fscanf(colorfile,"Cutplane Color:%hd,%hd,%hd\n",&cutplanecolor[0],
	 &cutplanecolor[1],&cutplanecolor[2]);
  fscanf(colorfile,"Number of Cutplane Hatches:%d\n",&numplanehatches);
  fscanf(colorfile,"Crosshair Color:%hd,%hd,%hd\n\n",
	 &crosshaircolor[0],&crosshaircolor[1], &crosshaircolor[2]);
  fscanf(colorfile,"Shadow Color:%hd,%hd,%hd\n\n",
	 &shadowcolor[0],&shadowcolor[1], &shadowcolor[2]);
  fscanf(colorfile,"Selectable Color:%hd,%hd,%hd\n\n",
	 &selectablecolor[0],&selectablecolor[1], &selectablecolor[2]);
  fscanf(colorfile, "Picked Color:%hd,%hd,%hd\n\n",
	 &pickedcolor[0],&pickedcolor[1], &pickedcolor[2]);
  fscanf(colorfile, "Picked Contour Color:%hd,%hd,%hd\n\n",
	 &pickedcontourcolor[0],&pickedcontourcolor[1], &pickedcontourcolor[2]);
  fscanf(colorfile, "Cursor Move Tolerance:%f\n",
	 &cursor_move_tolerance);
  fscanf(colorfile, "Cursor Accel Tolerance:%f\n",
	 &cursor_accel_tolerance);
/*  fscanf(colorfile,"%lx,%lx\n",&lsetnear,&lsetfar);*/

  fclose(colorfile);
}

  void
init_projmatrix(void)
{
  GLdouble xleft = Mincrossx, xright = Maxcrossx;
  GLdouble ytop = Maxcrossy, ybot = Mincrossy;
  GLdouble zfront = Maxplanez, zback = Minplanez;
  GLdouble roomradius,roomwidth,roomheight,roomdepth;
  GLdouble temp1, temp2, temp3;
  double temp4;
  int temp5;
  GLint matrixMode;
  ;
  /* roomradius = radius of the smallest sphere that could contain the room */
  roomwidth = xright-xleft+1.0;
  roomheight = ytop-ybot+1.0;
  roomdepth = zfront-zback+1.0;

  /* roomradius =812.1576 for 1024x768x1000 */
  temp1 = roomwidth * roomwidth;
  temp2 = roomheight * roomheight;
  temp3 = roomdepth * roomdepth;
  temp4 = (double) sqrt(temp1 + temp2 + temp3);
  temp5 = (int) temp4;

  fprintf(dbgdump, "temp5 over 2 is: %lf\n", (double) temp5 / 2);
  fflush(dbgdump);
  roomradius = (GLdouble) (temp4 / (double) 2.0);
  nearclipdist = (GLfloat) (viewpt[vz] - roomradius);
  farclipdist = (GLfloat) (viewpt[vz] + roomradius);
  fprintf(dbgdump, "nearclipdist: %lf\n", nearclipdist);
  fprintf(dbgdump, "farclipdist: %lf\n", farclipdist);

  glMatrixMode(GL_PROJECTION);

  glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum((GLfloat) xleft, (GLfloat) xright, (GLfloat) ybot, (GLfloat) ytop,
	    (GLfloat) nearclipdist, (GLfloat) farclipdist);
  glMatrixMode(matrixMode);
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) projmatrix);
  glMatrixMode(GL_MODELVIEW);
}

/* Pass compute_mapping == TRUE if you want to recompute the mapping matrix. */
/* You can also compute a conversion at the same time or not if you pass */
/* Nil for the pts. */

  void
map2screen(vertype thept, screenpos screenpt,GLboolean compute_mapping)
{
  static float cx, sx = -1.0, cy, sy;
  GLfloat xformedpos[4];
#ifdef IRIS
  float fraction,temp,homopt[4];
#else
  float fraction;
#endif
  int i;

  static Matrix combomatrix,viewmatrix;
  ;
  if (compute_mapping)
  {
    cx = 0.5 * (mainwindowright-mainwindowleft);
    sx = 0.5 * (mainwindowright-mainwindowleft);
    cy = 0.5 * (mainwindowtop-mainwindowbottom);
    sy = 0.5 * (mainwindowtop-mainwindowbottom);

    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) viewmatrix);
    
    glLoadMatrixf((GLfloat *) projmatrix);
    glMultMatrixf((GLfloat *) viewmatrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) combomatrix);
    glPopMatrix();
  }

  if (thept != Nil)
  {
    for (i=0; i<4; i++)
      xformedpos[i] = thept[0]*combomatrix[0][i] +
                      thept[1]*combomatrix[1][i] +
                      thept[2]*combomatrix[2][i] +
                      1.0*combomatrix[3][i];
    fraction = 1.0/xformedpos[3];

    screenpt[vx] = sx * xformedpos[0] * fraction + cx;
    screenpt[vy] = sy * xformedpos[1] * fraction + cy;
    
  }
}

  int *
scale2screen(float thept[3], int screenpt[2], int left,int right,int bot,int top)
{
  static float cx, sx = -1.0, cy, sy;
  GLfloat xformedpos[4];
#ifdef IRIS
  float fraction,temp,homopt[4];
#else
  float fraction;
#endif
  int i;

  Matrix projectmatrix,combomatrix,viewmatrix;
  ;
  cx = (right-left)/2.0;
  sx = (right-left)/2.0;
  cy = (top-bot)/2.0;
  sy = (top-bot)/2.0;

  glMatrixMode(GL_PROJECTION);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) projectmatrix);
  glMatrixMode(GL_MODELVIEW);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) viewmatrix);

  glPushMatrix();
  glLoadMatrixf((GLfloat *) projectmatrix);
  glMultMatrixf((GLfloat *) viewmatrix);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) combomatrix);
  glPopMatrix();

  for (i=0; i<4; i++)
    xformedpos[i] = thept[0]*combomatrix[0][i] +
                    thept[1]*combomatrix[1][i] +
                    thept[2]*combomatrix[2][i] +
                    1.0*combomatrix[3][i];
  fraction = 1.0/xformedpos[3];

	 
  screenpt[0] = sx * xformedpos[0] * fraction + cx;
  screenpt[1] = sy * xformedpos[1] * fraction + cy;
/*
  printf("screenx = %lf, screeny = %lf, right-left = %d, top-bottom = %d\n",
	 sx * xformedpos[0] * fraction + cx,sy * xformedpos[1] * fraction + cy,
	 right-left,top-bot);
*/

  return (screenpt);
}


  void
make_vanilla_matrix(int roomrotx,int roomroty)
{
  GLfloat twistangle = 0;
  /* hack 2001 */
  GLfloat light0Pos[4] = { 0.0F, 1.0F, 0.5F, 0.0F };
  ;
  /* must clear out top of matrix stack in split matrix mode */
  glLoadMatrixf((GLfloat *) imatrix);

  glPushMatrix();
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(-roomrotx),1.0,0.0,0.0);
  glRotatef(.1*(-roomroty),0.0,1.0,0.0);
  glTranslatef(viewpt[vx], viewpt[vy], viewpt[vz]); /* wee hack */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) invanillamatrix);
  glPopMatrix();


  /* This needs to get converted to gluLookat */
  /*
    void gluLookAt( GLdouble eyex, GLdouble eyey, GLdouble eyez,
    GLdouble centerx,    GLdouble centery,    GLdouble centerz,
    GLdouble upx,    GLdouble upy,    GLdouble upz
    );
    eyex, eyey, eyez : The position of the eye point. 
    centerx, centery, centerz : The position of the reference point. 
    upx, upy, upz : The direction of the up vector. 
  */

  gluLookAt(viewpt[vx],viewpt[vy],viewpt[vz],origin[vx],origin[vy],origin[vz],
	 0.0F,1.0F,0.0F);

  glRotatef(.1*(roomroty), 0.0,1.0,0.0);
  glRotatef(.1*(roomrotx), 1.0,0.0,0.0);

  glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
  //glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
  //  glEnable(GL_LIGHT0);

  /* Get the results into the global matrix var */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) vanillamatrix); 

  /* Precompute 3D->screen mapping matrix */
  map2screen(Nil,Nil,TRUE);


  /* make sure light positions and directions are transformed correctly! */
	/* OGLXXX lmbind: check object numbering. */
#ifdef IRIS
  if(1) {
    glCallList(1);
    glEnable(GL_LIGHT0);
  } else {
    glDisable(GL_LIGHT0);
  }
#else
    glCallList(1);
    glEnable(GL_LIGHT0);
#endif
}

  void
object_defs(void)
{
  make_vanilla_matrix(0,0);
}

  static void
draw_room(void)
{
  static GLfloat front[4][3] = {{Mincrossx,Maxcrossy,Maxplanez},
			      {Maxcrossx,Maxcrossy,Maxplanez},
			      {Maxcrossx,Mincrossy,Maxplanez},
			      {Mincrossx,Mincrossy,Maxplanez}};
  static GLfloat back[4][3] = {{Maxcrossx,Maxcrossy,Minplanez},
			     {Maxcrossx,Mincrossy,Minplanez},
			     {Mincrossx,Mincrossy,Minplanez},
			     {Mincrossx,Maxcrossy,Minplanez}};
  static GLfloat sides[8][3] = {{Maxcrossx,Maxcrossy,Maxplanez},
			      {Maxcrossx,Maxcrossy,Minplanez},
			      {Maxcrossx,Mincrossy,Maxplanez},
			      {Maxcrossx,Mincrossy,Minplanez},
			      {Mincrossx,Mincrossy,Maxplanez},
			      {Mincrossx,Mincrossy,Minplanez},
			      {Mincrossx,Maxcrossy,Maxplanez},
			      {Mincrossx,Maxcrossy,Minplanez}};
  ;

  //  glDepthFunc(GL_ALWAYS);

  glColor3sv(backgroundcolor);	/* erase surrounding pixels to heighten */
				/* contrast */
  glLineWidth(2.0F);
  glBegin(GL_LINE_LOOP);
  glVertex3fv(front[0]);
  glVertex3fv(front[1]);
  glVertex3fv(front[2]);
  glVertex3fv(front[3]);
  glEnd();

  glColor4f(0.0,0.0,0.0,1.0);
  glLineWidth(1.0F);
  glBegin(GL_LINE_LOOP);
  glVertex3fv(front[0]);
  glVertex3fv(front[1]);
  glVertex3fv(front[2]);
  glVertex3fv(front[3]);
  glEnd();

  glLineWidth(2.0F);
  glBegin(GL_LINE_LOOP);
  glVertex3fv(back[0]);
  glVertex3fv(back[1]);
  glVertex3fv(back[2]);
  glVertex3fv(back[3]);
  glEnd();

  glLineWidth(2.0F);	/* erase surrounding pixels to heighten */
				/* contrast */
  glColor3sv(backgroundcolor);
  /* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(sides[0]);
  glVertex3fv(sides[1]);
  glEnd();

  /* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(sides[6]);
  glVertex3fv(sides[7]);
  glEnd();

  glLineWidth(1.0F);
  glColor4f(0.0,0.0,0.0,1.0);
  /* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(sides[0]);
  glVertex3fv(sides[1]);
  glEnd();

  /* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(sides[6]);
  glVertex3fv(sides[7]);
  glEnd();

  glColor4f((((GLfloat) backgroundcolor[0]) / 255.0F),
	    (((GLfloat) backgroundcolor[1]) / 255.0F),
	    (((GLfloat) backgroundcolor[2]) / 255.0F),
	    1.0F);

  glLineWidth(3.0F);
  /* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(sides[2]);
  glVertex3fv(sides[3]);
  glEnd();

  /* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(sides[4]);
  glVertex3fv(sides[5]);
  glEnd();

  glColor4f(0.0,0.0,0.0,1.0);
  glLineWidth(2.0F);
  /* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(sides[2]);
  glVertex3fv(sides[3]);
  glEnd();

  /* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(sides[4]);
  glVertex3fv(sides[5]);
  glEnd();

  glLineWidth(1.0F);
}

  bboxscreenptr
find_cutplane_screenbbox(void)
{
  static bboxscreentype cutplane_screenbbox;
  bbox3dtype cutplanebbox;
  ;
  if (has_property((featureptr) cutplane_obj,picked_prop))
  {
    setpos3d(cutplanebbox.mincorner,Mincrossx - Pickedcontouroffset,
	     Mincrossy - Pickedcontouroffset,
	     First_obj_vfe(cutplane_obj)->pos[vz]);
    setpos3d(cutplanebbox.maxcorner,Maxcrossx + Pickedcontouroffset,
	     Maxcrossy + Pickedcontouroffset,
	     First_obj_vfe(cutplane_obj)->pos[vz]);
  }
  else
  {
    setpos3d(cutplanebbox.mincorner,Mincrossx,Mincrossy,
	     First_obj_vfe(cutplane_obj)->pos[vz]);
    setpos3d(cutplanebbox.maxcorner,Maxcrossx,Maxcrossy,
	     First_obj_vfe(cutplane_obj)->pos[vz]);
  }
  compute_screen_bbox(&cutplanebbox,&cutplane_screenbbox);
  return (&cutplane_screenbbox);
}

#define Magnetmarginx 50.0
#define Magnetmarginy (Magnetmarginx * ((GLfloat) Maxcrossy/(GLfloat) Maxcrossx))
#define Focusradius 50.0

  void
draw_cutplane(void)
{
  int i;
  double width,height,xinc,yinc;
  static GLubyte patarray1[128];
  ;
  for (i = 0; i < 128; i+= 8) {
    patarray1[i] = 0xAA;
    patarray1[i+1] = 0xAA;
    patarray1[i+2] = 0xAA;
    patarray1[i+3] = 0xAA;
    patarray1[i+4] = 0x55;
    patarray1[i+5] = 0x55;
    patarray1[i+6] = 0x55;
    patarray1[i+7] = 0x55;
  }


  glDisable(GL_LIGHTING);
  glEnable(GL_POLYGON_STIPPLE);
  glPolygonStipple(patarray1);
  glPolygonMode(GL_FRONT, GL_FILL);

  width = Maxcrossx - Mincrossx;
  height = Maxcrossy - Mincrossy;
  xinc = width/ (double) numplanehatches;
  yinc = height/ (double) numplanehatches;

  glPushMatrix();
  glColor4f((((GLfloat) cutplanecolor[0]) / 255.0F),
	    (((GLfloat) cutplanecolor[1]) / 255.0F),
	    (((GLfloat) cutplanecolor[2]) / 255.0F),
	    1.0F);
  glTranslatef(0.0, 0.0, state->cursor[vz]-2.0);
  glBegin(GL_POLYGON);
  glVertex3fv(corners[0]);
  glVertex3fv(corners[1]);
  glVertex3fv(corners[2]);
  glVertex3fv(corners[3]);
  glEnd();

  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);
  glLineWidth(2.0F);
  glTranslatef(0.0, 0.0, +7.0);
  glBegin(GL_LINE_LOOP);
  glVertex3fv(corners[0]);
  glVertex3fv(corners[1]);
  glVertex3fv(corners[2]);
  glVertex3fv(corners[3]);
  glEnd();

  glDisable(GL_POLYGON_STIPPLE);
  glPopMatrix();
  glEnable(GL_LIGHTING);

}

  void
draw_cutplane_pickedrect(void) 
{
  static vertype largecorners[4] = {{Mincrossx - Pickedcontouroffset/2.0,
				     Mincrossy - Pickedcontouroffset/2.0,0.0},
				    {Maxcrossx + Pickedcontouroffset/2.0,
				     Mincrossy - Pickedcontouroffset/2.0,0.0},
				    {Maxcrossx + Pickedcontouroffset/2.0,
				     Maxcrossy + Pickedcontouroffset/2.0,0.0},
				    {Mincrossx - Pickedcontouroffset/2.0,
				     Maxcrossy + Pickedcontouroffset/2.0,0.0}};
  ;
  glPushMatrix();
  glColor4f((((GLfloat) red[0]) / 255.0F),
	    (((GLfloat) red[1]) / 255.0F),
	    (((GLfloat) red[2]) / 255.0F),
	    1.0F);

  glTranslatef(0.0, 0.0, 2.0);
  /* OGLXXX setlinestyle: Check list numbering. */
  glEnable(GL_LINE_STIPPLE);

  glLineWidth(2.0F);
  glBegin(GL_LINE_LOOP);
  glVertex3fv(largecorners[0]);
  glVertex3fv(largecorners[1]);
  glVertex3fv(largecorners[2]);
  glVertex3fv(largecorners[3]);
  glEnd();
  glPopMatrix();


	/* OGLXXX Check list numbering. */
  glPushMatrix();
  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);
  glLineWidth(3.0F);
  glTranslatef(0.0, 0.0, 0.0);
  glBegin(GL_LINE_LOOP);
  glVertex3fv(corners[0]);
  glVertex3fv(corners[1]);
  glVertex3fv(corners[2]);
  glVertex3fv(corners[3]);
  glEnd();
  glLineWidth(1.0F);
  glPopMatrix();

}


void
draw_crosshair(void)
{
  static vertype vertshadowhair[2] =
    {{0.0-Crosshairshadowoffset,Mincrossy,0.0},
     {0.0-Crosshairshadowoffset, Maxcrossy,0.0}};
  static vertype horizshadowhair[2] =
    {{Mincrossx, 0.0+Crosshairshadowoffset,0.0},
     {Maxcrossx, 0.0+Crosshairshadowoffset,0.0}};
  static vertype verticalhair[2] = {{0.0,Mincrossy,0.0},{0.0,Maxcrossy,0.0}};
  static vertype horizontalhair[2] = {{Mincrossx,0.0,0.0},{Maxcrossx,0.0,0.0}};
  ;

  glEnable(GL_LINE_STIPPLE);
  /* draw black underlying crosshair first */
  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);

  glLineWidth(1.0F);
  /* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(horizshadowhair[0]);
  glVertex3fv(horizshadowhair[1]);
  glEnd();

  /* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(vertshadowhair[0]);
  glVertex3fv(vertshadowhair[1]);
  glEnd();
  
  glColor4f((((GLfloat) cyan[0]) / 255.0F),
	    (((GLfloat) cyan[1]) / 255.0F),
	    (((GLfloat) cyan[2]) / 255.0F),
	    1.0F);

  glLineWidth(3.0F);
  /* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(horizontalhair[0]);
  glVertex3fv(horizontalhair[1]);
  glEnd();

  /* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(verticalhair[0]);
  glVertex3fv(verticalhair[1]);
  glEnd();
  
  glLineWidth(1.0F);
  /* OGLXXX setlinestyle: Check list numbering. */
  glDisable(GL_LINE_STIPPLE);

}

#ifdef IRIS
				/* editing lists no longer supported */
				/* in openGL, so no point defining them*/
  void
background_defs(void)
{
  room_def();
  cutplane_def(); 
}
#endif /* IRIS */

  void
set_cutplanehatch_color(void)
{
#if 0
	/* OGLXXX
	 * editobj not supported
	 *  -- use display list hierarchy
	 * editobj(Cutplane)
	 */
  /*DELETED*/;
  /* replace(4,0); No longer supported!!! */ 
    setcolor(backgroundcolor);	/* actually should replace the hatches with */
				/* nada */
  glEndList();
#endif
}

/* update world updates all the graphical shells that make up the */
/* ENVIRONMENT to reflect the current state */

  void
update_world(stateptr state, stateptr oldstate)
{
  static vertype verticalhair[2] = {{0.0,Mincrossy,0.0},{0.0,Maxcrossy,0.0}};
  static vertype horizontalhair[2] = {{Mincrossx,0.0,0.0},{Maxcrossx,0.0,0.0}};
  ;
#ifdef IRIS
  if (state->cursor[vz] != oldstate->cursor[vz])
  {
	/* OGLXXX
	 * editobj not supported
	 *  -- use display list hierarchy
	 * editobj(Cutplane)
	 */
    /*DELETED*/;
    /*      replace(1,0);  No longer supported!!! */ 
      glTranslatef(0.0, 0.0, state->cursor[vz]-2.0);
    glEndList();

	/* OGLXXX
	 * editobj not supported
	 *  -- use display list hierarchy
	 * editobj(Cutplaneborder)
	 */
    /*DELETED*/;
    /* replace(1,0);  No longer supported!!! */ 
      glTranslatef(0.0, 0.0, state->cursor[vz]+5.0);
    glEndList();

	/* OGLXXX
	 * editobj not supported
	 *  -- use display list hierarchy
	 * editobj(Planepickedrect)
	 */
    /*DELETED*/;
    /* replace(1,0); No longer supported!!! */ 
      glTranslatef(0.0, 0.0, state->cursor[vz]+2.0);
    glEndList();
  }
#endif /* IRIS */

  if ((state->roomrotx != oldstate->roomrotx) ||
      (state->roomroty != oldstate->roomroty))
  {
    oldstate->roomrotx = state->roomrotx;
    oldstate->roomroty = state->roomroty;
  }
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                            LIGHTING ROUTINES                             */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

void
set_light_dir(int lightdefindex, vertype direction)
{
  static float lightprops[] = {GL_POSITION, 1.0, 0.0, 0.0, 0.0, };
  int nlightprops = sizeof(lightprops)/sizeof(float);
  ;
  lightprops[1] = -direction[vx];
  lightprops[2] = -direction[vy];
  lightprops[3] = -direction[vz];
	/* OGLXXX
	 * lmdef other possibilities include:
	 * 	glLightf(light, pname, *params);
	 * 	glLightModelf(pname, param);
	 * Check list numbering.
	 * Translate params as needed.
	 */
  glNewList(lightdefindex, GL_COMPILE); glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *lightprops); glEndList();;
}

void
set_light_color(int lightdefindex, colortype thecolor)
{
	/* OGLXXX
	 * light color: need to add GL_AMBIENT and GL_SPECULAR components.
	 * ALPHA needs to be included in parameters.
	 */
  static float lightprops[] = {GL_DIFFUSE, 0.0, 0.0, 0.0, };
  int nlightprops = sizeof(lightprops)/sizeof(float);
  double scaling = 1.0/255.0;
  ;
  lightprops[1] = thecolor[0]*scaling;
  lightprops[2] = thecolor[1]*scaling;
  lightprops[3] = thecolor[2]*scaling;
	/* OGLXXX
	 * lmdef other possibilities include:
	 * 	glLightf(light, pname, *params);
	 * 	glLightModelf(pname, param);
	 * Check list numbering.
	 * Translate params as needed.
	 */
  glNewList(lightdefindex, GL_COMPILE); glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *lightprops); glEndList();;
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                            DRAWING ROUTINES                              */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/



/**************************  Icon drawing routines **************************/


  void
nodraw_technique(objptr thisobj,stateptr state)
{
}

  void
draw_scribble_technique(objptr scribble,stateptr state)
{
  vfeptr thisvfe;
  vertype shadowpos;
  ;
  glPushMatrix();
  glColor4f((((GLfloat) scribble->thecolor[0]) / 255.0F),
	    (((GLfloat) scribble->thecolor[1]) / 255.0F),
	    (((GLfloat) scribble->thecolor[2]) / 255.0F),
	    1.0F);
  glLineWidth(3.0F);
  thisvfe = scribble->shellist->first.shell->vfelist->first.vfe;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  while (thisvfe != Nil)
  {
    glVertex3fv(thisvfe->pos);
    thisvfe = thisvfe->next;
  }
  glEnd();
  glLineWidth(1.0F);
  if (state->roomrotx > -250)
  {
    glColor4f((((GLfloat) black[0]) / 255.0F),
	      (((GLfloat) black[1]) / 255.0F),
	      (((GLfloat) black[2]) / 255.0F),
	      1.0F);
    thisvfe = scribble->shellist->first.shell->vfelist->first.vfe;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
    glBegin(GL_LINE_STRIP);
    while (thisvfe != Nil)
    {
      copypos3d(thisvfe->pos,shadowpos);
      shadowpos[vy] = Mincrossy;
      glVertex3fv(shadowpos);
      thisvfe = thisvfe->next;
    }
    glEnd();
  }
  glPopMatrix();
}

  void
draw_marquee_core(vertype corner1, vertype corner2) {
  vertype pos1,pos2;
  ;
  if (corner1[vz] == corner2[vz]) /* if not a full square, just draw front, */
				  /* offset */
  {
    /* draw front rectangle */
    glBegin(GL_LINE_LOOP);
      glVertex3fv(corner1);
      setpos3d(pos1,corner2[vx],corner1[vy],corner1[vz]+2.0);
      glVertex3fv(pos1);
      setpos3d(pos1,corner2[vx],corner2[vy],corner1[vz]+2.0);
      glVertex3fv(pos1);
      setpos3d(pos1,corner1[vx],corner2[vy],corner1[vz]+2.0);
      glVertex3fv(pos1);
    glEnd();
  }
  else
  {
    /* draw front rectangle */
    glBegin(GL_LINE_LOOP);
      glVertex3fv(corner1);
      setpos3d(pos1,corner2[vx],corner1[vy],corner1[vz]+2.0);
      glVertex3fv(pos1);
      setpos3d(pos1,corner2[vx],corner2[vy],corner1[vz]+2.0);
      glVertex3fv(pos1);
      setpos3d(pos1,corner1[vx],corner2[vy],corner1[vz]+2.0);
      glVertex3fv(pos1);
    glEnd();
    /* draw back rectangle */
    glBegin(GL_LINE_LOOP);
      glVertex3fv(corner2);
      setpos3d(pos1,corner1[vx],corner2[vy],corner2[vz]);
      glVertex3fv(pos1);
      setpos3d(pos1,corner1[vx],corner1[vy],corner2[vz]);
      glVertex3fv(pos1);
      setpos3d(pos1,corner2[vx],corner1[vy],corner2[vz]);
      glVertex3fv(pos1);
    glEnd();

    glBegin(GL_LINES);
    glVertex3fv(corner1);
    setpos3d(pos1,corner1[vx],corner1[vy],corner2[vz]);
    glVertex3fv(pos1);
    
    setpos3d(pos1,corner2[vx],corner1[vy],corner1[vz]);
    glVertex3fv(pos1);
    setpos3d(pos1,corner2[vx],corner1[vy],corner2[vz]);
    glVertex3fv(pos1);

    setpos3d(pos1,corner2[vx],corner2[vy],corner1[vz]);
    glVertex3fv(pos1);
    setpos3d(pos1,corner2[vx],corner2[vy],corner2[vz]);
    glVertex3fv(pos1);

    setpos3d(pos1,corner1[vx],corner2[vy],corner1[vz]);
    glVertex3fv(pos1);
    setpos3d(pos1,corner1[vx],corner2[vy],corner2[vz]);
    glVertex3fv(pos1);

    glEnd();
  }
}
    
  void
draw_marquee_technique(objptr marquee,stateptr state)
{
  GLfloat *corner1,*corner2;
  vertype pos1,pos2;
  static short marquee_linestyle = Dashpattern;
  ;
  corner1 = First_obj_vfe(marquee)->pos;
  corner2 = First_obj_vfe(marquee)->next->pos;

  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, Largedashpattern);

  glDepthFunc(GL_ALWAYS);  
  glLineWidth(1.0F);
  glColor4f((((GLfloat) white[0]) / 255.0F),
	    (((GLfloat) white[1]) / 255.0F),
	    (((GLfloat) white[2]) / 255.0F),
	    1.0F);
  draw_marquee_core(corner1,corner2);

  glDepthFunc(GL_LEQUAL);
  glLineWidth(3.0F);
  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);
  draw_marquee_core(corner1,corner2);

  /* draw shadow of marquee */
  glLineWidth(1.0);
  glColor4f((((GLfloat) shadowcolor[0]) / 255.0F),
	    (((GLfloat) shadowcolor[1]) / 255.0F),
	    (((GLfloat) shadowcolor[2]) / 255.0F),
	    1.0F);
  glBegin(GL_LINE_LOOP);
    setpos3d(pos1,corner1[vx],Mincrossy,corner1[vz]);
    glVertex3fv(pos1);
    setpos3d(pos1,corner2[vx],Mincrossy,corner1[vz]);
    glVertex3fv(pos1);
    setpos3d(pos1,corner2[vx],Mincrossy,corner2[vz]);
    glVertex3fv(pos1);
    setpos3d(pos1,corner1[vx],Mincrossy,corner2[vz]);
    glVertex3fv(pos1);
  glEnd();
	     
  glDisable(GL_LINE_STIPPLE);

}

/* Establish a base for the range of GL Lists that will be used, e.g. by */
/* decals. If this stuff works well, could be used for other cutplane */
/* objects.*/

  void
create_gl_list_ids(void)
{
  first_gl_list_id = current_gl_list_id = glGenLists(MAX_GL_LISTS);
}

/* Create a gl object that is a thin small version of the passed decal_obj, */
/* with backfaces eliminated, oriented near the front of the dest_obj's bbox. */

  void
draw_decal_globj(objptr decal_obj)
{
  objptr thischild;
  ;
  thischild = decal_obj->children->first.obj;
  while (thischild != Nil)
  {
    draw_decal_globj(thischild);
    thischild = thischild->next;
  }
  /* create the object by using the drawing routines */
  draw_software_shaded_obj(decal_obj);
}

  GLuint
create_decal_globj(objptr decal_obj, vertype to_origin,
		   vertype scale_amt, vertype decal_offset)
{
  GLuint newdecalobj;
  ;
  /* To create a decal, create a new gl object. */
  glPushMatrix();

/*
  glLoadMatrixf(vanillamatrix);
*/
  /* Get a new gl list id. */
  newdecalobj = current_gl_list_id++;
  while (glIsList(newdecalobj))
    newdecalobj = current_gl_list_id++;

  /* assume glCallList(Vanillaview) done by draw rtn */
  glNewList(newdecalobj, GL_COMPILE);	       

  glTranslatef(decal_offset[vx], decal_offset[vy], decal_offset[vz]);
  /* scale it down to ratio size */
  glScalef(scale_amt[vx], scale_amt[vy], scale_amt[vz]);
  /* because, why not? */
  /* translate to the origin */
  glTranslatef(to_origin[vx], to_origin[vy], to_origin[vz]);

  draw_decal_globj(decal_obj);
  glEndList();

  glPopMatrix();

  return(newdecalobj);
}

  static void
draw_toolpad(objptr toolpad,vertype padpoly[], int numpadverts,GLfloat padzoffset)
{
  colortype toolpadcolor;
  GLfloat *firstpos;
  GLuint decalobj;
  int i;
  ;
  glDisable(GL_LIGHTING);

  glPushMatrix();
  firstpos = First_obj_vfe(toolpad)->pos;
  get_feature_color((featureptr) toolpad,toolpadcolor);
  if ((toolpad->selectable_feature.selectkind != Nothingselect) &&
      (!has_property((featureptr) toolpad,nonselectable_prop)))
    copycolor(white,toolpadcolor);

  glColor4f((((GLfloat) toolpadcolor[0]) / 255.0F),
	    (((GLfloat) toolpadcolor[1]) / 255.0F),
	    (((GLfloat) toolpadcolor[2]) / 255.0F),
	    1.0F);

  glDepthFunc(GL_ALWAYS);
  glTranslatef(firstpos[vx], firstpos[vy], firstpos[vz] - padzoffset);
  glLineWidth(1.0F);
	/* OGLXXX setlinestyle: Check list numbering. */
  
  glEnable(GL_LINE_STIPPLE);

  glBegin(GL_LINE_LOOP);
  glLineStipple(1, Largedotline);
  for (i = 0; i < numpadverts; ++i)
    glVertex3fv(padpoly[i]);
  glEnd();
  glTranslatef(-firstpos[vx], -firstpos[vy], -firstpos[vz] + padzoffset);

  glColor4f(0.0F, 0.0F, 0.0F, 1.0F);
  glDepthFunc(GL_LEQUAL);
  glTranslatef(firstpos[vx], firstpos[vy], firstpos[vz] + padzoffset);
  glLineWidth((GLfloat)(Toolpadblacklinewidth));
  glBegin(GL_LINE_LOOP);
  for (i = 0; i < numpadverts; ++i)
    glVertex3fv(padpoly[i]);
  glEnd();

  glDisable(GL_LINE_STIPPLE);

  glColor4f((((GLfloat) toolpadcolor[0]) / 255.0F),
	    (((GLfloat) toolpadcolor[1]) / 255.0F),
	    (((GLfloat) toolpadcolor[2]) / 255.0F),
	    1.0F);

  glTranslatef(0.0, 0.0,  padzoffset);

  glBegin(GL_POLYGON);
  for (i = 0; i < numpadverts; ++i)
    glVertex3fv(padpoly[i]);
  glEnd();

  if (has_property((featureptr) toolpad, hasdecal_prop))
  {
    glPopMatrix();
    glPushMatrix();
    glMultMatrixf((GLfloat *) toolpad->xform);
    decalobj = (GLuint) (get_property_val((featureptr) toolpad,hasdecal_prop)).l;
	/* OGLXXX check list numbering */
    glCallList(decalobj);
  }

  glLineWidth(1.0F);
  glPopMatrix();

  glEnable(GL_LIGHTING);
}

  void
draw_pad_technique(objptr pad, stateptr state)
{
  colortype padcolor;
  GLuint decalobj;
  fveptr padfve;
  ;
  glDisable(GL_LIGHTING);
  padfve = First_obj_fve(pad);
  glPushMatrix();
  get_feature_color((featureptr) pad,padcolor);
  if ((pad->selectable_feature.selectkind != Nothingselect) &&
      (!has_property((featureptr) pad,nonselectable_prop)))
  {
    set_feature_color((featureptr) padfve,white);
    copycolor(white,padcolor);
  }
  else
  {
    get_feature_color((featureptr) pad,padcolor);
    set_feature_color((featureptr) padfve, padcolor);
  }

  glDepthFunc(GL_ALWAYS);
  glTranslatef(0.0,  0.0,  -Toolpadzoffset*2);
  glLineWidth(1.0F);
	/* OGLXXX setlinestyle: Check list numbering. */

  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, Largedotpattern);
  draw_wireframe_face(padfve,padcolor);
  glDisable(GL_LINE_STIPPLE);

  glDepthFunc(GL_LEQUAL);
  glTranslatef(0.0,  0.0,  Toolpadzoffset);
  glLineWidth((GLfloat)(Toolpadblacklinewidth));
	/* OGLXXX setlinestyle: Check list numbering. */

  draw_wireframe_face(padfve,black);
  glTranslatef(0.0,  0.0,  Toolpadzoffset);

  draw_solid_face(padfve,padcolor); 

  if (has_property((featureptr) pad, hasdecal_prop))
  {
    glDisable(GL_LIGHTING);
    glPopMatrix();
    glPushMatrix();
    glMultMatrixf((GLfloat *) pad->xform);
    decalobj = (GLuint) (get_property_val((featureptr) pad,hasdecal_prop)).l;
	/* OGLXXX check list numbering */
    glCallList(decalobj);
    glEnable(GL_LIGHTING);
  }

  glLineWidth(1.0F);
  glPopMatrix();
  glEnable(GL_LIGHTING);
}

  void
draw_toolpad_technique(objptr toolpad,stateptr state)
{
/*
  static vertype padpoly[Numtoolpadverts] =
  {{Toolpadsquare,-Toolpadsquare/2.0,0.0},
   {-Toolpadsquare,-Toolpadsquare/2.0,0.0},
   {-Toolpadsquare,3*Toolpadcorner,0.0},
   {-Toolpadsquare-Toolpadcorner*2.0,4.0*Toolpadcorner,0.0},
   {-2.0*Toolpadsquare+Toolpadcorner,4.0*Toolpadcorner,0.0},
   {-2.0*Toolpadsquare, 5.0*Toolpadcorner,0.0},
   {-2.0*Toolpadsquare,7.0*Toolpadcorner,0.0},
   {-2.0*Toolpadsquare+Toolpadcorner, Toolpadsquare,0.0},
   {Toolpadsquare - Toolpadcorner, Toolpadsquare,0.0},
   {Toolpadsquare,Toolpadsquare - Toolpadcorner,0.0}};
*/   
  static vertype padpoly[] =
  {{Toolpadsquare,-Toolpadsquare+Toolpadcorner,0.0},
   {Toolpadsquare-Toolpadcorner,-Toolpadsquare,0.0},
   {-Toolpadsquare,-Toolpadsquare,0.0},
   {-Toolpadsquare,Toolpadsquare,0.0},
   {Toolpadsquare-Toolpadcorner,Toolpadsquare,0.0},
   {Toolpadsquare,Toolpadsquare-Toolpadcorner,0.0}};
  int numpadverts = sizeof(padpoly) / sizeof(vertype);
  ;
  draw_toolpad(toolpad,padpoly,numpadverts,Toolpadzoffset);
}


  void
draw_headpad_technique(objptr headpad,stateptr state)
{
  static vertype padpoly[Numheadpadverts] =
  {{Headpadsquare - Headpadcorner,-Headpadsquare,0.0},
   {-Headpadsquare + Headpadcorner,-Headpadsquare,0.0},
   {-Headpadsquare,-Headpadsquare-Headpadcorner,0.0},
   {-Headpadsquare,-Headpadsquare-4.0*Headpadcorner,0.0},
   {-Headpadsquare-Headpadcorner*4.0,-Headpadsquare-4.0*Headpadcorner,0.0},
   {-Headpadsquare-Headpadcorner*4.0,Headpadsquare-Headpadcorner,0.0},
   {-Headpadsquare-Headpadcorner,Headpadsquare,0.0},
   {Headpadsquare - Headpadcorner,Headpadsquare,0.0},
   {Headpadsquare,Headpadsquare - Headpadcorner,0.0},
   {Headpadsquare,-Headpadsquare + Headpadcorner,0.0}};
  ;
  draw_toolpad(headpad,padpoly,Numheadpadverts,Headpadzoffset);
}


  void
draw_undertab_technique(objptr undertab,stateptr state)
{
  static vertype padpoly[] =
  {{-Normalpadsquare + Normalpadcorner,-Normalpadsquare,0.0},
   {Normalpadsquare - Normalpadcorner,-Normalpadsquare,0.0},
   {Normalpadsquare,-Normalpadsquare + Normalpadcorner,0.0},
   {Normalpadsquare,Normalpadsquare,0.0},
   {-Normalpadsquare,Normalpadsquare,0.0},
   {-Normalpadsquare,-Normalpadsquare + Normalpadcorner,0.0}};
  int numpadverts = sizeof(padpoly) / sizeof(vertype);
  ;
  draw_toolpad(undertab,padpoly,numpadverts,Toolpadzoffset);
}

  void
draw_plate_technique(objptr plate,stateptr state)
{
  vertype platepoly[8];
  vfeptr lowerleftvfe;
  vertype lowerleft,upperight;
  ;
  lowerleftvfe = First_obj_vfe(plate);
  diffpos3d(lowerleftvfe->pos,lowerleftvfe->next->pos,upperight);
  setpos3d(platepoly[0],Normalpadcorner,0.0,0.0);
  setpos3d(platepoly[1],upperight[vx] - Normalpadcorner,0.0, 0.0);
  setpos3d(platepoly[2],upperight[vx],Normalpadcorner, 0.0);
  setpos3d(platepoly[3],upperight[vx],upperight[vy] - Normalpadcorner,0.0);
  setpos3d(platepoly[4],upperight[vx] - Normalpadcorner,upperight[vy], 0.0);
  setpos3d(platepoly[5],Normalpadcorner,upperight[vy], 0.0);
  setpos3d(platepoly[6],0.0,upperight[vy] - Normalpadcorner, 0.0);
  setpos3d(platepoly[7],0.0,Normalpadcorner, 0.0);
  draw_toolpad(plate,platepoly,8,Toolpadzoffset);
}

  void
draw_padhandle_technique(objptr padhandle, stateptr state)
{
  static vertype handlepoly[6] =
  {{Padhandlewidth,-Padhandleheight,0.0},
   {Padhandlewidth,Padhandleheight,0.0},
   {-Padhandlewidth+Toolpadcorner,Padhandleheight,0.0},
   {-Padhandlewidth,Padhandleheight-Toolpadcorner,0.0},
   {-Padhandlewidth,-Padhandleheight + Toolpadcorner,0.0},
   {-Padhandlewidth+Toolpadcorner,-Padhandleheight,0.0}};
  ;
  draw_toolpad(padhandle,handlepoly,6,Toolpadzoffset);
}

  static void
draw_innercircle(vertype circle_center, colortype thecolor,
		 long frontmost_function)
{
  glDisable(GL_LIGHTING);

  glPushMatrix();

  glTranslatef(0.0, 0.0, circle_center[vz]);
  glDepthFunc(GL_ALWAYS);
  zdrawstate = GL_ALWAYS;

  glLineWidth(1.0F);
  glColor4f((((GLfloat) cutplanecolor[0]) / 255.0F),
	    (((GLfloat) cutplanecolor[1]) / 255.0F),
	    (((GLfloat) cutplanecolor[2]) / 255.0F),
	    1.0F);
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1,Dashpattern);

	/* OGLXXX See gluDisk man page. */
  { GLUquadricObj *qobj = gluNewQuadric(); gluQuadricDrawStyle(qobj, GLU_SILHOUETTE); glPushMatrix(); glTranslatef(circle_center[vx], circle_center[vy], 0.); gluDisk( qobj, 0., Rotateiconctrsize, 32, 1); glPopMatrix(); gluDeleteQuadric(qobj); };

  glDepthFunc(GL_LEQUAL);
  zdrawstate = GL_LEQUAL;

  glLineWidth(2.0F);
	/* OGLXXX setlinestyle: Check list numbering. */
  if(0) {glCallList(0); glEnable(GL_LINE_STIPPLE);} else glDisable(GL_LINE_STIPPLE);
  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);

	/* OGLXXX See gluDisk man page. */
  { GLUquadricObj *qobj = gluNewQuadric(); gluQuadricDrawStyle(qobj, GLU_SILHOUETTE); glPushMatrix(); glTranslatef(circle_center[vx], circle_center[vy], 0.); gluDisk( qobj, 0., Rotateiconctrsize, 32, 1); glPopMatrix(); gluDeleteQuadric(qobj); };

  glDepthFunc(frontmost_function);
  zdrawstate = frontmost_function;

  glLineWidth(2.0F);
  glTranslatef(Rotatetooldepth, -Rotatetooldepth, Rotatetooldepth);
  glColor4f((((GLfloat) thecolor[0]) / 255.0F),
	    (((GLfloat) thecolor[1]) / 255.0F),
	    (((GLfloat) thecolor[2]) / 255.0F),
	    1.0F);

	/* OGLXXX See gluDisk man page. */
  { GLUquadricObj *qobj = gluNewQuadric(); gluQuadricDrawStyle(qobj, GLU_SILHOUETTE); glPushMatrix(); glTranslatef(circle_center[vx], circle_center[vy], 0.); gluDisk( qobj, 0., Rotateiconctrsize, 32, 1); glPopMatrix(); gluDeleteQuadric(qobj); };

  glPopMatrix();
  glEnable(GL_LIGHTING);
}

  void
draw_rotate_innercircle(objptr thisobj,stateptr state)
{
  GLfloat *circle_center;
  ;
  if (!has_property((featureptr) thisobj,visible_prop))
    return;
  glDisable(GL_LIGHTING);
  draw_innercircle(First_obj_vfe(thisobj)->pos,white,GL_LEQUAL);
  glEnable(GL_LIGHTING);
}
  
  void
draw_innercircle_select_box(objptr thisobj,stateptr state)
{
  if (thisobj->selectable_feature.selectkind != Nothingselect)
    draw_innercircle(First_obj_vfe(thisobj)->pos,yellow,GL_ALWAYS);
}


  void
draw_rotate_outercircle(objptr thisobj,stateptr state)
{
  GLfloat *circle_center;
  ;
  if (!has_property((featureptr) thisobj,visible_prop))
    return;
  circle_center = First_obj_vfe(thisobj)->pos;
  glPushMatrix();

  glDisable(GL_LIGHTING);
  glTranslatef(0.0, 0.0, circle_center[vz]);
  
  glDepthFunc(GL_ALWAYS);
  zdrawstate = GL_ALWAYS;

  glLineWidth(1.0F);
	/* OGLXXX setlinestyle: Check list numbering. */
  if(Dashline) {glCallList(Dashline); glEnable(GL_LINE_STIPPLE);} else glDisable(GL_LINE_STIPPLE);
  glColor4f((((GLfloat) cutplanecolor[0]) / 255.0F),
	    (((GLfloat) cutplanecolor[1]) / 255.0F),
	    (((GLfloat) cutplanecolor[2]) / 255.0F),
	    1.0F);
	/* OGLXXX See gluDisk man page. */
  { GLUquadricObj *qobj = gluNewQuadric(); gluQuadricDrawStyle(qobj, GLU_SILHOUETTE); glPushMatrix(); glTranslatef(circle_center[vx], circle_center[vy], 0.); gluDisk( qobj, 0., rotatetool_radius + 4, 32, 1); glPopMatrix(); gluDeleteQuadric(qobj); };

  glDepthFunc(GL_LEQUAL);
  zdrawstate = GL_LEQUAL;

  glLineWidth(2.0F);
  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);

	/* OGLXXX setlinestyle: Check list numbering. */
  if(0) {glCallList(0); glEnable(GL_LINE_STIPPLE);} else glDisable(GL_LINE_STIPPLE);
	/* OGLXXX See gluDisk man page. */
  { GLUquadricObj *qobj = gluNewQuadric(); gluQuadricDrawStyle(qobj, GLU_SILHOUETTE); glPushMatrix(); glTranslatef(circle_center[vx], circle_center[vy], 0.); gluDisk( qobj, 0., rotatetool_radius, 32, 1); glPopMatrix(); gluDeleteQuadric(qobj); };

  glTranslatef(Rotatetooldepth, -Rotatetooldepth, Rotatetooldepth);
  glColor4f((((GLfloat) white[0]) / 255.0F),
	    (((GLfloat) white[1]) / 255.0F),
	    (((GLfloat) white[2]) / 255.0F),
	    1.0F);

	/* OGLXXX See gluDisk man page. */
  { GLUquadricObj *qobj = gluNewQuadric(); gluQuadricDrawStyle(qobj, GLU_SILHOUETTE); glPushMatrix(); glTranslatef(circle_center[vx], circle_center[vy], 0.); gluDisk( qobj, 0., rotatetool_radius, 32, 1); glPopMatrix(); gluDeleteQuadric(qobj); };

  glPopMatrix();

  glEnable(GL_LIGHTING);

}

 void
draw_one_xbar(vertype firstpos,vertype lastpos,GLboolean drawbar,
	      GLboolean drawbauble1,GLboolean drawbauble2,
	      GLboolean bauble1picked,GLboolean bauble2picked)
{
  glDisable(GL_LIGHTING);
  if (drawbar)
  {				/* only draw the xbar part if visible */
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
    glBegin(GL_LINE_STRIP);
    glVertex3fv(firstpos);
    glVertex3fv(lastpos);
    glEnd();
  }
  if (drawbauble1)
  {
    glTranslatef(0.0, 0.0, firstpos[vz]);
    if (bauble1picked) {
      /* OGLXXX See gluDisk man page. */
      { GLUquadricObj *qobj = gluNewQuadric(); glPushMatrix(); glTranslatef(firstpos[vx], firstpos[vy], 0.); gluDisk( qobj, 0., Baublesize, 32, 1); glPopMatrix(); gluDeleteQuadric(qobj); };
    } else {
      /* OGLXXX See gluDisk man page. */
      { GLUquadricObj *qobj = gluNewQuadric(); gluQuadricDrawStyle(qobj, GLU_SILHOUETTE); glPushMatrix(); glTranslatef(firstpos[vx], firstpos[vy], 0.); gluDisk( qobj, 0., Baublesize, 32, 1); glPopMatrix(); gluDeleteQuadric(qobj); };
      glTranslatef(0.0, 0.0, -firstpos[vz]);
    }
  }
  if (drawbauble2)
  {
    glTranslatef(0.0, 0.0, firstpos[vz]);
    if (bauble2picked) {
      /* OGLXXX See gluDisk man page. */
      { GLUquadricObj *qobj = gluNewQuadric(); glPushMatrix(); glTranslatef(lastpos[vx], lastpos[vy], 0.); gluDisk( qobj, 0., Baublesize, 32, 1); glPopMatrix(); gluDeleteQuadric(qobj); };
    } else {
      /* OGLXXX See gluDisk man page. */
      { GLUquadricObj *qobj = gluNewQuadric(); gluQuadricDrawStyle(qobj, GLU_SILHOUETTE); glPushMatrix(); glTranslatef(lastpos[vx], lastpos[vy], 0.); gluDisk( qobj, 0., Baublesize, 32, 1); glPopMatrix(); gluDeleteQuadric(qobj); };
    glTranslatef(0.0, 0.0, -firstpos[vz]);
    }
  }
  glEnable(GL_LIGHTING);
}

  void
draw_rotatexbar(objptr thisobj,stateptr state)
{
  leptr thisle;
  vertype firstpos,lastpos;
  GLboolean drawbar,drawbauble1,drawbauble2,bauble1picked,bauble2picked;
  ;
  glDisable(GL_LIGHTING);
  thisle = First_obj_le(thisobj);
  copypos3d(thisle->facevert->pos,firstpos);
  copypos3d(Twin_le(thisle)->facevert->pos,lastpos);
  glPushMatrix();
  drawbar = has_property((featureptr) First_obj_evf(thisobj),visible_prop);
  drawbauble1 = has_property((featureptr) thisle->facevert, visible_prop);
  drawbauble2 = has_property((featureptr) thisle->next->facevert, visible_prop);
/*
  bauble1picked = has_property((featureptr) thisle->facevert, picked_prop);
  bauble2picked = has_property((featureptr) thisle->next->facevert, picked_prop);
*/
  bauble1picked = bauble2picked = FALSE; /* fix me */

  glDepthFunc(GL_ALWAYS);
  zdrawstate = GL_ALWAYS;

	/* OGLXXX setlinestyle: Check list numbering. */
  if(Dashline) {glCallList(Dashline); glEnable(GL_LINE_STIPPLE);} else glDisable(GL_LINE_STIPPLE);

  glColor4f((((GLfloat) cutplanecolor[0]) / 255.0F),
	    (((GLfloat) cutplanecolor[1]) / 255.0F),
	    (((GLfloat) cutplanecolor[2]) / 255.0F),
	    1.0F);

  glLineWidth(1.0F);
  draw_one_xbar(firstpos,lastpos,drawbar,drawbauble1,drawbauble2,
		bauble1picked,bauble2picked);

  glDepthFunc(GL_LEQUAL);
  zdrawstate = GL_LEQUAL;

  glLineWidth(2.0F);
	/* OGLXXX setlinestyle: Check list numbering. */
  if(0) {glCallList(0); glEnable(GL_LINE_STIPPLE);} else glDisable(GL_LINE_STIPPLE);

  glColor4f((((GLfloat) white[0]) / 255.0F),
	    (((GLfloat) white[1]) / 255.0F),
	    (((GLfloat) white[2]) / 255.0F),
	    1.0F);

  draw_one_xbar(firstpos,lastpos,drawbar,drawbauble1,drawbauble2,
		bauble1picked,bauble2picked);

  firstpos[vx] += Rotatetooldepth;
  firstpos[vy] -= Rotatetooldepth;
  firstpos[vz] += Rotatetooldepth;
  lastpos[vx] += Rotatetooldepth;
  lastpos[vy] -= Rotatetooldepth;
  lastpos[vz] += Rotatetooldepth;

  glLineWidth(2.0F);

  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);

  draw_one_xbar(firstpos,lastpos,drawbar,drawbauble1,drawbauble2,
		bauble1picked,bauble2picked);

  glPopMatrix();
  glEnable(GL_LIGHTING);
}

  void
draw_big_circular_selectbox(objptr thisobj, stateptr state)
{
  if (thisobj->selectable_feature.selectkind != Nothingselect)
  {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glLineWidth(2.0F);
	/* OGLXXX setlinestyle: Check list numbering. */
    if(0) {glCallList(0); glEnable(GL_LINE_STIPPLE);} else glDisable(GL_LINE_STIPPLE);
    glTranslatef(0.0, 0.0, thisobj->selectable_feature.nearestpos[vz]);

  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);

	/* OGLXXX See gluDisk man page. */
    { GLUquadricObj *qobj = gluNewQuadric(); gluQuadricDrawStyle(qobj, GLU_SILHOUETTE); glPushMatrix(); glTranslatef(thisobj->selectable_feature.nearestpos[vx], 	 thisobj->selectable_feature.nearestpos[vy], 0.); gluDisk( qobj, 0., Baublesize, 32, 1); glPopMatrix(); gluDeleteQuadric(qobj); };
    glTranslatef(2.0, -2.0, 0.0);

    glColor4f((((GLfloat) yellow[0]) / 255.0F),
	      (((GLfloat) yellow[1]) / 255.0F),
	      (((GLfloat) yellow[2]) / 255.0F),
	      1.0F);

	/* OGLXXX See gluDisk man page. */
    { GLUquadricObj *qobj = gluNewQuadric(); gluQuadricDrawStyle(qobj, GLU_SILHOUETTE); glPushMatrix(); glTranslatef(thisobj->selectable_feature.nearestpos[vx], 	 thisobj->selectable_feature.nearestpos[vy], 0.); gluDisk( qobj, 0., Baublesize, 32, 1); glPopMatrix(); gluDeleteQuadric(qobj); };
    glPopMatrix();
  }
  glEnable(GL_LIGHTING);
}



/******************** Shell and section line drawing routines ***************/

  static void
set_sectionline_style(segmentptr thisegment, colortype sectioncolor,
		      int *sectionlinewidth,GLboolean *hiddensegment)
{
  evfptr inplanevf;
  double scale_amt;
  colortype reflectivity;
  fveptr thisfve;
  ;
  /* determine whether segment should be dashed or solid */
#if 0
  /* try to do section line as object color */
  if (thisegment->kind == Fve_intersect)
  {
    thisfve = thisegment->le[0]->motherbound->motherfve;
    *hiddensegment = backface_fve(thisfve);
    if (*hiddensegment)
      scale_amt = .5;
    else
      scale_amt = 2.0;
    get_feature_color((featureptr) thisfve,reflectivity);
    compute_reflected_color(thisfve->facenorm,reflectivity,sectioncolor);
    scalecolor(sectioncolor,scale_amt,sectioncolor);
  }
#else
  if (thisegment->kind == Fve_intersect)
  {
    *hiddensegment = backface_fve(thisegment->le[0]->motherbound->motherfve);
    copycolor(mediumyellow,sectioncolor);
  }
#endif  
  else if (thisegment->kind == Evf_inplane)
  {
    /* draw Oncut evfs as bright yellow */
    copycolor(yellow,sectioncolor);
    inplanevf = thisegment->le[0]->facedge;
    *hiddensegment = edge_is_hidden(inplanevf,
		     backface_fve(inplanevf->le1->motherbound->motherfve),
		     backface_fve(inplanevf->le2->motherbound->motherfve));
  }
  if (*hiddensegment)
  {
    *sectionlinewidth = Sectionlinewidth-1;
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, Largedotpattern);
  }
  else
  {
    *sectionlinewidth = Sectionlinewidth;
    glDisable(GL_LINE_STIPPLE);	/* solid line */
  }
}

  void
draw_shell_intersection(shellptr thishell)
{
  ringptr thisring;
  segmentptr thisegment;
  vertype shadowpt1,shadowpt2;
  colortype sectioncolor;
  int sectionlinewidth;
  GLboolean sectionlinehidden;
  ;
  shifted_zbuffer(1);
  thisring = thishell->ringlist->first.ring;
  while(thisring)
  {
    thisegment = thisring->seglist->first.seg;
    while(thisegment)
    {
      if (thisegment->kind != Fve_inplane)
      {
	set_sectionline_style(thisegment,sectioncolor,&sectionlinewidth,
			      &sectionlinehidden);
#if 0
	if ((!sectionlinehidden && use_zbuffer) ||
	    (!use_zbuffer))
	{
#endif
	  copypos3d(thisegment->pt[0],shadowpt1);
	  copypos3d(thisegment->pt[1],shadowpt2);

	  if (thishell->motherobj->motherworld == primary_world)
	  {
	    glDepthFunc(GL_ALWAYS); /* hack */
	    glPushAttrib(GL_ALL_ATTRIB_BITS);

	    glEnable(GL_LINE_STIPPLE);
	    glLineStipple(1,Largedotpattern);
	    glLineWidth(1.0F);

	    glColor4f((((GLfloat) darkyellow[0]) / 255.0F),
		      (((GLfloat) darkyellow[1]) / 255.0F),
		      (((GLfloat) darkyellow[2]) / 255.0F),
		      1.0F);

	/* OGLXXX for multiple, independent line segments: use GL_LINES */
	    glBegin(GL_LINE_STRIP); glVertex3fv(thisegment->pt[0]); glVertex3fv(thisegment->pt[1]); glEnd();
	    glPopAttrib();
	    glDepthFunc(GL_LEQUAL);
	  }
	  
	  glLineWidth((GLfloat)(sectionlinewidth + 3)); /* put a black line round it */
	  glColor4f((((GLfloat) black[0]) / 255.0F),
		    (((GLfloat) black[1]) / 255.0F),
		    (((GLfloat) black[2]) / 255.0F),
		    1.0F);

	/* OGLXXX for multiple, independent line segments: use GL_LINES */
	  glBegin(GL_LINE_STRIP); glVertex3fv(shadowpt1); glVertex3fv(shadowpt2); glEnd();
	  
	  glLineWidth((GLfloat)(sectionlinewidth));
	  glColor4f((((GLfloat) sectioncolor[0]) / 255.0F),
		    (((GLfloat) sectioncolor[1]) / 255.0F),
		    (((GLfloat) sectioncolor[2]) / 255.0F),
		    1.0F);

	/* OGLXXX for multiple, independent line segments: use GL_LINES */
	  glBegin(GL_LINE_STRIP); glVertex3fv(thisegment->pt[0]); glVertex3fv(thisegment->pt[1]); glEnd();
#if 0	  
	}
#endif
      }
      thisegment = thisegment->next;
    }
    thisring = thisring->next;
  }
  glLineWidth(1.0F);
	/* OGLXXX setlinestyle: Check list numbering. */
  if(0) {glCallList(0); glEnable(GL_LINE_STIPPLE);} else glDisable(GL_LINE_STIPPLE);
  glDepthFunc(GL_LEQUAL);
}

  void
draw_obj_intersection(objptr thisobj)
{
  shellptr thishell;
  ;
  if (has_property((featureptr) thisobj,sectioninvisible_prop))
    return;
  thishell = First_obj_shell(thisobj);
  while (thishell != Nil)
  {
    draw_shell_intersection(thishell); /* show the section line */
    thishell = thishell->next;
  }
}

  void
draw_edge(evfptr thisevf, colortype thiscolor)
{
  glColor4f((((GLfloat) thiscolor[0]) / 255.0F),
	    (((GLfloat) thiscolor[1]) / 255.0F),
	    (((GLfloat) thiscolor[2]) / 255.0F),
	    1.0F);

	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(thisevf->le1->facevert->pos);
  glVertex3fv(thisevf->le2->facevert->pos);
  glEnd();
}

  void
draw_shadowed_edge(evfptr thisevf, colortype thiscolor)
{
  GLfloat *endpt1,*endpt2;
  vertype shadowendpt1,shadowendpt2;;
  ;
  endpt1 = thisevf->le1->facevert->pos;
  endpt2 = thisevf->le2->facevert->pos;
  copypos3d(endpt1,shadowendpt1);
  copypos3d(endpt2,shadowendpt2);
  shadowendpt1[vx] += 2;
  shadowendpt1[vy] -= 2;
  shadowendpt2[vx] += 2;
  shadowendpt2[vy] -= 2;

  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);

	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(shadowendpt1);
  glVertex3fv(shadowendpt2);
  glEnd();
  
  glColor4f((((GLfloat) thiscolor[0]) / 255.0F),
	    (((GLfloat) thiscolor[1]) / 255.0F),
	    (((GLfloat) thiscolor[2]) / 255.0F),
	    1.0F);

	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(endpt1);
  glVertex3fv(endpt2);
  glEnd();
}

void
draw_edge_outline(evfptr thisevf, colortype thiscolor)
{
  vertype viewvec,edgevec,normalvec,negnormalvec,corner,projpt1,projpt2;
  vertype temp1,temp2,temp3,temp4,xformedpt1,xformedpt2;
  float *endpt1,*endpt2;
  int i;
  ;
  endpt1 = thisevf->le1->facevert->pos;
  endpt2 = thisevf->le2->facevert->pos;

  /* find endpoints in viewer space */
  transformpt(endpt1,vanillamatrix,xformedpt1);
  transformpt(endpt2,vanillamatrix,xformedpt2);
  /* find projection of endpts on viewplane (viewplane is at z=-3500) */
  temp1[vx] = -3500.0*xformedpt1[vx]/xformedpt1[vz];
  temp1[vy] = -3500.0*xformedpt1[vy]/xformedpt1[vz];
  temp1[vz] = -3500.0;
  temp2[vx] = -3500.0*xformedpt2[vx]/xformedpt2[vz];
  temp2[vy] = -3500.0*xformedpt2[vy]/xformedpt2[vz];
  temp2[vz] = -3500.0;
  /* construct unit vec along edge */
  diffvec3d(temp2,temp1,temp3);
  normalize(temp3,temp3);
  /* construct unit vec perpendicular to edge */
  temp4[vx] = temp3[vy];
  temp4[vy] = -temp3[vx];
  temp4[vz] = 0.0;
  normalize(temp4,temp4);

  /* but we really want pts in world coords for drawing routines... */
  for (i=0; i<3; i++)
  {
    projpt1[i] = temp1[0]*invanillamatrix[0][i] +
                 temp1[1]*invanillamatrix[1][i] +
	         temp1[2]*invanillamatrix[2][i] +
		 1.0*invanillamatrix[3][i];
    projpt2[i] = temp2[0]*invanillamatrix[0][i] +
                 temp2[1]*invanillamatrix[1][i] +
	         temp2[2]*invanillamatrix[2][i] +
		 1.0*invanillamatrix[3][i];
    edgevec[i] = temp3[0]*invanillamatrix[0][i] +
                 temp3[1]*invanillamatrix[1][i] +
	         temp3[2]*invanillamatrix[2][i];
    normalvec[i] = temp4[0]*invanillamatrix[0][i] +
                   temp4[1]*invanillamatrix[1][i] +
	           temp4[2]*invanillamatrix[2][i];
  }
  projpt1[vw] = projpt2[vw] = 1.0;
  normalvec[vw] = edgevec[vw] = 0.0;

  scalevec3d(normalvec,4.0,normalvec);
  scalevec3d(edgevec,4.0,edgevec);

  copyvec3d(normalvec,negnormalvec);
  flip_vec(negnormalvec);

  glColor4f((((GLfloat) thiscolor[0]) / 255.0F),
	    (((GLfloat) thiscolor[1]) / 255.0F),
	    (((GLfloat) thiscolor[2]) / 255.0F),
	    1.0F);

  glBegin(GL_LINE_LOOP);
  addvec3d(endpt1,edgevec,corner);
  addvec3d(corner,normalvec,corner);
  glVertex3fv(corner);
  addvec3d(endpt1,edgevec,corner);
  addvec3d(corner,negnormalvec,corner);
  glVertex3fv(corner);

  flip_vec(edgevec);

  addvec3d(endpt2,edgevec,corner);
  addvec3d(corner,negnormalvec,corner);
  glVertex3fv(corner);
  addvec3d(endpt2,edgevec,corner);
  addvec3d(corner,normalvec,corner);
  glVertex3fv(corner);
  glEnd();
}

void
draw_face_outline(fveptr thisfve, colortype thiscolor)
{
  vertype corner,bisector,vec1,vec2;
  vertype temp1,temp2,temp3,temp4,xformedpt1,xformedpt2,xformedpt3;
  float *pt1,*pt2,*pt3;
  leptr firstle,thisle;
  int i;
  ;
  firstle = thisle = thisfve->boundlist->first.bnd->lelist->first.le;
  glColor4f((((GLfloat) thiscolor[0]) / 255.0F),
	    (((GLfloat) thiscolor[1]) / 255.0F),
	    (((GLfloat) thiscolor[2]) / 255.0F),
	    1.0F);
  glBegin(GL_LINE_LOOP);
  do
  {
    pt1 = thisle->prev->facevert->pos;
    pt2 = thisle->facevert->pos;
    pt3 = thisle->next->facevert->pos;
    
    /* find points in viewer space */
    transformpt(pt1,vanillamatrix,xformedpt1);
    transformpt(pt2,vanillamatrix,xformedpt2);
    transformpt(pt3,vanillamatrix,xformedpt3);

    /* find projection of pts on viewplane (viewplane is at z=-3500) */
    temp1[vx] = -3500.0*xformedpt1[vx]/xformedpt1[vz];
    temp1[vy] = -3500.0*xformedpt1[vy]/xformedpt1[vz];
    temp1[vz] = -3500.0;

    temp2[vx] = -3500.0*xformedpt2[vx]/xformedpt2[vz];
    temp2[vy] = -3500.0*xformedpt2[vy]/xformedpt2[vz];
    temp2[vz] = -3500.0;

    temp3[vx] = -3500.0*xformedpt3[vx]/xformedpt3[vz];
    temp3[vy] = -3500.0*xformedpt3[vy]/xformedpt3[vz];
    temp3[vz] = -3500.0;

    /* construct unit vec bisector */
    diffvec3d(temp1,temp2,vec1);
    diffvec3d(temp2,temp3,vec2);
    flip_vec(vec2);
    addvec3d(vec1,vec2,temp4);
    normalize(temp4,temp4);
    
    /* but we really want bisector in world coords... */
    for (i=0; i<3; i++)
    {
      bisector[i] = temp4[0]*invanillamatrix[0][i] +
	            temp4[1]*invanillamatrix[1][i] +
		    temp4[2]*invanillamatrix[2][i];
    }

    scalevec3d(bisector,6.0,bisector);
    addvec3d(pt2,bisector,corner);

    glVertex3fv(corner);

    thisle = thisle->next;
  } while (thisle != firstle);
  glEnd();
}

  void
draw_wireframe_face(fveptr thisfve, colortype thiscolor)
{
  leptr thisle,firstle;
  boundptr thisbound;
  ;
  glColor4f((((GLfloat) thiscolor[0]) / 255.0F),
	    (((GLfloat) thiscolor[1]) / 255.0F),
	    (((GLfloat) thiscolor[2]) / 255.0F),
	    1.0F);

  thisbound = thisfve->boundlist->first.bnd;
  while(thisbound)
  {
    thisle = firstle = thisbound->lelist->first.le;
    glBegin(GL_LINE_LOOP);
    do
    {
      glVertex3fv(thisle->facevert->pos);
      thisle = thisle->next;
    } while (thisle != firstle);
    glEnd();
    thisbound = thisbound->next;
  }
}

  void
draw_shadowed_wireframe_face(fveptr thisfve, colortype thiscolor)
{
  leptr thisle,firstle;
  boundptr thisbound;
  ;
  glColor4f((((GLfloat) thiscolor[0]) / 255.0F),
	    (((GLfloat) thiscolor[1]) / 255.0F),
	    (((GLfloat) thiscolor[2]) / 255.0F),
	    1.0F);

  glPushMatrix();
  glTranslatef(2.0, -2.0, 0.0);
  thisbound = thisfve->boundlist->first.bnd;
  while(thisbound)
  {
    thisle = firstle = thisbound->lelist->first.le;
    glBegin(GL_LINE_LOOP);
    do
    {
      glVertex3fv(thisle->facevert->pos);
      thisle = thisle->next;
    } while (thisle != firstle);
    glEnd();
    thisbound = thisbound->next;
  }
  glPopMatrix();

  thisbound = thisfve->boundlist->first.bnd;
  while(thisbound)
  {
    thisle = firstle = thisbound->lelist->first.le;
    glBegin(GL_LINE_LOOP);
    do
    {
      glVertex3fv(thisle->facevert->pos);
      thisle = thisle->next;
    } while (thisle != firstle);
    glEnd();
    thisbound = thisbound->next;
  }
}
  
  void
draw_wireframe_shell(shellptr thishell, colortype thiscolor)
{
  evfptr thisevf;
  ;
  for (thisevf = thishell->evflist->first.evf; thisevf != Nil;
       thisevf = thisevf->next)
    draw_edge(thisevf,thiscolor);
}
  
  void
draw_shadowed_wireframe_shell(shellptr thishell, colortype thiscolor)
{
  evfptr thisevf;
  ;
  glPushMatrix();
  glTranslatef(2.0, -2.0, 0.0);
  for (thisevf = thishell->evflist->first.evf; thisevf != Nil;
       thisevf = thisevf->next)
    draw_edge(thisevf,black);
  glPopMatrix();
  for (thisevf = thishell->evflist->first.evf; thisevf != Nil;
       thisevf = thisevf->next)
    draw_edge(thisevf,thiscolor);
}

  void
draw_hidden_line_obj(objptr thisobj, colortype thiscolor)
{
  shellptr thishell;
  evfptr thisevf;
  fveptr fve1,fve2;
  ;
  for (thishell = First_obj_shell(thisobj); thishell != Nil; thishell =
       thishell->next)
  {
    for (thisevf = thishell->evflist->first.evf; thisevf != Nil;
	 thisevf = thisevf->next)
    {
      fve1 = thisevf->le1->motherbound->motherfve;
      fve2 = thisevf->le2->motherbound->motherfve;
      if (!edge_is_hidden(thisevf,backface_fve(fve1),backface_fve(fve2)))
	draw_edge(thisevf,thiscolor);
    }
  }
}

  void
draw_shadowed_hidden_line_shell(shellptr thishell, colortype thiscolor)
{
  evfptr thisevf;
  fveptr fve1,fve2;
  ;
  glPushMatrix();
  glTranslatef(2.0, -2.0, 0.0);
  for (thisevf = thishell->evflist->first.evf; thisevf != Nil;
       thisevf = thisevf->next)
  {
    fve1 = thisevf->le1->motherbound->motherfve;
    fve2 = thisevf->le2->motherbound->motherfve;
    if (!edge_is_hidden(thisevf,backface_fve(fve1),backface_fve(fve2)))
      draw_edge(thisevf,black);
  }
  glPopMatrix();
  for (thisevf = thishell->evflist->first.evf; thisevf != Nil;
       thisevf = thisevf->next)
  {
    fve1 = thisevf->le1->motherbound->motherfve;
    fve2 = thisevf->le2->motherbound->motherfve;
    if (!edge_is_hidden(thisevf,backface_fve(fve1),backface_fve(fve2)))
      draw_edge(thisevf,thiscolor);
  }
}

  void
draw_wireframe_obj(objptr thisobj, colortype thiscolor)
{
  shellptr thishell;
  ;
  thishell = First_obj_shell(thisobj);
  while (thishell)
  {
    draw_wireframe_shell(thishell,thiscolor);
    thishell = thishell->next;
  }
}

  void
draw_shadowed_wireframe_object(objptr thisobj, colortype thiscolor)
{
  shellptr thishell;
  ;
  thishell = First_obj_shell(thisobj);
  while (thishell)
  {
    draw_shadowed_wireframe_shell(thishell,thiscolor);
    thishell = thishell->next;
  }
}

  void
draw_shadowed_hidden_line_obj(objptr thisobj, colortype thiscolor)
{
  shellptr thishell;
  ;
  thishell = First_obj_shell(thisobj);
  while (thishell)
  {
    draw_shadowed_hidden_line_shell(thishell,thiscolor);
    thishell = thishell->next;
  }
}

/* draw a face as a solid polygon in the passed color... simpler than */
/* the routines in shade.c and doesn't draw innerbounds at all. */

  void
draw_solid_face(fveptr thisfve,colortype thiscolor)
{
  boundptr outerbnd;
  leptr firstle,thisle;
  ;
  glDisable(GL_LIGHTING);
  glColor4f((((GLfloat) thiscolor[0]) / 255.0F),
	    (((GLfloat) thiscolor[1]) / 255.0F),
	    (((GLfloat) thiscolor[2]) / 255.0F),
	    1.0F);
  glBegin(GL_POLYGON);
  outerbnd = thisfve->boundlist->first.bnd;
  firstle = thisle = outerbnd->lelist->first.le;
  do
  {
    glVertex3fv(thisle->facevert->pos);
    thisle = thisle->next;
  } while (thisle != outerbnd->lelist->first.le);
  glVertex3fv(thisle->facevert->pos); /* draw back to first again */
  glEnd();
  glEnable(GL_LIGHTING);
}  

/*********************** outer contour drawing routines ***********************/

GLboolean
isa_contour(evfptr thisevf)
{
  return (backface_fve(thisevf->le1->motherbound->motherfve) !=
	  backface_fve(thisevf->le2->motherbound->motherfve));
}

GLboolean
is_onend(evfptr thisevf)
{
  static vertype xformedpos1,xformedpos2;
  ;
  /* this works!! yehaah! */
  transformpt(thisevf->le1->facevert->pos,vanillamatrix,xformedpos1);
  transformpt(thisevf->le2->facevert->pos,vanillamatrix,xformedpos2);
  normalize(xformedpos1,xformedpos1);
  normalize(xformedpos2,xformedpos2);

  return (compare(fabs(dotvecs(xformedpos1,xformedpos2)),1.0,1.0e-4) == 0);
}

leptr
find_next_contour(leptr thisle)
{
  GLboolean done = FALSE;
  leptr firstle, degeneratele = Nil;
  ;
  firstle = Twin_le(thisle);
  thisle = thisle->next;
  do 
  {
    if (isa_contour(thisle->facedge))
      break;
    else if (is_onend(thisle->facedge))
      degeneratele = thisle;
    thisle = Twin_le(thisle)->next;
  } while (thisle != firstle);

  if (thisle != firstle)
    return (thisle);
  else
    return (degeneratele);
}

 void
compute_outdented_edge(evfptr contour_edge,GLfloat outdent_length,
		       vertype outdentpt1,vertype outdentpt2)
{
  fveptr f1,f2;
  vertype bisector;
  ;
  f1 = contour_edge->le1->motherbound->motherfve;
  f2 = contour_edge->le2->motherbound->motherfve;
  addvec3d(f1->facenorm,f2->facenorm,bisector);
  normalize(bisector,bisector);
  scalevec3d(bisector,outdent_length,bisector);
  addpos3d(contour_edge->le1->facevert->pos,bisector,outdentpt1);
  addpos3d(contour_edge->le2->facevert->pos,bisector,outdentpt2);
}

void
draw_shell_contour(shellptr thishell, GLfloat contouroffset,colortype thecolor)
{
  leptr tople,firstle,thisle;
	/* OGLXXX XXX fix this. */
  Matrix combomatrix,resultmatrix;
  vfeptr vfemax,thisvfe;
  vertype projpos;
  GLfloat thisy,maxprojy,maxprojz;
  evfptr firstcontourevf = Nil;
  vertype stupidpts[500],tempstupid;	/* hack */
  int i,numstupidpts;		/* hack */
  ;
  /* need global var to keep proj in and should move matmult routines into */
  /* math3d.c */
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) combomatrix);
  multmatrix_4x4(combomatrix,projmatrix,resultmatrix);
  thisvfe = thishell->vfelist->first.vfe;
  vfemax = thisvfe;
  transformpt(thisvfe->pos,resultmatrix,projpos);
  maxprojy = projpos[vy]/projpos[vw];
  maxprojz = projpos[vz]/projpos[vw];
  while (thisvfe)
  {
    transformpt(thisvfe->pos,resultmatrix,projpos);
    thisy = projpos[vy]/projpos[vw];
    if (thisy > maxprojy)
    {
      vfemax = thisvfe;
      maxprojy = projpos[vy]/projpos[vw];
      maxprojz = projpos[vz]/projpos[vw];
    }
    else if (thisy == maxprojy)
      /* we take back most one assuming side pointing faces are considered */
      /* to be front facing (which backface_fve does) */
      if (projpos[vz]/projpos[vw] < maxprojz)
      {
	vfemax = thisvfe;
	maxprojy = thisy;
	maxprojz = projpos[vz]/projpos[vw];
      }
    thisvfe = thisvfe->next;
  }

  if (vfemax->startle->facedge == Nil)
    return;

  /* find first contour */
  firstle = thisle = vfemax->startle;
  do
  {
    if (isa_contour(thisle->facedge))
    {
      firstcontourevf = thisle->facedge;
      break;
    }
    thisle = Twin_le(thisle)->next;
  } while (thisle != firstle);

  if (firstcontourevf == Nil)
    return;
  
  glColor4f((((GLfloat) thecolor[0]) / 255.0F),
	    (((GLfloat) thecolor[1]) / 255.0F),
	    (((GLfloat) thecolor[2]) / 255.0F),
	    1.0F);

  i = 0;
  compute_outdented_edge(firstcontourevf,contouroffset,contourpt1,contourpt2);
  copypos3d(contourpt1,stupidpts[i++]);
  copypos3d(contourpt2,stupidpts[i++]);

  /* walk around the contour */
  do
  {
    thisle = find_next_contour(thisle);
    if (thisle)
    {
      compute_outdented_edge(thisle->facedge,contouroffset,
			     contourpt1,contourpt2);
      if (distance2(stupidpts[i-1],contourpt1) <
	  distance2(stupidpts[i-1],contourpt2))
      {
	copypos3d(contourpt1,stupidpts[i++]);
	copypos3d(contourpt2,stupidpts[i++]);
      }
      else
      {
	copypos3d(contourpt2,stupidpts[i++]);
	copypos3d(contourpt1,stupidpts[i++]);
      }
    }
  } while ((thisle != Nil) && (thisle->facedge != firstcontourevf));

  numstupidpts = i;

  /* draw the first end case separately so the lines don't get crossed */
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  if (distance2(stupidpts[0],stupidpts[2]) <
      distance2(stupidpts[1],stupidpts[2]))
  {
    glVertex3fv(stupidpts[1]);
    glVertex3fv(stupidpts[0]);
  }
  else
  {
    glVertex3fv(stupidpts[0]);
    glVertex3fv(stupidpts[1]);
  }
  /* continue with the rest of the contour */
  i = 2;
  while (i < numstupidpts)
    glVertex3fv(stupidpts[i++]);
  glEnd();

  /* also draw the end last case separately so the lines don't cross */
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(stupidpts[numstupidpts-1]);
  if (distance2(stupidpts[numstupidpts-1],stupidpts[0]) <
      distance2(stupidpts[numstupidpts-1],stupidpts[1]))
    glVertex3fv(stupidpts[0]);
  else
    glVertex3fv(stupidpts[1]);
  glEnd();

}

  void
draw_obj_contour(objptr thisobj, GLfloat contouroffset,colortype thecolor)
{
  /* only draw outer contour */
  glDisable(GL_LIGHTING);
  draw_shell_contour(First_obj_shell(thisobj),contouroffset,thecolor);
  glEnable(GL_LIGHTING);
}

void
draw_surf_edges(surfptr thisurf, colortype thecolor)
{
  fveptr thisfve;
  boundptr thisbound;
  leptr firstle,thisle;
  elemptrptr ptr2fve;
  ;
  /* find a face on the boundary of the surface */
  ptr2fve = thisurf->fvelist->first.ep;
  while (ptr2fve)
  {
    thisbound = ((fveptr) (ptr2fve)->thiselem)->boundlist->first.bnd;
    while (thisbound)
    {
      firstle = thisle = thisbound->lelist->first.le;
      do
      {
	if (Twin_le(thisle)->motherbound->motherfve->mothersurf != thisurf)
	  draw_edge(thisle->facedge,thecolor);
	thisle = thisle->next;
      } while (thisle != firstle);
      thisbound = thisbound->next;
    }
    ptr2fve = ptr2fve->next;
  }
}

/************************* dimension drawing routines ************************/


/* NOTE: works wrong on concave edges cause compute_outdented_edge is not */
/* smart enough. */

  static void
draw_edge_dimension(evfptr thisevf,colortype linescolor,colortype textcolor)
{
  GLfloat *evfpt1,*evfpt2,*outwardsvec,edgelen;
  vertype dimpt1,dimpt2,mid_dim;
  vertype innervec,extendvec,dimvec;
  vertype marginpt1,marginpt2,extendpt1,extendpt2;
  char dimoutstr[20];		/* hack should be properly sized! */
  ;
  evfpt1 = thisevf->le1->facevert->pos;
  evfpt2 = thisevf->le2->facevert->pos;
  outwardsvec = thisevf->le1->motherbound->motherfve->facenorm;
  scalevec3d(outwardsvec,Edgedimensionmargin,innervec);
  scalevec3d(outwardsvec,Edgedimensionmargin + Edgedimensionoffset -
	     Edgedimensionindent,dimvec);
  scalevec3d(outwardsvec,Edgedimensionmargin + Edgedimensionoffset,extendvec);
  addvec3d(evfpt1,innervec,marginpt1);
  addvec3d(evfpt2,innervec,marginpt2);
  addvec3d(evfpt1,dimvec,dimpt1);
  addvec3d(evfpt2,dimvec,dimpt2);
  addvec3d(evfpt1,extendvec,extendpt1);
  addvec3d(evfpt2,extendvec,extendpt2);

  glColor4f((((GLfloat) linescolor[0]) / 255.0F),
	    (((GLfloat) linescolor[1]) / 255.0F),
	    (((GLfloat) linescolor[2]) / 255.0F),
	    1.0F);

  glLineWidth(2.0F);
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(marginpt1);
  glVertex3fv(extendpt1);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(marginpt2);
  glVertex3fv(extendpt2);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  glVertex3fv(dimpt1);
  glVertex3fv(dimpt2);
  glEnd();
  /* if the hasdimension_prop record has a nonzero value, draw the numeric */
  /* portion of the dimension. */
  if (get_property_val((featureptr) thisevf,hasdimension_prop).i == TRUE)
  {
    glLineWidth(1.0F);

    glColor4f((((GLfloat) textcolor[0]) / 255.0F),
	      (((GLfloat) textcolor[1]) / 255.0F),
	      (((GLfloat) textcolor[2]) / 255.0F),
	      1.0F);

    edgelen = distance(evfpt1,evfpt2);
    sprintf(dimoutstr,"%2.2f",edgelen);
    if (abs(thisevf->cutype) == Wascut)
    {				/* draw text on plane if possible */
      copypos3d(thisevf->cutpt,mid_dim);
      scalevec3d(outwardsvec,Edgedimensionmargin * 3 + Edgedimensionoffset,
		 extendvec);
      addvec3d(mid_dim,extendvec,mid_dim);
      mid_dim[vz] += Dimensionoffplane;
    }
    else			/* otherwise draw at evf's midpt */
      midpoint(dimpt1,dimpt2,mid_dim);
    objstr(dimoutstr,mid_dim,2.5,2.0);
  }
}

  static void
draw_obj_dimensions(objptr thisobj)
{
}

  static void
draw_dimensioned_features(worldptr world)
{
  proptr thisprop;
  featureptr thisfeature;
  ;
  thisprop = world->world_proplists[(int) hasdimension_prop]->first.prop;
  while (thisprop != Nil)
  {
    thisfeature = thisprop->owner;
    if (thisfeature->type_id == Evf_type)
    {
      draw_edge_dimension((evfptr) thisfeature,black,black);
    }
    else if (thisfeature->type_id == Obj_type)
    {
      draw_obj_dimensions((objptr) thisfeature);
    }

    thisprop = thisprop->next;
  }
}

  

/************************* shadow drawing routines ************************/

  static void
draw_face_shadow(fveptr thisfve,GLboolean object_is_transparent)
{
  leptr thisle,firstle;
  vertype pos;
  ;
  /* only draw faces pointing up */
  if (thisfve->facenorm[vy] <= 0.0)
    return;

  firstle = thisfve->boundlist->first.bnd->lelist->first.le;
  thisle = firstle;
  if (object_is_transparent)
    glBegin(GL_LINE_LOOP);
  else
	/* OGLXXX
	 * special cases for polygons:
	 * 	independant quads: use GL_QUADS
	 * 	independent triangles: use GL_TRIANGLES
	 */
    glBegin(GL_POLYGON);
  do
  {
    pos[vx] = thisle->facevert->pos[vx];
    pos[vy] = Mincrossy;
    pos[vz] = thisle->facevert->pos[vz];
    glVertex3fv(pos);
    thisle = thisle->next;
  } while (thisle != firstle);
  if (object_is_transparent)
    glEnd();
  else
    glEnd();
}

  static void
draw_face_shadow_backwall(fveptr thisfve)
{
  leptr thisle,firstle;
  vertype pos;
  ;
  /* only draw faces pointing forwards */
  if (thisfve->facenorm[vz] <= 0.0)
    return;

  firstle = thisfve->boundlist->first.bnd->lelist->first.le;
  thisle = firstle;
  glColor4f((((GLfloat) shadowcolor[0]) / 255.0F),
	    (((GLfloat) shadowcolor[1]) / 255.0F),
	    (((GLfloat) shadowcolor[2]) / 255.0F),
	    1.0F);
	/* OGLXXX
	 * special cases for polygons:
	 * 	independant quads: use GL_QUADS
	 * 	independent triangles: use GL_TRIANGLES
	 */
  glBegin(GL_POLYGON);
  do
  {
    pos[vx] = thisle->facevert->pos[vx];
    pos[vy] = thisle->facevert->pos[vy];
    pos[vz] = Minplanez;
    glVertex3fv(pos);
    thisle = thisle->next;
  } while (thisle != firstle);
  glEnd();
}

  static void
draw_face_shadow_sidewall(fveptr thisfve)
{
  leptr thisle,firstle;
  vertype pos;
  GLboolean leftside;
  ;
  leftside = (state->roomroty < 0);
    
  if (leftside)
  {
    /* only draw faces pointing right */
    if (thisfve->facenorm[vx] < 0.0)
      return;
  }
  else /* only draw faces pointing left */
    if (thisfve->facenorm[vx] > 0.0)
      return;

  firstle = thisfve->boundlist->first.bnd->lelist->first.le;
  thisle = firstle;
  glColor4f((((GLfloat) shadowcolor[0]) / 255.0F),
	    (((GLfloat) shadowcolor[1]) / 255.0F),
	    (((GLfloat) shadowcolor[2]) / 255.0F),
	    1.0F);

	/* OGLXXX
	 * special cases for polygons:
	 * 	independant quads: use GL_QUADS
	 * 	independent triangles: use GL_TRIANGLES
	 */
  glBegin(GL_POLYGON);
  do
  {
    if (leftside)
      pos[vx] = Mincrossx;
    else
      pos[vx] = Maxcrossx;
    pos[vy] = thisle->facevert->pos[vy];
    pos[vz] = thisle->facevert->pos[vz];
    glVertex3fv(pos);
    thisle = thisle->next;
  } while (thisle != firstle);
  glEnd();
}

  void
draw_obj_shadow(objptr thisobj)
{
  fveptr thisfve;
  GLboolean object_is_transparent;
  GLboolean lighting_was_on;
  ;
  /* only draw shadow of outer shell */
  lighting_was_on = glIsEnabled(GL_LIGHTING);
  if (lighting_was_on)
    glDisable(GL_LIGHTING);
  thisfve = First_obj_fve(thisobj);
  glColor4f((((GLfloat) shadowcolor[0]) / 255.0F),
	    (((GLfloat) shadowcolor[1]) / 255.0F),
	    (((GLfloat) shadowcolor[2]) / 255.0F),
	    1.0F);
  object_is_transparent = has_property((featureptr) thisobj,transparent_prop);
  while (thisfve != Nil)
  {
    draw_face_shadow(thisfve,object_is_transparent);
    thisfve = thisfve->next;
  }
  if (lighting_was_on)
    glEnable(GL_LIGHTING);
}

  void
draw_shadows(worldptr world)
{
  proptr thisprop;
  featureptr thisfeature;
  ;
	/* OGLXXX
	 * use gluBeginPolygon(tobj) to draw concave polys
	 * concave(TRUE)
	 */
  /*DELETED*/;
  glDisable(GL_LIGHTING);
  thisprop = world->world_proplists[(int) visible_prop]->first.prop;
  while (thisprop != Nil)
  {
    thisfeature = thisprop->owner;
    if (thisfeature->type_id == Obj_type)
      draw_obj_shadow((objptr) thisfeature);
    thisprop = thisprop->next;
  }
	/* OGLXXX
	 * use gluBeginPolygon(tobj) to draw concave polys
	 * concave(FALSE)
	 */
  /*DELETED*/;
  glEnable(GL_LIGHTING);
}

/***************** pick box and selectability drawing routines **************/

  void
draw_pick_box(vertype drawpt, int pick_linewidth,GLboolean filled)
{
  fprintf (dbgdump, "in draw_pick_box\n");
  fflush (dbgdump);

  /* move to plane z coord */
  glLineWidth((GLfloat)(pick_linewidth));
  glTranslatef(0.0, 0.0, drawpt[vz]);
  if (filled == Filledpkbox)	/* note I added +2 to Pkboxsize for */
				/* Hughes Demo 8/9/90 -- wak*/
    glRectf(drawpt[vx] - Pkboxsize, drawpt[vy] - Pkboxsize,
	    drawpt[vx] + Pkboxsize, drawpt[vy] + Pkboxsize);
  else {
    /* OGLXXX rect: remove extra PolygonMode changes */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(drawpt[vx] - Pkboxsize, drawpt[vy] - Pkboxsize,
	    drawpt[vx] + Pkboxsize, drawpt[vy] + Pkboxsize);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  glTranslatef(0.0, 0.0, -drawpt[vz]);
  glLineWidth(1.0F);
}

  void
draw_square(vertype centerpt, float width, GLboolean filled)
{

  fprintf (dbgdump, "in draw_square\n");
  fflush (dbgdump);

  glDisable(GL_LIGHTING);
  glTranslatef(0.0, 0.0, centerpt[vz]);
  if (filled)
    glRectf(centerpt[vx] - width, centerpt[vy] - width,
	    centerpt[vx] + width, centerpt[vy] + width);
  else {
    /* OGLXXX rect: remove extra PolygonMode changes */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(centerpt[vx] - width, centerpt[vy] - width,
	    centerpt[vx] + width, centerpt[vy] + width);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  glTranslatef(0.0, 0.0, -centerpt[vz]);
  glEnable(GL_LIGHTING);

}

  void
draw_shadowed_pick_box(vertype drawpt, colortype boxcolor, GLboolean filled)
{
  vertype offsetvert;
  colortype darker_boxcolor;
  GLboolean lighting_was_on;
  ;
  fprintf (dbgdump, "in draw_shadowed_pick_box\n");
  fflush (dbgdump);

  lighting_was_on = glIsEnabled(GL_LIGHTING);
  if (lighting_was_on)
    glDisable(GL_LIGHTING);

  scalecolor(boxcolor,0.35,darker_boxcolor);
  glColor4f((((GLfloat) darker_boxcolor[0]) / 255.0F),
	    (((GLfloat) darker_boxcolor[1]) / 255.0F),
	    (((GLfloat) darker_boxcolor[2]) / 255.0F),
	    1.0F);
  glDepthFunc(GL_ALWAYS);
	/* OGLXXX setlinestyle: Check list numbering. */
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1,Largedotpattern);

  draw_pick_box(drawpt,1,Hollowpkbox);

  glDisable(GL_LINE_STIPPLE);
  glDepthFunc(GL_LEQUAL);

  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);
  offsetvert[vx] = drawpt[vx] + 2.0;
  offsetvert[vy] = drawpt[vy] - 2.0;
  offsetvert[vz] = drawpt[vz];
  draw_pick_box(offsetvert,2,filled);
  glColor4f((((GLfloat) boxcolor[0]) / 255.0F),
	    (((GLfloat) boxcolor[1]) / 255.0F),
	    (((GLfloat) boxcolor[2]) / 255.0F),
	    1.0F);
  draw_pick_box(drawpt,2,filled);

  if (lighting_was_on)
    glEnable(GL_LIGHTING);
}

  void
draw_bounding_box(bbox3dptr thebbox)
{
  GLfloat xleft,xright,ybot,ytop,znear,zfar;
  vertype cornersquare,minusecorner,maxusecorner;
  char outstr[50];
  GLboolean lighting_was_on;
  ;
  lighting_was_on = glIsEnabled(GL_LIGHTING);
  if (lighting_was_on)
    glDisable(GL_LIGHTING);

  /* OGLXXX setlinestyle: Check list numbering. */
  if(0) {glCallList(0); glEnable(GL_LINE_STIPPLE);} else glDisable(GL_LINE_STIPPLE);
  glLineWidth(1.0F);
  xleft = thebbox->mincorner[vx];
  xright = thebbox->maxcorner[vx];
  ybot = thebbox->mincorner[vy];
  ytop = thebbox->maxcorner[vy];
  znear = thebbox->mincorner[vz];
  zfar = thebbox->maxcorner[vz];

  /* OGLXXX glBegin: Use GL_LINES if only one line segment is desired. */
  glBegin(GL_LINE_STRIP); glVertex3f(xleft, ytop, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xright, ytop, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xright, ybot, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xleft, ybot, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xleft, ytop, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xleft, ytop, zfar);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xright, ytop, zfar);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xright, ybot, zfar);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xleft, ybot, zfar);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xleft, ytop, zfar);
	/* OGLXXX glBegin: Use GL_LINES if only one line segment is desired. */
  glBegin(GL_LINE_STRIP); glVertex3f(xright, ytop, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xright, ytop, zfar);
	/* OGLXXX glBegin: Use GL_LINES if only one line segment is desired. */
  glBegin(GL_LINE_STRIP); glVertex3f(xright, ybot, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xright, ybot, zfar);
	/* OGLXXX glBegin: Use GL_LINES if only one line segment is desired. */
  glBegin(GL_LINE_STRIP); glVertex3f(xleft, ybot, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xleft, ybot, zfar);
/*
  glColor4f((((GLfloat) thisobj->thecolor[0]) / 255.0F),
	    (((GLfloat) thisobj->thecolor[1]) / 255.0F),
	    (((GLfloat) thisobj->thecolor[2]) / 255.0F),
	    1.0F);
  if (thisobj->obj_name != Nil)
    objstr(thisobj->obj_name,thebbox->mincorner,1.0,1.0);
  else
  {
    sprintf(outstr,"%x",thisobj);
    objstr(outstr,thebbox->mincorner,1.0,1.0);
  }
*/
  if (lighting_was_on)
    glEnable(GL_LIGHTING);
}

  void
draw_shells_pick_box(void)
{
}


  void
draw_scalebbox_technique(objptr scalebbox,stateptr state)
{
#ifdef standardway
  GLfloat xleft,xright,ybot,ytop,znear,zfar;
  vertype cornersquare,minusecorner,maxusecorner;
  colortype boxcolor;
  ;  
  if ( (state->cursor[vz] < scalebbox->groupbbox.mincorner[vz] - BBoxoutdent) ||
      (state->cursor[vz] > scalebbox->groupbbox.maxcorner[vz] + BBoxoutdent))
      return;

  setpos3d(minusecorner,scalebbox->groupbbox.mincorner[vx],
	   scalebbox->groupbbox.mincorner[vy],state->cursor[vz]+5.0);
  setpos3d(maxusecorner,scalebbox->groupbbox.maxcorner[vx],
	   scalebbox->groupbbox.maxcorner[vy],state->cursor[vz]+5.0);
  if ((compare(state->cursor[vz],
	       scalebbox->groupbbox.mincorner[vz],1.0) == 0) ||
      (compare(state->cursor[vz],
	       scalebbox->groupbbox.maxcorner[vz],1.0) == 0))
    copycolor(selectablecolor,boxcolor);
  else
    copycolor(lightgrey,boxcolor);

  glLineWidth(2.0F);
  setpos3d(cornersquare,minusecorner[vx], minusecorner[vy],minusecorner[vz]);
  if (pt_near_pt(cornersquare,state->cursor,Pkboxsize))
    draw_shadowed_pick_box(cornersquare,yellow,FALSE);
  else
    draw_shadowed_pick_box(cornersquare,boxcolor,FALSE);

  setpos3d(cornersquare,minusecorner[vx], maxusecorner[vy],minusecorner[vz]);
  if (pt_near_pt(cornersquare,state->cursor,Pkboxsize))
    draw_shadowed_pick_box(cornersquare,yellow,FALSE);
  else
    draw_shadowed_pick_box(cornersquare,boxcolor,FALSE);

  setpos3d(cornersquare,maxusecorner[vx], minusecorner[vy],minusecorner[vz]);
  if (pt_near_pt(cornersquare,state->cursor,Pkboxsize))
    draw_shadowed_pick_box(cornersquare,yellow,FALSE);
  else
    draw_shadowed_pick_box(cornersquare,boxcolor,FALSE);

  setpos3d(cornersquare,maxusecorner[vx], maxusecorner[vy],minusecorner[vz]);
  if (pt_near_pt(cornersquare,state->cursor,Pkboxsize))
    draw_shadowed_pick_box(cornersquare,yellow,FALSE);
  else
    draw_shadowed_pick_box(cornersquare,boxcolor,FALSE);

#else
  vfeptr thisvfe;
  evfptr thisevf;
  GLboolean pickstatus,draw_bbox_also = FALSE,selectable_check;
  static vertype cutoffset = {0.0,0.0,5.0};
  vertype pos1;
  ;
  thisevf = First_obj_evf(scalebbox);
  while (thisevf != Nil)
  {
    if (abs(thisevf->cutype) == Wascut)
    {
      pickstatus = has_property((featureptr) thisevf, picked_prop);
      draw_bbox_also |= pickstatus;
      addpos3d(thisevf->cutpt,cutoffset,pos1);
      if (selectable_check = has_property((featureptr) thisevf,selectable_prop))
	draw_shadowed_pick_box(pos1,yellow,
			       pickstatus);
      else
	draw_shadowed_pick_box(pos1,lightgrey, pickstatus);
      draw_bbox_also |= selectable_check;
    }
    thisevf = thisevf->next;
  }
  thisvfe = First_obj_vfe(scalebbox);
  while (thisvfe != Nil)
  {
    pickstatus = has_property((featureptr) thisvfe, picked_prop);
    draw_bbox_also |= pickstatus;
    addpos3d(thisvfe->pos,cutoffset,pos1);
    if (selectable_check = has_property((featureptr) thisvfe, selectable_prop))
      draw_shadowed_pick_box(pos1,yellow, pickstatus);
    else if (compare(state->cursor[vz],thisvfe->pos[vz],Pkboxsize/2.0) == 0)
      draw_shadowed_pick_box(pos1,selectablecolor,FALSE);
    thisvfe = thisvfe->next;
    draw_bbox_also |= selectable_check;
  }
  if (draw_bbox_also)
  {
    glLineWidth(2.0F);
    draw_wireframe_obj(scalebbox,darkgrey);
    draw_obj_intersection(scalebbox);
  }
#endif  
}


#ifdef YoucancutmesomedaybutnotquiteYET

  void
draw_pick_bbox(objptr thisobj,stateptr state)
{
  GLfloat xleft,xright,ybot,ytop,znear,zfar;
  vertype cornersquare,minusecorner,maxusecorner;
  colortype boxcolor;
  ;
#if 0
	/* OGLXXX setlinestyle: Check list numbering. */
  if(0) {glCallList(0); glEnable(GL_LINE_STIPPLE);} else glDisable(GL_LINE_STIPPLE);
  setcolor(darkgrey);
  xleft = thisobj->groupbbox.mincorner[vx] - BBoxoutdent;
  xright = thisobj->groupbbox.maxcorner[vx] + BBoxoutdent;
  ybot = thisobj->groupbbox.mincorner[vy] - BBoxoutdent;
  ytop = thisobj->groupbbox.maxcorner[vy] + BBoxoutdent;
  znear = thisobj->groupbbox.mincorner[vz] - BBoxoutdent;
  zfar = thisobj->groupbbox.maxcorner[vz] + BBoxoutdent;

	/* OGLXXX glBegin: Use GL_LINES if only one line segment is desired. */
  glBegin(GL_LINE_STRIP); glVertex3f(xleft, ytop, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xright, ytop, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xright, ybot, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xleft, ybot, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xleft, ytop, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xleft, ytop, zfar);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xright, ytop, zfar);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xright, ybot, zfar);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xleft, ybot, zfar);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xleft, ytop, zfar);
	/* OGLXXX glBegin: Use GL_LINES if only one line segment is desired. */
  glBegin(GL_LINE_STRIP); glVertex3f(xright, ytop, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xright, ytop, zfar);
	/* OGLXXX glBegin: Use GL_LINES if only one line segment is desired. */
  glBegin(GL_LINE_STRIP); glVertex3f(xright, ybot, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xright, ybot, zfar);
	/* OGLXXX glBegin: Use GL_LINES if only one line segment is desired. */
  glBegin(GL_LINE_STRIP); glVertex3f(xleft, ybot, znear);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
  glVertex3f(xleft, ybot, zfar);
#endif

  if ( (state->cursor[vz] < thisobj->groupbbox.mincorner[vz] - BBoxoutdent) ||
      (state->cursor[vz] > thisobj->groupbbox.maxcorner[vz] + BBoxoutdent) ||
      has_property((featureptr) thisobj,lockedgeometry_prop) ||
      has_property((featureptr) thisobj,nonmanifold_prop) )
    return;

  setpos3d(minusecorner,thisobj->groupbbox.mincorner[vx] - BBoxoutdent,
	   thisobj->groupbbox.mincorner[vy] - BBoxoutdent,state->cursor[vz]);
  setpos3d(maxusecorner,thisobj->groupbbox.maxcorner[vx] + BBoxoutdent,
	   thisobj->groupbbox.maxcorner[vy] + BBoxoutdent,state->cursor[vz]);
  if ((compare(state->cursor[vz],
	       thisobj->groupbbox.mincorner[vz] - BBoxoutdent,1.0) == 0) ||
      (compare(state->cursor[vz],
	       thisobj->groupbbox.maxcorner[vz] + BBoxoutdent,1.0) == 0))
    copycolor(selectablecolor,boxcolor);
  else
    copycolor(lightgrey,boxcolor);

  glLineWidth(2.0F);
  setpos3d(cornersquare,minusecorner[vx], minusecorner[vy],minusecorner[vz]);
  if (pt_near_pt(cornersquare,state->cursor,Pkboxsize))
    draw_shadowed_pick_box(cornersquare,yellow,FALSE);
  else
    draw_shadowed_pick_box(cornersquare,boxcolor,FALSE);

  setpos3d(cornersquare,minusecorner[vx], maxusecorner[vy],minusecorner[vz]);
  if (pt_near_pt(cornersquare,state->cursor,Pkboxsize))
    draw_shadowed_pick_box(cornersquare,yellow,FALSE);
  else
    draw_shadowed_pick_box(cornersquare,boxcolor,FALSE);

  setpos3d(cornersquare,maxusecorner[vx], minusecorner[vy],minusecorner[vz]);
  if (pt_near_pt(cornersquare,state->cursor,Pkboxsize))
    draw_shadowed_pick_box(cornersquare,yellow,FALSE);
  else
    draw_shadowed_pick_box(cornersquare,boxcolor,FALSE);

  setpos3d(cornersquare,maxusecorner[vx], maxusecorner[vy],minusecorner[vz]);
  if (pt_near_pt(cornersquare,state->cursor,Pkboxsize))
    draw_shadowed_pick_box(cornersquare,yellow,FALSE);
  else
    draw_shadowed_pick_box(cornersquare,boxcolor,FALSE);
}  

#endif

  GLboolean
surface_interior_feature(featureptr thisfeature)
{
  leptr firstle,thisle;
#ifdef IRIS
  vfeptr thisvfe;
  fveptr thisfve;
#endif
  evfptr thisevf;
  surfptr surf1,surf2;
  ;
  switch (thisfeature->type_id)
  {
  case Vfe_type:
    firstle = thisle = ((vfeptr) thisfeature)->startle;
    surf1 = thisle->motherbound->motherfve->mothersurf;
    if (surf1 == Nil)
      return (FALSE);
    do
    {
      surf2 = thisle->motherbound->motherfve->mothersurf;
      if ((surf2 != surf1) || (surf2 == Nil))
	return (FALSE);
      thisle = Twin_le(thisle)->next;
    } while (thisle != firstle);
    return (TRUE);
  case Evf_type:
    thisevf = (evfptr) thisfeature;
    surf1 = thisevf->le1->motherbound->motherfve->mothersurf;
    surf2 = thisevf->le2->motherbound->motherfve->mothersurf;
    return ((surf1 != Nil) && (surf1 == surf2));
  case Fve_type:
    return ((((fveptr) thisfeature)->mothersurf != Nil) &&
	    (((fveptr) thisfeature)->mothersurf->fvelist->numelems > 1));
  default:
    return (FALSE);
  }
}

  surfptr
find_mothersurf(featureptr thisfeature)
{
  leptr firstle,thisle;
#ifdef IRIS
  vfeptr thisvfe;
  fveptr thisfve;
#endif
  evfptr thisevf;
  surfptr surf1,surf2;
  ;
  switch (thisfeature->type_id)
  {
  case Vfe_type:
    firstle = thisle = ((vfeptr) thisfeature)->startle;
    surf1 = thisle->motherbound->motherfve->mothersurf;
    if (surf1 == Nil)
      return (FALSE);
    do
    {
      surf2 = thisle->motherbound->motherfve->mothersurf;
      if ((surf2 != surf1) || (surf2 == Nil))
	return (Nil);
      thisle = Twin_le(thisle)->next;
    } while (thisle != firstle);
    return (surf1);
  case Evf_type:
    thisevf = (evfptr) thisfeature;
    surf1 = thisevf->le1->motherbound->motherfve->mothersurf;
    surf2 = thisevf->le2->motherbound->motherfve->mothersurf;
    if ((surf1 != Nil) && (surf1 == surf2))
      return (surf1);
    return (Nil);
  case Fve_type:
    surf1 = ((fveptr) thisfeature)->mothersurf;
    if ((surf1 != Nil) && (surf1->fvelist->numelems > 1))
      return (surf1);
    return (Nil);
  default:
    return (Nil);
  }
}

  void
draw_picked_vertex_highlight(vfeptr thisvfe)
{
  objptr thisobj;
  ;
  thisobj = thisvfe->startle->motherbound->motherfve->mothershell->motherobj;

  if (!(thisobj->featureflags & Visibleflag))
    return;

  glLineWidth(3.0F);

  glColor4f((((GLfloat) red[0]) / 255.0F),
	    (((GLfloat) red[1]) / 255.0F),
	    (((GLfloat) red[2]) / 255.0F),
	    1.0F);

  draw_square(thisvfe->pos,12.0,FALSE);

  glLineWidth(1.0F);
}

  void
draw_picked_edge_highlight(evfptr thisevf)
{
  objptr thisobj;
  GLboolean backfeature_picked;
  vertype fudgenorm;
  ;
  thisobj = thisevf->le1->motherbound->motherfve->mothershell->motherobj;

  if (!(thisobj->featureflags & Visibleflag))
    return;

  glLineWidth(3.0F);

  if (backfeature_picked =
      (backface_fve(thisevf->le1->motherbound->motherfve) &&
       backface_fve(thisevf->le2->motherbound->motherfve)) )
  {
    glDepthFunc(GL_ALWAYS);
    zdrawstate = GL_ALWAYS;
    
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1,Largedotpattern);
  }
  else
  {
    glDepthFunc(GL_LEQUAL);
    zdrawstate = GL_LEQUAL;
  }

  draw_edge_outline(thisevf,red);

  if (backfeature_picked)
  {
    glDepthFunc(GL_LEQUAL);
    zdrawstate = GL_LEQUAL;
  }

  glLineWidth(1.0F);

  glDisable(GL_LINE_STIPPLE);
}

  void
draw_picked_face_highlight(fveptr thisfve)
{
  objptr thisobj;
  GLboolean backfeature_picked;
  ;
  thisobj = thisfve->mothershell->motherobj;

  if (!(thisobj->featureflags & Visibleflag))
    return;

  if (backfeature_picked = backface_fve(thisfve))
  {
    glLineWidth(3.0F);

    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1,Largedotpattern);

    glDepthFunc(GL_ALWAYS);
    zdrawstate = GL_ALWAYS;
  }
  else
  {
    glLineWidth(5.0F);
    glDepthFunc(GL_LEQUAL);
    zdrawstate = GL_LEQUAL;
  }

  draw_face_outline(thisfve,red);
/*
  draw_wireframe_face(thisfve,red);
*/

  if (backfeature_picked)
  {
    glDepthFunc(GL_LEQUAL);
    zdrawstate = GL_LEQUAL;
  }
  glLineWidth(1.0F);

  glDisable(GL_LINE_STIPPLE);
}

  void
draw_picked_surface_highlight(surfptr thisurf)
{
  objptr thisobj;
  ;
  glLineWidth(3.0F);

  thisobj = ((fveptr) thisurf->fvelist->first.ep->thiselem)->
                                                         mothershell->motherobj;

  if (!(thisobj->featureflags & Visibleflag))
    return;

  draw_surf_edges(thisurf,red);

  glLineWidth(1.0F);
}

  void
draw_picked_object_highlight_engine(objptr thisobj)
{
  glLineWidth(3.0F);

  if ((thisobj->featureflags & Visibleflag) &&
      !(has_property((featureptr) thisobj, pickedinvisible_prop)))
    draw_obj_contour(thisobj,Pickedcontouroffset,pickedcontourcolor);

  glLineWidth(1.0F);
}  

  void
draw_picked_object_highlight(objptr thisobj)
{
  objptr grouproot;
  ;
  glLineWidth(3.0F);

  grouproot = get_unlinked_ancestor(thisobj);
  apply_to_descendants(grouproot,draw_picked_object_highlight_engine,Nil);    

  glLineWidth(1.0F);
}

  static void
draw_picked_highlights(worldptr world)
{
  listptr pickedproplist;
  elemptr pickedfeature;
  proptr thisprop;
  objptr motherpicked;
  ;
  pickedproplist = world->world_proplists[picked_prop];
  thisprop = pickedproplist->first.prop;
  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);
  while (thisprop)
  {
    if (no_containing_feature_picked(thisprop->owner))
    {
      if (thisprop->owner->type_id != Obj_type)
      {
	if (surface_interior_feature(thisprop->owner))
	  pickedfeature = (featureptr)
	    ((fveptr) find_largergrain(thisprop->owner,Fve_type))->mothersurf;
	else
	  pickedfeature = thisprop->owner;
      }
      else
	pickedfeature = thisprop->owner;
      motherpicked = (objptr) find_largergrain(pickedfeature,Obj_type);
      if (!has_property((featureptr) motherpicked,pickedinvisible_prop))
      {
	switch (pickedfeature->type_id)
	{
	case Vfe_type:
	  draw_picked_vertex_highlight((vfeptr) pickedfeature);
	  break;
	case Evf_type:
	  draw_picked_edge_highlight((evfptr) pickedfeature);
	  break;
	case Fve_type:
	  draw_picked_face_highlight((fveptr) pickedfeature);
	  break;
	case Surf_type:
	  draw_picked_surface_highlight((surfptr) pickedfeature);
	  break;
	case Shell_type:
	  break;
	case Obj_type:
	  draw_picked_object_highlight((objptr) pickedfeature);
	  break;
	}
      }
    }
    thisprop = thisprop->next;
  }
}

  static void
draw_selectable_vertex_highlights(vfeptr thisvfe, vertype nearestpos)
{
#if 0
  if (has_property((featureptr) thisvfe,picked_prop))
    return;			/* let picked draw rtns take care of it... */
#endif
  glColor4f((((GLfloat) white[0]) / 255.0F),
	    (((GLfloat) white[1]) / 255.0F),
	    (((GLfloat) white[2]) / 255.0F),
	    1.0F);
  draw_square(thisvfe->pos,8.0,TRUE);
}

  static void
draw_selectable_edge_highlights(evfptr thisevf, vertype nearestpos)
{
#if 0
  if (has_property((featureptr) thisevf,picked_prop))
    return;			/* let picked draw rtns take care of it... */
#endif

  glLineWidth(4.0F);

  if (backface_fve(thisevf->le1->motherbound->motherfve) &&
      backface_fve(thisevf->le2->motherbound->motherfve))
  {
    glDepthFunc(GL_ALWAYS);
    zdrawstate = GL_ALWAYS;


    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1,Largedotpattern);

/*
    draw_shadowed_edge(thisevf,white);
*/
    draw_edge(thisevf,white);

    glDisable(GL_LINE_STIPPLE);

    glDepthFunc(GL_LEQUAL);
    zdrawstate = GL_LEQUAL;
  }
  else
    draw_edge(thisevf,white);
/*
    draw_shadowed_edge(thisevf,white);
*/

  glLineWidth(1.0F);
}

  static void
draw_selectable_face_highlights(fveptr thisfve, vertype nearestpos)
{
  colortype shellscolor;
  ;
#if 0
  /* draw all visible edges in shells color unless this face is hidden */
  if (has_property((featureptr) thisfve,picked_prop))
    return;			/* let picked draw rtns take care of it... */
#endif
  get_feature_color((featureptr) thisfve, shellscolor);
  if (backface_fve(thisfve))
  {
    glDepthFunc(GL_ALWAYS);
    zdrawstate = GL_ALWAYS;

    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1,Largedotpattern);

    draw_shadowed_wireframe_face(thisfve,white);

    glDisable(GL_LINE_STIPPLE);

    glDepthFunc(GL_LEQUAL);
    zdrawstate = GL_LEQUAL;
  }
  else
    draw_shadowed_wireframe_face(thisfve,white);
    
/*
  else
    draw_wireframe_face(thisfve,shellscolor);
*/
}

  void
draw_selectable_obj_highlight_engine(objptr thisobj)
{
  if ((thisobj->featureflags & Visibleflag) &&
      !(has_property((featureptr) thisobj, selectinvisible_prop)))
    draw_obj_contour(thisobj,3.0,white);
}

  static void
draw_selectable_obj_highlights(objptr thisobj)
{
  objptr grouproot;
  ;
  glLineWidth(4.0F);

  grouproot = get_unlinked_ancestor(thisobj);
  apply_to_descendants(grouproot,draw_selectable_obj_highlight_engine,Nil);

  glLineWidth(1.0F);
}

  static void
draw_selectable_shell_highlights(shellptr thishell, vertype nearestpos)
{
  colortype shellscolor;
  ;
  /* draw all visible edges in shells color */
  get_feature_color((featureptr) thishell,shellscolor);
  draw_shadowed_hidden_line_shell(thishell,shellscolor);
}

#define Showboundingboxes
#undef Showboundingboxes

  static void
draw_selectable_highlights(objptr thisobj, stateptr state)
{
  selectableptr selectable_feature;
  surfptr selectablesurf;
  objptr motherselectable;
  ;
  glDisable(GL_LINE_STIPPLE);

  glLineWidth(2.0F);
  selectable_feature = &(thisobj->selectable_feature);
  if (selectable_feature->selectkind != Nothingselect)
  {
    motherselectable = (objptr) find_largergrain(selectable_feature->
						 thefeature, Obj_type);
    if (has_property((featureptr) motherselectable, selectinvisible_prop))
    {
      glDepthFunc(GL_LEQUAL);
      zdrawstate = GL_LEQUAL;
      return;
    }
    if ((selectablesurf = find_mothersurf(selectable_feature->
					  thefeature)) != Nil)
    {
      draw_surf_edges(selectablesurf,white);
      return;
    }
    switch (selectable_feature->selectkind)
    {
    case Vertselect:
      draw_selectable_vertex_highlights((vfeptr) selectable_feature->thefeature,
					selectable_feature->nearestpos);
      break;
    case Edgeselect:
      draw_selectable_edge_highlights((evfptr) selectable_feature->thefeature,
				      selectable_feature->nearestpos);
      break;
    case Faceselect:
    case Inplanefaceselect:
      draw_selectable_face_highlights((fveptr) selectable_feature->thefeature,
				      selectable_feature->nearestpos);
      break;
    case Shellselect:
      break;
    case Objselect:
      draw_selectable_obj_highlights(thisobj);
#ifdef Showboundingboxes
      setcolor(red);
      draw_bounding_box(&(thisobj->objbbox));
      setcolor(white);
      draw_bounding_box(&(thisobj->groupbbox));
#endif      
      break;
    }
  }
  glDepthFunc(GL_LEQUAL);
  zdrawstate = GL_LEQUAL;
}

  void
draw_normal_objects_select_box(objptr selectobj)
{
  draw_shadowed_pick_box(selectobj->selectable_feature.nearestpos,yellow,FALSE);
}

  static void
draw_inplane_selectable_highlights(objptr thisobj, stateptr state)
{
  selectableptr selectable_feature;
  objptr motherselectable;
  ;
  glDepthFunc(GL_LEQUAL);
  zdrawstate = GL_LEQUAL;

  glDisable(GL_LINE_STIPPLE);

  glLineWidth(2.0F);
  selectable_feature = &(thisobj->selectable_feature);
  {
    switch (selectable_feature->selectkind)
    {
    case Inplanefaceselect:
      motherselectable = (objptr) find_largergrain(selectable_feature->
						   thefeature, Obj_type);
      if (!has_property((featureptr) motherselectable, selectinvisible_prop))
	draw_selectable_face_highlights((fveptr) selectable_feature->thefeature,
					selectable_feature->nearestpos);
      break;
    }
  }
  glDepthFunc(GL_LEQUAL);
  zdrawstate = GL_LEQUAL;
}

  void
normal_draw_technique(objptr thisobj, stateptr state)
{
  GLuint decalobj;
  objptr toolowner;
  colortype reducedcolor;
  GLboolean arewelit;
  ;
  /* hackerei!!! */
  toolowner = get_unlinked_ancestor(thisobj);
  if (has_property((featureptr) toolowner,tool_prop)) /* hack needs speedup  */
  {
    glPushMatrix();
	/* OGLXXX setlinestyle: Check list numbering. */
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1,Largedotpattern);

    glLineWidth(1.0F);
    glDepthFunc(GL_ALWAYS);
/*    scalecolor(thisobj->thecolor,0.35,reducedcolor);*/
/*    draw_obj_contour(thisobj,-1.0,reducedcolor);*/
    draw_obj_contour(thisobj,-1.0,lightgrey);
    glDepthFunc(GL_LEQUAL);
    glPopMatrix();
  }

  if (use_zbuffer)
  {
    glDepthFunc(GL_LEQUAL);
    zdrawstate = GL_LEQUAL;

    glPolygonMode(GL_FRONT, GL_FILL);
  arewelit = glIsEnabled(GL_LIGHTING);
    draw_flatshaded_obj(thisobj,state);
    //draw_software_shaded_obj(thisobj);
  }
  else
  {
    glLineWidth(1.0F);
    if (use_hidden_line_technique > 0)
      draw_hidden_line_obj(thisobj,black);
    else
      draw_wireframe_obj(thisobj,black);

    /*   draw_obj_shadow(thisobj) was here... */
  }
    
  if (has_property((featureptr) thisobj, hasdecal_prop))
  {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glMultMatrixf((GLfloat *) thisobj->xform);
    decalobj = (GLuint)
      (get_property_val((featureptr) thisobj,hasdecal_prop)).l;
    glCallList(decalobj);
    glPopMatrix();
    glEnable(GL_LIGHTING);
  }

#if 0
  if (thisobj->obj_name != Nil)
  {
    glPushMatrix();
    glMultMatrixf(thisobj->xform);
    glLineWidth(2.0F);
    setcolor(darkgrey);
    objstr(thisobj->obj_name,origin,1.0,1.0);
    glPopMatrix();
  }
#endif  
}

  void
draw_world_obj(objptr thisobj, GLboolean draw_everything,stateptr state)
{
  if (thisobj->redrawcode & Neverdraw)
    return;			/* some objects get drawn separately or never, */
				/* e.g. children of cursor (tools) */
  
  if ((draw_everything) || (thisobj->redrawcode & Redrawbody) ||
      has_property((featureptr) thisobj,alwaysdraw_prop))
  {
    normal_zbuffer();
    (*(thisobj->drawtechnique))(thisobj, state);
    if (has_property((featureptr) thisobj, haslabel_prop))
      draw_obj_label(thisobj);
    shifted_zbuffer(3);
    if (drawflags & Drawsections)
    {
      draw_selectable_highlights(thisobj,state);
      /* draw any inplane select highlights after section so they overdraw it */
      draw_inplane_selectable_highlights(thisobj,state);
    }
  }

  if (thisobj->drawtechnique == normal_draw_technique)
  {
    if ((draw_everything) || (thisobj->redrawcode & Redrawshadow) ||
	has_property((featureptr) thisobj,alwaysdraw_prop))
    {
      normal_zbuffer();
      if (drawflags & Drawshadows)
      {
	draw_obj_shadow(thisobj); /* currently no technique stored */
	thisobj->redrawcode &= ~Redrawshadow;
      }
    }
  }
}

  void
draw_world_objects(worldptr world, GLboolean draw_everything,stateptr state)
{
  objptr thisobj,selectobj;
  void (*selectroutine)() = Nil;
  featureptr thisfeature;
  proptr firstprop,thisprop;
  ;
  /* draw picked highlightsq */
  /* was:  glDepthFunc(GL_ALWAYS);*/
  glDepthFunc(GL_LEQUAL);
  zdrawstate = GL_LEQUAL;
  shifted_zbuffer(1);
  draw_picked_highlights(world);
  /* draw section lines of all visible objects */

  firstprop = thisprop =
    world->world_proplists[(int) visible_prop]->first.prop;
  while (thisprop != Nil)
  {
    thisfeature = thisprop->owner;
    if (thisfeature->type_id == Obj_type)
    {
      thisobj = (objptr) thisfeature;
      if (thisobj->selectable_feature.selectkind != Nothingselect)
      {
	selectroutine = thisobj->selectable_feature.selectboxtechnique;
	selectobj = thisobj;

#ifdef IRIS
	if ((*selectroutine) == 0x41f0e7) {
	  fprintf(dbgdump, "going into crashing routine 1\n");
	  fflush(dbgdump);
	}
	if (selectroutine == 0x41f0e7) {
	  fprintf(dbgdump, "going into crashing routine 2\n");
	  fflush(dbgdump);
	}
#endif

      }
      draw_world_obj(thisobj,draw_everything,state);
    }
    thisprop = thisprop->next;
  }

  /* draw section lines of all visible objects */
  if (drawflags & Drawsections)
  {
    glDisable(GL_LIGHTING);
    shifted_zbuffer(1);
    thisprop = firstprop;
    while (thisprop != Nil)
    {
      thisfeature = thisprop->owner;
      if (thisfeature->type_id == Obj_type)
      {
	thisobj = (objptr) thisfeature;
	if ((draw_everything) || thisobj->redrawcode & Redrawbody)
	{			/* show the section line */
	  draw_obj_intersection((objptr) thisfeature);
	  thisobj->redrawcode &= ~Redrawbody;
	}
      }
      thisprop = thisprop->next;
    }
    glEnable(GL_LIGHTING);
  }
  normal_zbuffer();

  draw_dimensioned_features(world);
  
  if (selectroutine != Nil)
  {
    if (global_selectroutine == Nil)
    {
      global_selectobj = selectobj;
      global_selectroutine = selectroutine;
    }
  }
}

  void
draw_slide_obj(objptr thisobj, GLboolean draw_everything,stateptr state)
{
  if (thisobj->redrawcode & Neverdraw)
    return;			/* some objects get drawn separately or never, */
				/* e.g. children of cursor (tools) */
  
  if ((draw_everything) || (thisobj->redrawcode & Redrawbody) ||
      has_property((featureptr) thisobj,alwaysdraw_prop))
  {
    (*(thisobj->drawtechnique))(thisobj, state);
    if (has_property((featureptr) thisobj, haslabel_prop))
      draw_obj_label(thisobj);
  }

  if (thisobj->drawtechnique == normal_draw_technique)
  {
    if ((draw_everything) || (thisobj->redrawcode & Redrawshadow) ||
	has_property((featureptr) thisobj,alwaysdraw_prop))
    {
      draw_obj_shadow(thisobj); /* currently no technique stored */
      thisobj->redrawcode &= ~Redrawshadow;
    }
  }
}

  void
draw_slide_objects(worldptr world, GLboolean draw_everything,stateptr state)
{
  objptr thisobj;
  void (*selectroutine)() = Nil;
  featureptr thisfeature;
  proptr firstprop,thisprop;
  ;
  /* draw all visible objects */

  normal_zbuffer();
  firstprop = thisprop = world->world_proplists[(int) visible_prop]->first.prop;
  while (thisprop != Nil)
  {
    thisfeature = thisprop->owner;
    if (thisfeature->type_id == Obj_type)
    {
      thisobj = (objptr) thisfeature;
      draw_slide_obj(thisobj,draw_everything,state);
    }
    thisprop = thisprop->next;
  }
}

#ifdef IRIS

void
draw_on_slide(void)
{
  fmfonthandle symbol,symbol12,times,times36,times12;
  fmfonthandle fsized,specfsized;
  static char displaystr[] = "Camera's View";/*"3Form Slide Show";*/
  ;
	/* OGLXXX
	 * winset not supported -- See Window Manager
	 * winset(slidewindow)
	 */
  /*DELETED*/;

	/* OGLXXX change glClearDepth parameter to be in [0, 1] */
  glClearDepth(0x7FFFFF);glClearColor(((float)((packcolor(lightblue))&0xff))/255., (float)((packcolor(lightblue))>>8&0xff)/255., (float)((packcolor(lightblue))>>16&0xff)/255., (float)((packcolor(lightblue))>>24&0xff)/255. );glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

  draw_slide_objects(working_world,TRUE,state);

  glDepthFunc(GL_ALWAYS);		/* set as the default */
  
  times = fmfindfont("Times-Italic");
  symbol = fmfindfont("Symbol");
  times36 = fmscalefont(times,36.0);
  symbol12 = fmscalefont(symbol,12.0);
  times12 = fmscalefont(times,12.0);

  fmsetfont(times36);
  setcolor(black);
  glRasterPos3f(-350.0, 200.0, 499.0);
  fmprstr(displaystr);
  setcolor(white);
  glRasterPos3f(-349.0, 199.0, 499.0);
  fmprstr(displaystr);

	/* OGLXXX swapbuffers: replace display and window */
  glXSwapBuffers(*display, window);

  glDepthFunc(GL_LEQUAL);		/* set as the default */

	/* OGLXXX
	 * winset not supported -- See Window Manager
	 * winset(mainwindow)
	 */
  /*DELETED*/;
}

#endif

void
pushpop_slide(GLboolean showslide)
{
  ;
  if (showslide)
  {
	/* OGLXXX
	 * winset not supported -- See Window Manager
	 * winset(slidewindow)
	 */
    /*DELETED*/;
	/* OGLXXX
	 * winpop not supported -- See Window Manager
	 * winpop()
	 */
    /*DELETED*/;
	/* OGLXXX
	 * winposition not supported -- See Window Manager
	 * winposition(640,1280,60,620)
	 */
    /*DELETED*/;
	/* OGLXXX
	 * reshapeviewport not supported -- See Window Manager
	 * reshapeviewport()
	 */
    /*DELETED*/;
	/* OGLXXX
	 * winset not supported -- See Window Manager
	 * winset(mainwindow)
	 */
    /*DELETED*/;
  }
  else
  {
	/* OGLXXX
	 * winset not supported -- See Window Manager
	 * winset(slidewindow)
	 */
    /*DELETED*/;
	/* OGLXXX
	 * winposition not supported -- See Window Manager
	 * winposition(-640,-1280,-60,-620)
	 */
    /*DELETED*/;
	/* OGLXXX
	 * reshapeviewport not supported -- See Window Manager
	 * reshapeviewport()
	 */
    /*DELETED*/;
	/* OGLXXX
	 * winpush not supported -- See Window Manager
	 * winpush()
	 */
    /*DELETED*/;
	/* OGLXXX
	 * winset not supported -- See Window Manager
	 * winset(mainwindow)
	 */
    /*DELETED*/;
  }
  doslideshow = showslide;
}

  void
set_camera_view(vertype direction, vertype eyept)
{
  vertype viewdir;
  ;
  /* OGLXXX
   * winset not supported -- See Window Manager
   * winset(slidewindow)
   */
  /*DELETED*/;

  glLoadMatrixf((GLfloat *) imatrix);
  addvec3d(eyept,direction,viewdir);
  gluLookAt(eyept[vx],eyept[vy],eyept[vz],viewdir[vx],viewdir[vy],viewdir[vz],
	    0.0F,1.0F,0.0F);
  /* OGLXXX
   * winset not supported -- See Window Manager
   * winset(mainwindow)
   */
  /*DELETED*/
}

  void
draw_underlying_objects(stateptr state, stateptr oldstate)
{
  static short planecolor[4] = {255,255,255,128};
  ;
  glDepthFunc(GL_LEQUAL);
  zdrawstate = GL_LEQUAL;

  if (drawflags & Drawroom)
    /*    glCallList(Room); */
    draw_room();

  if (drawflags & Drawplane)
  {
    draw_cutplane();
#if 0
    if (has_property((featureptr) cutplane_obj,picked_prop))
      draw_cutplane_pickedrect();
#endif
  }
}

  void
draw_gl_objects(stateptr state)
{
  stateptr grafx_state;
  GLboolean lighting_was_on;
  ;
  lighting_was_on = glIsEnabled(GL_LIGHTING);
  if (lighting_was_on)
    glDisable(GL_LIGHTING);

  draw_cursor(cursor_obj);

  if (global_selectroutine != Nil)
  {
    glDepthFunc(GL_ALWAYS);
    zdrawstate = GL_ALWAYS;

    fprintf(dbgdump, "trying to run routine at %p\n", (void *) *global_selectroutine);
    fflush(dbgdump);

    (*global_selectroutine)(global_selectobj);

    fprintf(dbgdump, "survived select routine run!\n");
    fflush(dbgdump);
  }
   grafx_state = state;
  if (lighting_was_on)
    glEnable(GL_LIGHTING);
}

  void
draw_worlds(GLboolean draw_everything)
{
  /* this is a kinda hack to make sure the right selectbox gets drawn */
  /* every time */
  void (*primary_globalselectroutine)() = Nil;
  ;

  /* The global_selectroutine prevents the code at the bottom of this
     routine from drawing select boxes for objects in several worlds. It
     will only permit a select box to be drawn for the first selectable
     object found in all worlds. */

  global_selectroutine = Nil;


  draw_world_objects(primary_world,draw_everything,state);

/* We are not supporting this yet... soon! */
#ifdef IRIS
  if ((doslideshow) && (using_slideshow == TRUE))
    draw_on_slide();
#endif /* IRIS */

  /* draw this second or rotate tool don't show through */
  primary_globalselectroutine = global_selectroutine;
  global_selectroutine = Nil;
#ifdef IRIS
  zfunction(ZF_LEQUAL);		/* set as the default */
  zdrawstate = ZF_LEQUAL;
#endif /* IRIS */

  draw_world_objects(environment_world,draw_everything,state);
  if (global_selectroutine == Nil)
    global_selectroutine = primary_globalselectroutine;

}

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* Stuff from OGLSDK's cube.c begins here, adapted for cutplane usage.    */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/


// FIX ME!!! LJE
#if 0
void
redraw_grafx(HDC hDC)
{
    /* clear color and depth buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Cutplane redrawing happens here... */

    SwapBuffers(hDC);
}
#endif

void
grafx_resize(int main_winWidth, int main_winHeight)
{
    /* set viewport to cover the window */
    glViewport(0, 0, main_winWidth, main_winHeight);
}

  void
clear_view(void)
{
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
}
