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

#include "StereoUtil.h"

#include "..\Config3DSettings.h"

using namespace std;
using namespace hive;

/*************************************************************/
extern HMODULE _libGlHandle;

extern HDC currentOpenGLContext;

extern bool g_stereoDetect;    ///< have we detected incoming stereo frames?
extern unsigned g_clearsPerEye;    ///< number of glClear calls per eye
extern unsigned g_clearCount;    ///< used to count number of glClear calls

//extern unsigned m_framesGL;    ///< OpenGL frame count

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
extern PIXELFORMATDESCRIPTOR currentStereo_ppfd;
extern int currentStereoPixelFormat;

extern long functionCallIndex;
extern Config3DSettings currentConfig3DSettings;

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

typedef void (APIENTRY * PFN_GLCOLOR3F)(GLfloat red, GLfloat green, GLfloat blue);
extern PFN_GLCOLOR3F _glColor3f;

typedef void (APIENTRY * PFN_GLROTATEF)(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
extern PFN_GLROTATEF _glRotatef;

typedef void (APIENTRY * PFN_GLLOADIDENTITY)(void);
extern PFN_GLLOADIDENTITY _glLoadIdentity;

typedef void (APIENTRY * PFN_GLMULTMATRIXF)(const GLfloat * m);
extern PFN_GLMULTMATRIXF _glMultMatrixf;

typedef void (APIENTRY * PFN_GLTRANSLATED)(GLdouble x, GLdouble y, GLdouble z);
extern PFN_GLTRANSLATED _glTranslated;

typedef void (APIENTRY * PFN_GLLIGHTFV)(GLenum light, GLenum pname, const GLfloat * params);
extern PFN_GLLIGHTFV _glLightfv;

typedef void (APIENTRY * PFN_GLTRANSLATEF)(GLfloat x, GLfloat y, GLfloat z);
extern PFN_GLTRANSLATEF _glTranslatef;

typedef void (APIENTRY * PFN_GLBEGIN)(GLenum mode);
extern PFN_GLBEGIN _glBegin;

typedef void (APIENTRY * PFN_GLVERTEX3F)(GLfloat x, GLfloat y, GLfloat z);
extern PFN_GLVERTEX3F _glVertex3f;

typedef void (APIENTRY * PFN_GLEND)(void);
extern PFN_GLEND _glEnd;

typedef void (APIENTRY * PFN_GLFLUSH)(void);
extern PFN_GLFLUSH _glFlush;

typedef BOOL(__stdcall * PFN_WGLMAKECURRENT)(HDC hdc, HGLRC hglrc);
extern PFN_WGLMAKECURRENT _wglMakeCurrent;

typedef void (APIENTRY * PFN_GLSCALED)(GLdouble x, GLdouble y, GLdouble z);
extern PFN_GLSCALED _glScaled;

typedef void (APIENTRY * PFN_GLCOLOR3D)(GLdouble red, GLdouble green, GLdouble blue);
extern PFN_GLCOLOR3D _glColor3d;

typedef void (APIENTRY * PFN_GLPUSHMATRIX)(void);
extern PFN_GLPUSHMATRIX _glPushMatrix;

typedef void (APIENTRY * PFN_GLMULTMATRIXD)(const GLdouble * m);
extern PFN_GLMULTMATRIXD _glMultMatrixd;

typedef HGLRC(__stdcall * PFN_WGLCREATECONTEXT)(HDC hdc);
extern PFN_WGLCREATECONTEXT _wglCreateContext;

typedef BOOL(__stdcall * PFN_WGLDELETECONTEXT)(HGLRC hglrc);
extern PFN_WGLDELETECONTEXT _wglDeleteContext;

typedef HGLRC(__stdcall * PFN_WGLGETCURRENTCONTEXT)(void);
extern PFN_WGLGETCURRENTCONTEXT _wglGetCurrentContext;

typedef BOOL(__stdcall * PFN_WGLMAKECURRENT)(HDC hdc, HGLRC hglrc);
extern PFN_WGLMAKECURRENT _wglMakeCurrent;

typedef BOOL(__stdcall * PFN_WGLCOPYCONTEXT)(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask);
extern PFN_WGLCOPYCONTEXT _wglCopyContext;

typedef int(__stdcall * PFN_WGLCHOOSEPIXELFORMAT)(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd);
extern PFN_WGLCHOOSEPIXELFORMAT _wglChoosePixelFormat;

typedef int(__stdcall * PFN_WGLDESCRIBEPIXELFORMAT)(HDC hdc, int iPixelFormat, UINT nBytes, PIXELFORMATDESCRIPTOR * ppfd);
extern PFN_WGLDESCRIBEPIXELFORMAT _wglDescribePixelFormat;

typedef HDC(__stdcall * PFN_WGLGETCURRENTDC)(void);
extern PFN_WGLGETCURRENTDC _wglGetCurrentDC;

typedef PROC(__stdcall * PFN_WGLGETDEFAULTPROCADDRESS)(const char * lpszProc);
extern PFN_WGLGETDEFAULTPROCADDRESS _wglGetDefaultProcAddress;

typedef int(__stdcall * PFN_WGLGETPIXELFORMAT)(HDC hdc);
extern PFN_WGLGETPIXELFORMAT _wglGetPixelFormat;

typedef BOOL(__stdcall * PFN_WGLSETPIXELFORMAT)(HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR * ppfd);
extern PFN_WGLSETPIXELFORMAT _wglSetPixelFormat;

typedef BOOL(__stdcall * PFN_WGLSWAPBUFFERS)(HDC hdc);
extern PFN_WGLSWAPBUFFERS _wglSwapBuffers;

typedef BOOL(__stdcall * PFN_WGLSHARELISTS)(HGLRC hglrc1, HGLRC hglrc2);
extern PFN_WGLSHARELISTS _wglShareLists;

typedef HGLRC(__stdcall * PFN_WGLCREATELAYERCONTEXT)(HDC hdc, int iLayerPlane);
extern PFN_WGLCREATELAYERCONTEXT _wglCreateLayerContext;

typedef BOOL(__stdcall * PFN_WGLDESCRIBELAYERPLANE)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LAYERPLANEDESCRIPTOR * plpd);
extern PFN_WGLDESCRIBELAYERPLANE _wglDescribeLayerPlane;

typedef int(__stdcall * PFN_WGLSETLAYERPALETTEENTRIES)(HDC hdc, int iLayerPlane, int iStart, int cEntries, const COLORREF * pcr);
extern PFN_WGLSETLAYERPALETTEENTRIES _wglSetLayerPaletteEntries;

typedef int(__stdcall * PFN_WGLGETLAYERPALETTEENTRIES)(HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF * pcr);
extern PFN_WGLGETLAYERPALETTEENTRIES _wglGetLayerPaletteEntries;

typedef BOOL(__stdcall * PFN_WGLREALIZELAYERPALETTE)(HDC hdc, int iLayerPlane, BOOL bRealize);
extern PFN_WGLREALIZELAYERPALETTE _wglRealizeLayerPalette;

typedef BOOL(__stdcall * PFN_WGLSWAPLAYERBUFFERS)(HDC hdc, UINT fuPlanes);
extern PFN_WGLSWAPLAYERBUFFERS _wglSwapLayerBuffers;

