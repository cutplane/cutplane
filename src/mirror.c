
/* FILE: mirror.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*         Object Mirroring Routines for the Cutplane Editor              */
/*                                                                        */
/* Author: WAK                                    Date: December 18,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define MIRRORMODULE

#include <config.h>
#include <stdio.h>

#include <platform_gl.h>

#include <cutplane.h>
#include <update.h>
#include <mirror.h>

/* This mirrors an shell about an INPLANE face and then glues the duplicate */
/* to it using glue ops in boolean.c. */

  void
mirror_obj(worldptr world,objptr mirror_obj, fveptr mirror_fve,stateptr state)
{
  vertype planescale,planeoffset;
  objptr otherhalf;
  fveptr glue_fve;
  Boolean reversed;
  ;
  if (mirror_fve->type_id != Fve_type)
    return;			/* do not do anything if not a face!!! */
  if ((compare(mirror_fve->facenorm[vz],1.0,Ptonlinetol) != 0) &&
      (compare(mirror_fve->facenorm[vz],-1.0,Ptonlinetol) != 0))
    return;			/* Does not do outofplane faces right now */
  setpos3d(planescale,1.0,1.0,-1.0);
  setpos3d(planeoffset,0.0,0.0,state->cursor[vz]);
  otherhalf = duplicate_obj(mirror_obj);
  glue_fve = find_same_face(mirror_fve,otherhalf,&reversed);
  scale_feature((featureptr) otherhalf,planescale,planeoffset);

  if (reversed)
    flip_obj_orientation(otherhalf);
  printf ("the mirror face is:\n");
  printfaceles(mirror_fve);
  printf ("the glue face is:\n");
  printfaceles(glue_fve);
  glue_objects_along_faces(glue_fve,mirror_fve,FALSE);

  log_update((featureptr) mirror_obj,Cutevfsflag | Makecutsectionflag |
	     Findnormalsflag | Getselectableflag, Immediateupdate);
}  
