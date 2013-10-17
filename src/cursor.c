
/* FILE: cursor.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*                 Cursor Display Routines for 3Form                      */
/*                                                                        */
/* Authors: WAK                                   Date: February 26, 1991 */
/*                                                                        */
/* Copyright (C) 1990,1991,1992 William Kessler & Laurence Edwards.       */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define CURSORMODULE

#include <config.h>
#include <stdio.h>
#include <math.h>

#include <platform_gl.h>
#include <platform_glu.h>

#include <cutplane.h>

#include <math3d.h>
#include <grafx.h>
#include <cursor.h>
#include <cpl_cutplane.h>

#include <demo.h>		/* shit shit shit */

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	                       GLOBAL VARIABLES				     */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* hack delete from grafx */
blitsavetype saved_blits[2];

cursorecordtype cursor_records[Maxcursors] = {{draw_hidden_cursor,
					       get_normal_cursor_blit,
					       restore_normal_cursor_blit},
					      {draw_plus_cursor,
					       get_normal_cursor_blit,
					       restore_normal_cursor_blit},
					      {draw_arrow_cursor,
					       get_normal_cursor_blit,
					       restore_normal_cursor_blit},
					      {draw_ibeam_cursor,
					       get_normal_cursor_blit,
					       restore_normal_cursor_blit},
					      {draw_xhair_cursor,
					       get_xhair_cursor_blit,
					       restore_xhair_cursor_blit}};

cursorkind current_cursor = Pluscursor;	/* global index into above array */

vertype cursor_sizes[Maxcursors] = {{Hiddencursorwidth,Hiddencursorheight,
				     Hiddencursordepth},
				  {Pluscursorwidth,Pluscursorheight,
				     Pluscursordepth},
				  {Arrowcursorwidth*2,Arrowcursorheight*1.5,
				     Arrowcursordepth},
				  {Ibeamcursorwidth*2.5,Ibeamcursorheight*2,
				     Ibeamcursordepth},
				  {Xhaircursorwidth,Xhaircursorheight,
				    Xhaircursordepth}};

vertype default_cursor_offset = {0.0,0.0,5.0};
vertype cursor_offset = {0.0,0.0,5.0};
vertype cursor_offset_setting = {0.0,0.0,5.0};
vertype cursor_unoffset_setting = {0.0,0.0,5.0};
GLboolean cursor_offset_this_refresh = FALSE;
GLboolean cursor_unoffset_this_refresh = FALSE;
GLboolean cursor_paused = FALSE;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*       	            CURSOR SETTING ROUTINES     		     */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */



/* Offset the drawn image of the cursor by some set amount to avoid zbuffer */
/* problems...*/

  void
offset_cursor_image(vertype new_cursor_offset)
{
  copypos3d(new_cursor_offset,cursor_offset_setting);
  cursor_offset_this_refresh = TRUE;
}

  void
unoffset_cursor_image(vertype new_cursor_unoffset)
{
  copypos3d(new_cursor_unoffset,cursor_unoffset_setting);
  cursor_unoffset_this_refresh = TRUE;
}

  void
plant_cursor_image(void)
{
  if (cursor_offset_this_refresh)
    copypos3d(cursor_offset_setting,cursor_offset);
  else if (cursor_unoffset_this_refresh)
    copypos3d(cursor_unoffset_setting,cursor_offset);
  cursor_offset_this_refresh = cursor_unoffset_this_refresh = FALSE;
}
    

  void
select_cursor(cursorkind new_cursorkind)
{
  current_cursor = new_cursorkind; /* boy that was rough */
}

  void
do_cursor_deselect(listptr worldlist)
{
  worldptr thisworld;
  ;
  thisworld = worldlist->first.world;
  while (thisworld != Nil)
  {
    if ((thisworld->world_proplists[selectable_prop]->numelems > 0) &&
	(thisworld != primary_world)) /* this condition is really a HACKhack */
      return;
    thisworld =thisworld->next;
  }
  /* if nothing was selectable in any world, use Pluscursor */
  select_cursor(Pluscursor);
  offset_cursor_image(default_cursor_offset);
}

