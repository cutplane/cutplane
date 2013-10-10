
/* FILE: duplicate.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*         Object Duplication Routines for the Cutplane Editor            */
/*                                                                        */
/* Author: WAK                                    Date: December 18,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define DUPLICATEMODULE

#include <config.h>
#include <stdio.h>

#include <platform_gl.h>

#include <cutplane.h>

int recursion_depth,max_recursion_depth; /* for testing how deep it goes */


  void
duplicate_engine(leptr firstoldle,leptr firstnewle,
		 listptr src_vfelist,listptr dest_lelist,
		 listptr src_fvelist,listptr dest_fvelist);


/* make a copy of the shell using mantylyisms. */
/* currently doesn't handle holes */

#define improveme

  elemptr
find_dest(elemptr search_elem,listptr src_list, listptr dest_list)
{
  elemptrptr this_src,this_dest;  
  ;
  this_src = src_list->first.ep;
  this_dest = dest_list->first.ep;
  while (this_src != Nil)
  {
    if (this_src->thiselem == search_elem)
      return(this_dest->thiselem);
    this_src = this_src->next;
    this_dest = this_dest->next;
  }
  return(Nil);
}



  void
mark_duped(elemptr oldmarkelem,elemptr newmarkelem,
	   listptr src_marklist,listptr dest_marklist)
{
  elemptrptr new_src,new_dest;
  ;
  if (find_dest(oldmarkelem,src_marklist,dest_marklist) == Nil)
  {				/* only mark if not already marked. */
    new_src = (elemptrptr) append_new_blank_elem(src_marklist,Ep_type);
    new_dest = (elemptrptr) append_new_blank_elem(dest_marklist,Ep_type);
    new_src->thiselem = (elemptr) oldmarkelem;
    new_dest->thiselem = (elemptr) newmarkelem;
  }
}


  void
duplicate_holes(fveptr sourcefve,
		listptr src_lelist,listptr dest_lelist,
		listptr src_fvelist,listptr dest_fvelist)
{
  boundptr oldhole,newhole;
  leptr firstle,newle,firstoldholele,firstnewholele;
  fveptr punchedfve;
  ;
  /* start with second bound (ie first hole) */
  oldhole = sourcefve->boundlist->first.bnd->next;
  punchedfve = (fveptr) find_dest((elemptr) sourcefve,
				  src_fvelist,dest_fvelist);
  firstle = punchedfve->boundlist->first.bnd->lelist->first.le;

  while (oldhole != Nil)
  {
#ifdef debugdup
    printf("start looking at holes\n");
#endif
    firstoldholele = oldhole->lelist->first.le;
    /* check to see if the hole already exists. */
    firstnewholele = (leptr) find_dest((elemptr) firstoldholele,
					 src_lelist,dest_lelist);
    /* if not, start it up with one point. */
    if (firstnewholele == Nil)
    {
#ifdef debugdup
      printf ("hole mev_kemr from\n");
      printvert(firstle->facevert->pos);
      printf ("\nto hole pos:\n");
      printvert(firstoldholele->facevert->pos);
      printf("\n");
#endif
      newle = make_edge_vert(firstle,firstle,firstoldholele->facevert->pos);
      newhole = kill_edge_make_ring(Twin_le(newle),newle);
      firstnewholele = newhole->lelist->first.le;
      duplicate_engine(firstoldholele,firstnewholele,
		       src_lelist,dest_lelist,
		       src_fvelist,dest_fvelist);
    }
    else			
    {
      /* only punch hole if already punched */
      if (punchedfve != firstnewholele->motherbound->motherfve)
      {
#ifdef debugdup
	printf ("punching hole\n");
#endif
	kill_face_make_loop(firstnewholele->motherbound->motherfve,punchedfve);
      }
    }
    oldhole = oldhole->next;
#ifdef debugdup
    printf ("done looking at holes\n");
#endif
  }
}


/* recursive routine that walks around a face, creating a new face */
/* in the new shell, then calling itself on the faces adjoining each of */
/* this face's edges.  It assumes that the first point in the face already */
/* exists (the first point in the shell is created by the make_shell call */
/* in duplicate_obj). */
/* Stage 1: Walk over the edges in the face. Three cases: */
/* a) if that edge already touched, skip to next. */
/* b) if it's not touched, and the next is touched, mef to the next. */
/* c) if it's not touched and the next is not touched, mev to the next. */
/* Stage 2: */
/* Walk over the edges in the face. For each adjoining face, call */
/* duplicate_engine with that le and the corresponding le in the new shell. */
/* */
/* Stage 3: */
/* Call duplicate_engine on each of the holes, after poking a point */
/* in the motherface. */

  void
