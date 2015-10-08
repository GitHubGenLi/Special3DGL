#ifndef OpenGLProc_H
#define OpenGLProc_H

#include <Windows.h>
#include <windef.h>
#include <minwindef.h>
#include <GL/gl.h>

#include <array>
#include <set>

#include <Log.h>

//#include "GLQuadifier.h"
#include "Event.h"
#include "Extensions.h"
#include "Target.h"
#include "DebugUtil.h"

using namespace std;
using namespace hive;

/*************************************************************/
extern HMODULE _libGlHandle;

extern HDC currentOpenGLContext;

extern bool g_stereoDetect;    ///< have we detected incoming stereo frames?
extern unsigned g_clearsPerEye;    ///< number of glClear calls per eye
extern unsigned g_clearCount;    ///< used to count number of glClear calls

extern unsigned m_framesGL;    ///< OpenGL frame count

extern unsigned m_samplesGL;   ///< OpenGL multisamples (or 0)

extern unsigned m_drawBuffer;  ///< buffer to draw to
extern unsigned m_readBuffer;  ///< buffer to read from

extern unsigned m_width;       ///< display width in pixels
extern unsigned m_height;      ///< display height in pixels

extern bool m_initialised;     ///< has initialisation completed?

/// Set of all render targets that have been presented
extern std::set< unsigned > m_presentedTargets;

extern std::tr1::array<Target, 3> m_target; ///< DX/GL targets for rendering

extern bool     m_verbose;             ///< Verbose logging

extern bool     m_stereoMode;          ///< Stereo mode enable/disable
extern bool     m_stereoAvailable;     ///< Is quad-buffer stereo available?
//extern unsigned m_clearCount;          ///< Number of clears per frame
extern unsigned m_clearCountPersist;   ///< Persistent number of clears

extern double   m_firstFrameTimeGL;    ///< time-stamp of first GL frame
extern double   m_lastFrameTimeGL;     ///< time-stamp of last GL frame

extern GLuint   m_quadListGL;          ///< GL display list for textured quad

extern uintptr_t m_thread;             ///< Handle of the rendering thread

extern Event m_frameDone;              ///< Signals when frame is rendered out

extern Extensions glx;                 ///< Stores the OpenGL extension functions

extern bool useTexture;
extern bool mustUseBlit;
extern GLint textureSize;
extern int widthScreen;
extern int heightScreen;
/*************************************************************/
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
void WINAPI _get_glClear(GLbitfield mask);

typedef void (WINAPI * PFN_GLCLEARCOLOR)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern PFN_GLCLEARCOLOR _glClearColor;

typedef void (WINAPI * PFN_GLCLEARDEPTH)(GLdouble depth);
extern PFN_GLCLEARDEPTH _glClearDepth;

typedef BOOL(__stdcall * PFN_WGLSWAPBUFFERS)(HDC hdc);
extern PFN_WGLSWAPBUFFERS _wglSwapBuffers;
BOOL __stdcall _get_wglSwapBuffers(HDC hdc);

typedef BOOL(__stdcall * PFN_WGLCHOOSEPIXELFORMATARB)(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats);
extern PFN_WGLCHOOSEPIXELFORMATARB _wglChoosePixelFormatARB;

typedef int(__stdcall * PFN_WGLCHOOSEPIXELFORMAT)(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd);
extern PFN_WGLCHOOSEPIXELFORMAT _wglChoosePixelFormat;
int __stdcall _get_wglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd);

typedef BOOL(__stdcall * PFN_WGLSETPIXELFORMAT)(HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR * ppfd);
extern PFN_WGLSETPIXELFORMAT _wglSetPixelFormat;

typedef int(__stdcall * PFN_WGLGETPIXELFORMAT)(HDC hdc);
extern PFN_WGLGETPIXELFORMAT _wglGetPixelFormat;

typedef int(__stdcall * PFN_WGLDESCRIBEPIXELFORMAT)(HDC hdc, int iPixelFormat, UINT nBytes, PIXELFORMATDESCRIPTOR * ppfd);
extern PFN_WGLDESCRIBEPIXELFORMAT _wglDescribePixelFormat;

/*************************************************************/
//void initialiseRealOpenGLFunctions();
void initialiseVariables();
/*************************************************************/
//define intercepted necessary opengl functions
int WINAPI interceptedwglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd);
void WINAPI interceptedglClear(GLbitfield mask);
BOOL WINAPI interceptedwglSwapBuffers(HDC hdc);
/*************************************************************/

/// OpenGL rendering thread function
static unsigned __stdcall threadFunc(void *context);

/**
* Send the last rendered frame from DX to GL and swap the render
* targets ready for next frame.
*/
void sendFrame(GLuint drawBuffer);

/**
* Returns true if the current render target has ever been presented
* (which we use to detect render targets that are actually displayed,
* rather than the various off screen render targets).
*/
bool isPresentedRenderTarget();
unsigned getSamples();
/*************************************************************/

#endif