
/* FILE: geometry.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Geometry Routines for the Cutplane Editor                    */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define GEOMETRYMODULE

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include <platform_gl.h>

 /* What can we do about this? these were irix-specific values. */
 /* E.g. Maxint, etc. */
#if 0
#include <values.h>
#endif

#include <cutplane.h>		/* cutplane.h includes geometry.h */

#include <math3d.h>
#include <pick.h>
 
/* LJE: should get_vertminmax and all those from vertex.c be in here? */


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                        LOW LEVEL SUPPORT ROUTINES                      */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
pickplane(vertype norm, int *axis1, int *axis2)
{
  int maxaxis = vz;
  int vzplus1 = vz + 1;
  ;
  if (Fabs(norm[vy]) > Fabs(norm[maxaxis]))
    maxaxis = vy;
  if (Fabs(norm[vx]) > Fabs(norm[maxaxis]))
    maxaxis = vx;

  *axis1 = (maxaxis + vy) % vzplus1;
  *axis2 = (maxaxis + vz) % vzplus1;

  if (norm[maxaxis] == 0.0)
    system_error("pickplane: magnitude of norm = 0.0");
}

leptr
find_noncollinear_le(leptr ref_le, int direction)
{
  leptr thisle;
  float *refpos;
  vertype refvec,norm,thisvec;
  ;
  refpos = ref_le->facevert->pos;
  makevec3d(refpos,ref_le->next->facevert->pos,refvec);
  if (direction == Prev)
  {
    thisle = ref_le->prev;
    do
    {
      makevec3d(refpos,thisle->facevert->pos,thisvec);
      if (!vector_near_zero(thisvec,1.0e-6))
      {
	cross_prod(refvec,thisvec,norm);
	if (!vector_near_zero(norm,1.0e-6))
	  return (thisle);
      }
      thisle = thisle->prev;
    } while (thisle != ref_le);
  }
  else
  {
    thisle = ref_le->next;
    do
    {
      makevec3d(refpos,thisle->facevert->pos,thisvec);

      if (!vector_near_zero(thisvec,1.0e-6))
      {
	cross_prod(refvec,thisvec,norm);
      	if (!vector_near_zero(norm,1.0e-6))
	  return (thisle);
      }
      thisle = thisle->next;
    } while (thisle != ref_le);
  }
  system_error("find_noncollinear_le: no noncollinear les found");
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                         NEARNESS TESTING ROUTINES                      */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  Boolean
pt_near_cutplane(vertype pt1,Coord zcut,float tol)
{
  return (Fabs(pt1[vz] - zcut) <= tol);
}

  Boolean
pt_near_pt(vertype sourcept,vertype checkpt,float toldist)
  /* is checkpt within tolerable picking */
  /* box of the source? Returns TRUE if yes. */
{
  fprintf(dbgdump, "pt_near_pt, comparing data at %p with data at %p\n", (void *) sourcept, (void *) checkpt);
  fflush(dbgdump);
  return ((Fabs(sourcept[vx] - checkpt[vx]) <= toldist) &&
	  (Fabs(sourcept[vy] - checkpt[vy]) <= toldist) &&
	  (Fabs(sourcept[vz] - checkpt[vz]) <= toldist));
}

  Boolean
pt_near_line(vertype pt0,vertype pt1,vertype chkpt,vertype nearpt,
	     float toldist)
  /* Returns TRUE if chkpt is near line segment (pt0,pt1).              */
  /* Line and point are assumed to be on a plane parallel to x-y plane. */
  /* Returns position of nearest point on line if the point is          */
  /* within the tolerance distance.                                     */
{
  float t,deltax,deltay,m,n,b,c;
  Boolean seg_dist;
  ;
  
  if (Fabs(pt1[vx] - pt0[vx]) < Tolerance) /* a vertical line? */
  {
    if (Fabs(chkpt[vx] - pt0[vx]) < toldist)
    {
      if ( ((pt0[vy] <= chkpt[vy]) && (pt1[vy] >= chkpt[vy])) ||
	  ((pt0[vy] >= chkpt[vy]) && (pt1[vy] <= chkpt[vy])) )
      {
        nearpt[vx] = pt0[vx];
        nearpt[vy] = chkpt[vy];
        nearpt[vz] = chkpt[vz];
        return (TRUE);
      }
    }
    else return (FALSE); /* vert, but not in y range */
  }
  else if (Fabs(pt1[vy] - pt0[vy]) < Tolerance) /* a horizontal line? */
  {
    if (Fabs(chkpt[vy] - pt0[vy]) < toldist)
    {
      if (((pt0[vx] <= chkpt[vx]) && (pt1[vx] >= chkpt[vx])) ||
          ((pt0[vx] >= chkpt[vx]) && (pt1[vx] <= chkpt[vx])))
      {
        nearpt[vx] = chkpt[vx];
        nearpt[vy] = pt0[vy];
        nearpt[vz] = chkpt[vz];
        return (TRUE);
      }
    }
    else return (FALSE); /* horiz, but not in x range */
  }
  else
  {
    /* do a bbox check */
    if ((((pt0[vx] + toldist) < chkpt[vx]) &&
	 ((pt1[vx] + toldist) < chkpt[vx])) ||
	(((pt0[vx] - toldist) > chkpt[vx]) &&
	 ((pt1[vx] - toldist) > chkpt[vx])) ||
	(((pt0[vy] + toldist) < chkpt[vy]) &&
	 ((pt1[vy] + toldist) < chkpt[vy])) ||
	(((pt0[vy] - toldist) > chkpt[vy]) &&
	 ((pt1[vy] - toldist) > chkpt[vy])))
      return(FALSE);
    
    deltax = pt1[vx] - pt0[vx];
    if (deltax == 0.0) exit(0);
    
    deltay = pt1[vy] - pt0[vy];
    if (deltay == 0.0) exit(0);
    
    if ((deltax != 0.0) && (deltay != 0.0))
    {
      m = deltay / deltax;     /* slope of segment */
      b = pt0[vy] - m * pt0[vx];            /* y-intersection of segment */
      n = - 1/m;                                      /* slope of normal */
      c = chkpt[vy] - n * chkpt[vx];        /* y-intersection of normal  */
      
      nearpt[vx] = (c - b) / (m - n);       /* coord of nearest point on */
      nearpt[vy] = (m * nearpt[vx]) + b;    /* segment to checkpt.       */
      nearpt[vz] = chkpt[vz];
      
      seg_dist = ((Fabs(chkpt[vx] - nearpt[vx]) < toldist) &&
                  (Fabs(chkpt[vy] - nearpt[vy]) < toldist));
      
      t = (nearpt[vx] - pt0[vx]) / deltax;  /* also check to see if near */
                                            /* point is on line segment  */

      if ((seg_dist) && (t >= 0.0) && (t <= 1.0)) 
        return (TRUE); /* if point was close enough to the line. */
    }
    else
      printf ("What the fuck is going on?");
  }
  return (FALSE);
}

  Boolean
pt_near_edge(vertype thispt, elemptr *nearelem, vertype nearest_pt,
	     listptr shellist)
  /* the point. */
  /* if smallest grainsize elem this pt is near,for now just vfes or evfs */
  /* the position of the nearest pt to cursor. */
  /* the shell list to search */
{
  vfeptr thisvfe;
  evfptr thisevf;
  shellptr thishell;
  float *pt1,*pt2;
  vertype intersectpt;
  ;
  *nearelem = Nil;
  /* hmmm...shouldn't we be returning a list of near edges & vfes instead of */
  /* the first one we find? */
  for (thishell = shellist->first.shell; thishell != Nil;
       thishell = thishell->next)
  {
    /* search for a vfe near thispt. */
    for (thisvfe = thishell->vfelist->first.vfe;
	 thisvfe != Nil; thisvfe = thisvfe->next)
    {
      pt1 = thisvfe->pos;
      if (pt_near_pt(pt1,thispt,Pkboxsize))
      {
	*nearelem = (elemptr) thisvfe;
	copypos3d(pt1,nearest_pt);
	return (TRUE);
      }
    }
    /* if not near vfe, check edges */
    for (thisevf = thishell->evflist->first.evf; thisevf != Nil;
	 thisevf = thisevf->next)
    {
      pt1 = thisevf->le1->facevert->pos;
      pt2 = thisevf->le2->facevert->pos;
      if (pt_near_line(pt1,pt2,thispt,intersectpt,Pkboxsize))
      {
	*nearelem = (elemptr) thisevf;
	copypos3d(intersectpt,nearest_pt);
	return (TRUE);
      }
    }
  }
  return (FALSE);
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                DISTANCE AND ANGLE MEASUREMENT ROUTINES                 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* Compute distance between a point and a line. */
  double
pt_to_line_dist_3d(vertype testpt,vertype pt1,vertype pt2)
{
  vertype fgh,nearpt;
  double denom,t;
  ;
  diffpos3d(pt1,pt2,fgh);
  denom = fgh[vx]*fgh[vx] + fgh[vy]*fgh[vy] + fgh[vz]*fgh[vz];
  if (divide_by_zero(denom))
    system_error("pt_to_line_dist_3d: denominator near zero!");
  else
  {
    normalize(fgh,fgh);
    t = fgh[vx]*(testpt[vx] - pt1[vx]) +
      fgh[vy]*(testpt[vy] - pt1[vy]) +
	fgh[vz]*(testpt[vz] - pt1[vz]);
    nearpt[vx] = pt1[vx] + t * fgh[vx];
    nearpt[vy] = pt1[vy] + t * fgh[vy];
    nearpt[vz] = pt1[vz] + t * fgh[vz];
    return(distance(testpt,nearpt));
  }
}

/* Compute distance between a point and a plane. Programmer's Geometry p 107 */

  double
pt_to_planedist_3d(vertype testpt,vertype plane, double planedist)
{
  return((double) dotvecs(plane,testpt) + planedist);
}

/* Compute angle between two planes. See Programmer's Geometry p 110*/

  double
angle_between_planes(vertype plane1,vertype plane2)
{
  double denom;
  ;
  denom = sqrt( (plane1[vx]*plane1[vx] +
		 plane1[vy]*plane1[vy] +
		 plane1[vz]*plane1[vz]) *
	        (plane2[vx]*plane2[vx] +
		 plane2[vy]*plane2[vy] +
		 plane2[vz]*plane2[vz]) );
  if (divide_by_zero(denom))
    system_error("angle_between_planes: denominator close to zero!");

  return( Rad2deg * Acos((double) dotvecs(plane1,plane2) / denom ));
}

/* Compute distance between two lines in space. */

  double
line_to_line_dist_3d(vertype pt1,vertype pt2,vertype pt3,vertype pt4)
{
  vertype fgh1,fgh2,pt31;
  double fg,gh,hf,denom;
  ;
  diffpos3d(pt1,pt2,fgh1);
  diffpos3d(pt3,pt4,fgh2);
  diffpos3d(pt3,pt1,pt31);
  fg = fgh1[vx]*fgh2[vy] - fgh2[vx]*fgh1[vy];
  gh = fgh1[vy]*fgh2[vz] - fgh2[vy]*fgh1[vz];
  hf = fgh1[vz]*fgh2[vx] - fgh2[vz]*fgh1[vx];

  denom = fg*fg + gh*gh + hf*hf;
  if (divide_by_zero(denom))
    /* lines are parallel, so call pt_to_line_dist */
    return(pt_to_line_dist_3d(pt1,pt3,pt4));
  else
    return(Fabs(pt31[vx]*gh + pt31[vy]*hf + pt31[vz]*fg)/sqrt(denom));
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                     3D NEARNESS TEST ROUTINES                          */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* Test whether a pt1 is within tol distance of pt2. */
/* Mantlya pg 220 */
  Boolean
pt_near_pt_3d(vertype pt1,vertype pt2, double tol)
{
  return( less_than_tol(distance2(pt1,pt2),tol*tol));
}

/* Check to see whether a vert is close to an line in space */
/* Mantlya pg 222 */
  Boolean
pt_near_line_3d(vertype pt1,vertype pt2,vertype testpt,vertype nearpt,
		double *t,double tol)
{
  vertype r1,r2;
  double r1mag;
  ;
  diffpos3d(pt1,pt2,r1);
  r1mag = dotvecs(r1,r1);
  if (r1mag < tol*tol)
  {				/* if given endpoints are within tol, just  */
    *t = 0.0;			/* check to see if testpt near one of them. */
    return (pt_near_pt_3d(pt1,testpt,tol)); /* (this is lame on mantlya's part) */
  }
  else
  {
    diffpos3d(pt1,testpt,r2);
    *t = dotvecs(r1,r2) / r1mag;
    nearpt[vx] = pt1[vx] + (*t) * r1[vx];
    nearpt[vy] = pt1[vy] + (*t) * r1[vy];
    nearpt[vz] = pt1[vz] + (*t) * r1[vz];
#if 0
    printf ("in pt_near_line_3d, t = %f\n",*t);
    printf ("in pt_near_line_3d, pt1,pt2 = ");
    printvert(pt1);
    printvert(pt2);
    printf ("r1mag = %f\n",r1mag);
    printf("\n\n");
#endif
    return (pt_near_pt_3d(nearpt,testpt,tol));
  }
}

/* Check to see whether a point is close to a line segment in space. */
/* Mantlya pg 222 */
  Boolean
pt_near_lineseg_3d(vertype pt1,vertype pt2,vertype testpt,vertype nearpt,
		   double *t,double tol)
{
  if (pt_near_line_3d(pt1,pt2,testpt,nearpt,t,tol))
/* old way
    if ((*t >= (-tol)) && (*t <= (1.0 + tol)))
      return(TRUE);
*/
    if ((*t >= 0.0) && (*t <= 1.0))
      return(TRUE);
  return(FALSE);
}

/* Assuming a point lies ON the line extended through pts 1 and 2, return TRUE*/
/* if it's between those two points. */

  Boolean
pt_on_lineseg_3d(vertype pt1,vertype pt2,vertype testpt)
{
  if ( (((pt1[vx] <= testpt[vx]) && (pt2[vx] >= testpt[vx])) ||
	((pt2[vx] <= testpt[vx]) && (pt1[vx] >= testpt[vx]))) &&
      (((pt1[vy] <= testpt[vy]) && (pt2[vy] >= testpt[vy])) ||
       ((pt2[vy] <= testpt[vy]) && (pt1[vy] >= testpt[vy]))) &&
      (((pt1[vz] <= testpt[vz]) && (pt2[vz] >= testpt[vz])) ||
       ((pt2[vz] <= testpt[vz]) && (pt1[vz] >= testpt[vz]))) )
    return(TRUE);
  return(FALSE);
}
     
/* Check to see whether a point is near to a plane in space. */

  Boolean
pt_near_plane_3d(vertype testpt,vertype planenorm,Coord planedist)
{
  return(less_than_tol(pt_to_planedist_3d(testpt,planenorm,planedist),
		       Ptonplanetol));
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                    Bounding Box Routines                               */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
copy_obj_bbox(objptr target_obj,bbox3dptr sourcebbox)
{
  target_obj->objbbox = *sourcebbox; /* assumes ANSI structure copy! */
}

  void
copy_group_bbox(objptr target_obj,bbox3dptr sourcebbox)
{
  target_obj->groupbbox = *sourcebbox; /* assumes ANSI structure copy! */
}

  void
compute_screen_bbox(bbox3dptr srcbbox,bboxscreenptr destbbox)
{
  screenpos corners[8];
  vertype corner;
  int minscrx,maxscrx,minscry,maxscry,i;
  ;
  setpos3d(corner,srcbbox->mincorner[vx],srcbbox->mincorner[vy],
	   srcbbox->mincorner[vz]);
  map2screen(corner,corners[0],FALSE);
  setpos3d(corner,srcbbox->mincorner[vx],srcbbox->maxcorner[vy],
	   srcbbox->mincorner[vz]);
  map2screen(corner,corners[1],FALSE);
  setpos3d(corner,srcbbox->maxcorner[vx],srcbbox->mincorner[vy],
	   srcbbox->mincorner[vz]);
  map2screen(corner,corners[2],FALSE);
  setpos3d(corner,srcbbox->maxcorner[vx],srcbbox->maxcorner[vy],
	   srcbbox->mincorner[vz]);
  map2screen(corner,corners[3],FALSE);

  setpos3d(corner,srcbbox->mincorner[vx],srcbbox->mincorner[vy],
	   srcbbox->maxcorner[vz]);
  map2screen(corner,corners[4],FALSE);
  setpos3d(corner,srcbbox->mincorner[vx],srcbbox->maxcorner[vy],
	   srcbbox->maxcorner[vz]);
  map2screen(corner,corners[5],FALSE);
  setpos3d(corner,srcbbox->maxcorner[vx],srcbbox->mincorner[vy],
	   srcbbox->maxcorner[vz]);
  map2screen(corner,corners[6],FALSE);
  setpos3d(corner,srcbbox->maxcorner[vx],srcbbox->maxcorner[vy],
	   srcbbox->maxcorner[vz]);
  map2screen(corner,corners[7],FALSE);
  
  /* sort the bbox to create a screen bbox. */
  i = 0;
#ifdef IRIS
  maxscrx = -MAXINT;  minscrx = MAXINT;
  maxscry = -MAXINT;  minscry = MAXINT;
#else
  maxscrx = -INT_MAX;  minscrx = INT_MAX;
  maxscry = -INT_MAX;  minscry = INT_MAX;
#endif

  while (i < 8)
  {
    if (corners[i][vx] > maxscrx)
      maxscrx = corners[i][vx];
    if (corners[i][vx] < minscrx)
      minscrx = corners[i][vx];
    if (corners[i][vy] > maxscry)
      maxscry = corners[i][vy];
    if (corners[i][vy] < minscry)
      minscry = corners[i][vy];
    i++;
  }
  destbbox->mincorner[vx] = minscrx;   destbbox->maxcorner[vx] = maxscrx;
  destbbox->mincorner[vy] = minscry;   destbbox->maxcorner[vy] = maxscry;
}

/* compute a screen bbox around an object's bbox (objbbox or groupbbox, */
/* depending on which one you pass) including pick boxes an other stuff */

  void
compute_object_screenbbox(bbox3dptr solid_bbox,bboxscreenptr objectscreenbbox,
			  Coord object_margin)
{
  bbox3dtype object_bbox;
  vertype expansion;
  ;
  setpos3d(object_bbox.mincorner,solid_bbox->mincorner[vx],
	   solid_bbox->mincorner[vy], solid_bbox->mincorner[vz]);
  setpos3d(expansion,-object_margin,-object_margin,-object_margin);
  addpos3d(object_bbox.mincorner,expansion,object_bbox.mincorner);
  setpos3d(object_bbox.maxcorner,solid_bbox->maxcorner[vx],
	   solid_bbox->maxcorner[vy], solid_bbox->maxcorner[vz]);
  setpos3d(expansion,object_margin,object_margin,object_margin);
  addpos3d(object_bbox.maxcorner,expansion,object_bbox.maxcorner);
  compute_screen_bbox(&object_bbox,objectscreenbbox);
}

/* compute a screen bbox around an object's shadow by projecting the 3D bbox */
/* to the floor of the room, and then call compute_screen_bbox above. */

  void
compute_shadow_screenbbox(bbox3dptr solid_bbox,bboxscreenptr shadowscreenbbox,
			  Coord shadow_margin)
{
  bbox3dtype shadow_bbox;
  vertype expansion;
  ;
  setpos3d(shadow_bbox.mincorner,solid_bbox->mincorner[vx],Mincrossy,
	   solid_bbox->mincorner[vz]);
  setpos3d(expansion,-shadow_margin,-shadow_margin,-shadow_margin);
  addpos3d(shadow_bbox.mincorner,expansion,shadow_bbox.mincorner);
  setpos3d(shadow_bbox.maxcorner,solid_bbox->maxcorner[vx],Mincrossy,
	   solid_bbox->maxcorner[vz]);
  setpos3d(expansion,shadow_margin,shadow_margin,shadow_margin);
  addpos3d(shadow_bbox.maxcorner,expansion,shadow_bbox.maxcorner);
  compute_screen_bbox(&shadow_bbox,shadowscreenbbox);
}


  int
bboxcode(vertype thept, bbox3dptr thebbox)
{
  int result = 0;
  ;
  if (thept[vx] < thebbox->mincorner[vx])
    result = Leftofbbox;
  else if (thept[vx] > thebbox->maxcorner[vx])
    result = Rightofbbox;

  if (thept[vy] < thebbox->mincorner[vy])
    result |= Bottomofbbox;
  else if (thept[vy] > thebbox->maxcorner[vy])
    result |= Topofbbox;

  if (thept[vz] < thebbox->mincorner[vz])
    result |= Backofbbox;
  else if (thept[vz] > thebbox->maxcorner[vz])
    result |= Frontofbbox;

  return (result);
}

void
find_edge_bbox3d(float *pt1, float *pt2, bbox3dptr thebbox)
{
  int i;
  ;
  for (i=vx; i<vw; i++)
  {
    if (pt1[i] > pt2[i])
    {
      thebbox->mincorner[i] = pt2[i];
      thebbox->maxcorner[i] = pt1[i];
    }
    else
    {
      thebbox->mincorner[i] = pt1[i];
      thebbox->maxcorner[i] = pt2[i];
    }
  }
}

Boolean
bbox_intersect3d(bbox3dptr bbox1, bbox3dptr bbox2)
{
  Boolean result1,result2;
  vertype mintestpt,maxtestpt;
  static float fuzzfactor = 1.0e-4;
  ;
  copypos3d(bbox1->mincorner,mintestpt);
  copypos3d(bbox1->maxcorner,maxtestpt);

  /* add some fuzz */
  mintestpt[vx] -= fuzzfactor;
  mintestpt[vy] -= fuzzfactor;
  mintestpt[vz] -= fuzzfactor;

  maxtestpt[vx] += fuzzfactor;
  maxtestpt[vy] += fuzzfactor;
  maxtestpt[vz] += fuzzfactor;

  result1 = bboxcode(mintestpt,bbox2);
  result2 = bboxcode(maxtestpt,bbox2);

  /* if (result1 & result2) != 0) box1 is completely outside of box2 */
  /* otherwise the boxes intersect */

  return ((result1 & result2) == 0);
}


  int
bboxscreencode(screenpos thept, bboxscreenptr thebbox)
{
  int result = 0;
  ;
  if (thept[vx] < thebbox->mincorner[vx])
    result = Leftofbbox;
  else if (thept[vx] > thebbox->maxcorner[vx])
    result = Rightofbbox;

  if (thept[vy] < thebbox->mincorner[vy])
    result |= Bottomofbbox;
  else if (thept[vy] > thebbox->maxcorner[vy])
    result |= Topofbbox;

  return (result);
}

  Boolean
bbox_intersect_screen(bboxscreenptr bbox1,bboxscreenptr bbox2)
{
  Boolean result1,result2;
  screenpos mintestpt,maxtestpt;
  ;
  /* make sure test points for bbox1 are at z == 0 */
  mintestpt[vx] = bbox1->mincorner[vx];
  mintestpt[vy] = bbox1->mincorner[vy];
  maxtestpt[vx] = bbox1->maxcorner[vx];
  maxtestpt[vy] = bbox1->maxcorner[vy];

  result1 = bboxscreencode(mintestpt,bbox2);
  result2 = bboxscreencode(maxtestpt,bbox2);

  /* if (result1 & result2) != 0) box1 is completely outside of box2 */
  /* otherwise the boxes intersect */

  return ((result1 & result2) == 0);
}
  

  Boolean
point_in_screenbbox(screenpos checkpt, bboxscreenptr thisbbox)
{
  if ((checkpt[vx] >= thisbbox->mincorner[vx]) &&
      (checkpt[vx] <= thisbbox->maxcorner[vx]) &&
      (checkpt[vy] <= thisbbox->maxcorner[vy]) &&
      (checkpt[vy] >= thisbbox->mincorner[vy]))
    return(TRUE);
  return(FALSE);
}

  Boolean
point_in_2dbbox(vertype checkpt, bbox2dptr thisbbox)
{
  if ((checkpt[vx] >= thisbbox->mincorner[vx]) &&
      (checkpt[vx] <= thisbbox->maxcorner[vx]) &&
      (checkpt[vy] <= thisbbox->maxcorner[vy]) &&
      (checkpt[vy] >= thisbbox->mincorner[vy]))
    return(TRUE);
  return(FALSE);
}

  Boolean
point_in_3dbbox(vertype checkpt,bbox3dptr thisbbox)
{
  return( ((checkpt[vx] >= thisbbox->mincorner[vx]) &&
	   (checkpt[vx] <= thisbbox->maxcorner[vx]) &&
	   (checkpt[vy] >= thisbbox->mincorner[vy]) &&
	   (checkpt[vy] <= thisbbox->maxcorner[vy]) &&
	   (checkpt[vz] >= thisbbox->mincorner[vz]) &&
	   (checkpt[vz] <= thisbbox->maxcorner[vz])) );
}

#ifdef waksway
  int
bboxcode(vertype thept,bbox3dtype thisbbox)
{
  int result1,result2,result3;
  ;
  result1 = (((thept)[vx] <= thisbbox.mincorner[vx]) ? Leftofbbox :
	     (((thept)[vx] >= thisbbox.maxcorner[vx]) ? Rightofbbox : 0));
  result2 = (((thept)[vy] <= thisbbox.mincorner[vy]) ? Bottomofbbox :
	     (((thept)[vy] >= thisbbox.maxcorner[vy]) ? Topofbbox : 0));
  result3 = (((thept)[vz] <= thisbbox.mincorner[vz]) ? Backofbbox :
	     (((thept)[vz] >= thisbbox.maxcorner[vz]) ? Frontofbbox : 0));
  return (result1 | result2 | result3);
}
#endif

  Boolean
lineseg_bbox_intersect(vertype pt1,vertype pt2,vertype pt3,vertype pt4)
{
  bbox3dtype segbbox;
  int result1,result2;
  ;
  copypos3d(pt1,segbbox.maxcorner);
  copypos3d(pt2,segbbox.mincorner);
  if (pt2[vx] > pt1[vx])
    swap_coords(&(segbbox.maxcorner[vx]),&(segbbox.mincorner[vx]));
  if (pt2[vy] > pt1[vy])
    swap_coords(&(segbbox.maxcorner[vy]),&(segbbox.mincorner[vy]));
  if (pt2[vz] > pt1[vz])
    swap_coords(&(segbbox.maxcorner[vz]),&(segbbox.mincorner[vz]));
  result1 = bboxcode(pt3,&segbbox);
  result2 = bboxcode(pt4,&segbbox);
  return ((result1 > 0) && (result2 > 0));
}

/* set the face bbox (using the outerbound only, assuming it contains the */
/* inner bounds... let's hope the user hasn't screwed the obj up too much */

  void
set_3d_obj_facebboxes(objptr thisobj)
{
  shellptr thishell;
  fveptr thisfve;
  leptr minle,maxle;
  boundptr outerbnd;
  ;
  thishell = First_obj_shell(thisobj);
  while (thishell != Nil)
  {
    thisfve = First_shell_fve(thishell);
    while (thisfve != Nil)
    {
      outerbnd = thisfve->boundlist->first.bnd;
      get_leminmax(vx,outerbnd,&minle,&maxle);
      thisfve->facebbox.mincorner[vx] = minle->facevert->pos[vx];
      thisfve->facebbox.maxcorner[vx] = maxle->facevert->pos[vx];

      get_leminmax(vy,outerbnd,&minle,&maxle);
      thisfve->facebbox.mincorner[vy] = minle->facevert->pos[vy];
      thisfve->facebbox.maxcorner[vy] = maxle->facevert->pos[vy];
      
      get_leminmax(vz,outerbnd,&minle,&maxle);
      thisfve->facebbox.mincorner[vz] = minle->facevert->pos[vz];
      thisfve->facebbox.maxcorner[vz] = maxle->facevert->pos[vz];
      
      thisfve->facebbox.mincorner[vw] = 1.0;
      thisfve->facebbox.maxcorner[vw] = 1.0;

      thisfve = thisfve->next;
    }
    thishell = thishell->next;
  }
}

  void
set_3d_objbbox(objptr thisobj)
{
  vfeptr minvfe,maxvfe;
  /* Axismax: bold as love ("Cut it out 99! */
  /* "Can't you see I've got a call on my shoe phone??") */
  /* Always use first shell to make obj bbox since it is assumed to */
  /* be the largest all-encompassing shell.  my son.  the outermost one. dude.*/

  if (thisobj->shellist->numelems == 0)
  {
    printf("Warning warning warning in set_3d_objbbox: no shells\n");
    return;			/* this object created with a no_shell routine */
  }
  
  get_vertminmax(vx,First_obj_shell(thisobj)->vfelist,&minvfe,&maxvfe);
  thisobj->objbbox.mincorner[vx] = minvfe->pos[vx];
  thisobj->objbbox.maxcorner[vx] = maxvfe->pos[vx];

  get_vertminmax(vy,First_obj_shell(thisobj)->vfelist,&minvfe,&maxvfe);
  thisobj->objbbox.mincorner[vy] = minvfe->pos[vy];
  thisobj->objbbox.maxcorner[vy] = maxvfe->pos[vy];

  get_vertminmax(vz,First_obj_shell(thisobj)->vfelist,&minvfe,&maxvfe);
  thisobj->objbbox.mincorner[vz] = minvfe->pos[vz];
  thisobj->objbbox.maxcorner[vz] = maxvfe->pos[vz];

  thisobj->objbbox.mincorner[vw] = 1.0;
  thisobj->objbbox.maxcorner[vw] = 1.0;

#if 0				/* hacked out until we're ready */
  /* set face bboxes for drawing purposes */
  set_3d_obj_facebboxes(thisobj);
#endif
}

  void
set_3d_branchbboxes(objptr thisobj)
{
  apply_to_descendants(thisobj,set_3d_objbbox, Nil);
}

  void
grow_2d_bbox(bbox2dptr newbbox, bbox2dptr growingbbox)
{
  if (newbbox->mincorner[vx] < growingbbox->mincorner[vx])
    growingbbox->mincorner[vx] = newbbox->mincorner[vx];
  if (newbbox->maxcorner[vx] > growingbbox->maxcorner[vx])
    growingbbox->maxcorner[vx] = newbbox->maxcorner[vx];


  if (newbbox->mincorner[vy] < growingbbox->mincorner[vy])
    growingbbox->mincorner[vy] = newbbox->mincorner[vy];
  if (newbbox->maxcorner[vy] > growingbbox->maxcorner[vy])
    growingbbox->maxcorner[vy] = newbbox->maxcorner[vy];
}

  void
grow_3d_bbox(bbox3dptr newbbox, bbox3dptr growingbbox)
{
  if (newbbox->mincorner[vx] < growingbbox->mincorner[vx])
    growingbbox->mincorner[vx] = newbbox->mincorner[vx];
  if (newbbox->maxcorner[vx] > growingbbox->maxcorner[vx])
    growingbbox->maxcorner[vx] = newbbox->maxcorner[vx];


  if (newbbox->mincorner[vy] < growingbbox->mincorner[vy])
    growingbbox->mincorner[vy] = newbbox->mincorner[vy];
  if (newbbox->maxcorner[vy] > growingbbox->maxcorner[vy])
    growingbbox->maxcorner[vy] = newbbox->maxcorner[vy];


  if (newbbox->mincorner[vz] < growingbbox->mincorner[vz])
    growingbbox->mincorner[vz] = newbbox->mincorner[vz];
  if (newbbox->maxcorner[vz] > growingbbox->maxcorner[vz])
    growingbbox->maxcorner[vz] = newbbox->maxcorner[vz];
}

#define Newgroupmethod

#ifdef Newgroupmethod

/* compute all the group bboxes by going down from the ancestor */
/* by traversing the tree postfix-style and setting the group bboxes records */
/* as you walk back up the tree. */
/* NOTA BENE: obj bboxes MUST be set prior to calling this routine. */


  bbox3dptr 
grow_group_bbox(objptr thisobj)
{
  objptr thischild;
  ;
  thisobj->groupbbox = thisobj->objbbox; /* unportable ANSI structure copy! */
  thischild = thisobj->children->first.obj;
  while (thischild != Nil)
  {
    grow_3d_bbox(grow_group_bbox(thischild),&(thisobj->groupbbox));
    thischild = thischild->next;
  }
  return(&(thisobj->groupbbox));
}

#else

  static void
grow_group_bbox_engine(objptr thisobj, bbox3dptr rootbbox)
{
  shellptr thishell;
  ;
  thishell = First_obj_shell(thisobj);
  while (thishell != Nil)
  {
    grow_3d_bbox(&(thisobj->objbbox),rootbbox);
    thishell = thishell->next;
  }
}

  void
grow_group_bbox(objptr thisobj)
{
  setpos3d(thisobj->groupbbox.mincorner,FLT_MAX,FLT_MAX,FLT_MAX);
  setpos3d(thisobj->groupbbox.maxcorner,-FLT_MAX,-FLT_MAX,-FLT_MAX);
  apply_to_descendants_with_prop(thisobj,visible_prop,grow_group_bbox_engine,
				 (void *) &(thisobj->groupbbox));
}

#endif

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                    Containment Test Routines                           */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* return 1 if point is to the left of the edge line segment */
/* Note: no longer handles case where point actually on the edge within  */
/* small tolerance because that is supposed to be handled by input_line() */
/* (see below). */

  unsigned int	
int_ray_edge(vertype facept1, vertype facept2, vertype raypt,
	     int rayaxis, int axis2)
{
  Coord m,b,xintr;
  ;
  if (facept1[axis2] == facept2[axis2])
    return(0);			/* don't count horizontal edges at all */

  if (((facept1[axis2] > raypt[axis2]) && (facept2[axis2] > raypt[axis2])) ||
      ((facept1[axis2] < raypt[axis2]) && (facept2[axis2] < raypt[axis2])))
    return(0);			/* edge is completely above or below ray */

  if ((facept1[rayaxis] < raypt[rayaxis]) && (facept2[rayaxis] < raypt[rayaxis]))
    return(0); 	/* edge is completely to the left of raypt: no intersectioon */

  if (((facept1[axis2] == raypt[axis2]) && facept1[rayaxis] == raypt[rayaxis]) ||
      ((facept2[axis2] == raypt[axis2]) && facept2[rayaxis] == raypt[rayaxis]))
    return(0);		/* an end point of the edge is coincident with raypt */

  if ((facept1[rayaxis] > raypt[rayaxis]) && (facept2[rayaxis] > raypt[rayaxis]))
  {				/* edge is completely to the right of raypt */
    if ((facept1[axis2] == raypt[axis2]) || (facept2[axis2] == raypt[axis2]))
      return(1);		/* one of the edge's endpts was on ray */
    return(2);			/* the edge intersects the ray */
  }

  if ((facept1[axis2] == raypt[axis2]) || (facept2[axis2] == raypt[axis2]))
    return(1); /* edge is not completely to right but */
	       /* one of the edge's endpts was on ray */

  if (facept1[rayaxis] == facept2[rayaxis]) /* handle vertical edge exception */
    xintr = facept1[rayaxis];
  else
  {
    /* slope of a line: m = (y2-y1)/(x2-x1) for points 1 & 2 on the line */
    m = (facept2[axis2]-facept1[axis2]) / (facept2[rayaxis]-facept1[rayaxis]);
    
    /* y-intercept of a line: b = y1 - m*x1 for point 1 on the line */
    /* intersection of horizontal ray and edge: x = (yray - b)/m */
    xintr = (raypt[axis2] - facept1[axis2])/m + facept1[rayaxis];
  }
  /* if intersection to right of raypt it was intersected. */
  if (xintr > raypt[rayaxis])	/* ray crosses this edge */
    return(2);
  else				/* ray doesn't cross edge */
    return(0);
}

  Boolean
point_in_bound(boundptr thisbound, vertype testpt)
{
  leptr firstle,thisle,markerle;
  unsigned int numedges = 0;   /* incremental count of ray-edge intersections */
  float *edgept1,*edgept2;
  Boolean done = FALSE;
  int returnval,rayaxis,onboundflag;
  ;
/* for now don't do this
  if ((onboundflag = pt_on_bound_3d(testpt,thisbound,tol)) != Ptnotonbound)
    return(FALSE);
*/
  rayaxis = vx;
  thisle = firstle = thisbound->lelist->first.le;
  do
  {
    edgept1 = thisle->facevert->pos;
    edgept2 = thisle->next->facevert->pos;
    returnval = int_ray_edge(edgept1,edgept2,testpt,rayaxis,vy);
    if (returnval == 1)		/* endpt of this edge lies on ray */
    {
      if (edgept2[vy] == testpt[vy]) /* only look at when pt2 is on ray*/
      {
	/* markerle is used to check for inf. loops. cant use firstle since */
	/* we may have started in middle of horizontal edge span...and */
	/* therefore would want to continue past it */
	markerle = thisle;
	do	/* skip edges that lie on ray */
	{
	  thisle = thisle->next;
	  if (thisle == firstle)
	    done = TRUE;
	} while ((edgept2[vy] == thisle->next->facevert->pos[vy]) &&
		 (thisle != markerle));

	if (thisle == markerle)
	  system_error("pt_in_bound_3d: could not find le off of ray");

	/* check to see if bound crosses ray */
	if (fsign(thisle->next->facevert->pos[vy] - edgept2[vy]) ==
	    fsign(edgept1[vy] - edgept2[vy]))
	  returnval = 0;
	else
	  returnval = 2;
      }
      else
      {
	returnval = 0;
	thisle = thisle->next;
      }
    }
    else
      thisle = thisle->next;

    numedges += returnval;
  } while ((thisle != firstle) && !done);

  /* divide by 2 since int_ray_edge returns twice the number of intersects */
  numedges = numedges >> 1;
  if ((numedges % 2)	== 0)	/* even number of edge intersections is bad!*/
    return(FALSE);
  return(TRUE);			/* odd number of edge intersections is good! */
}

  Boolean
pt_in_face(fveptr thisfve, vertype thepoint)	/* looks at all boundaries */
{
  leptr thisle;
  int numbounds = 0;
  boundptr thisbound;
  ;
  for (thisbound = thisfve->boundlist->first.bnd; thisbound != Nil;
       thisbound = thisbound->next)
  {
    if (point_in_bound(thisbound,thepoint))
      ++numbounds;
  }
  return ((numbounds % 2) != 0);	/* returns true if odd num of intrx. */
}

  Boolean
point_in_face(fveptr thisfve, vertype thepoint)
{				/* only looks at outer boundary */
  leptr firstle,thisle;
  unsigned int numedges = 0;   /* incremental count of ray-edge intersections */
  float *edgept1,*edgept2;
  ;
  firstle = thisfve->boundlist->first.bnd->lelist->first.le;
  thisle = firstle;
  do
  {
    edgept1 = thisle->facevert->pos;
    edgept2 = thisle->next->facevert->pos;
    numedges += int_ray_edge(edgept1,edgept2,thepoint,vx,vy);
    thisle = thisle->next;
  } while (thisle != firstle);
  /* divide by 2 since int_ray_edge returns twice the number of intersects */
  numedges = numedges >> 1;

  /* even number of edge intersections is bad! (point is not in face) */
  /* odd number of edge intersections is good! (point is in face) */

  return ((numedges % 2) != 0);
}

#if 0
  Boolean
cursor_in_shell(shellptr thishell, stateptr state)
{
  segmentptr thisegment;
  ringptr thisring;
  vertype pt1,pt2;
  int numsegs = 0;
  ;
  thisring = thishell->ringlist->first.ring;
  while(thisring)
  {
    if (point_in_2dbbox(state->cursor, &(thisring->ringbbox)))
    {
      thisegment = thisring->seglist->first.seg;
      while(thisegment)
      {
	if (thisegment->inplanefve == Nil)
	{
	  if (thisegment->inplanevfe != Nil)
	    copypos3d(thisegment->inplanevfe->pos,pt1);
	  else
	  {
	    pt1[vx] = thisegment->intersectevf->cutpt[vx];
	    pt1[vy] = thisegment->intersectevf->cutpt[vy];
	    pt1[vz] = state->cursor[vz];
	  }
	  if (thisegment->inplanevfe2 != Nil)
	    copypos3d(thisegment->inplanevfe2->pos,pt2);
	  else
	  {
	    pt2[vx] = thisegment->intersectevf2->cutpt[vx];
	    pt2[vy] = thisegment->intersectevf2->cutpt[vy];
	    pt2[vz] = state->cursor[vz];
	  }
	  numsegs += int_ray_edge(pt1,pt2,state->cursor,vx,vy);
	}
	thisegment = thisegment->next;
      }
    }
    thisring = thisring->next;
  }
  /* int_ray_edge returns twice the num of intersections */
  numsegs = numsegs >> 1;

  return ((numsegs % 2) != 0);
}
#endif

  Boolean		/* checks for fve2 in fve1.  Slow and stupid, */
bound_in_bound(boundptr bound1, boundptr bound2)
  /* but it works... can you say that of YOUR */
  /* algorithm, huh,huh,punk? */
{
  leptr bound1le,bound2le,first_innerle,first_outerle;
  vertype dummyvert;
  ;
  first_innerle = bound2->lelist->first.le;
  first_outerle = bound1->lelist->first.le;
  bound1le = first_outerle;
  do
  {
    bound2le = first_innerle;
    do
    {
      if (!(point_in_bound(bound1,bound2le->facevert->pos)))
	return(FALSE);		/* All points must be in the outer face! */
      bound2le = bound2le->next;
    } while (bound2le != first_innerle);
    bound1le = bound1le->next;
  } while (bound1le != first_outerle);
  return (TRUE);		/* all points were inside, okey dokey. */
}

/* return the smallest and the largest surrounding faces of the face */
/* new_face. */

  int
face_in_bounds(shellptr new_shell,  boundptr *tight_bound, boundptr *loose_bound,
	       listptr drawshellist)
{
  shellptr thishell;
  fveptr thisfve;
  boundptr this_hole,newbound,thisbound;
  int facecnt = 0;
  ;
  *tight_bound = Nil;
  *loose_bound = Nil;
  newbound = new_shell->fvelist->first.fve->boundlist->first.bnd;
  for (thishell = drawshellist->first.shell; thishell != Nil; 
       thishell = thishell->next)
  {
    if (thishell != new_shell)	/* don't check the new face against itself */
    {
      for (thisfve = thishell->fvelist->first.fve; thisfve != Nil;
	   thisfve = thisfve->next)
      {
	thisbound = thisfve->boundlist->first.bnd;
	if (bound_in_bound(thisbound,newbound))
	{
	  facecnt++;
	  if ((*tight_bound == Nil) || (bound_in_bound(*tight_bound,thisbound)))
	  {
	    *loose_bound = *tight_bound; /* the next tightest bound. */
	    *tight_bound = thisbound; /* if thisfve inside last one found */
	    if (*loose_bound == Nil)
	      *loose_bound = thisbound;	/* well hackity hack hack to you! */
	  }
	  /* assign the tight one to this latest one */
	  /* and assign the loose one to the tight one */
	  /* if the tight one is outside the loose one. */
/*	  if ((*loose_bound == Nil) ||
	      (bound_in_bound(*tight_bound,*loose_bound)))
	    *loose_bound = *tight_bound;*/
	  /* check to see if tight bound is in any holes. */
	  for (this_hole = thisfve->boundlist->first.bnd->next;
	       this_hole != Nil; this_hole = this_hole->next)
	    if (bound_in_bound(*tight_bound,this_hole))
	      *tight_bound = this_hole;
	}
      }
    }
  }
  if (*tight_bound != Nil)	/* hack to fix hole in shell in other shells */
    if ((*loose_bound)->motherfve->mothershell !=
      	(*tight_bound)->motherfve->mothershell)
      *loose_bound = *tight_bound; 
  return(facecnt);
}  

int
pt_on_bound_3d(vertype testpt, boundptr thebound, double tol)
{
  leptr firstle,loople;
  vertype dummynear;
  double dummyt;
  ;
  firstle = loople = thebound->lelist->first.le;
  do				/* check this point against verts in bound */
  {
    if (pt_near_pt_3d(testpt,loople->facevert->pos,tol))
      return((int) Ptonboundvfe);
    loople = loople->next;
  } while (loople != firstle);

  do				/* check this point against edges in bound */
  {
    if (pt_near_lineseg_3d(loople->facevert->pos,
			   loople->next->facevert->pos,
			   testpt,dummynear,&dummyt,Ptonlinetol))
      return((int) Ptonboundle);	
    loople = loople->next;
  } while (loople != firstle);

  return ((int) Ptnotonbound);
}

  int		/* returns: Ptoutofbound,Ptinsidebound,Ptonbound */
pt_in_bound_3d(vertype testpt, boundptr thebound, double tol)
{
  Coord *edgept1,*edgept2;
  leptr firstle,thisle,markerle;
  int axis1,axis2,rayaxis;
  Boolean done = FALSE;
  int onboundflag,returnval, sum = 0;	/* ray intersection count */
  ;
  if (!pt_near_plane_3d(testpt,thebound->motherfve->facenorm,
			thebound->motherfve->planedist))
    return((int) Ptoutsidebound); /* pt must be on plane of face to be inside */

  if ((onboundflag = pt_on_bound_3d(testpt,thebound,tol)) != Ptnotonbound)
    return(onboundflag);

  pickplane(thebound->motherfve->facenorm,&axis1,&axis2);
  rayaxis = axis1;

  thisle = firstle = thebound->lelist->first.le;
  do
  {
    edgept1 = thisle->facevert->pos;
    edgept2 = thisle->next->facevert->pos;
    returnval = int_ray_edge(edgept1,edgept2,testpt,rayaxis,axis2);
    if (returnval == 1)		/* endpt of this edge lies on ray */
    {
      if (edgept2[axis2] == testpt[axis2]) /* only look at when pt2 is on ray*/
      {
	/* markerle is used to check for inf. loops. cant use firstle since */
	/* we may have started in middle of horizontal edge span...and */
	/* therefore would want to continue past it */
	markerle = thisle;
	do			/* skip edges that lie on ray */
	{
	  thisle = thisle->next;
	  if (thisle == firstle)
	    done = TRUE;
	} while ((edgept2[axis2] == thisle->next->facevert->pos[axis2]) &&
		 (thisle != markerle));

	if (thisle == markerle)
	  system_error("pt_in_bound_3d: could not find le off of ray");

	/* check to see if bound crosses ray */
	if (fsign(thisle->next->facevert->pos[axis2] - edgept2[axis2]) ==
	    fsign(edgept1[axis2] - edgept2[axis2]))
	  returnval = 0;
	else
	  returnval = 2;
      }
      else
      {
	returnval = 0;
	thisle = thisle->next;
      }

    }
    else
      thisle = thisle->next;

    sum += returnval;
  } while ((thisle != firstle) && !done);
  
  sum = sum >> 1;		/* int ray edge returns twice the # of ints. */
  if ((sum % 2) == 0)		/* even number of edge intersections is bad!*/
    return((int) Ptoutsidebound);

  return((int) Ptinsidebound);	/* odd number of edge intersections is good! */
}

  int		  /* returns Ptinsideface,Ptoutsideface,Ptonfacebound */
pt_in_face_3d(vertype testpt, fveptr theface, double tol)
{
  boundptr thisbound;
  int result;
  ;
  thisbound = theface->boundlist->first.bnd;
  result = pt_in_bound_3d(testpt,thisbound,tol);
  if (result == (int) Ptoutsidebound)
    return((int) Ptoutsideface);
  else if (result == (int) Ptonboundvfe)
    return((int) Ptonfaceboundvfe);
  else if (result == (int) Ptonboundle)
    return((int) Ptonfaceboundle);
  
  thisbound = thisbound->next;
  while (thisbound != Nil)
  {
    result = pt_in_bound_3d(testpt,thisbound,tol);
    if (result == (int) Ptinsidebound)
      return((int) Ptoutsideface);
    else if (result == (int) Ptonboundvfe)
      return((int) Ptonfaceboundvfe);
    else if (result == (int) Ptonboundle)
      return((int) Ptonfaceboundle);
    thisbound = thisbound->next;
  }

  return((int) Ptinsideface);
}

Boolean
edge_in_face3d(float *pt1, float *pt2, fveptr thisfve)
{
  bbox3dtype testedgebbox,thisedgebbox;
  boundptr thisbound;
  leptr thisle,firstle;
  float *thispt1,*thispt2;
  double t1,t2,length1,length2;
  vertype intersectpt,midpoint;
  int i;
  ;
  midpoint[vx] = (pt1[vx]+pt2[vx])/2.0;
  midpoint[vy] = (pt1[vy]+pt2[vy])/2.0;
  midpoint[vz] = (pt1[vz]+pt2[vz])/2.0;
  find_edge_bbox3d(pt1,pt2,&testedgebbox);
  thisbound = thisfve->boundlist->first.bnd;
  while (thisbound)
  {
    firstle = thisle = thisbound->lelist->first.le;
    /* check to see if this edge intersects any edges */
    do
    {
      thispt1 = thisle->facevert->pos;
      thispt2 = thisle->next->facevert->pos;
      find_edge_bbox3d(thispt1,thispt2,&thisedgebbox);
      if (bbox_intersect3d(&testedgebbox,&thisedgebbox))
      {
	if (lines_intersect_3d(pt1,pt2,thispt1,thispt2,intersectpt,&t1,&t2,
			       1.0e-4) == (int) Linesintersect)
	{
	  length1 = distance(pt1,pt2);
	  t1 = t1*length1;
	  length2 = distance(thispt1,thispt2);
	  t2 = t2*length2;
	  if ((!compare(t1,0.0,1.0e-4) && !compare(t1,length1,1.0e-4)) &&
	      (!compare(t2,0.0,1.0e-4) && !compare(t2,length2,1.0e-4)))
	    return (FALSE);	/* edges intersect */
	}
      }
      thisle = thisle->next;
    } while (thisle != firstle);
    thisbound = thisbound->next;
  }
  /* note we also must check that line is not totally outside of face! */
  /* if we get to here we know this edge does not intersect the face */
  /* so we can just do a point in face on the midpoint to see if its inside */
  /* the face */
  /* wee hack to get around overly tightass routine, pt_in_face_3d */
  if (!pt_near_plane_3d(midpoint,thisfve->facenorm,thisfve->planedist))
    thisfve->planedist = -dotvecs(midpoint,thisfve->facenorm);
  if (pt_in_face_3d(midpoint,thisfve,1.0e-5) == Ptinsideface)
    return (TRUE);
  else
    return (FALSE);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                    Intersection Routines                            */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* Compute the intersection point of a line and a plane in space. */
/* Returns: Lineonplane if the line lies in the plane. */
/*          Lineparallesplane if the line parallels the plane but doesn't */
/*          intersect. */
/*          Lineintersectsplane if the line does intersect the plane. */
/*           The intersection point is returned in intrpt. */

  int
line_intersect_plane_3d(vertype pt1,vertype pt2,vertype plane,Coord planedist,
			vertype intrpt,double *t)
{
  vertype fgh;
  double denom;
  ;
  diffpos3d(pt1,pt2,fgh);
  denom = dotvecs(plane,fgh);
  if (divide_by_zero(denom))
  {
    if (pt_near_plane_3d(pt1,plane,planedist))
      return((int) Lineonplane);/* Line is in the plane because a point */
				/* on the line is on the plane*/
    return((int) Lineparallelsplane);
    /* Otherwise, the line is parallel but not */
    /* on the plane.*/
  }

  *t = -(dotvecs(plane,pt1) + planedist) / denom;
  intrpt[vx] = pt1[vx] + fgh[vx]* (*t);
  intrpt[vy] = pt1[vy] + fgh[vy]* (*t);
  intrpt[vz] = pt1[vz] + fgh[vz]* (*t);
  return((int) Lineintersectsplane);
}

/* Compute the intersection point of a linesegment and a plane in space. */
/* Contrasted with the optimized version of this in intersect.c that */
/* makes the assumption we are dealing with the Cutplane to simplify things.*/
/* Returns: Linesegonplane if the lineseg lies in the plane. */
/*          Linesegendpt1onplane if endpoint1 is on the plane. */
/*          Linesegendpt2onplane if endpoint2 is on the plane. */
/*          Linesegintersectsplane if the lineseg does intersect the plane. */
/*          Linesegoffplane if the lineseg does not intersect the plane. */
/*           The intersection point is returned in intrpt. */

  int
lineseg_intersect_plane_3d(vertype pt1,vertype pt2,
			   vertype plane,Coord planedist,
			   vertype intrpt,double *t)
{
  double d1,d2,denom;
  vertype fgh;
  Boolean pt1on,pt2on;
  ;
  d1 = pt_to_planedist_3d(pt1,plane,planedist);
  d2 = pt_to_planedist_3d(pt2,plane,planedist);
  /* first check to see if neither of the points is within a tolerance */
  /* distance of the plane... if not, check to see that they lie on */
  /* opposite sides of the plane. */
  if ((compare(d1,0.0,Ptonplanetol) != 0) &&
      (compare(d2,0.0,Ptonplanetol) != 0))
    if (((d1 < 0.0) && (d2 > 0.0)) || ((d1 > 0.0) && (d2 < 0.0)))
    {				/* line segment intersects plane */
      diffpos3d(pt1,pt2,fgh);
      denom = dotvecs(plane,fgh);
      
      *t = -(dotvecs(plane,pt1) + planedist) / denom;
      intrpt[vx] = pt1[vx] + fgh[vx] * (*t);
      intrpt[vy] = pt1[vy] + fgh[vy] * (*t);
      intrpt[vz] = pt1[vz] + fgh[vz] * (*t);
      return((int) Linesegintersectsplane);
    }
  /* If d1 and d2 were same sign or 0, then one or both points must be on */
  /* the plane, or the linesegment is completely non-intersecting with the */
  /* plane. */
  pt1on = pt_near_plane_3d(pt1,plane,planedist);
  pt2on = pt_near_plane_3d(pt2,plane,planedist);
  if (pt1on && pt2on)		/* line segment on the plane */
    return((int) Linesegonplane);
  else if (pt1on)		/* only point one on the plane. */
    return((int) Linesegendpt1onplane);
  else if (pt2on)		/* only point two on the plane.  */
    return((int) Linesegendpt2onplane);
  
  return((int) Linesegoffplane);
}

/* determine whether a line segment intersects a face.  */
/* returns Lineseginsideface,Linesegoutsideface,Ptonfacebound */
/* Pass the tolerance tol required for a point near the boundary of the face, */
/* and the accuracy accy near to zero that you want to use to verify that the */
/* line segment and the plane are parallel. */

  int
lineseg_int_face_3d(vertype pt1,vertype pt2,fveptr theface,
		    vertype intrpt, double *t,double tol)
{
  float *firstptinface;
  int linesegstatus,intresult,parallel_result1,parallel_result2,endpt_result;
  ;
  linesegstatus = lineseg_intersect_plane_3d(pt1,pt2,theface->facenorm,
					     theface->planedist,intrpt,t);
  switch (linesegstatus)
  {
  case Linesegonplane:
    parallel_result1 = pt_in_face_3d(pt1,theface,tol);
    parallel_result2 = pt_in_face_3d(pt2,theface,tol);
    if ((parallel_result1 != (int) Ptoutsideface) &&
	(parallel_result2 != (int) Ptoutsideface))
      return((int) Lineseginface);
    /* Only returned when one pt of seg in face. */
    else if (parallel_result1 != (int) Ptoutsideface)
      return((int) Lineseginfacept1);
    else if (parallel_result2 != (int) Ptoutsideface)
      return((int) Lineseginfacept2);

    /* NOTE: the seg could have both pts outside*/
    /* and still intersect the face. */
    /* This case would not be detected, so we */
    /* return possible multiple intersections.*/
    return((int) Linesegintspossible);
  case Linesegendpt1onplane:
    endpt_result = pt_in_face_3d(pt1,theface,tol);
    switch (endpt_result)
    {
    case (int) Ptonfaceboundvfe:
      return ((int) Linesegpt1onboundvfe);
    case (int) Ptonfaceboundle:
      return ((int) Linesegpt1onboundle);
    case (int) Ptinsideface:
      return ((int) Linesegpt1inface);
    case (int) Ptoutsideface:
      break;			/* this will use return(Linesegoutsideface) */
				/* at bottom */
    }
    break;
  case Linesegendpt2onplane:
    endpt_result = pt_in_face_3d(pt2,theface,tol);
    switch (endpt_result)
    {
    case (int) Ptonfaceboundvfe:
      return ((int) Linesegpt2onboundvfe);
    case (int) Ptonfaceboundle:
      return ((int) Linesegpt2onboundle);
    case (int) Ptinsideface:
      return ((int) Linesegpt2inface);
    case (int) Ptoutsideface:
      break;			/* this will use return(Linesegoutsideface) */
				/* at bottom */
    }
    break;
  case Linesegintersectsplane:
    intresult = pt_in_face_3d(intrpt,theface,tol);
    if (intresult == (int) Ptonfaceboundvfe)
    {
#if 0      
      printf ("from lineseg_intersect_plane_3d,returning intersectsboundvfe\n");
#endif
      return((int) Linesegintersectsboundvfe);
    }
    else if (intresult == (int) Ptonfaceboundle)
    {
#if 0
      printf ("from lineseg_intersect_plane_3d,returning intersectsboundle\n");
#endif
      return((int) Linesegintersectsboundle);
    }
    else if (intresult == (int) Ptinsideface)
    {
#if 0
      printf ("from lineseg_intersect_plane_3d, returning intersectsface\n");
#endif
      return((int) Linesegintersectsface);
    }
  default:
    break;
  }
  return((int) Linesegoutsideface);
}

/* Compute the intersection of two lines in space. */
/* Return: Linesintersect if they intersect within the given tolerance tol. */
/*          In this case return the intersection point in intrpt. */
/*         Linesequal if they are the same line. */
/*         Linesdonotintersect if they do not intersect at all. */
/* For tol, may I recommend passing the above defined Linetolinetol? */
/* It was a very good constant indeed, monsewer. */


  int
lines_intersect_3d(vertype pt1,vertype pt2,vertype pt3,vertype pt4,
		   vertype intrpt,double *t1,double *t2,double tol)
{
#ifdef oldjimsway
  /* This is jim's way and it is super fast (8 mults, 2 divs, 6 adds) */
  vertype p,fgh,fgh2,v1,v2,intrpt2,partial;
  double k1,k2,n1,n2;
  ;
  diffpos3d(pt1,pt2,fgh);
  diffpos3d(pt3,pt4,fgh2);
  normalize(fgh,v1);
  normalize(fgh2,v2);
  k1 = dotvecs(v1,v2);
  k2 = k1*k1 - 1;
  if (compare(k2,0.0,Floatol) == 0)
    return(Linesequal); /* they are parallel */
  diffpos3d(pt1,pt3,p);

  n1 = dotvecs(p,v1);
  n2 = dotvecs(p,v2);
  *t1 = ((k1 * n2) - n1) / k2;
  *t2 = (n2 - (k1 * n1)) / k2;

  intrpt[vx] = pt1[vx] + (*t1) * v1[vx];
  intrpt[vy] = pt1[vy] + (*t1) * v1[vy];
  intrpt[vz] = pt1[vz] + (*t1) * v1[vz];

  /* This is a hack but the above tolerance check isn't currently correct so..*/
  intrpt2[vx] = pt3[vx] + (*t2) * v2[vx];
  intrpt2[vy] = pt3[vy] + (*t2) * v2[vy];
  intrpt2[vz] = pt3[vz] + (*t2) * v2[vz];
  if (pt_near_pt_3d(intrpt,intrpt2,Pttopttol))
  {
    /* Convert t1 && t2 to normal format of 0-1 over the length of the segment*/
    diffpos3d(pt1,intrpt,partial);
    *t1 = (magvec(partial) / magvec(fgh)) * fsign(dotvecs(partial,fgh));
    diffpos3d(pt3,intrpt2,partial);
    *t2 = magvec(partial) / magvec(fgh2) * fsign(dotvecs(partial,fgh2));
    return((int) Linesintersect);
  }

  return((int) Linesdonotintersect);
#endif
  /* This is lje's version of jim's way and it is super fast */
  /*  (8 mults, 2 divs, 6 adds + some unknown) with corrected t values*/
  vertype p,fgh,fgh2,v1,v2,intrpt2,partial;
  double k1,k2,n1,n2,v1magsqr,v2magsqr;
  ;
  diffpos3d(pt1,pt2,v1);
  diffpos3d(pt3,pt4,v2);
  v1magsqr = dotvecs(v1,v1);
  v2magsqr = dotvecs(v2,v2);
  k1 = dotvecs(v1,v2);
  k2 = k1*k1 - v1magsqr*v2magsqr;
  if (compare(k2,0.0,Floatol) == 0)
    return((int) Linesequal); /* they are parallel */
  diffpos3d(pt1,pt3,p);

  n1 = dotvecs(p,v1);
  n2 = dotvecs(p,v2);
  *t1 = ((k1*n2) - (v2magsqr * n1)) / k2;
  *t2 = ((v1magsqr * n2) - (k1 * n1)) / k2;

  intrpt[vx] = pt1[vx] + (*t1) * v1[vx];
  intrpt[vy] = pt1[vy] + (*t1) * v1[vy];
  intrpt[vz] = pt1[vz] + (*t1) * v1[vz];

  /* This is a hack but the above tolerance check isn't currently correct so..*/
  intrpt2[vx] = pt3[vx] + (*t2) * v2[vx];
  intrpt2[vy] = pt3[vy] + (*t2) * v2[vy];
  intrpt2[vz] = pt3[vz] + (*t2) * v2[vz];
  if (pt_near_pt_3d(intrpt,intrpt2,Pttopttol))
    return((int) Linesintersect);

  return((int) Linesdonotintersect);

}  


/* Compute the intersection of two line segments in space. */
/* Return: Linesegsintersect if they intersect within the given tolerance tol*/
/*          In this case return the intersection point in intrpt. */
/*         Linesegsequal if they are the same line. */
/*         Linesegsdonotintersect if they do not intersect at all. */
/* For tol, may I recommend passing the above defined Linetolinetol? */
/* It was a very good constant indeed, monsewer. */

  int
linesegs_intersect_3d(vertype pt1,vertype pt2,vertype pt3,vertype pt4,
		      vertype intrpt,double *t1,double *t2,double tol)
{
  vertype fgh1,fgh2;
  double denom;
  int result;
  ;
  /* should use bbox test on linesegs here first */
  if (lineseg_bbox_intersect(pt1,pt2,pt3,pt4))
    if ((result = lines_intersect_3d(pt1,pt2,pt3,pt4,intrpt,t1,t2,tol)) ==
	(int) Linesintersect)
    {
      if (((*t1) < 0.0) || ((*t1) > 1.0) || ((*t2) < 0.0) || ((*t2) > 1.0))
	return((int) Linesegsdonotintersect);
      return((int) Linesegsintersect);
    }
    else if (result == (int) Linesequal)
      return((int) Linesegsequal);
  return((int) Linesegsdonotintersect);
}

/* This routine expects the (a,b,c,d) values of two plane eqn's */
/* at which point it returns in intersectvect a normalized vector */
/* along the line of intersection of those planes, in sectvect.*/
/* The anchor pt for the vector of intersection is returned in */
/* sectpt. (The equation it expects for the */
/* planes is ax + by + cz + d = 0, also known as AX = 0, where */
/* X is a vector to a point on the plane, and A = <a,b,c,d>. */
/* Tol is the smallest determinant that indicates the planes are parallel. */
/* The return value is TRUE if an intersection vector was found, */
/* FALSE if the planes were parallel (and one was not found). */
/* see Programmers Geometry pg 113-114 */

  Boolean
planes_intersect(vertype plane1,Coord plane1dist,
		 vertype plane2,Coord plane2dist,
		 vertype sectvect,vertype sectpt,
		 double tol)
{
  double det,detinv,detinvroot,dc,db,ad;
  ;
  sectvect[vx] = plane1[vy] * plane2[vz] - plane2[vy] * plane1[vz];
  sectvect[vy] = plane1[vz] * plane2[vx] - plane2[vz] * plane1[vx];
  sectvect[vz] = plane1[vx] * plane2[vy] - plane2[vx] * plane1[vy];
  det = sectvect[vx]*sectvect[vx] + sectvect[vy]*sectvect[vy] +
    sectvect[vz]*sectvect[vz];
  if (det < tol)
    return(FALSE);
  
  detinv = 1.0 / det;
  detinvroot = sqrt(detinv);
  dc = plane1dist * plane2[vz] - plane1[vz] * plane2dist;
  db = plane1dist * plane2[vy] - plane1[vy] * plane2[vz];
  ad = plane1[vx] * plane2dist - plane1dist * plane2[vx];

  sectvect[vx] *= detinvroot;	/* multiplying these by the sqrt of the */
  sectvect[vy] *= detinvroot;	/* determinant will normalize the  */
  sectvect[vz] *= detinvroot;	/* intersection line vector. */
  
  sectpt[vx] = ((sectvect[vy] * dc) - (sectvect[vz] * db)) * detinv;
  sectpt[vy] = -((sectvect[vx] * dc) + (sectvect[vz] * ad)) * detinv;
  sectpt[vz] = ((sectvect[vx] * db) + (sectvect[vy] * ad)) * detinv;
  
  return(TRUE);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                                                                            */
/*                      Coincidence Testing Routines                          */
/*                                                                            */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


  Boolean
same_bound(boundptr bound1,boundptr bound2,Boolean *reversed)
{
  leptr startle,otherle,thisle;
  Boolean test = TRUE;
  ;
  *reversed = FALSE;
  startle = thisle = bound1->lelist->first.le;
  otherle = bound2->lelist->first.le;
  do
  {				/* find first le that is the same */
    if (pt_near_pt_3d(thisle->facevert->pos,otherle->facevert->pos,Pttopttol))
      break;			/* found it */
    thisle = thisle->next;
    if (thisle == startle)
      return(FALSE);		/* none found, all faces points all different */
  } while (thisle != startle);
  startle = thisle;
  do
  {
    if (!pt_near_pt_3d(thisle->facevert->pos,otherle->facevert->pos,Pttopttol))
    {
      test = FALSE;		/* a point was different */
      break;
    }
    thisle = thisle->next;
    otherle = otherle->next;
  } while (thisle != startle);

  if (test)
    return(TRUE);			/* all points same going one way */
  *reversed = TRUE;		/* if we got here, then if geom. same, must */
				/* be reversed orientations. */
  thisle = startle;
  otherle = bound2->lelist->first.le;
  test = TRUE;
  do
  {
    if (!pt_near_pt_3d(thisle->facevert->pos,otherle->facevert->pos,Pttopttol))
    {
      test = FALSE;		/* a point was different */
      break;
    }
    thisle = thisle->prev;
    otherle = otherle->next;
  } while (thisle != startle);

  return(test);
}

/* Given one face, find a face in othershell that geometrically matches it. */
/* Maybe should be in geometry.c? Assumedly, thisfve is not in othershell. */
/* used to be used by boolean.c boolean_recombine to determine  which glue */
/* face to use after a duplicate/invert shell procedure.  However, no */
/* longer really needed. NOTE that it only confirms that the outer bounds */
/* are the same. */
  fveptr
find_same_face(fveptr thisfve, objptr otherobj, Boolean *reversed)
{
  fveptr otherfve;
  shellptr thishell;
  ;
  thishell = otherobj->shellist->first.shell;
  while (thishell != Nil)
  {
    otherfve = thishell->fvelist->first.fve;
    while (otherfve != Nil)
    {
      if (same_bound(thisfve->boundlist->first.bnd,
		     otherfve->boundlist->first.bnd,reversed))
	return(otherfve);
      otherfve = otherfve->next;
    }
    thishell = thishell->next;
  }
  return(Nil);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                                                                            */
/*                  Face Property Calculation Routines                        */
/*                                                                            */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  Boolean
face_is_planar(fveptr thisfve,Coord giventol)
{
  boundptr thisbound;
  leptr firstle,thisle;
  double dist;
  ;
  for (thisbound = thisfve->boundlist->first.bnd; thisbound != Nil;
       thisbound = thisbound->next)
  {
    thisle = firstle = thisbound->lelist->first.le;
    do
    {   
      dist = pt_to_planedist_3d(thisle->facevert->pos,
				thisle->motherbound->motherfve->facenorm,
				thisle->motherbound->motherfve->planedist);
      if (dist > giventol)
	return(FALSE);
      thisle = thisle->next;
    } while (thisle != firstle);
  }
  return(TRUE);
}

Boolean
planar_face_nonormal(fveptr thisfve)
{
  boundptr thisbound;
  leptr firstle,thisle,nextle;
  vertype vec1,vec2,firstnorm,thisnorm;
  Coord costh;
  ;
  thisbound = thisfve->boundlist->first.bnd;
  thisle = firstle = thisbound->lelist->first.le;
  nextle = thisle->next;
  diffpos3d(thisle->facevert->pos,thisle->next->facevert->pos,vec1);
  diffpos3d(nextle->facevert->pos,nextle->next->facevert->pos,vec2);
  cross_prod(vec1,vec2,firstnorm);
  if (!vec_eq_zero(firstnorm))
    normalize(firstnorm,firstnorm);

  thisle = nextle;
  while (thisle != firstle)
  {   
    nextle = thisle->next;
    diffpos3d(thisle->facevert->pos,thisle->next->facevert->pos,vec1);
    diffpos3d(nextle->facevert->pos,nextle->next->facevert->pos,vec2);
    cross_prod(vec1,vec2,thisnorm);
    if (!vec_eq_zero(thisnorm))
    {
      normalize(thisnorm,thisnorm);
      costh = dotvecs(thisnorm,firstnorm);
      if (compare(fabs(costh),1.0,1.0e-4) != 0)
	return(FALSE);
    }

    thisle = nextle;
  }

  return (TRUE);
}

  Boolean 
face_is_concave(fveptr thisfve)
{
  vertype firstnorm,thisnorm,vec1,vec2;
  leptr firstle,thisle;
  ;
  firstle = thisle = thisfve->boundlist->first.bnd->lelist->first.le;
  diffpos3d(thisle->facevert->pos,thisle->prev->facevert->pos,vec1);
  diffpos3d(thisle->facevert->pos,thisle->next->facevert->pos,vec2);
  cross_prod(vec1,vec2,firstnorm);
  thisle = thisle->next;
  do
  {
    diffpos3d(thisle->facevert->pos,thisle->prev->facevert->pos,vec1);
    diffpos3d(thisle->facevert->pos,thisle->next->facevert->pos,vec2);
    cross_prod(vec1,vec2,thisnorm);
    if ((!vector_near_zero(thisnorm,Zeroveclengthtol)) &&
	(dotvecs(thisnorm,firstnorm) < 0.0))
      return(TRUE);
    thisle = thisle->next;
  } while (thisle != firstle);

  return(FALSE);
}

/* Compute a centroid.  If this face has holes, these weigh into the */
/* centroid, and if it's concave, the centroid may not be inside!!! */

  void
compute_face_centroid(fveptr thisfve,vertype centroidpos)
{
  boundptr thisbound;
  leptr firstle,thisle;
  int count = 0;
  ;
  setpos3d(centroidpos,0.0,0.0,0.0);
  for (thisbound = thisfve->boundlist->first.bnd; thisbound != Nil;
       thisbound = thisbound->next)
  {
    firstle = thisle = thisbound->lelist->first.le;
    do
    {
      addvec3d(thisle->facevert->pos,centroidpos,centroidpos);
      thisle = thisle->next;
      count++;
    } while (thisle != firstle);
  }
  scalevec3d(centroidpos, 1.0 / (double) count,centroidpos);
}


/* Compute a centroid.  If this face has holes, these weigh into the */
/* centroid, and if it's concave, the centroid may not be inside!!! */

  void
compute_obj_centroid(objptr thisobj,vertype centroidpos)
{
  shellptr thishell;
  vertype facecenter;
  int count = 0;
  fveptr thisfve;
  ;
  thishell = thisobj->shellist->first.shell;
  setpos3d(centroidpos,0.0,0.0,0.0);
  while (thishell != Nil)
  {
    thisfve = thishell->fvelist->first.fve;
    while (thisfve != Nil)
    {
      compute_face_centroid(thisfve,facecenter);
      addvec3d(facecenter,centroidpos,centroidpos);
      count++;
      thisfve = thisfve->next;
    }
    thishell = thishell->next;
  }
  scalevec3d(centroidpos, 1.0 / (double) count,centroidpos);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                     2D CLIPPING ROUTINES: BARSKY                       */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


  Boolean
clip_t(p,q,t0,t1)
  float p,q,*t0,*t1;
{
  float r;
  Boolean accept = TRUE;
  ;
  if (p < 0)			/* Investigate cases of V0V1 going   */
  {				/* from invisible to visible side of */
    r = q / p;			/* plane. */
    if (r > *t1) 
      accept = FALSE;		/* Reject line because it remains on */
    else			/* invisible side of plane.*/
      if (r > *t0)
	*t0 = r;		/* Move t0 up to intersection point. */
  }
  else				/* Investigate cases of V0V1 going   */
    if (p > 0)			/* from visible to invisible side of */
    {				/* plane. */
      r = q / p;
      if (r < *t0) 
	accept = FALSE;		/* Reject line because it remains on */
      else			/* invisible side of plane. */
	if (r < *t1)
	  *t1 = r;		/* Move t1 up to intersection point. */
    }
    else			/* Investigate cases of totally on visible    */
      if (q < 0)		/* side of plane or totally on invisible side */
	accept = FALSE;		/* Line totally invisible. */

  return (accept);		/*Say whether something visible at all*/
}



  int
clip_lineseg_to_screenbbox(screenpos clippedpt0,screenpos clippedpt1,
			   bboxscreenptr thebbox)
{
  register float deltax,deltay,deltaz,deltaw;
  register float x0,y0,z0,w0;
  register int returncode = Notclipped;
  float t0 = 0.0, t1 = 1.0;
  Screencoord *mincorner,*maxcorner;
  ;
  x0 = clippedpt0[vx];
  deltax = clippedpt1[vx] - x0;
  mincorner = &(thebbox->mincorner[vx]);
  maxcorner = &(thebbox->maxcorner[vx]);
  if (clip_t(-deltax, x0 - mincorner[vx], &t0, &t1))
    if (clip_t(deltax, maxcorner[vx] - x0, &t0, &t1))
    {
      y0 = clippedpt0[vy];
      deltay = clippedpt1[vy] - y0;
      if (clip_t(-deltay, y0 - mincorner[vy], &t0, &t1))
	if (clip_t(deltay, maxcorner[vy] - y0, &t0, &t1))
	{
	  if (t1 < 1.0)
	  {
	    *clippedpt1++ = x0 + (t1 * deltax);
	    *clippedpt1++ = y0 + (t1 * deltay);
	    returncode = Pt1clipped;
	  }
	  if (t0 > 0.0)
	  {
	    *clippedpt0++ = x0 + (t0 * deltax);
	    *clippedpt0++ = y0 + (t0 * deltay);
	    returncode |= Pt0clipped;
	  }
	}
    }
  return(returncode);
}

int
clip4d (clippedpt0,clippedpt1)
  register float *clippedpt0,*clippedpt1;
{
  register float deltax,deltay,deltaz,deltaw;
  register float x0,y0,z0,w0;
  register int returncode = Notclipped;
  float t0 = 0.0, t1 = 1.0;
  ;
  x0 = clippedpt0[vx];
  w0 = clippedpt0[vw];
  deltax = clippedpt1[vx] - x0;
  deltaw = clippedpt1[vw] - w0;
  if (clip_t(-deltax - deltaw, x0 + w0, &t0, &t1))
    if (clip_t(deltax - deltaw, w0 - x0, &t0, &t1))
    {
      y0 = clippedpt0[vy];
      deltay = clippedpt1[vy] - y0;
      if (clip_t(-deltay - deltaw, y0 + w0, &t0, &t1))
	if (clip_t(deltay - deltaw, w0 - y0, &t0, &t1))
	{
	  z0 = clippedpt0[vz];
          deltaz = clippedpt1[vz] - z0;
	  if (clip_t(-deltaz - deltaw, w0 + z0, &t0, &t1))
	    if (clip_t(deltaz - deltaw, w0 - z0, &t0, &t1))
	    {
              if (t1 < 1.0)
	      {
                *clippedpt1++ = x0 + (t1 * deltax);
                *clippedpt1++ = y0 + (t1 * deltay);
                *clippedpt1 = z0 + (t1 * deltaz);
                returncode = Pt1clipped;
	      }
              if (t0 > 0.0)
	      {
                *clippedpt0++ = x0 + (t0 * deltax);
                *clippedpt0++ = y0 + (t0 * deltay);
                *clippedpt0 = z0 + (t0 * deltaz);
                returncode |= Pt0clipped;
	      }
	    }
	}
    }
  return(returncode);
}