duplicate_engine(leptr firstoldle,leptr firstnewle,
		 listptr src_lelist,listptr dest_lelist,
		 listptr src_fvelist,listptr dest_fvelist)
{
  leptr thisoldle,thisnewle,advance_newle;
  Boolean edge_made = FALSE;
  ;
  recursion_depth++;
  if (recursion_depth > max_recursion_depth)
    max_recursion_depth = recursion_depth;
  thisoldle = firstoldle;
  thisnewle = firstnewle;
  do				/* create the face */
  {
    advance_newle = thisnewle->next;
    if (!thisoldle->facedge->touched)
    {
      edge_made = TRUE;
      thisoldle->facedge->touched = TRUE; /* mark it as walked */
      if (!thisoldle->next->facedge->touched)
      {				/* case (c): do the mev, creating a strut.*/
#ifdef debugdup
	printf ("mev from\n");
	printvert(thisnewle->facevert->pos);
	printf ("\n to\n");
	printvert(thisoldle->next->facevert->pos);
	printf ("\n");
#endif
	advance_newle = make_edge_vert(thisnewle,thisnewle,
				       thisoldle->next->facevert->pos);
	/* mark going out */
	mark_duped((elemptr) thisoldle,
		   (elemptr) Twin_le(advance_newle),
		   src_lelist,dest_lelist);
	/* mark going back */
	mark_duped((elemptr) Twin_le(thisoldle),
		   (elemptr) advance_newle,
		   src_lelist,dest_lelist);
      }
      else			/* case (b): do the mef */
      {				/* setting advance_newle jumps the gap! */
	advance_newle =
	  (leptr) find_dest((elemptr) thisoldle->next,src_lelist,dest_lelist);
#ifdef debugdup
	printf ("mef from\n");
	printvert(thisnewle->facevert->pos);
	printf ("\n to\n");
	printvert(advance_newle->facevert->pos);
	printf("\n\n");
#endif
	make_edge_face(thisnewle,advance_newle);/* do the mef */
	/* mark other side of edge created */
	mark_duped((elemptr) thisoldle,
		   (elemptr) advance_newle->prev,
		   src_lelist,dest_lelist);
	mark_duped((elemptr) Twin_le(thisoldle),
		   (elemptr) Twin_le(advance_newle->prev),
		   src_lelist,dest_lelist);
      }
    }				/* else case (a), ie do nothing. */
    /* mark edge just created (going outwards or across) as well */
    thisoldle = thisoldle->next; /* go to next edge */
    thisnewle = advance_newle;
  } while (thisoldle != firstoldle);

  /* if the face was not a hole, mark it as duped and punch its holes */
  if (thisoldle->motherbound ==
      thisoldle->motherbound->motherfve->boundlist->first.bnd)
  {
    mark_duped((elemptr) firstoldle->motherbound->motherfve,
	       (elemptr) firstnewle->motherbound->motherfve,
	       src_fvelist,dest_fvelist);
    duplicate_holes(thisoldle->motherbound->motherfve,
		    src_lelist,dest_lelist,
		    src_fvelist,dest_fvelist);
  }
  
  if (edge_made)		/* no new edges were made, drop out. */
    do				/* Otherwise, */
    {				/* do the adjoining faces */
      duplicate_engine(Twin_le(thisoldle),Twin_le(thisnewle),
		       src_lelist,dest_lelist,
		       src_fvelist,dest_fvelist);
      thisoldle = thisoldle->next; /* go to next edge */
      thisnewle = thisnewle->next;
    } while (thisoldle != firstoldle);

  recursion_depth--;
}  

  void
duplicate_le_props(leptr srcle,leptr destle)
{
  vfeptr srcvfe,destvfe;
  ;
  srcvfe = srcle->facevert;	/* duplicate proplists of vfes as well as le's */
  destvfe = destle->facevert;
  copy_feature_properties((featureptr) srcvfe,(featureptr) destvfe);
  copy_feature_properties((featureptr) srcle, (featureptr) destle);
  copy_feature_properties((featureptr) srcle->facedge,
			  (featureptr) destle->facedge);
}

  void
duplicate_face_props(fveptr srcfve,fveptr destfve)
{
  boundptr srcbnd, destbnd;
  ;
  /* duplicate proplists of bounds as well as fves */
  srcbnd = srcfve->boundlist->first.bnd;
  destbnd = destfve->boundlist->first.bnd;
  while (srcbnd != Nil)
  {
    /* note MAJOR assumption here that bounds appeared in same order in both */
    /* source and dest.. reasonable assumption since it was by looping through */
    /* source bounds that the dest bonds were created. */
    copy_feature_properties((featureptr) srcbnd, (featureptr) destbnd);
    srcbnd = srcbnd->next;
    destbnd = destbnd->next;
  }
  copy_feature_properties((featureptr) srcfve,(featureptr) destfve);
  if (srcfve->thecolor != Nil)
    set_feature_color((featureptr) destfve,srcfve->thecolor);
}

  void