/* notify the cursor object to pause for X number of refreshes... e.g., when */
/* section lines are oncut, etc... */

  void
notify_cursor_pause(void)
{
  cursor_paused = TRUE;
}

  void
do_cursor_notifications(void)
{
  CPL_word notify_word;
  ;
  if (cursor_paused)
  {
    /* Tell cursor to stick in z... (if constraining on, see cpl/cursor.cpl.)*/
    /* Note that instead of using encode_CPL_RAM_instruction, I'm using */
    /* direct message tagging because otherwise i'll lose a refresh doing */
    /* the actual tag before executing it, which will make the cursor */
    /* overshoot its mark. */
    setup_CPL_word(&notify_word,Message_data);
    notify_word.word_data.CPLmessage = lookup_CPL_message("PauseinZ");
    enter_in_temp_message_block(&notify_word);
    queue_feature_message((featureptr) cursor_obj,Prioritymsg);
/*
    encode_CPL_RAM_instruction(" MAKEMSG Msg:PauseinZ");
    encode_CPL_RAM_instruction(" QMSG Obj:Crosshair_Cursor,Const:Prioritymsg");
*/
    cursor_paused = FALSE;
  }
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*                 ROUTINES TO FIND A CURSOR'S SCREENBBOX     		     */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* Find a screenbbox for a normal,single piece, */
/* bounded cursor (unlike the xhair). */

  bboxscreenptr
find_normal_cursor_screenbbox(objptr cursor_obj)
{
  GLfloat *centerpt;
  static bboxscreentype cursor_screenbbox;
  bbox3dtype cursorbbox;
  vertype expansion;
  ;
  centerpt = First_obj_vfe(cursor_obj)->pos;
  copypos3d(cursor_sizes[current_cursor],expansion);
  addpos3d(centerpt,expansion,cursorbbox.maxcorner);
  addpos3d(cursorbbox.maxcorner,cursor_offset,cursorbbox.maxcorner);
  flip_vec(expansion);
  addpos3d(centerpt,expansion,cursorbbox.mincorner);
  compute_screen_bbox(&cursorbbox,&cursor_screenbbox);
  return (&cursor_screenbbox);
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*     	            CURSOR BLIT SAVING ROUTINES     		             */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

  void
get_normal_cursor_blit(objptr cursor_obj,int framebuffer,stateptr state)
{
  bbox3dtype cursor_bbox;
  bboxscreenptr new_screen_bbox;
  int x1,y1,x2,y2;
  ;
  new_screen_bbox = find_normal_cursor_screenbbox(cursor_obj);
  
  /* get xy extents within window of last blit */
  x1 = saved_blits[framebuffer].x1 = new_screen_bbox->mincorner[vx];
  y1 = saved_blits[framebuffer].y1 = new_screen_bbox->mincorner[vy];
  x2 = saved_blits[framebuffer].x2 = new_screen_bbox->maxcorner[vx];
  y2 = saved_blits[framebuffer].y2 = new_screen_bbox->maxcorner[vy];

	/* OGLXXX SRC_AUTO not really supported -- see glReadBuffer man page */
  glReadBuffer(GL_BACK);
	/* OGLXXX lrectread: see man page for glReadPixels */
  glReadPixels(x1, y1, (x2)-(x1)+1, (y2)-(y1)+1, GL_RGBA, GL_BYTE, saved_blits[framebuffer].pix_save_area);

  saved_blits[framebuffer].validblit = TRUE; /* valid unless told otherwise */
}

  void
get_xhair_cursor_blit(objptr cursor_obj,int framebuffer,stateptr state)
{
  /* needs to be coded to get two separate blits, one for each xhair */
  get_normal_cursor_blit(cursor_obj, framebuffer,state);
}

  void
get_saved_blit(objptr cursor_obj,int framebuffer,stateptr state)
{
  bbox3dtype cursor_bbox;
  bboxscreentype new_screen_bbox;
  GLfloat mywidth = 35.0;
  int x1,y1,x2,y2;
  ;
  (*(cursor_records[current_cursor].cursor_saveptr))(cursor_obj,framebuffer,
							 state);

}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*     	            CURSOR BLIT RESTORING ROUTINES     		             */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

  void
restore_normal_cursor_blit(int framebuffer)
{
  int x1,y1,x2,y2;
  ;
  /* get xy position within window of last blit */
  x1 = saved_blits[framebuffer].x1;
  y1 = saved_blits[framebuffer].y1;
  x2 = saved_blits[framebuffer].x2;
  y2 = saved_blits[framebuffer].y2;

  /* blit rgb back into framebuffer */
	/* OGLXXX lrectwrite: see man page for glDrawPixels */
  glRasterPos2i(x1, y1);glDrawPixels((x2)-(x1)+1, (y2)-(y1)+1, GL_RGBA, GL_BYTE, saved_blits[framebuffer].pix_save_area);
}

  void
restore_xhair_cursor_blit(int framebuffer)
{
  /* needs to be coded to get two separate blits, one for each xhair */
  restore_normal_cursor_blit(framebuffer);
}

  void
restore_from_saved_blit(int framebuffer)
{
  int x1,y1,x2,y2;
  ;
  /* Use the current cursor's restore technique to call the correct */
  /* drawing routine for the cursor (set by calls from */
  /* selectability routines). */
  (*(cursor_records[current_cursor].cursor_restoreptr))(framebuffer);
}


    
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*     	            CURSOR DRAWING ROUTINES     		             */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* draw the "hidden" cursor: ie none at all */
  void
draw_hidden_cursor(objptr cursor_obj)
{
}

/* draw a small "plus" crosshair */

  void
draw_plus_cursor(objptr cursor_obj)
{
  vertype pos1, pos2;
  GLfloat crosshairwidth = 27.0,backdashdist = 5.0,forwardist = 1.0;
  GLfloat circlerad = 10.0;
  GLfloat ticksize = 10.0, tickoffsetz = 5.0;
  vertype pluspts[8],centerpt;
  GLfloat *cursorctr;
  int i;
  ;
  cursorctr = First_obj_vfe(cursor_obj)->pos;
  addpos3d(cursorctr,cursor_offset,centerpt);  
  setpos3d(pluspts[0],centerpt[vx] - crosshairwidth, centerpt[vy],
	   centerpt[vz]);
  setpos3d(pluspts[1],centerpt[vx] - circlerad, centerpt[vy],
	   centerpt[vz]);
  setpos3d(pluspts[2],centerpt[vx], centerpt[vy] + circlerad,
	   centerpt[vz]);
  setpos3d(pluspts[3],centerpt[vx], centerpt[vy] + crosshairwidth,
	   centerpt[vz]);
  setpos3d(pluspts[4],centerpt[vx] + circlerad, centerpt[vy],
	   centerpt[vz]);
  setpos3d(pluspts[5],centerpt[vx] + crosshairwidth, centerpt[vy],
	   centerpt[vz]);
  setpos3d(pluspts[6],centerpt[vx], centerpt[vy] - circlerad,
	   centerpt[vz]);
  setpos3d(pluspts[7],centerpt[vx], centerpt[vy] - crosshairwidth,
	   centerpt[vz]);

  if ((drawflags & Drawroom) != 0)
  {
    if (pluspts[0][vx] < Mincrossx)
      pluspts[0][vx] = Mincrossx;
    if (pluspts[1][vx] < Mincrossx)
      pluspts[1][vx] = Mincrossx;
    
    if (pluspts[2][vy] > Maxcrossy)
      pluspts[2][vy] = Maxcrossy;
    if (pluspts[3][vy] > Maxcrossy)
      pluspts[3][vy] = Maxcrossy;
    
    if (pluspts[4][vx] > Maxcrossx)
      pluspts[4][vx] = Maxcrossx;
    if (pluspts[5][vx] > Maxcrossx)
      pluspts[5][vx] = Maxcrossx;
    
    if (pluspts[6][vy] < Mincrossy)
      pluspts[6][vy] = Mincrossy;
    if (pluspts[7][vy] < Mincrossy)
      pluspts[7][vy] = Mincrossy;
  }

  /* Draw cursor in white, stippled, behind everything. */
  glDepthFunc(GL_ALWAYS);
  glColor4f((((GLfloat) white[0]) / 255.0F),
	    (((GLfloat) white[1]) / 255.0F),
	    (((GLfloat) white[2]) / 255.0F),
	    1.0F);
  glLineWidth((GLfloat)(2));

  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, Dotpattern);

  glPushMatrix();
  glTranslatef(0.0, 0.0, -backdashdist);
  glBegin(GL_LINES);
  for (i = 0; i < 8; ++i)
    glVertex3fv(pluspts[i]);
  glEnd();
  glTranslatef(0.0, 0.0, centerpt[vz]);
  /* OGLXXX See gluDisk man page. */
  {
    GLUquadricObj *qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);
    glPushMatrix();
    glTranslatef(centerpt[vx], centerpt[vy], 0.0);
    gluDisk( qobj, 0., circlerad, 32, 1);
    glPopMatrix();
    gluDeleteQuadric(qobj);
  };
  glPopMatrix();

  glDisable(GL_LINE_STIPPLE);

  /* Now draw in black, behind cyan part */
  glDepthFunc(GL_LEQUAL);
  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);
  glLineWidth((GLfloat)(2));
	/* OGLXXX setlinestyle: Check list numbering. */
  glDisable(GL_LINE_STIPPLE);
  glPushMatrix();
  glTranslatef(2.0, -2.0, 0.0);
  for (i = 0; i < 8; i += 2)
  {
    /* OGLXXX for multiple, independent line segments: use GL_LINES */
    glBegin(GL_LINE_STRIP);
    glVertex3fv(pluspts[i]);
    glVertex3fv(pluspts[i+1]);
    glEnd();
  }
  glTranslatef(0.0, 0.0, centerpt[vz]);
	/* OGLXXX See gluDisk man page. */
  {
    GLUquadricObj *qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);
    glPushMatrix();
    glTranslatef(centerpt[vx], centerpt[vy], 0.0);
    gluDisk( qobj, 0.0, circlerad, 32, 1);
    glPopMatrix();
    gluDeleteQuadric(qobj);
  };
  glPopMatrix();
  
  /* Now draw in front of the black, in cyan */
  glColor4f((((GLfloat) crosshaircolor[0]) / 255.0F),
	    (((GLfloat) crosshaircolor[1]) / 255.0F),
	    (((GLfloat) crosshaircolor[2]) / 255.0F),
	    1.0F);
  glPushMatrix();
  glTranslatef(0.0, 0.0, forwardist);
  glLineWidth((GLfloat)(2.0));
  for (i = 0; i < 8; i += 2)
  {
    /* OGLXXX for multiple, independent line segments: use GL_LINES */
    glBegin(GL_LINE_STRIP);
    glVertex3fv(pluspts[i]);
    glVertex3fv(pluspts[i+1]);
    glEnd();
  }

  glLineWidth((GLfloat)(2));
  glTranslatef(0.0, 0.0, centerpt[vz]);
	/* OGLXXX See gluDisk man page. */
  {
    GLUquadricObj *qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);
    glPushMatrix();
    glTranslatef(centerpt[vx], centerpt[vy], 0.0);
    gluDisk( qobj, 0., circlerad, 32, 1);
    glPopMatrix();
    gluDeleteQuadric(qobj);
  };
  glPopMatrix();
}

  void
