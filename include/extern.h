/* FILE: extern.h */

#ifndef EXTERN_H
#define EXTERN_H

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*    Here Lie All Extern Definitions For All the Cutplane Routines, RIP  */
/*                                                                        */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <stdio.h>
#include <setjmp.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

#include "cutplane.h"
#include "controls.h"
#include "actions.h"
#include "grafx.h"
#include "cursor.h"
#include "cpl_cutplane.h"
#include "rotatetool.h"
#include "text.h"
#include "glove.h"
#ifdef IRIS
#include "polhemus.h"
#endif
#include "primitives.h"

/****** externs for clip.c routines (only used on the sun) *****/

extern void clip();

/****** externs for debug.c routines *****/

extern FILE *dbgdump;
extern void init_dbgdump(char *dbgdump_filename);
extern void close_dbgdump(void);

extern void printscreenbbox(bboxscreenptr thebbox);
extern void print_veelist(listptr);
extern void print_coincidents(listptr, listptr);
extern void printnbrhood(listptr);
extern void printnbrhoods(listptr);
extern void printvert(vertype vert);
extern void printevf(evfptr thisevf);
extern void printevfs(shellptr thishell);
extern void printfaceles(fveptr thisfve);
extern void printshell_les(shellptr thishell);
extern void printface_evfs(fveptr thisfve);
extern void printfves(shellptr thishell);
extern void printvfefaces(vfeptr thisvfe);
extern void printvfe(vfeptr thisvfe);
extern void printvfes(shellptr thishell);
extern void printmatrix(GLfloat ** thematrix);
extern void printfnorms(listptr thisfvelist);
extern void print_props(featureptr thisfeature);
extern void check_all_nbrs(shellptr thishell);
extern void printneighbors(vfeptr thisvfe);
extern void print_face_coords(fveptr thisfve);
extern void print_face(fveptr thisfve);
extern void print_faces(shellptr thishell);
extern void printle_coords(leptr thisle);
extern void print_le(leptr thisle);
extern void old_print_evfs(listptr evflist);
extern void print_evfs(listptr evflist);
extern void print_sector(sectorptr);
extern void print_sectorlist(listptr);
extern void print_sectorpair(sectorpairptr);
extern void print_sectorpairlist(listptr);
extern Boolean topology_check(shellptr);
extern Boolean topology_check_shells(listptr);
extern int count_obj_polys(objptr thisobj);
extern int count_objtree_polys(objptr thisobj);
extern int count_world_polys(worldptr world);
extern void dump_scene(void);

/****** externs for deform.c routines *****/


extern vertype ppiped[];
extern void deform(objptr control_obj);

/****** externs for error.c routines *****/

extern void system_error(char *errorstr);
extern void system_warning(char *errorstr);
#ifndef WIN32
#ifdef IRIS
extern void error_handler(int thesignal, int code);
#else
extern void error_handler(int thesignal);
#endif
#endif
extern void init_error_handling(void);
extern void restart_3form(void);

/****** externs for split.c routines *****/

extern void splitup_edge(evfptr thisevf,int numsplits);
extern void splitup_edges(shellptr thishell,int numsplits);

extern leptr split_face_at_vert(fveptr splitfve,vertype splitvert);
extern void fan_split_fve_fromle(fveptr thisfve, leptr apexle);
extern void fan_split_fve(fveptr thisfve);
extern void zigzag_split_fve(fveptr thisfve);
extern void simple_face_split(fveptr thisfve, leptr apexle);

extern void complete_triangularization(fveptr thisfve);
extern leptr find_closest_vertex(leptr testle, fveptr thisfve);
extern void find_delauney_edge(fveptr thisfve);
extern void triangularize_fve(fveptr thefve);
extern void triangularize_shell(shellptr thishell);

extern Boolean planarize_face(fveptr,leptr);
extern void planarize_affected_fves(featureptr thisfeature, vertype deltapos);
extern void planarize_shell(shellptr thishell);
extern void planarize_object(objptr thisobj);
extern void recombine_affected_coplanar_fves(featureptr thefeature);


/****** externs for duplicate.c routines *****/


extern objptr duplicate_obj(objptr thisobj);
extern objptr duplicate_obj_branch(objptr thisobj);
extern objptr duplicate_and_translate_obj(objptr thisobj, vertype offset);


/****** externs for mirror.c routines *****/


extern void mirror_obj(worldptr world, objptr mirror_obj, fveptr mirror_fve,
		       stateptr state);

/****** externs for scale.c routines *****/

extern void calculate_scale_factors(vertype scale_center,vertype oldposition,
				    vertype newposition, vertype scalefactor);
extern void scale_feature(featureptr thisfeature, vertype scalefactor,
			  vertype scaleorigin);
extern void create_scalebbox(worldptr world);
extern void create_sizebbox(worldptr world);

/****** externs for boolprep.c routines *****/


extern void boolean_preprocess(shellptr shellA,shellptr shellB,
			       listptr vv_list,listptr vf_list);


/****** externs for boolean.c routines *****/


extern listptr do_boolean(objptr objA, objptr objB, int operation);
extern void init_facepair_lists(facepairptr thisfacepair) ;
extern void copy_facepair_elem(facepairptr sourcefacepair, facepairptr destfacepair);
extern void delete_facepair_lists(facepairptr thisfacepair);

/****** externs for globals.c variables *****/
extern long drawflags;

extern char execfilename[];
#ifndef WIN32
extern sigjmp_buf error_return;
#endif

extern unsigned short cutplane_time;
extern int zdrawstate;

extern listptr world_list;
extern worldptr scratch_world;
extern worldptr environment_world,primary_world,help_world,working_world;

extern objptr cursor_obj,cutplane_obj,room_obj,rotatetool_obj,systemcursor_obj;
extern objptr group_obj,scalebbox_obj,room_backwall_obj,line_obj;

extern Coord cursor_move_tolerance;
extern Coord cursor_accel_tolerance;

extern vertype x_axis,y_axis,z_axis,rotx_axis,roty_axis,rotz_axis,origin;
extern Coord rotatetool_radius;
extern vertype rotatetool_dock;
extern vertype cursor_delta;

extern int screensizex,screensizey;
extern long mainwindow,helpwindow,infowindow,slidewindow;
extern int mainwindowleft,mainwindowright,mainwindowtop,mainwindowbottom;
extern int helpwindowleft,helpwindowright,helpwindowtop,helpwindowbottom;
extern int infowindowleft,infowindowright,infowindowtop,infowindowbottom;

extern Matrix imatrix,projmatrix;

/* In the future we may need multiple shell lists for multiple rooms, */
/* requester windows, etc. */

extern stateptr state;		/* current state of all input devices */
extern stateptr oldstate;	/* state of all input devices at last refresh */
extern Boolean select_display_on; /* used by get_selection,draw_one_cutevf */
extern Boolean exit_system;	/* Set to true when user wants to leave system*/
extern Boolean use_zbuffer;	/* Can be set to FALSE by IRIS users. */
extern short use_hidden_line_technique; /* Set to >0  during */
					  /* rots w/wireframe */
extern Boolean rotate_this_refresh; /* Set to true if ANYTHING rotated */
				/* during current refresh loop */
extern Boolean set_backface_flags; /* becomes TRUE when view rotates */

extern Coord nearclipdist,farclipdist;
extern double unitsperpixel[2];

/* Used by pc to set main window size and control cursor */
extern int pc_winWidth, pc_winHeight;
extern int pc_winLeft, pc_winTop, pc_winRight, pc_winBottom;
extern int pc_xMargin, pc_yMargin;
extern int pc_innerLeft, pc_innerTop, pc_innerRight, pc_innerBottom;
extern float pc_windowMargin;

extern int global_refresh;	/* > 0 as long as total redraw necessary */

extern char cursor_string[];
extern char cursor_delta_string[];
extern char cursor_angle_string[];

extern Boolean using_polhemus; /* whether the polhemus is in use */
extern Boolean using_glove;	/* whether jim's glove is in use */
extern Boolean using_slideshow; /* whether the microsoft slide previewer is */
				/* in use */
extern Boolean stocks_demo_on;/* whether the stocks demo is running */

extern colortype black;
extern colortype red;
extern colortype green;
extern colortype blue;
extern colortype cyan;
extern colortype magenta;
extern colortype yellow;
extern colortype white;

extern colortype lightgrey;
extern colortype darkgrey;
extern colortype mediumgrey;
extern colortype darkred;
extern colortype darkblue;
extern colortype darkcyan;
extern colortype mediumyellow;
extern colortype darkyellow;

extern colortype pink;
extern colortype bluegrey;
extern colortype darkbrown;
extern colortype brown;
extern colortype mediumbrown;
extern colortype lightgreen;
extern colortype orange;
extern colortype lightblue;
extern colortype dkbluegrey;

extern colortype backgroundcolor;
extern colortype cutplanecolor;
extern colortype crosshaircolor;
extern colortype selectablecolor;
extern colortype pickedcolor;
extern colortype pickedcontourcolor;

