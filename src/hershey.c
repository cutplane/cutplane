
/* FILE: hershey.c */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                        */
/*    3-D Text Drawing Routines for 3Form Design System:Hershey Font      */
/*                                                                        */
/* Author: WAK&LJE                                   Date: January 9,1991 */
/* Version: 1                                                             */
/*                                                                        */
/* Copyright (C) 1987,1988,1989,1990,1991 William Kessler and             */
/* Larry Edwards. All Rights Reserved.                                    */
/*                                                                        */
/*                                                                        */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define HERSHEYMODULE

#include <config.h>
#include <stdio.h>

#include <platform_gl.h>
#include <math.h>

#include <cutplane.h>
#include <grafx.h>
#include <text.h>

void A_def();
void B_def();
void C_def();
void D_def();
void E_def();
void F_def();
void G_def();
void H_def();
void I_def();
void J_def();
void K_def();
void L_def();
void M_def();
void N_def();
void O_def();
void P_def();
void Q_def();
void R_def();
void S_def();
void T_def();
void U_def();
void V_def();
void W_def();
void X_def();
void Y_def();
void Z_def();
void a_def();
void b_def();
void c_def();
void d_def();
void e_def();
void f_def();
void g_def();
void h_def();
void i_def();
void j_def();
void k_def();
void l_def();
void m_def();
void n_def();
void o_def();
void p_def();
void q_def();
void r_def();
void s_def();
void t_def();
void u_def();
void v_def();
void w_def();
void x_def();
void y_def();
void z_def();
void backslash_def();
void leftcurly_def();
void rightcurly_def();
void bar_def();
void exclamation_def();
void at_def();
void pound_def();
void dollar_def();
void percent_def();
void caret_def();
void ampersand_def();
void asterisk_def();
void leftparen_def();
void rightparen_def();
void underscore_def();
void minus_def();
void equal_def();
void plus_def();
void zero_def();
void one_def();
void two_def();
void three_def();
void four_def();
void five_def();
void six_def();
void seven_def();
void eight_def();
void nine_def();

void doublequote_def();
void quote_def();
void del_def();
void tilde_def();
void leftbrace_def();
void rightbrace_def();
void tick_def();
void comma_def();
void period_def();
void slash_def();
void colon_def();
void semicolon_def();
void lessthan_def();
void greaterthan_def();
void questionmark_def();
void delta_def();

/* characters currently missing: " ' ? ; : < > ~ ` , . / [ ] */


