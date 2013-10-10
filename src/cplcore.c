
/* FILE: cplcore.c */
/* Updated and bugs repaired, 12/17/01 */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*     The Cutplane Language (CPL) Core Parser for the Cutplane Editor    */
/*                                                                        */
/* Author: WAK                                       Date: August 19,1990 */
/* Version: 3                                                             */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990,1991 William Kessler and             */
/* Larry Edwards. All Rights Reserved.                                    */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define CPLCOREMODULE

#include <config.h>
#include <stdio.h>

#include <platform_gl.h>

#ifdef IRIS
#include <strings.h>
#else
#include <string.h>
#endif

#include <cutplane.h>
#include <cpl_cutplane.h>	/* cut me when included by cutplane.h... */

#define debugged

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	                       GLOBAL VARIABLES				     */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

FILE *cplwatchfile = 0;
Boolean debugcpl = FALSE, debugcplspool = FALSE;

Boolean restart_CPLparser = FALSE;
CPL_word_ptr cpnormal_address;
CPL_word_ptr COP_init, CAS_init, CAT_init, CAR_init, CAD_init, CNF_init;
CPL_word_ptr XOP_init, XAS_init, XAT_init, XAR_init, XAD_init, XNF_init;
CPL_word_ptr COP_ptr_init,XOP_ptr_init;
CPL_word_ptr CPL_RAM_init;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=- ACCUMULATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* Loop registers are used while iterating through one instruction.  They are*/
/* used only for immediate data or holding a single feature for processing. */
/* They do not hold properties */

CPL_word_ptr OP;
CPL_word AS,AT,AR,AD;

/* Pointers to the current CPL_frame_bytes.  These are set assuming these */
/* frame bytes are *reserved* words somewhere in the CPL memory (for now, */
/* at the start of the main memory, although they could be somewhere else). */

CPL_word_ptr COP,CAS,CAT,CAR,CAD,CNF;
CPL_word_ptr XOP,XAS,XAT,XAR,XAD,XNF;

CPL_word_ptr ROM_start, RAM_start;

/* These records contain information on how to iterate on an accumulator. */
iteraterec AS_iteraterec,AT_iteraterec,AD_iteraterec;
						   

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- STACKS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* The general usage CPL word stack used by PUSH and POP. */
listptr CPL_stack; 

/* The JSR and RTS stack holding pointer to return CPL words. */
CPL_word CPL_controlstack[CPL_Controlstacksize];
int CPL_controlstack_top = 0;	/* Pointer to topmost entry in control stack */

/* The stack used to store the owner variable during JSR's and RTS's */
featureptr current_owner = Nil;	/* the most recent owner of the code currently */
				/* executing. */
featureptr owner_stack[CPL_Ownerstacksize];
int owner_stack_top = 0;	/* Pointer to topmost entry in owner stack */

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=- CODEBLOCKS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/* The core memory ROM & RAM */
CPL_word system_codebloc[Systemcodeblocsize];
/* Where a new instruction can be loaded into the core memory. */
CPL_word_ptr next_free_CPL_word = Nil;
/* Where a new instruction can be loaded into the RAM portion of core memory. */
CPL_word_ptr next_free_RAM_word = Nil;

CPLauxtype auxiliary_bloc[100/*Auxiliaryblocsize*/];
int next_free_auxiliary_index = 0;
Boolean CPL_aux_free_markers[Auxiliaryblocsize];

CPL_word temp_message_block[Maxmessagelength];
int end_temp_message_block = 0;
Boolean reset_temp_message_block = FALSE;

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-= LOOKUP TABLES =-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* This is the lookup table for all the symbols. */

symbolrec symbol_records[MaxCPLsymbols];
int top_symbol_record = 0;

/* This is the lookup table for one CPL file. */
symbolrec local_symbol_records[MaxCPLsymbols];
int top_local_symbol_record = -1;

/* This is a lookup table for ALL local names, including duplicates, which */
/* is meant to be looked up by address ONLY and only for debugging */
/* (disassembly) output.*/
symbolrec total_local_symbol_records[MaxCPLsymbols];
int top_total_local_symbol_record = 0;


/* This is a lookup table of all the known opcodes, incl. which args are */
/* req'd and which op routine in cplops.c to execute. */