draw_arrow_cursor(objptr cursor_obj)
{
  static vertype arrow[7] = {{0.0,0.0,0.0},
			      {Arrowcursorwidth, -Arrowcursorheight*0.55,0.0},
			     {Arrowcursorstemwidth,
				-Arrowcursorheight * 0.5,0.0},
			     {Arrowcursorstemwidth, -Arrowcursorheight,0.0},
			     {-Arrowcursorstemwidth,
				-Arrowcursorheight,0.0},
			     {-Arrowcursorstemwidth,
				-Arrowcursorheight * 0.5,0.0},
			      {-Arrowcursorwidth, -Arrowcursorheight*0.55,0.0}};
  static vertype leftarrowtip[4] = {{0.0,0.0,0.0},
			      {Arrowcursorwidth, -Arrowcursorheight*0.55,0.0},
			     {0.0, -Arrowcursorheight * 0.5,0.0},
			      {-Arrowcursorwidth, -Arrowcursorheight*0.55,0.0}};
  static vertype rightarrowtip[4] = {{0.0,0.0,0.0},
			      {Arrowcursorwidth, -Arrowcursorheight*0.55,0.0},
			     {0.0, -Arrowcursorheight * 0.5,0.0},
			      {-Arrowcursorwidth, -Arrowcursorheight*0.55,0.0}};
  static vertype arrowbase[4] = {{Arrowcursorstemwidth,
				-Arrowcursorheight * 0.5,0.0},
				 {Arrowcursorstemwidth, -Arrowcursorheight,0.0},
				 {-Arrowcursorstemwidth, -Arrowcursorheight,0.0},
				 {-Arrowcursorstemwidth,
				    -Arrowcursorheight * 0.5,0.0}};
  int i;
  GLfloat *cursorctr;
  vertype centerpt;
  ;
  glPushMatrix();
  cursorctr = First_obj_vfe(cursor_obj)->pos;
  addpos3d(cursorctr,cursor_offset,centerpt);  

  glTranslatef(centerpt[vx], centerpt[vy], centerpt[vz]);
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(-350), ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, Dotpattern);
  glLineWidth((GLfloat)(2.0));
  
  /* draw cursor underline dots,behind plane, and smaller */
  glPushMatrix();
  glTranslatef(0.0, 0.0, -10.0);
  glScalef(0.7, 0.7, 0.7);
  glDepthFunc(GL_ALWAYS);
  glColor4f((((GLfloat) cursor_obj->thecolor[0]) / 255.0F),
	    (((GLfloat) cursor_obj->thecolor[1]) / 255.0F),
	    (((GLfloat) cursor_obj->thecolor[2]) / 255.0F),
	    1.0F);
  glBegin(GL_LINE_LOOP);
  i = 0;
  while (i < (sizeof(arrow) / sizeof(vertype)))
    glVertex3fv(arrow[i++]);
  glEnd();
  glPopMatrix();

	/* OGLXXX setlinestyle: Check list numbering. */
  if(0) {glCallList(0); glEnable(GL_LINE_STIPPLE);} else glDisable(GL_LINE_STIPPLE);
  /* draw cursor shadow */
  glDepthFunc(GL_LEQUAL);
  glTranslatef(3.0, -3.0, 0.0);
  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);
	/* OGLXXX
	 * special cases for polygons:
	 * 	independant quads: use GL_QUADS
	 * 	independent triangles: use GL_TRIANGLES
	 */
  glBegin(GL_POLYGON);
  i = 0;
  while (i < (sizeof(leftarrowtip) / sizeof(vertype)))
    glVertex3fv(leftarrowtip[i++]);
  glEnd();
  glBegin(GL_POLYGON);
  i = 0;
  while (i < (sizeof(rightarrowtip) / sizeof(vertype)))
    glVertex3fv(rightarrowtip[i++]);
  glEnd();
  glBegin(GL_POLYGON);
  i = 0;
  while (i < (sizeof(arrowbase) / sizeof(vertype)))
    glVertex3fv(arrowbase[i++]);
  glEnd();

  glTranslatef(-3.0, 3.0, 0.0);

  /* draw cursor body */
  glTranslatef(0.0, 0.0, 3.0);
  glColor4f((((GLfloat) cursor_obj->thecolor[0]) / 255.0F),
	    (((GLfloat) cursor_obj->thecolor[1]) / 255.0F),
	    (((GLfloat) cursor_obj->thecolor[2]) / 255.0F),
	    1.0F);
  glBegin(GL_POLYGON);
  i = 0;
  while (i < (sizeof(leftarrowtip) / sizeof(vertype)))
    glVertex3fv(leftarrowtip[i++]);
  glEnd();

  glBegin(GL_POLYGON);
  i = 0;
  while (i < (sizeof(rightarrowtip) / sizeof(vertype)))
    glVertex3fv(rightarrowtip[i++]);
  glEnd();
  glBegin(GL_POLYGON);
  i = 0;
  while (i < (sizeof(arrowbase) / sizeof(vertype)))
    glVertex3fv(arrowbase[i++]);
  glEnd();
  glPopMatrix();
}

  void
