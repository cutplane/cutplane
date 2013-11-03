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
#include "deform.h"
#include "../src/device.h"
#include "storage_cutplane.h"
#include "pencilsketch.h"

/****** externs for clip.c routines (only used on the sun) *****/

extern void clip();

/* boolean.c */

extern void init_facepair_lists(facepairptr thisfacepair);
extern void copy_facepair_elem(facepairptr sourcefacepair, facepairptr destfacepair);
extern void delete_facepair_lists(facepairptr thisfacepair);
extern void make_sector(listptr sectorlist, leptr thisle, vertype sectornorm, vertype ref1, vertype ref2);
extern void create_sectorlist(leptr centerle, listptr sectorlist);
extern Boolean vector_in_sector(sectorptr thesector, vertype thevec);
extern Boolean sectors_intersect(sectorptr sectorA, sectorptr sectorB);
extern void bogosity_personified(void);
extern void set_sidesectorpairs(listptr sectorlist, sectorptr thisector, int thiside, sectorpairptr newsectorpair);
extern void set_sectorpair(vvptr thisvv, sectorpairptr new_sectorpair, sectorptr sectorA, sectorptr sectorB);
extern void create_sectorpairlist(vvptr thisvv);
extern void create_vv_neighborhood(vvptr thisvv);
extern void set_vf_sidecodes(vfptr thisvf, Boolean faceisptr1);
extern void create_vf_neighborhood(vfptr thisvf);
extern void create_neighborhoodlists(listptr vv_list, listptr vf_list);
extern void set_pair_oninfo(int index, pair_oninfo_type pair_oninfo[], sectorpairptr sectorpair, int Aonside, int Bonside);
extern int find_common_edge_sectorpairs(vvptr thisvv, sectorpairptr thisectorpair, int sectorAonside, int sectorBonside, pair_oninfo_type pair_oninfo[]);
extern int findsectoronside(int sidecodes[]);
extern Boolean surfaces_intersect(int sectorAonside, int sectorBonside, pair_oninfo_type pair_oninfo[]);
extern void reclassify_onface_edges(vvptr thisvv);
extern void reclassify_adjoining_sectors(sectorpairptr thisectorpair, sectorptr sectorA, sectorptr sectorB, sectorptr prevsectorA, sectorptr prevsectorB, sectorptr nextsectorA, sectorptr nextsectorB, int newsidecodeA, int newsidecodeB);
extern void reclassify_onface_sectors(vvptr thisvv, int operation);
extern void classify_vertex_touching_vertex(vvptr thisvv, int operation);
extern void reclassify_vf_onsectors(vfptr thisvf, int operation);
extern void reclassify_vf_onedges(vfptr thisvf);
extern void classify_vertex_touching_face(vfptr thisvf, int operation);
extern void classify_neighborhoods(shellptr shellA, shellptr shellB, int operation, listptr vv_list, listptr vf_list);
extern Boolean scan_for_intersecting_pair(sectorpairptr *thisectorpair);
extern void set_from_to_les(sectorpairptr thisectorpair, leptr *fromle_A, leptr *to_le_A, leptr *fromle_B, leptr *to_le_B);
extern void insert_null_le(leptr from_le, leptr to_le, lepairptr thislepair, int index);
extern void set_out_in_flag(Boolean *out_in_flag, sectorpairptr thisectorpair, sectorpairptr lastsectorpair, int shellindex);
extern void insert_null_loop_into_face(vertype breakpos, fveptr theface, lepairptr lepair, int index);
extern void sorted_insert_null_lepair(lepairptr new_lepair, listptr sorted_lepairlist);
extern listptr sort_null_lelist(listptr null_lepairlist);
extern void connect_null_les(leptr null_le, leptr othernull_twin);
extern Boolean isa_strut(leptr thisle);
extern Boolean isa_hole(leptr thisle);
extern Boolean inward_nullstrutle(leptr thisle);
extern Boolean outward_nullstrutle(leptr thisle);
extern Boolean connectable_struts(leptr le1, leptr le2);
extern Boolean connectable(leptr le1, leptr le2);
extern lepairptr getnext_connectable(listptr lepairlist, lepairptr thislepair, lepairptr (*next_end)(void), int shellindex);
extern lepairptr choose_connectable(leptr startle, lepairptr connectable1, lepairptr connectable2, int shellindex);
extern lepairptr first_connectable(listptr lepairlist, lepairptr startlepair, lepairptr (**getnext_lepair)(void), int shellindex);
extern void make_solidA_nullfaces(lepairptr startlepair, listptr sorted_null_lelist, listptr temp_null_lelist);
extern void make_solidB_nullfaces(lepairptr startlepair, listptr sorted_null_lelist, listptr temp_null_lelist);
extern lepairptr find_nonstrut_lepair(listptr sorted_null_lelist, int shellindex);
extern void make_null_faces_boolean(listptr sorted_null_lelist);
extern void find_null_le_rings(listptr null_lelist, listptr null_le_rings, int shellindex);
extern void find_interior_bisector(leptr le1, leptr le2, vertype bisector);
extern Boolean null_le_ring_ok(lepairptr thislepair, int shellindex);
extern void flip_null_le_ring(lepairptr thislepair, int shellindex);
extern void fix_null_le_rings(listptr null_lelist, int shellindex);
extern void snip_null_le(lepairptr thislepair, int solidflag, listptr break_faces);
extern void consolidate_null_faces_boolean(listptr snip_null_lelist, listptr outfacesA, listptr outfacesB);
extern void ensure_hole_orientations(shellptr thishell);
extern listptr separate_pieces_boolean(shellptr thishell, listptr outfaces);
extern void boolean_recombine(int /*operator*/, listptr infacesA, listptr outfacesA, listptr infacesB, listptr outfacesB);
extern void del_extra_shells(worldptr world);
extern listptr do_boolean(objptr objA, objptr objB, int operation);

/* boolprep.c */

extern leptr find_correct_splitedge(evfptr oldsplitedge, vertype breakpos);
extern void log_pure_vv(listptr vv_list, vfeptr vfeA, vfeptr vfeB);
extern void log_vf(listptr vf_list, vfeptr touchvfe, fveptr touchfve, int solidflag);
extern void log_pure_vvs(shellptr shellA, shellptr shellB, listptr vv_list);
extern void log_vee(evfptr evfA, evfptr evfB, vertype breakpos, int onptcode, listptr vee_list);
extern void log_vertex_edges(shellptr shellA, shellptr shellB, listptr vee_list);
extern void compute_breakpos(evfptr thisevf, double t, vertype breakpos);
extern void split_from_veelist(shellptr shellA, shellptr shellB, listptr vee_list, listptr vv_list);
extern void log_edge_faces(shellptr shell1, shellptr shell2, listptr vf_list, listptr ef_list, int solidflag);
extern void resolve_edge_faces(shellptr shellA, shellptr shellB, listptr vf_list, listptr ef_list);
extern void boolean_preprocess(shellptr shellA, shellptr shellB, listptr vv_list, listptr vf_list);

/* color.c */

extern unsigned long packcolor(colortype thecolor);
extern void unpackcolor(unsigned long packedcolor, colortype unpackedcolor);
extern void copycolor(colortype sourcecolor, colortype destcolor);
extern void scalecolor(colortype thecolor, float scalefactor, colortype result);
extern void addcolors(colortype color1, colortype color2, colortype resultcolor);
extern void multcolors(colortype color1, colortype color2, colortype resultcolor);
extern void set_feature_color(featureptr thisfeature, colortype thiscolor);
extern void get_feature_color(featureptr thisfeature, colortype thiscolor);
extern void copy_feature_color(featureptr sourcefeature, featureptr destfeature);
extern void get_named_color(char *colorname, colortype rgbval);
extern void set_feature_named_color(featureptr thisfeature, char *colorname);

/* controls.c */

/* For the pc */
extern void scan_pc_buttons(void);
extern void reset_pc_valuator_store(void);
extern void set_pc_valuators(int x, int y);
extern int pc_queue_push(long qentry, short qval);
extern int pc_queue_push_key(int wParam);

extern tiedbuttontype tiedbuttons[];

extern char kbdevice_to_ascii(int thedev);
extern Boolean is_keyboard_dev(int thedev);
extern void store_roomrot(int delta1, int delta2);
extern tiedbuttonkind map_qentry2tiedbutton(int qentry, int actual_qentry);
extern Boolean button_clear(tiedbuttonkind thistied);
extern Boolean button_pressed(tiedbuttonkind thistied);
extern Boolean encode_statechange(tiedbuttonkind thistied, stateptr state);
extern void init_tied_buttons(stateptr state);
extern void init_valuatorec(int thevaluator, Device thedev, int initialvalue, int minrange, int maxrange, int scanrangemin, int scanrangemax);
extern void init_valuatorecs(void);
/* this should be in utilities.c or some such */
extern void clear_record(char *therec, int recsize);
extern void init_controls(stateptr state);
extern Boolean update_queued_buttons(int *qentry);
extern Boolean update_tied_buttons(int qentry, stateptr state);
extern void update_nonqueued_tied_buttons(stateptr state);
extern Boolean lowlevel_controldev_update(int *delta, int thevaluator);
extern void calc_z_direction(stateptr state, vertype z_dir);
extern int planemove_interpolate(stateptr state, int mouse_deltax, int mouse_deltay);
extern void store_cursor_move(vertype cursor_move, Boolean dragging);
extern void store_mouse_cursor_move(int delta1, int delta2, stateptr state);
extern void reset_multiple_clicks(void);
extern int check_for_multiple_click(tiedbuttonkind checkbutton);
extern void record_devhistory(tiedbuttonkind recordbutton);
extern Boolean update_polled_valuators(stateptr state);
extern Boolean pick_button_down(void);
extern Boolean cutplane_button_down(void);
extern Boolean roomrot_button_down(void);

/* cplcore.c */

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
extern void copy_wordA_to_wordB(CPL_word_ptr wordA, CPL_word_ptr wordB);
extern void add_wordA_to_wordB(CPL_word_ptr wordA, CPL_word_ptr wordB);
extern featureptr lookup_feature_fcn(iterateptr iterate_info);
extern Boolean advance_feature_fcn(iterateptr iterate_info);
extern void reset_feature_fcn(iterateptr iterate_info);
extern featureptr lookup_props_fcn(iterateptr iterate_info);
extern void reset_props_innerloop(iterateptr iterate_info);
extern Boolean advance_props_fcn(iterateptr iterate_info);
extern void reset_props_fcn(iterateptr iterate_info);
extern void setup_CPL_word(CPL_word_ptr setup_word, CPLwordkind setupkind);
extern void setup_CPL_word_blank(CPL_word_ptr setup_word, CPLwordkind setupkind);
extern void setup_CPL_auxword(CPL_word_ptr setup_word, auxdatakind word_auxkind);
extern CPL_word_ptr resolve_indirection(CPL_word_ptr this_CPLptr);
extern Boolean initialize_iterate(iterateptr iterate_info, CPL_word_ptr this_word_iterate, CPL_word_ptr loop_reg);
extern Boolean CPL_iterate(CPL_word_ptr fill_reg, iterateptr iterate_info, Boolean setup_only);
extern void CPL_resolver(listptr world_list, stateptr state, CPL_word_ptr OP_ptr, CPL_word_ptr AS_ptr, CPL_word_ptr AT_ptr, CPL_word_ptr AR_ptr, CPL_word_ptr AD_ptr);
extern void CPL_set_frame(CPL_word_ptr OP_ptr, CPL_word_ptr AS_ptr, CPL_word_ptr AT_ptr, CPL_word_ptr AR_ptr, CPL_word_ptr AD_ptr, CPL_word_ptr NF_ptr);
extern void CPL_advance_frame(CPL_word_ptr OP_ptr, CPL_word_ptr AS_ptr, CPL_word_ptr AT_ptr, CPL_word_ptr AR_ptr, CPL_word_ptr AD_ptr, CPL_word_ptr NF_ptr);
extern void CPL_parse(listptr world_list, stateptr state, stateptr oldstate);
extern void toggle_cpldebug(void);
extern void reset_CPL_frames(void);
extern void reset_CPL_RAM(void);
extern void start_CPL_parse(listptr world_list, stateptr state, stateptr oldstate);
extern void restart_CPL_parse(listptr world_list, stateptr state, stateptr oldstate);
extern void init_CPL_frames(void);
extern void init_CPL_RAM(void);
extern void clear_CPL_membloc(CPL_word bloc[], int blocsize);
extern void init_CPL_language(void);

/* cplib.c */

extern Croutinerec Croutines[];