extern colortype zbuffershellcolor;
extern colortype shadowcolor;
extern colortype rotatetoolcolor;
extern colortype sectionlinecolor;
extern colortype pickboxcolor;

extern property noproperty_prop;
extern property nullfeature_prop;
extern property touched_prop;
extern property killed_prop;
extern property quickupdate_prop;
extern property regularupdate_prop;
extern property thislocation_prop;
extern property nextlocation_prop;
extern property msgpending_prop;

extern property translated_prop;
extern property selectable_prop;
extern property nonselectable_prop;
extern property picked_prop;
extern property grabbed_prop;

extern property transparent_prop;
extern property neverdraw_prop;
extern property alwaysdraw_prop;
extern property nonmanifold_prop;
extern property sectioninvisible_prop;
extern property selectinvisible_prop;
extern property pickedinvisible_prop;
extern property lockedgeometry_prop;
extern property connection_prop;
extern property copyproperty_prop;
extern property forcedcpnormal_prop;
extern property hasdecal_prop;
extern property haslabel_prop;
extern property hasdimension_prop;
extern property background_prop;

extern property visible_prop;
extern property invisible_prop;
extern property hideable_prop;
extern property luminous_prop;
extern property noshadow_prop;

extern property tool_prop;
extern property menu_prop;
extern property toolbox_prop;
extern property active_prop;
extern property activemenu_prop;
extern property hotspot_prop;
extern property oriented_prop;
extern property dragonly_prop;
extern property goodies_prop;
extern property primitive_prop;

extern property draw_prop;
extern property erase_prop;
extern property sketch_prop;
extern property colorbar_prop;
extern property paint_prop;
extern property drill_prop;
extern property slice_prop;
extern property glue_prop;
extern property stamp_prop;
extern property magnetic_prop;
extern property zoom_prop;
extern property group_prop;
extern property deform_prop;
extern property puncture_prop;
extern property lightbulb_prop;

extern property toolbar_prop;
extern property toolpad_prop;
extern property toolvisible_prop;
extern property headpad_prop;
extern property padhandle_prop;
extern property dialog_prop;
extern property textinput_prop;
extern property inputplate_prop;
extern property scalebbox_prop;

extern property next_available_property;

extern propinfotype propinfo[];


/****** externs for geometry.c routines *****/


extern leptr find_noncollinear_le(leptr ref_le, int direction);
extern void swap_ints(int *,int *);
extern void swap_coords(Coord *,Coord *);
extern void swap_verts(vertype pt1,vertype pt2);
extern Boolean pt_near_cutplane(vertype pt1,  Coord zcut, float tol);
extern Boolean pt_near_pt(vertype sourcept,vertype checkpt,float toldist);
extern Boolean pt_near_line(vertype pt0,vertype pt1,vertype chkpt,
			    vertype nearpt,float toldist);
extern Boolean pt_near_edge(vertype, elemptr *, vertype, listptr);

extern void copy_obj_bbox(objptr target_obj,bbox3dptr sourcebbox);
extern void copy_group_bbox(objptr target_obj,bbox3dptr sourcebbox);
extern void compute_screen_bbox(bbox3dptr srcbbox,bboxscreenptr destbbox);
extern void compute_object_screenbbox(bbox3dptr solid_bbox,
				      bboxscreenptr objectscreenbbox,
				      Coord object_margin);
extern void compute_shadow_screenbbox(bbox3dptr solid_bbox,
				      bboxscreenptr shadowscreenbbox,
				      Coord shadow_margin);
extern void find_edge_bbox3d(float *pt1, float *pt2, bbox3dptr thebbox);
extern Boolean bbox_intersect3d(bbox3dptr bbox1, bbox3dptr bbox2);
extern Boolean bbox_intersect_screen(bboxscreenptr bbox1,bboxscreenptr bbox2);

extern Boolean point_in_screenbbox(screenpos checkpt,bboxscreenptr thisbbox);
extern Boolean point_in_2dbbox(vertype checkpt,bbox2dptr thisbbox);
extern Boolean point_in_3dbbox(vertype checkpt,bbox3dptr thisbbox);
extern unsigned int int_ray_edge(vertype facept1,vertype facept2,
				 vertype raypt,int rayaxis, int axis2);
extern Boolean point_in_bound(boundptr thisbound, vertype thisvert);
extern Boolean pt_in_face(fveptr thisfve,vertype pt1);
extern Boolean point_in_face(fveptr thisfve, vertype thisvert);
extern Boolean pt_in_shell(shellptr thishell,  vertype thept,  stateptr state);
extern Boolean bound_in_bound(boundptr bound1,boundptr bound2);
extern int face_in_bounds(shellptr new_shell,  boundptr *tight_bound,
			  boundptr *loose_bound,listptr drawshellist);

extern double pt_to_line_dist_3d(vertype testpt,vertype pt1,vertype pt2);
extern double pt_to_planedist_3d(vertype testpt,vertype plane,
				 double planedist);
extern double angle_between_planes(vertype plane1,vertype plane2);
extern double line_to_line_dist_3d(vertype pt1,vertype pt2,
				   vertype pt3,vertype pt4);
extern Boolean pt_near_pt_3d(vertype pt1,vertype pt2, double tol);
extern Boolean pt_near_line_3d(vertype pt1,vertype pt2,vertype testpt,
			       vertype nearpt, double *t, double tol);
extern Boolean pt_near_lineseg_3d(vertype pt1,vertype pt2,vertype testpt,
				  vertype nearpt,double *t,double tol);
extern Boolean pt_on_lineseg_3d(vertype pt1,vertype pt2,vertype testpt);
extern Boolean pt_near_plane_3d(vertype testpt,vertype planenorm,
				Coord planedist);
extern int line_intersect_plane_3d(vertype pt1,vertype pt2,
				   vertype plane,Coord planedist,
				   vertype intrpt,double *t);

extern int lineseg_intersect_plane_3d(vertype pt1,vertype pt2,
				      vertype plane,Coord planedist,
				      vertype intrpt,double *t);

extern int lines_intersect_3d(vertype pt1,vertype pt2,vertype pt3,vertype pt4,
			      vertype intrpt,double *t1,double *t2,double tol);
extern Boolean lineseg_bbox_intersect(vertype pt1,vertype pt2,
				      vertype pt3,vertype pt4);
extern void set_3d_objbbox(objptr thisobj);

extern bbox3dptr grow_group_bbox(objptr thisobj);
/*
extern void grow_group_bbox(objptr thisobj);
*/
extern int linesegs_intersect_3d(vertype pt1,vertype pt2,
				 vertype pt3,vertype pt4,
				 vertype intrpt,double *t1,double *t2,
				 double tol);
extern Boolean planes_intersect(vertype plane1,Coord plane1dist,
				vertype plane2,Coord plane2dist,
				vertype sectvect,vertype sectpt,
				double tol);
extern int pt_in_bound_3d(vertype testpt, boundptr thebound, double tol);
extern int pt_in_face_3d(vertype testpt, fveptr theface, double tol);
extern Boolean edge_in_face3d(float *pt1, float *pt2, fveptr thisfve);

extern int lineseg_int_face_3d(vertype pt1,vertype pt2,fveptr theface,
			       vertype intrpt, double *t,double tol);
extern Boolean same_bound(boundptr bound1,boundptr bound2,Boolean *reversed);
extern fveptr find_same_face(fveptr thisfve,objptr otherobj,Boolean *reversed);
extern Boolean face_is_planar(fveptr thisfve,Coord giventol);
extern Boolean planar_face_nonormal(fveptr thisfve);
extern Boolean face_is_concave(fveptr thisfve);
extern void compute_face_centroid(fveptr, vertype);
extern void compute_obj_centroid(objptr, vertype);


/****** externs for grafx.c routines *****/

extern GLuint first_gl_list_id, current_gl_list_id;
extern GLfloat light1Pos[4];

extern Matrix vanillamatrix, invanillamatrix;
extern unsigned long packcolor(colortype thecolor);
extern long lsetnear,lsetfar,lset_offset;

extern int framebuffer;
extern erase_record_type erase_records[2][2];
extern erase_record_ptr old_erase_records[2],new_erase_records[2];

extern bboxscreenptr find_cutplane_screenbbox(void);

extern void unpackcolor(unsigned long packedcolor, colortype unpackedcolor);
extern void copycolor(colortype sourcecolor, colortype destcolor);
extern void scalecolor(colortype thecolor, float scalefactor, colortype result);
extern void addcolors(colortype color1, colortype color2, colortype resultcolor);
extern void multcolors(colortype color1, colortype color2,
		       colortype resultcolor);
extern void toggle_zbuffering(void);
extern void normal_zbuffer(void);
extern Boolean backface_fve(fveptr);
extern Boolean edge_is_hidden(evfptr thisevf, Boolean fve1_facesback,
			      Boolean fve2_facesback);