typedef BOOL(__stdcall * PFN_WGLUSEFONTBITMAPSA)(HDC hdc, DWORD first, DWORD count, DWORD listBase);
extern PFN_WGLUSEFONTBITMAPSA _wglUseFontBitmapsA;

typedef BOOL(__stdcall * PFN_WGLUSEFONTBITMAPSW)(HDC hdc, DWORD first, DWORD count, DWORD listBase);
extern PFN_WGLUSEFONTBITMAPSW _wglUseFontBitmapsW;

typedef DWORD(__stdcall * PFN_WGLSWAPMULTIPLEBUFFERS)(UINT n, const WGLSWAP * ps);
extern PFN_WGLSWAPMULTIPLEBUFFERS _wglSwapMultipleBuffers;

typedef BOOL(__stdcall * PFN_WGLUSEFONTOUTLINESA)(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, GLYPHMETRICSFLOAT * lpgmf);
extern PFN_WGLUSEFONTOUTLINESA _wglUseFontOutlinesA;

typedef BOOL(__stdcall * PFN_WGLUSEFONTOUTLINESW)(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, GLYPHMETRICSFLOAT * lpgmf);
extern PFN_WGLUSEFONTOUTLINESW _wglUseFontOutlinesW;

typedef HANDLE(__stdcall * PFN_WGLCREATEBUFFERREGIONARB)(HDC hDC, int iLayerPlane, UINT uType);
extern PFN_WGLCREATEBUFFERREGIONARB _wglCreateBufferRegionARB;

typedef void(__stdcall * PFN_WGLDELETEBUFFERREGIONARB)(HANDLE hRegion);
extern PFN_WGLDELETEBUFFERREGIONARB _wglDeleteBufferRegionARB;

typedef BOOL(__stdcall * PFN_WGLSAVEBUFFERREGIONARB)(HANDLE hRegion, int x, int y, int width, int height);
extern PFN_WGLSAVEBUFFERREGIONARB _wglSaveBufferRegionARB;

typedef BOOL(__stdcall * PFN_WGLRESTOREBUFFERREGIONARB)(HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc);
extern PFN_WGLRESTOREBUFFERREGIONARB _wglRestoreBufferRegionARB;

typedef const char * (__stdcall * PFN_WGLGETEXTENSIONSSTRINGARB)(HDC hdc);
extern PFN_WGLGETEXTENSIONSSTRINGARB _wglGetExtensionsStringARB;

typedef BOOL(__stdcall * PFN_WGLGETPIXELFORMATATTRIBIVARB)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, int * piValues);
extern PFN_WGLGETPIXELFORMATATTRIBIVARB _wglGetPixelFormatAttribivARB;

typedef BOOL(__stdcall * PFN_WGLGETPIXELFORMATATTRIBFVARB)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, FLOAT * pfValues);
extern PFN_WGLGETPIXELFORMATATTRIBFVARB _wglGetPixelFormatAttribfvARB;

typedef BOOL(__stdcall * PFN_WGLCHOOSEPIXELFORMATARB)(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats);
extern PFN_WGLCHOOSEPIXELFORMATARB _wglChoosePixelFormatARB;

typedef BOOL(__stdcall * PFN_WGLMAKECONTEXTCURRENTARB)(HDC hDrawDC, HDC hReadDC, HGLRC hglrc);
extern PFN_WGLMAKECONTEXTCURRENTARB _wglMakeContextCurrentARB;

typedef HDC(__stdcall * PFN_WGLGETCURRENTREADDCARB)(void);
extern PFN_WGLGETCURRENTREADDCARB _wglGetCurrentReadDCARB;

typedef HPBUFFERARB(__stdcall * PFN_WGLCREATEPBUFFERARB)(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int * piAttribList);
extern PFN_WGLCREATEPBUFFERARB _wglCreatePbufferARB;

typedef HDC(__stdcall * PFN_WGLGETPBUFFERDCARB)(HPBUFFERARB hPbuffer);
extern PFN_WGLGETPBUFFERDCARB _wglGetPbufferDCARB;

typedef int(__stdcall * PFN_WGLRELEASEPBUFFERDCARB)(HPBUFFERARB hPbuffer, HDC hDC);
extern PFN_WGLRELEASEPBUFFERDCARB _wglReleasePbufferDCARB;

typedef BOOL(__stdcall * PFN_WGLDESTROYPBUFFERARB)(HPBUFFERARB hPbuffer);
extern PFN_WGLDESTROYPBUFFERARB _wglDestroyPbufferARB;

typedef BOOL(__stdcall * PFN_WGLQUERYPBUFFERARB)(HPBUFFERARB hPbuffer, int iAttribute, int * piValue);
extern PFN_WGLQUERYPBUFFERARB _wglQueryPbufferARB;

typedef BOOL(__stdcall * PFN_WGLBINDTEXIMAGEARB)(HPBUFFERARB hPbuffer, int iBuffer);
extern PFN_WGLBINDTEXIMAGEARB _wglBindTexImageARB;

typedef BOOL(__stdcall * PFN_WGLRELEASETEXIMAGEARB)(HPBUFFERARB hPbuffer, int iBuffer);
extern PFN_WGLRELEASETEXIMAGEARB _wglReleaseTexImageARB;

typedef BOOL(__stdcall * PFN_WGLSETPBUFFERATTRIBARB)(HPBUFFERARB hPbuffer, const int * piAttribList);
extern PFN_WGLSETPBUFFERATTRIBARB _wglSetPbufferAttribARB;

typedef HGLRC(__stdcall * PFN_WGLCREATECONTEXTATTRIBSARB)(HDC hDC, HGLRC hShareContext, const int * attribList);
extern PFN_WGLCREATECONTEXTATTRIBSARB _wglCreateContextAttribsARB;

typedef GLboolean(__stdcall * PFN_WGLCREATEDISPLAYCOLORTABLEEXT)(GLushort id);
extern PFN_WGLCREATEDISPLAYCOLORTABLEEXT _wglCreateDisplayColorTableEXT;

typedef GLboolean(__stdcall * PFN_WGLLOADDISPLAYCOLORTABLEEXT)(const GLushort * table, GLuint length);
extern PFN_WGLLOADDISPLAYCOLORTABLEEXT _wglLoadDisplayColorTableEXT;

typedef GLboolean(__stdcall * PFN_WGLBINDDISPLAYCOLORTABLEEXT)(GLushort id);
extern PFN_WGLBINDDISPLAYCOLORTABLEEXT _wglBindDisplayColorTableEXT;

typedef void(__stdcall * PFN_WGLDESTROYDISPLAYCOLORTABLEEXT)(GLushort id);
extern PFN_WGLDESTROYDISPLAYCOLORTABLEEXT _wglDestroyDisplayColorTableEXT;

typedef const char * (__stdcall * PFN_WGLGETEXTENSIONSSTRINGEXT)(void);
extern PFN_WGLGETEXTENSIONSSTRINGEXT _wglGetExtensionsStringEXT;

typedef BOOL(__stdcall * PFN_WGLMAKECONTEXTCURRENTEXT)(HDC hDrawDC, HDC hReadDC, HGLRC hglrc);
extern PFN_WGLMAKECONTEXTCURRENTEXT _wglMakeContextCurrentEXT;