extern listptr create_CPL_stack(void);
extern Boolean CPL_stack_empty(listptr this_CPL_stack);
extern void push_on_CPL_stack(listptr this_CPL_stack, CPL_word_ptr push_CPL_word);
extern void pop_off_CPL_stack(listptr this_CPL_stack, CPL_word_ptr pop_CPL_word, Boolean clear_target);
extern listptr create_CPL_queue(void);
extern Boolean CPL_queue_empty(listptr this_CPL_queue);
extern void add_to_CPL_queue(listptr this_CPL_queue, CPL_word_ptr added_CPL_word, Boolean prepend);
extern void remove_from_CPL_queue(listptr this_CPL_queue, CPL_word_ptr removed_CPL_word, Boolean from_top);
extern void copy_from_CPL_queue(listptr this_CPL_queue, CPL_word_ptr removed_CPL_word, Boolean from_top);
extern void push_owner_stack(void);
extern void pop_owner_stack(void);
extern Boolean CPL_controlstack_full(void);
extern void push_on_CPL_controlstack(CPL_word_ptr push_CPL_controlword);
extern void pop_off_CPL_controlstack(CPL_word_ptr pop_CPL_controlword);
extern void init_obj_charstream_list(void);
extern Boolean objects_in_charstream(void);
extern void stream_char_to_objects(char typed_char);
extern void CPL_start_marquee(listptr world_list, stateptr state);
extern void CPL_start_draw(listptr world_list, stateptr state);
extern void CPL_end_draw(listptr world_list, stateptr state);
extern void CPL_extend_draw(listptr world_list, stateptr state);
extern void CPL_draw_on_surface(listptr world_list, stateptr state);
extern void CPL_draw_line(listptr world_list, stateptr state);
extern void CPL_do_zoom(listptr world_list, stateptr state);
extern void CPL_world_2_inch(listptr world_list, stateptr state);
extern void CPL_inch_2_world(listptr world_list, stateptr state);
extern void CPL_make_decal(listptr world_list, stateptr state);
extern void CPL_tool_off_plane(listptr world_list, stateptr state);
extern void CPL_slice_object(listptr world_list, stateptr state);
extern void CPL_arrange_prims(listptr world_list, stateptr state);
extern void CPL_arrange_toolpads(listptr world_list, stateptr state);
extern void CPL_near_toolbar(listptr world_list, stateptr state);
extern void CPL_do_color(listptr world_list, stateptr state);
extern void CPL_set_light(listptr world_list, stateptr state);
extern void CPL_switch_light(listptr world_list, stateptr state);
extern short make_colorbar_value(objptr colorbar);
extern void CPL_control_color(listptr world_list, stateptr state);
extern void set_colorbar_height(objptr colorbar, short value);
extern void CPL_set_color_bars(listptr world_list, stateptr state);
extern void CPL_set_picksize(listptr world_list, stateptr state);
extern void CPL_setup_scalebbox(listptr world_list, stateptr state);
extern void find_other_bboxcorner(bbox3dptr thisbbox, vertype thiscorner, vertype othercorner);
extern void CPL_scalebbox_trans(listptr world_list, stateptr state);
extern void CPL_always_set_select(listptr world_list, stateptr state);
extern void CPL_reset_picksize(listptr world_list, stateptr state);
extern void CPL_add_obj_charstream(listptr world_list, stateptr state);
extern void CPL_rem_obj_charstream(listptr world_list, stateptr state);
extern void CPL_filter_char(listptr world_list, stateptr state);
extern void CPL_label_fits(listptr world_list, stateptr state);
extern void CPL_add_to_label(listptr world_list, stateptr state);
extern void CPL_backspace_label(listptr world_list, stateptr state);
extern void CPL_set_insertion_pt(listptr world_list, stateptr state);
extern void CPL_move_insertion_pt(listptr world_list, stateptr state);
extern void CPL_read_from_file(listptr world_list, stateptr state);
extern void CPL_write_to_file(listptr world_list, stateptr state);
extern void CPL_get_feature_size(listptr world_list, stateptr state);
extern void CPL_set_feature_size(listptr world_list, stateptr state);
extern void CPL_get_feature_dist(listptr world_list, stateptr state);
extern void CPL_set_feature_dist(listptr world_list, stateptr state);
extern void CPL_remake_piechart(listptr world_list, stateptr state);
extern void CPL_toggle_slide(listptr world_list, stateptr state);
extern void CPL_set_camera(listptr world_list, stateptr state);
extern void CPL_rotate_camera(listptr world_list, stateptr state);
extern void CPL_turnon_arrowcursor(listptr world_list, stateptr state);
extern void CPL_set_hiddenedge(listptr world_list, stateptr state);
extern void CPL_cursor_on_border(listptr world_list, stateptr state);
extern void CPL_normal_constrain(listptr world_list, stateptr state);
extern void CPL_remake_prim(listptr world_list, stateptr state);
extern void CPL_get_plate_by_name(listptr world_list, stateptr state);
extern void CPL_generate_info_rec(listptr world_list, stateptr state);
extern void CPL_breakup_info_rec(listptr world_list, stateptr state);
extern void CPL_breakup_dist_value(listptr world_list, stateptr state);

/* cplops.c */

extern CPL_word JSRAT_codebloc[];
extern CPL_word_ptr next_free_JSRAT_word;
extern Boolean start_JSRAT;
extern Boolean finish_JSRAT;

extern Coord *get_first_vert(featureptr thefeature);
extern Boolean records_equal(char *rec1, char *rec2, int recsize);
extern Boolean aux_records_equal(CPL_word_ptr wordA, CPL_word_ptr wordB);
extern CPL_word_ptr inc_CPL_blocptr(CPL_word_ptr inc_word, CPL_word_ptr base_address, int arraysize);
extern featureptr find_subfeature(objptr motherobj, property thisprop);
extern void find_subfeatures(objptr motherobj, property thisprop, listptr cull_list);
extern Boolean sectionline_in_3dbbox(objptr testobj, bbox3dptr testbbox, Coord section_z);
extern void enter_in_temp_message_block(CPL_word_ptr new_queued_word);
extern void queue_feature_message(featureptr thefeature, Boolean priority);
extern void message_block_reset(void);
extern void execute_NOP(listptr world_list, stateptr state);
extern void execute_MOVE(listptr world_list, stateptr state);
extern void execute_ADD(listptr world_list, stateptr state);
extern void execute_CLEAR(listptr world_list, stateptr state);
extern void execute_PUSH(listptr world_list, stateptr state);
extern void execute_POP(listptr world_list, stateptr state);
extern void execute_WIPE(listptr world_list, stateptr state);
extern void execute_UNWIPE(listptr world_list, stateptr state);
extern void execute_AND(listptr world_list, stateptr state);
extern void execute_OR(listptr world_list, stateptr state);
extern void execute_NOT(listptr world_list, stateptr state);
extern void execute_COUNT(listptr world_list, stateptr state);
extern void execute_INDEX(listptr world_list, stateptr state);
extern void execute_HASPROP(listptr world_list, stateptr state);
extern void execute_GETPROP(listptr world_list, stateptr state);
extern void execute_JUMP(listptr world_list, stateptr state);
extern void execute_BEQUAL(listptr world_list, stateptr state);
extern void execute_BNEQUAL(listptr world_list, stateptr state);
extern void execute_BNEMPTY(listptr world_list, stateptr state);
extern void execute_BEMPTY(listptr world_list, stateptr state);
extern void execute_BTRUE(listptr world_list, stateptr state);
extern void execute_BFALSE(listptr world_list, stateptr state);
extern void execute_BWIPED(listptr world_list, stateptr state);
extern void execute_OWNER(listptr world_list, stateptr state);
extern void execute_SETOWN(listptr world_list, stateptr state);
extern void execute_MOTHER(listptr world_list, stateptr state);
extern void execute_ANCEST(listptr world_list, stateptr state);
extern void execute_PARENT(listptr world_list, stateptr state);
extern void execute_CHLDRN(listptr world_list, stateptr state);
extern void get_descendent_features(objptr this_obj, objptr mother_obj, listptr cull_list);
extern void execute_DESCEND(listptr world_list, stateptr state);
extern void execute_SIBLING(listptr world_list, stateptr state);
extern void execute_MAKE(listptr world_list, stateptr state);
extern void execute_BMSGS(listptr world_list, stateptr state);
extern void execute_BNOMSGS(listptr world_list, stateptr state);
extern void execute_MAKEMSG(listptr world_list, stateptr state);
extern void execute_QMSG(listptr world_list, stateptr state);
extern void execute_UNQMSG(listptr world_list, stateptr state);
extern void execute_CHKMSG(listptr world_list, stateptr state);
extern void execute_JSR(listptr world_list, stateptr state);
extern void execute_JSRAT(listptr world_list, stateptr state);
extern void execute_RTS(listptr world_list, stateptr state);
extern void execute_SETFR(listptr world_list, stateptr state);
extern void execute_ENDFR(listptr world_list, stateptr state);
extern void execute_CRTN(listptr world_list, stateptr state);
extern void execute_TRANS(listptr world_list, stateptr state);
extern void execute_ROTATE(listptr world_list, stateptr state);
extern void execute_SCALE(listptr world_list, stateptr state);
extern void execute_TRNSFRM(listptr world_list, stateptr state);
extern void execute_CONCAT(listptr world_list, stateptr state);
extern void execute_DIFF3D(listptr world_list, stateptr state);
extern void snap_features_together(featureptr snapfeature, featureptr snaptofeature);
extern void execute_SNAP(listptr world_list, stateptr state);
extern void execute_WITHIN(listptr world_list, stateptr state);
extern void execute_GETNAME(listptr world_list, stateptr state);
extern void execute_SETNAME(listptr world_list, stateptr state);
extern void execute_GETLABEL(listptr world_list, stateptr state);
extern void execute_SETLABEL(listptr world_list, stateptr state);
extern void execute_GETCOLOR(listptr world_list, stateptr state);
extern void execute_SETCOLOR(listptr world_list, stateptr state);
extern void execute_AUX2STR(listptr world_list, stateptr state);
extern void execute_STR2AUX(listptr world_list, stateptr state);
extern void execute_ADDPROP(listptr world_list, stateptr state);
extern void execute_REMPROP(listptr world_list, stateptr state);
extern void execute_SELECT(listptr world_list, stateptr state);
extern void execute_UNSEL(listptr world_list, stateptr state);
extern void execute_PICK(listptr world_list, stateptr state);
extern void execute_RELEAS(listptr world_list, stateptr state);
extern void execute_MRELEAS(listptr world_list, stateptr state);
extern void execute_UNPICK(listptr world_list, stateptr state);
extern void execute_DBLPIC(listptr world_list, stateptr state);
extern void execute_MPICK(listptr world_list, stateptr state);
extern void execute_DUP(listptr world_list, stateptr state);
extern void execute_DUPCPL(listptr world_list, stateptr state);
extern void execute_DELETE(listptr world_list, stateptr state);
extern void execute_BOOLEAN(listptr world_list, stateptr state);
extern void execute_ADOPT(listptr world_list, stateptr state);
extern void execute_UNADOPT(listptr world_list, stateptr state);
extern void execute_GETLINK(listptr world_list, stateptr state);
extern void execute_UPDATE(listptr world_list, stateptr state);
extern void execute_WTRANS(listptr world_list, stateptr state);
extern void execute_WORLD(listptr world_list, stateptr state);
extern void execute_GLUPDATE(listptr world_list, stateptr state);
extern void execute_CPSHADE(listptr world_list, stateptr state);
extern void execute_CONNECT(listptr world_list, stateptr state);
extern void execute_GETCONN(listptr world_list, stateptr state);
extern void execute_RECON(listptr world_list, stateptr state);
extern void execute_RECOFF(listptr world_list, stateptr state);
extern void execute_UPDATEON(listptr world_list, stateptr state);
extern void execute_UPDATEOFF(listptr world_list, stateptr state);
extern void execute_QUIT(listptr world_list, stateptr state);
extern void execute_DEBUG(listptr world_list, stateptr state);

/* cplread.c */

