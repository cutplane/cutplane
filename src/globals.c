
/* FILE: globals.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*               Global Variables for the Cutplane Editor                 */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define GLOBALSMODULE

#include <config.h>

#include <stdio.h>
#include <math.h>
#include <setjmp.h>

#include <platform_gl.h>

#include <cutplane.h>
#include <grafx.h>
#include <cpl_cutplane.h>
#include <rotatetool.h>

#include <demo.h>

char execfilename[80];

#ifndef WIN32
sigjmp_buf error_return;
#endif

unsigned short cutplane_time = 0;
int zdrawstate = GL_ALWAYS;

vertype x_axis = {1.0,0.0,0.0,1.0};
vertype y_axis = {0.0,1.0,0.0,1.0};
vertype z_axis = {0.0,0.0,1.0,1.0};
vertype rotx_axis = {1.0,0.0,0.0,0.0};
vertype roty_axis = {0.0,1.0,0.0,0.0};
vertype rotz_axis = {0.0,0.0,1.0,0.0};
vertype origin = {0.0,0.0,0.0,1.0};

Coord rotatetool_radius = Rotatetoolradius; 
vertype rotatetool_dock;

vertype cursor_delta;		/* debug only */

int screensizex;
int screensizey;

/* Used by main.c, for pc window/mouse interactions. */
int pc_winWidth, pc_winHeight;
int pc_winLeft, pc_winTop, pc_winRight, pc_winBottom;
int pc_xMargin, pc_yMargin;
int pc_innerLeft, pc_innerTop, pc_innerRight, pc_innerBottom;
float pc_windowMargin;

/*
long drawflags = 0;
*/

long drawflags = Drawroom | Drawplane | Drawsections | Drawshadows;

long mainwindow = 0, helpwindow = 0, infowindow = 0, slidewindow = 0;

int mainwindowleft;
int mainwindowright;
int mainwindowtop;
int mainwindowbottom;
int helpwindowleft;
int helpwindowright;
int helpwindowtop;
int helpwindowbottom;
int infowindowleft;
int infowindowright;
int infowindowtop;
int infowindowbottom;

Matrix imatrix = {{1.0,0.0,0.0,0.0},
		  {0.0,1.0,0.0,0.0},
		  {0.0,0.0,1.0,0.0},
		  {0.0,0.0,0.0,1.0}};

Matrix projmatrix = {{1.0,0.0,0.0,0.0},
		     {0.0,1.0,0.0,0.0},
		     {0.0,0.0,1.0,0.0},
		     {0.0,0.0,0.0,1.0}};


/* List of worlds in use in 3Form. */
listptr world_list;

worldptr scratch_world;		/* world to make temp shells in etc. */

worldptr environment_world;	/* pointer to the world containing the */
				/* cutplane, cursor, room, etc. */
worldptr primary_world;		/* pointer to the first world containing */
				/* user's objects. */
worldptr help_world;		/* pointer to world used to demonstrate help */
				/* concepts.*/
worldptr working_world;		/* pointer to world currently in use. */

/* Pointers to the basic environment objects */
objptr cursor_obj, cutplane_obj, room_obj, rotatetool_obj, systemcursor_obj;
objptr group_obj,scalebbox_obj,room_backwall_obj, line_obj;

Coord cursor_move_tolerance = 500.0;
Coord cursor_accel_tolerance;

statetype thestate,theoldstate;
stateptr state = &thestate;
stateptr oldstate = &theoldstate;

Boolean select_display_on;	/* used by get_selection,draw_one_cutevf() */
Boolean exit_system = FALSE;	/* Set to true when user wants to leave system*/
Boolean use_zbuffer = TRUE;	     /* Can be set to FALSE by IRIS users. */
short use_hidden_line_technique = 0; /* > 0 during rots w/wireframe */
Boolean rotate_this_refresh = FALSE; /* Set to true if ANYTHING rotated */
                                     /* during current refresh loop */
Boolean set_backface_flags = TRUE;   /* Set to TRUE when view rotates */

Coord nearclipdist,farclipdist;
double unitsperpixel[2];

int global_refresh = 0;		/* > 0 as long as total redraw necessary */

char cursor_string[50] = "{0.0 0.0 0.0}"; /* string showing where cursor is */
char cursor_delta_string[50] = "delta 0.0"; /* string showing drag amount */
char cursor_angle_string[50] = "< 0"; /* string showing angle change */

#ifdef IRIS
Boolean using_polhemus = FALSE;
Boolean using_glove = FALSE;
Boolean using_slideshow = TRUE;
Boolean stocks_demo_on = FALSE;
#else
Boolean using_polhemus = FALSE;
Boolean using_glove = FALSE;
#endif


/* these used to be defined with defcolor */

colortype black = {0,0,0};
colortype red = {255,0,0};
colortype green = {0,255,0};
colortype blue = {0,0,255};
colortype cyan = {0,255,255};
colortype magenta = {255,0,255};
colortype yellow = {255,255,0};
colortype white = {255,255,255};

colortype lightgrey = {150,150,150};
colortype mediumgrey = {100,100,100};
colortype darkgrey = {50,50,50};
colortype darkred = {128,0,0};
colortype darkblue = {0,50,50};
colortype darkcyan = {0,100,100};
colortype mediumyellow = {210,210,0};
colortype darkyellow = {100,100,0};

