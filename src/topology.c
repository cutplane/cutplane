
/* FILE: topology.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Topology Specific Routines for the Cutplane Editor           */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define TOPOLOGYMODULE

#include <config.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>		/* for strlen, strcpy only */
#include <string.h>		/* for strlen, strcpy only */

#include <platform_gl.h>
#ifdef IRIS
#include <strings.h>		/* For bcopy, but note this won't work under */
				/* MFC as far as I can tell. */
#endif

#include <cutplane.h> /* cutplane.h includes topology.h */

#include <globals.h>
#include <math3d.h>
#include <pencilsketch.h>
#include <boolean.h>
#include <update.h>

  void
init_shell_defaults(shellptr newshell)
{
  newshell->featureflags = Noflags;         /* Not selectable or picked yet */
}

  void
init_shell_lists(shellptr newshell)
{
  newshell->vfelist = create_list();        /* list of verts in the shell */
  newshell->evflist = create_list();        /* list of edges in the shell */
  newshell->fvelist = create_list();        /* list of faces in the shell */
  newshell->surflist = create_list();       /* list of surfaces in the shell */
  newshell->ringlist = create_list();       /* list of section rings */
  newshell->inplanefvelist = create_list(); /* list of faces in the cutplane */
  newshell->inplanevflist = create_list();  /* list of edges in the cutplane */
  newshell->inplanevfelist = create_list();/* list of vertices in the cutplane */
}

  void
init_obj_lists(objptr newobj)
{
  newobj->shellist = create_list();
  newobj->children = create_list();
  newobj->motherworld = working_world;
  newobj->selectechnique = set_object_selectable; /* the usual way */
  newobj->drawtechnique = normal_draw_technique; /* the usual way */
  newobj->screenbboxtechnique = Nil; /* if Nil, update.c uses screenbbox field */
}

  void
init_world_lists(worldptr neworld)
{
  neworld->world_name = Nil;
  neworld->root = (objptr) create_blank_elem(Obj_type);
  neworld->root->motherworld = neworld;
  neworld->root->parent = Nil;
  neworld->root->linked = TRUE;	/* always linked so get_unlinked_ancestor */
				/* does the right thing. */
  init_proplists(neworld->world_proplists); /* kinda wasteful hack. */
}

  void
init_ring_lists(ringptr newring)
{
  newring->seglist = create_list();
}

  void
init_fve_lists(fveptr newfve)
{
  newfve->boundlist = create_list();
}

  void
init_bound_lists(boundptr newbound)
{
  newbound->lelist = create_circular_list();
}

  void
init_curve_lists(curveptr newcurve)
{
  newcurve->evflist = create_list();
}

  void
init_surf_lists(surfptr newsurf)
{
  newsurf->fvelist = create_list();
  newsurf->curvelist = create_list();
}

  void
copy_shell_elem(shellptr sourceshell, shellptr destshell)
{
  listptr vfelist,evflist,fvelist,ringlist,surflist;
  listptr inplanefvelist,inplanevflist,inplanevfelist;
  ;
  vfelist = destshell->vfelist;
  evflist = destshell->evflist;
  fvelist = destshell->fvelist;
  surflist = destshell->surflist;
  ringlist = destshell->ringlist;
  inplanefvelist = destshell->inplanefvelist;
  inplanevflist = destshell->inplanevflist;
  inplanevfelist = destshell->inplanevfelist;
  clear_list(vfelist);
  clear_list(evflist);
  clear_list(fvelist);
  clear_list(surflist);
  clear_list(ringlist);
  clear_list(inplanefvelist);
  clear_list(inplanevflist);
  clear_list(inplanevfelist);

  memmove((void *) destshell,(const void *) sourceshell,Shellsize);
  copy_list(sourceshell->vfelist,vfelist);
  copy_list(sourceshell->evflist,evflist);
  copy_list(sourceshell->fvelist,fvelist);
  copy_list(sourceshell->surflist,surflist);
  copy_list(sourceshell->ringlist,ringlist);
  copy_list(sourceshell->inplanefvelist,inplanefvelist);
  copy_list(sourceshell->inplanevflist,inplanevflist);
  copy_list(sourceshell->inplanevfelist,inplanevfelist);

  destshell->vfelist = vfelist;
  destshell->evflist = evflist;
  destshell->fvelist = fvelist;
  destshell->surflist = surflist;
  destshell->ringlist = ringlist;
  destshell->inplanefvelist = inplanefvelist;
  destshell->inplanevflist = inplanevflist;
  destshell->inplanevfelist = inplanevfelist;
}

  void
copy_obj_elem(objptr sourceobj, objptr destobj)
{
  listptr children,shellist;
  ;
  shellist = destobj->shellist;
  children = destobj->children;
  clear_list(shellist);
  clear_list(children);
  
  memmove((void *) destobj,(const void *) sourceobj,Objsize);
  copy_list(sourceobj->shellist,shellist);
  copy_list(sourceobj->children,children);
  destobj->shellist = shellist;
  destobj->children = children;
}

  void
copy_world_elem(worldptr sourceworld, worldptr destworld)
{
  listptr world_shellist;
  char *world_name;
  ;
  world_name = destworld->world_name;
  
  memmove((void *) destworld,(const void *) sourceworld,Worldsize);
  if ((sourceworld->world_name != Nil) && (world_name != Nil))
    strcpy(sourceworld->world_name,world_name);

  destworld->world_name = world_name;
}

  void
copy_ring_elem(ringptr sourcering, ringptr destring)
{
  listptr seglist;
  ;
  seglist = destring->seglist;
  clear_list(seglist);
  
  memmove((void *) destring, (const void *) sourcering,Ringsize);
  copy_list(sourcering->seglist,seglist);

  destring->seglist = seglist;
}

  void
copy_fve_elem(fveptr sourcefve, fveptr destfve)
{
  listptr boundlist;
  ;
  boundlist = destfve->boundlist;
  clear_list(boundlist);
  
  memmove((void *) destfve,(const void *) sourcefve,Fvesize);
  copy_list(sourcefve->boundlist,boundlist);

  destfve->boundlist = boundlist;
}

  void
copy_bound_elem(boundptr sourcebound, boundptr destbound)
{
  listptr lelist;
  ;
  lelist = destbound->lelist;
  clear_list(lelist);
  
  memmove((void *) destbound,(const void *) sourcebound,Boundsize);
  copy_list(sourcebound->lelist,lelist);

  destbound->lelist = lelist;
}

  void
copy_curve_elem(curveptr sourcecurve, curveptr destcurve)
{
  listptr evflist;
  ;
  evflist = destcurve->evflist;
  clear_list(evflist);

  memmove((void *) destcurve,(const void *) sourcecurve,Curvesize);
  copy_list(sourcecurve->evflist,evflist);

  destcurve->evflist = evflist;
}

  void
copy_surf_elem(surfptr sourcesurf, surfptr destsurf)
{
  listptr fvelist,curvelist;
  ;
  fvelist = destsurf->fvelist;
  curvelist = destsurf->curvelist;
  clear_list(fvelist);
  clear_list(curvelist);

  memmove((void *) destsurf,(const void *) sourcesurf,Surfsize);
  copy_list(sourcesurf->fvelist,fvelist);
  copy_list(sourcesurf->curvelist,curvelist);
  
  destsurf->fvelist = fvelist;
  destsurf->curvelist = curvelist;
}

  void
delete_fve_lists(fveptr thisfve)
{
  del_list(thisfve->boundlist);
  thisfve->boundlist = Nil;
}

  void
delete_bound_lists(boundptr thisbound)
{
  del_list(thisbound->lelist);
  thisbound->lelist = Nil;
}

  void
delete_shell_lists(shellptr delshell)
{
  del_list(delshell->vfelist);	/* list of verts in the shell */
  del_list(delshell->evflist);	/* list of edges in the shell */
  del_list(delshell->fvelist);	/* list of faces in the shell */
  del_list(delshell->ringlist);	/* list of section rings */
  del_list(delshell->inplanefvelist);	/* list of faces in the cutplane */
  del_list(delshell->inplanevflist);	/* list of edges in the cutplane */
  del_list(delshell->inplanevfelist);	/* list of vertices in the cutplane */

  delshell->vfelist = Nil;
  delshell->evflist = Nil;
  delshell->fvelist = Nil;
  delshell->ringlist = Nil;
  delshell->inplanefvelist = Nil;
  delshell->inplanevflist = Nil;
  delshell->inplanevfelist = Nil;
}

  void
