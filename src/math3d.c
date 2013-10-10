
/* FILE: math3d.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*               3D Math Routines for the Cutplane Editor                 */
/*                                                                        */
/* Copyright (C) 1987,1988,1989 The Board of Trustees of The Leland       */
/* Stanford Junior University.  All Rights Reserved.                      */
/*                                                                        */
/* Authors: LJE and WAK                           Date: August 26,1988    */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define MATH3DMODULE

#include <config.h>
#include <math.h>

#include <platform_gl.h>

#include <cutplane.h>

#include <globals.h>
#include <math3d.h>

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                        LOW LEVEL MATH ROUTINES                         */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  int
sign(int number)
{
/*
  if (number == 0)
    return(0);
*/
/*
  if (number < 0)
    return (-1);
  return (1);
*/
  /* NOTE: this assumes a true expression evaluates to 1 */
  return (((number >= 0) << 1) - 1);
}

  float
fsign(float number)
{
/*
  if (number == 0.0)
    return(0.0);
*/
  if (number < 0.0)
    return (-1.0);
  return (1.0);
}

  int
iround(float number)
{
  return ((number < 0.0) ? (int) (number - 0.5) : (int) (number + 0.5));
/*
  return ((int) (number + 0.5 - (float) (number < 0.0)));
*/
}

  float