char_rec char_lookups[256] = {{10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {10.0,Nil},
			      {14.0,delta_def},
			      {10.0,Nil},			/* space */
				
			      {3.6363,exclamation_def},
			      {7.99988,doublequote_def},
			      {10.1819,pound_def},
			      {9.09087,dollar_def},
			      {11.8182,percent_def},
			      {12.3637,ampersand_def},
			      {3.6363,quote_def},
			      {6.36359,leftparen_def},
			      {6.36377,rightparen_def},
			      {7.45457,asterisk_def},
			      {11.2727,plus_def},
			      {3.6363,comma_def},
			      {6.36359,minus_def},
			      {3.6363,period_def},
			      {9.63618,slash_def},
				
			      {9.63637,zero_def},
			      {4.72729,one_def},
			      {9.63637,two_def},
			      {9.63637,three_def},
			      {10.1819,four_def},
			      {9.63637,five_def},
			      {9.09087,six_def},
			      {9.63637,seven_def},
			      {9.63637,eight_def},
			      {9.09087,nine_def},
				
			      {3.6363,colon_def},
			      {3.6363,semicolon_def},
			      {10.7272,lessthan_def},
			      {11.2727,equal_def},
			      {10.7272,greaterthan_def},
			      {9.09087,questionmark_def},
			      {5.81809,at_def},
				
			      {10.7274,A_def},
			      {9.09087,B_def},
			      {10.1819,C_def},
			      {9.63637,D_def},
			      {8.54538,E_def},
			      {8.54538,F_def},
			      {10.1819,G_def},
			      {9.63637,H_def},
			      {2.54549,I_def},
			      {7.45457,J_def},
			      {9.63637,K_def},
			      {8.54538,L_def},
			      {10.7272,M_def},
			      {9.63637,N_def},
			      {10.7274,O_def},
			      {9.09087,P_def},
			      {10.7274,Q_def},
			      {9.09087,R_def},
			      {9.63637,S_def},
			      {9.09087,T_def},
			      {9.63637,U_def},
			      {10.7274,V_def},
			      {14.0001,W_def},
			      {9.63637,X_def},
			      {10.1819,Y_def},
			      {9.63637,Z_def},
				
			      {5.81809,leftbrace_def},
			      {9.6361810,backslash_def},
			      {5.81809,rightbrace_def},
			      {10.7272,caret_def},
			      {10.0,underscore_def},
			      {3.6363,tick_def},
				
			      {9.09087,a_def},
			      {9.09087,b_def},
			      {8.54538,c_def},
			      {9.09087,d_def},
			      {8.54538,e_def},
			      {6.36359,f_def},
			      {9.09087,g_def},
			      {8.54538,h_def},
			      {3.6363,i_def},
			      {4.6363,j_def},
			      {8.54538,k_def},
			      {3.54549,l_def},
			      {13.5454,m_def},
			      {8.54538,n_def},
			      {9.09087,o_def},
			      {9.09087,p_def},
			      {9.09087,q_def},
			      {6.90908,r_def},
			      {8.00007,s_def},
			      {5.81827,t_def},
			      {8.54538,u_def},
			      {8.54556,v_def},
			      {11.8182,w_def},
			      {8.54538,x_def},
			      {8.54556,y_def},
			      {8.54538,z_def},
				
			      {4.72729,leftcurly_def},
			      {2,bar_def},
			      {4.72729,rightcurly_def},
			      {11.8182,tilde_def},
			      {10,del_def}};


  void
del_def()			/* should be a box */
{
}

  void
tilde_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 1.6363; pos[vy] = 3.27266;
  glVertex2fv(pos);
  pos[vx] = 1.6363; pos[vy] = 4.36349;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 5.99992;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 6.54534;
  glVertex2fv(pos);
  pos[vx] = 4.36359; pos[vy] = 6.54534;
  glVertex2fv(pos);
  pos[vx] = 5.45439; pos[vy] = 5.99992;
  glVertex2fv(pos);
  pos[vx] = 7.63618; pos[vy] = 4.36349;
  glVertex2fv(pos);
  pos[vx] = 8.72717; pos[vy] = 3.81807;
  glVertex2fv(pos);
  pos[vx] = 9.81816; pos[vy] = 3.81807;
  glVertex2fv(pos);
  pos[vx] = 10.909; pos[vy] = 4.36349;
  glVertex2fv(pos);
  pos[vx] = 11.4545; pos[vy] = 5.45451;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 1.6363; pos[vy] = 4.36349;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 5.45451;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 5.99992;
  glVertex2fv(pos);
  pos[vx] = 4.36359; pos[vy] = 5.99992;
  glVertex2fv(pos);
  pos[vx] = 5.45439; pos[vy] = 5.45451;
  glVertex2fv(pos);
  pos[vx] = 7.63618; pos[vy] = 3.81807;
  glVertex2fv(pos);
  pos[vx] = 8.72717; pos[vy] = 3.27266;
  glVertex2fv(pos);
  pos[vx] = 9.81816; pos[vy] = 3.27266;
  glVertex2fv(pos);
  pos[vx] = 10.909; pos[vy] = 3.81807;
  glVertex2fv(pos);
  pos[vx] = 11.4545; pos[vy] = 5.45451;
  glVertex2fv(pos);
  pos[vx] = 11.4545; pos[vy] = 6.54534;
  glVertex2fv(pos);
  glEnd();
}

  void