delete_obj_lists(objptr thisobj)
{
  del_list(thisobj->shellist);
  del_list(thisobj->children);
  thisobj->shellist = Nil;
  thisobj->children = Nil;
}

  void
delete_world_lists(worldptr thisworld)
{
}

  void
delete_ring_lists(ringptr thisring)
{
  del_list(thisring->seglist);
  thisring->seglist = Nil;
}

  void
delete_curve_lists(curveptr thiscurve)
{
  del_list(thiscurve->evflist);
  thiscurve->evflist = Nil;
}

  void
delete_surf_lists(surfptr thisurf)
{
  del_list(thisurf->fvelist);
  del_list(thisurf->curvelist);
  thisurf->fvelist = Nil;
  thisurf->curvelist = Nil;
}

  void
delete_vv_lists(vvptr thisvv)
{
  if (thisvv->sectorlistA != Nil)
  {
    del_list(thisvv->sectorlistA);
    thisvv->sectorlistA = Nil;
  }
  if (thisvv->sectorlistB != Nil)
  {
    del_list(thisvv->sectorlistB);
    thisvv->sectorlistB = Nil;
  }
  if (thisvv->sectorpairlist != Nil)
  {
    del_list(thisvv->sectorpairlist);
    thisvv->sectorpairlist = Nil;
  }
}

  void
delete_vf_lists(vfptr thisvf)
{
  if (thisvf->sectorlist != Nil)
  {
    del_list(thisvf->sectorlist);
    thisvf->sectorlist = Nil;
  }
}

/* world manipulation stuff...shouldnt really be in here but only two routines */
/* right now... */

  void
merge_worlds(worldptr sourceworld, worldptr destworld)
{
  objptr sourceroot;
  int i;
  ;
  /* make the stuff from the source world have same zoom factor as destworld */
  totally_unzoom_world(sourceworld);
  totally_zoom_world(sourceworld,destworld->viewer.xform,
		     destworld->viewer.invxform);
  sourceroot = sourceworld->root;
  /* transfer global prop lists from source world to destworld */
  for (i=0; i < next_available_property; i++)
    if (sourceworld->world_proplists[i]->numelems != 0)
      transfer_list_contents(sourceworld->world_proplists[i],
			     destworld->world_proplists[i]);
  /* reset the motherworld pointers of the sourceworld so that they */
  /* refer to the destworld...this is now done in adopt! */
  adopt_obj_branch(destworld->root,sourceroot,TRUE);
  remove_obj(sourceroot);
  del_elem(world_list, (elemptr) sourceworld);
}

  void
clear_world(worldptr thisworld)
{
  kill_elem((elemptr) thisworld->root);
  thisworld->root = (objptr) create_blank_elem(Obj_type);
  thisworld->root->motherworld = thisworld;
  thisworld->root->parent = Nil;
}

/* hey euler stuff herehahah */


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*      equivalences between mantylas names and ours                      */
/*                                                                        */
/*  DATA STRUCTURES:                                                      */
/*                                                                        */
/*  Node -> liststruct (sort of, he doesn't point to the first element of */
/*                      the list, he actually stores the first element of */
/*                      the list in the list head. Also, he doesn't keep  */
/*                      track of the last list element or the number of   */
/*                      elements in the list)                             */
/*  Solid -> shellstruct                                                    */
/*  Face -> fvestruct (fve stands for face-vertex-edge)                   */
/*  Edge -> evfstruct (evf stands for edge-vertex-face)                   */
/*  Vertex -> vfestruct (vfe stands for vertex-face-edge)                 */
/*  Halfedge -> lestruct (le stands for loop-edge)                        */
/*  Loop -> boundstruct                                                   */
/*                                                                        */
/*  (NOTE: The names; fve, evf,and vfe no longer accurately indicate the  */
/*  contents of the structures, they are holdovers from previous days)    */
/*                                                                        */
/*  FUNCTIONS:                                                            */
/*                                                                        */
/*  mate() -> Twin_le()                                                   */
/*  mvfs() -> make_shell()                                                  */
/*  lkvfs() -> kill_shell()                                                 */
/*  lmev() -> make_edge_vert()                                            */
/*  lkev() -> kill_edge_vert()                                            */
/*  lmef() -> make_edge_face()                                            */
/*  lkef() -> kill_edge_face()                                            */
/*  lmekr() -> kill_ring_make_edge()                                      */
/*  lkemr() -> kill_edge_make_ring()                                      */
/*  lringmv() -> move_loop()                                              */
/*  lkfmrh() -> kill_face_make_loop()                                     */
/*  lmfkrh() -> make_face_kill_loop()                                     */
/*                                                                        */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


#ifndef FAST

  leptr
Twin_le(leptr thisle)
{
  if (thisle->facedge == Nil)
    return(thisle);
  if (thisle->facedge->le1 == thisle)
    return(thisle->facedge->le2);
  return(thisle->facedge->le1);
}

#endif /* FAST */

leptr
new_le(leptr next_le, evfptr thisevf, vfeptr thisvfe, int dir)
{
  leptr newle;
  ;
  if (next_le == Nil)
    system_error("new_le: next_le == Nil");

  if (next_le->facedge == Nil) /* a single vertex with no associated */
  {				/* edge (the no loop case). we won't add */
    newle = next_le;	/* any le's, just associate an edge with it */
  }				/* yielding the single vertex loop case. */
  else
  {
    newle = (leptr) create_blank_elem(Le_type);

    newle->prev = next_le->prev;
    newle->next = next_le;
    next_le->prev = newle;
    newle->prev->next = newle;

    newle->motherbound = next_le->motherbound;

    newle->motherbound->lelist->numelems += 1;
    newle->motherbound->lelist->first.le = newle;
    /* not needed but do it for old times sake */
    newle->motherbound->lelist->last.le = newle->prev;
  }

  newle->facedge = thisevf;
  newle->facevert = thisvfe;

  if (dir == Plus)
    thisevf->le1 = newle;
  else
    thisevf->le2 = newle;

  return(newle);
}

leptr
del_le(leptr thisle)
{
  leptr thisleprev;
  listptr lelist;
  ;
  lelist = thisle->motherbound->lelist;
  if (thisle->next == thisle) // Single vertex boundary
  {
    if (thisle->facedge == Nil)	// Single vertex no loop: 1 face, 1 vertex, 0 edges
    {
      lelist->first.le = Nil;
      lelist->last.le = Nil;
      lelist->numelems = 0;
      thisleprev = Nil;
      clear_feature_properties((featureptr) thisle);
      free_elem((elemptr *) &thisle);
      thisle = Nil;
    }
    else // Single vertex loop: 2 faces, 1 vertex, 1 edge
    {
      if (thisle->facedge->le1 == thisle)
	thisle->facedge->le1 = Nil;
      else
	thisle->facedge->le2 = Nil;
      thisle->facedge = Nil;
      thisleprev = thisle;
    }
  }
  else				// Multiple vertex loop
  {
    thisle->prev->next = thisle->next;
    thisle->next->prev = thisle->prev;
    thisleprev = thisle->prev;
        
    if (lelist->first.le == thisle)
      lelist->first.le = thisle->next;
    lelist->last.le = lelist->first.le->prev;
    lelist->numelems -= 1;
    clear_feature_properties((featureptr) thisle);
    free_elem((elemptr *) &thisle);
  }
    
  return(thisleprev);
}

  shellptr		/* mvfs, create shell with 1 vert & 1 face at pos */