fround(float number)
{
  return((float) (int) (number + 0.5 - (number < 0.0)));
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                  NUMBER COMPARISON ROUTINE                             */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* compare doubles.  If a within tol dist of b, return 0, else */
/* return 1 if a > b, -1 if a < b.*/
  int
compare(double a,double b,double tol)
{
  double delta;
  ;
  delta = a - b;
  if (delta < 0.0)
  {
    if (delta > -tol)
      return(0);
  }
  else if (delta < tol)
    return (0);

  if (a > b)
    return(1);
  else
    return(-1);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                         VECTOR MATH ROUTINES                           */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
setscreenpos(screenpos pos,int x,int y)
{
  pos[vx] = x;
  pos[vy] = y;
}

/* Returns true if two vertypes are the same */

  Boolean
pos_equal(vertype v1, vertype v2)
{
  return ((v1[vx] == v2[vx]) && (v1[vy] == v2[vy]) && (v1[vz] == v2[vz]));
}

  void
setpos3d(float *pos, Coord x, Coord y, Coord z)
{
  *(pos++) = x;
  *(pos++) = y;
  *(pos++) = z;
  *pos = 1.0;
}

  void
copypos3d(vertype v1, vertype v2)
{
  v2[vx] = v1[vx];
  v2[vy] = v1[vy];
  v2[vz] = v1[vz];
  v2[vw] = 1.0;
}

  void
addpos3d(register vertype pos, register vertype delta, vertype result)
{
  result[vx] = pos[vx] + delta[vx];
  result[vy] = pos[vy] + delta[vy];
  result[vz] = pos[vz] + delta[vz];
  result[vw] = 1.0;
}

  void
diffpos3d(vertype pt1,vertype pt2,vertype result)
  /* returns position vector between 3d coords in pt1 and pt2 */
{
  result[vx] = pt2[vx] - pt1[vx];
  result[vy] = pt2[vy] - pt1[vy]; 
  result[vz] = pt2[vz] - pt1[vz];
  result[vw] = 1.0;
}

  void
setvec3d(vertype vec, Coord x, Coord y, Coord z)
{
  vec[vx] = x;
  vec[vy] = y;
  vec[vz] = z;
  vec[vw] = 0.0;
}

  void
makevec3d(vertype pt1, vertype pt2, vertype result)
{
  result[vx] = pt2[vx] - pt1[vx];
  result[vy] = pt2[vy] - pt1[vy];
  result[vz] = pt2[vz] - pt1[vz];
  result[vw] = 0.0;		/* 0.0 since its a vector */
}

  void
copyvec3d(vertype v1, vertype v2)
{
  v2[vx] = v1[vx];
  v2[vy] = v1[vy];
  v2[vz] = v1[vz];
  v2[vw] = 0.0;
}

  void
addvec3d(vertype vec1, vertype vec2, vertype result)
{
  result[vx] = vec1[vx] + vec2[vx];
  result[vy] = vec1[vy] + vec2[vy]; 
  result[vz] = vec1[vz] + vec2[vz];
  result[vw] = 0.0;
}

  void
diffvec3d(vertype pt1, vertype pt2, vertype result)
  /* returns position vector between 3d coords in pt1 and pt2 */
{
  result[vx] = pt2[vx] - pt1[vx];
  result[vy] = pt2[vy] - pt1[vy]; 
  result[vz] = pt2[vz] - pt1[vz];
  result[vw] = 0.0;
}

  void
multvec3d(vertype vec1, vertype vec2, vertype result)
{
  result[vx] = vec1[vx] * vec2[vx];
  result[vy] = vec1[vy] * vec2[vy]; 
  result[vz] = vec1[vz] * vec2[vz];
  result[vw] = 0.0;
}

  void
scalevec3d(vertype vec1, float scalefactor, vertype result)
{
  result[vx] = vec1[vx] * scalefactor;
  result[vy] = vec1[vy] * scalefactor; 
  result[vz] = vec1[vz] * scalefactor;
  result[vw] = 0.0;
}

  Coord
dotprod(Coord ax, Coord ay, Coord az,
	Coord bx, Coord by, Coord bz)
{
  return ( (Coord) ((ax*bx) + (ay*by) + (az*bz)) );
}

  Coord
dotvecs(vertype vec1, vertype vec2)
{
  Coord sum;
  ;
  sum = (vec1[vx] * vec2[vx]) + (vec1[vy] * vec2[vy]) + (vec1[vz] * vec2[vz]);
  return(sum);
}

/* this is bogus somehow */
/*
  Coord
dotvecs(Coord *vec1,Coord *vec2)
{
  Coord sum;
  ;
  sum = (*(vec1++)) * (*(vec2++));
  sum += (*(vec1++)) * (*(vec2++));
  sum += (*vec1) * (*vec2);
  return (sum);
}
*/

  void
cross_prod(float *v1, float *v2, float *v1Xv2)
{
  v1Xv2[vx] =    (v1[vy] * v2[vz]) - (v1[vz] * v2[vy]);
  v1Xv2[vy] = - ((v1[vx] * v2[vz]) - (v1[vz] * v2[vx]));
  v1Xv2[vz] =    (v1[vx] * v2[vy]) - (v1[vy] * v2[vx]);
  v1Xv2[vw] = 0.0;
}

  Coord
mag(Coord deltax, Coord deltay, Coord deltaz)
{
  return ((Coord) Sqrt(deltax*deltax + deltay*deltay + deltaz*deltaz));
}

  Coord
magvec(vertype thevec)
{
  return ((Coord) Sqrt(thevec[vx]*thevec[vx] +
		       thevec[vy]*thevec[vy] +
		       thevec[vz]*thevec[vz]));
}

  void
flip_vec(vertype thevec)
{
  thevec[vx] = -thevec[vx];
  thevec[vy] = -thevec[vy];
  thevec[vz] = -thevec[vz];
}

void
force_vec_up(vertype thevec, int axis)
{
  if (thevec[axis] < 0.0)
    flip_vec(thevec);
}

  Boolean
vector_near_zero(float *thevec,double zerotol)
{
  return ( (compare(thevec[vx],0.0,zerotol) == 0) &&
	  (compare(thevec[vy],0.0,zerotol) == 0) &&
	  (compare(thevec[vz],0.0,zerotol) == 0));
}

  Boolean
vec_eq_zero(float *thevec)
{
  return ((thevec[vx] == 0.0) &&
	  (thevec[vy] == 0.0) &&
	  (thevec[vz] == 0.0));
}

  Boolean
vectors_nearly_equal(float *thevec1, float *thevec2, double zerotol)
{
  return ( (compare(thevec1[vx],thevec2[vx],zerotol) == 0) &&
	  (compare(thevec1[vy],thevec2[vy],zerotol) == 0) &&
	  (compare(thevec1[vz],thevec2[vz],zerotol) == 0));
}

  Coord
distance(vertype pt1, vertype pt2)
{
  return ((Coord) Sqrt((pt2[vx] - pt1[vx]) * (pt2[vx] - pt1[vx]) + 
		       (pt2[vy] - pt1[vy]) * (pt2[vy] - pt1[vy]) +
		       (pt2[vz] - pt1[vz]) * (pt2[vz] - pt1[vz])));
}

  Coord
distance2(vertype pt1, vertype pt2)
				/* eventually change distance, above */
{
  return ((Coord) (pt2[vx] - pt1[vx]) * (pt2[vx] - pt1[vx]) + 
	  (pt2[vy] - pt1[vy]) * (pt2[vy] - pt1[vy]) +
	  (pt2[vz] - pt1[vz]) * (pt2[vz] - pt1[vz]));
}

  void
norm2pt(vertype p1, vertype p2, vertype p3)
{  /* return the normalized vector p1,p2 centered at origin */
  vertype atorigin;
  Coord length;
  ;
  diffpos3d(p1,p2,atorigin);
  length = distance(p1,p2);
  if (length > 0.0)
  {
    p3[vx] = atorigin[vx] / length;
    p3[vy] = atorigin[vy] / length;
    p3[vz] = atorigin[vz] / length;
  }
  else
  {
    p3[vx] = 0.0;
    p3[vy] = 0.0;
    p3[vz] = 0.0;
  }
  p3[vw] = 1.0;
}

  void
normalize(vertype v1, vertype v2)
  /* normalizes a vector v1, centered at origin, in vect v2 */
{
  Coord length;
  ;
  length = distance(origin,v1);
  if (length > 0.0) /* as long as not the zero vector, normalize, giles! */
  {               /* nothing from nothing, leaves zero, ok bub? */
    v2[vx] = v1[vx] / length;
    v2[vy] = v1[vy] / length;
    v2[vz] = v1[vz] / length;
    v2[vw] = 0.0;
  }
  else
    setvec3d(v2,0.0,0.0,0.0);
}

  void
midpt(Coord x1, Coord y1, Coord z1, Coord x2, Coord y2, Coord z2,
      Coord *midx, Coord *midy, Coord *midz)
{
  *midx = (x1 + x2)/ 2.0;
  *midy = (y1 + y2)/ 2.0;
  *midz = (z1 + z2)/ 2.0;
}

  void
midpoint(vertype pt1, vertype pt2, vertype mid_pt)
{
  mid_pt[vx] = (pt1[vx] + pt2[vx]) / 2.0;
  mid_pt[vy] = (pt1[vy] + pt2[vy]) / 2.0;
  mid_pt[vz] = (pt1[vz] + pt2[vz]) / 2.0;
  mid_pt[vw] = 1.0;
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                         MATRIX MATH ROUTINES                           */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

void
transpose_matrix(Matrix thematrix, Matrix thetranspose)
{
  float temp;
  int i,j;
  ;
  if (thematrix == thetranspose)
    for (i=0; i<3; i++)
      for (j=i+1; j<4; j++)
      {
	temp = thematrix[i][j];
	thematrix[i][j] = thematrix[j][i];
	thematrix[j][i] = temp;
      }
  else
    for (i=0; i<4; i++)
      for (j=0; j<4; j++)
	thetranspose[i][j] = thematrix[j][i];
}

  void
copy_matrix(Matrix a, Matrix b)
{
  int i,j;
  ;
  for (i=0; i < 4; i++)
    for (j=0; j < 4; j++)
      b[i][j] = a[i][j];
}
  
  void
multmatrix_2x2(Matrix m1, Matrix m2, Matrix m3)
{
  m3[0][0] = (m1[0][0] * m2[0][0]) + (m1[0][1] * m2[1][0]);
  m3[0][1] = (m1[0][0] * m2[0][1]) + (m1[0][1] * m2[1][1]);
  m3[1][0] = (m1[1][0] * m2[0][0]) + (m1[1][1] * m2[1][0]);
  m3[1][1] = (m1[1][0] * m2[0][1]) + (m1[1][1] * m2[1][1]);
}

  Boolean			/* return FALSE if inversion not possible */
invertmatrix_2x2(Matrix m, Matrix minv)
{
  double det;
  ;
  det = (m[0][0] * m[1][1]) - (m[0][1] * m[1][0]);
  if (less_than_tol(det,Floatol))
    return(FALSE);

  minv[0][0] = m[1][1] / det;
  minv[0][1] = -m[0][1] / det;
  minv[1][0] = -m[1][0] / det;
  minv[1][1] = m[0][0] / det;
  return(TRUE);
}

  void
multmatrix_4x4(Matrix matrixa, Matrix matrixb, Matrix matrixc)
{
  int i,j;
  float *ma,*mb,*mc;
  Matrix temp;
  ;
  ma = (float *) matrixa;
  mb = (float *) matrixb;
  mc = (float *) matrixc;
  if (mc == mb)
  {
    copy_matrix(matrixb,temp);
    for (i=0; i<4; i++)
    {
      matrixc[i][0] = matrixa[i][0]*temp[0][0] + matrixa[i][1]*temp[1][0] +
	              matrixa[i][2]*temp[2][0] + matrixa[i][3]*temp[3][0];

      matrixc[i][1] = matrixa[i][0]*temp[0][1] + matrixa[i][1]*temp[1][1] +
	              matrixa[i][2]*temp[2][1] + matrixa[i][3]*temp[3][1];

      matrixc[i][2] = matrixa[i][0]*temp[0][2] + matrixa[i][1]*temp[1][2] +
	              matrixa[i][2]*temp[2][2] + matrixa[i][3]*temp[3][2];

      matrixc[i][3] = matrixa[i][0]*temp[0][3] + matrixa[i][1]*temp[1][3] +
	              matrixa[i][2]*temp[2][3] + matrixa[i][3]*temp[3][3];
    }
  }
  else if (mc == ma)
  {
    copy_matrix(matrixa,temp);
    for (i=0; i<4; i++)
    {
      matrixc[i][0] = temp[i][0]*matrixb[0][0] + temp[i][1]*matrixb[1][0] +
	              temp[i][2]*matrixb[2][0] + temp[i][3]*matrixb[3][0];

      matrixc[i][1] = temp[i][0]*matrixb[0][1] + temp[i][1]*matrixb[1][1] +
	              temp[i][2]*matrixb[2][1] + temp[i][3]*matrixb[3][1];

      matrixc[i][2] = temp[i][0]*matrixb[0][2] + temp[i][1]*matrixb[1][2] +
	              temp[i][2]*matrixb[2][2] + temp[i][3]*matrixb[3][2];

      matrixc[i][3] = temp[i][0]*matrixb[0][3] + temp[i][1]*matrixb[1][3] +
	              temp[i][2]*matrixb[2][3] + temp[i][3]*matrixb[3][3];
    }
  }
  else
  {
    for (i=0; i<4; i++)
    {
      matrixc[i][0] = matrixa[i][0]*matrixb[0][0] + matrixa[i][1]*matrixb[1][0] +
	              matrixa[i][2]*matrixb[2][0] + matrixa[i][3]*matrixb[3][0];

      matrixc[i][1] = matrixa[i][0]*matrixb[0][1] + matrixa[i][1]*matrixb[1][1] +
	              matrixa[i][2]*matrixb[2][1] + matrixa[i][3]*matrixb[3][1];

      matrixc[i][2] = matrixa[i][0]*matrixb[0][2] + matrixa[i][1]*matrixb[1][2] +
	              matrixa[i][2]*matrixb[2][2] + matrixa[i][3]*matrixb[3][2];

      matrixc[i][3] = matrixa[i][0]*matrixb[0][3] + matrixa[i][1]*matrixb[1][3] +
	              matrixa[i][2]*matrixb[2][3] + matrixa[i][3]*matrixb[3][3];
    }
  }
#if 0
  if (mc == mb)
  {
    for (j=0; j<4; ++j)
    {
      temp[0] = (*(ma)) * (*(mb))+
                   (*(ma+1)) * (*(mb+4))+
	           (*(ma+2)) * (*(mb+8))+
	           (*(ma+3)) * (*(mb+12));

      temp[1] = (*(ma+4)) * (*(mb))+
                     (*(ma+4+1)) * (*(mb+4))+
	             (*(ma+4+2)) * (*(mb+8))+
	             (*(ma+4+3)) * (*(mb+12));

      temp[2] = (*(ma+8)) * (*(mb))+
                     (*(ma+8+1)) * (*(mb+4))+
	             (*(ma+8+2)) * (*(mb+8))+
	             (*(ma+8+3)) * (*(mb+12));

      temp[3] = (*(ma+12)) * (*(mb))+
                      (*(ma+12+1)) * (*(mb+4))+
	              (*(ma+12+2)) * (*(mb+8))+
	              (*(ma+12+3)) * (*(mb+12));
      mc[0] = temp[0];
      mc[4] = temp[1];
      mc[8] = temp[2];
      mc[12] = temp[3];
      ++mb;
      ++mc;
    }
  }
  else if (mc == ma)
  {
    for (i=0; i<16; i+=4)
    {
      temp[0] = (*(ma)) * (*(mb))+
                   (*(ma+1)) * (*(mb+4))+
	           (*(ma+2)) * (*(mb+8))+
		   (*(ma+3)) * (*(mb+12));

      temp[1] = (*(ma)) * (*(mb+1))+
                     (*(ma+1)) * (*(mb+5))+
	             (*(ma+2)) * (*(mb+9))+
		     (*(ma+3)) * (*(mb+13));

      temp[2] = (*(ma)) * (*(mb+2))+
                     (*(ma+1)) * (*(mb+6))+
                     (*(ma+2)) * (*(mb+10))+
	             (*(ma+3)) * (*(mb+14));

      temp[3] = (*(ma)) * (*(mb+3))+
	             (*(ma+1)) * (*(mb+7))+
                     (*(ma+2)) * (*(mb+11))+
                     (*(ma+3)) * (*(mb+15));
      mc[0] = temp[0];
      mc[1] = temp[1];
      mc[2] = temp[2];
      mc[3] = temp[3];
      ++ma;
      ++mc;
    }
  }
  else
  {
    for (j=0; j<4; ++j)
    {
      *(mc) = (*(ma)) * (*(mb))+
                   (*(ma+1)) * (*(mb+4))+
	           (*(ma+2)) * (*(mb+8))+
	           (*(ma+3)) * (*(mb+12));

      *(mc+4) = (*(ma+4)) * (*(mb))+
                     (*(ma+4+1)) * (*(mb+4))+
	             (*(ma+4+2)) * (*(mb+8))+
	             (*(ma+4+3)) * (*(mb+12));

      *(mc+8) = (*(ma+8)) * (*(mb))+
                     (*(ma+8+1)) * (*(mb+4))+
	             (*(ma+8+2)) * (*(mb+8))+
	             (*(ma+8+3)) * (*(mb+12));

      *(mc+12) = (*(ma+12)) * (*(mb))+
                      (*(ma+12+1)) * (*(mb+4))+
	              (*(ma+12+2)) * (*(mb+8))+
	              (*(ma+12+3)) * (*(mb+12));
      ++mb;
      ++mc;
    }
  }
#endif

}

  Boolean
matrices_equal(Matrix a, Matrix b,double tol)
{
  int i,j;
  ;
  for (i = 0; i < 4; ++i)
    for (j = 0; j < 4; ++j)
      if (compare(a[i][j],b[i][j],tol) != 0)
	return(FALSE);
  return(TRUE);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*                       GAUSSIAN ELIMINATION                             */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

  void
swap_ints(int *val1,int *val2)
{
  int temp;
  ;
  temp = *val2;
  *val2 = *val1;
  *val1 = temp;
}

  void
swap_coords(Coord *val1,Coord *val2)
{
  Coord temp;
  ;
  temp = *val2;
  *val2 = *val1;
  *val1 = temp;
}

  void
swap_verts(vertype pt1,vertype pt2)
{
  swap_coords(&pt1[vx],&pt2[vx]);
  swap_coords(&pt1[vy],&pt2[vy]);
  swap_coords(&pt1[vz],&pt2[vz]);
}

  static void
pivot(Matrix elimatrix,int i)
{
  int j,k,mark;
  double max = 0.0;		/* find row with maximum left col val  */
  ;
  mark = i;	/* assume this row has biggest left col val. */
  for (j = i; j < 4; ++j)
    if (Fabs(elimatrix[j][0]) > max)
    {
      mark = j;
      max = Fabs(elimatrix[j][0]);
    }
  if (mark != i)
    for (k = 0; k < 4; ++k)
      swap_coords(&(elimatrix[mark][k]),&(elimatrix[i][k])); /*swap two rows.*/
}
    
  static void
gauss_process(Matrix elimatrix,int i)
{
  int j,k,pivot;
  double reduction;
  ;
  pivot = 0;
  while ((compare(elimatrix[i][pivot],0.0,Pttopttol) == 0) && (pivot < 4))
    pivot++;
  if (pivot == 4)
    system_error("gauss_process: SINGULAR MATRIX!");
  for (j = i+1; j < 4; ++j)
  {
    reduction = elimatrix[j][pivot] / elimatrix[i][pivot];
    for (k = 0; k < 4; ++k)
      elimatrix[j][k] -= elimatrix[i][k] * reduction;
  }
}

/* Simple gaussian elimination of a matrix,  with pivoting.  Called */
/* by lines_intersect to determine params of intersection of two lines.*/
/* Doesn't check for singularity or inf. # of solutions so you'd better */
/* not abuse it! */

  void
gaussian_elim(Matrix elimatrix)
{
  int i;
  ;
  for (i = 0; i < 3; ++i)
  {
    pivot(elimatrix,i);		/* swap in row with greatest abs value */
				/* in leftmost column.*/
    gauss_process(elimatrix,i);	/* Then process through lower rows. */
  }
}