extern void def_environment_colors(void);
extern void init_projmatrix(void);
extern void map2screen(vertype thept,screenpos screenpt,Boolean compute_mapping);

extern void object_defs(void);

#ifdef IRIS
extern void background_defs(void);
#endif /* IRIS */

extern void set_cutplanehatch_color(void);
extern void update_world(stateptr state, stateptr oldstate);

extern void create_stencil(vertype pt1,vertype pt2,int fudge);

extern void nodraw_technique(objptr thisobj, stateptr state);
extern void draw_marquee_technique(objptr marquee,stateptr state);
extern void draw_scribble_technique(objptr scribble,stateptr state);

extern void create_gl_list_ids(void);
extern GLuint create_decal_globj(objptr decal_obj,
				 vertype to_origin,vertype scale_amt,
				 vertype decal_offset);
extern void draw_rotate_icon(stateptr state);
extern void draw_pad_technique(objptr pad,stateptr state);
extern void draw_toolpad_technique(objptr pad, stateptr state);
extern void draw_headpad_technique(objptr pad, stateptr state);
extern void draw_undertab_technique(objptr pad, stateptr state);
extern void draw_plate_technique(objptr pad, stateptr state);
extern void draw_padhandle_technique(objptr padhandle, stateptr state);
extern void draw_rotate_innercircle(objptr thisobj,stateptr state);
extern void draw_innercircle_select_box(objptr thisobj,stateptr state);
extern void draw_rotate_outercircle(objptr thisobj,stateptr state);
extern void draw_rotatexbar(objptr thisobj,stateptr state);
extern void draw_big_circular_selectbox(objptr thisobj,stateptr state);

extern void draw_obj_contour(objptr thisobj,Coord contouroffset,
			     colortype thecolor);
extern void draw_shell_intersection(shellptr thishell);
extern void draw_edge(evfptr thisevf, colortype thiscolor);
extern void draw_shadowed_edge(evfptr thisevf, colortype thiscolor);
extern void draw_wireframe_face(fveptr thisfve, colortype thiscolor);
extern void draw_shadowed_wireframe_face(fveptr thisfve, colortype thiscolor);
extern void draw_wireframe_shell(shellptr thishell, colortype thiscolor);
extern void draw_shadowed_wireframe_shell(shellptr thishell, colortype thiscolor);
extern void draw_wireframe_shells(listptr shell_list, colortype thiscolor);
extern void draw_shadowed_wireframe_shells(listptr shell_list,
					    colortype thiscolor);
extern void draw_hidden_line_shell(shellptr thishell, colortype thiscolor);
extern void draw_solid_face(fveptr thisfve, colortype thiscolor);
extern void draw_shadowed_hidden_line_shell(shellptr thishell,colortype thiscolor);
extern void draw_shell_shadow(shellptr thishell);
extern void draw_shadows(worldptr world);
extern void draw_pick_box(vertype drawpt, int pick_linewidth,Boolean filled);
extern void draw_shadowed_pick_box(vertype drawpt, colortype boxcolor,
				   Boolean filled);
extern void draw_select_box(shellptr thishell);
extern void draw_big_circular_selectbox(objptr thisobj, stateptr state);
extern void draw_normal_objects_select_box(objptr selectobj);
extern void draw_scalebbox_technique(objptr thisobj,stateptr state);
extern void draw_pick_bbox(objptr thisobj,stateptr state);
extern void normal_draw_technique(objptr thisobj, stateptr state);
extern void draw_world_shell(shellptr thishell, stateptr state);
extern void draw_world_objects(worldptr world, Boolean draw_everything,
			       stateptr state);
extern void draw_worlds(Boolean draw_everything);

extern void set_camera_view(vertype direction, vertype eyept);

/* Added for win port */
extern void clear_view(void);

extern void init_saved_blits();
extern Boolean only_cursor_changed(objptr cursor_obj,int framebuffer);
extern Boolean saved_blit_valid(int framebuffer);
extern void zbuffer_writing_off();
extern void zbuffer_writing_on();

extern void draw_erase_bboxes(int framebuffer);
extern void draw_cutplane(void);
extern void draw_cutplane_pickedrect(void);
extern void draw_crosshair(void);
extern void draw_underlying_objects(stateptr state,stateptr oldstate);
extern void draw_gl_objects(stateptr state);

extern void draw_help_window(Boolean,Boolean,Boolean);
extern void draw_info_window(stateptr state);

/* hack debug routine */
extern void display_cursor_move(stateptr state,vertype cursor_delta);

extern void grafx_resize(int main_winWidth, int main_winHeight);

  // FIX ME!!! LJE
// extern void redraw_grafx(HDC hDC);

/****** externs for cursor.c routines *****/

extern blitsavetype saved_blits[];
extern vertype default_cursor_offset;

extern void select_cursor(cursorkind new_cursorkind);
extern void do_cursor_deselect(listptr worldlist);
extern void notify_cursor_pause(void);
extern void do_cursor_notifications(void);

extern void draw_hidden_cursor(objptr cursor_obj);
extern void get_normal_cursor_blit(objptr cursor_obj,int framebuffer,
				   stateptr state);
extern void restore_normal_cursor_blit(int framebuffer);
extern void draw_plus_cursor(objptr cursor_obj);
extern void get_normal_cursor_blit(objptr cursor_obj,int framebuffer,
				   stateptr state);
extern void restore_normal_cursor_blit(int framebuffer);
extern void draw_arrow_cursor(objptr cursor_obj);
extern void get_normal_cursor_blit(objptr cursor_obj,int framebuffer,
				   stateptr state);
extern void restore_normal_cursor_blit(int framebuffer);
extern void draw_ibeam_cursor(objptr cursor_obj);
extern void get_normal_cursor_blit(objptr cursor_obj,int framebuffer,
				   stateptr state);
extern void restore_normal_cursor_blit(int framebuffer);
extern void draw_xhair_cursor(objptr cursor_obj);
extern void get_xhair_cursor_blit(objptr cursor_obj,int framebuffer,
				  stateptr state);
extern void restore_xhair_cursor_blit(int framebuffer);
extern void draw_cursor(objptr cursor_obj);
extern void restore_from_saved_blit(int framebuffer);
extern void get_saved_blit(objptr cursor_obj,int framebuffer,stateptr state);
extern bboxscreenptr find_normal_cursor_screenbbox(objptr cursor_obj);
extern void offset_cursor_image(vertype);
extern void unoffset_cursor_image(vertype);
extern void plant_cursor_image(void);

/****** externs for text.c routines *****/

extern char textstring_styles[Maxstringstyle][Maxstringstylelength];
extern Coord text_string_width(char *thestring,textfontype thefont,
			       Coord string_x_scale);
extern textstringptr create_text_string(char *new_string,
					vertype string_offset,
					textfontype initial_font,
					Coord string_x_scale,
					Coord string_y_scale,
					Coord string_pen_width,
					colortype string_color,
					stringstyletype string_style);
extern textstringptr find_label_item(objptr labelled_obj,int item_number);
extern char *get_label_item(objptr labelled_obj,int item_number);
extern void set_label_item(objptr labelled_obj,int item_number,char *new_string);
extern void set_label_item_style(objptr labelled_obj,int item_number,
				  stringstyletype new_style);
extern void set_label_item_offset(objptr labelled_obj,int item_number,
				  vertype new_offset);
extern void set_label_item_scale(objptr labelled_obj,int item_number,
				 Coord new_x_scale,Coord new_y_scale);
extern void set_label_item_pen_width(objptr labelled_obj,int item_number,
				     int new_pen_width);
extern void set_label_item_color(objptr labelled_obj,int item_number,
				 colortype new_color);
extern void set_label_item_insertion_point(objptr labelled_obj,int item_number,
					   int new_insertion_point);
extern void move_label_insertion_point(objptr labelled_obj,int item_number,
				       int offset);
extern void insert_string_into_label(objptr labelled_obj,int item_number,
				     char *insertstring);
extern void remove_chars_from_label(objptr labelled_obj,int item_number,
				    int startindex,int endindex);
extern void add_label_item(objptr labelled_obj,char *initial_string,
			   vertype initialoffset,textfontype initial_font);
extern void delete_label_item(objptr labelled_obj,int item_number);
extern textgrouptr create_label(objptr labelled_obj,char *initial_string,
				vertype initialoffset, textfontype initial_font);

extern void draw_text_string(textstringptr textstring);
extern void draw_obj_label(objptr labelled_object);

extern void objstr(char *thestring,vertype pos,Coord xscale,Coord yscale);


/****** externs for hershey.c routines *****/

extern char_rec char_lookups[];

extern Coord hershey_Xheight();
extern Coord hershey_descender_height();
extern Coord hershey_height();

/****** externs for init.c routines *****/


extern void qbuttons(void);
extern void unqbuttons(void);
extern void init_3form(void);
extern void setup_light_source(int lightnumber, vertype lightpos,
			       colortype lightcolor);
// FIX ME -- LJE
// extern void setupPixelFormat(HDC hDC);
extern void init_grafx(void);


