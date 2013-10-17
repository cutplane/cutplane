
/* FILE: init.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*             System Startup Routines for the Cutplane Editor            */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define INITMODULE

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <platform_gl.h>
#include <platform_glut.h>

#ifdef IRIS
#include <fmclient.h>
#endif

#include <cutplane.h>

#include <init.h>
#include <globals.h>
#include <grafx.h>	/* for color definitions */
#include <shade.h>	/* for grey-ramp range definitions , Pi*/

/* things we are testing today: */
#define Createtools
/*
#define Createfullworkspace
#define Copslikedonuts
#define Timetest
#define Testboolbeam
#define Testboolholes
#define Testboolcube
#define Testsegments
#define Aliasvs3formdemo
#define Twolbeams
#define Stocksdemo
#define Onelbeam
#define Shipcube
*/

  void
def_lighting_props()
{
  /* OGLXXX include ALPHA parameter with diffuse */
  /* OGLXXX
   * Ambient:
   * If this is a light model lmdef,
   * then use glLightModelf and GL_LIGHT_MODEL_AMBIENT.
   * Include ALPHA parameter with ambient
   */

  static float materialprops[] = {GL_DIFFUSE,1.0,1.0,1.0,
				  GL_AMBIENT,1.0,1.0,1.0,};

  /* OGLXXX
   * light color: need to add GL_AMBIENT and GL_SPECULAR components.
   * ALPHA needs to be included in parameters.
   * Ambient:
   * If this is a light model lmdef, then use glLightModelf
   * and GL_LIGHT_MODEL_AMBIENT.
   * Include ALPHA parameter with ambient
   */

  static float lightprops[] = {GL_AMBIENT, 0.31,0.31,0.31,
			       GL_DIFFUSE,0.69,0.69,0.69,
			       GL_POSITION,0.0,1.0,0.0,0.0,};

  /* OGLXXX
   * light color: need to add GL_AMBIENT and GL_SPECULAR components.
   * ALPHA needs to be included in parameters.
   */

  static float mobilelightprops[] = {GL_AMBIENT,0.0,0.0,0.0,
				     GL_DIFFUSE,0.3,0.3,0.3, /* light color */
				     GL_POSITION,0.5,0.0,1.0,0.0, /* dir vec */
  };	/* end of def */

#ifdef IRIS
  static float lmodelprops[] = {};
#endif
  int nmaterialprops = sizeof(materialprops)/sizeof(float);
  int nlightprops = sizeof(lightprops)/sizeof(float);
  int nmobilelightprops = sizeof(mobilelightprops)/sizeof(float);
#ifdef IRIS
  int nlmodelprops = sizeof(lmodelprops)/sizeof(float);
#endif
  int i;
  ;
  /* OGLXXX
   * lmdef other possibilities include:
   * 	glLightf(light, pname, *params);
   * 	glLightModelf(pname, param);
   * Check list numbering.
   * Translate params as needed.
   */
  glNewList(1, GL_COMPILE);
  glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *materialprops);
  glEndList();

  glNewList(1, GL_COMPILE);
  glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *lightprops);
  glEndList();
  /* setup defs for all possible mobile lights */
  for (i=2; i < 9; i++) {
    glNewList(i, GL_COMPILE);
    glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *mobilelightprops);
    glEndList();
  }
#ifdef IRIS
  glNewList(1, GL_COMPILE);
  glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *lmodelprops);
  glEndList();;
#endif
}

void
setMaterial(void)
{
    GLfloat matAmb[4] =   { 1.00F, 1.00F, 1.00F, 1.00F };
    GLfloat matDiff[4] =  { 0.00F, 0.00F, 0.00F, 1.00F };
    GLfloat matSpec[4] =  { 0.00F, 0.00F, 0.00F, 0.00F };
    GLfloat matShine[1] = { 0.00F };

    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
}

  void
turn_on_lighting()
{
	/* OGLXXX
	 * lmbind: check object numbering.
	 * Use GL_FRONT in call to glMaterialf.
	 * Use GL_FRONT in call to glMaterialf.
	 */

  /* Overhead light */
  /* OGLXXX lmbind: check object numbering. */
  glCallList(1);
  glEnable(GL_FRONT);
  glCallList(1);
  glEnable(GL_LIGHT0);
  glCallList(2);
  glEnable(GL_LIGHT1);
}
 