extern void init_CPL_global_lists(void);
extern void transfer_into_temp_buffer(char cpl_file_name[]);
extern void write_CPLparsebug_notice(void);
extern void convert_to_lower(char *lowerstr);
extern void convert_to_upper(char *lowerstr);
extern void copy_CPL_bloc(CPL_word_ptr bloc1, CPL_word_ptr bloc2, int blocsize);
extern CPL_word_ptr find_symbol(char *symbolstr);
extern Boolean lookup_symbol_from_address(CPL_word_ptr lookup_address, char *symbolstr);
extern objptr locate_obj(objptr searchobj, char *object_name);
extern objptr find_object_in_any_world(char *object_name);
extern worldptr find_world(char *world_name);
extern long find_update(char *update_name);
extern Boolean get_cpl_line(char cpl_line[]);
extern int read_CPL_string(char cpl_line[], int point, int maxlen, char readstr[]);
extern void add_symbolrec(char *labelstr, CPL_word_ptr load_word);
extern opcodekind lookup_CPL_opcode(char opcodestr[]);
extern CPL_word_ptr encode_CPL_opcode(char labelstr[], opcodekind encode_opcode, CPL_word_ptr load_word, Boolean *directive, Boolean *noargs);
extern constant_kind lookup_CPL_const(char conststr[]);
extern void encode_const_word(char symbolnamestr[], CPL_word_ptr load_word, featureptr known_owner);
extern void store_forward_def(CPL_word_ptr load_word, char symbolnamestr[]);
extern void store_forward_object_name_ref(CPL_word_ptr load_word, char objectnamestr[]);
extern messagekind lookup_CPL_message(char messagestr[]);
extern void encode_CPL_symbol(char symbolstr[], CPL_word_ptr load_word, featureptr known_owner);
extern CPL_word_ptr read_cpl_line(char cpl_line[], CPL_word_ptr load_word, featureptr known_owner);
extern void resolve_CPL_forward_defs(void);
extern void resolve_CPL_forward_object_refs(void);
extern CPL_word_ptr read_cplfile(char *cpl_file_name, CPL_word_ptr load_word, featureptr known_owner);
extern void close_RAM(void);
extern void store_CPL_RAM_word(CPL_word_ptr storeword);
extern void store_CPL_RAM_word_a(CPL_word_ptr storeword);
extern void store_simple_RAM_instruction(opcodekind theopcode);
extern void store_CPL_RAM_instruction(CPL_word_ptr OP_ptr, ...);
extern void encode_CPL_RAM_instruction(char *CPL_instruction_line);
extern void read_CPLcode_into_rom(char *rom_filename);
extern void read_CPLcode_into_ram(char *ram_filename);
extern void read_CPLcode_into_property(char *prop_filename);
extern void read_CPL_privatecode(char *private_filename, featureptr known_owner);
extern void read_CPL_propfile(char *property_name);
extern void read_CPL_propfiles(void);
extern void disassemble_CPL_auxdata(CPL_word_ptr dis_word, char *auxwordstr);
extern void disassemble_CPL_word(CPL_word_ptr dis_word, char *wordstr);
extern void disassemble_CPL_sentence(CPL_word_ptr OP_ptr, CPL_word_ptr AS_ptr, CPL_word_ptr AT_ptr, CPL_word_ptr AR_ptr, CPL_word_ptr AD_ptr, char *outstr);

/* cursor.c */

extern blitsavetype saved_blits[];
extern vertype default_cursor_offset;

extern void offset_cursor_image(vertype new_cursor_offset);
extern void unoffset_cursor_image(vertype new_cursor_unoffset);
extern void plant_cursor_image(void);
extern void select_cursor(cursorkind new_cursorkind);
extern void do_cursor_deselect(listptr worldlist);
extern void notify_cursor_pause(void);
extern void do_cursor_notifications(void);
extern bboxscreenptr find_normal_cursor_screenbbox(objptr cursor_obj);
extern void get_normal_cursor_blit(objptr cursor_obj, int framebuffer, stateptr state);
extern void get_xhair_cursor_blit(objptr cursor_obj, int framebuffer, stateptr state);
extern void get_saved_blit(objptr cursor_obj, int framebuffer, stateptr state);
extern void restore_normal_cursor_blit(int framebuffer);
extern void restore_xhair_cursor_blit(int framebuffer);
extern void restore_from_saved_blit(int framebuffer);
extern void draw_hidden_cursor(objptr cursor_obj);
extern void draw_plus_cursor(objptr cursor_obj);
extern void draw_arrow_cursor(objptr cursor_obj);
extern void draw_ibeam_cursor(objptr cursor_obj);
extern void draw_xhair_cursor(objptr cursor_obj);
extern void draw_cursor(objptr cursor_obj);

/* debug.c */

extern FILE *dbgdump;
extern void init_dbgdump(const char *dbgdump_filename);
extern void close_dbgdump(void);
extern void printvert(vertype thevert);
extern void iprintvert(vertype thevert);
extern void print_le(leptr thisle);
extern void printle_coords(leptr thisle);
extern void printevf(evfptr thisevf);
extern void printevfs(shellptr thishell);
extern void printfaceles(fveptr thisfve);
extern void iprintfaceles(fveptr thisfve);
extern void printshell_les(shellptr thishell);
extern void iprintshell_les(shellptr thishell);
extern void printface_evfs(fveptr thisfve);
extern void printfves(shellptr thishell);
extern void printvfefaces(vfeptr thisvfe);
extern void printvfe(vfeptr thisvfe);
extern void printvfes(shellptr thishell);
extern void printmatrix(GLfloat **thematrix);
extern void printfnorms(listptr thisfvelist);
extern void print_props(featureptr thisfeature);
extern void print_transition_props(featureptr thisfeature);
extern void print_transition_list(listptr transitionlist);
extern void print_sector(sectorptr thisector);
extern void print_sectorpair(sectorpairptr thisectorpair);
extern void print_sectorpairlist(listptr sectorpairlist);
extern void print_sectorlist(listptr sectorlist);
extern void print_veelist(listptr vee_list);
extern void print_coincidents(listptr vv_list, listptr vf_list);
extern void printnbrhood(listptr sectorlist);
extern void printnbrhoods(listptr nbrhoodlist);
extern void check_all_nbrs(shellptr thishell);
extern void printscreenbbox(bboxscreenptr thebbox);
extern void printneighbors(vfeptr thisvfe);
extern void print_face_coords(fveptr thisfve);
extern void print_face(fveptr thisfve);
extern void print_faces(shellptr thishell);
extern void old_print_evfs(listptr evflist);
extern void print_evfs(listptr evflist);
extern void printloopart(leptr le1);
extern void print_lasso(leptr le1);
extern void print_null_le_lassoes(listptr snip_null_lelist);
extern void planarity_check(shellptr thishell);
extern void walk_shell(leptr startle);
extern int confirm_handle(leptr firstle);
extern void count_rings_holes(shellptr thishell, int *r, int *h);
extern int compute_shells(shellptr thishell);
extern GLboolean topology_check(shellptr thishell);
extern GLboolean topology_check_obj(objptr thisobj);
extern int count_obj_polys(objptr thisobj);
extern int count_objtree_polys(objptr thisobj);
extern int count_world_polys(worldptr world);

/* deform.c */

extern vertype ppiped[];
extern void bicoeff(int n, lattsizevec bc_array);
extern void powers(Coord number, int maxpower, lattsizevec numpower);
extern void set_controlpt_array(shellptr control_shell, vertype controlpts[][4][4]);
extern void evaluate(objptr control_obj, vfeptr thisvfe, Coord s, Coord t, Coord u);
extern void precompute_prods(xyzstruct *TxU, xyzstruct *UxS, xyzstruct *SxT, Coord *ToUxS, Coord *X0oTxU, Coord *X0oUxS, Coord *X0oSxT);
extern Boolean obj_in_lattice(objptr thisobj, objptr control_obj);
extern void deform(objptr control_obj);

/* duplicate.c */

extern elemptr find_dest(elemptr search_elem, listptr src_list, listptr dest_list);
extern void mark_duped(elemptr oldmarkelem, elemptr newmarkelem, listptr src_marklist, listptr dest_marklist);
extern void duplicate_holes(fveptr sourcefve, listptr src_lelist, listptr dest_lelist, listptr src_fvelist, listptr dest_fvelist);
extern void duplicate_engine(leptr firstoldle, leptr firstnewle, listptr src_lelist, listptr dest_lelist, listptr src_fvelist, listptr dest_fvelist);
extern void duplicate_le_props(leptr srcle, leptr destle);
extern void duplicate_face_props(fveptr srcfve, fveptr destfve);
extern void duplicate_properties(objptr sourceobj, objptr destobj, listptr src_lelist, listptr dest_lelist, listptr src_fvelist, listptr dest_fvelist);
extern objptr duplicate_obj(objptr thisobj);
extern objptr duplicate_obj_branch_engine(objptr thisobj, objptr newparent);
extern objptr duplicate_obj_branch(objptr thisobj);
extern objptr duplicate_and_translate_obj(objptr thisobj, vertype offsetvert);

/* error.c */

extern void reset_grafx(void);
extern void restart_3form(void);
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

/* geometry.c */

extern void pickplane(vertype norm, int *axis1, int *axis2);
extern leptr find_noncollinear_le(leptr ref_le, int direction);
extern Boolean pt_near_cutplane(vertype pt1, Coord zcut, float tol);
extern Boolean pt_near_pt(vertype sourcept, vertype checkpt, float toldist);
extern Boolean pt_near_line(vertype pt0, vertype pt1, vertype chkpt, vertype nearpt, float toldist);
extern Boolean pt_near_edge(vertype thispt, elemptr *nearelem, vertype nearest_pt, listptr shellist);
extern double pt_to_line_dist_3d(vertype testpt, vertype pt1, vertype pt2);
extern double pt_to_planedist_3d(vertype testpt, vertype plane, double planedist);
extern double angle_between_planes(vertype plane1, vertype plane2);
extern double line_to_line_dist_3d(vertype pt1, vertype pt2, vertype pt3, vertype pt4);
extern Boolean pt_near_pt_3d(vertype pt1, vertype pt2, double tol);
extern Boolean pt_near_line_3d(vertype pt1, vertype pt2, vertype testpt, vertype nearpt, double *t, double tol);
extern Boolean pt_near_lineseg_3d(vertype pt1, vertype pt2, vertype testpt, vertype nearpt, double *t, double tol);
extern Boolean pt_on_lineseg_3d(vertype pt1, vertype pt2, vertype testpt);
extern Boolean pt_near_plane_3d(vertype testpt, vertype planenorm, Coord planedist);
extern void copy_obj_bbox(objptr target_obj, bbox3dptr sourcebbox);
extern void copy_group_bbox(objptr target_obj, bbox3dptr sourcebbox);
extern void compute_screen_bbox(bbox3dptr srcbbox, bboxscreenptr destbbox);
extern void compute_object_screenbbox(bbox3dptr solid_bbox, bboxscreenptr objectscreenbbox, Coord object_margin);
extern void compute_shadow_screenbbox(bbox3dptr solid_bbox, bboxscreenptr shadowscreenbbox, Coord shadow_margin);
extern int bboxcode(vertype thept, bbox3dptr thebbox);
extern void find_edge_bbox3d(float *pt1, float *pt2, bbox3dptr thebbox);
extern Boolean bbox_intersect3d(bbox3dptr bbox1, bbox3dptr bbox2);
extern int bboxscreencode(screenpos thept, bboxscreenptr thebbox);
extern Boolean bbox_intersect_screen(bboxscreenptr bbox1, bboxscreenptr bbox2);
extern Boolean point_in_screenbbox(screenpos checkpt, bboxscreenptr thisbbox);
extern Boolean point_in_2dbbox(vertype checkpt, bbox2dptr thisbbox);
extern Boolean point_in_3dbbox(vertype checkpt, bbox3dptr thisbbox);
extern Boolean lineseg_bbox_intersect(vertype pt1, vertype pt2, vertype pt3, vertype pt4);
extern void set_3d_obj_facebboxes(objptr thisobj);
extern void set_3d_objbbox(objptr thisobj);
extern void set_3d_branchbboxes(objptr thisobj);
extern void grow_2d_bbox(bbox2dptr newbbox, bbox2dptr growingbbox);
extern void grow_3d_bbox(bbox3dptr newbbox, bbox3dptr growingbbox);
extern bbox3dptr grow_group_bbox(objptr thisobj);
extern unsigned int int_ray_edge(vertype facept1, vertype facept2, vertype raypt, int rayaxis, int axis2);
extern Boolean point_in_bound(boundptr thisbound, vertype testpt);
extern Boolean pt_in_face(fveptr thisfve, vertype thepoint);
extern Boolean point_in_face(fveptr thisfve, vertype thepoint);
extern Boolean bound_in_bound(boundptr bound1, boundptr bound2);
extern int face_in_bounds(shellptr new_shell, boundptr *tight_bound, boundptr *loose_bound, listptr drawshellist);
extern int pt_on_bound_3d(vertype testpt, boundptr thebound, double tol);
extern int pt_in_bound_3d(vertype testpt, boundptr thebound, double tol);
extern int pt_in_face_3d(vertype testpt, fveptr theface, double tol);
extern Boolean edge_in_face3d(float *pt1, float *pt2, fveptr thisfve);
extern int line_intersect_plane_3d(vertype pt1, vertype pt2, vertype plane, Coord planedist, vertype intrpt, double *t);
extern int lineseg_intersect_plane_3d(vertype pt1, vertype pt2, vertype plane, Coord planedist, vertype intrpt, double *t);
extern int lineseg_int_face_3d(vertype pt1, vertype pt2, fveptr theface, vertype intrpt, double *t, double tol);
extern int lines_intersect_3d(vertype pt1, vertype pt2, vertype pt3, vertype pt4, vertype intrpt, double *t1, double *t2, double tol);
extern int linesegs_intersect_3d(vertype pt1, vertype pt2, vertype pt3, vertype pt4, vertype intrpt, double *t1, double *t2, double tol);
extern Boolean planes_intersect(vertype plane1, Coord plane1dist, vertype plane2, Coord plane2dist, vertype sectvect, vertype sectpt, double tol);
extern Boolean same_bound(boundptr bound1, boundptr bound2, Boolean *reversed);
extern fveptr find_same_face(fveptr thisfve, objptr otherobj, Boolean *reversed);
extern Boolean face_is_planar(fveptr thisfve, Coord giventol);
extern Boolean planar_face_nonormal(fveptr thisfve);
extern Boolean face_is_concave(fveptr thisfve);
extern void compute_face_centroid(fveptr thisfve, vertype centroidpos);
extern void compute_obj_centroid(objptr thisobj, vertype centroidpos);
extern Boolean clip_t(double p, double q, float *t0, float *t1);
extern int clip_lineseg_to_screenbbox(screenpos clippedpt0, screenpos clippedpt1, bboxscreenptr thebbox);
extern int clip4d(register float *clippedpt0, register float *clippedpt1);