doublequote_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 11.4545;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 7.63617;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 7.63617;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 11.4545;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 7.63617;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 7.63618; pos[vy] = 11.4545;
  glVertex2fv(pos);
  pos[vx] = 7.09087; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 7.09087; pos[vy] = 7.63617;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 7.63618; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 7.09087; pos[vy] = 7.63617;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 7.63618; pos[vy] = 11.4545;
  glVertex2fv(pos);
  pos[vx] = 8.18168; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 7.09087; pos[vy] = 7.63617;
  glVertex2fv(pos);
  glEnd();
}

  void
quote_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 3.81809; pos[vy] = 10.3634;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 9.81802;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 9.81802;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 10.3634;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 11.4545;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 11.4545;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = 9.27261;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 8.18177;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 7.63617;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 10.3634;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 10.3634;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 3.2726; pos[vy] = 9.81802;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = 9.27261;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 3.81809; pos[vy] = 10.3634;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 8.18177;
  glVertex2fv(pos);
  glEnd();
}

  void
leftbrace_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.18179; pos[vy] = 13.6363;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = -3.81813;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 13.6363;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = -3.81813;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.18179; pos[vy] = 13.6363;
  glVertex2fv(pos);
  pos[vx] = 5.99989; pos[vy] = 13.6363;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.18179; pos[vy] = -3.81813;
  glVertex2fv(pos);
  pos[vx] = 5.99989; pos[vy] = -3.81813;
  glVertex2fv(pos);
  glEnd();
}

  void
rightbrace_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 4.90908; pos[vy] = 13.6363;
  glVertex2fv(pos);
  pos[vx] = 4.90908; pos[vy] = -3.81813;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 5.45439; pos[vy] = 13.6363;
  glVertex2fv(pos);
  pos[vx] = 5.45439; pos[vy] = -3.81813;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 1.6363; pos[vy] = 13.6363;
  glVertex2fv(pos);
  pos[vx] = 5.45439; pos[vy] = 13.6363;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 1.6363; pos[vy] = -3.81813;
  glVertex2fv(pos);
  pos[vx] = 5.45439; pos[vy] = -3.81813;
  glVertex2fv(pos);
  glEnd();
}

  void
tick_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 3.81809; pos[vy] = 11.4545;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 9.81802;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 8.18177;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 7.63617;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 7.63617;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = 8.18177;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = 8.72719;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 9.27261;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 9.27261;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 8.72719;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 8.72719;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 8.18177;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 8.18177;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 8.72719;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 8.72719;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 8.72719;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.18179; pos[vy] = 9.81802;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 9.27261;
  glVertex2fv(pos);
  glEnd();
}

  void
comma_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 3.81809; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = -2.72989e-05;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = -2.72989e-05;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 1.63622;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 1.63622;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = -.545444;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = -1.63628;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = -2.18169;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 1.09081;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 3.2726; pos[vy] = -2.72989e-05;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = -.545444;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 3.81809; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = -1.63628;
  glVertex2fv(pos);
  glEnd();
}

  void
period_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 1.63622;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = -2.72989e-05;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = -2.72989e-05;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 1.63622;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 1.09081;
  glVertex2fv(pos);
  glEnd();
}

  void
slash_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 0; pos[vy] = -1.63628;
  glVertex2fv(pos);
  pos[vx] = 7.63618; pos[vy] = 11.4545;
  glVertex2fv(pos);
  glEnd();
}

  void
colon_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 7.63617;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 7.09076;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 6.54534;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 5.99992;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 5.99992;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = 6.54534;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = 7.09076;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 7.63617;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 7.63617;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 7.09076;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 6.54534;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 6.54534;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 7.09076;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 7.09076;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 1.63622;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = -2.72989e-05;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = -2.72989e-05;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 1.63622;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 1.09081;
  glVertex2fv(pos);
  glEnd();
}

  void
semicolon_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 7.63617;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 7.09076;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 6.54534;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 5.99992;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 5.99992;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = 6.54534;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = 7.09076;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 7.63617;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 7.63617;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 7.09076;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 6.54534;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 6.54534;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 7.09076;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 7.09076;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 3.81809; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = -2.72989e-05;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = -2.72989e-05;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 1.63622;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 1.63622;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = -.545444;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = -1.63628;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = -2.18169;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.7271; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 1.09081;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 3.2726; pos[vy] = -2.72989e-05;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = -.545444;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 3.81809; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 3.2726; pos[vy] = -1.63628;
  glVertex2fv(pos);
  glEnd();
}

  void