GLboolean light_bound[8] = {TRUE,FALSE,FALSE,FALSE,
			    FALSE,FALSE,FALSE,FALSE};

  void
setup_light_source(int lightnumber,vertype lightpos,colortype lightcolor)
{
#ifdef IRIS
  /* OGLXXX
   * Ambient:
   * 	If this is a light model lmdef, then use glLightModelf and
   *    GL_LIGHT_MODEL_AMBIENT.
   * Include ALPHA parameter with ambient
   */
  /* OGLXXX
   * light color: need to add GL_AMBIENT and GL_SPECULAR components.
   * ALPHA needs to be included in parameters.
   */
  static float mobilelightprops[] =
  {GL_AMBIENT,0.0,0.0,0.0,
   GL_DIFFUSE,0.3,0.3,0.3, /* light color */
   GL_POSITION,0.5,0.0,1.0,1.0}; /* dir vec */
  int nmobilelightprops = sizeof(mobilelightprops)/sizeof(float);
  ;
  mobilelightprops[5] = ((float) lightcolor[0]) / 255.0;
  mobilelightprops[6] = ((float) lightcolor[1]) / 255.0;
  mobilelightprops[7] = ((float) lightcolor[2]) / 255.0;
  mobilelightprops[9] = lightpos[0];
  mobilelightprops[10] = lightpos[1];
  mobilelightprops[11] = lightpos[2];

  /* OGLXXX
   * lmdef other possibilities include:
   * 	glLightf(light, pname, *params);
   * 	glLightModelf(pname, param);
   * Check list numbering.
   * Translate params as needed.
   */
  glNewList(lightnumber, GL_COMPILE);
  glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *mobilelightprops);
  glEndList();;
#else
  /* We're gonna do it all OpenGL way for now */
  GLfloat diffuse_light_vals[4];
  ;
  diffuse_light_vals[0] = ((GLfloat) lightcolor[0]) / 255.0F;
  diffuse_light_vals[1] = ((GLfloat) lightcolor[1]) / 255.0F;
  diffuse_light_vals[2] = ((GLfloat) lightcolor[2]) / 255.0F;
  diffuse_light_vals[3] = 1.0;
  glLightfv(GL_LIGHT0+lightnumber-1, GL_DIFFUSE,diffuse_light_vals);
  glLightfv(GL_LIGHT0+lightnumber-1, GL_SPECULAR,diffuse_light_vals);
  //glLightfv(GL_LIGHT0+lightnumber-1, GL_AMBIENT,diffuse_light_vals);
  fprintf(dbgdump, "setting light %d diffuse to [%f, %f, %f, %f]\n",
	  GL_LIGHT0+lightnumber-1,
	  diffuse_light_vals[0],diffuse_light_vals[1],diffuse_light_vals[2],
	  diffuse_light_vals[3]);
  copypos3d(lightpos, light1Pos);
  glPushMatrix();
  glLoadMatrixf((GLfloat *)(vanillamatrix));
  glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
  glPopMatrix();
  fprintf(dbgdump, "moving light %d to [%f, %f, %f, %f]\n",
	  GL_LIGHT0+lightnumber-1,
	  lightpos[0],lightpos[1],lightpos[2],lightpos[3]);
#endif

}

/* turn a light source on or off. Checks if it is already in the requested */
/* state before turning it on or off. */

  void
switch_light_source(int lightnumber,GLboolean turn_it_on)
{
  if (turn_it_on)
  {
    if (!light_bound[lightnumber - 1])
    {
      light_bound[lightnumber-1] = TRUE;
      /* OGLXXX lmbind: check object numbering. */
      if(lightnumber) {
	glEnable(GL_LIGHT0+lightnumber-1);
      } else
	glDisable(GL_LIGHT0+lightnumber-1);
    }
  }
  else if (light_bound[lightnumber - 1])
  {
    light_bound[lightnumber-1] = FALSE;
    glDisable(GL_LIGHT0+lightnumber-1);
  }
}


  void