opcoderec opcoderecs[Maxopcodes] = {{"NOP",Noargs,execute_NOP},
				    {"CPLDATA",Noargs,execute_NOP},
				    {"CPLPTR",Noargs,execute_NOP},
				    {"CPLHANDLE",Noargs,execute_NOP},
				    {"MOVE",ASarg | ARarg,execute_MOVE},
				    {"ADD",ASarg | ATarg | ARarg,execute_ADD},
				    {"CLEAR",ARarg,execute_CLEAR},
				    {"PUSH",ASarg,execute_PUSH},
				    {"POP",ARarg,execute_POP},
				    {"WIPE",ASarg,execute_WIPE},
				    {"UNWIPE",ASarg,execute_UNWIPE},

				    {"AND",ASarg | ATarg | ARarg,execute_AND},
				    {"OR",ASarg | ATarg | ARarg, execute_OR},
				    {"NOT",ASarg | ATarg | ARarg, execute_NOT},
				    {"COUNT",ASarg | ARarg, execute_COUNT},
				    {"INDEX",ASarg | ARarg | ADarg,
				       execute_INDEX},

				    {"BEQUAL",ASarg | ATarg | ADarg,
				       execute_BEQUAL},
				    {"BNEQUAL",ASarg | ATarg | ADarg,
				       execute_BNEQUAL},
				    {"BEMPTY",ASarg | ADarg, execute_BEMPTY},
				    {"BNEMPTY",ASarg | ADarg, execute_BNEMPTY},
				    {"BTRUE",ASarg | ADarg, execute_BTRUE},
				    {"BFALSE",ASarg | ADarg, execute_BFALSE},
				    {"BWIPED",ASarg | ADarg,execute_BWIPED},

				    {"OWNER",ARarg | ADarg, execute_OWNER},
				    {"SETOWN",ADarg,execute_SETOWN},
				    {"MOTHER",ASarg | ARarg | ADarg,
				       execute_MOTHER},
				    {"ANCEST",ASarg | ARarg, execute_ANCEST},
				    {"DESCEND",ASarg | ARarg, execute_DESCEND},
				    {"PARENT",ASarg | ARarg,execute_PARENT},
				    {"CHLDRN",ASarg | ARarg,execute_CHLDRN},
				    {"SIBLING",ASarg | ARarg, execute_SIBLING},
				    {"MAKE",ATarg | ARarg | ADarg, execute_MAKE},

				    {"BMSGS",ASarg | ADarg,execute_BMSGS},
				    {"BNOMSGS",ASarg | ADarg,execute_BNOMSGS},
				    {"MAKEMSG",ASarg,execute_MAKEMSG},
				    {"QMSG",ASarg | ADarg,execute_QMSG},
				    {"UNQMSG",ARarg, execute_UNQMSG},
				    {"CHKMSG",ARarg, execute_CHKMSG},

				    {"JSR", ADarg,execute_JSR},
				    {"JSRAT",ASarg | ADarg,execute_JSRAT},
				    {"RTS",Noargs,execute_RTS},
				    {"JUMP",ADarg,execute_JUMP},

				    {"SETFR",ASarg, execute_SETFR},
				    {"ENDFR",Noargs,execute_ENDFR},

				      /* arguments to C routines are passed */
				      /* on the CPL stack...*/
				    {"CRTN", ADarg, execute_CRTN},

				    {"TRANS",ASarg | ATarg | ADarg,
				       execute_TRANS},
				    {"ROTATE",ASarg | ADarg, execute_ROTATE},
				    {"SCALE",ASarg | ADarg, execute_SCALE},
				    {"TRNSFRM",ASarg | ARarg | ADarg,
				       execute_TRNSFRM},
				    {"CONCAT",ASarg | ATarg | ARarg,
				       execute_CONCAT},
				    {"DIFF3D",ASarg | ATarg | ARarg,
				       execute_DIFF3D},
				    {"SNAP",ASarg | ATarg,
				       execute_SNAP},
				    {"WITHIN",ASarg | ATarg | ARarg,
				       execute_WITHIN},

				    {"GETNAME",ASarg | ARarg,execute_GETNAME},
				    {"SETNAME",ASarg | ADarg,execute_SETNAME},
				    {"GETLABEL",ASarg | ARarg | ADarg,
				       execute_GETLABEL},
				    {"SETLABEL",ASarg | ATarg | ADarg,
				       execute_SETLABEL},

				    {"GETCOLOR",ASarg | ARarg, execute_GETCOLOR},
				    {"SETCOLOR",ASarg | ADarg, execute_SETCOLOR},

				    {"AUX2STR",ASarg | ARarg, execute_AUX2STR},
				    {"STR2AUX",ASarg | ARarg, execute_STR2AUX},
				      
				      /* HASPROP does get_property if applic. */
				    {"HASPROP",ASarg | ATarg | ARarg | ADarg,
				       execute_HASPROP},
				    {"GETPROP", ASarg | ATarg | ARarg,
				       execute_GETPROP},
				    {"ADDPROP",ASarg | ADarg,execute_ADDPROP},
				    {"REMPROP",ASarg | ADarg,execute_REMPROP},

				    {"SELECT",ASarg,execute_SELECT},
				    {"UNSEL",ASarg,execute_UNSEL},
				    {"PICK",ASarg,execute_PICK},
				    {"RELEAS",ASarg,execute_RELEAS},
				    {"UNPICK",ASarg,execute_UNPICK},
				    {"DBLPIC",ASarg,execute_DBLPIC},
				    {"MPICK",ASarg,execute_MPICK},
				    {"MRELEAS",ASarg,execute_MRELEAS},
				      
				    {"DUP",ASarg | ARarg, execute_DUP},
				    {"DUPCPL",ASarg | ARarg, execute_DUPCPL},

				    {"CPDELETE",ASarg,execute_DELETE},
				    {"CPBOOLEAN",ASarg | ATarg | ARarg | ADarg,
				       execute_BOOLEAN},

				    {"ADOPT",ASarg | ATarg | ADarg,
				       execute_ADOPT},
				    {"UNADOPT",ASarg, execute_UNADOPT},
				    {"GETLINK",ASarg | ARarg,
				       execute_GETLINK},

				    {"WORLD", ADarg, execute_WORLD},
				    {"WTRANS", ASarg | ATarg, execute_WTRANS},
				    {"UPDATE",ASarg | ADarg, execute_UPDATE},
				    {"GLUPDATE",ADarg, execute_GLUPDATE},
				    {"CPSHADE",ASarg,execute_CPSHADE},

				    {"CONNECT",ASarg | ATarg | ADarg,
				       execute_CONNECT},
				    {"GETCONN",ASarg | ARarg,execute_GETCONN},

				    {"RECON",ASarg | ADarg,execute_RECON},
				    {"RECOFF",ASarg,execute_RECOFF},
				    {"UPDATEON",Noargs,execute_UPDATEON},
				    {"UPDATEOFF",Noargs,execute_UPDATEOFF},

				    {"QUIT",Noargs,execute_QUIT},

				    {"DEBUG",ADarg,execute_DEBUG},
				      
				    {"DEFPROP",Noargs, execute_NOP},
				    {"STARTPROP",Noargs, execute_NOP} };

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

  void