lessthan_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 10.909; pos[vy] = 9.81802;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 4.90891;
  glVertex2fv(pos);
  pos[vx] = 10.909; pos[vy] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
greaterthan_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.18179; pos[vy] = 9.81802;
  glVertex2fv(pos);
  pos[vx] = 10.909; pos[vy] = 4.90891;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
questionmark_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 1.6363; pos[vy] = 8.72719;
  glVertex2fv(pos);
  pos[vx] = 1.6363; pos[vy] = 9.27261;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 10.3634;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 4.36359; pos[vy] = 11.4545;
  glVertex2fv(pos);
  pos[vx] = 5.99989; pos[vy] = 11.4545;
  glVertex2fv(pos);
  pos[vx] = 7.63618; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 8.18168; pos[vy] = 10.3634;
  glVertex2fv(pos);
  pos[vx] = 8.72717; pos[vy] = 9.27261;
  glVertex2fv(pos);
  pos[vx] = 8.72717; pos[vy] = 8.18177;
  glVertex2fv(pos);
  pos[vx] = 8.18168; pos[vy] = 7.09076;
  glVertex2fv(pos);
  pos[vx] = 7.63618; pos[vy] = 6.54534;
  glVertex2fv(pos);
  pos[vx] = 6.54538; pos[vy] = 5.99992;
  glVertex2fv(pos);
  pos[vx] = 4.90908; pos[vy] = 5.45451;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 1.6363; pos[vy] = 8.72719;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 8.72719;
  glVertex2fv(pos);
  pos[vx] = 2.18179; pos[vy] = 9.27261;
  glVertex2fv(pos);
  pos[vx] = 2.7271; pos[vy] = 10.3634;
  glVertex2fv(pos);
  pos[vx] = 4.36359; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 5.99989; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 7.63618; pos[vy] = 10.3634;
  glVertex2fv(pos);
  pos[vx] = 8.18168; pos[vy] = 9.27261;
  glVertex2fv(pos);
  pos[vx] = 8.18168; pos[vy] = 8.18177;
  glVertex2fv(pos);
  pos[vx] = 7.63618; pos[vy] = 7.09076;
  glVertex2fv(pos);
  pos[vx] = 6.54538; pos[vy] = 6.54534;
  glVertex2fv(pos);
  pos[vx] = 4.90908; pos[vy] = 5.99992;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 2.18179; pos[vy] = 9.81802;
  glVertex2fv(pos);
  pos[vx] = 3.81809; pos[vy] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 6.54538; pos[vy] = 10.909;
  glVertex2fv(pos);
  pos[vx] = 8.18168; pos[vy] = 9.81802;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 8.18168; pos[vy] = 7.63617;
  glVertex2fv(pos);
  pos[vx] = 5.99989; pos[vy] = 5.99992;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 4.90908; pos[vy] = 5.99992;
  glVertex2fv(pos);
  pos[vx] = 4.90908; pos[vy] = 3.81807;
  glVertex2fv(pos);
  pos[vx] = 5.45439; pos[vy] = 3.81807;
  glVertex2fv(pos);
  pos[vx] = 5.45439; pos[vy] = 5.99992;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 4.90908; pos[vy] = 1.63622;
  glVertex2fv(pos);
  pos[vx] = 4.36359; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 4.36359; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 4.90908; pos[vy] = -2.72989e-05;
  glVertex2fv(pos);
  pos[vx] = 5.45439; pos[vy] = -2.72989e-05;
  glVertex2fv(pos);
  pos[vx] = 5.99989; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 5.99989; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 5.45439; pos[vy] = 1.63622;
  glVertex2fv(pos);
  pos[vx] = 4.90908; pos[vy] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[vx] = 4.90908; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 4.90908; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 5.45439; pos[vy] = .54539;
  glVertex2fv(pos);
  pos[vx] = 5.45439; pos[vy] = 1.09081;
  glVertex2fv(pos);
  pos[vx] = 4.90908; pos[vy] = 1.09081;
  glVertex2fv(pos);
  glEnd();
}


  void