typedef HDC(__stdcall * PFN_WGLGETCURRENTREADDCEXT)(void);
extern PFN_WGLGETCURRENTREADDCEXT _wglGetCurrentReadDCEXT;

typedef HPBUFFEREXT(__stdcall * PFN_WGLCREATEPBUFFEREXT)(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int * piAttribList);
extern PFN_WGLCREATEPBUFFEREXT _wglCreatePbufferEXT;

typedef HDC(__stdcall * PFN_WGLGETPBUFFERDCEXT)(HPBUFFEREXT hPbuffer);
extern PFN_WGLGETPBUFFERDCEXT _wglGetPbufferDCEXT;

typedef int(__stdcall * PFN_WGLRELEASEPBUFFERDCEXT)(HPBUFFEREXT hPbuffer, HDC hDC);
extern PFN_WGLRELEASEPBUFFERDCEXT _wglReleasePbufferDCEXT;

typedef BOOL(__stdcall * PFN_WGLDESTROYPBUFFEREXT)(HPBUFFEREXT hPbuffer);
extern PFN_WGLDESTROYPBUFFEREXT _wglDestroyPbufferEXT;

typedef BOOL(__stdcall * PFN_WGLQUERYPBUFFEREXT)(HPBUFFEREXT hPbuffer, int iAttribute, int * piValue);
extern PFN_WGLQUERYPBUFFEREXT _wglQueryPbufferEXT;

typedef BOOL(__stdcall * PFN_WGLGETPIXELFORMATATTRIBIVEXT)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, int * piValues);
extern PFN_WGLGETPIXELFORMATATTRIBIVEXT _wglGetPixelFormatAttribivEXT;

typedef BOOL(__stdcall * PFN_WGLGETPIXELFORMATATTRIBFVEXT)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, FLOAT * pfValues);
extern PFN_WGLGETPIXELFORMATATTRIBFVEXT _wglGetPixelFormatAttribfvEXT;

typedef BOOL(__stdcall * PFN_WGLCHOOSEPIXELFORMATEXT)(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats);
extern PFN_WGLCHOOSEPIXELFORMATEXT _wglChoosePixelFormatEXT;

typedef BOOL(__stdcall * PFN_WGLSWAPINTERVALEXT)(int interval);
extern PFN_WGLSWAPINTERVALEXT _wglSwapIntervalEXT;

typedef int(__stdcall * PFN_WGLGETSWAPINTERVALEXT)(void);
extern PFN_WGLGETSWAPINTERVALEXT _wglGetSwapIntervalEXT;

typedef void * (__stdcall * PFN_WGLALLOCATEMEMORYNV)(GLsizei size, GLfloat readfreq, GLfloat writefreq, GLfloat priority);
extern PFN_WGLALLOCATEMEMORYNV _wglAllocateMemoryNV;

typedef void(__stdcall * PFN_WGLFREEMEMORYNV)(void * pointer);
extern PFN_WGLFREEMEMORYNV _wglFreeMemoryNV;

typedef BOOL(__stdcall * PFN_WGLGETSYNCVALUESOML)(HDC hdc, INT64 * ust, INT64 * msc, INT64 * sbc);
extern PFN_WGLGETSYNCVALUESOML _wglGetSyncValuesOML;

typedef BOOL(__stdcall * PFN_WGLGETMSCRATEOML)(HDC hdc, INT32 * numerator, INT32 * denominator);
extern PFN_WGLGETMSCRATEOML _wglGetMscRateOML;

typedef INT64(__stdcall * PFN_WGLSWAPBUFFERSMSCOML)(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder);
extern PFN_WGLSWAPBUFFERSMSCOML _wglSwapBuffersMscOML;

typedef INT64(__stdcall * PFN_WGLSWAPLAYERBUFFERSMSCOML)(HDC hdc, int fuPlanes, INT64 target_msc, INT64 divisor, INT64 remainder);
extern PFN_WGLSWAPLAYERBUFFERSMSCOML _wglSwapLayerBuffersMscOML;

typedef BOOL(__stdcall * PFN_WGLWAITFORMSCOML)(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder, INT64 * ust, INT64 * msc, INT64 * sbc);
extern PFN_WGLWAITFORMSCOML _wglWaitForMscOML;

typedef BOOL(__stdcall * PFN_WGLWAITFORSBCOML)(HDC hdc, INT64 target_sbc, INT64 * ust, INT64 * msc, INT64 * sbc);
extern PFN_WGLWAITFORSBCOML _wglWaitForSbcOML;

typedef BOOL(__stdcall * PFN_WGLGETDIGITALVIDEOPARAMETERSI3D)(HDC hDC, int iAttribute, int * piValue);
extern PFN_WGLGETDIGITALVIDEOPARAMETERSI3D _wglGetDigitalVideoParametersI3D;

typedef BOOL(__stdcall * PFN_WGLSETDIGITALVIDEOPARAMETERSI3D)(HDC hDC, int iAttribute, const int * piValue);
extern PFN_WGLSETDIGITALVIDEOPARAMETERSI3D _wglSetDigitalVideoParametersI3D;

typedef BOOL(__stdcall * PFN_WGLGETGAMMATABLEPARAMETERSI3D)(HDC hDC, int iAttribute, int * piValue);
extern PFN_WGLGETGAMMATABLEPARAMETERSI3D _wglGetGammaTableParametersI3D;

typedef BOOL(__stdcall * PFN_WGLSETGAMMATABLEPARAMETERSI3D)(HDC hDC, int iAttribute, const int * piValue);
extern PFN_WGLSETGAMMATABLEPARAMETERSI3D _wglSetGammaTableParametersI3D;

typedef BOOL(__stdcall * PFN_WGLGETGAMMATABLEI3D)(HDC hDC, int iEntries, USHORT * puRed, USHORT * puGreen, USHORT * puBlue);
extern PFN_WGLGETGAMMATABLEI3D _wglGetGammaTableI3D;

typedef BOOL(__stdcall * PFN_WGLSETGAMMATABLEI3D)(HDC hDC, int iEntries, const USHORT * puRed, const USHORT * puGreen, const USHORT * puBlue);
extern PFN_WGLSETGAMMATABLEI3D _wglSetGammaTableI3D;

typedef BOOL(__stdcall * PFN_WGLENABLEGENLOCKI3D)(HDC hDC);
extern PFN_WGLENABLEGENLOCKI3D _wglEnableGenlockI3D;

typedef BOOL(__stdcall * PFN_WGLDISABLEGENLOCKI3D)(HDC hDC);
extern PFN_WGLDISABLEGENLOCKI3D _wglDisableGenlockI3D;

typedef BOOL(__stdcall * PFN_WGLISENABLEDGENLOCKI3D)(HDC hDC, BOOL * pFlag);
extern PFN_WGLISENABLEDGENLOCKI3D _wglIsEnabledGenlockI3D;

typedef BOOL(__stdcall * PFN_WGLGENLOCKSOURCEI3D)(HDC hDC, UINT uSource);
extern PFN_WGLGENLOCKSOURCEI3D _wglGenlockSourceI3D;

typedef BOOL(__stdcall * PFN_WGLGETGENLOCKSOURCEI3D)(HDC hDC, UINT * uSource);
extern PFN_WGLGETGENLOCKSOURCEI3D _wglGetGenlockSourceI3D;