make_shell(vertype pos)
{
  shellptr newshell;
  fveptr newfve;
  boundptr newbound;
  vfeptr newvfe;
  leptr newle;
  ;
  newshell = (shellptr) create_blank_elem(Shell_type);
  newshell->motherobj = Nil;
    
  newfve = (fveptr) add_new_blank_elem(newshell->fvelist,Nil,Fve_type);

  newfve->mothershell = newshell;
  setpos3d(newfve->facenorm,0.0,0.0,0.0);
  newfve->facenorm[vw] = 0.0;
  setpos3d(newfve->local_facenorm,0.0,0.0,0.0);
  newfve->local_facenorm[vw] = 0.0;
  newbound = (boundptr) add_new_blank_elem(newfve->boundlist,Nil,Bound_type);

  newvfe = new_vfe(pos,newshell);

  /* the following is equivalent to a specialized new_le(): */
  newle = (leptr) create_blank_elem(Le_type);
  newbound->lelist->first.le = newle;
  newbound->lelist->last.le = newle; /* not really needed...but why not? */
  newbound->lelist->numelems = 1;
  newle->prev = newle;
  newle->next = newle;

  newvfe->startle = newle;
  newle->facedge = Nil;
  newle->facevert = newvfe;
  
  newle->motherbound = newbound;
  newbound->motherfve = newfve;
  newfve->mothershell = newshell;

  return(newshell);
}

/* Move a shell from one object to another. */

  void
move_shell_to_obj(objptr newmotherobj, shellptr moved_shell)
{
  objptr oldmotherobj;
  ;
  /* all shells created by make_obj now have motherobjs, only shells */
  /* created with low level list routines have no motherobj */

  oldmotherobj = moved_shell->motherobj;
  if (oldmotherobj != Nil)
  {
    /* make sure the vertices have been transformed */
    transform_shell(moved_shell,oldmotherobj->xform);
    rem_elem(oldmotherobj->shellist, (elemptr) moved_shell);
    /* now use the newowners invxform to reset the local_pos */
    transform_shell_reverse(moved_shell,newmotherobj->invxform);
  }
  else
  {
    /* here we assume that the shell was just made with make_shell */
    /* so that locaL_pos == pos and we assume the pos is correct */
    /* so we use the newobjs invxform to set the local_pos */
    transform_shell_reverse(moved_shell,newmotherobj->invxform);
  }
  append_elem(newmotherobj->shellist, (elemptr) moved_shell);

  /* reset the shell's motherobj ptr. */
  moved_shell->motherobj = newmotherobj;
  set_3d_objbbox(newmotherobj);
}

  objptr
make_childobj_noshell(objptr parent)
{
  listptr objlist;
  objptr newobj;
  worldptr previous_world;
  ;
  previous_world = working_world;
  working_world = parent->motherworld;

  objlist = parent->children;
  newobj = (objptr) append_new_blank_elem(objlist,Obj_type);
  copy_matrix(working_world->viewer.xform,newobj->xform);
  copy_matrix(working_world->viewer.invxform,newobj->invxform);
  newobj->parent = parent;

  /* All objects are assumed to be displayed, and in Grey, to start with. */
  add_property((featureptr) newobj,visible_prop);
  set_feature_named_color((featureptr) newobj,"White");
  log_update((featureptr) newobj,Allupdateflags,Immediateupdate);

  if (parent->motherworld->root == parent)
    newobj->linked = TRUE;
  else
    newobj->linked = FALSE;

  working_world = previous_world;

  return(newobj);
}

  objptr
make_parentobj_noshell(objptr child)
{				/* Creates object as parent of object passed. */
  objptr old_parent,new_parent;
  worldptr thisworld;
  ;
  if (child->parent == Nil)	/* this is a root */
  {
    thisworld = child->motherworld;
    new_parent = thisworld->root = (objptr) create_blank_elem(Obj_type);
    thisworld->root->motherworld = thisworld;
    thisworld->root->parent = Nil;
    append_elem(thisworld->root->children, (elemptr) child);
  }
  else
  {
    old_parent = child->parent;
    new_parent = make_childobj_noshell(old_parent);
    adopt_obj_branch(new_parent,child,FALSE);
  }

  return(new_parent);
}

  objptr
make_obj_noshell(worldptr world)
{
  objptr newobj;
  shellptr newshell;
  ;
  if (world == Nil)
    world = scratch_world;

  /* Create a new object as a child of root */
  newobj = make_childobj_noshell(world->root);
  newobj->linked = TRUE;

  return(newobj);
}

  objptr
make_childobj(objptr parent, vertype pos)
{
  listptr objlist;
  objptr newobj;
  worldptr previous_world;
  ;
  previous_world = working_world;
  working_world = parent->motherworld;

  objlist = parent->children;
  newobj = (objptr) append_new_blank_elem(objlist,Obj_type);
  copy_matrix(working_world->viewer.xform,newobj->xform);
  copy_matrix(working_world->viewer.invxform,newobj->invxform);
  newobj->parent = parent;
  if (parent->motherworld->root == parent)
    newobj->linked = TRUE;	/* parent is root, do not group with it */
  else
    newobj->linked = FALSE;	/* otherwise assume free linkage */

  move_shell_to_obj(newobj,make_shell(pos));

  /* All objects are assumed to be displayed, and in Grey, to start with. */
  add_property((featureptr) newobj,visible_prop);
  set_feature_named_color((featureptr) newobj,"White");
  log_update((featureptr) newobj,Allupdateflags,Immediateupdate);

  working_world = previous_world;

  return(newobj);
}

  objptr
make_parentobj(objptr child, vertype pos)
{				/* Creates object as parent of object passed. */
  objptr old_parent,new_parent;
  ;
  if (child->parent == Nil)	/* this is a root */
    system_error("make_parentobj: root cannot have a shell");
  else
  {
    old_parent = child->parent;
    new_parent = make_childobj(old_parent,pos);
    adopt_obj_branch(new_parent,child,FALSE);
  }

  return(new_parent);
}

/* Create a new object as a child of the root object. */

  objptr
make_obj(worldptr world, vertype pos)
{
  objptr newobj;
  shellptr newshell;
  ;
  if (world == Nil)
    world = scratch_world;

  /* Create a new object as a child of root */
  newobj = make_childobj(world->root,pos);
  newobj->linked = TRUE;	/* we dont want new object grouped with root */

  return(newobj);
}  

  char *
get_object_name(objptr named_obj)
{
  char *obj_name;
  ;
  if (named_obj->obj_name != Nil)
  {
    obj_name = (char *) alloc_elem(strlen(named_obj->obj_name)+1);
    strcpy(obj_name,named_obj->obj_name);
  }
  else
    obj_name = Nil;
  return(obj_name);
}

  void
set_object_name(objptr named_obj,char *obj_name)
{
  /* Watch out for Nil passed as obj_name: means clear the obj_name record */
  /* entirely. */
  if (named_obj->obj_name != Nil)
  {
    if (obj_name == Nil)
      free_elem((elemptr *) &(named_obj->obj_name));
    else
    {
      named_obj->obj_name = (char *) realloc(named_obj->obj_name,
					     strlen(obj_name)+1);
      strcpy(named_obj->obj_name,obj_name);
    }
  }
  else
  {
    if (obj_name != Nil)
    {
      named_obj->obj_name = (char *) alloc_elem(strlen(obj_name)+1);
      strcpy(named_obj->obj_name,obj_name);
    }
  }
}

  void
set_world_name(worldptr named_world,char *world_name)
{
  if (named_world->world_name != Nil)
    named_world->world_name = (char *) realloc(named_world->world_name,
					       strlen(world_name)+1);
  else
    named_world->world_name = (char *) alloc_elem(strlen(world_name)+1);
  strcpy(named_world->world_name,world_name);
}

/* Remove and delete (FREE) a shell. */

  void
kill_shell(shellptr thishell)
{
  del_elem(thishell->motherobj->shellist, (elemptr) thishell);
}

/* Remove and delete (FREE) an object. */

  void
kill_obj(objptr deleted_obj)
{	/* completely wipes out a solid with any number of faces, edges, etc */
	/* and kills any updates that might be lying around for that shell*/
  remove_obj(deleted_obj);
  kill_elem((elemptr) deleted_obj);
}

  void
kill_obj_branch(objptr deleted_obj)
{	/* completely wipes out a solid with any number of faces, edges, etc */
	/* and kills any updates that might be lying around for that shell*/
  /* as well as wiping out all its children */
  del_elem(deleted_obj->parent->children,(elemptr) deleted_obj);

}

  leptr