/****** externs for intersect.c routines *****/


extern void set_obj_evf_eqns(objptr thisobj);
extern void set_shell_evf_cutpts(shellptr, stateptr);
extern void set_obj_evf_cutpts(objptr thisobj, stateptr state);
extern void make_obj_ringlist(objptr thisobj);
extern void clear_obj_ringlist(objptr thisobj);
extern Boolean must_make_obj_ringlist(objptr, stateptr, stateptr);

/***** externs for list.c routines *****/


#ifndef FAST
extern elemptr next_elem(listptr, elemptr);
extern elemptr prev_elem(listptr, elemptr);
#endif
extern int get_elem_index(elemptr, listptr);
extern elemptr get_elem_ptr(int, listptr);

extern void add_elem(listptr, elemptr, elemptr);
extern void insert_elem(listptr, elemptr, elemptr);


extern void append_elem(listptr, elemptr);
extern void rem_elem(listptr, elemptr);
extern void move_elem(listptr, elemptr, elemptr);

extern elemptr alloc_elem(int);
extern elemptr create_blank_elem(int);
extern elemptr add_new_blank_elem(listptr, elemptr, int);
extern elemptr insert_new_blank_elem(listptr, elemptr, int);
extern elemptr append_new_blank_elem(listptr, int);
extern void copy_elem(elemptr,elemptr);
extern elemptr clone_elem(elemptr);

extern void set_elem_fields();	/* no proto here since variable arg list */
extern elemptr create_elem();	/* no proto here since variable arg list */
extern elemptr add_new_elem();  /* no proto here since variable arg list */
extern elemptr insert_new_elem(); /* no proto here since variable arg list */
extern elemptr append_new_elem(); /* no proto here since variable arg list */
extern Boolean already_in_featurelist(listptr check_list,
				      featureptr check_feature);
extern void add_to_featurelist(listptr add_list,featureptr new_feature);
extern void merge_lists_uniquely(listptr list1,listptr list2);
extern void remove_from_featurelist(listptr remove_list,
				    featureptr removed_feature);
extern void free_elem(elemptr *);
extern void del_elem(listptr, void *);
extern void kill_elem(void *);

extern listptr create_list(void);
extern listptr create_circular_list(void);
extern void clear_list(listptr);
extern void del_list(listptr);
extern listptr transfer_list_contents(listptr, listptr);
extern listptr merge_lists(listptr, listptr);
extern listptr clone_list(listptr);
extern void copy_list(listptr,listptr);
extern listptr append_list(listptr, listptr);
extern Boolean elem_has_proplist(int);

extern Boolean elem_qtest(listptr);
extern void queue_elem(listptr,elemptr);
extern elemptr unqueue_elem(listptr);


/***** externs for math3d.c routines ****/


extern int sign(int);
extern float fsign(float);
extern int iround(float);
extern float fround(float);
extern int compare(double,double,double tol);

extern void setscreenpos(screenpos pos,int x, int y);
extern Boolean pos_equal(vertype v1, vertype v2);
extern void setpos3d(float *, Coord, Coord, Coord);
extern void copypos3d(register vertype, register vertype);
extern void addpos3d(register vertype pos, register vertype delta,
		     vertype result);
extern void diffpos3d(vertype pt1, vertype pt2, vertype result);

extern void setvec3d(vertype vec, Coord x, Coord y, Coord z);
extern void makevec3d(vertype, vertype, vertype);
extern void copyvec3d(vertype v1,  vertype v2);
extern void addvec3d(vertype, vertype, vertype);
extern void diffvec3d(vertype, vertype, vertype);
extern void multvec3d(vertype, vertype, vertype);
extern void scalevec3d(vertype, float, vertype);

extern Coord dotprod(Coord, Coord, Coord,
		     Coord, Coord, Coord);
extern Coord dotvecs(vertype, vertype);
extern void cross_prod(float *, float *, float *);
extern Coord mag(Coord, Coord, Coord);
extern Coord magvec(vertype);
extern void flip_vec(vertype);
extern void force_vec_up(vertype, int);
extern Boolean vector_near_zero(float *thevec, double zerotol);
extern Boolean vec_eq_zero(float *);
extern Coord distance(vertype, vertype);
extern Coord distance2(vertype, vertype);
extern void norm2pt(vertype p1, vertype p2, vertype p3);
extern void normalize(vertype, vertype);
extern void midpt(Coord, Coord, Coord, Coord, Coord, Coord,
		  Coord *, Coord *, Coord *);
extern void midpoint(vertype, vertype, vertype);
extern void transpose_matrix(Matrix thematrix, Matrix thetranspose);
extern void copy_matrix(Matrix source, Matrix dest);
extern void multmatrix_2x2(Matrix, Matrix, Matrix);
extern Boolean invertmatrix_2x2(Matrix m, Matrix minv);
extern void multmatrix_4x4(Matrix, Matrix, Matrix);
extern Boolean matrices_equal(Matrix a,Matrix b,double tol);
extern void gaussian_elim(Matrix);

/****** externs for normals.c routines *****/


extern Boolean cclockwise_bound(boundptr thisbound);
extern Boolean cclockwise(fveptr thisfve);
extern void find_obj_normals(objptr thisobj);
extern void find_branch_normals(objptr thisobj);
extern void find_normals(listptr shell_list);
extern void compute_planedist(fveptr);
extern void compute_obj_planedists(objptr);
extern void find_face_plane_dists(shellptr);
extern void invert_shell(shellptr);


/****** externs for objtree.c routines *****/

extern void insert_parent(objptr parent,objptr child);
extern void add_child(objptr parent, objptr child);
extern objptr remove_obj(objptr removed_obj);
extern void adopt_obj_branch(objptr master,objptr slave,
			      Boolean linked);
extern void adopt_obj(objptr master,objptr slave,Boolean linked);
extern void get_obj_shells(objptr this_obj,listptr cull_list);
extern void get_descendents(objptr this_obj,objptr mother_obj,
			    listptr cull_list);
extern objptr get_unlinked_ancestor(objptr thisobj);
extern void apply_to_descendants(objptr thisobj, void (*thefunction)(),
				 void *function_data);
extern void apply_to_linked_descendants(objptr thisobj, void (*thefunction)(),
				 void *function_data);
extern void apply_to_descendants_with_prop(objptr thisobj, property chosen_prop,
					   void (*thefunction)(),
					   void *function_data);

/****** externs for pencilsketch.c routines *****/


extern void update_sketch_mode(listptr,colortype,stateptr);
extern void sketch_action(stateptr,actionptr new_action);
extern void enter_sketch_mode(void);
extern void exit_sketch_mode(listptr);
extern Boolean sketch_mode_on(void);


/****** externs for pick.c routines *****/


extern void pick_feature(featureptr pickfeature);
extern void unpick_feature(featureptr pickfeature);

extern void boolean_op_picked_shells(worldptr world, stateptr state);


/****** externs for primitives.c routines *****/

extern prim_create_rec prim_create_recs[];

extern objptr create_isosceles_triangle(worldptr world, Coord trianglesize);
extern objptr create_isosceles_prism(worldptr world, Coord trianglesize,
				     Coord triangledepth);
extern objptr create_cubehole(worldptr world);
extern objptr create_square(worldptr world,Coord width,Coord height);
extern objptr create_cube(worldptr world, Coord width, Coord height,
			  Coord depth);
extern objptr recreate_cube(objptr oldcube,Coord length,Coord width,
			    Coord height);
extern objptr create_pyramid(worldptr,Coord width,Coord height,Coord depth);
extern objptr create_sphere(worldptr world,double radius,
			    int numincrements);
extern objptr create_oval(worldptr world,Coord radius_a,Coord radius_b,
			  int numincrements);
extern objptr create_oval_cylinder(worldptr world,
				   Coord radius_a,Coord radius_b,
				   Coord depth,
				   int numincrements);
extern objptr create_oval_cone(worldptr world,Coord radius_a,
			       Coord radius_b, Coord depth,int numincrements);
extern objptr create_paraboloid(worldptr world,Coord width,Coord height,
				Coord thickness,int numpoints);
extern objptr create_lbeam(worldptr world,Coord width, Coord height,
			   Coord thickness,Coord depth);
extern objptr create_donut(worldptr world,Coord radius,int numincrements);
extern objptr create_table(worldptr world, Coord width, Coord height,
			   Coord depth);
extern objptr create_vase(worldptr world, double xscale, double yscale,
			  int numfaces);



/****** externs for properties.c routines *****/


extern void init_global_properties(void);
extern void init_proplists(listptr * proplist_array);

extern Boolean has_property(featureptr, property);
extern featureptr get_property_feature(worldptr,property,int);
extern alltypes get_property_val(elemptr, property);
extern void add_property(featureptr, property);
extern void set_property();	/* no proto because arg type varies */
extern void del_property(featureptr, property);
extern void clear_property(property);
extern int find_property(char *property_name);
extern int define_new_property(char *property_name,
				CPL_word_ptr prop_code_address);
