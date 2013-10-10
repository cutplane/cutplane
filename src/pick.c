
/* FILE: pick.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Picking Routines for the Cutplane Editor                     */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define PICKMODULE

#include <config.h>
#include <stdio.h>
#include <math.h>

#include <platform_gl.h>

#include <cutplane.h>

#include <pick.h>	/* for Pkboxsize */
#include <globals.h>
#include <math3d.h>
#include <grafx.h>	/* for c-c-colors */

/* 3d selection routines were once here but now gone due to plane tabbing (!)*/

/* also choose_selectable was here but is now kaput*/

  void
clear_pickedtranslates()
{
  proptr thispicked;
  ;
  thispicked = (working_world->world_proplists[(int) picked_prop])->first.prop;
  while (thispicked != Nil)
  {
    clear_feature_translates(thispicked->owner);
    thispicked = thispicked->next;
  }
}

/* unpick_feature assumes the feature is ALREADY picked.  If not, */
/* you will see a message from remove_property. */

  void
unpick_feature(featureptr pickfeature)
{
  if (pickfeature == Nil)
    return;
  fprintf(dbgdump, "trying to unpick\n");
  del_property(pickfeature,picked_prop);
}


/* pick_feature merely adds the picked property to feature and sets the */
/* featureflags Pickedflag bit to TRUE.*/
  void
pick_feature(featureptr pickfeature)
{
  if (pickfeature == Nil)
    return;
  fprintf(dbgdump, "trying to pick\n");
  set_property(pickfeature,picked_prop,0);
}


  void
prune_pickedlist(stateptr state)
{
  proptr thispicked,nextpicked;
  shellptr motherpicked;
  ;
  thispicked = working_world->world_proplists[(int) picked_prop]->first.prop;
  while (thispicked != Nil)
  {
    motherpicked = (shellptr) find_largergrain(thispicked->owner,Shell_type);
    nextpicked = thispicked->next;
    /* if shell no longer intersects plane, unpick */
    if (motherpicked->ringlist->numelems == 0)
      unpick_feature(thispicked->owner);
    thispicked = nextpicked;
  }
}


#if 0				/* ancient stuff */
  void
boolean_op_picked_objects(worldptr world, stateptr state)
{
  proptr thispicked,nextpicked;
  shellptr shellA,shellB,newshell;
  static char boolsavename[] = "boolsave.cut";
  listptr save_shellist;
  listptr shell_list;
  ;
  shell_list = world->world_shellist;
  save_shellist = create_list();
  thispicked = (world->world_proplists[picked_prop])->first.prop;
  while (thispicked != Nil)
  {
    shellA = (shellptr) find_largergrain(thispicked->owner,Shell_type);
    nextpicked = thispicked->next;
    if (nextpicked != Nil)
    {
      shellB = (shellptr) find_largergrain(nextpicked->owner,Shell_type);
      /* beware of multiple picked features in one object because */
      /* that object will be dup'ed once for every picked feature! */
      unpick_feature((featureptr) thispicked->owner);
      unpick_feature((featureptr) nextpicked->owner);
      thispicked = nextpicked->next;
      newshell = duplicate_obj(objA);
      rem_elem(shell_list,(elemptr) newshell);
      append_elem(save_shellist, (elemptr) newshell);
      newshell = duplicate_obj(objB);
      rem_elem(shell_list,(elemptr) newshell);
      append_elem(save_shellist, (elemptr) newshell);
/* hack fix me!!! for new shellist_to_file on worlds */
/*      shelllist_to_file(boolsavename,save_shellist);*/
      transform_shells(save_shellist);
      find_normals(save_shellist);
      topology_check(shellA);
      topology_check(shellB);      
      do_boolean(shellA->motherobj->motherworld,shellA,shellB,Subtraction,state);
/*      transform_shells(shell_list);
      find_normals(shell_list);*/
      break;			/* for now do two objects only */
    }
  }
}
#endif
