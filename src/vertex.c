
/* FILE: vertex.c */

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                                                                        */
/*         Vertex Management Routines for the Cutplane Editor             */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 27,1989  */
/* Version: 3.0                                                           */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#define VERTEXMODULE

#include <config.h>
#include <stdio.h>

#include <platform_gl.h>
#include <math.h>
#include <limits.h>

#include <cutplane.h>

#include <vertex.h>
#include <pick.h>
#include <globals.h>

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                       MACROS                                             */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#define move_elem_macro(the_head,the_elem,dest) \
{\
      vfeptr local_elem,local_dest;\
      ;\
      local_elem = the_elem;\
      local_dest = dest;\
      if (local_elem->prev == Nil) /* then it is the first element */ \
      {\
	the_head->first.vfe = local_elem->next;\
	the_head->first.vfe->prev = Nil;\
      }\
      else\
      {\
	local_elem->prev->next = local_elem->next;\
	if (local_elem->next)\
	  local_elem->next->prev = local_elem->prev;\
	else\
	  the_head->last.vfe = local_elem->prev;\
      }\
      if (local_dest)\
      {\
	local_elem->next = local_dest->next;\
	local_elem->prev = local_dest;\
	if (local_dest->next)\
	  local_dest->next->prev = local_elem;\
	else\
	  the_head->last.vfe = local_elem;\
	local_dest->next = local_elem;\
      }\
      else\
      {\
	local_elem->next = the_head->first.vfe;\
	local_elem->prev = Nil;\
	the_head->first.vfe->prev = local_elem;\
	the_head->first.vfe = local_elem;\
      }\
}\

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                           VERTEX ROUTINES                                */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
setvfe(vfeptr thisvfe, float * newpos, Matrix global2local_xform)
{
  register Coord *local_pos,*pos,*matrixptr;
  ;
  pos = thisvfe->pos;
  *(pos++) = *(newpos++);
  *(pos++) = *(newpos++);
  *(pos++) = *newpos;
  *pos = 1.0;
  
  matrixptr = (Coord *) global2local_xform;

  local_pos = thisvfe->local_pos;
  pos = thisvfe->pos;

  local_pos[0] = pos[0]*matrixptr[0] + pos[1]*matrixptr[4] +
    pos[2]*matrixptr[8] + pos[3]*matrixptr[12];
    
  local_pos[1] = pos[0]*matrixptr[1] + pos[1]*matrixptr[5] +
    pos[2]*matrixptr[9] + pos[3]*matrixptr[13];
    
  local_pos[2] = pos[0]*matrixptr[2] + pos[1]*matrixptr[6] +
    pos[2]*matrixptr[10] + pos[3]*matrixptr[14];

  local_pos[3] = pos[0]*matrixptr[3] + pos[1]*matrixptr[7] +
    pos[2]*matrixptr[11] + pos[3]*matrixptr[15];
}

  void
setvfe_local(vfeptr thisvfe, float * newpos, Matrix local2global_xform)
{
  register Coord *local_pos,*pos,*matrixptr;
  ;
  local_pos = thisvfe->local_pos;
  *(local_pos++) = *(newpos++);
  *(local_pos++) = *(newpos++);
  *(local_pos++) = *newpos;
  *local_pos = 1.0;
  
  matrixptr = (Coord *) local2global_xform;

  local_pos = thisvfe->local_pos;
  pos = thisvfe->pos;

  pos[0] = local_pos[0]*matrixptr[0] + local_pos[1]*matrixptr[4] +
    local_pos[2]*matrixptr[8] + local_pos[3]*matrixptr[12];
    
  pos[1] = local_pos[0]*matrixptr[1] + local_pos[1]*matrixptr[5] +
    local_pos[2]*matrixptr[9] + local_pos[3]*matrixptr[13];
    
  pos[2] = local_pos[0]*matrixptr[2] + local_pos[1]*matrixptr[6] +
    local_pos[2]*matrixptr[10] + local_pos[3]*matrixptr[14];

  pos[3] = local_pos[0]*matrixptr[3] + local_pos[1]*matrixptr[7] +
    local_pos[2]*matrixptr[11] + local_pos[3]*matrixptr[15];
}

  vfeptr
