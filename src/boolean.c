 
/* FILE: boolean.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*     Solid Object Boolean Operator Routines for the Cutplane Editor     */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/* Authors: WAK,LJE                              Date: January 15,1990    */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define BOOLEANMODULE

#include <config.h>

#include <math.h>
#include <string.h>

#include <platform_gl.h>

#include <cutplane.h>

#include <globals.h>
#include <math3d.h>
#include <boolean.h>

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                         BOOLEAN GLOBALS                                */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

int intrcallcount = 0;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                         UTILITY ROUTINES                               */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

void
init_facepair_lists(facepairptr thisfacepair)
{
  thisfacepair->paired_null_lelist = create_list();
}

void
copy_facepair_elem(facepairptr sourcefacepair, facepairptr destfacepair)
{
  listptr paired_null_lelist;
  ;
  paired_null_lelist = destfacepair->paired_null_lelist;

  memmove((void *) destfacepair,(const void *) sourcefacepair,Facepairsize);
  copy_list(sourcefacepair->paired_null_lelist,paired_null_lelist);

  destfacepair->paired_null_lelist = paired_null_lelist;
}

void
delete_facepair_lists(facepairptr thisfacepair)
{
  del_list(thisfacepair->paired_null_lelist);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                  VERTEX NEIGHBORHOOD ORGANIZER ROUTINES                */
/* These routines create neighborhoods for each vertex of the             */
/* and store the neighborhoods in the lists neighborhoodA,neighborhoodB.  */
/* Each neighborhood consists of a lists of sectors surrounding the vertex*/
/* in the center of that neighborhood.  The sectors are first generated   */
/* from the le's attached to the vertex, but additional sectors are       */
/* inserted if a sector turns out the be > 180 degrees wide.              */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
 
/* Record the sector in a sectorlist. */
/* NOTE: the centerle lies ALONG ref1, but goes in the opposite */
/* direction (see create_sectorlist() below for details.) */

  void
make_sector(listptr sectorlist,leptr thisle,vertype sectornorm,
	    vertype ref1,vertype ref2)
{
  sectorptr newsector;
  ;
  newsector = (sectorptr) append_new_blank_elem(sectorlist,Sector_type);
  newsector->centerle = thisle;
  copypos3d(ref1,newsector->ref1);
  copypos3d(ref2,newsector->ref2);
  copypos3d(sectornorm,newsector->sectornorm);
  cross_prod(ref1,ref2,newsector->ref1xref2);
}

/* Create all sectors centered around an le of an shell.  If a sector */
/* is found to be 180 degrees, bisect it and record two sectors in its */
/* sector list.  If it is > 180 then this le must lie in a concavity of the */
/* face that the sector lies in, in which case, also bisect it.  If it is */
/* greater than 180 wide but the face is not concave at that le, then */
/* this routine will do the wrong thing... there should never be a face */
/* like that!!!! (Potential bug) If a sector */
/* is 0 degrees wide, then gag, big time, and report an error. */

  void
create_sectorlist(leptr centerle,listptr sectorlist)
{
  leptr thisle,nextle;
  fveptr thisfve;
  boundptr thisbound;
  vertype ref1,ref2,ref12,bisect1,bisect2,bisector;
  Coord *thisectornorm,normdot;
  Boolean wrongway,zeroref12,thisbnd_ccw, makenewsector = TRUE;
  Boolean concavity = FALSE;
  ;
  thisle = centerle;
  do
  {
#ifndef debug
    if (thisle->facevert != centerle->facevert)
      system_error("create_sectorlist: bad le neighborhood!!!!");
#endif
    thisbound = thisle->motherbound;
    thisfve = thisbound->motherfve;
    thisectornorm = thisfve->facenorm;
    nextle = (Twin_le(thisle))->next;
    /* compute vectors bounding sector */
    diffpos3d(thisle->facevert->pos,thisle->prev->facevert->pos,ref1);
    diffpos3d(thisle->facevert->pos,thisle->next->facevert->pos,ref2);
    normalize(ref1,ref1);
    normalize(ref2,ref2);
    /* find cross prod of the bounding vectors */
    cross_prod(ref1,ref2,ref12);
    thisbnd_ccw = cclockwise_bound(thisbound);
    if (thisfve->boundlist->first.bnd != thisbound)
      thisbnd_ccw = !thisbnd_ccw; /* reverse if it was a hole. */
    zeroref12 = vector_near_zero(ref12,Zeroveclengthtol);
    /* If the dot prod of the crossprod of the boundaries with the facenorm */
    /* is positive and the face is counterclockwise, or if the dotprod is */
    /* negative and the face is clockwise, then we have a concavity */
    if (!zeroref12)
    {
      normdot = dotvecs(ref12,thisectornorm);
      concavity = (((normdot > 0.0) && thisbnd_ccw) ||
		   ((normdot < 0.0) && !thisbnd_ccw));
    }
    /* Check for zero length cross prod then the boundaries are colinear */
    /* and either point the same direction or in opposite directions. The */
    /* first case is degenerate (we just barf for now). In the second case */
    /* we must bisect the sector. Also if we are at a concavity we must */
    /* bisect the sector */
    if (zeroref12 || concavity)
    {
      /* if the crossprod has no length, then the vectors are collinear */
      /* which could mean pointing the same way or have 180 between them. */
      if (zeroref12)
      {
	/* if the dot product of the bounding vectors is positive, */
	/* they point in the same direction. For now just barf...maybe */
	/* do something smarter later. */
	if (dotvecs(ref1,ref2) > 0.0)
	  system_error("create_sectorlist: ref1 && ref2 point the same way");
	/* Create a new bisector halfway between the two 180 deg */
	/* boundaries in the interior of the sector. */
	cross_prod(ref1,thisectornorm,bisector);
	if (!thisbnd_ccw) /* If face was cw, then must reverse bisector. */
	  flip_vec(bisector);	    
      }
      else			/* we have a concavity */
      {
	if (dotvecs(ref1,ref2) < 0.0) /* when angle on other side > 90, */
	{			/* to avoid zero vec bisector, use xprod */
	  cross_prod(ref1,thisectornorm,bisect1); /* with facenorm rather */
	  cross_prod(thisectornorm,ref2,bisect2); /* than vector addition */
	  addvec3d(bisect1,bisect2,bisector);
	}
	else
	  addvec3d(ref1,ref2,bisector);
	flip_vec(bisector);
      }
      make_sector(sectorlist,thisle,thisectornorm,ref1,bisector);
      make_sector(sectorlist,thisle,thisectornorm,bisector,ref2);
    }
    else			/* log a regular, non > 180 sector */
      make_sector(sectorlist,thisle,thisectornorm,ref1,ref2);
    
    thisle = nextle;
  } while (thisle != centerle);
}

/* Check to see if a vector is within a sector.  This returns true if the */
/* vector is lying ON one of the boundaries of the sector as well as */
/* completely inside the sector. */

  Boolean
vector_in_sector(sectorptr thesector,vertype thevec)
{
  Boolean vector_contained;
  vertype crossprod1,crossprod2;
  double refdotvec;
  int dotresult1,dotresult2;
  ;
#ifdef oldway
  if (vector_near_zero(crossprod1,Zeroveclengthtol))
  {
    result = dotvecs(thesector->ref1,thevec);
    return (result > 0.0);
  }
  /* if thevec is lying ON ref2, then check if it points the same way. */
  cross_prod(thesector->ref2,thevec,crossprod2);
  if (vector_near_zero(crossprod2,Zeroveclengthtol))
  {
    result = dotvecs(thesector->ref2,thevec);
    return (result > 0.0);
  }
#endif
  /* check to see if thevec is lying ON ref1 and points the same way. */
  refdotvec = dotvecs(thesector->ref1,thevec);
  if (compare(refdotvec,1.0,Zeroveclengthtol) == 0)
    return (TRUE);
  else if (compare(refdotvec,-1.0,Zeroveclengthtol) == 0)
    return (FALSE);
  /* check to see if thevec is lying ON ref2 and points the same way. */
  refdotvec = dotvecs(thesector->ref2,thevec);
  if (compare(refdotvec,1.0,Zeroveclengthtol) == 0)
    return (TRUE);
  else if (compare(refdotvec,-1.0,Zeroveclengthtol) == 0)
    return (FALSE);

  /* otherwise, if the two crossprods point in opposite directions as */
  /* the face normal, then the vector is completely contained in the sector */
  cross_prod(thevec,thesector->ref1,crossprod1);
  cross_prod(thesector->ref2,thevec,crossprod2);
  dotresult1 = compare(dotvecs(crossprod1,thesector->ref1xref2),0.0,
		       Linetolinetol);
  dotresult2 = compare(dotvecs(crossprod2,thesector->ref1xref2),0.0,
		       Linetolinetol);
  vector_contained = ((dotresult1 < 0) && (dotresult2 < 0));

  return(vector_contained);
}

/* check to see if two sectors intersect.  Return true if they do, or if */
/* they overlap (even when just two boundaries of the sectors overlap). */

  Boolean
sectors_intersect(sectorptr sectorA,sectorptr sectorB)
{
  vertype intersectvec;
  Boolean inside_sectorA,inside_sectorB;
  ;
  cross_prod(sectorA->centerle->motherbound->motherfve->facenorm,
	     sectorB->centerle->motherbound->motherfve->facenorm,
	     intersectvec);
  if (vector_near_zero(intersectvec,Zeroveclengthtol))
    /* check for overlapping sectors */
    return(vector_in_sector(sectorA,sectorB->ref1) ||
	   vector_in_sector(sectorA,sectorB->ref2));
  /* if not overlapping sectors, check to see if the intersection line */
  /* is completely inside both sectors. */

  normalize(intersectvec,intersectvec);

  inside_sectorA = vector_in_sector(sectorA,intersectvec);
  inside_sectorB = vector_in_sector(sectorB,intersectvec);
  if (inside_sectorA && inside_sectorB)
    return(TRUE);
  /* if it wasn't, it could be facing the wrong way, so check it again */
  /* after flipping it. */
  flip_vec(intersectvec);
  inside_sectorA = vector_in_sector(sectorA,intersectvec);
  inside_sectorB = vector_in_sector(sectorB,intersectvec);
  if (inside_sectorA && inside_sectorB)
    return(TRUE);
	   
  return(FALSE);		/* otherwise, the intersection line is not */
				/* inside both sectors. */
}

void
bogosity_personified()
{				/* we hate mips */
}

/* Fill in the sidesectorpair records for sectorpairs with ON edge codes. */
/* LJE: explain this one!! */

  void
set_sidesectorpairs(listptr sectorlist, sectorptr thisector, int thiside,
		    sectorpairptr newsectorpair)
{
  sectorpairptr thisectorpair;
  ;
  if (thisector->sidesectorpair[thiside][0] == Nil)
    thisector->sidesectorpair[thiside][0] = newsectorpair;
  else if (thisector->sidesectorpair[thiside][1] == Nil)
    thisector->sidesectorpair[thiside][1] = newsectorpair;
  else
    system_error("set_sidesectorpairs: both sectorpairs already set");
}

/* Compute side codes of each sector with respect to the other sector by */
/* finding the dot product of the face normal of the other sector with */
/* each bounding vector of this sector.  If the dot product > 0, then */
/* compare() will return 1 == IN, otherwise range will return 0 == OUT if */
/* the dot prod < 0, and finally 0 == ON if the dot prod == 0, if the normal */
/* and the bounding vect are at 90 degrees.  These ON boundaries will later */
/* be reclassified according to the boolean op in question. */

  void
set_sectorpair(vvptr thisvv, sectorpairptr new_sectorpair, sectorptr sectorA,
	       sectorptr sectorB)
{
  double dot1,dot2,dot3,dot4;
  Coord *facenorm1,*facenorm2;
  ;
  new_sectorpair->sectorA = sectorA;
  new_sectorpair->sectorB = sectorB;
  facenorm1 = sectorA->centerle->motherbound->motherfve->facenorm;
  facenorm2 = sectorB->centerle->motherbound->motherfve->facenorm;
  dot1 = dotvecs(facenorm2,sectorA->ref1);
  dot2 = dotvecs(facenorm2,sectorA->ref2);
  dot3 = dotvecs(facenorm1,sectorB->ref1);
  dot4 = dotvecs(facenorm1,sectorB->ref2);
  /* Note the minus sign on compare...mantyla had this wrong!!!!!! */
  new_sectorpair->sidecodesA[0] = -compare(dot1,0.0,Onplanetol);
  new_sectorpair->sidecodesA[1] = -compare(dot2,0.0,Onplanetol);
  new_sectorpair->sidecodesB[0] = -compare(dot3,0.0,Onplanetol);
  new_sectorpair->sidecodesB[1] = -compare(dot4,0.0,Onplanetol);
  new_sectorpair->intersectcode = TRUE;

  if ((new_sectorpair->sidecodesA[0] == BOOL_ON) &&
      (new_sectorpair->sidecodesA[1] == BOOL_ON))
  {
    if (vector_in_sector(sectorB,sectorA->ref1))
      set_sidesectorpairs(thisvv->sectorlistA,sectorA,0,new_sectorpair);
    if (vector_in_sector(sectorB,sectorA->ref2))
      set_sidesectorpairs(thisvv->sectorlistA,sectorA,1,new_sectorpair);

    if (vector_in_sector(sectorA,sectorB->ref1))
      set_sidesectorpairs(thisvv->sectorlistB,sectorB,0,new_sectorpair);
    if (vector_in_sector(sectorA,sectorB->ref2))
      set_sidesectorpairs(thisvv->sectorlistB,sectorB,1,new_sectorpair);
  }
  else
  {
    if (new_sectorpair->sidecodesA[0] == BOOL_ON)
      set_sidesectorpairs(thisvv->sectorlistA,sectorA,0,new_sectorpair);
    else if (new_sectorpair->sidecodesA[1] == BOOL_ON)
      set_sidesectorpairs(thisvv->sectorlistA,sectorA,1,new_sectorpair);
    if (new_sectorpair->sidecodesB[0] == BOOL_ON)
      set_sidesectorpairs(thisvv->sectorlistB,sectorB,0,new_sectorpair);
    else if (new_sectorpair->sidecodesB[1] == BOOL_ON)
      set_sidesectorpairs(thisvv->sectorlistB,sectorB,1,new_sectorpair);
  }

}

/* For each two coincident vertices, one from solid A and one from SolidB,*/
/* compute a list of pairs of sectors that intersect in some way (eg, cross */
/* through each other or are coplanar and overlap.  Note n^2 nature of this */
/* loop: can't be helped (?) */

  void
create_sectorpairlist(vvptr thisvv)
{
  sectorptr sectorA,sectorB;
  sectorpairptr new_sectorpair;
  listptr sectorlistA,sectorlistB;
  ;
  sectorlistA = thisvv->sectorlistA;
  sectorlistB = thisvv->sectorlistB;
  sectorA = sectorlistA->first.sector;
  thisvv->sectorpairlist = Nil;
  while (sectorA != Nil)
  {
    sectorB = sectorlistB->first.sector;
    while (sectorB != Nil)
    {
      if (sectors_intersect(sectorA,sectorB))
      {
	if (thisvv->sectorpairlist == Nil)
	  thisvv->sectorpairlist = create_list();
	new_sectorpair = (sectorpairptr)
	  append_new_blank_elem(thisvv->sectorpairlist,
				Sectorpair_type);
	/* fill in the sectorpair */
	set_sectorpair(thisvv,new_sectorpair,sectorA,sectorB);
      }	
      sectorB = sectorB->next;
    }
    sectorA = sectorA->next;
  }
}

/* Fill in details of two coincident vertices, namely: */
/* their sectorlists and their paired sectors. */
  void
create_vv_neighborhood(vvptr thisvv)
{
  leptr centerA,centerB;
  ;
  centerA = thisvv->vfeA->startle;
  centerB = thisvv->vfeB->startle;
  thisvv->sectorlistA = create_list();
  thisvv->sectorlistB = create_list();
  create_sectorlist(centerA,thisvv->sectorlistA);
  create_sectorlist(centerB,thisvv->sectorlistB);
  create_sectorpairlist(thisvv);
}

/* Set the sidecodes of the boundaries of sectors of a vertex that lies */
/* on a face of the other solid.  Similar to set_sectorpair except that */
/* instead of using another sector as a plane to set the boundary sidecodes */
/* of each sector of the vertex, we just use the plane of the face it is lying*/
/* on. */

  void
set_vf_sidecodes(vfptr thisvf,Boolean faceisptr1)
{
  Coord *facenorm;
  sectorptr thisector;
  double dot1,dot2;
  ;
  if (faceisptr1)
    facenorm = ((fveptr) thisvf->touchingelem[SolidA])->facenorm;
  else
    facenorm = ((fveptr) thisvf->touchingelem[SolidB])->facenorm;
  thisector = thisvf->sectorlist->first.sector;
  while (thisector != Nil)
  {
    dot1 = dotvecs(facenorm,thisector->ref1);
    dot2 = dotvecs(facenorm,thisector->ref2);
    thisector->vfsidecodes[0] = -compare(dot1,0.0,Onplanetol);
    thisector->vfsidecodes[1] = -compare(dot2,0.0,Onplanetol);
    thisector = thisector->next;
  }
}
  
/* Fill in details of a coincident vertex and face, namely: */
/* the sectorlist of the vertex, and sidecodes of those sectors w/ respect */
/* to the coincident face. */
    
  void
create_vf_neighborhood(vfptr thisvf)
{
  leptr centerle;
  Boolean ptr1isface = FALSE;
  ;
  if (thisvf->touchingelem[SolidA]->type_id != Vfe_type)
  {
    ptr1isface = TRUE;
    centerle = (leptr) (((vfeptr) thisvf->touchingelem[SolidB])->startle);
  }
  else if (thisvf->touchingelem[SolidA]->type_id != Fve_type)
    centerle = (leptr) (((vfeptr) thisvf->touchingelem[SolidA])->startle);
  else
    system_error("create_vf_nbrhoods: touchingelem is not an fve or a vfe");
  thisvf->sectorlist = create_list();
  create_sectorlist(centerle,thisvf->sectorlist);
  set_vf_sidecodes(thisvf,ptr1isface);
}

/* Create all neighborhoods for all coincident vertex pairs and coincident */
/* vertex-face pairs from the two shells. */

  void
create_neighborhoodlists(listptr vv_list,listptr vf_list)
{
  vvptr thisvv;
  vfptr thisvf;
  ;
  thisvv = vv_list->first.vv;
  while (thisvv != Nil)
  {
    create_vv_neighborhood(thisvv);
    thisvv = thisvv->next;
  }
  thisvf = vf_list->first.vf;
  while (thisvf != Nil)
  {
    create_vf_neighborhood(thisvf);
    thisvf = thisvf->next;
  }
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                  VERTEX NEIGHBORHOOD CLASSIFIER ROUTINES               */
/* These routines take pairs of vertices that are coincident in the two   */
/* shells and run a series of tests on their neighborhoods to determine  */
/* codes for each sector pair, which will be used to determine where to   */
/* insert null edges.   These codes will instruct later splitting         */
/* routines on how to split up the two shells A and B into the parts     */
/* AinB,AoutB, BinA, BoutA.                                               */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


  void
set_pair_oninfo(int index, pair_oninfo_type pair_oninfo[],
		sectorpairptr sectorpair, int Aonside, int Bonside)
{
  pair_oninfo[index].sectorpair = sectorpair;
  pair_oninfo[index].Aonside = Aonside;
  pair_oninfo[index].Bonside = Bonside;
}

  int
find_common_edge_sectorpairs(vvptr thisvv, sectorpairptr thisectorpair,
			     int sectorAonside, int sectorBonside,
			     pair_oninfo_type pair_oninfo[])
{
  sectorptr sectorA,sectorB,othersectorA,othersectorB;
  listptr sectorlist;
  sectorpairptr *othersectorAonsidepair,*sectorAonsidesectorpair;
  sectorpairptr *othersectorBonsidepair,*sectorBonsidesectorpair;
  int index,otherindex;
  ;
  sectorA = thisectorpair->sectorA;
  sectorB = thisectorpair->sectorB;
  set_pair_oninfo(0,pair_oninfo,thisectorpair,sectorAonside,sectorBonside);
  if (sectorAonside >= 0)
  {
    sectorlist = thisvv->sectorlistA;
    if (sectorAonside == 0)
      othersectorA = (sectorptr) prev_elem(sectorlist, (elemptr) sectorA);
    else
      othersectorA = (sectorptr) next_elem(sectorlist, (elemptr) sectorA);
    othersectorAonsidepair = othersectorA->sidesectorpair[1-sectorAonside];
    otherindex = (othersectorAonsidepair[0]->sectorB == sectorB) ? 0 : 1;
    set_pair_oninfo(1,pair_oninfo,othersectorAonsidepair[otherindex],
		    1-sectorAonside,sectorBonside);
    if (sectorBonside >= 0)
    {
      sectorAonsidesectorpair = sectorA->sidesectorpair[sectorAonside];
      index = (sectorAonsidesectorpair[0] != thisectorpair) ? 0 : 1;
      set_pair_oninfo(2,pair_oninfo,sectorAonsidesectorpair[index],
		      sectorAonside,1-sectorBonside);
      set_pair_oninfo(3,pair_oninfo,othersectorAonsidepair[1-otherindex],
		      1-sectorAonside,1-sectorBonside);
      return (4);
    }
  }
  else if (sectorBonside >= 0)
  {
    sectorlist = thisvv->sectorlistB;
    if (sectorBonside == 0)
      othersectorB = (sectorptr) prev_elem(sectorlist, (elemptr) sectorB);
    else
      othersectorB = (sectorptr) next_elem(sectorlist, (elemptr) sectorB);
    othersectorBonsidepair = othersectorB->sidesectorpair[1-sectorBonside];
    otherindex = (othersectorBonsidepair[0]->sectorA == sectorA) ? 0 : 1;
    set_pair_oninfo(1,pair_oninfo,othersectorBonsidepair[otherindex],
		    sectorAonside,1-sectorBonside);
  }
  return (2);
}

  int
findsectoronside(int sidecodes[])
{
  int sectoronside;
  ;
  sectoronside = (sidecodes[0] == BOOL_ON) ? 0 : -1;
  sectoronside = (sidecodes[1] == BOOL_ON) ? 1 : sectoronside;
  
  return (sectoronside);
}

  Boolean
surfaces_intersect(int sectorAonside, int sectorBonside,
		   pair_oninfo_type pair_oninfo[])
{
  if (sectorAonside >= 0)
    return (pair_oninfo[0].sectorpair->sidecodesA[1-sectorAonside] !=
	    pair_oninfo[1].sectorpair->sidecodesA[sectorAonside]);
  return (pair_oninfo[0].sectorpair->sidecodesB[1-sectorBonside] !=
	pair_oninfo[1].sectorpair->sidecodesB[sectorBonside]);
}

  void
reclassify_onface_edges(vvptr thisvv)
{
  sectorpairptr thisectorpair;
  sectorptr thisectorA,thisectorB;
  Boolean on_edge_A,on_edge_B,intersecting;
  pair_oninfo_type pair_oninfo[4];
  int sectorAonside,sectorBonside,*sidecodesA,*sidecodesB,i,numsectorpairs;
  ;
  /* mental never says it explicitly but we will assume the following: */
  /* if the surfaces intersect: in-on sectors -> in-out & out-on -> out->out */
  /* if they don't: in-on sectors -> in-in & out-on -> out->out */
  thisectorpair = thisvv->sectorpairlist->first.sectorpair;
  while (thisectorpair)
  {
    if (thisectorpair->intersectcode &&
	((thisectorpair->sidecodesA[0] != thisectorpair->sidecodesA[1]) ||
	 (thisectorpair->sidecodesB[0] != thisectorpair->sidecodesB[1])))
    {
      sectorAonside = findsectoronside(thisectorpair->sidecodesA);
      sectorBonside = findsectoronside(thisectorpair->sidecodesB);
      if ((sectorAonside >= 0) || (sectorBonside >= 0))
      {
	numsectorpairs = find_common_edge_sectorpairs(thisvv,
						      thisectorpair,
						      sectorAonside,
						      sectorBonside,
						      pair_oninfo);
	if (surfaces_intersect(sectorAonside,sectorBonside,pair_oninfo))
	{			/* surfaces intersect */
	  /* reclassify all sectors that share edge */
	  for (i=0; i < numsectorpairs; i++)
	  {
	    sectorAonside = pair_oninfo[i].Aonside;
	    sectorBonside = pair_oninfo[i].Bonside;
	    sidecodesA = pair_oninfo[i].sectorpair->sidecodesA;
	    sidecodesB = pair_oninfo[i].sectorpair->sidecodesB;
	    if (sectorAonside >= 0)
	      sidecodesA[sectorAonside] = BOOL_OUT;
	    if (sectorBonside >= 0)
	      sidecodesB[sectorBonside] = BOOL_OUT;
	    if ((sidecodesA[0] != sidecodesA[1]) &&
		(sidecodesB[0] != sidecodesB[1]))
	      pair_oninfo[i].sectorpair->intersectcode = 1;
	    else
	      pair_oninfo[i].sectorpair->intersectcode = 0;
	  }
	}
	else			/* surfaces don't intersect */
	{			/* reclassify all sectors that share edge */
	  for (i=0; i < numsectorpairs; i++)
	  {
	    sectorAonside = pair_oninfo[i].Aonside;
	    sectorBonside = pair_oninfo[i].Bonside;
	    sidecodesA = pair_oninfo[i].sectorpair->sidecodesA;
	    sidecodesB = pair_oninfo[i].sectorpair->sidecodesB;
	    if (sectorAonside >= 0)
	      sidecodesA[sectorAonside] = sidecodesA[1-sectorAonside];
	    if (sectorBonside >= 0)
	      sidecodesB[sectorBonside] = sidecodesB[1-sectorBonside];
	    pair_oninfo[i].sectorpair->intersectcode = 0;
	  }
	}
      }
    }
    thisectorpair = thisectorpair->next;
  }
}

  void
reclassify_adjoining_sectors(sectorpairptr thisectorpair,
			     sectorptr sectorA, sectorptr sectorB,
			     sectorptr prevsectorA, sectorptr prevsectorB,
			     sectorptr nextsectorA, sectorptr nextsectorB,
			     int newsidecodeA, int newsidecodeB)
{
  /* note sidecode[1] coincides with centerle */
  if ((thisectorpair->sectorA == prevsectorA) &&
      (thisectorpair->sectorB == sectorB))
    if (thisectorpair->sidecodesA[0] != BOOL_ON)
      thisectorpair->sidecodesA[1] = newsidecodeA;

  if ((thisectorpair->sectorA == nextsectorA) &&
      (thisectorpair->sectorB == sectorB))
    if (thisectorpair->sidecodesA[1] != BOOL_ON)
      thisectorpair->sidecodesA[0] = newsidecodeA;

  if ((thisectorpair->sectorA == sectorA) &&
      (thisectorpair->sectorB == prevsectorB))
    if (thisectorpair->sidecodesB[0] != BOOL_ON)
      thisectorpair->sidecodesB[1] = newsidecodeB;

  if ((thisectorpair->sectorA == sectorA) &&
      (thisectorpair->sectorB == nextsectorB))
    if (thisectorpair->sidecodesB[1] != BOOL_ON)
      thisectorpair->sidecodesB[0] = newsidecodeB;
}

  void
reclassify_onface_sectors(vvptr thisvv, int operation)
{
  listptr sectorpairlist;
  sectorpairptr thisectorpair,thatsectorpair;
  listptr sectorlistA,sectorlistB;
  sectorptr sectorA,sectorB,prevsectorA,prevsectorB,nextsectorA,nextsectorB;
  float *sectornormalA,*sectornormalB;
  int newsidecodeA,newsidecodeB;
  ;
  sectorpairlist = thisvv->sectorpairlist;
  sectorlistA = thisvv->sectorlistA;
  sectorlistB = thisvv->sectorlistB;
  thisectorpair = sectorpairlist->first.sectorpair;
  while (thisectorpair)
  {
    if ((thisectorpair->sidecodesA[0] == BOOL_ON) &&
	(thisectorpair->sidecodesA[1] == BOOL_ON) &&
	(thisectorpair->sidecodesB[0] == BOOL_ON) &&
	(thisectorpair->sidecodesB[1] == BOOL_ON))
    {
      sectorA = thisectorpair->sectorA;
      sectorB = thisectorpair->sectorB;
      sectornormalA = sectorA->centerle->motherbound->motherfve->facenorm;
      sectornormalB = sectorB->centerle->motherbound->motherfve->facenorm;
      if (dotvecs(sectornormalA,sectornormalB) > 0)
      {
	if (operation == BOOL_Union)
	  newsidecodeA = BOOL_OUT;
	else
	  newsidecodeA = BOOL_IN;
	newsidecodeB = -newsidecodeA;
      }
      else
      {
	if (operation == BOOL_Union)
	  newsidecodeA = BOOL_IN;
	else
	  newsidecodeA = BOOL_OUT;
	newsidecodeB = newsidecodeA;
      }
      prevsectorA = (sectorptr) prev_elem(sectorlistA, (elemptr) sectorA);
      nextsectorA = (sectorptr) next_elem(sectorlistA, (elemptr) sectorA);
      prevsectorB = (sectorptr) prev_elem(sectorlistB, (elemptr) sectorB);
      nextsectorB = (sectorptr) next_elem(sectorlistB, (elemptr) sectorB);
      thatsectorpair = sectorpairlist->first.sectorpair;
      while (thatsectorpair)
      {		/* we have to loop after each nbr reclass? */
	reclassify_adjoining_sectors(thatsectorpair,sectorA,sectorB,
				     prevsectorA,prevsectorB,
				     nextsectorA,nextsectorB,
				     newsidecodeA,newsidecodeB);
	if ((thatsectorpair->sidecodesA[0] == thatsectorpair->sidecodesA[1]) &&
	    (thatsectorpair->sidecodesA[0] != BOOL_ON))
	  thatsectorpair->intersectcode = 0;
	if ((thatsectorpair->sidecodesB[0] == thatsectorpair->sidecodesB[1]) &&
	    (thatsectorpair->sidecodesB[0] != BOOL_ON))
	  thatsectorpair->intersectcode = 0;
	thatsectorpair = thatsectorpair->next;
      }
      thisectorpair->sidecodesA[0] = thisectorpair->sidecodesA[1] =newsidecodeA;
      thisectorpair->sidecodesB[0] = thisectorpair->sidecodesB[1] =newsidecodeB;
      thisectorpair->intersectcode = 0;
    }
    thisectorpair = thisectorpair->next;
  }
}

  void
classify_vertex_touching_vertex(vvptr thisvv, int operation)
{
  reclassify_onface_sectors(thisvv,operation);
  reclassify_onface_edges(thisvv);
}

  void
reclassify_vf_onsectors(vfptr thisvf, int operation)
{
  int vertexindex,newsidecodeA,newsidecodeB;
  sectorptr thisector,prevsector,nextsector;
  float *facenorm,*touchedfacenorm;
  double dotresult;
  ;
  vertexindex = (thisvf->touchingelem[SolidA]->type_id == Vfe_type) ? 0 : 1;
  touchedfacenorm = ((fveptr) thisvf->touchingelem[1-vertexindex])->facenorm;
  thisector = thisvf->sectorlist->first.sector;
  while (thisector != Nil)
  {
    facenorm = thisector->centerle->motherbound->motherfve->facenorm;
    dotresult = dotvecs(facenorm,touchedfacenorm);
    if (compare(Fabs(dotresult),1.0,Floatol) == 0)
    {
      if (dotresult > 0.0)
      {
	if (operation == BOOL_Union)
	  newsidecodeA = BOOL_OUT;
	else
	  newsidecodeA = BOOL_IN;
	newsidecodeB = -newsidecodeA;
      }
      else
      {
	if (operation == BOOL_Union)
	  newsidecodeA = BOOL_IN;
	else
	  newsidecodeA = BOOL_OUT;
	newsidecodeB = newsidecodeA;
      }
      if (vertexindex == SolidA)
      {
	thisector->vfsidecodes[0] = newsidecodeA;
	thisector->vfsidecodes[1] = newsidecodeA;
      }
      else
      {
	thisector->vfsidecodes[0] = newsidecodeB;
	thisector->vfsidecodes[1] = newsidecodeB;
      }
      /* reclassify adjoining sector sidecodes */
      nextsector = (sectorptr) next_elem(thisvf->sectorlist,
					 (elemptr) thisector);
      prevsector = (sectorptr) prev_elem(thisvf->sectorlist,
					 (elemptr) thisector);
      nextsector->vfsidecodes[0] = thisector->vfsidecodes[1];
      prevsector->vfsidecodes[1] = thisector->vfsidecodes[0];
    }
    thisector = thisector->next;
  }
}

  void
reclassify_vf_onedges(vfptr thisvf)
{
  sectorptr thisector,nextsector;
  int prevsideout,nextsideout;
  ;
  /* in-on-in   -> in-out-in */
  /* in-on-out  -> in-out-out */
  /* out-on-out -> out-in-out */
  /* out-on-in  -> out-out-in */
  thisector = thisvf->sectorlist->first.sector;
  while (thisector != Nil)
  {
    if (thisector->vfsidecodes[1] == BOOL_ON)
    {
      nextsector = (sectorptr) next_elem(thisvf->sectorlist,
					 (elemptr) thisector);
      prevsideout = (thisector->vfsidecodes[0] == BOOL_OUT);
      nextsideout = (nextsector->vfsidecodes[1] == BOOL_OUT);
      if (prevsideout && nextsideout)
      {
	thisector->vfsidecodes[1] = BOOL_IN;
	nextsector->vfsidecodes[0] = BOOL_IN;
      }
      else if (!prevsideout || (prevsideout && !nextsideout))
      {
	thisector->vfsidecodes[1] = BOOL_OUT;
	nextsector->vfsidecodes[0] = BOOL_OUT;
      }
    }
    thisector = thisector->next;
  }
}

  void
classify_vertex_touching_face(vfptr thisvf, int operation)
{
  /* this is the same as slice, except the plane is now the plane of a face */
  /* in shell_b */
  reclassify_vf_onsectors(thisvf,operation);
  reclassify_vf_onedges(thisvf);
}

  void
classify_neighborhoods(shellptr shellA, shellptr shellB, int operation,
		       listptr vv_list,listptr vf_list)
{
  vvptr thisvv;
  vfptr thisvf;
  ;
  thisvv = vv_list->first.vv;
  while (thisvv)
  {
    classify_vertex_touching_vertex(thisvv,operation);
    thisvv = thisvv->next;
  }
  thisvf = vf_list->first.vf;
  while (thisvf)
  {
    classify_vertex_touching_face(thisvf,operation);
    thisvf = thisvf->next;
  }
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                     NULL EDGE INSERTION ROUTINES                       */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* Look for next sectorpair in a neighborhood's sectorpair list that is logged*/
/* as intersecting after classification. This will determine where the */
/* null_le will be inserted in calls to mev. */

  Boolean
scan_for_intersecting_pair(sectorpairptr *thisectorpair)
{
  if (*thisectorpair == Nil)
    return(TRUE);		/* already reached the end, quit! */
  while (!((*thisectorpair)->intersectcode))
  {
    *thisectorpair = (*thisectorpair)->next;
    if (*thisectorpair == Nil)	/* no more sector intrs in this neighborhood */
      return(TRUE);
  }
  return(FALSE);
}

/* Decide, from side codes of the sectors in a sectorpair, whether to assign */
/* from and to le's that will be used in insert_null_le or */
/* insert_strut_null_le. */
/* The resulting null_le's will go from OUT to IN. */

  void
set_from_to_les(sectorpairptr thisectorpair, leptr *fromle_A, leptr *to_le_A,
		leptr *fromle_B, leptr *to_le_B)
{
  leptr side_1_le;
  ;
  side_1_le = thisectorpair->sectorA->centerle;

  /* Mantle had this OUT because he assumes same orientation of A and B */
  /* and he wants the fromle to be from the same sector pair in both a and B */

  if (thisectorpair->sidecodesA[0] == BOOL_IN) /* we've got a sector going in->out */
    *fromle_A = side_1_le;	/* set from_le to the out side */
  else				/* here's one going out->in */
    *to_le_A = side_1_le;	/* set to_le to the in side */

  side_1_le = thisectorpair->sectorB->centerle;
  if (thisectorpair->sidecodesB[0] == BOOL_IN)
    *fromle_B = side_1_le;
  else
    *to_le_B = side_1_le;
}

/* Insert a null_le at a coincident vertex that has been reclassified. */

  void
insert_null_le(leptr from_le, leptr to_le, lepairptr thislepair, int index)
{
  thislepair->le[index] = make_edge_vert(from_le,to_le, from_le->facevert->pos);
  set_property((featureptr) thislepair->le[index],nullfeature_prop,
	       (elemptr) thislepair);
}

void
set_out_in_flag(Boolean *out_in_flag, sectorpairptr thisectorpair,
		sectorpairptr lastsectorpair, int shellindex)
{
  sectorptr thisector,othersector1,othersector2;
  float *thisnorm,*side_0_vec,*othernorm1,*othernorm2,costh1,costh2;
  vertype intersect_vec1,intersect_vec2;
  int side_0_code;
  ;
  /* given multiple intersections in a sector this routine determines */
  /* which sector is closer to side[0] of the intersected sector */
  if (shellindex == SolidA)
  {
    thisector = thisectorpair->sectorA;
    side_0_code = thisectorpair->sidecodesA[0];
    /* if this is a shellA strut that means we had multiple shellB intersects */
    othersector1 = lastsectorpair->sectorB;
    othersector2 = thisectorpair->sectorB;
  }
  else
  {
    thisector = thisectorpair->sectorB;
    side_0_code = thisectorpair->sidecodesB[0];
    /* if this is a shellB strut that means we had multiple shellA intersects */
    othersector1 = lastsectorpair->sectorA;
    othersector2 = thisectorpair->sectorA;
  }
  thisnorm = thisector->sectornorm;
  othernorm1 = othersector1->sectornorm;
  othernorm2 = othersector2->sectornorm;
  side_0_vec = thisector->ref1;
  normalize(thisnorm,thisnorm);
  normalize(othernorm1,othernorm1);
  normalize(othernorm2,othernorm2);
  normalize(side_0_vec,side_0_vec);
  /* MIGHT NEED SOME CONCAVITY CHECKS HERE */
  /* find the intersection of othersector1 and thisector */
  cross_prod(thisnorm,othernorm1,intersect_vec1);
  normalize(intersect_vec1,intersect_vec1);
  /* find the intersection of othersector2 and thisector */
  cross_prod(othernorm2,thisnorm,intersect_vec2);
  normalize(intersect_vec2,intersect_vec2);
  /* determine angle between other sectors and side[0] of thisector */
  costh1 = fabs(dotvecs(intersect_vec1,side_0_vec));
  costh2 = fabs(dotvecs(intersect_vec2,side_0_vec));
  /* use in out codes from thisectorpair if othersector2 is closer to side 0 */
  if (costh2 > costh1)
    *out_in_flag = (side_0_code == BOOL_OUT);
}

/* To insert a null_edge in a vv neighborhood, find the first sectorpair */
/* that is still logged as intersecting, and record the centerle's according */
/* to whether the sectors went sidecode IN to OUT or vice versa.  Then */
/* skip ahead to the next intersecting sectorpair, and record the le's again */
/* (only this time you will get the opposite le's since the transitions must */
/* be opposite (ie you entered the solid in one sector then later on left */
/* the solid.  Once you have two le's for each solid, you can call insert */
/* null_le which will separate the solid to each side (either IN or OUT). */
/* Also, you want to insert a strut null_le for cases where a sector is */
/* intersect multiple times by other sectors (ie leaves and reenters the */
/* solid more than once in a single sector.)  (see mantlya for */
/* details, pg 291.) */


  static void
add_vv_null_edges(vvptr thisvv, listptr null_lelist)
{
  listptr sectorpairlist;
  sectorpairptr thisectorpair,lastsectorpair,prevsectorpair;
  sectorptr prevsector;
  Boolean all_sectorpairs_done = FALSE;
  leptr fromle_A = Nil, to_le_A = Nil, fromle_B = Nil, to_le_B = Nil;
  Boolean out_in_A,out_in_B;
  lepairptr new_lepair;
  ;
  sectorpairlist = thisvv->sectorpairlist; 
  thisectorpair = sectorpairlist->first.sectorpair;
  while ((thisectorpair != Nil) && (!all_sectorpairs_done))
  {
    /* look for the next sectorpair that is still logged as intersecting. */
    if (all_sectorpairs_done = scan_for_intersecting_pair(&thisectorpair))
      break;			/* prematurely finished */
    
    lastsectorpair = thisectorpair;
    out_in_A = (thisectorpair->sidecodesA[0] == BOOL_OUT);
    out_in_B = (thisectorpair->sidecodesB[0] == BOOL_OUT);
    set_from_to_les(thisectorpair,&fromle_A,&to_le_A,&fromle_B,&to_le_B);
    /* skip over first sector pair */
    thisectorpair = thisectorpair->next;
    if (all_sectorpairs_done = scan_for_intersecting_pair(&thisectorpair))
      break;			/* prematurely finished */
    set_from_to_les(thisectorpair,&fromle_A,&to_le_A,&fromle_B,&to_le_B);
    new_lepair = (lepairptr) append_new_blank_elem(null_lelist,Lepair_type);
    new_lepair->sectorlist = Nil;
    if (fromle_A == to_le_A)
    {
      if (thisectorpair->sectorA == lastsectorpair->sectorA)
	set_out_in_flag(&out_in_A,thisectorpair,lastsectorpair,SolidA);
      else
      {
	prevsector = (sectorptr) prev_elem(thisvv->sectorlistA,
					   (elemptr) lastsectorpair->sectorA);
	/* in case we dont start at the first sector of a wide sector */
	if (thisectorpair->sectorA == prevsector)
	  out_in_A = !out_in_A;
      }
      insert_null_le(fromle_A,fromle_A,new_lepair,SolidA); /* insert strut */
      if (out_in_A)	/* transition was OUT-IN-OUT, so reverse  */
	new_lepair->le[SolidA] = Twin_le(new_lepair->le[SolidA]);
      insert_null_le(fromle_B,to_le_B,new_lepair,SolidB);
    }
    else if (fromle_B == to_le_B)
    {
      if (thisectorpair->sectorB == lastsectorpair->sectorB)
	set_out_in_flag(&out_in_B,thisectorpair,lastsectorpair,SolidB);
      else
      {
	prevsector = (sectorptr) prev_elem(thisvv->sectorlistB,
					   (elemptr) lastsectorpair->sectorB);
	/* in case we dont start at the first sector of a wide sector */
	if (thisectorpair->sectorB == prevsector)
	  out_in_B = !out_in_B;
      }
      insert_null_le(fromle_B,fromle_B,new_lepair,SolidB); /* insert strut */
      if (out_in_B)	/* transition was OUT-IN-OUT, so reverse  */
	new_lepair->le[SolidB] = Twin_le(new_lepair->le[SolidB]);
      insert_null_le(fromle_A,to_le_A,new_lepair,SolidA);
    }
    else
    {
      insert_null_le(fromle_A,to_le_A,new_lepair,SolidA);
      insert_null_le(fromle_B,to_le_B,new_lepair,SolidB);
    }
    fromle_A = Nil; to_le_A = Nil; fromle_B = Nil; to_le_B = Nil;
    thisectorpair = thisectorpair->next;
  }
}


/* Put a null loop into a face where a vertex of the other shell lay on it. */

  void
insert_null_loop_into_face(vertype breakpos,fveptr theface,
			   lepairptr lepair, int index)
{
  leptr firstle,newholestartle;
  lepairptr new_lepair;
  boundptr newbound;
  ;
  firstle = theface->boundlist->first.bnd->lelist->first.le;
  newholestartle = make_edge_vert(firstle,firstle,breakpos);
  newbound = kill_edge_make_ring(Twin_le(newholestartle),newholestartle);
  lepair->le[index] = newholestartle;
  set_property((featureptr) lepair->le[index],nullfeature_prop,
	       (elemptr) lepair);
  /* make sure the hole has TWO le's and one edge so make_null_faces() */
  /* will work properly*/
  make_edge_vert(newholestartle,newholestartle,breakpos);
}

/* Insert a null_le where a vertex lay on a face.  */
/* LJE comment please. */

  static void
add_vf_null_edges(vfptr thisvf, listptr null_lelist)
{
  listptr sectorlist,vertex_null_lelist,face_null_lelist;
  sectorptr firstsector,thisector;
  fveptr thisfve;
  leptr headle,taile,null_le;
  lepairptr new_lepair;
  Boolean done = FALSE;
  int faceindex,vertexindex;
  ;
  sectorlist = thisvf->sectorlist;
  thisector = sectorlist->first.sector;
  faceindex = (thisvf->touchingelem[0]->type_id == Fve_type) ? 0 : 1;
  vertexindex = 1 - faceindex;
  thisfve = (fveptr) thisvf->touchingelem[faceindex];
  /* note: sidecodes go 0->1 as you go around a neighborhood */
  /* and centerle corresponds to sidecode[1] */
  while ((thisector != Nil) && !((thisector->vfsidecodes[0] == BOOL_IN) &&
				 (thisector->vfsidecodes[1] == BOOL_OUT)))
    thisector = thisector->next;
  if (thisector == Nil)
    return;
  firstsector = thisector;
  headle = thisector->centerle;
  while (!done)
  {
    while (!((thisector->vfsidecodes[0] == BOOL_OUT) &&
	     (thisector->vfsidecodes[1] == BOOL_IN)))
      thisector = (sectorptr) next_elem((listptr) sectorlist,
					(elemptr) thisector);
    taile = thisector->centerle;

    new_lepair = (lepairptr) append_new_blank_elem(null_lelist,Lepair_type);
    /* need to remember the sectors of the vertex that's on the face */
    /* so that later we can pick the proper null_le in snip_null_le's. */
    new_lepair->sectorlist = sectorlist;
    insert_null_le(headle,taile,new_lepair,vertexindex);
    insert_null_loop_into_face(headle->facevert->pos,thisfve,new_lepair,
			       faceindex);
    while (!((thisector->vfsidecodes[0] == BOOL_IN) &&
	     (thisector->vfsidecodes[1] == BOOL_OUT)))
      thisector = (sectorptr) next_elem((listptr) sectorlist,
					(elemptr) thisector);
    if (thisector != firstsector)
    {
      headle = thisector->centerle;
      thisector = (sectorptr) next_elem((listptr) sectorlist,
					(elemptr) thisector);
    }
    else
      done = TRUE;
  }
}


/* Insert all null edges in a solid where it was broken by intersection */
/* with the other solid. */

  static void
add_all_null_edges_boolean(listptr vv_list,listptr vf_list, listptr null_lelist)
{
  vvptr thisvv;
  vfptr thisvf;
  ;
  thisvv = vv_list->first.vv;
  while (thisvv != Nil)
  {
    add_vv_null_edges(thisvv,null_lelist);
    thisvv = thisvv->next;
  }
  thisvf = vf_list->first.vf;
  while (thisvf != Nil)
  {
    add_vf_null_edges(thisvf,null_lelist);
    thisvf = thisvf->next;
  }
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                      NULL EDGE SORTING ROUTINES                        */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* Insert a null_le pair record into a sorted paired_null_lelist. */

  void
sorted_insert_null_lepair(lepairptr new_lepair, listptr sorted_lepairlist)
{
  lepairptr this_lepair;
  float *thispos,*newpos;
  ;
  /* if there are no records in the sorted null le pair list, just add it. */
  if (sorted_lepairlist->numelems == 0)
  {
    append_elem(sorted_lepairlist, (elemptr) new_lepair);
    return;
  }
  else			/* otherwise sort it into list based on coords */
  {
    /* this is the infamous lexicographical sort a la mental */
    newpos = new_lepair->le[SolidA]->facevert->pos;
    this_lepair = sorted_lepairlist->first.lp;
    while (this_lepair != Nil)
    {
      thispos = this_lepair->le[SolidA]->facevert->pos;
      if (newpos[vx] > thispos[vx])
      {
	insert_elem(sorted_lepairlist,(elemptr) this_lepair,
		    (elemptr) new_lepair);
	return;
      }
      else if (newpos[vx] == thispos[vx])
      {
	while (this_lepair != Nil)
	{
	  if ((newpos[vy] > thispos[vy]) || (newpos[vx] > thispos[vx]))
	  {
	    insert_elem(sorted_lepairlist,(elemptr) this_lepair,
			(elemptr) new_lepair);
	    return;
	  }
	  else if (newpos[vy] == thispos[vy])
	  {
	    while (this_lepair != Nil)
	    {
	      if ((newpos[vz] > thispos[vz]) || (newpos[vy] > thispos[vy]) ||
		  (newpos[vx] > thispos[vx]))
	      {
		insert_elem(sorted_lepairlist,(elemptr) this_lepair,
			    (elemptr) new_lepair);
		return;
	      }
	      this_lepair = this_lepair->next;
	    }
	  }
	  if (this_lepair != Nil)
	    this_lepair = this_lepair->next;
	}
      }
      if (this_lepair != Nil)
	this_lepair = this_lepair->next;
    }
    append_elem(sorted_lepairlist, (elemptr) new_lepair); /* add on end */
  }
}

/* Given a paired null_lelist for both shells, sort it according to shell A */
/* (this is arbitrary, we could have used shell B). */

  listptr
sort_null_lelist(listptr null_lepairlist)
{
  lepairptr thislepair;
  listptr sorted_lepairlist = create_list();
  ;
  thislepair = null_lepairlist->first.lp;
  while (thislepair != Nil)
  {
    rem_elem(null_lepairlist, (elemptr) thislepair);
    sorted_insert_null_lepair(thislepair,sorted_lepairlist);
    thislepair = null_lepairlist->first.lp;
  }
  del_list(null_lepairlist);
  return (sorted_lepairlist);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*               NULL FACE CREATION/CONSOLIDATION ROUTINES                */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* This either connects a hole to an exterior null_le or two exterior/interior*/
/* null_le's together.  Note that if it finds that they are not in the same */
/* face it tries to connect with the Twin_le->next because this works in the */
/* case of holes that are actually now part of a null face because of a */
/* previous call to this routine. */

  void
connect_null_les(leptr null_le, leptr othernull_twin)
{
  boundptr firstbound;
  fveptr oldfve, newfve = Nil;
  ;
  /* note: we check to make sure these le's weren't already connected */
  firstbound = null_le->motherbound->motherfve->boundlist->first.bnd;
  if (null_le->motherbound != othernull_twin->motherbound)
  {			/* one of the les (or possibly both) is a hole */
    /* make sure the hole is the second parameter for krme */
    if (null_le->motherbound != firstbound)
      kill_ring_make_edge(othernull_twin,null_le->next);
    else
      kill_ring_make_edge(null_le->next,othernull_twin);
    if (null_le->prev->prev != othernull_twin)
      /* this arg order on mef insures that the next null_le will remain */
      /* in the old face and the null face will be the new face */
      make_edge_face(null_le,othernull_twin->next);
  }
  else
  {
    oldfve = null_le->motherbound->motherfve;
    if (othernull_twin->prev->prev != null_le)
      newfve = make_edge_face(null_le->next,othernull_twin);
    /* do a move_holes only once since the face you're creating has no */
    /* width anyway */
    if (newfve)
      move_holes(oldfve,newfve);
    if (null_le->prev->prev != othernull_twin)
      make_edge_face(null_le,othernull_twin->next);
  }
}

Boolean
isa_strut(leptr thisle)
{
  return (thisle->motherbound == Twin_le(thisle)->motherbound);
}

Boolean
isa_hole(leptr thisle)
{
  return (thisle->motherbound !=
	  thisle->motherbound->motherfve->boundlist->first.bnd);
}

  Boolean
inward_nullstrutle(leptr thisle)
{
  /* the strut points towards the boundary */
  return (thisle->prev->facevert == thisle->next->facevert);
}

  Boolean
outward_nullstrutle(leptr thisle)
{
  /* the strut points away from the boundary */
  return (thisle->facevert == thisle->next->next->facevert);
}

  Boolean
connectable_struts(leptr le1, leptr le2) /* le2 must be a strut */
{
  leptr thisle,le2_twin;
  ;
  if (le1->motherbound != le2->motherbound)
    system_error("connectable_struts: le2 not in same bound as le1");

  if (isa_strut(le1))		/* both are struts */
  {		/* if one points away from bound the other must point to it */
     return ((inward_nullstrutle(le1) && outward_nullstrutle(le2)) ||
	     (inward_nullstrutle(le2) && outward_nullstrutle(le1)));
  }
  else				/* only le2 is a strut */
  {
    le2_twin = Twin_le(le2);
    if (outward_nullstrutle(le2)) /* the strut points away from the boundary */
    {
      thisle = le1->next;
      do
      {
	if (thisle == le2_twin) return (FALSE);
	if (thisle == le2) return (TRUE);
	thisle = thisle->next;
      } while (thisle != le1);
      system_error("connectable_struts: outward strut not reached");
    }
    else if (inward_nullstrutle(le2)) /* the strut points towards boundary */
    {
      thisle = le1->prev;
      do
      {
	if (thisle == le2_twin) return (FALSE);
	if (thisle == le2) return (TRUE);
	thisle = thisle->prev;
      } while (thisle != le1);
      system_error("connectable_struts: inward strut not reached");
    }
    system_error("connectable_struts: strut fails both inward & outward tests");
  }
}

  Boolean
connectable(leptr le1, leptr le2)
{
  Boolean result1,result2;
  ;
  if ((le1->motherbound == Twin_le(le1)->motherbound) ||
      (le2->motherbound == Twin_le(le2)->motherbound))
  {
    if (le1->motherbound->motherfve != le2->motherbound->motherfve)
      return (FALSE);
    /* see if we are connecting to a hole...might also have to do some */
    /* special check for holes ...im not sure, but i dont think so */
    result1 = le1->motherbound != le2->motherbound;
    if (result1)
      return (TRUE);
    /* at least one of these les is a strut */
    if (isa_strut(le1))
      result1 = connectable_struts(le2,le1); /* make sure strut is second arg */
    else
      result1 = connectable_struts(le1,le2);
    return (result1);
  }
  else
  {
    result2 = le1->motherbound->motherfve == le2->motherbound->motherfve;
    return (result2);
  }
}

  lepairptr
getnext_connectable(listptr lepairlist, lepairptr thislepair,
		    lepairptr (*next_end)(), int shellindex)
{
  lepairptr otherlepair;
  leptr thisle;
  int othershell;
  Boolean result1,result2,result3;
  ;
  othershell = SolidB - shellindex;
  otherlepair = next_end(lepairlist, (elemptr) thislepair);
  thisle = thislepair->le[shellindex];
  while (otherlepair != thislepair)
  {
    result1 = connectable(thisle,Twin_le(otherlepair->le[shellindex]));
    result2 = connectable(thislepair->le[othershell],
			  Twin_le(otherlepair->le[othershell]));
    result3 = connectable(Twin_le(thislepair->le[othershell]),
			  otherlepair->le[othershell]);
    if (result1 && (result2 || result3))
      return (otherlepair);
    otherlepair = next_end(lepairlist,(elemptr) otherlepair);
  }

  return (Nil);
}

lepairptr
choose_connectable(leptr startle, lepairptr connectable1,
		   lepairptr connectable2, int shellindex)
{
  leptr thisle,twinle1,twinle2;
  ;
  thisle = startle;
  twinle1 = Twin_le(connectable1->le[shellindex]);
  twinle2 = Twin_le(connectable2->le[shellindex]);
  /* if both connectable are holes arbitrarily return one */
  if (isa_hole(twinle1) && isa_hole(twinle2))
    return(connectable1);
  do
  {
    if (thisle == twinle1)
    {
      if (inward_nullstrutle(startle))
	return (connectable1);
      else
	return (connectable2);
    }
    else if (thisle == twinle2)
    {
      if (inward_nullstrutle(startle))
	return (connectable2);
      else
	return (connectable1);
    }
    thisle = thisle->next;
  } while (thisle != startle);
  if (thisle == startle)
    system_error("choose_connectable: neither connectable reached");
}

lepairptr
first_connectable(listptr lepairlist, lepairptr startlepair,
		  lepairptr (**getnext_lepair)(), int shellindex)
{
  lepairptr endlepair, connectable1 = Nil;
  leptr startle;
  int nextcount = 0, prevcount = 0, othershell;
  Boolean result1,result2,result3;
  ;
  othershell = SolidB - shellindex;
  startle = startlepair->le[shellindex];
      
  endlepair = (lepairptr) next_elem(lepairlist,(elemptr) startlepair);

  while (endlepair != startlepair)
  {
    nextcount++;
    /* must make sure that lepairs are connectable in both shells */
    result1 = connectable(startle,Twin_le(endlepair->le[shellindex]));
    result2 = connectable(startlepair->le[othershell],
			  Twin_le(endlepair->le[othershell]));
    result3 = connectable(Twin_le(startlepair->le[othershell]),
			  endlepair->le[othershell]);
    if (result1 && (result2 || result3))
    {
      if (!isa_strut(startle) || isa_hole(startle))
	break;
      else		/* startle is a strut and all other lepairs are also */
	if (connectable1 == Nil)
	  connectable1 = endlepair;
	else
	{
	  endlepair = choose_connectable(startle,connectable1,endlepair,
					 shellindex);
	  break;
	}
    }
    endlepair = (lepairptr) next_elem(lepairlist, (elemptr) endlepair);
  }
  if (endlepair == startlepair)
    return (Nil);
  prevcount = lepairlist->numelems - nextcount;
  if (prevcount < nextcount)	/* see if the other way is closer */
    *getnext_lepair = (lepairptr (*)()) prev_elem;
  else
    *getnext_lepair = (lepairptr (*)()) next_elem;

  return (endlepair);
}

  void
make_solidA_nullfaces(lepairptr startlepair, listptr sorted_null_lelist,
		     listptr temp_null_lelist)
{
  lepairptr freelepair,connectable_lepair;
  lepairptr (*getnext_lepair)();
  ;
  if (startlepair == Nil)
    return;
  connectable_lepair = first_connectable(sorted_null_lelist,startlepair,
					 &getnext_lepair,SolidA);
  connect_null_les(startlepair->le[SolidA],
		   Twin_le(connectable_lepair->le[SolidA]));
  freelepair = connectable_lepair;
  do
  {
    connectable_lepair = getnext_connectable(sorted_null_lelist,freelepair,
					     getnext_lepair,SolidA);
    connect_null_les(freelepair->le[SolidA],
		     Twin_le(connectable_lepair->le[SolidA]));
    rem_elem(sorted_null_lelist, (elemptr) freelepair);
    append_elem(temp_null_lelist, (elemptr) freelepair);
    freelepair = connectable_lepair;
  } while (freelepair != startlepair);
  rem_elem(sorted_null_lelist, (elemptr) freelepair);
  add_elem(temp_null_lelist, Nil, (elemptr) freelepair);
}

  void
make_solidB_nullfaces(lepairptr startlepair, listptr sorted_null_lelist,
		     listptr temp_null_lelist)
{
  lepairptr freelepair,connectable_lepair;
  lepairptr (*getnext_lepair)();
  leptr startle;
  ;
  if (startlepair == Nil)
    return;
  startle = startlepair->le[SolidB];
    
  if (isa_strut(startle))
  {
    connectable_lepair = choose_connectable(startle,startlepair->next,
					    sorted_null_lelist->last.lp,SolidB);
    if (connectable_lepair == startlepair->next)
      getnext_lepair = (lepairptr (*) ()) next_elem;
    else
      getnext_lepair = (lepairptr (*) ()) prev_elem;
  }
  else
  {
    if (connectable(startlepair->le[SolidB],
		    Twin_le(startlepair->next->le[SolidB])))
      getnext_lepair = (lepairptr (*) ()) next_elem;
    else
      getnext_lepair = (lepairptr (*) ()) prev_elem;
    connectable_lepair = getnext_lepair(sorted_null_lelist,startlepair);
  }

  connect_null_les(startlepair->le[SolidB],
		   Twin_le(connectable_lepair->le[SolidB]));
  freelepair = connectable_lepair;
  do
  {
    connectable_lepair = getnext_lepair(sorted_null_lelist,freelepair);
    connect_null_les(freelepair->le[SolidB],
		     Twin_le(connectable_lepair->le[SolidB]));
    rem_elem(sorted_null_lelist, (elemptr) freelepair);
    append_elem(temp_null_lelist, (elemptr) freelepair);
    freelepair = connectable_lepair;
  } while (freelepair != startlepair);
  rem_elem(sorted_null_lelist, (elemptr) freelepair);
  add_elem(temp_null_lelist, Nil, (elemptr) freelepair);
}

lepairptr
find_nonstrut_lepair(listptr sorted_null_lelist, int shellindex)
{				/* find a non-strut null-le to start on */
  lepairptr thislepair;
  ;
  thislepair = sorted_null_lelist->first.lp;
  while(thislepair)
  {
    if (!isa_strut(thislepair->le[shellindex]))
      return (thislepair);
    thislepair = thislepair->next;
  }
  return (sorted_null_lelist->first.lp);
}

  void
make_null_faces_boolean(listptr sorted_null_lelist)
{
  listptr temp_null_lelist = create_list();
  listptr null_lelist_copy = create_list();
  lepairptr startlepair;
  Boolean done;
  ;
  while (sorted_null_lelist->numelems > 0)
  {
    startlepair = find_nonstrut_lepair(sorted_null_lelist,SolidA);
    make_solidA_nullfaces(startlepair,sorted_null_lelist,temp_null_lelist);
    startlepair = find_nonstrut_lepair(temp_null_lelist,SolidB);
    make_solidB_nullfaces(startlepair,temp_null_lelist,null_lelist_copy);
    if (temp_null_lelist->numelems != 0)
      system_error("make_null_faces_boolean: temp_null_lelist->numelems != 0");
  }
  merge_lists(null_lelist_copy,sorted_null_lelist);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*        FIX NULL LE RINGS WHERE LES GO IN->OUT INSTEAD OF OUT->IN       */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
find_null_le_rings(listptr null_lelist, listptr null_le_rings, int shellindex)
{
  lepairptr thislepair,nextlepair;
  listptr checked_list = create_list();
  leptr this_null_le;
  boundptr ringbound,nullbound,twinbound;
  ;
  thislepair = null_lelist->first.lp;
  while (thislepair)
  {
    this_null_le = thislepair->le[shellindex];
    ringbound = Nil;
    nullbound = Twin_le(this_null_le->prev)->motherbound;
    twinbound = Twin_le(Twin_le(this_null_le)->prev)->motherbound;
    /* if we have a null le ring remove all null les in that ring */
    /* so they arent rechecked (nullbound & twinbound cant both be holes) */
    if (nullbound != nullbound->motherfve->boundlist->first.bnd)
      ringbound = nullbound;
    else if (twinbound != twinbound->motherfve->boundlist->first.bnd)
      ringbound = twinbound;
    if (ringbound != Nil)	/* we have a null_le ring */
    {
      rem_elem(null_lelist, (elemptr) thislepair);
      append_elem(null_le_rings, (elemptr) thislepair);
      thislepair = null_lelist->first.lp;
      while (thislepair)	/* remove null_les in the same ring */
      {
	this_null_le = thislepair->le[shellindex];
	nextlepair = thislepair->next;
	if ((Twin_le(this_null_le->prev)->motherbound == ringbound) ||
	    (Twin_le(Twin_le(this_null_le)->prev)->motherbound == ringbound))
	{
	  rem_elem(null_lelist, (elemptr) thislepair);
	  append_elem(checked_list, (elemptr) thislepair);
	}
	thislepair = nextlepair;
      }
    }
    else
    {
      rem_elem(null_lelist, (elemptr) thislepair);
      append_elem(checked_list, (elemptr) thislepair);
    }
    thislepair = null_lelist->first.lp;
  }
  transfer_list_contents(checked_list,null_lelist);
  del_list(checked_list);
}

  void
find_interior_bisector(leptr le1, leptr le2, vertype bisector)
{
  float *facenorm,normdot;
  vertype vec1,vec2,vec1Xvec2,componenta,componentb;
  Boolean bound_is_ccw;
  ;
  /* see create_sectorlist for detailed comments */
  facenorm = le1->motherbound->motherfve->facenorm;
  makevec3d(le1->facevert->pos,le1->next->facevert->pos,vec1);
  makevec3d(le1->facevert->pos,le2->facevert->pos,vec2);
  normalize(vec1,vec1);
  normalize(vec2,vec2);
  cross_prod(vec1,vec2,vec1Xvec2);
  /* take the complement here since we know this is a hole */
  bound_is_ccw = !cclockwise_bound(le1->motherbound);
  if (vector_near_zero(vec1Xvec2,Zeroveclengthtol)) /* vec1&vec2 are colinear */
  {
    if (dotvecs(vec1,vec2) > 0.0) /* vectors point in same direction */
      system_error("create_sectorlist: vec1 && vec2 point the same way");
    cross_prod(vec1,facenorm,bisector);
    if (!bound_is_ccw)
      flip_vec(bisector);	    
  }
  else 
  {
    normdot = dotvecs(vec1Xvec2,facenorm);
    if (((normdot > 0.0) && bound_is_ccw) || ((normdot < 0.0) && !bound_is_ccw))
    {				/* we have a concavity */
      if (dotvecs(vec1,vec2) < 0.0) /* when angle on other side > 90, */
      {			/* to avoid zero vec bisector, use xprod */
	cross_prod(vec1,facenorm,componenta); /* with facenorm rather */
	cross_prod(facenorm,vec2,componentb); /* than vector addition */
	addvec3d(componenta,componentb,bisector);
      }
      else
	addvec3d(vec1,vec2,bisector);
      flip_vec(bisector);
    }
    else
      addvec3d(vec1,vec2,bisector);
  }
  normalize(bisector,bisector);
}

  Boolean
null_le_ring_ok(lepairptr thislepair, int shellindex)
{		/* check to see that null les in this ring point out->in */
  leptr this_null_le,le1,le2,otherle;
  vertype bisector;
  int othershell;
  fveptr othershellface;
  ;
  this_null_le = thislepair->le[shellindex];
  /* get 2 successive les in one of the faces adjoining null_le's nullface */
  le1 = Twin_le(this_null_le->prev);
  if (le1->motherbound == le1->motherbound->motherfve->boundlist->first.bnd)
    le1 = Twin_le(Twin_le(this_null_le)->prev);
  le2 = le1->prev;
  find_interior_bisector(le1,le2,bisector);
  /* see if interior bisector points the same way as other shells normals */
  othershell = SolidA + SolidB - shellindex;

  otherle = thislepair->le[othershell];
  othershellface = Twin_le(otherle->prev)->motherbound->motherfve;
  if (compare(dotvecs(othershellface->facenorm,bisector),0.0,1.0e-6) == 0)
    /* the face is parallel, try other face that adjoins otherles null face */
    othershellface = Twin_le(otherle->next)->motherbound->motherfve;

  if (le1 == Twin_le(this_null_le->prev))
    /* if bisector (and therefore the null le) and the face norms of the */
    /* other shell points in opposite directions the the null le IS pointing */
    /* out to in*/
    return (dotvecs(othershellface->facenorm,bisector) < 0.0);
  else	/* null le points from the hole to the containing face */
    /* and since bisector points from the containing face into the hole */
    /* we must reverse the test */
    return (dotvecs(othershellface->facenorm,bisector) > 0.0);
}

  void
flip_null_le_ring(lepairptr thislepair, int shellindex)
{
  leptr this_null_le,le1,le2;
  fveptr containingfve;
  ;
  this_null_le = thislepair->le[shellindex];
  le1 = Twin_le(this_null_le->prev);
  le2 = Twin_le(this_null_le->next);
  if (le1->motherbound == le1->motherbound->motherfve->boundlist->first.bnd)
  {
    le1 = Twin_le(Twin_le(this_null_le)->prev);
    le2 = Twin_le(Twin_le(this_null_le)->next);
  }
  containingfve = le1->motherbound->motherfve;
  make_face_kill_loop(le1->motherbound);
  kill_face_make_loop(le2->motherbound->motherfve,containingfve);
}

  void
fix_null_le_rings(listptr null_lelist, int shellindex)
{
  listptr null_le_rings = create_list();
  lepairptr thislepair;
  ;
  find_null_le_rings(null_lelist,null_le_rings,shellindex);
  thislepair = null_le_rings->first.lp;
  while (thislepair)
  {
    if (!null_le_ring_ok(thislepair,shellindex))
      flip_null_le_ring(thislepair,shellindex);
    thislepair = thislepair->next;
  }
  transfer_list_contents(null_le_rings,null_lelist);
  del_list(null_le_rings);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                            SNIP NULL LE'S                              */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* This snips all the null les, and if the null le is the last in a long */
/* null face around the shell then you can completely cut it off with kemr, */
/* yielding a shell, and the separation face for that shell is recorded */
/* in break_faces.  (hey, what did you eat for break_faces anyways?) */
/* Once null faces have been made, snipping the null_le's that make up */
/* their sides will separate off chunks of the solid, with the new resulting */
/* rings of the IN piece becoming rings of the face created in the OUT piece. */
/* These rings are recorded the break_faces list. */

  void
snip_null_le(lepairptr thislepair, int solidflag,listptr break_faces)
{
  elemptrptr nullfaceptr;
  leptr this_null_le;
  boundptr checkle1bound,checkle2bound;
  ;
  this_null_le = thislepair->le[solidflag];
  if (this_null_le->motherbound != Twin_le(this_null_le)->motherbound)
    kill_edge_face(this_null_le,Twin_le(this_null_le));
  else
  {			    /* kill the last edge holding 2 pieces together*/
    nullfaceptr = (elemptrptr) append_new_blank_elem(break_faces,Ep_type);
    nullfaceptr->thiselem = (elemptr) this_null_le->motherbound->motherfve;
    kill_edge_make_ring(this_null_le,Twin_le(this_null_le));
  }
}

/* NOTE that in the last case where we are snipping a null_le in shell B */
/* but it was not a hole we need to reverse it because the solidB null_le's */
/* go in the opposite direction as the solidB null_les but unlike the cases  */
/* of the hole null_le's (see above) determine_hole_direction() won't */
/* be called and won't take care of flipping the null_le properly.  In */
/* the case of non-hole null_le's however, it's always gonna be backwards. */
/* See set_from_to_les() for details. */

  void
consolidate_null_faces_boolean(listptr snip_null_lelist,
			       listptr outfacesA,listptr outfacesB)
{
  lepairptr thislepair;
  ;
  thislepair = snip_null_lelist->first.lp;
  while (thislepair != Nil)
  {
    snip_null_le(thislepair,SolidA,outfacesA);
    snip_null_le(thislepair,SolidB,outfacesB);
    thislepair = thislepair->next;
  }
}  

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*               SEPARATE PIECES BEFORE BOOLEAN RECOMBINATION             */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* Makes sure all holes are going the opposite direction as the face that */
/* contains them.  This should maybe be called on a per-face basis after mef.*/

  void
ensure_hole_orientations(shellptr thishell)
{
  fveptr thisfve;
  Boolean outerorient,innerorient;
  boundptr thisbound;
  ;
  for (thisfve = thishell->fvelist->first.fve; thisfve != Nil;
       thisfve = thisfve->next)
  {
    if (thisfve->boundlist->numelems > 1)
    {
      outerorient = cclockwise_bound(thisfve->boundlist->first.bnd);
      for (thisbound = thisfve->boundlist->first.bnd->next; thisbound != Nil;
	   thisbound = thisbound->next)
      {
	innerorient = cclockwise_bound(thisbound);
	if (innerorient == outerorient)
	{
	  printf("Will flip hole orientation here!!!\n");
	  flip_hole_orientation(thisbound);
	}
      }
    }
  }
}

  listptr
separate_pieces_boolean(shellptr thishell, listptr outfaces)
{
  elemptrptr thisinface,thisoutface;
  listptr infaces = Nil;
  elemptrptr check;
  ;
  infaces = separate_shell_into_pieces(thishell,outfaces);
  /* make out pieces into shells */
  thisoutface = outfaces->first.ep;
  while (thisoutface != Nil)
  {
    /* this should probably make piece into object to maintain convention */
    /* that outer shell is always first shell */
    /* actually to do things right we need to look at other shells and see */
    /* if the new shell encloses any others and make them secondary shells */
    /* insideshellist and outside shellist are remnants of slice and */
    /* arent used in booleans since we dont need to know which objects */
    /* were inside and which were outside */
    make_piece_into_shell(thishell, (fveptr) thisoutface->thiselem,Nil);
    thisoutface = thisoutface->next;
  }
  /* make in pieces into shells */
  thisinface = infaces->first.ep;
  while (thisinface != Nil)
  {
    make_piece_into_shell(thishell, (fveptr) thisinface->thiselem,Nil);
    thisinface = thisinface->next;
  }

  /* kill the original shell because now you should have both pieces left */
  /* over.  This prevents multiple walks in make_piece_into_shell producing */
  /* multiple copies of the same thing. */
  kill_shell(thishell);
  
  return(infaces);
}



/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*               FINAL BOOLEAN RECOMBINATION ROUTINES                     */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* Put shells back together according to the following boolean recombination*/
/* rules, where reverse() means flipvec'ing the face normals:                */
/* A union B =                 AoutB + BoutA                                 */
/* A intersect B =             AinB + BinA                                   */
/* A subtract B =              AoutB + reverse(BinA)                         */

  void
boolean_recombine(int operator,listptr infacesA,listptr outfacesA,
		  listptr infacesB,listptr outfacesB)
{
  elemptrptr thisinfaceArec,thisinfaceBrec,thisoutfaceArec,thisoutfaceBrec;
  elemptrptr thisdupinrecB,newdupinrecB;
  fveptr thisinfaceA,thisinfaceB,thisoutfaceA,thisoutfaceB;
  fveptr newinfaceB;
  shellptr shellA;
  listptr duplicate_inshellist;
  ;
  thisinfaceArec = infacesA->first.ep;
  thisinfaceBrec = infacesB->first.ep;
  thisoutfaceArec = outfacesA->first.ep;
  thisoutfaceBrec = outfacesB->first.ep;
  while (thisinfaceArec != Nil)
  {
    thisinfaceA = (fveptr) thisinfaceArec->thiselem;
    thisinfaceB = (fveptr) thisinfaceBrec->thiselem;
    thisoutfaceA = (fveptr) thisoutfaceArec->thiselem;
    thisoutfaceB = (fveptr) thisoutfaceBrec->thiselem;
    switch (operator)
    {
    case BOOL_Union:
      add_property((featureptr) thisinfaceA->mothershell,touched_prop);
      add_property((featureptr) thisinfaceB->mothershell,touched_prop);
      glue_objects_along_faces(thisoutfaceA,thisoutfaceB,FALSE);
      break;
    case BOOL_Intersection:
      /* Note: the big OUT parts can be deleted here. */
      add_property((featureptr) thisoutfaceA->mothershell,touched_prop);
      add_property((featureptr) thisoutfaceB->mothershell,touched_prop);
      glue_objects_along_faces(thisinfaceA,thisinfaceB,FALSE);
      break;
    case BOOL_Subtraction:		/* B loses parts that are inside A */
      /* Stupid way, assumes shellB has been dup'ed in do_boolean */
      add_property((featureptr) thisinfaceA->mothershell,touched_prop);
      add_property((featureptr) thisoutfaceB->mothershell,touched_prop);
      shellA = thisoutfaceA->mothershell;
      glue_objects_along_faces(thisinfaceB,thisoutfaceA,FALSE);
      break;
    }
    thisinfaceArec = thisinfaceArec->next;
    thisinfaceBrec = thisinfaceBrec->next;
    thisoutfaceArec = thisoutfaceArec->next;
    thisoutfaceBrec = thisoutfaceBrec->next;    
  }
}

  void
del_extra_shells(worldptr world)
{
  shellptr next_del_shell;
  ;
  next_del_shell = (shellptr) get_property_feature(world,touched_prop,1);
  while (next_del_shell != Nil)
  {
    kill_shell(next_del_shell);
    next_del_shell = (shellptr) get_property_feature(world,touched_prop,1);
  }    
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                     MAIN BOOLEAN OPERATOR ROUTINES                     */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* Do a boolean operation between two solids given a specific operator, */
/* ie Union, Subtraction or Difference. The resulting solid(s) will show */
/* up in the shell_list.  If you wanna preserve your originals, pass copies */
/* by calling duplicate_obj() beforehand. */

  listptr
do_boolean(objptr objA, objptr objB, int operation)
{
  listptr vv_list = create_list(), vf_list = create_list();
  listptr null_lelist = create_list(), sorted_null_lelist = 0;
  listptr infacesA, outfacesA = create_list();
  listptr infacesB, outfacesB = create_list();
  listptr resultlist = create_list();
  elemptrptr thisresult = 0;
  shellptr shellA= 0, shellB = 0,thishell = 0;
  objptr newobjA = 0, newobjB = 0;
  colortype objAcolor;
  ;
  if (objA->motherworld != objB->motherworld)
    system_error("do_boolean: objA and objB not in the same world");

  if (has_property((featureptr) objA,picked_prop))
    unpick_feature((featureptr) objA);
  if (has_property((featureptr) objB,picked_prop))
    unpick_feature((featureptr) objB);

  /* dup obj hack (putting B back together after splitting is HARD) */
  if (operation == BOOL_Subtraction)
  {
    newobjB = duplicate_obj(objB);
    /* duplicate of B will be a child of root...this is wrong we need to */
    /* move duped obj to the same level as original here */
    find_obj_normals(newobjB);
  }

  /* For now just do outer shells...eventually must do all shells */
  shellA = objA->shellist->first.shell;
  while ((shellA != Nil) && (shellA == objA->shellist->first.shell))
  {
    shellptr nextShellA = shellA->next;
    shellB = objB->shellist->first.shell;
    while ((shellB != Nil) && (shellB == objB->shellist->first.shell))
    {
      shellptr nextShellB = shellB->next;
      
      clear_property(touched_prop);   /* make sure nothing is Touched to start */

      ensure_hole_orientations(shellA);
      ensure_hole_orientations(shellB);
      
      compute_shell_planedists(shellA);
      compute_shell_planedists(shellB);
      
      boolean_preprocess(shellA,shellB,vv_list,vf_list);
      
      create_neighborhoodlists(vv_list,vf_list);
      /* process vertex neighborhoodlist here */
      classify_neighborhoods(shellA,shellB,operation,vv_list,vf_list);
      
      /* add null le's to objs and record them in the null_lelist's */
      add_all_null_edges_boolean(vv_list,vf_list,null_lelist);
      
      // Note: sort_null_lelist() deletes null_list, so we must
      // re-allocate it each iteration.
      sorted_null_lelist = sort_null_lelist(null_lelist);
      make_null_faces_boolean(sorted_null_lelist);
      
      fix_null_le_rings(sorted_null_lelist,SolidA);
      fix_null_le_rings(sorted_null_lelist,SolidB);
      
      consolidate_null_faces_boolean(sorted_null_lelist,outfacesA,outfacesB);
      
      // NOTE: separate_pieces_boolean() kills the shell!
      infacesA = separate_pieces_boolean(shellA,outfacesA);
      infacesB = separate_pieces_boolean(shellB,outfacesB);
      
      /* Decide what to combine with what and do the proper merging/loop */
      /* gluing. Then do joining, glueing according to the spec'd boolean op */
      
      boolean_recombine(operation,infacesA,outfacesA,infacesB,outfacesB);
      
      /* note that this will not free up the alloc_elem(alltypes) done on the */
      /* sortdata1 and sortdata2 fields */

      clear_property(nullfeature_prop);
      del_extra_shells(objA->motherworld);

      clear_list(vv_list);
      clear_list(vf_list);
      // null_lelist was killed above, need a new list for the next
      // iteration:
      null_lelist = create_list();
      clear_list(sorted_null_lelist);
      clear_list(outfacesA);
      clear_list(outfacesB);
      del_list(infacesA);
      del_list(infacesB);
      shellB = nextShellB;
    }
    shellA = nextShellA;
  }
  if (operation == BOOL_Subtraction)	/* wee hack to go along with previous dup hack */
  {
    kill_obj(objB);
    objB = Nil;			/* ok?! */

    // Update objA and add it to the results list
    log_update((featureptr) objA,Allupdateflags,Immediateupdate);
    find_obj_normals(objA);	/* hackkkkkkkkkkkkkkkk */
    get_feature_color((featureptr) objA, objAcolor);
    thisresult = (elemptrptr) append_new_blank_elem(resultlist,Ep_type);
    thisresult->thiselem = (elemptr) objA;

    /* objA could have multiple shells now...make them into objects */
    /* this should really check for containment of one shell in another */
    /* and only make objects out of shells that are not inside others */
    thishell = objA->shellist->first.shell->next; /* leave 1 shell in A */
    while (thishell)
    {
      newobjA = make_childobj_noshell(objA->parent);
      move_shell_to_obj(newobjA,thishell);
      set_feature_color((featureptr) newobjA,objAcolor);
      log_update((featureptr) newobjA,Allupdateflags,Immediateupdate);
      find_obj_normals(newobjA);	/* hackkkkkkkkkkkkkkkk */
      thisresult = (elemptrptr) append_new_blank_elem(resultlist,Ep_type);
      thisresult->thiselem = (elemptr) newobjA;
      thishell = thishell->next;
    }
  }
  else
  {
    log_update((featureptr) objA,Allupdateflags,Immediateupdate);
    find_obj_normals(objA);	/* hackkkkkkkkkkkkkkkk */
  }

  del_list(vv_list);
  del_list(vf_list);
  del_list(outfacesA);
  del_list(outfacesB);
  del_list(null_lelist);
  del_list(sorted_null_lelist);

  kill_obj_branch(newobjB);	/* HACKHACKHACKHACKHACKHACKHACKHACK */

  return (resultlist);
}