read_window_resource_files()
{
  FILE *windowfile;
  int window_width,window_height;
  ;
  /* init values in globals.c */
  windowfile = fopen("./rsc/windowsizes.rsc","r");
  fscanf(windowfile,"Screensize:%d,%d\n",&screensizex,&screensizey);
  fscanf(windowfile,"Main Window Top Left Corner:%d,%d\n",
	 &mainwindowleft,&mainwindowtop);
  fscanf(windowfile,"Main Window Width,Height:%d,%d\n",
	 &window_width,&window_height);
  mainwindowright = mainwindowleft + window_width;
  mainwindowbottom = mainwindowtop - window_height;
  fscanf(windowfile,"Help Window Top Left Corner:%d,%d\n",
	 &helpwindowleft,&helpwindowtop);
  fscanf(windowfile,"Help Window Width,Height:%d,%d\n",
	 &window_width,&window_height);
  helpwindowright = helpwindowleft + window_width;
  helpwindowbottom = helpwindowtop - window_height;
  fscanf(windowfile,"Information Window Top Left Corner:%d,%d\n",
	 &infowindowleft,&infowindowtop);
  fscanf(windowfile,"Information Window Width,Height:%d,%d\n",
	 &window_width,&window_height);
  infowindowright = infowindowleft + window_width;
  infowindowbottom = infowindowtop - window_height;
  fclose(windowfile);
}


void
init_main_window()
{

  /* OGLXXX change glClearDepth parameter to be in [0, 1] */
  glClearDepth(0x7FFFFF);
  /*
  glClearColor((GLfloat) ((GLfloat) backgroundcolor[0]) / 255.0F,
	       (GLfloat) ((GLfloat) backgroundcolor[1]) / 255.0F,
	       (GLfloat) ((GLfloat) backgroundcolor[2]) / 255.0F,
	       0.0F);
	       
  glClearColor(((float)((packcolor(backgroundcolor))&0xff))/255.0,
	       (float)((packcolor(backgroundcolor))>>8&0xff)/255.0,
	       (float)((packcolor(backgroundcolor))>>16&0xff)/255.0,
	       (float)((packcolor(backgroundcolor))>>24&0xff)/255.0 );
  */

  normal_zbuffer();

  glMatrixMode(GL_MODELVIEW);

  /* begin lighting model setup */
  glShadeModel(GL_FLAT);

  /*  def_lighting_props(); */
  /*  turn_on_lighting();*/

// Turn off the 2d OS cursor
  glutSetCursor(GLUT_CURSOR_NONE);

}


  void
init_state()
{
  state->delroomrot = oldstate->delroomrot = Slowrot;
  state->roomrotx = oldstate->roomrotx = 0;
  state->roomroty = oldstate->roomroty = 0;
  state->controlbuttonsdown = Nocontrolbuttonsbit;
}


  void
init_global_lists()
{
  world_list = create_list();
  init_CPL_global_lists();

  /* this a scratch pad world for temp shells and other grunginess */
  scratch_world = (worldptr) append_new_blank_elem(world_list,World_type);

#ifdef IRIS
 /* This is special old-GL magic to speed up rendering stuff */
  init_erase_records();
#endif /* IRIS */

}  

  void
