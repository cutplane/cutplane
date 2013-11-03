
// FILE: main.cc

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
//
//
//                   Main Startup Code for the Cutplane Editor
//              (Faster, Stronger, and Much Much More Powerful)
//
// Authors: LJE and WAK                           Date: December 27,1989
//
// Copyright (C) 1987,1988,1989,1990 The Board of Trustees of The Leland
// Stanford Junior University, and William Kessler and Larry Edwards.
// All Rights Reserved.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//

#define MAINMODULE

#include <config.h>


#include <platform_gl.h>
#include <platform_glu.h>
#include <platform_glut.h>

#include <stdio.h>
#ifndef _WIN32
#include <signal.h>
#endif
#ifdef __APPLE__
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#include <time.h>

#include <cutplane.h>
#include <globals.h>
#include <init.h>
#include <grafx.h>		// for definition of black
#include <update.h>
#include <tools.h>

#include "device.h"

// things we are testing today:
/*
#define Createfullworkspace
#define Copslikedonuts
#define Timetest
#define Testboolbeam
#define Testboolholes
#define Testboolcube
#define Testsegments
#define Aliasvs3formdemo
#define Twolbeams
#define Stocksdemo
#define Onelbeam
#define Createtools
#define Shipcube
*/


const char className[] = { "3Form" };
const char windowName[] = { "3Form" };

// FIXE ME -- LJE
// HDC hDC;
// HGLRC hGLRC;
// HPALETTE hPalette;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
// Stuff from OGLSDK's cube.c begins here, adapted for cutplane usage.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//

#if 0

void
set_pc_window_values(int new_winLeft, int new_winTop,
		     int new_winWidth, int new_winHeight)
{
  if (new_winLeft > -1) {
    pc_winLeft = new_winLeft;
    pc_winTop = new_winTop;
  } else {
    pc_winLeft = 10;
    pc_winTop = 10;
  }

  if (new_winWidth > -1) {
    pc_winWidth = new_winWidth;
    pc_winHeight = new_winHeight;
  }

  pc_xMargin = pc_winWidth * pc_windowMargin;
  pc_yMargin = pc_winHeight * pc_windowMargin;

  pc_winRight = pc_winLeft + pc_winWidth;
  pc_winBottom = pc_winTop + pc_winHeight; 
  pc_innerLeft = pc_winLeft + pc_xMargin;
  pc_innerRight = pc_winRight - pc_xMargin;
  pc_innerTop = pc_winTop + pc_yMargin;
  pc_innerBottom = pc_winBottom - pc_yMargin;
}