make_edge_vert(leptr le1, leptr le2, vertype pos)
  /* NOTE: le1 & le2 must point to the same vert!! */
  /* le1 gets the new vertex, and the routine */
  /* returns an le going from the new vertex */
  /* to the old */
{
  shellptr thishell;
  evfptr newevf;
  vfeptr newvfe;
  leptr temple,newle1,newle2;
  ;
  if (le1->facevert != le2->facevert)
    system_error("MAKE_EDGE_VERT: le1&le2 are different vertices\n");

  thishell = le1->motherbound->motherfve->mothershell;
  newevf = (evfptr) append_new_blank_elem(thishell->evflist,Evf_type);

  newvfe = new_vfe(pos,thishell);

  /* move verts of loop edges to new position starting with le 1 and */
  /* ending with the one before le2 */
  temple = le1;
  while (temple != le2)
  {
    temple->facevert = newvfe;
    temple = Twin_le(temple)->next;
  }
  /* add new loop edges for the edge between the old vert pointed to by le2 */
  /* and the new vert pointed to by le1 */
  /* these must be in this order for the Nil facedge case to work */
  /* mental had these in the wrong order */
/* used to be this way: */
/*
  newle1 = new_le(le1,newevf,le2->facevert,Minus);
  newle2 = new_le(le2,newevf,newvfe,Plus);
*/
/* now, have le1 going away from pt1vfe, and le2 going towards it. */
  newle1 = new_le(le1,newevf,le2->facevert,Plus);
  newle2 = new_le(le2,newevf,newvfe,Minus);
  /* make the newvfe point back to the new loop edge, newle1 points to newevf */
  /* and the old vfe, newle2 points to newevf and the new vfe */
  newvfe->startle = newle2;
  /* make the old vert point back to le2, to be sure it points back to an */
  /* edge that didn't have its vertex moved */
  le2->facevert->startle = le2;

  return(newle2);
}

  void
kill_edge_vert(leptr le1, leptr le2) /* le1 & le2 point to same evf,
					le2->facevert stays */
{
  leptr temple,le2prev;
  vfeptr doomedvfe,savedvfe;
  evfptr doomedevf;
  ;
  if (le1->facedge != le2->facedge)
    system_error("KILL_EDGE_VERT: le1&le2 are different edges.");
  else if (le1->facedge == Nil)
    system_error("KILL_EDGE_VERT: the facedges of le1&le2 are Nil.");

  doomedevf = le1->facedge;
  doomedvfe = le1->facevert;
  savedvfe = le2->facevert;

  temple = le1;
  do
  {
    temple->facevert = savedvfe;
    temple = Twin_le(temple)->next;
  } while (temple != le1);

  /* must do the following in this order so that le2 remains */
  /* if we have an isolated edge !!*/
  del_le(le1);
  le2prev = del_le(le2);
  /* have to point savedvfe to an existing le (if there is one) */
  /* used to be: savedvfe->startle = le2prev; i think we need le2prev->next */
  savedvfe->startle = le2prev->next;

  del_elem(le2prev->motherbound->motherfve->mothershell->evflist,
	   (elemptr) doomedevf);
  del_elem(le2prev->motherbound->motherfve->mothershell->vfelist,
	   (elemptr) doomedvfe);
}

  fveptr
make_edge_face(leptr le1, leptr le2)
  /* splits a face making a new edge and a */
  /* new face, le1 will be part of the new face */
{
  fveptr thisfve;
  shellptr thishell;
  fveptr newfve;
  boundptr newbound;
  evfptr newevf;
  leptr temple,newle1,newle2;
  int numles = 0;
  ;
  /* error condition, le's must be in same face */
  if (le1->motherbound->motherfve != le2->motherbound->motherfve)
    system_error("make_edge_face: le1 and le2 are not in the same face!");

  thisfve = le1->motherbound->motherfve;
  thishell = thisfve->mothershell;

  newfve = (fveptr) append_new_blank_elem(thishell->fvelist,Fve_type);

  newfve->mothershell = thishell;
  copyvec3d(thisfve->local_facenorm,newfve->local_facenorm);
  copyvec3d(thisfve->facenorm,newfve->facenorm);
  newfve->planedist = thisfve->planedist;

  newbound = (boundptr) append_new_blank_elem(newfve->boundlist,Bound_type);

  newbound->motherfve = newfve;
  newevf = (evfptr) append_new_blank_elem(thishell->evflist,Evf_type);

  /* point loopedges from le1 to the one before le2 back to the new face */
  temple = le1;
  while (temple != le2)
  {
    temple->motherbound = newbound;
    temple = temple->next;
    numles++;
  }

  /* create two half edges and point them to the newevf splitting thisfve */
  /* this inserts newle1 in front of le1 and newle2 in front of le2 */
/* old way: (see make_edge_vert for details): */
  newle1 = new_le(le1,newevf,le2->facevert,Minus);
  newle2 = new_le(le2,newevf,le1->facevert,Plus);
/* WAS changed to be this way: */
/*
  newle1 = new_le(le1,newevf,le2->facevert,Plus);
  newle2 = new_le(le2,newevf,le1->facevert,Minus);
*/
  /* this splits the old loop of edges into two loops, newle1 & 2 are now */
  /* along the cutline newle1 will be in the new boundary, newle2 will be */
  /* in the old boundary */
  newle1->prev->next = newle2;
  newle2->prev->next = newle1;
  temple = newle1->prev;
  newle1->prev = newle2->prev;
  newle2->prev = temple;

  /* mantlee had a bug I fix it here (for the case where le1 = le2!!!) */
  newle1->motherbound = newbound;
  /* make the new boundary's lelist point at newle1 */
  newbound->lelist->first.le = newle1;
  newbound->lelist->last.le = newle1->prev;
  newbound->lelist->numelems += numles;

  /* makes the first edge in the old boundary edge list newle2 this is    */
  /* done because we need to make certain that the old boundary's lelist  */
  /* points to an edge in the old loop edge list */
  le2->motherbound->lelist->first.le = newle2;
  le2->motherbound->lelist->last.le = newle2->prev;
  le2->motherbound->lelist->numelems -= numles;

  return(newfve);
}

  void
kill_edge_face(leptr le1, leptr le2)
  /* joins two faces and deletes common edge */
  /* le1 and le2 must be in different faces */
  /* and point to the same edge, le2's face */
  /* is removed. le1 and le2 may point to */
  /* the same facevert */
{
  leptr temple,le2prev,le1prev;
  boundptr sourcebound,destbound;
  evfptr doomedevf;
  fveptr doomedfve;
  int numles = 0;
  ;
  if ((le1->facedge != le2->facedge) || (le1->facedge == Nil))
    system_error("KILL_EDGE_FACE: le1&le2 are different edges");
  else if (le1->motherbound->motherfve == le2->motherbound->motherfve)
    system_error("KILL_EDGE_FACE: le1&le2 are in same face");

  destbound = le1->motherbound;
  sourcebound = le2->motherbound;
  
  doomedevf = le1->facedge;
  doomedfve = sourcebound->motherfve;
  
  temple = le2;
  do
  {
    temple->motherbound = le1->motherbound;
    temple = temple->next;
    numles++;
  } while (temple != le2);
  
  /* join the two loops together first */
  le1->next->prev = le2;
  le2->next->prev = le1;
  temple = le2->next;
  le2->next = le1->next;
  le1->next = temple;
  destbound->lelist->numelems += numles;
  destbound->lelist->last.le = destbound->lelist->first.le->prev;
  
  /* get rid of the extra le's */
  /* note must do this in this order if we are getting rid of le2's face */
  /* otherwise the le2 = single vert case wont work */
  le1prev = del_le(le1);
  le2prev = del_le(le2);
  /* make sure vfe's don't point to le's that were deleted */
  (le1prev->next)->facevert->startle = le1prev->next;
  (le2prev->next)->facevert->startle = le2prev->next;
  
  /* delete extra edge */
  del_elem(destbound->motherfve->mothershell->evflist, (elemptr) doomedevf);
  
  /* delete extra bound */
  /* make sure we have a nil list to avoid unwanted deletion of lelist */
  /* by del_elem */
  sourcebound->lelist->first.le = sourcebound->lelist->last.le = Nil;
  sourcebound->lelist->numelems = 0;
  del_elem(sourcebound->motherfve->boundlist, (elemptr) sourcebound);
  
  /* delete extra face */
  /* must handle holes otherwise the following will kill them */
  del_elem(doomedfve->mothershell->fvelist, (elemptr) doomedfve);
}

  leptr
