#ifndef _PLATFORM_GL_H_
#define _PLATFORM_GL_H_

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
// Apparently (according to a post in stackoverflow) one must include
// windows.h before gl.h
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#endif

#endif	// _PLATFORM_GL_H_
