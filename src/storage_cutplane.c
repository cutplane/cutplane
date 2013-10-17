
/* FILE: storage.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*   Read/Save Shells from/to Disk Routines for the Cutplane Editor      */
/*                                                                        */
/* Author: WAK                                    Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define STORAGEMODULE

#include <config.h>
#include <stdio.h>
#include <math.h>

#include <platform_gl.h>
#include <string.h>
#include <ctype.h>


#include <cutplane.h>
#include <storage_cutplane.h>

#include <math3d.h>
#include <update.h>
#include <cpl_cutplane.h>
#include <primitives.h>


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			GLOBAL VARIABLES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

void load_prims(worldptr);
void load_goodies(worldptr);
void load_light(worldptr);
void load_pencil(worldptr);
void load_colors(worldptr);
void load_storagepad(worldptr);

/* This one has been gutted for Windows. */
void load_stocks(worldptr);

void load_lbeam(worldptr);
void load_toolbar(worldptr);
void load_arrangebar(worldptr);
void load_piechart(worldptr);
void load_measurepad(worldptr);
void load_slidepad(worldptr);
void load_arrowpad(worldptr);
void load_rotatetool(worldptr);

char specialfile_name[Numspecialfiles][NAMELENGTH] = {"prims.cut",
							"goodies.cut",
							"stocks",
							"pencil.cut",
							"storagepad",
							"lbeam.cut",
							"tools",
							"arrangers",
							"piechart",
							"measurer",
							"slidepad",
							"arrowpad",
							"rt"};
							
void (*specialfile_rtn[Numspecialfiles])() = {load_prims,
						load_goodies,
						load_stocks,
						load_pencil,
						load_storagepad,
						load_lbeam,
						load_toolbar,
						load_arrangebar,
						load_piechart,
						load_measurepad,
						load_slidepad,
						load_arrowpad,
						load_rotatetool};

techniquerec selectechniques[Numtechniques] =
{{"set_object_selectable",set_object_selectable},
 {"set_rotate_innercircle_selectable",set_rotate_innercircle_selectable},
 {"set_rotatexbar_selectable",set_rotatexbar_selectable},
 {"set_rotate_outercircle_selectable",set_rotate_outercircle_selectable},
 {"set_toolpad_selectable",set_toolpad_selectable},
 {"set_headpad_selectable",set_headpad_selectable},
 {"set_normalpad_selectable",set_headpad_selectable},
 {"set_scalebbox_selectable",set_scalebbox_selectable}};

techniquerec drawtechniques[Numtechniques] =
{{"normal_draw_technique",normal_draw_technique},
 {"no_draw_technique",Nil}, 
 {"draw_rotate_innercircle",draw_rotate_innercircle},
 {"draw_rotate_outercircle",draw_rotate_outercircle},
 {"draw_rotatexbar",draw_rotatexbar},
 {"draw_scalebbox_technique",draw_scalebbox_technique},
 {"draw_plate_technique",draw_plate_technique},
 {"draw_headpad_technique",draw_headpad_technique},
 {"draw_pad_technique",draw_pad_technique},
 {"draw_marquee_technique",draw_marquee_technique},
 {"draw_scribble_technique",draw_scribble_technique}};
   

techniquerec screenbboxtechniques[Numtechniques] =
{{"Normal",Nil},
 {"find_cutplane_screenbbox",(void (*)()) find_cutplane_screenbbox},
 {"find_rotatetool_screenbbox",(void (*)()) find_rotatetool_screenbbox},
 {"find_normal_cursor_screenbbox",(void (*)()) find_normal_cursor_screenbbox}};

int object_id;
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*           	FAKE (space) STORAGE ROUTINES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


  void
load_light(worldptr world)
{
  vertype offset;
  objptr light;
  ;
  world = primary_world;
  light = create_oval_cylinder(world,30.0,30.0,60.0,10);
  add_property((featureptr) light,lockedgeometry_prop);
  set_feature_named_color((featureptr) light,"Orange");
  add_property((featureptr) First_obj_fve(light),luminous_prop);
  set_feature_named_color((featureptr) First_obj_fve(light),"Yellow");
  axis_rot_obj(light,x_axis,origin,1350);
  axis_rot_obj(light,y_axis,origin,450);
  setpos3d(offset,200.0,150.0,150.0);
  translate_feature((featureptr) light,offset,FALSE);
  log_global_update(Allupdateflags);
}

  void
load_stocks(worldptr world)
{
#ifdef IRIS
  setup_stocks_demo(working_world,40.0,90.0,40.0,4,5);
  stocks_demo_on = TRUE;
#endif
}


  void
load_pencil(worldptr world)
{
  vertype offset;
  ;
  world = primary_world;
  setpos3d(offset,250.0,-200.0,150.0);
  create_sketch_tool(world,offset,"Yellow","Orange");
  log_global_update(Allupdateflags);
}

  void
load_colors(worldptr world)
{
  static colortype startcolor = {0,0,0};
  ;
  log_global_update(Allupdateflags);
}

  void
load_storagepad(worldptr world)
{
  create_storage_pad(environment_world,orange);
  log_global_update(Allupdateflags);
}

  void
load_lbeam(worldptr world)
{
  vertype offset;
  objptr lbeamobj;
  ;
  world = primary_world;
  lbeamobj = create_lbeam(world,200,180,55,200);

  setpos3d(offset,-250.0,-20.0,-40.0);
  translate_feature((featureptr) lbeamobj,offset,FALSE);
  set_feature_named_color((featureptr) lbeamobj,"Red");
  setpos3d(offset,50.0,100.0,50.0);
  lbeamobj = duplicate_obj(lbeamobj);
  translate_feature((featureptr) lbeamobj,offset,FALSE);
  /* leave second one grey for the time being... */
  set_feature_named_color((featureptr) lbeamobj,"Green");
/*  set_feature_named_color((featureptr) lbeamobj,"Grey");*/

  log_global_update(Allupdateflags);
}  


  void
load_prims(worldptr world)
{
  setup_prims_toolbox(environment_world);
}

  void
load_goodies(worldptr world)
{
  setup_goodies_toolbox(environment_world);
}


  void
load_toolbar(worldptr world)
{
  objptr toolbar;
  vertype offset;
  ;
  toolbar = create_toolbar(environment_world,"./rsc/toolpads.rsc");
  diffpos3d(First_obj_vfe(toolbar)->pos,state->cursor,offset);
  setpos3d(offset,0.0,0.0,offset[vz]);
  translate_feature((featureptr) toolbar,offset,FALSE);
}

  void
load_arrangebar(worldptr world)
{
  objptr toolbar;
  vertype offset;
  ;
  toolbar = create_toolbar(environment_world,"./rsc/arrangepads.rsc");
  diffpos3d(First_obj_vfe(toolbar)->pos,state->cursor,offset);
  setpos3d(offset,0.0,0.0,offset[vz]);
  translate_feature((featureptr) toolbar,offset,FALSE);
}

  void
load_piechart(worldptr world)
{
  setup_piechart_demo(environment_world,100.0,50.0,36,orange);
}

  void
load_measurepad(worldptr world)
{
  create_measure_pad(environment_world,orange);
}

  void
load_slidepad(worldptr world)
{
  setup_slide_pad(environment_world);
}

  void
load_arrowpad(worldptr world)
{
  setup_arrowtool_pad(environment_world);
}

  void
load_rotatetool(worldptr world)
{
  add_property_to_descendants((featureptr) rotatetool_obj,visible_prop);
}

  Boolean