kill_ring_make_edge(leptr le1, leptr le2)
  /* merges two loops from the same face into one */
  /* this is done by connecting both loops with a single */
  /* edge, which then is traversed twice in the loop */
{
  shellptr thishell;
  evfptr newevf;
  leptr temple,newle1,newle2;
  boundptr sourcebound,destbound;
  int numles = 0;
  ;
  if (le1->motherbound->motherfve != le2->motherbound->motherfve)
    system_error("KILL_RING_MAKE_EDGE: le1&le2 are in different faces.");
  else if (le1->motherbound == le2->motherbound)
    system_error("KILL_RING_MAKE_EDGE: le1&le2 are in the same boundary");

  destbound = le1->motherbound;
  sourcebound = le2->motherbound;
  
  thishell = le1->motherbound->motherfve->mothershell;
  newevf = (evfptr) append_new_blank_elem(thishell->evflist,Evf_type);

  temple = le2;
  do
  {
    temple->motherbound = destbound;
    temple = temple->next;
    numles++;
  } while (temple != le2);

  /* add the new loop edges in front of le1 and le2 */
/* used to be this way: */
/*
  newle1 = new_le(le1,newevf,le1->facevert,Minus);
  newle2 = new_le(le2,newevf,le2->facevert,Plus);
*/
  /* now it's for sketching, this way: */
  newle1 = new_le(le1,newevf,le1->facevert,Plus);
  newle2 = new_le(le2,newevf,le2->facevert,Minus);
  
  if (le1->facevert->startle == Nil)
    le1->facevert->startle = le1;
  if (le2->facevert->startle == Nil)
    le2->facevert->startle = le2;

  /* split and join the two loops */
  newle2->next = le1;
  le1->prev = newle2;
  newle1->next = le2;
  le2->prev = newle1;

  destbound->lelist->last.le = destbound->lelist->first.le->prev;
  destbound->lelist->numelems += numles;

  /* delete sourcebound */
  sourcebound->lelist->numelems = 0;
  sourcebound->lelist->first.le = Nil;
  sourcebound->lelist->last.le = Nil;
  del_elem(sourcebound->motherfve->boundlist, (elemptr) sourcebound);

  return(newle1);
}

  boundptr
kill_edge_make_ring(leptr le1, leptr le2)
  /* splits a loop into 2 loops in the same face */
  /* the loop must have an edge that appears twice when */
  /* traversing the loop, this is the edge that is removed */
{  
  boundptr newbound, oldbound;
  leptr temple,le1next,le1prev,le2prev;
  evfptr doomedevf;
  int numles = 0;
  ;
  if (le1->facedge != le2->facedge)
    system_error("KILL_EDGE_MAKE_RING: le1&le2 are different edges");
  else if (le1->motherbound != le2->motherbound)
    system_error("KILL_EDGE_RING_MAKE: le1&le2 are in different boundaries");

  /* note, the vertex of le1 stays in the old loop, so if you want that */
  /* vertex to remain in the outer loop you should pass it as le1 */
  oldbound = le1->motherbound;
  /*  make new boundary for this face */
  newbound = (boundptr) append_new_blank_elem(oldbound->motherfve->boundlist,
				    Bound_type);
  newbound->motherfve = oldbound->motherfve;
  /* evf to be removed */
  doomedevf = le1->facedge;

  /* split the loop into two loops */
  /* le1 goes in the outerloop and le2 goes in the innerloop */
  /* le1 and le2 are now extra (2 verts in the same place) so they are */
  /* deleted below */
  le1next = le1->next;
  le1->next = le2->next;
  le2->next->prev = le1;
  le2->next = le1next;
  le1next->prev = le2;

  temple = le2;
  do
  {
    temple->motherbound = newbound;
    temple = temple->next;
    numles++;
  } while (temple != le2);

  // Initialize the new boundary lelist so that del_le() works
  newbound->lelist->first.le = le2;
  newbound->lelist->last.le = le2->prev;
  newbound->lelist->numelems = numles;
  // Adjust the old boundary as well...
  oldbound->lelist->first.le = le1; // Just to be sure...
  oldbound->lelist->last.le = le1->prev;  
  oldbound->lelist->numelems -= numles;

  /* delete the extra loop edges le1 and le2 */
  le1prev = del_le(le1);
  le2prev = del_le(le2);

  /* point the old boundary to the loop edge that was before le1 */
  oldbound->lelist->first.le = le1prev;
  oldbound->lelist->last.le = le1prev->prev;
  /* make sure le1's vfe points back to a half edge in the correct loop */
  (le1prev->next)->facevert->startle = le1prev->next;

  /* same comments as above */
  newbound->lelist->first.le = le2prev;
  newbound->lelist->last.le = le2prev->prev;
  (le2prev->next)->facevert->startle = le2prev->next;

  /* delete the extra evf */
  del_elem(oldbound->motherfve->mothershell->evflist, (elemptr) doomedevf);

  return(newbound);
}

  void
kill_face_make_loop(fveptr sourcefve, fveptr destfve)
  /* remove face and make it a loop in another face */
  /* SOURCEFVE MUST HAVE JUST ONE LOOP. faces can be on disconnected parts */
  /* of the same shell but they must be part of the same shell */
{
  boundptr thisbound;
  ;
  if (sourcefve->boundlist->numelems > 1)
    system_error("kill_face_make_loop: sourcefve has a hole");

  thisbound = sourcefve->boundlist->first.bnd;
  /* add the boundary of the source face to the dest face */
  rem_elem(sourcefve->boundlist,(elemptr) thisbound);
  add_elem(destfve->boundlist,(elemptr) destfve->boundlist->last.bnd,
	  (elemptr) thisbound);
  /* point the added boundary back to the destface */
  thisbound->motherfve = destfve;

  /* possible bug here!!! if all lists havent already been cleaned up */
  del_elem(sourcefve->mothershell->fvelist, (elemptr) sourcefve);
}

  fveptr
make_face_kill_loop(boundptr bound)
{
  fveptr newfve;
  shellptr thishell;
  leptr firstle,thisle;
  ;
  /* remove the loop from its current face */
  thishell = bound->motherfve->mothershell;
  rem_elem(bound->motherfve->boundlist,(elemptr) bound);
  /* make a newface */
  newfve = (fveptr) append_new_blank_elem(thishell->fvelist,Fve_type);
  /* make the new face point back at the shell */
  newfve->mothershell = thishell;
  /* add boundary to new face */
  add_elem(newfve->boundlist,(elemptr) newfve->boundlist->last.bnd,
	  (elemptr) bound);
  /* make boundary point back at new face */
  bound->motherfve = newfve;

  return(newfve);
}

  void
move_loop(boundptr thisbound, fveptr destfve, int outflag)
{
  /* could unite shells if they are different */
  if (thisbound->motherfve->mothershell != destfve->mothershell)
    system_error("move_loop: thisbound and destfve are in different shells");

  rem_elem(thisbound->motherfve->boundlist,(elemptr) thisbound);
  if (outflag == Outer)
    add_elem(destfve->boundlist,Nil,(elemptr) thisbound);
  else
    add_elem(destfve->boundlist,(elemptr) destfve->boundlist->last.bnd,
	    (elemptr) thisbound);
  thisbound->motherfve = destfve;
}

/******************************************************************************/
/*            Generic routine to split an edge at a coordinate.               */
/******************************************************************************/

  leptr
break_edge_at_pos(leptr thisle,vertype pos)
{
  leptr newle;
  vertype vec1,vec2;
  ;
  diffpos3d(thisle->facevert->pos,thisle->next->facevert->pos,vec1);
  diffpos3d(thisle->facevert->pos,pos,vec2);
  normalize(vec1,vec1);
  normalize(vec2,vec2);
  if (compare(dotvecs(vec1,vec2),1.0,0.00001) != 0)
  {
    printf("*** Error in break_edge_at_pos: breakpt not in edge segment!!\n");
    printf("Edge le is:\n");
    printle_coords(thisle);
    printf("Breakpos is: ");
    printvert(pos);
    system_error("break_edge_at_pos: PANIC! Run from the room!");
  }
  newle = make_edge_vert(thisle,Twin_le(thisle)->next,pos);

  return(newle);
}