blankout_CPL_word(CPL_word_ptr blanked_word)
{
  /* blank out a new word */
  blanked_word->word_type = Blank_word;
  blanked_word->word_data.CPLptr = Nil;
  blanked_word->contents_of = blanked_word->address_of = 
    blanked_word->wiped = FALSE;
}

  
  void
clear_word(CPL_word_ptr word_to_clear)
{
  switch (word_to_clear->word_type)
  {
  case Opcode_data:
    word_to_clear->word_data.CPLopcode = NOP;
    break;
  case Message_data:
    word_to_clear->word_data.CPLmessage = -1;
    break;
  case Environment_data:
    word_to_clear->word_data.CPLenvironment_part = Nothingness;
    break;
  case Icon_data:
    word_to_clear->word_data.CPLicon = Noicon;
    break;
  case CPLptr_data:
    word_to_clear->word_data.CPLptr = Nil;
    break;
  case CPLhandle_data:
    word_to_clear->word_data.CPLhandle = Nil;
    break;
  case Featurelist_data:
    del_list(word_to_clear->word_data.CPLfeaturelist);
    break;
  case Proplist_data:
    del_list(word_to_clear->word_data.CPLproplist);
    break;
  case Auxiliary_data:
    break;
  }
  word_to_clear->word_type = Blank_word;
  word_to_clear->contents_of = word_to_clear->address_of = 
    word_to_clear->wiped = FALSE;
}

  listptr
proplist_to_featurelist(listptr proplist)
{
  listptr new_list;
  sortptr thisprop;
  elemptrptr newfeature_ep;
  property theprop;
  proptr propelement;
  ;
  new_list = create_list();
  for (thisprop = proplist->first.sort; thisprop != Nil;
       thisprop = thisprop->next)
  {
    theprop = (property) thisprop->sortdata1->i;
    for (propelement = working_world->world_proplists[(int) theprop]->
	 first.prop; propelement != Nil; propelement = propelement->next)
    {
      newfeature_ep = (elemptrptr) append_new_elem(new_list,Ep_type);
      newfeature_ep->thiselem = propelement->owner;
    }
  }
  return(new_list);
}

  void
copy_wordA_to_wordB(CPL_word_ptr wordA,CPL_word_ptr wordB)
{
  if (wordB->word_type != Blank_word)
    clear_word(wordB);
  
  if (wordA->word_type <= Auxiliary_data)
    *wordB = *wordA;
  else if (wordA->word_type == Featurelist_data)
  {
    wordB->word_type = Featurelist_data;
    wordB->word_data.CPLfeaturelist =
      clone_list(wordA->word_data.CPLfeaturelist);
  }
  else if (wordA->word_type == Proplist_data)
  {
    wordB->word_type = Proplist_data;
    wordB->word_data.CPLproplist =
      clone_list(wordA->word_data.CPLproplist);
  }
}

  void
add_wordA_to_wordB(CPL_word_ptr wordA,CPL_word_ptr wordB)
{
#ifdef IRIS
  listptr newordAlist,newordBlist;
#else
  listptr newordAlist;
#endif
  ;
  /* Operates only on lists of features or props. */
  if ((wordB->word_type != Featurelist_data) &&
      (wordB->word_type != Proplist_data))
  {
    if (wordB->word_type != Blank_word)
      clear_word(wordB);
    copy_wordA_to_wordB(wordA,wordB);
  }
  else if ((wordB->word_type == Featurelist_data) &&
      (wordA->word_type == Featurelist_data))
  {
    newordAlist = clone_list(wordA->word_data.CPLfeaturelist);
    merge_lists(newordAlist,wordB->word_data.CPLfeaturelist);
/*
    merge_lists_uniquely(newordAlist,wordB->word_data.CPLfeaturelist);
*/    
  }
  else if ((wordB->word_type == Featurelist_data) &&
      (wordA->word_type == Proplist_data))
  {
    newordAlist = proplist_to_featurelist(wordA->word_data.CPLproplist);
    merge_lists(newordAlist,wordB->word_data.CPLfeaturelist);
/*
    merge_lists_uniquely(newordAlist,wordB->word_data.CPLfeaturelist);
*/
  }
  else if ((wordB->word_type == Proplist_data) &&
      (wordA->word_type == Featurelist_data))
  {				/* if wordB has a property list convert to a */
				/* feature list before adding */
    /* The next line appears to have never been used, so commented out on */
    /* 12/17/01 by WAK */
    /* newordBlist = proplist_to_featurelist(wordB->word_data.CPLproplist); */
    del_list(wordB->word_data.CPLproplist);
    wordB->word_type = Featurelist_data;
    newordAlist = clone_list(wordA->word_data.CPLfeaturelist);
    merge_lists(newordAlist,wordB->word_data.CPLfeaturelist);
/*
    merge_lists_uniquely(newordAlist,wordB->word_data.CPLfeaturelist);
*/
  }
  else if ((wordB->word_type == Proplist_data) &&
      (wordA->word_type == Proplist_data))
  {
    newordAlist = clone_list(wordA->word_data.CPLproplist);
    merge_lists(newordAlist,wordB->word_data.CPLproplist);
/*
    merge_lists_uniquely(newordAlist,wordB->word_data.CPLproplist);
*/
  }
}

  featureptr