/* globals.c */

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

/* glove.c */

extern void read_glove_bytes(char buf[], int nbytes);
extern void init_glove_serialport(void);
extern void close_glove_serialport(void);
extern char read_glove(void);
extern void write_glove(unsigned char data);
extern Boolean check_glove(void);
extern void init_glove(void);
extern void quit_glove(void);
extern gloveventkind get_glove_event(void);
extern void get_glove_wrist(void);
extern void glove_pick(stateptr state);
extern void glove_release(stateptr state);
extern Boolean glove_statechange(stateptr state);
extern Boolean update_glove_events(stateptr state);
extern Boolean glove_in_use(void);

/* grafx.c */

extern GLuint first_gl_list_id, current_gl_list_id;
extern GLfloat light1Pos[4];
extern Matrix vanillamatrix, invanillamatrix;
extern long lsetnear,lsetfar,lset_offset;
extern int framebuffer;
extern erase_record_type erase_records[2][2];
extern erase_record_ptr old_erase_records[2],new_erase_records[2];

extern void toggle_zbuffering(void);
extern void normal_zbuffer(void);
extern void shifted_zbuffer(int numtimes);
extern GLboolean backface_fve(fveptr thisfve);
extern GLboolean edge_is_hidden(evfptr thisevf, GLboolean fve1_facesback, GLboolean fve2_facesback);
extern GLboolean no_containing_feature_picked(elemptr thisfeature);
extern void def_environment_colors(void);
extern void init_projmatrix(void);
extern void map2screen(vertype thept, screenpos screenpt, GLboolean compute_mapping);
extern int *scale2screen(float thept[3], int screenpt[2], int left, int right, int bot, int top);
extern void make_vanilla_matrix(int roomrotx, int roomroty);
extern void object_defs(void);
#ifdef IRIS
extern void background_defs(void);
#endif /* IRIS */
extern bboxscreenptr find_cutplane_screenbbox(void);
extern void draw_cutplane(void);
extern void draw_cutplane_pickedrect(void);
extern void draw_crosshair(void);
extern void set_cutplanehatch_color(void);
extern void update_world(stateptr state, stateptr oldstate);
extern void set_light_dir(int lightdefindex, vertype direction);
extern void set_light_color(int lightdefindex, colortype thecolor);
extern void nodraw_technique(objptr thisobj, stateptr state);
extern void draw_scribble_technique(objptr scribble, stateptr state);
extern void draw_marquee_core(vertype corner1, vertype corner2);
extern void draw_marquee_technique(objptr marquee, stateptr state);
extern void create_gl_list_ids(void);
extern void draw_decal_globj(objptr decal_obj);
extern GLuint create_decal_globj(objptr decal_obj, vertype to_origin, vertype scale_amt, vertype decal_offset);
extern void draw_pad_technique(objptr pad, stateptr state);
extern void draw_toolpad_technique(objptr toolpad, stateptr state);
extern void draw_headpad_technique(objptr headpad, stateptr state);
extern void draw_undertab_technique(objptr undertab, stateptr state);
extern void draw_plate_technique(objptr plate, stateptr state);
extern void draw_padhandle_technique(objptr padhandle, stateptr state);
extern void draw_rotate_innercircle(objptr thisobj, stateptr state);
extern void draw_innercircle_select_box(objptr thisobj, stateptr state);
extern void draw_rotate_outercircle(objptr thisobj, stateptr state);
extern void draw_one_xbar(vertype firstpos, vertype lastpos, GLboolean drawbar, GLboolean drawbauble1, GLboolean drawbauble2, GLboolean bauble1picked, GLboolean bauble2picked);
extern void draw_rotatexbar(objptr thisobj, stateptr state);
extern void draw_big_circular_selectbox(objptr thisobj, stateptr state);
extern void draw_shell_intersection(shellptr thishell);
extern void draw_obj_intersection(objptr thisobj);
extern void draw_edge(evfptr thisevf, colortype thiscolor);
extern void draw_shadowed_edge(evfptr thisevf, colortype thiscolor);
extern void draw_edge_outline(evfptr thisevf, colortype thiscolor);
extern void draw_face_outline(fveptr thisfve, colortype thiscolor);
extern void draw_wireframe_face(fveptr thisfve, colortype thiscolor);
extern void draw_shadowed_wireframe_face(fveptr thisfve, colortype thiscolor);
extern void draw_wireframe_shell(shellptr thishell, colortype thiscolor);
extern void draw_shadowed_wireframe_shell(shellptr thishell, colortype thiscolor);
extern void draw_hidden_line_obj(objptr thisobj, colortype thiscolor);
extern void draw_shadowed_hidden_line_shell(shellptr thishell, colortype thiscolor);
extern void draw_wireframe_obj(objptr thisobj, colortype thiscolor);
extern void draw_shadowed_wireframe_object(objptr thisobj, colortype thiscolor);
extern void draw_shadowed_hidden_line_obj(objptr thisobj, colortype thiscolor);
extern void draw_solid_face(fveptr thisfve, colortype thiscolor);
extern GLboolean isa_contour(evfptr thisevf);
extern GLboolean is_onend(evfptr thisevf);
extern leptr find_next_contour(leptr thisle);
extern void compute_outdented_edge(evfptr contour_edge, GLfloat outdent_length, vertype outdentpt1, vertype outdentpt2);
extern void draw_shell_contour(shellptr thishell, GLfloat contouroffset, colortype thecolor);
extern void draw_obj_contour(objptr thisobj, GLfloat contouroffset, colortype thecolor);
extern void draw_surf_edges(surfptr thisurf, colortype thecolor);
extern void draw_obj_shadow(objptr thisobj);
extern void draw_shadows(worldptr world);
extern void draw_pick_box(vertype drawpt, int pick_linewidth, GLboolean filled);
extern void draw_square(vertype centerpt, float width, GLboolean filled);
extern void draw_shadowed_pick_box(vertype drawpt, colortype boxcolor, GLboolean filled);
extern void draw_bounding_box(bbox3dptr thebbox);
extern void draw_shells_pick_box(void);
extern void draw_scalebbox_technique(objptr scalebbox, stateptr state);
extern GLboolean surface_interior_feature(featureptr thisfeature);
extern surfptr find_mothersurf(featureptr thisfeature);
extern void draw_picked_vertex_highlight(vfeptr thisvfe);
extern void draw_picked_edge_highlight(evfptr thisevf);
extern void draw_picked_face_highlight(fveptr thisfve);
extern void draw_picked_surface_highlight(surfptr thisurf);
extern void draw_picked_object_highlight_engine(objptr thisobj);
extern void draw_picked_object_highlight(objptr thisobj);
extern void draw_selectable_obj_highlight_engine(objptr thisobj);
extern void draw_normal_objects_select_box(objptr selectobj);
extern void normal_draw_technique(objptr thisobj, stateptr state);
extern void draw_world_obj(objptr thisobj, GLboolean draw_everything, stateptr state);
extern void draw_world_objects(worldptr world, GLboolean draw_everything, stateptr state);
extern void draw_slide_obj(objptr thisobj, GLboolean draw_everything, stateptr state);
extern void draw_slide_objects(worldptr world, GLboolean draw_everything, stateptr state);
extern void pushpop_slide(GLboolean showslide);
extern void set_camera_view(vertype direction, vertype eyept);
extern void draw_underlying_objects(stateptr state, stateptr oldstate);
extern void draw_gl_objects(stateptr state);
extern void draw_worlds(GLboolean draw_everything);
extern void grafx_resize(int main_winWidth, int main_winHeight);
extern void clear_view(void); /* Added for win port */

/* hack debug routine */
extern void display_cursor_move(stateptr state,vertype cursor_delta);
// FIX ME!!! LJE
// extern void redraw_grafx(HDC hDC);

/* hershey.c */

extern char_rec char_lookups[];

extern void del_def(void);
extern void tilde_def(void);
extern void doublequote_def(void);
extern void quote_def(void);
extern void leftbrace_def(void);
extern void rightbrace_def(void);
extern void tick_def(void);
extern void comma_def(void);
extern void period_def(void);
extern void slash_def(void);
extern void colon_def(void);
extern void semicolon_def(void);
extern void lessthan_def(void);
extern void greaterthan_def(void);
extern void questionmark_def(void);
extern void exclamation_def(void);
extern void A_def(void);
extern void B_def(void);
extern void C_def(void);
extern void D_def(void);
extern void E_def(void);
extern void F_def(void);
extern void G_def(void);
extern void H_def(void);
extern void I_def(void);
extern void J_def(void);
extern void K_def(void);
extern void L_def(void);
extern void M_def(void);
extern void N_def(void);
extern void O_def(void);
extern void P_def(void);
extern void Q_def(void);
extern void R_def(void);
extern void S_def(void);
extern void T_def(void);
extern void U_def(void);
extern void V_def(void);
extern void W_def(void);
extern void X_def(void);
extern void Y_def(void);
extern void Z_def(void);
extern void a_def(void);
extern void b_def(void);
extern void c_def(void);
extern void d_def(void);
extern void e_def(void);
extern void f_def(void);
extern void g_def(void);
extern void h_def(void);
extern void i_def(void);
extern void j_def(void);
extern void k_def(void);
extern void l_def(void);
extern void m_def(void);
extern void n_def(void);
extern void o_def(void);
extern void p_def(void);
extern void q_def(void);
extern void r_def(void);
extern void s_def(void);
extern void t_def(void);
extern void u_def(void);
extern void v_def(void);
extern void w_def(void);
extern void x_def(void);
extern void y_def(void);
extern void z_def(void);
extern void backslash_def(void);
extern void leftcurly_def(void);
extern void rightcurly_def(void);
extern void bar_def(void);
extern void pound_def(void);
extern void dollar_def(void);
extern void percent_def(void);
extern void caret_def(void);
extern void ampersand_def(void);
extern void leftparen_def(void);
extern void rightparen_def(void);
extern void asterisk_def(void);
extern void underscore_def(void);
extern void plus_def(void);
extern void minus_def(void);
extern void zero_def(void);
extern void one_def(void);
extern void two_def(void);
extern void three_def(void);
extern void four_def(void);
extern void five_def(void);
extern void six_def(void);
extern void seven_def(void);
extern void eight_def(void);
extern void nine_def(void);
extern void equal_def(void);
extern void at_def(void);
extern void delta_def(void);
extern GLfloat hershey_Xheight(void);
extern GLfloat hershey_descender_height(void);
extern GLfloat hershey_height(void);

/* init.c */

extern void def_lighting_props(void);
extern void setMaterial(void);
extern void turn_on_lighting(void);
extern void setup_light_source(int lightnumber, vertype lightpos, colortype lightcolor);
extern void switch_light_source(int lightnumber, GLboolean turn_it_on);
extern void read_window_resource_files(void);
extern void init_main_window(void);
extern void init_state(void);
extern void init_global_lists(void);
extern void init_environment(void);
extern void init_other_worlds(void);
extern void init_primary_world(void);
extern void attach_otherworlds_CPLcode(void);
extern void init_3form(void);
extern void setCheckTexture(void);
extern void matrixIdentity(GLfloat m[4][4]);
// FIX ME -- LJE
// extern void setupPixelFormat(HDC hDC);
extern void init_grafx(void);

/* intersect.c */

extern void set_obj_evf_eqns(objptr thisobj);
extern void set_shell_evf_cutpts(shellptr thishell, stateptr state);
extern void set_obj_evf_cutpts(objptr thisobj, stateptr state);
extern void prune_edgelist(listptr edgelist);
extern void add_segment(listptr seglist, leptr le1, leptr le2);
extern void add_oncut_edges(listptr seglist, leptr thisle);
extern void make_obj_ringlist(objptr thisobj);
extern void clear_obj_ringlist(objptr thisobj);
extern Boolean must_make_obj_ringlist(objptr thisobj, stateptr state, stateptr oldstate);