init_environment()
{
  vertype pos;
  ;
  environment_world = (worldptr) append_new_blank_elem(world_list,World_type);
  set_world_name(environment_world,"Environment_world");

  cutplane_obj = make_obj(environment_world,origin);
  cutplane_obj->selectechnique = set_cutplane_selectable;
  cutplane_obj->drawtechnique = nodraw_technique;
  cutplane_obj->screenbboxtechnique = find_cutplane_screenbbox;
  set_object_name(cutplane_obj,"Cutplane");
  add_property((featureptr) cutplane_obj, nonmanifold_prop);
  add_property((featureptr) cutplane_obj, noshadow_prop);
  unlog_updates((featureptr) cutplane_obj);

  line_obj = make_obj(environment_world,origin);
  make_edge_vert(First_obj_le(line_obj),First_obj_le(line_obj),origin);
  line_obj->selectechnique = set_always_selectable;
  line_obj->drawtechnique = draw_scribble_technique;
  line_obj->screenbboxtechnique = Nil;
  set_object_name(line_obj,"Line");
  add_property((featureptr) line_obj, nonmanifold_prop);
  add_property((featureptr) line_obj, noshadow_prop);
  del_property((featureptr) line_obj, visible_prop);
  unlog_updates((featureptr) line_obj);

  cursor_obj = make_obj(environment_world,origin);
  cursor_obj->selectechnique = set_always_selectable;
  cursor_obj->drawtechnique = nodraw_technique;
  cursor_obj->screenbboxtechnique = find_normal_cursor_screenbbox;
  set_object_name(cursor_obj,"Crosshair_Cursor");
  set_feature_color((featureptr) cursor_obj,red);
  add_property((featureptr) cursor_obj, nonmanifold_prop);
  add_property((featureptr) cursor_obj, noshadow_prop);
  add_property((featureptr) cursor_obj, background_prop);
  adopt_obj_branch(cutplane_obj,cursor_obj,TRUE);
  unlog_updates((featureptr) cursor_obj);


  create_rotatetool(environment_world);
  del_property_from_descendants((featureptr) rotatetool_obj,visible_prop);


  systemcursor_obj = room_obj = Nil; /* if you use em, you die */

#if 0				/* retired for the time being */
  systemcursor_obj = make_obj(environment_world,origin);
  systemcursor_obj->selectechnique = set_systemcursor_selectable;
  systemcursor_obj->drawtechnique = nodraw_technique;
  set_object_name(systemcursor_obj,"Arrow_Cursor");
  add_property((featureptr) systemcursor_obj, nonmanifold_prop);
  add_property((featureptr) systemcursor_obj, noshadow_prop);
  /* hack delete this line when the appropriate object is created */
  del_property((featureptr) systemcursor_obj, visible_prop);
  unlog_updates((featureptr) systemcursor_obj);

  room_obj = make_obj(environment_world,origin);
  room_obj->selectechnique = set_room_selectable;
  room_obj->drawtechnique = nodraw_technique;
  set_object_name(room_obj,"Working_Room");
  add_property((featureptr) room_obj, nonmanifold_prop);
  add_property((featureptr) room_obj, noshadow_prop);
  /* hack delete me later */
  del_property((featureptr) room_obj, visible_prop);
  unlog_updates((featureptr) room_obj);
#endif

  setpos3d(pos,0.0,0.0,Minplanez);
  room_backwall_obj = make_obj(environment_world,pos);
  room_backwall_obj->selectechnique = set_always_selectable;
  room_backwall_obj->drawtechnique = nodraw_technique;
  set_object_name(room_backwall_obj,"Room_Backwall");
  add_property((featureptr) room_backwall_obj, nonmanifold_prop);
  add_property((featureptr) room_backwall_obj, noshadow_prop);
  del_property((featureptr) room_backwall_obj, visible_prop);
  unlog_updates((featureptr) room_backwall_obj);

  create_scalebbox(environment_world);
/*  create_sizebbox(environment_world);*/
  
  group_obj = make_obj_noshell(environment_world);
  set_object_name(group_obj,"Groupobj");
  del_property((featureptr) group_obj, visible_prop);
  add_property((featureptr) group_obj, nonmanifold_prop);
  add_property((featureptr) group_obj, noshadow_prop);
}

  void
init_other_worlds()
{
  init_environment();

  help_world = (worldptr) append_new_blank_elem(world_list,World_type);
  help_world->world_name = (char *) alloc_elem(Maxworldnamelength);
  strcpy(help_world->world_name,"Help_world");
}


  void
init_primary_world()
{
  primary_world = (worldptr) append_new_blank_elem(world_list,World_type);
  primary_world->world_name = (char *) alloc_elem(Maxworldnamelength);
  strcpy(primary_world->world_name,"Primary_world");

  working_world = primary_world;
}

  void
attach_otherworlds_CPLcode()
{
  read_CPL_privatecode("./cpl/cursor.cpl",(featureptr) cursor_obj);
  attach_rotatetool_CPLcode();
}


  void
init_3form()
{
  objptr objA;	
  ;

  init_state();			

  init_global_lists();
  init_primary_world();
  init_other_worlds();

  init_CPL_language();

  attach_otherworlds_CPLcode();
  init_nearpts_list();
  init_acos_table();

#ifdef IRIS
  init_error_handling();
#endif

  init_controls(state);		

  /* setup all the tools in the environment... see tools.c */
#ifdef Createtools
  setup_tools();
#endif

  objA = create_lbeam(working_world,200,180,55,200),
  set_feature_named_color((featureptr) objA, "Blue");

  log_global_update(Allupdateflags);
  process_updates(environment_world,state,oldstate,quickupdate_prop);
  process_updates(primary_world,state,oldstate,quickupdate_prop);
  process_updates(environment_world,state,oldstate,regularupdate_prop);
  process_updates(primary_world,state,oldstate,regularupdate_prop);
  process_global_updates(world_list,state,oldstate);

#if 0
  while(new_user_input(world_list,state,oldstate))
    ;
#endif
}