/* Geometric test to see if, after a mef, the holes from the old face */
/* need to go in the new face because they are not GEOMETRICALLY in the old */
/* face. */

  void
move_holes(fveptr oldface, fveptr newface)
{
  boundptr thisbound,nextbound,outerbound;
  leptr thisle;
  ;
  outerbound = oldface->boundlist->first.bnd;
  thisbound = outerbound->next;
  
  while (thisbound != Nil)
  {
    nextbound = thisbound->next;
    thisle = thisbound->lelist->first.le;
    if (pt_in_bound_3d(thisle->facevert->pos,outerbound,Ptonplanetol) !=
	(int) Ptinsidebound)
      move_loop(thisbound,newface,FALSE);
    thisbound = nextbound;
  }
}

/* new mantlya-derived code that also punches holes */

  void
extrude_face(fveptr thisfve, vertype depth)
{
  boundptr thisbound;
  fveptr backface;
  leptr firstle,thisle;
  vertype newpos;
  ;
  thisbound = thisfve->boundlist->first.bnd;
  while (thisbound)
  {
    firstle = thisbound->lelist->first.le;
    thisle = firstle->next;
    copypos3d(thisle->facevert->pos,newpos);
    addpos3d(newpos,depth,newpos);
    make_edge_vert(thisle,thisle,newpos);
    while (thisle != firstle)
    {
      copypos3d(thisle->next->facevert->pos,newpos);
      addpos3d(newpos,depth,newpos);
      make_edge_vert(thisle->next,thisle->next,newpos);
      make_edge_face(thisle->prev,thisle->next->next);
      thisle = Twin_le(thisle->next)->next;
    }
    backface = make_edge_face(thisle->prev,thisle->next->next);
/*    if (outerbackface == Nil)
      outerbackface = Twin_le(thisle->prev->prev)->motherbound->motherfve; */
    thisbound = thisbound->next;
  }
}

  void
extrude_faces(listptr drawshellist, vertype depth) /* meet george hacker */
{
  shellptr thishell;
  ;
  for (thishell = drawshellist->first.shell; thishell != Nil;
       thishell = thishell->next)
    extrude_face(thishell->fvelist->first.fve,depth);
}

/* create an solid of rotation, by extruding the given wire (should not */
/* cross over the axis of rotation or the shell will be illegal) about */
/* the given axis of rotation in degree_inc increments, a full 360 degrees.*/
/* The wire solid is assumed to have been created from one end to the other */
/* with makeshell() calls and then successive mev's. */

  void
make_solid_of_rotation(objptr wireobj,vertype rotvec,
		       vertype rotpt,int degree_inc)
{
  leptr firstle,lastle,endle,connectle,strutle,otherfirstle;
  vertype newpos;
  Matrix axisrotmat,dummymat;
  ;
  copy_matrix(imatrix,axisrotmat);
  firstle = First_obj_le(wireobj);

  /* find one end of the wireshell */
  while (firstle->next->facedge != firstle->facedge)
    firstle = firstle->next;
  endle = firstle;
  /* find other end of the wireshell */
  otherfirstle = lastle = firstle->next;
  while (lastle->next->facedge != lastle->facedge)
    lastle = lastle->next;
  connectle = lastle;
  make_axisrot_matrix(rotvec,rotpt,(3600 / degree_inc),axisrotmat,dummymat);
  while (--degree_inc > 0)
  {
    transformpt(endle->next->facevert->pos,axisrotmat,newpos);
    make_edge_vert(endle->next,endle->next,newpos);
    strutle = endle->next;
    /* make one strip along the edge of the (widening) wire */
    while (strutle != lastle->next)
    {
      transformpt(strutle->prev->facevert->pos,axisrotmat,newpos);
      make_edge_vert(strutle->next,strutle->next,newpos);
      make_edge_face(strutle->prev,strutle->next->next);
      strutle = Twin_le(strutle->next->next);
    }
    lastle = strutle;
    endle = Twin_le(endle->next->next);
  }
  /* now mef up the seam */
  connectle = otherfirstle;
  do
  {
    make_edge_face(endle->next,connectle);
    endle = endle->prev;
    connectle = connectle->next;
  } while (endle != lastle->prev); 

}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*      GLUE TWO SHELLS TOGETHER ALONG GEOMETRICALLY IDENTICAL FACES     */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* Assumption: the loops go in opposite directions. */
/* If killfinaloop is TRUE, then the face edges that are left over on the */
/* inside after the kill_edge_face in the bottom while loop (see diagram */
/* in Mantlya p 211) are also deleted.  This is used when gluing two */
/* faces together in booleans that were once part of a hole (eg after */
/* subtraction) and you don't want an ugly separation line left around after- */
/* wards. */

  void
combine_loops(fveptr faceA, fveptr faceB, Boolean killbehind)
{
  leptr firstleA,startleA,thisleB,startleB,nextle,killbehindle;
  ;
  startleA = firstleA = faceA->boundlist->first.bnd->lelist->first.le;
  startleB = faceB->boundlist->first.bnd->lelist->first.le;
  while (!pt_near_pt_3d(startleA->facevert->pos,
			startleB->facevert->pos,1.0e-2/*<-hack Pttopttol*/))
  {
    startleA = startleA->next;
    if (startleA == firstleA)
    {
#ifdef IRIS
      plotfaceles(faceA,"faceA.ps");
      plotfaceles(faceB,"faceB.ps");
#endif
      system_error("combine_loops: glue loops are not identical!!!");
    }
  }

#if 0
  printf("Combining:\nFace A:\n");
  printfaceles(startleA->motherbound->motherfve);
  printf("\n\nwith Face B:\n");
  printfaceles(startleB->motherbound->motherfve);    
#endif

  /* Assumption: this is one shell (that may have been two shells before, */
  /* but now has been merged into one). */
  /* now make face A a hole in face B */
  kill_face_make_loop(faceA,faceB);

  kill_ring_make_edge(startleB,startleA);
  kill_edge_vert(startleB->prev,startleA->prev);
  thisleB = startleB;
  while (thisleB->next != startleA)
  {
    nextle = thisleB->next;
    make_edge_face(thisleB->next,thisleB->prev);
    kill_edge_vert(thisleB->next,Twin_le(thisleB->next));
    if (killbehind)
      killbehindle = thisleB->next;
    kill_edge_face(Twin_le(thisleB),thisleB);
    if (killbehind)
    {
      if (killbehindle->next == Twin_le(killbehindle)) /* this is a strut */
	kill_edge_vert(killbehindle->next,killbehindle);
      else
	kill_edge_face(Twin_le(killbehindle),killbehindle);
    }
    thisleB = nextle;
  }
  if (killbehind)
    killbehindle = thisleB->next;
  kill_edge_face(Twin_le(thisleB),thisleB);
  if (killbehind)
    kill_edge_vert(Twin_le(killbehindle),killbehindle);
}

/* merge the two shells into one shell (put the source shell in the destshell */
/* by list transferral routines found in slice.c */
/* should this be in topology.c or euler.c? */

  void
merge_shells(shellptr sourceshell,shellptr destshell)
{
  fveptr thisfve;
  ;
/* hmmmm what object records?
  system_error("merge_shells: need to kill some object records here!\n");
*/
  permanent_transform_obj(sourceshell->motherobj);
  transform_shell_reverse(sourceshell,destshell->motherobj->invxform);

  thisfve = sourceshell->fvelist->first.fve;
  while (thisfve)
  {
    thisfve->mothershell = destshell;
    thisfve = thisfve->next;
  }

  transfer_list_contents(sourceshell->fvelist,destshell->fvelist);
  transfer_list_contents(sourceshell->evflist,destshell->evflist);
  transfer_list_contents(sourceshell->vfelist,destshell->vfelist);
  transfer_list_contents(sourceshell->ringlist,destshell->ringlist);
  transfer_feature_properties((featureptr) sourceshell,(featureptr) destshell);
  /*** NOTE: some IMPORTANT stuff in destshell is lost here, e.g. bbox,*/
  /* hotspotle.  How to deal with this? */
  set_3d_objbbox(destshell->motherobj);
  kill_shell(sourceshell);
}