/* list.c */

#ifndef FAST
extern elemptr next_elem(listptr thishead, elemptr thisptr);
extern elemptr prev_elem(listptr thishead, elemptr thisptr);
#endif
extern int get_elem_index(elemptr the_elem, listptr the_list);
extern elemptr get_elem_ptr(int i, listptr the_list);
extern void add_elem(listptr the_list, elemptr prev_elem, elemptr this_elem);
extern void insert_elem(listptr thelist, elemptr nextelem, elemptr thelem);
extern void append_elem(listptr the_list, elemptr thiselem);
extern void rem_elem(listptr the_list, elemptr the_elem);
extern void move_elem(register listptr the_list, register elemptr the_elem, elemptr new_prev_elem);
extern elemptr alloc_elem(int elemsize);
extern elemptr create_blank_elem(int type_id);
extern elemptr add_new_blank_elem(listptr the_list, elemptr prev_elem, int type_id);
extern elemptr insert_new_blank_elem(listptr the_list, elemptr next_elem, int type_id);
extern elemptr append_new_blank_elem(listptr the_list, int type_id);
extern void copy_elem(elemptr sourcelem, elemptr destelem);
extern elemptr clone_elem(elemptr original);
extern void set_elem_fields(elemptr new_elem, long int *firstfield);
extern elemptr create_elem(int type_id, long int firstfield);
extern elemptr add_new_elem(listptr the_list, elemptr prev_elem, int type_id, long int firstfield);
extern elemptr insert_new_elem(listptr the_list, elemptr next_elem, int type_id, long int firstfield);
extern elemptr append_new_elem(listptr the_list, int type_id, long firstfield);
extern Boolean already_in_featurelist(listptr check_list, featureptr check_feature);
extern void add_to_featurelist(listptr add_list, featureptr new_feature);
extern void merge_lists_uniquely(listptr list1, listptr list2);
extern void remove_from_featurelist(listptr remove_list, featureptr removed_feature);
extern void free_elem(elemptr *thiselem);
extern void del_elem(listptr the_list, void *this_elem);
extern void kill_elem(void *this_elem);
extern listptr create_list(void);
extern listptr create_circular_list(void);
extern void clear_list(listptr the_list);
extern void del_list(listptr the_list);
extern listptr transfer_list_contents(listptr sourcelist, listptr destlist);
extern listptr merge_lists(listptr sourcelist, listptr destlist);
extern listptr clone_list(listptr old_list);
extern listptr append_list(listptr sourcelist, listptr destlist);
extern void copy_list(listptr sourcelist, listptr destlist);
extern Boolean elem_has_proplist(int elem_type);
extern Boolean elem_qtest(listptr elemqueue);
extern void queue_elem(listptr elemqueue, elemptr new_qelem);
extern elemptr unqueue_elem(listptr elemqueue);

/* math3d.c */

extern int sign(int number);
extern float fsign(float number);
extern int iround(float number);
extern float fround(float number);
extern int compare(double a, double b, double tol);
extern void setscreenpos(screenpos pos, int x, int y);
extern Boolean pos_equal(vertype v1, vertype v2);
extern void setpos3d(float *pos, Coord x, Coord y, Coord z);
extern void copypos3d(vertype v1, vertype v2);
extern void addpos3d(register vertype pos, register vertype delta, vertype result);
extern void diffpos3d(vertype pt1, vertype pt2, vertype result);
extern void setvec3d(vertype vec, Coord x, Coord y, Coord z);
extern void makevec3d(vertype pt1, vertype pt2, vertype result);
extern void copyvec3d(vertype v1, vertype v2);
extern void addvec3d(vertype vec1, vertype vec2, vertype result);
extern void diffvec3d(vertype pt1, vertype pt2, vertype result);
extern void multvec3d(vertype vec1, vertype vec2, vertype result);
extern void scalevec3d(vertype vec1, float scalefactor, vertype result);
extern Coord dotprod(Coord ax, Coord ay, Coord az, Coord bx, Coord by, Coord bz);
extern Coord dotvecs(vertype vec1, vertype vec2);
extern void cross_prod(float *v1, float *v2, float *v1Xv2);
extern Coord mag(Coord deltax, Coord deltay, Coord deltaz);
extern Coord magvec(vertype thevec);
extern void flip_vec(vertype thevec);
extern void force_vec_up(vertype thevec, int axis);
extern Boolean vector_near_zero(float *thevec, double zerotol);
extern Boolean vec_eq_zero(float *thevec);
extern Boolean vectors_nearly_equal(float *thevec1, float *thevec2, double zerotol);
extern Coord distance(vertype pt1, vertype pt2);
extern Coord distance2(vertype pt1, vertype pt2);
extern void norm2pt(vertype p1, vertype p2, vertype p3);
extern void normalize(vertype v1, vertype v2);
extern void midpt(Coord x1, Coord y1, Coord z1, Coord x2, Coord y2, Coord z2, Coord *midx, Coord *midy, Coord *midz);
extern void midpoint(vertype pt1, vertype pt2, vertype mid_pt);
extern void transpose_matrix(Matrix thematrix, Matrix thetranspose);
extern void copy_matrix(Matrix a, Matrix b);
extern void multmatrix_2x2(Matrix m1, Matrix m2, Matrix m3);
extern Boolean invertmatrix_2x2(Matrix m, Matrix minv);
extern void multmatrix_4x4(Matrix matrixa, Matrix matrixb, Matrix matrixc);
extern Boolean matrices_equal(Matrix a, Matrix b, double tol);
extern void swap_ints(int *val1, int *val2);
extern void swap_coords(Coord *val1, Coord *val2);
extern void swap_verts(vertype pt1, vertype pt2);
extern void gaussian_elim(Matrix elimatrix);

/* mirror.c */

extern void mirror_obj(worldptr world, objptr mirror_obj, fveptr mirror_fve, stateptr state);

/* normals.c */

extern void flip_bound_orientation(boundptr thisbound);
extern Boolean cclockwise_bound(boundptr thisbound);
extern Boolean cclockwise(fveptr thisfve);
extern void calc_face_norm(fveptr thisfve, vertype facenorm);
extern void find_normals_guts(shellptr this_shell, fveptr thisfve);
extern leptr find_top_edge(shellptr the_shell, vfeptr vfemax);
extern Boolean sameside(vertype othervec[], vertype norm);
extern void fix_direction(vertype facenorm[], vertype othervec[]);
extern leptr find_first_norms(shellptr thishell);
extern void clear_normals(shellptr thishell);
extern void find_obj_normals(objptr thisobj);
extern void find_branch_normals(objptr thisobj);
extern void find_normals(listptr shell_list);
extern void compute_planedist(fveptr thisfve);
extern void compute_shell_planedists(shellptr thishell);
extern void compute_obj_planedists(objptr thisobj);
extern void invert_shell(shellptr thishell);

/* objtree.c */

extern void reset_motherworld_ptr(objptr reset_obj, worldptr new_world);
extern void transfer_moved_obj_props(worldptr oldworld, worldptr neworld);
extern void insert_parent(objptr child, objptr newparent);
extern void add_child(objptr parent, objptr newchild);
extern void swap_child_with_parent(objptr child);
extern objptr remove_obj(objptr removed_obj);
extern objptr remove_obj_branch(objptr removed_obj);
extern void adopt_obj_branch(objptr master, objptr slave, Boolean linked);
extern void adopt_obj(objptr master, objptr slave, Boolean linked);
extern void get_obj_shells(objptr this_obj, listptr cull_list);
extern void get_descendents(objptr this_obj, objptr mother_obj, listptr cull_list);
extern objptr get_unlinked_ancestor(objptr thisobj);
extern void apply_to_descendants(objptr thisobj, void (*thefunction)(void), void *function_data);
extern void apply_to_unlinked_descendants(objptr thisobj, void (*thefunction)(void), void *function_data);
extern void apply_to_descendants_with_prop(objptr thisobj, property chosen_prop, void (*thefunction)(void), void *function_data);

/* pencilsketch.c */

extern double atan_rectify(double satan);
extern vfeptr other_vfe(evfptr thisevf, vfeptr thisvfe);
extern elemptrptr find_ccwedge(vfeptr connectvfe, evfptr anchorevf);
extern void init_sle(sketchleptr newsle, evfptr newevf, vfeptr thesketchvert);
extern vfeptr new_sketchvfe(listptr sketchvfelist, vertype vertpt, evfptr newevf, shellptr newshell);
extern void rem_edge_from_vfe(vfeptr thisvfe, evfptr thisevf);
extern void add_edge_to_vfe(vfeptr thisvfe, evfptr thisevf);
extern evfptr make_edge(listptr sketchvfelist, listptr sketchevflist, vertype drawpt1, vertype drawpt2, vfeptr drawvfe1, vfeptr drawvfe2);
extern void fix_sles(evfptr thisevf);
extern evfptr break_edge(evfptr thisevf, vfeptr oldbreakvfe, vertype breakpos, listptr sketchvfelist, listptr sketchevflist);
extern vfeptr pt_near_sketch(vertype newdrawpt, vertype nearvert, evfptr *nearedge, listptr sketchvfelist, listptr sketchevflist);
extern void draw_sketch_box(stateptr state, colortype thiscolor, GLboolean started_sketch);
extern void rubber_band(stateptr state, colortype thiscolor);

/* pick.c */

extern void clear_pickedtranslates(void);
extern void unpick_feature(featureptr pickfeature);
extern void pick_feature(featureptr pickfeature);
extern void prune_pickedlist(stateptr state);

/* polhemus.c */

#ifdef IRIS
extern void init_polhemus_serialport(void);
extern void close_polhemus_serialport(void);
extern void write_polhemus_initialization(void);
extern void write_polhemus_request(void);
extern void read_polhemus_bytes(char buf[], int nbytes);
extern GLboolean read_polhemus_status(void);
extern void read_polhemus_trailer(void);
extern void make_polh_base_xform(void);
extern void make_polhemus_rotatematrix(GLfloat *transformatrix);
extern void read_polhemus(polhemusvalptr polhvals);
extern void update_polh_xform(void);
extern GLboolean update_polhemus_state(void);
extern void init_polhemus(void);
extern void quit_polhemus(void);
extern void encode_polhemus_roomrot(stateptr state);
extern void encode_polhemus_cursor_move(stateptr state, glovestatekind glove_state, GLboolean dragging);
extern void encode_polhemus_transform(stateptr state);
#endif

/* primitives.c */

extern prim_create_rec prim_create_recs[];

extern objptr create_isosceles_triangle(worldptr world, GLfloat trianglesize);
extern objptr create_isosceles_prism(worldptr world, GLfloat prismsize, GLfloat prismdepth);
extern objptr create_cubehole(worldptr world);
extern objptr create_square(worldptr world, GLfloat width, GLfloat height);
extern objptr create_cube(worldptr world, GLfloat width, GLfloat height, GLfloat length);
extern objptr recreate_cube(objptr oldcube, GLfloat length, GLfloat width, GLfloat height);
extern objptr create_pyramid(worldptr world, GLfloat width, GLfloat height, GLfloat depth);
extern objptr create_sphere(worldptr world, double radius, int numincrements);
extern objptr create_oval(worldptr world, GLfloat radius_a, GLfloat radius_b, int numincrements);
extern objptr create_oval_cylinder(worldptr world, GLfloat radius_a, GLfloat radius_b, GLfloat depth, int numincrements);
extern objptr create_oval_cone(worldptr world, GLfloat radius_a, GLfloat radius_b, GLfloat depth, int numincrements);
extern objptr create_paraboloid(worldptr world, GLfloat width, GLfloat height, GLfloat thickness, int numpoints);
extern objptr create_donut(worldptr world, GLfloat radius, int numincrements);
extern objptr create_lbeam(worldptr world, GLfloat width, GLfloat height, GLfloat thickness, GLfloat depth);
extern objptr create_prism(worldptr world);
extern objptr create_tbeam(worldptr world, GLfloat width, GLfloat height, GLfloat thickness, GLfloat depth);
extern objptr create_chair(worldptr world, GLfloat width, GLfloat height, GLfloat depth);
extern objptr create_tableg(worldptr world, vertype delta);
extern objptr create_table(worldptr world, GLfloat width, GLfloat height, GLfloat depth);
extern objptr create_vase(worldptr world, double xscale, double yscale, int numfaces);

/* properties.c */