exclamation_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.7271;   pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.7271;   pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 3.2726;   pos[1] = 3.81807;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.7271;   pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.2726;   pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.2726;   pos[1] = 3.81807;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] =2.7271;    pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  glEnd();
}



  void
A_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.90908;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = .545311;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.90908;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = .545311;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.90908;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.90908;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 3.27266;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 2.72724;
  glVertex2fv(pos);
  glEnd();
}

  void
B_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 5.45451;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 5.99992;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
}

  void
C_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.27267;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 2.72724;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.27267;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 2.72724;
  glVertex2fv(pos);
  glEnd();
}

  void
D_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
}

  void
E_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 11.4545;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 11.4545;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 5.45451;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 5.45451;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
F_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 11.4545;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 11.4545;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 5.45451;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 5.45451;
  glVertex2fv(pos);
  glEnd();
}

  void
G_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.27267;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 4.90891;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.27267;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 4.90891;
  glVertex2fv(pos);
  glEnd();
}

  void
H_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.27267;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.27267;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 5.99992;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 5.45451;
  glVertex2fv(pos);
  glEnd();
}

  void
I_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
J_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.99989;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 2.72724;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.99989;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 2.72724;
  glVertex2fv(pos);
  glEnd();
}

  void
K_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.27267;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 4.90891;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.27267;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 4.36349;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.81809;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.36359;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
L_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
M_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 10.3635;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.81816;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.81816;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 10.3635;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 10.3635;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 10.3635;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
N_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.72717;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.72717;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
O_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.90908;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
}

  void
P_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 5.45451;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  glEnd();
}

  void
Q_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.90908;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.99989;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = -1.09086;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = -1.09086;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.99989;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = -1.09086;
  glVertex2fv(pos);
  glEnd();
}

  void
R_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 5.45451;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.90908;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.45439;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
S_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.72717;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.72717;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
}

  void
T_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.81809;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.36359;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = .545311;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = .545311;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = .545311;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
}

  void
U_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 11.4545;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 11.4545;
  glVertex2fv(pos);
  glEnd();
}

  void
V_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = .545311;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = .545311;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.27267;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.27267;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
W_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = .545311;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = .545311;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 6.54538;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 6.54538;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 6.54538;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 6.54538;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 12.5454;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 12;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 12.5454;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
X_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.72717;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.72717;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
Y_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = .545311;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = .545311;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 5.99992;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.72717;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 5.99992;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.72717;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
Z_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.18168;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.72717;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 11.4545;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
a_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
}

  void
b_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
}

  void
c_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
}

  void
d_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
}

  void
e_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
}

  void
f_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.45439;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.45439;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.3634;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.81809;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.09076;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.09076;
  glVertex2fv(pos);
  glEnd();
}

  void
g_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.18168;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -.545444;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = -2.18169;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = -2.72729;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -3.27271;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -3.27271;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = -2.72729;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = -2.18169;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.18169;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.18168;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -.545444;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -2.18169;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = -3.27271;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -3.81813;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -3.81813;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = -3.27271;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.18169;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
}

  void
h_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
i_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
j_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -3.81813;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
}

  void
k_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.18168;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.18164;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.18168;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.81809;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.36359;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
l_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
m_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.18168;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 10.909;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 12.5454;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 13.6363;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 14.1817;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 14.1817;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.18168;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 10.909;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 12.0;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 13.0908;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 13.6363;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 13.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 14.1817;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
n_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
o_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.81809;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.63617;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.81809;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.09076;
  glVertex2fv(pos);
  glEnd();
}

  void
p_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -3.81813;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
}

  void
q_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -3.81813;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
}

  void
r_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.63617;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.63617;
  glVertex2fv(pos);
  glEnd();
}

  void
s_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 2.72724;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.99989;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 6.54538;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 1.09081;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 7.09076;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 6.54538;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 6.54534;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 4.90891;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
}

  void