lookup_feature_fcn(iterateptr iterate_info)
{
  /* Featurelists are done with ep's */
  return ( (featureptr) (iterate_info->looptr1->thiselem) );
}

  Boolean
advance_feature_fcn(iterateptr iterate_info)
{
  iterate_info->looptr1 = iterate_info->nextlooptr1;
  if (iterate_info->looptr1 != Nil) {
    /* If TRUE, a reset is necessary */
    iterate_info->nextlooptr1 = iterate_info->looptr1->next;
    return(FALSE);		/* added 12/17/01 to fix major bug! */
  } else
    return(TRUE);
}

  void
reset_feature_fcn(iterateptr iterate_info)
{
  iterate_info->looptr1 = iterate_info->loopreset1;
  iterate_info->nextlooptr1 = iterate_info->looptr1->next;
}

  featureptr
lookup_props_fcn(iterateptr iterate_info)
{
  /* For props, we're going down the actual proplist (no ep's involved) */
  if (iterate_info->looptr1 != Nil)
    return(((proptr) iterate_info->looptr1)->owner);
  return(Nil);
}

  void
reset_props_innerloop(iterateptr iterate_info)
{
  property thisproperty;
  ;
  thisproperty = (property) (((sortptr) iterate_info->looptr2)->sortdata1->i);
  iterate_info->looptr1 = (elemptrptr) (working_world->
					world_proplists[(int) thisproperty]->
					first.prop);
}

  Boolean
advance_props_fcn(iterateptr iterate_info)
{
  iterate_info->looptr1 = iterate_info->nextlooptr1;
  if (iterate_info->looptr1 == Nil)
  {
    iterate_info->looptr2 = iterate_info->nextlooptr2;
    if (iterate_info->looptr2 == Nil)
      return (TRUE);		/* return TRUE when a reset is necessary */
    /* otherwise, be sure to set the next pointer */
    iterate_info->nextlooptr2 = iterate_info->looptr2->next;
    reset_props_innerloop(iterate_info);
  }

  if (iterate_info->looptr1 != Nil) /* don't do advancement if no props left */
    iterate_info->nextlooptr1 = iterate_info->looptr1->next;
    
  return(FALSE);
}

  void
reset_props_fcn(iterateptr iterate_info)
{
  iterate_info->looptr2 = iterate_info->loopreset2;
  if (iterate_info->looptr2 != Nil)
    iterate_info->nextlooptr2 = iterate_info->looptr2->next;
  reset_props_innerloop(iterate_info);
  if (iterate_info->looptr1 != Nil) /* don't do advancement if no props left */
    iterate_info->nextlooptr1 = iterate_info->looptr1->next;
}



  void
setup_CPL_word(CPL_word_ptr setup_word,CPLwordkind setupkind)
{
#ifdef IRIS
  elemptrptr only_epelem;
#endif
  sortptr only_sortelem;
  ;
  clear_word(setup_word);
  setup_word->word_type = setupkind;
  if (setupkind == Featurelist_data)
  {
    setup_word->word_data.CPLfeaturelist = create_list();
  }
  else if (setupkind == Proplist_data)
  {
    setup_word->word_data.CPLproplist = create_list();
    append_new_blank_elem(setup_word->word_data.CPLproplist,Sort_type);
    only_sortelem = setup_word->word_data.CPLproplist->first.sort;
    only_sortelem->sortdata1 = (alltypes *) alloc_elem(Allsize);
    only_sortelem->sortdata1->i = (int) noproperty_prop;
  }
  else if (setupkind == Auxiliary_data)
  {
    /* does nothing since USUALLY(!!) called by setup_CPL_auxword(), below. */
    /* So, all this routine does in the case of Auxiliary_data is clear the */
    /* word in question. */
  }
}


  void
setup_CPL_word_blank(CPL_word_ptr setup_word,CPLwordkind setupkind)
{
  blankout_CPL_word(setup_word);
  setup_CPL_word(setup_word,setupkind);
}

  void
setup_CPL_auxword(CPL_word_ptr setup_word,auxdatakind word_auxkind)
{
  blankout_CPL_word(setup_word);
  setup_CPL_word(setup_word,Auxiliary_data);
  setup_word->aux_kind = word_auxkind;
}

  
/* Resolve indirections up to two levels deep (ie. return a pointer to */
/* the word this_CPLptr points to if this_CPLptr is a single pointer, or */
/* to the word pointed to by the word this_CPLptr points to if this_CPLptr */
/* is a handle, or only return this_CPLptr itself if it is not a pointer */
/* at all. */

  CPL_word_ptr
