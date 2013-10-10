
/* FILE: text.h */

#ifndef text_includes
#define text_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*            3-D Text Drawing Type Definitions for 3Form                 */
/*                                                                        */
/* Authors: LJE and WAK                           Date: January 9,1991    */
/*                                                                        */
/* Copyright (C) 1989,1990,1991 William Kessler and Larry Edwards.        */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <platform_gl.h>

#include "list.h"

#define Textstringsize sizeof(textstringtype)
#define Textgroupsize sizeof(textgrouptype)
#define Maxstringstylelength 50
#define Shadowoffset 2.0

typedef enum
{
  Hershey,
  Helvetica,
  Times,
  Palatino,
  Courier,
  Maxtextfontype
} textfontype;

typedef enum
{
  Plain,
  Underline,
  Bold,
  Shadow,
  Italic,
  Maxstringstyle
} stringstyletype;

typedef struct textstringstruct
{
  char *string;
  Coord string_width;
  vertype string_offset;
  Coord string_x_scale,string_y_scale;
  int string_pen_width;
  colortype string_color;
  stringstyletype string_style;
  Boolean visible;
  short insertion_point; /* This is the index of char after the insertion pt */
  short num_selected_chars;	/* Selection from insertion point forwards... */
} textstringtype,*textstringptr;

typedef struct textgroupstruct
{
  textfontype font;
  struct liststruct *textstrings; /* linked list of ep's to text strings */
} textgrouptype,*textgrouptr;


typedef struct {
  float char_width;
  void (*char_def) ();
} char_rec;

#endif /* text_includes */
