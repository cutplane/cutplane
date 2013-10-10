
/* FILE: cpl_cutplane.h */

#ifndef cpl_includes
#define cpl_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*  The Cutplane Language (CPL) Type Definitions for the Cutplane Editor  */
/*                                                                        */
/* Author: WAK                                      Date: August 19,1990  */
/* Version: 3                                                             */
/* Copyright (C) 1987,1988,1989,1990,1991 William Kessler and             */
/* Larry Edwards. All Rights Reserved.                                    */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <platform_gl.h>
// #include <device.h>

#include "geometry.h"
#include "topology.h"
#include "rotateobj.h"
#include "update.h"

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                           CONSTANT DEFINITIONS                       */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#define Maxopcodes 150
#define Maxmessagenames 1000
#define Maxmessagelength 50
#define Maxpropcodeblocsize 1000
#define MaxCroutines 1000
#define MaxCroutinename 50
#define Maxenvparts 25
#define Maxicons 25
#define Maxcodekinds 2

#define MaxCPLsymbols 2000
#define MaxCPLinelength 256
#define MaxCPLabelength 25
#define MaxCPLopcodelength 15
#define MaxCPLmessagelength 25
#define MaxCPLsymbolength 256
#define Maxproplength 25
#define MaxCPLconstantlength 256
#define MaxCPLenvpartlength 25
#define MaxCPLiconlength 25
#define MaxCPLsymbolsperinst 5
#define MaxCPLcodekindlength 25

#define CPL_Stacksize 50
#define Systemcodeblocsize 1000 /* no more than this # of CPL words/frame */
#define JSRATcodeblocsize 1000	/* no more than 1000 JSR's per prop. */
#define Auxiliaryblocsize 1000	/* no more than 1000 auxiliary words avail */

#define CPL_Controlstacksize 50	/* can't go deeper than 50 subroutine calls! */
#define CPL_Ownerstacksize CPL_Controlstacksize

#define CPL_Maxpriority 5	/* Order: Icons,Activetools,Picked, */
				/* Selectable, Environment*/


#define Setupiterateregsonly TRUE
#define Advanceanditerate FALSE

typedef enum
{
  /* these consts interrogate current_owner or a feature to get these larger */
  /* features at run time */
  Self,
  Subfeatures,
  Mothervfe,
  Motherevf,
  Motherfve,
  Mothershell,
  Motherobj,
  Motherworld,

  /* These constants refer to whether a built message gets queued at the */
  /* beginning or end of an object's message queue. */
  Prioritymsg,
  Noprioritymsg,
  
  /* this const is filled in as a pointer at cpl compile time  */
  Codeowner,

  /* These constants refer to fast or regular object translation. */
  Quicktranslate,
  Normaltranslate,
  
  /* These constants refer to boolean operations. */
  Boolunion,
  Boolsubtract,
  Boolintersect,
  
  /* Various predefined constants */
  Origin,
  X_axis,
  Y_axis,
  Z_axis,
  Imatrix,
  Pickboxsize,
  Rotateiconcentersize,

  /* These constants refer to strings stored globally and used by character */
  /* filtering when interpreting characters inputted from the keyboard. */
  Alphabet,
  Numeric,
  Alphanumeric,
  Asciiset,			/* lets it all through except ctrl chars */
  
  Maxconsts
} constant_kind;

#define CPLauxsize sizeof(union CPLauxunion)
#define CPLwordsize sizeof(CPL_word)

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                    OPCODE ARGUMENT CASE CONSTANTS                    */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

typedef enum
{
  Noargs = 0,
  ASarg = 1,
  ATarg = ASarg*2,
  ARarg = ATarg*2,
  ADarg = ARarg*2,
  MaxCPLargs
} opcode_argkind;


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                         CPL_WORD TYPE DEFINITIONS                    */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

typedef enum
{
  Blank_word = 0,		/* This word is unused (padding) */
  Opcode_data = 1,		/* This word contains an opcode. */
  Message_data,
  Environment_data,		/* This word indicates an environment part.*/
  Icon_data,			/* This word indicates an icon. */

  CPLptr_data,			/* Represents a pointer to another CPL word */
  CPLhandle_data,		/* Represents a ptr to a ptr to a CPL word */
  Auxiliary_data,		/* This word contains other immediate data, */
				/* e.g. rotate data.*/
  Featurelist_data,		/* This word points to a list of features. */
  Proplist_data			/* This word contains a list of properties. */
} CPLwordkind;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                          CPL ACCUMULATOR NAMES                       */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

