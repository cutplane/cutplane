
/* FILE: polhemus.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Polhemus Reading Routines for the Cutplane Editor            */
/*                                                                        */
/* Author: LJE&WAK                            Date: September 22,1990     */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990,1991 William Kessler, Larry          */
/* Edwards (Beyond Technology, Inc.), and Jim Kramer (Virtex, Inc.)       */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define POLHEMUSMODULE

#include <config.h>

#include <stdio.h>

#include <platform_gl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>	/* for sleep() in write_polhemus_initialization */

#include <cutplane.h>
#include <globals.h>
#include <controls.h>
#include <math3d.h>
#include <cpl_cutplane.h>
#include <glove.h>
#include <polhemus.h>

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			GLOBAL VARIABLES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


static struct termio polhtermio = {0,
				     0,
				     B19200|CS8|CREAD|CLOCAL,
				     0,
				     '^A',
				     0};

static struct termio safetermio = {IXANY|IXON|ICRNL|ISTRIP|BRKINT|IGNPAR,
				     TAB3|ONLCR|OPOST,
				     HUPCL|CLOCAL|CREAD|CS8|B9600,
				     ECHOK|ECHO|ICANON|ISIG,
				     '^A',
//				     "\177^\\r^U^D"};
				     0};


int polhfd;

	/* OGLXXX XXX fix this. */
GLfloat * polh_base_xform = {{1.0,0.0,0.0,0.0},
			  {0.0,1.0,0.0,0.0},
			  {0.0,0.0,1.0,0.0},
			  {0.0,0.0,0.0,1.0}};

	/* OGLXXX XXX fix this. */
GLfloat * polh_base_invxform = {{1.0,0.0,0.0,0.0},
			     {0.0,1.0,0.0,0.0},
			     {0.0,0.0,1.0,0.0},
			     {0.0,0.0,0.0,1.0}};

/* current and prev polhemus position vec and orientation matrix and deltas */
polhstatestruct polhemustatestruct;
polhstateptr polhemustate = &polhemustatestruct;
/* for calibration: */
static calibratestruct polhemuscal;
/* For filtering: */
static int historylength = 2;
static polhemusvalstruct *history;
/* for adjusting polhemus sensitivity and location */
vertype polhemus_posoffset,polhemus_poscale;
int transform_routine_index;

static CPL_word op_word,as_word,ar_word,at_word,ad_word;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*		     LOW LEVEL SUPPORT ROUTINES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
init_polhemus_serialport(void)
{
  polhfd = open("/dev/ttyd2",O_RDWR);
  if (polhfd == -1)
  {
    perror ("Error opening polhemus for reading, milordship! ");
    exit(1);
  }  
  /* finish setting up new tty state */
  polhtermio.c_cc[VMIN] = 1;	/* incoming char buffer size */
  polhtermio.c_cc[VTIME] = 0;	/* length time to wait for chars */
  /* enact new tty state: */
  ioctl(polhfd,TCSETA,&polhtermio);
}

void
close_polhemus_serialport(void)
{
  /* reset tty to a safe state */
  ioctl(polhfd,TCSETA,&safetermio);
  close(polhfd);
}

  void
write_polhemus_initialization(void)
{
  static char headerbytes[3] = {Resetpolhemus,Quietmode,Binarymode};
  int numbytes;
  ;
  if ((numbytes = write(polhfd,headerbytes,1)) < 1)
    printf ("Failure writing polhemus init: %d bytes written\n",numbytes);

  sleep(5);			/* needs <unistd.h> above */

  if ((numbytes = write(polhfd,&(headerbytes[1]),2)) < 2)
    printf ("Failure writing polhemus init: %d bytes written\n",numbytes);
}

  void
write_polhemus_request(void)
{
  static char requestbytes[1] = {0x50};
  int numbytes;
  ;
  if ((numbytes = write(polhfd,requestbytes,1)) < 1)
    printf ("Failure writing polhemus request: %d bytes written\n",numbytes);
}

void
read_polhemus_bytes(char buf[], int nbytes)
{
  int i;
  ;
  for (i=0; i<nbytes; i++)
    while (read(polhfd,&buf[i],1) < 1);
}

  GLboolean
read_polhemus_status(void)
{
  static char statusbytes[3];
  ;
  statusbytes[0] = 0;
  statusbytes[1] = 0;
  statusbytes[2] = 0;
  read_polhemus_bytes(statusbytes,3);

  return ((statusbytes[0] == '0') && (statusbytes[1] == '1') &&
	  (statusbytes[2] == ' '));
}

  void