typedef BOOL(__stdcall * PFN_WGLGENLOCKSOURCEEDGEI3D)(HDC hDC, UINT uEdge);
extern PFN_WGLGENLOCKSOURCEEDGEI3D _wglGenlockSourceEdgeI3D;

typedef BOOL(__stdcall * PFN_WGLGETGENLOCKSOURCEEDGEI3D)(HDC hDC, UINT * uEdge);
extern PFN_WGLGETGENLOCKSOURCEEDGEI3D _wglGetGenlockSourceEdgeI3D;

typedef BOOL(__stdcall * PFN_WGLGENLOCKSAMPLERATEI3D)(HDC hDC, UINT uRate);
extern PFN_WGLGENLOCKSAMPLERATEI3D _wglGenlockSampleRateI3D;

typedef BOOL(__stdcall * PFN_WGLGETGENLOCKSAMPLERATEI3D)(HDC hDC, UINT * uRate);
extern PFN_WGLGETGENLOCKSAMPLERATEI3D _wglGetGenlockSampleRateI3D;

typedef BOOL(__stdcall * PFN_WGLGENLOCKSOURCEDELAYI3D)(HDC hDC, UINT uDelay);
extern PFN_WGLGENLOCKSOURCEDELAYI3D _wglGenlockSourceDelayI3D;

typedef BOOL(__stdcall * PFN_WGLGETGENLOCKSOURCEDELAYI3D)(HDC hDC, UINT * uDelay);
extern PFN_WGLGETGENLOCKSOURCEDELAYI3D _wglGetGenlockSourceDelayI3D;

typedef BOOL(__stdcall * PFN_WGLQUERYGENLOCKMAXSOURCEDELAYI3D)(HDC hDC, UINT * uMaxLineDelay, UINT * uMaxPixelDelay);
extern PFN_WGLQUERYGENLOCKMAXSOURCEDELAYI3D _wglQueryGenlockMaxSourceDelayI3D;

typedef void * (__stdcall * PFN_WGLCREATEIMAGEBUFFERI3D)(HDC hDC, DWORD dwSize, UINT uFlags);
extern PFN_WGLCREATEIMAGEBUFFERI3D _wglCreateImageBufferI3D;

typedef BOOL(__stdcall * PFN_WGLDESTROYIMAGEBUFFERI3D)(HDC hDC, void * pAddress);
extern PFN_WGLDESTROYIMAGEBUFFERI3D _wglDestroyImageBufferI3D;

typedef BOOL(__stdcall * PFN_WGLASSOCIATEIMAGEBUFFEREVENTSI3D)(HDC hDC, const HANDLE * pEvent, void * const * pAddress, const DWORD * pSize, UINT count);
extern PFN_WGLASSOCIATEIMAGEBUFFEREVENTSI3D _wglAssociateImageBufferEventsI3D;

typedef BOOL(__stdcall * PFN_WGLRELEASEIMAGEBUFFEREVENTSI3D)(HDC hDC, void * const * pAddress, UINT count);
extern PFN_WGLRELEASEIMAGEBUFFEREVENTSI3D _wglReleaseImageBufferEventsI3D;

typedef BOOL(__stdcall * PFN_WGLENABLEFRAMELOCKI3D)(void);
extern PFN_WGLENABLEFRAMELOCKI3D _wglEnableFrameLockI3D;

typedef BOOL(__stdcall * PFN_WGLDISABLEFRAMELOCKI3D)(void);
extern PFN_WGLDISABLEFRAMELOCKI3D _wglDisableFrameLockI3D;

typedef BOOL(__stdcall * PFN_WGLISENABLEDFRAMELOCKI3D)(BOOL * pFlag);
extern PFN_WGLISENABLEDFRAMELOCKI3D _wglIsEnabledFrameLockI3D;

typedef BOOL(__stdcall * PFN_WGLQUERYFRAMELOCKMASTERI3D)(BOOL * pFlag);
extern PFN_WGLQUERYFRAMELOCKMASTERI3D _wglQueryFrameLockMasterI3D;

typedef BOOL(__stdcall * PFN_WGLGETFRAMEUSAGEI3D)(float * pUsage);
extern PFN_WGLGETFRAMEUSAGEI3D _wglGetFrameUsageI3D;

typedef BOOL(__stdcall * PFN_WGLBEGINFRAMETRACKINGI3D)(void);
extern PFN_WGLBEGINFRAMETRACKINGI3D _wglBeginFrameTrackingI3D;

typedef BOOL(__stdcall * PFN_WGLENDFRAMETRACKINGI3D)(void);
extern PFN_WGLENDFRAMETRACKINGI3D _wglEndFrameTrackingI3D;

typedef BOOL(__stdcall * PFN_WGLQUERYFRAMETRACKINGI3D)(DWORD * pFrameCount, DWORD * pMissedFrames, float * pLastMissedUsage);
extern PFN_WGLQUERYFRAMETRACKINGI3D _wglQueryFrameTrackingI3D;

typedef BOOL(__stdcall * PFN_WGLSETSTEREOEMITTERSTATE3DL)(HDC hDC, UINT uState);
extern PFN_WGLSETSTEREOEMITTERSTATE3DL _wglSetStereoEmitterState3DL;

typedef int(__stdcall * PFN_WGLENUMERATEVIDEODEVICESNV)(HDC hDC, HVIDEOOUTPUTDEVICENV * phDeviceList);
extern PFN_WGLENUMERATEVIDEODEVICESNV _wglEnumerateVideoDevicesNV;

typedef BOOL(__stdcall * PFN_WGLBINDVIDEODEVICENV)(HDC hDC, unsigned int uVideoSlot, HVIDEOOUTPUTDEVICENV hVideoDevice, const int * piAttribList);
extern PFN_WGLBINDVIDEODEVICENV _wglBindVideoDeviceNV;

typedef BOOL(__stdcall * PFN_WGLQUERYCURRENTCONTEXTNV)(int iAttribute, int * piValue);
extern PFN_WGLQUERYCURRENTCONTEXTNV _wglQueryCurrentContextNV;

typedef BOOL(__stdcall * PFN_WGLGETVIDEODEVICENV)(HDC hDC, int numDevices, HPVIDEODEV * hVideoDevice);
extern PFN_WGLGETVIDEODEVICENV _wglGetVideoDeviceNV;

typedef BOOL(__stdcall * PFN_WGLRELEASEVIDEODEVICENV)(HPVIDEODEV hVideoDevice);
extern PFN_WGLRELEASEVIDEODEVICENV _wglReleaseVideoDeviceNV;

typedef BOOL(__stdcall * PFN_WGLBINDVIDEOIMAGENV)(HPVIDEODEV hVideoDevice, HPBUFFERARB hPbuffer, int iVideoBuffer);
extern PFN_WGLBINDVIDEOIMAGENV _wglBindVideoImageNV;

typedef BOOL(__stdcall * PFN_WGLRELEASEVIDEOIMAGENV)(HPBUFFERARB hPbuffer, int iVideoBuffer);
extern PFN_WGLRELEASEVIDEOIMAGENV _wglReleaseVideoImageNV;