resolve_indirection(CPL_word_ptr this_CPLptr)
{
  CPL_word_ptr returnptr;
  ;
  /* Indirection on Nil not possible (!) so return Nil if you get passed it */
  if (this_CPLptr == Nil)
    return(Nil);
  
  switch (this_CPLptr->word_type)
  {
  case CPLptr_data:
    if (this_CPLptr->address_of)
      returnptr = this_CPLptr;
    else
      returnptr = this_CPLptr->word_data.CPLptr;
    break;
  case CPLhandle_data:
    if (this_CPLptr->address_of)
      returnptr = this_CPLptr;
    else
      returnptr = this_CPLptr->word_data.CPLhandle->word_data.CPLptr;
    break;
  case Blank_word:
  default:
    returnptr = this_CPLptr;
    break;
  }
  if (returnptr == Nil)
    return(this_CPLptr);
  return(returnptr);
}

  Boolean
initialize_iterate(iterateptr iterate_info, CPL_word_ptr this_word_iterate,
		   CPL_word_ptr loop_reg)
{
  CPL_word_ptr actual_word;
  property thisproperty;
  ;
  if (this_word_iterate == Nil)
  {				/* Iteration not necessary */
    iterate_info->iteration_on = FALSE;
    return(FALSE);
  }
  else
    /* If iterate word is an indirect pointer,access the word it's */
    /* referring to. Most often used to refer to registers which occupy */
    /* cpl words in memory like any other cplwords, */
    /* but can also deal with handles.*/
    actual_word = resolve_indirection(this_word_iterate);
  
  switch (actual_word->word_type)
  {
  case Blank_word:
  case Opcode_data:
  case Message_data:
  case Environment_data:
  case Auxiliary_data:
  case Icon_data:
  case CPLptr_data:		/* Just copy pointers if the opcode requires */
  case CPLhandle_data:		/* them. (eg BEQ, JSR, JUMP) */
    iterate_info->iteration_on = FALSE;	/* No iteration necessary */
    /* Copy the immediate data into the temp reg (incl aux data if present). */
    /* Note that the feature list/proplist stuff in copy_wordA_to_wordB won't */
    /* get used since the source word (actual_word) is not of those types. */
    copy_wordA_to_wordB(actual_word,loop_reg);
    return(FALSE);
  case Featurelist_data:	/* Set up iteration over features in list. */
				/* You can never put the actual features list */
				/* in the register verbatim, and must always */
				/* iterate. */
    if (this_word_iterate->contents_of)
    {
      setup_CPL_word(loop_reg,Featurelist_data);
      add_to_featurelist(loop_reg->word_data.CPLfeaturelist,Nil);
      if (actual_word->word_data.CPLfeaturelist->numelems == 0)
      {				/* in this case, no elements are in list! */
	iterate_info->iteration_on = FALSE;
	return(FALSE);
      }
      else
      {				/* if some elements in list, then iterate */
	iterate_info->iteration_on = TRUE;
	iterate_info->element_lookup_fcn = lookup_feature_fcn;
	iterate_info->iterate_advance_fcn = advance_feature_fcn;
	iterate_info->iterate_reset_fcn = reset_feature_fcn;
	iterate_info->looptr1 = actual_word->word_data.CPLfeaturelist->first.ep;
	iterate_info->nextlooptr1 = iterate_info->looptr1->next;
	iterate_info->looptr2 = iterate_info->nextlooptr2 = Nil;
	iterate_info->loopreset1 = iterate_info->looptr1;
	iterate_info->loopreset2 = Nil;
	return(TRUE);
      }
    }
    else
    {				/* in case not using contents of, put the */
				/* featurelist in the register verbatim. This */
				/* opcode is therefore expecting an actual */
				/* proplist, not the elements it refers to. */
      iterate_info->iteration_on = FALSE;	/* Do not iteration necessary */
      copy_wordA_to_wordB(actual_word,loop_reg);
      return(FALSE);
    }
  case Proplist_data:	
    if (this_word_iterate->contents_of)
    {				/* Set up iteration over things w/props */
      setup_CPL_word(loop_reg,Featurelist_data);
      add_to_featurelist(loop_reg->word_data.CPLfeaturelist,Nil);
      thisproperty = (property) actual_word->word_data.CPLproplist->
	first.sort->sortdata1->i;
      if (working_world->world_proplists[(int) thisproperty]->numelems == 0)
      {				/* in this case, no elements have this prop! */
	iterate_info->iteration_on = FALSE;
	return(FALSE);
      }
      else
      {				/* if some elements have this prop, iterate */
	iterate_info->iteration_on = TRUE;
	iterate_info->element_lookup_fcn = lookup_props_fcn;
	iterate_info->iterate_advance_fcn = advance_props_fcn;
	iterate_info->iterate_reset_fcn = reset_props_fcn;
	iterate_info->looptr1 = (elemptrptr)
	  working_world->world_proplists[(int) thisproperty]->first.prop;
	iterate_info->nextlooptr1 = iterate_info->looptr1->next;
	iterate_info->looptr2 =
	  (elemptrptr) actual_word->word_data.CPLproplist->first.sort;
	iterate_info->nextlooptr2 = iterate_info->looptr2->next;
	iterate_info->loopreset1 = iterate_info->looptr1;
	iterate_info->loopreset2 = iterate_info->looptr2;
	return(TRUE);
      }
    }
    else
    {				/* in case not using contents of, put the */
				/* proplist in the register verbatim. This */
				/* opcode is therefore expecting an actual */
				/* proplist, not the elements it refers to. */
      iterate_info->iteration_on = FALSE;	/* Do not iterate */
      copy_wordA_to_wordB(actual_word,loop_reg);
      return(FALSE);
    }
  }
  return(FALSE);		/* added 12/17/01 so that something is */
				/* always returned */
}