extern void define_property(char *property_name,CPL_word_ptr prop_code_address);
extern void clear_feature_properties(featureptr);
extern void transfer_feature_properties(featureptr,featureptr);
extern void copy_feature_properties(featureptr,featureptr);
extern void add_property_to_descendants(featureptr, property);
extern void del_property_from_descendants(featureptr, property);

/****** externs for rotateobj.c routines *****/


extern void make_axisrot_matrix(vertype rotvec,vertype rotpt,int rotangle,
				Matrix axisrotmat,Matrix invaxisrotmat);
extern void axis_rot_obj(objptr, vertype, vertype, int);
extern void rotate_features(stateptr, stateptr, listptr);
extern void inplane_axis_rot_obj(objptr, vertype, vertype, int);
extern int snap_face_to_plane(vertype,vertype,int);
extern int snap_vector_to_vector(vertype,vertype, vertype);


/****** externs for rotatetool.c routines *****/


extern void create_rotatetool(worldptr world);
extern bboxscreenptr find_rotatetool_screenbbox(void);
extern void attach_rotatetool_CPLcode(void);
extern void CPL_snap_rotatetool(listptr world_list,stateptr state);
extern void CPL_setup_rotatetool(listptr world_list,stateptr state);
extern void CPL_xbar_rotate(listptr world_list,stateptr state);
extern void CPL_bauble_rotate(listptr world_list,stateptr state);


/****** externs for translate.c routines *****/
extern void translatevert(register vfeptr thisvfe,Matrix the_invxform,
			  register vertype delta);
extern void clear_vertranslate(vfeptr thisvfe);
extern void clear_shelltranslates(shellptr thishell);
extern void clear_feature_translates(featureptr thisfeature);
extern void translate_vfe_core(vfeptr thevert, shellptr mothershell,
			       vertype delta, Boolean useflags);
extern void translate_vfe(vfeptr,vertype delta,Boolean useflags);
extern void translate_evf(evfptr,vertype delta,Boolean useflags);
extern void translate_fve(fveptr,vertype delta,Boolean useflags);
extern void translate_shell(shellptr thishell,vertype delta,Boolean useflags);
extern void translate_obj(objptr thisobj, vertype delta);
extern void translate_obj_quick(objptr thisobj,vertype delta);

extern void translate_feature(featureptr thisfeature,vertype delta,
			      Boolean useflags);
extern void clear_property_translates(property thisproperty);


/****** externs for the select.c routines *****/


extern Coord global_picksize;
extern Boolean always_set_selectable;

extern Boolean cursor_in_ring(ringptr thisring,stateptr state);

extern void get_intersect_pts(segmentptr thisegment, float **pt1,float **pt2);

extern void set_object_selectable(objptr thisobj, stateptr state);
extern void set_feature_selectable(featureptr thisfeature, stateptr state);

extern void set_oneworld_selectables(worldptr thisworld,stateptr state);
extern void set_selectables(listptr world_list,stateptr state,
			    stateptr oldstate);
extern void set_selectables_definite(listptr world_list,stateptr state,
				     stateptr oldstate);
extern void unselect_obj(objptr unselected_obj);

extern void set_global_picksize(Coord newpicksize);
extern void set_always_selectable(void);
extern void set_cutplane_selectable(void);
extern void set_room_selectable(void);
extern void set_rotate_innercircle_selectable(objptr inner_circle,
					      stateptr state);
extern void set_rotate_outercircle_selectable(objptr outer_circle,
					      stateptr state);
extern void set_rotatexbar_selectable(objptr rotate_xbar,stateptr state);
extern void set_systemcursor_selectable(void);
extern void set_toolpad_selectable(objptr pad, stateptr state);
extern void set_headpad_selectable(objptr pad, stateptr state);
extern void set_normalpad_selectable(objptr pad,stateptr state);
extern void set_scalebbox_selectable(objptr scalebbox,stateptr state);

extern Boolean accelerating(stateptr state,stateptr oldstate);
extern Boolean accelerating_z(stateptr state,stateptr oldstate);

/****** externs for shade.c routines *****/


extern void set_feature_color(featureptr thisfeature, colortype thiscolor);
extern void set_feature_named_color(featureptr thisfeature, char *colorname);
extern void get_feature_color(featureptr thisfeature, colortype thiscolor);
extern void copy_feature_color(featureptr sourcefeature, featureptr destfeature);
extern void get_named_color(char *colorname, colortype colorvals);
extern void init_acos_table(void);
extern void make_face_into_light(fveptr thisfve,double brightness);
extern void compute_reflected_color(vertype,colortype,colortype);
extern void draw_flatshaded_face(fveptr thisfve);
extern void draw_flatshaded_shell(shellptr thishell, stateptr state);
extern void draw_flatshaded_shells(listptr shell_list,stateptr state);
extern void draw_software_shaded_face(fveptr thisfve);
extern void draw_software_shaded_shell(shellptr thishell);
extern void draw_software_shaded_shells(listptr shell_list);


/****** externs for slice.c routines *****/


extern listptr separate_shell_into_pieces(shellptr thishell,listptr faces);
extern void slice_object(objptr thisobj,stateptr state,
			 listptr newfrontobjs,listptr newrearobjs);


/****** externs for storage.c routines *****/



/* eventually, put me in topology.c */
extern void append_evf(listptr the_head,leptr le1ptr,leptr le2ptr,
		       fveptr face1,fveptr face2);
extern Boolean read_data_file(char* infile_name,featureptr parentfeature);
extern void write_data_file(char* outfile_name,featureptr thefeature);
extern void write_dxf_file(char *outfile_name,worldptr world);

extern void read_decal_file(objptr parent, char *decal_filename);

extern objptr read_plate(worldptr world, objptr parent,char *plate_rscfilename);
extern void reset_cutplanetime_flags(objptr thisobj,void *unused_data);
extern void reset_cutplanetime(listptr world_list);

/****** externs for tools.c routines *****/


extern vertype penciloffset,toolholderoffset,pencilvect,pencilvect2;

extern void apply_front_decal(objptr decal_obj,objptr dest_obj,
			      Coord decal_thickness,
			      Coord decal_forward_offset,Coord decal_margin);

extern void read_decal_obj(objptr apply_obj,char *decalobj_filename);
extern objptr create_sketch_tool(worldptr world,vertype offset,
			       char *pencilcolor,char *erasercolor);
extern objptr create_slice_tool(worldptr world,vertype offset,
			       char *bladecolor,char *handlecolor);
extern objptr create_lattice(worldptr world);

extern objptr create_magnet(worldptr world, Coord magnetradius,
			    Coord barthickness, Coord magnetdepth,
			    int magnetresolution,
			    colortype bodycolor, colortype endscolor);

extern objptr create_magnifier(worldptr world, Coord lensradius,
			       Coord holeradius, int lensresolution,
			       colortype lenscolor,Coord handlewidth,
			       Coord handleheight, colortype handlecolor);
extern objptr create_fence(worldptr world,
			   Coord fencewidth,Coord fenceheight,Coord fencedepth,
			   colortype fencecolor);

extern objptr create_storage_pad(worldptr world, colortype storagepadcolor);

extern objptr adjust_toolbar(objptr toolbar);
extern objptr create_toolbar(worldptr world,char *toolpad_rsc_filename);

extern objptr create_color_control(worldptr world, vertype controlctr,
				   Coord colorbasewidth);

extern objptr setup_prims_toolbox(worldptr world);
extern objptr setup_goodies_toolbox(worldptr world);
extern objptr create_piechart(worldptr world, Coord radius,Coord depth,
			      int resolution,objptr chart_plate);

extern objptr setup_piechart_demo(worldptr,Coord,Coord,int,colortype);
extern objptr setup_slidepad(worldptr environment_world);
extern objptr setup_arrowtool_pad(worldptr environment_world);
extern void setup_tools(void);

extern objptr setup_stocks_demo(worldptr world,
				Coord barwidth,Coord barheight,Coord bardepth,
				int numstocks, int timedepth);
extern void update_stocks_demo();

/****** externs for topology.c routines *****/



#ifndef FAST
extern leptr Twin_le(leptr);
#endif
extern leptr new_le(leptr, evfptr, vfeptr, int);
extern leptr del_le(leptr);
extern shellptr make_shell(vertype);
extern objptr make_childobj_noshell(objptr parent);
extern objptr make_parentobj_noshell(objptr child);
extern objptr make_childobj(objptr parent,vertype pos);
extern objptr make_parentobj(objptr child,vertype pos);
extern void move_shell_to_obj(objptr parent, shellptr moved_shell);
extern objptr make_obj_noshell(worldptr world);
extern objptr make_obj(worldptr world,vertype pos);
extern char *get_object_name(objptr named_obj);
extern void set_object_name(objptr named_obj,char *obj_name);
extern void set_world_name(worldptr named_world, char* world_name);
extern void merge_worlds(worldptr sourceworld,worldptr destworld);
extern void clear_world(worldptr world);
extern void kill_obj(objptr deleted_obj);
extern void kill_shell(shellptr);
extern leptr make_edge_vert(leptr, leptr, vertype);
extern void kill_edge_vert(leptr, leptr);
extern fveptr make_edge_face(leptr, leptr);
extern fveptr make_edge_reg_face(leptr, leptr);
extern void kill_edge_face(leptr, leptr);
extern leptr kill_ring_make_edge(leptr, leptr);
extern boundptr kill_edge_make_ring(leptr, leptr);
extern void kill_face_make_loop(fveptr, fveptr);
extern fveptr make_face_kill_loop(boundptr);
extern void move_loop(boundptr, fveptr, int);