new_vfe(vertype pos, shellptr thishell)
{
  vfeptr newvfe;
  ;
  if (thishell == Nil)
    system_error("new_vfe: thishell is Nil");

  newvfe = (vfeptr) append_new_blank_elem(thishell->vfelist,Vfe_type);
  pos[vw] = 1.0;		/* ensure pos is a fixed vector */

  if (thishell->motherobj == Nil)
    setvfe(newvfe,pos,imatrix); /* shell just created, no object yet */
  else
    setvfe(newvfe,pos,thishell->motherobj->invxform);

  return(newvfe);
}

  vfeptr
new_vfe_local(vertype local_pos, shellptr thishell)
{
  vfeptr newvfe;
  ;
  if (thishell == Nil)
    system_error("new_vfe: thishell is Nil");

  newvfe = (vfeptr) append_new_blank_elem(thishell->vfelist,Vfe_type);
  local_pos[vw] = 1.0;		/* ensure pos is a fixed vector */

  if (thishell->motherobj == Nil) /* shell just created, no object yet */
    setvfe_local(newvfe,local_pos,imatrix);
  else
    setvfe_local(newvfe,local_pos,thishell->motherobj->xform);

  return(newvfe);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                            FACE NORMAL ROUTINES                          */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
setfnorm(fveptr thisfve, vertype newfnorm)
{
  register Coord *local_facenorm,*facenorm,*matrixptr;
  ;
  thisfve->featureflags &= ~Nonormalflag;
  facenorm = thisfve->facenorm;
  *(facenorm++) = *(newfnorm++);
  *(facenorm++) = *(newfnorm++);
  *(facenorm++) = *newfnorm;
  *facenorm = 0.0;		/* this is a free vector bozo */

  facenorm = thisfve->facenorm;

  /* if the normal is zero don't bother transforming it */
  if ((facenorm[0] == 0.0) && (facenorm[1] == 0.0) && (facenorm[2] == 0.0))
    copyvec3d(thisfve->facenorm,thisfve->local_facenorm);
  else
  {
    matrixptr = (Coord *) thisfve->mothershell->motherobj->invxform;
    local_facenorm = thisfve->local_facenorm;
    
    local_facenorm[0] = facenorm[0]*matrixptr[0] + facenorm[1]*matrixptr[4] +
      facenorm[2]*matrixptr[8] + facenorm[3]*matrixptr[12];
    
    local_facenorm[1] = facenorm[0]*matrixptr[1] + facenorm[1]*matrixptr[5] +
      facenorm[2]*matrixptr[9] + facenorm[3]*matrixptr[13];
    
    local_facenorm[2] = facenorm[0]*matrixptr[2] + facenorm[1]*matrixptr[6] +
      facenorm[2]*matrixptr[10] + facenorm[3]*matrixptr[14];
    
    local_facenorm[3] = facenorm[0]*matrixptr[3] + facenorm[1]*matrixptr[7] +
    facenorm[2]*matrixptr[11] + facenorm[3]*matrixptr[15];
  }
  compute_planedist(thisfve);
}

  void
setfnorm_local(fveptr thisfve, vertype newfnorm)
{
  register Coord *local_facenorm,*facenorm,*matrixptr;
  ;
  thisfve->featureflags &= ~Nonormalflag;
  local_facenorm = thisfve->local_facenorm;
  *(local_facenorm++) = *(newfnorm++);
  *(local_facenorm++) = *(newfnorm++);
  *(local_facenorm++) = *newfnorm;
  *local_facenorm = 0.0;		/* this is a free vector bozo */

  local_facenorm = thisfve->local_facenorm;
  /* if the normal is zero don't bother transforming it */
  if ((local_facenorm[0] == 0.0) && (local_facenorm[1] == 0.0) &&
      (local_facenorm[2] == 0.0))
    copyvec3d(thisfve->local_facenorm,thisfve->facenorm);
  else
  {
    matrixptr = (Coord *) thisfve->mothershell->motherobj->xform;
    facenorm = thisfve->facenorm;
    
    facenorm[0] = local_facenorm[0]*matrixptr[0]+local_facenorm[1]*matrixptr[4] +
      local_facenorm[2]*matrixptr[8] + local_facenorm[3]*matrixptr[12];
    
    facenorm[1] = local_facenorm[0]*matrixptr[1]+local_facenorm[1]*matrixptr[5] +
      local_facenorm[2]*matrixptr[9] + local_facenorm[3]*matrixptr[13];
    
    facenorm[2] = local_facenorm[0]*matrixptr[2]+local_facenorm[1]*matrixptr[6] +
      local_facenorm[2]*matrixptr[10] + local_facenorm[3]*matrixptr[14];
    
    facenorm[3] = local_facenorm[0]*matrixptr[3]+local_facenorm[1]*matrixptr[7] +
    local_facenorm[2]*matrixptr[11] + local_facenorm[3]*matrixptr[15];
  }
  compute_planedist(thisfve);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                       VERTEX SORTING ROUTINES                            */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  static void
quicksort_engine(listptr vertlist, int axis, int numvfes, vfeptr pivotvfe)
{
  register int numvfes1 = 0, numvfes2 = 0;
  register float pivotpos;
  register vfeptr thisvfe,nextvfe,marker;
  vfeptr pivotvfe1,pivotvfe2;
  ;
  marker = pivotvfe->prev;
  thisvfe = pivotvfe->next;
  pivotpos = pivotvfe->pos[axis];
  while ((numvfes1+numvfes2) < numvfes)
  {
    nextvfe = thisvfe->next;  /* if thisvfe is Nil something is wrong */
    /* if thisvfe is > pivotvfe, place before pivotvfe (right after marker) */
    if (thisvfe->pos[axis] > pivotpos)
    {
      move_elem_macro(vertlist,thisvfe,marker)
      ++numvfes1;
    }
    else            /* otherwise don't move anything */
      ++numvfes2;
    thisvfe = nextvfe;
  }
  if (marker == Nil)
    pivotvfe1 = vertlist->first.vfe;
  else
    pivotvfe1 = marker->next;
  pivotvfe2 = pivotvfe->next;
  if (numvfes1 > 2)
    quicksort_engine(vertlist,axis,numvfes1-1,pivotvfe1);
  else if (numvfes1 == 2)
  {
    if ((pivotvfe1->next)->pos[axis] > pivotvfe1->pos[axis])
      move_elem_macro(vertlist,pivotvfe1,pivotvfe1->next)
  }
  if (numvfes2 > 2)
    quicksort_engine(vertlist,axis,numvfes2-1,pivotvfe2);
  else if (numvfes2 == 2)
  {
    if ((pivotvfe2->next)->pos[axis] > pivotvfe2->pos[axis])
      move_elem_macro(vertlist,pivotvfe2,pivotvfe2->next)
  }
}

  void
sortvertlist(int axis, listptr vfelist)
{
  int numvfes;
  vfeptr pivotvfe;
  ;
  pivotvfe = vfelist->first.vfe;
  numvfes = (vfelist->numelems) - 1;
  quicksort_engine(vfelist,axis,numvfes,pivotvfe);
}

  leptr
get_lemax(int axis, boundptr thisbound)
{
  register leptr firstle,thisle,lemax;
  register float maxval,thisval;
  ;
  firstle = thisbound->lelist->first.le;
  thisle = firstle;
  maxval =  thisle->facevert->pos[axis];
  lemax = thisle;
  do
  {
    thisval = thisle->facevert->pos[axis];
    if (thisval > maxval)
    {
      maxval = thisval;
      lemax = thisle;
    }
    thisle = thisle->next;
  }  while (thisle != firstle);

  return (lemax);
}

  leptr
get_lemin(int axis, boundptr thisbound)
{
  register leptr firstle,thisle,lemin;
  register float minval,thisval;
  ;
  firstle = thisbound->lelist->first.le;
  thisle = firstle;
  minval =  thisle->facevert->pos[axis];
  lemin = thisle;
  do
  {
    thisval = thisle->facevert->pos[axis];
    if (thisval < minval)
    {
      minval = thisval;
      lemin = thisle;
    }
    thisle = thisle->next;
  } while (thisle != firstle);

  return (lemin);
}

  void
get_leminmax(int axis, boundptr thisbound, leptr *lemin, leptr *lemax)
{
  register leptr firstle,thisle;
  register float minval,maxval,thisval;
  ;
  thisle = firstle = thisbound->lelist->first.le;
  maxval = minval =  thisle->facevert->pos[axis];
  *lemax = *lemin = thisle;
  do
  {
    thisval = thisle->facevert->pos[axis];
    if (thisval < minval)
    {
      minval = thisval;
      *lemin = thisle;
    }
    else if (thisval > maxval)
    {
      maxval = thisval;
      *lemax = thisle;
    }
    thisle = thisle->next;
  } while (thisle != firstle);
}

  vfeptr
get_vertmax(int axis, listptr vertlist)
{
  register vfeptr thisvfe,vfemax;
  register float maxval,thisval;
  ;
  thisvfe = vertlist->first.vfe;
  if (thisvfe == Nil)
    return (Nil);
  maxval = thisvfe->pos[axis];
  vfemax = thisvfe;
  while (thisvfe)
  {
    thisval = thisvfe->pos[axis];
    if (thisval > maxval)
    {
      maxval = thisval;
      vfemax = thisvfe;
    }
    thisvfe = thisvfe->next;
  }

  return (vfemax);
}

  vfeptr
get_3axis_vfemax(int axis1, int axis2, int axis3, listptr vertlist)
{
  register vfeptr thisvfe,vfemax;
  register float *maxpos;
  ;
  thisvfe = vertlist->first.vfe;
  if (thisvfe == Nil)
    return (Nil);
  vfemax = thisvfe;
  maxpos = vfemax->pos;
  while (thisvfe)
  {
    if (thisvfe->pos[axis1] > maxpos[axis1])
    {
      vfemax = thisvfe;
      maxpos = vfemax->pos;
    }
    else if (thisvfe->pos[axis1] == maxpos[axis1])
    {
      if (thisvfe->pos[axis2] > maxpos[axis2])
      {
	vfemax = thisvfe;
	maxpos = vfemax->pos;
      }
      else if ((thisvfe->pos[axis2] == maxpos[axis2]) &&
	       (thisvfe->pos[axis3] > maxpos[axis3]))
      {
	vfemax = thisvfe;
	maxpos = vfemax->pos;
      }
    }

    thisvfe = thisvfe->next;
  }

  return (vfemax);
}

  vfeptr
get_vertmin(int axis, listptr vertlist)
{
  register vfeptr thisvfe,vfemin;
  register float minval,thisval;
  ;
  thisvfe = vertlist->first.vfe;
  if (thisvfe == Nil)
    return (Nil);
  minval =  thisvfe->pos[axis];
  vfemin = thisvfe;
  while (thisvfe)
  {
    thisval = thisvfe->pos[axis];
    if (thisval < minval)
    {
      minval = thisval;
      vfemin = thisvfe;
    }
    thisvfe = thisvfe->next;
  }

  return (vfemin);
}

  void
get_vertminmax(int axis, listptr vertlist, vfeptr *minvfe, vfeptr *maxvfe)
{
  register vfeptr thisvfe,vfemin,vfemax;
  register float minval,maxval,thisval;
  ;
  thisvfe = vertlist->first.vfe;
  if (thisvfe == Nil)
    return;
  minval = maxval = thisvfe->pos[axis];
  vfemin = vfemax = thisvfe;
  while (thisvfe)
  {
    thisval = thisvfe->pos[axis];
    if (thisval > maxval)
    {
      maxval = thisval;
      vfemax = thisvfe;
    }
    else if (thisval < minval)
    {
      minval = thisval;
      vfemin = thisvfe;
    }
    thisvfe = thisvfe->next;
  }
  *minvfe = vfemin;
  *maxvfe = vfemax;
}

  void
get_local_vertminmax(int axis, listptr vertlist, vfeptr *minvfe, vfeptr *maxvfe)
{
  register vfeptr thisvfe,vfemin,vfemax;
  register float minval,maxval,thisval;
  ;
  thisvfe = vertlist->first.vfe;
  if (thisvfe == Nil)
    return;
  minval = maxval = thisvfe->local_pos[axis];
  vfemin = vfemax = thisvfe;
  while (thisvfe)
  {
    thisval = thisvfe->local_pos[axis];
    if (thisval > maxval)
    {
      maxval = thisval;
      vfemax = thisvfe;
    }
    else if (thisval < minval)
    {
      minval = thisval;
      vfemin = thisvfe;
    }
    thisvfe = thisvfe->next;
  }
  *minvfe = vfemin;
  *maxvfe = vfemax;
}