t_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.7271;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.7271;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.09076;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.09076;
  glVertex2fv(pos);
  glEnd();
}

  void
u_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 2.18164;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 2.18164;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
v_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = .545311;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = .545311;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 1.09081;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 1.09081;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
w_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.99989;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.99989;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.99989;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.99989;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 10.909;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 10.3635;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 10.909;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
x_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
y_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = .545311;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = .545311;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 1.09081;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -3.81813;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
}

  void
z_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 6.54538;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 7.63617;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.09076;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
backslash_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 0;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -1.63628;
  glVertex2fv(pos);
  glEnd();
}

  void
leftcurly_def()
 {
   float pos[2];
   ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
   glBegin(GL_LINE_STRIP);
   pos[0] = 4.90908;  pos[1] = 13.6363;
   glVertex2fv(pos);
   pos[0] = 3.81809;  pos[1] = 13.0907;
   glVertex2fv(pos);
   pos[0] = 3.2726;  pos[1] = 12.5453;
   glVertex2fv(pos);
   pos[0] = 2.7271;  pos[1] = 11.4545;
   glVertex2fv(pos);
   pos[0] = 2.7271;  pos[1] = 10.3634;
   glVertex2fv(pos);
   pos[0] = 3.2726;  pos[1] = 9.27261;
   glVertex2fv(pos);
   pos[0] = 3.81809;  pos[1] = 8.72719;
   glVertex2fv(pos);
   pos[0] = 4.36359;  pos[1] = 7.63617;
   glVertex2fv(pos);
   pos[0] = 4.36359;  pos[1] = 6.54534;
   glVertex2fv(pos);
   pos[0] = 3.2726;  pos[1] = 5.45451;
   glVertex2fv(pos);
   glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
   glBegin(GL_LINE_STRIP);
   pos[0] = 3.81809;  pos[1] = 13.0907;
   glVertex2fv(pos);
   pos[0] = 3.2726;  pos[1] = 11.9999;
   glVertex2fv(pos);
   pos[0] = 3.2726;  pos[1] = 10.909;
   glVertex2fv(pos);
   pos[0] = 3.81809;  pos[1] = 9.81802;
   glVertex2fv(pos);
   pos[0] = 4.36359;  pos[1] = 9.27261;
   glVertex2fv(pos);
   pos[0] = 4.90908;  pos[1] = 8.18177;
   glVertex2fv(pos);
   pos[0] = 4.90908;  pos[1] = 7.09076;
   glVertex2fv(pos);
   pos[0] = 4.36359;  pos[1] = 5.99992;
   glVertex2fv(pos);
   pos[0] = 2.18179;  pos[1] = 4.90891;
   glVertex2fv(pos);
   pos[0] = 4.36359;  pos[1] = 3.81807;
   glVertex2fv(pos);
   pos[0] = 4.90908;  pos[1] = 2.72724;
   glVertex2fv(pos);
   pos[0] = 4.90908;  pos[1] = 1.63622;
   glVertex2fv(pos);
   pos[0] = 4.36359;  pos[1] = .54539;
   glVertex2fv(pos);
   pos[0] = 3.81809;  pos[1] = -2.72989e-05;
   glVertex2fv(pos);
   pos[0] = 3.2726;  pos[1] = -1.09086;
   glVertex2fv(pos);
   pos[0] = 3.2726;  pos[1] = -2.18169;
   glVertex2fv(pos);
   pos[0] = 3.81809;  pos[1] = -3.27271;
   glVertex2fv(pos);
   glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
   glBegin(GL_LINE_STRIP);
   pos[0] = 3.2726;  pos[1] = 4.36349;
   glVertex2fv(pos);
   pos[0] = 4.36359;  pos[1] = 3.27266;
   glVertex2fv(pos);
   pos[0] = 4.36359;  pos[1] = 2.18164;
   glVertex2fv(pos);
   pos[0] = 3.81809;  pos[1] = 1.09081;
   glVertex2fv(pos);
   pos[0] = 3.2726;  pos[1] = .54539;
   glVertex2fv(pos);
   pos[0] = 2.7271;  pos[1] = -.545444;
   glVertex2fv(pos);
   pos[0] = 2.7271;  pos[1] = -1.63628;
   glVertex2fv(pos);
   pos[0] = 3.2726;  pos[1] = -2.72729;
   glVertex2fv(pos);
   pos[0] = 3.81809;  pos[1] = -3.27271;
   glVertex2fv(pos);
   pos[0] = 4.90908;  pos[1] = -3.81813;
   glVertex2fv(pos);
   glEnd();
 }
 
  void
