
/* FILE: pencilsketch.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                                                                         */
/*           Pencil Sketching Algorithm for the Cutplane Editor:           */
/*           Determination of Faces from User-Inputted Edge Sets,          */
/*           Using Godly Data Structures & Algorithms                      */
/*                                                                         */
/* Author:  LJE & WAK                            Date: October 4,1989      */
/*                                                     Late at night       */
/*                                                                         */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland   */
/* Stanford Junior University, and William Kessler and Larry Edwards.      */
/* All Rights Reserved.                                                    */
/*                                                                         */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#define PENCILSKETCHMODULE

#include <config.h>
#include <stdio.h>
#include <math.h>

#include <platform_gl.h>

#include <cutplane.h>

#include <pencilsketch.h>
#include <grafx.h>
#include <math3d.h>
#include <pick.h>		/* for Pkboxsize */

#include <controls.h>
#include <update.h>

#define Lbutton_down(s) (((s)->mousebuttonsdown) & Lbuttonbit)
#define Mbutton_down(s) (((s)->mousebuttonsdown) & Mbuttonbit)
#define Rbutton_down(s) (((s)->mousebuttonsdown) & Rbuttonbit)

#define Lbutton_up(s) (!(((s)->mousebuttonsdown) & Lbuttonbit))
#define Mbutton_up(s) (!(((s)->mousebuttonsdown) & Mbuttonbit))
#define Rbutton_up(s) (!(((s)->mousebuttonsdown) & Rbuttonbit))

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                      SKETCH GLOBAL VARIABLES                           */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

static listptr sketchvfelist,sketchevflist;
static listptr drawshellist;

static GLboolean started_sketch;
static GLboolean entered_sketch_mode = FALSE,entered_newsketch_mode = FALSE;

/* Used by rubber_band, and input_line */
static GLboolean rbanding = FALSE;
static vertype oldrawpt;

/* used by draw_sketch_box and input_line */
static vfeptr newdrawvfe = Nil;
static evfptr nearedge = Nil;
static evfptr newdrawevf;
static GLboolean candoubleclick = FALSE;
static vertype newdrawpt,nearvert;


 double
