#include <windows.h>
#include <mhook-lib/mhook.h>
#include <atlconv.h>
#include "Log.h"
#include <iostream>
#include "opengl\OpenGLProc.h"

#include <stdio.h>

#include "Config3DSettings.h"

using namespace hive;
using namespace std;

//This project has been modified from Quadifier
//-----------------------------------------------------------------------------
//
// Copyright (C) 2012-14 James Ward, University of Hull
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//    1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software.
//
//    2. If you use this software in a product, an acknowledgment in the
//    product documentation is required.
//
//    3. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
//    4. This notice may not be removed or altered from any source
//    distribution.
//
//-----------------------------------------------------------------------------

void initialiseRealOpenGLFunctions()
{
	_wglChoosePixelFormat = (PFN_WGLCHOOSEPIXELFORMAT)_getAnyGLFuncAddress("wglChoosePixelFormat");
	_glClear = (PFN_GLCLEAR)_getAnyGLFuncAddress("glClear");
	_wglSwapBuffers = (PFN_WGLSWAPBUFFERS)_getAnyGLFuncAddress("wglSwapBuffers");

	//_wglChoosePixelFormat = &_get_wglChoosePixelFormat; //not working with styles
	//_glClear = &_get_glClear;
	//_wglSwapBuffers = &_get_wglSwapBuffers;

	_wglSetPixelFormat = (PFN_WGLSETPIXELFORMAT)_getPublicProcAddress("wglSetPixelFormat");

	_wglChoosePixelFormatARB = (PFN_WGLCHOOSEPIXELFORMATARB)_getAnyGLFuncAddress("wglChoosePixelFormatARB");
	_glColor3f = (PFN_GLCOLOR3F)_getAnyGLFuncAddress("glColor3f");
	_glRotatef = (PFN_GLROTATEF)_getAnyGLFuncAddress("glRotatef");
	_glLoadIdentity = (PFN_GLLOADIDENTITY)_getAnyGLFuncAddress("glLoadIdentity");

	_glMultMatrixf = (PFN_GLMULTMATRIXF)_getAnyGLFuncAddress("glMultMatrixf");
	_glTranslated = (PFN_GLTRANSLATED)_getAnyGLFuncAddress("glTranslated");
	_glTranslatef = (PFN_GLTRANSLATEF)_getAnyGLFuncAddress("glTranslatef");
	_glBegin = (PFN_GLBEGIN)_getAnyGLFuncAddress("glBegin");
	_glVertex3f = (PFN_GLVERTEX3F)_getAnyGLFuncAddress("glVertex3f");
	_glEnd = (PFN_GLEND)_getAnyGLFuncAddress("glEnd");
	_glFlush = (PFN_GLFLUSH)_getAnyGLFuncAddress("glFlush");
}

void processAttach()
{
	//bool isOpened = Log::open("intercept.log");
	Log::print("DLL_PROCESS_ATTACH\n");

	printf("processAttach\n");
	
	initialiseRealOpenGLFunctions(); 

	if ((_wglChoosePixelFormat == 0) || !Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglChoosePixelFormat), interceptedwglChoosePixelFormat))
	{
		cerr << "Failed to hook _wglChoosePixelFormat" << endl;
		MessageBox(0, "Failed to hook _wglChoosePixelFormat", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _wglChoosePixelFormat\n");
	}
	if ((_wglSetPixelFormat == 0) || !Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSetPixelFormat), interceptedwglSetPixelFormat))
	{
		cerr << "Failed to hook _wglSetPixelFormat" << endl;
		MessageBox(0, "Failed to hook _wglSetPixelFormat", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _wglSetPixelFormat\n");
	}

	if ((_glClear == 0) || !Mhook_SetHook(reinterpret_cast<PVOID*>(&_glClear), interceptedglClear))
	{
		cerr << "Failed to hook _glClear" << endl;
		MessageBox(0, "Failed to hook _glClear", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glClear\n");
	}
	if ((_wglSwapBuffers == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSwapBuffers), interceptedwglSwapBuffers))
	{
		cerr << "Failed to hook _wglSwapBuffers" << endl;
		MessageBox(0, "Failed to hook _wglSwapBuffers", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _wglSwapBuffers\n");
	}
	if ((_glColor3f == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glColor3f), interceptedglColor3f))
	{
		cerr << "Failed to hook _glColor3f" << endl;
		MessageBox(0, "Failed to hook _glColor3f", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glColor3f\n");
	}
	if ((_glRotatef == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glRotatef), interceptedglRotatef))
	{
		cerr << "Failed to hook _glRotatef" << endl;
		MessageBox(0, "Failed to hook _glRotatef", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glRotatef\n");
	}
	if ((_glLoadIdentity == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glLoadIdentity), interceptedglLoadIdentity))
	{
		cerr << "Failed to hook _glLoadIdentity" << endl;
		MessageBox(0, "Failed to hook _glLoadIdentity", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glLoadIdentity\n");
	}
	if ((_glMultMatrixf == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glMultMatrixf), interceptedglMultMatrixf))
	{
		cerr << "Failed to hook _glMultMatrixf" << endl;
		MessageBox(0, "Failed to hook _glMultMatrixf", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glMultMatrixf\n");
	}
	if ((_glTranslated == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glTranslated), interceptedglTranslated))
	{
		cerr << "Failed to hook _glTranslated" << endl;
		MessageBox(0, "Failed to hook _glTranslated", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glTranslated\n");
	}
	if ((_glTranslatef == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glTranslatef), interceptedglTranslatef))
	{
		cerr << "Failed to hook _glTranslatef" << endl;
		MessageBox(0, "Failed to hook _glTranslatef", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glTranslatef\n");
	}
	if ((_glBegin == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glBegin), interceptedglBegin))
	{
		cerr << "Failed to hook _glBegin" << endl;
		MessageBox(0, "Failed to hook _glBegin", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glBegin\n");
	}
	if ((_glVertex3f == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glVertex3f), interceptedglVertex3f))
	{
		cerr << "Failed to hook _glVertex3f" << endl;
		MessageBox(0, "Failed to hook _glVertex3f", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glVertex3f\n");
	}
	if ((_glEnd == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glEnd), interceptedglEnd))
	{
		cerr << "Failed to hook _glEnd" << endl;
		MessageBox(0, "Failed to hook _glEnd", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glEnd\n");
	}
	if ((_glFlush == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glFlush), interceptedglFlush))
	{
		cerr << "Failed to hook _glFlush" << endl;
		MessageBox(0, "Failed to hook _glFlush", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glFlush\n");
	}
}
void processDetach()
{
	printf("processDetach\n");

	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglChoosePixelFormat));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSetPixelFormat));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glClear));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSwapBuffers));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glColor3f));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glRotatef));
}
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	bool isOpened = Log::open("intercept.log");

	if (isOpened)
	{
		printf("Opened\n");
	}

	//Config3DSettings config;
	//config.readConfig3DSettingsFromFile();

	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(instance);
		processAttach();
		break;
	case DLL_PROCESS_DETACH:
		processDetach();
		break;
	}

	return TRUE;
}