extern listptr *determine_world_proplist(featureptr thisfeature);
extern void init_proplists(listptr *proplist_array);
extern Boolean has_property(featureptr thisfeature, property searchpropkind);
extern featureptr get_property_feature(worldptr world, property thisproperty, int n);
extern alltypes get_property_val(featureptr thisfeature, property thispropkind);
extern void set_prop_flags(featureptr thisfeature, property setpropkind);
extern void add_property(featureptr thisfeature, property newpropkind);
extern void set_property(elemptr thisfeature, property thepropkind, long value);
extern void set_any_property(elemptr thisfeature, property thepropkind, union anydata value);
extern void clear_prop_flags(featureptr thisfeature, property doomedpropkind);
extern void del_property(featureptr thisfeature, property doomedpropkind);
extern void clear_property(property doomedpropkind);
extern void add_property_to_descendants(featureptr thisfeature, property newpropkind);
extern void del_property_from_descendants(featureptr thisfeature, property doomedpropkind);
extern int find_property(char *property_name);
extern int define_new_property(char *prop_name, CPL_word_ptr prop_code_address);
extern void define_property(char *prop_name, CPL_word_ptr prop_code_address);
extern void clear_feature_properties(featureptr thisfeature);
extern void transfer_feature_properties(featureptr sourcefeature, featureptr destfeature);
extern void copy_feature_properties(featureptr sourcefeature, featureptr destfeature);

/* psprint.c */

extern float stacknum(void);
extern int stackint(void);
extern char *stackstr(void);
extern char *stacksym(void);
extern void ps_int(int number);
extern void ps_num(float number);
extern void ps_sym(char *thesymbol);
extern void ps_str(char *thestring);
extern void ps_add(float x, float y);
extern void ps_arc(float x, float y, float r, float startangle, float stopangle);
extern void ps_circle(float x, float y, float r);
extern void ps_closepath(void);
extern void ps_currentpoint(void);
extern void ps_def(char *key);
extern void ps_div(float x, float y);
extern void ps_dup(void);
extern void ps_exch(void);
extern void ps_fill(void);
extern void ps_findfont(char *fontname);
extern void ps_gsave(void);
extern void ps_grestore(void);
extern void ps_idiv(float x, float y);
extern void ps_initgraphics(void);
extern void ps_initmatrix(void);
extern void ps_itransform(float x, float y);
extern void ps_lineto(float x, float y);
extern void ps_moveto(float x, float y);
extern void ps_mod(int x, int y);
extern void ps_mul(float x, float y);
extern void ps_neg(void);
extern void ps_newpath(void);
extern void ps_pop(void);
extern void ps_rand(void);
extern void ps_rectangle(float xmin, float ymin, float xmax, float ymax);
extern void ps_rlineto(float x, float y);
extern void ps_rmoveto(float dx, float dy);
extern void ps_rotate(float angle);
extern void ps_round(float num);
extern void ps_rrectangle(float xmin, float ymin, float xmax, float ymax);
extern void ps_scale(float xscale, float yscale);
extern void ps_scalefont(char *fontname, float scale);
extern void ps_setfont(char *fontname);
extern void ps_setgray(float graylevel);
extern void ps_setlinejoin(int joinstyle);
extern void ps_setlinewidth(float width);
extern void ps_show(char *thestring);
extern void ps_showpage(void);
extern void ps_stringwidth(char *thestring);
extern void ps_stroke(void);
extern void ps_sub(float x, float y);
extern void ps_transform(float x, float y);
extern void ps_translate(float xpos, float ypos);
extern FILE *open_psfile(char *filename);
extern void set_psfile(FILE *thefile);
extern void close_psfile(void);
extern char *time_stamp(void);
extern void make_border(void);
extern void make_title(char *filename, int orientation);
extern int page_setup(float xmin, float xmax, float ymin, float ymax, float xmargin, float ymargin);
extern void set_projection(vertype eyept, Coord projplanez);
extern void set_lighting(vertype lightvec);
extern void print_psfile(char *filename);
extern void plot_fve(fveptr thisfve);
extern void plot_fve_outline(fveptr thisfve);
extern void plot_shell(shellptr thishell);
extern void plot_object(objptr thisobj, char *filename);
extern void sort_objects(listptr objlist, int axis);
extern void plot_obj_branch_engine(objptr rootobj);
extern void plot_obj_branch(objptr rootobj, char *filename);
extern void label_null_le(float x, float y, char *label1, char *label2, int orientation);
extern void label_null_strut(float x, float y, char *label1, char *label2, char *label3, int orientation);
extern void label_le(float x, float y, char *thelabel, int orientation);
extern void make_axes(int axis1, int axis2, int orientation);
extern void itoa(int num, char *numstring);
extern Boolean le_isnull(leptr thisle, int axis1, int axis2);
extern int get_le_vfeid(leptr thisle);
extern void plotfaceles(fveptr thisfve, char *filename);

/* rotateobj.c */

extern void make_axisrot_matrix(vertype rotvec, vertype rotpt, int rotangle, Matrix axisrotmat, Matrix invaxisrotmat);
extern void axis_rot_obj_engine(objptr rotateobj, rotateinfoptr rotateinfo);
extern void axis_rot_obj(objptr rotateobj, vertype rotvec, vertype rotpt, int rotangle);
extern void inplane_axis_rot_obj_engine(objptr rotateobj, vertype rotvec, vertype rotpt, int rotangle);
extern int snap_face_to_plane(vertype rotaxis, vertype fnorm, int delangle);
extern int snap_vector_to_vector(vertype adjustvec, vertype snapvec, vertype rotaxis);
extern int snap_face_to_plane_perp(vertype rotaxis, vertype fnorm);

/* rotatetool.c */

extern void give_children_property(objptr thisobj, int thisprop);
extern void show_xbar(objptr xbar);
extern void hide_xbar(objptr xbar, GLboolean hide_baubles);
extern void show_all_xbars(void);
extern void reset_rotatetool(void);
extern void create_rotatetool(worldptr world);
extern bboxscreenptr find_rotatetool_screenbbox(void);
extern void attach_rotatetool_CPLcode(void);
extern void point_vec_up(vertype pt1, vertype pt2, vertype inplane_axis);
extern void create_xbar_rotrec(evfptr motherxbar, CPLauxptr rotaterec, CPLauxptr transrec, CPLauxptr snappedrec, CPL_word_ptr paused_rec, CPL_word_ptr inplane_record, CPL_word_ptr outofplane_record);
extern void CPL_xbar_rotate(listptr world_list, stateptr state);
extern int compute_snapped_rotangle(vertype normvec1, vertype normvec2, vertype snapvec, int snapangle, int maxsnaprange, vertype rotzaxis, GLboolean *snapped_bauble);
extern void CPL_bauble_rotate(listptr world_list, stateptr state);
extern void orient_rotatetool(objptr selectobj, vertype pt1, vertype pt2);
extern void CPL_snap_rotatetool(listptr world_list, stateptr state);
extern void setup_other_zaxis(objptr motherselectable, evfptr other_zaxis_evf);
extern void CPL_setup_rotatetool(listptr world_list, stateptr state);

/* scale.c */

extern void calculate_scale_factors(vertype scale_center, vertype oldposition, vertype newposition, vertype scalefactor);
extern void scale_vfe(vfeptr thisvfe, Matrix scalefactor);
extern void scale_evf(evfptr thisevf, Matrix scalefactor);
extern void scale_fve(fveptr thisfve, Matrix scalefactor);
extern void scale_shell(shellptr thishell, Matrix scalefactor);
extern void scale_obj(objptr thisobj, Matrix scalefactor, Matrix invscalefactor);
extern void scale_feature(featureptr thisfeature, vertype scalefactor, vertype scaleorigin);
extern void create_scalebbox(worldptr world);
extern void create_sizebbox(worldptr world);

/* select.c */

extern Coord global_picksize;
extern Boolean always_set_selectable;

extern GLboolean cursor_in_shell(shellptr thishell, stateptr state);
extern void set_normal_selectbox_technique(selectableptr selectable_record);
extern void set_cursor_by_selectable(selectableptr selectable_record);
extern int determine_selectable(shellptr thishell, stateptr state, selectableptr foundselect);
extern void set_object_selectable(objptr thisobj, stateptr state);
extern void set_feature_selectable(featureptr thisfeature, stateptr state);
extern void set_oneworld_selectables(worldptr world, stateptr state);
extern void select_obj(objptr selected_obj);
extern void unselect_obj(objptr unselected_obj);
extern GLboolean stationary(stateptr oldstate, stateptr state);
extern GLboolean accelerating(stateptr oldstate, stateptr state);
extern GLboolean accelerating_z(stateptr oldstate, stateptr state);
extern void set_selectables(listptr world_list, stateptr state, stateptr oldstate);
extern void set_selectables_definite(listptr world_list, stateptr state, stateptr oldstate);
extern void set_global_picksize(GLfloat newpicksize);
extern void set_always_selectable(void);
extern void set_cutplane_selectable(void);
extern void set_room_selectable(void);
extern void set_rotate_innercircle_selectable(objptr inner_circle, stateptr state);
extern void set_rotatexbar_selectable(objptr rotate_xbar, stateptr state);
extern void set_rotate_outercircle_selectable(objptr outer_circle, stateptr state);
extern void set_systemcursor_selectable(void);
extern void set_toolpad_selectable(objptr toolpad, stateptr state);
extern void set_headpad_selectable(objptr headpad, stateptr state);
extern void set_normalpad_selectable(objptr normalpad, stateptr state);
extern void set_scalebbox_selectable(objptr scalebbox, stateptr state);

/* shade.c */

extern void init_acos_table(void);
extern void make_face_into_light(fveptr thisfve, double brightness);
extern void compute_reflected_color(vertype facenorm, colortype reflectivity, colortype reflectedcolor);
extern void compute_highlighted_face_color(vertype facenorm, colortype reflectivity, colortype totalcolor);
extern void draw_flatshaded_face(fveptr thisfve);
extern void draw_flatshaded_shell(shellptr thishell, stateptr state);
extern void draw_flatshaded_obj(objptr thisobj, stateptr state);
extern void draw_software_shaded_face(fveptr thisfve);
extern void draw_software_shaded_shell(shellptr thishell);
extern void draw_software_shaded_obj(objptr thisobj);

/* slice.c */

extern listptr separate_shell_into_pieces(shellptr thishell, listptr frontcutfaces);
extern void make_piece_into_shell(shellptr sourceshell, fveptr startfve, listptr newshellist);
extern GLboolean already_connected(leptr le1, leptr le2);
extern void slice_shell(shellptr thishell, stateptr state, listptr rearshellist, listptr frontshellist);
extern void slice_object(objptr thisobj, stateptr state, listptr frontobjlist, listptr rearobjlist);

/* split.c */

extern void splitup_edge(evfptr thisevf, int numsplits);
extern void splitup_edges(shellptr thishell, int numsplits);
extern leptr split_face_at_vert(fveptr thisfve, vertype centerpos);
extern void fan_split_fve_fromle(fveptr thisfve, leptr apexle);
extern void fan_split_fve(fveptr thisfve);
extern void zigzag_split_fve(fveptr thisfve);
extern void simple_face_split(fveptr thisfve, leptr apexle);
extern void complete_triangularization(fveptr thisfve);
extern leptr find_closest_vertex(leptr testle, fveptr thisfve);
extern void find_delauney_edge(fveptr thisfve);
extern void triangularize_fve(fveptr thisfve);
extern void triangularize_shell(shellptr thishell);
extern Boolean planarize_face(fveptr thisfve, leptr thisle);
extern void planarize_affected_fves(featureptr thisfeature, vertype deltapos);
extern void planarize_shell(shellptr thishell);
extern void planarize_object(objptr thisobj);
extern Boolean coplanar(fveptr fve1, fveptr fve2);
extern void recombine_coplanar_faces(objptr thisobj);
extern void dumb_recombine_coplanar_faces(objptr thisobj);
extern fveptr remove_coplanar_surface_facets(surfptr thisurf);
extern void regen_surface_fvelist_engine(fveptr seedfve);
extern void regen_surface_fvelist_from_seed(fveptr seedfve);
extern leptr next_surfle_cyclical(leptr thisle);
extern leptr prev_surfle_cyclical(leptr thisle);
extern void recombine_affected_coplanar_fves(featureptr thisfeature);

/* storage_cutplane.c */