// FIX ME!!! LJE
#if 0
void
setupPixelFormat(HDC hDC)
{
    PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR),	/* size */
	1,				/* version */
	PFD_SUPPORT_OPENGL |
	PFD_DRAW_TO_WINDOW |
	PFD_DOUBLEBUFFER,		/* support double-buffering */
	PFD_TYPE_RGBA,			/* color type */
	16,				/* prefered color depth */
	0, 0, 0, 0, 0, 0,		/* color bits (ignored) */
	0,				/* no alpha buffer */
	0,				/* alpha bits (ignored) */
	0,				/* no accumulation buffer */
	0, 0, 0, 0,			/* accum bits (ignored) */
	16,				/* depth buffer */
	0,				/* no stencil buffer */
	0,				/* no auxiliary buffers */
	PFD_MAIN_PLANE,			/* main layer */
	0,				/* reserved */
	0, 0, 0,			/* no layer, visible, damage masks */
    };
    int pixelFormat;

    pixelFormat = ChoosePixelFormat(hDC, &pfd);
    if (pixelFormat == 0) {
	MessageBox(WindowFromDC(hDC), "ChoosePixelFormat failed.", "Error",
		MB_ICONERROR | MB_OK);
	exit(1);
    }

    if (SetPixelFormat(hDC, pixelFormat, &pfd) != TRUE) {
	MessageBox(WindowFromDC(hDC), "SetPixelFormat failed.", "Error",
		MB_ICONERROR | MB_OK);
	exit(1);
    }
}
#endif

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* Stuff from vcull.c begins here, adapted for cutplane usage.            */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

GLfloat objectXform[4][4];
float angle = 10.0F, axis[3] = { 0.0F, 0.0F, 1.0F };
#define NUM_OBJECTS (sizeof(drawObject) / sizeof(drawObject[0]))

int objectIndex;

int objectNumMajor = 24, objectNumMinor = 32;

GLboolean halfObject = GL_FALSE;
GLboolean redrawContinue = GL_TRUE;
GLboolean doubleBuffered = GL_TRUE;
GLboolean depthBuffered = GL_TRUE;
GLboolean drawOutlines = GL_FALSE;
GLboolean textureEnabled = GL_FALSE;
GLboolean textureReplace = GL_FALSE;
GLboolean useVertexCull = GL_TRUE;
GLboolean useFaceCull = GL_TRUE;
GLboolean useVertexArray = GL_TRUE;
GLboolean useVertexLocking = GL_TRUE;
GLboolean useLighting = GL_TRUE;
GLboolean perspectiveProj = GL_TRUE;
GLboolean useFog = GL_FALSE;
enum MoveModes { MoveNone, MoveObject };
enum MoveModes mode = MoveObject;

#define X_OFFSET_STEP 0.025F;
#define Y_OFFSET_STEP 0.025F;
GLfloat xOffset, yOffset;