draw_ibeam_cursor(objptr cursor_obj)
{
  GLfloat *cursorctr;
  vertype centerpt;
  static vertype left_ibeampts[6] = {{-10.0,22.0,0.0},
				     {-3.0,22.0,0.0},
				     {0.0,20.0,0.0},
				     {0.0,-10.0,0.0},
				     {-3.0,-12.0,0.0},
				     {-10.0,-12.0,0.0}};
  static vertype right_ibeampts[6] = {{10.0,22.0,0.0},
				     {3.0,22.0,0.0},
				     {0.0,20.0,0.0},
				     {0.0,-10.0,0.0},
				     {3.0,-12.0,0.0},
				     {10.0,-12.0,0.0}};
  int i;
  ;
  cursorctr = First_obj_vfe(cursor_obj)->pos;
  addpos3d(cursorctr,cursor_offset,centerpt);  

  glDepthFunc(GL_ALWAYS);
  glPushMatrix();
  glTranslatef(centerpt[vx], centerpt[vy], centerpt[vz]-10.0);
	/* OGLXXX setlinestyle: Check list numbering. */
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, Dotpattern);

  glLineWidth((GLfloat)(1));
  glColor4f((((GLfloat) white[0]) / 255.0F),
	    (((GLfloat) white[1]) / 255.0F),
	    (((GLfloat) white[2]) / 255.0F),
	    1.0F);
  glBegin(GL_LINE_STRIP);
  for (i = 0; i < sizeof(left_ibeampts) / sizeof(vertype); ++i)
    glVertex3fv(left_ibeampts[i]);
  glEnd();
  glBegin(GL_LINE_STRIP);
  for (i = 0; i < sizeof(right_ibeampts) / sizeof(vertype); ++i)
    glVertex3fv(right_ibeampts[i]);
  glEnd();
  glDisable(GL_LINE_STIPPLE);

  glTranslatef(2.0, 2.0, 10.0);
  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);
  glDepthFunc(GL_LEQUAL);
  glLineWidth((GLfloat)(2));
  glBegin(GL_LINE_STRIP);
  for (i = 0; i < sizeof(left_ibeampts) / sizeof(vertype); ++i)
    glVertex3fv(left_ibeampts[i]);
  glEnd();

  glBegin(GL_LINE_STRIP);
  for (i = 0; i < sizeof(right_ibeampts) / sizeof(vertype); ++i)
    glVertex3fv(right_ibeampts[i]);
  glEnd();

  glTranslatef(-2.0,-2.0,2.0);
  glColor4f((((GLfloat) white[0]) / 255.0F),
	    (((GLfloat) white[1]) / 255.0F),
	    (((GLfloat) white[2]) / 255.0F),
	    1.0F);
  glLineWidth((GLfloat)(2));
  glBegin(GL_LINE_STRIP);
  for (i = 0; i < sizeof(left_ibeampts) / sizeof(vertype); ++i)
    glVertex3fv(left_ibeampts[i]);
  glEnd();

  glBegin(GL_LINE_STRIP);
  for (i = 0; i < sizeof(right_ibeampts) / sizeof(vertype); ++i)
    glVertex3fv(right_ibeampts[i]);
  glEnd();

  glPopMatrix();
}