/* returns TRUE for successful iteration; if reset, then iteration was NOT */
 /* successful. */
    
  Boolean
CPL_iterate(CPL_word_ptr fill_reg,iterateptr iterate_info,Boolean setup_only)
{
  Boolean reset_reqd;		/* (resetting not required). */
  elemptr successful_lookup;
  ;
  if (!(iterate_info->iteration_on))
    return(FALSE);		/* No iteration done (same as reset required) */
  else
  {
    if (setup_only)
    {
      fill_reg->word_data.CPLfeaturelist->first.ep->thiselem = (elemptr)
       (iterate_info->element_lookup_fcn) (iterate_info);
      return(TRUE);
    }
    else
    {
      reset_reqd = (iterate_info->iterate_advance_fcn) (iterate_info);
      if (reset_reqd)
	(iterate_info->iterate_reset_fcn) (iterate_info);
      successful_lookup =
	fill_reg->word_data.CPLfeaturelist->first.ep->thiselem =
	  (elemptr) (iterate_info->element_lookup_fcn) (iterate_info);
      /* if a reset was required, return FALSE; if the lookup failed because */
      /* no more iteration was possible (e.g. came up NIL after a reset), */
      /* return FALSE. */
      return(!reset_reqd && (successful_lookup != Nil));
    }
  }
}

/* The main resolver routine that executes one instruction by calling the */
/* instruction while iterating through its arguments. The "lead iteration */
/* argument" is the first one among the AS,AT, and AD words that requires */
/* iteration, and the resolver will stop when iteration is not possible */
/* on that register. The iteration scheme is n^2, like the digits in */
/* a speedometer. For every item iterated in AT all the items in AD are */
/* iterated on, and then for every item iterated in AS all the items in AT */
/* are iterated on (with all the items in AD iterated for each AT item, etc).*/
/* Thus, when you do say a boolean union on featurelists in AS and AT, */
/* note that every feature in AS will be unioned with every feature in AT */
/* and the result will be m*n features if AS holds m features and AT holds n.*/

  void
CPL_resolver(listptr world_list,stateptr state,
	     CPL_word_ptr OP_ptr,CPL_word_ptr AS_ptr,CPL_word_ptr AT_ptr,
	     CPL_word_ptr AR_ptr,CPL_word_ptr AD_ptr)
{
  opcodekind opcode_id;
  CPL_word_ptr results_word;
  Boolean still_iterating,AS_iterated,AT_iterated,AD_iterated;
  Boolean at_least_one_iteration,did_first_iteration = FALSE;
  accumulatorkind lead_iterator = Noaccumulator;
#ifdef IRIS
  char symbol_name[Maxpropertynamelen],debugstr[MaxCPLinelength];
#else
  char debugstr[MaxCPLinelength];
#endif
  ;
  if (debugcpl)
  {
#ifdef IRIS
    heapinfo();
#endif
    disassemble_CPL_sentence(OP_ptr,AS_ptr,AT_ptr,AR_ptr,AD_ptr,debugstr);
    printf ("%s",debugstr);
  }
  if (debugcplspool)
  {
    disassemble_CPL_sentence(OP_ptr,AS_ptr,AT_ptr,AR_ptr,AD_ptr,debugstr);
    fprintf (cplwatchfile,"%s",debugstr);
    fflush(cplwatchfile);
  }
  
  if (OP_ptr->word_data.CPLptr->wiped)
    return;		/* if instruction non-executing, don't resolve */

  OP = OP_ptr->word_data.CPLptr;
  opcode_id = OP->word_data.CPLopcode;

  if (debugcplspool && (opcode_id == NOP))
    fprintf(cplwatchfile, "NOP encountered\n");

  if (initialize_iterate(&AD_iteraterec,AD_ptr->word_data.CPLptr,&AD))
    lead_iterator = AD_accum;
  if (initialize_iterate(&AT_iteraterec,AT_ptr->word_data.CPLptr,&AT))
    lead_iterator = AT_accum;
  if (initialize_iterate(&AS_iteraterec,AS_ptr->word_data.CPLptr,&AS))
    lead_iterator = AS_accum;

  at_least_one_iteration = AS_iteraterec.iteration_on ||
    AT_iteraterec.iteration_on || AD_iteraterec.iteration_on;
  /* If this instruction is to place its results in a result word then */
  /* be sure to set up copying of results of each iteration from AR to */
  /* that result word. */
  results_word = resolve_indirection(AR_ptr->word_data.CPLptr);

  /* CLEAR THE RESULT REG FIRST */
  if (results_word != Nil)
    clear_word(results_word);

  if (opcode_id == JSRAT)
    start_JSRAT = TRUE;		/* if doing a JSRAT, make sure execute_JSRAT */
				/* knows that we are starting iteration */

  do				/* instructions are always executed ONCE */
  {
				/* ITERATE: N^2 iteration*/
    if (!did_first_iteration)
    {			/* if the very first iteration, set them all up! */
      AD_iterated = CPL_iterate(&AD, &AD_iteraterec,Setupiterateregsonly);
      AT_iterated = CPL_iterate(&AT, &AT_iteraterec,Setupiterateregsonly);
      AS_iterated = CPL_iterate(&AS, &AS_iteraterec,Setupiterateregsonly);
      did_first_iteration = TRUE;
    }
    else
    {
      AD_iterated = CPL_iterate(&AD, &AD_iteraterec,Advanceanditerate);
      if (!AD_iterated)
      {
	AT_iterated = CPL_iterate(&AT, &AT_iteraterec,Advanceanditerate);
	if (!AT_iterated)
	  AS_iterated = CPL_iterate(&AS, &AS_iteraterec,Advanceanditerate);
      }
    }
    still_iterating = (((lead_iterator == AS_accum) && AS_iterated) ||
		       ((lead_iterator == AT_accum) && AT_iterated) ||
		       ((lead_iterator == AD_accum) && AD_iterated));

    

    /* If we are executing JSRAT, we need to tell it when to conclude */
    /* JSR expansions. JSR expansion should happen when we iterated at least */
    /* once and now have finished iterating. */
    finish_JSRAT = (!still_iterating && at_least_one_iteration);

    /* terminate on second go round if not doing JSRAT but we're done */
    /* iterating. We HAVE to execute the opcode one extra time if this IS */
    /* a JSRAT since that adds the RTS on the JSRAT block (see cplops.c) */
    if (finish_JSRAT && (opcode_id != JSRAT))
      break;
				/* CALL THE OPCODE ROUTINE */
    (*(opcoderecs[opcode_id].opcoderoutine)) (world_list,state);

    /* Now the results from this one iteration must be added to the */
    /* final results place, if there is one.  If no iteration at all, */
    /* just copy the results to the final results place, if there is one. */
    if (results_word != Nil)
      if (lead_iterator == Noaccumulator)
	copy_wordA_to_wordB(&AR,results_word);
      else
	add_wordA_to_wordB(&AR,results_word);
/*    clear_word(&AR);*/

  } while (still_iterating);
}
    
  void
