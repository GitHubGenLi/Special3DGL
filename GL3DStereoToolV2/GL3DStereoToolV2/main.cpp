#include <windows.h>
#include <mhook-lib/mhook.h>
#include <atlconv.h>
#include "Log.h"
#include <iostream>
#include "opengl\OpenGLProc.h"

#include <stdio.h>

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

void processAttach()
{
	/*bool isOpened = Log::open("intercept.log");*/
	Log::print("DLL_PROCESS_ATTACH\n");

	printf("processAttach\n");

	/*if (isOpened)
	{
		printf("Opened\n");
	}*/

	//typedef FARPROC(WINAPI *PFNGetProcAddress)(HMODULE hModule, LPCSTR lpProcName);

	//PFNGetProcAddress real_GetProcAddress = reinterpret_cast<PFNGetProcAddress>(
	//	GetProcAddress(GetModuleHandle("kernel32"), "GetProcAddress"));

	/*if (_getPublicProcAddress("glClearColor") != 0)
	{
		Log::print("glClearColor OK\n");
	}
	else
	{
		Log::print("glClearColor Fail\n");
	}

	if (_wglGetProcAddress != 0)
	{
		Log::print("_wglGetProcAddress OK\n");
	}
	else
	{
		Log::print("_wglGetProcAddress Fail\n");
	}

	if (_getAnyGLFuncAddress("glClear") != 0)
	{
		Log::print("glClear 2 OK\n");
		printf("%d", _getAnyGLFuncAddress("glClear"));
	}
	else
	{
		Log::print("glClear 2 Fail\n");
	}


	*/
	_wglChoosePixelFormat = (PFN_WGLCHOOSEPIXELFORMAT)_getAnyGLFuncAddress("wglChoosePixelFormat");
	_glClear = (PFN_GLCLEAR)_getAnyGLFuncAddress("glClear");
	_wglSwapBuffers = (PFN_WGLSWAPBUFFERS)_getAnyGLFuncAddress("wglSwapBuffers");

	if ((_wglChoosePixelFormat == 0) || !Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglChoosePixelFormat), interceptedwglChoosePixelFormat))
	{
		cerr << "Failed to hook _wglChoosePixelFormat" << endl;
		MessageBox(0, "Failed to hook _wglChoosePixelFormat", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _wglChoosePixelFormat\n");
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
}
void processDetach()
{
	printf("processDetach\n");

	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglChoosePixelFormat));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glClear));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSwapBuffers));
}
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	bool isOpened = Log::open("intercept.log");

	if (isOpened)
	{
		printf("Opened\n");
	}

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