/* draw the full crosshair cursor spanning the cutplane's breadth. */

  void
draw_xhair_cursor(objptr cursor_obj)
{
  GLfloat *cursorctr;
  vertype centerpt;
  vertype pos1, pos2;
  ;
  cursorctr = First_obj_vfe(cursor_obj)->pos;
  addpos3d(cursorctr,cursor_offset,centerpt);  

	/* OGLXXX
	 * editobj not supported
	 *  -- use display list hierarchy
	 * editobj(Crosshair)
	 */
  /*DELETED*/;
#ifdef IRIS
  replace(7,0);
#endif /* IRIS */
	/* OGLXXX setlinestyle: Check list numbering. */
    if(0) {glCallList(0); glEnable(GL_LINE_STIPPLE);} else glDisable(GL_LINE_STIPPLE);
  glColor4f((((GLfloat) black[0]) / 255.0F),
	    (((GLfloat) black[1]) / 255.0F),
	    (((GLfloat) black[2]) / 255.0F),
	    1.0F);
    glLineWidth((GLfloat)(2));
#ifdef IRIS
    replace(4,0);
#endif /* IRIS */
  glColor4f((((GLfloat) crosshaircolor[0]) / 255.0F),
	    (((GLfloat) crosshaircolor[1]) / 255.0F),
	    (((GLfloat) crosshaircolor[2]) / 255.0F),
	    1.0F);
    glLineWidth((GLfloat)(3));
      /* replace shadow line in normal position, even if xhair not moved. */
    setpos3d(pos1,centerpt[vx]+Crosshairshadowoffset,Mincrossy,
	    centerpt[vz]);
    setpos3d(pos2,centerpt[vx]+Crosshairshadowoffset,Maxcrossy,
	    centerpt[vz]);
#ifdef IRIS
    replace(2,0);
#endif /* IRIS */

    glVertex3fv(pos1);
    glVertex3fv(pos2);
    setpos3d(pos1,Mincrossx,centerpt[vy]-Crosshairshadowoffset,
	    centerpt[vz]);
    setpos3d(pos2,Maxcrossx,centerpt[vy]-Crosshairshadowoffset,
	    centerpt[vz]);
#ifdef IRIS
    replace(3,0);
#endif /* IRIS */
    glVertex3fv(pos1);
    glVertex3fv(pos2);   
  glEndList();

  glDepthFunc(GL_LEQUAL);
  zdrawstate = GL_LEQUAL;

	/* OGLXXX check list numbering */
  //glCallList(Crosshair);

#ifdef IRIS
	/* OGLXXX
	 * editobj not supported
	 *  -- use display list hierarchy
	 * editobj(Crosshair)
	 */
  /*DELETED*/;
#ifdef IRIS
    replace(7,0);
#endif /* IRIS */
	/* OGLXXX setlinestyle: Check list numbering. */
    if(Largedotline) {
      glCallList(Largedotline);
      glEnable(GL_LINE_STIPPLE);
    } else
      glDisable(GL_LINE_STIPPLE);
#ifdef IRIS
    replace(4,0);
#endif /* IRIS */
    glColor4f((((GLfloat) darkcyan[0]) / 255.0F),
	      (((GLfloat) darkcyan[1]) / 255.0F),
	      (((GLfloat) darkcyan[2]) / 255.0F),
	      1.0F);
    glLineWidth((GLfloat)(2));
    /* turn off xhair shadow line temporarily */
    setpos3d(pos1,0.0,Mincrossy,centerpt[vz]);
    setpos3d(pos2,0.0,Mincrossy,centerpt[vz]);
#ifdef IRIS
    replace(2,0);
#endif /* IRIS */
    glVertex3fv(pos1);
    glVertex3fv(pos2);
    setpos3d(pos1,Mincrossx,0.0,centerpt[vz]);
    setpos3d(pos2,Mincrossx,0.0,centerpt[vz]);
#ifdef IRIS
    replace(3,0);
#endif /* IRIS */
    glVertex3fv(pos1);
    glVertex3fv(pos2);   
  glEndList();

  glDepthFunc(GL_GREATER);
  zdrawstate = GL_GREATER;

  /* OGLXXX check list numbering */
  //glCallList(Crosshair);
#endif
}

/* Draw the cursor, by forking to the appropriate routine stored in */
/* the cursor_obj drawtechnique... (also stored in */
/* cursor_records[current_cursor])... */

  void
draw_cursor(objptr cursor_obj)
{
  /* Use the current cursor's drawtechnique field to call the correct */
  /* drawing routine for the cursor (set by calls from */
  /* selectability routines). */
  plant_cursor_image();
  (*(cursor_records[current_cursor].cursor_drawptr))(cursor_obj);
}