extern void load_light(worldptr world);
extern void load_stocks(worldptr world);
extern void load_pencil(worldptr world);
extern void load_colors(worldptr world);
extern void load_storagepad(worldptr world);
extern void load_lbeam(worldptr world);
extern void load_prims(worldptr world);
extern void load_goodies(worldptr world);
extern void load_toolbar(worldptr world);
extern void load_arrangebar(worldptr world);
extern void load_piechart(worldptr world);
extern void load_measurepad(worldptr world);
extern void load_slidepad(worldptr world);
extern void load_arrowpad(worldptr world);
extern void load_rotatetool(worldptr world);
extern Boolean special_file(char *infile_name);
extern techniquerecptr find_technique(char *techniquename, techniquerecptr techniques);
extern char *find_technique_name(void (*techniquertn)(void), techniquerecptr techniques);
/* eventually, put me in topology.c */
extern void append_evf(listptr the_head, leptr le1ptr, leptr le2ptr, fveptr face1, fveptr face2);
extern void read_matrix(FILE *infile, Matrix thematrix);
extern void read_vfes(FILE *infile, listptr thevfelist, shellptr thishell);
extern void read_bounds(FILE *infile, shellptr thishell, fveptr thisfve);
extern void read_fves(FILE *infile, shellptr thishell);
extern leptr find_leptr(shellptr thishell, fveptr f1, vfeptr v1, vfeptr v2);
extern void find_edge_faceles(shellptr thishell, vfeptr vfe1, vfeptr vfe2, fveptr *fve1, fveptr *fve2, leptr *evfle1, leptr *evfle2);
extern leptr find_other_le(shellptr thishell, fveptr currentfve, leptr thisle);
extern void make_shell_evfs(shellptr thishell);
extern void read_evfs(FILE *infile, int numedges, shellptr thishell);
extern void read_feature_props(FILE *infile, featureptr thisfeature, int numprops);
extern void read_face_colors(FILE *infile, shellptr thishell, int numfacecolors);
extern void read_shell_properties(FILE *infile, shellptr thishell);
extern void get_CPLprivatecode(FILE *infile, int featurenum, listptr featurelist);
extern void read_shell_CPLcode(FILE *infile, shellptr thishell);
extern void read_shell_specials(FILE *infile, shellptr thishell);
extern void read_shell(FILE *infile, shellptr thishell);
extern void read_shells(FILE *infile, objptr newobj, int numshells);
extern Boolean find_object_by_number(objptr search_obj, objptr *foundobj, int search_object_id, int *search_id);
extern char *reset_white_space(char *read_name);
extern objptr read_object(FILE *infile, worldptr world);
extern void read_branch(FILE *infile, objptr parentobj);
extern void read_world(FILE *infile);
extern void resolve_decals(void);
extern FILE *find_input_file(char *infile_name);
extern Boolean read_data_file(char *infile_name, featureptr parentfeature);
extern void read_decal_file(objptr parent, char *decalfile_name);
extern void write_matrix(FILE *outfile, Matrix thematrix);
extern void write_vfes(FILE *outfile, listptr vfelist);
extern void write_evfs(FILE *outfile, shellptr thishell);
extern void write_bounds(FILE *outfile, fveptr this_fve, shellptr thishell);
extern void write_fves(FILE *outfile, shellptr thishell);
extern void write_feature_props(FILE *outfile, featureptr thisfeature);
extern void write_face_colors(FILE *outfile, shellptr thishell);
extern void write_shell_properties(FILE *outfile, shellptr thishell);
extern void write_CPLprivatecode_name(FILE *outfile, featureptr thisfeature);
extern void write_shell_CPLcode(FILE *outfile, shellptr thishell);
extern void write_specials(FILE *outfile, shellptr thishell);
extern void write_shell(FILE *outfile, shellptr thishell);
extern void write_shells(FILE *outfile, objptr thisobj);
extern Boolean find_object_number(objptr thisobj, objptr searchobj, int *search_id, int *found_id);
extern void count_objects(objptr grouproot, objptr thisobj, int *count);
extern char *fix_white_space(char *orig_name);
extern void write_object(FILE *outfile, objptr thisobj);
extern void write_objects(FILE *outfile, worldptr world, objptr thisobj);
extern void write_branch(objptr branchroot, FILE *outfile);
extern void write_world(worldptr world, FILE *outfile);
extern FILE *create_output_file(char *outfile_name);
extern void write_data_file(char *outfile_name, featureptr thefeature);
extern Boolean write_dxf_head(FILE *outfile);
extern void write_dxf_tail(FILE *outfile);
extern void write_dxf_vertex(FILE *outfile, vertype thisvert);
extern void write_dxf_polyline(FILE *outfile, boundptr thisbound);
extern void write_dxf_face(FILE *outfile, boundptr thisbound);
extern void write_dxf_shell(FILE *outfile, shellptr thishell);
extern void write_dxf_object(FILE *outfile, objptr thisobj);
extern void write_dxf_objects(FILE *outfile, worldptr world, objptr thisobj);
extern void write_dxf_file(char outfile_name[], worldptr world);
extern void read_plate_labels(objptr plate, int num_labels, FILE *plate_rscfile);
extern void read_plate_textinputs(objptr plate, int num_textinputs, FILE *plate_rscfile);
extern objptr read_plate(worldptr world, objptr parent, char *plate_rscfilename);

/* text.c */

extern char textstring_styles[Maxstringstyle][Maxstringstylelength];

extern GLfloat compute_insert_height(textfontype thefont, GLfloat yscale);
extern void compute_insert_pos(char *thestring, int insert_pt, vertype insert_pos);
extern void draw_insertion_point(textfontype thefont, textstringptr textstring);
extern void draw_obj_label(objptr labelled_obj);
extern GLfloat text_string_width(char *thestring, textfontype thefont, GLfloat string_x_scale);
extern textstringptr create_text_string(char *new_string, vertype string_offset, textfontype initial_font, GLfloat string_x_scale, GLfloat string_y_scale, GLfloat string_pen_width, colortype string_color, stringstyletype string_style);
extern textstringptr find_label_item(objptr labelled_obj, int item_number);
extern char *get_label_item(objptr labelled_obj, int item_number);
extern void set_label_item(objptr labelled_obj, int item_number, char *new_string);
extern void set_label_item_style(objptr labelled_obj, int item_number, stringstyletype new_style);
extern void set_label_item_offset(objptr labelled_obj, int item_number, vertype new_offset);
extern void set_label_item_scale(objptr labelled_obj, int item_number, GLfloat new_x_scale, GLfloat new_y_scale);
extern void set_label_item_pen_width(objptr labelled_obj, int item_number, int new_pen_width);
extern void set_label_item_color(objptr labelled_obj, int item_number, colortype new_color);
extern void set_label_insertion_point(objptr labelled_obj, int item_number, int new_insertion_point);
extern void move_label_insertion_point(objptr labelled_obj, int item_number, int offset);
extern void insert_string_into_label(objptr labelled_obj, int item_number, char *insertstring);
extern void remove_chars_from_label(objptr labelled_obj, int item_number, int startindex, int endindex);
extern textgrouptr create_label(objptr labelled_obj, char *initial_string, vertype initial_offset, textfontype initial_font);
extern void add_label_item(objptr labelled_obj, char *initial_string, vertype initial_offset, textfontype initial_font);
extern void delete_label_item(objptr labelled_obj, int item_number);
extern void objstr(char *thestring, vertype pos, GLfloat x_scale, GLfloat y_scale);

/* tools.c */

extern vertype penciloffset,toolholderoffset,pencilvect,pencilvect2;

extern void apply_front_decal(objptr decal_obj, objptr dest_obj, GLfloat decal_thickness, GLfloat decal_forward_offset, GLfloat decal_margin);
extern void read_decal_obj(objptr apply_obj, char *decalobj_filename);
extern objptr create_hammer(worldptr world, colortype hammercolor, colortype handlecolor, GLfloat hammerheadwidth, GLfloat hammerheadheight, GLfloat hammerheadepth, GLfloat hammerhandleheight);
extern objptr create_sketch_tool(worldptr world, vertype offset, char *pencilcolor, char *erasercolor);
extern objptr create_slice_tool(worldptr world, vertype offset, char *bladecolor, char *handlecolor);
extern objptr create_magnet(worldptr world, GLfloat magnetradius, GLfloat barthickness, GLfloat magnetdepth, int magnetresolution, colortype bodycolor, colortype endscolor);
extern objptr create_magnifier(worldptr world, GLfloat lensradius, GLfloat holeradius, int lensresolution, colortype lenscolor, GLfloat handlewidth, GLfloat handleheight, colortype handlecolor);
extern objptr create_fencepart(worldptr world, GLfloat fencepartwidth, GLfloat fencepartheight, GLfloat fencepartdepth, colortype fencepartcolor, int numposts);
extern objptr create_fence(worldptr world, GLfloat fencewidth, GLfloat fenceheight, GLfloat fencedepth, colortype fencecolor);
extern void create_grouped_littlemen(worldptr world);
extern void create_ungrouped_littlemen(worldptr world);
extern objptr create_spotlight(worldptr world, GLfloat spotradius, GLfloat spotdepth, colortype litfacecolor, colortype bodycolor);
extern objptr create_lightbulb(worldptr world, colortype bulbcolor, colortype basecolor);
extern objptr create_nikon(worldptr world, GLfloat width, GLfloat height, GLfloat depth, GLfloat lensradius, int lensresolution, colortype bodycolor, colortype lenscolor, colortype windercolor, colortype capcolor);
extern objptr create_movie_camera(worldptr world, GLfloat width, GLfloat height, GLfloat depth, int lensresolution, int reelresolution, colortype bodycolor, colortype lenscolor, colortype reelscolor);
extern objptr create_floppy(worldptr world, colortype floppycolor, colortype labelcolor, colortype doorcolor);
extern objptr create_storage_pad(worldptr world, colortype storagepadcolor);
extern objptr create_ruler(worldptr world, GLfloat width, GLfloat height, GLfloat depth, int numnotches, colortype rulercolor);
extern objptr create_measure_pad(worldptr world, colortype measurepadcolor);
extern objptr create_drill_tool(worldptr world, GLfloat width, GLfloat height, GLfloat drillbitlength, int drillresolution, colortype drillcolor, colortype bitcolor);
extern objptr adjust_toolbar(objptr toolbar);
extern objptr create_toolbar(worldptr world, char *rscfilename);
extern objptr create_arrow(worldptr world, GLfloat width, GLfloat height, GLfloat thickness, GLfloat stem_width, GLfloat stem_to_arrowhead_ratio);
extern objptr create_color_control(worldptr world, vertype controlcorner, GLfloat colorbasewidth);
extern void make_lattice_face(leptr startle, leptr stople, int numpts, vertype pt1, vertype pt2, vertype pt3);
extern void set_controlpts(vertype controlpts[]);
extern objptr create_lattice(worldptr world);
extern objptr create_toolbox(worldptr world, objptr decal_obj, float leafratio, GLfloat width, GLfloat height, GLfloat depth);
extern objptr setup_prims_toolbox(worldptr world);
extern objptr setup_goodies_toolbox(worldptr world);
extern objptr setup_stocks_demo(worldptr world, GLfloat barwidth, GLfloat barheight, GLfloat bardepth, int numstocks, int timedepth);
extern void update_stocks_demo(void);
extern objptr create_pieslice(worldptr world, GLfloat radius, GLfloat depth, float angleinc, int thiseg, int addsegs);
extern objptr create_piechart(worldptr world, GLfloat radius, GLfloat depth, int resolution, objptr chart_plate);
extern objptr setup_piechart_demo(worldptr world, GLfloat radius, GLfloat depth, int resolution, colortype piechartpadcolor);
extern objptr create_slide(worldptr world);
extern objptr setup_slide_pad(worldptr world);
extern objptr create_arrowtool(worldptr world);
extern objptr setup_arrowtool_pad(worldptr world);
extern void setup_tools(void);

/* topology.c */