typedef BOOL(__stdcall * PFN_WGLSENDPBUFFERTOVIDEONV)(HPBUFFERARB hPbuffer, int iBufferType, unsigned long * pulCounterPbuffer, BOOL bBlock);
extern PFN_WGLSENDPBUFFERTOVIDEONV _wglSendPbufferToVideoNV;

typedef BOOL(__stdcall * PFN_WGLGETVIDEOINFONV)(HPVIDEODEV hpVideoDevice, unsigned long * pulCounterOutputPbuffer, unsigned long * pulCounterOutputVideo);
extern PFN_WGLGETVIDEOINFONV _wglGetVideoInfoNV;

typedef BOOL(__stdcall * PFN_WGLJOINSWAPGROUPNV)(HDC hDC, GLuint group);
extern PFN_WGLJOINSWAPGROUPNV _wglJoinSwapGroupNV;

typedef BOOL(__stdcall * PFN_WGLBINDSWAPBARRIERNV)(GLuint group, GLuint barrier);
extern PFN_WGLBINDSWAPBARRIERNV _wglBindSwapBarrierNV;

typedef BOOL(__stdcall * PFN_WGLQUERYSWAPGROUPNV)(HDC hDC, GLuint * group, GLuint * barrier);
extern PFN_WGLQUERYSWAPGROUPNV _wglQuerySwapGroupNV;

typedef BOOL(__stdcall * PFN_WGLQUERYMAXSWAPGROUPSNV)(HDC hDC, GLuint * maxGroups, GLuint * maxBarriers);
extern PFN_WGLQUERYMAXSWAPGROUPSNV _wglQueryMaxSwapGroupsNV;

typedef BOOL(__stdcall * PFN_WGLQUERYFRAMECOUNTNV)(HDC hDC, GLuint * count);
extern PFN_WGLQUERYFRAMECOUNTNV _wglQueryFrameCountNV;

typedef BOOL(__stdcall * PFN_WGLRESETFRAMECOUNTNV)(HDC hDC);
extern PFN_WGLRESETFRAMECOUNTNV _wglResetFrameCountNV;

typedef BOOL(__stdcall * PFN_WGLENUMGPUSNV)(UINT iGpuIndex, HGPUNV * phGpu);
extern PFN_WGLENUMGPUSNV _wglEnumGpusNV;

typedef BOOL(__stdcall * PFN_WGLENUMGPUDEVICESNV)(HGPUNV hGpu, UINT iDeviceIndex, _GPU_DEVICE * lpGpuDevice);
extern PFN_WGLENUMGPUDEVICESNV _wglEnumGpuDevicesNV;

typedef HDC(__stdcall * PFN_WGLCREATEAFFINITYDCNV)(const HGPUNV * phGpuList);
extern PFN_WGLCREATEAFFINITYDCNV _wglCreateAffinityDCNV;

typedef BOOL(__stdcall * PFN_WGLENUMGPUSFROMAFFINITYDCNV)(HDC hAffinityDC, UINT iGpuIndex, HGPUNV * hGpu);
extern PFN_WGLENUMGPUSFROMAFFINITYDCNV _wglEnumGpusFromAffinityDCNV;

typedef BOOL(__stdcall * PFN_WGLDELETEDCNV)(HDC hdc);
extern PFN_WGLDELETEDCNV _wglDeleteDCNV;

typedef UINT(__stdcall * PFN_WGLGETGPUIDSAMD)(UINT maxCount, UINT * ids);
extern PFN_WGLGETGPUIDSAMD _wglGetGPUIDsAMD;

typedef INT(__stdcall * PFN_WGLGETGPUINFOAMD)(UINT id, int property, GLenum dataType, UINT size, void * data);
extern PFN_WGLGETGPUINFOAMD _wglGetGPUInfoAMD;

typedef UINT(__stdcall * PFN_WGLGETCONTEXTGPUIDAMD)(HGLRC hglrc);
extern PFN_WGLGETCONTEXTGPUIDAMD _wglGetContextGPUIDAMD;

typedef HGLRC(__stdcall * PFN_WGLCREATEASSOCIATEDCONTEXTAMD)(UINT id);
extern PFN_WGLCREATEASSOCIATEDCONTEXTAMD _wglCreateAssociatedContextAMD;

typedef HGLRC(__stdcall * PFN_WGLCREATEASSOCIATEDCONTEXTATTRIBSAMD)(UINT id, HGLRC hShareContext, const int * attribList);
extern PFN_WGLCREATEASSOCIATEDCONTEXTATTRIBSAMD _wglCreateAssociatedContextAttribsAMD;

typedef BOOL(__stdcall * PFN_WGLDELETEASSOCIATEDCONTEXTAMD)(HGLRC hglrc);
extern PFN_WGLDELETEASSOCIATEDCONTEXTAMD _wglDeleteAssociatedContextAMD;

typedef BOOL(__stdcall * PFN_WGLMAKEASSOCIATEDCONTEXTCURRENTAMD)(HGLRC hglrc);
extern PFN_WGLMAKEASSOCIATEDCONTEXTCURRENTAMD _wglMakeAssociatedContextCurrentAMD;

typedef HGLRC(__stdcall * PFN_WGLGETCURRENTASSOCIATEDCONTEXTAMD)(void);
extern PFN_WGLGETCURRENTASSOCIATEDCONTEXTAMD _wglGetCurrentAssociatedContextAMD;