CPL_set_frame(CPL_word_ptr OP_ptr,CPL_word_ptr AS_ptr,CPL_word_ptr AT_ptr,
	      CPL_word_ptr AR_ptr,CPL_word_ptr AD_ptr,CPL_word_ptr NF_ptr)
{
  opcodekind opcode_id;
  CPL_word_ptr next_CPLword_addr;
  opcode_argkind reqd_opcode_args;
  ;
  opcode_id = OP_ptr->word_data.CPLptr->word_data.CPLopcode;

  reqd_opcode_args = opcoderecs[opcode_id].opcode_args;

  next_CPLword_addr = OP_ptr->word_data.CPLptr + 1;
  if (reqd_opcode_args & ASarg)	/* Get a source arg if req'd */
  {
    AS_ptr->word_data.CPLptr = next_CPLword_addr;
    next_CPLword_addr++;
  }
  else
    AS_ptr->word_data.CPLptr = Nil;

  if (reqd_opcode_args & ATarg)	/* Get a target arg if req'd */
  {
    AT_ptr->word_data.CPLptr = next_CPLword_addr;
    next_CPLword_addr++;
  }
  else
    AT_ptr->word_data.CPLptr = Nil;

  if (reqd_opcode_args & ARarg)	/* Get a result arg if req'd */
  {
    AR_ptr->word_data.CPLptr = next_CPLword_addr;
    next_CPLword_addr++;
  }
  else
    AR_ptr->word_data.CPLptr = Nil;
  
  if (reqd_opcode_args & ADarg)	/* Get a data arg if req'd */
  {
    AD_ptr->word_data.CPLptr = next_CPLword_addr;
    next_CPLword_addr++;
  }
  else
    AD_ptr->word_data.CPLptr = Nil;

  NF_ptr->word_data.CPLptr = next_CPLword_addr;
}


/* Note that the opcode that advances a frame calls this routine... how */
/* do we specify a frame as an argument to an opcode though? */

  void
CPL_advance_frame(CPL_word_ptr OP_ptr,CPL_word_ptr AS_ptr,CPL_word_ptr AT_ptr,
		  CPL_word_ptr AR_ptr,CPL_word_ptr AD_ptr,CPL_word_ptr NF_ptr)
{
  OP_ptr->word_data.CPLptr = NF_ptr->word_data.CPLptr;
  CPL_set_frame(OP_ptr,AS_ptr,AT_ptr,AR_ptr,AD_ptr,NF_ptr);
}


  void
CPL_parse(listptr world_list,stateptr state,stateptr oldstate)
{
  do
  {
    CPL_resolver(world_list,state,COP,CAS,CAT,CAR,CAD);
    process_updates(environment_world,state,oldstate,quickupdate_prop);
    process_updates(working_world,state,oldstate,quickupdate_prop);
    CPL_advance_frame(COP,CAS,CAT,CAR,CAD,CNF); /* advance current frame */
    if (reset_temp_message_block)
      message_block_reset();	/* reset message system after QMSG opcode! */
  }
  while (COP->word_data.CPLptr->word_data.CPLopcode != ENDFR);

  if (debugcpl)
    printf ("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");
  if (debugcplspool)
  {
    fprintf (cplwatchfile, "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");
    fflush(cplwatchfile);
  }
}


  void