special_file(char *infile_name)
{
  int i;
  ;
  for (i = 0; i < Numspecialfiles; ++i)
    if (strcmp(specialfile_name[i],infile_name) == 0)
    {
      (specialfile_rtn[i])();
      return(TRUE);
    }
  return(FALSE);
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			STORAGE ROUTINES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  techniquerecptr
find_technique(char *techniquename,techniquerecptr techniques)
{
  int i = 0;
  ;
  while (i < Numtechniques)
  {
    if (strcmp(techniques[i].technique_name,techniquename) == 0)
      return(&(techniques[i]));
    i++;
  }
  return(Nil);
}

  char *
find_technique_name(void (*techniquertn)(),techniquerecptr techniques)
{
  int i = 0;
  ;
  while (i < Numtechniques)
  {
    if (techniques[i].technique_rtn == techniquertn)
      return (techniques[i].technique_name);
    i++;
  }
  return(Nil);
}


/* should put this next routine in topology.c */

  void
append_evf(the_head,le1ptr,le2ptr,face1,face2)
  listptr the_head;
  leptr le1ptr,le2ptr;
  fveptr face1,face2;
{
  evfptr newevf;
  shellptr thishell;
  ;
  thishell = le1ptr->motherbound->motherfve->mothershell;
  newevf = (evfptr) append_new_blank_elem(the_head,Evf_type);

  newevf->le1 = le1ptr;
  newevf->le2 = le2ptr;
  newevf->f1 = face1;
  newevf->f2 = face2;
  le1ptr->facedge = newevf;	/* make sure le's point back to */
  le2ptr->facedge = newevf;	/* evf so we can get back and forth */
}


  void
read_matrix(FILE *infile,Matrix thematrix)
{
  int i,j;
  ;
  for (i = 0; i < 4; ++i)
  {
    fscanf(infile," [ ");
    for (j = 0; j <4; ++j)
      fscanf(infile," %f ",&(thematrix[i][j]));
    fscanf(infile," ]\n");
  }
}

  
  /* read vertex list */
  void
read_vfes(FILE *infile,listptr thevfelist,shellptr thishell)
{
  int vcount,dumnumvert,shellnumverts;
  vertype newvert;
  ;
  fscanf (infile," Number of Vertices:%d\n",&shellnumverts);
  fscanf (infile," VFE List:\n");
  for (vcount = 1;vcount <= shellnumverts; ++vcount)
  {
    fscanf (infile,"Vertex %d:%f %f %f\n",&dumnumvert,
	    &(newvert[vx]),&(newvert[vy]),&(newvert[vz]));
    newvert[vw] = 1.0;
/* no more vw hacking
    newvert[vy] -= 400.0;
    scalevec3d(newvert,5.0,newvert);
*/
    new_vfe_local(newvert,thishell);
    /* Initialize head of vertex's face list to empty*/
    /* obsolete: thevfelist->last.vfe->facelist = create_list();*/
  }
}

  void
read_bounds(infile,thishell,thisfve)
  FILE *infile;
  shellptr thishell;
  fveptr thisfve;
{
  int dumfacenum,dumboundnum,boundcnt,lecnt,numbounds,numles,vfe_index;
  boundptr newbound;
  leptr newle;
  ;
  fscanf(infile,"Face:%d (%d bounds)\n",&dumfacenum,&numbounds);
  for (boundcnt = 0; boundcnt < numbounds; ++boundcnt)
  {
    newbound = (boundptr) append_new_blank_elem(thisfve->boundlist,Bound_type);
    newbound->motherfve = thisfve;
    newbound->lelist = create_circular_list();
    fscanf (infile,"\tBound %d (%d le's):",&dumboundnum,&numles);
    for (lecnt = 0; lecnt < numles; ++lecnt)
    {
      fscanf (infile,"%d ",&vfe_index);
      newle = (leptr) append_new_blank_elem(newbound->lelist,Le_type);
      newle->motherbound = newbound;
      newle->facevert = (vfeptr) get_elem_ptr(vfe_index,thishell->vfelist);
      newle->facevert->startle = newle;
      newle->facedge = Nil;	/* reconciled in read_evfs */
    }
    /* make sure list is circular, larry */
    newle->next = newbound->lelist->first.le;
    newbound->lelist->first.le->prev = newle;
  }
}
    
  void
read_fves(FILE *infile,shellptr thishell)
{
  int facecount,numfaces;
  static vertype dum_norm = {0.0,0.0,0.0,0.0};
  fveptr thisfve;
  ;
  fscanf (infile," Number of Faces:%d\n",&numfaces);
  fscanf(infile," FVE List:\n");
  for (facecount = 1; facecount<=numfaces; ++facecount)
  {
    thisfve = (fveptr) add_new_blank_elem((listptr) thishell->fvelist,
			       (elemptr) thishell->fvelist->last.fve,
			       Fve_type);
    thisfve->mothershell = thishell;
    read_bounds(infile,thishell,thisfve);

  }
}

  leptr
find_leptr(thishell,f1,v1,v2)
  shellptr thishell;
  fveptr f1;
  vfeptr v1,v2;
{
  boundptr thisbound;
  leptr thisle;
  ;
  for (thisbound = f1->boundlist->first.bnd;
       thisbound != Nil; thisbound = thisbound->next)
  {
    thisle = thisbound->lelist->first.le;
    do				/* go through bound forwards */
    {
      if ((thisle->facevert == v1) && (thisle->next->facevert == v2))
	return(thisle);		/* le found! */
      thisle = thisle->next;
    } while (thisle != thisbound->lelist->first.le);
    do				/* also go through bound backwards */
    {
      if ((thisle->facevert == v1) && (thisle->prev->facevert == v2))
	return(thisle->prev);		/* le found! */
      thisle = thisle->prev;
    } while (thisle != thisbound->lelist->first.le);
  }
  return(Nil);			/* no le found. a pity, i must say. */
}

/* find le's and faces adjoining an edge just read in. */
  void
find_edge_faceles(shellptr thishell,vfeptr vfe1,vfeptr vfe2,
		  fveptr *fve1,fveptr *fve2,leptr *evfle1,leptr *evfle2)
{
  fveptr thisfve;
  ;
  *evfle1 = *evfle2 = Nil;
  for (thisfve = thishell->fvelist->first.fve; thisfve != Nil;
       thisfve = thisfve->next)
  {
    if (*evfle1 == Nil)
    {
      *evfle1 = find_leptr(thishell,thisfve,vfe1,vfe2);
      if (*evfle1 != Nil)
	*fve1 = thisfve;
    }
    else
    {
      *evfle2 = find_leptr(thishell,thisfve,vfe1,vfe2);
      if (*evfle2 != Nil)
      {
	*fve2 = thisfve;
	return;			/* found both faces for this edge, stop */
      }
    }
  }
}

  leptr
find_other_le(shellptr thishell, fveptr currentfve, leptr thisle)
{
  fveptr thisfve;
  leptr otherle;
  ;
  thisfve = thishell->fvelist->first.fve;
  while (thisfve != Nil)
  {
    if (thisfve != currentfve)
    {
      otherle = find_leptr(thishell,
			   thisfve,thisle->facevert,thisle->next->facevert);
      if (otherle != Nil)
	return (otherle);
    }
    thisfve = thisfve->next;
  }
  /* this should never happen!!! */
  /* commented out so we can read vw */
  /* system_error("find_other_le: otherle not found!\n");*/
  return (Nil);
}

void
make_shell_evfs(shellptr thishell)
{
  fveptr thisfve,otherfve;
  boundptr thisbound;
  leptr firstle,thisle,otherle;
  evfptr newevf;
  ;
  thisfve = thishell->fvelist->first.fve;
  while (thisfve != Nil)
  {
    thisbound = thisfve->boundlist->first.bnd;
    while (thisbound != Nil)
    {
      firstle = thisle = thisbound->lelist->first.le;
      do
      {
	if (thisle->facedge == Nil)
	{
	  otherle = find_other_le(thishell,thisfve,thisle);
	  otherfve = Nil;
	  if (otherle == Nil)
	    otherle = thisle;
	  otherfve = otherle->motherbound->motherfve;
	  append_evf(thishell->evflist,thisle,otherle,thisfve,otherfve);
	}
	thisle = thisle->next;
      } while (thisle != firstle);
      thisbound = thisbound->next;
    }
    thisfve = thisfve->next;
  }
}

  void
read_evfs(infile,numedges,thishell)  /* read in edge-vertex list for evflist */
  FILE *infile;
  int numedges;
  shellptr thishell;
{
  int ecount,edgeno,v1,v2;
  listptr head_evf;
  leptr evfle1 = Nil,evfle2 = Nil;
  vfeptr vfe1,vfe2;
  fveptr fve1 = Nil,fve2 = Nil;
  ;
  fscanf (infile," EVF List:\n");
  for (ecount = 1; ecount <= numedges; ++ecount)
  {
    fscanf (infile,"Edge %d: (Vertices %d, %d)\n",
	    &edgeno,&v1,&v2);
    vfe1 = (vfeptr) get_elem_ptr(v1,thishell->vfelist);
    vfe2 = (vfeptr) get_elem_ptr(v2,thishell->vfelist);
    find_edge_faceles(thishell,vfe1,vfe2,&fve1,&fve2,&evfle1,&evfle2);
    append_evf(thishell->evflist,evfle1,evfle2,fve1,fve2);
  }
}



  void
read_feature_props(FILE* infile, featureptr thisfeature,int numprops)
{
  char newpropname[Maxpropertynamelen],decalfilename[50];
  int count;
  objptr motherobj;
  ;
  while (numprops-- > 0)
  {
    fscanf(infile,"%s\n",newpropname);
    count = 0;
    while (count < next_available_property)
    {
      if (strcmp(propinfo[count].propname,newpropname) == 0)
      {
	if (propinfo[count].propertycode == Nil)
	  read_CPL_propfile(newpropname); /* MUST get CPL code for this */
					  /* (already defined) prop */
	break;
      }
      else
	count++;
    }
    if (count == next_available_property)
    {
      define_new_property(newpropname,Nil);
      read_CPL_propfile(newpropname); /* get CPL code for this prop,if exists! */
    }
    /* Do NOT add selectable prop even if the thing was stored with the */
    /* stoopid selectable prop.*/
    if (count != selectable_prop)
      add_property(thisfeature,(property) count);
  }
  if (has_property(thisfeature,hasdecal_prop))
  {
    motherobj = (objptr) find_largergrain(thisfeature,Obj_type);
    fscanf(infile,"Decal File:%s\n",decalfilename);
    set_property((featureptr) motherobj,hasdecal_prop,strdup(decalfilename));
  }
}

  void
read_face_colors(FILE *infile, shellptr thishell,int numfacecolors)
{
  fveptr thisfve;
  colortype newcolor;
  int facenum;
  ;
  /* right now faces only */
  while (numfacecolors-- > 0)
  {
    fscanf (infile, "\tFace %d:%hd %hd %hd\n", &facenum,
	    &(newcolor[0]),&(newcolor[1]),&(newcolor[2]));
    thisfve = (fveptr) get_elem_ptr(facenum,thishell->fvelist);
    set_feature_color((featureptr) thisfve,newcolor);
  }
}

  void
read_shell_properties(FILE *infile, shellptr thishell)
{
  vfeptr thisvfe;
  evfptr thisevf;
  fveptr thisfve;
  int count,numprops,numvert,numedge,numface;
  ;
  fscanf (infile, "Vertices with properties:%d\n",&count);
  if (count > 0)
  {
    while (count-- > 0)
    {
      fscanf(infile, "Vertex %d has %d properties:\n",&numvert,&numprops);
      thisvfe = (vfeptr) get_elem_ptr(numvert,thishell->vfelist);
      read_feature_props(infile, (featureptr) thisvfe,numprops);
    }
  }
  
  fscanf (infile, "Edges with properties:%d\n",&count);
  if (count > 0)
  {
    while (count-- > 0)
    {
      fscanf (infile, "Edge %d has %d properties:\n",&numedge,&numprops);
      thisevf = (evfptr) get_elem_ptr(numedge,thishell->evflist);
      read_feature_props(infile, (featureptr) thisevf,numprops);
    }
  }  

  fscanf (infile, "Faces with properties:%d\n",&count);
  if (count > 0)
  {
    while (count-- > 0)
    {
      fscanf (infile, "Face %d has %d properties:\n",&numface,&numprops);
      thisfve = (fveptr) get_elem_ptr(numface,thishell->fvelist);
      read_feature_props(infile, (featureptr) thisfve,numprops);
    }
  }
}

  void
get_CPLprivatecode(FILE* infile, int featurenum, listptr featurelist)
{
  CPL_word_ptr privatecode_start;
  // What goes on here? We use privatecode_filename, but that's a single char!? -- LJE
//  char privatecode_name[MaxCPLabelength],privatecode_filename;
  char privatecode_name[MaxCPLabelength];
  featureptr thisfeature;
  ;
  thisfeature = (featureptr) get_elem_ptr(featurenum,featurelist);
  fscanf(infile, "%s\n",privatecode_name);
  read_CPL_privatecode((char *) privatecode_name,thisfeature);
}
  
  void
read_shell_CPLcode(FILE* infile, shellptr thishell)
{
  /* needs to write the CPL code file name that got this private code for the */
  /* feature into the object... but since thats not stored... i'll take */
  /* label of first address as the filename. (after lowercasing it)*/
  vfeptr thisvfe;
  evfptr thisevf;
  fveptr thisfve;
  int count,vertnum,edgenum,facenum;
  ;
  fscanf (infile, "Vertices with CPL private code:%d\n",&count);
  while (count-- > 0)
  {
    fscanf(infile, "Vertex %d: ",&vertnum);
    get_CPLprivatecode(infile,vertnum,thishell->vfelist);
  }

  fscanf (infile, "Edges with CPL private code:%d\n",&count);
  while (count-- > 0)
  {
    fscanf(infile, "Edge %d: ",&edgenum);
    get_CPLprivatecode(infile,edgenum,thishell->evflist);
  }

  fscanf (infile, "Faces with CPL private code:%d\n",&count);
  while (count-- > 0)
  {
    fscanf(infile, "Face %d: ",&facenum);
    get_CPLprivatecode(infile,facenum,thishell->fvelist);
  }
}

  void
read_shell_specials(FILE *infile,shellptr thishell)
{
  int numfacecolors;
  ;
  /* read in unique colors */
  fscanf (infile,"Faces with different colors:%d\n", &numfacecolors); 
  read_face_colors(infile, thishell, numfacecolors);
  /* read in properties */
  read_shell_properties(infile, thishell);
  /* read in private cpl code names */
  read_shell_CPLcode(infile, thishell);
}


  void
read_shell(FILE* infile,shellptr thishell)
{
  int dumshellnum;
  ;
  /* get shell num, just for readability */
  fscanf(infile, "Shell %d:\n", &dumshellnum);
  /* read in shell's vertices */
  read_vfes(infile,thishell->vfelist,thishell);
  /* read in face lists */
  read_fves(infile,thishell);
}

  void
read_shells(FILE* infile, objptr newobj, int numshells)
{
  shellptr thishell;
  ;
  while (numshells-- > 0)
  {
    thishell = (shellptr) append_new_blank_elem(newobj->shellist, Shell_type);
    thishell->motherobj = newobj;
    read_shell(infile, thishell);
    /* construct the evfs from the les */
    make_shell_evfs(thishell);
    /* read in shells specials */
    read_shell_specials(infile, thishell);
  }
}

/* given an object id, find a pointer to the object in the tree, counting */
 /* in PREORDER fashion (NOT the level of the object...) LEFT-TO-RIGHT. */

  Boolean
find_object_by_number(objptr search_obj,objptr *foundobj,
		      int search_object_id, int *search_id)
{
  objptr thischild;
  ;
  if (*search_id == search_object_id)
  {
    *foundobj = search_obj;
    return(TRUE);
  }
  else
  {
    *search_id += 1;
    thischild = search_obj->children->first.obj;
    while (thischild != Nil)
    {
      if (find_object_by_number(thischild,foundobj,search_object_id,search_id))
	return(TRUE);
      thischild = thischild->next;
    }
  }
  return(FALSE);
}

	
  char *
reset_white_space(char *read_name)
{
  int i = 0,read_name_len;
  char *tempobjname;
  ;
  read_name_len = strlen(read_name);
  tempobjname = (char *) alloc_elem(read_name_len+1);
  strcpy(tempobjname,read_name);
  while (i < read_name_len)
  {
    if (tempobjname[i] == 27)
      tempobjname[i] = ' ';	/* spaces back where esc's were put... others */
				/* lost in sauce*/
    i++;
  }
  return(tempobjname);
}

  objptr
read_object(FILE* infile,worldptr world)
  /* reads in one obj from infile into the tree */
{
  int parent_id,search_id = 0;
  int objnumdum,numprops,numshells;
  objptr newobj,parent;
  char obj_name[Maxobjectnamelength],*tempobjname;
  colortype newobj_color;
  char newtechnique[Techniquenamelen];
  Coord cube_length,cube_width,cube_height;
  prim_rec_ptr new_prim_rec;
  ;
  /* not used except for readability*/
  fscanf(infile,"\n\n\tObject %d:\n",&objnumdum);

  newobj = make_obj_noshell(world);

  fscanf(infile,"Object Name:%s\n",obj_name);
  tempobjname = reset_white_space(obj_name);
  if (strcmp(obj_name,"<none>") != 0)
    set_object_name(newobj,tempobjname);
  free_elem((elemptr *) &tempobjname);

  fscanf (infile," Object Transform:\n");
  read_matrix(infile,newobj->xform);
  fscanf (infile," Object Inverse Transform:\n");
  read_matrix(infile,newobj->invxform);

  fscanf (infile,"Object Parent:%d\n", &parent_id);

  if (find_object_by_number(world->root,&parent,parent_id,&search_id))
    if (parent != world->root)
      adopt_obj_branch(parent,newobj,FALSE);
  
  fscanf(infile,"Object Color:%hd %hd %hd\n",
	 &(newobj_color[0]),&(newobj_color[1]),
	 &(newobj_color[2]));
  set_feature_color((featureptr) newobj,newobj_color);

  fscanf (infile, "Object has %d properties:\n",&numprops);
  read_feature_props(infile,(featureptr) newobj,numprops);
  
  if (has_property((featureptr) newobj, primitive_prop))
  {
    fscanf(infile, "\tPrimitive Info:\n");
    fscanf(infile, "\tCube Length:%f\n",&cube_length);
    fscanf(infile, "\tCube Width:%f\n",&cube_width);
    fscanf(infile, "\tCube Height:%f\n",&cube_height);
    new_prim_rec = (prim_rec_ptr) alloc_elem(Primrecsize);
    new_prim_rec->prim_id = Cube_id;
    new_prim_rec->prim_info = (void *) alloc_elem(Cuberecsize);
    ((cube_rec_ptr) new_prim_rec->prim_info)->length = cube_length;
    ((cube_rec_ptr) new_prim_rec->prim_info)->width = cube_width;
    ((cube_rec_ptr) new_prim_rec->prim_info)->height = cube_height;
    set_property(newobj,primitive_prop,new_prim_rec);
  }
  
  fscanf (infile, "Object Selectechnique:%s\n",newtechnique);
  newobj->selectechnique = (find_technique(newtechnique,selectechniques))->
    technique_rtn;
  fscanf (infile, "Object Drawtechnique:%s\n",newtechnique);
  newobj->drawtechnique = (find_technique(newtechnique,drawtechniques))->
    technique_rtn;
  fscanf (infile, "Object Screenbboxtechnique:%s\n",newtechnique);
  newobj->screenbboxtechnique = (bboxscreenptr (*)())
    (find_technique(newtechnique,
		    screenbboxtechniques))->
		      technique_rtn;

  fscanf (infile,"\nNumber of Shells:%d\n",&numshells);
  read_shells(infile,newobj,numshells);

  /* Especially important on DXF imported files */
  if (!has_property((featureptr) newobj, nonmanifold_prop))
    find_obj_normals(newobj);

/*
  dumb_recombine_coplanar_faces(newobj);
*/
  log_update((featureptr) newobj,Allupdateflags,Immediateupdate);

  /* for debugging */
/*
  topology_check_obj(newobj);
*/

  return(newobj);
}


  void
read_branch(FILE *infile, objptr parentobj)
  /* adds objs from passed */
  /* file infile_name to obj_list*/
{
  objptr new_obj,thischild;
  int numobjs,i;
  char branch_name[50] = { 0 }, branch_motherworld_name[50] = { 0 };
  worldptr branch_world;
  ;
  /* Use the scratch world to read it in before merging it into the */
  /* requested world. */
  clear_world(scratch_world);

  fscanf(infile, "Branch Name:%s\n", branch_name);
  if (fscanf(infile,"Branch Preferred Motherworld:%s\n",branch_motherworld_name) < 1)
    strcpy(branch_motherworld_name, "(none)");
  /* if branch_motherworld_name == "<none>" the preferred pointer will */
  /* be Nil */
  branch_world = find_world(branch_motherworld_name);
  
  fscanf (infile,"\tTotal Number of Objects In Branch:%d\n",&numobjs);
  i = 0;
  while (i < numobjs)
  {
    new_obj = read_object(infile,scratch_world);
    if ((strcmp(branch_name,"<none>") != 0) && (i == 0))
      set_object_name(new_obj,branch_name);
    i++;
  }

  /* Now combine the read-in objects from the scratch world into the */
  /* requested world. */
  thischild = scratch_world->root->children->first.obj;
  while (thischild != Nil)
  {
    if ((branch_world != Nil) && (parentobj != Nil))
      if (parentobj->motherworld != branch_world)
	adopt_obj_branch(branch_world->root,thischild,TRUE);
      else
	adopt_obj_branch(parentobj,thischild,TRUE);	/* assume TRUE linkage */
    else
      adopt_obj_branch(parentobj,thischild,TRUE);	/* assume TRUE linkage */
    thischild = thischild->next;
  }
}


  void
read_world(FILE *infile)
{
  objptr new_obj;
  int numobjs;
  worldptr brave_new_world;
  char new_world_name[50];
  ;
  /* lousy hack requires access to world_list without passing it */
  brave_new_world = (worldptr) append_new_blank_elem(world_list,World_type);

  fscanf(infile,"World Name:%s\n",new_world_name);
  if (strcmp(new_world_name,"<none>") != 0)
    set_world_name(brave_new_world,new_world_name);
  fscanf (infile,"\tTotal Number of Objects In World:%d\n",&numobjs);
  
  while (numobjs-- > 0)
    new_obj = read_object(infile,brave_new_world);
  merge_worlds(brave_new_world,working_world);
}

  void
resolve_decals(void)
{
#if 0
  int i = 0;
  ;
  while ((decalobj = (objptr)
	  get_property_feature(search_world,decal_prop,i)) != Nil)
  {
    apply_front_decal
#endif  
}
  
  FILE *
find_input_file(char *infile_name)
{
  FILE *infile;
  filename_type total_name;
  ;
  /* first, check if opening icon files */
  if (strstr(infile_name,".icon") != NULL)
  {
    sprintf(total_name,"./icons/%s",infile_name);
    infile = fopen(total_name,"r");
    return(infile);
  }
  sprintf(total_name,"./cut/%s",infile_name);
  /* user doesn't have to type .cut extension any more!!! yeah!!! */
  if (strstr(total_name,".cut") == NULL)
    strcat(total_name,".cut");
  infile = fopen(total_name,"r");
  if (infile == NULL)
  {
    strcpy(total_name,infile_name);
    if (strstr(total_name,".cut") == NULL)
      strcat(total_name,".cut");
    infile = fopen(total_name,"r");
  }
  return(infile);
}

/* read a branch of a tree from a file, or read an entire world to a file */
/* depending on the identifier on the second line in the file */

  Boolean
read_data_file(char *infile_name,featureptr parentfeature)
{
  FILE *infile;
  objptr motherobj;
  char outfile_name[NAMELENGTH],filetypestr[NAMELENGTH];
  ;
  printf ("Reading file:%s...\n",infile_name);
  if (!special_file(infile_name))
  {
    infile = find_input_file(infile_name);
    if (infile == NULL)
    {
      system_warning ("read_data_file: File not found error! ***\n");
      return(FALSE);
    }
    fscanf(infile,"** Start of file %s **\n",outfile_name);
#if 0				/* this is a total waste... */
    if (strcmp(infile_name,outfile_name) != 0)
      printf ("read_world warning: File has incorrect title!\n");
#endif
    
    fscanf(infile,"Contains:%s\n",filetypestr);
    if (strcmp(filetypestr,"World") == 0)
      /* in this case, pass parentfeature == Nil */
      read_world(infile);		
    else if (strcmp(filetypestr,"Branch") == 0)
    {
      motherobj = (objptr) find_largergrain(parentfeature,Obj_type);
      read_branch(infile,motherobj);
    }
    fscanf(infile,"** End of file %s **\n",infile_name);
    fclose(infile);
  }
    
  resolve_decals();
  /* You must resolve CPL references to objects that didn't exist while */
  /* things were being loaded. */
  resolve_CPL_forward_object_refs();
  
  printf ("Done reading file:%s...\n",infile_name);
  fflush(stdout);
  return(TRUE);
}

/* this specialized obj reading routine reads decal files, which are */
/* just strings of polygons with separators between each polygon. It creates */
/* a new child lamina obj of the passed parent for each polygon. */

  void
read_decal_file(objptr parent,char *decalfile_name)
{
  FILE *decal_file;
  char decalfile_inname[50];
  objptr first_decal_obj,decal_obj;
  double decal_scale;
  vertype attachpt,offset, decal_origin,pos,rotvec,rotpt,decal_fnorm;
  long rotangle;
  int numpolys,numverts;
  colortype polycolor;
  leptr firstle,thisle;
  Boolean firstvert,firstpoly = TRUE;
  worldptr world;
  ;
  world = parent->motherworld;
  decal_file = fopen(decalfile_name,"r");
  if (decal_file == NULL)
  {
    printf ("Warning:Decal file %s not found\n",decalfile_name);
    return;
  }
  
  fscanf(decal_file, "*** Start of Decal File: %s\n",decalfile_inname);
  fscanf(decal_file,"Number of Polygons:%d\n",&numpolys);
  fscanf(decal_file,"Decal Origin:%f %f %f\n",&(decal_origin[vx]),
	 &(decal_origin[vy]),&(decal_origin[vz]));
  fscanf(decal_file,"Decal Scale:%lf\n",&decal_scale);
  while (numpolys-- > 0)
  {
    fscanf(decal_file, "Polygon Color:%hd %hd %hd\n",&(polycolor[0]),
	   &(polycolor[1]),&(polycolor[2]));
    fscanf(decal_file, "Number of Vertices:%d\n",&numverts);
    firstvert = TRUE;
    while (numverts-- > 0)
    {
      fscanf (decal_file,"%f %f %f\n",&(pos[vx]),&(pos[vy]),&(pos[vz]));
      pos[vw] = 1.0;
      scalevec3d(pos,decal_scale,pos);
      addvec3d(pos,decal_origin,pos);
      if (firstvert)
      {
	firstvert = FALSE;
	decal_obj = make_obj(world,pos);
	if (firstpoly)
	  first_decal_obj = decal_obj;
	firstle = thisle = First_obj_le(decal_obj);
      }
      else
	thisle = make_edge_vert(thisle,thisle,pos);
    }
    make_edge_face(thisle,firstle);

#if 0			/* all stuff to attach decals to parents ignored */
    calc_face_norm(First_obj_fve(decal_obj),decal_fnorm);
    rotangle = (int) (Rad2deg * Acos(dotvecs(decal_attach_fve->facenorm,
					     decal_fnorm))) * 10;
    cross_prod(decal_attach_fve->facenorm,decal_fnorm,rotvec);
    copypos3d(decal_attach_fve->boundlist->first.bnd->lelist->first.le->
	     facevert->pos,attachpt);
    diffpos3d(First_obj_vfe(decal_obj)->pos,attachpt,offset);
    translate_feature ((featureptr) decal_obj,offset,Donotusetranslateflags);
    axis_rot_obj(decal_obj,rotvec,attachpt,rotangle);
#endif    
    if (firstpoly)
    {
      if (parent->parent == Nil) /* adopt with TRUE if adding decal to root */
	adopt_obj_branch(parent,decal_obj,TRUE);
      else
	adopt_obj_branch(parent,decal_obj,FALSE);
    }
    else
      adopt_obj_branch(first_decal_obj,decal_obj,FALSE);
    firstpoly = FALSE;
    set_feature_color((featureptr) decal_obj,polycolor);
    add_property((featureptr) decal_obj,nonmanifold_prop);
  }
    
  fscanf(decal_file,"*** End of Decal File\n");
  fclose(decal_file);
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                        OBJECT WRITING ROUTINES BEGIN                 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
write_matrix(FILE *outfile,Matrix thematrix)
{
  int i,j;
  ;
  for (i = 0; i < 4; ++i)
  {
    fprintf(outfile," [ ");
    for (j = 0; j <4; ++j)
      fprintf(outfile," %f ",thematrix[i][j]);
    fprintf(outfile," ]\n");
  }
}

  void
write_vfes(outfile,vfelist)  /* write vertex array */
  FILE *outfile;
  listptr vfelist;
{
  vfeptr thisvfe;
  int vfecnt = 1;
  ;
  fprintf (outfile," Number of Vertices:%d\n",
	   vfelist->numelems);
  fprintf (outfile," VFE List:\n");
  thisvfe = vfelist->first.vfe;
  while (thisvfe != Nil)
  {
    fprintf (outfile,"Vertex %d:%5.1f %5.1f %5.1f\n",
	     vfecnt++,
	     thisvfe->local_pos[vx],
	     thisvfe->local_pos[vy],
	     thisvfe->local_pos[vz]);
    thisvfe = thisvfe->next;
  }
}


  void
write_evfs(outfile,thishell)  /* write out edge-vertex list for this shell */
  FILE *outfile;
  shellptr thishell;
{
  evfptr this_evf;
  int edgecnt = 1;
  ;
  fprintf (outfile," EVF List:\n");
  this_evf = thishell->evflist->first.evf;
  while (this_evf != Nil)
  {
    fprintf (outfile,"Edge %d: (Vertices %d, %d)\n",edgecnt++,
	     get_elem_index((elemptr) this_evf->le1->facevert,
			    (listptr) thishell->vfelist),
	     get_elem_index((elemptr) this_evf->le2->facevert,
			    (listptr) thishell->vfelist));
    this_evf = this_evf->next;
  }
}

  void
write_bounds(outfile,this_fve,thishell)
  FILE *outfile;
  fveptr this_fve;
  shellptr thishell;
{
  boundptr firstbound,thisbound;
  int boundcnt = 1;
  leptr firstle,thisle;
  ;
  thisbound = this_fve->boundlist->first.bnd;
  while (thisbound != Nil)
  {
    fprintf (outfile,"\tBound %d (%d le's):",boundcnt++,
	     thisbound->lelist->numelems);
    thisle = firstle = thisbound->lelist->first.le;
    do
    {
      fprintf (outfile,"%d ",get_elem_index((elemptr) thisle->facevert,
					    (listptr) thishell->vfelist));
      thisle = thisle->next;
    } while (thisle != firstle);
    fprintf(outfile,"\n");
    thisbound = thisbound->next;
  }
}

  void
write_fves(outfile,thishell)
  FILE *outfile;
  shellptr thishell;
  /* write face list */
{
  fveptr this_fve;
  int facecount = 1;
  ;
  fprintf (outfile," Number of Faces:%d\n",
	   thishell->fvelist->numelems);
  fprintf (outfile," FVE List:\n");
  this_fve = thishell->fvelist->first.fve;
  while (this_fve != Nil)
  {
    fprintf (outfile,"Face:%d (%d bounds)\n",facecount++,
	     this_fve->boundlist->numelems);
    write_bounds(outfile,this_fve,thishell);
    this_fve = this_fve->next;
  }
}

  void
write_feature_props(FILE* outfile, featureptr thisfeature)
{
  proptr thisprop;
  char decalfilename[50];
  ;
  for (thisprop = thisfeature->proplist->first.prop; thisprop != Nil;
       thisprop = thisprop->next)
    fprintf(outfile, "%s\n",propinfo[thisprop->propkind].propname);
  if (has_property(thisfeature,hasdecal_prop))
  {
    /* HACKKK */
    /* i have no way of figuring out the decalobj file name right now, */
    /* especially if the decal object has been killed as in the case of */
    /* the headpads.... */
    strcpy(decalfilename,"***FILLMEIN!!***");
    fprintf(outfile, "Decal File:%s\n", decalfilename);
  }
}

  void
write_face_colors(FILE *outfile, shellptr thishell)
{
  fveptr thisfve;
  int count;
  ;
  for (thisfve = thishell->fvelist->first.fve, count = 0; thisfve != Nil;
       thisfve = thisfve->next)
    if (thisfve->thecolor != Nil)
      count++;
  /* right now faces only */
  fprintf (outfile,"Faces with different colors:%d\n",count); 
  for (thisfve = thishell->fvelist->first.fve, count = 1; thisfve != Nil;
       thisfve = thisfve->next, count++)
    if (thisfve->thecolor != Nil)
      fprintf (outfile, "\tFace %d:%hd %hd %hd\n", count,
	       thisfve->thecolor[0],thisfve->thecolor[1],thisfve->thecolor[2]);
}

  void
write_shell_properties(FILE* outfile, shellptr thishell)
{
  vfeptr thisvfe;
  evfptr thisevf;
  fveptr thisfve;
  int count;
  ;
  for (thisvfe = thishell->vfelist->first.vfe,count = 0; thisvfe != Nil;
       thisvfe = thisvfe->next)
    if (thisvfe->proplist->numelems > 0)
      count++;
  fprintf (outfile, "Vertices with properties:%d\n",count);
  for (thisvfe = thishell->vfelist->first.vfe,count = 1; thisvfe != Nil;
       thisvfe = thisvfe->next,count++)
    if (thisvfe->proplist->numelems > 0)
    {
      fprintf (outfile, "Vertex %d has %d properties:\n",count,
	       thisvfe->proplist->numelems);
      write_feature_props(outfile, (featureptr) thisvfe);
    }

  for (thisevf = thishell->evflist->first.evf,count = 0; thisevf != Nil;
       thisevf = thisevf->next)
    if (thisevf->proplist->numelems > 0)
      count++;
  fprintf (outfile, "Edges with properties:%d\n",count);
  for (thisevf = thishell->evflist->first.evf,count = 1; thisevf != Nil;
       thisevf = thisevf->next,count++)
    if (thisevf->proplist->numelems > 0)
    {
      fprintf (outfile, "Edge %d has %d properties:\n",count,
	       thisevf->proplist->numelems);
      write_feature_props(outfile, (featureptr) thisevf);
    }

  for (thisfve = thishell->fvelist->first.fve,count = 0; thisfve != Nil;
       thisfve = thisfve->next)
    if (thisfve->proplist->numelems > 0)
      count++;
  fprintf (outfile, "Faces with properties:%d\n",count);
  for (thisfve = thishell->fvelist->first.fve,count = 1; thisfve != Nil;
       thisfve = thisfve->next,count++)
    if (thisfve->proplist->numelems != 0)
    {
      fprintf (outfile, "Face %d has %d properties:\n",count,
	       thisfve->proplist->numelems);
      write_feature_props(outfile, (featureptr) thisfve);
    }
}

  void
write_CPLprivatecode_name(FILE* outfile, featureptr thisfeature)
{
  CPL_word_ptr privatecode_start;
  char privatecode_name[MaxCPLabelength],privatecode_filename[MaxCPLabelength];
  int count;
  ;
  privatecode_start = thisfeature->privatecode;
  if (!lookup_symbol_from_address(privatecode_start,privatecode_name))
    return;			/* private code had no name: too bad you lose! */
  for (count = 0; count < strlen(privatecode_name); ++count)
  {
    if (!islower(privatecode_name[count]))
      privatecode_name[count] = tolower(privatecode_name[count]);
  }
  strcpy(privatecode_filename,"./cpl/");
  strcat(privatecode_filename,privatecode_name);
  strcat(privatecode_filename,".cpl");
  fprintf(outfile, "%s\n",privatecode_filename);
}
  
  void
write_shell_CPLcode(FILE* outfile, shellptr thishell)
{
  /* needs to write the CPL code file name that got this private code for the */
  /* feature into the object... but since thats not stored... i'll take */
  /* label of first address as the filename. (after lowercasing it)*/
  vfeptr thisvfe;
  evfptr thisevf;
  fveptr thisfve;
  int count;
  ;
  for (thisvfe = thishell->vfelist->first.vfe,count = 0; thisvfe != Nil;
       thisvfe = thisvfe->next)
    if (thisvfe->privatecode != Nil)
      count++;
  fprintf (outfile, "Vertices with CPL private code:%d\n",count);
  for (thisvfe = thishell->vfelist->first.vfe,count = 1; thisvfe != Nil;
       thisvfe = thisvfe->next,count++)
    if (thisvfe->privatecode != Nil)
    {
      fprintf (outfile, "Vertex %d: ",count);
      write_CPLprivatecode_name(outfile,(featureptr) thisvfe);
    }

  for (thisevf = thishell->evflist->first.evf,count = 0; thisevf != Nil;
       thisevf = thisevf->next)
    if (thisevf->privatecode != Nil)
      count++;
  fprintf (outfile, "Edges with CPL private code:%d\n",count);
  for (thisevf = thishell->evflist->first.evf,count = 1; thisevf != Nil;
       thisevf = thisevf->next,count++)
    if (thisevf->privatecode != Nil)
    {
      fprintf (outfile, "Edge %d: ",count);
      write_CPLprivatecode_name(outfile,(featureptr) thisevf);
    }

  for (thisfve = thishell->fvelist->first.fve,count = 0; thisfve != Nil;
       thisfve = thisfve->next)
    if (thisfve->privatecode != Nil)
      count++;
  fprintf (outfile, "Faces with CPL private code:%d\n",count);
  for (thisfve = thishell->fvelist->first.fve,count = 1; thisfve != Nil;
       thisfve = thisfve->next,count++)
    if (thisfve->privatecode != Nil)
    {
      fprintf (outfile, "Face %d: ",count);
      write_CPLprivatecode_name(outfile,(featureptr) thisfve);
    }
}

  void
write_specials(FILE* outfile, shellptr thishell)
{
  /* write out unique colors */
  write_face_colors(outfile, thishell);
  /* write out properties */
  write_shell_properties(outfile, thishell);
  /* write out private cpl code names */
  write_shell_CPLcode(outfile, thishell);
}

  void
write_shell(FILE* outfile,shellptr thishell)
{
  /* write shell's vertices */
  write_vfes(outfile,thishell->vfelist);
  /* write bounds lists for each face */
  write_fves(outfile,thishell);
  /* write special records for the shell if any */
  write_specials(outfile,thishell);
}

  void
write_shells(FILE* outfile,objptr thisobj)
{
  shellptr thishell,last_shell;
  int shellcount = 1;
  ;
  fprintf(outfile,"\nNumber of Shells:%d\n",thisobj->shellist->numelems);
  thishell = First_obj_shell(thisobj);
  while (thishell != Nil)
  {
    fprintf (outfile, "Shell %d:\n",shellcount++);
    write_shell(outfile,thishell);
    thishell = thishell->next;
  }
}

/* given an object, find the number of the object in the tree, counting */
 /* in PREORDER fashion (NOT the level of the object...) LEFT-TO-RIGHT. */

  Boolean
find_object_number(objptr thisobj, objptr searchobj,
		   int *search_id, int *found_id)
{
  objptr thischild;
  ;
  if (thisobj == searchobj)
  {
    *found_id = *search_id;
    return(TRUE);		/* we found it! */
  }
  else
  {
    *search_id += 1;
    thischild = thisobj->children->first.obj;
    while (thischild != Nil)
    {
      if (find_object_number(thischild,searchobj,search_id,found_id))
	return(TRUE);
      thischild = thischild->next;
    }
  }
  return(FALSE);
}

  void
count_objects(objptr grouproot,objptr thisobj, int *count)
{
  objptr thischild;
  ;
  if (thisobj != grouproot)
    *count += 1;

  thischild = thisobj->children->first.obj;
  while (thischild != Nil)
  {
    count_objects(grouproot,thischild,count);
    thischild = thischild->next;
  }
}


	
  char *
fix_white_space(char *orig_name)
{
  int i = 0,orig_name_len;
  char *tempobjname;
  ;
  orig_name_len = strlen(orig_name);
  tempobjname = (char *) alloc_elem(orig_name_len+1);
  strcpy(tempobjname,orig_name);
  while (i < orig_name_len)
  {
    if ((tempobjname[i] == ' ') || (tempobjname[i] == '\t'))
      tempobjname[i] = 27;	/* put esc's in place of white space, clean */
				/* when reading...*/
    i++;
  }
  return(tempobjname);
}
  

/* writes one obj from tree to outfile */
  void
write_object(FILE* outfile,objptr thisobj)
{
  int parent_id,search_id = 0;
  char *tempobjname;
  prim_rec_ptr old_prim_rec;
  Coord cube_length,cube_width,cube_height;
  ;
  fprintf (outfile,"\n\n\tObject %d:\n",object_id);
  if (thisobj->obj_name != Nil)
  {
    tempobjname = fix_white_space(thisobj->obj_name);
    fprintf (outfile,"Object Name:%s\n",tempobjname);
    free_elem((elemptr *) &tempobjname);
  }
  else
    fprintf (outfile,"Object Name:<none>\n");

  fprintf (outfile," Object Transform:\n");
  write_matrix(outfile,thisobj->xform);
  fprintf (outfile," Object Inverse Transform:\n");
  write_matrix(outfile,thisobj->invxform);

  if (find_object_number(thisobj->motherworld->root,
			 thisobj->parent,&search_id,&parent_id))
    fprintf (outfile,"Object Parent:%d\n", parent_id);

  fprintf (outfile, "Object Color:%hd %hd %hd\n",
	   thisobj->thecolor[0],thisobj->thecolor[1],thisobj->thecolor[2]);

  fprintf (outfile, "Object has %d properties:\n",thisobj->proplist->numelems);
  write_feature_props(outfile,(featureptr) thisobj);
  if (has_property((featureptr) thisobj,primitive_prop))
  {
    old_prim_rec = (prim_rec_ptr) get_property_val((featureptr) thisobj,
					       primitive_prop).ptr;
    cube_length = ((cube_rec_ptr) old_prim_rec->prim_info)->length;
    cube_width = ((cube_rec_ptr) old_prim_rec->prim_info)->width;
    cube_height = ((cube_rec_ptr) old_prim_rec->prim_info)->height;

    fprintf(outfile, "\tPrimitive Info:\n");
    fprintf(outfile, "\tCube Length:%2.2f\n",cube_length);
    fprintf(outfile, "\tCube Width:%2.2f\n",cube_width);
    fprintf(outfile, "\tCube Height:%2.2f\n",cube_height);
  }

  fprintf (outfile, "Object Selectechnique:%s\n",
	   find_technique_name(thisobj->selectechnique,selectechniques));
  fprintf (outfile, "Object Drawtechnique:%s\n",
	   find_technique_name(thisobj->drawtechnique,drawtechniques));
  fprintf (outfile, "Object Screenbboxtechnique:%s\n",
	   find_technique_name((void (*)()) thisobj->screenbboxtechnique,
			       screenbboxtechniques));
  write_shells(outfile,thisobj);
}


/* walk down the tree writing the shells in preorder (parents first). */
  void
write_objects(FILE* outfile, worldptr world,objptr thisobj)
{
  objptr thischild;
  ;
  if (thisobj != world->root) /* never write the tree's root */
    write_object(outfile,thisobj);
  object_id++;
  thischild = thisobj->children->first.obj;
  while (thischild != Nil)	/* write the children next */
  {
    write_objects(outfile,world,thischild);
    thischild = thischild->next;
  }
}

  void
write_branch(objptr branchroot,FILE *outfile)
{
  int branch_obj_count = 1;	/* start at one since the branchroot is */
  objptr branchmother,mother_doomed; /* included */
  Boolean linkstatus;
  char preferred_world_name[50]; /* hacked size */
  ;
  fprintf(outfile,"Branch Name:");
  if (branchroot->obj_name != Nil)
    fprintf(outfile,"%s\n",branchroot->obj_name);
  else
    fprintf(outfile,"<none>\n");
  count_objects(branchroot,branchroot,&branch_obj_count);
  if (branchroot->motherworld == primary_world)
    strcpy(preferred_world_name,"<none>");
  else
    strcpy(preferred_world_name,branchroot->motherworld->world_name);
  fprintf (outfile,"Branch Preferred Motherworld:%s\n",preferred_world_name);
  fprintf (outfile,"\tTotal Number of Objects In Branch:%d\n",branch_obj_count);

  object_id = 0;

  /* Make sure not to saved picked scalebboxes. */
  clear_property(picked_prop);
  while ((mother_doomed =
	 (objptr) get_property_feature(working_world,scalebbox_prop,1)) != Nil)
    kill_obj(mother_doomed);
  
  /* move the branch into the scratch world before writing it so that object */
  /* id's will not also refer to the objects in existence in this branch's */
  /* motherworld. */
  branchmother = branchroot->parent;
  linkstatus = branchroot->linked;
  clear_world(scratch_world);
  adopt_obj_branch(scratch_world->root,branchroot,TRUE);
  zoom_world(scratch_world, origin, 1.0 / fetch_system_zoom());
  write_objects(outfile,branchroot->motherworld,branchroot);
  zoom_world(scratch_world, origin, fetch_system_zoom());
  adopt_obj_branch(branchmother,branchroot,linkstatus);
}

  void
write_world(worldptr world,FILE *outfile)
  /* write a world out to given outfile */
{
  Matrix zoomxform,zoominvxform;
  shellptr thishell;
  int world_obj_count = 0;	/* start at zero since the world root is */
  ;				/* NOT included */
  fprintf(outfile,"World Name:");
  if (world->world_name != Nil)
    fprintf(outfile,"%s\n",world->world_name);
  else
    fprintf(outfile,"<none>\n");
  count_objects(world->root,world->root,&world_obj_count);
  fprintf (outfile,"\tTotal Number of Objects In World:%d\n",world_obj_count);

  object_id = 0;
  copy_matrix(world->viewer.xform,zoomxform);
  copy_matrix(world->viewer.invxform,zoominvxform);
  totally_unzoom_world(world);
  write_objects(outfile,world,world->root);
  totally_zoom_world(world,zoomxform,zoominvxform);
}


  FILE *
create_output_file(char *outfile_name)
{
  FILE *outfile;
  filename_type total_name;
  ;
  /* first, check if writing icon files */
  if (strstr(outfile_name,".icon") != NULL)
  {
    sprintf(total_name,"./icons/%s",outfile_name);
    outfile = fopen(total_name,"w");
    return(outfile);
  }
  sprintf(total_name,"./cut/%s",outfile_name);
  /* user doesn't have to type .cut extension any more!!! yeah!!! */
  /* Always puts .cut files into the cut directory below the working dir. */
  /* if there is no .cut directory, this will DIE... should check with ftest */
  /* but that is really non portable.... hack.*/
  if (strstr(total_name,".cut") == NULL)
    strcat(total_name,".cut");
  outfile = fopen(total_name,"w");
  return(outfile);
}

/* write a branch of a tree to a file, or write an entire world to a file */
/* depending on the type_id of the passed feature. */

  void
write_data_file(char *outfile_name,featureptr thefeature)
{
  FILE *outfile;
  objptr motherobj;
  char dxf_outfile_name[150];	/* hack size */
  ;
  printf ("Writing file:%s...",outfile_name);
  outfile = create_output_file(outfile_name);
  fprintf(outfile,"** Start of file %s **\n",outfile_name);
  if (thefeature->type_id == World_type)
  {
    fprintf(outfile,"Contains:World\n");
    write_world((worldptr) thefeature,outfile);
    sprintf (dxf_outfile_name,"%s.dxf", outfile_name);
/*    write_dxf_file(dxf_outfile_name,(worldptr) thefeature);*/
  }
  else
  {
    fprintf(outfile,"Contains:Branch\n");
    motherobj = (objptr) find_largergrain(thefeature,Obj_type);
    write_branch(motherobj,outfile);
  }
  fprintf(outfile,"** End of file %s **\n",outfile_name);
  fclose(outfile);

  printf ("Done writing file:%s...\n",outfile_name);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                 AUTOCAD OUTPUT ROUTINES BEGIN		        */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  Boolean
write_dxf_head(FILE *outfile)
{
  /* here we just copy the header from a generic file I got from ACAD itself */
  /* rather than try to figure out all these entities and what they do. */
  static char headerfilename[] = "./rsc/ACAD_header.rsc";
  FILE *headerfile;
  char c;
  ;
  headerfile = fopen(headerfilename,"r");
  if (headerfile == NULL)
    return(FALSE);
  while ((c = (char) getc(headerfile)) != '@') /* lameness because iris eof */
  {				/* is weird */
    putc(c,outfile);
  }
  fclose(headerfile);
  return(TRUE);
}

  void
write_dxf_tail(FILE *outfile)
{
  /* write end of file marker */
  fprintf (outfile,"%3d\nENDSEC\n%3d\nEOF\n",0,0);
}

  void
write_dxf_vertex(FILE *outfile, vertype thisvert)
{
  fprintf (outfile,"%3d\n%f\n%3d\n%f\n%3d\n%f\n",
	   10,thisvert[vx],20,thisvert[vy],30,thisvert[vz]);
}

  void
write_dxf_polyline(FILE *outfile, boundptr thisbound)
{
  leptr thisle,startle;
  Coord *thisvert;
  ;
  // See Paul Bourke's page: http://paulbourke.net/dataformats/dxf
  /* 0 = Entity group code
     POLYLINE = type of entity
     8 = Layer group code
     0 = layer #
     66 = Vertices Follow Flag group code
     1 = Always 1 (i.e., true)
     10 = X of polyline elevation
     0.0 = dummy value, always zero
     20 = Y of polyline elevation
     0.0 = dummy value, always zero
     30 = Z of polyline elevation
     0.0 = the elevation
     70 = Polyline attribute bit codes
     9 = 1 (closed) | 8 (3d)

     0 = entity group code
     VERTEX = type of entity
     8 = Layer groupo code
     0 = layer #
     10 = X coordinate
     nnn.nnn = value of X coordinate
     20 = Y coordinate
     nnn.nnn = value of Y coordinate
     30 = Z coordinate
     nnn.nnn = value of Z coordinate
     70 = Vertex attribute bit codes
     32 = this is a 3D polyline vertex
     ...
  */
  /* write 0 group code, POLYLINE marker, layer 0, "ENTITIES FOLLOW" flag */
  fprintf(outfile, "%3d\nPOLYLINE\n%3d\n0\n%3d\n%5d\n", 0, 8, 66, 1);
  /* write the first entity (is this necessary? unclear) */
  fprintf (outfile," 10\n0.0\n 20\n0.0\n 30\n%f\n", 0.0);
  fprintf (outfile," 70\n9\n");	/* write polyline flags!!! much necessary */
  
  thisle = startle = thisbound->lelist->first.le;
  do
  {
    thisvert = thisle->facevert->pos;
    /* identify this group as a vertex in layer 0 */
    fprintf (outfile,"%3d\nVERTEX\n%3d\n0\n",0,8);
    /* put down x,y,z coords for this vertex */
    write_dxf_vertex(outfile,thisvert);
    /* mark this vertex with a VERTEX FLAG indicating it's part of a */
    /* polyline. */
    fprintf (outfile,"%3d\n%6d\n",70,32);
    thisle = thisle->next;
  } while (thisle != startle);

  fprintf (outfile,"%3d\nSEQEND\n%3d\n0\n",0,8);
}

  void
write_dxf_face(FILE *outfile, boundptr thisbound)
{
  leptr thisle,startle;
  Coord *thisvert;
  int i = 0;
  ;
  /* thisfunction assumes the shell has been triangularized */
  /* write 0 group code, 3DFACE marker layer 0 group*/
  fprintf(outfile,"  0\n3DFACE\n  8\n0\n");
  thisle = startle = thisbound->lelist->first.le;
  do
  {
    thisvert = thisle->facevert->pos;
    /* put down x,y,z coords for this vertex */
    fprintf (outfile,"%3d\n%f\n%3d\n%f\n%3d\n%f\n",
	     10+i,thisvert[vx],20+i,thisvert[vy],30+i,thisvert[vz]);
    i++;
    thisle = thisle->next;
  } while (thisle != startle);
  /* make last point same as the first */
  thisvert = thisle->facevert->pos;
  fprintf (outfile,"%3d\n%f\n%3d\n%f\n%3d\n%f\n",
	   13,thisvert[vx],23,thisvert[vy],33,thisvert[vz]);
  fprintf (outfile," 70\n8\n");	/* invisible edge flag, 4th edge is invisible */
  /* we could make better use of this flag using the original shell edge info */
}

  void
write_dxf_shell(FILE *outfile, shellptr thishell)
{
  Coord *thisvert;
  fveptr thisfve;
  boundptr thisbound;
  ;
  for (thisfve = thishell->fvelist->first.fve; thisfve != Nil;
       thisfve = thisfve->next)
    /* write the vertices in order going around the face...*/
    for (thisbound = thisfve->boundlist->first.bnd; thisbound != Nil;
	 thisbound = thisbound->next)
      write_dxf_face(outfile,thisbound); /* must be a triangular face */
}

  void
write_dxf_object(FILE* outfile, objptr thisobj)
{
  shellptr thishell;
  objptr output_object;
  ;
  output_object = duplicate_obj(thisobj); /* put dup in scratch world */
  planarize_object(output_object);
  thishell = First_obj_shell(thisobj);
  while (thishell != Nil)
  {
    write_dxf_shell(outfile,thishell);
    thishell = thishell->next;
  }
  kill_obj(output_object);
}

  void
write_dxf_objects(FILE* outfile, worldptr world, objptr thisobj)
{
  objptr thischild;
  ;
  if (thisobj != world->root) /* never write the tree's root */
    write_dxf_object(outfile,thisobj);
  thischild = thisobj->children->first.obj;
  while (thischild != Nil)	/* write the children next */
  {
    write_dxf_objects(outfile,world,thischild);
    thischild = thischild->next;
  }
}

/* Write Autocad DXF formatted file from shell_list */
  void
write_dxf_file(char outfile_name[],worldptr world)
{
  FILE *outfile;
  shellptr thishell,dupedshell;
  listptr shell_list;
  ;
  printf ("Writing dxf file:%s...",outfile_name);
  outfile = fopen(outfile_name,"w");
  if (write_dxf_head(outfile) != TRUE)
  {
    printf ("Error in write_dxf_file: no generic header file found!\n");
    fclose(outfile);
    return;
  }
  write_dxf_objects(outfile,world,world->root);
  write_dxf_tail(outfile);
  fclose(outfile);
  printf ("done.\n");
}


/* Code for reading resource files for Dialog Plates */

  void
read_plate_labels(objptr plate, int num_labels,FILE *plate_rscfile)
{
  int i,j,label_pen_width;
  vertype label_offset;
  Coord label_xscale,label_yscale;
  char labelstr[50],label_stylestr[50];
  colortype label_color;
  ;
  for (i = 0; i < num_labels; ++i)
  {
    fscanf(plate_rscfile,"\tLabel Text:");	/* skip past indent */
    getc(plate_rscfile);	/* skip past open quote */
    j = 0;
    while ((labelstr[j++] = getc(plate_rscfile)) != '"') ;
    labelstr[j-1] = '\0';	/* erase tailing doublequote */

    fscanf(plate_rscfile,"\tLabel Offset:[%f,%f,%f]\n",
	   &label_offset[vx],&label_offset[vy],&label_offset[vz]);
    label_offset[vw] = 1.0;
    if (i == 0)
      create_label(plate,labelstr,label_offset,Hershey);
    else
      add_label_item(plate,labelstr,label_offset,Hershey);
    fscanf(plate_rscfile,"\tLabel Color:%hd,%hd,%hd\n",
	   &label_color[0],&label_color[1],&label_color[2]);
    set_label_item_color(plate,i+1,label_color);
    fscanf(plate_rscfile,"\tLabel Pen Width:%d\n", &label_pen_width);
    set_label_item_pen_width(plate,i+1,label_pen_width);
    fscanf(plate_rscfile,"\tLabel Scale:%f,%f\n",
	   &label_xscale,&label_yscale);
    set_label_item_scale(plate,i+1,label_xscale,label_yscale);
    fscanf(plate_rscfile,"\tLabel Style:%s\n",label_stylestr);
    for (j = 0; j < Maxstringstyle; ++j)
      if (strcmp(label_stylestr,textstring_styles[j]) == 0)
	set_label_item_style(plate,i+1,(stringstyletype) j);
    fscanf(plate_rscfile,"\n");
  }
}

  void
read_plate_textinputs(objptr plate,int num_textinputs,FILE *plate_rscfile)
{
  int i,j,textinput_pen_width;
  vertype textinput_offset,textinput_size,pos;
  Coord textinput_xscale,textinput_yscale;
  char textinputstr[50],textinput_fieldname[50];
  char textinput_stylestr[50],textcplcodestr[50];
  colortype textinput_color;
  objptr text_input_pad;
  worldptr world;
  ;
  world = plate->motherworld;
  
  for (i = 0; i < num_textinputs; ++i)
  {
    fscanf(plate_rscfile,"\tTextinput Field Name:%s",textinput_fieldname);
    fscanf(plate_rscfile,"\tTextinput Size:%f,%f\n",&(textinput_size[vx]),
	   &(textinput_size[vy]));
    text_input_pad = create_square(world,textinput_size[vx],textinput_size[vy]);
    if (strcmp(textinput_fieldname,"<none>") != 0)
      set_object_name(text_input_pad,textinput_fieldname);
    add_property((featureptr) text_input_pad,nonmanifold_prop);
    fscanf(plate_rscfile,"\tTextinput Default:");
    getc(plate_rscfile);	/* skip past open quote */
    j = 0;
    while ((textinputstr[j++] = getc(plate_rscfile)) != '"') ;
    textinputstr[j-1] = '\0';	/* erase tailing doublequote, or set nullstr */
    fscanf(plate_rscfile,"\tTextinput Text Offset:[%f,%f,%f]\n",
	   &pos[vx],&pos[vy],&pos[vz]);
    add_label_item(text_input_pad,textinputstr,pos,Hershey);

    /* up and off the text_input_pad a bit... this is kinda hardwired */
    setpos3d(pos,10.0,10.0,10.0);
    set_label_item_offset(text_input_pad,1,pos);

    fscanf(plate_rscfile,"\tTextinput Privatecode File:%s\n",textcplcodestr);
    if (strcmp("<none>",textcplcodestr) != 0)
      read_CPL_privatecode(textcplcodestr,(featureptr) text_input_pad);

    fscanf(plate_rscfile,"\tTextinput Type:%s\n",textinputstr);
    set_property((featureptr) text_input_pad,textinput_prop,
		 lookup_CPL_const(textinputstr));
    
    text_input_pad->selectechnique = set_normalpad_selectable;
    /* uses regular draw technique for the time being */

    fscanf(plate_rscfile,"\tTextinput Offset:[%f,%f,%f]\n",
	   &(textinput_offset[vx]),&(textinput_offset[vy]),
	   &(textinput_offset[vz]));
    textinput_offset[vw] = 1.0;
    translate_feature((featureptr) text_input_pad,textinput_offset,FALSE);

    fscanf(plate_rscfile,"\tTextinput Color:%hd,%hd,%hd\n",
	   &textinput_color[0],&textinput_color[1],&textinput_color[2]);
    set_feature_color((featureptr) text_input_pad,textinput_color);
    fscanf(plate_rscfile,"\tTextinput Text Color:%hd,%hd,%hd\n",
	   &textinput_color[0],&textinput_color[1],&textinput_color[2]);
    set_label_item_color(text_input_pad,1,textinput_color);
    fscanf(plate_rscfile,"\tTextinput Pen Width:%d\n", &textinput_pen_width);
    set_label_item_pen_width(text_input_pad,1,textinput_pen_width);
    fscanf(plate_rscfile,"\tTextinput Scale:%f,%f\n",
	   &textinput_xscale,&textinput_yscale);
    set_label_item_scale(text_input_pad,1,textinput_xscale,textinput_yscale);
    fscanf(plate_rscfile,"\tTextinput Style:%s\n",textinput_stylestr);
    for (j = 0; j < Maxstringstyle; ++j)
      if (strcmp(textinput_stylestr,textstring_styles[j]) == 0)
	set_label_item_style(text_input_pad,1,(stringstyletype) j);
    fscanf(plate_rscfile,"\n");

    adopt_obj_branch(plate,text_input_pad,FALSE);
  }
}


  objptr
read_plate(worldptr world,objptr parent,char *plate_rscfilename)
{
  int num_labels,num_textinputs;
  objptr plate;
  vertype plate_offset,pos;
  Coord plate_width,plate_height;
  colortype plate_color,label_color;
  char plate_filename[50],platecplcodestr[50];
  char plate_name[50];
  leptr thisle;
  FILE *plate_rscfile;
  ;
  plate_rscfile = fopen(plate_rscfilename,"r");
  fscanf(plate_rscfile,"%s Plate Resource File\n\n",plate_filename);
  fscanf(plate_rscfile,"Plate Name:%s\n",plate_name);
  fscanf(plate_rscfile,"Plate Width,Height:%f,%f\n",
	 &plate_width,&plate_height);
  fscanf(plate_rscfile,"Plate Offset:[%f,%f,%f]\n",
	 &plate_offset[vx],&plate_offset[vy],&plate_offset[vz]);
  plate_offset[vw] = 1.0;
  fscanf(plate_rscfile,"Plate Color:%hd,%hd,%hd\n",
	 &plate_color[0],&plate_color[1],&plate_color[2]);
  fscanf(plate_rscfile,"Plate Privatecode File:%s\n",
	 platecplcodestr);

  plate = make_obj(world,origin);
  set_object_name(plate,plate_name);
  set_feature_color((featureptr) plate,plate_color);
  add_property((featureptr) plate,nonmanifold_prop);
  add_property((featureptr) plate,nonselectable_prop);
  add_property((featureptr) plate,inputplate_prop);

  thisle = First_obj_le(plate);
  setpos3d(pos,plate_width,plate_height,0.0);
  thisle = make_edge_vert(thisle,thisle,pos);

  plate->selectechnique = set_normalpad_selectable;
  plate->drawtechnique = draw_plate_technique;

  fscanf(plate_rscfile,"Number of Plate Labels:%d\n",&num_labels);
  fscanf(plate_rscfile,"Number of Plate Text Inputs:%d\n\n",&num_textinputs);

  /* Get plate labels from resource file */
  read_plate_labels(plate,num_labels,plate_rscfile);
  /* Get plate text input pads from resource file */
  read_plate_textinputs(plate,num_textinputs,plate_rscfile);

  translate_feature((featureptr) plate,plate_offset,FALSE);
  adopt_obj_branch(parent,plate,FALSE);	/* FALSE GODDAMMIT!!! */

  fclose(plate_rscfile);
  
  del_property_from_descendants((featureptr) plate,visible_prop);
  if (strcmp("<none>",platecplcodestr) != 0)
    read_CPL_privatecode(platecplcodestr,(featureptr) plate);
  return(plate);
}