typedef void(__stdcall * PFN_WGLBLITCONTEXTFRAMEBUFFERAMD)(HGLRC dstCtx, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
extern PFN_WGLBLITCONTEXTFRAMEBUFFERAMD _wglBlitContextFramebufferAMD;

typedef BOOL(__stdcall * PFN_WGLBINDVIDEOCAPTUREDEVICENV)(UINT uVideoSlot, HVIDEOINPUTDEVICENV hDevice);
extern PFN_WGLBINDVIDEOCAPTUREDEVICENV _wglBindVideoCaptureDeviceNV;

typedef UINT(__stdcall * PFN_WGLENUMERATEVIDEOCAPTUREDEVICESNV)(HDC hDc, HVIDEOINPUTDEVICENV * phDeviceList);
extern PFN_WGLENUMERATEVIDEOCAPTUREDEVICESNV _wglEnumerateVideoCaptureDevicesNV;

typedef BOOL(__stdcall * PFN_WGLLOCKVIDEOCAPTUREDEVICENV)(HDC hDc, HVIDEOINPUTDEVICENV hDevice);
extern PFN_WGLLOCKVIDEOCAPTUREDEVICENV _wglLockVideoCaptureDeviceNV;

typedef BOOL(__stdcall * PFN_WGLQUERYVIDEOCAPTUREDEVICENV)(HDC hDc, HVIDEOINPUTDEVICENV hDevice, int iAttribute, int * piValue);
extern PFN_WGLQUERYVIDEOCAPTUREDEVICENV _wglQueryVideoCaptureDeviceNV;

typedef BOOL(__stdcall * PFN_WGLRELEASEVIDEOCAPTUREDEVICENV)(HDC hDc, HVIDEOINPUTDEVICENV hDevice);
extern PFN_WGLRELEASEVIDEOCAPTUREDEVICENV _wglReleaseVideoCaptureDeviceNV;

typedef BOOL(__stdcall * PFN_WGLCOPYIMAGESUBDATANV)(HGLRC hSrcRC, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, HGLRC hDstRC, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth);
extern PFN_WGLCOPYIMAGESUBDATANV _wglCopyImageSubDataNV;

typedef BOOL(__stdcall * PFN_WGLDXSETRESOURCESHAREHANDLENV)(void * dxObject, HANDLE shareHandle);
extern PFN_WGLDXSETRESOURCESHAREHANDLENV _wglDXSetResourceShareHandleNV;

typedef HANDLE(__stdcall * PFN_WGLDXOPENDEVICENV)(void * dxDevice);
extern PFN_WGLDXOPENDEVICENV _wglDXOpenDeviceNV;

typedef BOOL(__stdcall * PFN_WGLDXCLOSEDEVICENV)(HANDLE hDevice);
extern PFN_WGLDXCLOSEDEVICENV _wglDXCloseDeviceNV;

typedef HANDLE(__stdcall * PFN_WGLDXREGISTEROBJECTNV)(HANDLE hDevice, void * dxObject, GLuint name, GLenum type, GLenum access);
extern PFN_WGLDXREGISTEROBJECTNV _wglDXRegisterObjectNV;

typedef BOOL(__stdcall * PFN_WGLDXUNREGISTEROBJECTNV)(HANDLE hDevice, HANDLE hObject);
extern PFN_WGLDXUNREGISTEROBJECTNV _wglDXUnregisterObjectNV;

typedef BOOL(__stdcall * PFN_WGLDXOBJECTACCESSNV)(HANDLE hObject, GLenum access);
extern PFN_WGLDXOBJECTACCESSNV _wglDXObjectAccessNV;

typedef BOOL(__stdcall * PFN_WGLDXLOCKOBJECTSNV)(HANDLE hDevice, GLint count, HANDLE * hObjects);
extern PFN_WGLDXLOCKOBJECTSNV _wglDXLockObjectsNV;

typedef BOOL(__stdcall * PFN_WGLDXUNLOCKOBJECTSNV)(HANDLE hDevice, GLint count, HANDLE * hObjects);
extern PFN_WGLDXUNLOCKOBJECTSNV _wglDXUnlockObjectsNV;

typedef BOOL(__stdcall * PFN_WGLDELAYBEFORESWAPNV)(HDC hDC, GLfloat seconds);
extern PFN_WGLDELAYBEFORESWAPNV _wglDelayBeforeSwapNV;

/*************************************************************/
typedef void (APIENTRY * PFN_GLGETINTEGERV)(GLenum pname, GLint * params);
extern PFN_GLGETINTEGERV _glGetIntegerv;

typedef void (APIENTRY * PFN_GLGETBOOLEANV)(GLenum pname, GLboolean * params);
extern PFN_GLGETBOOLEANV _glGetBooleanv;

typedef void (APIENTRY * PFN_GLDISABLE)(GLenum cap);
extern PFN_GLDISABLE _glDisable;

typedef void (APIENTRY * PFN_GLVERTEX2D)(GLdouble x, GLdouble y);
extern PFN_GLVERTEX2D _glVertex2d;

typedef void (APIENTRY * PFN_GLCALLLIST)(GLuint list);
extern PFN_GLCALLLIST _glCallList;

typedef void (APIENTRY * PFN_GLMATERIALFV)(GLenum face, GLenum pname, const GLfloat * params);
extern PFN_GLMATERIALFV _glMaterialfv;

typedef void (APIENTRY * PFN_GLENABLE)(GLenum cap);
extern PFN_GLENABLE _glEnable;

typedef void (APIENTRY * PFN_GLPOPMATRIX)(void);
extern PFN_GLPOPMATRIX _glPopMatrix;
/*************************************************************/
//void initialiseRealOpenGLFunctions();
void initialiseVariables();
/*************************************************************/
//define intercepted necessary opengl functions
int WINAPI interceptedwglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd);
void WINAPI interceptedglClear(GLbitfield mask);
BOOL WINAPI interceptedwglSwapBuffers(HDC hdc);
BOOL WINAPI interceptedwglSetPixelFormat(HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR * ppfd);
void APIENTRY interceptedglRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void APIENTRY interceptedglColor3f(GLfloat red, GLfloat green, GLfloat blue);
void APIENTRY interceptedglLoadIdentity(void);
void APIENTRY interceptedglMultMatrixf(const GLfloat * m);
void APIENTRY interceptedglTranslated(GLdouble x, GLdouble y, GLdouble z);
void APIENTRY interceptedglLightfv(GLenum light, GLenum pname, const GLfloat * params);
void APIENTRY interceptedglTranslatef(GLfloat x, GLfloat y, GLfloat z);
void APIENTRY interceptedglBegin(GLenum mode);
void APIENTRY interceptedglVertex3f(GLfloat x, GLfloat y, GLfloat z);
void APIENTRY interceptedglEnd(void);
void APIENTRY interceptedglFlush(void);

