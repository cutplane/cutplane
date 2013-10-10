
/* FILE: objtree.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*          Obect Tree Traversal Routines for the Cutplaen Editariat      */
/*                                                                        */
/* Author: WAK/LJE                                   Date: October 22,1990*/
/*                                                                        */
/* Copyright (C) 1988,1989,1990,1991 William Kessler and Laurence Edwards.*/
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define OBJTREEMODULE

#include <config.h>

#include <platform_gl.h>

#include <cutplane.h>
#include <update.h>

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                 Object tree manipulation Routines                      */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

  void
reset_motherworld_ptr(objptr reset_obj, worldptr new_world)
{
  reset_obj->motherworld = new_world;
}

  void
transfer_moved_obj_props(worldptr oldworld, worldptr neworld)
{	/* go through proplists in new world and look for objs in old world */
  listptr oldproplist,newproplist;
  objptr containingobj;
  proptr thisprop,nextprop;
  int i;
  ;
  for (i=0; i < next_available_property; i++)
  {
    oldproplist = oldworld->world_proplists[i];
    newproplist = neworld->world_proplists[i];
    thisprop = oldproplist->first.prop;
    while (thisprop != Nil)
    {
      nextprop = thisprop->next;
      containingobj = (objptr) find_largergrain(thisprop->owner,Obj_type);
      if (containingobj->motherworld == neworld)
      {			/* move properties instances to childs world */
	rem_elem(oldproplist,(elemptr) thisprop);
	append_elem(newproplist, (elemptr) thisprop);
      }
      thisprop = nextprop;	/* can't just do thisprop=thisprop->next */
				/* because thisprop is now at the end of*/
    }				/* the neworld proplist so the next==Nil!! */
  }
}

  void
insert_parent(objptr child, objptr newparent)
{				/* insert an object as a parent above child */
  objptr oldparent;
  worldptr oldworld;
  ;
  if (newparent->motherworld != child->motherworld)
  {
    oldworld = newparent->motherworld;
    if (newparent->children->numelems == 0)
      newparent->motherworld = child->motherworld;
    else
      apply_to_descendants(newparent,reset_motherworld_ptr,child->motherworld);
    transfer_moved_obj_props(oldworld,child->motherworld);
  }
  oldparent = child->parent;
  if (oldparent == Nil)		/* child is root */
  {
    child->motherworld->root = newparent;
    newparent->parent = Nil;
  }
  else
  {
    append_elem(oldparent->children, (elemptr) newparent);
    rem_elem(oldparent->children, (elemptr) child);
    newparent->parent = oldparent->parent;
  }
  append_elem(newparent->children, (elemptr) child);
  child->parent = newparent;
}

  void
add_child(objptr parent, objptr newchild)
{				/* add an object as a child below parent */
  worldptr oldworld;
  ;
  if (newchild->motherworld != parent->motherworld)
  {
    oldworld = newchild->motherworld;
    if (newchild->children->numelems == 0)
      newchild->motherworld = parent->motherworld;
    else
      apply_to_descendants(newchild,reset_motherworld_ptr,parent->motherworld);
    transfer_moved_obj_props(oldworld,parent->motherworld);
  }
  append_elem(parent->children,(elemptr) newchild);
  newchild->parent = parent;
}

  void
swap_child_with_parent(objptr child)
{
  objptr parent,thischild;
  listptr temp;
  ;
  parent = child->parent;
  temp = child->children;
  child->children = parent->children;
  parent->children = temp;

  child->parent = parent->parent;
  parent->parent = child;
  /* remove child from parent children and put in parent->parent children */
  /* also remove parent from parent->parent children and put in child children */
  rem_elem(parent->children, (elemptr) child);
  append_elem(child->parent->children, (elemptr) child);
  rem_elem(child->parent->children, (elemptr) parent);
  append_elem(child->children, (elemptr) parent);
  /* make sure childs children and parents children point to their new parents */
  thischild = child->children->first.obj;
  while (thischild)
  {
    thischild->parent = child;
    thischild = thischild->next;
  }
  thischild = parent->children->first.obj;
  while (thischild)
  {
    thischild->parent = parent;
    thischild = thischild->next;
  }
}

/* Removes a single obj from the tree, returning the object that was */
/* removed. Attaches the children to the parent of the removed object. */

  objptr
