
/* FILE: text.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*         3-D Text Drawing Routines for 3Form Design System              */
/*                                                                        */
/* Author: WAK&LJE                                   Date: January 9,1991 */
/* Version: 1                                                             */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990,1991 William Kessler and             */
/* Larry Edwards. All Rights Reserved.                                    */
/*                                                                        */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define TEXTMODULE

#include <config.h>
#include <stdio.h>

#include <platform_gl.h>
#include <math.h>

#include <cutplane.h>
#include <grafx.h>
#include <text.h>

char textstring_styles[Maxstringstyle][Maxstringstylelength] = {"Plain",
								  "Underline",
								  "Bold",
								  "Shadow",
								  "Italic"};




/* Object font text string drawing routines begin here */


  Coord
compute_insert_height(textfontype thefont, Coord yscale)
{
  /* for now we ignore the passed font and just use hershey's height */
  return(hershey_height() * yscale);
}

  void
compute_insert_pos(char *thestring,int insert_pt, vertype insert_pos)
{
  int i = 0;
  Coord xoffset;
  ;
  setpos3d(insert_pos,0.0,0.0,0.0);
  while (i < insert_pt)
    insert_pos[vx] += char_lookups[(int) thestring[i++]].char_width;
}

  void
draw_insertion_point(textfontype thefont,textstringptr textstring)
{
  vertype pos,insertpos;
  Coord insert_height;
  ;
  glPushMatrix();

  compute_insert_pos(textstring->string,textstring->insertion_point,insertpos);
  setcolor(red);
  translate(textstring->string_offset[vx],textstring->string_offset[vy],
	    textstring->string_offset[vz]);
  scale(textstring->string_x_scale,textstring->string_y_scale,1.0);
  glTranslatef(insertpos[vx],insertpos[vy],insertpos[vz]);
  
  /* for now we ignore this information from the font */
  insert_height = compute_insert_height(thefont,textstring->string_y_scale);

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  linewidth(textstring->string_pen_width+1);
  bgnline();
  setpos3d(pos,0.0,hershey_descender_height(),0.0);
  v3f(pos);
  setpos3d(pos,0.0,hershey_Xheight(),0.0);
  v3f(pos);
  endline();
  popattributes();

  popmatrix();
}

  void
draw_obj_label(objptr labelled_obj)
{
  elemptrptr this_string_ep;
  textgrouptr text_group;
  textstringptr textstring;
  vertype pos,beampos;
  ;
  pushmatrix();

  multmatrix(labelled_obj->xform);
  text_group = (textgrouptr)
    get_property_val((featureptr) labelled_obj,haslabel_prop).ptr;
  /* setobjfont(text_group->font); */
  this_string_ep = text_group->textstrings->first.ep;
  while (this_string_ep != Nil)
  {
    textstring = (textstringptr) this_string_ep->thiselem;
    if (textstring->string != Nil)
    {
      switch (textstring->string_style)
      {
      case Bold:
	linewidth(textstring->string_pen_width*2);
	break;
      case Shadow:
	setpos3d(pos,textstring->string_offset[vx] + Shadowoffset,
		 textstring->string_offset[vy] - Shadowoffset,
		 textstring->string_offset[vz] - Shadowoffset);
	setcolor(black);
	objstr(textstring->string,pos, textstring->string_x_scale,
	       textstring->string_y_scale);
	break;
      case Underline:
      case Italic:
	break;
      case Plain:
	linewidth(textstring->string_pen_width);
	break;
      }
      setcolor(textstring->string_color);
      objstr(textstring->string,textstring->string_offset,
	     textstring->string_x_scale,
	     textstring->string_y_scale);
    }
    if (textstring->insertion_point >= 0)
      draw_insertion_point(text_group->font,textstring);    
    this_string_ep = this_string_ep->next;
  }

  popmatrix();
}

/* Object font text string utility routines begin here */

  Coord
text_string_width(char *thestring,textfontype thefont,Coord string_x_scale)
{
  int i = 0;
  Coord width = 0;
  ;
  /* currently this really ignores the font... the array char_lookups should */
  /* have another indirection to land in the indicated font, thefont. */
  while (thestring[i] != '\0')
    width += char_lookups[(int) thestring[i++]].char_width * string_x_scale;

  return(width);
}

  textstringptr
