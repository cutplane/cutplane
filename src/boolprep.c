
/* FILE: boolprep.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*     Solid Object Boolean Operator Preprocess Routines for Cutplane     */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/* Authors: WAK,LJE                              Date: January 15,1990    */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define BOOLPREPMODULE

#include <config.h>
#include <math.h>

#include <platform_gl.h>

#include <cutplane.h>

#include <globals.h>
#include <math3d.h>
#include <boolean.h>

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                         UTILITY ROUTINES                               */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


/* Sometimes an edge to split in shell B has already been split by other */
/* edges of A.  If this happens, find the correct split edge by scanning */
/* forwards and backwards from the edge you had recorded as the edge to */
/* split until you find an edge that actually contains the splitting point. */

  leptr
find_correct_splitedge(evfptr oldsplitedge,vertype breakpos)
{
  leptr forwardle,backle,oldsplitle;
  vertype nearpt;		/* dummy here since we know breakpos */
  evfptr splitedge;
  Boolean searchforwards = TRUE,searchbackwards = TRUE;
  double dummyt;
  ;
  /* change this call to be pt_near_lineseg_3d and make sure *that* routine */
  /* doesn't let t+- tol's to slip through anymore, ok? */
  oldsplitle = oldsplitedge->le1;
  if (pt_near_lineseg_3d(oldsplitle->facevert->pos,
			 oldsplitle->next->facevert->pos,breakpos,nearpt,
			 &dummyt,Ptonlinetol))
      return(oldsplitle);
#ifdef debug
  printf("\t\t*** Doing find_correct_splitedge!\n\n");
#endif
  forwardle = oldsplitle->next;
  backle = oldsplitle->prev;
  while (searchforwards || searchbackwards)
  {
    if (searchforwards)
    {
      if (pt_near_lineseg_3d(forwardle->facevert->pos,
			     Twin_le(forwardle)->facevert->pos,
			     breakpos,nearpt,&dummyt,Ptonlinetol))
	return(forwardle);
      else
      {
	forwardle = forwardle->next;
	/* make sure you didn't branch... if you did you're no longer on */
	/* the broken up edge. */
	if ((forwardle == oldsplitle) || 
	    ((Twin_le(Twin_le(forwardle->prev)->prev)) != forwardle))
	  searchforwards = FALSE;
      }
    }
    if (searchbackwards)
    {
      if (pt_near_lineseg_3d(backle->facevert->pos,
			     Twin_le(backle)->facevert->pos,
			     breakpos,nearpt,&dummyt,Ptonlinetol))
	return(backle);
      else
      {
	backle = backle->prev;
	/* make sure you didn't branch... if you did you're no longer on */
	/* the broken up edge. */
	if ((backle == oldsplitle) || 
	    ((Twin_le(Twin_le(backle->next)->next)) != backle))
	  searchbackwards = FALSE;
      }
    }
  }
  system_error("find_correct_splitedge: break edge search failure!!\n");
  return 0; 			// We'll never get here... -- LJE
}

  void
log_pure_vv(listptr vv_list,vfeptr vfeA,vfeptr vfeB)
{
  vvptr newvv;
  ;
  newvv = (vvptr) append_new_blank_elem(vv_list,Vv_type);
  newvv->vfeA = vfeA;
  newvv->vfeB = vfeB;
}

  void
log_vf(listptr vf_list,vfeptr touchvfe,fveptr touchfve,int solidflag)
{
  vfptr newvf;
  ;
  newvf = (vfptr) append_new_blank_elem(vf_list,Vf_type);
  newvf->touchingelem[solidflag] = (elemptr) touchvfe;
  newvf->touchingelem[1-solidflag] = (elemptr) touchfve;
}

  void
log_pure_vvs(shellptr shellA,shellptr shellB,listptr vv_list)
{			/* find and record all vertex/vertex intersections */
  vfeptr vfeA,vfeB;
  vvptr newvv;
  ;
  vfeA = shellA->vfelist->first.vfe;
  while (vfeA != Nil)
  {
    vfeB = shellB->vfelist->first.vfe;
    while (vfeB != Nil)
    {
      if (pt_near_pt_3d(vfeA->pos,vfeB->pos,Pttopttol))
      {
	log_pure_vv(vv_list,vfeA,vfeB);
      }
      vfeB = vfeB->next;
    }
    vfeA = vfeA->next;
  }
}

  void
