#ifndef OpenGLProc_H
#define OpenGLProc_H

#include <Windows.h>
#include <windef.h>
#include <minwindef.h>
#include <GL/gl.h>

#include <Log.h>

using namespace std;
using namespace hive;

extern HMODULE _libGlHandle;

extern HDC currentOpenGLContext;

extern bool g_stereoDetect;    ///< have we detected incoming stereo frames?
extern unsigned g_clearsPerEye;    ///< number of glClear calls per eye
extern unsigned g_clearCount;    ///< used to count number of glClear calls

void * _getPublicProcAddress(const char *procName);
void * _getPrivateProcAddress(const char *procName);

void *_getAnyGLFuncAddress(const char *name);

typedef PROC(__stdcall * PFN_WGLGETPROCADDRESS)(const char * lpszProc);
extern PFN_WGLGETPROCADDRESS _wglGetProcAddress;

/*************************************************************/
// define real opengl functions
/// define function type for glClear
typedef void (WINAPI * PFN_GLCLEAR)(GLbitfield mask);
extern PFN_GLCLEAR _glClear;

typedef void (WINAPI * PFN_GLCLEARCOLOR)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern PFN_GLCLEARCOLOR _glClearColor;

typedef void (WINAPI * PFN_GLCLEARDEPTH)(GLdouble depth);
extern PFN_GLCLEARDEPTH _glClearDepth;

typedef BOOL(__stdcall * PFN_WGLSWAPBUFFERS)(HDC hdc);
extern PFN_WGLSWAPBUFFERS _wglSwapBuffers;

typedef BOOL(__stdcall * PFN_WGLCHOOSEPIXELFORMATARB)(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats);
extern PFN_WGLCHOOSEPIXELFORMATARB _wglChoosePixelFormatARB;

typedef int(__stdcall * PFN_WGLCHOOSEPIXELFORMAT)(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd);
extern PFN_WGLCHOOSEPIXELFORMAT _wglChoosePixelFormat;

typedef BOOL(__stdcall * PFN_WGLSETPIXELFORMAT)(HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR * ppfd);
extern PFN_WGLSETPIXELFORMAT _wglSetPixelFormat;

typedef int(__stdcall * PFN_WGLGETPIXELFORMAT)(HDC hdc);
extern PFN_WGLGETPIXELFORMAT _wglGetPixelFormat;

typedef int(__stdcall * PFN_WGLDESCRIBEPIXELFORMAT)(HDC hdc, int iPixelFormat, UINT nBytes, PIXELFORMATDESCRIPTOR * ppfd);
extern PFN_WGLDESCRIBEPIXELFORMAT _wglDescribePixelFormat;

/*************************************************************/
//define intercepted necessary opengl functions
int WINAPI interceptedwglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd);
void WINAPI interceptedglClear(GLbitfield mask);
BOOL WINAPI interceptedwglSwapBuffers(HDC hdc);
/*************************************************************/

#endif