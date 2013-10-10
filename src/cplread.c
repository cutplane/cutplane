
/* FILE: cplread.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*  The Cutplane Language (CPL) Line Input Rtns for the Cutplane Editor   */
/*                                                                        */
/* Author: WAK                                       Date: August 28,1990 */
/* Version: 1                                                             */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990,1991 William Kessler and             */
/* Larry Edwards. All Rights Reserved.                                    */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define CPLREADMODULE

#ifdef IRIS
#include <sys/types.h>		/* this is for stat: UNIX ONLY!!! */
#include <sys/stat.h>		/* this is for stat: UNIX ONLY!!! */
#include <fcntl.h>		/* this is for open: UNIX ONLY!!! */
#include <unistd.h>		/* this is for read: UNIX ONLY!!! */
#else
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>		/* this is for open */
#include <time.h>
#include <sys/types.h>		/* this is for stat */
#include <sys/stat.h>		/* this is for stat */
#endif

#include <stdint.h>
#include <stdarg.h>

#include <config.h>
#include <stdio.h>
#include <stdlib.h>		/* added for Windows port */

#include <platform_gl.h>
#include <ctype.h>		/* for islower,toupper */
#include <string.h>

#include <cutplane.h>
#include <cpl_cutplane.h>
#include <update.h>		/* for updatetimelen, etc *>
#include <boolean.h>

#define debugging
#undef debugging

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	                       GLOBAL VARIABLES				     */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* List of sort records containing forward symbol definitions during reading. */
listptr CPL_forward_defs;

/* List of sort records containing forward object references during reading. */
listptr CPL_forward_object_refs;

char CPL_constnames[Maxconsts][MaxCPLconstantlength] = { "Self",
							 "Subfeatures",
							 "Mothervfe",
							 "Motherevf",
							 "Motherfve",
							 "Mothershell",
							 "Motherobj",
							 "Motherworld",
							 "Prioritymsg",
							 "Noprioritymsg",
							 "Codeowner",
							 "Quicktranslate",
							 "Normaltranslate",
							 "Boolunion",
							 "Boolsubtract",
							 "Boolintersect",
							 "Origin",
							 "x_axis",
							 "y_axis",
							 "z_axis",
							 "imatrix",
							 "Pickboxsize",
							 "Rotateiconcentersize",
							 "Alphabet",
							 "Numeric",
							 "Alphanumeric",
							 "Asciiset"};

/* Note the last two are NOT the ones in the enum but the actual values */
/* because the enum'd ones are just placeholders for actual values. */
constant_kind CPL_constvals[Maxconsts] = {Self,Subfeatures,
					  Mothervfe,Motherevf,Motherfve,
					  Mothershell, Motherobj, Motherworld,
					  Prioritymsg,Noprioritymsg,
					  Codeowner,
					  Quicktranslate,Normaltranslate,
					  Boolunion,Boolsubtract,Boolintersect,
					  Origin,X_axis,Y_axis,Z_axis,
					  Imatrix,Pickboxsize,
					  Rotateiconcentersize,
					  Alphabet,Numeric,
					  Alphanumeric,Asciiset};

char environmentpart_names[Maxenvparts][MaxCPLenvpartlength] =
{"Nothingness","Crosshair_part", "Cutplane_part",
 "Room_part", "Universe_part"};
								    
char icon_names[Maxicons][MaxCPLiconlength] = {"Noicon",
					       "Rotateicon",
					       "Planecontrasticon",
					       "Helpicon"};

char codekind_names[Maxcodekind][MaxCPLcodekindlength] = {"Nocodekind",
							  "Private_code",
							  "Property_code"};

int max_stored_message = 0;
char message_names[Maxmessagenames][MaxCPLmessagelength];

char update_names[Maxupdatenames]
[Maxupdateflaglength] = {"Noupdateflag",
			 "Transformflag",
			 "Evfeqnsflag",
			 "Cutevfsflag",
			 "Transformcutevfsflag",
			 "Clearcutsectionflag",
			 "Makecutsectionflag",
			 "Findnormalsflag",
			 "Computeplanedistsflag",
			 "Computebboxflag",
			 "Cleartranslatesflag",
			 "Getselectableflag",
			 "Getnearestptsflag",
			 "Recomputebackfaceflag",
			 "Findplanenearptsflag",
			 "Redrawflag",
			 "Logerasebboxflag",
			 "Allupdateflags"};

long updatenamevals[Maxupdatenames] = {Noupdateflag,
				       Transformflag,
				       Evfeqnsflag,
				       Cutevfsflag,
				       Transformcutevfsflag,
				       Clearcutsectionflag,
				       Makecutsectionflag,
				       Findnormalsflag,
				       Computeplanedistsflag,
				       Computebboxflag,
				       Cleartranslatesflag,
				       Getselectableflag,
				       Getnearestptsflag,
				       Recomputebackfaceflag,
				       Findplanenearptsflag,
				       Redrawflag,
				       Logerasebboxflag,
				       Allupdateflags};

FILE *parse_file;
						     
char current_property_name[Maxpropertynamelen];	/* set by STARTPROP */

int parsefile_linenum = 0;
char parse_filename[100];	/* for debugging */

/* Constants used for keyboard input filtering. */