log_vee(evfptr evfA,evfptr evfB,vertype breakpos,int onptcode,listptr vee_list)
{
  veeptr thisvee,newvee;
  ;
  if (onptcode != Evfsintersect)
  {		/* only do check for multiple entries in ve cases */
    thisvee = vee_list->first.vee;
    while (thisvee != Nil)
    {
      if (((thisvee->onptcode == EvfApt1on) || (thisvee->onptcode == EvfApt2on))
	  && (evfB == thisvee->evfB))
	return;			/* already logged */	
      else if (((thisvee->onptcode == EvfBpt1on) ||
		(thisvee->onptcode == EvfBpt2on)) && (evfA == thisvee->evfA))
	return;			/* already logged */	
      thisvee = thisvee->next;
    }
  }
  newvee = (veeptr) append_new_blank_elem(vee_list,Vee_type);
  newvee->evfA = evfA;
  newvee->evfB = evfB;
  copypos3d(breakpos,newvee->breakpos);
  newvee->onptcode = onptcode;
}

  void
log_vertex_edges(shellptr shellA,shellptr shellB,listptr vee_list)
{		/* find and record all vertex/edge & edge/edge intersections */
  evfptr evfA,evfB;
  Coord *pt1,*pt2,*pt3,*pt4;
  double t,t2;
  vertype intrpt,nearpt;
  Boolean veelogged;
  ;
  evfA = shellA->evflist->first.evf;
  while (evfA != Nil)
  {
    pt1 = evfA->le1->facevert->pos;
    pt2 = evfA->le2->facevert->pos;
    evfB = shellB->evflist->first.evf;
    while (evfB != Nil)
    {
      veelogged = FALSE;
      pt3 = evfB->le1->facevert->pos;
      pt4 = evfB->le2->facevert->pos;

      if (veelogged |= pt_near_lineseg_3d(pt1,pt2,pt3,nearpt,&t,Ptonlinetol))
        log_vee(evfA,evfB,nearpt,EvfBpt1on,vee_list); /* log ve for pt1 */

      if (veelogged |= pt_near_lineseg_3d(pt1,pt2,pt4,nearpt,&t,Ptonlinetol))
        log_vee(evfA,evfB,nearpt,EvfBpt2on,vee_list); /* log ve for pt2 */

      if (veelogged |= pt_near_lineseg_3d(pt3,pt4,pt1,nearpt,&t,Ptonlinetol))
        log_vee(evfA,evfB,nearpt,EvfApt1on,vee_list); /* log ve for pt3 */

      if (veelogged |= pt_near_lineseg_3d(pt3,pt4,pt2,nearpt,&t,Ptonlinetol))
        log_vee(evfA,evfB,nearpt,EvfApt2on,vee_list); /* log ve for pt4 */

      if (!veelogged)
	if (linesegs_intersect_3d(pt1,pt2,pt3,pt4,intrpt,&t,&t2,
				  Linetolinetol) == (int) Linesegsintersect)
	  log_vee(evfA,evfB,intrpt,Evfsintersect,vee_list);
      evfB = evfB->next;
    }
    evfA = evfA->next;
  }
}

  void
compute_breakpos(evfptr thisevf,double t,vertype breakpos)
{
  vertype fgh;
  Coord *pt1,*pt2;
  ;
  pt1 = thisevf->le1->facevert->pos;
  pt2 = thisevf->le2->facevert->pos;
  diffpos3d(pt1,pt2,fgh);
  breakpos[vx] = pt1[vx] + t * fgh[vx];
  breakpos[vy] = pt1[vy] + t * fgh[vy];
  breakpos[vz] = pt1[vz] + t * fgh[vz];
  breakpos[vw] = 1.0;
}

  void