extern void init_shell_defaults(shellptr newshell);
extern void init_shell_lists(shellptr newshell);
extern void init_obj_lists(objptr newobj);
extern void init_world_lists(worldptr neworld);
extern void init_ring_lists(ringptr newring);
extern void init_fve_lists(fveptr newfve);
extern void init_bound_lists(boundptr newbound);
extern void init_curve_lists(curveptr newcurve);
extern void init_surf_lists(surfptr newsurf);
extern void copy_shell_elem(shellptr sourceshell, shellptr destshell);
extern void copy_obj_elem(objptr sourceobj, objptr destobj);
extern void copy_world_elem(worldptr sourceworld, worldptr destworld);
extern void copy_ring_elem(ringptr sourcering, ringptr destring);
extern void copy_fve_elem(fveptr sourcefve, fveptr destfve);
extern void copy_bound_elem(boundptr sourcebound, boundptr destbound);
extern void copy_curve_elem(curveptr sourcecurve, curveptr destcurve);
extern void copy_surf_elem(surfptr sourcesurf, surfptr destsurf);
extern void delete_fve_lists(fveptr thisfve);
extern void delete_bound_lists(boundptr thisbound);
extern void delete_shell_lists(shellptr delshell);
extern void delete_obj_lists(objptr thisobj);
extern void delete_world_lists(worldptr thisworld);
extern void delete_ring_lists(ringptr thisring);
extern void delete_curve_lists(curveptr thiscurve);
extern void delete_surf_lists(surfptr thisurf);
extern void delete_vv_lists(vvptr thisvv);
extern void delete_vf_lists(vfptr thisvf);
extern void merge_worlds(worldptr sourceworld, worldptr destworld);
extern void clear_world(worldptr thisworld);
#ifndef FAST
extern leptr Twin_le(leptr thisle);
#endif
extern leptr new_le(leptr next_le, evfptr thisevf, vfeptr thisvfe, int dir);
extern leptr del_le(leptr thisle);
extern shellptr make_shell(vertype pos);
extern void move_shell_to_obj(objptr newmotherobj, shellptr moved_shell);
extern objptr make_childobj_noshell(objptr parent);
extern objptr make_parentobj_noshell(objptr child);
extern objptr make_obj_noshell(worldptr world);
extern objptr make_childobj(objptr parent, vertype pos);
extern objptr make_parentobj(objptr child, vertype pos);
extern objptr make_obj(worldptr world, vertype pos);
extern char *get_object_name(objptr named_obj);
extern void set_object_name(objptr named_obj, char *obj_name);
extern void set_world_name(worldptr named_world, char *world_name);
extern void kill_shell(shellptr thishell);
extern void kill_obj(objptr deleted_obj);
extern void kill_obj_branch(objptr deleted_obj);
extern leptr make_edge_vert(leptr le1, leptr le2, vertype pos);
extern void kill_edge_vert(leptr le1, leptr le2);
extern fveptr make_edge_face(leptr le1, leptr le2);
extern void kill_edge_face(leptr le1, leptr le2);
extern leptr kill_ring_make_edge(leptr le1, leptr le2);
extern boundptr kill_edge_make_ring(leptr le1, leptr le2);
extern void kill_face_make_loop(fveptr sourcefve, fveptr destfve);
extern fveptr make_face_kill_loop(boundptr bound);
extern void move_loop(boundptr thisbound, fveptr destfve, int outflag);
extern leptr break_edge_at_pos(leptr thisle, vertype pos);
extern void move_holes(fveptr oldface, fveptr newface);
extern void extrude_face(fveptr thisfve, vertype depth);
extern void extrude_faces(listptr drawshellist, vertype depth);
extern void make_solid_of_rotation(objptr wireobj, vertype rotvec, vertype rotpt, int degree_inc);
extern void combine_loops(fveptr faceA, fveptr faceB, Boolean killbehind);
extern void merge_shells(shellptr sourceshell, shellptr destshell);
extern void fix_flipped_bounds(shellptr thishell);
extern void flip_evf(evfptr thisevf);
extern void clear_flip_flags(listptr evflist);
extern void tag_evfs_to_flip(boundptr thisbound, listptr evflist);
extern void flip_hole_orientation(boundptr thisbound);
extern void flip_shell_orientation(shellptr thishell);
extern void flip_obj_orientation(objptr thisobj);
extern void glue_objects_along_faces(fveptr face1, fveptr face2, Boolean killbehind);
extern fveptr delete_vertex(leptr thisle);
extern fveptr delete_edge(leptr thisle);
extern fveptr delete_face(fveptr thisfve);
extern featureptr find_largergrain(featureptr smallfeature, int larger_type);
extern fveptr find_face_given_normal(objptr thisobj, vertype searchnorm);
extern Boolean find_les_same_face(leptr *le1, leptr *le2);
extern Boolean find_les_same_face_same_vfes(leptr *le1, leptr *le2);
extern Boolean find_les_same_face_walking(leptr *le1, leptr *le2);
extern void reset_cutplanetime_flags(objptr thisobj, void *unused_data);
extern void reset_cutplanetime(listptr world_list);

/* transform.c */

extern void transformpt(Coord *thept, Matrix xform, Coord *xformedpt);
extern void transformpts(long npts, Coord *pts, Matrix xform, Coord *xformedpts);
extern void transform_shellvfes(shellptr thishell, Coord *xform);
extern void transform_shellnormals(shellptr thishell, Coord *xform);
extern void transform_shell_cutpts(shellptr thishell, Coord *xform);
extern void transform_shell(shellptr thishell, Matrix xformatrix);
extern void transform_shellvfes_reverse(shellptr thishell, Coord *invxform);
extern void transform_shellnormals_reverse(shellptr thishell, Coord *invxform);
extern void transform_shell_cutpts_reverse(shellptr thishell, Coord *invxform);
extern void transform_shell_reverse(shellptr thishell, Matrix invxform);
#ifdef IRIS
extern void transform_shells();
#endif
extern void transform_obj_reverse(objptr thisobj, Matrix invxform);
extern void transform_obj_evf_cutpts(objptr thisobj);
extern void transform_obj(objptr thisobj);
extern void transform_obj_tree(objptr thisobj);
extern void permanent_transform_obj(objptr thisobj);
extern void apply_invtransform(objptr thisobj, Matrix invtransform);
extern void apply_transform(objptr thisobj, Matrix newtransform);
extern void apply_transform_to_tree(objptr thisobj, Matrix newtransform, Matrix invtransform);
extern void apply_postmultinv(objptr thisobj, Matrix invxform);
extern void apply_postmult(objptr thisobj, Matrix xform);
extern void postmult_tree_by_xform(objptr thisobj, Matrix newtransform, Matrix invtransform);

/* translate.c */

extern void translatevert(register vfeptr thisvfe, Matrix the_invxform, register vertype delta);
extern void clear_vertranslate(vfeptr thisvfe);
extern void clear_shelltranslates(shellptr thishell);
extern void clear_feature_translates(featureptr thisfeature);
extern void translate_vfe_core(vfeptr thevert, shellptr mothershell, vertype delta, GLboolean useflags);
extern void translate_surface(surfptr thisurf, vertype delta, GLboolean useflags);
extern void translate_vfe(vfeptr thisvfe, vertype delta, GLboolean useflags);
extern void translate_evf(evfptr thisevf, vertype delta, GLboolean useflags);
extern void translate_fve(fveptr thisfve, vertype delta, GLboolean useflags);
extern void translate_shell(shellptr thishell, vertype delta, GLboolean useflags);
extern void translate_obj(objptr thisobj, vertype delta);
extern void translate_obj_quick(objptr thisobj, vertype delta);
extern void translate_feature(featureptr thisfeature, vertype delta, GLboolean useflags);
extern void clear_property_translates(property thisproperty);

/* update.c */


extern vertype drag_origin;
extern Boolean selectables_set;

#ifdef IRIS
extern void init_updates();
#endif
/* this should be in utilities.c or some such */
extern double enforce_flimits(double fvalue, double fmin, double fmax);
extern int enforce_ilimits(int ivalue, int imin, int imax);
extern long enforce_llimits(long lvalue, long lmin, long lmax);
extern void constrain_movecrosshair(vertype delta);
extern void compute_crosshair_move(vertype delta, vertype actualdelta, stateptr state);
extern void zoom_room_view(vertype center, double scale, stateptr state);
extern void rotate_room_view(int xamount, int yamount, stateptr state);
extern void set_drag_origin(stateptr state);
extern void clear_drag_origin(void);
extern GLboolean drag_valid(vertype newcursorpos);
extern GLboolean first_valid_drag(stateptr state, vertype extra_offset);
extern GLboolean currently_dragging(void);
extern void set_constrain(void);
extern void clear_constrain(void);
extern void do_cursor_updates(void);
extern void unlog_updates(featureptr unlogged_feature);
extern void remove_killed_features(listptr world_list, stateptr state);
extern void process_obj_update_engine(objptr motherobj, updateptr update_rec);
extern void process_feature_update(featureptr thisfeature, long updateflags, stateptr state, stateptr oldstate);
extern void process_tree_update(objptr thisobj, long updateflags, stateptr state, stateptr oldstate);
extern void process_updates(worldptr world, stateptr state, stateptr oldstate, property update_prop);
extern void process_global_updates(listptr world_list, stateptr state, stateptr oldstate);
extern void log_update(featureptr updatefeature, long updateflags, GLboolean immediately);
extern void log_property_update(worldptr world, property logprop, long updateflags, GLboolean immediately);
extern void log_global_update(long updateflags);
extern GLboolean handle_user_events(stateptr state);
extern void new_update_state(listptr world_list, stateptr state, stateptr oldstate);
extern void swap_erase_buffers(int framebuffer);
extern void log_global_refresh(void);
extern GLboolean already_logged(featureptr log_feature, bboxscreenptr new_screen_bbox, erase_record_ptr this_erase_records);
extern void do_erase_log(erase_record_ptr this_erase_records, featureptr log_feature, bboxscreenptr new_screen_bbox, bbox3dptr new_3d_bbox);
extern void log_screen_erase_bbox(featureptr log_feature, bboxscreenptr new_screen_bbox, bbox3dptr new_erase_bbox, GLboolean log_in_old);
extern void log_3d_erase_bbox(featureptr log_feature, bbox3dptr new_erase_bbox, GLfloat bboxmargin, GLboolean log_in_old);
extern void log_object_erase_bbox(objptr erase_obj, GLboolean use_objbbox, GLboolean log_in_old);
extern void log_branch_erase_bbox(objptr erase_obj, GLboolean use_objbbox, GLboolean log_in_old);
extern void log_feature_redraw(featureptr log_feature);
extern void log_crosshair_erase_bboxes(stateptr state, GLboolean log_in_old);
extern void log_cutplane_erase_bbox(stateptr state, GLboolean log_in_old);
extern GLboolean screen_bbox_collision(objptr thisobj, bboxscreenptr test_bbox);
extern void log_erasebbox_intersections(listptr world_list, int framebuffer);
extern void update_cutplanetime(listptr world_list);
extern GLboolean new_user_input(listptr world_list, stateptr state, stateptr oldstate);
extern void update_display(void);

/* user.c */

extern void inquire_filename(char *inoutfilename, char *message, char *defaultfile);
extern double inquire_double(char *message, double defaultval);
extern void inquire_vfe(char *message, vfeptr thisvfe);
extern void inquire_evf(char *message, evfptr thisevf);
extern GLfloat do_sticking(GLfloat planetransz, GLboolean start_sticking);
extern void set_nearest_marker(stateptr state);
extern void sort_into_nearpts(listptr nearpts_list, vfeptr thisvfe);
extern void fill_in_nearpts_from_obj(objptr nearobj, listptr nearpts_list);
extern void create_nearpts_list(worldptr world, stateptr state);
extern void init_nearpts_list(void);
extern void reset_nearpts_list(void);
extern sortptr go_below_range(GLfloat oldplanez, GLfloat newplanez, sortptr searchpt, GLboolean search_forwards);
extern sortptr go_into_range(GLfloat oldplanez, GLfloat newplanez, sortptr searchpt, GLboolean search_forwards);
extern sortptr walk_up_range(GLfloat oldplanez, GLfloat newplanez, sortptr searchpt, GLboolean search_forwards);
extern GLboolean in_the_range(GLfloat oldplanez, GLfloat newplanez, sortptr searchpt, GLboolean search_forwards);
extern GLboolean snap_plane_to_nearestpt(worldptr world, stateptr state, vertype cursor_move);

/* vertex.c */

extern void setvfe(vfeptr thisvfe, float *newpos, Matrix global2local_xform);
extern void setvfe_local(vfeptr thisvfe, float *newpos, Matrix local2global_xform);
extern vfeptr new_vfe(vertype pos, shellptr thishell);
extern vfeptr new_vfe_local(vertype local_pos, shellptr thishell);
extern void setfnorm(fveptr thisfve, vertype newfnorm);
extern void setfnorm_local(fveptr thisfve, vertype newfnorm);
extern void sortvertlist(int axis, listptr vfelist);
extern leptr get_lemax(int axis, boundptr thisbound);
extern leptr get_lemin(int axis, boundptr thisbound);
extern void get_leminmax(int axis, boundptr thisbound, leptr *lemin, leptr *lemax);
extern vfeptr get_vertmax(int axis, listptr vertlist);
extern vfeptr get_3axis_vfemax(int axis1, int axis2, int axis3, listptr vertlist);
extern vfeptr get_vertmin(int axis, listptr vertlist);
extern void get_vertminmax(int axis, listptr vertlist, vfeptr *minvfe, vfeptr *maxvfe);
extern void get_local_vertminmax(int axis, listptr vertlist, vfeptr *minvfe, vfeptr *maxvfe);

/* zoom.c */

extern void fix_prim_recs(worldptr world, GLfloat scalefactor);
extern void zoom_world(worldptr world, vertype zoomcenter, GLfloat scalefactor);
extern void totally_unzoom_world(worldptr world);
extern void totally_zoom_world(worldptr world, Matrix xform, Matrix invxform);
extern GLfloat zoom_unit(GLfloat unzoomed_amt);
extern GLfloat unzoom_unit(GLfloat zoomed_amt);
extern GLfloat fetch_system_zoom(void);
extern void set_system_zoom(GLfloat new_zoom);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* EXTERN_H */
