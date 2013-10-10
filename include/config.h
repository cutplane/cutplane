
/* FILE: config.h */

#ifndef config_includes
#define config_includes

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*        Machine-Specific Configuration Constants Defined Here           */
/*                                                                        */
/* Authors: LJE and WAK                           Date: December 26,1989  */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland  */
/* Stanford Junior University, and William Kessler and Larry Edwards.     */
/* All Rights Reserved.                                                   */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/* #define IRIS */   /* Toto, we're not in kansas anymore! 10/18/01 */
/* #define FAST */

#define Cos(fnum) cos((double) (fnum))
#define Acos(fnum) acos((double) (fnum))
#define Fabs(fnum) fabs((double) (fnum))
#define Sqrt(fnum) sqrt((double) (fnum))

#define ROOMUPKEY UPARROWKEY	      /* Sun is :I */
#define ROOMDOWNKEY DOWNARROWKEY      /* J */
#define ROOMLEFTKEY LEFTARROWKEY      /* K */
#define ROOMRIGHTKEY RIGHTARROWKEY    /* L */

#define qbutton qdevice
#define unqbutton unqdevice
#define setbutton(device,value)
#define replace(tag,offset) objreplace(tag)
#define setvaluator gl_setvaluator
#define getvaluator gl_getvaluator
// #define finish()

#define malloc_verify()

/* undef me to go back to the old way. */
#define tryingnewinputs

#endif // config_includes