BOOL APIENTRY interceptedwglMakeCurrent(HDC hdc, HGLRC hglrc);
void APIENTRY interceptedglScaled(GLdouble x, GLdouble y, GLdouble z);
void APIENTRY interceptedglColor3d(GLdouble red, GLdouble green, GLdouble blue);
void APIENTRY interceptedglPushMatrix(void);
void APIENTRY interceptedglMultMatrixd(const GLdouble * m);
HGLRC APIENTRY interceptedwglCreateContext(HDC hdc);
BOOL APIENTRY interceptedwglDeleteContext(HGLRC hglrc);
HGLRC APIENTRY interceptedwglGetCurrentContext(void);
BOOL APIENTRY interceptedwglCopyContext(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask);
HDC APIENTRY interceptedwglGetCurrentDC(void);
PROC APIENTRY interceptedwglGetDefaultProcAddress(const char * lpszProc);
BOOL APIENTRY interceptedwglShareLists(HGLRC hglrc1, HGLRC hglrc2);
HGLRC APIENTRY interceptedwglCreateLayerContext(HDC hdc, int iLayerPlane);
BOOL APIENTRY interceptedwglDescribeLayerPlane(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LAYERPLANEDESCRIPTOR * plpd);
int APIENTRY interceptedwglSetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, const COLORREF * pcr);
int APIENTRY interceptedwglGetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF * pcr);
BOOL APIENTRY interceptedwglRealizeLayerPalette(HDC hdc, int iLayerPlane, BOOL bRealize);
BOOL APIENTRY interceptedwglSwapLayerBuffers(HDC hdc, UINT fuPlanes);
BOOL APIENTRY interceptedwglUseFontBitmapsA(HDC hdc, DWORD first, DWORD count, DWORD listBase);
BOOL APIENTRY interceptedwglUseFontBitmapsW(HDC hdc, DWORD first, DWORD count, DWORD listBase);
DWORD APIENTRY interceptedwglSwapMultipleBuffers(UINT n, const WGLSWAP * ps);
BOOL APIENTRY interceptedwglUseFontOutlinesA(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, GLYPHMETRICSFLOAT * lpgmf);
BOOL APIENTRY interceptedwglUseFontOutlinesW(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, GLYPHMETRICSFLOAT * lpgmf);
HANDLE APIENTRY interceptedwglCreateBufferRegionARB(HDC hDC, int iLayerPlane, UINT uType);
void APIENTRY interceptedwglDeleteBufferRegionARB(HANDLE hRegion);
BOOL APIENTRY interceptedwglSaveBufferRegionARB(HANDLE hRegion, int x, int y, int width, int height);
BOOL APIENTRY interceptedwglRestoreBufferRegionARB(HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc);
const char * APIENTRY interceptedwglGetExtensionsStringARB(HDC hdc);
BOOL APIENTRY interceptedwglGetPixelFormatAttribivARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, int * piValues);
BOOL APIENTRY interceptedwglGetPixelFormatAttribfvARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, FLOAT * pfValues);
BOOL APIENTRY interceptedwglMakeContextCurrentARB(HDC hDrawDC, HDC hReadDC, HGLRC hglrc);
HDC APIENTRY interceptedwglGetCurrentReadDCARB(void);
HPBUFFERARB APIENTRY interceptedwglCreatePbufferARB(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int * piAttribList);
HDC APIENTRY interceptedwglGetPbufferDCARB(HPBUFFERARB hPbuffer);
int APIENTRY interceptedwglReleasePbufferDCARB(HPBUFFERARB hPbuffer, HDC hDC);
BOOL APIENTRY interceptedwglDestroyPbufferARB(HPBUFFERARB hPbuffer);
BOOL APIENTRY interceptedwglQueryPbufferARB(HPBUFFERARB hPbuffer, int iAttribute, int * piValue);
BOOL APIENTRY interceptedwglBindTexImageARB(HPBUFFERARB hPbuffer, int iBuffer);
BOOL APIENTRY interceptedwglReleaseTexImageARB(HPBUFFERARB hPbuffer, int iBuffer);
BOOL APIENTRY interceptedwglSetPbufferAttribARB(HPBUFFERARB hPbuffer, const int * piAttribList);
HGLRC APIENTRY interceptedwglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int * attribList);
GLboolean APIENTRY interceptedwglCreateDisplayColorTableEXT(GLushort id);
GLboolean APIENTRY interceptedwglLoadDisplayColorTableEXT(const GLushort * table, GLuint length);
GLboolean APIENTRY interceptedwglBindDisplayColorTableEXT(GLushort id);
void APIENTRY interceptedwglDestroyDisplayColorTableEXT(GLushort id);
const char * APIENTRY interceptedwglGetExtensionsStringEXT(void);
BOOL APIENTRY interceptedwglMakeContextCurrentEXT(HDC hDrawDC, HDC hReadDC, HGLRC hglrc);
HDC APIENTRY interceptedwglGetCurrentReadDCEXT(void);
HPBUFFEREXT APIENTRY interceptedwglCreatePbufferEXT(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int * piAttribList);
HDC APIENTRY interceptedwglGetPbufferDCEXT(HPBUFFEREXT hPbuffer);
int APIENTRY interceptedwglReleasePbufferDCEXT(HPBUFFEREXT hPbuffer, HDC hDC);
BOOL APIENTRY interceptedwglDestroyPbufferEXT(HPBUFFEREXT hPbuffer);
BOOL APIENTRY interceptedwglQueryPbufferEXT(HPBUFFEREXT hPbuffer, int iAttribute, int * piValue);
BOOL APIENTRY interceptedwglGetPixelFormatAttribivEXT(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, int * piValues);
BOOL APIENTRY interceptedwglGetPixelFormatAttribfvEXT(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, FLOAT * pfValues);
BOOL APIENTRY interceptedwglChoosePixelFormatEXT(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats);
BOOL APIENTRY interceptedwglSwapIntervalEXT(int interval);
int APIENTRY interceptedwglGetSwapIntervalEXT(void);
void * APIENTRY interceptedwglAllocateMemoryNV(GLsizei size, GLfloat readfreq, GLfloat writefreq, GLfloat priority);
void APIENTRY interceptedwglFreeMemoryNV(void * pointer);
BOOL APIENTRY interceptedwglGetSyncValuesOML(HDC hdc, INT64 * ust, INT64 * msc, INT64 * sbc);
BOOL APIENTRY interceptedwglGetMscRateOML(HDC hdc, INT32 * numerator, INT32 * denominator);
INT64 APIENTRY interceptedwglSwapBuffersMscOML(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder);
INT64 APIENTRY interceptedwglSwapLayerBuffersMscOML(HDC hdc, int fuPlanes, INT64 target_msc, INT64 divisor, INT64 remainder);
BOOL APIENTRY interceptedwglWaitForMscOML(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder, INT64 * ust, INT64 * msc, INT64 * sbc);
BOOL APIENTRY interceptedwglWaitForSbcOML(HDC hdc, INT64 target_sbc, INT64 * ust, INT64 * msc, INT64 * sbc);
BOOL APIENTRY interceptedwglGetDigitalVideoParametersI3D(HDC hDC, int iAttribute, int * piValue);
BOOL APIENTRY interceptedwglSetDigitalVideoParametersI3D(HDC hDC, int iAttribute, const int * piValue);
BOOL APIENTRY interceptedwglGetGammaTableParametersI3D(HDC hDC, int iAttribute, int * piValue);
BOOL APIENTRY interceptedwglSetGammaTableParametersI3D(HDC hDC, int iAttribute, const int * piValue);
BOOL APIENTRY interceptedwglGetGammaTableI3D(HDC hDC, int iEntries, USHORT * puRed, USHORT * puGreen, USHORT * puBlue);
BOOL APIENTRY interceptedwglSetGammaTableI3D(HDC hDC, int iEntries, const USHORT * puRed, const USHORT * puGreen, const USHORT * puBlue);
BOOL APIENTRY interceptedwglEnableGenlockI3D(HDC hDC);
BOOL APIENTRY interceptedwglDisableGenlockI3D(HDC hDC);
BOOL APIENTRY interceptedwglIsEnabledGenlockI3D(HDC hDC, BOOL * pFlag);
BOOL APIENTRY interceptedwglGenlockSourceI3D(HDC hDC, UINT uSource);
BOOL APIENTRY interceptedwglGetGenlockSourceI3D(HDC hDC, UINT * uSource);
BOOL APIENTRY interceptedwglGenlockSourceEdgeI3D(HDC hDC, UINT uEdge);
BOOL APIENTRY interceptedwglGetGenlockSourceEdgeI3D(HDC hDC, UINT * uEdge);
BOOL APIENTRY interceptedwglGenlockSampleRateI3D(HDC hDC, UINT uRate);
BOOL APIENTRY interceptedwglGetGenlockSampleRateI3D(HDC hDC, UINT * uRate);
BOOL APIENTRY interceptedwglGenlockSourceDelayI3D(HDC hDC, UINT uDelay);
BOOL APIENTRY interceptedwglGetGenlockSourceDelayI3D(HDC hDC, UINT * uDelay);
BOOL APIENTRY interceptedwglQueryGenlockMaxSourceDelayI3D(HDC hDC, UINT * uMaxLineDelay, UINT * uMaxPixelDelay);
void * APIENTRY interceptedwglCreateImageBufferI3D(HDC hDC, DWORD dwSize, UINT uFlags);
BOOL APIENTRY interceptedwglDestroyImageBufferI3D(HDC hDC, void * pAddress);
BOOL APIENTRY interceptedwglAssociateImageBufferEventsI3D(HDC hDC, const HANDLE * pEvent, void * const * pAddress, const DWORD * pSize, UINT count);
BOOL APIENTRY interceptedwglReleaseImageBufferEventsI3D(HDC hDC, void * const * pAddress, UINT count);
BOOL APIENTRY interceptedwglEnableFrameLockI3D(void);
BOOL APIENTRY interceptedwglDisableFrameLockI3D(void);
BOOL APIENTRY interceptedwglIsEnabledFrameLockI3D(BOOL * pFlag);
BOOL APIENTRY interceptedwglQueryFrameLockMasterI3D(BOOL * pFlag);
BOOL APIENTRY interceptedwglGetFrameUsageI3D(float * pUsage);
BOOL APIENTRY interceptedwglBeginFrameTrackingI3D(void);
BOOL APIENTRY interceptedwglEndFrameTrackingI3D(void);
BOOL APIENTRY interceptedwglQueryFrameTrackingI3D(DWORD * pFrameCount, DWORD * pMissedFrames, float * pLastMissedUsage);
BOOL APIENTRY interceptedwglSetStereoEmitterState3DL(HDC hDC, UINT uState);
int APIENTRY interceptedwglEnumerateVideoDevicesNV(HDC hDC, HVIDEOOUTPUTDEVICENV * phDeviceList);
BOOL APIENTRY interceptedwglBindVideoDeviceNV(HDC hDC, unsigned int uVideoSlot, HVIDEOOUTPUTDEVICENV hVideoDevice, const int * piAttribList);
BOOL APIENTRY interceptedwglQueryCurrentContextNV(int iAttribute, int * piValue);
BOOL APIENTRY interceptedwglGetVideoDeviceNV(HDC hDC, int numDevices, HPVIDEODEV * hVideoDevice);
BOOL APIENTRY interceptedwglReleaseVideoDeviceNV(HPVIDEODEV hVideoDevice);
BOOL APIENTRY interceptedwglBindVideoImageNV(HPVIDEODEV hVideoDevice, HPBUFFERARB hPbuffer, int iVideoBuffer);
BOOL APIENTRY interceptedwglReleaseVideoImageNV(HPBUFFERARB hPbuffer, int iVideoBuffer);
BOOL APIENTRY interceptedwglSendPbufferToVideoNV(HPBUFFERARB hPbuffer, int iBufferType, unsigned long * pulCounterPbuffer, BOOL bBlock);
BOOL APIENTRY interceptedwglGetVideoInfoNV(HPVIDEODEV hpVideoDevice, unsigned long * pulCounterOutputPbuffer, unsigned long * pulCounterOutputVideo);
BOOL APIENTRY interceptedwglJoinSwapGroupNV(HDC hDC, GLuint group);
BOOL APIENTRY interceptedwglBindSwapBarrierNV(GLuint group, GLuint barrier);
BOOL APIENTRY interceptedwglQuerySwapGroupNV(HDC hDC, GLuint * group, GLuint * barrier);
BOOL APIENTRY interceptedwglQueryMaxSwapGroupsNV(HDC hDC, GLuint * maxGroups, GLuint * maxBarriers);
BOOL APIENTRY interceptedwglQueryFrameCountNV(HDC hDC, GLuint * count);
BOOL APIENTRY interceptedwglResetFrameCountNV(HDC hDC);
BOOL APIENTRY interceptedwglEnumGpusNV(UINT iGpuIndex, HGPUNV * phGpu);
BOOL APIENTRY interceptedwglEnumGpuDevicesNV(HGPUNV hGpu, UINT iDeviceIndex, _GPU_DEVICE * lpGpuDevice);
HDC APIENTRY interceptedwglCreateAffinityDCNV(const HGPUNV * phGpuList);
BOOL APIENTRY interceptedwglEnumGpusFromAffinityDCNV(HDC hAffinityDC, UINT iGpuIndex, HGPUNV * hGpu);
BOOL APIENTRY interceptedwglDeleteDCNV(HDC hdc);
UINT APIENTRY interceptedwglGetGPUIDsAMD(UINT maxCount, UINT * ids);
INT APIENTRY interceptedwglGetGPUInfoAMD(UINT id, int property, GLenum dataType, UINT size, void * data);
UINT APIENTRY interceptedwglGetContextGPUIDAMD(HGLRC hglrc);
HGLRC APIENTRY interceptedwglCreateAssociatedContextAMD(UINT id);
HGLRC APIENTRY interceptedwglCreateAssociatedContextAttribsAMD(UINT id, HGLRC hShareContext, const int * attribList);
BOOL APIENTRY interceptedwglDeleteAssociatedContextAMD(HGLRC hglrc);
BOOL APIENTRY interceptedwglMakeAssociatedContextCurrentAMD(HGLRC hglrc);
HGLRC APIENTRY interceptedwglGetCurrentAssociatedContextAMD(void);
void APIENTRY interceptedwglBlitContextFramebufferAMD(HGLRC dstCtx, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
BOOL APIENTRY interceptedwglBindVideoCaptureDeviceNV(UINT uVideoSlot, HVIDEOINPUTDEVICENV hDevice);
UINT APIENTRY interceptedwglEnumerateVideoCaptureDevicesNV(HDC hDc, HVIDEOINPUTDEVICENV * phDeviceList);
BOOL APIENTRY interceptedwglLockVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice);
BOOL APIENTRY interceptedwglQueryVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice, int iAttribute, int * piValue);
BOOL APIENTRY interceptedwglReleaseVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice);
BOOL APIENTRY interceptedwglCopyImageSubDataNV(HGLRC hSrcRC, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, HGLRC hDstRC, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth);
BOOL APIENTRY interceptedwglDXSetResourceShareHandleNV(void * dxObject, HANDLE shareHandle);
HANDLE APIENTRY interceptedwglDXOpenDeviceNV(void * dxDevice);
BOOL APIENTRY interceptedwglDXCloseDeviceNV(HANDLE hDevice);
HANDLE APIENTRY interceptedwglDXRegisterObjectNV(HANDLE hDevice, void * dxObject, GLuint name, GLenum type, GLenum access);
BOOL APIENTRY interceptedwglDXUnregisterObjectNV(HANDLE hDevice, HANDLE hObject);
BOOL APIENTRY interceptedwglDXObjectAccessNV(HANDLE hObject, GLenum access);
BOOL APIENTRY interceptedwglDXLockObjectsNV(HANDLE hDevice, GLint count, HANDLE * hObjects);
BOOL APIENTRY interceptedwglDXUnlockObjectsNV(HANDLE hDevice, GLint count, HANDLE * hObjects);
BOOL APIENTRY interceptedwglDelayBeforeSwapNV(HDC hDC, GLfloat seconds);
/*************************************************************/
void APIENTRY interceptedglGetIntegerv(GLenum pname, GLint * params);
void APIENTRY interceptedglGetBooleanv(GLenum pname, GLboolean * params);
void APIENTRY interceptedglDisable(GLenum cap);
void APIENTRY interceptedglVertex2d(GLdouble x, GLdouble y);
void APIENTRY interceptedglCallList(GLuint list);
void APIENTRY interceptedglMaterialfv(GLenum face, GLenum pname, const GLfloat * params);
void APIENTRY interceptedglEnable(GLenum cap);
void APIENTRY interceptedglPopMatrix(void);

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