rightcurly_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.7271;  pos[1] = 13.6363;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 13.0907;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 12.5453;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 5.45451;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.81809;  pos[1] = 13.0907;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 11.9999;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 8.18177;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -1.09086;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.18169;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -3.27271;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.36359;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = -.545444;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = -1.63628;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72729;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -3.27271;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
}

  void
bar_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 13.6363;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
}




  void
pound_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.99989;  pos[1] = 13.6363;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.27267;  pos[1] = 13.6363;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 6.54534;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 3.27266;
  glVertex2fv(pos);
  glEnd();
}

  void
dollar_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.90908;  pos[1] = 13.6363;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = -2.18169;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -2.18169;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.90908;  pos[1] = 13.6363;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 13.6363;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -2.18169;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.18168;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
}

  void
percent_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 11.4545;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 8.18177;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 10.3635;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 11.4545;  pos[1] = 11.4545;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.27267;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 10.909;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 11.4545;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 11.4545;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 10.3635;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 3.81807;
  glVertex2fv(pos);
  glEnd();
}

  void
caret_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 10.3635;  pos[1] = 3.81807;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 10.3635;  pos[1] = 3.81807;
  glVertex2fv(pos);
  glEnd();
}

  void
ampersand_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 12.0;    pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 10.909;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 10.909;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 12.0;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 12.0;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 12.0;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 10.909;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 6.54534;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 10.3635;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 10.3634;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.90908;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 10.909;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 12.0;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 12.0;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}


  void
leftparen_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.45439;  pos[1] = 13.6363;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 12.5453;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = -1.09086;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72729;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -3.81813;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.45439;  pos[1] = 13.6363;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 13.6363;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 12.5453;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -1.09086;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = -2.72729;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
}

  void
rightparen_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = .545311;  pos[1] = 13.6363;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 12.5453;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = -1.09086;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72729;
  glVertex2fv(pos);
  pos[0] = .545311;  pos[1] = -3.81813;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = .545311;  pos[1] = 13.6363;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = 13.6363;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 12.5453;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = -1.09086;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72729;
  glVertex2fv(pos);
  pos[0] = 1.09081;  pos[1] = -3.81813;
  glVertex2fv(pos);
  glEnd();
}


  void
asterisk_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 4.90891;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 4.90891;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 4.90891;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 6.54534;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 6.54534;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 6.54534;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 6.54534;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 6.54534;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 6.54534;
  glVertex2fv(pos);
  glEnd();
}

  void
underscore_def()
{
  float pos[2];
  ;
  glEnd();
}

  void
plus_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 6.54538;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 6.54538;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 11.4545;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 11.4545;  pos[1] = 4.90891;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 11.4545;  pos[1] = 4.90891;
  glVertex2fv(pos);
  glEnd();
}


  void
minus_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.09081;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 4.90891;
  glVertex2fv(pos);
  glEnd();
}

  void
zero_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.90908;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 11.4545;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.81809;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.2726;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.09081;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.3634;
  glVertex2fv(pos);
  glEnd();
}

  void
one_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.2726;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.2726;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
two_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 8.18177;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 8.18177;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
three_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.7271;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 6.54534;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.7271;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.18168;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 6.54534;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.90908;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.18164;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.36359;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 4.36349;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 6.54538;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.72717;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.18164;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
}

  void
four_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 2.72724;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.72724;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 2.72724;
  glVertex2fv(pos);
  glEnd();
}

  void