typedef enum
{
  Noaccumulator = 0,
  OP_accum,			/* Action Opcode */
  AS_accum,			/* Action Source */
  AT_accum,			/* Action Target */
  AR_accum,			/* Action Result */
  AD_accum,			/* Action Data */

  A1_accum,A2_accum, A3_accum, A4_accum, A5_accum,
  D1_accum,D2_accum, D3_accum, D4_accum, D5_accum,

  Maxaccumulatorkind
} accumulatorkind;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                          ENVIRONMENT PART NAMES                      */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

typedef enum
{
  Nothingness = 0,
  Crosshair_part,
  Cutplane_part,
  Room_part,
  Universe_part
} environmentkind;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                               ICON NAMES                             */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


typedef enum
{
  Noicon = 0,
  Rotateicon,
  Planecontrasticon,
  Helpicon
} iconkind;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                        CODE BLOC KINDS DEFINITION                    */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

typedef enum
{
  Nocodekind = 0,
  Private_code,
  Property_code,
  Maxcodekind
} codekind;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                              OPCODE NAMES                            */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

typedef enum
{
  NOP = 0,

  CPLDATA,			/* Reserves space for one CPL word */
  CPLPTR,			/* Reserves space for a word with a pointer */
  CPLHANDLE,			/* Reserves space for a word with a handle */
  MOVE,				/* Moves a CPL word from one place to another*/
  ADD,				/* Adds two values. */
  CLEAR,			/* Clear CPL word (set word_type =Blank_word)*/
  PUSH,				/* Push an word on the approp stack */
  POP,				/* Pop an word off the approp stack */
  WIPE,				/* Set an instruction to non-executing.*/
  UNWIPE,			/* Set an instruction to executing. */

  AND,				/* Do a logical test between words */
  OR,				/* and put result in a third. Does set */
  NOT,				/* intersections. Word_type is Blank_word */
				/* if result is NIL */
  COUNT,			/* Count the number of elements in a set. */
  INDEX,			/* Extract the nth element of a set. */

  BEQUAL,			/* Test two words for (partial) (in)equality */
  BNEQUAL,			/* and jump to address in AD if (false/)true. */
  BEMPTY,			/* Test a set to see if full or empty and jump*/
  BNEMPTY,			/* to address in AD if empty/not empty. */
  BTRUE,			/* Test a word to see if true/false and jump */
  BFALSE,			/* to address in AD on result. */
  BWIPED,			/* Branch if an instruction is wiped. */

  OWNER,			/* Get the owner obj/caller obj of this code */
  SETOWN,			/* Set the owner obj/caller obj of this code */
  MOTHER,			/* Get the mother feature of type passed in AD*/
  ANCEST,			/* Put unlinked ancestor of this object in AR */
  DESCEND,			/* Get all descendents of this object into */
				/* a featurelist. */
  PARENT,			/* Get the object parent of a feature.  */
  CHLDRN,			/* Get the object children of a feature. */
  SIBLING,			/* Get all the brothers and sisters of this */
				/* object in a featurelist excluding this */
				/* object.*/
  MAKE,				/* Make a new, empty object. Parent in AT. */


  BMSGS,			/* Branch if object(s) in AS has/have messages*/
  BNOMSGS,			/* Branch if object(s) in AS have NO messages.*/
  MAKEMSG,			/* Add onto the current message string. */
  QMSG, 			/* Send message string to object(s) in AS. */
  UNQMSG,			/* Get front word from an object's msg queue  */
  CHKMSG,			/* Copy front word from msg queue undisturbed */

  JSR,				/* Jump to subroutine */
  JSRAT,			/* Jump to subroutine via feature private or */
				/* property code. */
  RTS,				/* Return from subroutine */
  JUMP,				/* Jump n actionwords */

  SETFR,			/* Set one frame pointer value */
  ENDFR,			/* Marker of end of this frame of CPL code */

  CRTN,				/* Used to call C routines directly */

  TRANS,
  ROTATE,
  SCALE,
  TRNSFRM,			/* Can accomplish above three but uses matrix */
  CONCAT,			/* Concatenate matrices in AS and AT giving AR*/
  DIFF3D,			/* compute diff vector between 2 points/featrs*/
  SNAP,				/* Snap a feature to a given pos. */
  WITHIN,			/* Whether one obj's bbox intersects another */

  GETNAME,			/* Get/Set an object's name or label */
  SETNAME,			/* (objects only). */
  GETLABEL,
  SETLABEL,

  GETCOLOR,			/* Get a feature's thecolor record */
  SETCOLOR,			/* Set a feature's thecolor record */

  AUX2STR,			/* Convert an aux word to a string aux word */
  STR2AUX,			/* Convert a string aux word to an auxword. */

  HASPROP,			/* Test a feature to see if it has property.*/
  GETPROP,			/* Get a property rec if possible. */
  ADDPROP,			/* If AD has Boolean TRUE, checks subfeatures */
  REMPROP,			/* of motherobj of feature as well. */

  SELECT,			/* These must be here to simulate */
  UNSEL,			/* user actions even if they COULD be */
  PICK,				/* duplicated by the proper ADDPROP and */
				/* REMPROP operations-- and some can't */
  RELEAS,			/* (e.g. RELEAS, UNPICK, DBLPIC). */
  UNPICK,
  DBLPIC,
  MPICK,			/* Multiple pick/release (ie shift key */
  MRELEAS,			/* held down), so no other objects get */
				/* unpicked. */
  DUP,
  DUPCPL,

  CPDELETE,			/* Delete a feature. */

  CPBOOLEAN,			/* Do a boolean op between AS and AT, */
				/* results in AR, which bool op spec'd in AD. */
  ADOPT,
  UNADOPT,
  GETLINK,			/* Get the link status of an object. */

  WTRANS,			/* Move objs to another world. */
  WORLD,			/* Select a new current world for CPL usage. */
  
  UPDATE,			/* Update something about specific items */
  GLUPDATE,			/* Set (a) global update flag(s). */
  CPSHADE,

  CONNECT,			/* Connect object in AS to object in AT. */
  GETCONN,			/* Get the object one object is connected to. */

  RECON,
  RECOFF,

  UPDATEON,
  UPDATEOFF,

  QUIT,				/* End using 3Form! */
  
  DEBUG,			/* Turn on/off CPL disassembly */

  /* Compiler directives */
  DEFPROP,			/* Define a new property. See properties.c */
  STARTPROP,			/* Start a new property's CPL code. */

  MAX_OPCODEKIND = Maxopcodes
} opcodekind;


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                   MESSAGE DATA TYPE DEFINITION                       */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

