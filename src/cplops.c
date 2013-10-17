
/* FILE: cplops.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*   The Cutplane Language (CPL) Opcode Routines for the Cutplane Editor  */
/*                                                                        */
/* Author: WAK                                       Date: August 19,1990 */
/* Version: 1                                                             */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990,1991 William Kessler and             */
/* Larry Edwards. All Rights Reserved.                                    */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define CPLOPSMODULE

#include <config.h>
#include <stdio.h>

#include <platform_gl.h>
#include <math.h>
#include <string.h>

#include <cutplane.h>
#include <cpl_cutplane.h>
#include <update.h>
#include <tools.h>
#include <grafx.h>
#include <select.h>
#include <intersect.h>

#include <math3d.h>

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	                       GLOBAL VARIABLES				     */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* The expansion space for JSRAT calls. */
CPL_word JSRAT_codebloc[JSRATcodeblocsize];
CPL_word_ptr next_free_JSRAT_word;

/* These are variables communicating between cplcore and cplops' JSRAT rtn. */
/* They indicate when to begin and end JSRAT expansion. */
Boolean start_JSRAT = FALSE;
Boolean finish_JSRAT = FALSE;

Boolean update_on = TRUE;

/* This hack allows us to bypass borland's warning message about parameters */
/* not being used. Later, we should clean all those up if they're */
/* really of no use. FOr now, I kinda like knowing those routines can get */
/* at the world_list or state if they need to. */

#ifndef IRIS
listptr cplops_world_list;
#endif

/* Same for state. */
#ifndef IRIS
stateptr cplops_state;
#endif

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	                       SUPPORT ROUTINES				     */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

  Coord *
get_first_vert(featureptr thefeature)
{
  vfeptr thevfe;
  ;
  switch (thefeature->type_id)
  {
  case Vfe_type:
    thevfe = ((vfeptr) thefeature);
    break;
  case Evf_type:
    thevfe = ((evfptr) thefeature)->le1->facevert;
    break;
  case Fve_type:
    thevfe = ((fveptr) thefeature)->boundlist->first.bnd->lelist->first.le->
      facevert;
    break;
  case Shell_type:
    thevfe = First_shell_vfe(((shellptr) thefeature));
    break;
  case Obj_type:
    thevfe = First_obj_vfe(((objptr) thefeature));
    break;
  default:
    return(Nil);
  }
  return(thevfe->pos);
}  

  Boolean
records_equal(char *rec1,char *rec2,int recsize)
{
  int p;
  ;
  p = 0;
  while (p < recsize)
    if ( *(((char *) rec1) + p) != *(((char *) rec2) + p) )
      break;
    else
      p++;
  if (p < recsize)		/* the two auxdata's were not identical */
    return(FALSE);
  return(TRUE);
}

  Boolean
aux_records_equal(CPL_word_ptr wordA,CPL_word_ptr wordB)
{
  CPLauxptr aux1,aux2;
  ;
  if (wordA->aux_kind !=
      wordB->aux_kind)
    return(FALSE);		/* must be same auxiliary data kind, */
				/* first of all */
  aux1 = &(wordA->word_data.CPLauxdata);
  aux2 = &(wordB->word_data.CPLauxdata);

  switch (wordA->aux_kind)
  {
  case CPLaux_int_data:
    return(aux1->CPLintdata == aux2->CPLintdata);
  case CPLaux_char_data:
    return(aux1->CPLchardata == aux2->CPLchardata);
  case CPLaux_string_data:
    return(strcmp(aux1->CPLstringdata,aux2->CPLstringdata) == 0);
  case CPLaux_float_data:
    return(aux1->CPLfloatdata == aux2->CPLfloatdata);
  case CPLaux_boolean_data:
    return(aux1->CPLbooleandata == aux2->CPLbooleandata);
  case CPLaux_address_data:
    return(aux1->CPLaddressdata == aux2->CPLaddressdata);
  case CPLaux_color_data:
    return((aux1->CPLcolordata[0] == aux2->CPLcolordata[0]) &&
	   (aux1->CPLcolordata[1] == aux2->CPLcolordata[1]) &&
	   (aux1->CPLcolordata[2] == aux2->CPLcolordata[2]));
  case CPLaux_duplicate_destroy_data: /* oldshit */
    return(TRUE);
  case CPLaux_owner_feature_data:
    return(aux1->owner_feature == aux2->owner_feature);
  case CPLaux_Croutine_index_data:
    return(aux1->Croutine_index == aux2->Croutine_index);
  case CPLaux_update_time_data:	/* ???? */
    return(TRUE);
  case CPLaux_cutplane_shade_data:
    return(aux1->cutplane_shade == aux2->cutplane_shade);
  case CPLaux_codekind_value_data:
    return(aux1->codekind_value == aux2->codekind_value);
  case CPLaux_new_world_data:
    return(aux1->new_world == aux2->new_world);

    /* these need structure comparisons */
  case CPLaux_update_data:
    return ((aux1->update_data.update_flags == aux2->update_data.update_flags) &&
	    (aux1->update_data.update_now == aux2->update_data.update_now));
  case CPLaux_rotate_data:
    return ((aux1->rotate_data.rotangle == aux2->rotate_data.rotangle) &&
	    (pt_near_pt_3d(aux1->rotate_data.rotpt,aux2->rotate_data.rotpt,
			   Pttopttol)) &&
	    (vectors_nearly_equal(aux1->rotate_data.rotvec,
				  aux2->rotate_data.rotvec,Pttopttol)));
  case CPLaux_scale_data:
    /* this record needs to be changed anyhow */
    return(TRUE);
  case CPLaux_offset_data:
    return(vectors_nearly_equal(aux1->CPLoffsetdata,aux2->CPLoffsetdata,
				Pttopttol));
  case CPLaux_matrix_data:
    /* oy vay is mir... tu blagues! */
    return(TRUE);

  }    
}

  CPL_word_ptr
inc_CPL_blocptr(CPL_word_ptr inc_word,CPL_word_ptr base_address,
		int arraysize)
{
  inc_word++;
  /* If there is not enough room for a full CPL instruction sentence remaining */
  /* in the bloc, then return the base address. */
  if (((base_address + arraysize) - inc_word) < 5)
    return(base_address);
  return(inc_word);
}


  featureptr
find_subfeature(objptr motherobj,property thisprop)
{
  vfeptr thisvfe;
  evfptr thisevf;
  fveptr thisfve;
  shellptr thishell;
  ;
  for (thishell = First_obj_shell(motherobj); thishell != Nil;
       thishell = thishell->next)
  {
    for (thisvfe = First_shell_vfe(thishell);
	 thisvfe != Nil; thisvfe = thisvfe->next)
      if (has_property((featureptr) thisvfe,thisprop))
	return ((featureptr) thisvfe);
    
    for (thisevf = First_shell_evf(thishell);
	 thisevf != Nil; thisevf = thisevf->next)
      if (has_property((featureptr) thisevf,thisprop))
	return((featureptr) thisevf);
    
    for (thisfve = First_shell_fve(thishell);
	 thisfve != Nil; thisfve = thisfve->next)
      if (has_property((featureptr) thisfve,thisprop))
	return ((featureptr) thisfve);
  }
  return(Nil);
}

  void
find_subfeatures(objptr motherobj,property thisprop,
		 listptr cull_list)
{
  vfeptr thisvfe;
  evfptr thisevf;
  fveptr thisfve;
  shellptr thishell;
  ;
  for (thishell = First_obj_shell(motherobj); thishell != Nil;
       thishell = thishell->next)
  {
    for (thisvfe = First_shell_vfe(thishell);
	 thisvfe != Nil; thisvfe = thisvfe->next)
      if (has_property((featureptr) thisvfe,thisprop))
	add_to_featurelist(cull_list, (featureptr) thisvfe);
    
    for (thisevf = First_shell_evf(thishell);
	 thisevf != Nil; thisevf = thisevf->next)
      if (has_property((featureptr) thisevf,thisprop))
	add_to_featurelist(cull_list, (featureptr) thisevf);
    
    for (thisfve = First_shell_fve(thishell);
	 thisfve != Nil; thisfve = thisfve->next)
      if (has_property((featureptr) thisfve,thisprop))
	add_to_featurelist(cull_list, (featureptr) thisfve);
  }
}

/* Assumption here hack!!!! section_z is passed as the state->cursor[vz] */
/* because the section line is supposed to lie in the plane!! */

  Boolean
sectionline_in_3dbbox(objptr testobj,bbox3dptr testbbox,Coord section_z)
{
  shellptr thishell;
  ringptr thisring;
  Coord *endpt1,*endpt2;
  bbox3dtype ringbbox3d;
  ;
  thishell = First_obj_shell(testobj);
  while (thishell != Nil)
  {
    thisring = thishell->ringlist->first.ring;
    while(thisring)
    {
      setpos3d(ringbbox3d.mincorner,thisring->ringbbox.mincorner[vx],
	       thisring->ringbbox.mincorner[vy],section_z);
      setpos3d(ringbbox3d.maxcorner,thisring->ringbbox.maxcorner[vx],
	       thisring->ringbbox.maxcorner[vy],section_z);
      if (point_in_3dbbox(ringbbox3d.mincorner,testbbox) &&
	  point_in_3dbbox(ringbbox3d.maxcorner,testbbox))
	return(TRUE);
      thisring = thisring->next;
    }
    thishell = thishell->next;
  }
  return(FALSE);
}

  void
enter_in_temp_message_block(CPL_word_ptr new_queued_word)
{
  copy_wordA_to_wordB(new_queued_word,
		      &(temp_message_block[end_temp_message_block++]));
}

  void
queue_feature_message(featureptr thefeature, Boolean priority)
{
  int i;
  ;
  if (thefeature != Nil)	/* if Nil, just reset the message block */
  {
    /* If this is the first time this feature has ever received a message, */
    /* create a message queue for it! */
    if (thefeature->messagequeue == Nil)
      thefeature->messagequeue = create_CPL_queue();
    /* Mark this feature with a property so that its code gets called. */
    add_property(thefeature,msgpending_prop);
    if (priority == TRUE)	/* Top priority:queue block at beginning of q.*/
      for (i = end_temp_message_block-1; i >= 0; --i)
	add_to_CPL_queue(thefeature->messagequeue,
			 &(temp_message_block[i]),TRUE);
    else			/* Less priority: can go at end of message q. */
      for (i = 0; i < end_temp_message_block; ++i)
	add_to_CPL_queue(thefeature->messagequeue,
			 &(temp_message_block[i]),FALSE);
    
    restart_CPLparser = TRUE;	/* Catch any unprocessed messages on next */
				/* refresh in case next refresh generates no */
				/* CPL code. */
  }
  reset_temp_message_block = TRUE;
}

  void
