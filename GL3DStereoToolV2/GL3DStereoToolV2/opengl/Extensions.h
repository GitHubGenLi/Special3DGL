#ifndef hive_Extensions_h
#define hive_Extensions_h

//-----------------------------------------------------------------------------

#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

#include <stdio.h>
//-----------------------------------------------------------------------------

struct Extensions {
   /* PFNWGLDXSETRESOURCESHAREHANDLENVPROC    wglDXSetResourceShareHandleNV;
    PFNWGLDXOPENDEVICENVPROC                wglDXOpenDeviceNV;
    PFNWGLDXCLOSEDEVICENVPROC               wglDXCloseDeviceNV;
    PFNWGLDXREGISTEROBJECTNVPROC            wglDXRegisterObjectNV;
    PFNWGLDXUNREGISTEROBJECTNVPROC          wglDXUnregisterObjectNV;
    PFNWGLDXOBJECTACCESSNVPROC              wglDXObjectAccessNV;
    PFNWGLDXLOCKOBJECTSNVPROC               wglDXLockObjectsNV;
    PFNWGLDXUNLOCKOBJECTSNVPROC             wglDXUnlockObjectsNV;*/
    PFNGLGENFRAMEBUFFERSPROC                glGenFramebuffers;
    PFNGLBINDFRAMEBUFFERPROC                glBindFramebuffer;
    PFNGLFRAMEBUFFERTEXTURE2DPROC           glFramebufferTexture2D;
    PFNGLBINDRENDERBUFFERPROC               glBindRenderbuffer;
    PFNGLGENRENDERBUFFERSPROC               glGenRenderbuffers;
    PFNGLRENDERBUFFERSTORAGEPROC            glRenderbufferStorage;
    PFNGLDELETERENDERBUFFERSPROC            glDeleteRenderbuffers;
    PFNGLFRAMEBUFFERRENDERBUFFERPROC        glFramebufferRenderbuffer;
    PFNGLBLITFRAMEBUFFERPROC                glBlitFramebuffer;
    PFNGLCHECKFRAMEBUFFERSTATUSPROC         glCheckFramebufferStatus;
    PFNGLGETRENDERBUFFERPARAMETERIVPROC     glGetRenderbufferParameteriv;
	void *_getAnyGLFuncAddress(const char *name)
	{
		//Log::open("intercept.log");
		//Log::print("_getAnyGLFuncAddress\n");

		void *p = (void *)wglGetProcAddress(name);
		if (p == 0 ||
			(p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
			(p == (void*)-1))
		{
			HMODULE module = LoadLibraryA("opengl32.dll");
			p = (void *)GetProcAddress(module, name);
		}

		return p;
	}
    Extensions();

    bool load();
};

//-----------------------------------------------------------------------------

#endif//hive_Extensions_h