#if 0
  void
combine_shells(listptr shell_list,shellptr sourceshell,shellptr destshell)
{
  permanent_transform_obj(sourceshell->motherobj);
  transform_shell_reverse(sourceshell,destshell->invxform);
  transfer_faces(sourceshell->fvelist,sourceshell->fvelist->first.fve,destshell);
  transfer_edges(sourceshell->evflist,destshell);
  transfer_vertices(sourceshell->vfelist,destshell);
  transfer_list_contents(sourceshell->ringlist,destshell->ringlist);
  transfer_feature_properties((featureptr) sourceshell,(featureptr) destshell);
  /* should this free the space occupied by destshell or delete it from the  */
  /* global shellist or what? just, what? */
  /*** NOTE: some IMPORTANT stuff in destshell is lost here, e.g. bbox,*/
  /* hotspotle.  How to deal with this? */
  add_property((featureptr) sourceshell,killed_prop);
}
#endif

/* Topology inversion routines. */

/* When an shell gets inverted, the lelist's first and last ptrs are not */
/* necessarily pointing at the correct le's any more. */
/* This routine resets the lelist->last ptr with reference to the */
/* lelist->first ptr. */

  void
fix_flipped_bounds(shellptr thishell)
{
  fveptr thisfve;
  boundptr thisbound;
  leptr thisle;
  ;
  for (thisfve = thishell->fvelist->first.fve; thisfve != Nil;
       thisfve = thisfve->next)
    for (thisbound = thisfve->boundlist->first.bnd; thisbound != Nil;
	 thisbound = thisbound->next)
      if (thisbound->lelist->numelems > 1)
	thisbound->lelist->last.le = thisbound->lelist->first.le->prev;
}

  void
flip_evf(evfptr thisevf)
{

  leptr thisle,otherle,thisprevle,otherprevle;
  vfeptr thisfacevert,otherfacevert;
  ;
  thisle = thisevf->le1;
  otherle = thisevf->le2;
  thisfacevert = thisle->facevert;
  otherfacevert = otherle->facevert;
  thisprevle = thisle->prev;
  otherprevle = otherle->prev;
  
  /* first change their startles */
  if (thisfacevert->startle == thisle)
    thisfacevert->startle = otherle;
  if (otherfacevert->startle == otherle)
    otherfacevert->startle = thisle;
  
  /* now change their directions */
  thisle->prev = thisle->next;
  thisle->next = thisprevle;
  otherle->prev = otherle->next;
  otherle->next = otherprevle;
  
  /* now change their faceverts */
  thisle->facevert = otherfacevert;
  otherle->facevert = thisfacevert;
}

  void
clear_flip_flags(listptr evflist)
{
  evfptr thisevf;
  ;
  thisevf = evflist->first.evf;
  while (thisevf)
  {
    thisevf->featureflags &= ~Flipflag;
    thisevf->le1->motherbound->featureflags &= ~Flipflag;
    thisevf = thisevf->next;
  }
}

void
tag_evfs_to_flip(boundptr thisbound, listptr evflist)
{
  leptr firstle,thisle;
  ;
  if ((thisbound == thisbound->motherfve->boundlist->first.bnd) &&
      !(thisbound->featureflags & Flipflag))
  {
    /* first tag all evfs in this bound */
    firstle = thisle = thisbound->lelist->first.le;
    do
    {
      thisle->facedge->featureflags |= Flipflag;
      thisle = thisle->next;
    } while (thisle != firstle);
    thisbound->featureflags |= Flipflag;
    /* then look at adjacent bounds */
    firstle = thisle = thisbound->lelist->first.le;
    do
    {
      tag_evfs_to_flip(Twin_le(thisle)->motherbound,evflist);
      thisle = thisle->next;
    } while (thisle != firstle);
  }
}

  void
flip_hole_orientation(boundptr thisbound)
{
  listptr evflist;
  evfptr thisevf;
  shellptr thishell;
  ;
  if (thisbound == thisbound->motherfve->boundlist->first.bnd)
    system_error("flip_hole_orientation: thisbound is not a hole.");
  thishell = thisbound->motherfve->mothershell;
  evflist = thishell->evflist;
  clear_flip_flags(evflist);
  tag_evfs_to_flip(Twin_le(thisbound->lelist->first.le)->motherbound,evflist);

  thisevf = evflist->first.evf;
  while (thisevf)
  {
    if (thisevf->featureflags & Flipflag)
      flip_evf(thisevf);
    thisevf = thisevf->next;
  }
  fix_flipped_bounds(thishell);
}

  void
flip_shell_orientation(shellptr thishell)
{
  evfptr thisevf;
  ;
  for (thisevf = thishell->evflist->first.evf; thisevf != Nil;
       thisevf = thisevf->next)
    flip_evf(thisevf);
  /* Now the bounds' lelists first and last ptrs must be rearranged to be */
  /* correct. */
  fix_flipped_bounds(thishell);
  thishell->cclockwise = !thishell->cclockwise;
}

/* This version works great!! */
  void
flip_obj_orientation(objptr thisobj)
{
  evfptr thisevf;
  shellptr thishell;
  ;
  thishell = First_obj_shell(thisobj);
  while (thishell != Nil)
  {
    flip_shell_orientation(thishell);
    thishell = thishell->next;
  }
}

/* glue two objs together by merging the two shells, then glueing face */
/* 1 into shell 2. */

  void
glue_objects_along_faces(fveptr face1,fveptr face2,Boolean killbehind)
{
  shellptr shell1,shell2;
  leptr startle1,startle2,firstle1;
  alltypes check;
  ;
  shell1 = face1->mothershell;
  shell2 = face2->mothershell;
  startle1 = firstle1 = face1->boundlist->first.bnd->lelist->first.le;
  startle2 = face2->boundlist->first.bnd->lelist->first.le;
  while (!pt_near_pt_3d(startle1->facevert->pos,
			startle2->facevert->pos,Pttopttol))
  {
    startle1 = startle1->next;
    if (startle1 == firstle1)
    {
      printf("glue_objects_along_faces: faces are not coincident!!!");
      return;
    }
/* system_error("glue_objects_along_faces: faces are not coincident!!!");*/
  }
  if (face1->boundlist->first.bnd->lelist->numelems !=
      face2->boundlist->first.bnd->lelist->numelems)
    system_error("glue_objects_along_faces: faces have different # of le's!!!");
  /* flip all le's in the other solid to make */
  /* sure that loops are going in opposite direction */
  if (shell1 != shell2)
  {
    if (!pt_near_pt_3d(startle1->next->facevert->pos,
		     startle2->prev->facevert->pos,Pttopttol))
      flip_obj_orientation(shell1->motherobj);

    merge_shells(shell1,shell2);
  }

  combine_loops(face1,face2,killbehind);
}

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                                                                           */
/*                       Feature Deleltion Routines                          */
/*                                                                           */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

fveptr
delete_vertex(leptr thisle)
{
  leptr firstle,nextle,stople;
  fveptr newfve,thisfve;
  shellptr thishell;
  objptr motherobj;
  ;
  if (thisle == thisle->next)	/* we've only got a single vert with no edges */
  {
    thishell = thisle->motherbound->motherfve->mothershell;
    motherobj = thishell->motherobj;
    if (motherobj->shellist->first.shell == thishell)
      kill_obj(motherobj);
    else
      kill_shell(thishell);
    return(Nil);
  }

  firstle = thisle;
  do
  {
    nextle = Twin_le(thisle)->next;

    /* if this face has more than three verts or thisle is a strut,
       make edge */
    if (thisle->motherbound->lelist->numelems > 3)
      make_edge_face(thisle->prev,thisle->next);
    thisle = nextle;
  } while (thisle != firstle);
  thisle = firstle;
  stople = Twin_le(firstle->prev);

  while (thisle != stople)
  {
    nextle = Twin_le(thisle)->next;
    if (thisle->motherbound != Twin_le(thisle)->motherbound)
      kill_edge_face(Twin_le(thisle),thisle);
    else
      kill_edge_make_ring(thisle,Twin_le(thisle));
    thisle = nextle;
  }

  newfve = stople->motherbound->motherfve;
  
  kill_edge_vert(stople,Twin_le(stople));
  
  return (newfve);
}

