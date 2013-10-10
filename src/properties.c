
/* FILE: properties.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*      Object Property List Handling Routines for the Cutplane Editor    */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 18,1988  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define PROPERTIESMODULE

#include <config.h>

#include <stdio.h>
#include <string.h>

#include <platform_gl.h>

#include <cutplane.h> /* cutplane.h includes properties.h */
#include <globals.h>
#include <update.h>

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                     Low level global property list routines              */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

listptr *
determine_world_proplist(featureptr thisfeature)
{
  objptr theobj;
  ;
  switch(thisfeature->type_id)
  {
  case Le_type:
    theobj = ((leptr) thisfeature)->motherbound->
      motherfve->mothershell->motherobj;
    break;
  case Vfe_type:
    theobj = ((vfeptr) thisfeature)->startle->motherbound->
      motherfve->mothershell->motherobj;
    break;
  case Evf_type:
    theobj = ((evfptr) thisfeature)->le1->motherbound->
      motherfve->mothershell->motherobj;
    break;
  case Fve_type:
    theobj = ((fveptr) thisfeature)->mothershell->motherobj;
    break;
  case Shell_type:
    theobj = ((shellptr) thisfeature)->motherobj;
    break;
  case Obj_type:
    theobj = ((objptr) thisfeature);
    break;
  default:
    return (working_world->world_proplists);
  }
  if (theobj != Nil)
    return (theobj->motherworld->world_proplists);
  else
    return (working_world->world_proplists);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                     global property list routines                        */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
  

  void
init_proplists(listptr * proplist_array)
{
  int thisproperty;
  ;
  thisproperty = (int) noproperty_prop;
  /* Note: Iris lameness cannot compare two enums of same type!!!! */
  while (thisproperty < (int) next_available_property)
  {
    proplist_array[thisproperty] = create_list();
    thisproperty++;
  }
}
  
  static void
add_global_property(proptr thisprop,listptr *global_proplists)
{
  proptr new_globalprop;
  ;
  new_globalprop =
    (proptr) add_new_blank_elem(global_proplists[(int) thisprop->propkind],Nil,
				Prop_type);
  new_globalprop->owner = thisprop->owner;
  new_globalprop->value = thisprop->value;
}

  static void
del_global_property(proptr thisprop,listptr *global_proplists)
{
  elemptr propowner;
  proptr globalprop;
  listptr theproplist;
  ;
  propowner = thisprop->owner;
  theproplist = global_proplists[(int) thisprop->propkind];
  globalprop = theproplist->first.prop;
  while (globalprop != Nil)
  {
    if (globalprop->owner == propowner)
    {
      del_elem(theproplist,(elemptr) globalprop);
      return;
    }
    globalprop = globalprop->next;
  }
  printf ("msg from del_global_property:feature not in world proplist!\n");
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                indiviual element property list routines                  */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  Boolean
has_property(featureptr thisfeature,property searchpropkind)
{
  proptr thisprop;
  ;
  /* first do a quick check of fast flags. */
  if (searchpropkind == visible_prop)
    return ((thisfeature->featureflags & Visibleflag) > 0);
  if (searchpropkind == selectable_prop)
    return ((thisfeature->featureflags & Selectableflag) > 0);
  if (searchpropkind == picked_prop)
    return ((thisfeature->featureflags & Pickedflag) > 0);
  if (searchpropkind == transparent_prop)
    return ((thisfeature->featureflags & Transparentflag) > 0);
  if (searchpropkind == msgpending_prop)
    return ((thisfeature->featureflags & Msgpendingflag) > 0);
  if (searchpropkind == nonmanifold_prop)
    return ((thisfeature->featureflags & Nonmanifoldflag) > 0);
  if (searchpropkind == noshadow_prop)
    return ((thisfeature->featureflags & Noshadowflag) > 0);
  if (searchpropkind == lockedgeometry_prop)
    return ((thisfeature->featureflags & Lockedgeometryflag) > 0);
  if (searchpropkind == neverdraw_prop)
    return ((thisfeature->featureflags & Neverdrawflag) > 0);
  if (searchpropkind == alwaysdraw_prop)
    return ((thisfeature->featureflags & Alwaysdrawflag) > 0);
  if (searchpropkind == sectioninvisible_prop)
    return ((thisfeature->featureflags & Sectioninvisibleflag) > 0);
  if (searchpropkind == selectinvisible_prop)
    return ((thisfeature->featureflags & Selectinvisibleflag) > 0);
  if (searchpropkind == pickedinvisible_prop)
    return ((thisfeature->featureflags & Pickedinvisibleflag) > 0);

  /* if not one of the fast flags, search through the property linked list */
  for (thisprop = thisfeature->proplist->first.prop; thisprop != Nil;
       thisprop = thisprop->next)
    if (thisprop->propkind == searchpropkind)
      return(TRUE);

  return(FALSE);
}


/* Get the nth feature that has the property from the global proplist */
/* for the working world. */

  featureptr
get_property_feature(worldptr world,property thisproperty,int n)
{
  proptr thisprop;
  int count = 1;
  listptr *working_proplists;
  ;
  working_proplists = world->world_proplists;
  for (thisprop = working_proplists[(int) thisproperty]->first.prop;
       thisprop != Nil; thisprop = thisprop->next,count++)
    if (count == n)
      return(thisprop->owner);
  return(Nil);			/* n out of range or no features w/that prop */
}

  alltypes
get_property_val(featureptr thisfeature, property thispropkind)
{	/* maybe return a found flag and value in arg instead? */
  proptr thisprop;
  static alltypes zero;
  ;
  for (thisprop = thisfeature->proplist->first.prop; thisprop != Nil;
       thisprop = thisprop->next)
    if (thisprop->propkind == thispropkind)
      return(thisprop->value);

  printf ("Message from get_property: feature doesn't have %s property (%d)!\n",
	  propinfo[thispropkind].propname, (int) thispropkind);
  zero.l = 0;
  return(zero);
}

  void
set_prop_flags(featureptr thisfeature,property setpropkind)
{
  shellptr mothershell;
  ;
  if (setpropkind == visible_prop)
  {
    thisfeature->featureflags |= Visibleflag;
    log_update(thisfeature,Transformflag | Evfeqnsflag |
	       Cutevfsflag | Makecutsectionflag | Findnormalsflag |
	       Computebboxflag | Recomputebackfaceflag | Logerasebboxflag,
	       Immediateupdate);
    log_global_update(Redrawflag);
  }
  else if (setpropkind == selectable_prop)
    thisfeature->featureflags |= Selectableflag;
  else if (setpropkind == picked_prop)
    thisfeature->featureflags |= Pickedflag;
  else if (setpropkind == transparent_prop)
    thisfeature->featureflags |= Transparentflag;
  else if (setpropkind == msgpending_prop)
    thisfeature->featureflags |= Msgpendingflag;
  else if (setpropkind == nonmanifold_prop)
    thisfeature->featureflags |= Nonmanifoldflag;
  else if (setpropkind == noshadow_prop)
    thisfeature->featureflags |= Noshadowflag;
  else if (setpropkind == lockedgeometry_prop)
    thisfeature->featureflags |= Lockedgeometryflag;
  else if (setpropkind == neverdraw_prop)
    thisfeature->featureflags |= Neverdrawflag;
  else if (setpropkind == alwaysdraw_prop)
    thisfeature->featureflags |= Alwaysdrawflag;
  else if (setpropkind == sectioninvisible_prop)
    thisfeature->featureflags |= Sectioninvisibleflag;
  else if (setpropkind == selectinvisible_prop)
    thisfeature->featureflags |= Selectinvisibleflag;
  else if (setpropkind == pickedinvisible_prop)
    thisfeature->featureflags |= Pickedinvisibleflag;
}

/* Adds new properties to a feature at the BEGINNING of the proplist. */

  void
add_property(featureptr thisfeature,property newpropkind)
{
  proptr newprop;
  ;
  if (!has_property(thisfeature,newpropkind))
  {
    newprop = (proptr) add_new_blank_elem(thisfeature->proplist,Nil,Prop_type);
    newprop->propkind = newpropkind;
    newprop->owner = thisfeature;
    add_global_property(newprop,determine_world_proplist(thisfeature));
    set_prop_flags(thisfeature,newpropkind);
  }
}


  void
set_property(thisfeature,thepropkind,value) /* no proto here since value  */
  elemptr thisfeature;		/* could be any type */
  property thepropkind;
  long value;
{
  proptr thisprop,newprop;  
  ;
  thisprop = thisfeature->proplist->first.prop;
  while (thisprop)
  {
    if (thisprop->propkind == thepropkind)
    {
      thisprop->value.l = value;
      return;
    }
    thisprop = thisprop->next;
  }
  newprop = (proptr) add_new_blank_elem(thisfeature->proplist,Nil,Prop_type);
  newprop->propkind = thepropkind;
  newprop->owner = thisfeature;
  newprop->value.l = value;
  set_prop_flags(thisfeature,thepropkind);
  add_global_property(newprop,determine_world_proplist(thisfeature));
}

/* Clear a property flag... if clearing it requires updating the selectability */
/* status of a motherobject, log an update to do so. */

  void
clear_prop_flags(featureptr thisfeature,property doomedpropkind)
{
  objptr motherobj;
  ;
  if (doomedpropkind == visible_prop)
  {
    thisfeature->featureflags &= ~Visibleflag;
    motherobj = (objptr) find_largergrain(thisfeature,Obj_type);
    log_update(thisfeature,Clearcutsectionflag | Getselectableflag |
	       Computebboxflag | Logerasebboxflag,Immediateupdate);
    log_global_update(Redrawflag);
  }
  else if (doomedpropkind == selectable_prop)
    thisfeature->featureflags &= ~Selectableflag;
  else if (doomedpropkind == picked_prop)
    thisfeature->featureflags &= ~Pickedflag;
  else if (doomedpropkind == transparent_prop)
    thisfeature->featureflags &= ~Transparentflag;
  else if (doomedpropkind == msgpending_prop)
    thisfeature->featureflags &= ~Msgpendingflag;
  else if (doomedpropkind == nonmanifold_prop)
    thisfeature->featureflags &= ~Nonmanifoldflag;
  else if (doomedpropkind == noshadow_prop)
    thisfeature->featureflags &= ~Noshadowflag;
  else if (doomedpropkind == lockedgeometry_prop)
    thisfeature->featureflags &= ~Lockedgeometryflag;
  else if (doomedpropkind == neverdraw_prop)
    thisfeature->featureflags &= ~Neverdrawflag;
  else if (doomedpropkind == alwaysdraw_prop)
    thisfeature->featureflags &= ~Alwaysdrawflag;
  else if (doomedpropkind == sectioninvisible_prop)
    thisfeature->featureflags &= ~Sectioninvisibleflag;
  else if (doomedpropkind == selectinvisible_prop)
    thisfeature->featureflags &= ~Selectinvisibleflag;
  else if (doomedpropkind == pickedinvisible_prop)
    thisfeature->featureflags &= ~Pickedinvisibleflag;
}

  void
del_property(featureptr thisfeature, property doomedpropkind)
{
  proptr doomedprop;
  worldptr feature_world;
  shellptr owner_shell;
  ;
  doomedprop = thisfeature->proplist->first.prop;
  while (doomedprop)
  {
    if (doomedprop->propkind == doomedpropkind)
    {
#if 0
      fprintf (dbgdump,
	       "del_property: deleting property %d!\n",doomedpropkind);
#endif

      del_global_property(doomedprop,determine_world_proplist(thisfeature));
      del_elem(thisfeature->proplist,(elemptr) doomedprop);
      clear_prop_flags(thisfeature,doomedpropkind);
      return;
    }
    doomedprop = doomedprop->next;
  }
  printf("Message from get_property: feature doesn't have %s property (%d)!\n",
	 propinfo[doomedpropkind].propname, (int) doomedpropkind);
}

/* Clear all features in the working world of the given property kind. */

  void
clear_property(property doomedpropkind)
{
  listptr theproplist;
  listptr *working_proplists;
  ;
  working_proplists = working_world->world_proplists;
  theproplist = working_proplists[(int) doomedpropkind];
  while (theproplist->numelems > 0)
    del_property(theproplist->last.prop->owner,doomedpropkind); 
}

/* add property to a feature's mother object and all its children objects */

  void
add_property_to_descendants(featureptr thisfeature,property newpropkind)
{
  objptr motherobj;
  ;
  motherobj = (objptr) find_largergrain(thisfeature,Obj_type);
  apply_to_descendants(motherobj, add_property, (void *) newpropkind);
}

/* remove a property from a feature's mother object and its children objects */

  void
del_property_from_descendants(featureptr thisfeature,property doomedpropkind)
{
  objptr motherobj;
  ;
  motherobj = (objptr) find_largergrain(thisfeature,Obj_type);
  apply_to_descendants(motherobj, del_property, (void *) doomedpropkind);
}

  int
find_property(char *property_name)
{
  property searchprop;
  ;
  searchprop = noproperty_prop;
  while (searchprop < next_available_property)
  {
    if (strcmp(propinfo[searchprop].propname,property_name) == 0)
      return(searchprop);
    searchprop++;
  }
  return(-1);
}

  int
define_new_property(char *prop_name,CPL_word_ptr prop_code_address)
{
  worldptr world;
  ;
  strcpy(propinfo[next_available_property].propname,prop_name);
  propinfo[next_available_property].propertycode = prop_code_address;
  world = world_list->first.world;
  while (world != Nil)
  {				/* initialize proplists in all worlds */
    world->world_proplists[next_available_property] = create_list();
    world = world->next;
  }
  next_available_property++;
  return(next_available_property);
}

  void
define_property(char *prop_name,CPL_word_ptr prop_code_address)
{
  property lookup_prop;
  ;
  /* first, see if there is not already defined a property by this name. */
  /* if so, just change the CPLcode it uses.  This allows you to dynamically */
  /* change the action of properties.  Whether this is useful remains to be */
  /* seen. */
  lookup_prop = find_property(prop_name);
  if (lookup_prop != -1)
    propinfo[lookup_prop].propertycode = prop_code_address;
  else
    define_new_property(prop_name,prop_code_address);
}


  void
clear_feature_properties(featureptr thisfeature)
{
  proptr doomedprop,nextprop;
  ;
  doomedprop = thisfeature->proplist->first.prop;
  while (doomedprop)
  {
    nextprop = doomedprop->next;
    del_global_property(doomedprop,determine_world_proplist(thisfeature));
    del_elem(thisfeature->proplist,(elemptr) doomedprop);
    doomedprop = nextprop;
  }
}
  
  void
transfer_feature_properties(featureptr sourcefeature,featureptr destfeature)
{
  proptr transprop,nextprop;
  alltypes tempvalue;
  property tempkind;
  ;
  transprop = sourcefeature->proplist->first.prop;
  while (transprop)
  {
    nextprop = transprop->next;
    tempkind = transprop->propkind;
    tempvalue = transprop->value;
    del_global_property(transprop,determine_world_proplist(sourcefeature));
    del_elem(sourcefeature->proplist,(elemptr) transprop);
    set_property(destfeature,tempkind,tempvalue);
    transprop = nextprop;
  }
}
  
  void
copy_feature_properties(featureptr sourcefeature,featureptr destfeature)
{
  proptr copyprop;
  ;
  copyprop = sourcefeature->proplist->first.prop;
  while (copyprop)
  {
    set_property(destfeature,copyprop->propkind,copyprop->value);
    copyprop = copyprop->next;
  }
}