extern leptr break_edge_at_pos(leptr thisle, vertype pos);

extern void move_holes(fveptr, fveptr);
extern void extrude_face(fveptr, vertype);
extern void extrude_faces(listptr, vertype);
extern void make_solid_of_rotation(objptr wireobj, vertype rotvec,
				       vertype rotpt, int degree_inc);
extern void combine_loops(fveptr, fveptr, Boolean);
extern void merge_shells(shellptr sourceshell, shellptr destshell);
extern void combine_shells(listptr, shellptr, shellptr);
extern void fix_flipped_bounds(shellptr);
extern void flip_evf(evfptr);
extern void clear_flip_flags(listptr);
extern void tag_evfs_to_flip(boundptr, listptr);
extern void flip_hole_orientation(boundptr);
extern void flip_obj_orientation(objptr);
extern void flip_shell_orientation(shellptr);
extern void glue_objects_along_faces(fveptr, fveptr, Boolean);

extern fveptr delete_vertex(leptr thisle);
extern fveptr delete_edge(leptr thisle);
extern fveptr delete_face(fveptr thisfve);

extern void init_shell_lists(shellptr);
extern void delete_shell_lists(shellptr);
extern void init_shell_defaults(shellptr);
extern void init_fve_lists(fveptr);
extern void delete_fve_lists(fveptr);
extern void init_bound_lists(boundptr);
extern void delete_bound_lists(boundptr);
extern void init_ring_lists(ringptr);
extern void delete_ring_lists(ringptr);
extern void init_obj_lists(objptr newobj);
extern void delete_vv_lists(vvptr);
extern void delete_vf_lists(vfptr);
extern featureptr find_largergrain(featureptr,int);
extern fveptr find_face_given_normal(objptr thisobj, vertype searchnorm);
extern Boolean find_les_same_face(leptr *le1,leptr *le2);
extern Boolean find_les_same_face_same_vfes(leptr *le1,leptr *le2);
extern Boolean find_les_same_face_walking(leptr *le1,leptr *le2);
extern void init_world_lists(worldptr neworld);
extern void init_curve_lists(curveptr newcurve);
extern void init_surf_lists(surfptr newsurf);

extern void delete_obj_lists(objptr thisobj);
extern void delete_world_lists(worldptr thisworld);
extern void delete_curve_lists(curveptr thiscurve);
extern void delete_surf_lists(surfptr thisurf);

extern void copy_shell_elem(shellptr sourceshell, shellptr destshell);
extern void copy_obj_elem(objptr sourceobj, objptr destobj);
extern void copy_world_elem(worldptr sourceworld, worldptr destworld);
extern void copy_ring_elem(ringptr sourcering, ringptr destring);
extern void copy_fve_elem(fveptr sourcefve, fveptr destfve);
extern void copy_bound_elem(boundptr sourcebound, boundptr destbound);
extern void copy_curve_elem(curveptr sourcecurve, curveptr destcurve);
extern void copy_surf_elem(surfptr sourcesurf, surfptr destsurf);

/****** externs for transform.c routines *****/

extern void permanent_transform_obj(objptr thisobj);
extern void transformpt(Coord *thept, Matrix xform, Coord * xformedpt);
extern void transformpts(long npts, Coord *pts, Matrix xform, Coord *xformedpts);
extern void transform_shell(shellptr thishell, Matrix xformatrix);
extern void transform_shell_reverse(shellptr thishell, Matrix invxform);

#ifdef IRIS
extern void transform_shells();
#endif
extern void transform_obj_reverse(objptr thisobj, Matrix invxform);
extern void transform_obj(objptr);
extern void transform_obj_evf_cutpts(objptr);
extern void transform_obj_tree(objptr);

/****** externs for zoom.c routines *****/

extern void zoom_world(worldptr world, vertype zoom_center, Coord scale_factor);
extern Coord zoom_unit(Coord unzoomed_amt);
extern Coord unzoom_unit(Coord zoomed_amt);
extern Coord fetch_system_zoom(void);
extern void set_system_zoom(Coord new_zoom);
extern void totally_unzoom_world(worldptr world);
extern void totally_zoom_world(worldptr world, Matrix xform, Matrix invxform);

/****** externs for user.c routines *****/


extern void switchmousemap(stateptr);
extern void inquire_filename(char *inoutfilename,char*message,
			     char *defaultfile);
extern double inquire_double(char *,double);
extern void inquire_vfe(char *,vfeptr);
extern void inquire_evf(char *,evfptr);
extern Boolean user_input(listptr);
extern void create_nearpts_list(worldptr world,stateptr state);
extern void init_nearpts_list(void);
extern void reset_nearpts_list(void);
extern Boolean snap_plane_to_nearestpt(worldptr world, stateptr state,
				    vertype cursor_move);

/****** externs for controls.c routines *****/

/* For the pc */
extern void scan_pc_buttons(void);
extern void reset_pc_valuator_store(void);
extern void set_pc_valuators(int x, int y);
extern int pc_queue_push(long qentry, short qval);
extern int pc_queue_push_key(int wParam);

extern tiedbuttontype tiedbuttons[];
extern void init_controls(stateptr state);
extern void reset_multiple_clicks(void);
extern Boolean update_tied_buttons(int qentry,stateptr state);
extern void update_nonqueued_tied_buttons(stateptr state);
extern Boolean update_queued_buttons(int *qentry);
extern Boolean update_polled_valuators(stateptr state);
/* this should be in utilities.c or some such */
extern void clear_record(char *therec, int recsize);
extern Boolean button_pressed(tiedbuttonkind thistied);
extern Boolean pick_button_down(void),cutplane_button_down(void),roomrot_button_down(void);
extern void store_cursor_move(vertype cursor_move,Boolean dragging);


/****** externs for polhemus.c routines *****/


extern Boolean polhemus_controldev_update();
extern void encode_polhemus_cursor_move(stateptr state,
					glovestatekind glove_state,
					Boolean dragging);
extern void encode_polhemus_roomrot(stateptr state);
extern void encode_polhemus_transform(stateptr state);
extern void quit_polhemus(void);


/****** externs for glove.c routines *****/


extern glovestatekind glove_state;
extern unsigned char glove_wrist_offset;
extern Boolean update_glove_actions(stateptr state);
extern Boolean glove_in_use();
extern void quit_glove(void);


/****** externs for update.c routines *****/


extern vertype drag_origin;
extern Boolean selectables_set;

#ifdef IRIS
extern void init_updates();
#endif
/* this should be in utilities.c or some such */
extern double enforce_flimits(double fvalue,double fmin, double fmax);
extern int enforce_ilimits(int ivalue,int imin,int imax);
extern long enforce_llimits(long lvalue,long lmin,long lmax);

extern void set_drag_origin(stateptr);
extern void clear_drag_origin(void);
extern Boolean drag_valid(vertype newcursorpos);
extern Boolean first_valid_drag(stateptr state,vertype extra_offset);
extern void set_constrain(void);
extern void clear_constrain(void);

extern void constrain_movecrosshair(vertype delta);
extern void compute_crosshair_move(vertype delta,vertype actualdelta,
			   stateptr state);
extern void zoom_room_view(vertype center,double scale,stateptr state);
extern void rotate_room_view(int xamount, int yamount,stateptr state);
extern void do_cursor_updates(void);
extern void process_updates(worldptr world,stateptr state,stateptr oldstate,
			    property update_prop);
extern void process_global_updates(listptr world_list,stateptr state,
				   stateptr oldstate);
extern void unlog_update(featureptr unlogged_feature);
extern void log_update(featureptr updatefeature,long updateflags,
		       Boolean immediately);
extern void log_property_update(worldptr world,property logprop,long updateflags,
		       Boolean immediately);
extern void log_global_update(long updateflags);
extern Boolean handle_user_events(stateptr state);

extern void new_update_state(listptr world_list,stateptr state,
			     stateptr oldstate);

extern void log_global_refresh(void);
extern void log_feature_redraw(featureptr log_feature);
extern void log_screen_erase_bbox(featureptr log_feature,
				  bboxscreenptr new_screen_bbox,
				  bbox3dptr new_3d_bbox,
				  Boolean log_in_old);
extern void log_3d_erase_bbox(featureptr log_feature,
			      bbox3dptr new_erase_bbox,Coord bboxmargin,
			      Boolean log_in_old);
