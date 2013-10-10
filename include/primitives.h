
/* FILE: primitives.h */

#ifndef primitives_includes
#define primitives_includes

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                                                                          */
/* Primitive Object Generation Routines Header File for the Cutplane Editor */
/*                                                                          */
/* Authors: LJE and WAK                                                     */
/* Date: Aug 9 1988                                                         */
/* Version: 1.0                                                             */
/*                                                                          */
/* Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland    */
/* Stanford Junior University, and William Kessler and Larry Edwards.       */
/* All Rights Reserved.                                                     */
/*                                                                          */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#define Toolholderwidth 200.0
#define Toolholderheight 50.0
#define Toolholderdepth 50.0

#define Numprimtypes 5
#define Priminfonamelength 40

#define Primrecsize sizeof(prim_rec)

#define Cuberecsize sizeof(cube_rec)
#define Cylinderecsize sizeof(cylinder_rec)
#define Conerecsize sizeof(cone_rec)
#define Sphererecsize sizeof(sphere_rec)
#define Pyramidrecsize sizeof(pyramid_rec)

typedef enum
{
  Cube_id = 0,
  Cylinder_id,
  Cone_id,
  Sphere_id,
  Pyramid_id
} prim_id_kind;

/* These are records containing enough information to make all the important */
/* primitives defined so far in primitives.c */

typedef struct cuberec_struct
{
  double length,width,height;
} cube_rec,*cube_rec_ptr;

typedef struct cylinderec_struct
{
  double radius,height;
  int num_facets;
} cylinder_rec,*cylinder_rec_ptr;

typedef struct conerec_struct
{
  double radius,height;
  int num_facets;
} cone_rec,*cone_rec_ptr;

typedef struct sphererec_struct
{
  double radius;
  int num_facets;
} sphere_rec,*sphere_rec_ptr;

typedef struct pyramidrec_struct
{
  double length,width,height;
} pyramid_rec,*pyramid_rec_ptr;

  
/* This record is attached via the primitive_prop to an object, and points */
/* to the routine used to (re) create the primitive, a pointer into a table */
/* of names of infopads (supposedly) loaded into the system that the user */
/* can access (don't actually have to be loaded), and a pointer to one of */
/* the above structures. */

typedef struct primrec_struct
{
  short prim_id;		/* index constant defining primitive type */
  /* Pointer to record containing pertinent information about this primitive. */
  void *prim_info;
} prim_rec,*prim_rec_ptr;


/* These records are used to add new primitives routines */
/* and the names of associated input plates in a global array. */

typedef struct prim_create_struct
{
  /* Pointer to the routine used to create this primitive */
  objptr (* createtechnique)(/* struct primrec_struct * */);
  char prim_info_platename[Priminfonamelength];
} prim_create_rec;

#endif