toggle_cpldebug(void)
{
  if (debugcplspool)
    fclose(cplwatchfile);
  debugcplspool = !debugcplspool;
  if (debugcplspool)
    cplwatchfile = fopen("cplwatch.out","w");
}


  void
reset_CPL_frames(void)
{
  /* init current and examination frames */
  COP = COP_init;
  CAS = CAS_init;
  CAT = CAT_init;
  CAR = CAR_init;
  CAD = CAD_init;
  CNF = CNF_init;

  XOP = XOP_init;
  XAS = XAS_init;
  XAT = XAT_init;
  XAR = XAR_init;
  XAD = XAD_init;
  XNF = XNF_init;

  COP->word_data.CPLptr = COP_ptr_init;
  XOP->word_data.CPLptr = XOP_ptr_init;
}

  void
reset_CPL_RAM(void)
{
  next_free_RAM_word = CPL_RAM_init;
  /* initialize the first two words to ENDFR in case this time around */
  /* we are just processing messages; this allows the CPL ROM to process */
  /* messages without executing any useful RAM, as if the one event received */
  /* was actually and ENDFR. I do this to avoid having to muck with SETFR */
  /* which can set a frame to any place in memory; if i changed it to only */
  /* set a frame when the old frame does not point to an ENDFR opcode then */
  /* it would lose functionality. */
  setup_CPL_word(next_free_RAM_word,Opcode_data);
  next_free_RAM_word->word_data.CPLopcode = ENDFR;
  setup_CPL_word(next_free_RAM_word+1,Opcode_data);
  (next_free_RAM_word+1)->word_data.CPLopcode = ENDFR;
}

  void
start_CPL_parse(listptr world_list,stateptr state,stateptr oldstate)
{
  CPL_set_frame(COP,CAS,CAT,CAR,CAD,CNF);
  CPL_set_frame(XOP,XAS,XAT,XAR,XAD,XNF);
  CPL_parse(world_list,state,oldstate);
  /* When you're done reset the RAM */
  reset_CPL_RAM();
}

  void
restart_CPL_parse(listptr world_list,stateptr state,stateptr oldstate)
{
  reset_CPL_frames();
  start_CPL_parse(world_list, state,oldstate);
}

  void				/* declared to be type void, 12/17/01 */
init_CPL_frames(void)
{
  /* init current and examination frames */
  COP_init = find_symbol("COP");
  CAS_init = find_symbol("CAS");
  CAT_init = find_symbol("CAT");
  CAR_init = find_symbol("CAR");
  CAD_init = find_symbol("CAD");
  CNF_init = find_symbol("CNF");

  XOP_init = find_symbol("XOP");
  XAS_init = find_symbol("XAS");
  XAT_init = find_symbol("XAT");
  XAR_init = find_symbol("XAR");
  XAD_init = find_symbol("XAD");
  XNF_init = find_symbol("XNF");

  COP_ptr_init = find_symbol("ROM_start");
  XOP_ptr_init = find_symbol("RAM_start");
}

  void
init_CPL_RAM(void)
{
  CPL_RAM_init = find_symbol("RAM_start");
}

  void
clear_CPL_membloc(CPL_word bloc[],int blocsize)
{
  int count = 0;
  ;
  while (count < blocsize)
  {
    blankout_CPL_word(&(bloc[count]));
    count++;
  }
}

  void
init_CPL_language(void)
{
  FILE *dbgcontrolfile;
  ;
  next_free_CPL_word = &(system_codebloc[0]);
  clear_CPL_membloc(system_codebloc,Systemcodeblocsize);

/*  clear_CPL_membloc(CPL_stack, CPL_Stacksize);*/ /* no longer req'd */
  CPL_stack = create_CPL_stack();
  clear_CPL_membloc(CPL_controlstack,CPL_Controlstacksize);

  clear_CPL_membloc(JSRAT_codebloc,JSRATcodeblocsize);
  next_free_JSRAT_word = &(JSRAT_codebloc[0]);

  next_free_auxiliary_index = 0;
  while (next_free_auxiliary_index < Auxiliaryblocsize)
    CPL_aux_free_markers[next_free_auxiliary_index++] = TRUE;
  next_free_auxiliary_index = 0;

  read_CPLcode_into_rom("./cpl/rom.cpl");

  init_CPL_frames();
  init_CPL_RAM();
  
  reset_CPL_frames();
  reset_CPL_RAM();

  /* The first time make sure loop regs' type is initialized to Blank_word */
  /* so that the first call to clear_word in CPL_resolver() doesn't try to */
  /* free garbage. */
  AS.word_type = AT.word_type = AD.word_type = Blank_word;

  /* I put the cpnormal into the beginning of the ROM */
  cpnormal_address = find_symbol("CPnormal");

  read_CPL_propfiles();
  
  dbgcontrolfile = fopen("./rsc/cpldebug.rsc","r");
  // Boolean (== GLboolean) is an unsigned char -- LJE
  fscanf(dbgcontrolfile,"Debugging CPL:%hhu\nDebug spool to watch file:%hhu\n",
	 &debugcpl, &debugcplspool);
  fclose(dbgcontrolfile);

  if (debugcplspool)
    cplwatchfile = fopen("cplwatch.out","w");
}