void
setCheckTexture(void)
{
    int texWidth = 256;
    int texHeight = 256;
    GLubyte *texPixels, *p;
    int texSize;
    int i, j;

    texSize = texWidth*texHeight*4*sizeof(GLubyte);
    if (textureReplace) {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    } else {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    texPixels = (GLubyte *) malloc(texSize);
    if (texPixels == NULL) {
	return;
    }

    p = texPixels;
    for (i=0; i<texHeight; ++i) {
	for (j=0; j<texWidth; ++j) {
	    if ((i ^ j) & 32) {
		p[0] = 0xff; p[1] = 0xff; p[2] = 0xff; p[3] = 0xff;
	    } else {
		p[0] = 0x10; p[1] = 0x10; p[2] = 0x10; p[3] = 0xff;
	    }
	    p += 4;
	}
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		 texWidth, texHeight, 0,
		 GL_RGBA, GL_UNSIGNED_BYTE, texPixels);

    free(texPixels);
}

void
matrixIdentity(GLfloat m[4][4])
{
    m[0][0] = 1.0F; m[0][1] = 0.0F; m[0][2] = 0.0F; m[0][3] = 0.0F;
    m[1][0] = 0.0F; m[1][1] = 1.0F; m[1][2] = 0.0F; m[1][3] = 0.0F;
    m[2][0] = 0.0F; m[2][1] = 0.0F; m[2][2] = 1.0F; m[2][3] = 0.0F;
    m[3][0] = 0.0F; m[3][1] = 0.0F; m[3][2] = 0.0F; m[3][3] = 1.0F;
}

void
init_grafx(void)
{
  GLfloat light0Pos[4] = { 0.0F, 0.0F, -1.0F, 0.0F };
  GLfloat light0diffuse[4] = { 0.5F, 0.5F, 0.5F, 1.0F };
  GLfloat light0spec[4] = { 0.5F, 0.5F, 0.5F, 1.0F };
  GLfloat light0ambient[4] = { 0.0F,0.0F,0.0F, 0.0F };
  GLfloat light1diffuse[4] = { 0.5F, 0.5F, 0.5F, 1.0F };
  GLfloat light1ambient[4] = { 0.0F,0.0F,0.0F, 0.0F };
  GLfloat matSpec[4] = { 1.0F, 1.0F, 1.0F, 1.0F };
  GLfloat matShine[1] = { 20.00F };

  // GLfloat light0Intensity[4] = { 0.31F, 0.31F, 0.31F, 1.0F };
  // GLfloat light0Intensity[4] = { 1.0F,1.0F,1.0F, 1.0F };
  GLfloat light1Intensity[4] = { 0.31F, 0.31F, 0.31F, 1.0F };
  GLfloat fogDensity = 2.35F*0.180F;
  GLfloat fogColor[4] = { 0.4F, 0.4F, 0.5F, 1.0F };
  ;
  read_window_resource_files();

  init_main_window();

  init_projmatrix();

  /*  Set up buffers and clear 'em. */
  glClearDepth(0x7FFFFF);
  glClearColor((((GLfloat) backgroundcolor[0]) / 255.0F),
	       (((GLfloat) backgroundcolor[1]) / 255.0F),
	       (((GLfloat) backgroundcolor[2]) / 255.0F),
	       1.0F);
  /*
  glClearColor(((GLfloat)(backgroundcolor[0])/255.0,
	       (float)((packcolor(backgroundcolor))>>8&0xff)/255.0,
	       (float)((packcolor(backgroundcolor))>>16&0xff)/255.0,
	       (float)((packcolor(backgroundcolor))>>24&0xff)/255.0 );
  */
  /* Clear both buffers. Used to be a call to clear_buffers in grafx.c. */
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
  glutSwapBuffers();
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
  glutSwapBuffers();

  create_gl_list_ids();		/* Added 12/30/01 for opengl list support */

  def_environment_colors();
  object_defs();
#ifdef IRIS
  background_defs();
#endif /* IRIS */

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);

  glLineWidth((GLfloat)(1));

#if 0
  glFogi(GL_FOG_MODE, GL_EXP2);
  glFogf(GL_FOG_DENSITY, fogDensity);
  glFogfv(GL_FOG_COLOR, fogColor);
#endif

  // setMaterial();


  //  glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
  //  glMaterialfv(GL_FRONT, GL_SHININESS, matShine);

  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  //  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0ambient);
  //glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0diffuse);
  //glLightfv(GL_LIGHT0, GL_SPECULAR, light0spec);
  glEnable(GL_LIGHT0);

  glLightfv(GL_LIGHT1, GL_AMBIENT, light1ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1diffuse);
  glDisable(GL_LIGHT1);
  //  glLightfv(GL_LIGHT2, GL_AMBIENT, light1ambient);
  //  glLightfv(GL_LIGHT2, GL_DIFFUSE, light1diffuse);
  //  glDisable(GL_LIGHT2);


  // glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientSettings);
  // glLightfv(GL_LIGHT0, GL_AMBIENT, light0Intensity);
  // glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
  // glLightfv(GL_LIGHT1, GL_AMBIENT, light1Intensity);
  // glEnable(GL_LIGHT1);

  setCheckTexture();
}


/*****************************************************************/