atan_rectify(double satan)
{
  if (satan < 0.0)
    return(satan + 2*Pi);
  return(satan);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                      SEGMENT SORTING ROUTINES                          */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* returns TRUE if line segs intersect. Return intersection point in intrpt */

  static GLboolean
intersect_segments(vertype k,vertype l,vertype m,vertype n,float toldist,
		   vertype intrpt,int *endptintr)  /* XY PLANE ONLY! */
{
  GLfloat xlk,ylk,xnm,ynm,xmk,ymk,det,detinv,s,t;
  GLfloat t_extender;		/* add Pkboxsize diagonal to endpts of line 2 */
  GLfloat line2length;
  GLboolean knear,lnear,mnear,nnear;
  ;
  *endptintr = 0;		/* Initialize in case no intersection found. */
  xlk = l[vx] - k[vx];
  ylk = l[vy] - k[vy];
  xnm = n[vx] - m[vx];
  ynm = n[vy] - m[vy];
  xmk = m[vx] - k[vx];
  ymk = m[vy] - k[vy];
  det = xnm*ylk - ynm*xlk;
  if (fabs(det) < Accy)
    return(FALSE);
  else 
  {
    detinv = 1.0 / det;
    s = (xnm*ymk - ynm*xmk)*detinv;
    t = (xlk*ymk - ylk*xmk)*detinv;
    line2length = distance(m,n);
    t_extender = (Pkboxsize * 1.414) / line2length;
    if ((s < 0.0) || (s > 1.0) ||
	(t < (0.0 - t_extender)) || (t > (1.0 + t_extender)))
      return(FALSE);
    else
    {
      intrpt[vx] = k[vx] + xlk*s;
      intrpt[vy] = k[vy] + ylk*s;
      knear = pt_near_pt(k,intrpt,toldist);
      lnear = pt_near_pt(l,intrpt,toldist);
      mnear = pt_near_pt(m,intrpt,toldist);
      nnear = pt_near_pt(n,intrpt,toldist);
      *endptintr = 0 + /* (knear * Line1End1) + (lnear * Line1End2) + */
	(mnear * Line2End1) + (nnear * Line2End2);
      if (*endptintr == 0)	/* if no endpoint intersection, but there WAS */
	*endptintr = Center_intersect; /* an intersection, note it.*/
      return(TRUE);
    }
  }
}

  static int
pick_axis(vertype drawpt1,vertype drawpt2)
{
  /* this works because vx = 0 & vy = 1! */
  return (fabs(drawpt1[vx] - drawpt2[vx]) < fabs(drawpt1[vy] - drawpt2[vy]));
}


  static sortptr
sort_insert(listptr sortedlist,evfptr thisketch,float distance,
	    vertype intersectpt,int intresult)
{
  sortptr thisort,newsort;
  float *pos;
  ;
  newsort = (sortptr) create_blank_elem(Sort_type);
  newsort->sortelem = (elemptr) thisketch;

  newsort->sortdata1 = (alltypes *) alloc_elem(3 * Allsize);
  newsort->sortdata2 = (alltypes *) alloc_elem(Allsize);
  newsort->sortdata3 = (alltypes *) alloc_elem(Allsize);

  (newsort->sortdata1)[vx].f = intersectpt[vx];
  (newsort->sortdata1)[vy].f = intersectpt[vy];
  (newsort->sortdata1)[vz].f = 0.0; /* filled in in another routine */
  newsort->sortdata2->f = distance;
  newsort->sortdata3->i = intresult;

  thisort = sortedlist->first.sort;
  while ((thisort != Nil) && (newsort->sortdata2->f > thisort->sortdata2->f))
    thisort = thisort->next;
  insert_elem((listptr) sortedlist,(elemptr) thisort,(elemptr) newsort);

  return(newsort);
}

  static void
sort_segs(listptr sketchevflist,vertype drawpt1,vertype drawpt2,
	  vfeptr vfept1,vfeptr vfept2,listptr seglist)
{
  evfptr thisketchevf;
  sortptr newsort;
  vfeptr endpt1vfe,endpt2vfe;
  vertype intersectpt;
  float *endpt1,*endpt2;
  float distance;
  int intresult,axis;
  GLboolean doit;
  ;
  axis = pick_axis(drawpt1,drawpt2);
  thisketchevf = sketchevflist->first.evf;
  while (thisketchevf)
  {
    endpt1vfe = thisketchevf->pt1vfe;
    endpt2vfe = thisketchevf->pt2vfe;
    doit = TRUE;
    /* if the endpts of this seg are the same as vfept1 or vfept2 do nothin */
    /* but, if vfept1 and vfept2 are both nil we must always doit */
    if (vfept1 != Nil)
      doit = (vfept1 != endpt1vfe) && (vfept1 != endpt2vfe);
    if (doit && (vfept2 != Nil))
      doit = (vfept2 != endpt1vfe) && (vfept2 != endpt2vfe);
    if (doit)
    {
      endpt1 = endpt1vfe->pos;
      endpt2 = endpt2vfe->pos;
      if (intersect_segments(drawpt1,drawpt2,endpt1,endpt2,
			     Pkboxsize,/*was Tolerance,*/
			     intersectpt,&intresult))
      {
	distance = fabs(drawpt1[axis] - intersectpt[axis]);
	
	newsort = sort_insert(seglist,thisketchevf,distance,
			      intersectpt,intresult);
	(newsort->sortdata1)[vz].f = drawpt1[vz];
      }
    }
    thisketchevf = thisketchevf->next;
  }
}



/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                      OBJECT MANIPULATION ROUTINES                      */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


  vfeptr
other_vfe(evfptr thisevf,vfeptr thisvfe)
{
  if (thisevf->pt1vfe == thisvfe)
    return(thisevf->pt2vfe);
  return(thisevf->pt1vfe);
}

  elemptrptr			/* find the next edge ccw from the edge */
find_ccwedge(vfeptr connectvfe,/* passed */
	     evfptr anchorevf)
{
  evfptr checkevf;
  elemptrptr maxelemptrptr = Nil;
  double startangle,checkangle,diffangle = 0.0,prevangle,maxangle = 0;
  vertype vect1,vect2,outvect;
  vfeptr anchorendvfe,checkendvfe;
  elemptrptr check_lelemptr;
  ;
  check_lelemptr = connectvfe->edgelist->first.ep;
  if (check_lelemptr == Nil)
    return(Nil);		/* no edges attached to this vfe */
  anchorendvfe = other_vfe(anchorevf,connectvfe);
  diffpos3d(connectvfe->pos,anchorendvfe->pos,vect1);
  startangle = atan_rectify(atan2(vect1[vy],vect1[vx]));
  do
  {
    checkevf = (evfptr) check_lelemptr->thiselem;
    if (checkevf != anchorevf)
    {
      checkendvfe = other_vfe(checkevf,connectvfe);
      diffpos3d(connectvfe->pos,checkendvfe->pos,vect1);
      checkangle = atan_rectify(atan2(vect1[vy],vect1[vx]));
      diffangle = checkangle - startangle;
      if (diffangle < 0.0)
	diffangle += 2*Pi;
      if (diffangle > maxangle)
      {
	maxangle = diffangle;
	maxelemptrptr = check_lelemptr;
      }
    }
    check_lelemptr = check_lelemptr->next;
  } while (check_lelemptr != Nil);
  return(maxelemptrptr);
}				

  void
init_sle(sketchleptr newsle,evfptr newevf,vfeptr thesketchvert)
{
  newsle->sketchevf = newevf;
  newsle->sketchvert = thesketchvert;
  newsle->prev = newsle->next = Nil;
}

  static void
init_sides(evfptr newevf)
{
  newevf->traversed[Left] = newevf->traversed[Right] = FALSE;
  newevf->leftsle_c = (sketchleptr) create_blank_elem(Sketchle_type);
  init_sle(newevf->leftsle_c,newevf,newevf->pt2vfe);
  newevf->leftsle_g = (sketchleptr) create_blank_elem(Sketchle_type);
  init_sle(newevf->leftsle_g,newevf,newevf->pt1vfe);
  newevf->rightsle_c = (sketchleptr) create_blank_elem(Sketchle_type);
  init_sle(newevf->rightsle_c,newevf,newevf->pt2vfe);
  newevf->rightsle_g = (sketchleptr) create_blank_elem(Sketchle_type);
  init_sle(newevf->rightsle_g,newevf,newevf->pt1vfe);
}

  vfeptr
new_sketchvfe(listptr sketchvfelist,vertype vertpt,evfptr newevf,shellptr newshell)
{
  vfeptr newone;
  elemptrptr new_edgeelemptr;
  ;
  newone = new_vfe(vertpt,newshell);
  newone->edgelist = create_list();
  return(newone);
}

  void
rem_edge_from_vfe(vfeptr thisvfe,evfptr thisevf)
{
  elemptrptr thisvfe_edge;
  ;
  thisvfe_edge = thisvfe->edgelist->first.ep;
  while (thisvfe_edge != Nil)
  {
    if ((evfptr) thisvfe_edge->thiselem == thisevf)
    {
      del_elem((listptr) thisvfe->edgelist,(elemptr) thisvfe_edge);
      return;
    }
    thisvfe_edge = thisvfe_edge->next;
  }				/* does nothing if the edge not found */
}				/* in the vfe's edge list */

  void
add_edge_to_vfe(vfeptr thisvfe,evfptr thisevf)
{
  elemptrptr prev_lelem,new_vfe_edge;
  ;
  prev_lelem = find_ccwedge(thisvfe,thisevf);
  new_vfe_edge = (elemptrptr) add_new_blank_elem((listptr) thisvfe->edgelist,
				     (elemptr) prev_lelem,Ep_type);
  new_vfe_edge->thiselem = (elemptr) thisevf;
}

  evfptr
make_edge(listptr sketchvfelist,listptr sketchevflist,
		vertype drawpt1,vertype drawpt2,
		vfeptr drawvfe1,vfeptr drawvfe2)
{				/* coded right now for no edge intersections */
  evfptr newevf;
  ;
  newevf = (evfptr) append_new_blank_elem(sketchevflist,Evf_type);
  if (drawvfe1)
    newevf->pt1vfe = drawvfe1;
  else
    newevf->pt1vfe = new_sketchvfe(sketchvfelist,drawpt1,newevf,Nil);
  if (drawvfe2)
    newevf->pt2vfe = drawvfe2;
  else
    newevf->pt2vfe = new_sketchvfe(sketchvfelist,drawpt2,newevf,Nil);
  add_edge_to_vfe(newevf->pt1vfe,newevf);
  add_edge_to_vfe(newevf->pt2vfe,newevf);
  init_sides(newevf);		/* initialize sle's */
  return(newevf);
}

  void
fix_sles(evfptr thisevf)
{
  /* reinitialize sle's after splitting an edge */
  init_sle(thisevf->leftsle_g,thisevf,thisevf->pt1vfe);
  init_sle(thisevf->leftsle_c,thisevf,thisevf->pt2vfe);
  init_sle(thisevf->rightsle_g,thisevf,thisevf->pt1vfe);
  init_sle(thisevf->rightsle_c,thisevf,thisevf->pt2vfe);
}

  evfptr
break_edge(evfptr thisevf,vfeptr oldbreakvfe,vertype breakpos,
	  listptr sketchvfelist,listptr sketchevflist)
{
  evfptr newevf;
  vertype bogusvert; /* these are placeholders for weird ccom bugginess */
  vfeptr bogusvfe = Nil; /* on Piris only */
  ;
  rem_edge_from_vfe(thisevf->pt2vfe,thisevf);
  /* oldbreakvfe exists if the break is right at an existing vfe, eg */
  /* when two edges cross over each other or one end touches the middle */
  /* of another. */
  if (oldbreakvfe != Nil)
    newevf = make_edge(sketchvfelist,sketchevflist,
		       bogusvert,bogusvert,oldbreakvfe,thisevf->pt2vfe);
  else
    newevf = make_edge(sketchvfelist,sketchevflist,
		       breakpos,bogusvert,bogusvfe,thisevf->pt2vfe);
  thisevf->pt2vfe = newevf->pt1vfe;
  fix_sles(thisevf);
  add_edge_to_vfe(thisevf->pt2vfe,thisevf);
  return(newevf);
}



  static void
make_edges(listptr sketchvfelist,listptr sketchevflist,
		vertype drawpt1,vertype drawpt2,
		vfeptr drawvfe1,vfeptr drawvfe2)
{
  listptr seglist;
  sortptr thisort;
  evfptr newevf,thisevf,thisevfpt2;
  vfeptr newvfe;
  vertype pos,bogusvert;
  ;
  /* LJE: Note: also deal with end cases, smallest angle stuff etc, here. */
  /* or maybe put in connect_verts */
  newevf = make_edge(sketchvfelist,sketchevflist,
		      drawpt1,drawpt2,drawvfe1,drawvfe2);
  seglist = create_list(); 
  sort_segs(sketchevflist,drawpt1,drawpt2,drawvfe1,drawvfe2,seglist);

  thisort = seglist->first.sort;
  while(thisort)
  {
    thisevf = (evfptr) thisort->sortelem;
    /* mev the seg verts here depending on intresult */
    switch(thisort->sortdata3->i)
    {
    case End1:		/* Endpoint one touches middle of drawn edge.*/
      /* need to break drawn edge and attach the two new edges to the old vfe*/
/*
      thisevf->pt1vfe->pos[vx] = (thisort->sortdata1)[0].f;
      thisevf->pt1vfe->pos[vy] = (thisort->sortdata1)[1].f;
      thisevf->pt1vfe->pos[vz] = (thisort->sortdata1)[2].f;
      newevf = break_edge(newevf,thisevf->pt1vfe,Nil,
			  sketchvfelist,sketchevflist);
*/
      break;
    case End2:		/* Endpoint two touches middle of drawn edge.*/
      /* need to break drawn edge and attach the two new edges to the old vfe*/
/*
      thisevf->pt2vfe->pos[vx] = (thisort->sortdata1)[0].f;
      thisevf->pt2vfe->pos[vy] = (thisort->sortdata1)[1].f;
      thisevf->pt2vfe->pos[vz] = (thisort->sortdata1)[2].f;
      newevf = break_edge(newevf,thisevf->pt2vfe,Nil,
			  sketchvfelist,sketchevflist);
*/
      break;
    case Center_intersect:	/* This edge intersects drawn edge. */
      pos[0] = (thisort->sortdata1)[0].f;
      pos[1] = (thisort->sortdata1)[1].f;
      pos[2] = (thisort->sortdata1)[2].f;
      thisevfpt2 = break_edge(thisevf,(vfeptr) Nil,pos,sketchvfelist,sketchevflist);
      newvfe = thisevfpt2->pt1vfe;
      newevf = break_edge(newevf,newvfe,bogusvert,sketchvfelist,sketchevflist);
      break;
    default:
      /* this should never happen */
      printf("AAAAAAAAAAARRRRRRRRRRGGGGGGGHHHHHHHHHH\n");
      fflush(stdout);
    }
    thisort = thisort->next;
  }
  clear_list(seglist);	/* what is it doing in xsect.c LJE? */
  free_elem((elemptr *) &seglist);
}

/*******************************************************************/
/*                                                                 */
/*                    Heart of KURTS (Kessler's Ultra Redundant    */
/*                    Technique for Sketching!)                    */
/*                                                                 */
/*******************************************************************/

/* This routine may be now inutile */
  static sketchleptr
Twin_sle(sketchleptr this_sle,int side)
{
  evfptr motherevf;
  ;
  motherevf = this_sle->sketchevf;
  if (side == Left)
  {
    if (motherevf->leftsle_c == this_sle)
      return(motherevf->leftsle_g);
    return(motherevf->leftsle_c);
  }
  else
  {
    if (motherevf->rightsle_c == this_sle)
      return(motherevf->rightsle_g);
    return(motherevf->rightsle_c);
  }
}

  static int
coming_or_going(evfptr checkevf,vfeptr anchorvfe)
{
  if (checkevf->pt1vfe == anchorvfe)
    return(Going);
  return(Coming);
}

  static void
connect_ccw(evfptr newevf,evfptr oldevf,vfeptr connectvfe)
{
  int newevfdir,oldevfdir;
  ;
  newevfdir = coming_or_going(newevf,connectvfe);
  oldevfdir = coming_or_going(oldevf,connectvfe);
  if ((newevfdir == Coming) && (oldevfdir == Coming))
  {				/* connect right side to left side */
    oldevf->leftsle_g->next = newevf->rightsle_c;
    newevf->rightsle_g->next = oldevf->leftsle_c;
    newevf->rightsle_c->prev = oldevf->leftsle_g;
    oldevf->leftsle_g->prev = newevf->rightsle_g;
  }
  else if ((newevfdir == Coming) && (oldevfdir == Going))
  {				/* connect right side to right side */
    oldevf->rightsle_c->next = newevf->rightsle_c;
    newevf->rightsle_g->next = oldevf->rightsle_g;
    newevf->rightsle_c->prev = oldevf->rightsle_c;
    oldevf->rightsle_g->prev = newevf->rightsle_g;
  }
  else if ((newevfdir == Going) && (oldevfdir == Coming))
  {				/* connect left side to left side */
    oldevf->leftsle_g->next = newevf->leftsle_g;
    newevf->leftsle_c->next = oldevf->leftsle_c;
    newevf->leftsle_g->prev = oldevf->leftsle_g;
    oldevf->leftsle_c->prev = newevf->leftsle_c;
  }
  else if ((newevfdir == Going) && (oldevfdir == Going))
  {				/* connect left side to right side */
    oldevf->rightsle_c->next = newevf->leftsle_g;
    newevf->leftsle_c->next = oldevf->rightsle_g;
    newevf->leftsle_g->prev = oldevf->rightsle_c;
    oldevf->rightsle_g->prev = newevf->leftsle_c;
  }
}

/* Crucial routine: this walks around a vfe clockwise, connecting sketchle's.*/
/* May redo some earlier connections, but that's ok, that inefficiency */
/* can be improved later. */

  static void
walk_vfe(vfeptr connectvfe)
{
  evfptr startccwevf,lastccwevf,nextccwevf;
  elemptrptr this_lelemptr;
  ;
  if (connectvfe->edgelist->numelems == 0)
    return;			/* dont do anything if a point by itself */

  this_lelemptr = connectvfe->edgelist->first.ep;
  startccwevf = lastccwevf = (evfptr) this_lelemptr->thiselem;
  if (connectvfe->edgelist->numelems == 1)
  {
    connect_ccw(startccwevf,startccwevf,connectvfe);
    return;
  }

  this_lelemptr = this_lelemptr->next;
  do
  {
    nextccwevf = (evfptr) this_lelemptr->thiselem;
    connect_ccw(lastccwevf,nextccwevf,connectvfe);
    lastccwevf = nextccwevf;
    this_lelemptr = this_lelemptr->next;
  } while (this_lelemptr != Nil);
  /* end case, include startevf's right side*/
  connect_ccw(nextccwevf,startccwevf,connectvfe);
}

  static void
walk_vfes(listptr sketchvfelist)
{
  vfeptr thisvfe;
  ;
  for (thisvfe = sketchvfelist->first.vfe; thisvfe != Nil;
       thisvfe = thisvfe->next)
    walk_vfe(thisvfe);
}    


  static int
proper_side(sketchleptr walksle,int givenside)
{
  if (coming_or_going(walksle->sketchevf,walksle->sketchvert) == Coming)
  {				/* if the edge is not Going, then we need */
    if (givenside == Left)	/* to reverse the side marked because */
      return(Right);		/* we assume we always start Going. */
    return(Left);
  }
  return(givenside);
}

  static void
break_off_strut(leptr breakle,listptr shell_list)
{
  leptr nulledge,prevnull,postnull,new_null_le;
  shellptr newshell;
  fveptr firstface;
  vfeptr breakptvfe;
  boundptr newring;
  leptr walkle;
  ;
/*
  printf ("here is the face:\n");
  walkle = breakle;
  do
  {
    printf ("[%f,%f,%f]\n",walkle->facevert->pos[vx],
	    walkle->facevert->pos[vy],walkle->facevert->pos[vz]);
    walkle = walkle->next;
  } while (walkle != breakle);
*/

  /* if need to break the strut off, make a new shell for it*/
  newshell = make_shell(breakle->facevert->pos);
  firstface = newshell->fvelist->first.fve;
  prevnull = breakle;
  postnull = Twin_le(breakle)->next;
  /* insert a null edge at the break point  */
  nulledge = make_edge_vert(prevnull,postnull,postnull->facevert->pos);
  /* make the strut part into a ring of the face being built */
  newring = kill_edge_make_ring(Twin_le(nulledge),nulledge);
  
  move_loop(newring,firstface,FALSE);
  new_null_le = First_shell_le(newshell);
  kill_ring_make_edge(new_null_le,
		      firstface->boundlist->first.bnd->next->lelist->first.le);
  kill_edge_vert(new_null_le,Twin_le(new_null_le));
}

  static GLboolean
on_a_strut(leptr thisle)
{
  if (thisle->motherbound == (Twin_le(thisle))->motherbound)
    return(TRUE);
  return(FALSE);
}
  

  static void
cut_struts(leptr startle,listptr shell_list)
{
  leptr thisle,othersidele,goodstartle;
  ;
  goodstartle = startle;
  while (on_a_strut(goodstartle))
  {
    goodstartle = goodstartle->next;
    if (on_a_strut(goodstartle) && (goodstartle == startle))
      return;		/* don't try to cut struts from an actual strut */
  }
  thisle = goodstartle;
  do
  {
    /* if next le is out on a strut, break off that strut before going on */
    while (on_a_strut(thisle->next))
      break_off_strut(thisle->next,shell_list);
    thisle = thisle->next;
  } while (thisle != goodstartle);
}

  static void
clear_touched(sketchleptr startsle)
{
  sketchleptr walksle;
  ;
  walksle = startsle;
  do
  {
    walksle->sketchvert->startle = Nil;
    walksle->sketchevf->touched = FALSE;
    walksle = walksle->next;
  }  while (walksle != startsle);
}

  static shellptr
create_loop(listptr shell_list,sketchleptr startsle,int side)
{
  sketchleptr walksle,cleanupsle;
  leptr firstle,walkle,closing_le;
  shellptr newshell,lastshell;
  int correctside;
  GLboolean walkle_is_a_strut = FALSE;
  ;
  clear_touched(startsle);
  walksle = startsle;
  /* ASSUMPTION: the startsle is going AWAY from the pt1vfe on this edge!  */
  newshell = make_shell(startsle->sketchevf->pt1vfe->pos);
  firstle = First_shell_le(newshell);
  walkle = firstle;
  correctside = proper_side(walksle,side);
  walksle->sketchevf->traversed[correctside] = TRUE;
  walksle->sketchevf->touched = TRUE;
  walksle->sketchvert->startle = firstle;
  walkle = make_edge_vert(walkle,walkle,
			  walksle->next->sketchvert->pos);
  walksle = walksle->next;
  walksle->sketchvert->startle = walkle;
  while (walksle != startsle)
  {
    if (!(walksle->sketchevf->touched))
    {
      walkle_is_a_strut = FALSE;
      if (walksle->next != startsle) /* don't mev to the first point though */
      {
	walksle->sketchevf->touched = TRUE;
	/* make sure to mef if going back over a previously mev'd vertex */
	closing_le = walksle->next->sketchvert->startle;
	if (closing_le != Nil)
	{
	  make_edge_face(walkle,closing_le);
	  walkle = closing_le;
	}
	else
	  walkle = make_edge_vert(walkle,walkle,
				  walksle->next->sketchvert->pos);
      }
    }
    else
    {
      walkle_is_a_strut = TRUE;
      walkle = walksle->next->sketchvert->startle; /* back up on les */
      /* used to be: walkle = walkle->next; but above code is more */
      /* comprehensible */      
    }
    correctside = proper_side(walksle,side);
    walksle->sketchevf->traversed[correctside] = TRUE;
    walksle = walksle->next;
    walksle->sketchvert->startle = walkle;
  }
  /* close it up and send it away */
  /* except if it was an only strut */
  if (!walkle_is_a_strut)
    make_edge_face(walkle,firstle);

/*
  printf ("face has %d vfes\n",newshell->vfelist->numelems);
  walksle = startsle;
  do
  {
    printf ("[%f,%f,%f]\n",walksle->sketchvert->pos[vx],
	    walksle->sketchvert->pos[vy],walksle->sketchvert->pos[vz]);
    walksle = walksle->next;
  }  while (walksle != startsle);

  printf ("According to le's its:\n");
  walkle = firstle;
  do
  {
    printf ("[%f,%f,%f]\n",walkle->facevert->pos[vx],
	    walkle->facevert->pos[vy],walkle->facevert->pos[vz]);
    walkle = walkle->next;
  }  while (walkle != firstle);
*/

  /* break off struts on the new shell and any shells broken off along */
  /* with the struts, added after the call to cut_struts */
  lastshell = newshell;
  while (lastshell != Nil)
  {
    firstle = First_shell_le(lastshell);
    cut_struts(firstle,shell_list);
    lastshell = lastshell->next;
  }

  return(newshell);
}

/*return the clockwise or ccw orientation of a face.  Won't work if the */
/* edges are not all in the plane.  works by finding the topmost two vectors */
/* (topmost in y), taking cross prod, if zvec points up, they are ccw. */

  static int
face_orient(shellptr orientface)
{
  leptr thisle,maxle,firstle,aftermax;
  vertype vect1,vect2,xprod;
  ;
  firstle = maxle = First_shell_le(orientface);
  thisle = firstle->next;
  while (thisle != firstle)
  {
    if (thisle->facevert->pos[vy] > maxle->facevert->pos[vy])
      maxle = thisle;
    thisle = thisle->next;
  }
  diffpos3d(maxle->prev->facevert->pos,maxle->facevert->pos,vect1);
  aftermax = maxle->next;
  /* skip past straightaway if there is one */
  while (aftermax->facevert->pos[vy] == maxle->facevert->pos[vy])
    aftermax = aftermax->next;
  diffpos3d(aftermax->prev->facevert->pos,aftermax->facevert->pos,vect2);
  cross_prod(vect1,vect2,xprod);
  if (xprod[vz] > 0.0)		/* face is going counterclockwise */
    return(Counterclockwise);
  else
    return(Clockwise);		/* otherwise always use left side */
}


  static void
find_innerloops(evfptr thisketchevf,listptr shell_list)
{
  shellptr newleftshell,newrightshell,nextnewleftshell,nextnewrightshell;
  leptr firstnewle;
  static vertype depth = {0.0,0.0,-100.0};
  ;
  if (!(thisketchevf->traversed[Left]))
  {
    newleftshell = create_loop(shell_list,thisketchevf->leftsle_g,Left);
    /* trash the created face and all spun off faces (from struts) */
    /* if they are clockwise oriented (not good because going left side) */
    while (newleftshell != Nil)
    {
      nextnewleftshell = newleftshell->next;
      extrude_face(newleftshell->fvelist->first.fve,depth);

      firstnewle = First_shell_le(newleftshell);
      if (firstnewle->motherbound != (Twin_le(firstnewle))->motherbound)
      {				/* don't try to delete a strut */
	if (face_orient(newleftshell) == Clockwise)
	  del_elem(shell_list,(elemptr) newleftshell);
      }
      newleftshell = nextnewleftshell;
    }
  }
  if (!(thisketchevf->traversed[Right]))
  {
    newrightshell = create_loop(shell_list,thisketchevf->rightsle_g,Right);
    /* trash the created face and all spun off faces (from struts) */
    /* if they are counter-clockwise oriented (bad because going right side) */
    while (newrightshell != Nil)
    {
      nextnewrightshell = newrightshell->next;
      extrude_face(newrightshell->fvelist->first.fve,depth);

      firstnewle = First_shell_le(newrightshell);
      if (firstnewle->motherbound != (Twin_le(firstnewle))->motherbound)
      {				/* don't try to delete a strut */
	if (face_orient(newrightshell) == Counterclockwise)
	  del_elem(shell_list,(elemptr) newrightshell);
      }
      newrightshell = nextnewrightshell;
    }
  }
}


  static void
find_faces(listptr sketchvfelist,listptr sketchevflist,listptr shell_list)
{
  evfptr thisketchevf;
  ;
  for (thisketchevf = sketchevflist->first.evf; thisketchevf != Nil;
       thisketchevf = thisketchevf->next)
    find_innerloops(thisketchevf,shell_list);
}


/*
Passed: the position of the cursor.
Returned:the position of the nearest pt to cursor.
         if near to an edge return the evf in *nearedge.
         if near to a vfe, returned as fcn value.
*/

  vfeptr
pt_near_sketch(vertype newdrawpt,vertype nearvert,
	       evfptr *nearedge,listptr sketchvfelist,
	       listptr sketchevflist)
{
  vfeptr thisvfe;
  evfptr thisevf;
  shellptr thishell;
  float *pos1,*pos2;
  vertype intrpt;
  ;
  for (thisvfe = sketchvfelist->first.vfe;
       thisvfe != Nil; thisvfe = thisvfe->next)
    {				/* search for a near vfe. */
      pos1 = thisvfe->pos;
      if (pt_near_pt(pos1,newdrawpt,Pkboxsize))
      {
	copypos3d(pos1,nearvert); /* not really necessary since we have vfeptr */
	return(thisvfe);
      }
    }		/* if no near vfe found, check for near edges */
    for (thisevf = sketchevflist->first.evf;
	 thisevf != Nil; thisevf = thisevf->next)
    {
      pos1 = thisevf->pt1vfe->pos;
      pos2 = thisevf->pt2vfe->pos;
      if (pt_near_line(pos1,pos2,newdrawpt,intrpt,Pkboxsize))
      {
	copypos3d(intrpt,nearvert);
	*nearedge = thisevf;
      }
    }
  return (Nil);
}

  void
draw_sketch_box(stateptr state, colortype thiscolor,GLboolean started_sketch)
{
  int boxfilled = Hollowpkbox;
  ;
  nearedge = Nil;
  candoubleclick = FALSE;
  newdrawvfe = Nil;
  copypos3d(state->cursor,newdrawpt);
  if (started_sketch)
  {
    if (button_pressed(Pickbutton))
      boxfilled = Filledpkbox;
    setcolor(thiscolor);
    /* extend point near pt & line to 3-d */
    if (rbanding && (pt_near_pt(oldrawpt,newdrawpt,Pkboxsize)))
    {
      /* to draw pick box near start of current r-banded line */
      candoubleclick = TRUE;
      draw_pick_box(oldrawpt,2,boxfilled);
    }
    else
    {
      /* extend point near pt & line to 3-d */
      newdrawvfe = pt_near_sketch(newdrawpt,nearvert,&nearedge,
				  sketchvfelist,sketchevflist);
      if ((newdrawvfe != Nil) || (nearedge != Nil))
	draw_pick_box(nearvert,2,boxfilled);
    }
  }
}

  void
rubber_band(stateptr state, colortype thiscolor)
{
  if (rbanding)
  {
    setcolor(thiscolor);
	/* OGLXXX glBegin: Use GL_LINES if only one line segment is desired. */
    glBegin(GL_LINE_STRIP); glVertex3f(oldrawpt[vx], oldrawpt[vy], oldrawpt[vz]);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
    glVertex3f(state->cursor[vx], state->cursor[vy], state->cursor[vz]);
  }
}

#ifdef RECODEME

  static GLboolean
input_line(listptr sketchvfelist,listptr sketchevflist,
	   stateptr state,actionptr new_action,
	   vertype drawpt1,vertype drawpt2,
	   vfeptr *drawvfe1,vfeptr *drawvfe2,
	   GLboolean *started_sketch, colortype thiscolor)
{
  static vfeptr oldrawvfe = Nil;
  ;
  if (*started_sketch)
    draw_sketch_box(state,thiscolor,*started_sketch);

  if (new_action->theaction == Pick_act)
  {				/* if user pressed the left button down */
    *started_sketch = TRUE;	/* will stay this way from now on. */
    if (rbanding)		/* if you are rubber banding */
    {
      if (candoubleclick)
      {
	rbanding = FALSE;	/* double clicked, stop rbanding. */
	return(FALSE);
      }
      else
      {			/* finished one segment. */
	if (nearedge != Nil)
	{
	  newdrawevf = break_edge(nearedge,Nil,nearvert,
				  sketchvfelist,sketchevflist);
	  newdrawvfe = newdrawevf->pt1vfe;
	  copypos3d(nearvert,newdrawpt);
	}
	if (oldrawvfe == Nil)
	  *drawvfe1 = pt_near_sketch(oldrawpt,nearvert,&nearedge,
				   sketchvfelist,sketchevflist);
	else
	  *drawvfe1 = oldrawvfe;
	copypos3d(oldrawpt,drawpt1);
	*drawvfe2 = newdrawvfe;
	copypos3d(newdrawpt,drawpt2);
	copypos3d(newdrawpt,oldrawpt);	/* update last down point. */
	if (*drawvfe2 != Nil)
	  rbanding = FALSE;	/* connected blobs, stop rbanding. */
	oldrawvfe = newdrawvfe;
	return(TRUE);
      }
    }
    else			/* very first point, store it and turn */
    {				/* on rubber banding. */
      if (nearedge != Nil)
      {
	newdrawevf = break_edge(nearedge,Nil,nearvert,
				sketchvfelist,sketchevflist);
	newdrawvfe = newdrawevf->pt1vfe;
	copypos3d(nearvert,newdrawpt);
      }
      if (newdrawvfe != Nil)
	copypos3d(newdrawvfe->pos,oldrawpt);
      else
	copypos3d(newdrawpt,oldrawpt);
      oldrawvfe = newdrawvfe;
      rbanding = TRUE;
      return(FALSE);
    }
  }
  rubber_band(state,thiscolor);
  return(FALSE);
}

  void
draw_sketch_shells(listptr sketchevflist, colortype thiscolor)
{
  evfptr thisevf;
  vertype pt1,pt2;
  ;
  setcolor(thiscolor);
  glDepthFunc(GL_ALWAYS);
  zdrawstate = GL_ALWAYS;

  if (sketchevflist != Nil)
  {
    glLineWidth((GLfloat)(2));
	/* OGLXXX setlinestyle: Check list numbering. */
    if(0) {glCallList(0); glEnable(GL_LINE_STIPPLE);} else glDisable(GL_LINE_STIPPLE);
    /*printf ("#edges :%d\n",sketchevflist->numelems);*/
    for (thisevf = sketchevflist->first.evf; thisevf != Nil;
	 thisevf = thisevf->next)
    {
      copypos3d(thisevf->pt1vfe->pos,pt1);
      copypos3d(thisevf->pt2vfe->pos,pt2);
	/* OGLXXX glBegin: Use GL_LINES if only one line segment is desired. */
      glBegin(GL_LINE_STRIP); glVertex3f(pt1[vx], pt1[vy], pt1[vz]);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
      glVertex3f(pt2[vx], pt2[vy], pt2[vz]);
    }
  }
}


  void
enter_sketch_mode(void)
{
  sketchvfelist = create_list();
  sketchevflist = create_list();
  started_sketch = FALSE;
  entered_sketch_mode = TRUE;
  candoubleclick = FALSE;
  rbanding = FALSE;
}

  GLboolean
sketch_mode_on(void)
{
  return(entered_sketch_mode);
}

  void
exit_sketch_mode(listptr shell_list)
{
  /* find all sketched faces and struts and create new shells for them  */
  walk_vfes(sketchvfelist);
  find_faces(sketchvfelist,sketchevflist,shell_list);
  clear_list((listptr) sketchvfelist);
  free_elem((elemptr *) &sketchvfelist);
  clear_list((listptr) sketchevflist);
  free_elem((elemptr *) &sketchevflist);
  started_sketch = FALSE;
  entered_sketch_mode = FALSE;
}

  void
sketch_action(stateptr state,actionptr new_action)
{
  static GLboolean line_drawn = FALSE;
  vertype drawpt1,drawpt2;
  vfeptr drawvfe1 = Nil,drawvfe2 = Nil;
  ;
  /*  printf ("  sketchaction now!\n");*/
  if (!sketch_mode_on())
    enter_sketch_mode();
  
  glDepthFunc(GL_ALWAYS);
  zdrawstate = GL_ALWAYS;

  /* see if user has completed a line */
  line_drawn = input_line(sketchvfelist,sketchevflist,
			  state,new_action,drawpt1,drawpt2,
			  &drawvfe1,&drawvfe2,&started_sketch,yellow);
  if (line_drawn)
    make_edges(sketchvfelist,sketchevflist,
	       drawpt1,drawpt2,drawvfe1,drawvfe2);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                 LJE's quick and dirty pencil sketch code                  */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

  void
make_moveto_point(listptr drawshellist, leptr *currentle, vertype newpt,
		  elemptr nearelem, vertype nearestpt)
{
  shellptr newshell;
  ;
  if (nearelem)
  {
    if (nearelem->type_id == Vfe_type)
      *currentle = ((vfeptr) nearelem)->startle;
    else if (nearelem->type_id == Evf_type)
      *currentle = make_edge_vert(((evfptr) nearelem)->le1,
				  ((evfptr) nearelem)->le2->next,newpt);
    else
      system_error("make_moveto_point: bad type id for nearelem");
  }
  else
  {
    newshell = make_shell(newpt); /* used to use drawshellist */
    del_property((featureptr) newshell,
		 visible_prop); /* not Visible until ready! hackedywak */
    *currentle = newshell->fvelist->first.fve->boundlist->first.bnd->
                         lelist->first.le;
  }
}


  void
make_drawto_point(listptr drawshellist, leptr *currentle, vertype newpt,
		  elemptr nearelem, vertype nearestpt)
{
  leptr drawtole;
  ;
  if (nearelem)
  {
    if (nearelem->type_id == Vfe_type)
      drawtole = ((vfeptr) nearelem)->startle;
    else if (nearelem->type_id == Evf_type)
      drawtole = make_edge_vert(((evfptr) nearelem)->le1,
				((evfptr) nearelem)->le2->next,newpt);
    else
      system_error("make_drawto_point: bad type id for nearelem");
    if (find_les_same_face(currentle,&drawtole))
    {
      if (drawtole->motherbound == (*currentle)->motherbound)
	make_edge_face(*currentle,drawtole);
      else if (drawtole->motherbound->motherfve ==
	       (*currentle)->motherbound->motherfve)
	kill_ring_make_edge(*currentle,drawtole);
    }
    else
      system_warning("make_drawto_point: les belong to different faces");
    
    *currentle = drawtole;
  }
  else
    *currentle = make_edge_vert(*currentle,*currentle,newpt);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                         USER I/O ROUTINES                              */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  static int
input_point(listptr drawshellist, stateptr state, actionptr new_action,
	    vertype newpt, elemptr *nearelem, vertype nearestpt,
	    colortype thiscolor)
{
  static vertype lastpt;	/* just used for whatever rbanding is done */
  static GLboolean drawing = FALSE;
  GLboolean lbutton_went_down,lbutton_went_up,near_something,moved_enough;
  GLboolean newline = FALSE, boxfilled = FALSE;
  ;
  newpt[vx] = state->cursor[vx]; /* new point we might draw to */
  newpt[vy] = state->cursor[vy];
  newpt[vz] = state->cursor[vz];

  lbutton_went_down = (new_action->theaction == Pick_act);
  lbutton_went_up = (new_action->theaction == Release_act);

  /* if new point is near an existing pt or edge, we connect to that */
  near_something = pt_near_edge(newpt,nearelem,nearestpt,drawshellist);
  if (lbutton_went_down)
  {
    copypos3d(newpt,lastpt);	/* so rubberbanding starts in the right place*/
    drawing = TRUE;

    return (Movetopoint);
  }
  /* lets see how far we've moved since we were last here */
  moved_enough = !pt_near_pt(lastpt,newpt,Pkboxsize);
  if (near_something && moved_enough)
    copypos3d(nearestpt,newpt);
  /* give some feedback that new point is near a point or an edge */
  if (near_something)
  {
    setcolor(yellow);
    draw_pick_box(nearestpt,2,boxfilled);
  }
  if (lbutton_went_up)
  {
    drawing = FALSE;

    return (Autoclose);
  }
  else if (drawing && moved_enough)
  {				/* we are far enough away to make a new line */
    copypos3d(newpt,lastpt);	/* so rubberbanding starts in the right place*/

    return (Drawtopoint);
  }
  else if (drawing)		/* we rubberband if too close to the last pt */
  {
    setcolor(thiscolor);
	/* OGLXXX glBegin: Use GL_LINES if only one line segment is desired. */
    glBegin(GL_LINE_STRIP); glVertex3f(lastpt[vx], lastpt[vy], lastpt[vz]);
	/* OGLXXX Add glEnd() after these vertices,  before next glBegin() */
    glVertex3f(newpt[vx], newpt[vy], newpt[vz]);
  }
  return(Nonewpoint);
}

  GLboolean
newsketch_mode_on()
{
  return(entered_newsketch_mode);
}

  listptr
enter_newsketch_mode()
{
  entered_newsketch_mode = TRUE;
  return(create_list());
}

  void
exit_newsketch_mode(listptr shell_list)
{
  shellptr thishell,nextshell;
  fveptr thisfve;
  static vertype depth = {0.0,0.0,-100.0};
  ;
  thishell = drawshellist->first.shell;
  while (thishell != Nil)
  {
    nextshell = thishell->next;
    thisfve = thishell->fvelist->first.fve;
    extrude_face(thisfve,depth);
    rem_elem(drawshellist,(elemptr) thishell);
    append_elem(shell_list,(elemptr) thishell);
    /* because hack removal above */
    add_property((featureptr) thishell,visible_prop);
    thishell = nextshell;
  }
  del_list(drawshellist);
  entered_newsketch_mode = FALSE;
}

GLboolean
colinearpt(evfptr lastevf, vertype newpt)
{
  vertype linevec,newptvec,nearestpt_online;
  double t;
  float *pt1,*pt2;
  ;
  pt1 = lastevf->le1->facevert->pos;
  pt2 = lastevf->le2->facevert->pos;
  diffpos3d(pt1,pt2,linevec);
  diffpos3d(pt1,newpt,newptvec);
  normalize(linevec,linevec);

  t = linevec[vx]*newptvec[vx] + linevec[vy]*newptvec[vy] +
                                                     linevec[vz]*newptvec[vz];

  nearestpt_online[vx] = pt1[vx] + t * linevec[vx];
  nearestpt_online[vy] = pt1[vy] + t * linevec[vy];
  nearestpt_online[vz] = pt1[vz] + t * linevec[vz];

  return(distance(newpt,nearestpt_online) < 0.1);
}

  void
new_sketch_method(listptr shell_list, stateptr state, actionptr newaction)
{
  elemptr nearelem;
  vertype newpt,nearestpt;
  static leptr currentle = Nil,startgoddamle;
  int newpoint;
  shellptr thishell;
  ;
  if (!newsketch_mode_on())
    drawshellist = enter_newsketch_mode();
  /* see if user has input a new point */
  newpoint = input_point(drawshellist,state,newaction,
			 newpt,&nearelem,nearestpt,yellow);
  switch (newpoint)
  {
  case Movetopoint:
    make_moveto_point(drawshellist,&currentle,newpt,nearelem,nearestpt);
    startgoddamle = currentle;
    break;
  case Drawtopoint:
    if (currentle->facedge != Nil && colinearpt(currentle->facedge,newpt))
      setvfe(currentle->facevert,newpt,
	      currentle->motherbound->motherfve->mothershell->motherobj->
	      invxform);
    else
      make_drawto_point(drawshellist,&currentle,newpt,nearelem,nearestpt);
    break;
  case Autoclose:
    make_edge_face(currentle,startgoddamle);
    break;
  }
/*  printf ("newpoint is %d\n",newpoint);*/
}

/*========================================*/

  void
update_sketch_mode(listptr shell_list, colortype thiscolor,stateptr state)
{
  static actiontype bogus_action = {0,0,0,0,0,0,0,0,0};	/* some judithhackoo */
  ;
  if (sketch_mode_on())
  {
    draw_sketch_shells(sketchevflist,thiscolor);
    rubber_band(state,thiscolor);
    draw_sketch_box(state,thiscolor,started_sketch);
  }
  else if (newsketch_mode_on())
  {
    new_sketch_method(shell_list, state, &bogus_action);
    /* fix me */
/*    draw_wireframe_shells(drawshellist,thiscolor);*/
  }
}

#endif /* RECODEME */