create_text_string(char *new_string,vertype string_offset,
		   textfontype initial_font,
		   Coord string_x_scale,Coord string_y_scale,
		   Coord string_pen_width,colortype string_color,
		   stringstyletype string_style)
{
  textstringptr new_text_string;
  ;
  new_text_string = (textstringptr) alloc_elem(Textstringsize);
  if (new_string == Nil)
    new_text_string->string = Nil;
  else
  {
    new_text_string->string = (char *) alloc_elem(strlen(new_string)+1);
    strcpy(new_text_string->string,new_string);
    new_text_string->string_width = text_string_width(new_string,initial_font,
						      string_x_scale);
  }
  copypos3d(string_offset,new_text_string->string_offset);
  new_text_string->string_x_scale = string_x_scale;
  new_text_string->string_y_scale = string_y_scale;
  new_text_string->string_pen_width = string_pen_width;
  copycolor(string_color,new_text_string->string_color);
  new_text_string->string_style = string_style;
  /* The I-beam must be positive to be visible. */
  new_text_string->insertion_point = -1; 
  return(new_text_string);
}

  textstringptr
find_label_item(objptr labelled_obj,int item_number)
{
  textgrouptr text_group;
  elemptrptr text_string_ep;
  ;
  text_group = (textgrouptr) get_property_val((featureptr) labelled_obj,
					  haslabel_prop).ptr;
  text_string_ep = (elemptrptr) get_elem_ptr(item_number,
					     text_group->textstrings);
  return((textstringptr) text_string_ep->thiselem);
}
  
  char *
get_label_item(objptr labelled_obj, int item_number)
{
  textstringptr text_string;
  char *returnstr = Nil;
  ;
  if ((text_string = find_label_item(labelled_obj,item_number)) != Nil)
    returnstr = text_string->string;

  return(returnstr);
}

/* a label is a textgroup and so can be composed of more than independent */
/* string... this allows you to set the text in one string in the group. */
/* don't call this unless this object definitely has a textgroup associated */
/* with it. */

  void
set_label_item(objptr labelled_obj, int item_number,char *new_string)
{
  textstringptr text_string;
  ;
  if ((text_string = find_label_item(labelled_obj,item_number)) != Nil)
  {
    if (new_string == Nil)	/* passed if you want to clear the string */
      free_elem((elemptr *) &(text_string->string));
    else
    {
      if (text_string->string == Nil)
	text_string->string = (char *) alloc_elem(strlen(new_string)+1);
      else
	text_string->string = (char *) realloc(text_string->string,
					       strlen(new_string)+1);
      strcpy(text_string->string,new_string);
    }
  }
}

  void
set_label_item_style(objptr labelled_obj,int item_number,
		     stringstyletype new_style)
{
  textstringptr text_string;
  ;
  if ((text_string = find_label_item(labelled_obj,item_number)) != Nil)
    text_string->string_style = new_style;
}

  void
set_label_item_offset(objptr labelled_obj, int item_number,vertype new_offset)
{
  textstringptr text_string;
  ;
  if ((text_string = find_label_item(labelled_obj,item_number)) != Nil)
    copypos3d(new_offset,text_string->string_offset);
}

  void
set_label_item_scale(objptr labelled_obj, int item_number,Coord new_x_scale,
		     Coord new_y_scale)
{
  textstringptr text_string;
  ;
  if ((text_string = find_label_item(labelled_obj,item_number)) != Nil)
  {
    text_string->string_x_scale = new_x_scale;
    text_string->string_y_scale = new_y_scale;
  }
}

  void
set_label_item_pen_width(objptr labelled_obj, int item_number,int new_pen_width)
{
  textstringptr text_string;
  ;
  if ((text_string = find_label_item(labelled_obj,item_number)) != Nil)
    text_string->string_pen_width = new_pen_width;
}

  void
set_label_item_color(objptr labelled_obj, int item_number,colortype new_color)
{
  textstringptr text_string;
  ;
  if ((text_string = find_label_item(labelled_obj,item_number)) != Nil)
    copycolor(new_color,text_string->string_color);
}


  void
set_label_insertion_point(objptr labelled_obj, int item_number,
			  int new_insertion_point)
{
  textstringptr text_string;
  int actual_insertion_point;
  ;
  if ((text_string = find_label_item(labelled_obj,item_number)) != Nil)
  {
    actual_insertion_point = new_insertion_point;
    if (new_insertion_point == 10000) /* this means place insertion point */
				      /* at end of the string */
    {
      if (text_string->string == Nil)
	actual_insertion_point = 0;
      else
	actual_insertion_point = strlen(text_string->string);
    }
    text_string->insertion_point = actual_insertion_point;
  }
}

  void
