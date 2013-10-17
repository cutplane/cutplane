
/* FILE: debug.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*           Printing/Debugging Routines for the Cutplane Editor          */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define DEBUGMODULE

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <platform_gl.h>
#include <stdlib.h>		// For malloc
#include "cutplane.h"

#include "euler.h"
#include "debug.h"
#include "globals.h"
#include "vertex.h"
#include "controls.h"
#include "update.h"
#include "slice.h"
#include "color.h"

FILE *dbgdump;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*			GLOBAL VARIABLES				*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                         WINDOZE DEBUG ROUTINES                           */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

void init_dbgdump(char *dbgdump_filename)
{
  dbgdump = fopen (dbgdump_filename, "w");
}

void close_dbgdump()
{
  fclose(dbgdump);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                         PRINTING ROUTINES                                */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
printvert (vertype thevert)
{
  printf (" [%10.8f,%10.8f,%10.8f]\n",(float)thevert[vx],(float)thevert[vy],(float)thevert[vz]);
}

  void
iprintvert (vertype thevert)
{
  printf (" [%d,%d,%d]\n",
	  iround(thevert[vx]),iround(thevert[vy]),iround(thevert[vz]));
}

  void
print_le(leptr thisle)
{
  int vert1,vert2;
  shellptr thishell;
  ;
  thishell = thisle->motherbound->motherfve->mothershell;
  vert1 = get_elem_index((elemptr) thisle->facevert,(listptr) thishell->vfelist);
  vert2 = get_elem_index((elemptr) (Twin_le(thisle)->facevert),
			 (listptr) thishell->vfelist);
  
  printf("\n\t[%d,%d]\n",vert1,vert2);
}

  void
printle_coords(leptr thisle)
{
  printf("address: 0x%p\n", (void *) thisle);
  printvert(thisle->facevert->pos);
  printf("   :   ");
  printvert(thisle->next->facevert->pos);
}

  void
printevf(evfptr thisevf)
{
  shellptr thishell;
  ;
  thishell = thisevf->le1->motherbound->motherfve->mothershell;
  
  printf ("Edge %d contains verts [%d,%d] ",
	  get_elem_index((elemptr) thisevf,
			 thishell->evflist),
	  get_elem_index((elemptr) thisevf->le1->facevert,
			 (listptr) thishell->vfelist),
	  get_elem_index((elemptr) thisevf->le2->facevert,
			   (listptr) thishell->vfelist));
  printf ("is part of faces [%d,%d] ",
	  get_elem_index((elemptr) thisevf->le1->motherbound->motherfve,
			 (listptr) thishell->fvelist),
	  get_elem_index((elemptr) thisevf->le2->motherbound->motherfve,
			 (listptr) thishell->fvelist));
  printf ("@ [%p, %p]\n", thisevf->f1, thisevf->f2);
  printf ("Coords:\n");
  printvert(thisevf->le1->facevert->pos);
  printf(" : ");
  printvert(thisevf->le2->facevert->pos);
  printf("\n");
}

  void
printevfs (shellptr thishell)
{
  evfptr thisevf;
  ;
  thisevf = thishell->evflist->first.evf;
  while (thisevf)
  {
    printevf(thisevf);
    thisevf = thisevf->next;
  }
}

  void
printfaceles(fveptr thisfve)
{
  shellptr thishell;
  leptr startle,thisle;
  boundptr thisbound;
  int count;
  ;
  thishell = thisfve->mothershell;
  thisbound = thisfve->boundlist->first.bnd;
  while (thisbound != Nil)
  {
    if (thisbound->prev != Nil)
      printf("This is a hole:\n");
    count = 0;
    startle = thisle = thisbound->lelist->first.le;
    do
    {
      if (thisle->motherbound == Nil)
	printf("Motherbound == Nil\n");
      else if (thisle->motherbound->motherfve == Nil)
	printf("Motherfve == Nil\n");
      printf ("V (le %p): %d = ", (void *) thisle,
	      get_elem_index((elemptr) thisle->facevert,
			     (listptr) thishell->vfelist));
      printvert(thisle->facevert->pos);
      printf ("\n");
      count++;
      thisle = thisle->next;
    } while (thisle != startle);
    if (count != thisbound->lelist->numelems)
      printf("\n*** count != numelems ***\n");
    thisbound = thisbound->next;
  }
}

  void
iprintfaceles(fveptr thisfve)
{
  shellptr thishell;
  leptr startle,thisle;
  boundptr thisbound;
  int count;
  ;
  thishell = thisfve->mothershell;
  thisbound = thisfve->boundlist->first.bnd;
  while (thisbound != Nil)
  {
    if (thisbound->prev != Nil)
      printf("This is a hole:\n");
    count = 0;
    startle = thisle = thisbound->lelist->first.le;
    do
    {
      if (thisle->motherbound == Nil)
	printf("Motherbound == Nil\n");
      else if (thisle->motherbound->motherfve == Nil)
	printf("Motherfve == Nil\n");
      printf ("V (le %p): %d = ", (void *) thisle,
	      get_elem_index((elemptr) thisle->facevert,
			     (listptr) thishell->vfelist));
      iprintvert(thisle->facevert->pos);
      printf ("\n");
      count++;
      thisle = thisle->next;
    } while (thisle != startle);
    if (count != thisbound->lelist->numelems)
      printf("\n*** count != numelems ***\n");
    thisbound = thisbound->next;
  }
}

  void
printshell_les(shellptr thishell)
{
  fveptr thisfve;
  ;
  thisfve = thishell->fvelist->first.fve;
  while(thisfve != Nil)
  {
    printfaceles(thisfve);
    printf("\n");
    thisfve = thisfve->next;
  }
}

  void
iprintshell_les(shellptr thishell)
{
  fveptr thisfve;
  ;
  thisfve = thishell->fvelist->first.fve;
  while(thisfve != Nil)
  {
    iprintfaceles(thisfve);
    printf("\n");
    thisfve = thisfve->next;
  }
}

  void
printface_evfs(fveptr thisfve)
{
  leptr startle,thisle;
  shellptr thishell;
  ;
  thishell = thisfve->mothershell;
  startle = thisle = thisfve->boundlist->first.bnd->lelist->first.le;
  do
  {
    printf ("[%d] ",get_elem_index((elemptr) thisle->facedge,
				   (listptr) thishell->evflist));
    if ((thisle->facedge->f1 != thisfve) &&
	(thisle->facedge->f2 != thisfve) )
      printf (" le has INCORRECT facedge!");
    thisle = thisle->next;
  } while (thisle != startle);
}

  void
printfves (shellptr thishell)
{
  fveptr thisfve;
  int count = 1;
  ;
  thisfve = thishell->fvelist->first.fve;
  while (thisfve)
  {
    printf("\nFace %d (address %p) contains vertices:\n", count++, (void *) thisfve);
    printfaceles(thisfve);
    printf ("\nand evfs:\n");
    printface_evfs(thisfve);
    printf ("\n");
    thisfve = thisfve->next;
  }
}

  void
printvfefaces(vfeptr thisvfe)
{
  leptr thisle;
  shellptr thishell;
  ;
  thisle = thisvfe->startle;
#ifdef IRIS
  thishell = thisle->motherbound->motherfve->mothershell;
#endif
  printf ("This vertex is a part of the following faces:\n");
  do
  {
    printfaceles(thisle->motherbound->motherfve);
    printf ("\n");
    thisle = (Twin_le(thisle))->next;
  } while (thisle != thisvfe->startle);
}


  void
printvfe(vfeptr thisvfe)
{
  float *vertpos;
  shellptr thishell;
  ;
  thishell = thisvfe->startle->motherbound->motherfve->mothershell;
  vertpos = thisvfe->pos;
  printf ("Vertex %d is at ",get_elem_index((elemptr) thisvfe,
					    (listptr) thishell->vfelist));
  printvert(vertpos);
  printvfefaces(thisvfe);
}

  void
printvfes(shellptr thishell)
{
  vfeptr thisvert;
#ifdef IRIS
  vertype vertpos;
#endif
  ;
  thisvert = thishell->vfelist->first.vfe;
  while (thisvert != Nil)
  {
    printvfe(thisvert);
    thisvert = thisvert->next;
  }
}

  void
	/* OGLXXX XXX fix this. */
printmatrix(GLfloat ** thematrix)
{
  int i,j;
  ;
  printf("\n");
  for (i = 0; i < 4; ++i)
  {
    for (j = 0; j < 4; ++j)
      printf("%11.5f",thematrix[i][j]);
    printf("\n");
  }
}

  void
printfnorms(listptr thisfvelist)
{
  fveptr thisfve;
  int facecount = 1;
#ifdef IRIS
  vertype thefnorm;
#endif
  ;
  thisfve = thisfvelist->first.fve;
  while (thisfve != Nil)
  {
    printf ("For face #%d, the normal is:",facecount++);
    printvert(thisfve->facenorm);
    thisfve = thisfve->next;
    printf (" Time be time,  mom!\n");
  }
}

  void
print_props(featureptr thisfeature)
{
  proptr thisprop;
  ;
  if (thisfeature->proplist->numelems == 0)
    printf ("No properties.\n");
  else
  {
    thisprop = thisfeature->proplist->first.prop;
    while (thisprop != Nil)
    {
      printf("%s\n",propinfo[thisprop->propkind].propname);
      thisprop = thisprop->next;
    }
  }
}

  void
print_transition_props(featureptr thisfeature)
{
  proptr thisprop;
  int thislocation = -1, nextlocation = -1;
  ;
  if (thisfeature->proplist->numelems == 0)
    printf ("No properties.\n");
  else
  {
    thisprop = thisfeature->proplist->first.prop;
    while (thisprop != Nil)
    {
      if (strcmp("Nextlocation",propinfo[thisprop->propkind].propname) == 0)
	thislocation = thisprop->value.i;
      else if (strcmp("Thislocation",propinfo[thisprop->propkind].propname) == 0)
	nextlocation = thisprop->value.i;
      thisprop = thisprop->next;
    }
  }
  if (thislocation > -1)
  {
    printf("Thislocation ");
    switch (thislocation)
    {
    case Infront:
      printf("Infront\n");
      break;
    case Inback:
      printf("Inback\n");
      break;
    case Onplane:
      printf("Onplane\n");
      break;
    }
  }
  if (nextlocation > -1)
  {
    printf("Nextlocation ");
    switch (nextlocation)
    {
    case Infront:
      printf("Infront\n");
      break;
    case Inback:
      printf("Inback\n");
      break;
    case Onplane:
      printf("Onplane\n");
      break;
    }
  }
  printf("\n");
}

  void
print_transition_list(listptr transitionlist)
{
  elemptrptr thistransition;
  ;
  thistransition = transitionlist->first.ep;
  while (thistransition)
  {
    print_transition_props((featureptr) thistransition);
    thistransition = thistransition->next;
  }
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                      BOOLEAN PRINT ROUTINES!!!                         */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
print_sector(sectorptr thisector)
{
  shellptr thishell;
  leptr centerle;
  ;
  centerle = thisector->centerle;
  thishell = centerle->motherbound->motherfve->mothershell;
  printf("centerle ");
  printle_coords(thisector->centerle);
  printf("center vfe index = %d\n",get_elem_index((elemptr) centerle->facevert,
						  thishell->vfelist));

  printf("ref1 is: ");
  printvert(thisector->ref1);

  printf("ref2 is: ");
  printvert(thisector->ref2);

  printf("ref1xref2 is: ");
  printvert(thisector->ref1xref2);

  printf("sector normal is: ");
  printvert(thisector->sectornorm);

/*
  printf("vf sidecodes are: [%d,%d]\n",
	 thisector->vfsidecodes[0],thisector->vfsidecodes[1]);
*/
}

  void
print_sectorpair(sectorpairptr thisectorpair)
{
  printf("intersectcode = %d\n",thisectorpair->intersectcode);
  printf("sidecodesA = [%d,%d]\n",
	 thisectorpair->sidecodesA[0],thisectorpair->sidecodesA[1]);
  printf("sidecodesB = [%d,%d]\n",
	 thisectorpair->sidecodesB[0],thisectorpair->sidecodesB[1]);
  printf("\nsectorA is:\n");
  print_sector(thisectorpair->sectorA);
  printf("\nsectorB is:\n");
  print_sector(thisectorpair->sectorB);
}

  void
print_sectorpairlist(listptr sectorpairlist)
{
  sectorpairptr thisectorpair;
  int i = 1;
  ;
  thisectorpair = sectorpairlist->first.sectorpair;
  while (thisectorpair)
  {
    printf("\nSECTORPAIR %d :\n",i++);
    print_sectorpair(thisectorpair);
    thisectorpair = thisectorpair->next;
  }
}

  void
print_sectorlist(listptr sectorlist)
{
  sectorptr thisector;
  int i = 1;
  ;
  thisector = sectorlist->first.sector;
  while (thisector)
  {
    printf("\nSECTOR %d :\n",i++);
    print_sector(thisector);
    thisector = thisector->next;
  }
}

  void
print_veelist(listptr vee_list)
{
  veeptr thisvee;
  int veecnt = 1;
  ;
  for (thisvee = vee_list->first.vee; thisvee != Nil; thisvee = thisvee->next)
  {
    printf("\nVee:%d\n",veecnt++);
    printf ("Evf A:\n");
    printevf(thisvee->evfA);
    printf ("Evf B:\n");
    printevf(thisvee->evfB);
    printf("Breakpos:");
    printvert(thisvee->breakpos);
    printf ("\nOnptcode:%d\n\n",thisvee->onptcode);
  }
}

  void
print_coincidents(listptr vv_list,listptr vf_list)
{
  vvptr thisvv;
  vfptr thisvf;
  int count = 1;
  ;
  thisvv = vv_list->first.vv;
  while (thisvv != Nil)
  {
    printf("\n\n");
    printf ("Coincident vertex-vertex pair #%d:\n",count++);
    printf("Vertex 1:");
    printvert(thisvv->vfeA->pos);
    printf("\n");
    printf("Vertex 2:");
    printvert(thisvv->vfeB->pos);
    printf("\n");
    thisvv = thisvv->next;
  }
  count = 1;
  thisvf = vf_list->first.vf;
  while (thisvf != Nil)
  {
    printf("\n\n");
    printf ("Coincident vertex-face pair #%d:\n",count++);
    if (thisvf->touchingelem[SolidA]->type_id == Vfe_type)
    {
      printf("Vertex 1:");
      printvert(((vfeptr) thisvf->touchingelem[SolidA])->pos);
      printf("\n");
    }
    else if (thisvf->touchingelem[SolidA]->type_id == Fve_type)
    {
      printf ("Face 1:\n");
      printfaceles((fveptr) thisvf->touchingelem[SolidA]);
    }
    else
      system_error("print_coincidents: bad type_id (vfstruct must be bad)");
    if (thisvf->touchingelem[SolidB]->type_id == Vfe_type)
    {
      printf("Vertex 2:");
      printvert(((vfeptr) thisvf->touchingelem[SolidB])->pos);
      printf("\n");
    }
    else if (thisvf->touchingelem[SolidB]->type_id == Fve_type)
    {
      printf ("Face 2:\n");
      printfaceles((fveptr) thisvf->touchingelem[SolidB]);
    }
    else
      system_error("print_coincidents: bad type_id (vfstruct must be bad)");
    thisvf = thisvf->next;
  }
}

  void
printnbrhood(listptr sectorlist)
{
  sectorptr thisector;
#ifdef IRIS
  leptr centerle;
#endif
  vfeptr centervfe;
  int count = 0;
  ;
  printf("\nNeighborhood le's:\n");
  thisector = sectorlist->first.sector;
  if (thisector == Nil)
    printf("\n***** this sectorlist has 0 elements!! ****\n");
  while (thisector)
  {
    centervfe = thisector->centerle->facevert;
    count++;
    print_le(thisector->centerle);
    if (thisector->centerle->facevert != centervfe)
      printf("\n***** the center vfe for sector %d is different!! ****\n",
	     count);
    thisector = thisector->next;
  }
}

  void
printnbrhoods(listptr nbrhoodlist)
{
  elemptr thiselem;
  int count = 0;
  ;
  thiselem = nbrhoodlist->first.any;
  while (thiselem)
  {
    count++;
    if (thiselem->type_id == Vf_type)
      printnbrhood(((vfptr) thiselem)->sectorlist);
    else if (thiselem->type_id == Vv_type)
    {
      printnbrhood(((vvptr) thiselem)->sectorlistA);
      printnbrhood(((vvptr) thiselem)->sectorlistB);
    }
    else
      printf("\n***** this list element is not a neighborhood!! ****\n");
    thiselem = thiselem->next;
  }
}

  void
check_all_nbrs(shellptr thishell)
{
  void printneighbors(vfeptr);
  listptr vfelist;
  vfeptr thisvfe;
  ;
  vfelist = thishell->vfelist;
  thisvfe = vfelist->first.vfe;
  while (thisvfe)
  {
    printf("\n\ncenterle: ");
    printf("%d",get_elem_index((elemptr) thisvfe->startle->facevert,vfelist));
    printvert(thisvfe->pos);
    printf("neighbors: ");
    printneighbors(thisvfe);
    thisvfe = thisvfe->next;
  }
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                      EULER PRINT ROUTINES!!!                           */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


  void
printscreenbbox(bboxscreenptr thebbox)
{
  printf ("[%d,%d]-->[%d,%d]\n",thebbox->mincorner[vx],thebbox->mincorner[vy],
	  thebbox->maxcorner[vx],thebbox->maxcorner[vy]);
}


  void
printneighbors(vfeptr thisvfe)
{
  leptr thisle,startle;
  ;
  startle = thisvfe->startle;
  thisle = startle;
  if (thisle)
  {
    do
    {
      /* this assumes all faces have the same orientation!!! */
      printf("%d ",
	     get_elem_index((elemptr) thisle->next->facevert,
			    (listptr) thisle->motherbound->motherfve->
			    mothershell->vfelist));
      thisle = Twin_le(thisle)->next;
    }
    while (thisle != startle);
  }
}

  void
print_face_coords(fveptr thisfve)
{
  boundptr thisbound;
  leptr thisle,firstle;
  float *thispos;
  ;
  thisbound = thisfve->boundlist->first.bnd;
  printf("\n");
  while(thisbound)
  {
    thisle = thisbound->lelist->first.le;
    firstle = thisle;
    do
    {
      printf("For vertex #%d,",get_elem_index((elemptr) thisle->facevert,
					      (listptr) thisfve->mothershell->
					      vfelist));
      thispos = thisle->facevert->pos;
      printf(" position: [%f,%f,%f]\n",thispos[vx],thispos[vy],thispos[vz]);
      thisle = thisle->next;
    } while(thisle != firstle);
    thisbound = thisbound->next;
  }
}

  void
print_face(fveptr thisfve)
{
  boundptr thisbound;
  leptr thisle,firstle;
  int boundnum = 1;
  ;
  thisbound = thisfve->boundlist->first.bnd;
  printf("\nFace vertices for face %p:", (void *) thisfve);
  while(thisbound)
  {
    thisle = thisbound->lelist->first.le;
    firstle = thisle;
    printf("\n    For boundary %d vertex indices are:\n\t",boundnum);
    do
    {
      printf("%d ",
	     get_elem_index((elemptr) thisle->facevert,
			    (listptr) thisfve->mothershell->vfelist));
      thisle = thisle->next;
    } while(thisle != firstle);
    boundnum++;
    thisbound = thisbound->next;
  }
  printf("\n");
}

  void
print_faces(shellptr thishell)
{
  boundptr thisbound;
  fveptr thisfve;
  leptr thisle,firstle;
  int boundnum = 1;
  ;
  thisfve = thishell->fvelist->first.fve;
  printf("\n");
  while(thisfve)
  {
    boundnum = 1;
    thisbound = thisfve->boundlist->first.bnd;
    printf("Face vertices for face %p:", (void *) thisfve);
    while(thisbound)
    {
      thisle = thisbound->lelist->first.le;
      firstle = thisle;
      printf("\n    For boundary %d vertex indices are:\n\t",boundnum);
      do
      {
	printf("%d ",
	       get_elem_index((elemptr) thisle->facevert,
			      (listptr) thisfve->mothershell->vfelist));
	thisle = thisle->next;
      } while(thisle != firstle);
      boundnum++;
      thisbound = thisbound->next;
    }
    print_face_coords(thisfve);
    printf("\n\n");
    thisfve = thisfve->next;
  }
}
  void
old_print_evfs(listptr evflist)
{
  evfptr thisevf;
  ;
  thisevf = evflist->first.evf;
  while (thisevf)
  {
    if (thisevf->le1)
      print_le(thisevf->le1);
    else
      printf("le1 == Nil\n");
    if (thisevf->le1)
      print_le(thisevf->le2);
    else
      printf("le2 == Nil\n");
    thisevf = thisevf->next;
  }
}

  void
print_evfs(listptr evflist)
{
  evfptr thisevf;
  ;
  thisevf = evflist->first.evf;
  while (thisevf)
  {
    printf("le1 = %p", (void *) thisevf->le1);
    printf("le2 = %p\n", (void *) thisevf->le2);
    thisevf = thisevf->next;
  }
}

  void
printloopart(leptr le1)
{
  leptr startle,startwin;
  ;
  startle = le1;
  startwin = Twin_le(le1);
  le1 = le1->next;
  do
  {
    printf("[%p] ", (void *) le1);
    le1 = le1->next;
  } while ((le1 != startwin) && (le1 != startle));
  printf("\n");
}

  void
print_lasso(leptr le1)
{
  leptr startle;
  ;
  startle = le1;
  do
  {
    printf("[%p] ", (void *) le1->next);
    le1 = Twin_le(le1->next->next);
  } while (le1 != startle);
  printf("\n");
}

  void
print_null_le_lassoes(listptr snip_null_lelist)
{
  lepairptr thislepair;
  ;
  printf ("\n\nShell A null les:\n");
  thislepair = snip_null_lelist->first.lp;
  while (thislepair != Nil)
  {
    printf("Shell A null_le:%p\n", (void *) thislepair->le[SolidA]);
    printle_coords(thislepair->le[SolidA]);
    printf ("Top Portion:\n");
    print_lasso(thislepair->le[SolidA]);
    printf ("Bottom Portion:\n");
    print_lasso(Twin_le(thislepair->le[SolidA]));
    if (thislepair->isahole[SolidA])
      printf("\n\t\t*** THIS IS A HOLE!!! \n");
    thislepair = thislepair->next;
  }
  printf ("\n\nShell B null les:\n");
  thislepair = snip_null_lelist->first.lp;
  while (thislepair != Nil)
  {
    printf("Shell B null_le:%p\n", (void *) thislepair->le[SolidB]);
    printle_coords(thislepair->le[SolidB]);
    printf ("Top Portion:\n");
    print_lasso(thislepair->le[SolidB]);
    printf ("Bottom Portion:\n");
    print_lasso(Twin_le(thislepair->le[SolidB]));
    if (thislepair->isahole[SolidB])
      printf("\n\t\t*** THIS IS A HOLE!!! \n");
    thislepair = thislepair->next;
  }
}  

  void
planarity_check(shellptr thishell)
{
  vfeptr thisvfe;
  leptr thisle;
  GLfloat *testpt;
  ;
  for (thisvfe = thishell->vfelist->first.vfe; thisvfe != Nil;
       thisvfe = thisvfe->next)
  {
    testpt = thisvfe->pos;
    thisle = thisvfe->startle;
    do
    {
      if (!(pt_near_plane_3d(testpt,thisle->motherbound->motherfve->facenorm,
			     thisle->motherbound->motherfve->planedist)))
      {
	printf("*** planarity_check: vfepos not in its face-plane!\n");
	printvert(testpt);
	printf("Distance from plane = %f\n",
	       pt_to_planedist_3d(testpt,
				  thisle->motherbound->motherfve->facenorm,
				  thisle->motherbound->motherfve->planedist));
	printf("Face normal is:\n");
	printvert(thisle->motherbound->motherfve->facenorm);
	printf("Face distance = %f\n",
	       thisle->motherbound->motherfve->planedist);
	printf ("Face:\n");
	printfaceles(thisle->motherbound->motherfve);
	printf("----------------------\n\n");
      }
      thisle = Twin_le(thisle)->next;
    } while (thisle != thisvfe->startle);
  }
}

  void
walk_shell(leptr startle)
{
  fveptr thisfve;
  boundptr thisbound;
  leptr thisle;
  ;
  thisfve = startle->motherbound->motherfve;
  thisbound = thisfve->boundlist->first.bnd;
  while (thisbound != Nil)
  {
    thisle = thisbound->lelist->first.le;
    do
    {
      if (!has_property((featureptr) thisle,touched_prop))
      {
	add_property((featureptr) thisle,touched_prop);
	walk_shell(Twin_le(thisle));
      }
      thisle = thisle->next;
    } while (thisle != thisbound->lelist->first.le);
    thisbound = thisbound->next;
  }	     
}

/* NOTE: the following does not snag handles that aren't ring through holes */
/* e.g. it won't catch fully triangulated donuts */
/* to catch these stubborn cases do the following: */
/* grow a single hole in the solid by cutting (or touching) away edges one */
/* at a time. when you get to an edge that is the same face on both sides */
/* (yet is not a strut) check to see how many seperate solids you will */
/* generate when you cut it ... if 2 or 0 than its not a donut */


  int
confirm_handle(leptr firstle)
{
  leptr thisle;
  ;
  clear_property(touched_prop);
  thisle = firstle;
  do
  {
    add_property((featureptr) Twin_le(thisle),touched_prop);
    thisle = thisle->next;
  } while (thisle != firstle);
  walk_shell(Twin_le(firstle));

  if (has_property((featureptr) firstle,touched_prop))
    return(TRUE);		/* this was a handle!! */

  return(FALSE);
}


  void
count_rings_holes(shellptr thishell,int *r, int *h)
{
  boundptr thisbound;
  fveptr thisfve;
  leptr firstle;
  ;
  *r = *h = 0;
  thisfve = thishell->fvelist->first.fve;
  while (thisfve != Nil)
  {
    if (thisfve->boundlist->numelems > 1) /* if there is a ring */
    {			/* check to see if actually a ring or a handle*/
      thisbound = thisfve->boundlist->first.bnd->next;
      while (thisbound != Nil)
      {
	firstle = thisbound->lelist->first.le;
	(*r)++;			/* always increase ring count */
	*h += confirm_handle(firstle); /* maybe increase handle count */
	thisbound = thisbound->next;
      }
    }
    thisfve = thisfve->next;
  }
}

  int
compute_shells(shellptr thishell)
{
  int count = 0;
  evfptr thisevf;
  ;
  clear_property(touched_prop);
  thisevf = thishell->evflist->first.evf;
  while (thisevf != Nil)
  {
    if (!has_property((featureptr) thisevf->le1,touched_prop))
    {
      count++;
      walk_shell(thisevf->le1);
    }
    thisevf = thisevf->next;
  }
  return(count);
}

/* Check for proper shell topology. If it is bad, report why and return FALSE*/

  GLboolean
topology_check(shellptr thishell)
{
  GLboolean result = TRUE,found;
  evfptr thisevf;
  fveptr thisfve;
  boundptr thisbound;
  leptr thisle;
  int eulercheck;
  int v,e,f,s,r,h;
  vfeptr thisvfe,othervfe;
  float *vertpos;
  ;
  /* Do euler equation check */
  f = thishell->fvelist->numelems;
  v = thishell->vfelist->numelems;
  e = thishell->evflist->numelems;
  eulercheck = v - e + f;
  if (eulercheck != 2)	/* we might have face-holes (rings) and handles */
  {
    count_rings_holes(thishell,&r,&h);
    s = compute_shells(thishell);
    if (eulercheck != 2 * (s - h) + r)
    {
      printf("\n*** topology_check: shell %p fails euler equation!\n",
	     (void *) thishell);
      printf("\tleft side of euler eqn = %d",eulercheck);
      printf(" but should be: %d\n",2 * (s - h) + r);
      result = FALSE;
    }
  }

  thisevf = thishell->evflist->first.evf;
  while (thisevf != Nil)
  {
    /* check for struts */
    if (thisevf->le1->motherbound->motherfve ==
	thisevf->le2->motherbound->motherfve)
    {
      printf("\n*** topology_check: shell %p has a strut!\n", (void *) thishell);
      printf ("at edge:\n");
      printevf(thisevf);
      result = FALSE;
    }
    else
    {
      /* see if this edge is in boundlist of the faces it points to */
      thisfve = thisevf->le1->motherbound->motherfve;
      found = FALSE;
      for (thisbound = thisfve->boundlist->first.bnd; thisbound != Nil;
	   thisbound = thisbound->next)
      {
	thisle = thisbound->lelist->first.le;
	do
	{
	  if (thisle->facedge == thisevf)
	  {
	    found = TRUE;
	    break;
	  }
	  thisle = thisle->next;
	} while (thisle != thisbound->lelist->first.le);
      }
      if (!found)
      {
	printf("\n** topology_check: shell %p edge not in neighbor faces!",
	       (void *) thishell);
	printf ("at edge\n");
	printevf(thisevf);
	result = FALSE;
      }      
      thisfve = thisevf->le2->motherbound->motherfve;
      found = FALSE;
      for (thisbound = thisfve->boundlist->first.bnd; thisbound != Nil;
	   thisbound = thisbound->next)
      {
	thisle = thisbound->lelist->first.le;
	do
	{
	  if (thisle->facedge == thisevf)
	  {
	    found = TRUE;
	    break;
	  }
	  thisle = thisle->next;
	} while (thisle != thisbound->lelist->first.le);
      }
      if (!found)
      {
	printf("\n** topology_check: shell %p edge not in neighbor faces!",
	       (void *) thishell);
	printf ("at edge\n");
	printevf(thisevf);
	result = FALSE;
      }      
    }
    thisevf = thisevf->next;
  }	

  /* Also do vertex integrity check */

  for (thisvfe = thishell->vfelist->first.vfe; thisvfe != Nil;
       thisvfe = thisvfe->next)
    for (othervfe = thishell->vfelist->first.vfe; othervfe != Nil;
       othervfe = othervfe->next)
      if ((thisvfe != othervfe) &&
	  (pt_near_pt_3d(thisvfe->pos,othervfe->pos,1.0e-2)))
      {
	printf ("*** topology_check:Error, overlapping points...they are:");
	vertpos = thisvfe->pos;
	printf ("Vertex %d is at ",get_elem_index((elemptr) thisvfe,
						  (listptr) thishell->vfelist));
	printvert(vertpos);
	vertpos = othervfe->pos;
	printf ("Vertex %d is at ",get_elem_index((elemptr) othervfe,
						  (listptr) thishell->vfelist));
	printvert(vertpos);
      }
#ifdef vertexonvertexcheck
#endif

  planarity_check(thishell);
  for (thisfve = thishell->fvelist->first.fve; thisfve != Nil;
       thisfve = thisfve->next)
    for (thisbound = thisfve->boundlist->first.bnd; thisbound != Nil;
	 thisbound = thisbound->next)
      if ((thisbound->lelist->last.le == thisbound->lelist->first.le->next) &&
	  (thisbound->lelist->numelems > 2))
      {
	printf("*** topology_check: Major lameness in lelist in face:\n");
	printfaceles(thisfve);
	break;
      }
  
  clear_property(touched_prop);
  return(result);
}

  GLboolean
topology_check_obj(objptr thisobj)
{
  shellptr thishell;
  GLboolean result = TRUE;
  ;
  for (thishell = First_obj_shell(thisobj); thishell != Nil;
       thishell = thishell->next)
    result &= topology_check(thishell);
  return(result);
}

  int
count_obj_polys(objptr thisobj)
{
  int polycount = 0;
  shellptr thishell;
  ;
  if (has_property((featureptr) thisobj,visible_prop))
  {
    thishell = thisobj->shellist->first.shell;
    while (thishell != Nil)
    {
      polycount += thishell->fvelist->numelems;
      thishell = thishell->next;
    }
  }
  return(polycount);
}

  int
count_objtree_polys(objptr thisobj)
{
  int polycount = 0;
  objptr thischild;
  ;
  polycount += count_obj_polys(thisobj);
  thischild = thisobj->children->first.obj;
  while (thischild != Nil)
  {
    polycount += count_objtree_polys(thischild);
    thischild = thischild->next;
  }
  return(polycount);
}

  int
count_world_polys(worldptr world)
{
  int polycount = 0;
  objptr thischild;
  ;
  thischild = world->root->children->first.obj;
  while (thischild != Nil)
  {
    polycount += count_objtree_polys(thischild);
    thischild = thischild->next;
  }

  return(polycount);
}

#ifdef IRIS

void
heapinfo()
{
  struct mallinfo info;
  int heapused,heapfree;
  ;
  info = mallinfo();
  heapused = info.usmblks + info.uordblks;
  heapfree = info.fsmblks + info.fordblks;
/*
  printf("used space = %d, free space = %d\n",heapused,heapfree);
*/
  printf("\t\tusmblks:%d\tfsmblks:%d\n", info.usmblks,info.fsmblks);
}

#endif /* IRIS */

/* This was only for the SGI */
#ifdef NEED_TO_DUMP_SCENE
void
dump_scene(void)
{
  unsigned long *data;
  long numbytes,numpixels,i,width,height,newindow;
  colortype temp;
  long header[4];
  FILE *datafile;
  /* OGLXXX
   * ZMAX not needed -- always 1.
   * getgdesc other posiblilties:
   * 	glxGetConfig();
   * 	glxGetCurrentContext();
   * 	glxGetCurrentDrawable();
   * GLint gdtmp;
   */
  float zbuffmax = (long) (glGetIntegerv(XXX_ZMAX, &gdtmp), gdtmp);
  ;
  printf ("Starting dump:\n\n");
  width = mainwindowright - mainwindowleft + 1;
  height = mainwindowtop - mainwindowbottom + 1;
  numpixels = width*height;
  numbytes = numpixels * sizeof(unsigned long);
  data = (unsigned long *) alloc_elem(numbytes);

  printf ("Reading RGB...\n");
  glReadBuffer(GL_FRONT);
  /* OGLXXX lrectread: see man page for glReadPixels */
  glReadPixels(0, 0, (width-1)-(0)+1, (height-1)-(0)+1, GL_RGBA, GL_BYTE, data);
  datafile = fopen("image.dat","w");
  printf ("Writing RGB...\n");
  
  header[0] = 0;		/* x-origin */
  header[1] = 0;		/* y-origin */
  header[2] = width;
  header[3] = height;
  fwrite(header,sizeof(header),1,datafile);

  fwrite(data,sizeof(unsigned long),numpixels,datafile); /* fwrite does append */
  fclose(datafile);

  printf ("Reading Depth...\n");
  /* OGLXXX SRC_ZBUFFER: use GL_DEPTH_COMPONENT in glReadPixels call */
  glReadBuffer(XXX);
  /* OGLXXX lrectread: see man page for glReadPixels */
  glReadPixels(0, 0, (width-1)-(0)+1, (height-1)-(0)+1, GL_RGBA, GL_BYTE, data);
  datafile = fopen("rangemap.dat","w");
  printf ("Writing Depth...\n");
  fwrite(header,sizeof(header),1,datafile);
  fwrite(data,sizeof(unsigned long),numpixels,datafile);
  fclose(datafile);
  printf ("Done dumping.\n");

  /* OGLXXX SRC_AUTO not really supported -- see glReadBuffer man page */
  glReadBuffer(GL_BACK);

  /* OGLXXX change glClearDepth parameter to be in [0, 1] */
  glClearDepth(zbuffmax);glClearColor(((float)((0)&0xff))/255., (float)((0)>>8&0xff)/255., (float)((0)>>16&0xff)/255., (float)((0)>>24&0xff)/255. );glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
  /* OGLXXX
   * qenter not supported -- See Events
   * qenter(REDRAW,0)
   */
  /*DELETED*/;
  CPL_turnon_arrowcursor(world_list,state); /* aggghack */

  printf("Verifying dump...\n");
  /* OGLXXX
   * prefposition not supported -- See Window Manager
   * prefposition(mainwindowleft+10,mainwindowright+10,	       mainwindowbottom-10,mainwindowtop-10)
   */
  /*DELETED*/;
  /* OGLXXX
   * winopen not supported -- See Window Manager
   * winopen("Dump")
   */
  newindow = /*DELETED*/;
  /* OGLXXX RGBmode: use GLX_RGBA in attriblist */
  glxChooseVisual(*dpy, screen, *attriblist);
  /* OGLXXX singlebuffer: don't use GLX_DOUBLEBUFFER in attriblist */
  glxChooseVisual(*dpy, screen, *attriblist);
  /* OGLXXX
   * gconfig not supported:
   * 	collect glxChooseVisual attributes into one list for visual selection
   * gconfig()
   */
  /*DELETED*/;
  /* OGLXXX change glClearDepth parameter to be in [0, 1] */
  glClearDepth(zbuffmax);glClearColor(((float)((0)&0xff))/255., (float)((0)>>8&0xff)/255., (float)((0)>>16&0xff)/255., (float)((0)>>24&0xff)/255. );glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
  datafile = fopen("rangemap.dat","r");
  printf ("Rereading depth...\n");
  fread(header,sizeof(header),1,datafile);
  fread(data,sizeof(unsigned long),numpixels,datafile);

  numpixels = header[2]*header[3];
  for (i=0; i<numpixels; i++)
    {
      temp[0] = iround(255.0 * (float) data[i]/zbuffmax);
      temp[1] = temp[2] = temp[0];
      data[i] = packcolor(temp);
    }
  /* OGLXXX lrectwrite: see man page for glDrawPixels */
  glRasterPos2i(0, 0);glDrawPixels((header[2]-1)-(0)+1, (header[3]-1)-(0)+1, GL_RGBA, GL_BYTE, data);

  /* OGLXXX
   * winset not supported -- See Window Manager
   * winset(mainwindow)
   */
  /*DELETED*/;
}

#endif