read_polhemus_trailer(void)
{
  char trailerbuffer[2];
  int numbytes;
  ;
  /* read the trailer bytes, should be CR/LF */
  read_polhemus_bytes(trailerbuffer,2);
  if ((trailerbuffer[0] != Carriagereturn) || (trailerbuffer[1] != Linefeed))
    printf ("Failure in read_polhemus_trailer: wrong bytes read (%c %c)\n",
	    trailerbuffer[0],trailerbuffer[1]);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*          Routines to build various polhemus transformstransforms           */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
make_polh_base_xform(void)
{
  static vertype baseoffset = {0.0,0.0,0.0};
  ;
  /* the positive x-axis of the polhemus goes either out along its cord */
  /* or in the opposite direction (x is always positive) */
  /* the positive y-axis goes to the left when viewed from above */
  /* the positive z-axis goes up through the top */

  glPushMatrix();

  /* build polhemus base to 3form transform */
  glLoadMatrixf(imatrix);
  /* note base offset is given in unrotated polhemus base coords */
  glTranslatef(baseoffset[vx], baseoffset[vy], baseoffset[vz]);
	/* OGLXXX You can do better than this. */
  glRotatef(180.0, ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(-90.0, ('x')=='x'||('x')=='X', ('x')=='y'||('x')=='Y', ('x')=='z'||('x')=='Z');
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, polh_base_xform);

  /* build 3form to polhemus base transform */
  glLoadMatrixf(imatrix);
	/* OGLXXX You can do better than this. */
  glRotatef(90.0, ('x')=='x'||('x')=='X', ('x')=='y'||('x')=='Y', ('x')=='z'||('x')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(-180.0, ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z');
  glTranslatef(-baseoffset[vx], -baseoffset[vy], -baseoffset[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, polh_base_invxform);

  glPopMatrix();
}

  void
	/* OGLXXX XXX fix this. */
make_polhemus_rotatematrix(GLfloat * transformatrix)
{
  /* the base to sensor euler angles are returned by the polhemus         */
  /* the euler angles are NOT about a fixed frame, and are given as       */
  /* yaw,pitch,roll (i.e. azimuth,elevation,roll) that is:                */
  /* rotate about Z followed by rotate about Y followed by rotate about X */

  glPushMatrix();
  glLoadMatrixf(imatrix);
	/* OGLXXX You can do better than this. */
  glRotatef(polhemustate->orient[vx], ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z'); /* yaw   */
	/* OGLXXX You can do better than this. */
  glRotatef(polhemustate->orient[vy], ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z'); /* pitch */
	/* OGLXXX You can do better than this. */
  glRotatef(polhemustate->orient[vz], ('x')=='x'||('x')=='X', ('x')=='y'||('x')=='Y', ('x')=='z'||('x')=='Z'); /* roll  */
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, transformatrix);
  glPopMatrix();
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*		 HIGH LEVEL I/O AND INIT ROUTINES                	*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
read_polhemus(polhemusvalptr polhvals)
{
  char databuffer[12];
  short thenumber;
  int lowbyte,highbyte;
  static vertype angleconversion = {conv2deg,conv2deg,conv2deg};
  ;
  do
  {
    write_polhemus_request();
  } while (!read_polhemus_status());
  read_polhemus_bytes(databuffer,12);

  thenumber = (databuffer[0] | (databuffer[1] << 8));
  polhvals->pos[vx] = (float) thenumber;

  thenumber = (databuffer[2] | (databuffer[3] << 8));
  polhvals->pos[vy] =  (float) thenumber;

  thenumber = (databuffer[4] | (databuffer[5] << 8));
  polhvals->pos[vz] =  (float) thenumber;

  thenumber = (databuffer[6] | (databuffer[7] << 8));
  polhvals->orient[vx] = (float) thenumber;

  thenumber = (databuffer[8] | (databuffer[9] << 8));
  polhvals->orient[vy] =  (float) thenumber;

  thenumber = (databuffer[10] | (databuffer[11] << 8));
  polhvals->orient[vz] =  (float) thenumber;

  read_polhemus_trailer();

  addpos3d(polhvals->pos,polhemus_posoffset,polhvals->pos);
  multvec3d(polhvals->pos,polhemus_poscale,polhvals->pos);
  multvec3d(polhvals->orient,angleconversion,polhvals->orient);
}

  void
update_polh_xform(void)
{		/* update polhemus base to polhemus sensor transform */
	/* OGLXXX XXX fix this. */
  GLfloat * oldxform,oldinvxform,temp;
	/* OGLXXX XXX fix this. */
  GLfloat * world2sensor,sensor2world;
  ;
  /* the positive x-axis of the polhemus goes either out along its cord */
  /* or in the opposite direction (x is always positive) */
  /* the positive y-axis goes to the left when viewed from above */
  /* the positive z-axis goes up through the top */

  /* the base to sensor euler angles are returned by the polhemus         */
  /* the euler angles are NOT about a fixed frame, and are given as       */
  /* yaw,pitch,roll (i.e. azimuth,elevation,roll) that is:                */
  /* rotate about Z followed by rotate about Y followed by rotate about X */

  copy_matrix(polhemustate->xform,oldxform);
  copy_matrix(polhemustate->invxform,oldinvxform);
  glPushMatrix();

  /* determine new xform */
  glLoadMatrixf(imatrix);
  glTranslatef(polhemustate->pos[vx], polhemustate->pos[vy], polhemustate->pos[vz]);
	/* OGLXXX You can do better than this. */
  glRotatef(polhemustate->orient[vx], ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z'); /* yaw   */
	/* OGLXXX You can do better than this. */
  glRotatef(polhemustate->orient[vy], ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z'); /* pitch */
	/* OGLXXX You can do better than this. */
  glRotatef(polhemustate->orient[vz], ('x')=='x'||('x')=='X', ('x')=='y'||('x')=='Y', ('x')=='z'||('x')=='Z'); /* roll  */
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, polhemustate->xform);

  /* determine new invxform */
  glLoadMatrixf(imatrix);
	/* OGLXXX You can do better than this. */
  glRotatef(-polhemustate->orient[vz], ('x')=='x'||('x')=='X', ('x')=='y'||('x')=='Y', ('x')=='z'||('x')=='Z'); /* roll  */
	/* OGLXXX You can do better than this. */
  glRotatef(-polhemustate->orient[vy], ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z'); /* pitch */
	/* OGLXXX You can do better than this. */
  glRotatef(-polhemustate->orient[vx], ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z'); /* yaw   */
  glTranslatef(-polhemustate->pos[vx], 	    -polhemustate->pos[vy], 	    -polhemustate->pos[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, polhemustate->invxform);

  /* mult old invxform by new xform to get the new deltaxform */
  glLoadMatrixf(oldinvxform);
  glMultMatrixf(polhemustate->xform);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, polhemustate->deltaxform);

  /* mult newinvxform by old xform to get inverse of deltaxform */
  glLoadMatrixf(polhemustate->invxform);
  glMultMatrixf(oldxform);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, polhemustate->invdeltaxform);

  glLoadMatrixf(polh_base_invxform); /* world to polhemus */
  glMultMatrixf(oldxform);		/* polhemus to sensor */
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, world2sensor);
  /* we dont want translation in there */
  world2sensor[3][vx] = 0.0;
  world2sensor[3][vy] = 0.0;
  world2sensor[3][vz] = 0.0;
  transpose_matrix(world2sensor,sensor2world);

  glLoadMatrixf(world2sensor);
  glMultMatrixf(polhemustate->deltaxform);
  glMultMatrixf(sensor2world);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, polhemustate->deltaxform_world);

  glLoadMatrixf(world2sensor);
  glMultMatrixf(polhemustate->invdeltaxform);
  glMultMatrixf(sensor2world);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, polhemustate->invdeltaxform_world);

#if 0
  /* save old 3form to sensor transform */
  glLoadMatrixf(polh_base_invxform);
  glMultMatrixf(oldxform);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, polhemustate->oldxform_world);

/*
  multmatrix_4x4(polhemustate->deltaxform,polhemustate->invdeltaxform,
		 temp);
  printf("deltaxform:");
  printmatrix(temp);
*/
#endif
  glPopMatrix();
}

  GLboolean
update_polhemus_state(void)
{
  polhemusvalstruct newpolhvals;
  vertype deltapos,deltaorient;
  GLboolean polhemus_moved,polhemus_rotated;
  ;
  read_polhemus(&newpolhvals);

  diffpos3d(polhemustate->pos,newpolhvals.pos,deltapos);
  diffpos3d(polhemustate->orient,newpolhvals.orient,deltaorient);

  polhemus_moved = !(vector_near_zero(deltapos,1.0));
  polhemus_rotated = !(vec_eq_zero(deltaorient));
  if (polhemus_moved)
  {
    copypos3d(newpolhvals.pos,polhemustate->pos);
    copypos3d(deltapos,polhemustate->deltapos);
  }
  if (polhemus_rotated)
  {
    copypos3d(newpolhvals.orient,polhemustate->orient);
    copypos3d(deltaorient,polhemustate->deltaorient);
  }
  if ((polhemus_moved || polhemus_rotated) && glove_state == Gloverotatestate)
    update_polh_xform();
#if 0
  printf("------------------------------------------------------------------\n");
  printf("X: %-6.0f Y: %-6.0f Z: %-6.0f   YAW: %-6.0f PITCH: %-6.0f ROLL: %-6.0f\n",
	 polhemustate->pos[vx],polhemustate->pos[vy],polhemustate->pos[vz],
	 polhemustate->orient[vx],
	 polhemustate->orient[vy],
	 polhemustate->orient[vz]);
  printf("thisxform:");
  printmatrix(polhemustate->xform);
  printf("deltaxform:");
  printmatrix(polhemustate->deltaxform);
#endif
#ifdef debugmalloc
  malloc_check();
#endif
  return(polhemus_moved);
}

  void
init_polhemus(void)
{
  FILE *polhemusinfo;
  polhemusvalstruct polhvals;
  int cnt;
  ;
  setup_CPL_word_blank(&op_word,Opcode_data);
  setup_CPL_word_blank(&as_word,Blank_word);
  setup_CPL_word_blank(&at_word,Blank_word);
  setup_CPL_word_blank(&ad_word,Blank_word);

  polhemusinfo = fopen("./rsc/polhemus.rsc","r");
  /* reset the globals via the input file */
  fscanf (polhemusinfo, "Polhemus On:%d\n",&using_polhemus);
  
  if (using_polhemus)
  {

    fscanf(polhemusinfo,"Polhemus Position Scalar:%f,%f,%f\n",
	   &(polhemus_poscale[vx]),&(polhemus_poscale[vy]),
	   &(polhemus_poscale[vz]));
    fscanf(polhemusinfo,"Polhemus Position Error:%f,%f,%f\n",
	   &(polhemus_posoffset[vx]),&(polhemus_posoffset[vy]),
	   &(polhemus_posoffset[vz]));

    init_polhemus_serialport();
    write_polhemus_initialization();

    make_polh_base_xform();
    /* initialize the polhemus state */
    read_polhemus(&polhvals);
    copypos3d(polhvals.pos,polhemustate->pos);
    copypos3d(polhvals.orient,polhemustate->orient);

    /* initialize the xforms */
    copy_matrix(imatrix,polhemustate->deltaxform);
    copy_matrix(imatrix,polhemustate->invdeltaxform);
    glPushMatrix();

    glLoadMatrixf(imatrix);
    glTranslatef(polhemustate->pos[vx], polhemustate->pos[vy], polhemustate->pos[vz]);
	/* OGLXXX You can do better than this. */
    glRotatef(polhemustate->orient[vx], ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z'); /* yaw   */
	/* OGLXXX You can do better than this. */
    glRotatef(polhemustate->orient[vy], ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z'); /* pitch */
	/* OGLXXX You can do better than this. */
    glRotatef(polhemustate->orient[vz], ('x')=='x'||('x')=='X', ('x')=='y'||('x')=='Y', ('x')=='z'||('x')=='Z'); /* roll  */
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
    glGetFloatv(GL_MODELVIEW_MATRIX, polhemustate->xform);
    
    glLoadMatrixf(polh_base_invxform); /* 3form to polhemus xform */
    glMultMatrixf(polhemustate->xform); /* polhemus base to sensor */
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
    glGetFloatv(GL_MODELVIEW_MATRIX, polhemustate->oldxform_world); /* 3form to sensor */

    glLoadMatrixf(imatrix);
	/* OGLXXX You can do better than this. */
    glRotatef(-polhemustate->orient[vz], ('x')=='x'||('x')=='X', ('x')=='y'||('x')=='Y', ('x')=='z'||('x')=='Z'); /* roll  */
	/* OGLXXX You can do better than this. */
    glRotatef(-polhemustate->orient[vy], ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z'); /* pitch */
	/* OGLXXX You can do better than this. */
    glRotatef(-polhemustate->orient[vx], ('z')=='x'||('z')=='X', ('z')=='y'||('z')=='Y', ('z')=='z'||('z')=='Z'); /* yaw   */
    glTranslatef(-polhemustate->pos[vx], 	      -polhemustate->pos[vy], 	      -polhemustate->pos[vz]);
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
    glGetFloatv(GL_MODELVIEW_MATRIX, polhemustate->invxform);


    /* find address of C routine that does polhemus style transforms... */
    /* hackwheeze*/
    for (cnt = 0; cnt < MaxCroutines; ++cnt)
    {
      if (strcmp(Croutines[cnt].Croutine_name,"polhemus_transform") == 0)
	break;
    }
    transform_routine_index = cnt;
    glPopMatrix();
  }

  fclose(polhemusinfo);
#ifdef debugmalloc
  malloc_check();
#endif
}

  void
quit_polhemus(void)
{
  close_polhemus_serialport();
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*          ROUTINES TO TRANSLATE POLHEMUS MOVEMENTS INTO 3FORM MOVEMENTS     */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
encode_polhemus_roomrot(stateptr state)
{
  int deltax, deltay;
  ;
  deltax  = (int) polhemustate->deltapos[vz];
  deltay  = (int) -polhemustate->deltapos[vx];
  
  store_roomrot(deltax,deltay); /* see controls.c */
}  

/* update_polhemus_state must have been called before this routine is */
/* run: the value in polhemus_posdelta is assumed to be already computed. */

  void
encode_polhemus_cursor_move(stateptr state, glovestatekind glove_state,
			    GLboolean dragging)
{
  GLfloat temp;
	/* OGLXXX XXX fix this. */
  GLfloat * rotmatrix;
  vertype cursormove,unrotated_cursormove;
  ;
  /* do some coordinate switching */
  cursormove[vx] = -polhemustate->deltapos[vx];
  cursormove[vy] = -polhemustate->deltapos[vz];
  cursormove[vz] = -polhemustate->deltapos[vy];
  /* rotate the coordinates back from the user's oriented view into regular */
  /* cutplane xyz space...*/
  /* yecch gl calls outside of grafx.c a major no no */
  glPushMatrix();
  glLoadMatrixf(imatrix);
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(-state->roomrotx), ('x')=='x'||('x')=='X', ('x')=='y'||('x')=='Y', ('x')=='z'||('x')=='Z');
	/* OGLXXX You can do better than this. */
  glRotatef(.1*(-state->roomroty), ('y')=='x'||('y')=='X', ('y')=='y'||('y')=='Y', ('y')=='z'||('y')=='Z');
	/* OGLXXX
	 * getmatrix: you might mean
	 * glGetFloatv(GL_PROJECTION_MATRIX, $1)
	 */
  glGetFloatv(GL_MODELVIEW_MATRIX, rotmatrix);
  glPopMatrix();
  transformpt(cursormove,rotmatrix,unrotated_cursormove);

  /* make a couple of constraining adjustments  */
  switch (glove_state)
  {
  case Glovepointstate:
  case Glovedragstate:
    break;
  case GloveconstraintoXYstate:
  case GloveXYdragstate:
    unrotated_cursormove[vz] = 0.0;
    break;
  case GloveconstraintoZstate:
  case GloveZdragstate:
    unrotated_cursormove[vx] = unrotated_cursormove[vy] = 0.0;
    break;
  }

  store_cursor_move(unrotated_cursormove,dragging);
}

  void
encode_polhemus_transform(stateptr state)
{
  /* assuming you have a matrix ready, this just transforms all the picked */
  /* objects by it */
#ifdef debugmalloc
  malloc_check();
#endif

  /* push the polhemus state record pointer */
  setup_CPL_word_blank(&op_word,Opcode_data);
  op_word.word_data.CPLopcode = PUSH;
  setup_CPL_auxword(&as_word,CPLaux_address_data);
  as_word.word_data.CPLauxdata.CPLaddressdata = polhemustate;
  store_CPL_RAM_instruction(&op_word,&as_word);

  /* push all the picked objects... */
  setup_CPL_word_blank(&op_word,Opcode_data);
  op_word.word_data.CPLopcode = PUSH;
  setup_CPL_word_blank(&as_word,Proplist_data);
  as_word.word_data.CPLproplist->first.sort->sortdata1->i = picked_prop;
  as_word.contents_of = TRUE;
  store_CPL_RAM_instruction(&op_word,&as_word);

  /* call the Croutine that does the ugly work */
  op_word.word_data.CPLopcode = CRTN;
  setup_CPL_auxword(&as_word,CPLaux_Croutine_index_data);
  as_word.word_data.CPLauxdata.Croutine_index = transform_routine_index;
  store_CPL_RAM_instruction(&op_word,&as_word);

  /* ar word can just be blank because TRNSFRM doesn't use it when */
  /* transforming features... */
#ifdef debugmalloc
  malloc_check();
#endif
}