char alphabetstr[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
char numericstr[] = "+-1234567890.e";
char alphanumericstr[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+-1234567890. ";
char asciisetstr[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+-1234567890 .,`~!@#$%^&*()=_[]{}\\|;:/?<>";

/* This is the temporary space that gets initially 20Kbytes allocated to store */
/* a CPL file so that they get parsed faster... It gets resized if req'd. */
  
char *temp_cpl_file_buffer;
int current_temp_cpl_char, max_read_temp_cpl_char;
int max_temp_cpl_char = 20000;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	                       SUPPORT ROUTINES				     */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

  void
init_CPL_global_lists(void)
{
  CPL_forward_defs = create_list();
  CPL_forward_object_refs = create_list();
  init_obj_charstream_list();
  temp_cpl_file_buffer = (char *) alloc_elem(max_temp_cpl_char * sizeof(char));
}

  void
transfer_into_temp_buffer(char cpl_file_name[])
{
  struct stat cpl_statbuf;
  int cpl_filedes;
  int result,i;
  ;
  fprintf (dbgdump, "Reading CPL file:%s\n",cpl_file_name);

#ifdef IRIS
  cpl_filedes = open(cpl_file_name, O_RDONLY);
  fstat(cpl_filedes, &cpl_statbuf);
#else
#ifdef WIN32
  cpl_filedes = open(cpl_file_name, O_RDONLY | O_BINARY);
#else
  cpl_filedes = open(cpl_file_name, O_RDONLY);
#endif
  result = fstat(cpl_filedes, &cpl_statbuf);
  fprintf(dbgdump, "fstat result code %d\n", result);
#endif /* IRIS */

  max_read_temp_cpl_char = cpl_statbuf.st_size;  
  fprintf(dbgdump, "size we think we're reading is: %ld\n", cpl_statbuf.st_size);

  fprintf(dbgdump, "Time modified : %s\n", ctime( &(cpl_statbuf.st_ctime) ));
   
  if (max_temp_cpl_char < max_read_temp_cpl_char)
  {
    temp_cpl_file_buffer = (char *) realloc(temp_cpl_file_buffer,
					    max_read_temp_cpl_char);
    max_temp_cpl_char = max_read_temp_cpl_char;
  }

  fprintf(dbgdump, "Trying to read: %ld chars to buffer ptr %p\n", max_read_temp_cpl_char, (void *) temp_cpl_file_buffer);

#ifdef WIN32
  if ((result = _read(cpl_filedes,temp_cpl_file_buffer,max_read_temp_cpl_char))
      <= 0)
#else
  if ((result = read(cpl_filedes, temp_cpl_file_buffer, max_read_temp_cpl_char))
      <= 0)
#endif
    fprintf(dbgdump, "Error code from _read!: %d\n", result);
  else
    fprintf(dbgdump, "We think we read: %d chars\n", result);

  if (result < max_read_temp_cpl_char) {
    /* I don't know why, but what _fstat reports */
    /* on windows is larger than the actual */
    /* number of characters in the goddam file!!! */
    max_read_temp_cpl_char = result;
  }
  current_temp_cpl_char = 0;	/* reset for get_cpl_line, below */

  close(cpl_filedes);
  
#ifdef IRIS
  for (i = 0; i < result; ++i) {
    fprintf (dbgdump, "%c", temp_cpl_file_buffer[i]);
  }
#endif

  fprintf (dbgdump, "got here\n");
  fflush(dbgdump);
}

  void
write_CPLparsebug_notice(void)
{
  printf ("Error in file %s, line %d:\n",parse_filename,parsefile_linenum+1);
}

  void
convert_to_lower(char *lowerstr)
{
  int i = 0;
  ;
  while (i < strlen(lowerstr))
  {
    if (isupper(lowerstr[i]))
      lowerstr[i] = tolower(lowerstr[i]);
    i++;
  }
}

  void
convert_to_upper(char *lowerstr)
{
  int i = 0;
  ;
  while (i < strlen(lowerstr))
  {
    if (islower(lowerstr[i]))
      lowerstr[i] = toupper(lowerstr[i]);
    i++;
  }
}

/* Copies blocsize cplwords from bloc1 to bloc2. Make sure there's enough */
/* room in bloc2 before calling this routine. */
  void
copy_CPL_bloc(CPL_word_ptr bloc1,CPL_word_ptr bloc2,int blocsize)
{
  CPL_word_ptr copyword;
  int count;
  ;
  copyword = bloc1;
  count = 0;
  while (count < blocsize)
  {
    *(bloc2+count) = *(bloc1+count);
    count++;
  }
}

  CPL_word_ptr
find_symbol(char *symbolstr)
{
  int nameindex = 0;
  ;
  /* first look in local symbols, if they are currently active (reading). */
  if (top_local_symbol_record > 0)
  {
    while ((strcmp(symbolstr,
		   local_symbol_records[nameindex].symbol_name) != 0) &&
	   (nameindex < top_local_symbol_record))
      nameindex++;
    if (nameindex < top_local_symbol_record)
      return(local_symbol_records[nameindex].symbol_address);
  }
  nameindex = 0;
  while ((strcmp(symbolstr,symbol_records[nameindex].symbol_name) != 0) &&
	 (nameindex < top_symbol_record))
    nameindex++;
  if (nameindex < top_symbol_record)
    return(symbol_records[nameindex].symbol_address);
  return(Nil);
}

  Boolean
lookup_symbol_from_address(CPL_word_ptr lookup_address,char *symbolstr)
{
  int nameindex = 0;
  ;
  symbolstr[0] = '\0';
  while ((symbol_records[nameindex].symbol_address != lookup_address) &&
	 (nameindex < top_symbol_record))
    nameindex++;
  if (nameindex < top_symbol_record)
  {
    strcpy(symbolstr,symbol_records[nameindex].symbol_name);
    return (TRUE);
  }
  else
  {
    nameindex = 0;
    while ((total_local_symbol_records[nameindex].symbol_address !=
	    lookup_address) && (nameindex < top_total_local_symbol_record))
      nameindex++;
    if (nameindex < top_total_local_symbol_record)
    {
      strcpy(symbolstr,total_local_symbol_records[nameindex].symbol_name);
      return(TRUE);
    }
  }
  return(FALSE);
}

  objptr
locate_obj(objptr searchobj,char *object_name)
{
  objptr thisobj,foundobj;
  ;
  if (searchobj->obj_name != Nil)
    if (strcmp(object_name,searchobj->obj_name) == 0)
      return(searchobj);

  thisobj = searchobj->children->first.obj;
  while (thisobj != Nil)
    if ((foundobj = locate_obj(thisobj,object_name)) != Nil)
      return(foundobj);
    else
      thisobj = thisobj->next;
  return(Nil);
}
  
  objptr
find_object_in_any_world(char *object_name)
{
  worldptr thisworld;
  objptr foundobj;
  ;		       /* using world_list as global even tho not passed in */
  thisworld = world_list->first.world;
  while (thisworld != Nil)
  {
    if ((foundobj = locate_obj(thisworld->root,object_name)) != Nil)
      return(foundobj);
    thisworld = thisworld->next;
  }
  return(Nil);
}

  worldptr
find_world(char *world_name)
{
  worldptr searchworld;
  ;
  searchworld = world_list->first.world;
  while (searchworld != Nil)
  {
    if (searchworld->world_name != Nil)
      if (strcmp(searchworld->world_name,world_name) == 0)
	return(searchworld);
    searchworld = searchworld->next;
  }
  return(Nil);
} 

  long
find_update(char *update_name)
{
  int cnt = 0;
  ;
  while ((strcmp(update_names[cnt], update_name) != 0) && (cnt < Maxupdatenames))
    cnt++;
  if (cnt == Maxupdatenames)
    return(Noupdateflag);			/* name not found! */
  return(updatenamevals[cnt]);
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	                 COMPILING CORE ROUTINES		             */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */


#if 0
  Boolean
get_cpl_line(FILE *cplfile,char cpl_line[])
{
  char c;
  int count = 0,check = 0;
  int scanstatus;
  ;
  scanstatus = fscanf(cplfile,"%c",&c);
  while ((scanstatus != EOF) && (scanstatus != 0))
  {
    cpl_line[count++] = c;
    if (c == '\n')
      break;
    scanstatus = fscanf(cplfile,"%c",&c);
  }
  if (count == 0)
    cpl_line[0] = '\n';
  else
  {
    /* prescan what portion of the line we have so far... if it is all white */
    /* space, then set this up as a true blank line, ie \n at beginning */
    while (((cpl_line[check] == ' ') || (cpl_line[check] == '\t')) &&
	   (check < count))
      check++;
    if (check == count-1)
      cpl_line[0] = '\n';
  }
  return((scanstatus != EOF) && (scanstatus != 0));
}
#endif

#undef DEBUGCPLREAD

  Boolean
get_cpl_line(char cpl_line[])
{
  char c;
  int i = 0,check = 0;
  Boolean returnval = TRUE;
  ;

#ifdef DEBUGCPLREAD
  fprintf (dbgdump, "get_cpl_line:");
  fflush(dbgdump);
#endif
  do
  {
    c = cpl_line[i++] = temp_cpl_file_buffer[current_temp_cpl_char++];
#ifdef DEBUGCPLREAD
    fprintf (dbgdump, "%c", c);
    fflush(dbgdump);
#endif
    if (current_temp_cpl_char > max_read_temp_cpl_char)
    {
      returnval = FALSE;
      break;
    }
  }
  while (c != '\n');

#ifdef DEBUGCPLREAD
  fprintf (dbgdump, "\n");
  fflush(dbgdump);
#endif

  /* prescan what portion of the line we have so far... if it is all white */
  /* space, then set this up as a true blank line, ie \n at beginning */
  while (((cpl_line[check] == ' ') || (cpl_line[check] == '\t')) &&
	 (check < i))
    check++;
  if (check == i-1)
    cpl_line[0] = '\n';
  
  return(returnval);
}

  int
read_CPL_string(char cpl_line[],int point,int maxlen,char readstr[])
{
  int count = 0;
  ;
  /* skip past tabs, spaces, and commas to get to first char in readstr */
  while ((cpl_line[point] == '\t') || (cpl_line[point] == ' ') ||
	 (cpl_line[point] == ','))
    point++;

#ifdef IRIS
  if ((cpl_line[point] == ';') || (cpl_line[point] == '\n') ||
      (cpl_line[point] == '\0'))
    return(-1);
#else
  if ((cpl_line[point] == ';') || (cpl_line[point] == '\n') ||
      (cpl_line[point] == '\0') || (cpl_line[point] == '\r'))
    return(-1);
#endif

#ifdef IRIS
  while ((cpl_line[point] != '\t') && (cpl_line[point] != ' ') &&
	 (cpl_line[point] != ',') && (cpl_line[point] != ';') &&
	 (cpl_line[point] != '\n') && (cpl_line[point] != '\0'))
#else
    /* This is the version for pc's, since they */
    /* use CRLF's and unix doesn't. */
  while ((cpl_line[point] != '\t') && (cpl_line[point] != ' ') &&
	 (cpl_line[point] != ',') && (cpl_line[point] != ';') &&
	 (cpl_line[point] != '\n') && (cpl_line[point] != '\0') &&
	 (cpl_line[point] != '\r'))
#endif
  {
    readstr[count++] = cpl_line[point++];
    if (count == maxlen)
      return(point);
    /* if beginning of blockstream, must read til end */
    if (cpl_line[point] == '[')
    {
      do
      {
	readstr[count++] = cpl_line[point++];
	if (count == maxlen)
	  return(point);
      }
      while (cpl_line[point] != ']');
      readstr[count++] = cpl_line[point++];
    }
  }
  readstr[count] = '\0';
  return(point);
}

  void
add_symbolrec(char *labelstr,CPL_word_ptr load_word)
{
  if (labelstr[0] == '_')
  {				/* this is a local label only */
    strcpy(local_symbol_records[top_local_symbol_record].symbol_name,
	   labelstr);
    local_symbol_records[top_local_symbol_record].symbol_address = load_word;
    top_local_symbol_record++;
    strcpy(total_local_symbol_records[top_total_local_symbol_record].symbol_name,
	   labelstr);
    total_local_symbol_records[top_total_local_symbol_record].symbol_address =
      load_word;
    top_total_local_symbol_record++;

  }
  else
  {
    strcpy(symbol_records[top_symbol_record].symbol_name,labelstr);
    symbol_records[top_symbol_record].symbol_address = load_word;
    top_symbol_record++;
  }
}
  
/* NOTE: if you ran into a word reservation opcode you must actually */
/* initialize the CPLDATA words' auxdata pointers!!!! */

  opcodekind
lookup_CPL_opcode(char opcodestr[])
{
  opcodekind searchopcode = NOP;
  char upcase_opcodestr[MaxCPLopcodelength];
  int i;
  ;
  for (i = 0; i < strlen(opcodestr); ++i)
    if (islower(opcodestr[i]))
      upcase_opcodestr[i] = toupper(opcodestr[i]);
    else
      upcase_opcodestr[i] = opcodestr[i];
  
  upcase_opcodestr[i] = '\0';
  
  while ((strcmp(opcoderecs[(int) searchopcode].opcodename,
		 upcase_opcodestr) != 0) && (searchopcode < Maxopcodes))
    searchopcode++;

  if (searchopcode >= Maxopcodes)
  {
    fprintf(dbgdump, "lookup_CPL_opcode: opcode %s unknown!\n", opcodestr);
    fflush(dbgdump);
#if 0
    write_CPLparsebug_notice();
    printf("CPL Opcode %s unknown!\n",opcodestr);
    system_error ("lookup_CPL_opcode.");
#endif
  }
  return(searchopcode);
}

  CPL_word_ptr
encode_CPL_opcode(char labelstr[],opcodekind encode_opcode,
		  CPL_word_ptr load_word, Boolean *directive,
		  Boolean *noargs)
{
  if (encode_opcode == CPLDATA)	/* reservation word. */
    setup_CPL_auxword(load_word,CPLaux_int_data); 
  else if (encode_opcode == CPLPTR) /* reservation word. */
    setup_CPL_word(load_word,CPLptr_data);
  else if (encode_opcode == CPLHANDLE) /* reservation word. */
    setup_CPL_word(load_word,CPLhandle_data);
  else if (encode_opcode == DEFPROP)
  {				/* new prop defs take up no space and have */
				/* no args. They also have no CPL code, hence */
				/* the Nil. */
    if (find_property(labelstr) == -1) /* property not already defined */
      define_new_property(labelstr,Nil);
    *noargs = *directive = TRUE;
    return(load_word);
  }
  else if (encode_opcode == STARTPROP)
  {				/* prop def code: attach a def'd prop to this */
				/* code. */
    define_property(labelstr,load_word);
    strcpy(current_property_name,labelstr);
    *noargs = TRUE;
    return(load_word);
  }
  else
  {
    load_word->word_type = Opcode_data;
    load_word->word_data.CPLopcode = encode_opcode;
    *noargs = (opcoderecs[(int) encode_opcode].opcode_args == 0);
  }
  
  load_word->contents_of = load_word->address_of = FALSE;
#ifdef debugging
  fprintf (parse_file,"%s\t",opcoderecs[(int) encode_opcode].opcodename);
#endif

  return(load_word+1);
}

  constant_kind
lookup_CPL_const(char conststr[])
{
  constant_kind cnt;
  ;
  cnt = Self;
  while ((strcmp(CPL_constnames[cnt],conststr) != 0) && (cnt < Maxconsts))
    cnt++;
  return(cnt);
}

  void
encode_const_word(char symbolnamestr[],CPL_word_ptr load_word,
		  featureptr known_owner)
{
  constant_kind cnt;
  union anydata newval;
  vertype new_pos;
  Matrix new_matrix;
  char inchar;
  int hex_inchar;
  static char world_name[Maxworldnamelength],inchars[Maxpropertynamelen];
  colortype newcolor;
  Boolean updatetime;
  static char updateflagstr[Maxupdateflaglength];
  static char whenupdatestr[Maxupdatetimelength];
  int i,j;
  ;
  load_word->word_type = Auxiliary_data;
  cnt = lookup_CPL_const(symbolnamestr);
  if (cnt < Maxconsts)
  {
    switch(cnt)
    {
    case Prioritymsg:
      load_word->word_data.CPLauxdata.CPLbooleandata = TRUE;
      load_word->aux_kind = CPLaux_boolean_data;
      break;
    case Noprioritymsg:
      load_word->word_data.CPLauxdata.CPLbooleandata = FALSE;
      load_word->aux_kind = CPLaux_boolean_data;
      break;
    case Quicktranslate:
      load_word->word_data.CPLauxdata.CPLbooleandata = TRUE;
      load_word->aux_kind = CPLaux_boolean_data;
      break;
    case Normaltranslate:
      load_word->word_data.CPLauxdata.CPLbooleandata = FALSE;
      load_word->aux_kind = CPLaux_boolean_data;
      break;
    case Boolunion:
      load_word->word_data.CPLauxdata.CPLintdata = BOOL_Union;
      load_word->aux_kind = CPLaux_int_data;
      break;
    case Boolsubtract:
      load_word->word_data.CPLauxdata.CPLintdata = BOOL_Subtraction;
      load_word->aux_kind = CPLaux_int_data;
      break;
    case Boolintersect:
      load_word->word_data.CPLauxdata.CPLintdata = BOOL_Intersection;
      load_word->aux_kind = CPLaux_int_data;
      break;
    case Codeowner:
      load_word->word_data.CPLauxdata.owner_feature = known_owner;
      load_word->aux_kind = CPLaux_owner_feature_data;
      break;
    case Self:
      load_word->word_data.CPLauxdata.CPLintdata = -1;
      load_word->aux_kind = CPLaux_int_data;
      break;
    case Subfeatures:
      load_word->word_data.CPLauxdata.CPLintdata = Subfeatures;
      load_word->aux_kind = CPLaux_int_data;
      break;
    case Mothervfe:
      load_word->word_data.CPLauxdata.CPLintdata = Vfe_type;
      load_word->aux_kind = CPLaux_int_data;
      break;
    case Motherevf:
      load_word->word_data.CPLauxdata.CPLintdata = Evf_type;
      load_word->aux_kind = CPLaux_int_data;
      break;
    case Motherfve:
      load_word->word_data.CPLauxdata.CPLintdata = Fve_type;
      load_word->aux_kind = CPLaux_int_data;
      break;
    case Mothershell:
      load_word->word_data.CPLauxdata.CPLintdata = Shell_type;
      load_word->aux_kind = CPLaux_int_data;
      break;
    case Motherobj:
      load_word->word_data.CPLauxdata.CPLintdata = Obj_type;
      load_word->aux_kind = CPLaux_int_data;
      break;
    case Motherworld:
      load_word->word_data.CPLauxdata.CPLintdata = World_type;
      load_word->aux_kind = CPLaux_int_data;
      break;
    case Origin:
      copypos3d(origin,load_word->word_data.CPLauxdata.CPLoffsetdata);
      load_word->aux_kind = CPLaux_offset_data;
      break;
    case X_axis:
      copypos3d(x_axis,load_word->word_data.CPLauxdata.CPLoffsetdata);
      load_word->aux_kind = CPLaux_offset_data;
      break;
    case Y_axis:
      copypos3d(y_axis,load_word->word_data.CPLauxdata.CPLoffsetdata);
      load_word->aux_kind = CPLaux_offset_data;
      break;
    case Z_axis:
      copypos3d(z_axis,load_word->word_data.CPLauxdata.CPLoffsetdata);
      load_word->aux_kind = CPLaux_offset_data;
      break;
    case Imatrix:
      copy_matrix(imatrix,load_word->word_data.CPLauxdata.CPLmatrixdata);
      load_word->aux_kind = CPLaux_matrix_data;
      break;
    case Pickboxsize:
      load_word->word_data.CPLauxdata.CPLfloatdata = Pkboxsize;
      load_word->aux_kind = CPLaux_float_data;
      break;
    case Rotateiconcentersize:
      load_word->word_data.CPLauxdata.CPLfloatdata = Rotateiconctrsize;
      load_word->aux_kind = CPLaux_float_data;
      break;
    case Alphabet:
      load_word->word_data.CPLauxdata.CPLstringdata = (char *)
	&(alphabetstr[0]);
      load_word->aux_kind = CPLaux_string_data;
      break;
    case Numeric:
      load_word->word_data.CPLauxdata.CPLstringdata = (char *)
	&(numericstr[0]);
      load_word->aux_kind = CPLaux_string_data;
      break;
    case Alphanumeric:
      load_word->word_data.CPLauxdata.CPLstringdata = (char *)
	&(alphanumericstr[0]);
      load_word->aux_kind = CPLaux_string_data;
      break;
    case Asciiset:
      load_word->word_data.CPLauxdata.CPLstringdata = (char *)
	&(asciisetstr[0]);
      load_word->aux_kind = CPLaux_string_data;
      break;
    }      
  }
  else if (strcmp("TRUE",symbolnamestr) == 0)
  {
    load_word->word_data.CPLauxdata.CPLbooleandata = TRUE;
    load_word->aux_kind = CPLaux_boolean_data;
  }
  else if (strcmp("FALSE",symbolnamestr) == 0)
  {
    load_word->word_data.CPLauxdata.CPLbooleandata = FALSE;
    load_word->aux_kind = CPLaux_boolean_data;
  }
  else
  {
    switch(symbolnamestr[0])
    {
    case 'A':			/* ascii char. NOTE: no spaces allowed! */
      /* Added 2/17/02 to better support special ascii chars, such as Escape */
      /* If we have a long string in this spot, it means it's in the format */
      /* 0xYY, which is a hex representation of the intended ascii value. */
      if (strlen(symbolnamestr) > 2) {
	sscanf(&(symbolnamestr[3]),"%x",&hex_inchar);
	inchar = (char) hex_inchar;
      } else {
	sscanf(&(symbolnamestr[1]),"%c",&inchar);
      }
      load_word->word_data.CPLauxdata.CPLchardata = inchar;
      load_word->aux_kind = CPLaux_char_data;
      break;      
    case 'T':			/* strings. NOTE: no spaces allowed! */
      sscanf(&(symbolnamestr[1]),"[%s]",inchars);
      load_word->aux_kind = CPLaux_string_data;
      if (strlen(inchars) == 0)	/* passed [^@], ie the null string */
	load_word->word_data.CPLauxdata.CPLstringdata = Nil;
      else
      {
	load_word->word_data.CPLauxdata.CPLstringdata = (char *)
	  alloc_elem(strlen(inchars));
	strcpy(load_word->word_data.CPLauxdata.CPLstringdata,inchars);
      }
      break;      
    case 'I':			/* integer constants */
      sscanf(&(symbolnamestr[1]),"%d",&(newval.i));
      load_word->word_data.CPLauxdata.CPLintdata = newval.i;
      load_word->aux_kind = CPLaux_int_data;
      break;
    case 'D':			/* double constants */
      sscanf(&(symbolnamestr[1]),"%lf",&(newval.d));
      load_word->word_data.CPLauxdata.CPLfloatdata = newval.d;
      load_word->aux_kind = CPLaux_float_data;
      break;
    case 'V':			/* vertype constants */
      sscanf(&(symbolnamestr[1]),"[%f,%f,%f]",&(new_pos[vx]),
	     &(new_pos[vy]),&(new_pos[vz]));
      copypos3d(new_pos,load_word->word_data.CPLauxdata.CPLoffsetdata);
      load_word->aux_kind = CPLaux_offset_data;
      break;
    case 'M':			/* Matrix constants */
      sscanf(&(symbolnamestr[1]),"[{%f,%f,%f,%f}, {%f,%f,%f,%f}, {%f,%f,%f,%f}, {%f,%f,%f,%f}]",
	     &(new_matrix[0][vx]), &(new_matrix[0][vy]),
	     &(new_matrix[0][vz]), &(new_matrix[0][vw]),
	     &(new_matrix[1][vx]), &(new_matrix[1][vy]),
	     &(new_matrix[1][vz]), &(new_matrix[1][vw]),
	     &(new_matrix[2][vx]), &(new_matrix[2][vy]),
	     &(new_matrix[2][vz]), &(new_matrix[2][vw]),
	     &(new_matrix[3][vx]), &(new_matrix[3][vy]),
	     &(new_matrix[3][vz]), &(new_matrix[3][vw]));
      copy_matrix(new_matrix,load_word->word_data.CPLauxdata.CPLmatrixdata);
      load_word->aux_kind = CPLaux_matrix_data;
      break;
    case 'P':			/* pointer constants */
      sscanf(&(symbolnamestr[1]),"%p", &(newval.ptr)); // Use %p to avoid pointer size issues -- LJE
      load_word->word_data.CPLauxdata.CPLaddressdata = (void *) newval.ptr;
      load_word->aux_kind = CPLaux_address_data;
      break;
    case 'S':			/* scaling constants */
      // The field newval.f is a float, so we need %f not %lf -- LJE
      sscanf(&(symbolnamestr[1]),"%f",&(newval.f));
      load_word->word_data.CPLauxdata.scale_data.scale_factor =
	newval.f;
      load_word->aux_kind = CPLaux_scale_data;
      break;
    case 'R':			/* rotate data */
      sscanf(&(symbolnamestr[1]),"vec:[%f,%f,%f]/pt:[%f,%f,%f]/ang:%d",
	     &(load_word->word_data.CPLauxdata.rotate_data.rotvec[vx]),
	     &(load_word->word_data.CPLauxdata.rotate_data.rotvec[vy]),
	     &(load_word->word_data.CPLauxdata.rotate_data.rotvec[vz]),
	     &(load_word->word_data.CPLauxdata.rotate_data.rotpt[vx]),
	     &(load_word->word_data.CPLauxdata.rotate_data.rotpt[vy]),
	     &(load_word->word_data.CPLauxdata.rotate_data.rotpt[vz]),
	     &(load_word->word_data.CPLauxdata.rotate_data.rotangle));
      load_word->aux_kind = CPLaux_rotate_data;
      break;
    case 'W':			/* world constants */
      sscanf(&(symbolnamestr[1]),"%[A-z_]",world_name);
      load_word->word_data.CPLauxdata.new_world = find_world(world_name);
      load_word->aux_kind = CPLaux_new_world_data;
      if (load_word->word_data.CPLauxdata.new_world == Nil)
      {
	write_CPLparsebug_notice();
	system_error("encode_const_word: unknown world name!\n");
      }
      break;
    case 'C':			/* color constants */
      sscanf(&(symbolnamestr[1]),"[%hd,%hd,%hd]",&(newcolor[0]),&(newcolor[1]),
	     &(newcolor[2]));
      copycolor(newcolor,load_word->word_data.CPLauxdata.CPLcolordata);
      load_word->aux_kind = CPLaux_color_data;
      break;      
    case 'U':			/* update data */
      i = 1;			/* skip over "U" */
      j = 0;
      while ((symbolnamestr[i] != '/') &&
	     (symbolnamestr[i] != '\0')) /*'/' separates flag name and "when:" */
	updateflagstr[j++] = symbolnamestr[i++];
      /* Serious ugliness here, if user accidentally forgets "when:" phrase, */
      /* very bad shit comes down.... gotta check that here... */
      if (symbolnamestr[i] == '\0')  /* found termination prematurely, no */
      {				     /* "when" phrase found. */
	write_CPLparsebug_notice();
	system_error("encode_const_word: UPDATE constant requires When!\n");
      }
      updateflagstr[j] = '\0';
      i += 6;			/* skip over "when:" */
      j = 0;
      while (symbolnamestr[i] != '\0')
	whenupdatestr[j++] = symbolnamestr[i++];
      if ((strcmp(whenupdatestr,"Immediateupdate") == 0) ||
	  (strcmp(whenupdatestr,"Now") == 0))
	updatetime = Immediateupdate;
      else
	updatetime = Nonimmediateupdate;
      load_word->word_data.CPLauxdata.update_data.update_flags =
	find_update(updateflagstr);
      load_word->word_data.CPLauxdata.update_data.update_now =
	updatetime;
      load_word->aux_kind = CPLaux_update_data;
      break;
    }
  }
}

  void
store_forward_def(CPL_word_ptr load_word,char symbolnamestr[])
{
  sortptr new_forward;
  ;
  if (load_word->word_data.CPLptr == Nil) /* symbol not found: assume */
  {				/* forward definition and record it */
    new_forward = (sortptr) append_new_blank_elem(CPL_forward_defs,Sort_type);
    new_forward->sortelem = (elemptr) load_word;
    new_forward->sortdata1 = (alltypes *) alloc_elem(Allsize);
    new_forward->sortdata1->ptr = (char *) alloc_elem(strlen(symbolnamestr)+1);
    strcpy(new_forward->sortdata1->ptr,symbolnamestr);
  }
}

/* store a reference to an object that does not exist yet.  After */
/* reading the file (or all files if we're doing propfiles) we will resolve */
/* the Nil pointers again. */

  void
store_forward_object_name_ref(CPL_word_ptr load_word,char objectnamestr[])
{
  sortptr new_forward;
  ;
  if (load_word->word_data.CPLfeaturelist->first.ep->thiselem == Nil)
				/* object wasn't found: assume */
  {				/* forward definition and record it */
    new_forward = (sortptr) append_new_blank_elem(CPL_forward_object_refs,
						  Sort_type);
    new_forward->sortelem = (elemptr) load_word;
    new_forward->sortdata1 = (alltypes *) alloc_elem(Allsize);
    new_forward->sortdata1->ptr = (char *) alloc_elem(strlen(objectnamestr)+1);
    strcpy(new_forward->sortdata1->ptr,objectnamestr);
  }
}
 
  
/* Lookup a message in the message lookup table. If that message is unknown, */
/* then if cpldebug is TRUE print a notification to the user that a new */
/* message is being defined.  Then, store the new message and return */
/* its index. */

  messagekind
lookup_CPL_message(char messagestr[])
{
  messagekind searchmessage = 0;
  ;
  while ((strcmp(message_names[searchmessage],messagestr) != 0) &&
	 (searchmessage < max_stored_message))
    searchmessage++;
  if (searchmessage >= Maxmessagenames)
  {
    write_CPLparsebug_notice();
    printf("CPL Message %s unknown and no room left for new ones!\n",messagestr);
    system_error ("lookup_CPL_message.");
  }
  else if (searchmessage >= max_stored_message)
  {
#ifdef debug
    if (debugcpl)
    {
      printf("Defining new message:%s\n",messagestr);
      printf ("In file %s, line %d:\n",parse_filename,parsefile_linenum);
    }
#endif    
    strcpy(message_names[max_stored_message],messagestr);
    max_stored_message++;
    return((messagekind) max_stored_message-1);
  }
  return(searchmessage);
}
  
  void
encode_CPL_symbol(char symbolstr[],CPL_word_ptr load_word,
		  featureptr known_owner)
{
  int colonpos = 0, cnt = 0;
  char symboltypestr[MaxCPLsymbolength], symbolnamestr[MaxCPLsymbolength];
  char *bogus_label;
  Boolean bogus_noargs;
  objptr named_object;
  opcodekind opcode_symbol;
  messagekind message_symbol;
  ;
  while ((symbolstr[colonpos] != ':') && (symbolstr[colonpos] != '\0'))
    symboltypestr[cnt++] = symbolstr[colonpos++];
  colonpos++;
  symboltypestr[cnt] = '\0';
  cnt = 0;
  while (symbolstr[colonpos] != '\0')
    symbolnamestr[cnt++] = symbolstr[colonpos++];
  symbolnamestr[cnt] = '\0';

  /* Check for indirection on this symbol */
  if (symbolstr[colonpos-1] == '*')
  {
    load_word->contents_of = TRUE;
    symbolnamestr[cnt-1] = '\0';
#ifdef debugging
    fprintf (parse_file,"%s:%s(),",symboltypestr,symbolnamestr);
#endif
  }
  else if (symbolstr[colonpos-1] == '&')
  {
    load_word->address_of = TRUE;
    symbolnamestr[cnt-1] = '\0';
#ifdef debugging
    fprintf (parse_file,"%s:%s(),",symboltypestr,symbolnamestr);
#endif
  }
  else
  {
    load_word->contents_of = load_word->address_of = FALSE;
#ifdef debugging
    fprintf (parse_file,"%s:%s,",symboltypestr,symbolnamestr);
#endif
  }
    
  /* Now you must recognize the symbol's type identifier, and then lookup */
  /* the symbol unless it is one of the predefined types (e.g. an icon) */
  if (strcmp("Opcd",symboltypestr) == 0)
  {				/* symbol is an opcode */
    opcode_symbol = lookup_CPL_opcode(symbolnamestr);
    load_word->word_type = Opcode_data;
    load_word->word_data.CPLopcode = opcode_symbol;
  }
  else if (strcmp("Msg",symboltypestr) == 0)
  {				/* symbol is a message */
    message_symbol = lookup_CPL_message(symbolnamestr);
    load_word->word_type = Message_data;
    load_word->word_data.CPLmessage = message_symbol;
  }
  else if (strcmp("Env",symboltypestr) == 0)
  {				/* symbol is an environment part */
    load_word->word_type = Environment_data;
    for (cnt = 0; ((strcmp(environmentpart_names[cnt],symbolnamestr) != 0) &&
		   (cnt < Maxenvparts)); cnt++) ;
    if (cnt < Maxenvparts)
      load_word->word_data.CPLenvironment_part = (environmentkind) cnt;
    else
    {
      write_CPLparsebug_notice();
      system_error("encode_CPL_symbol: unknown environment part");
    }
  }
  else if (strcmp("Icon",symboltypestr) == 0)
  {				/* symbol is an icon */
    load_word->word_type = Icon_data;
    for (cnt = 0; ((strcmp(icon_names[cnt],symbolnamestr) != 0) &&
	 (cnt < Maxicons));
	 cnt++) ;
    if (cnt < Maxicons)
      load_word->word_data.CPLicon = (iconkind) cnt;
    else
    {
      write_CPLparsebug_notice();
      system_error("encode_CPL_symbol: unknown icon");
    }
  }
  else if (strcmp("Prop",symboltypestr) == 0)
  {				/* symbol is a prop */
    cnt = 0;
    while ((strcmp(propinfo[cnt].propname,symbolnamestr) != 0) &&
	   (cnt < next_available_property))
      cnt++;
    if (cnt < next_available_property)
    {
      load_word->word_type = Proplist_data;
      load_word->word_data.CPLproplist = create_list();
      append_new_blank_elem(load_word->word_data.CPLproplist,Sort_type);
      load_word->word_data.CPLproplist->first.sort->sortdata1 =
	(alltypes *) alloc_elem(Allsize);
      load_word->word_data.CPLproplist->first.sort->sortdata1->i =
	cnt;
    }
    else
    {
      write_CPLparsebug_notice();
      printf ("Property %s unknown!\n",symbolnamestr);
      system_error("encode_CPL_symbol!");
    }
  }
  else if (strcmp("Crtn",symboltypestr) == 0)
  {				/* symbol refers to a C routine */
    load_word->word_type = Auxiliary_data;
    load_word->aux_kind = CPLaux_Croutine_index_data;
    for (cnt = 0; ((strcmp(Croutines[cnt].Croutine_name,symbolnamestr) != 0) &&
		   cnt < MaxCroutines); cnt++) ;
    if (cnt < MaxCroutines)
      load_word->word_data.CPLauxdata.Croutine_index = cnt;
    else
    {
      write_CPLparsebug_notice();
      printf ("C routine %s unknown!\n",symbolnamestr);
      system_error("encode_CPL_symbol!");
    }
  }
  else if (strcmp("Const",symboltypestr) == 0)
  {				/* symbol is a constant value */
    encode_const_word(symbolnamestr,load_word,known_owner);
  }
  else if (strcmp("A",symboltypestr) == 0)
  {				/* symbol is a pointer */
    load_word->word_type = CPLptr_data;
    load_word->word_data.CPLptr = find_symbol(symbolnamestr);
    store_forward_def(load_word,symbolnamestr);
  }
  else if (strcmp("AA",symboltypestr) == 0)
  {				/* symbol is a handle */
    load_word->word_type = CPLhandle_data;
    load_word->word_data.CPLptr = find_symbol(symbolnamestr);
    store_forward_def(load_word,symbolnamestr);
  }
  else if (strcmp("Obj",symboltypestr) == 0)
  {
    load_word->word_type = Featurelist_data;
    load_word->word_data.CPLfeaturelist = create_list();
    /* find the named object, but if it's not found this will be an empty set */
    named_object = find_object_in_any_world(symbolnamestr); 
    add_to_featurelist(load_word->word_data.CPLfeaturelist,(featureptr)
		       named_object);
    /* if it wasn't found then we forward reference it for later fixing. */
    store_forward_object_name_ref(load_word,symbolnamestr);
  }
  else if (strcmp("Code",symboltypestr) == 0)
  {				/* symbol refers to a CPL code bloc */
    load_word->word_type = Auxiliary_data;
    load_word->aux_kind = CPLaux_codekind_value_data;
    for (cnt = 0; ((strcmp(codekind_names[cnt],symbolnamestr) != 0) &&
		   (cnt < Maxcodekind)); cnt++)
      ;
    if (cnt < Maxcodekind)
      load_word->word_data.CPLauxdata.codekind_value = (codekind) cnt;
    else
    {
      write_CPLparsebug_notice();
      fprintf(dbgdump, "Code kind %s unknown!\n", symbolnamestr); // CHECK ME!!! dow we want to print the address or the string? -- LJE
      system_error("encode_CPL_symbol!");
    }
  }
  else
  {
    write_CPLparsebug_notice();
    fprintf (dbgdump, "CPL Symbol type %s unknown!\n",symboltypestr);
    system_error ("encode_CPL_symbol.");
  }
}


  CPL_word_ptr
read_cpl_line(char cpl_line[],CPL_word_ptr load_word,featureptr known_owner)
{
  int point = 0, i;
  char labelstr[MaxCPLabelength];
  char opcodestr[MaxCPLopcodelength];
  char symbolstr[MaxCPLsymbolength];
  Boolean noargs = FALSE,add_symbol = FALSE,cpl_directive_read = FALSE;
  opcodekind opcode_read;
  int args_read = 0,reqd_args = 0;
  CPL_word_ptr old_load_word = 0;
  ;
  /* If first character is a semi-colon or a return this line is a comment */
  /* or a blank line used for spacing. */

  /*  fprintf(dbgdump, "Parsing:%s\n", cpl_line); */

#ifdef IRIS
  if ((cpl_line[point] == '\n') || (cpl_line[point] == ';'))
#else
  if ((cpl_line[point] == '\n') || (cpl_line[point] == ';') ||
      (cpl_line[point] == '\r'))
#endif
    return(load_word);
  /* Code a label if it is found in the first column  */
  labelstr[0] = '\0';

  if ((cpl_line[point] != '\t') && (cpl_line[point] != ' '))
  {
    point = read_CPL_string(cpl_line,point,MaxCPLabelength,labelstr);
    add_symbol = TRUE;
#ifdef debugging
    fprintf (parse_file,"%s\t",labelstr);
#endif
  }
#ifdef debugging
  else
    fprintf (parse_file,"\t\t");
#endif

  point = read_CPL_string(cpl_line,point,MaxCPLopcodelength,opcodestr);
  if (point == -1)
  {
    if (add_symbol) {
      /* 12/1/2001: If we got a symbol but no opcode, THAT'S a fatal error. */
      /* however, if there was no symbol and no opcode, dump the entire line */
      /* and assume it was just some tabs accidentally inserted on a blank */
      /* line, as on line 148 of rom.cpl. */
      write_CPLparsebug_notice();
      system_error("in read_CPL_string: no opcode found!");
    } else {
      return(load_word);
    }
  }

  opcode_read = lookup_CPL_opcode(opcodestr);

  old_load_word = load_word;

  load_word = encode_CPL_opcode(labelstr, opcode_read, load_word,
				&cpl_directive_read,&noargs);

  /* only add the symbol to the symbol table if this was not a compiler */
  /* directive such as DEFPROP */
  if (add_symbol && !cpl_directive_read)
    add_symbolrec(labelstr,old_load_word);
  

  if (noargs)			/* done parsing this line on opcodes with */
  {				/* no arguments.  */
#ifdef debugging
    fprintf(parse_file,"\n");
#endif
    return(load_word);
  }

  for (i = 0; i < MaxCPLsymbolsperinst; ++i)
  {
    point = read_CPL_string(cpl_line,point,MaxCPLsymbolength,symbolstr);
    if ((point == -1) || (point > MaxCPLinelength))
      break;	/* reached end of arguments, line, and/or comment */
    encode_CPL_symbol(symbolstr,load_word,known_owner);
    load_word++;
    args_read++;
  }

  /* Figure out how many args are really reqd */
  for (i = 0; i < (int) MaxCPLargs; ++i)
    if ((opcoderecs[(int) opcode_read].opcode_args & (0x1 << i)) > 0)
      reqd_args++;
  /* If we didn't get enough, complain! */
  if (args_read != reqd_args)
  {
    write_CPLparsebug_notice();
    system_error("in read_cpl_line: not enough args parsed for opcode!\n");
  }
  
#ifdef debugging
  fprintf (parse_file,"\n");
#endif

  return(load_word);
}

  void
resolve_CPL_forward_defs(void)
{
  sortptr thisdef,nextdef;
  CPL_word_ptr fill_in_word,result;
  ;
  thisdef = CPL_forward_defs->first.sort;
  while (thisdef != Nil)
  {
    nextdef = thisdef->next;
    fill_in_word = (CPL_word_ptr) thisdef->sortelem;
    result = fill_in_word->word_data.CPLptr =
      find_symbol(thisdef->sortdata1->ptr);
    if (result == Nil)
    {
      write_CPLparsebug_notice();
      printf ("Symbol name:%s unknown.\n",thisdef->sortdata1->ptr);
      system_error("resolve_CPL_forward_defs");
    }
    del_elem(CPL_forward_defs,(elemptr) thisdef);
    thisdef = nextdef;
  }
}

  void
resolve_CPL_forward_object_refs(void)
{
  sortptr thisref,nextref;
  CPL_word_ptr fill_in_word;
  elemptr result;
  ;
  thisref = CPL_forward_object_refs->first.sort;
  while (thisref != Nil)
  {
    nextref = thisref->next;
    fill_in_word = (CPL_word_ptr) thisref->sortelem;
    result = fill_in_word->word_data.CPLfeaturelist->first.ep->thiselem =
      (elemptr) find_object_in_any_world(thisref->sortdata1->ptr);
    if (result == Nil)
    {
      /* Don't do warnings any more because if you don't have that object */
      /* you may have it later... the system fixes outstanding refs as */
      /* new objects are imported, but if an object is imported twice, */
      /* name resolution only finds the first object with that name, which */
      /* can cause problemos, e.g. color bars in a second color tool will */
      /* color the paintchip in the first color tool... o well. */
      /*printf ("Warning: Object name:%s unknown.\n",thisref->sortdata1->ptr);*/
      /*printf ("Automatic name resolution will have to be done later.\n");*/
    }
    else
      del_elem(CPL_forward_object_refs,(elemptr) thisref);
    thisref = nextref;
  }
}
      
  CPL_word_ptr
read_cplfile(char *cpl_file_name, CPL_word_ptr load_word,featureptr known_owner)
{
  char cpl_line[MaxCPLinelength];
  int miaocnt = 0,i;
  ;
  top_local_symbol_record = 0;
  parsefile_linenum = 0;	/* for debugging, when errors are made */

  fprintf(dbgdump, "Miao0\n");
  fflush(dbgdump);

  /* Read it all in a chunk first. */
  transfer_into_temp_buffer(cpl_file_name);

  fprintf(dbgdump, "Miao1\n");
  fflush(dbgdump);

#ifdef debugging
  parse_file = fopen("./rsc/CPLparse.rsc","w");
#endif  
  while (get_cpl_line(cpl_line))
  {
    fprintf(dbgdump, "Miao1.5--%d\n", miaocnt++);
    for (i = 0; cpl_line[i] != '\n'; ++i)
      fprintf(dbgdump, "%c", cpl_line[i]);
    fprintf(dbgdump, "\n\n");
    fflush(dbgdump);
    load_word = read_cpl_line(cpl_line,load_word,known_owner);
    parsefile_linenum++;
  }
  fprintf(dbgdump, "Miao2\n");
  fflush(dbgdump);

  resolve_CPL_forward_defs();	/* resolve any forward address definitions */
#ifdef debugging
  fclose(parse_file);
#endif
  top_local_symbol_record = -1;

  fprintf(dbgdump, "Miao3\n");
  fflush(dbgdump);

  return(load_word);
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	                 CODE-LEVEL ENCODING ROUTINES		             */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */


/* Always follow any instruction written with an ENDFR instruction. */

  void
close_RAM(void)
{
  /* This word gets overwritten by the next call that stores in RAM */
  clear_word(next_free_RAM_word);
  next_free_RAM_word->word_type = Opcode_data;
  next_free_RAM_word->word_data.CPLopcode = ENDFR; 
  /* Note no advancement of next_free_RAM_word here! */
}

  void
store_CPL_RAM_word(CPL_word_ptr storeword)
{
  /* Assumption: all words start as Blankword */
  clear_word(next_free_RAM_word); 
  *next_free_RAM_word = *storeword;
  next_free_RAM_word = inc_CPL_blocptr(next_free_RAM_word,
				   system_codebloc, Systemcodeblocsize);
}

  void
store_CPL_RAM_word_a(CPL_word_ptr storeword)
{
}

/* Always follow any instruction written with an ENDFR instruction. */
  void
store_simple_RAM_instruction(opcodekind theopcode)
{
  clear_word(next_free_RAM_word); /* assumption: all words start as Blankword */
  next_free_RAM_word->word_type = Opcode_data;
  next_free_RAM_word->word_data.CPLopcode = theopcode;
  next_free_RAM_word = inc_CPL_blocptr(next_free_RAM_word,
				   system_codebloc, Systemcodeblocsize);
  close_RAM();  
  restart_CPLparser = TRUE;
}

  void
store_CPL_RAM_instruction(CPL_word_ptr OP_ptr, ...)
{
  va_list args;
  opcodekind opcode_id;
  opcode_argkind reqd_opcode_args;
  CPL_word_ptr *OP_param, next_arg;
  ;
  OP_param = &OP_ptr;
  opcode_id = OP_ptr->word_data.CPLopcode;

  reqd_opcode_args = opcoderecs[opcode_id].opcode_args;

  store_CPL_RAM_word(OP_ptr);

  va_start(args, OP_ptr);
  next_arg = va_arg(args, CPL_word_ptr);
  if (reqd_opcode_args & ASarg)
  {
    //    store_CPL_RAM_word(*next_arg);
    store_CPL_RAM_word(next_arg);
    next_arg = va_arg(args, CPL_word_ptr);
  }
  if (reqd_opcode_args & ATarg)
  {
    store_CPL_RAM_word(next_arg);
    next_arg = va_arg(args, CPL_word_ptr);
  }
  if (reqd_opcode_args & ARarg)
  {
    store_CPL_RAM_word(next_arg);
    next_arg = va_arg(args, CPL_word_ptr);
  }
  if (reqd_opcode_args & ADarg)
  {
    store_CPL_RAM_word(next_arg);
  }
  va_end(args);

  close_RAM();
  restart_CPLparser = TRUE;
}

  void
encode_CPL_RAM_instruction(char *CPL_instruction_line)
{
  next_free_RAM_word = read_cpl_line(CPL_instruction_line,
				     next_free_RAM_word,Nil);
  resolve_CPL_forward_defs();
  close_RAM();
  restart_CPLparser = TRUE;
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	                 GENERAL CODE READING ROUTINES		             */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

  void
read_CPLcode_into_rom(char *rom_filename)
{
  FILE *rom_file;
  ;
  rom_file = fopen(rom_filename,"r");
  if (rom_file == NULL)
  {
    printf ("CPL ROM file %s not found!\n",rom_filename);
    return;
  }
  fclose(rom_file);
  strcpy(parse_filename,rom_filename); /* for debugging */
  next_free_CPL_word = read_cplfile(rom_filename, next_free_CPL_word,Nil);
}

  void
read_CPLcode_into_ram(char *ram_filename)
{
  FILE *ram_file;
  ;
  ram_file = fopen(ram_filename,"r");
  if (ram_file == NULL)
  {
    printf ("CPL RAM file %s not found!\n",ram_filename);
    return;
  }
  fclose(ram_file);
  strcpy(parse_filename,ram_filename); /* for debugging */
  next_free_CPL_word = read_cplfile(ram_filename,next_free_CPL_word,Nil);
}

  void
read_CPLcode_into_property(char *prop_filename)
{
  FILE *prop_file;
  int i,property_num,reqd_size;
  CPL_word_ptr CPL_temp_RAM_ptr,end_of_propertycode;
  ;
  prop_file = fopen(prop_filename,"r");
  if (prop_file == NULL)
  {
#ifdef Beingnice
    printf ("warning:CPL property file %s not found!\n",prop_filename);
#endif
    return;
  }
  fclose(prop_file);
  CPL_temp_RAM_ptr = (CPL_word_ptr) alloc_elem(Systemcodeblocsize * CPLwordsize);
  strcpy(parse_filename,prop_filename); /* for debugging */
  end_of_propertycode = read_cplfile(prop_filename,CPL_temp_RAM_ptr,Nil);
  reqd_size = (uint64_t) end_of_propertycode - (uint64_t) CPL_temp_RAM_ptr; // 64 bit change -- LJE
#ifdef debug
  heapinfo();
  printf("# of words used:%d\n",reqd_size /** CPLwordsize*/);
#endif
  CPL_temp_RAM_ptr = realloc(CPL_temp_RAM_ptr, reqd_size /** CPLwordsize*/);
#ifdef debug
  heapinfo();
#endif
  /* Assume the property_name memory position was stored by a STARTPROP */
  /* compiler directive! */
  property_num = find_property(current_property_name);
  if (property_num == -1)
  {
    printf ("Failure to read good property code\n");
    return;
  }
  propinfo[property_num].propertycode =  find_symbol(current_property_name);
}

/* These need to read the code into some temporary space, determine the */
/* size, then allocate that much for the feature and copy over to the feature*/

  void
read_CPL_privatecode(char *private_filename,featureptr known_owner)
{
  FILE *private_file;
  int reqd_size;
  CPL_word_ptr CPL_temp_RAM_ptr,end_of_privatecode;
  ;
  private_file = fopen(private_filename,"r");
  if (private_file == NULL)
  {
    printf ("Warning: CPL private code file %s not found!\n",private_filename);
    return;
  }
  fclose(private_file);
  CPL_temp_RAM_ptr = (CPL_word_ptr) alloc_elem(Systemcodeblocsize * CPLwordsize);
  strcpy(parse_filename,private_filename); /* for debugging */
  end_of_privatecode = read_cplfile(private_filename,
				    CPL_temp_RAM_ptr,known_owner);
  reqd_size = (int) (end_of_privatecode - CPL_temp_RAM_ptr);

  CPL_temp_RAM_ptr = realloc(CPL_temp_RAM_ptr, reqd_size * CPLwordsize);

  known_owner->privatecode = CPL_temp_RAM_ptr;
}

  void
read_CPL_propfile(char *property_name)
{
  char propfilename[Maxpropertynamelen*4];
  ;
  strcpy(current_property_name,property_name); /* simulate STARTPROP directive */
  strcpy(propfilename,"./cpl/");
  strcat(propfilename,property_name);
  strcat(propfilename,".cpl");
  convert_to_lower(propfilename);
  read_CPLcode_into_property(propfilename);
}
  
  void
read_CPL_propfiles(void)
{
  FILE *proprscfile;
  char property_name[Maxpropertynamelen];
  int numpropfiles = 0;
  ;
  proprscfile = fopen("./rsc/cplpropfiles.rsc","r");
  if (proprscfile == NULL)
    system_error("read_CPL_propfiles: no property files resource file found!");
  
  fscanf(proprscfile,"Number of Property Files:%d\n",&numpropfiles);
  while (numpropfiles-- > 0)
  {
    fscanf(proprscfile,"%s",property_name);
    read_CPL_propfile(property_name);
  }
  fclose(proprscfile);
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*   	                 DISASSEMBLY ROUTINES		                     */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

  void
disassemble_CPL_auxdata(CPL_word_ptr dis_word,char *auxwordstr)
{
  short *the_color;
  ;
  auxwordstr[0] = '\0';
  switch (dis_word->aux_kind)
  {
  case CPLaux_int_data:
    sprintf (auxwordstr,"Const:I%d",dis_word->word_data.CPLauxdata.CPLintdata);
    break;
  case CPLaux_string_data:
    sprintf (auxwordstr,"Const:A%s",dis_word->word_data.
	     CPLauxdata.CPLstringdata);
    break;
  case CPLaux_float_data:
    sprintf (auxwordstr,"Const:D%lf",dis_word->word_data.
	     CPLauxdata.CPLfloatdata);
    break;
  case CPLaux_offset_data:
    sprintf (auxwordstr,"Const:V[%f,%f,%f]",
	     dis_word->word_data.CPLauxdata.CPLoffsetdata[vx],
	     dis_word->word_data.CPLauxdata.CPLoffsetdata[vy],
	     dis_word->word_data.CPLauxdata.CPLoffsetdata[vz]);
    break;
  case CPLaux_matrix_data:
    sprintf(auxwordstr,"Const:M[{%5.2f,%5.2f,%5.2f,%5.2f}, {%5.2f,%5.2f,%5.2f,%5.2f}, {%5.2f,%5.2f,%5.2f,%5.2f}, {%5.2f,%5.2f,%5.2f,%5.2f}]",
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[0][vx],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[0][vy],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[0][vz],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[0][vw],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[1][vx],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[1][vy],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[1][vz],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[1][vw],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[2][vx],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[2][vy],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[2][vz],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[2][vw],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[3][vx],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[3][vy],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[3][vz],
	    dis_word->word_data.CPLauxdata.CPLmatrixdata[3][vw]);
    break;
  case CPLaux_boolean_data:
    if (dis_word->word_data.CPLauxdata.CPLbooleandata)
      sprintf(auxwordstr,"Const:TRUE");
    else
      sprintf(auxwordstr,"Const:FALSE");
    break;
  case CPLaux_address_data:
    sprintf(auxwordstr, "Const:P%p", dis_word->word_data.CPLauxdata.CPLaddressdata);
    break;
  case CPLaux_color_data:
    the_color = dis_word->word_data.CPLauxdata.CPLcolordata;
    sprintf (auxwordstr, "Const:C[%hd,%hd,%hd]", the_color[0],the_color[1],
	     the_color[2]);
    break;
  case CPLaux_update_data: /* fix this up so they can be really interpreted! */	
    sprintf (auxwordstr, "Const:U%d/when:%d",
	     dis_word->word_data.CPLauxdata.update_data.update_flags,
	     dis_word->word_data.CPLauxdata.update_data.update_now);
    break;
  case CPLaux_rotate_data:
    sprintf (auxwordstr,"Const:Rvec:[%5.2f,%5.2f,%5.2f]/pt:[%5.2f,%5.2f,%5.2f]/angle:%d",
	     dis_word->word_data.CPLauxdata.rotate_data.rotvec[vx],
	     dis_word->word_data.CPLauxdata.rotate_data.rotvec[vy],
	     dis_word->word_data.CPLauxdata.rotate_data.rotvec[vz],
	     dis_word->word_data.CPLauxdata.rotate_data.rotpt[vx],
	     dis_word->word_data.CPLauxdata.rotate_data.rotpt[vy],
	     dis_word->word_data.CPLauxdata.rotate_data.rotpt[vz],
	     dis_word->word_data.CPLauxdata.rotate_data.rotangle);	     
    break;
  case CPLaux_scale_data:
    sprintf (auxwordstr,"Scale:%lf",
	     dis_word->word_data.CPLauxdata.scale_data.scale_factor);
    break;			/* fix me up later */
  case CPLaux_duplicate_destroy_data:
    sprintf (auxwordstr,"Duplicatedestroy");
    break;			/* fix me up later */
  case CPLaux_owner_feature_data:
    if (dis_word->word_data.CPLauxdata.owner_feature->type_id == Obj_type)
       sprintf (auxwordstr, "Owner_feature:%p (%s)",
		(void *) dis_word->word_data.CPLauxdata.owner_feature,
		((objptr) (dis_word->word_data.CPLauxdata.owner_feature))->obj_name);
    else
      sprintf (auxwordstr,"Owner_feature:%p",
	       (void *) dis_word->word_data.CPLauxdata.owner_feature);
    break;
  case CPLaux_Croutine_index_data:
    sprintf (auxwordstr,"Crtn:%s",
	     Croutines[dis_word->word_data.CPLauxdata.Croutine_index].
	     Croutine_name);
    break;
  case CPLaux_cutplane_shade_data:
    break;			/* someday... */
  case CPLaux_codekind_value_data:
    sprintf (auxwordstr,"Code:%s",
	     codekind_names[dis_word->word_data.CPLauxdata.codekind_value]);
    break;
  case CPLaux_new_world_data:
    sprintf (auxwordstr,"Const:W%s",
	     dis_word->word_data.CPLauxdata.new_world->world_name);
    break;
  }
}

  void
disassemble_CPL_word(CPL_word_ptr dis_word,char *wordstr)
{
  opcodekind opcode_id;
  messagekind message_id;
  environmentkind env_part;
  char auxwordstr[MaxCPLinelength],address_name[MaxCPLsymbolength];
  featureptr firstfeature;
  int firstproperty;
  ;
  if (dis_word == Nil)
  {
    wordstr[0] = '\0';
    return;
  }

  switch (dis_word->word_type)
  {
  case Opcode_data:
    opcode_id = dis_word->word_data.CPLopcode;
    strcpy(wordstr,opcoderecs[opcode_id].opcodename);
    if (strcmp(wordstr, "OWNER") == 0) 
      1;			/* hack! */
    break;
  case Message_data:
    message_id = dis_word->word_data.CPLmessage;
    sprintf(wordstr,"Msg:%s",message_names[message_id]);
    break;
  case CPLptr_data:
    strcpy(wordstr,"A:");
    if (!lookup_symbol_from_address(dis_word->word_data.CPLptr,address_name))
      sprintf(address_name,"%p", (void *) dis_word->word_data.CPLptr);
    strcat(wordstr,address_name);
    break;
  case CPLhandle_data:
    strcpy(wordstr,"AA:");
    if (!lookup_symbol_from_address(dis_word->word_data.CPLhandle,address_name))
      sprintf(address_name,"%p", (void *) dis_word->word_data.CPLhandle);
    strcat(wordstr,address_name);
    break;
  case Auxiliary_data:
    disassemble_CPL_auxdata(dis_word,wordstr);
    break;
  case Featurelist_data:
    firstfeature = dis_word->word_data.CPLfeaturelist->first.ep->thiselem;
    sprintf(wordstr,"Featurelist:%p", (void *) firstfeature);
    if (dis_word->word_data.CPLfeaturelist->numelems > 1)
      strcat(wordstr,"...");    
    break;
  case Proplist_data:
    firstproperty = dis_word->word_data.CPLproplist->first.sort->sortdata1->i;
    sprintf(wordstr,"Prop:%s",propinfo[firstproperty].propname);
    if (dis_word->word_data.CPLproplist->numelems > 1)
      strcat(wordstr,"...");
    break;
  case Environment_data:
    env_part = dis_word->word_data.CPLenvironment_part;
    sprintf(wordstr,"Env:%s",environmentpart_names[env_part]);
    break;
  case Icon_data:		/* unused so far */
  default:
    strcpy(wordstr,"(Unknown)");
  }  
  if (dis_word->contents_of)
    strcat(wordstr,"*");
  else if (dis_word->address_of)
    strcat(wordstr,"&");
}

  void
disassemble_CPL_sentence(CPL_word_ptr OP_ptr,CPL_word_ptr AS_ptr,
			 CPL_word_ptr AT_ptr, CPL_word_ptr AR_ptr,
			 CPL_word_ptr AD_ptr,
			 char *outstr)
{
  char wordstr[MaxCPLinelength];
  char AS_wordstr[MaxCPLinelength],AT_wordstr[MaxCPLinelength];
  char AR_wordstr[MaxCPLinelength],AD_wordstr[MaxCPLinelength];
  ;
  /* lookup the symbol name if it has one */
  if (!lookup_symbol_from_address(OP_ptr->word_data.CPLptr,outstr))
    strcat(outstr,"\t\t");
  else
  {	/* if there is a label, make sure to print enough tabs! */
    if (strlen(outstr) <= 9)
      strcat(outstr,"\t\t");
    else
      strcat(outstr,"\t");
  }

  /* lookup the opcode */
  disassemble_CPL_word(OP_ptr->word_data.CPLptr,wordstr);
  if (wordstr[0] != '\0')
  {
    strcat(outstr,wordstr);
    strcat(outstr,"\t");
  }
  /* lookup various regs and disassemble their contents */
  disassemble_CPL_word(AS_ptr->word_data.CPLptr,AS_wordstr);
  disassemble_CPL_word(AT_ptr->word_data.CPLptr,AT_wordstr);
  disassemble_CPL_word(AR_ptr->word_data.CPLptr,AR_wordstr);
  disassemble_CPL_word(AD_ptr->word_data.CPLptr,AD_wordstr);
  if (AS_wordstr[0] != '\0')
  {
    strcat(outstr,AS_wordstr);
    if ((AT_wordstr[0] != '\0') || (AR_wordstr[0] != '\0') ||
	(AD_wordstr[0] != '\0'))
      strcat(outstr,",");
  }
  if (AT_wordstr[0] != '\0')
  {
    strcat(outstr,AT_wordstr);
    if ((AR_wordstr[0] != '\0') || (AD_wordstr[0] != '\0'))
      strcat(outstr,",");
  }
  if (AR_wordstr[0] != '\0')
  {
    strcat(outstr,AR_wordstr);
    if (AD_wordstr[0] != '\0')
      strcat(outstr,",");
  }
  if (AD_wordstr[0] != '\0')
    strcat(outstr,AD_wordstr);
  
  if (OP_ptr->word_data.CPLptr->wiped)
    strcat(outstr, "\t(*** WIPED ***)\n");
  else
    strcat(outstr,"\n");
}