move_label_insertion_point(objptr labelled_obj, int item_number,int offset)
{
  textstringptr text_string;
  int thestringlen;
  ;
  if ((text_string = find_label_item(labelled_obj,item_number)) != Nil)
  {
    text_string->insertion_point += offset;
    if (offset > 0)
    {
      if (text_string->string != Nil)
      {
	thestringlen = strlen(text_string->string);
	if (text_string->insertion_point > thestringlen)
	  text_string->insertion_point = thestringlen;
      }
      else			/* don't move it if no string present!*/
	text_string->insertion_point = 0;
    }
    else
      if (text_string->insertion_point < 0)
	text_string->insertion_point = 0; /* to get to -1 use */
					 /* set_label_insertion_point */
  }
}


  void
insert_string_into_label(objptr labelled_obj, int item_number,
			 char * insertstring)
{
  textstringptr text_string;
  static char newstr[500];	/* hope this is big enough! */
  ;
  if ((text_string = find_label_item(labelled_obj,item_number)) != Nil)
  {
    strncpy(newstr,text_string->string,(size_t) text_string->insertion_point);
    newstr[text_string->insertion_point] = '\0'; /* strncpy doesn't null term. */
    strcat(newstr,insertstring);
    strcat(newstr,&(text_string->string[text_string->insertion_point]));
    set_label_item(labelled_obj,item_number,newstr);
  }
}

  void
remove_chars_from_label(objptr labelled_obj, int item_number,
			int startindex,int endindex)
{
  textstringptr text_string;
  static char newstr[500];	/* hope this is big enough! */
  ;
  if ((text_string = find_label_item(labelled_obj,item_number)) != Nil)
  {
    if (startindex > 0)		/* don't copy if deleting from start pos. */
    {
      strncpy(newstr,text_string->string,startindex);
      newstr[startindex] = '\0';	/* strncpy doesn't null terminate! */
    }
    else
      newstr[0] = '\0';
    if (text_string->string[endindex] != '\0') /* if not deleting off the end */
      strcat(newstr,&(text_string->string[endindex]));
    set_label_item(labelled_obj,item_number,newstr);
  }
}


  textgrouptr
create_label(objptr labelled_obj,char *initial_string, vertype initial_offset,
	     textfontype initial_font)
{
  objptr text_object;
  textgrouptr new_textgroup;
  textstringptr new_text_string;
  ;
  new_text_string = create_text_string(initial_string,initial_offset,
				       initial_font,1.0,1.0,1,black,Plain);
  new_textgroup = (textgrouptr) alloc_elem(Textgroupsize);
  new_textgroup->textstrings = create_list();
  new_textgroup->font = initial_font;
  add_to_featurelist(new_textgroup->textstrings,(featureptr) new_text_string);

  set_property((featureptr) labelled_obj,haslabel_prop,new_textgroup);

  return(new_textgroup);
}

  void
add_label_item(objptr labelled_obj,char *initial_string, vertype initial_offset,
	       textfontype initial_font)
{
  textgrouptr text_group;
  textstringptr new_text_string;
  ;
  if (!has_property((featureptr) labelled_obj,haslabel_prop))
    text_group = create_label(labelled_obj,initial_string,initial_offset,
			      initial_font);
  else
  {
    text_group = (textgrouptr) get_property_val((featureptr) labelled_obj,
					    haslabel_prop).ptr;
    new_text_string = create_text_string(initial_string,initial_offset,
					 initial_font,1.0,1.0,1,black,Plain);
    add_to_featurelist(text_group->textstrings,(featureptr) new_text_string);
  }
}

  void
delete_label_item(objptr labelled_obj,int item_number)
{
  textgrouptr text_group;
  textstringptr text_string;
  ;
  text_group = (textgrouptr) get_property_val((featureptr) labelled_obj,
					  haslabel_prop).ptr;
  text_string = (textstringptr) get_elem_ptr(item_number,
					     text_group->textstrings);
  del_elem(text_group->textstrings,(elemptr) text_string);
}


				/* draw a string in the hershey font */
				/* assuming that the graphics linewidth,pos, */
  void				/* color, etc are all set. */
objstr(char *thestring,vertype pos,Coord x_scale, Coord y_scale)
{
  int i = 0;
  ;
  glPushMatrix();
  glTranslatef(pos[vx],pos[vy],pos[vz]);
  while (thestring[i] != '\0')
  {
    glScalef(x_scale,y_scale,1.0);
    if ((char_lookups[(int) thestring[i]].char_def) != Nil)
      (*(char_lookups[(int) thestring[i]].char_def)) ();
    glScalef(1.0/x_scale,1.0/y_scale,1.0);
    glTranslatef(char_lookups[(int) thestring[i]].char_width*x_scale,0.0,0.0);
    i++;
  }
  glPopMatrix();
}
