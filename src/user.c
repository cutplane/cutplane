
/* FILE: user.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*  Improved User Input Event Handling Routines for the Cutplane Editor   */
/*            (Faster, Stronger, and Much Much More Powerful)             */
/*                                                                        */
/* Authors: LJE and WAK                           Date: August 26,1988    */
/*                                                                        */
/* Copyright (C) 1987,1988,1989 The Board of Trustees of The Leland       */
/* Stanford Junior University.  All Rights Reserved.                      */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define USERMODULE

#include <config.h>
#include <stdio.h>
#include <string.h>

#include <platform_gl.h>
#include <math.h>

#include <cutplane.h> /* cutplane.h includes user.h */
#include <pick.h>	/* for access to global_pickedlist */
#include <globals.h>
#include <grafx.h>


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			GLOBAL VARIABLES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

static FILE *dbgfile;
listptr nearpts_list;
sortptr nearest_nearpt;
GLboolean reset_nearpts = TRUE;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			LOW LEVEL FUNCTIONS				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#define even(n) (~(n & 0x1))
#define odd(n) (n & 0x1)

/* double_clickp returns true if the mouse was clicked twice in the same */
/* spot */

  static int
double_clickp(oldstate,state)
  stateptr oldstate,state;
{
  return ((fabs(oldstate->cursor[vx] - state->cursor[vx]) < 3) &&
	  (fabs(oldstate->cursor[vy] - state->cursor[vy]) < 3));
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			USER INPUT FUNCTIONS				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


/* enforce_limits limits any state variables that need to be limited */

  static void
enforce_limits(state)
  stateptr state;
{
  if (state->roomroty > Maxroomroty) state->roomroty = Maxroomroty;
    else if (state->roomroty < Minroomroty) state->roomroty = Minroomroty;
  if (state->roomrotx > Maxroomrotx) state->roomrotx = Maxroomrotx;
    else if (state->roomrotx < Minroomrotx) state->roomrotx = Minroomrotx;
  if (state->cursor[vz] > Maxplanez)
    state->cursor[vz] = Maxplanez;
  else if (state->cursor[vz] < Minplanez)
    state->cursor[vz] = Minplanez;
}


/* Use iris prompting stuff if possible */

#ifdef IRIS
  void
GetInputFromUser(char *s, int len_s, char *m)
{
  char launchline[300];	/* Should dynamically allocate... */
  FILE *fp;
  ;
  sprintf(launchline, "launch -h echo -m %s", m);
  if ((fp = popen(launchline, "r")) != NULL)
  {
    fgets(s, len_s, fp);
    pclose(fp);
    /* Strip off trailing newline */
    if (s[0] != '\0' && s[strlen(s)-1] == '\n')
      s[strlen(s)-1] = '\0';
  }
  else s[0] = '\0';
	/* OGLXXX
	 * cursoff not supported
	 * cursoff()
	 */
  /*DELETED*/;			/* cause launch seems to do curson... */
}
#endif


  void
inquire_filename(char *inoutfilename, char *message, char *defaultfile)
{
  static char oldfilename[20],newfilename[20];
  char s1[100];
  ;
#ifdef IRIS
  unqbuttons();
  sprintf(s1,"\"%s\" \"%s\"",message,defaultfile);
  GetInputFromUser(newfilename,20,s1);
  strcpy(inoutfilename,newfilename);
  qbuttons();
#endif /* IRIS */

#ifdef IRIS
  unqbuttons();
#endif /* IRIS */
  printf ("%s",message);
  printf (" (%s)",defaultfile);
  printf ("Enter file name (or 'old' to use old filename:");
  scanf("%s",newfilename);
  if (strcmp(newfilename,oldfilename) == 0)
    strcpy(oldfilename,inoutfilename);
  else
  {
    strcpy(inoutfilename,newfilename);
    strcpy(oldfilename,newfilename);
  }
#ifdef IRIS
  qbuttons();
#endif /* IRIS */
}


  double
inquire_double(char *message,double defaultval)
{
  char instr[40],outstr[40],s1[100];
  double newvalue;
  ;
#ifdef IRIS
  unqbuttons();
#endif /* IRIS */
  sprintf(s1,"\"%s\" -- \"%f\"",message,defaultval);
#ifdef IRIS
  GetInputFromUser(outstr,40,s1);
#endif
  sscanf(outstr,"%lf",&(newvalue));
#ifdef IRIS
  qbuttons();
#endif /* IRIS */
  return(newvalue);
}


  void
inquire_vfe(char *message,vfeptr thisvfe)
{
  char instr[40],outstr[40],s1[100];
  vertype newpos;
  ;
#ifdef IRIS
  unqbuttons();
#endif /* IRIS */
  sprintf(s1,"\"%s\" -- \"%f,%f,%f\"",message,thisvfe->pos[vx],
	  thisvfe->pos[vy],thisvfe->pos[vz]);
#ifdef IRIS
  GetInputFromUser(outstr,40,s1);
#endif
  sscanf(outstr,"%f,%f,%f",&(newpos[vx]),
	 &(newpos[vy]),&(newpos[vz]));
  setvfe(thisvfe,newpos,
	  thisvfe->startle->motherbound->motherfve->mothershell->motherobj->
	  invxform);

#ifdef IRIS
  qbuttons();
#endif /* IRIS */
}

  void
inquire_evf(char *message,evfptr thisevf)
{
  char instr[40],outstr[40],s1[100];
  float *pt1,*pt2;
  vertype pt3,midevf,evfvec,outer;
  GLfloat length,newlength;
  vertype new1;
  ;
#ifdef IRIS
  unqbuttons();
#endif /* IRIS */

  pt1 = thisevf->le1->facevert->pos;
  pt2 = thisevf->le2->facevert->pos;
  diffpos3d(pt1,pt2,pt3);
  length = magvec(pt3);
  normalize(pt3,evfvec);
  sprintf(s1,"\"%s\" -- \"%f\"",message,length);
#ifdef IRIS
  GetInputFromUser(outstr,40,s1);
#endif
  sscanf(outstr,"%f",&newlength);
  midpoint(pt1,pt2,midevf);
  scalevec3d(evfvec,newlength/2.0,outer);
  addvec3d(midevf,outer,new1);
  setvfe(thisevf->le2->facevert,new1,
	  thisevf->le2->motherbound->motherfve->mothershell->motherobj->
	  invxform);
  flip_vec(outer);
  addvec3d(midevf,outer,new1);
  setvfe(thisevf->le1->facevert,new1,
	  thisevf->le1->motherbound->motherfve->mothershell->motherobj->
	  invxform);

#ifdef IRIS
  qbuttons();
#endif /* IRIS */
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			PLANE SNAPPING ROUTINES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#define Freedomforplane 5.0

  GLfloat
do_sticking(GLfloat planetransz, GLboolean start_sticking)
{
  static GLboolean plane_stuck = FALSE;
  static GLfloat zaccum;
  ;
  if (start_sticking)
  {
    plane_stuck = TRUE;
    zaccum = 0.0;
  }
  else if (plane_stuck)
  {
    zaccum += Fabs(planetransz);
    if (zaccum > Freedomforplane)
      plane_stuck = FALSE;
    else
      return(0.0);		/* if stuck, no ztrans possible!!! */
  }
  return(planetransz);
}

  void
set_nearest_marker(stateptr state)
{
  sortptr thisnearpt;
  GLfloat test,minimum = 10000.0;
  ;
  nearest_nearpt = Nil;
  for (thisnearpt = nearpts_list->first.sort; thisnearpt != Nil;
       thisnearpt = thisnearpt->next)
  {
    test = Fabs(state->cursor[vz] - ((vfeptr) thisnearpt->sortelem)->pos[vz]);
    if (test < minimum)
    {
      minimum = test;
      nearest_nearpt = thisnearpt;
    }
  }    
}


  void
sort_into_nearpts(listptr nearpts_list,vfeptr thisvfe)
{
  sortptr newnearpt,thisnearpt;
  GLfloat *newnearpos,*thisnearpos;
  ;
  newnearpos = thisvfe->pos;
  thisnearpt = nearpts_list->first.sort;
  while (thisnearpt != Nil)
  {
    thisnearpos = ((vfeptr) thisnearpt->sortelem)->pos;
    if (compare(thisnearpos[vz],newnearpos[vz],0.5) == 0.0)
      return;			/* a point already listed that's close enough */
    if (thisnearpos[vz] > newnearpos[vz])
    {
      newnearpt = (sortptr) insert_new_blank_elem(nearpts_list,
						  (elemptr) thisnearpt,
						  Sort_type);
      newnearpt->sortelem = (elemptr) thisvfe;
      return;
    }
    thisnearpt = thisnearpt->next;
  }
  newnearpt = (sortptr) append_new_blank_elem(nearpts_list,Sort_type);
  newnearpt->sortelem = (elemptr) thisvfe;
}


  void
fill_in_nearpts_from_obj(objptr nearobj,listptr nearpts_list)
{
  shellptr thishell;
  vfeptr thisvfe;
  ;
  thishell = nearobj->shellist->first.shell;
  while (thishell != Nil)
  {
    for (thisvfe = First_shell_vfe(thishell); thisvfe != Nil;
	 thisvfe = thisvfe->next)
      sort_into_nearpts(nearpts_list,thisvfe);
    thishell = thishell->next;
  }
}

  void
create_nearpts_list(worldptr world,stateptr state)
{
  featureptr thisfeature;
  shellptr thishell;
  proptr thisprop;
  vfeptr thisvfe;
  ;
  clear_list(nearpts_list);
  thisprop = world->world_proplists[(int) visible_prop]->first.prop;
  while (thisprop != Nil)
  {
    thisfeature = thisprop->owner;
    if (thisfeature->type_id == Obj_type)
      fill_in_nearpts_from_obj((objptr) thisfeature,nearpts_list);
    thisprop = thisprop->next;
  }
  
  set_nearest_marker(state);
  reset_nearpts = FALSE;
}


  void
init_nearpts_list(void)
{
  nearpts_list = create_list();
  nearest_nearpt = Nil;
}

  void
reset_nearpts_list(void)
{
  reset_nearpts = TRUE;
}

  sortptr
go_below_range(GLfloat oldplanez,GLfloat newplanez,sortptr searchpt,
		 GLboolean search_forwards)
{
  GLfloat searchplanez;
  ;
  searchplanez = ((vfeptr) searchpt->sortelem)->pos[vz];
  if (search_forwards)
  {
    while (searchplanez >= oldplanez)
    {
      searchpt = searchpt->prev;
      if (searchpt == Nil)
	return(Nil);	/* failure to get beyond range */
      searchplanez = ((vfeptr) searchpt->sortelem)->pos[vz];
    }
  }
  else
  {
    while (searchplanez <= oldplanez)
    {
      searchpt = searchpt->next;
      if (searchpt == Nil)
	return(Nil);	/* failure to get beyond range */
      searchplanez = ((vfeptr) searchpt->sortelem)->pos[vz];
    }
  } 
  return(searchpt);
}

  sortptr
go_into_range(GLfloat oldplanez,GLfloat newplanez,sortptr searchpt,
		 GLboolean search_forwards)
{
  GLfloat searchplanez;
  ;
  searchplanez = ((vfeptr) searchpt->sortelem)->pos[vz];
  /* first loop till inside the range */
  while (Fabs(searchplanez - newplanez) < Fabs(searchplanez - oldplanez))
  {
    if (search_forwards)
      searchpt = searchpt->prev;
    else
      searchpt = searchpt->next;
    if (searchpt == Nil)
      return(Nil);		/* failure to reach range at all */
    searchplanez = ((vfeptr) searchpt->sortelem)->pos[vz];
  }
  return(searchpt);
}

  sortptr
walk_up_range(GLfloat oldplanez,GLfloat newplanez,sortptr searchpt,
	      GLboolean search_forwards)
{
  GLfloat searchplanez;
  ;
  searchplanez = ((vfeptr) searchpt->sortelem)->pos[vz];
  if (search_forwards)
  {
    if (searchpt->next == Nil)
      return(Nil);		/* couldn't get back into range */
    while (searchplanez < newplanez)
    {
      searchpt = searchpt->next;
      searchplanez = ((vfeptr) searchpt->sortelem)->pos[vz];
      if (searchplanez > oldplanez)
	return(searchpt);
      else if (searchpt->next == Nil)
	return(Nil);		/* no points lie in or above the range! */
    }
  }
  else
  {
    if (searchpt->prev == Nil)
      return(Nil);		/* couldn't get back into range */
    while (searchplanez > newplanez)
    {
      searchpt = searchpt->prev;
      searchplanez = ((vfeptr) searchpt->sortelem)->pos[vz];
      if (searchplanez < oldplanez)
	return(searchpt);
      else if (searchpt->prev == Nil)
	return(Nil);		/* no points lie in or above the range! */
    }
  }
  return(Nil);			/* cound't get back into range */
}

  GLboolean
in_the_range(GLfloat oldplanez,GLfloat newplanez,sortptr searchpt,
	     GLboolean search_forwards)
{
  GLfloat searchplanez;
  ;
  searchplanez = ((vfeptr) searchpt->sortelem)->pos[vz];
  if (search_forwards)
    return((newplanez > searchplanez) && (searchplanez > oldplanez));
  else
    return((oldplanez > searchplanez) && (newplanez < searchplanez));
}


  GLboolean
snap_plane_to_nearestpt(worldptr world,stateptr state,vertype cursor_move)
{
  sortptr searchpt;
  GLfloat absplanemove,oldplanez,newplanez,searchplanez;
  static GLboolean lastmovelarge = FALSE;
  GLboolean search_forwards;
  ;
  absplanemove = Fabs(cursor_move[vz]);
  if (absplanemove > Minplanesnapdist)
  {
    lastmovelarge = TRUE;
    return;			/* note if plane moved a lot and return */
  }
  if (lastmovelarge)
  {      /* if moved a lot last time... */
    if ((reset_nearpts == TRUE) || (nearpts_list->numelems == 0))
      /* and no list (first time through), */
      /* create a new nearpts list */
      create_nearpts_list(world,state);
    else
      /* else just set the mark */
      set_nearest_marker(state);
    lastmovelarge = FALSE;
  }
  searchpt = nearest_nearpt;	/* set equal to global search mark */
  if (searchpt == Nil)
    return;			/* there are no points in the world! */
  searchplanez = ((vfeptr) searchpt->sortelem)->pos[vz];

  search_forwards = (cursor_move[vz] > 0.0);

  oldplanez = state->cursor[vz];
  newplanez = oldplanez+cursor_move[vz];
  /* If the current searchpt is below the range, search in move direction*/
  /* until you get just past the oldplanepos, and you're done.  (If you don't */
  /* find a searchpt, remember the one right above the range for a later */
  /* search.) If the current searchpt is above the range, search opposite to */
  /* the move direction till you hit a point just below the range, and then */
  /* step back up into the range as before.  If you are in the range, step */
  /* till you're just below the range, and then step back up into the range.*/

  /* searchpt is sitting on an endpt of range: only go below range. (Note */
  /* that the most likely condition that the searchplanez be sitting right */
  /* on the endpoints of the range is when it was placed there by the last */
  /* call to this routine which snapped.) */
  if ((searchplanez == oldplanez) || (searchplanez == newplanez) ||
      (in_the_range(oldplanez,newplanez,searchpt,search_forwards)))
    searchpt = go_below_range(oldplanez,newplanez,searchpt,search_forwards);
  /* searchpt is above range: go into the range, then below range */
  else if (Fabs(searchplanez - newplanez) < Fabs(searchplanez - oldplanez))
  {
    searchpt = go_into_range(oldplanez,newplanez,searchpt,search_forwards);
    if (searchpt == Nil)
      return;			/* coudn't get into range. */
    searchpt = go_below_range(oldplanez,newplanez,searchpt,search_forwards);
  }

  if (searchpt == Nil)
    return;			/* coudn't get below range. */

  /* walk back up through range until you either find a nearpt or you exit */
  /* top of range. if you exit top of range, record that searchpt as the new */
  /* one but don't try to snap the cursor_move to that nearpt. If on the */
  /* other hand the nearpt is the same as the most current nearpt, don't */
  /* do anything either. Otherwise, adjust the delta to snap onto the nearpt. */
  searchpt = walk_up_range(oldplanez,newplanez,searchpt,search_forwards);
  if (searchpt == Nil)
    return;			/* coudn't get back into or above range. */

  if ((in_the_range(oldplanez,newplanez,searchpt,search_forwards)) &&
      (searchpt != nearest_nearpt))
  {
    searchplanez = ((vfeptr) searchpt->sortelem)->pos[vz];
    cursor_move[vz] = searchplanez - state->cursor[vz];
    nearest_nearpt = searchpt;
    return(TRUE);		/* returns true if snapping took place */
  }
  return(FALSE);
}