void
setupPalette(HDC hDC)
{
    int pixelFormat = GetPixelFormat(hDC);
    PIXELFORMATDESCRIPTOR pfd;
    LOGPALETTE* pPal;
    int paletteSize;

    DescribePixelFormat(hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    if (pfd.dwFlags & PFD_NEED_PALETTE) {
	paletteSize = 1 << pfd.cColorBits;
    } else {
	return;
    }

    pPal = (LOGPALETTE*)
	malloc(sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY));
    pPal->palVersion = 0x300;
    pPal->palNumEntries = paletteSize;

    // build a simple RGB color palette
    {
	int redMask = (1 << pfd.cRedBits) - 1;
	int greenMask = (1 << pfd.cGreenBits) - 1;
	int blueMask = (1 << pfd.cBlueBits) - 1;
	int i;

	for (i=0; i<paletteSize; ++i) {
	    pPal->palPalEntry[i].peRed =
		    (((i >> pfd.cRedShift) & redMask) * 255) / redMask;
	    pPal->palPalEntry[i].peGreen =
		    (((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask;
	    pPal->palPalEntry[i].peBlue =
		    (((i >> pfd.cBlueShift) & blueMask) * 255) / blueMask;
	    pPal->palPalEntry[i].peFlags = 0;
	}
    }

    hPalette = CreatePalette(pPal);
    free(pPal);

    if (hPalette) {
	SelectPalette(hDC, hPalette, FALSE);
	RealizePalette(hDC);
    }
}


LRESULT APIENTRY
WndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
  int new_winLeft, new_winTop, new_winWidth, new_winHeight;

  switch (message) {
  case WM_CREATE:
    init_dbgdump("./tmptest");   
    // initialize OpenGL rendering
    hDC = GetDC(hWnd);
    setupPixelFormat(hDC);
    setupPalette(hDC);
    hGLRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hGLRC);

    init_grafx(hDC);
    init_3form();
    toggle_cpldebug();

    ShowCursor(FALSE);
    // Initially, move cursor to ctr of screen.
#if 0
    SetCursorPos(pc_winLeft + (pc_winWidth / 2),
		 pc_winTop + (pc_winHeight /2));
#endif
    return 0;
  case WM_DESTROY:
    close_dbgdump();
    // finish OpenGL rendering
    if (hGLRC) {
      wglMakeCurrent(NULL, NULL);
      wglDeleteContext(hGLRC);
    }
    if (hPalette) {
      DeleteObject(hPalette);
    }
    ReleaseDC(hWnd, hDC);
    PostQuitMessage(0);
    ShowCursor(TRUE);
    return 0;
  case WM_MOVE:
    // track window size changes
    if (hGLRC) {
      new_winLeft = (int) LOWORD(lParam);
      new_winTop = (int) HIWORD(lParam);
      set_pc_window_values(new_winLeft, new_winTop,-1,-1);
      reset_pc_valuator_store();
      grafx_resize(pc_winWidth, pc_winHeight);
      new_user_input(world_list,state,oldstate, hDC);
      return 0;
    }
  case WM_SIZE:
    // track window size changes
    if (hGLRC) {
      new_winWidth = (int) LOWORD(lParam);
      new_winHeight = (int) HIWORD(lParam);
      set_pc_window_values(-1,-1,new_winWidth, new_winHeight);
      reset_pc_valuator_store();
      grafx_resize(pc_winWidth, pc_winHeight);
      new_user_input(world_list,state,oldstate, hDC);
      return 0;
    }
  case WM_PALETTECHANGED:
    // realize palette if this is *not* the current window
    if (hGLRC && hPalette && (HWND) wParam != hWnd) {
      UnrealizeObject(hPalette);
      SelectPalette(hDC, hPalette, FALSE);
      RealizePalette(hDC);
      redraw_grafx(hDC);
      break;
    }
    break;
  case WM_QUERYNEWPALETTE:
    // realize palette if this is the current window
    if (hGLRC && hPalette) {
      UnrealizeObject(hPalette);
      SelectPalette(hDC, hPalette, FALSE);
      RealizePalette(hDC);
      redraw_grafx(hDC);
      return TRUE;
    }
    break;
  case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);
      if (hGLRC) {
	redraw_grafx(hDC);
	new_user_input(world_list,state,oldstate, hDC);
      }
      EndPaint(hWnd, &ps);
      return 0;
    }
    break;
  case WM_MOUSEMOVE:
    if (hGLRC) {
      int x = ((int) LOWORD(lParam) << 16) >> 16;
      int y = ((int) HIWORD(lParam) << 16) >> 16;
      // Translate from pc devices->iris universe.

      scan_pc_buttons();
      set_pc_valuators(x,y);

      // Now run 3form's user input handling code.
      new_user_input(world_list,state,oldstate, hDC);
      break;
    }
  case WM_LBUTTONDOWN:
    pc_queue_push(PICKBUTTON, (short) 1);
    new_user_input(world_list,state,oldstate, hDC);

    break;
  case WM_LBUTTONUP:
    pc_queue_push(PICKBUTTON, (short) -1);
    // Now run 3form's user input handling code.
    new_user_input(world_list,state,oldstate, hDC);
    break;
  case WM_RBUTTONDOWN:
    pc_queue_push(PLANEMOVEBUTTON, (short) 1);
    break;
  case WM_RBUTTONUP:
    pc_queue_push(PLANEMOVEBUTTON, (short) -1);
    break;
  case WM_CHAR:
    // handle keyboard input
    switch ((int)wParam) {
    case VK_ESCAPE:
      DestroyWindow(hWnd);
      return 0;
    default:
      break;
    }
    break;
  default:
    break;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

int APIENTRY
WinMain(
    HINSTANCE hCurrentInst,
    HINSTANCE hPreviousInst,
    LPSTR lpszCmdLine,
    int nCmdShow)
{
    WNDCLASS wndClass;
    HWND hWnd;
    MSG msg;

    // Presumably handle a crash gracefully?
    // Only for unix, really.
#ifdef IRIS
    signal(SIGSEGV, error_handler);
#endif

    // register window class
    wndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hCurrentInst;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = className;
    RegisterClass(&wndClass);

    pc_windowMargin = .25;
    set_pc_window_values(10,10,500,500);

    // create window
    hWnd = CreateWindow(
	className, windowName,
	WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
	pc_winLeft, pc_winTop, pc_winWidth, pc_winHeight,
	NULL, NULL, hCurrentInst, NULL);

    // display window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // process messages
    while (GetMessage(&msg, NULL, 0, 0) == TRUE) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    }

    return msg.wParam;
}
#endif

void
Init()
{
  GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light_direction[] = { 1.0, 1.0, 1.0, 0.0 };

  // Enable a single OpenGL light.
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  // Use depth buffering for hidden surface elimination.
  glEnable(GL_DEPTH_TEST);

  // Setup the view
  glMatrixMode(GL_PROJECTION);
  gluPerspective(40.0,		// FOV in degrees
		 1.0,		// aspect ratio
		 1.0,		// Z near
		 10.0);		// Z far
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 5.0,	// Eye point
	    0.0, 0.0, 0.0,      // Reference point
	    0.0, 1.0, 0.);      // Up vector (+Y axis)
}

void
Render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Draw the scene...
  new_user_input(world_list, state, oldstate);
  glutSwapBuffers();
}

int
main(int argc, char *argv[])
{
  init_dbgdump("./debug-dump.txt");
#ifndef WIN32
  init_error_handling();
#endif
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow(windowName);
  glutDisplayFunc(Render);
  glutReshapeFunc(grafx_resize);

// void processMouse(int button, int state, int x, int y)
//
// Button will recieve an integer constant identified by a name in all
// caps. These identifiers have names like GLUT_DOWN, GLUT_UP etc. and
// represent the current state of the mouse buttons (using 0 and 1 for
// true or false). x and y are the position of the mouse measured from
// the upper left corner of the current window. The callback for
// glutMotionFunc should have the format:
//
//   void processMouseActiveMotion(int x, int y)
  glutKeyboardFunc(keyboard_event_handler);
  // Triggered when a mouse button is pressed...
  glutMouseFunc(mouse_button_event_handler);
  // Triggered when the mouse moves within a window and buttons are
  // pressed...
  glutMotionFunc(drag_event_handler);
  // Triggered when the mouse moves within a window and no buttons are
  // pressed...
  glutPassiveMotionFunc(motion_event_handler);

  init_grafx();
  init_3form();
//   toggle_cpldebug();

  glutDisplayFunc(update_display);

  glutMainLoop();

  return 0;
}