fveptr
delete_edge(leptr thisle)
{
  leptr nextle;
  fveptr fve1,fve2;
  ;
  nextle = thisle->next;
  fve1 = thisle->motherbound->motherfve;
  fve2 = Twin_le(thisle)->motherbound->motherfve;
  if (compare(dotvecs(fve1->facenorm,fve2->facenorm),1.0,1.0e-4) == 0)
  {
    kill_edge_face(thisle,Twin_le(thisle));
    return (thisle->motherbound->motherfve);
  }
  else
  {
    delete_vertex(thisle);
    return (delete_vertex(nextle));
  }
}

fveptr
delete_face(fveptr thisfve)
{
  leptr thisle,nextle;
  vfeptr stopvfe;
  boundptr thisbound,nextbound;
  fveptr newfve;
  ;
  thisbound = thisfve->boundlist->first.bnd;
  while (thisbound)
  {
    nextbound = thisbound->next;
    thisle = thisbound->lelist->first.le;
    stopvfe = thisle->prev->facevert;
    do
    {
      nextle = thisle->next;
      newfve = delete_vertex(thisle);
      thisle = nextle;
    } while (thisle->facevert != stopvfe);
    newfve = delete_vertex(stopvfe->startle);
    thisbound = nextbound;
  }
  return (newfve);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                                                                            */
/*                            Utility Routines                                */
/*                                                                            */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* This routine returns the containing feature of the feature you specify */
/* at the requested grain size.  Eg, if you pass an LE and FVE_TYPE, you get */
/* the containing fve for that le.  In some cases, (pass an evf and FVE_TYPE) */
/* you get the first of the two adjoining faces only. This may be useless.*/

  featureptr
find_largergrain(featureptr smallfeature,int larger_type)
{
  leptr themotherle;
  boundptr themotherbound;
  evfptr themotherevf;
  fveptr themotherfve;
  shellptr themothershell;
  objptr themotherobj;
  ;
  switch (smallfeature->type_id)
  {
  case Vfe_type:
    themotherle = ((vfeptr) smallfeature)->startle;
    themotherbound =  themotherle->motherbound;
    themotherevf = themotherle->facedge;
    themotherfve = themotherbound->motherfve;
    themothershell = themotherfve->mothershell;
    themotherobj = themothershell->motherobj;
    break;
  case Le_type:
    themotherle = (leptr) smallfeature;
    themotherbound = themotherle->motherbound;
    themotherevf = themotherle->facedge;
    themotherfve = themotherbound->motherfve;
    themothershell = themotherfve->mothershell;
    themotherobj = themothershell->motherobj;
    break;
  case Evf_type:
    themotherevf = ((evfptr) smallfeature);
    themotherbound = themotherevf->le1->motherbound;
    themotherfve = themotherbound->motherfve;
    themothershell = themotherfve->mothershell;
    themotherobj = themothershell->motherobj;
    break;
  case Bound_type:
    themotherbound = (boundptr) smallfeature;
    themotherfve = themotherbound->motherfve;
    themothershell = themotherfve->mothershell;
    themotherobj = themothershell->motherobj;
    break;
  case Fve_type:
    themotherfve = ((fveptr) smallfeature);
    themothershell = themotherfve->mothershell;
    themotherobj = themothershell->motherobj;
    break;
  case Shell_type:
    themothershell = ((shellptr) smallfeature);
    themotherobj = themothershell->motherobj;
    break;
  case Obj_type:		
    themotherobj = ((objptr) smallfeature);
    break;
  }
  switch (larger_type)
  {
  case Vfe_type:			/* if this is what you're asking for, */
    return (smallfeature);	/* you've already got it (nothing smaller!) */
  case Le_type:
    return((featureptr) themotherle);
  case Bound_type:
    return((featureptr) themotherbound);
  case Evf_type:
    return((featureptr) themotherevf);
  case Fve_type:
    return((featureptr) themotherfve);
  case Shell_type:
    return((featureptr) themothershell);
  case Obj_type:
    return((featureptr) themotherobj);
  }

  // This should never happen -- LJE
  return 0;
}


  fveptr
find_face_given_normal(objptr thisobj, vertype searchnorm)
{
  shellptr thishell;
  fveptr thisfve,returnfve = Nil;
  vertype checknorm;
  ;
  transform_obj_tree(thisobj);
  find_obj_normals(thisobj);
  for (thishell = thisobj->shellist->first.shell; thishell != Nil;
       thishell = thishell->next)
  {
    for (thisfve = thishell->fvelist->first.fve; thisfve != Nil;
	 thisfve = thisfve->next)
    {
      diffvec3d(searchnorm,thisfve->facenorm,checknorm);
      if (vector_near_zero(checknorm,Pttopttol))
      {
	returnfve = thisfve;
	break;
      }
    }
  }
  return(returnfve);
}

  Boolean
find_les_same_face(leptr *le1, leptr *le2)
{
  fveptr le2fve;
  ;
  le2fve = (*le2)->motherbound->motherfve;
  if ((*le1)->motherbound->motherfve != le2fve)
  {
    if (Twin_le(*le1)->motherbound->motherfve == le2fve)
      *le1 = Twin_le(*le1);
    else
    {
      *le2 = Twin_le(*le2);
      le2fve = (*le2)->motherbound->motherfve;
      if ((*le1)->motherbound->motherfve != le2fve)
      {
	if (Twin_le(*le1)->motherbound->motherfve == le2fve)
	  *le1 = Twin_le(*le1);
	else
	  return (FALSE);
      }
    }
  }
  return (TRUE);
}

  Boolean
find_les_same_face_same_vfes(leptr *le1, leptr *le2)
{
  fveptr le2fve;
  ;
  le2fve = (*le2)->motherbound->motherfve;
  if ((*le1)->motherbound->motherfve != le2fve)
  {
    if (Twin_le(*le1)->motherbound->motherfve == le2fve)
      *le1 = Twin_le(*le1)->next;
    else
    {
      *le2 = Twin_le(*le2)->next;
      le2fve = (*le2)->motherbound->motherfve;
      if ((*le1)->motherbound->motherfve != le2fve)
      {
	if (Twin_le(*le1)->motherbound->motherfve == le2fve)
	  *le1 = Twin_le(*le1)->next;
	else
	  return (FALSE);
      }
    }
  }
  return (TRUE);
}

/* Find two le's belonging to the same face, given two le's, if possible. */
/* The only way to do this non-n^2 is to walk around the le's of the vfe at */
/* le1,  walking each face, and stopping in a face containing the vfe for le2. */
/* In case of failure, return false. */

  Boolean
find_les_same_face_walking(leptr *le1,leptr *le2)
{
  leptr searchle,walkle;
  fveptr searchfve;
  ;
  searchle = *le1;
  do
  {
    walkle = searchle->next;
    while (walkle != searchle)
    {
      if (walkle->facevert == (*le2)->facevert)
      {
	*le1 = searchle;
	*le2 = walkle;
	return(TRUE);
      }
      walkle = walkle->next;
    }
    searchle = Twin_le(searchle)->next;
  } while (searchle != *le1);
  return(FALSE);
}

  void
reset_cutplanetime_flags(objptr thisobj,void *unused_data)
{
  vfeptr thisvfe;
  fveptr thisfve;
  shellptr thishell;
  ;
  thisobj->timestamp = 0;
  thishell = First_obj_shell(thisobj);
  while (thishell != Nil)
  {
    thisvfe = thishell->vfelist->first.vfe;
    while (thisvfe != Nil)
    {
      thisvfe->timestamp = 0;
      thisvfe = thisvfe->next;
    }
    thisfve = thishell->fvelist->first.fve;
    while (thisfve != Nil)
    {
      thisfve->timestamp = 0;
      thisfve = thisfve->next;
    }
    thishell = thishell->next;
  }
}

  void
reset_cutplanetime(listptr world_list)
{
  worldptr thisworld;
  ;
  cutplane_time = 1;
  thisworld = world_list->first.world;
  while (thisworld != Nil)
  {
    apply_to_descendants(thisworld->root,reset_cutplanetime_flags,Nil);
    thisworld = thisworld->next;
  }
}
  