remove_obj(objptr removed_obj)
{
  objptr parent_of_removed,thisobj;
  ;
  if (removed_obj->parent == Nil)	/* this is a root */
    parent_of_removed = make_parentobj_noshell(removed_obj);
  else
    parent_of_removed = removed_obj->parent;
  rem_elem(parent_of_removed->children,(elemptr) removed_obj);
  thisobj = removed_obj->children->first.obj;
  while (thisobj)
  {
    thisobj->parent = parent_of_removed;
    thisobj = thisobj->next;
  }
  transfer_list_contents(removed_obj->children,parent_of_removed->children);
  removed_obj->linked = FALSE;

  return(removed_obj);
}

/* Removes an obj from the tree, returning the object that was removed. */
/* Does not attach the children to the removed object's parent; thus it */
/* removes an entire branch of the tree. */

  objptr
remove_obj_branch(objptr removed_obj)
{
  objptr parent_of_removed;
  ;
  if (removed_obj->parent == Nil)	/* this is a root */
    parent_of_removed = make_parentobj_noshell(removed_obj);
  else
    parent_of_removed = removed_obj->parent;

  rem_elem(parent_of_removed->children,(elemptr) removed_obj);
  removed_obj->linked = FALSE;

  return(removed_obj);
}

  void
adopt_obj_branch(objptr master,objptr slave,Boolean linked)
{
  remove_obj_branch(slave);
  add_child(master,slave);
  slave->linked = linked;
}

  void
adopt_obj(objptr master,objptr slave,Boolean linked)
{
  remove_obj(slave);
  add_child(master,slave);
  slave->linked = linked;
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			Object Utility Routines                   	*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


  void
get_obj_shells(objptr this_obj,listptr cull_list)
{
  shellptr thishell;
  ;
  thishell = First_obj_shell(this_obj);
  while (thishell != Nil)
  {
    add_to_featurelist(cull_list, (featureptr) thishell);
    thishell = thishell->next;
  }
}

/* Recursive routine that walks down the object tree from the given */
/* feature's object, getting subfeatures as it goes. The recursion */
/* is postorder (root last). */

  void
get_descendents(objptr this_obj,objptr mother_obj,
		listptr cull_list)
{
  objptr recurse_obj;
  shellptr thishell;
  ;
  /* First, recurse on the children of this object.  */
  recurse_obj = this_obj->children->first.obj;
  if (recurse_obj != Nil)	/* don't descend if at a leaf */
    do
    {
      get_descendents(recurse_obj,mother_obj,cull_list);
      recurse_obj = recurse_obj->next;
    } while (recurse_obj != Nil);

  if (this_obj == mother_obj)
    return;			/* don't store mother object too! */
  /* Now, add the shells for this object to the cull_list. */
  get_obj_shells(this_obj,cull_list);
}

  objptr
get_unlinked_ancestor(objptr thisobj)
{
  if (thisobj->linked)
    return(thisobj);
  return(get_unlinked_ancestor(thisobj->parent));
}

  void
apply_to_descendants(objptr thisobj,
		     void (*thefunction)(), void *function_data)
{
  objptr thischild;
  ;
  (*thefunction)(thisobj,function_data);
  thischild = thisobj->children->first.obj;
  while (thischild)	/* don't descend if at a leaf */
  {
    apply_to_descendants(thischild,thefunction,function_data);
    thischild = thischild->next;
  }
}

  void
apply_to_unlinked_descendants(objptr thisobj,
			      void (*thefunction)(), void *function_data)
{
  objptr thischild;
  ;
  (*thefunction)(thisobj,function_data);
  thischild = thisobj->children->first.obj;
  while (thischild)	/* don't descend if at a leaf or at a linked child */
  {
    if (thischild->linked == FALSE)
      apply_to_unlinked_descendants(thischild,thefunction,function_data);
    thischild = thischild->next;
  }
}

  void
apply_to_descendants_with_prop(objptr thisobj,property chosen_prop,
			       void (*thefunction)(), void *function_data)
{
  objptr thischild;
  ;
  (*thefunction)(thisobj,function_data);
  thischild = thisobj->children->first.obj;
  while (thischild)	/* don't descend if at a leaf or doesn't have this */
			/* property, e.g. visible. */
  {
    if (has_property((featureptr) thischild, chosen_prop))
      apply_to_descendants_with_prop(thischild,chosen_prop,thefunction,
				     function_data);
    thischild = thischild->next;
  }
}