colortype pink = {255,150,150};
colortype bluegrey = {210,210,210/*166,166,166*/};
colortype darkbrown = {166,98,65};
colortype brown = {255,150,100};  /* was defcolor(brown,255,130,18);*/
colortype mediumbrown = {200,123,82};
colortype lightgreen = {100,255,100};
colortype orange = {255,110,10}; /* was mediumcyan once upon a time */
colortype lightblue = {128,128,255};
colortype dkbluegrey = {166,166,166};

colortype backgroundcolor = {255,175,125};
colortype cutplanecolor = {155,180,200};
colortype crosshaircolor = {0,255,255};
colortype selectablecolor = {200,200,255};
colortype pickedcolor = {0,0,0};
colortype pickedcontourcolor = {255,0,0};

colortype zbufferobjcolor = {166,166,166};
colortype shadowcolor = {100,100,110};
colortype rotatetoolcolor = {0,0,0};
colortype sectionlinecolor = {255,255,0};
colortype pickboxcolor = {255,255,0};

property noproperty_prop = 0;
property nullfeature_prop = 1;
property touched_prop = 2;
property killed_prop = 3;
property quickupdate_prop = 4;
property regularupdate_prop = 5;
property thislocation_prop = 6;
property nextlocation_prop = 7;
property msgpending_prop = 8;

property translated_prop = 9;
property selectable_prop = 10;
property nonselectable_prop = 11;
property picked_prop = 12;
property grabbed_prop = 13;

property transparent_prop = 14;
property neverdraw_prop = 15;
property alwaysdraw_prop = 16;
property nonmanifold_prop = 17;
property sectioninvisible_prop = 18;
property selectinvisible_prop = 19;
property pickedinvisible_prop = 20;
property lockedgeometry_prop = 21;
property connection_prop = 22;
property copyproperty_prop = 23;
property forcedcpnormal_prop = 24;
property hasdecal_prop = 25;
property haslabel_prop = 26;
property hasdimension_prop = 27;
property background_prop = 28;

property visible_prop = 29;
property invisible_prop = 30;
property hideable_prop = 31;
property luminous_prop = 32;
property noshadow_prop = 33;

property tool_prop = 34;
property menu_prop = 35;
property toolbox_prop = 36;
property active_prop = 37;
property activemenu_prop = 38;
property hotspot_prop = 39;
property oriented_prop = 40;
property dragonly_prop = 41;
property goodies_prop = 42;
property primitive_prop = 43;
property draw_prop = 44;
property erase_prop = 45;
property sketch_prop = 46;
property colorbar_prop = 47;
property paint_prop = 48;
property drill_prop = 49;
property slice_prop = 50;
property glue_prop = 51;
property stamp_prop = 52;
property magnetic_prop = 53;
property zoom_prop = 54;
property group_prop = 55;
property deform_prop = 56;
property puncture_prop = 57;
property lightbulb_prop = 58;

property toolbar_prop = 59;
property toolpad_prop = 60;
property toolvisible_prop = 61;
property headpad_prop = 62;
property padhandle_prop = 63;
property dialog_prop = 64;
property textinput_prop = 65;
property inputplate_prop = 66;
property scalebbox_prop = 67;

property next_available_property = 68;

propinfotype propinfo[Maxpossibleproperty] = {{"Noproperty",Nil},
					      {"Nullfeature",Nil},
					      {"Touched",Nil},
					      {"Killed",Nil},
					      {"Quickupdate",Nil},
					      {"Regularupdate",Nil},
					      {"Thislocation",Nil},
					      {"Nextlocation",Nil},
					      {"Msgpending",Nil},

					      {"Translated",Nil},
					      {"Selectable",Nil},
					      {"Nonselectable",Nil},
					      {"Picked",Nil},
					      {"Grabbed",Nil},

					      {"Transparent",Nil},
					      {"Neverdraw",Nil},
					      {"Alwaysdraw",Nil},
					      {"Nonmanifold",Nil},
					      {"Sectioninvisible",Nil},
					      {"Selectinvisible",Nil},
					      {"Pickedinvisible",Nil},
					      {"Lockedgeometry",Nil},
					      {"Connection",Nil},
					      {"Copyproperty",Nil},
					      {"Forcedcpnormal",Nil},
					      {"Hasdecal",Nil},
					      {"Haslabel",Nil},
					      {"Hasdimension",Nil},
					      {"Background",Nil},

					      {"Visible",Nil},
					      {"Invisible",Nil},
					      {"Hideable",Nil},
					      {"Luminous",Nil},
					      {"Noshadow",Nil},

					      {"Tool",Nil},
					      {"Menu",Nil},
					      {"Toolbox",Nil},
					      {"Active",Nil},
					      {"Activemenu",Nil},
					      {"Hotspot",Nil},
					      {"Oriented",Nil},
					      {"Dragonly",Nil},
					      {"Goodies",Nil},
					      {"Primitive",Nil},

					      {"Draw",Nil},
					      {"Erase",Nil},
					      {"Sketch",Nil},
					      {"Colorbar",Nil},
					      {"Paint",Nil},
					      {"Drill",Nil},
					      {"Slice",Nil},
					      {"Glue",Nil},
					      {"Stamp",Nil},
					      {"Magnetic",Nil},
					      {"Zoom",Nil},
					      {"Group",Nil},
					      {"Deform",Nil},
					      {"Puncture",Nil},
					      {"Lightbulb",Nil},

					      {"Toolbar",Nil},
					      {"Toolpad",Nil},
					      {"Toolvisible",Nil},
					      {"Headpad",Nil},
					      {"Padhandle",Nil},
					      {"Dialog",Nil},
					      {"Textinput",Nil},
					      {"Inputplate",Nil},
					      {"Scalebbox",Nil}};