typedef long messagekind;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                   AUXILIARY DATA TYPE NAMES                          */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

typedef enum
{
  CPLaux_int_data = 1,
  CPLaux_long_data,
  CPLaux_char_data,
  CPLaux_string_data,
  CPLaux_float_data,
  CPLaux_offset_data,
  CPLaux_matrix_data,
  CPLaux_boolean_data,
  CPLaux_address_data,
  CPLaux_color_data,
  CPLaux_update_data,
  CPLaux_rotate_data,
  CPLaux_scale_data,
  CPLaux_duplicate_destroy_data,
  CPLaux_owner_feature_data,
  CPLaux_Croutine_index_data,
  CPLaux_update_time_data,
  CPLaux_cutplane_shade_data,
  CPLaux_codekind_value_data,
  CPLaux_new_world_data
} auxdatakind;
  
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                   AUXILIARY DATA STRUCTURES DEFINITION               */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


/* They are used as arguments to CPL opcodes. */

union CPLauxunion
{
  /* Numerical (Simple) Constants */
  int CPLintdata;
  long CPLlongdata;
  char CPLchardata;
  char *CPLstringdata;
  double CPLfloatdata;
  vertype CPLoffsetdata;
  Matrix CPLmatrixdata;
  Boolean CPLbooleandata;
  void *CPLaddressdata;
  colortype CPLcolordata;

  /* Values used by UPDATE and GLUPDATE opcodes. */
  updatetype update_data;
  