split_from_veelist(shellptr shellA,shellptr shellB,listptr vee_list,listptr vv_list)
{
  veeptr thisvee,nextvee;
  evfptr evfA,evfB;
  leptr newle,newle1,newle2,correctle,correctle1,correctle2;
  ;
  thisvee = vee_list->first.vee;
  while (thisvee != Nil)
  {
    nextvee = thisvee->next;
    evfA = thisvee->evfA;
    evfB = thisvee->evfB;
    switch (thisvee->onptcode)
    {
    case EvfApt1on:
      correctle = find_correct_splitedge(thisvee->evfB,thisvee->breakpos);
      newle = break_edge_at_pos(correctle,thisvee->breakpos);
      log_pure_vv(vv_list,thisvee->evfA->le1->facevert,newle->facevert);
      break;
    case EvfApt2on:
      correctle = find_correct_splitedge(thisvee->evfB,thisvee->breakpos);
      newle = break_edge_at_pos(correctle,thisvee->breakpos);
      log_pure_vv(vv_list,thisvee->evfA->le2->facevert,newle->facevert);
      break;
    case EvfBpt1on:
      correctle = find_correct_splitedge(thisvee->evfA,thisvee->breakpos);
      newle = break_edge_at_pos(correctle,thisvee->breakpos);
      log_pure_vv(vv_list,newle->facevert,thisvee->evfB->le1->facevert);
      break;
    case EvfBpt2on:
      correctle = find_correct_splitedge(thisvee->evfA,thisvee->breakpos);
      newle = break_edge_at_pos(correctle,thisvee->breakpos);
      log_pure_vv(vv_list,newle->facevert,thisvee->evfB->le1->facevert);
      break;
    case Evfsintersect:
      correctle1 = find_correct_splitedge(thisvee->evfA,thisvee->breakpos);
      correctle2 = find_correct_splitedge(thisvee->evfB,thisvee->breakpos);
      newle1 = break_edge_at_pos(correctle1,thisvee->breakpos);
      newle2 = break_edge_at_pos(correctle2,thisvee->breakpos);
      log_pure_vv(vv_list,newle1->facevert,newle2->facevert);
      break;
    }
    del_elem(vee_list,(elemptr) thisvee);
    thisvee = nextvee;
  }
}

  void
log_edge_faces(shellptr shell1,shellptr shell2,listptr vf_list,listptr ef_list,
               int solidflag)
{		/* find and record all vertex/face & edge/face intersections */
  evfptr thisevf;
  fveptr thisfve;
  int intresult;
  Coord *pt1,*pt2;
  double t;
  vertype intrpt;
  leptr dummyle;
  vfptr newvf;
  efptr newef;
  ;
  thisevf = shell1->evflist->first.evf;
  while (thisevf != Nil)
  {
    pt1 = thisevf->le1->facevert->pos;
    pt2 = thisevf->le2->facevert->pos;
    thisfve = shell2->fvelist->first.fve;
    while (thisfve != Nil)
    {
      intresult = lineseg_int_face_3d(pt1,pt2,thisfve,intrpt,&t,Linetolinetol);
      /* should record intrpt instead to avoid numerical difficulties? */
      /* also seems like you should check for multiple counting of endpts */
      if (intresult == (int) Linesegpt1inface) /* log vf_list record */
	log_vf(vf_list,thisevf->le1->facevert,thisfve,solidflag);
      else if (intresult == (int) Linesegpt2inface) /* log vf_list record */
	log_vf(vf_list,thisevf->le2->facevert,thisfve,solidflag);
      else if (intresult == (int) Linesegintersectsface)
      {
        newef = (efptr) append_new_blank_elem(ef_list,Ef_type);
        newef->breakevf = thisevf;
        newef->breakfve = thisfve;
        copypos3d(intrpt,newef->breakpos);
	newef->evfinwhichsolid = solidflag;
      }
      thisfve = thisfve->next;
    }
    thisevf = thisevf->next;
  }
}

  void
resolve_edge_faces(shellptr shellA,shellptr shellB,listptr vf_list,listptr ef_list)
{		/* logs final entry in vf list */
  efptr thisef,nextef;
  vertype breakpos;
  leptr breakle,newle;
  ;
  /* assumes ef_list has been created */
  log_edge_faces(shellA,shellB,vf_list,ef_list,SolidA);
  log_edge_faces(shellB,shellA,vf_list,ef_list,SolidB);

  thisef = ef_list->first.ef;
  while (thisef != Nil)
  {
    nextef = thisef->next;
    /* for edge/face case make sure you'r splitting the correct edge */
    breakle = find_correct_splitedge(thisef->breakevf,thisef->breakpos);
    newle = break_edge_at_pos(breakle,thisef->breakpos);
    log_vf(vf_list,newle->facevert,thisef->breakfve,thisef->evfinwhichsolid);
    del_elem(ef_list,(elemptr) thisef);
    thisef = nextef;
  }
}

  void
boolean_preprocess(shellptr shellA, shellptr shellB,
		   listptr vv_list,listptr vf_list)
{
  listptr vee_list,ef_list;
  ;
  vee_list = create_list();
  ef_list = create_list();

  log_pure_vvs(shellA,shellB,vv_list);
  log_vertex_edges(shellA,shellB,vee_list);
  split_from_veelist(shellA,shellB,vee_list,vv_list);
  resolve_edge_faces(shellA,shellB,vf_list,ef_list);

#ifdef debug
  topology_check(shellA);
  topology_check(shellB);
  malloc_check();
#endif
}
