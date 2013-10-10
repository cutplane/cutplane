 
/* FILE: update.h */

#ifndef update_includes
#define update_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*      System Update Routines Header File for the Cutplane Editor        */
/*                                                                        */
/* Authors: WAK                                   Date: May 1,1990        */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 William Kessler and Larry Edwards.   */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <platform_gl.h>

#include "geometry.h"
#include "list.h"

#define Immediateupdate TRUE
#define Nonimmediateupdate FALSE

#define Loginoldbuffer TRUE
#define Loginthisbuffer FALSE

#define Useobjbbox TRUE
#define Usegroupbbox FALSE

/* This is how many refreshes during constraining it takes to decide which */
/* axis the user wished to constrain with. */

#define Maxconstrainscans 2

/* Update Flags for Feature updating are defined here.  They affect shells */
/* only. */

#define Maxupdatenames 50	/* increase this if more flags needed below */

#define Noupdateflag 0
#define Transformflag 1
#define Evfeqnsflag (Transformflag*2)
#define Cutevfsflag (Evfeqnsflag*2)
#define Transformcutevfsflag (Cutevfsflag*2)
#define Clearcutsectionflag (Transformcutevfsflag*2)
#define Makecutsectionflag (Clearcutsectionflag*2)
#define Findnormalsflag (Makecutsectionflag*2)
#define Computeplanedistsflag (Findnormalsflag*2)
#define Computebboxflag (Computeplanedistsflag*2)
#define Cleartranslatesflag (Computebboxflag*2)
#define Getselectableflag (Cleartranslatesflag*2)
#define Getnearestptsflag (Getselectableflag*2)
#define Recomputebackfaceflag (Getnearestptsflag*2)
#define Findplanenearptsflag (Recomputebackfaceflag*2)
#define Redrawflag (Findplanenearptsflag*2)
#define Logerasebboxflag (Redrawflag*2)
#define Allupdateflags (Transformflag | Evfeqnsflag | Cutevfsflag | Clearcutsectionflag | Makecutsectionflag | Findnormalsflag | Computebboxflag | Cleartranslatesflag | Getselectableflag | Getnearestptsflag | Recomputebackfaceflag | Findplanenearptsflag | Redrawflag | Logerasebboxflag)

#define Maxupdateflaglength 50
#define Maxupdatetimelength 50

struct updatestruct
{
  long update_flags;
  Boolean update_now;
  statetype state,oldstate;
};

typedef struct updatestruct updatetype;
typedef updatetype * updateptr;

#endif