duplicate_properties(objptr sourceobj,objptr destobj,
		     listptr src_lelist,listptr dest_lelist,
		     listptr src_fvelist,listptr dest_fvelist)
{
  elemptrptr first_le_ep,src_le,dest_le;
  elemptrptr src_fve,dest_fve;
  shellptr srcshell,destshell;
  ;
  src_le = src_lelist->first.ep;
  dest_le = dest_lelist->first.ep;
  while (src_le != Nil)
  {
    duplicate_le_props((leptr) src_le->thiselem,(leptr) dest_le->thiselem);
    src_le = src_le->next;
    dest_le = dest_le->next;
  }

  src_fve = src_fvelist->first.ep;
  dest_fve = dest_fvelist->first.ep;
  while (src_fve != Nil)
  {
    duplicate_face_props((fveptr) src_fve->thiselem,(fveptr) dest_fve->thiselem);
    src_fve = src_fve->next;
    dest_fve = dest_fve->next;
  }
  for (srcshell = First_obj_shell(sourceobj),
       destshell = First_obj_shell(sourceobj); srcshell != Nil;
       srcshell = srcshell->next, destshell = destshell->next)
    copy_feature_properties((featureptr) srcshell, (featureptr) destshell);
  
  copy_feature_properties((featureptr) sourceobj, (featureptr) destobj);
}

  objptr
duplicate_obj(objptr thisobj)
{
  objptr newobj;
  shellptr thishell,newshell;
  leptr oldle,newle;
  evfptr thisevf;
  listptr src_lelist,dest_lelist;
  listptr src_fvelist,dest_fvelist;
  worldptr world;
  ;
  world = thisobj->motherworld;	/* duplicate to same world as original object */
  newobj = make_obj_noshell(world);

  /* question: why were the next two lists circular once upon a time? */
  /* if you know the answer you win the magic prize */
  src_lelist = create_list();
  dest_lelist = create_list();
  src_fvelist = create_list();
  dest_fvelist = create_list();

  recursion_depth = max_recursion_depth = 0;
  
  thishell = First_obj_shell(thisobj);
  while (thishell != Nil)
  {
    oldle = First_shell_le(thishell);
    if (thishell->vfelist->numelems < 2)
    {
      newshell = make_shell(oldle->facevert->pos);
      newle = First_shell_le(newshell);
      move_shell_to_obj(newobj,newshell);
    }
    else
    {
      recursion_depth = max_recursion_depth = 0;
      /* The clear loop is ABSOLUTELY necessary before duping.  I'm not going */
      /* to use add_property on each evf to mark as touched unless add_property*/
      /* starts using a flag for touched...otherwise dup will take forever. */
      thisevf = thishell->evflist->first.evf;
      while (thisevf != Nil)
      {
	thisevf->touched = FALSE;
	thisevf = thisevf->next;
      }
      newshell = make_shell(oldle->facevert->pos);
      newle = First_shell_le(newshell);
      move_shell_to_obj(newobj,newshell);
      duplicate_engine(oldle,newle,src_lelist,dest_lelist,
		       src_fvelist,dest_fvelist); /* do the dirty deed! */
    }
    thishell = thishell->next;
  }
  

  duplicate_properties(thisobj,newobj,
		       src_lelist, dest_lelist, src_fvelist, dest_fvelist);

  del_list(src_lelist);
  del_list(dest_lelist);
  del_list(src_fvelist);
  del_list(dest_fvelist);

  copycolor(thisobj->thecolor,newobj->thecolor);
  copy_matrix(thisobj->xform,newobj->xform);
  copy_matrix(thisobj->invxform,newobj->invxform);
  transform_obj_reverse(newobj,newobj->invxform);
  newobj->selectechnique = thisobj->selectechnique;
  newobj->drawtechnique = thisobj->drawtechnique;
  newobj->screenbboxtechnique = thisobj->screenbboxtechnique;

  return(newobj);
}

  objptr
duplicate_obj_branch_engine(objptr thisobj,objptr newparent)
{
  objptr newobj;
  objptr thischild;
  Boolean linkstatus;
  ;
  newobj = duplicate_obj(thisobj);
  linkstatus = thisobj->linked;
  adopt_obj_branch(newparent,newobj,linkstatus); 
  thischild = thisobj->children->first.obj;
  while (thischild != Nil)
  {
    duplicate_obj_branch_engine(thischild,newobj);
    thischild = thischild->next;
  }
  return(newobj);
}

  objptr
duplicate_obj_branch(objptr thisobj)
{
  objptr newbranch;
  ;
  newbranch = duplicate_obj_branch_engine(thisobj,thisobj->motherworld->root);
  /* ignore whatever the duplicated branch had as its topmost linked flag */
  /* and instead set it to TRUE since we make the duplicate branch a child */
  /* of the root. A hack from LJE, but it works ok i guess. */
  newbranch->linked = TRUE;
  return(newbranch);
}
    

  objptr
duplicate_and_translate_obj(objptr thisobj, vertype offsetvert)
{
  objptr newobj;
  ;
  newobj = duplicate_obj_branch(thisobj);
  translate_feature((featureptr) newobj,offsetvert,FALSE);

  return(newobj);
}
