
/* FILE: list.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*          Generic List Handling Routines for the Cutplane Editor        */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 18,1988  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define LISTMODULE

#include <config.h>

#include <stdlib.h>

#include <platform_gl.h>

#include <cutplane.h>		/* cutplane.h includes list.h */
#include <properties.h>		/* for elem type_id's and sizes */
#include <tools.h>		/* for elem type_id's and sizes */
#include <pencilsketch.h>	/* for elem type_id's and sizes */
#include <boolean.h>		/* for elem type_id's and sizes */
#include <sun.h>

#include <controls.h>
#include <update.h>
#include <primitives.h>

				/* This is for borland's compiler and the */
				/* memmove call. */
#ifdef WIN32
#include <mem.h>
#else
#include <string.h>
#endif

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                       global variables                                 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* elem types defined in list.h properties.h tools.h topology.h */
/* pencilsketch.h, boolean.h and sun.h */

int typesize[Maxnumtypes] = {Elemsize,Epsize,Delemsize,Oelemsize,Qelemsize,
			       Sortsize,Sketchlesize,Propsize,Vfesize,
			       Evfsize,Lesize,Boundsize,Fvesize,Shellsize,
			       Objsize,Worldsize, Segsize,Ringsize,Curvesize,
			       Surfsize,
			       Sectorsize,Sectorpairsize,
			       Vvsize,Vfsize,Veesize,Efsize,
			       Facepairsize,Lepairsize};

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                       routines that find elements                      */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#ifndef FAST_N_SMART

  elemptr
next_elem(thishead,thisptr)
  listptr thishead;
  elemptr thisptr;
{
  if (thisptr->next)
    return (thisptr->next);
  return(thishead->first.any);
}

  elemptr
prev_elem(thishead,thisptr)
  listptr thishead;
  elemptr thisptr;
{
  if (thisptr->prev)
    return (thisptr->prev);
  return(thishead->last.any);
}

#endif /* FAST_N_SMART */

  int
get_elem_index(the_elem,the_list)
  elemptr the_elem;
  listptr the_list;
  /* returns the index of element pointed to by the_elem in the list */
  /* given by the_list. Returns 0 if the_elem not found. */
{
  elemptr this_item;
  int the_index = 0, this_index = 1;
  ;
  this_item = the_list->first.any;
  while ((this_item != Nil) && (the_index == 0))
  {
    if (this_item == the_elem)
      the_index = this_index;
    this_item = this_item->next;
    this_index++;
  }
  return(the_index);
}

  elemptr