extern void log_object_erase_bbox(objptr erase_obj,Boolean use_objbbox,
				  Boolean log_in_old);
extern void log_branch_erase_bbox(objptr erase_obj,Boolean use_objbbox,
				  Boolean log_in_old);

extern void log_crosshair_erase_bboxes(stateptr state,Boolean log_in_old);
extern void log_cutplane_erase_bbox(stateptr state,Boolean log_in_old);
extern Boolean new_user_input(listptr world_list, stateptr state,
			      stateptr oldstate);

extern void update_help_window(actionkind theaction);

extern void update_display();

/****** externs for vertex.c routines *****/

extern void setvfe(vfeptr, float *, Matrix);
extern void setvfe_local(vfeptr, float *, Matrix);
extern vfeptr new_vfe(vertype pos, shellptr thishell);
extern vfeptr new_vfe_local(vertype local_pos, shellptr thishell);
extern void setfnorm(fveptr thisfve, vertype newfnorm);
extern void setfnorm_local(fveptr thisfve, vertype newfnorm);
extern void sortvertlist(int axis, listptr vfelist);
extern leptr get_lemax(int axis, boundptr thisbound);
extern leptr get_lemin(int axis, boundptr thisbound);
extern vfeptr get_vertmax(int axis, listptr vertlist);
extern vfeptr get_3axis_vfemax(int, int, int, listptr);
extern vfeptr get_vertmin(int, listptr);
extern void get_vertminmax(int, listptr, vfeptr *, vfeptr *);
extern void get_local_vertminmax(int, listptr, vfeptr *, vfeptr *);

/****** externs for psprint.c routines *****/

extern void plot_fve(fveptr thisfve);
extern void plot_fve_outline(fveptr thisfve);
extern void plot_shell(shellptr thishell);
extern void plot_obj(objptr thisobj, char *filename);
extern void plot_obj_branch(objptr rootobj, char *filename);
extern void plotfaceles(fveptr thisfve, char *filename);

/****** externs for CPL core routines in cplcore.c *****/

extern Boolean debugcpl,debugcplspool;
extern FILE *cplwatchfile;
extern Boolean restart_CPLparser;
extern CPL_word_ptr cpnormal_address;

extern CPL_word_ptr OP;
extern CPL_word AS,AT,AR,AD;

extern CPL_word_ptr COP,CAS,CAT,CAR,CAD,CNF;
extern CPL_word_ptr XOP,XAS,XAT,XAR,XAD,XNF;
extern CPL_word_ptr ROM_start, RAM_start;

extern listptr CPL_stack;

extern CPL_word CPL_controlstack[];
extern int CPL_controlstack_top;
extern featureptr current_owner;
extern featureptr owner_stack[];
extern int owner_stack_top;

extern CPL_word system_codebloc[];
extern CPL_word_ptr next_free_CPL_word;
extern CPL_word_ptr next_free_RAM_word;
extern CPL_word temp_message_block[];
extern int end_temp_message_block;
extern Boolean reset_temp_message_block;

extern CPL_word system_codebloc[];

extern CPLauxtype auxiliary_bloc[];
extern CPLauxptr next_free_auxiliary_ptr;

extern symbolrec symbol_records[];
extern int top_symbol_record;
extern symbolrec local_symbol_records[];
extern int top_local_symbol_record;
extern symbolrec total_local_symbol_records[];
extern int top_total_local_symbol_record;

extern opcoderec opcoderecs[];

extern void blankout_CPL_word(CPL_word_ptr blanked_word);
extern void clear_word(CPL_word_ptr word_to_clear);
extern listptr proplist_to_featurelist(listptr proplist);
extern void copy_wordA_to_wordB(CPL_word_ptr wordA,CPL_word_ptr wordB);
extern void add_wordA_to_wordB(CPL_word_ptr wordA,CPL_word_ptr wordB);

extern void setup_CPL_word(CPL_word_ptr setup_word,CPLwordkind setupkind);
extern void setup_CPL_word_blank(CPL_word_ptr setup_word,CPLwordkind setupkind);
extern void setup_CPL_auxword(CPL_word_ptr setup_word,auxdatakind aux_kind);
extern void reset_CPL_frames(void);
extern void reset_CPL_RAM(void);
extern void start_CPL_parse(listptr world_list,stateptr state,stateptr oldstate);
extern void restart_CPL_parse(listptr world_list,stateptr state,
			      stateptr oldstate);
extern void init_CPL_language(void);
extern void toggle_cpldebug(void);

/****** externs for CPL library routines cplib.c *****/

extern Croutinerec Croutines[];

extern listptr create_CPL_stack(void);
extern Boolean CPL_stack_empty(listptr this_CPL_stack);
extern void push_on_CPL_stack(listptr this_CPL_stack,CPL_word_ptr push_CPL_word);
extern void pop_off_CPL_stack(listptr this_CPL_stack,CPL_word_ptr pop_CPL_word,
			      Boolean clear_target);
extern void push_on_CPL_controlstack(CPL_word_ptr push_CPL_controlword);
extern void pop_off_CPL_controlstack(CPL_word_ptr pop_CPL_controlword);
extern void push_owner_stack(void) ;
extern void pop_owner_stack(void);
extern Boolean CPL_queue_empty(listptr this_CPL_queue);
extern listptr create_CPL_queue(void);
extern Boolean CPL_queue_empty(listptr this_CPL_queue);
extern void add_to_CPL_queue(listptr this_CPL_queue,CPL_word_ptr added_CPL_word,
			     Boolean prepend);
extern void remove_from_CPL_queue(listptr this_CPL_queue,
				  CPL_word_ptr removed_CPL_word,
				  Boolean from_top);
extern void copy_from_CPL_queue(listptr this_CPL_queue,
				CPL_word_ptr removed_CPL_word,
				Boolean from_top);

extern void init_obj_charstream_list(void);
extern Boolean objects_in_charstream(void);
extern void stream_char_to_objects(char typed_char);

extern void CPL_start_draw(listptr world_list,stateptr state);
extern void CPL_end_draw(listptr world_list,stateptr state);
extern void CPL_extend_draw(listptr world_list,stateptr state);
extern void CPL_draw_on_surface(listptr world_list,stateptr state);
extern void CPL_draw_line(listptr world_list,stateptr state);
extern void CPL_start_marquee(listptr world_list,stateptr state);
extern void CPL_do_zoom(listptr world_list,stateptr state);
extern void CPL_world_2_inch(listptr world_list,stateptr state);
extern void CPL_inch_2_world(listptr world_list,stateptr state);
extern void CPL_make_decal(listptr world_list,stateptr state);
extern void CPL_tool_off_plane(listptr world_list,stateptr state);
extern void CPL_slice_object(listptr world_list,stateptr state);
extern void CPL_arrange_prims(listptr world_list,stateptr state);
extern void CPL_arrange_toolpads(listptr world_list,stateptr state);
extern void CPL_near_toolbar(listptr world_list,stateptr state);
extern void CPL_do_color(listptr world_list,stateptr state);
extern void CPL_set_light(listptr world_list,stateptr state);
extern void CPL_switch_light(listptr world_list,stateptr state);
extern void CPL_control_color(listptr world_list,stateptr state);
extern void CPL_set_color_bars(listptr world_list,stateptr state);
extern void CPL_set_picksize(listptr world_list,stateptr state);
extern void CPL_setup_scalebbox(listptr world_list,stateptr state);
extern void CPL_scalebbox_trans(listptr world_list,stateptr state);
extern void CPL_always_set_select(listptr world_list,stateptr state);
extern void CPL_reset_picksize(listptr world_list,stateptr state);
extern void CPL_add_obj_charstream(listptr world_list,stateptr state);
extern void CPL_rem_obj_charstream(listptr world_list,stateptr state);
extern void CPL_filter_char(listptr world_list,stateptr state);
extern void CPL_add_to_label(listptr world_list,stateptr state);
extern void CPL_label_fits(listptr world_list,stateptr state);
extern void CPL_backspace_label(listptr world_list,stateptr state);
extern void CPL_set_insertion_pt(listptr world_list,stateptr state);
extern void CPL_move_insertion_pt(listptr world_list,stateptr state);
extern void CPL_read_from_file(listptr world_list,stateptr state);
extern void CPL_write_to_file(listptr world_list,stateptr state);
extern void CPL_get_feature_size(listptr world_list,stateptr state);
extern void CPL_set_feature_size(listptr world_list,stateptr state);
extern void CPL_get_feature_dist(listptr world_list,stateptr state);
extern void CPL_set_feature_dist(listptr world_list,stateptr state);
extern void CPL_remake_piechart(listptr world_list,stateptr state);
extern void CPL_toggle_slide(listptr world_list,stateptr state);
extern void CPL_set_camera(listptr world_list,stateptr state);
extern void CPL_rotate_camera(listptr world_list,stateptr state);
extern void CPL_turnon_arrowcursor(listptr world_list,stateptr state);
extern void CPL_set_hiddenedge(listptr world_list, stateptr state);
extern void CPL_cursor_on_border(listptr world_list,stateptr state);
extern void CPL_normal_constrain(listptr world_list,stateptr state);
extern void CPL_remake_prim(listptr world_list,stateptr state);
extern void CPL_get_plate_by_name(listptr world_list,stateptr state);
extern void CPL_generate_info_rec(listptr world_list,stateptr state);
extern void CPL_breakup_info_rec(listptr world_list,stateptr state);
extern void CPL_breakup_dist_value(listptr world_list,stateptr state);