message_block_reset(void)
{
  reset_temp_message_block = FALSE;
  end_temp_message_block = 0;
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	                       OPCODE ROUTINES				     */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */


/* Does nothing! */
  void
execute_NOP(listptr world_list,stateptr state)
{
#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* This simply moves a CPL word or if the word contains aux data or a */
/* feature/prop list it copies the lists (leaves original in *AS_ptr). */
  void
execute_MOVE(listptr world_list,stateptr state)
{
  copy_wordA_to_wordB(&AS,&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* This adds two addresses, two numbers, or a number and an address */
/* and puts the result in AR. It can also add two offsets. */
  void
execute_ADD(listptr world_list,stateptr state)
{
  if ((AS.word_type == Auxiliary_data) && (AT.word_type == Auxiliary_data))
  {
    if ((AS.aux_kind == CPLaux_offset_data) &&
	(AT.aux_kind == CPLaux_offset_data))
    {
      setup_CPL_auxword(&AR,CPLaux_offset_data);
      addpos3d(AS.word_data.CPLauxdata.CPLoffsetdata,
	       AT.word_data.CPLauxdata.CPLoffsetdata,
	       AR.word_data.CPLauxdata.CPLoffsetdata);
    }
    else if ((AS.aux_kind == CPLaux_int_data) &&
	     (AT.aux_kind == CPLaux_int_data))
    {
      setup_CPL_auxword(&AR,CPLaux_int_data);
      AR.word_data.CPLauxdata.CPLintdata = 
	AS.word_data.CPLauxdata.CPLintdata +
	  AT.word_data.CPLauxdata.CPLintdata;
    }
    else if ((AS.aux_kind == CPLaux_float_data) &&
	     (AT.aux_kind == CPLaux_float_data))
    {
      setup_CPL_auxword(&AR,CPLaux_float_data);
      AR.word_data.CPLauxdata.CPLfloatdata = 
	AS.word_data.CPLauxdata.CPLfloatdata +
	  AT.word_data.CPLauxdata.CPLfloatdata;
    }
    else if ((AS.aux_kind == CPLaux_address_data) &&
	     (AT.aux_kind == CPLaux_address_data))
    {
      setup_CPL_auxword(&AR,CPLaux_address_data);
      AR.word_data.CPLauxdata.CPLaddressdata = (void *)
	((addr_sized_uint) (AS.word_data.CPLauxdata.CPLaddressdata) +
	 (addr_sized_uint) (AT.word_data.CPLauxdata.CPLaddressdata));
    }
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


/* This clears a word in CPL memory.   */
  void
execute_CLEAR(listptr world_list,stateptr state)
{
  clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_PUSH(listptr world_list,stateptr state)
{
  push_on_CPL_stack(CPL_stack,&AS);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_POP(listptr world_list,stateptr state)
{
  pop_off_CPL_stack(CPL_stack,&AR,FALSE); /* assume RAM initialized */

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_WIPE(listptr world_list,stateptr state)
{
  AS.word_data.CPLptr->wiped = TRUE;

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_UNWIPE(listptr world_list,stateptr state)
{
  AS.word_data.CPLptr->wiped = FALSE;

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


  /********************************************************************/
  /*                                                                  */
  /* AND follows these rules:                                         */
  /*                                                                  */
  /*     AS argument         AT argument       rtn if True/False/Equal*/
  /*    ------------        -------------     ----------------------- */
  /*                                                                  */
  /*       (Note: the following lists have only ONE element each)     */
  /*                                                                  */
  /*      CPLfeaturelist    CPLfeaturelist    CPLfeaturelist/Nil      */
  /*      CPLproplist       CPLproplist       CPLproplist/Nil         */
  /*                                                                  */
  /*       (These are single items and not lists.)                    */
  /*      CPLptr            CPLptr            CPLptr/Nil              */
  /*      CPLhandle         CPLhandle         CPLhandle/Nil           */
  /*                                                                  */
  /*       (This checks for equality of auxdata.)                     */
  /*      CPLword           CPLword           CPLword/Nil             */
  /*                                                                  */
  /********************************************************************/

  void
execute_AND(listptr world_list,stateptr state)
{
  Boolean copyit;
  ;
  if ((AS.word_type == Featurelist_data) &&
      (AT.word_type == Featurelist_data))
  {			/* if the two features are identical AND is true */
			/* so return a pointer to the feature */
    if ((AS.word_data.CPLfeaturelist->first.ep->thiselem ==
	 AT.word_data.CPLfeaturelist->first.ep->thiselem) &&
	(AS.word_data.CPLfeaturelist->first.ep->thiselem != Nil))
    {
      setup_CPL_word(&AR,Featurelist_data);
      add_to_featurelist(AR.word_data.CPLfeaturelist,(featureptr)
			 AS.word_data.CPLfeaturelist->first.ep->thiselem);
    }
    else
      clear_word(&AR);		/* result is left blank if AND is false */
  }
  else if ((AS.word_type == Proplist_data) &&
	   (AT.word_type == Proplist_data))
  {
    if ((AS.word_data.CPLproplist->first.sort->sortdata1->i ==
	AT.word_data.CPLproplist->first.sort->sortdata1->i) &&
	(AS.word_data.CPLproplist->first.sort->sortdata1->i != noproperty_prop))
    {
      setup_CPL_word(&AR,Proplist_data);
      AR.word_data.CPLproplist->first.sort->sortdata1->i =
      	AS.word_data.CPLproplist->first.sort->sortdata1->i;
    }
    else
      clear_word(&AR);		/* result is left blank if AND is false */
  }
  else if ((AS.word_type == Auxiliary_data) && (AT.word_type == Auxiliary_data))
  {
    if (aux_records_equal(&AS,&AT))
      AR = AS;
    else
      clear_word(&AR);		/* result is left blank: AND is false */

  }
  else if (AS.word_type == AT.word_type)
  {
    switch (AS.word_type)
    {
    case Opcode_data:
      copyit = (AS.word_data.CPLopcode == AT.word_data.CPLopcode);
      break;
    case Message_data:
      copyit = (AS.word_data.CPLmessage == AT.word_data.CPLmessage);
      break;
    case Environment_data:
      copyit = (AS.word_data.CPLenvironment_part ==
		AT.word_data.CPLenvironment_part);
      break;
    case Icon_data:
      copyit = (AS.word_data.CPLicon == AT.word_data.CPLicon);
      break;
    case CPLptr_data:
      copyit = (AS.word_data.CPLptr == AT.word_data.CPLptr);
      break;
    case CPLhandle_data:
      copyit = (AS.word_data.CPLhandle == AT.word_data.CPLhandle);
      break;
    default:
      copyit = FALSE;
      break;
    }
    if (copyit)
      AR = AS;
    else
      clear_word(&AR);
  }
  else
    clear_word(&AR);		/* if none of above cases, clear so FALSE */

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


  void
execute_OR(listptr world_list,stateptr state)
{
  CPLauxptr aux1,aux2;
  Boolean copyit,gotone = FALSE;
  ;
  if ((AS.word_type == Featurelist_data) &&
      (AT.word_type == Featurelist_data))
  {			/* if either AS or AT or both have features,  */
			/* return either or both */
    if ((AS.word_data.CPLfeaturelist->first.ep->thiselem != Nil) ||
	(AT.word_data.CPLfeaturelist->first.ep->thiselem != Nil))
      setup_CPL_word(&AR,Featurelist_data);
    if (AS.word_data.CPLfeaturelist->first.ep->thiselem != Nil)
    {
      add_to_featurelist(AR.word_data.CPLfeaturelist, (featureptr)
			 AS.word_data.CPLfeaturelist->first.ep->thiselem);
      gotone = TRUE;
    }
    if (AT.word_data.CPLfeaturelist->first.ep->thiselem != Nil)
    {
      add_to_featurelist(AR.word_data.CPLfeaturelist, (featureptr)
			 AT.word_data.CPLfeaturelist->first.ep->thiselem);
      gotone = TRUE;
    }
    if (!gotone)
      clear_word(&AR);		/* result is left blank if both are nil */
  }
  else if ((AS.word_type == Proplist_data) &&
	   (AT.word_type == Proplist_data))
  {
    if ((AS.word_data.CPLproplist->first.sort->sortdata1->i !=
	 noproperty_prop) ||
	(AT.word_data.CPLproplist->first.sort->sortdata1->i !=
	 noproperty_prop))
    {
      setup_CPL_word(&AR,Proplist_data);
      if (AS.word_data.CPLproplist->first.sort->sortdata1->i != noproperty_prop)
      {
	AR.word_data.CPLproplist->first.sort->sortdata1->i =
	  AS.word_data.CPLproplist->first.sort->sortdata1->i;
      }
      if (AT.word_data.CPLproplist->first.sort->sortdata1->i !=
	  noproperty_prop)
      {
	AR.word_data.CPLproplist->first.sort->sortdata1->i =
	  AT.word_data.CPLproplist->first.sort->sortdata1->i;
      }
    }
    else
      clear_word(&AR);		/* result is left blank if both are noproperty */
  }
  else if ((AS.word_type == Auxiliary_data) &&
	   (AT.word_type == Auxiliary_data))
  {
    /* Needs coding of some kind: binary OR??? */
    AR = AS;
  }
  else if (AS.word_type == AT.word_type)
  {
    /* this is sort of meaningless: OR of two simple operands, what should */
    /* result be???? */
    switch (AS.word_type)
    {
    case Opcode_data:
      copyit = (AS.word_data.CPLopcode != Blank_word) ||
	(AT.word_data.CPLopcode != Blank_word);
      break;
    case Message_data:
      copyit = (AS.word_data.CPLmessage != -1) ||
	(AT.word_data.CPLmessage != -1);
      break;
    case Environment_data:	/* sartre would love this */
      copyit = (AS.word_data.CPLenvironment_part != Nothingness) ||
		(AT.word_data.CPLenvironment_part != Nothingness);
      break;
    case Icon_data:
      copyit = (AS.word_data.CPLicon != Noicon) &&
	(AT.word_data.CPLicon != Noicon);
      break;
    case CPLptr_data:
      copyit = (AS.word_data.CPLptr != Nil) ||
	(AT.word_data.CPLptr != Nil);
      break;
    case CPLhandle_data:
      copyit = (AS.word_data.CPLhandle != Nil) ||
	(AT.word_data.CPLhandle != Nil);
      break;
    default:
      copyit = FALSE;
      break;
    }
    if (copyit)
      AR = AS;
    else
      clear_word(&AR);
  }
  else
    clear_word(&AR);		/* if none of above cases, clear so FALSE */

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_NOT(listptr world_list,stateptr state)
{
  Boolean copyit;
  ;
  if ((AS.word_type == Featurelist_data) &&
      (AT.word_type == Featurelist_data))
  {			/* if the two features are not identical NOT is true */
			/* so return a pointer to the source feature */
    if (((AS.word_data.CPLfeaturelist->first.ep->thiselem !=
	  AT.word_data.CPLfeaturelist->first.ep->thiselem)) &&
	(AS.word_data.CPLfeaturelist->first.ep->thiselem != Nil) )
    {
      setup_CPL_word(&AR,Featurelist_data);
      add_to_featurelist(AR.word_data.CPLfeaturelist, (featureptr)
			 AS.word_data.CPLfeaturelist->first.ep->thiselem);
    }
    else
      clear_word(&AR);		/* result is left blank if AND is false */
  }
  else if (((AS.word_type == Proplist_data) &&
	    (AT.word_type == Proplist_data)) &&
	   (AS.word_data.CPLproplist->first.sort->
	    sortdata1->i !=  noproperty_prop) &&
	   (AT.word_data.CPLproplist->first.sort->sortdata1->i !=
	    noproperty_prop))
    /* NB: Both must be non- noproperty_prop for this to be useful */
  {
    if ((AS.word_data.CPLproplist->first.sort->sortdata1->i !=
	AT.word_data.CPLproplist->first.sort->sortdata1->i))
    {
      setup_CPL_word(&AR,Proplist_data);
      AR.word_data.CPLproplist->first.sort->sortdata1->i =
      	AS.word_data.CPLproplist->first.sort->sortdata1->i;
    }
    else
      clear_word(&AR);		/* result is left blank if AND is false */
  }
  else if ((AS.word_type == Auxiliary_data) &&
	   (AT.word_type == Auxiliary_data))
  {
    if (!aux_records_equal(&AS,&AT))
      AR = AS;
    else
      clear_word(&AR);		/* result is left blank: AND is false */

  }
  else if (AS.word_type == AT.word_type)
  {
    switch (AS.word_type)
    {
    case Opcode_data:
      copyit = (AS.word_data.CPLopcode != AT.word_data.CPLopcode);
      break;
    case Message_data:
      copyit = (AS.word_data.CPLmessage != AT.word_data.CPLmessage);
      break;
    case Environment_data:
      copyit = (AS.word_data.CPLenvironment_part !=
		AT.word_data.CPLenvironment_part);
      break;
    case Icon_data:
      copyit = (AS.word_data.CPLicon != AT.word_data.CPLicon);
      break;
    case CPLptr_data:
      copyit = (AS.word_data.CPLptr != AT.word_data.CPLptr);
      break;
    case CPLhandle_data:
      copyit = (AS.word_data.CPLhandle != AT.word_data.CPLhandle);
      break;
    default:
      copyit = FALSE;
      break;
    }
    if (copyit)
      AR = AS;
    else
      clear_word(&AR);
  }
  else
    clear_word(&AR);		/* if none of above cases, clear so FALSE */


#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif

}

  void
execute_COUNT(listptr world_list,stateptr state)
{
  int given_numelems;
  ;
  if (AS.word_type == Featurelist_data)
  {
    setup_CPL_auxword(&AR,CPLaux_int_data);
    if (AS.word_data.CPLfeaturelist->first.ep->thiselem == Nil)
      given_numelems = 0;
    else
      given_numelems = AS.word_data.CPLfeaturelist->numelems;
    AR.word_data.CPLauxdata.CPLintdata = given_numelems;
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_INDEX(listptr world_list,stateptr state)
{
  listptr the_list;
  int requested_elem;
  elemptrptr requested_ep;
  featureptr thefeature;
  ;
  if ((AS.word_type == Featurelist_data) && (AD.word_type == Auxiliary_data))
  {
    if (AS.word_data.CPLfeaturelist->first.ep->thiselem == Nil)
      clear_word(&AR);
    else
    {
      the_list = AS.word_data.CPLfeaturelist;
      requested_elem = AD.word_data.CPLauxdata.CPLintdata;
      setup_CPL_word(&AR,Featurelist_data);
      requested_ep = (elemptrptr) get_elem_ptr(requested_elem,the_list);
      thefeature = (featureptr) requested_ep->thiselem;
      add_to_featurelist(AR.word_data.CPLfeaturelist,thefeature);
    }
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


/* This makes the assumption that AS holds one feature and AT holds one */
/* prop only, and that we want to know whether that feature has that prop. */

  void
execute_HASPROP(listptr world_list,stateptr state)
{
  property thisproperty;
  featureptr thefeature;
  objptr motherobj;
  ;
  if ((AS.word_type == Featurelist_data) && (AT.word_type == Proplist_data))
  {
    /* This may be wasteful, maybe we should have just a boolean type */
    /* put in AR rather than a copy of the featureptr (a call to copy_list */
    /* is made). */
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      thisproperty = (property) AT.word_data.CPLproplist->first.sort->
	sortdata1->i;
      if (AD.word_data.CPLauxdata.CPLintdata == Subfeatures)
      {
	/* if data is true, do property search on all features that belong to */
	/* this feature's motherobj */
	motherobj = (objptr) find_largergrain(thefeature,Obj_type);
	setup_CPL_word(&AR,Featurelist_data);
	find_subfeatures(motherobj,thisproperty,
			 AR.word_data.CPLfeaturelist);
	if (AR.word_data.CPLfeaturelist->numelems == 0)
	  clear_word(&AR);
      }
      else if (has_property(thefeature,thisproperty))
      {
	copy_wordA_to_wordB(&AS,&AR);
      }
      else
	clear_word(&AR);
    }
    else
      clear_word(&AR);
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


/* This makes the assumption that AS holds one feature and AT holds one */
/* prop only, and that we want to know whether that feature has that prop. */

  void
execute_GETPROP(listptr world_list,stateptr state)
{
  property thisproperty;
  featureptr thefeature;
  objptr motherobj;
  long propdata;
  ;
  if ((AS.word_type == Featurelist_data) && (AT.word_type == Proplist_data))
  {
    /* This may be wasteful, maybe we should have just a boolean type */
    /* put in AR rather than a copy of the featureptr (a call to copy_list */
    /* is made). */
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      thisproperty = (property) AT.word_data.CPLproplist->first.sort->
	sortdata1->i;
      if (has_property(thefeature,thisproperty))
      {
	if ((propdata = get_property_val((featureptr) AR.word_data.CPLfeaturelist->
				     first.ep->thiselem, thisproperty).l) != 0)
	{
	  setup_CPL_word(&AR,Auxiliary_data);
	  AR.word_data.CPLauxdata.CPLlongdata = propdata;
	  return;
	}
      }
    }
  }
  clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


/* All jump does is change the next frame pointer to the address found in */
/* AD.  Then when we're done with CPL_resolver and advance the frame we'll */
/* go to this new address. */

  void
execute_JUMP(listptr world_list,stateptr state)
{
  if (AD.word_type == CPLptr_data)
    CNF->word_data.CPLptr = AD.word_data.CPLptr;

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


/* If two words are (partially) equal according to an execute_AND, then BEQUAL */
/* will branch to the address given in AD.  This is aimed at comparison of */
/* simple types, not sets, because AND of two sets will give their */
/* intersection and BEQUAL will branch on any non-empty set, for which the */
/* intersection may qualify even if the two sets are not EXACTLY equal. */

  void
execute_BEQUAL(listptr world_list,stateptr state)
{
  if (AD.word_type == CPLptr_data)
  {
    execute_AND(world_list, state);

    if (AR.word_type == Featurelist_data)
    {
      if (AR.word_data.CPLfeaturelist->first.ep->thiselem != Nil)
	execute_JUMP(world_list, state);
    }
    else if (AR.word_type != Blank_word)
      execute_JUMP(world_list, state);
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* If two words are TOTALLY unequal according to an execute_AND, then BEQUAL */
/* will branch to the address given in AD.  This is aimed at comparison of */
/* simple types, not sets. */

  void
execute_BNEQUAL(listptr world_list,stateptr state)
{
  if (AD.word_type == CPLptr_data)
  {
    execute_AND(world_list, state);

    if (AR.word_type == Blank_word)
      execute_JUMP(world_list, state);
    else if (AR.word_type == Featurelist_data)
    {
      if (AR.word_data.CPLfeaturelist->first.ep->thiselem == Nil)
	execute_JUMP(world_list, state);
    }
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Branch if AS has a set. */

  void
execute_BNEMPTY(listptr world_list,stateptr state)
{
  if (AD.word_type == CPLptr_data)
    if (AS.word_type == Featurelist_data)
      if (AS.word_data.CPLfeaturelist->first.ep->thiselem != Nil)
	execute_JUMP(world_list, state);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Branch if AS has the empty set.  */

  void
execute_BEMPTY(listptr world_list,stateptr state)
{
  if (AD.word_type == CPLptr_data)
  {
    if (AS.word_type == Blank_word)
      execute_JUMP(world_list, state);
    else if (AS.word_type == Featurelist_data)
    {
      if (AS.word_data.CPLfeaturelist->numelems == 0)
	execute_JUMP(world_list, state);
      else if (AS.word_data.CPLfeaturelist->first.ep->thiselem == Nil)
	execute_JUMP(world_list, state);
    }
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_BTRUE(listptr world_list,stateptr state)
{
  if (AD.word_type == CPLptr_data)
  {
    if (AS.word_type == Featurelist_data)
    {
      if (AS.word_data.CPLfeaturelist->first.ep->thiselem != Nil)
	execute_JUMP(world_list, state);
    }
    else if (AS.word_type == Auxiliary_data)
    {
      if (AS.word_data.CPLauxdata.CPLbooleandata == TRUE)
	execute_JUMP(world_list, state);
    }
    else if (AS.word_type != Blank_word)
      execute_JUMP(world_list, state);
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_BFALSE(listptr world_list,stateptr state)
{
  if (AD.word_type == CPLptr_data)
  {
    if (AS.word_type == Blank_word)
      execute_JUMP(world_list, state);
    else if (AS.word_type == Featurelist_data)
    {
      if (AS.word_data.CPLfeaturelist->first.ep->thiselem == Nil)
	execute_JUMP(world_list, state);
    }
    else if (AS.word_type == Auxiliary_data)
    {
      if (AS.word_data.CPLauxdata.CPLbooleandata == FALSE)
	execute_JUMP(world_list, state);
    }      
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}
  
  void
execute_BWIPED(listptr world_list,stateptr state)
{
  if (AS.wiped == TRUE)
    execute_JUMP(world_list,state);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_OWNER(listptr world_list,stateptr state)
{
  int larger_type;
  featureptr thefeature,ownerfeature;
  ;
  if (AD.word_type == Auxiliary_data)
  {
    if (current_owner != Nil)
    {
      thefeature = current_owner;
      larger_type = AD.word_data.CPLauxdata.CPLintdata;
      if (larger_type < 0)	/* use same feature: Const:Self */
	ownerfeature = thefeature;
      else
	ownerfeature = find_largergrain(thefeature,larger_type);
      setup_CPL_word(&AR,Featurelist_data);
      add_to_featurelist(AR.word_data.CPLfeaturelist, (elemptr) ownerfeature);
    }
    else
      clear_word(&AR);
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_SETOWN(listptr world_list,stateptr state)
{
  if (AD.word_data.CPLauxdata.owner_feature)
    current_owner = AD.word_data.CPLauxdata.owner_feature;

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* This is exactly the same as execute_OWNER, above, only this works on */
/* the passed feature in AS. Actually, execute_OWNER could be simplified to */
/* only return the current_owner and force CPL users to call execute_MOTHER */
/* to get a larger containing feature... but that's slower. */

  void
execute_MOTHER(listptr world_list,stateptr state)
{
  int larger_type;
  featureptr thefeature,ownerfeature;
  ;
  if ((AS.word_type == Featurelist_data) && (AD.word_type == Auxiliary_data))
  {
    thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      larger_type = AD.word_data.CPLauxdata.CPLintdata;
      if (larger_type < 0)	/* use same feature: Const:Self */
	ownerfeature = thefeature;
      else
	ownerfeature = find_largergrain(thefeature,larger_type);
      setup_CPL_word(&AR,Featurelist_data);
      add_to_featurelist(AR.word_data.CPLfeaturelist, (elemptr) ownerfeature);
    }
    else
      clear_word(&AR);
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Get group unlinked ancestor of a given object, which may be the object */
/* itself if it's at the top of its world tree.*/

  void
execute_ANCEST(listptr world_list,stateptr state)
{
  featureptr thefeature;
  objptr motherobj,grouproot;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      setup_CPL_word(&AR,Featurelist_data);
      motherobj = (objptr) find_largergrain(thefeature,Obj_type);
      grouproot = get_unlinked_ancestor(motherobj);
      add_to_featurelist(AR.word_data.CPLfeaturelist,(featureptr) grouproot);
    }
    else
      clear_word(&AR);
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Get the parent object of a given feature (the parent of the motherobj */
/* if this feature is not a object, or just the parent if this feature is */
/* a object. Returns the motherworld if the parent is the root. */

  void
execute_PARENT(listptr world_list,stateptr state)
{
  featureptr thefeature;
  objptr featureparent;
  objptr motherobj;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      motherobj = (objptr) find_largergrain(thefeature,Obj_type);
      featureparent = motherobj->parent;
      if (featureparent == motherobj->motherworld->root)
      {
	setup_CPL_auxword(&AR,CPLaux_new_world_data);
	AR.word_data.CPLauxdata.new_world = motherobj->motherworld;
      }
      else
      {
	setup_CPL_word(&AR,Featurelist_data);
	add_to_featurelist(AR.word_data.CPLfeaturelist,
			   (featureptr) featureparent);
      }
    }
    else
    {
      /*      printf("execute_PARENT:warning, object not in an object tree!");*/
      clear_word(&AR);
    }
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Get the children objects of a given feature's motherobj */
/* or the children of the feature if the feature is a object. */

  void
execute_CHLDRN(listptr world_list,stateptr state)
{
  featureptr thefeature;
  objptr motherobj,thischild;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      setup_CPL_word(&AR,Featurelist_data);
      motherobj = (objptr) find_largergrain(thefeature,Obj_type);
      thischild = motherobj->children->first.obj;
      if (thischild == Nil)
	add_to_featurelist(AR.word_data.CPLfeaturelist,Nil);
      else
	while (thischild != Nil)
	{
	  add_to_featurelist(AR.word_data.CPLfeaturelist,(elemptr) thischild);
	  thischild = thischild->next;
	}
    }
    else
      clear_word(&AR);
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Recursive routine that walks down the object tree from the given */
/* feature's object, getting subfeatures as it goes. The recursion */
/* is postorder (root last). */

  void
get_descendent_features(objptr this_obj,objptr mother_obj,
			listptr cull_list)
{
  objptr recurse_obj;
  ;
  /* First, recurse on the children of this object.  */
  recurse_obj = this_obj->children->first.obj;
  if (recurse_obj != Nil)	/* don't descend if at a leaf */
    do
    {
      get_descendent_features(recurse_obj,mother_obj,cull_list);
      recurse_obj = recurse_obj->next;
    } while (recurse_obj != Nil);

  if (this_obj == mother_obj)
    return;			/* don't store mother object too! */
  /* no actually, just get objects! */
  add_to_featurelist(cull_list,(elemptr) this_obj);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Get all descendents of a feature, not including the feature itself, and */
/* them in a Featurelist in AR. */

  void
execute_DESCEND(listptr world_list,stateptr state)
{
  featureptr thefeature;
  objptr motherobj;
  listptr cull_list;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      setup_CPL_word(&AR,Featurelist_data);
      motherobj = (objptr) find_largergrain(thefeature,Obj_type);
      cull_list = AR.word_data.CPLfeaturelist;
      get_descendent_features(motherobj,motherobj,cull_list);
      if (cull_list->numelems == 0)
	add_to_featurelist(cull_list,Nil); /* for BEQ */
    }
    else
      clear_word(&AR);
  }
  else if (AS.word_type == Auxiliary_data)
  {
    if (AS.word_data.CPLauxdata.new_world != Nil)
    {
      setup_CPL_word(&AR,Featurelist_data);
      motherobj = AS.word_data.CPLauxdata.new_world->root;
      cull_list = AR.word_data.CPLfeaturelist;
      get_descendent_features(motherobj,motherobj,cull_list);
      if (cull_list->numelems == 0)
	add_to_featurelist(cull_list,Nil); /* for BEQ */
    }	
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Get all siblings of the features found in AS-- excluding the features */
/* themselves-- into a featurelist passed back in AR. */

  void
execute_SIBLING(listptr world_list,stateptr state)
{
  featureptr thefeature,motherfeature;
  objptr mother_obj,this_obj;
  listptr cull_list;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      setup_CPL_word(&AR,Featurelist_data);
      cull_list = AR.word_data.CPLfeaturelist;
      mother_obj = (objptr) find_largergrain(thefeature,Obj_type);
      if (mother_obj->parent != Nil) /* if not at root, proceed */
      {
	this_obj = mother_obj->parent->children->first.obj;
	while (this_obj != Nil)
	{
	  if (this_obj != mother_obj)
	    add_to_featurelist(cull_list, (featureptr) this_obj);
	  this_obj = this_obj->next;
	}
	if (cull_list->numelems == 0)
	  add_to_featurelist(cull_list,Nil); /* for BEQ */
	return;
      }
    }
  }
  clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


/* Make a new object with featuretype as passed in AD const. */
/* Parent in AT. If AD const is Motherobj, make an empty object. */
/* If AT has a world const, then */
/* make this object a child of that world's root. */

  void
execute_MAKE(listptr world_list,stateptr state)
{
  featureptr parentfeature;
  objptr parent,newobj;
  worldptr neworld;
  int make_type;
  ;
  clear_word(&AR);
  if (AD.word_type != Auxiliary_data)
    return;
  else
    make_type = AD.word_data.CPLauxdata.CPLintdata;

  if (AT.word_type == Auxiliary_data)
  {
    neworld = AT.word_data.CPLauxdata.new_world;
    parent = neworld->root;
  }
  else if (AT.word_type == Featurelist_data)
  {
    parentfeature = AT.word_data.CPLfeaturelist->first.ep->thiselem;
    if (parentfeature == Nil)
      return;
    parent = (objptr) find_largergrain(parentfeature,Obj_type);
  }
  else
    return;

  switch (make_type)
  {
    /* these need to actually create the requested subfeatures someday */
  case Vfe_type:
  case Evf_type:
  case Fve_type:
  case Shell_type:
  case Obj_type:
    setup_CPL_word(&AR,Featurelist_data);
    /*  noshell'd be a lot better but lotsa shit dies */
    newobj = make_childobj(parent,origin);
    /* no updates until it's all done! */
    del_property((featureptr) newobj,visible_prop);
    add_to_featurelist(AR.word_data.CPLfeaturelist,(featureptr) newobj);
    break;
  default:
    break;
  }    

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Branch to the address given in AD if the given object has messages in */
/* its message queue. */

  void
execute_BMSGS(listptr world_list,stateptr state)
{
  featureptr thefeature;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
      if (has_property(thefeature,msgpending_prop))
	execute_JUMP(world_list,state);
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Branch to the address given in AD if the given object has NO messages in */
/* its message queue. */

  void
execute_BNOMSGS(listptr world_list,stateptr state)
{
  featureptr thefeature;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
      if (!has_property(thefeature,msgpending_prop))
	execute_JUMP(world_list,state);
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


/* this puts the word passed in AS into the temporary message block. */
/* The pointer to the end of that block is incremented.  */

  void
execute_MAKEMSG(listptr world_list,stateptr state)
{
  enter_in_temp_message_block(&AS); /* not too complex here... */

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


/* Queue a message string message queue of object(s) in AS.  These messages */
/* are NOT processed until the object(s)' code(s) are run. Any feature may */
/* queue messages into any other feature's message queue! */

  void
execute_QMSG(listptr world_list,stateptr state)
{
  featureptr thefeature;
  Boolean priority;
  ;
  if (((AS.word_type == Featurelist_data) || (AS.word_type == Blank_word)) &&
      (AD.word_type == Auxiliary_data))
  {
    if (AS.word_type == Blank_word)
      thefeature = Nil;
    else
      thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    priority = AD.word_data.CPLauxdata.CPLintdata;
    /* thefeature *can* be Nil because it handles it and sets the */
    /* the message block to reset itself by the next refresh. */
    queue_feature_message(thefeature,priority);
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Unqueue one word into AR from the message queue(s) of the current feature. */
/* If the queue is empty, no words get popped. Features should avoid this */
/* situation by first testing for pending messages with BMSGS/BNOMSGS. */

  void
execute_UNQMSG(listptr world_list,stateptr state)
{
  featureptr thefeature;
  ;      
  thefeature = current_owner;
  if (thefeature != Nil)
  {
    /* mark this feature with a property so that its code gets called. */
    if (has_property(thefeature,msgpending_prop))
    {
      remove_from_CPL_queue(thefeature->messagequeue,&AR,TRUE);
      /* if no more messages for this feature, remove msgpending property */
      if (CPL_queue_empty(thefeature->messagequeue))
	del_property(thefeature,msgpending_prop);
    }
    else
      clear_word(&AR);
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Copy one word into AR from the top of the message queue(s) of the current */
/* feature.  If the queue is empty, no words get copied. The message queue is */
/* left unchanged. */

extern char message_names[][MaxCPLmessagelength];

  void
execute_CHKMSG(listptr world_list,stateptr state)
{
  featureptr thefeature;
  ;      
  thefeature = current_owner;
  if (thefeature != Nil)
  {
    /* mark this feature with a property so that its code gets called. */
    if (has_property(thefeature,msgpending_prop))
    {
      copy_from_CPL_queue(thefeature->messagequeue,&AR,TRUE);
      if (debugcplspool)
	fprintf (cplwatchfile,"Message checked for object %s was %s\n",
		 ((objptr) thefeature)->obj_name,
		 message_names[AR.word_data.CPLmessage]);
    }
    else
      clear_word(&AR);
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


  void
execute_JSR(listptr world_list,stateptr state)
{
  CPL_word return_address;
  ;
  return_address.word_type = CPLptr_data;
  return_address.word_data.CPLptr = CNF->word_data.CPLptr;
  push_on_CPL_controlstack(&return_address);
  /* You can just do execute_JUMP because AD has pointer to jsr address! */
  push_owner_stack();
  execute_JUMP(world_list,state);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* encode one SETOWN/JSR pair in the JSRAT_codebloc. */

  static void
encode_one_JSR_pair(featureptr thefeature,CPL_word_ptr jsr_address)
{
  /* Put a JSR to the objects private/property code in the JSRAT codebloc */
  next_free_JSRAT_word->word_type = Opcode_data;
  next_free_JSRAT_word->word_data.CPLopcode = SETOWN;
  next_free_JSRAT_word = inc_CPL_blocptr(next_free_JSRAT_word,
					 JSRAT_codebloc, JSRATcodeblocsize);
  setup_CPL_auxword(next_free_JSRAT_word,CPLaux_owner_feature_data);
  next_free_JSRAT_word->word_data.CPLauxdata.owner_feature = thefeature;
  next_free_JSRAT_word = inc_CPL_blocptr(next_free_JSRAT_word,
					 JSRAT_codebloc, JSRATcodeblocsize);
  next_free_JSRAT_word->word_type = Opcode_data;
  next_free_JSRAT_word->word_data.CPLopcode = JSR;
  next_free_JSRAT_word = inc_CPL_blocptr(next_free_JSRAT_word,
					 JSRAT_codebloc, JSRATcodeblocsize);
  next_free_JSRAT_word->word_type = CPLptr_data;
  next_free_JSRAT_word->word_data.CPLptr = jsr_address;
  next_free_JSRAT_word->address_of = TRUE;
  next_free_JSRAT_word = inc_CPL_blocptr(next_free_JSRAT_word,
					 JSRAT_codebloc, JSRATcodeblocsize);
}

  void
execute_JSRAT(listptr world_list,stateptr state)
{
  CPL_word return_address;
  codekind whichcodekind;
  featureptr thefeature;
  static CPL_word_ptr start_of_jsrlist;
  static Boolean coded_one_JSR = FALSE;
  Boolean coded_one_prop = FALSE;
  proptr thisprop;
  ;
  if (start_JSRAT)
  {
    start_of_jsrlist = next_free_JSRAT_word;
    start_JSRAT = FALSE;
  }
  /* Can't do JSRAT if AS does not hold a feature whose code we jsr to! */
  /* Also can't do JSRAT if AD does not contain a Privatecode or Propertycode */
  /* constant. */
  if ((AS.word_type != Featurelist_data) || (AD.word_type != Auxiliary_data))
    return;
  thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
  /* If the set is empty, don't try to JSRAT! */
  if (thefeature == Nil)
    return;
  /* if time to finish up, do it */
  if (coded_one_JSR && finish_JSRAT)
  {
    next_free_JSRAT_word->word_type = Opcode_data;
    next_free_JSRAT_word->word_data.CPLopcode = RTS;
    inc_CPL_blocptr(next_free_JSRAT_word,
		    JSRAT_codebloc, JSRATcodeblocsize);
    /* reset next free JSRAT_word */
    next_free_JSRAT_word = &(JSRAT_codebloc[0]);

    /* Now everything is setup as if we were doing a regular JSR */
    /* You can do JSR because AD has pointer to jsr address! */
    coded_one_JSR = FALSE;
    setup_CPL_word(&AD,CPLptr_data);
    AD.word_data.CPLptr = start_of_jsrlist;
    execute_JSR(world_list,state);
    return;			/* do NOT code any more JSR's!  */
  }

  whichcodekind = AD.word_data.CPLauxdata.codekind_value;
  if (whichcodekind == Private_code)
  {
    if  (thefeature->privatecode != Nil)
    {
      coded_one_JSR = TRUE;
      encode_one_JSR_pair(thefeature,thefeature->privatecode);
    }
  }
  else if (whichcodekind == Property_code)
  {
    thisprop = thefeature->proplist->first.prop;
    while (thisprop != Nil)
    {				/* encode JSRs to all props of this feature */
      if (propinfo[thisprop->propkind].propertycode != Nil)
      {
	encode_one_JSR_pair(thefeature,
			    propinfo[thisprop->propkind].propertycode);
	coded_one_JSR = TRUE;
	coded_one_prop = TRUE;
      }
      thisprop = thisprop->next;
    }
    /* also call cpnormal on all features that didn't call one single propcode */
    if ((!coded_one_prop) || (has_property(thefeature,forcedcpnormal_prop)))
    {
      coded_one_JSR = TRUE;
      encode_one_JSR_pair(thefeature,cpnormal_address);
    }
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_RTS(listptr world_list,stateptr state)
{
  pop_off_CPL_controlstack(&AD);
  pop_owner_stack();
  execute_JUMP(world_list, state);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


  void
execute_SETFR(listptr world_list,stateptr state)
{
  CPL_word_ptr set_OP,set_AS,set_AT,set_AR,set_AD,set_NF;
  ;
  /* Given that the beginning of the frame to set is */
  /* pointed to by AS, we assume the rest of the frame pointers follow */
  /* and we set the frame as normally */
  set_OP = AS.word_data.CPLptr;
  set_AS = set_OP+1;
  set_AT = set_AS+1;
  set_AR = set_AT+1;
  set_AD = set_AR+1;
  set_NF = set_AD+1;  
  CPL_set_frame(set_OP,set_AS,set_AT,set_AR,set_AD,set_NF);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_ENDFR(listptr world_list,stateptr state)
{
  /* Does nothing-- never called. */

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_CRTN(listptr world_list,stateptr state)
{
  int routine_index;
  ;
  routine_index = AD.word_data.CPLauxdata.Croutine_index;
  /* assume C routine can get what it wants from various globals */
  (Croutines[routine_index].croutineptr)(world_list,state);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_TRANS(listptr world_list,stateptr state)
{
  vertype actualoffset,cursor_trans_amt,cutplane_trans_amt;
  featureptr thefeature;
  objptr thisobj;
  Coord *transoffset;
  Boolean plane_moved;
  ;
  if (AS.word_type == Environment_data)
  {
    switch (AS.word_data.CPLenvironment_part)
    {
    case Crosshair_part:
      compute_crosshair_move(AD.word_data.CPLauxdata.CPLoffsetdata,
			     actualoffset, state);
      fprintf(dbgdump, "execute_TRANS: [%f,%f,%f],[%f,%f,%f]\n",
	      AD.word_data.CPLauxdata.CPLoffsetdata[vx],
	      AD.word_data.CPLauxdata.CPLoffsetdata[vy],
	      AD.word_data.CPLauxdata.CPLoffsetdata[vz],
	      actualoffset[vx], actualoffset[vy], actualoffset[vz]);
      fflush(dbgdump);
      /* make sure working_world is object world!  */
      if (plane_moved = (actualoffset[vz] != 0.0))
      {
 	snap_plane_to_nearestpt(working_world,state,actualoffset);
	log_cutplane_erase_bbox(state,Loginoldbuffer);
      }
      else
	log_crosshair_erase_bboxes(state,Loginoldbuffer);
      /* translate environment's cursor object as well as updating state vars*/
      setpos3d(cursor_trans_amt,actualoffset[vx],actualoffset[vy],0.0);
      translate_feature((featureptr) cursor_obj,cursor_trans_amt,FALSE);

      /* Update the state var */
      addpos3d(state->cursor,actualoffset,state->cursor);
      /* Log any global updates required if the plane moved. */
      /* Translate environment's cutplane object if zmove. */
      if (plane_moved && update_on)
      {
	setpos3d(cutplane_trans_amt,0.0,0.0,actualoffset[vz]);
	translate_feature((featureptr) cutplane_obj,cutplane_trans_amt,FALSE);

        log_global_update(Cutevfsflag | Makecutsectionflag);
	 log_global_update(Redrawflag); /* should be done for intr objs */
      }
      else
      {
	log_crosshair_erase_bboxes(state,Loginthisbuffer);
	/* Log this flag so that children will get erased */
      }

      break;
    default:
      break;
    }
  }
  else if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->
      thiselem;
    if (thefeature == Nil)
      return;
    transoffset = AD.word_data.CPLauxdata.CPLoffsetdata;
    /* HACK to allow debugging of triangularize */
    if (!vec_eq_zero(transoffset))
    {
      if (thefeature->type_id != Obj_type)
      {
	planarize_affected_fves(thefeature,transoffset);
	translate_feature(thefeature,transoffset,Usetranslateflags);
      }
      else
      {
	thisobj = (objptr) thefeature;
	if (AT.word_type == Auxiliary_data)
	{
	  if (AT.word_data.CPLauxdata.CPLbooleandata == TRUE)
#ifdef ready
	    translate_obj_quick(thisobj,transoffset);
#endif
	    translate_feature(thefeature,transoffset,Usetranslateflags);
	  else
	    translate_feature(thefeature,transoffset,Usetranslateflags);
	}
	else
	  translate_feature(thefeature,transoffset,Usetranslateflags);
      }
    }
  }
  else if (AS.word_type == Auxiliary_data)
    if (AS.aux_kind == CPLaux_new_world_data)
    {
      transoffset = AD.word_data.CPLauxdata.CPLoffsetdata;
      if (!vec_eq_zero(transoffset))
      {
	thisobj = AS.word_data.CPLauxdata.new_world->root;
	translate_feature((featureptr) thisobj, transoffset, Usetranslateflags);
      }
    }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_ROTATE(listptr world_list,stateptr state)
{
  featureptr thefeature;
  objptr motherobj;
  ;
  if (AS.word_type == Environment_data)
  {
    switch (AS.word_data.CPLenvironment_part)
    {
    case Room_part:
      rotate_room_view((int) AD.word_data.CPLauxdata.rotate_data.rotvec[vx],
		       (int) AD.word_data.CPLauxdata.rotate_data.rotvec[vy],
		       state);
      log_global_update(Recomputebackfaceflag | Redrawflag);
      break;
    default:
      break;
    }
  }
  else if ((AS.word_type == Featurelist_data) ||
	   (AS.word_type == Auxiliary_data))
  {
    if (AS.word_type == Featurelist_data)
    {
      thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
      if (thefeature == Nil)
	return;
      motherobj = (objptr) find_largergrain(thefeature, Obj_type);
    }
    else if (AS.aux_kind == CPLaux_new_world_data)      
      motherobj = AS.word_data.CPLauxdata.new_world->root;
    else
      return;
    axis_rot_obj(motherobj,AD.word_data.CPLauxdata.rotate_data.rotvec,
		 AD.word_data.CPLauxdata.rotate_data.rotpt,
		 AD.word_data.CPLauxdata.rotate_data.rotangle);
    if (update_on)
      log_update((featureptr) motherobj, Transformflag | Evfeqnsflag |
		 Cutevfsflag | Clearcutsectionflag | Makecutsectionflag | 
		 Computebboxflag | Recomputebackfaceflag | Logerasebboxflag,
		 Immediateupdate);
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_SCALE(listptr world_list,stateptr state)
{
  featureptr thefeature,motherfeature;
  double newscale;
  vertype newscalevert,newscaleorig;
  objptr motherobj;
  ;
  if (AS.word_type != Featurelist_data)
    return;
  thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
  if (thefeature == Nil)
    return;
  newscale = AD.word_data.CPLauxdata.scale_data.scale_factor;
  motherobj = (objptr) find_largergrain(thefeature,Obj_type);
  set_3d_objbbox(motherobj);
  setpos3d(newscalevert,newscale,newscale,newscale);
  midpoint(motherobj->objbbox.mincorner,motherobj->objbbox.maxcorner,
	   newscaleorig);
  motherfeature = (featureptr) motherobj;
/* new scaling:
  transformpt(newscaleorig,motherobj->invxform,newscaleorig);
*/
  scale_feature(motherfeature,newscalevert,newscaleorig);
  if (update_on)
    log_update(motherfeature,Evfeqnsflag | Cutevfsflag | Makecutsectionflag |
	       Recomputebackfaceflag | Logerasebboxflag,
	       Nonimmediateupdate);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_TRNSFRM(listptr world_list,stateptr state)
{
  Coord *transform_pos,*new_transform_pos;
  ;
  if (AS.word_type == Featurelist_data)
  {
    /* transform features according to matrix in AD. */
  }
  else if (AS.word_type == Auxiliary_data)
  {
    /* transform a point stored in AS. How do we do multmatrix? need */
    /* CONCAT operator.*/
    transform_pos = AS.word_data.CPLauxdata.CPLoffsetdata;
    setup_CPL_auxword(&AR,CPLaux_offset_data);
    new_transform_pos = AR.word_data.CPLauxdata.CPLoffsetdata;
    transformpt(transform_pos,AD.word_data.CPLauxdata.CPLmatrixdata,
		       new_transform_pos);
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_CONCAT(listptr world_list,stateptr state)
{
  if ((AS.word_type == Auxiliary_data) && (AT.word_type == Auxiliary_data))
  {
    setup_CPL_auxword(&AR,CPLaux_matrix_data);
    glPushMatrix();
    glLoadMatrixf((GLfloat *) AS.word_data.CPLauxdata.CPLmatrixdata);
    glMultMatrixf((GLfloat *) AT.word_data.CPLauxdata.CPLmatrixdata);
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) AR.word_data.CPLauxdata.CPLmatrixdata);
    glPopMatrix();
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_DIFF3D(listptr world_list,stateptr state)
{
  Coord *pt1,*pt2;
  featureptr thefeature;
  ;
  clear_word(&AR);
  if (AS.word_type == Environment_data)
  {
    if ((AS.word_data.CPLenvironment_part == Crosshair_part) ||
	(AS.word_data.CPLenvironment_part == Cutplane_part))
      pt1 = state->cursor;
    else
      return;
  }
  else if (AS.word_type == Featurelist_data)
  {
    thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature == Nil)
      return;
    pt1 = get_first_vert(thefeature);
  }
  else if (AS.word_type == Auxiliary_data)
    pt1 = AS.word_data.CPLauxdata.CPLoffsetdata;
  else
    return;			/* AS contains invalid point */

  if (AT.word_type == Environment_data)
  {
    if (AT.word_data.CPLenvironment_part == Crosshair_part)
      pt2 = state->cursor;
    else
      return;
  }
  else if (AT.word_type == Featurelist_data)
  {
    thefeature = AT.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature == Nil)
      return;
    pt2 = get_first_vert(thefeature);
  }
  else if (AT.word_type == Auxiliary_data)
    pt2 = AT.word_data.CPLauxdata.CPLoffsetdata;
  else
    return;			/* AT contains invalid point */
  setup_CPL_auxword(&AR,CPLaux_offset_data);
  diffpos3d(pt1,pt2,AR.word_data.CPLauxdata.CPLoffsetdata);    

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Snapfeature gets adjusted to align with the snaptofeature. */
/* Currently only works with two faces... */

  void
snap_features_together(featureptr snapfeature,featureptr snaptofeature)
{
  segmentptr motherseg1,motherseg2;
  evfptr snapevf,snaptoevf;
  fveptr snapface1,snapface2;
  int rotangle;
  objptr snapobj1,snapobj2;
  vertype snapvec,snaptovec;
  vertype snaprotaxis,snapfacectr1,snapfacectr2,trans,obj2trans;
  vertype rotpt,flippedtarget;
  Coord *pt1,*pt2,*pt3,*pt4,adjust_dist;
  double dummyt1,dummyt2;
  Boolean post_snap_required;
  ;
  snapobj1 = (objptr) find_largergrain(snaptofeature,Obj_type);
  snapobj2 = (objptr) find_largergrain(snapfeature,Obj_type);
  if ((snapfeature->type_id == Fve_type) &&
      (snaptofeature->type_id == Fve_type))
  {
    snapface1 = (fveptr) snaptofeature;
    snapface2 = (fveptr) snapfeature;
    /* get from the face to the obj to the seg... then rotate the face */
    /* in AS to align with the face in AT, and also bring them together */
    /* with a translate... */
    motherseg1 = snapobj1->selectable_feature.motherseg;
    motherseg2 = snapobj2->selectable_feature.motherseg;
    /* eventually this must handle inplane faces! */
    if ((motherseg1->kind == Fve_intersect) &&
	(motherseg2->kind == Fve_intersect))
    {
      pt1 = motherseg1->pt[0];
      pt2 = motherseg1->pt[1];
      pt3 = motherseg2->pt[0];
      pt4 = motherseg2->pt[1];
      if (post_snap_required = (lines_intersect_3d(pt1,pt2,pt3,pt4,rotpt,
						   &dummyt1,&dummyt2,
						   Linetolinetol) !=
				Linesintersect))
	copypos3d(snapobj2->selectable_feature.nearestpos,rotpt);
      
      copypos3d(snapface1->facenorm,flippedtarget);
      flip_vec(flippedtarget);
      rotangle = snap_vector_to_vector(snapface2->facenorm,
				       flippedtarget,snaprotaxis);
      normalize(snaprotaxis,snaprotaxis);
      if (!vector_near_zero(snaprotaxis,Zeroveclengthtol))
	axis_rot_obj(snapobj2,snaprotaxis,rotpt,rotangle);
      
      if (post_snap_required)
      {
	find_obj_normals(snapobj2);
	compute_obj_planedists(snapobj2);
	adjust_dist = Fabs(Fabs(snapface2->planedist) -
			   Fabs(snapface1->planedist));
	scalevec3d(snapface2->facenorm,adjust_dist,trans);
	translate_feature((featureptr) snapobj2,trans,FALSE);
      }
      
      log_update((featureptr) snapobj2,Allupdateflags,Immediateupdate);
    }
  }
  else if ((snapfeature->type_id == Evf_type) &&
	   ((snaptofeature->type_id == Evf_type) ||
	    (snaptofeature->type_id == Fve_type)) )
  {
    snapevf = (evfptr) snapfeature;
    pt1 = snapevf->le1->facevert->pos;
    pt2 = snapevf->le2->facevert->pos;
    if (snaptofeature->type_id == Fve_type)
    {
      snapface1 = (fveptr) snaptofeature;
      motherseg1 = snapobj1->selectable_feature.motherseg;
      if (motherseg1->kind != Fve_intersect)
	return;			/* fail right now... */
      pt3 = motherseg1->pt[0];
      pt4 = motherseg1->pt[1];
    }      
    else
    {
      snaptoevf = (evfptr) snaptofeature;
      pt3 = snaptoevf->le1->facevert->pos;
      pt4 = snaptoevf->le2->facevert->pos;
    }
    diffpos3d(pt1,pt2,snapvec);
    normalize(snapvec,snapvec);
    diffpos3d(pt3,pt4,snaptovec);
    normalize(snaptovec,snaptovec);
    rotangle = snap_vector_to_vector(snapvec,snaptovec,snaprotaxis);
    normalize(snaprotaxis,snaprotaxis);
    if (!vector_near_zero(snaprotaxis,Zeroveclengthtol))
      axis_rot_obj(snapobj2,snaprotaxis,rotpt,rotangle);
    diffpos3d(pt1,pt3,trans);
    translate_feature((featureptr) snapobj2,trans,FALSE);
  }
}


/* This is some serious hack: there should be no exceptions for environment */
/* objects: one feature should be translated to a point dictated by another */
/* feature, that is all. */

  void
execute_SNAP(listptr world_list,stateptr state)
{
  featureptr snapfeature,snapsourcefeature,snaptargetfeature,motherfeature;
  vfeptr snapvfe;
  vertype snapvec,actualoffset,snap_pos;
  Coord *snapped;
  CPL_word op_word,as_word, ad_word;
  Boolean plane_moved = FALSE;
  ;
  if (((AS.word_type != Environment_data) &&
       (AS.word_type != Featurelist_data)) ||
      ((AT.word_type != Environment_data) &&
       (AT.word_type != Featurelist_data) &&
       (AT.word_type != Auxiliary_data)))
    return;
  /* right now can only snap a feature to the cursor or some CPLoffset */
  /* found in AT. Snapping to another feature not yet implemented, will be */
  /* in place for magnet tool */
  if (AS.word_type == Environment_data)
  {
    /* snap the cursor to a feature's first vertex, for now. */
    if (AS.word_data.CPLenvironment_part == Crosshair_part)
    {
      log_crosshair_erase_bboxes(state,Loginoldbuffer);
      if (AT.word_type == Featurelist_data)
      {
	if (AT.word_data.CPLfeaturelist->first.ep->thiselem != Nil)
	{
	  snapfeature = AT.word_data.CPLfeaturelist->first.ep->thiselem;
	  diffpos3d(state->cursor,get_first_vert(snapfeature),snapvec);
	  /* don't let snapping take the cursor outside the plane, EVER! */
	  compute_crosshair_move(snapvec,actualoffset,state);
	  addpos3d(state->cursor,actualoffset,state->cursor);
	  plane_moved = (actualoffset[vz] != 0.0);
	  if (plane_moved)
	    log_global_update(Redrawflag);
	  else
	    log_crosshair_erase_bboxes(state,Loginthisbuffer);
	}
      }
      else if (AT.word_type == Auxiliary_data)
      {
	copypos3d(AT.word_data.CPLauxdata.CPLoffsetdata,snapvec);
	/* don't let snapping take the cursor outside the plane, EVER! */
	compute_crosshair_move(snapvec,actualoffset,state);
	addpos3d(state->cursor,actualoffset,state->cursor);
	plane_moved = (actualoffset[vz] != 0.0);
	  if (plane_moved)
	    log_global_update(Redrawflag);
	  else
	    log_crosshair_erase_bboxes(state,Loginthisbuffer);
      }
    }
  }
  else if (AS.word_type == Featurelist_data)
  {
    setup_CPL_word_blank(&as_word,Featurelist_data);
    setup_CPL_auxword(&ad_word,CPLaux_offset_data);
    snapfeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (snapfeature != Nil)
    {
      if ((AT.word_type == Environment_data) || (AT.word_type == Auxiliary_data))
      {
	motherfeature = find_largergrain(snapfeature,Obj_type);
	snapped = get_first_vert(snapfeature);

	if (AT.word_type == Environment_data)
	{
	  if (AT.word_data.CPLenvironment_part == Crosshair_part)
	    copypos3d(state->cursor,snap_pos);
	  else if (AT.word_data.CPLenvironment_part == Cutplane_part)
	  {
	    copypos3d(snapped, snap_pos);
	    snap_pos[vz] = state->cursor[vz];
	  }	   
	}
	else
	  if (AT.aux_kind == CPLaux_offset_data) /* snap TO a point in space */
	    copypos3d(AT.word_data.CPLauxdata.CPLoffsetdata,snap_pos);
	
	/* hack for tools on toolpads */
	transform_obj_tree((objptr) motherfeature);
	diffpos3d(snapped,snap_pos,snapvec);
	translate_feature(motherfeature,snapvec,FALSE);
      }
      else if (AT.word_type == Featurelist_data)
      {			/* snap two features together... for magnet tool */
	/* What you are snapping TO */
	snaptargetfeature = snapfeature;
	/* What you are snapping. */
	snapsourcefeature = AT.word_data.CPLfeaturelist->first.ep->thiselem;
	if (snapsourcefeature != Nil)
	  snap_features_together(snapsourcefeature,snaptargetfeature);
      }
    }
  }
}


/* return obj2 in AR if obj2's bbox is within obj1's bbox */

  void
execute_WITHIN(listptr world_list,stateptr state)
{
  featureptr feature1,feature2;
  objptr motherobj1,motherobj2;
  bbox3dtype testbbox;
  ;
  if ((AS.word_type == Featurelist_data) && (AT.word_type == Featurelist_data))
  {
    feature1 = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    feature2 = AT.word_data.CPLfeaturelist->first.ep->thiselem;
    if ((feature1 != Nil) && (feature2 != Nil))
    {
      motherobj1 = (objptr) find_largergrain(feature1,Obj_type);
      motherobj2 = (objptr) find_largergrain(feature2,Obj_type);
      
      testbbox = motherobj1->objbbox;
      testbbox.mincorner[vz] -= 1.0; /* gotta have a little space or else*/
      testbbox.maxcorner[vz] += 1.0; /* it's intolerant */
      if ((sectionline_in_3dbbox(motherobj2, &(testbbox), state->cursor[vz])) ||
	  (point_in_3dbbox(motherobj2->objbbox.mincorner,
			   &(motherobj1->objbbox)) &&
	   (point_in_3dbbox(motherobj2->objbbox.maxcorner,
			    &(motherobj1->objbbox)))))
      {
	setup_CPL_word(&AR,Featurelist_data);
	add_to_featurelist(AR.word_data.CPLfeaturelist,(elemptr) motherobj2);
	return;
      }
    }
  }
  clear_word(&AR);  
}

/* Get the name of the mother obj of the feature passed in AS. */
  void
execute_GETNAME(listptr world_list,stateptr state)
{
  featureptr thefeature;
  objptr motherobj;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      motherobj = (objptr) find_largergrain(thefeature,Obj_type);
      setup_CPL_auxword(&AR,CPLaux_string_data);
      AR.word_data.CPLauxdata.CPLstringdata = get_object_name(motherobj);
      return;
    }
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Set an object's name if passed an object in AS, a new name in AD. */

  void
execute_SETNAME(listptr world_list,stateptr state)
{
  featureptr thefeature;
  objptr motherobj;
  ;
  if ((AS.word_type == Featurelist_data) && (AD.word_type == Auxiliary_data))
  {
    thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      if (AD.word_type == Auxiliary_data)
      {
	motherobj = (objptr) find_largergrain(thefeature,Obj_type);
	set_object_name(motherobj,AD.word_data.CPLauxdata.CPLstringdata);
      }
    }
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_GETLABEL(listptr world_list,stateptr state)
{
  featureptr thefeature;
  textstringptr text_string;
  objptr motherobj;
  int labelnumber;
  ;
  if ((AS.word_type == Featurelist_data) && (AD.word_type == Auxiliary_data))
  {
    thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      motherobj = (objptr) find_largergrain(thefeature,Obj_type);
      if (has_property((featureptr) motherobj,haslabel_prop))
      {
	labelnumber = AD.word_data.CPLauxdata.CPLintdata;
	text_string = find_label_item(motherobj,labelnumber);
	setup_CPL_auxword(&AR,CPLaux_string_data);
	if (text_string->string == Nil)
	  AR.word_data.CPLauxdata.CPLstringdata = Nil;
	else
	{
	  AR.word_data.CPLauxdata.CPLstringdata =
	    (char *) alloc_elem(strlen(text_string->string)+1);
	  strcpy(AR.word_data.CPLauxdata.CPLstringdata,text_string->string);
	}
	return;
      }
    }
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_SETLABEL(listptr world_list,stateptr state)
{
  featureptr thefeature;
  objptr motherobj;
  char *new_string;
  int labelnumber;
  ;
  if ((AS.word_type == Featurelist_data) && (AT.word_type == Auxiliary_data) &&
      (AD.word_type == Auxiliary_data))
  {
    thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      motherobj = (objptr) find_largergrain(thefeature,Obj_type);
      new_string = AT.word_data.CPLauxdata.CPLstringdata;
      labelnumber = AD.word_data.CPLauxdata.CPLintdata;
      set_label_item(motherobj,labelnumber,new_string);
    }
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_GETCOLOR(listptr world_list,stateptr state)
{
  featureptr thefeature;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature == Nil)
    {
      clear_word(&AR);
      return;
    }
    setup_CPL_auxword(&AR,CPLaux_color_data);
    get_feature_color(thefeature,AR.word_data.CPLauxdata.CPLcolordata);
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_SETCOLOR(listptr world_list,stateptr state)
{
  featureptr thefeature;
  ;
  if ((AS.word_type == Featurelist_data) && (AD.word_type == Auxiliary_data))
  {
    thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
      set_feature_color(thefeature, AD.word_data.CPLauxdata.CPLcolordata);
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_AUX2STR(listptr world_list,stateptr state)
{
  static char new_tempstr[50];
  ;
  if (AS.word_type == Auxiliary_data)
  {
    setup_CPL_word(&AR,Auxiliary_data);
    switch (AS.aux_kind)
    {
    case CPLaux_int_data:
    case CPLaux_char_data:
    case CPLaux_string_data:
      break;
    case CPLaux_float_data:
      sprintf(new_tempstr,"%1.2f",AS.word_data.CPLauxdata.CPLfloatdata);
      AR.word_data.CPLauxdata.CPLstringdata = strdup(new_tempstr);
      break;
    case CPLaux_offset_data:
    case CPLaux_matrix_data:
    case CPLaux_boolean_data:
    case CPLaux_address_data:
    case CPLaux_color_data:
    case CPLaux_update_data:
    case CPLaux_rotate_data:
    case CPLaux_scale_data:
    case CPLaux_duplicate_destroy_data:
    case CPLaux_owner_feature_data:
    case CPLaux_Croutine_index_data:
    case CPLaux_update_time_data:
    case CPLaux_cutplane_shade_data:
    case CPLaux_codekind_value_data:
    case CPLaux_new_world_data:
    default:
      break;
    }
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* This is moronic since once it's a string we have NO IDEA what kind of */
/* auxdata it once was. Assume it was a float for the heck of it.*/
  
  void
execute_STR2AUX(listptr world_list,stateptr state)
{
  float newval = 0.0;
  ;
  if (AS.word_type == Auxiliary_data)
  {
    setup_CPL_word(&AR,Auxiliary_data);
    if (AS.word_data.CPLauxdata.CPLstringdata != Nil)
      sscanf(AS.word_data.CPLauxdata.CPLstringdata,"%f",&newval);
    AR.aux_kind = CPLaux_float_data;
    AR.word_data.CPLauxdata.CPLfloatdata = newval;
  }
  else
    clear_word(&AR);      

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


  void
execute_ADDPROP(listptr world_list,stateptr state)
{
  featureptr thefeature;
  property theproperty;
  objptr ownerobj;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature == Nil)
      return;
    theproperty = (property) AD.word_data.CPLproplist->first.sort->sortdata1->i;
    add_property(thefeature, theproperty);
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_REMPROP(listptr world_list,stateptr state)
{
  featureptr thefeature;
  property theproperty;
  objptr ownerobj;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature == Nil)
      return;
    theproperty = (property) AD.word_data.CPLproplist->first.sort->sortdata1->i;
    
    if (has_property(thefeature,theproperty))
      del_property(thefeature, theproperty);
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Set the selectable for the motherobj of a feature. Typically the given */
/* feature is the one that will be selected. */

  void
execute_SELECT(listptr world_list,stateptr state)
{
  featureptr thefeature;
  objptr motherobj;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      if (!has_property(thefeature,selectable_prop))
      {
	motherobj = (objptr) find_largergrain(thefeature,Obj_type);
	unselect_obj(motherobj);
	set_feature_selectable(thefeature,state);
      }
    }
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* unselect the motherobj of a feature. Typically the given feature is the */
/* motherobj's selectable_feature.thefeature. */

  void
execute_UNSEL(listptr world_list,stateptr state)
{
  featureptr thefeature;
  objptr motherobj;
  ;
  fprintf(dbgdump, "\t*** Execute UNSEL\n");

  if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      if (has_property(thefeature,selectable_prop))
      {
	motherobj = (objptr) find_largergrain(thefeature,Obj_type);
	unselect_obj(motherobj);
      }
    }
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_PICK(listptr world_list,stateptr state)
{
  featureptr thefeature;
  ;
  fprintf(dbgdump, "\n\t*** Execute PICK\n\n");
  fflush(dbgdump);

  if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      if (!has_property(thefeature,picked_prop))
      {
	log_update((featureptr) thefeature,Logerasebboxflag,Immediateupdate);
	pick_feature(thefeature);
      }
    }
  }
  else if (AS.word_type == Environment_data)
  {
    if (AS.word_data.CPLenvironment_part == Cutplane_part)
    {
      log_cutplane_erase_bbox(state,Loginoldbuffer);
      pick_feature((featureptr) cutplane_obj);
      log_update((featureptr) cutplane_obj, Logerasebboxflag,Immediateupdate);
    }
  }    

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_RELEAS(listptr world_list,stateptr state)
{
  featureptr thefeature;
  objptr motherobj;
  ;
  fprintf(dbgdump, "\n\t*** Execute RELEAS\n\n");
  fflush(dbgdump);

  if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
      if (has_property(thefeature,picked_prop) &&
	  (thefeature->type_id != Obj_type))
      {
	motherobj = (objptr) find_largergrain(thefeature,Obj_type);
	find_obj_normals(motherobj);
	recombine_coplanar_faces(motherobj);
	if (update_on)
	  log_update((featureptr) motherobj,Cutevfsflag | Makecutsectionflag |
		     Findnormalsflag | Getselectableflag | Computebboxflag,
		     Immediateupdate);
      }
  }
  else if (AS.word_type == Environment_data)
  {
    if (AS.word_data.CPLenvironment_part == Cutplane_part)
    {
      log_cutplane_erase_bbox(state,Loginoldbuffer);
      unpick_feature((featureptr) cutplane_obj);
      log_update((featureptr) cutplane_obj, Logerasebboxflag,Immediateupdate);
      /* temp hack */
      log_global_update(Redrawflag);
    }
  }    

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_MRELEAS(listptr world_list,stateptr state)
{				/* right now exactly the same as RELEASE */
  featureptr thefeature;
  objptr motherobj;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
      if (has_property(thefeature,picked_prop) &&
	  (thefeature->type_id != Obj_type))
      {
	motherobj = (objptr) find_largergrain(thefeature,Obj_type);
	find_obj_normals(motherobj);
	recombine_coplanar_faces(motherobj);
	if (update_on)
	  log_update((featureptr) motherobj,Cutevfsflag | Makecutsectionflag |
		     Findnormalsflag | Getselectableflag | Computebboxflag,
		     Immediateupdate);      
      }
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_UNPICK(listptr world_list,stateptr state)
{
  featureptr thefeature;
  ;
  fprintf(dbgdump, "\t*** Execute UNPICK\n");
  if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
      if (has_property(thefeature,picked_prop))
      {
	log_update((featureptr) thefeature,Logerasebboxflag,Immediateupdate);
	unpick_feature(thefeature);
      }
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_DBLPIC(listptr world_list,stateptr state)
{

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_MPICK(listptr world_list,stateptr state)
{
  featureptr thefeature;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      log_update((featureptr) thefeature,Logerasebboxflag,Immediateupdate);
      pick_feature(thefeature);
    }
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_DUP(listptr world_list,stateptr state)
{
  featureptr thefeature,newmotherfeature;
  objptr motherobj;
  vertype offset;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature == Nil)
      return;
    motherobj = (objptr) find_largergrain(thefeature,Obj_type);
    
    newmotherfeature = (featureptr) duplicate_obj_branch(motherobj);
    if (update_on)
      log_update(newmotherfeature,Cutevfsflag | Makecutsectionflag |
		 Findnormalsflag | Getselectableflag,
		 Immediateupdate);
    setup_CPL_word(&AR,Featurelist_data);
    add_to_featurelist(AR.word_data.CPLfeaturelist,(elemptr) newmotherfeature);
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_DUPCPL(listptr world_list,stateptr state)
{

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_DELETE(listptr world_list,stateptr state)
{
  featureptr thefeature,motherobjfeature;
  worldptr theworld;
  objptr thischild;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = (featureptr) AS.word_data.CPLfeaturelist->first.ep->
      thiselem;
    if (thefeature == Nil)
      return;
    else			/* otherwise just try to kill it. */
      add_property(thefeature,killed_prop);
  }
  else if (AS.word_type == Auxiliary_data)
  {
    if (AS.word_data.CPLauxdata.new_world != Nil)
    {
      theworld = AS.word_data.CPLauxdata.new_world;
      thischild = theworld->root->children->first.obj;
      while (thischild != Nil)
      {
	add_property((featureptr) thischild,killed_prop);
	thischild = thischild->next;
      }
    }
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_BOOLEAN(listptr world_list,stateptr state)
{
  featureptr featureA,featureB;
  int operation;
  objptr objA,objB;
  listptr resultlist;
  elemptrptr thisresult;
  ;
  if ((AS.word_type == Featurelist_data) &&
      (AT.word_type == Featurelist_data) &&
      (AD.word_type == Auxiliary_data))
  {
    featureA = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (featureA == Nil)
      return;
    featureB = AT.word_data.CPLfeaturelist->first.ep->thiselem;
    if (featureB == Nil)
      return;
    objA = (objptr) find_largergrain(featureA,Obj_type);
    objB = (objptr) find_largergrain(featureB,Obj_type);
    operation = AD.word_data.CPLauxdata.CPLintdata;

    setup_CPL_word(&AR,Featurelist_data);
    resultlist = do_boolean(objA,objB,operation);
    thisresult = resultlist->first.ep;
    while (thisresult)
    {
      add_to_featurelist(AR.word_data.CPLfeaturelist, thisresult->thiselem);
      thisresult = thisresult->next;
    }
    del_list(resultlist);
#if 0
    /* Hack should return results in ep list for AR!! */
    clear_word(&AR);
#endif
  }
  else
    clear_word(&AR);

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


/* adopt an object in AT as a child to an object or a world passed in AS, using */
/* a link flag passed in AD. */

  void
execute_ADOPT(listptr world_list,stateptr state)
{
  featureptr parentfeature,childfeature;
  objptr parent,child;
  Boolean adopt_link;
  ;
  /* ADOPT expects a feature, environment part or World in AS, features in AT, */
  /* and a boolean const in AD.*/
  if (((AS.word_type == Featurelist_data) ||
       (AS.word_type == Environment_data) ||
       (AS.word_type == Auxiliary_data)) &&
      ((AT.word_type == Featurelist_data) ||
       (AT.word_type == Environment_data)) &&
      (AD.word_type == Auxiliary_data))
  {
    childfeature = AT.word_data.CPLfeaturelist->first.ep->thiselem;
    if (childfeature == Nil)
      return;
    adopt_link = AD.word_data.CPLauxdata.CPLbooleandata;
    switch (AS.word_type)
    {
    case Featurelist_data:
      parentfeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
      if (parentfeature == Nil)
	return;
      parent = (objptr) find_largergrain(parentfeature,Obj_type);
      break;
    case Environment_data:
      switch (AS.word_data.CPLenvironment_part)
      {
      case Crosshair_part:
	parent = cursor_obj;
	break;
      case Cutplane_part:
	parent = cutplane_obj;
	break;
      default:
	break;
      }
      break;
    case Auxiliary_data:
      adopt_link = TRUE;	/* when adopting at root, link is always TRUE */
      parent = AS.word_data.CPLauxdata.new_world->root;
      break;
    default:			
      adopt_link = TRUE;	/* when adopting at root, link is always TRUE */
      parent = working_world->root; /* if all else fails... but this is really */
      break;			    /* an error on CPL user's part! */
    }
    child = (objptr) find_largergrain(childfeature,Obj_type);
    adopt_obj_branch(parent,child,adopt_link);
    log_update((featureptr) parent,Computebboxflag,Immediateupdate);
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

/* Separate a child from its parent and make it a child of the root in its */
/* world. */

  void
execute_UNADOPT(listptr world_list,stateptr state)
{
  featureptr parentfeature,childfeature;
  objptr parent,child;
  worldptr world;
  ;
  if (AS.word_type == Featurelist_data)
  {
    childfeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (childfeature != Nil)
    {
      child = (objptr) find_largergrain(childfeature,Obj_type);
      world = child->motherworld;
      adopt_obj_branch(world->root,child,FALSE);
    }
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_GETLINK(listptr world_list,stateptr state)
{
  featureptr motherfeature;
  objptr motherobj;
  ;
  if (AS.word_type == Featurelist_data)
  {
    setup_CPL_auxword(&AR,CPLaux_boolean_data);
    motherfeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (motherfeature != Nil)
    {
      motherobj = (objptr) find_largergrain(motherfeature,Obj_type);
      AR.word_data.CPLauxdata.CPLbooleandata = motherobj->linked;
    }
    else
      AR.word_data.CPLauxdata.CPLbooleandata = FALSE;
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_UPDATE(listptr world_list,stateptr state)
{
  featureptr updatefeature;
  long updateflags;
  ;
  if (update_on)
  {
    if ((AS.word_type == Featurelist_data) && (AD.word_type == Auxiliary_data))
    {
      updatefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
      if (updatefeature != Nil)
      {
	updateflags = AD.word_data.CPLauxdata.update_data.update_flags;
	if (AD.word_data.CPLauxdata.update_data.update_now)
	  log_update(updatefeature,updateflags,Immediateupdate);
	else
	  log_update(updatefeature,updateflags,Nonimmediateupdate);
      }
    }
  }      

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_WTRANS(listptr world_list,stateptr state)
{
  featureptr thefeature;
  objptr motherobj;
  worldptr target_world;
  ;
  if ((AS.word_type == Featurelist_data) && (AT.word_type == Auxiliary_data))
  {
    thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      motherobj = (objptr) find_largergrain(thefeature,Obj_type);
      target_world = AT.word_data.CPLauxdata.new_world;
      
      /* according to lje this takes care of EVERYTHING!! */
      adopt_obj_branch(target_world->root,motherobj,TRUE);
    }
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_WORLD(listptr world_list,stateptr state)
{
  if (AD.word_type == Auxiliary_data)
    if (AD.word_data.CPLauxdata.new_world != Nil)
      working_world = AD.word_data.CPLauxdata.new_world;

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_GLUPDATE(listptr world_list,stateptr state)
{
  stateptr oldupdatestate;
  ;
  if (AD.word_type == Auxiliary_data)
  {
    oldupdatestate = oldstate;
    log_global_update(AD.word_data.CPLauxdata.update_data.update_flags);
    if (AD.word_data.CPLauxdata.update_data.update_now)
      process_global_updates(world_list,state,oldupdatestate);
  }

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_CPSHADE(listptr world_list,stateptr state)
{
  /* right now just turn zbuffering on and off.  Was supposed to turn shading */
  /* on/off for different environment parts, etc. */
  toggle_zbuffering();

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


  void
execute_CONNECT(listptr world_list,stateptr state)
{

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_GETCONN(listptr world_list,stateptr state)
{
  featureptr thefeature,theconnection;
  objptr motherobj;
  ;
  if (AS.word_type == Featurelist_data)
  {
    thefeature = AS.word_data.CPLfeaturelist->first.ep->thiselem;
    if (thefeature != Nil)
    {
      motherobj = (objptr) find_largergrain(thefeature,Obj_type);
      setup_CPL_word(&AR,Featurelist_data);
      if (has_property((featureptr) motherobj,connection_prop))
      {
	theconnection = (featureptr) get_property_val((featureptr) motherobj,
						  connection_prop).l;
	add_to_featurelist(AR.word_data.CPLfeaturelist, theconnection);
      }
      else
	clear_word(&AR);
    }
    else
      clear_word(&AR);
  }
  else
    clear_word(&AR);


#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_RECON(listptr world_list,stateptr state)
{

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_RECOFF(listptr world_list,stateptr state)
{

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_UPDATEON(listptr world_list,stateptr state)
{
  update_on = TRUE;

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_UPDATEOFF(listptr world_list,stateptr state)
{
  update_on = FALSE;

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}


  void
execute_QUIT(listptr world_list,stateptr state)
{
  exit_system = TRUE;

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

  void
execute_DEBUG(listptr world_list,stateptr state)
{
  if (AD.word_type == Auxiliary_data)
    debugcpl = AD.word_data.CPLauxdata.CPLbooleandata;

#ifndef IRIS
  cplops_world_list = world_list;
  cplops_state = state;
#endif
}