  /* Rotate data used by ROTATE opcode. */
  struct rotateinfostruct rotate_data;

  /* Scale data used by SCALE opcode. */
  struct
  {
    double scale_factor;
    bbox3dtype scale_bbox;
  } scale_data;

  /* Data used during duplication of features, of environment, or of CPL */
  /* code. */
  short duplicate_destroy; /* Can be 0, Destroysource || Destroytarget */

  /* Used for SETOWN statements. This is a pointer to the feature that */
  /* contains a bloc of code as private code */
  featureptr owner_feature;

  /* NEED TO HAVE something here for file source and target pointers */
  /* so things can be saved, loaded, etc. */

  /* Index into a C routinerec table. Used by the opcode CALL_C_ROUTINE */
  int Croutine_index;

  /* When opcode is SET_CP_SHADE this specifies the level of opacity. */
  short cutplane_shade;

  /* The type of code bloc, whether the code executes a property or */
  /* whether it is private to a feature. */
  codekind codekind_value;

  /* Pointer to a world you want to switch the context to. */
  worldptr new_world;
};

typedef union CPLauxunion CPLauxtype;
typedef CPLauxtype *CPLauxptr;


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                     CPL WORD STRUCTURE DEFINITION                    */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

struct CPL_word_struct
{
  /* The word_type identifies this word as immediate data or dereferencing */
  /* data. Dereferenced data can be a list of features, a list of props, */
  /* a pointer to another CPL word, or a handle (double indirect) */
  /* pointer to a CPL word.  Can also be set to Blank_word if NIL (used */
  /* after Logical tests,etc.*/
  CPLwordkind word_type;

  union
  {
    /* immediate data */
    opcodekind CPLopcode;
    messagekind CPLmessage;
    environmentkind CPLenvironment_part;
    iconkind CPLicon;
    union CPLauxunion CPLauxdata; /* Pointer to larger auxiliary data */
    /* dereferenced data */
    listptr CPLfeaturelist;	/* Refers to features by list of ep ptrs */
    listptr CPLproplist;	/* Refers to group(s) of features by prop */
    struct CPL_word_struct * CPLptr;	/* Single indirect pointer to memory */
    struct CPL_word_struct * CPLhandle;	/* Double indirect pointer to memory */
  } word_data;

  Boolean contents_of;	/* TRUE if the word is not be taken as simply */
				/* an address but should be resolved into */
				/* what it actually points to. */
  Boolean address_of;		/* TRUE if the word is to be taken as */
				/* containing an address that is not to be */
				/* resolved. */
  Boolean wiped;		/* If TRUE, this instruction will not execute */
  auxdatakind aux_kind;		/* if word_type == Auxiliary_data, tells type */
				/* of auxiliary data.*/
};

typedef struct CPL_word_struct CPL_word;
typedef CPL_word * CPL_word_ptr;

/* An accumulator holds one CPL word. */
typedef CPL_word accumulatortype;


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                PARSING LOOKUP STRUCTURES AND TABLES                  */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* Lookup record for an opcode, giving its identification number, its */
/* name (character string) and the number of arguments that follow it */
/* (in CPL words) */

struct opcoderecstruct
{
				/* The name of the opcode for text parse*/
  char opcodename[MaxCPLopcodelength]; 
  int opcode_args;		/* Which arguments get used with this opcode */
  void (*opcoderoutine)();
};

typedef struct opcoderecstruct opcoderec;


struct symbolrecstruct
{
  char symbol_name[Maxpropertynamelen];
  struct CPL_word_struct *symbol_address;
};

typedef struct symbolrecstruct symbolrec;

struct Croutinerecstruct
{
  char Croutine_name[MaxCroutinename];
  void (* croutineptr)();
};

typedef struct Croutinerecstruct Croutinerec;

struct iteratestruct
{
  elemptrptr looptr1,nextlooptr1,looptr2,nextlooptr2,loopreset1,loopreset2;
  featureptr (*element_lookup_fcn) ();
  Boolean (*iterate_advance_fcn) ();
  void (*iterate_reset_fcn) ();
  Boolean iteration_on;
};

typedef struct iteratestruct iteraterec;
typedef iteraterec *iterateptr;

#endif