five_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.7271;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 6.54534;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.2726;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 7.09076;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.7271;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.2726;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.7271;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.18164;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 4.90891;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 6.54538;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.72717;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.18164;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
}

  void
six_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 5.45451;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.18168;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 10.3634;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.36359;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.7271;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 2.72724;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 6.54534;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.72717;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 4.36349;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.90908;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 3.81807;
  glVertex2fv(pos);
  glEnd();
}

  void
seven_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 1.6363;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.72717;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  glEnd();
}

  void
eight_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 11.4545;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 3.2726;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.18168;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 10.3634;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
}

  void
nine_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.18168;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 8.18177;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.63622;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.18168;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 4.90891;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 8.18168;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 7.09076;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 4.36359;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 8.18177;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 10.909;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 3.2726;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 7.09087;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 8.72719;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 5.99989;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 9.81802;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = .54539;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 7.09087;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 1.09081;
  glVertex2fv(pos);
  glEnd();
}


  void
equal_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 11.4545;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 11.4545;  pos[1] = 7.09076;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 11.4545;  pos[1] = 7.09076;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 11.4545;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 11.4545;  pos[1] = 2.72724;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 2.18179;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 11.4545;  pos[1] = 2.72724;
  glVertex2fv(pos);
  glEnd();
}

  void
at_def()
{
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.81816;  pos[1] = 7.09076;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 8.18177;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 8.18177;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 4.36359;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 7.63618;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 8.72717;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 4.36349;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 6.54538;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 7.63617;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 5.99992;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 5.45439;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 5.99989;  pos[1] = 2.72724;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 9.81816;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 9.27267;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 10.3635;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 11.4545;  pos[1] = 2.72724;
  glVertex2fv(pos);
  pos[0] = 12.5454;  pos[1] = 3.81807;
  glVertex2fv(pos);
  pos[0] = 13.0908;  pos[1] = 5.45451;
  glVertex2fv(pos);
  pos[0] = 13.0908;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 12.5454;  pos[1] = 8.18177;
  glVertex2fv(pos);
  pos[0] = 12;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 10.909;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = 11.4545;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = 10.909;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 10.3634;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 9.27261;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 8.18177;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 6.54534;
  glVertex2fv(pos);
  pos[0] = 1.6363;  pos[1] = 4.90891;
  glVertex2fv(pos);
  pos[0] = 2.18179;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 2.7271;  pos[1] = 2.18164;
  glVertex2fv(pos);
  pos[0] = 3.81809;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 4.90908;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 6.54538;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 8.18168;  pos[1] = -2.72989e-05;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = .54539;
  glVertex2fv(pos);
  pos[0] = 10.909;  pos[1] = 1.09081;
  glVertex2fv(pos);
  pos[0] = 11.4545;  pos[1] = 1.63622;
  glVertex2fv(pos);
  glEnd();
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 10.3635;  pos[1] = 8.72719;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 4.36349;
  glVertex2fv(pos);
  pos[0] = 9.81816;  pos[1] = 3.27266;
  glVertex2fv(pos);
  pos[0] = 10.3635;  pos[1] = 2.72724;
  glVertex2fv(pos);
  glEnd();
}


 void
delta_def()
{
  /* This is a delta symbol that i just made up from scratch for the distance */
  /* pads... */
  float pos[2];
  ;
	/* OGLXXX for multiple, independent line segments: use GL_LINES */
  glBegin(GL_LINE_STRIP);
  pos[0] = 0.0; pos[1] = 0.0;
  glVertex2fv(pos);
  pos[0] = 12.0; 
  glVertex2fv(pos);
  pos[0] = 6.0; pos[1] = 12.0;
  glVertex2fv(pos);
  pos[0] = 0.0; pos[1] = 0.0;
  glVertex2fv(pos);
  glEnd();
}


/* These values come from bar_def() above, assumed to be the tallest char. */

  GLfloat
hershey_Xheight()
{
  return(13.6383);
}

  GLfloat
hershey_descender_height()
{
  return(-3.81813);
}

  GLfloat
hershey_height()
{
  return(hershey_Xheight() - hershey_descender_height());
}