get_elem_ptr(i,the_list)
  int i;
  listptr the_list;
  /* returns a pointer to item i of a list */
{
  elemptr the_elem;
  int j;
  ;
  the_elem = the_list->first.any;
  for (j=1; j < i; ++j)
  {
    the_elem = the_elem->next;
    if (!the_elem) return(Nil);
  }
  return(the_elem);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*            routines that add or remove elements from lists             */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void		/* insert an existing element AFTER a given element in a list.*/
add_elem(listptr the_list, elemptr prev_elem, elemptr this_elem)
{
  elemptr oldfirst,oldlast;
  ;
  /* we now handle circular lists correctly */
  the_list->numelems += 1;
  if (the_list->kind == Circular_list)
  {
    if (the_list->first.any == Nil) /* Nil list case */
    {
      the_list->last.any = the_list->first.any = this_elem;
      this_elem->next = this_elem;
      this_elem->prev = this_elem;
      return;
    }
    if (prev_elem == Nil)
      prev_elem = the_list->last.any;
    this_elem->next = prev_elem->next;
    this_elem->prev = prev_elem;
    prev_elem->next = this_elem;
    this_elem->next->prev = this_elem;
    /* never change the first elem just the last: */
    the_list->last.any = the_list->first.any->prev;
    if (this_elem->prev == Nil)
      system_error("add_elem: this_elem->prev == Nil");
    if (this_elem->next == Nil)
      system_error("add_elem: this_elem->next == Nil\n");
  }
  else
  {
    if (this_elem->type_id == Le_type)
      system_error("add_elem: lelist is non circular");
    if (the_list->first.any == Nil) /* Nil list case */
    {
      the_list->first.any = the_list->last.any = this_elem;
      this_elem->prev = Nil;
      this_elem->next = Nil;
      return;
    }
    else if (prev_elem == the_list->last.any) /* appending case */
    {
      oldlast = the_list->last.any;
      oldlast->next = this_elem;
      this_elem->prev = oldlast;
      this_elem->next = Nil;
      the_list->last.any = this_elem;
    }
    else if (prev_elem != Nil)		/* normal case */
    {
      this_elem->next = prev_elem->next;
      this_elem->prev = prev_elem;
      prev_elem->next = this_elem;
      this_elem->next->prev = this_elem;
    }
    else				/* insert at beginning of list */
    {
      oldfirst = the_list->first.any;
      this_elem->next = oldfirst;
      this_elem->prev = Nil;
      oldfirst->prev = this_elem;
      the_list->first.any = this_elem;
    }
  }
}

  void
insert_elem(thelist,nextelem,thelem) /* insert an existing element */
  listptr thelist;		/* BEFORE a given element in a list */
  elemptr nextelem,thelem;
{
  elemptr templast;
  ;
  (thelist->numelems)++;
  if (nextelem)
  {
    thelem->next = nextelem;
    thelem->prev = nextelem->prev;
    if (nextelem->prev)
      nextelem->prev->next = thelem;
    else
      thelist->first.any = thelem;
    nextelem->prev = thelem;
  }
  else
  {
    templast = thelist->last.any;
    thelist->last.any = thelem;
    thelist->last.any->next = Nil;
    thelist->last.any->prev = templast;
    if (templast == Nil)
      /* in case original list was empty point first ptr to new last ptr... */
      thelist->first.any = thelist->last.any; 
    else
      templast->next = thelist->last.any;
  }
}


  void
append_elem(the_list,thiselem)	/* appends an existing element of type  */
  listptr the_list;		/* type_id to the given linked list */
  elemptr thiselem;
{
  elemptr old_last_elem;
  ;
  old_last_elem = the_list->last.any;
  if (old_last_elem)
  {
    old_last_elem->next = thiselem;
    old_last_elem->next->prev = old_last_elem;
    old_last_elem->next->next = Nil;
    the_list->last.any = old_last_elem->next;
  }
  else
  {
    the_list->first.any = thiselem;
    the_list->first.any->prev = Nil;
    the_list->first.any->next = Nil;
    the_list->last.any = the_list->first.any;
  }
  the_list->numelems += 1;
}

  void			/* remove element from list, element still exists */
rem_elem(listptr the_list,elemptr the_elem)
{
  elemptr prev_elem,next_elem;
  ;
  if (the_list->first.any == Nil) /* empty list */
    system_error("rem_elem: list is empty already!");
  /* the 2nd and 3rd cases are done in this way to handle circ lists */
  (the_list->numelems)--;
  if ((the_elem == the_list->first.any) && (the_elem == the_list->last.any))
  {				/* delete only element */
    the_list->first.any = Nil;
    the_list->last.any = Nil;
    return;
  }
  if (the_elem == the_list->first.any)
  {				/* delete first element but not only one */
    the_list->first.any = the_elem->next;
    the_list->first.any->prev = the_elem->prev;
  }
  else if (the_elem == the_list->last.any)
  {				/* delete last but not only one */
    the_list->last.any = the_elem->prev;
    the_list->last.any->next = the_elem->next;
  }
  else
  {				/* normal case */
    prev_elem = the_elem->prev;
    next_elem = the_elem->next;
    prev_elem->next = next_elem;
    next_elem->prev = prev_elem;
  }
  the_elem->prev = the_elem->next = Nil; /* done for newpick.c */
}

  void
move_elem(the_list,the_elem,new_prev_elem) /* move an element to another */
  register listptr the_list;           /* position in the SAME list */
  register elemptr the_elem;
  elemptr new_prev_elem;
{
  if (new_prev_elem == the_elem)
    system_error("move_elem: preceding element = the element");

  if (the_list->first.any == Nil)        /* this list has no elements */
    system_error("move_elem: this list has no elements");

  if ((the_list->last.any->prev == Nil) && /* this list has only one element */
      (new_prev_elem != Nil))	/* and you're trying to move it to a position */
			       /* other than the first position */
    system_error("move_elem: this list has only 1 element");

  rem_elem(the_list,the_elem);
  add_elem(the_list,new_prev_elem,the_elem);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                routines that create new or delete elements             */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


int freecount = 0;

  elemptr  
alloc_elem(elemsize)	/* return pointer to new list element */
  int elemsize;		/* uses calloc so all bytes are zeroed. */
{
  register elemptr temp;
  ;
  temp = (elemptr) calloc(1,elemsize);
  if (temp == Nil)
    system_error("alloc_elem:out of memory");

  freecount++;

  return (temp);
}

  static void
init_elem(elemptr new_elem) /* note this assumes type_id has been set */
{
  /* NOTE: message stacks get listptrs the first time the feature receives */
  /* a message so that liststructs aren't lying around for all features. */
  switch (new_elem->type_id)
  {
  case Elem_type:
    new_elem->privatecode = Nil;
    new_elem->messagequeue = Nil;
    break;
  case Ep_type:
    new_elem->privatecode = Nil;
    new_elem->messagequeue = Nil;
    break;
  case Delem_type: break;
  case Oelem_type: break;
  case Qelem_type: break;
  case Sort_type:
    new_elem->privatecode = Nil;
    new_elem->messagequeue = Nil;
    break;
  case Sketchle_type: break;
  case Sector_type: break;
  case Sectorpair_type: break;
  case Vee_type: break;
  case Ef_type: break;
  case Vv_type: break;
  case Vf_type: break;
  case Prop_type: break;
  case Vfe_type:
    new_elem->privatecode = Nil;
    new_elem->messagequeue = Nil;
    break;
  case Evf_type:
    new_elem->privatecode = Nil;
    new_elem->messagequeue = Nil;
    break;
  case Le_type:
    new_elem->privatecode = Nil;
    new_elem->messagequeue = Nil;
    break;
  case Bound_type:
    init_bound_lists((boundptr) new_elem);
    new_elem->privatecode = Nil;
    new_elem->messagequeue = Nil;
    break;
  case Fve_type:
    init_fve_lists((fveptr) new_elem);
    new_elem->privatecode = Nil;
    new_elem->messagequeue = Nil;
    break;
  case Shell_type:
    init_shell_lists((shellptr) new_elem);
    init_shell_defaults((shellptr) new_elem);
    new_elem->privatecode = Nil;
    new_elem->messagequeue = Nil;
    break;
  case Obj_type:
    init_obj_lists((objptr) new_elem);
    copy_matrix(imatrix,((objptr) new_elem)->xform);
    copy_matrix(imatrix,((objptr) new_elem)->invxform);
    new_elem->privatecode = Nil;
    new_elem->messagequeue = Nil;
    break;
  case World_type:
    init_world_lists((worldptr) new_elem);
    copy_matrix(imatrix,((worldptr) new_elem)->viewer.xform);
    copy_matrix(imatrix,((worldptr) new_elem)->viewer.invxform);
    new_elem->privatecode = Nil;
    new_elem->messagequeue = Nil;
    break;
  case Seg_type: break;
  case Ring_type: 
    init_ring_lists((ringptr) new_elem);
    break;
  case Curve_type:
    init_curve_lists((curveptr) new_elem);
    break;
  case Surf_type:
    init_surf_lists((surfptr) new_elem);
    break;
  case Facepair_type:
    init_facepair_lists((facepairptr) new_elem);
    break;
  case Lepair_type:
    break;
  default:
    system_error("init_elem: unknown type id!");
    break;			/* stupid but ok */
  }
  new_elem->proplist = create_list();
  /* new elems use the global_proplists of the working world unless set */
  /* otherwise. */
  new_elem->global_proplists = working_world->world_proplists;
}

  elemptr
create_blank_elem(int type_id)
{
  elemptr new_elem;
  ;
  new_elem = alloc_elem(typesize[type_id]);
  new_elem->type_id = type_id;
  init_elem(new_elem);

  return (new_elem);
}

  elemptr			/* insert new blank element in a list */
add_new_blank_elem(the_list,prev_elem,type_id)
  listptr the_list;
  elemptr prev_elem;
  int type_id;
{
  elemptr new_elem,tempfirst;
  ;
  new_elem = create_blank_elem(type_id);
  add_elem(the_list,prev_elem,new_elem);

  return(new_elem);
}

  elemptr
insert_new_blank_elem(the_list,next_elem,type_id)
  listptr the_list;		/* insert a new blank element */
  elemptr next_elem;		/* BEFORE a given element in a list */
{
  elemptr new_elem;
  ;
  new_elem = create_blank_elem(type_id);
  insert_elem(the_list,next_elem,new_elem);

  return(new_elem);
}

  elemptr
append_new_blank_elem(the_list,type_id) /* appends a blank elem of type  */
  listptr the_list;		/* type_id to the given linked list */
  int type_id;
{
  elemptr new_elem;
  ;
  new_elem = create_blank_elem(type_id);
  append_elem(the_list,new_elem);

  return(new_elem);
}

  void
copy_elem(elemptr sourcelem, elemptr destelem)
{ /* copys fields and lists of one elem into another */
  char errormsg[128];
  listptr destproplist;
  ;
  destproplist = destelem->proplist;
  switch (sourcelem->type_id)
  {
  case Elem_type:
  case Ep_type:
  case Delem_type:
  case Oelem_type:
  case Qelem_type:
  case Sketchle_type:
  case Sector_type:
  case Sectorpair_type:
  case Vee_type:
  case Ef_type:
  case Vv_type:
  case Vf_type:
  case Prop_type:
  case Vfe_type:
  case Evf_type:
  case Le_type:
  case Lepair_type:
  case Seg_type:
    memmove((void *) destelem,(const void *) sourcelem,typesize[destelem->type_id]);
    break;
  case Sort_type:
    memmove((void *) destelem,(const void *) sourcelem,typesize[destelem->type_id]);
    if (((sortptr) sourcelem)->sortdata1 != Nil)
    {
      ((sortptr) destelem)->sortdata1 = (alltypes *)
	create_blank_elem(Sort_type);
      memmove((void *) ((sortptr) destelem)->sortdata1,
	      (const void *) ((sortptr) sourcelem)->sortdata1,
	      sizeof(union anydata));
    }
    if (((sortptr) sourcelem)->sortdata2 != Nil)
    {
      ((sortptr) destelem)->sortdata2 = (alltypes *)
	create_blank_elem(Sort_type);
      memmove((void *) ((sortptr) destelem)->sortdata2,
	      (const void *) ((sortptr) sourcelem)->sortdata2,
	      sizeof(union anydata));
    }
    if (((sortptr) sourcelem)->sortdata3 != Nil)
    {
      ((sortptr) destelem)->sortdata3 = (alltypes *)
	create_blank_elem(Sort_type);
      memmove((void *) ((sortptr) destelem)->sortdata3,
	      (const void *) ((sortptr) sourcelem)->sortdata3,
	      sizeof(union anydata));
    }
    break;
  case Bound_type:
    copy_bound_elem((boundptr) sourcelem, (boundptr) destelem);
    break;
  case Fve_type:
    copy_fve_elem((fveptr) sourcelem, (fveptr) destelem);
    break;
  case Shell_type:
    copy_shell_elem((shellptr) sourcelem, (shellptr) destelem);
    break;
  case Obj_type:
    copy_obj_elem((objptr) sourcelem, (objptr) destelem);
    break;
  case World_type:
    copy_world_elem((worldptr) sourcelem, (worldptr) destelem);
    break;
  case Ring_type: 
    copy_ring_elem((ringptr) sourcelem, (ringptr) destelem);
    break;
  case Curve_type:
    copy_curve_elem((curveptr) sourcelem, (curveptr) destelem);
    break;
  case Surf_type:
    copy_surf_elem((surfptr) sourcelem, (surfptr) destelem);
    break;
  case Facepair_type:
    copy_facepair_elem((facepairptr) sourcelem, (facepairptr) destelem);
    break;
  default:
    sprintf(errormsg,"copy_elem: unknown type id %d",destelem->type_id);
    system_error(errormsg);
  }
  destelem->proplist = destproplist;
  copy_list(sourcelem->proplist,destelem->proplist);
  destelem->global_proplists = sourcelem->global_proplists;
}

  elemptr
clone_elem(elemptr original)
{ /* returns a copy,byte for byte, of the original list element */
  elemptr new_elem;
  ;
  new_elem = create_elem(original->type_id);
  copy_elem(original,new_elem);

  return (new_elem);
}

  void
set_elem_fields(new_elem,firstfield) /* assumes type_id has been set */
  elemptr new_elem;
  long *firstfield;
{
  switch (new_elem->type_id)
  {
  case Elem_type:
    break;
  case Ep_type:
    break;
  case Delem_type:
    break;
  case Oelem_type:
    break;
  case Qelem_type:
    break;
  case Sort_type:
    break;
  default:
    system_error("append: unknown type id!");
    break;
  }
}

  elemptr
create_elem(type_id,firstfield)
  int type_id;
  long firstfield;
{
  elemptr new_elem,tempfirst;
  ;
  new_elem = create_blank_elem(type_id);
  set_elem_fields(new_elem,&firstfield);
  
  return (new_elem);
}

  elemptr
add_new_elem(the_list,prev_elem,type_id,firstfield) /* insert an element */
  listptr the_list;		/* in a list, setting its fields */
  elemptr prev_elem;
  int type_id;
  long firstfield;
{
  elemptr new_elem,tempfirst;
  ;
  new_elem = create_blank_elem(type_id);
  set_elem_fields(new_elem,&firstfield);
  add_elem(the_list,prev_elem,new_elem);
  
  return (new_elem);
}

  elemptr
insert_new_elem(the_list,next_elem,type_id,firstfield) /* insert an element */
  listptr the_list;		/* in a list, setting its fields */
  elemptr next_elem;
  int type_id;
  long firstfield;
{
  elemptr new_elem,tempfirst;
  ;
  new_elem = create_blank_elem(type_id);
  set_elem_fields(new_elem,&firstfield);
  insert_elem(the_list,next_elem,new_elem);
  
  return (new_elem);
}

  elemptr
append_new_elem(the_list,type_id,firstfield) /* appends an elem of type */
  listptr the_list;		/* type_id to the given linked list filling */
  int type_id;			/* in its fields */
  long firstfield;
{
  elemptr old_last_elem,new_elem;
  ;
  new_elem = create_blank_elem(type_id);
  set_elem_fields(new_elem,&firstfield);
  append_elem(the_list,new_elem);

  return(new_elem);
}

  Boolean
already_in_featurelist(listptr check_list,featureptr check_feature)
{
  elemptrptr check_ep;
  ;
  check_ep = check_list->first.ep;
  while (check_ep != Nil)
  {
    if (check_ep->thiselem == check_feature)
      return(TRUE);
    check_ep = check_ep->next;
  }
  return(FALSE);
}

  void
add_to_featurelist(listptr add_list,featureptr new_feature)
{
  elemptrptr newfeature_ep;
  ;
  if (!already_in_featurelist(add_list,new_feature))
  {
    long dummy;
    newfeature_ep = (elemptrptr) append_new_elem(add_list, Ep_type, dummy);
    newfeature_ep->thiselem = new_feature;
  }
}
  

/* merge two ep lists, but only the elements that are unique (don't make */
/* duplicates).  N^2 algorithm... oh well. */

  void
merge_lists_uniquely(listptr list1,listptr list2)
{
  elemptrptr newfeature_ep,nextep;
  featureptr newfeature;
  ;
  newfeature_ep = list2->first.ep;
  while (newfeature_ep != Nil)
  {
    newfeature = newfeature_ep->thiselem;
    nextep = newfeature_ep->next;
    if (!already_in_featurelist(list1,newfeature))
    {
      rem_elem(list2,(elemptr) newfeature_ep);
      append_elem(list1,(elemptr) newfeature_ep);
    }
    newfeature_ep = nextep;
  }
}

  void
remove_from_featurelist(listptr remove_list,featureptr removed_feature)
{
  elemptrptr removedfeature_ep;
  ;
  removedfeature_ep = remove_list->first.ep;
  while (removedfeature_ep != Nil)
  {
    if (removedfeature_ep->thiselem == removed_feature)
    {
      del_elem(remove_list,removedfeature_ep);
      break;
    }
    removedfeature_ep = removedfeature_ep->next;
  }
}

  void				/* freee...nelson mandela */
free_elem(elemptr *thiselem)
{
  freecount--;
/*  printf ("freecount = %d\n",freecount);*/
  free(*thiselem);
  *thiselem = Nil;		/* so we barf if accidentally refree... */
}

void del_elem(listptr, void *);
void del_list(listptr);

  static void
delete_elem_lists(elemptr this_elem)
{
  prim_rec_ptr killed_prim_rec;
  register listptr thislist;
  ;
  switch (this_elem->type_id)
  {
  case Elem_type: break;
  case Ep_type: break;
  case Delem_type: break;
  case Oelem_type: break;
  case Qelem_type: break;
  case Sort_type:
    if (((sortptr) this_elem)->sortdata1)
      free_elem((elemptr *) &(((sortptr) this_elem)->sortdata1));
    if (((sortptr) this_elem)->sortdata2)
      free_elem((elemptr *) &(((sortptr) this_elem)->sortdata2));
    if (((sortptr) this_elem)->sortdata3)
      free_elem((elemptr *) &(((sortptr) this_elem)->sortdata3));
    break;
  case Sketchle_type: break;
  case Sector_type: break;
  case Sectorpair_type: break;
  case Vee_type: break;
  case Ef_type: break;
  case Vv_type:
    delete_vv_lists((vvptr) this_elem);
    break;
  case Vf_type:
    delete_vf_lists((vfptr) this_elem);
    break;
  case Prop_type: break;
  case Vfe_type:
    if (((vfeptr) this_elem)->thecolor)
      free_elem((elemptr *) &(((vfeptr) this_elem)->thecolor));
    break;
  case Evf_type:
    if (((evfptr) this_elem)->thecolor)
      free_elem((elemptr *) &(((evfptr) this_elem)->thecolor));
    break;
  case Le_type:
    ((leptr) this_elem)->facedge = Nil;
    ((leptr) this_elem)->facevert = Nil;
    break;
  case Bound_type:
    delete_bound_lists((boundptr) this_elem);
    break;
  case Fve_type:
    if (((fveptr) this_elem)->thecolor)
      free_elem((elemptr *) &(((fveptr) this_elem)->thecolor));
    delete_fve_lists((fveptr) this_elem);
    break;
  case Shell_type:
    delete_shell_lists((shellptr) this_elem);
    break;
  case Obj_type:
    if (((objptr) this_elem)->thecolor)
      free_elem((elemptr *) &(((objptr) this_elem)->thecolor));
    if (has_property((featureptr) this_elem,primitive_prop))
    {
      killed_prim_rec = (prim_rec_ptr) get_property_val((featureptr) this_elem,
						    primitive_prop).ptr;
      if (killed_prim_rec->prim_info != Nil)
	free_elem((elemptr *) &(killed_prim_rec->prim_info));
      free_elem((elemptr *) &killed_prim_rec);
    }
    delete_obj_lists((objptr) this_elem);
    break;
  case World_type:
    delete_world_lists((worldptr) this_elem);
    break;
  case Seg_type: break;
  case Ring_type:
    delete_ring_lists((ringptr) this_elem);
    break;
  case Curve_type:
    delete_curve_lists((curveptr) this_elem);
    break;
  case Surf_type:
    delete_surf_lists((surfptr) this_elem);
    break;
  case Facepair_type:
    delete_facepair_lists((facepairptr) this_elem);
    break;
  case Lepair_type: break;
  default:
    system_error("delete_elem_lists: unknown type id %d!\n");
    break;
  }
  clear_feature_properties((featureptr) this_elem);
  /* We need to set the mother fields to Nil separately and AFTER */
  /* clear_feature_properties because */
  /* determine_world_proplist, used by clear_feature_properties call above, */
  /* depends on them still being set in order to get at the correct world */
  /* proplist for the elem. */
  switch(this_elem->type_id)
  {
  case Le_type:
    ((leptr) this_elem)->motherbound = Nil;
    break;
  case Bound_type:
    ((boundptr) this_elem)->motherfve = Nil;
    break;
  case Fve_type:
    ((fveptr) this_elem)->mothersurf = Nil;
    ((fveptr) this_elem)->mothershell = Nil;
    break;
  case Shell_type:
    ((shellptr) this_elem)->motherobj = Nil;
    break;
  case Obj_type:
    ((objptr) this_elem)->motherworld = Nil;
    break;
  }

  free_elem((elemptr *) &(this_elem->proplist));
}

  void
del_elem(listptr the_list, void *this_elem) /* delete an element from a list */
{
  delete_elem_lists(this_elem);
  rem_elem(the_list,this_elem);
  free_elem((elemptr *) &this_elem);
}  

void
kill_elem(void *this_elem)	/* wipe out an elem not associated with a list */
{
  delete_elem_lists(this_elem);
  free_elem((elemptr *) &this_elem);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                           list routines                                */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  listptr
create_list()                       /* Create an empty list */
{
  register listptr new_list;
  ;
  
  new_list = (listptr) alloc_elem(Listsize);
  /* alloc_elem uses calloc so: # of elems is 0, last and first point to Nil */
  /* and the list_kind is Normal_list */
  if (new_list == Nil)
    system_error("create_list: out of memory!");

  return (new_list);
}

  listptr
create_circular_list()		/* Create an empty circular list */
{
  register listptr new_list;
  ;
  
  new_list = (listptr) alloc_elem(Listsize);
  /* alloc_elem uses calloc so: # of elems is 0, last and first point to Nil */
  new_list->kind = Circular_list;
  if (new_list == Nil)
    system_error(" create_list: out of memory!");

  return (new_list);
}

  void
clear_list(the_list)		/* deletes list but not its head */
  listptr the_list;
{
  while (the_list->numelems > 0) /* stop on # elem's in case of circ. list */
    del_elem(the_list,the_list->first.any);
}
  void
del_list(the_list)		/* deletes list and its head */
  listptr the_list;
{
  while (the_list->numelems > 0) /* stop on # elem's in case of circ. list */
    del_elem(the_list,the_list->first.any);
  free_elem((elemptr *) &the_list);
}

  listptr
transfer_list_contents(listptr sourcelist, listptr destlist)
{
  /* this is a destructive append sourcelist is appended onto destlist */
  /* and sourcelist's head is NOT freed */
  if (sourcelist->first.any)
  {
    if (destlist->first.any)
    {
      destlist->last.any->next = sourcelist->first.any;
      sourcelist->first.any->prev = destlist->last.any;
    }
    else
      destlist->first.any = sourcelist->first.any;
    destlist->last.any = sourcelist->last.any;
    destlist->numelems += sourcelist->numelems;

    sourcelist->numelems = 0;
    sourcelist->first.any = Nil;
    sourcelist->last.any = Nil;
  }

  return(destlist);
}

  listptr
merge_lists(listptr sourcelist, listptr destlist)
{
  /* this is a destructive append sourcelist is appended onto destlist */
  /* and sourcelist's head IS destroyed */
  if (sourcelist->first.any)
  {
    if (destlist->first.any)
    {
      destlist->last.any->next = sourcelist->first.any;
      sourcelist->first.any->prev = destlist->last.any;
    }
    else
      destlist->first.any = sourcelist->first.any;
    destlist->last.any = sourcelist->last.any;
    destlist->numelems += sourcelist->numelems;
  }

  free_elem((elemptr *) &sourcelist);

  return(destlist);
}

  listptr
clone_list(listptr old_list)
{ /* copies the list pointed to by old_list and returns the copy */
  elemptr this_elem,new_elem;
  listptr new_list;  
  int elemsize;
  ;
  new_list = Nil;
  if (old_list->first.any != Nil)
  {
    if (old_list->kind == Circular_list)
      new_list = create_circular_list();
    else
      new_list = create_list();
    new_list->first.any = clone_elem(old_list->first.any);
    new_list->numelems = old_list->numelems;
    new_elem = new_list->first.any;
    this_elem = old_list->first.any->next;
    while (this_elem)
    {
      new_elem->next = clone_elem(this_elem);
      new_elem->next->prev = new_elem;
      new_elem->next->next = Nil;
      new_elem = new_elem->next;
      this_elem = this_elem->next;
    }
    if (old_list->kind == Circular_list)
    {
      new_elem->next = new_list->first.any;
      new_list->first.any->prev = new_elem;
    }
    else
      new_elem->next = Nil;
    new_list->last.any = new_elem;
  }
  return(new_list);
}

  listptr
append_list(listptr sourcelist, listptr destlist)
{ /* this is just like merge_lists except it is non-destructive */
  listptr new_list;
  elemptr this_elem,new_elem;
  int elemsize;
  ;
  elemsize = typesize[sourcelist->first.any->type_id];
  new_list = clone_list(destlist);
  this_elem = sourcelist->first.any;
  while (this_elem)
  {
    new_elem = clone_elem(this_elem);
    append_elem(new_list,new_elem);
    this_elem = this_elem->next;
  }
  if (destlist->kind == Circular_list)
  {
    new_list->last.any = new_elem;
    new_elem->next = new_list->first.any;
    new_list->first.any->prev = new_elem;
  }
  else
  {
    new_elem->next = Nil;
    new_list->last.any = new_elem;
  }

  return(new_list);
}

  void
copy_list(listptr sourcelist, listptr destlist)
{ /* this this makes a clone of the source and appends it on the dest */
  listptr sourcelist_clone;
  ;
  sourcelist_clone = clone_list(sourcelist);
  if (sourcelist_clone != Nil)
    merge_lists(sourcelist_clone,destlist);
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                        miscellaneous routines                          */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* returns TRUE if an element of the type elem_type normally has a proplist */
/* Used to determine whether to allocate one in create_blank_elem, and */
/* whether one needs to remove the proplist when deleting an element. */

  Boolean
elem_has_proplist(int elem_type)
{
  switch (elem_type)
  {
  case Elem_type:
  case Sort_type:
  case Sector_type:
  case Sectorpair_type:
  case Vv_type:
  case Vf_type:
  case Vee_type:
  case Ef_type:
  case Ep_type:
  case Vfe_type:
  case Evf_type:
  case Le_type:
  case Bound_type:
  case Fve_type:
  case Shell_type:
  case Obj_type:
  case World_type:
  case Seg_type:
  case Ring_type:
  case Curve_type:
  case Surf_type:
  case Prop_type:
  case Sketchle_type:
    return(TRUE);
  default:
    return(FALSE);
  }
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                       basic FIFO queue routines                        */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


  Boolean
elem_qtest(listptr elemqueue)
{
  return (elemqueue->numelems > 0);
}

/* Add new queued elems at bottom of list */
  void
queue_elem(listptr elemqueue,elemptr new_qelem)
{
  append_elem(elemqueue,new_qelem);
}

/* Pull old queued elems from top of list. */
  elemptr
unqueue_elem(listptr elemqueue)
{
  elemptr top_elem;
  ;
  top_elem = elemqueue->first.any;
  if (top_elem != Nil)
    rem_elem(elemqueue,top_elem);
  else
    system_error("unqueue_elem:Empty elem queue!!!");
  return(top_elem);
}