/****** externs for CPL opcodes routines cplops.c *****/


extern CPL_word JSRAT_codebloc[];
extern CPL_word_ptr next_free_JSRAT_word;
extern Boolean start_JSRAT;
extern Boolean finish_JSRAT;

extern Coord * get_first_vert(featureptr thefeature);
extern CPL_word_ptr inc_CPL_blocptr(CPL_word_ptr inc_word,
				    CPL_word_ptr base_address,int arraysize);
extern featureptr find_subfeature(objptr motherobj,property thisprop);
extern void find_subfeatures(objptr motherobj,property thisprop,
			     listptr cull_list);
extern void enter_in_temp_message_block(CPL_word_ptr new_queued_word);
extern void queue_feature_message(featureptr thefeature, Boolean priority);
extern void message_block_reset(void);

extern void execute_NOP(listptr world_list,stateptr state);
extern void execute_MOVE(listptr world_list,stateptr state);
extern void execute_ADD(listptr world_list,stateptr state);
extern void execute_CLEAR(listptr world_list,stateptr state);
extern void execute_PUSH(listptr world_list,stateptr state);
extern void execute_POP(listptr world_list,stateptr state);
extern void execute_WIPE(listptr world_list,stateptr state);
extern void execute_UNWIPE(listptr world_list,stateptr state);
extern void execute_AND(listptr world_list,stateptr state);
extern void execute_OR(listptr world_list,stateptr state);
extern void execute_NOT(listptr world_list,stateptr state);
extern void execute_COUNT(listptr world_list,stateptr state);
extern void execute_INDEX(listptr world_list,stateptr state);
extern void execute_BEQUAL(listptr world_list,stateptr state);
extern void execute_BNEQUAL(listptr world_list,stateptr state);
extern void execute_BEMPTY(listptr world_list,stateptr state);
extern void execute_BNEMPTY(listptr world_list,stateptr state);
extern void execute_BTRUE(listptr world_list,stateptr state);
extern void execute_BFALSE(listptr world_list,stateptr state);
extern void execute_BWIPED(listptr world_list,stateptr state);
extern void execute_OWNER(listptr world_list,stateptr state);
extern void execute_SETOWN(listptr world_list,stateptr state);
extern void execute_MOTHER(listptr world_list,stateptr state);
extern void execute_ANCEST(listptr world_list,stateptr state);
extern void execute_DESCEND(listptr world_list,stateptr state);
extern void execute_PARENT(listptr world_list,stateptr state);
extern void execute_CHLDRN(listptr world_list,stateptr state);
extern void execute_SIBLING(listptr world_list,stateptr state);
extern void execute_MAKE(listptr world_list,stateptr state);
extern void execute_BMSGS(listptr world_list,stateptr state);
extern void execute_BNOMSGS(listptr world_list,stateptr state);
extern void execute_MAKEMSG(listptr world_list,stateptr state);
extern void execute_QMSG(listptr world_list,stateptr state);
extern void execute_UNQMSG(listptr world_list,stateptr state);
extern void execute_CHKMSG(listptr world_list,stateptr state);
extern void execute_JSR(listptr world_list,stateptr state);
extern void execute_JSRAT(listptr world_list,stateptr state);
extern void execute_RTS(listptr world_list,stateptr state);
extern void execute_JUMP(listptr world_list,stateptr state);
extern void execute_SETFR(listptr world_list,stateptr state);
extern void execute_ENDFR(listptr world_list,stateptr state);
extern void execute_CRTN(listptr world_list,stateptr state);
extern void execute_TRANS(listptr world_list,stateptr state);
extern void execute_ROTATE(listptr world_list,stateptr state);
extern void execute_SCALE(listptr world_list,stateptr state);
extern void execute_TRNSFRM(listptr world_list,stateptr state);
extern void execute_CONCAT(listptr world_list,stateptr state);
extern void execute_DIFF3D(listptr world_list,stateptr state);
extern void execute_SNAP(listptr world_list,stateptr state);
extern void execute_WITHIN(listptr world_list,stateptr state);
extern void execute_GETNAME(listptr world_list,stateptr state);
extern void execute_SETNAME(listptr world_list,stateptr state);
extern void execute_GETLABEL(listptr world_list,stateptr state);
extern void execute_SETLABEL(listptr world_list,stateptr state);
extern void execute_GETCOLOR(listptr world_list,stateptr state);
extern void execute_SETCOLOR(listptr world_list,stateptr state);
extern void execute_AUX2STR(listptr world_list,stateptr state);
extern void execute_STR2AUX(listptr world_list,stateptr state);
extern void execute_HASPROP(listptr world_list,stateptr state);
extern void execute_GETPROP(listptr world_list,stateptr state);
extern void execute_ADDPROP(listptr world_list,stateptr state);
extern void execute_REMPROP(listptr world_list,stateptr state);
extern void execute_SELECT(listptr world_list,stateptr state);
extern void execute_UNSEL(listptr world_list,stateptr state);
extern void execute_PICK(listptr world_list,stateptr state);
extern void execute_RELEAS(listptr world_list,stateptr state);
extern void execute_UNPICK(listptr world_list,stateptr state);
extern void execute_DBLPIC(listptr world_list,stateptr state);
extern void execute_MPICK(listptr world_list,stateptr state);
extern void execute_MRELEAS(listptr world_list,stateptr state);
extern void execute_DUP(listptr world_list,stateptr state);
extern void execute_DUPCPL(listptr world_list,stateptr state);
extern void execute_DELETE(listptr world_list,stateptr state);
extern void execute_BOOLEAN(listptr world_list,stateptr state);
extern void execute_ADOPT(listptr world_list,stateptr state);
extern void execute_UNADOPT(listptr world_list,stateptr state);
extern void execute_GETLINK(listptr world_list,stateptr state);
extern void execute_UPDATE(listptr world_list,stateptr state);
extern void execute_WTRANS(listptr world_list,stateptr state);
extern void execute_WORLD(listptr world_list,stateptr state);
extern void execute_GLUPDATE(listptr world_list,stateptr state);
extern void execute_CPSHADE(listptr world_list,stateptr state);
extern void execute_CONNECT(listptr world_list,stateptr state);
extern void execute_GETCONN(listptr world_list,stateptr state);
extern void execute_RECON(listptr world_list,stateptr state);
extern void execute_RECOFF(listptr world_list,stateptr state);
extern void execute_UPDATEON(listptr world_list,stateptr state);
extern void execute_UPDATEOFF(listptr world_list,stateptr state);
extern void execute_QUIT(listptr world_list,stateptr state);
extern void execute_DEBUG(listptr world_list,stateptr state);



/****** externs for CPL read-in routines in cplread.c *****/

extern void init_CPL_global_lists(void);
extern CPL_word_ptr find_symbol(char *symbolstr);
extern Boolean lookup_symbol_from_address(CPL_word_ptr lookup_address,
					  char *symbolstr);
extern messagekind lookup_CPL_message(char messagestr[]);
extern constant_kind lookup_CPL_const(char conststr[]);
extern objptr find_object_in_any_world(char *object_name);
extern worldptr find_world(char *world_name);

extern CPL_word_ptr read_cplfile(char *cpl_file_name,CPL_word_ptr load_word,
				 featureptr known_owner);
extern void read_CPLcode_into_rom(char *rom_filename);
extern void read_CPLcode_into_ram(char *ram_filename);
extern void read_CPLcode_into_property(char *ram_filename);
extern void read_CPL_privatecode(char *ram_filename,featureptr known_owner);
extern void read_CPL_propfile(char *property_name);
extern void read_CPL_propfiles(void);
extern void resolve_CPL_forward_defs(void);
extern void resolve_CPL_forward_object_refs(void);

extern void store_CPL_RAM_word(CPL_word_ptr);
extern void store_simple_RAM_instruction(opcodekind theopcode);
extern void store_CPL_RAM_instruction(CPL_word_ptr OP_ptr,...);
extern void encode_CPL_RAM_instruction(char *CPL_instruction_line);

extern void disassemble_CPL_word(CPL_word_ptr dis_word,char *wordstr);
extern void disassemble_CPL_sentence(CPL_word_ptr OP_ptr,CPL_word_ptr AS_ptr,
				     CPL_word_ptr AT_ptr, CPL_word_ptr AR_ptr,
				     CPL_word_ptr AD_ptr,
				     char *outstr);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* EXTERN_H */

