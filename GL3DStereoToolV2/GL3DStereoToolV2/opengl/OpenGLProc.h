#include <Windows.h>
#include <windef.h>
#include <minwindef.h>
#include <GL/gl.h>

using namespace std;

HMODULE _libGlHandle = NULL;

HDC currentOpenGLContext;

bool g_stereoDetect = false;    ///< have we detected incoming stereo frames?
unsigned g_clearsPerEye = 0;    ///< number of glClear calls per eye
unsigned g_clearCount = 0;    ///< used to count number of glClear calls

void * _getPublicProcAddress(const char *procName);
void * _getPrivateProcAddress(const char *procName);

typedef PROC(__stdcall * PFN_WGLGETPROCADDRESS)(const char * lpszProc);
extern PFN_WGLGETPROCADDRESS _wglGetProcAddress;

/*************************************************************/
// define real opengl functions
/// define function type for glClear
typedef void (APIENTRY * PFN_GLCLEAR)(GLbitfield mask);
extern PFN_GLCLEAR _glClear;

typedef void (APIENTRY * PFN_GLCLEARCOLOR)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern PFN_GLCLEARCOLOR _glClearColor;

typedef void (APIENTRY * PFN_GLCLEARDEPTH)(GLdouble depth);
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
BOOL APIENTRY interceptedwglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd);

BOOL APIENTRY interceptedglClear(GLbitfield mask);
/*************************************************************/