#include "OpenGLProc.h"

#include <GL\GL.h>

#include <stdio.h>
#include <iostream>
#include "GLWindow.h"

bool g_stereoDetect = false;
unsigned g_clearsPerEye = 0;
unsigned g_clearCount = 0;
HMODULE _libGlHandle = NULL;
HDC currentOpenGLContext;

//unsigned m_framesGL = 0;    ///< OpenGL frame count

unsigned m_samplesGL;   ///< OpenGL multisamples (or 0)

unsigned m_drawBuffer;  ///< buffer to draw to
unsigned m_readBuffer;  ///< buffer to read from

unsigned m_width;       ///< display width in pixels
unsigned m_height;      ///< display height in pixels

bool m_initialised;     ///< has initialisation completed?

/// Set of all render targets that have been presented
std::set< unsigned > m_presentedTargets;

std::tr1::array<Target, 3> m_target; ///< DX/GL targets for rendering

bool     m_verbose;             ///< Verbose logging

bool     m_stereoMode;          ///< Stereo mode enable/disable
bool     m_stereoAvailable;     ///< Is quad-buffer stereo available?
//unsigned m_clearCount;          ///< Number of clears per frame
unsigned m_clearCountPersist;   ///< Persistent number of clears

double   m_firstFrameTimeGL;    ///< time-stamp of first GL frame
double   m_lastFrameTimeGL;     ///< time-stamp of last GL frame

GLuint   m_quadListGL;          ///< GL display list for textured quad

uintptr_t m_thread;             ///< Handle of the rendering thread

Event m_frameDone;              ///< Signals when frame is rendered out

Extensions glx;                 ///< Stores the OpenGL extension functions

bool useTexture = false;
bool mustUseBlit;
GLint textureSize;

int widthScreen = 1920;
int heightScreen = 1080;

PIXELFORMATDESCRIPTOR currentStereo_ppfd;
int currentStereoPixelFormat;
bool increaseClearCount = false;

long functionCallIndex = 0;

Config3DSettings currentConfig3DSettings;
bool inLeft = false;
bool firstForground = false;
/************************************************************************************/
void * _getPublicProcAddress(const char *procName)
{
	//Log::open("intercept.log");
	//Log::print("_getPublicProcAddress\n");

	if (!_libGlHandle) {
		char szDll[MAX_PATH] = { 0 };

		if (!GetSystemDirectoryA(szDll, MAX_PATH)) {
			return NULL;
		}

		//Log::print("System directory: ");
		//Log::print(szDll);
		
		strcat_s(szDll, "\\opengl32.dll");
		//Log::print("System directory: ");
		//Log::print(szDll);
		
		//Log::print("\n");
		
		_libGlHandle = LoadLibraryA(szDll);

		if (!_libGlHandle) {
			Log::print("couldn't load opengl32.dll\n");
			return NULL;
		}

		//Log::print("Can get opengl handle.");

	}
	//Log::print("Proc name: ");
	//Log::print(procName);
	//Log::print("\n");

	return (void *)GetProcAddress(_libGlHandle, procName);
}

void * _getPrivateProcAddress(const char *procName) {
	return (void *)_wglGetProcAddress(procName);
}
/************************************************************************************/
static PROC __stdcall _fail_wglGetProcAddress(const char * lpszProc) {
	const char *_name = "wglGetProcAddress";
	Log::print("error: unavailable function ");
	Log::print(_name);
	Log::print("\n");
	//os::abort();
	return NULL;
}

static PROC __stdcall _get_wglGetProcAddress(const char * lpszProc) {
	PFN_WGLGETPROCADDRESS _ptr;
	_ptr = (PFN_WGLGETPROCADDRESS)_getPublicProcAddress("wglGetProcAddress");
	if (!_ptr) {
		_ptr = &_fail_wglGetProcAddress;
	}
	_wglGetProcAddress = _ptr;
	return _wglGetProcAddress(lpszProc);
}

PFN_WGLGETPROCADDRESS _wglGetProcAddress = &_get_wglGetProcAddress;

/************************************************************************************/
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
/************************************************************************************/
BOOL __stdcall _fail_wglChoosePixelFormatARB(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats) {
	const char *_name = "wglChoosePixelFormatARB";
	Log::print("error: unavailable function ");
	Log::print(_name);
	Log::print("\n");
	return 0;
}

BOOL __stdcall _get_wglChoosePixelFormatARB(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats) {
	PFN_WGLCHOOSEPIXELFORMATARB _ptr;
	_ptr = (PFN_WGLCHOOSEPIXELFORMATARB)_getPrivateProcAddress("wglChoosePixelFormatARB");
	if (!_ptr) {
		_ptr = &_fail_wglChoosePixelFormatARB;
	}
	_wglChoosePixelFormatARB = _ptr;
	return _wglChoosePixelFormatARB(hdc, piAttribIList, pfAttribFList, nMaxFormats, piFormats, nNumFormats);
}

PFN_WGLCHOOSEPIXELFORMATARB _wglChoosePixelFormatARB = &_get_wglChoosePixelFormatARB;
/************************************************************************************/
BOOL __stdcall _fail_wglSwapBuffers(HDC hdc) {
	const char *_name = "wglSwapBuffers";
	Log::print("error: unavailable function ");
	Log::print(_name);
	Log::print("\n");
	return 0;
}

BOOL __stdcall _get_wglSwapBuffers(HDC hdc) {
	PFN_WGLSWAPBUFFERS _ptr;
	//_ptr = (PFN_WGLSWAPBUFFERS)_getPublicProcAddress("wglSwapBuffers");
	_ptr = (PFN_WGLSWAPBUFFERS)_getAnyGLFuncAddress("wglSwapBuffers");
	if (!_ptr) {
		_ptr = &_fail_wglSwapBuffers;
	}
	_wglSwapBuffers = _ptr;
	return _wglSwapBuffers(hdc);
}

PFN_WGLSWAPBUFFERS _wglSwapBuffers = &_get_wglSwapBuffers;
/************************************************************************************/
int __stdcall _fail_wglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd) {
	const char *_name = "wglChoosePixelFormat";
	Log::print("error: unavailable function ");
	Log::print(_name);
	Log::print("\n");
	return 0;
}

int __stdcall _get_wglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd) {
	//Log::open("intercept.log");
	Log::print("OK: get _get_wglChoosePixelFormat function \n");
	PFN_WGLCHOOSEPIXELFORMAT _ptr;
	//_ptr = (PFN_WGLCHOOSEPIXELFORMAT)_getPublicProcAddress("wglChoosePixelFormat");
	_ptr = (PFN_WGLCHOOSEPIXELFORMAT)_getAnyGLFuncAddress("wglChoosePixelFormat");
	if (!_ptr) {
		_ptr = &_fail_wglChoosePixelFormat;
	}
	_wglChoosePixelFormat = _ptr;
	return _wglChoosePixelFormat(hdc, ppfd);
}

PFN_WGLCHOOSEPIXELFORMAT _wglChoosePixelFormat = &_get_wglChoosePixelFormat;
/************************************************************************************/
BOOL __stdcall _fail_wglSetPixelFormat(HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR * ppfd) {
	const char *_name = "wglSetPixelFormat";
	Log::print("error: unavailable function ");
	Log::print(_name);
	Log::print("\n");
	return 0;
}

BOOL __stdcall _get_wglSetPixelFormat(HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR * ppfd) {
	PFN_WGLSETPIXELFORMAT _ptr;
	_ptr = (PFN_WGLSETPIXELFORMAT)_getPublicProcAddress("wglSetPixelFormat");
	if (!_ptr) {
		_ptr = &_fail_wglSetPixelFormat;
	}
	_wglSetPixelFormat = _ptr;
	return _wglSetPixelFormat(hdc, iPixelFormat, ppfd);
}

PFN_WGLSETPIXELFORMAT _wglSetPixelFormat = &_get_wglSetPixelFormat;
/************************************************************************************/
void WINAPI _fail_glClear(GLbitfield mask) {
	const char *_name = "glClear";
	//os::log("warning: ignoring call to unavailable function %s\n", _name);
	return;
}

void WINAPI _get_glClear(GLbitfield mask) {
	PFN_GLCLEAR _ptr;
	//_ptr = (PFN_GLCLEAR)_getPublicProcAddress("glClear");
	_ptr = (PFN_GLCLEAR)_getAnyGLFuncAddress("glClear");
	if (!_ptr) {
		_ptr = &_fail_glClear;
	}
	_glClear = _ptr;
	_glClear(mask);
}

PFN_GLCLEAR _glClear = &_get_glClear;
/************************************************************************************/
void WINAPI _fail_glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
	const char *_name = "glClearColor";
	//os::log("warning: ignoring call to unavailable function %s\n", _name);
	return;
}

void WINAPI _get_glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
	PFN_GLCLEARCOLOR _ptr;
	_ptr = (PFN_GLCLEARCOLOR)_getPublicProcAddress("glClearColor");
	if (!_ptr) {
		_ptr = &_fail_glClearColor;
	}
	_glClearColor = _ptr;
	_glClearColor(red, green, blue, alpha);
}

PFN_GLCLEARCOLOR _glClearColor = &_get_glClearColor;
/************************************************************************************/
void WINAPI _fail_glClearDepth(GLdouble depth) {
	const char *_name = "glClearDepth";
	//os::log("warning: ignoring call to unavailable function %s\n", _name);
	return;
}

void WINAPI _get_glClearDepth(GLdouble depth) {
	PFN_GLCLEARDEPTH _ptr;
	_ptr = (PFN_GLCLEARDEPTH)_getPublicProcAddress("glClearDepth");
	if (!_ptr) {
		_ptr = &_fail_glClearDepth;
	}
	_glClearDepth = _ptr;
	_glClearDepth(depth);
}

PFN_GLCLEARDEPTH _glClearDepth = &_get_glClearDepth;
/************************************************************************************/
int __stdcall _fail_wglGetPixelFormat(HDC hdc) {
	const char *_name = "wglGetPixelFormat";
	Log::print("error: unavailable function ");
	Log::print(_name);
	Log::print("\n");
	return 0;
}

int __stdcall _get_wglGetPixelFormat(HDC hdc) {
	PFN_WGLGETPIXELFORMAT _ptr;
	Log::print("OK: get wglGetPixelFormat function ");
	_ptr = (PFN_WGLGETPIXELFORMAT)_getPublicProcAddress("wglGetPixelFormat");
	if (!_ptr) {
		_ptr = &_fail_wglGetPixelFormat;
	}
	_wglGetPixelFormat = _ptr;
	return _wglGetPixelFormat(hdc);
}

PFN_WGLGETPIXELFORMAT _wglGetPixelFormat = &_get_wglGetPixelFormat;
/************************************************************************************/
int __stdcall _fail_wglDescribePixelFormat(HDC hdc, int iPixelFormat, UINT nBytes, PIXELFORMATDESCRIPTOR * ppfd) {
	const char *_name = "wglDescribePixelFormat";
	Log::print("error: unavailable function ");
	Log::print(_name);
	Log::print("\n");
	return 0;
}

int __stdcall _get_wglDescribePixelFormat(HDC hdc, int iPixelFormat, UINT nBytes, PIXELFORMATDESCRIPTOR * ppfd) {
	PFN_WGLDESCRIBEPIXELFORMAT _ptr;
	_ptr = (PFN_WGLDESCRIBEPIXELFORMAT)_getPublicProcAddress("wglDescribePixelFormat");
	if (!_ptr) {
		_ptr = &_fail_wglDescribePixelFormat;
	}
	_wglDescribePixelFormat = _ptr;
	return _wglDescribePixelFormat(hdc, iPixelFormat, nBytes, ppfd);
}

PFN_WGLDESCRIBEPIXELFORMAT _wglDescribePixelFormat = &_get_wglDescribePixelFormat;
/************************************************************************************/
void APIENTRY _fail_glColor3f(GLfloat red, GLfloat green, GLfloat blue) {
	const char *_name = "glColor3f";
	Log::print("error: unavailable function ");
	Log::print(_name);
	Log::print("\n");
	return;
}

void APIENTRY _get_glColor3f(GLfloat red, GLfloat green, GLfloat blue) {
	PFN_GLCOLOR3F _ptr;
	_ptr = (PFN_GLCOLOR3F)_getPublicProcAddress("glColor3f");
	if (!_ptr) {
		_ptr = &_fail_glColor3f;
	}
	_glColor3f = _ptr;
	_glColor3f(red, green, blue);
}

PFN_GLCOLOR3F _glColor3f = &_get_glColor3f;

void APIENTRY _fail_glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
	const char *_name = "glRotatef";
	Log::print("error: unavailable function ");
	Log::print(_name);
	Log::print("\n");
	return;
}

void APIENTRY _get_glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
	PFN_GLROTATEF _ptr;
	_ptr = (PFN_GLROTATEF)_getPublicProcAddress("glRotatef");
	if (!_ptr) {
		_ptr = &_fail_glRotatef;
	}
	_glRotatef = _ptr;
	_glRotatef(angle, x, y, z);
}

PFN_GLROTATEF _glRotatef = &_get_glRotatef;

static void APIENTRY _fail_glLoadIdentity(void) {
	const char *_name = "glLoadIdentity";
	
	return;
}

static void APIENTRY _get_glLoadIdentity(void) {
	PFN_GLLOADIDENTITY _ptr;
	_ptr = (PFN_GLLOADIDENTITY)_getPublicProcAddress("glLoadIdentity");
	if (!_ptr) {
		_ptr = &_fail_glLoadIdentity;
	}
	_glLoadIdentity = _ptr;
	_glLoadIdentity();
}

PFN_GLLOADIDENTITY _glLoadIdentity = &_get_glLoadIdentity;

static void APIENTRY _fail_glMultMatrixf(const GLfloat * m) {
	const char *_name = "glMultMatrixf";
	
	return;
}

static void APIENTRY _get_glMultMatrixf(const GLfloat * m) {
	PFN_GLMULTMATRIXF _ptr;
	_ptr = (PFN_GLMULTMATRIXF)_getPublicProcAddress("glMultMatrixf");
	if (!_ptr) {
		_ptr = &_fail_glMultMatrixf;
	}
	_glMultMatrixf = _ptr;
	_glMultMatrixf(m);
}

PFN_GLMULTMATRIXF _glMultMatrixf = &_get_glMultMatrixf;

static void APIENTRY _fail_glTranslated(GLdouble x, GLdouble y, GLdouble z) {
	const char *_name = "glTranslated";
	
	return;
}

static void APIENTRY _get_glTranslated(GLdouble x, GLdouble y, GLdouble z) {
	PFN_GLTRANSLATED _ptr;
	_ptr = (PFN_GLTRANSLATED)_getPublicProcAddress("glTranslated");
	if (!_ptr) {
		_ptr = &_fail_glTranslated;
	}
	_glTranslated = _ptr;
	_glTranslated(x, y, z);
}

PFN_GLTRANSLATED _glTranslated = &_get_glTranslated;

static void APIENTRY _fail_glLightfv(GLenum light, GLenum pname, const GLfloat * params) {
	const char *_name = "glLightfv";
	
	return;
}

static void APIENTRY _get_glLightfv(GLenum light, GLenum pname, const GLfloat * params) {
	PFN_GLLIGHTFV _ptr;
	_ptr = (PFN_GLLIGHTFV)_getPublicProcAddress("glLightfv");
	if (!_ptr) {
		_ptr = &_fail_glLightfv;
	}
	_glLightfv = _ptr;
	_glLightfv(light, pname, params);
}

PFN_GLLIGHTFV _glLightfv = &_get_glLightfv;

static void APIENTRY _fail_glTranslatef(GLfloat x, GLfloat y, GLfloat z) {
	const char *_name = "glTranslatef";
	
	return;
}

static void APIENTRY _get_glTranslatef(GLfloat x, GLfloat y, GLfloat z) {
	PFN_GLTRANSLATEF _ptr;
	_ptr = (PFN_GLTRANSLATEF)_getPublicProcAddress("glTranslatef");
	if (!_ptr) {
		_ptr = &_fail_glTranslatef;
	}
	_glTranslatef = _ptr;
	_glTranslatef(x, y, z);
}

PFN_GLTRANSLATEF _glTranslatef = &_get_glTranslatef;

static void APIENTRY _fail_glBegin(GLenum mode) {
	const char *_name = "glBegin";
	
	return;
}

static void APIENTRY _get_glBegin(GLenum mode) {
	PFN_GLBEGIN _ptr;
	_ptr = (PFN_GLBEGIN)_getPublicProcAddress("glBegin");
	if (!_ptr) {
		_ptr = &_fail_glBegin;
	}
	_glBegin = _ptr;
	_glBegin(mode);
}

PFN_GLBEGIN _glBegin = &_get_glBegin;

static void APIENTRY _fail_glVertex3f(GLfloat x, GLfloat y, GLfloat z) {
	const char *_name = "glVertex3f";

	return;
}

static void APIENTRY _get_glVertex3f(GLfloat x, GLfloat y, GLfloat z) {
	PFN_GLVERTEX3F _ptr;
	_ptr = (PFN_GLVERTEX3F)_getPublicProcAddress("glVertex3f");
	if (!_ptr) {
		_ptr = &_fail_glVertex3f;
	}
	_glVertex3f = _ptr;
	_glVertex3f(x, y, z);
}

PFN_GLVERTEX3F _glVertex3f = &_get_glVertex3f;

static void APIENTRY _fail_glEnd(void) {
	const char *_name = "glEnd";
	
	return;
}

static void APIENTRY _get_glEnd(void) {
	PFN_GLEND _ptr;
	_ptr = (PFN_GLEND)_getPublicProcAddress("glEnd");
	if (!_ptr) {
		_ptr = &_fail_glEnd;
	}
	_glEnd = _ptr;
	_glEnd();
}

PFN_GLEND _glEnd = &_get_glEnd;

static void APIENTRY _fail_glFlush(void) {
	const char *_name = "glFlush";
	
	return;
}

static void APIENTRY _get_glFlush(void) {
	PFN_GLFLUSH _ptr;
	_ptr = (PFN_GLFLUSH)_getPublicProcAddress("glFlush");
	if (!_ptr) {
		_ptr = &_fail_glFlush;
	}
	_glFlush = _ptr;
	_glFlush();
}

PFN_GLFLUSH _glFlush = &_get_glFlush;

static BOOL __stdcall _fail_wglMakeCurrent(HDC hdc, HGLRC hglrc) {
	const char *_name = "wglMakeCurrent";
	
	return false;
}

static BOOL __stdcall _get_wglMakeCurrent(HDC hdc, HGLRC hglrc) {
	PFN_WGLMAKECURRENT _ptr;
	_ptr = (PFN_WGLMAKECURRENT)_getPublicProcAddress("wglMakeCurrent");
	if (!_ptr) {
		_ptr = &_fail_wglMakeCurrent;
	}
	_wglMakeCurrent = _ptr;
	return _wglMakeCurrent(hdc, hglrc);
}

PFN_WGLMAKECURRENT _wglMakeCurrent = &_get_wglMakeCurrent;

static void APIENTRY _fail_glScaled(GLdouble x, GLdouble y, GLdouble z) {
	const char *_name = "glScaled";
	
	return;
}

static void APIENTRY _get_glScaled(GLdouble x, GLdouble y, GLdouble z) {
	PFN_GLSCALED _ptr;
	_ptr = (PFN_GLSCALED)_getPublicProcAddress("glScaled");
	if (!_ptr) {
		_ptr = &_fail_glScaled;
	}
	_glScaled = _ptr;
	_glScaled(x, y, z);
}

PFN_GLSCALED _glScaled = &_get_glScaled;

static void APIENTRY _fail_glColor3d(GLdouble red, GLdouble green, GLdouble blue) {
	const char *_name = "glColor3d";
	
	return;
}

static void APIENTRY _get_glColor3d(GLdouble red, GLdouble green, GLdouble blue) {
	PFN_GLCOLOR3D _ptr;
	_ptr = (PFN_GLCOLOR3D)_getPublicProcAddress("glColor3d");
	if (!_ptr) {
		_ptr = &_fail_glColor3d;
	}
	_glColor3d = _ptr;
	_glColor3d(red, green, blue);
}

PFN_GLCOLOR3D _glColor3d = &_get_glColor3d;

static void APIENTRY _fail_glPushMatrix(void) {
	const char *_name = "glPushMatrix";
	
	return;
}

static void APIENTRY _get_glPushMatrix(void) {
	PFN_GLPUSHMATRIX _ptr;
	_ptr = (PFN_GLPUSHMATRIX)_getPublicProcAddress("glPushMatrix");
	if (!_ptr) {
		_ptr = &_fail_glPushMatrix;
	}
	_glPushMatrix = _ptr;
	_glPushMatrix();
}

PFN_GLPUSHMATRIX _glPushMatrix = &_get_glPushMatrix;

static void APIENTRY _fail_glMultMatrixd(const GLdouble * m) {
	const char *_name = "glMultMatrixd";
	
	return;
}

static void APIENTRY _get_glMultMatrixd(const GLdouble * m) {
	PFN_GLMULTMATRIXD _ptr;
	_ptr = (PFN_GLMULTMATRIXD)_getPublicProcAddress("glMultMatrixd");
	if (!_ptr) {
		_ptr = &_fail_glMultMatrixd;
	}
	_glMultMatrixd = _ptr;
	_glMultMatrixd(m);
}

PFN_GLMULTMATRIXD _glMultMatrixd = &_get_glMultMatrixd;


static HGLRC __stdcall _fail_wglCreateContext(HDC hdc) {
	const char *_name = "wglCreateContext";
	
	return false;
}

static HGLRC __stdcall _get_wglCreateContext(HDC hdc) {
	PFN_WGLCREATECONTEXT _ptr;
	_ptr = (PFN_WGLCREATECONTEXT)_getPublicProcAddress("wglCreateContext");
	if (!_ptr) {
		_ptr = &_fail_wglCreateContext;
	}
	_wglCreateContext = _ptr;
	return _wglCreateContext(hdc);
}

PFN_WGLCREATECONTEXT _wglCreateContext = &_get_wglCreateContext;

static BOOL __stdcall _fail_wglDeleteContext(HGLRC hglrc) {
	const char *_name = "wglDeleteContext";
	
	return false;
}

static BOOL __stdcall _get_wglDeleteContext(HGLRC hglrc) {
	PFN_WGLDELETECONTEXT _ptr;
	_ptr = (PFN_WGLDELETECONTEXT)_getPublicProcAddress("wglDeleteContext");
	if (!_ptr) {
		_ptr = &_fail_wglDeleteContext;
	}
	_wglDeleteContext = _ptr;
	return _wglDeleteContext(hglrc);
}

PFN_WGLDELETECONTEXT _wglDeleteContext = &_get_wglDeleteContext;

static HGLRC __stdcall _fail_wglGetCurrentContext(void) {
	const char *_name = "wglGetCurrentContext";

	return false;
}

static HGLRC __stdcall _get_wglGetCurrentContext(void) {
	PFN_WGLGETCURRENTCONTEXT _ptr;
	_ptr = (PFN_WGLGETCURRENTCONTEXT)_getPublicProcAddress("wglGetCurrentContext");
	if (!_ptr) {
		_ptr = &_fail_wglGetCurrentContext;
	}
	_wglGetCurrentContext = _ptr;
	return _wglGetCurrentContext();
}

PFN_WGLGETCURRENTCONTEXT _wglGetCurrentContext = &_get_wglGetCurrentContext;

static BOOL __stdcall _fail_wglCopyContext(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask) {
	const char *_name = "wglCopyContext";
	
	return false;
}

static BOOL __stdcall _get_wglCopyContext(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask) {
	PFN_WGLCOPYCONTEXT _ptr;
	_ptr = (PFN_WGLCOPYCONTEXT)_getPublicProcAddress("wglCopyContext");
	if (!_ptr) {
		_ptr = &_fail_wglCopyContext;
	}
	_wglCopyContext = _ptr;
	return _wglCopyContext(hglrcSrc, hglrcDst, mask);
}

PFN_WGLCOPYCONTEXT _wglCopyContext = &_get_wglCopyContext;

static HDC __stdcall _fail_wglGetCurrentDC(void) {
	const char *_name = "wglGetCurrentDC";
	return false;
}

static HDC __stdcall _get_wglGetCurrentDC(void) {
	PFN_WGLGETCURRENTDC _ptr;
	_ptr = (PFN_WGLGETCURRENTDC)_getPublicProcAddress("wglGetCurrentDC");
	if (!_ptr) {
		_ptr = &_fail_wglGetCurrentDC;
	}
	_wglGetCurrentDC = _ptr;
	return _wglGetCurrentDC();
}

PFN_WGLGETCURRENTDC _wglGetCurrentDC = &_get_wglGetCurrentDC;

static PROC __stdcall _fail_wglGetDefaultProcAddress(const char * lpszProc) {
	const char *_name = "wglGetDefaultProcAddress";
	
	return false;
}

static PROC __stdcall _get_wglGetDefaultProcAddress(const char * lpszProc) {
	PFN_WGLGETDEFAULTPROCADDRESS _ptr;
	_ptr = (PFN_WGLGETDEFAULTPROCADDRESS)_getPublicProcAddress("wglGetDefaultProcAddress");
	if (!_ptr) {
		_ptr = &_fail_wglGetDefaultProcAddress;
	}
	_wglGetDefaultProcAddress = _ptr;
	return _wglGetDefaultProcAddress(lpszProc);
}

PFN_WGLGETDEFAULTPROCADDRESS _wglGetDefaultProcAddress = &_get_wglGetDefaultProcAddress;

static BOOL __stdcall _fail_wglShareLists(HGLRC hglrc1, HGLRC hglrc2) {
	const char *_name = "wglShareLists";
	return false;
}

static BOOL __stdcall _get_wglShareLists(HGLRC hglrc1, HGLRC hglrc2) {
	PFN_WGLSHARELISTS _ptr;
	_ptr = (PFN_WGLSHARELISTS)_getPublicProcAddress("wglShareLists");
	if (!_ptr) {
		_ptr = &_fail_wglShareLists;
	}
	_wglShareLists = _ptr;
	return _wglShareLists(hglrc1, hglrc2);
}

PFN_WGLSHARELISTS _wglShareLists = &_get_wglShareLists;

static HGLRC __stdcall _fail_wglCreateLayerContext(HDC hdc, int iLayerPlane) {
	const char *_name = "wglCreateLayerContext";
	return NULL;
}

static HGLRC __stdcall _get_wglCreateLayerContext(HDC hdc, int iLayerPlane) {
	PFN_WGLCREATELAYERCONTEXT _ptr;
	_ptr = (PFN_WGLCREATELAYERCONTEXT)_getPublicProcAddress("wglCreateLayerContext");
	if (!_ptr) {
		_ptr = &_fail_wglCreateLayerContext;
	}
	_wglCreateLayerContext = _ptr;
	return _wglCreateLayerContext(hdc, iLayerPlane);
}

PFN_WGLCREATELAYERCONTEXT _wglCreateLayerContext = &_get_wglCreateLayerContext;

static BOOL __stdcall _fail_wglDescribeLayerPlane(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LAYERPLANEDESCRIPTOR * plpd) {
	const char *_name = "wglDescribeLayerPlane";
	return false;
}

static BOOL __stdcall _get_wglDescribeLayerPlane(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LAYERPLANEDESCRIPTOR * plpd) {
	PFN_WGLDESCRIBELAYERPLANE _ptr;
	_ptr = (PFN_WGLDESCRIBELAYERPLANE)_getPublicProcAddress("wglDescribeLayerPlane");
	if (!_ptr) {
		_ptr = &_fail_wglDescribeLayerPlane;
	}
	_wglDescribeLayerPlane = _ptr;
	return _wglDescribeLayerPlane(hdc, iPixelFormat, iLayerPlane, nBytes, plpd);
}

PFN_WGLDESCRIBELAYERPLANE _wglDescribeLayerPlane = &_get_wglDescribeLayerPlane;

static int __stdcall _fail_wglSetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, const COLORREF * pcr) {
	const char *_name = "wglSetLayerPaletteEntries";
	return NULL;
}

static int __stdcall _get_wglSetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, const COLORREF * pcr) {
	PFN_WGLSETLAYERPALETTEENTRIES _ptr;
	_ptr = (PFN_WGLSETLAYERPALETTEENTRIES)_getPublicProcAddress("wglSetLayerPaletteEntries");
	if (!_ptr) {
		_ptr = &_fail_wglSetLayerPaletteEntries;
	}
	_wglSetLayerPaletteEntries = _ptr;
	return _wglSetLayerPaletteEntries(hdc, iLayerPlane, iStart, cEntries, pcr);
}

PFN_WGLSETLAYERPALETTEENTRIES _wglSetLayerPaletteEntries = &_get_wglSetLayerPaletteEntries;

static int __stdcall _fail_wglGetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF * pcr) {
	const char *_name = "wglGetLayerPaletteEntries";
	return 0;
}

static int __stdcall _get_wglGetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF * pcr) {
	PFN_WGLGETLAYERPALETTEENTRIES _ptr;
	_ptr = (PFN_WGLGETLAYERPALETTEENTRIES)_getPublicProcAddress("wglGetLayerPaletteEntries");
	if (!_ptr) {
		_ptr = &_fail_wglGetLayerPaletteEntries;
	}
	_wglGetLayerPaletteEntries = _ptr;
	return _wglGetLayerPaletteEntries(hdc, iLayerPlane, iStart, cEntries, pcr);
}

PFN_WGLGETLAYERPALETTEENTRIES _wglGetLayerPaletteEntries = &_get_wglGetLayerPaletteEntries;

static BOOL __stdcall _fail_wglRealizeLayerPalette(HDC hdc, int iLayerPlane, BOOL bRealize) {
	const char *_name = "wglRealizeLayerPalette";

	return false;
}

static BOOL __stdcall _get_wglRealizeLayerPalette(HDC hdc, int iLayerPlane, BOOL bRealize) {
	PFN_WGLREALIZELAYERPALETTE _ptr;
	_ptr = (PFN_WGLREALIZELAYERPALETTE)_getPublicProcAddress("wglRealizeLayerPalette");
	if (!_ptr) {
		_ptr = &_fail_wglRealizeLayerPalette;
	}
	_wglRealizeLayerPalette = _ptr;
	return _wglRealizeLayerPalette(hdc, iLayerPlane, bRealize);
}

PFN_WGLREALIZELAYERPALETTE _wglRealizeLayerPalette = &_get_wglRealizeLayerPalette;

static BOOL __stdcall _fail_wglSwapLayerBuffers(HDC hdc, UINT fuPlanes) {
	const char *_name = "wglSwapLayerBuffers";
	return false;
}

static BOOL __stdcall _get_wglSwapLayerBuffers(HDC hdc, UINT fuPlanes) {
	PFN_WGLSWAPLAYERBUFFERS _ptr;
	_ptr = (PFN_WGLSWAPLAYERBUFFERS)_getPublicProcAddress("wglSwapLayerBuffers");
	if (!_ptr) {
		_ptr = &_fail_wglSwapLayerBuffers;
	}
	_wglSwapLayerBuffers = _ptr;
	return _wglSwapLayerBuffers(hdc, fuPlanes);
}

PFN_WGLSWAPLAYERBUFFERS _wglSwapLayerBuffers = &_get_wglSwapLayerBuffers;

static BOOL __stdcall _fail_wglUseFontBitmapsA(HDC hdc, DWORD first, DWORD count, DWORD listBase) {
	const char *_name = "wglUseFontBitmapsA";
	return false;
}

static BOOL __stdcall _get_wglUseFontBitmapsA(HDC hdc, DWORD first, DWORD count, DWORD listBase) {
	PFN_WGLUSEFONTBITMAPSA _ptr;
	_ptr = (PFN_WGLUSEFONTBITMAPSA)_getPublicProcAddress("wglUseFontBitmapsA");
	if (!_ptr) {
		_ptr = &_fail_wglUseFontBitmapsA;
	}
	_wglUseFontBitmapsA = _ptr;
	return _wglUseFontBitmapsA(hdc, first, count, listBase);
}

PFN_WGLUSEFONTBITMAPSA _wglUseFontBitmapsA = &_get_wglUseFontBitmapsA;

static BOOL __stdcall _fail_wglUseFontBitmapsW(HDC hdc, DWORD first, DWORD count, DWORD listBase) {
	const char *_name = "wglUseFontBitmapsW";
	return false;
}

static BOOL __stdcall _get_wglUseFontBitmapsW(HDC hdc, DWORD first, DWORD count, DWORD listBase) {
	PFN_WGLUSEFONTBITMAPSW _ptr;
	_ptr = (PFN_WGLUSEFONTBITMAPSW)_getPublicProcAddress("wglUseFontBitmapsW");
	if (!_ptr) {
		_ptr = &_fail_wglUseFontBitmapsW;
	}
	_wglUseFontBitmapsW = _ptr;
	return _wglUseFontBitmapsW(hdc, first, count, listBase);
}

PFN_WGLUSEFONTBITMAPSW _wglUseFontBitmapsW = &_get_wglUseFontBitmapsW;

static DWORD __stdcall _fail_wglSwapMultipleBuffers(UINT n, const WGLSWAP * ps) {
	const char *_name = "wglSwapMultipleBuffers";
	return false;
}

static DWORD __stdcall _get_wglSwapMultipleBuffers(UINT n, const WGLSWAP * ps) {
	PFN_WGLSWAPMULTIPLEBUFFERS _ptr;
	_ptr = (PFN_WGLSWAPMULTIPLEBUFFERS)_getPublicProcAddress("wglSwapMultipleBuffers");
	if (!_ptr) {
		_ptr = &_fail_wglSwapMultipleBuffers;
	}
	_wglSwapMultipleBuffers = _ptr;
	return _wglSwapMultipleBuffers(n, ps);
}

PFN_WGLSWAPMULTIPLEBUFFERS _wglSwapMultipleBuffers = &_get_wglSwapMultipleBuffers;

static BOOL __stdcall _fail_wglUseFontOutlinesA(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, GLYPHMETRICSFLOAT * lpgmf) {
	const char *_name = "wglUseFontOutlinesA";
	return false;
}

static BOOL __stdcall _get_wglUseFontOutlinesA(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, GLYPHMETRICSFLOAT * lpgmf) {
	PFN_WGLUSEFONTOUTLINESA _ptr;
	_ptr = (PFN_WGLUSEFONTOUTLINESA)_getPublicProcAddress("wglUseFontOutlinesA");
	if (!_ptr) {
		_ptr = &_fail_wglUseFontOutlinesA;
	}
	_wglUseFontOutlinesA = _ptr;
	return _wglUseFontOutlinesA(hdc, first, count, listBase, deviation, extrusion, format, lpgmf);
}

PFN_WGLUSEFONTOUTLINESA _wglUseFontOutlinesA = &_get_wglUseFontOutlinesA;

static BOOL __stdcall _fail_wglUseFontOutlinesW(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, GLYPHMETRICSFLOAT * lpgmf) {
	const char *_name = "wglUseFontOutlinesW";
	return false;
}

static BOOL __stdcall _get_wglUseFontOutlinesW(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, GLYPHMETRICSFLOAT * lpgmf) {
	PFN_WGLUSEFONTOUTLINESW _ptr;
	_ptr = (PFN_WGLUSEFONTOUTLINESW)_getPublicProcAddress("wglUseFontOutlinesW");
	if (!_ptr) {
		_ptr = &_fail_wglUseFontOutlinesW;
	}
	_wglUseFontOutlinesW = _ptr;
	return _wglUseFontOutlinesW(hdc, first, count, listBase, deviation, extrusion, format, lpgmf);
}

PFN_WGLUSEFONTOUTLINESW _wglUseFontOutlinesW = &_get_wglUseFontOutlinesW;

static HANDLE __stdcall _fail_wglCreateBufferRegionARB(HDC hDC, int iLayerPlane, UINT uType) {
	const char *_name = "wglCreateBufferRegionARB";
	return false;
}

static HANDLE __stdcall _get_wglCreateBufferRegionARB(HDC hDC, int iLayerPlane, UINT uType) {
	PFN_WGLCREATEBUFFERREGIONARB _ptr;
	_ptr = (PFN_WGLCREATEBUFFERREGIONARB)_getPrivateProcAddress("wglCreateBufferRegionARB");
	if (!_ptr) {
		_ptr = &_fail_wglCreateBufferRegionARB;
	}
	_wglCreateBufferRegionARB = _ptr;
	return _wglCreateBufferRegionARB(hDC, iLayerPlane, uType);
}

PFN_WGLCREATEBUFFERREGIONARB _wglCreateBufferRegionARB = &_get_wglCreateBufferRegionARB;

static void __stdcall _fail_wglDeleteBufferRegionARB(HANDLE hRegion) {
	const char *_name = "wglDeleteBufferRegionARB";
	
	return;
}

static void __stdcall _get_wglDeleteBufferRegionARB(HANDLE hRegion) {
	PFN_WGLDELETEBUFFERREGIONARB _ptr;
	_ptr = (PFN_WGLDELETEBUFFERREGIONARB)_getPrivateProcAddress("wglDeleteBufferRegionARB");
	if (!_ptr) {
		_ptr = &_fail_wglDeleteBufferRegionARB;
	}
	_wglDeleteBufferRegionARB = _ptr;
	_wglDeleteBufferRegionARB(hRegion);
}

PFN_WGLDELETEBUFFERREGIONARB _wglDeleteBufferRegionARB = &_get_wglDeleteBufferRegionARB;

static BOOL __stdcall _fail_wglSaveBufferRegionARB(HANDLE hRegion, int x, int y, int width, int height) {
	const char *_name = "wglSaveBufferRegionARB";
	return false;
}

static BOOL __stdcall _get_wglSaveBufferRegionARB(HANDLE hRegion, int x, int y, int width, int height) {
	PFN_WGLSAVEBUFFERREGIONARB _ptr;
	_ptr = (PFN_WGLSAVEBUFFERREGIONARB)_getPrivateProcAddress("wglSaveBufferRegionARB");
	if (!_ptr) {
		_ptr = &_fail_wglSaveBufferRegionARB;
	}
	_wglSaveBufferRegionARB = _ptr;
	return _wglSaveBufferRegionARB(hRegion, x, y, width, height);
}

PFN_WGLSAVEBUFFERREGIONARB _wglSaveBufferRegionARB = &_get_wglSaveBufferRegionARB;

static BOOL __stdcall _fail_wglRestoreBufferRegionARB(HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc) {
	const char *_name = "wglRestoreBufferRegionARB";
	return false;
}

static BOOL __stdcall _get_wglRestoreBufferRegionARB(HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc) {
	PFN_WGLRESTOREBUFFERREGIONARB _ptr;
	_ptr = (PFN_WGLRESTOREBUFFERREGIONARB)_getPrivateProcAddress("wglRestoreBufferRegionARB");
	if (!_ptr) {
		_ptr = &_fail_wglRestoreBufferRegionARB;
	}
	_wglRestoreBufferRegionARB = _ptr;
	return _wglRestoreBufferRegionARB(hRegion, x, y, width, height, xSrc, ySrc);
}

PFN_WGLRESTOREBUFFERREGIONARB _wglRestoreBufferRegionARB = &_get_wglRestoreBufferRegionARB;

static const char * __stdcall _fail_wglGetExtensionsStringARB(HDC hdc) {
	const char *_name = "wglGetExtensionsStringARB";
	return "";
}

static const char * __stdcall _get_wglGetExtensionsStringARB(HDC hdc) {
	PFN_WGLGETEXTENSIONSSTRINGARB _ptr;
	_ptr = (PFN_WGLGETEXTENSIONSSTRINGARB)_getPrivateProcAddress("wglGetExtensionsStringARB");
	if (!_ptr) {
		_ptr = &_fail_wglGetExtensionsStringARB;
	}
	_wglGetExtensionsStringARB = _ptr;
	return _wglGetExtensionsStringARB(hdc);
}

PFN_WGLGETEXTENSIONSSTRINGARB _wglGetExtensionsStringARB = &_get_wglGetExtensionsStringARB;

static BOOL __stdcall _fail_wglGetPixelFormatAttribivARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, int * piValues) {
	const char *_name = "wglGetPixelFormatAttribivARB";
	return false;
}

static BOOL __stdcall _get_wglGetPixelFormatAttribivARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, int * piValues) {
	PFN_WGLGETPIXELFORMATATTRIBIVARB _ptr;
	_ptr = (PFN_WGLGETPIXELFORMATATTRIBIVARB)_getPrivateProcAddress("wglGetPixelFormatAttribivARB");
	if (!_ptr) {
		_ptr = &_fail_wglGetPixelFormatAttribivARB;
	}
	_wglGetPixelFormatAttribivARB = _ptr;
	return _wglGetPixelFormatAttribivARB(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, piValues);
}

PFN_WGLGETPIXELFORMATATTRIBIVARB _wglGetPixelFormatAttribivARB = &_get_wglGetPixelFormatAttribivARB;

static BOOL __stdcall _fail_wglGetPixelFormatAttribfvARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, FLOAT * pfValues) {
	const char *_name = "wglGetPixelFormatAttribfvARB";
	return false;
}

static BOOL __stdcall _get_wglGetPixelFormatAttribfvARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, FLOAT * pfValues) {
	PFN_WGLGETPIXELFORMATATTRIBFVARB _ptr;
	_ptr = (PFN_WGLGETPIXELFORMATATTRIBFVARB)_getPrivateProcAddress("wglGetPixelFormatAttribfvARB");
	if (!_ptr) {
		_ptr = &_fail_wglGetPixelFormatAttribfvARB;
	}
	_wglGetPixelFormatAttribfvARB = _ptr;
	return _wglGetPixelFormatAttribfvARB(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, pfValues);
}

PFN_WGLGETPIXELFORMATATTRIBFVARB _wglGetPixelFormatAttribfvARB = &_get_wglGetPixelFormatAttribfvARB;

static BOOL __stdcall _fail_wglMakeContextCurrentARB(HDC hDrawDC, HDC hReadDC, HGLRC hglrc) {
	const char *_name = "wglMakeContextCurrentARB";
	return false;
}

static BOOL __stdcall _get_wglMakeContextCurrentARB(HDC hDrawDC, HDC hReadDC, HGLRC hglrc) {
	PFN_WGLMAKECONTEXTCURRENTARB _ptr;
	_ptr = (PFN_WGLMAKECONTEXTCURRENTARB)_getPrivateProcAddress("wglMakeContextCurrentARB");
	if (!_ptr) {
		_ptr = &_fail_wglMakeContextCurrentARB;
	}
	_wglMakeContextCurrentARB = _ptr;
	return _wglMakeContextCurrentARB(hDrawDC, hReadDC, hglrc);
}

PFN_WGLMAKECONTEXTCURRENTARB _wglMakeContextCurrentARB = &_get_wglMakeContextCurrentARB;

static HDC __stdcall _fail_wglGetCurrentReadDCARB(void) {
	const char *_name = "wglGetCurrentReadDCARB";
	return 0;
}

static HDC __stdcall _get_wglGetCurrentReadDCARB(void) {
	PFN_WGLGETCURRENTREADDCARB _ptr;
	_ptr = (PFN_WGLGETCURRENTREADDCARB)_getPrivateProcAddress("wglGetCurrentReadDCARB");
	if (!_ptr) {
		_ptr = &_fail_wglGetCurrentReadDCARB;
	}
	_wglGetCurrentReadDCARB = _ptr;
	return _wglGetCurrentReadDCARB();
}

PFN_WGLGETCURRENTREADDCARB _wglGetCurrentReadDCARB = &_get_wglGetCurrentReadDCARB;

static HPBUFFERARB __stdcall _fail_wglCreatePbufferARB(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int * piAttribList) {
	const char *_name = "wglCreatePbufferARB";
	return NULL;
}

static HPBUFFERARB __stdcall _get_wglCreatePbufferARB(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int * piAttribList) {
	PFN_WGLCREATEPBUFFERARB _ptr;
	_ptr = (PFN_WGLCREATEPBUFFERARB)_getPrivateProcAddress("wglCreatePbufferARB");
	if (!_ptr) {
		_ptr = &_fail_wglCreatePbufferARB;
	}
	_wglCreatePbufferARB = _ptr;
	return _wglCreatePbufferARB(hDC, iPixelFormat, iWidth, iHeight, piAttribList);
}

PFN_WGLCREATEPBUFFERARB _wglCreatePbufferARB = &_get_wglCreatePbufferARB;

static HDC __stdcall _fail_wglGetPbufferDCARB(HPBUFFERARB hPbuffer) {
	const char *_name = "wglGetPbufferDCARB";
	return NULL;
}

static HDC __stdcall _get_wglGetPbufferDCARB(HPBUFFERARB hPbuffer) {
	PFN_WGLGETPBUFFERDCARB _ptr;
	_ptr = (PFN_WGLGETPBUFFERDCARB)_getPrivateProcAddress("wglGetPbufferDCARB");
	if (!_ptr) {
		_ptr = &_fail_wglGetPbufferDCARB;
	}
	_wglGetPbufferDCARB = _ptr;
	return _wglGetPbufferDCARB(hPbuffer);
}

PFN_WGLGETPBUFFERDCARB _wglGetPbufferDCARB = &_get_wglGetPbufferDCARB;

static int __stdcall _fail_wglReleasePbufferDCARB(HPBUFFERARB hPbuffer, HDC hDC) {
	const char *_name = "wglReleasePbufferDCARB";
	return NULL;
}

static int __stdcall _get_wglReleasePbufferDCARB(HPBUFFERARB hPbuffer, HDC hDC) {
	PFN_WGLRELEASEPBUFFERDCARB _ptr;
	_ptr = (PFN_WGLRELEASEPBUFFERDCARB)_getPrivateProcAddress("wglReleasePbufferDCARB");
	if (!_ptr) {
		_ptr = &_fail_wglReleasePbufferDCARB;
	}
	_wglReleasePbufferDCARB = _ptr;
	return _wglReleasePbufferDCARB(hPbuffer, hDC);
}

PFN_WGLRELEASEPBUFFERDCARB _wglReleasePbufferDCARB = &_get_wglReleasePbufferDCARB;

static BOOL __stdcall _fail_wglDestroyPbufferARB(HPBUFFERARB hPbuffer) {
	const char *_name = "wglDestroyPbufferARB";
	return false;
}

static BOOL __stdcall _get_wglDestroyPbufferARB(HPBUFFERARB hPbuffer) {
	PFN_WGLDESTROYPBUFFERARB _ptr;
	_ptr = (PFN_WGLDESTROYPBUFFERARB)_getPrivateProcAddress("wglDestroyPbufferARB");
	if (!_ptr) {
		_ptr = &_fail_wglDestroyPbufferARB;
	}
	_wglDestroyPbufferARB = _ptr;
	return _wglDestroyPbufferARB(hPbuffer);
}

PFN_WGLDESTROYPBUFFERARB _wglDestroyPbufferARB = &_get_wglDestroyPbufferARB;

static BOOL __stdcall _fail_wglQueryPbufferARB(HPBUFFERARB hPbuffer, int iAttribute, int * piValue) {
	const char *_name = "wglQueryPbufferARB";
	return false;
}

static BOOL __stdcall _get_wglQueryPbufferARB(HPBUFFERARB hPbuffer, int iAttribute, int * piValue) {
	PFN_WGLQUERYPBUFFERARB _ptr;
	_ptr = (PFN_WGLQUERYPBUFFERARB)_getPrivateProcAddress("wglQueryPbufferARB");
	if (!_ptr) {
		_ptr = &_fail_wglQueryPbufferARB;
	}
	_wglQueryPbufferARB = _ptr;
	return _wglQueryPbufferARB(hPbuffer, iAttribute, piValue);
}

PFN_WGLQUERYPBUFFERARB _wglQueryPbufferARB = &_get_wglQueryPbufferARB;

static BOOL __stdcall _fail_wglBindTexImageARB(HPBUFFERARB hPbuffer, int iBuffer) {
	const char *_name = "wglBindTexImageARB";
	return false;
}

static BOOL __stdcall _get_wglBindTexImageARB(HPBUFFERARB hPbuffer, int iBuffer) {
	PFN_WGLBINDTEXIMAGEARB _ptr;
	_ptr = (PFN_WGLBINDTEXIMAGEARB)_getPrivateProcAddress("wglBindTexImageARB");
	if (!_ptr) {
		_ptr = &_fail_wglBindTexImageARB;
	}
	_wglBindTexImageARB = _ptr;
	return _wglBindTexImageARB(hPbuffer, iBuffer);
}

PFN_WGLBINDTEXIMAGEARB _wglBindTexImageARB = &_get_wglBindTexImageARB;

static BOOL __stdcall _fail_wglReleaseTexImageARB(HPBUFFERARB hPbuffer, int iBuffer) {
	const char *_name = "wglReleaseTexImageARB";
	return false;
}

static BOOL __stdcall _get_wglReleaseTexImageARB(HPBUFFERARB hPbuffer, int iBuffer) {
	PFN_WGLRELEASETEXIMAGEARB _ptr;
	_ptr = (PFN_WGLRELEASETEXIMAGEARB)_getPrivateProcAddress("wglReleaseTexImageARB");
	if (!_ptr) {
		_ptr = &_fail_wglReleaseTexImageARB;
	}
	_wglReleaseTexImageARB = _ptr;
	return _wglReleaseTexImageARB(hPbuffer, iBuffer);
}

PFN_WGLRELEASETEXIMAGEARB _wglReleaseTexImageARB = &_get_wglReleaseTexImageARB;

static BOOL __stdcall _fail_wglSetPbufferAttribARB(HPBUFFERARB hPbuffer, const int * piAttribList) {
	const char *_name = "wglSetPbufferAttribARB";
	return false;
}

static BOOL __stdcall _get_wglSetPbufferAttribARB(HPBUFFERARB hPbuffer, const int * piAttribList) {
	PFN_WGLSETPBUFFERATTRIBARB _ptr;
	_ptr = (PFN_WGLSETPBUFFERATTRIBARB)_getPrivateProcAddress("wglSetPbufferAttribARB");
	if (!_ptr) {
		_ptr = &_fail_wglSetPbufferAttribARB;
	}
	_wglSetPbufferAttribARB = _ptr;
	return _wglSetPbufferAttribARB(hPbuffer, piAttribList);
}

PFN_WGLSETPBUFFERATTRIBARB _wglSetPbufferAttribARB = &_get_wglSetPbufferAttribARB;

static HGLRC __stdcall _fail_wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int * attribList) {
	const char *_name = "wglCreateContextAttribsARB";
	return NULL;
}

static HGLRC __stdcall _get_wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int * attribList) {
	PFN_WGLCREATECONTEXTATTRIBSARB _ptr;
	_ptr = (PFN_WGLCREATECONTEXTATTRIBSARB)_getPrivateProcAddress("wglCreateContextAttribsARB");
	if (!_ptr) {
		_ptr = &_fail_wglCreateContextAttribsARB;
	}
	_wglCreateContextAttribsARB = _ptr;
	return _wglCreateContextAttribsARB(hDC, hShareContext, attribList);
}

PFN_WGLCREATECONTEXTATTRIBSARB _wglCreateContextAttribsARB = &_get_wglCreateContextAttribsARB;

static GLboolean __stdcall _fail_wglCreateDisplayColorTableEXT(GLushort id) {
	const char *_name = "wglCreateDisplayColorTableEXT";
	return false;
}

static GLboolean __stdcall _get_wglCreateDisplayColorTableEXT(GLushort id) {
	PFN_WGLCREATEDISPLAYCOLORTABLEEXT _ptr;
	_ptr = (PFN_WGLCREATEDISPLAYCOLORTABLEEXT)_getPrivateProcAddress("wglCreateDisplayColorTableEXT");
	if (!_ptr) {
		_ptr = &_fail_wglCreateDisplayColorTableEXT;
	}
	_wglCreateDisplayColorTableEXT = _ptr;
	return _wglCreateDisplayColorTableEXT(id);
}

PFN_WGLCREATEDISPLAYCOLORTABLEEXT _wglCreateDisplayColorTableEXT = &_get_wglCreateDisplayColorTableEXT;

static GLboolean __stdcall _fail_wglLoadDisplayColorTableEXT(const GLushort * table, GLuint length) {
	const char *_name = "wglLoadDisplayColorTableEXT";
	return false;
}

static GLboolean __stdcall _get_wglLoadDisplayColorTableEXT(const GLushort * table, GLuint length) {
	PFN_WGLLOADDISPLAYCOLORTABLEEXT _ptr;
	_ptr = (PFN_WGLLOADDISPLAYCOLORTABLEEXT)_getPrivateProcAddress("wglLoadDisplayColorTableEXT");
	if (!_ptr) {
		_ptr = &_fail_wglLoadDisplayColorTableEXT;
	}
	_wglLoadDisplayColorTableEXT = _ptr;
	return _wglLoadDisplayColorTableEXT(table, length);
}

PFN_WGLLOADDISPLAYCOLORTABLEEXT _wglLoadDisplayColorTableEXT = &_get_wglLoadDisplayColorTableEXT;

static GLboolean __stdcall _fail_wglBindDisplayColorTableEXT(GLushort id) {
	const char *_name = "wglBindDisplayColorTableEXT";
	return false;
}

static GLboolean __stdcall _get_wglBindDisplayColorTableEXT(GLushort id) {
	PFN_WGLBINDDISPLAYCOLORTABLEEXT _ptr;
	_ptr = (PFN_WGLBINDDISPLAYCOLORTABLEEXT)_getPrivateProcAddress("wglBindDisplayColorTableEXT");
	if (!_ptr) {
		_ptr = &_fail_wglBindDisplayColorTableEXT;
	}
	_wglBindDisplayColorTableEXT = _ptr;
	return _wglBindDisplayColorTableEXT(id);
}

PFN_WGLBINDDISPLAYCOLORTABLEEXT _wglBindDisplayColorTableEXT = &_get_wglBindDisplayColorTableEXT;

static void __stdcall _fail_wglDestroyDisplayColorTableEXT(GLushort id) {
	const char *_name = "wglDestroyDisplayColorTableEXT";
	
	return;
}

static void __stdcall _get_wglDestroyDisplayColorTableEXT(GLushort id) {
	PFN_WGLDESTROYDISPLAYCOLORTABLEEXT _ptr;
	_ptr = (PFN_WGLDESTROYDISPLAYCOLORTABLEEXT)_getPrivateProcAddress("wglDestroyDisplayColorTableEXT");
	if (!_ptr) {
		_ptr = &_fail_wglDestroyDisplayColorTableEXT;
	}
	_wglDestroyDisplayColorTableEXT = _ptr;
	_wglDestroyDisplayColorTableEXT(id);
}

PFN_WGLDESTROYDISPLAYCOLORTABLEEXT _wglDestroyDisplayColorTableEXT = &_get_wglDestroyDisplayColorTableEXT;

static const char * __stdcall _fail_wglGetExtensionsStringEXT(void) {
	const char *_name = "wglGetExtensionsStringEXT";
	return NULL;
}

static const char * __stdcall _get_wglGetExtensionsStringEXT(void) {
	PFN_WGLGETEXTENSIONSSTRINGEXT _ptr;
	_ptr = (PFN_WGLGETEXTENSIONSSTRINGEXT)_getPrivateProcAddress("wglGetExtensionsStringEXT");
	if (!_ptr) {
		_ptr = &_fail_wglGetExtensionsStringEXT;
	}
	_wglGetExtensionsStringEXT = _ptr;
	return _wglGetExtensionsStringEXT();
}

PFN_WGLGETEXTENSIONSSTRINGEXT _wglGetExtensionsStringEXT = &_get_wglGetExtensionsStringEXT;

static BOOL __stdcall _fail_wglMakeContextCurrentEXT(HDC hDrawDC, HDC hReadDC, HGLRC hglrc) {
	const char *_name = "wglMakeContextCurrentEXT";
	return false;
}

static BOOL __stdcall _get_wglMakeContextCurrentEXT(HDC hDrawDC, HDC hReadDC, HGLRC hglrc) {
	PFN_WGLMAKECONTEXTCURRENTEXT _ptr;
	_ptr = (PFN_WGLMAKECONTEXTCURRENTEXT)_getPrivateProcAddress("wglMakeContextCurrentEXT");
	if (!_ptr) {
		_ptr = &_fail_wglMakeContextCurrentEXT;
	}
	_wglMakeContextCurrentEXT = _ptr;
	return _wglMakeContextCurrentEXT(hDrawDC, hReadDC, hglrc);
}

PFN_WGLMAKECONTEXTCURRENTEXT _wglMakeContextCurrentEXT = &_get_wglMakeContextCurrentEXT;

static HDC __stdcall _fail_wglGetCurrentReadDCEXT(void) {
	const char *_name = "wglGetCurrentReadDCEXT";
	return NULL;
}

static HDC __stdcall _get_wglGetCurrentReadDCEXT(void) {
	PFN_WGLGETCURRENTREADDCEXT _ptr;
	_ptr = (PFN_WGLGETCURRENTREADDCEXT)_getPrivateProcAddress("wglGetCurrentReadDCEXT");
	if (!_ptr) {
		_ptr = &_fail_wglGetCurrentReadDCEXT;
	}
	_wglGetCurrentReadDCEXT = _ptr;
	return _wglGetCurrentReadDCEXT();
}

PFN_WGLGETCURRENTREADDCEXT _wglGetCurrentReadDCEXT = &_get_wglGetCurrentReadDCEXT;

static HPBUFFEREXT __stdcall _fail_wglCreatePbufferEXT(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int * piAttribList) {
	const char *_name = "wglCreatePbufferEXT";
	return NULL;
}

static HPBUFFEREXT __stdcall _get_wglCreatePbufferEXT(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int * piAttribList) {
	PFN_WGLCREATEPBUFFEREXT _ptr;
	_ptr = (PFN_WGLCREATEPBUFFEREXT)_getPrivateProcAddress("wglCreatePbufferEXT");
	if (!_ptr) {
		_ptr = &_fail_wglCreatePbufferEXT;
	}
	_wglCreatePbufferEXT = _ptr;
	return _wglCreatePbufferEXT(hDC, iPixelFormat, iWidth, iHeight, piAttribList);
}

PFN_WGLCREATEPBUFFEREXT _wglCreatePbufferEXT = &_get_wglCreatePbufferEXT;

static HDC __stdcall _fail_wglGetPbufferDCEXT(HPBUFFEREXT hPbuffer) {
	const char *_name = "wglGetPbufferDCEXT";
	return NULL;
}

static HDC __stdcall _get_wglGetPbufferDCEXT(HPBUFFEREXT hPbuffer) {
	PFN_WGLGETPBUFFERDCEXT _ptr;
	_ptr = (PFN_WGLGETPBUFFERDCEXT)_getPrivateProcAddress("wglGetPbufferDCEXT");
	if (!_ptr) {
		_ptr = &_fail_wglGetPbufferDCEXT;
	}
	_wglGetPbufferDCEXT = _ptr;
	return _wglGetPbufferDCEXT(hPbuffer);
}

PFN_WGLGETPBUFFERDCEXT _wglGetPbufferDCEXT = &_get_wglGetPbufferDCEXT;

static int __stdcall _fail_wglReleasePbufferDCEXT(HPBUFFEREXT hPbuffer, HDC hDC) {
	const char *_name = "wglReleasePbufferDCEXT";
	return false;
}

static int __stdcall _get_wglReleasePbufferDCEXT(HPBUFFEREXT hPbuffer, HDC hDC) {
	PFN_WGLRELEASEPBUFFERDCEXT _ptr;
	_ptr = (PFN_WGLRELEASEPBUFFERDCEXT)_getPrivateProcAddress("wglReleasePbufferDCEXT");
	if (!_ptr) {
		_ptr = &_fail_wglReleasePbufferDCEXT;
	}
	_wglReleasePbufferDCEXT = _ptr;
	return _wglReleasePbufferDCEXT(hPbuffer, hDC);
}

PFN_WGLRELEASEPBUFFERDCEXT _wglReleasePbufferDCEXT = &_get_wglReleasePbufferDCEXT;

static BOOL __stdcall _fail_wglDestroyPbufferEXT(HPBUFFEREXT hPbuffer) {
	const char *_name = "wglDestroyPbufferEXT";
	return false;
}

static BOOL __stdcall _get_wglDestroyPbufferEXT(HPBUFFEREXT hPbuffer) {
	PFN_WGLDESTROYPBUFFEREXT _ptr;
	_ptr = (PFN_WGLDESTROYPBUFFEREXT)_getPrivateProcAddress("wglDestroyPbufferEXT");
	if (!_ptr) {
		_ptr = &_fail_wglDestroyPbufferEXT;
	}
	_wglDestroyPbufferEXT = _ptr;
	return _wglDestroyPbufferEXT(hPbuffer);
}

PFN_WGLDESTROYPBUFFEREXT _wglDestroyPbufferEXT = &_get_wglDestroyPbufferEXT;

static BOOL __stdcall _fail_wglQueryPbufferEXT(HPBUFFEREXT hPbuffer, int iAttribute, int * piValue) {
	const char *_name = "wglQueryPbufferEXT";
	return false;
}

static BOOL __stdcall _get_wglQueryPbufferEXT(HPBUFFEREXT hPbuffer, int iAttribute, int * piValue) {
	PFN_WGLQUERYPBUFFEREXT _ptr;
	_ptr = (PFN_WGLQUERYPBUFFEREXT)_getPrivateProcAddress("wglQueryPbufferEXT");
	if (!_ptr) {
		_ptr = &_fail_wglQueryPbufferEXT;
	}
	_wglQueryPbufferEXT = _ptr;
	return _wglQueryPbufferEXT(hPbuffer, iAttribute, piValue);
}

PFN_WGLQUERYPBUFFEREXT _wglQueryPbufferEXT = &_get_wglQueryPbufferEXT;

static BOOL __stdcall _fail_wglGetPixelFormatAttribivEXT(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, int * piValues) {
	const char *_name = "wglGetPixelFormatAttribivEXT";
	return false;
}

static BOOL __stdcall _get_wglGetPixelFormatAttribivEXT(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, int * piValues) {
	PFN_WGLGETPIXELFORMATATTRIBIVEXT _ptr;
	_ptr = (PFN_WGLGETPIXELFORMATATTRIBIVEXT)_getPrivateProcAddress("wglGetPixelFormatAttribivEXT");
	if (!_ptr) {
		_ptr = &_fail_wglGetPixelFormatAttribivEXT;
	}
	_wglGetPixelFormatAttribivEXT = _ptr;
	return _wglGetPixelFormatAttribivEXT(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, piValues);
}

PFN_WGLGETPIXELFORMATATTRIBIVEXT _wglGetPixelFormatAttribivEXT = &_get_wglGetPixelFormatAttribivEXT;

static BOOL __stdcall _fail_wglGetPixelFormatAttribfvEXT(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, FLOAT * pfValues) {
	const char *_name = "wglGetPixelFormatAttribfvEXT";
	return false;
}

static BOOL __stdcall _get_wglGetPixelFormatAttribfvEXT(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, FLOAT * pfValues) {
	PFN_WGLGETPIXELFORMATATTRIBFVEXT _ptr;
	_ptr = (PFN_WGLGETPIXELFORMATATTRIBFVEXT)_getPrivateProcAddress("wglGetPixelFormatAttribfvEXT");
	if (!_ptr) {
		_ptr = &_fail_wglGetPixelFormatAttribfvEXT;
	}
	_wglGetPixelFormatAttribfvEXT = _ptr;
	return _wglGetPixelFormatAttribfvEXT(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, pfValues);
}

PFN_WGLGETPIXELFORMATATTRIBFVEXT _wglGetPixelFormatAttribfvEXT = &_get_wglGetPixelFormatAttribfvEXT;

static BOOL __stdcall _fail_wglChoosePixelFormatEXT(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats) {
	const char *_name = "wglChoosePixelFormatEXT";
	return false;
}

static BOOL __stdcall _get_wglChoosePixelFormatEXT(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats) {
	PFN_WGLCHOOSEPIXELFORMATEXT _ptr;
	_ptr = (PFN_WGLCHOOSEPIXELFORMATEXT)_getPrivateProcAddress("wglChoosePixelFormatEXT");
	if (!_ptr) {
		_ptr = &_fail_wglChoosePixelFormatEXT;
	}
	_wglChoosePixelFormatEXT = _ptr;
	return _wglChoosePixelFormatEXT(hdc, piAttribIList, pfAttribFList, nMaxFormats, piFormats, nNumFormats);
}

PFN_WGLCHOOSEPIXELFORMATEXT _wglChoosePixelFormatEXT = &_get_wglChoosePixelFormatEXT;

static BOOL __stdcall _fail_wglSwapIntervalEXT(int interval) {
	const char *_name = "wglSwapIntervalEXT";
	return false;
}

static BOOL __stdcall _get_wglSwapIntervalEXT(int interval) {
	PFN_WGLSWAPINTERVALEXT _ptr;
	_ptr = (PFN_WGLSWAPINTERVALEXT)_getPrivateProcAddress("wglSwapIntervalEXT");
	if (!_ptr) {
		_ptr = &_fail_wglSwapIntervalEXT;
	}
	_wglSwapIntervalEXT = _ptr;
	return _wglSwapIntervalEXT(interval);
}

PFN_WGLSWAPINTERVALEXT _wglSwapIntervalEXT = &_get_wglSwapIntervalEXT;

static int __stdcall _fail_wglGetSwapIntervalEXT(void) {
	const char *_name = "wglGetSwapIntervalEXT";
	return false;
}

static int __stdcall _get_wglGetSwapIntervalEXT(void) {
	PFN_WGLGETSWAPINTERVALEXT _ptr;
	_ptr = (PFN_WGLGETSWAPINTERVALEXT)_getPrivateProcAddress("wglGetSwapIntervalEXT");
	if (!_ptr) {
		_ptr = &_fail_wglGetSwapIntervalEXT;
	}
	_wglGetSwapIntervalEXT = _ptr;
	return _wglGetSwapIntervalEXT();
}

PFN_WGLGETSWAPINTERVALEXT _wglGetSwapIntervalEXT = &_get_wglGetSwapIntervalEXT;

static void * __stdcall _fail_wglAllocateMemoryNV(GLsizei size, GLfloat readfreq, GLfloat writefreq, GLfloat priority) {
	const char *_name = "wglAllocateMemoryNV";
	return NULL;
}

static void * __stdcall _get_wglAllocateMemoryNV(GLsizei size, GLfloat readfreq, GLfloat writefreq, GLfloat priority) {
	PFN_WGLALLOCATEMEMORYNV _ptr;
	_ptr = (PFN_WGLALLOCATEMEMORYNV)_getPrivateProcAddress("wglAllocateMemoryNV");
	if (!_ptr) {
		_ptr = &_fail_wglAllocateMemoryNV;
	}
	_wglAllocateMemoryNV = _ptr;
	return _wglAllocateMemoryNV(size, readfreq, writefreq, priority);
}

PFN_WGLALLOCATEMEMORYNV _wglAllocateMemoryNV = &_get_wglAllocateMemoryNV;

static void __stdcall _fail_wglFreeMemoryNV(void * pointer) {
	const char *_name = "wglFreeMemoryNV";
	
	return;
}

static void __stdcall _get_wglFreeMemoryNV(void * pointer) {
	PFN_WGLFREEMEMORYNV _ptr;
	_ptr = (PFN_WGLFREEMEMORYNV)_getPrivateProcAddress("wglFreeMemoryNV");
	if (!_ptr) {
		_ptr = &_fail_wglFreeMemoryNV;
	}
	_wglFreeMemoryNV = _ptr;
	_wglFreeMemoryNV(pointer);
}

PFN_WGLFREEMEMORYNV _wglFreeMemoryNV = &_get_wglFreeMemoryNV;

static BOOL __stdcall _fail_wglGetSyncValuesOML(HDC hdc, INT64 * ust, INT64 * msc, INT64 * sbc) {
	const char *_name = "wglGetSyncValuesOML";
	return false;
}

static BOOL __stdcall _get_wglGetSyncValuesOML(HDC hdc, INT64 * ust, INT64 * msc, INT64 * sbc) {
	PFN_WGLGETSYNCVALUESOML _ptr;
	_ptr = (PFN_WGLGETSYNCVALUESOML)_getPrivateProcAddress("wglGetSyncValuesOML");
	if (!_ptr) {
		_ptr = &_fail_wglGetSyncValuesOML;
	}
	_wglGetSyncValuesOML = _ptr;
	return _wglGetSyncValuesOML(hdc, ust, msc, sbc);
}

PFN_WGLGETSYNCVALUESOML _wglGetSyncValuesOML = &_get_wglGetSyncValuesOML;

static BOOL __stdcall _fail_wglGetMscRateOML(HDC hdc, INT32 * numerator, INT32 * denominator) {
	const char *_name = "wglGetMscRateOML";
	return false;
}

static BOOL __stdcall _get_wglGetMscRateOML(HDC hdc, INT32 * numerator, INT32 * denominator) {
	PFN_WGLGETMSCRATEOML _ptr;
	_ptr = (PFN_WGLGETMSCRATEOML)_getPrivateProcAddress("wglGetMscRateOML");
	if (!_ptr) {
		_ptr = &_fail_wglGetMscRateOML;
	}
	_wglGetMscRateOML = _ptr;
	return _wglGetMscRateOML(hdc, numerator, denominator);
}

PFN_WGLGETMSCRATEOML _wglGetMscRateOML = &_get_wglGetMscRateOML;

static INT64 __stdcall _fail_wglSwapBuffersMscOML(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder) {
	const char *_name = "wglSwapBuffersMscOML";
	return false;
}

static INT64 __stdcall _get_wglSwapBuffersMscOML(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder) {
	PFN_WGLSWAPBUFFERSMSCOML _ptr;
	_ptr = (PFN_WGLSWAPBUFFERSMSCOML)_getPrivateProcAddress("wglSwapBuffersMscOML");
	if (!_ptr) {
		_ptr = &_fail_wglSwapBuffersMscOML;
	}
	_wglSwapBuffersMscOML = _ptr;
	return _wglSwapBuffersMscOML(hdc, target_msc, divisor, remainder);
}

PFN_WGLSWAPBUFFERSMSCOML _wglSwapBuffersMscOML = &_get_wglSwapBuffersMscOML;

static INT64 __stdcall _fail_wglSwapLayerBuffersMscOML(HDC hdc, int fuPlanes, INT64 target_msc, INT64 divisor, INT64 remainder) {
	const char *_name = "wglSwapLayerBuffersMscOML";
	return false;
}

static INT64 __stdcall _get_wglSwapLayerBuffersMscOML(HDC hdc, int fuPlanes, INT64 target_msc, INT64 divisor, INT64 remainder) {
	PFN_WGLSWAPLAYERBUFFERSMSCOML _ptr;
	_ptr = (PFN_WGLSWAPLAYERBUFFERSMSCOML)_getPrivateProcAddress("wglSwapLayerBuffersMscOML");
	if (!_ptr) {
		_ptr = &_fail_wglSwapLayerBuffersMscOML;
	}
	_wglSwapLayerBuffersMscOML = _ptr;
	return _wglSwapLayerBuffersMscOML(hdc, fuPlanes, target_msc, divisor, remainder);
}

PFN_WGLSWAPLAYERBUFFERSMSCOML _wglSwapLayerBuffersMscOML = &_get_wglSwapLayerBuffersMscOML;

static BOOL __stdcall _fail_wglWaitForMscOML(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder, INT64 * ust, INT64 * msc, INT64 * sbc) {
	const char *_name = "wglWaitForMscOML";
	return false;
}

static BOOL __stdcall _get_wglWaitForMscOML(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder, INT64 * ust, INT64 * msc, INT64 * sbc) {
	PFN_WGLWAITFORMSCOML _ptr;
	_ptr = (PFN_WGLWAITFORMSCOML)_getPrivateProcAddress("wglWaitForMscOML");
	if (!_ptr) {
		_ptr = &_fail_wglWaitForMscOML;
	}
	_wglWaitForMscOML = _ptr;
	return _wglWaitForMscOML(hdc, target_msc, divisor, remainder, ust, msc, sbc);
}

PFN_WGLWAITFORMSCOML _wglWaitForMscOML = &_get_wglWaitForMscOML;

static BOOL __stdcall _fail_wglWaitForSbcOML(HDC hdc, INT64 target_sbc, INT64 * ust, INT64 * msc, INT64 * sbc) {
	const char *_name = "wglWaitForSbcOML";
	return false;
}

static BOOL __stdcall _get_wglWaitForSbcOML(HDC hdc, INT64 target_sbc, INT64 * ust, INT64 * msc, INT64 * sbc) {
	PFN_WGLWAITFORSBCOML _ptr;
	_ptr = (PFN_WGLWAITFORSBCOML)_getPrivateProcAddress("wglWaitForSbcOML");
	if (!_ptr) {
		_ptr = &_fail_wglWaitForSbcOML;
	}
	_wglWaitForSbcOML = _ptr;
	return _wglWaitForSbcOML(hdc, target_sbc, ust, msc, sbc);
}

PFN_WGLWAITFORSBCOML _wglWaitForSbcOML = &_get_wglWaitForSbcOML;

static BOOL __stdcall _fail_wglGetDigitalVideoParametersI3D(HDC hDC, int iAttribute, int * piValue) {
	const char *_name = "wglGetDigitalVideoParametersI3D";
	return false;
}

static BOOL __stdcall _get_wglGetDigitalVideoParametersI3D(HDC hDC, int iAttribute, int * piValue) {
	PFN_WGLGETDIGITALVIDEOPARAMETERSI3D _ptr;
	_ptr = (PFN_WGLGETDIGITALVIDEOPARAMETERSI3D)_getPrivateProcAddress("wglGetDigitalVideoParametersI3D");
	if (!_ptr) {
		_ptr = &_fail_wglGetDigitalVideoParametersI3D;
	}
	_wglGetDigitalVideoParametersI3D = _ptr;
	return _wglGetDigitalVideoParametersI3D(hDC, iAttribute, piValue);
}

PFN_WGLGETDIGITALVIDEOPARAMETERSI3D _wglGetDigitalVideoParametersI3D = &_get_wglGetDigitalVideoParametersI3D;

static BOOL __stdcall _fail_wglSetDigitalVideoParametersI3D(HDC hDC, int iAttribute, const int * piValue) {
	const char *_name = "wglSetDigitalVideoParametersI3D";
	return false;
}

static BOOL __stdcall _get_wglSetDigitalVideoParametersI3D(HDC hDC, int iAttribute, const int * piValue) {
	PFN_WGLSETDIGITALVIDEOPARAMETERSI3D _ptr;
	_ptr = (PFN_WGLSETDIGITALVIDEOPARAMETERSI3D)_getPrivateProcAddress("wglSetDigitalVideoParametersI3D");
	if (!_ptr) {
		_ptr = &_fail_wglSetDigitalVideoParametersI3D;
	}
	_wglSetDigitalVideoParametersI3D = _ptr;
	return _wglSetDigitalVideoParametersI3D(hDC, iAttribute, piValue);
}

PFN_WGLSETDIGITALVIDEOPARAMETERSI3D _wglSetDigitalVideoParametersI3D = &_get_wglSetDigitalVideoParametersI3D;

static BOOL __stdcall _fail_wglGetGammaTableParametersI3D(HDC hDC, int iAttribute, int * piValue) {
	const char *_name = "wglGetGammaTableParametersI3D";
	return false;
}

static BOOL __stdcall _get_wglGetGammaTableParametersI3D(HDC hDC, int iAttribute, int * piValue) {
	PFN_WGLGETGAMMATABLEPARAMETERSI3D _ptr;
	_ptr = (PFN_WGLGETGAMMATABLEPARAMETERSI3D)_getPrivateProcAddress("wglGetGammaTableParametersI3D");
	if (!_ptr) {
		_ptr = &_fail_wglGetGammaTableParametersI3D;
	}
	_wglGetGammaTableParametersI3D = _ptr;
	return _wglGetGammaTableParametersI3D(hDC, iAttribute, piValue);
}

PFN_WGLGETGAMMATABLEPARAMETERSI3D _wglGetGammaTableParametersI3D = &_get_wglGetGammaTableParametersI3D;

static BOOL __stdcall _fail_wglSetGammaTableParametersI3D(HDC hDC, int iAttribute, const int * piValue) {
	const char *_name = "wglSetGammaTableParametersI3D";
	return false;
}

static BOOL __stdcall _get_wglSetGammaTableParametersI3D(HDC hDC, int iAttribute, const int * piValue) {
	PFN_WGLSETGAMMATABLEPARAMETERSI3D _ptr;
	_ptr = (PFN_WGLSETGAMMATABLEPARAMETERSI3D)_getPrivateProcAddress("wglSetGammaTableParametersI3D");
	if (!_ptr) {
		_ptr = &_fail_wglSetGammaTableParametersI3D;
	}
	_wglSetGammaTableParametersI3D = _ptr;
	return _wglSetGammaTableParametersI3D(hDC, iAttribute, piValue);
}

PFN_WGLSETGAMMATABLEPARAMETERSI3D _wglSetGammaTableParametersI3D = &_get_wglSetGammaTableParametersI3D;

static BOOL __stdcall _fail_wglGetGammaTableI3D(HDC hDC, int iEntries, USHORT * puRed, USHORT * puGreen, USHORT * puBlue) {
	const char *_name = "wglGetGammaTableI3D";
	return false;
}

static BOOL __stdcall _get_wglGetGammaTableI3D(HDC hDC, int iEntries, USHORT * puRed, USHORT * puGreen, USHORT * puBlue) {
	PFN_WGLGETGAMMATABLEI3D _ptr;
	_ptr = (PFN_WGLGETGAMMATABLEI3D)_getPrivateProcAddress("wglGetGammaTableI3D");
	if (!_ptr) {
		_ptr = &_fail_wglGetGammaTableI3D;
	}
	_wglGetGammaTableI3D = _ptr;
	return _wglGetGammaTableI3D(hDC, iEntries, puRed, puGreen, puBlue);
}

PFN_WGLGETGAMMATABLEI3D _wglGetGammaTableI3D = &_get_wglGetGammaTableI3D;

static BOOL __stdcall _fail_wglSetGammaTableI3D(HDC hDC, int iEntries, const USHORT * puRed, const USHORT * puGreen, const USHORT * puBlue) {
	const char *_name = "wglSetGammaTableI3D";
	return false;
}

static BOOL __stdcall _get_wglSetGammaTableI3D(HDC hDC, int iEntries, const USHORT * puRed, const USHORT * puGreen, const USHORT * puBlue) {
	PFN_WGLSETGAMMATABLEI3D _ptr;
	_ptr = (PFN_WGLSETGAMMATABLEI3D)_getPrivateProcAddress("wglSetGammaTableI3D");
	if (!_ptr) {
		_ptr = &_fail_wglSetGammaTableI3D;
	}
	_wglSetGammaTableI3D = _ptr;
	return _wglSetGammaTableI3D(hDC, iEntries, puRed, puGreen, puBlue);
}

PFN_WGLSETGAMMATABLEI3D _wglSetGammaTableI3D = &_get_wglSetGammaTableI3D;

static BOOL __stdcall _fail_wglEnableGenlockI3D(HDC hDC) {
	const char *_name = "wglEnableGenlockI3D";
	return false;
}

static BOOL __stdcall _get_wglEnableGenlockI3D(HDC hDC) {
	PFN_WGLENABLEGENLOCKI3D _ptr;
	_ptr = (PFN_WGLENABLEGENLOCKI3D)_getPrivateProcAddress("wglEnableGenlockI3D");
	if (!_ptr) {
		_ptr = &_fail_wglEnableGenlockI3D;
	}
	_wglEnableGenlockI3D = _ptr;
	return _wglEnableGenlockI3D(hDC);
}

PFN_WGLENABLEGENLOCKI3D _wglEnableGenlockI3D = &_get_wglEnableGenlockI3D;

static BOOL __stdcall _fail_wglDisableGenlockI3D(HDC hDC) {
	const char *_name = "wglDisableGenlockI3D";
	return false;
}

static BOOL __stdcall _get_wglDisableGenlockI3D(HDC hDC) {
	PFN_WGLDISABLEGENLOCKI3D _ptr;
	_ptr = (PFN_WGLDISABLEGENLOCKI3D)_getPrivateProcAddress("wglDisableGenlockI3D");
	if (!_ptr) {
		_ptr = &_fail_wglDisableGenlockI3D;
	}
	_wglDisableGenlockI3D = _ptr;
	return _wglDisableGenlockI3D(hDC);
}

PFN_WGLDISABLEGENLOCKI3D _wglDisableGenlockI3D = &_get_wglDisableGenlockI3D;

static BOOL __stdcall _fail_wglIsEnabledGenlockI3D(HDC hDC, BOOL * pFlag) {
	const char *_name = "wglIsEnabledGenlockI3D";
	return false;
}

static BOOL __stdcall _get_wglIsEnabledGenlockI3D(HDC hDC, BOOL * pFlag) {
	PFN_WGLISENABLEDGENLOCKI3D _ptr;
	_ptr = (PFN_WGLISENABLEDGENLOCKI3D)_getPrivateProcAddress("wglIsEnabledGenlockI3D");
	if (!_ptr) {
		_ptr = &_fail_wglIsEnabledGenlockI3D;
	}
	_wglIsEnabledGenlockI3D = _ptr;
	return _wglIsEnabledGenlockI3D(hDC, pFlag);
}

PFN_WGLISENABLEDGENLOCKI3D _wglIsEnabledGenlockI3D = &_get_wglIsEnabledGenlockI3D;

static BOOL __stdcall _fail_wglGenlockSourceI3D(HDC hDC, UINT uSource) {
	const char *_name = "wglGenlockSourceI3D";
	return false;
}

static BOOL __stdcall _get_wglGenlockSourceI3D(HDC hDC, UINT uSource) {
	PFN_WGLGENLOCKSOURCEI3D _ptr;
	_ptr = (PFN_WGLGENLOCKSOURCEI3D)_getPrivateProcAddress("wglGenlockSourceI3D");
	if (!_ptr) {
		_ptr = &_fail_wglGenlockSourceI3D;
	}
	_wglGenlockSourceI3D = _ptr;
	return _wglGenlockSourceI3D(hDC, uSource);
}

PFN_WGLGENLOCKSOURCEI3D _wglGenlockSourceI3D = &_get_wglGenlockSourceI3D;

static BOOL __stdcall _fail_wglGetGenlockSourceI3D(HDC hDC, UINT * uSource) {
	const char *_name = "wglGetGenlockSourceI3D";
	return false;
}

static BOOL __stdcall _get_wglGetGenlockSourceI3D(HDC hDC, UINT * uSource) {
	PFN_WGLGETGENLOCKSOURCEI3D _ptr;
	_ptr = (PFN_WGLGETGENLOCKSOURCEI3D)_getPrivateProcAddress("wglGetGenlockSourceI3D");
	if (!_ptr) {
		_ptr = &_fail_wglGetGenlockSourceI3D;
	}
	_wglGetGenlockSourceI3D = _ptr;
	return _wglGetGenlockSourceI3D(hDC, uSource);
}

PFN_WGLGETGENLOCKSOURCEI3D _wglGetGenlockSourceI3D = &_get_wglGetGenlockSourceI3D;

static BOOL __stdcall _fail_wglGenlockSourceEdgeI3D(HDC hDC, UINT uEdge) {
	const char *_name = "wglGenlockSourceEdgeI3D";
	return false;
}

static BOOL __stdcall _get_wglGenlockSourceEdgeI3D(HDC hDC, UINT uEdge) {
	PFN_WGLGENLOCKSOURCEEDGEI3D _ptr;
	_ptr = (PFN_WGLGENLOCKSOURCEEDGEI3D)_getPrivateProcAddress("wglGenlockSourceEdgeI3D");
	if (!_ptr) {
		_ptr = &_fail_wglGenlockSourceEdgeI3D;
	}
	_wglGenlockSourceEdgeI3D = _ptr;
	return _wglGenlockSourceEdgeI3D(hDC, uEdge);
}

PFN_WGLGENLOCKSOURCEEDGEI3D _wglGenlockSourceEdgeI3D = &_get_wglGenlockSourceEdgeI3D;

static BOOL __stdcall _fail_wglGetGenlockSourceEdgeI3D(HDC hDC, UINT * uEdge) {
	const char *_name = "wglGetGenlockSourceEdgeI3D";
	return false;
}

static BOOL __stdcall _get_wglGetGenlockSourceEdgeI3D(HDC hDC, UINT * uEdge) {
	PFN_WGLGETGENLOCKSOURCEEDGEI3D _ptr;
	_ptr = (PFN_WGLGETGENLOCKSOURCEEDGEI3D)_getPrivateProcAddress("wglGetGenlockSourceEdgeI3D");
	if (!_ptr) {
		_ptr = &_fail_wglGetGenlockSourceEdgeI3D;
	}
	_wglGetGenlockSourceEdgeI3D = _ptr;
	return _wglGetGenlockSourceEdgeI3D(hDC, uEdge);
}

PFN_WGLGETGENLOCKSOURCEEDGEI3D _wglGetGenlockSourceEdgeI3D = &_get_wglGetGenlockSourceEdgeI3D;

static BOOL __stdcall _fail_wglGenlockSampleRateI3D(HDC hDC, UINT uRate) {
	const char *_name = "wglGenlockSampleRateI3D";
	return false;
}

static BOOL __stdcall _get_wglGenlockSampleRateI3D(HDC hDC, UINT uRate) {
	PFN_WGLGENLOCKSAMPLERATEI3D _ptr;
	_ptr = (PFN_WGLGENLOCKSAMPLERATEI3D)_getPrivateProcAddress("wglGenlockSampleRateI3D");
	if (!_ptr) {
		_ptr = &_fail_wglGenlockSampleRateI3D;
	}
	_wglGenlockSampleRateI3D = _ptr;
	return _wglGenlockSampleRateI3D(hDC, uRate);
}

PFN_WGLGENLOCKSAMPLERATEI3D _wglGenlockSampleRateI3D = &_get_wglGenlockSampleRateI3D;

static BOOL __stdcall _fail_wglGetGenlockSampleRateI3D(HDC hDC, UINT * uRate) {
	const char *_name = "wglGetGenlockSampleRateI3D";
	return false;
}

static BOOL __stdcall _get_wglGetGenlockSampleRateI3D(HDC hDC, UINT * uRate) {
	PFN_WGLGETGENLOCKSAMPLERATEI3D _ptr;
	_ptr = (PFN_WGLGETGENLOCKSAMPLERATEI3D)_getPrivateProcAddress("wglGetGenlockSampleRateI3D");
	if (!_ptr) {
		_ptr = &_fail_wglGetGenlockSampleRateI3D;
	}
	_wglGetGenlockSampleRateI3D = _ptr;
	return _wglGetGenlockSampleRateI3D(hDC, uRate);
}

PFN_WGLGETGENLOCKSAMPLERATEI3D _wglGetGenlockSampleRateI3D = &_get_wglGetGenlockSampleRateI3D;

static BOOL __stdcall _fail_wglGenlockSourceDelayI3D(HDC hDC, UINT uDelay) {
	const char *_name = "wglGenlockSourceDelayI3D";
	return false;
}

static BOOL __stdcall _get_wglGenlockSourceDelayI3D(HDC hDC, UINT uDelay) {
	PFN_WGLGENLOCKSOURCEDELAYI3D _ptr;
	_ptr = (PFN_WGLGENLOCKSOURCEDELAYI3D)_getPrivateProcAddress("wglGenlockSourceDelayI3D");
	if (!_ptr) {
		_ptr = &_fail_wglGenlockSourceDelayI3D;
	}
	_wglGenlockSourceDelayI3D = _ptr;
	return _wglGenlockSourceDelayI3D(hDC, uDelay);
}

PFN_WGLGENLOCKSOURCEDELAYI3D _wglGenlockSourceDelayI3D = &_get_wglGenlockSourceDelayI3D;

static BOOL __stdcall _fail_wglGetGenlockSourceDelayI3D(HDC hDC, UINT * uDelay) {
	const char *_name = "wglGetGenlockSourceDelayI3D";
	return false;
}

static BOOL __stdcall _get_wglGetGenlockSourceDelayI3D(HDC hDC, UINT * uDelay) {
	PFN_WGLGETGENLOCKSOURCEDELAYI3D _ptr;
	_ptr = (PFN_WGLGETGENLOCKSOURCEDELAYI3D)_getPrivateProcAddress("wglGetGenlockSourceDelayI3D");
	if (!_ptr) {
		_ptr = &_fail_wglGetGenlockSourceDelayI3D;
	}
	_wglGetGenlockSourceDelayI3D = _ptr;
	return _wglGetGenlockSourceDelayI3D(hDC, uDelay);
}

PFN_WGLGETGENLOCKSOURCEDELAYI3D _wglGetGenlockSourceDelayI3D = &_get_wglGetGenlockSourceDelayI3D;

static BOOL __stdcall _fail_wglQueryGenlockMaxSourceDelayI3D(HDC hDC, UINT * uMaxLineDelay, UINT * uMaxPixelDelay) {
	const char *_name = "wglQueryGenlockMaxSourceDelayI3D";
	return false;
}

static BOOL __stdcall _get_wglQueryGenlockMaxSourceDelayI3D(HDC hDC, UINT * uMaxLineDelay, UINT * uMaxPixelDelay) {
	PFN_WGLQUERYGENLOCKMAXSOURCEDELAYI3D _ptr;
	_ptr = (PFN_WGLQUERYGENLOCKMAXSOURCEDELAYI3D)_getPrivateProcAddress("wglQueryGenlockMaxSourceDelayI3D");
	if (!_ptr) {
		_ptr = &_fail_wglQueryGenlockMaxSourceDelayI3D;
	}
	_wglQueryGenlockMaxSourceDelayI3D = _ptr;
	return _wglQueryGenlockMaxSourceDelayI3D(hDC, uMaxLineDelay, uMaxPixelDelay);
}

PFN_WGLQUERYGENLOCKMAXSOURCEDELAYI3D _wglQueryGenlockMaxSourceDelayI3D = &_get_wglQueryGenlockMaxSourceDelayI3D;

static void * __stdcall _fail_wglCreateImageBufferI3D(HDC hDC, DWORD dwSize, UINT uFlags) {
	const char *_name = "wglCreateImageBufferI3D";
	return NULL;
}

static void * __stdcall _get_wglCreateImageBufferI3D(HDC hDC, DWORD dwSize, UINT uFlags) {
	PFN_WGLCREATEIMAGEBUFFERI3D _ptr;
	_ptr = (PFN_WGLCREATEIMAGEBUFFERI3D)_getPrivateProcAddress("wglCreateImageBufferI3D");
	if (!_ptr) {
		_ptr = &_fail_wglCreateImageBufferI3D;
	}
	_wglCreateImageBufferI3D = _ptr;
	return _wglCreateImageBufferI3D(hDC, dwSize, uFlags);
}

PFN_WGLCREATEIMAGEBUFFERI3D _wglCreateImageBufferI3D = &_get_wglCreateImageBufferI3D;

static BOOL __stdcall _fail_wglDestroyImageBufferI3D(HDC hDC, void * pAddress) {
	const char *_name = "wglDestroyImageBufferI3D";
	return false;
}

static BOOL __stdcall _get_wglDestroyImageBufferI3D(HDC hDC, void * pAddress) {
	PFN_WGLDESTROYIMAGEBUFFERI3D _ptr;
	_ptr = (PFN_WGLDESTROYIMAGEBUFFERI3D)_getPrivateProcAddress("wglDestroyImageBufferI3D");
	if (!_ptr) {
		_ptr = &_fail_wglDestroyImageBufferI3D;
	}
	_wglDestroyImageBufferI3D = _ptr;
	return _wglDestroyImageBufferI3D(hDC, pAddress);
}

PFN_WGLDESTROYIMAGEBUFFERI3D _wglDestroyImageBufferI3D = &_get_wglDestroyImageBufferI3D;

static BOOL __stdcall _fail_wglAssociateImageBufferEventsI3D(HDC hDC, const HANDLE * pEvent, void * const * pAddress, const DWORD * pSize, UINT count) {
	const char *_name = "wglAssociateImageBufferEventsI3D";
	return false;
}

static BOOL __stdcall _get_wglAssociateImageBufferEventsI3D(HDC hDC, const HANDLE * pEvent, void * const * pAddress, const DWORD * pSize, UINT count) {
	PFN_WGLASSOCIATEIMAGEBUFFEREVENTSI3D _ptr;
	_ptr = (PFN_WGLASSOCIATEIMAGEBUFFEREVENTSI3D)_getPrivateProcAddress("wglAssociateImageBufferEventsI3D");
	if (!_ptr) {
		_ptr = &_fail_wglAssociateImageBufferEventsI3D;
	}
	_wglAssociateImageBufferEventsI3D = _ptr;
	return _wglAssociateImageBufferEventsI3D(hDC, pEvent, pAddress, pSize, count);
}

PFN_WGLASSOCIATEIMAGEBUFFEREVENTSI3D _wglAssociateImageBufferEventsI3D = &_get_wglAssociateImageBufferEventsI3D;

static BOOL __stdcall _fail_wglReleaseImageBufferEventsI3D(HDC hDC, void * const * pAddress, UINT count) {
	const char *_name = "wglReleaseImageBufferEventsI3D";
	return false;
}

static BOOL __stdcall _get_wglReleaseImageBufferEventsI3D(HDC hDC, void * const * pAddress, UINT count) {
	PFN_WGLRELEASEIMAGEBUFFEREVENTSI3D _ptr;
	_ptr = (PFN_WGLRELEASEIMAGEBUFFEREVENTSI3D)_getPrivateProcAddress("wglReleaseImageBufferEventsI3D");
	if (!_ptr) {
		_ptr = &_fail_wglReleaseImageBufferEventsI3D;
	}
	_wglReleaseImageBufferEventsI3D = _ptr;
	return _wglReleaseImageBufferEventsI3D(hDC, pAddress, count);
}

PFN_WGLRELEASEIMAGEBUFFEREVENTSI3D _wglReleaseImageBufferEventsI3D = &_get_wglReleaseImageBufferEventsI3D;

static BOOL __stdcall _fail_wglEnableFrameLockI3D(void) {
	const char *_name = "wglEnableFrameLockI3D";
	return false;
}

static BOOL __stdcall _get_wglEnableFrameLockI3D(void) {
	PFN_WGLENABLEFRAMELOCKI3D _ptr;
	_ptr = (PFN_WGLENABLEFRAMELOCKI3D)_getPrivateProcAddress("wglEnableFrameLockI3D");
	if (!_ptr) {
		_ptr = &_fail_wglEnableFrameLockI3D;
	}
	_wglEnableFrameLockI3D = _ptr;
	return _wglEnableFrameLockI3D();
}

PFN_WGLENABLEFRAMELOCKI3D _wglEnableFrameLockI3D = &_get_wglEnableFrameLockI3D;

static BOOL __stdcall _fail_wglDisableFrameLockI3D(void) {
	const char *_name = "wglDisableFrameLockI3D";
	return false;
}

static BOOL __stdcall _get_wglDisableFrameLockI3D(void) {
	PFN_WGLDISABLEFRAMELOCKI3D _ptr;
	_ptr = (PFN_WGLDISABLEFRAMELOCKI3D)_getPrivateProcAddress("wglDisableFrameLockI3D");
	if (!_ptr) {
		_ptr = &_fail_wglDisableFrameLockI3D;
	}
	_wglDisableFrameLockI3D = _ptr;
	return _wglDisableFrameLockI3D();
}

PFN_WGLDISABLEFRAMELOCKI3D _wglDisableFrameLockI3D = &_get_wglDisableFrameLockI3D;

static BOOL __stdcall _fail_wglIsEnabledFrameLockI3D(BOOL * pFlag) {
	const char *_name = "wglIsEnabledFrameLockI3D";
	return false;
}

static BOOL __stdcall _get_wglIsEnabledFrameLockI3D(BOOL * pFlag) {
	PFN_WGLISENABLEDFRAMELOCKI3D _ptr;
	_ptr = (PFN_WGLISENABLEDFRAMELOCKI3D)_getPrivateProcAddress("wglIsEnabledFrameLockI3D");
	if (!_ptr) {
		_ptr = &_fail_wglIsEnabledFrameLockI3D;
	}
	_wglIsEnabledFrameLockI3D = _ptr;
	return _wglIsEnabledFrameLockI3D(pFlag);
}

PFN_WGLISENABLEDFRAMELOCKI3D _wglIsEnabledFrameLockI3D = &_get_wglIsEnabledFrameLockI3D;

static BOOL __stdcall _fail_wglQueryFrameLockMasterI3D(BOOL * pFlag) {
	const char *_name = "wglQueryFrameLockMasterI3D";
	return false;
}

static BOOL __stdcall _get_wglQueryFrameLockMasterI3D(BOOL * pFlag) {
	PFN_WGLQUERYFRAMELOCKMASTERI3D _ptr;
	_ptr = (PFN_WGLQUERYFRAMELOCKMASTERI3D)_getPrivateProcAddress("wglQueryFrameLockMasterI3D");
	if (!_ptr) {
		_ptr = &_fail_wglQueryFrameLockMasterI3D;
	}
	_wglQueryFrameLockMasterI3D = _ptr;
	return _wglQueryFrameLockMasterI3D(pFlag);
}

PFN_WGLQUERYFRAMELOCKMASTERI3D _wglQueryFrameLockMasterI3D = &_get_wglQueryFrameLockMasterI3D;

static BOOL __stdcall _fail_wglGetFrameUsageI3D(float * pUsage) {
	const char *_name = "wglGetFrameUsageI3D";
	return false;
}

static BOOL __stdcall _get_wglGetFrameUsageI3D(float * pUsage) {
	PFN_WGLGETFRAMEUSAGEI3D _ptr;
	_ptr = (PFN_WGLGETFRAMEUSAGEI3D)_getPrivateProcAddress("wglGetFrameUsageI3D");
	if (!_ptr) {
		_ptr = &_fail_wglGetFrameUsageI3D;
	}
	_wglGetFrameUsageI3D = _ptr;
	return _wglGetFrameUsageI3D(pUsage);
}

PFN_WGLGETFRAMEUSAGEI3D _wglGetFrameUsageI3D = &_get_wglGetFrameUsageI3D;

static BOOL __stdcall _fail_wglBeginFrameTrackingI3D(void) {
	const char *_name = "wglBeginFrameTrackingI3D";
	return false;
}

static BOOL __stdcall _get_wglBeginFrameTrackingI3D(void) {
	PFN_WGLBEGINFRAMETRACKINGI3D _ptr;
	_ptr = (PFN_WGLBEGINFRAMETRACKINGI3D)_getPrivateProcAddress("wglBeginFrameTrackingI3D");
	if (!_ptr) {
		_ptr = &_fail_wglBeginFrameTrackingI3D;
	}
	_wglBeginFrameTrackingI3D = _ptr;
	return _wglBeginFrameTrackingI3D();
}

PFN_WGLBEGINFRAMETRACKINGI3D _wglBeginFrameTrackingI3D = &_get_wglBeginFrameTrackingI3D;

static BOOL __stdcall _fail_wglEndFrameTrackingI3D(void) {
	const char *_name = "wglEndFrameTrackingI3D";
	return false;
}

static BOOL __stdcall _get_wglEndFrameTrackingI3D(void) {
	PFN_WGLENDFRAMETRACKINGI3D _ptr;
	_ptr = (PFN_WGLENDFRAMETRACKINGI3D)_getPrivateProcAddress("wglEndFrameTrackingI3D");
	if (!_ptr) {
		_ptr = &_fail_wglEndFrameTrackingI3D;
	}
	_wglEndFrameTrackingI3D = _ptr;
	return _wglEndFrameTrackingI3D();
}

PFN_WGLENDFRAMETRACKINGI3D _wglEndFrameTrackingI3D = &_get_wglEndFrameTrackingI3D;

static BOOL __stdcall _fail_wglQueryFrameTrackingI3D(DWORD * pFrameCount, DWORD * pMissedFrames, float * pLastMissedUsage) {
	const char *_name = "wglQueryFrameTrackingI3D";
	return false;
}

static BOOL __stdcall _get_wglQueryFrameTrackingI3D(DWORD * pFrameCount, DWORD * pMissedFrames, float * pLastMissedUsage) {
	PFN_WGLQUERYFRAMETRACKINGI3D _ptr;
	_ptr = (PFN_WGLQUERYFRAMETRACKINGI3D)_getPrivateProcAddress("wglQueryFrameTrackingI3D");
	if (!_ptr) {
		_ptr = &_fail_wglQueryFrameTrackingI3D;
	}
	_wglQueryFrameTrackingI3D = _ptr;
	return _wglQueryFrameTrackingI3D(pFrameCount, pMissedFrames, pLastMissedUsage);
}

PFN_WGLQUERYFRAMETRACKINGI3D _wglQueryFrameTrackingI3D = &_get_wglQueryFrameTrackingI3D;

static BOOL __stdcall _fail_wglSetStereoEmitterState3DL(HDC hDC, UINT uState) {
	const char *_name = "wglSetStereoEmitterState3DL";
	return false;
}

static BOOL __stdcall _get_wglSetStereoEmitterState3DL(HDC hDC, UINT uState) {
	PFN_WGLSETSTEREOEMITTERSTATE3DL _ptr;
	_ptr = (PFN_WGLSETSTEREOEMITTERSTATE3DL)_getPrivateProcAddress("wglSetStereoEmitterState3DL");
	if (!_ptr) {
		_ptr = &_fail_wglSetStereoEmitterState3DL;
	}
	_wglSetStereoEmitterState3DL = _ptr;
	return _wglSetStereoEmitterState3DL(hDC, uState);
}

PFN_WGLSETSTEREOEMITTERSTATE3DL _wglSetStereoEmitterState3DL = &_get_wglSetStereoEmitterState3DL;

static int __stdcall _fail_wglEnumerateVideoDevicesNV(HDC hDC, HVIDEOOUTPUTDEVICENV * phDeviceList) {
	const char *_name = "wglEnumerateVideoDevicesNV";
	return false;
}

static int __stdcall _get_wglEnumerateVideoDevicesNV(HDC hDC, HVIDEOOUTPUTDEVICENV * phDeviceList) {
	PFN_WGLENUMERATEVIDEODEVICESNV _ptr;
	_ptr = (PFN_WGLENUMERATEVIDEODEVICESNV)_getPrivateProcAddress("wglEnumerateVideoDevicesNV");
	if (!_ptr) {
		_ptr = &_fail_wglEnumerateVideoDevicesNV;
	}
	_wglEnumerateVideoDevicesNV = _ptr;
	return _wglEnumerateVideoDevicesNV(hDC, phDeviceList);
}

PFN_WGLENUMERATEVIDEODEVICESNV _wglEnumerateVideoDevicesNV = &_get_wglEnumerateVideoDevicesNV;

static BOOL __stdcall _fail_wglBindVideoDeviceNV(HDC hDC, unsigned int uVideoSlot, HVIDEOOUTPUTDEVICENV hVideoDevice, const int * piAttribList) {
	const char *_name = "wglBindVideoDeviceNV";
	return false;
}

static BOOL __stdcall _get_wglBindVideoDeviceNV(HDC hDC, unsigned int uVideoSlot, HVIDEOOUTPUTDEVICENV hVideoDevice, const int * piAttribList) {
	PFN_WGLBINDVIDEODEVICENV _ptr;
	_ptr = (PFN_WGLBINDVIDEODEVICENV)_getPrivateProcAddress("wglBindVideoDeviceNV");
	if (!_ptr) {
		_ptr = &_fail_wglBindVideoDeviceNV;
	}
	_wglBindVideoDeviceNV = _ptr;
	return _wglBindVideoDeviceNV(hDC, uVideoSlot, hVideoDevice, piAttribList);
}

PFN_WGLBINDVIDEODEVICENV _wglBindVideoDeviceNV = &_get_wglBindVideoDeviceNV;

static BOOL __stdcall _fail_wglQueryCurrentContextNV(int iAttribute, int * piValue) {
	const char *_name = "wglQueryCurrentContextNV";
	return false;
}

static BOOL __stdcall _get_wglQueryCurrentContextNV(int iAttribute, int * piValue) {
	PFN_WGLQUERYCURRENTCONTEXTNV _ptr;
	_ptr = (PFN_WGLQUERYCURRENTCONTEXTNV)_getPrivateProcAddress("wglQueryCurrentContextNV");
	if (!_ptr) {
		_ptr = &_fail_wglQueryCurrentContextNV;
	}
	_wglQueryCurrentContextNV = _ptr;
	return _wglQueryCurrentContextNV(iAttribute, piValue);
}

PFN_WGLQUERYCURRENTCONTEXTNV _wglQueryCurrentContextNV = &_get_wglQueryCurrentContextNV;

static BOOL __stdcall _fail_wglGetVideoDeviceNV(HDC hDC, int numDevices, HPVIDEODEV * hVideoDevice) {
	const char *_name = "wglGetVideoDeviceNV";
	return false;
}

static BOOL __stdcall _get_wglGetVideoDeviceNV(HDC hDC, int numDevices, HPVIDEODEV * hVideoDevice) {
	PFN_WGLGETVIDEODEVICENV _ptr;
	_ptr = (PFN_WGLGETVIDEODEVICENV)_getPrivateProcAddress("wglGetVideoDeviceNV");
	if (!_ptr) {
		_ptr = &_fail_wglGetVideoDeviceNV;
	}
	_wglGetVideoDeviceNV = _ptr;
	return _wglGetVideoDeviceNV(hDC, numDevices, hVideoDevice);
}

PFN_WGLGETVIDEODEVICENV _wglGetVideoDeviceNV = &_get_wglGetVideoDeviceNV;

static BOOL __stdcall _fail_wglReleaseVideoDeviceNV(HPVIDEODEV hVideoDevice) {
	const char *_name = "wglReleaseVideoDeviceNV";
	return false;
}

static BOOL __stdcall _get_wglReleaseVideoDeviceNV(HPVIDEODEV hVideoDevice) {
	PFN_WGLRELEASEVIDEODEVICENV _ptr;
	_ptr = (PFN_WGLRELEASEVIDEODEVICENV)_getPrivateProcAddress("wglReleaseVideoDeviceNV");
	if (!_ptr) {
		_ptr = &_fail_wglReleaseVideoDeviceNV;
	}
	_wglReleaseVideoDeviceNV = _ptr;
	return _wglReleaseVideoDeviceNV(hVideoDevice);
}

PFN_WGLRELEASEVIDEODEVICENV _wglReleaseVideoDeviceNV = &_get_wglReleaseVideoDeviceNV;

static BOOL __stdcall _fail_wglBindVideoImageNV(HPVIDEODEV hVideoDevice, HPBUFFERARB hPbuffer, int iVideoBuffer) {
	const char *_name = "wglBindVideoImageNV";
	return false;
}

static BOOL __stdcall _get_wglBindVideoImageNV(HPVIDEODEV hVideoDevice, HPBUFFERARB hPbuffer, int iVideoBuffer) {
	PFN_WGLBINDVIDEOIMAGENV _ptr;
	_ptr = (PFN_WGLBINDVIDEOIMAGENV)_getPrivateProcAddress("wglBindVideoImageNV");
	if (!_ptr) {
		_ptr = &_fail_wglBindVideoImageNV;
	}
	_wglBindVideoImageNV = _ptr;
	return _wglBindVideoImageNV(hVideoDevice, hPbuffer, iVideoBuffer);
}

PFN_WGLBINDVIDEOIMAGENV _wglBindVideoImageNV = &_get_wglBindVideoImageNV;

static BOOL __stdcall _fail_wglReleaseVideoImageNV(HPBUFFERARB hPbuffer, int iVideoBuffer) {
	const char *_name = "wglReleaseVideoImageNV";
	return false;
}

static BOOL __stdcall _get_wglReleaseVideoImageNV(HPBUFFERARB hPbuffer, int iVideoBuffer) {
	PFN_WGLRELEASEVIDEOIMAGENV _ptr;
	_ptr = (PFN_WGLRELEASEVIDEOIMAGENV)_getPrivateProcAddress("wglReleaseVideoImageNV");
	if (!_ptr) {
		_ptr = &_fail_wglReleaseVideoImageNV;
	}
	_wglReleaseVideoImageNV = _ptr;
	return _wglReleaseVideoImageNV(hPbuffer, iVideoBuffer);
}

PFN_WGLRELEASEVIDEOIMAGENV _wglReleaseVideoImageNV = &_get_wglReleaseVideoImageNV;

static BOOL __stdcall _fail_wglSendPbufferToVideoNV(HPBUFFERARB hPbuffer, int iBufferType, unsigned long * pulCounterPbuffer, BOOL bBlock) {
	const char *_name = "wglSendPbufferToVideoNV";
	return false;
}

static BOOL __stdcall _get_wglSendPbufferToVideoNV(HPBUFFERARB hPbuffer, int iBufferType, unsigned long * pulCounterPbuffer, BOOL bBlock) {
	PFN_WGLSENDPBUFFERTOVIDEONV _ptr;
	_ptr = (PFN_WGLSENDPBUFFERTOVIDEONV)_getPrivateProcAddress("wglSendPbufferToVideoNV");
	if (!_ptr) {
		_ptr = &_fail_wglSendPbufferToVideoNV;
	}
	_wglSendPbufferToVideoNV = _ptr;
	return _wglSendPbufferToVideoNV(hPbuffer, iBufferType, pulCounterPbuffer, bBlock);
}

PFN_WGLSENDPBUFFERTOVIDEONV _wglSendPbufferToVideoNV = &_get_wglSendPbufferToVideoNV;

static BOOL __stdcall _fail_wglGetVideoInfoNV(HPVIDEODEV hpVideoDevice, unsigned long * pulCounterOutputPbuffer, unsigned long * pulCounterOutputVideo) {
	const char *_name = "wglGetVideoInfoNV";
	return false;
}

static BOOL __stdcall _get_wglGetVideoInfoNV(HPVIDEODEV hpVideoDevice, unsigned long * pulCounterOutputPbuffer, unsigned long * pulCounterOutputVideo) {
	PFN_WGLGETVIDEOINFONV _ptr;
	_ptr = (PFN_WGLGETVIDEOINFONV)_getPrivateProcAddress("wglGetVideoInfoNV");
	if (!_ptr) {
		_ptr = &_fail_wglGetVideoInfoNV;
	}
	_wglGetVideoInfoNV = _ptr;
	return _wglGetVideoInfoNV(hpVideoDevice, pulCounterOutputPbuffer, pulCounterOutputVideo);
}

PFN_WGLGETVIDEOINFONV _wglGetVideoInfoNV = &_get_wglGetVideoInfoNV;

static BOOL __stdcall _fail_wglJoinSwapGroupNV(HDC hDC, GLuint group) {
	const char *_name = "wglJoinSwapGroupNV";
	return false;
}

static BOOL __stdcall _get_wglJoinSwapGroupNV(HDC hDC, GLuint group) {
	PFN_WGLJOINSWAPGROUPNV _ptr;
	_ptr = (PFN_WGLJOINSWAPGROUPNV)_getPrivateProcAddress("wglJoinSwapGroupNV");
	if (!_ptr) {
		_ptr = &_fail_wglJoinSwapGroupNV;
	}
	_wglJoinSwapGroupNV = _ptr;
	return _wglJoinSwapGroupNV(hDC, group);
}

PFN_WGLJOINSWAPGROUPNV _wglJoinSwapGroupNV = &_get_wglJoinSwapGroupNV;

static BOOL __stdcall _fail_wglBindSwapBarrierNV(GLuint group, GLuint barrier) {
	const char *_name = "wglBindSwapBarrierNV";
	return false;
}

static BOOL __stdcall _get_wglBindSwapBarrierNV(GLuint group, GLuint barrier) {
	PFN_WGLBINDSWAPBARRIERNV _ptr;
	_ptr = (PFN_WGLBINDSWAPBARRIERNV)_getPrivateProcAddress("wglBindSwapBarrierNV");
	if (!_ptr) {
		_ptr = &_fail_wglBindSwapBarrierNV;
	}
	_wglBindSwapBarrierNV = _ptr;
	return _wglBindSwapBarrierNV(group, barrier);
}

PFN_WGLBINDSWAPBARRIERNV _wglBindSwapBarrierNV = &_get_wglBindSwapBarrierNV;

static BOOL __stdcall _fail_wglQuerySwapGroupNV(HDC hDC, GLuint * group, GLuint * barrier) {
	const char *_name = "wglQuerySwapGroupNV";
	return false;
}

static BOOL __stdcall _get_wglQuerySwapGroupNV(HDC hDC, GLuint * group, GLuint * barrier) {
	PFN_WGLQUERYSWAPGROUPNV _ptr;
	_ptr = (PFN_WGLQUERYSWAPGROUPNV)_getPrivateProcAddress("wglQuerySwapGroupNV");
	if (!_ptr) {
		_ptr = &_fail_wglQuerySwapGroupNV;
	}
	_wglQuerySwapGroupNV = _ptr;
	return _wglQuerySwapGroupNV(hDC, group, barrier);
}

PFN_WGLQUERYSWAPGROUPNV _wglQuerySwapGroupNV = &_get_wglQuerySwapGroupNV;

static BOOL __stdcall _fail_wglQueryMaxSwapGroupsNV(HDC hDC, GLuint * maxGroups, GLuint * maxBarriers) {
	const char *_name = "wglQueryMaxSwapGroupsNV";
	return false;
}

static BOOL __stdcall _get_wglQueryMaxSwapGroupsNV(HDC hDC, GLuint * maxGroups, GLuint * maxBarriers) {
	PFN_WGLQUERYMAXSWAPGROUPSNV _ptr;
	_ptr = (PFN_WGLQUERYMAXSWAPGROUPSNV)_getPrivateProcAddress("wglQueryMaxSwapGroupsNV");
	if (!_ptr) {
		_ptr = &_fail_wglQueryMaxSwapGroupsNV;
	}
	_wglQueryMaxSwapGroupsNV = _ptr;
	return _wglQueryMaxSwapGroupsNV(hDC, maxGroups, maxBarriers);
}

PFN_WGLQUERYMAXSWAPGROUPSNV _wglQueryMaxSwapGroupsNV = &_get_wglQueryMaxSwapGroupsNV;

static BOOL __stdcall _fail_wglQueryFrameCountNV(HDC hDC, GLuint * count) {
	const char *_name = "wglQueryFrameCountNV";
	return false;
}

static BOOL __stdcall _get_wglQueryFrameCountNV(HDC hDC, GLuint * count) {
	PFN_WGLQUERYFRAMECOUNTNV _ptr;
	_ptr = (PFN_WGLQUERYFRAMECOUNTNV)_getPrivateProcAddress("wglQueryFrameCountNV");
	if (!_ptr) {
		_ptr = &_fail_wglQueryFrameCountNV;
	}
	_wglQueryFrameCountNV = _ptr;
	return _wglQueryFrameCountNV(hDC, count);
}

PFN_WGLQUERYFRAMECOUNTNV _wglQueryFrameCountNV = &_get_wglQueryFrameCountNV;

static BOOL __stdcall _fail_wglResetFrameCountNV(HDC hDC) {
	const char *_name = "wglResetFrameCountNV";
	return false;
}

static BOOL __stdcall _get_wglResetFrameCountNV(HDC hDC) {
	PFN_WGLRESETFRAMECOUNTNV _ptr;
	_ptr = (PFN_WGLRESETFRAMECOUNTNV)_getPrivateProcAddress("wglResetFrameCountNV");
	if (!_ptr) {
		_ptr = &_fail_wglResetFrameCountNV;
	}
	_wglResetFrameCountNV = _ptr;
	return _wglResetFrameCountNV(hDC);
}

PFN_WGLRESETFRAMECOUNTNV _wglResetFrameCountNV = &_get_wglResetFrameCountNV;

static BOOL __stdcall _fail_wglEnumGpusNV(UINT iGpuIndex, HGPUNV * phGpu) {
	const char *_name = "wglEnumGpusNV";
	return false;
}

static BOOL __stdcall _get_wglEnumGpusNV(UINT iGpuIndex, HGPUNV * phGpu) {
	PFN_WGLENUMGPUSNV _ptr;
	_ptr = (PFN_WGLENUMGPUSNV)_getPrivateProcAddress("wglEnumGpusNV");
	if (!_ptr) {
		_ptr = &_fail_wglEnumGpusNV;
	}
	_wglEnumGpusNV = _ptr;
	return _wglEnumGpusNV(iGpuIndex, phGpu);
}

PFN_WGLENUMGPUSNV _wglEnumGpusNV = &_get_wglEnumGpusNV;

static BOOL __stdcall _fail_wglEnumGpuDevicesNV(HGPUNV hGpu, UINT iDeviceIndex, _GPU_DEVICE * lpGpuDevice) {
	const char *_name = "wglEnumGpuDevicesNV";
	return false;
}

static BOOL __stdcall _get_wglEnumGpuDevicesNV(HGPUNV hGpu, UINT iDeviceIndex, _GPU_DEVICE * lpGpuDevice) {
	PFN_WGLENUMGPUDEVICESNV _ptr;
	_ptr = (PFN_WGLENUMGPUDEVICESNV)_getPrivateProcAddress("wglEnumGpuDevicesNV");
	if (!_ptr) {
		_ptr = &_fail_wglEnumGpuDevicesNV;
	}
	_wglEnumGpuDevicesNV = _ptr;
	return _wglEnumGpuDevicesNV(hGpu, iDeviceIndex, lpGpuDevice);
}

PFN_WGLENUMGPUDEVICESNV _wglEnumGpuDevicesNV = &_get_wglEnumGpuDevicesNV;

static HDC __stdcall _fail_wglCreateAffinityDCNV(const HGPUNV * phGpuList) {
	const char *_name = "wglCreateAffinityDCNV";
	return false;
}

static HDC __stdcall _get_wglCreateAffinityDCNV(const HGPUNV * phGpuList) {
	PFN_WGLCREATEAFFINITYDCNV _ptr;
	_ptr = (PFN_WGLCREATEAFFINITYDCNV)_getPrivateProcAddress("wglCreateAffinityDCNV");
	if (!_ptr) {
		_ptr = &_fail_wglCreateAffinityDCNV;
	}
	_wglCreateAffinityDCNV = _ptr;
	return _wglCreateAffinityDCNV(phGpuList);
}

PFN_WGLCREATEAFFINITYDCNV _wglCreateAffinityDCNV = &_get_wglCreateAffinityDCNV;

static BOOL __stdcall _fail_wglEnumGpusFromAffinityDCNV(HDC hAffinityDC, UINT iGpuIndex, HGPUNV * hGpu) {
	const char *_name = "wglEnumGpusFromAffinityDCNV";
	return false;
}

static BOOL __stdcall _get_wglEnumGpusFromAffinityDCNV(HDC hAffinityDC, UINT iGpuIndex, HGPUNV * hGpu) {
	PFN_WGLENUMGPUSFROMAFFINITYDCNV _ptr;
	_ptr = (PFN_WGLENUMGPUSFROMAFFINITYDCNV)_getPrivateProcAddress("wglEnumGpusFromAffinityDCNV");
	if (!_ptr) {
		_ptr = &_fail_wglEnumGpusFromAffinityDCNV;
	}
	_wglEnumGpusFromAffinityDCNV = _ptr;
	return _wglEnumGpusFromAffinityDCNV(hAffinityDC, iGpuIndex, hGpu);
}

PFN_WGLENUMGPUSFROMAFFINITYDCNV _wglEnumGpusFromAffinityDCNV = &_get_wglEnumGpusFromAffinityDCNV;

static BOOL __stdcall _fail_wglDeleteDCNV(HDC hdc) {
	const char *_name = "wglDeleteDCNV";
	return false;
}

static BOOL __stdcall _get_wglDeleteDCNV(HDC hdc) {
	PFN_WGLDELETEDCNV _ptr;
	_ptr = (PFN_WGLDELETEDCNV)_getPrivateProcAddress("wglDeleteDCNV");
	if (!_ptr) {
		_ptr = &_fail_wglDeleteDCNV;
	}
	_wglDeleteDCNV = _ptr;
	return _wglDeleteDCNV(hdc);
}

PFN_WGLDELETEDCNV _wglDeleteDCNV = &_get_wglDeleteDCNV;

static UINT __stdcall _fail_wglGetGPUIDsAMD(UINT maxCount, UINT * ids) {
	const char *_name = "wglGetGPUIDsAMD";
	return false;
}

static UINT __stdcall _get_wglGetGPUIDsAMD(UINT maxCount, UINT * ids) {
	PFN_WGLGETGPUIDSAMD _ptr;
	_ptr = (PFN_WGLGETGPUIDSAMD)_getPrivateProcAddress("wglGetGPUIDsAMD");
	if (!_ptr) {
		_ptr = &_fail_wglGetGPUIDsAMD;
	}
	_wglGetGPUIDsAMD = _ptr;
	return _wglGetGPUIDsAMD(maxCount, ids);
}

PFN_WGLGETGPUIDSAMD _wglGetGPUIDsAMD = &_get_wglGetGPUIDsAMD;

static INT __stdcall _fail_wglGetGPUInfoAMD(UINT id, int property, GLenum dataType, UINT size, void * data) {
	const char *_name = "wglGetGPUInfoAMD";
	return false;
}

static INT __stdcall _get_wglGetGPUInfoAMD(UINT id, int property, GLenum dataType, UINT size, void * data) {
	PFN_WGLGETGPUINFOAMD _ptr;
	_ptr = (PFN_WGLGETGPUINFOAMD)_getPrivateProcAddress("wglGetGPUInfoAMD");
	if (!_ptr) {
		_ptr = &_fail_wglGetGPUInfoAMD;
	}
	_wglGetGPUInfoAMD = _ptr;
	return _wglGetGPUInfoAMD(id, property, dataType, size, data);
}

PFN_WGLGETGPUINFOAMD _wglGetGPUInfoAMD = &_get_wglGetGPUInfoAMD;

static UINT __stdcall _fail_wglGetContextGPUIDAMD(HGLRC hglrc) {
	const char *_name = "wglGetContextGPUIDAMD";
	return false;
}

static UINT __stdcall _get_wglGetContextGPUIDAMD(HGLRC hglrc) {
	PFN_WGLGETCONTEXTGPUIDAMD _ptr;
	_ptr = (PFN_WGLGETCONTEXTGPUIDAMD)_getPrivateProcAddress("wglGetContextGPUIDAMD");
	if (!_ptr) {
		_ptr = &_fail_wglGetContextGPUIDAMD;
	}
	_wglGetContextGPUIDAMD = _ptr;
	return _wglGetContextGPUIDAMD(hglrc);
}

PFN_WGLGETCONTEXTGPUIDAMD _wglGetContextGPUIDAMD = &_get_wglGetContextGPUIDAMD;

static HGLRC __stdcall _fail_wglCreateAssociatedContextAMD(UINT id) {
	const char *_name = "wglCreateAssociatedContextAMD";
	return false;
}

static HGLRC __stdcall _get_wglCreateAssociatedContextAMD(UINT id) {
	PFN_WGLCREATEASSOCIATEDCONTEXTAMD _ptr;
	_ptr = (PFN_WGLCREATEASSOCIATEDCONTEXTAMD)_getPrivateProcAddress("wglCreateAssociatedContextAMD");
	if (!_ptr) {
		_ptr = &_fail_wglCreateAssociatedContextAMD;
	}
	_wglCreateAssociatedContextAMD = _ptr;
	return _wglCreateAssociatedContextAMD(id);
}

PFN_WGLCREATEASSOCIATEDCONTEXTAMD _wglCreateAssociatedContextAMD = &_get_wglCreateAssociatedContextAMD;

static HGLRC __stdcall _fail_wglCreateAssociatedContextAttribsAMD(UINT id, HGLRC hShareContext, const int * attribList) {
	const char *_name = "wglCreateAssociatedContextAttribsAMD";
	return false;
}

static HGLRC __stdcall _get_wglCreateAssociatedContextAttribsAMD(UINT id, HGLRC hShareContext, const int * attribList) {
	PFN_WGLCREATEASSOCIATEDCONTEXTATTRIBSAMD _ptr;
	_ptr = (PFN_WGLCREATEASSOCIATEDCONTEXTATTRIBSAMD)_getPrivateProcAddress("wglCreateAssociatedContextAttribsAMD");
	if (!_ptr) {
		_ptr = &_fail_wglCreateAssociatedContextAttribsAMD;
	}
	_wglCreateAssociatedContextAttribsAMD = _ptr;
	return _wglCreateAssociatedContextAttribsAMD(id, hShareContext, attribList);
}

PFN_WGLCREATEASSOCIATEDCONTEXTATTRIBSAMD _wglCreateAssociatedContextAttribsAMD = &_get_wglCreateAssociatedContextAttribsAMD;

static BOOL __stdcall _fail_wglDeleteAssociatedContextAMD(HGLRC hglrc) {
	const char *_name = "wglDeleteAssociatedContextAMD";
	return false;
}

static BOOL __stdcall _get_wglDeleteAssociatedContextAMD(HGLRC hglrc) {
	PFN_WGLDELETEASSOCIATEDCONTEXTAMD _ptr;
	_ptr = (PFN_WGLDELETEASSOCIATEDCONTEXTAMD)_getPrivateProcAddress("wglDeleteAssociatedContextAMD");
	if (!_ptr) {
		_ptr = &_fail_wglDeleteAssociatedContextAMD;
	}
	_wglDeleteAssociatedContextAMD = _ptr;
	return _wglDeleteAssociatedContextAMD(hglrc);
}

PFN_WGLDELETEASSOCIATEDCONTEXTAMD _wglDeleteAssociatedContextAMD = &_get_wglDeleteAssociatedContextAMD;

static BOOL __stdcall _fail_wglMakeAssociatedContextCurrentAMD(HGLRC hglrc) {
	const char *_name = "wglMakeAssociatedContextCurrentAMD";
	return false;
}

static BOOL __stdcall _get_wglMakeAssociatedContextCurrentAMD(HGLRC hglrc) {
	PFN_WGLMAKEASSOCIATEDCONTEXTCURRENTAMD _ptr;
	_ptr = (PFN_WGLMAKEASSOCIATEDCONTEXTCURRENTAMD)_getPrivateProcAddress("wglMakeAssociatedContextCurrentAMD");
	if (!_ptr) {
		_ptr = &_fail_wglMakeAssociatedContextCurrentAMD;
	}
	_wglMakeAssociatedContextCurrentAMD = _ptr;
	return _wglMakeAssociatedContextCurrentAMD(hglrc);
}

PFN_WGLMAKEASSOCIATEDCONTEXTCURRENTAMD _wglMakeAssociatedContextCurrentAMD = &_get_wglMakeAssociatedContextCurrentAMD;

static HGLRC __stdcall _fail_wglGetCurrentAssociatedContextAMD(void) {
	const char *_name = "wglGetCurrentAssociatedContextAMD";
	return false;
}

static HGLRC __stdcall _get_wglGetCurrentAssociatedContextAMD(void) {
	PFN_WGLGETCURRENTASSOCIATEDCONTEXTAMD _ptr;
	_ptr = (PFN_WGLGETCURRENTASSOCIATEDCONTEXTAMD)_getPrivateProcAddress("wglGetCurrentAssociatedContextAMD");
	if (!_ptr) {
		_ptr = &_fail_wglGetCurrentAssociatedContextAMD;
	}
	_wglGetCurrentAssociatedContextAMD = _ptr;
	return _wglGetCurrentAssociatedContextAMD();
}

PFN_WGLGETCURRENTASSOCIATEDCONTEXTAMD _wglGetCurrentAssociatedContextAMD = &_get_wglGetCurrentAssociatedContextAMD;

static void __stdcall _fail_wglBlitContextFramebufferAMD(HGLRC dstCtx, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) {
	const char *_name = "wglBlitContextFramebufferAMD";
	
	return;
}

static void __stdcall _get_wglBlitContextFramebufferAMD(HGLRC dstCtx, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) {
	PFN_WGLBLITCONTEXTFRAMEBUFFERAMD _ptr;
	_ptr = (PFN_WGLBLITCONTEXTFRAMEBUFFERAMD)_getPrivateProcAddress("wglBlitContextFramebufferAMD");
	if (!_ptr) {
		_ptr = &_fail_wglBlitContextFramebufferAMD;
	}
	_wglBlitContextFramebufferAMD = _ptr;
	_wglBlitContextFramebufferAMD(dstCtx, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

PFN_WGLBLITCONTEXTFRAMEBUFFERAMD _wglBlitContextFramebufferAMD = &_get_wglBlitContextFramebufferAMD;

static BOOL __stdcall _fail_wglBindVideoCaptureDeviceNV(UINT uVideoSlot, HVIDEOINPUTDEVICENV hDevice) {
	const char *_name = "wglBindVideoCaptureDeviceNV";
	return false;
}

static BOOL __stdcall _get_wglBindVideoCaptureDeviceNV(UINT uVideoSlot, HVIDEOINPUTDEVICENV hDevice) {
	PFN_WGLBINDVIDEOCAPTUREDEVICENV _ptr;
	_ptr = (PFN_WGLBINDVIDEOCAPTUREDEVICENV)_getPrivateProcAddress("wglBindVideoCaptureDeviceNV");
	if (!_ptr) {
		_ptr = &_fail_wglBindVideoCaptureDeviceNV;
	}
	_wglBindVideoCaptureDeviceNV = _ptr;
	return _wglBindVideoCaptureDeviceNV(uVideoSlot, hDevice);
}

PFN_WGLBINDVIDEOCAPTUREDEVICENV _wglBindVideoCaptureDeviceNV = &_get_wglBindVideoCaptureDeviceNV;

static UINT __stdcall _fail_wglEnumerateVideoCaptureDevicesNV(HDC hDc, HVIDEOINPUTDEVICENV * phDeviceList) {
	const char *_name = "wglEnumerateVideoCaptureDevicesNV";
	return false;
}

static UINT __stdcall _get_wglEnumerateVideoCaptureDevicesNV(HDC hDc, HVIDEOINPUTDEVICENV * phDeviceList) {
	PFN_WGLENUMERATEVIDEOCAPTUREDEVICESNV _ptr;
	_ptr = (PFN_WGLENUMERATEVIDEOCAPTUREDEVICESNV)_getPrivateProcAddress("wglEnumerateVideoCaptureDevicesNV");
	if (!_ptr) {
		_ptr = &_fail_wglEnumerateVideoCaptureDevicesNV;
	}
	_wglEnumerateVideoCaptureDevicesNV = _ptr;
	return _wglEnumerateVideoCaptureDevicesNV(hDc, phDeviceList);
}

PFN_WGLENUMERATEVIDEOCAPTUREDEVICESNV _wglEnumerateVideoCaptureDevicesNV = &_get_wglEnumerateVideoCaptureDevicesNV;

static BOOL __stdcall _fail_wglLockVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice) {
	const char *_name = "wglLockVideoCaptureDeviceNV";
	return false;
}

static BOOL __stdcall _get_wglLockVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice) {
	PFN_WGLLOCKVIDEOCAPTUREDEVICENV _ptr;
	_ptr = (PFN_WGLLOCKVIDEOCAPTUREDEVICENV)_getPrivateProcAddress("wglLockVideoCaptureDeviceNV");
	if (!_ptr) {
		_ptr = &_fail_wglLockVideoCaptureDeviceNV;
	}
	_wglLockVideoCaptureDeviceNV = _ptr;
	return _wglLockVideoCaptureDeviceNV(hDc, hDevice);
}

PFN_WGLLOCKVIDEOCAPTUREDEVICENV _wglLockVideoCaptureDeviceNV = &_get_wglLockVideoCaptureDeviceNV;

static BOOL __stdcall _fail_wglQueryVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice, int iAttribute, int * piValue) {
	const char *_name = "wglQueryVideoCaptureDeviceNV";
	return false;
}

static BOOL __stdcall _get_wglQueryVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice, int iAttribute, int * piValue) {
	PFN_WGLQUERYVIDEOCAPTUREDEVICENV _ptr;
	_ptr = (PFN_WGLQUERYVIDEOCAPTUREDEVICENV)_getPrivateProcAddress("wglQueryVideoCaptureDeviceNV");
	if (!_ptr) {
		_ptr = &_fail_wglQueryVideoCaptureDeviceNV;
	}
	_wglQueryVideoCaptureDeviceNV = _ptr;
	return _wglQueryVideoCaptureDeviceNV(hDc, hDevice, iAttribute, piValue);
}

PFN_WGLQUERYVIDEOCAPTUREDEVICENV _wglQueryVideoCaptureDeviceNV = &_get_wglQueryVideoCaptureDeviceNV;

static BOOL __stdcall _fail_wglReleaseVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice) {
	const char *_name = "wglReleaseVideoCaptureDeviceNV";
	return false;
}

static BOOL __stdcall _get_wglReleaseVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice) {
	PFN_WGLRELEASEVIDEOCAPTUREDEVICENV _ptr;
	_ptr = (PFN_WGLRELEASEVIDEOCAPTUREDEVICENV)_getPrivateProcAddress("wglReleaseVideoCaptureDeviceNV");
	if (!_ptr) {
		_ptr = &_fail_wglReleaseVideoCaptureDeviceNV;
	}
	_wglReleaseVideoCaptureDeviceNV = _ptr;
	return _wglReleaseVideoCaptureDeviceNV(hDc, hDevice);
}

PFN_WGLRELEASEVIDEOCAPTUREDEVICENV _wglReleaseVideoCaptureDeviceNV = &_get_wglReleaseVideoCaptureDeviceNV;

static BOOL __stdcall _fail_wglCopyImageSubDataNV(HGLRC hSrcRC, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, HGLRC hDstRC, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth) {
	const char *_name = "wglCopyImageSubDataNV";
	return false;
}

static BOOL __stdcall _get_wglCopyImageSubDataNV(HGLRC hSrcRC, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, HGLRC hDstRC, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth) {
	PFN_WGLCOPYIMAGESUBDATANV _ptr;
	_ptr = (PFN_WGLCOPYIMAGESUBDATANV)_getPrivateProcAddress("wglCopyImageSubDataNV");
	if (!_ptr) {
		_ptr = &_fail_wglCopyImageSubDataNV;
	}
	_wglCopyImageSubDataNV = _ptr;
	return _wglCopyImageSubDataNV(hSrcRC, srcName, srcTarget, srcLevel, srcX, srcY, srcZ, hDstRC, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, width, height, depth);
}

PFN_WGLCOPYIMAGESUBDATANV _wglCopyImageSubDataNV = &_get_wglCopyImageSubDataNV;

static BOOL __stdcall _fail_wglDXSetResourceShareHandleNV(void * dxObject, HANDLE shareHandle) {
	const char *_name = "wglDXSetResourceShareHandleNV";
	return false;
}

static BOOL __stdcall _get_wglDXSetResourceShareHandleNV(void * dxObject, HANDLE shareHandle) {
	PFN_WGLDXSETRESOURCESHAREHANDLENV _ptr;
	_ptr = (PFN_WGLDXSETRESOURCESHAREHANDLENV)_getPrivateProcAddress("wglDXSetResourceShareHandleNV");
	if (!_ptr) {
		_ptr = &_fail_wglDXSetResourceShareHandleNV;
	}
	_wglDXSetResourceShareHandleNV = _ptr;
	return _wglDXSetResourceShareHandleNV(dxObject, shareHandle);
}

PFN_WGLDXSETRESOURCESHAREHANDLENV _wglDXSetResourceShareHandleNV = &_get_wglDXSetResourceShareHandleNV;

static HANDLE __stdcall _fail_wglDXOpenDeviceNV(void * dxDevice) {
	const char *_name = "wglDXOpenDeviceNV";
	return false;
}

static HANDLE __stdcall _get_wglDXOpenDeviceNV(void * dxDevice) {
	PFN_WGLDXOPENDEVICENV _ptr;
	_ptr = (PFN_WGLDXOPENDEVICENV)_getPrivateProcAddress("wglDXOpenDeviceNV");
	if (!_ptr) {
		_ptr = &_fail_wglDXOpenDeviceNV;
	}
	_wglDXOpenDeviceNV = _ptr;
	return _wglDXOpenDeviceNV(dxDevice);
}

PFN_WGLDXOPENDEVICENV _wglDXOpenDeviceNV = &_get_wglDXOpenDeviceNV;

static BOOL __stdcall _fail_wglDXCloseDeviceNV(HANDLE hDevice) {
	const char *_name = "wglDXCloseDeviceNV";
	return false;
}

static BOOL __stdcall _get_wglDXCloseDeviceNV(HANDLE hDevice) {
	PFN_WGLDXCLOSEDEVICENV _ptr;
	_ptr = (PFN_WGLDXCLOSEDEVICENV)_getPrivateProcAddress("wglDXCloseDeviceNV");
	if (!_ptr) {
		_ptr = &_fail_wglDXCloseDeviceNV;
	}
	_wglDXCloseDeviceNV = _ptr;
	return _wglDXCloseDeviceNV(hDevice);
}

PFN_WGLDXCLOSEDEVICENV _wglDXCloseDeviceNV = &_get_wglDXCloseDeviceNV;

static HANDLE __stdcall _fail_wglDXRegisterObjectNV(HANDLE hDevice, void * dxObject, GLuint name, GLenum type, GLenum access) {
	const char *_name = "wglDXRegisterObjectNV";
	return false;
}

static HANDLE __stdcall _get_wglDXRegisterObjectNV(HANDLE hDevice, void * dxObject, GLuint name, GLenum type, GLenum access) {
	PFN_WGLDXREGISTEROBJECTNV _ptr;
	_ptr = (PFN_WGLDXREGISTEROBJECTNV)_getPrivateProcAddress("wglDXRegisterObjectNV");
	if (!_ptr) {
		_ptr = &_fail_wglDXRegisterObjectNV;
	}
	_wglDXRegisterObjectNV = _ptr;
	return _wglDXRegisterObjectNV(hDevice, dxObject, name, type, access);
}

PFN_WGLDXREGISTEROBJECTNV _wglDXRegisterObjectNV = &_get_wglDXRegisterObjectNV;

static BOOL __stdcall _fail_wglDXUnregisterObjectNV(HANDLE hDevice, HANDLE hObject) {
	const char *_name = "wglDXUnregisterObjectNV";
	return false;
}

static BOOL __stdcall _get_wglDXUnregisterObjectNV(HANDLE hDevice, HANDLE hObject) {
	PFN_WGLDXUNREGISTEROBJECTNV _ptr;
	_ptr = (PFN_WGLDXUNREGISTEROBJECTNV)_getPrivateProcAddress("wglDXUnregisterObjectNV");
	if (!_ptr) {
		_ptr = &_fail_wglDXUnregisterObjectNV;
	}
	_wglDXUnregisterObjectNV = _ptr;
	return _wglDXUnregisterObjectNV(hDevice, hObject);
}

PFN_WGLDXUNREGISTEROBJECTNV _wglDXUnregisterObjectNV = &_get_wglDXUnregisterObjectNV;

static BOOL __stdcall _fail_wglDXObjectAccessNV(HANDLE hObject, GLenum access) {
	const char *_name = "wglDXObjectAccessNV";
	return false;
}

static BOOL __stdcall _get_wglDXObjectAccessNV(HANDLE hObject, GLenum access) {
	PFN_WGLDXOBJECTACCESSNV _ptr;
	_ptr = (PFN_WGLDXOBJECTACCESSNV)_getPrivateProcAddress("wglDXObjectAccessNV");
	if (!_ptr) {
		_ptr = &_fail_wglDXObjectAccessNV;
	}
	_wglDXObjectAccessNV = _ptr;
	return _wglDXObjectAccessNV(hObject, access);
}

PFN_WGLDXOBJECTACCESSNV _wglDXObjectAccessNV = &_get_wglDXObjectAccessNV;

static BOOL __stdcall _fail_wglDXLockObjectsNV(HANDLE hDevice, GLint count, HANDLE * hObjects) {
	const char *_name = "wglDXLockObjectsNV";
	return false;
}

static BOOL __stdcall _get_wglDXLockObjectsNV(HANDLE hDevice, GLint count, HANDLE * hObjects) {
	PFN_WGLDXLOCKOBJECTSNV _ptr;
	_ptr = (PFN_WGLDXLOCKOBJECTSNV)_getPrivateProcAddress("wglDXLockObjectsNV");
	if (!_ptr) {
		_ptr = &_fail_wglDXLockObjectsNV;
	}
	_wglDXLockObjectsNV = _ptr;
	return _wglDXLockObjectsNV(hDevice, count, hObjects);
}

PFN_WGLDXLOCKOBJECTSNV _wglDXLockObjectsNV = &_get_wglDXLockObjectsNV;

static BOOL __stdcall _fail_wglDXUnlockObjectsNV(HANDLE hDevice, GLint count, HANDLE * hObjects) {
	const char *_name = "wglDXUnlockObjectsNV";
	return false;
}

static BOOL __stdcall _get_wglDXUnlockObjectsNV(HANDLE hDevice, GLint count, HANDLE * hObjects) {
	PFN_WGLDXUNLOCKOBJECTSNV _ptr;
	_ptr = (PFN_WGLDXUNLOCKOBJECTSNV)_getPrivateProcAddress("wglDXUnlockObjectsNV");
	if (!_ptr) {
		_ptr = &_fail_wglDXUnlockObjectsNV;
	}
	_wglDXUnlockObjectsNV = _ptr;
	return _wglDXUnlockObjectsNV(hDevice, count, hObjects);
}

PFN_WGLDXUNLOCKOBJECTSNV _wglDXUnlockObjectsNV = &_get_wglDXUnlockObjectsNV;

static BOOL __stdcall _fail_wglDelayBeforeSwapNV(HDC hDC, GLfloat seconds) {
	const char *_name = "wglDelayBeforeSwapNV";
	return false;
}

static BOOL __stdcall _get_wglDelayBeforeSwapNV(HDC hDC, GLfloat seconds) {
	PFN_WGLDELAYBEFORESWAPNV _ptr;
	_ptr = (PFN_WGLDELAYBEFORESWAPNV)_getPrivateProcAddress("wglDelayBeforeSwapNV");
	if (!_ptr) {
		_ptr = &_fail_wglDelayBeforeSwapNV;
	}
	_wglDelayBeforeSwapNV = _ptr;
	return _wglDelayBeforeSwapNV(hDC, seconds);
}

PFN_WGLDELAYBEFORESWAPNV _wglDelayBeforeSwapNV = &_get_wglDelayBeforeSwapNV;

static void APIENTRY _fail_glGetIntegerv(GLenum pname, GLint * params) {
	const char *_name = "glGetIntegerv";
	
	return;
}

static void APIENTRY _get_glGetIntegerv(GLenum pname, GLint * params) {
	PFN_GLGETINTEGERV _ptr;
	_ptr = (PFN_GLGETINTEGERV)_getPublicProcAddress("glGetIntegerv");
	if (!_ptr) {
		_ptr = &_fail_glGetIntegerv;
	}
	_glGetIntegerv = _ptr;
	_glGetIntegerv(pname, params);
}

PFN_GLGETINTEGERV _glGetIntegerv = &_get_glGetIntegerv;

static void APIENTRY _fail_glGetBooleanv(GLenum pname, GLboolean * params) {
	const char *_name = "glGetBooleanv";
	
	return;
}

static void APIENTRY _get_glGetBooleanv(GLenum pname, GLboolean * params) {
	PFN_GLGETBOOLEANV _ptr;
	_ptr = (PFN_GLGETBOOLEANV)_getPublicProcAddress("glGetBooleanv");
	if (!_ptr) {
		_ptr = &_fail_glGetBooleanv;
	}
	_glGetBooleanv = _ptr;
	_glGetBooleanv(pname, params);
}

PFN_GLGETBOOLEANV _glGetBooleanv = &_get_glGetBooleanv;

static void APIENTRY _fail_glDisable(GLenum cap) {
	const char *_name = "glDisable";
	
	return;
}

static void APIENTRY _get_glDisable(GLenum cap) {
	PFN_GLDISABLE _ptr;
	_ptr = (PFN_GLDISABLE)_getPublicProcAddress("glDisable");
	if (!_ptr) {
		_ptr = &_fail_glDisable;
	}
	_glDisable = _ptr;
	_glDisable(cap);
}

PFN_GLDISABLE _glDisable = &_get_glDisable;

static void APIENTRY _fail_glVertex2d(GLdouble x, GLdouble y) {
	const char *_name = "glVertex2d";
	
	return;
}

static void APIENTRY _get_glVertex2d(GLdouble x, GLdouble y) {
	PFN_GLVERTEX2D _ptr;
	_ptr = (PFN_GLVERTEX2D)_getPublicProcAddress("glVertex2d");
	if (!_ptr) {
		_ptr = &_fail_glVertex2d;
	}
	_glVertex2d = _ptr;
	_glVertex2d(x, y);
}

PFN_GLVERTEX2D _glVertex2d = &_get_glVertex2d;

static void APIENTRY _fail_glCallList(GLuint list) {
	const char *_name = "glCallList";
	
	return;
}

static void APIENTRY _get_glCallList(GLuint list) {
	PFN_GLCALLLIST _ptr;
	_ptr = (PFN_GLCALLLIST)_getPublicProcAddress("glCallList");
	if (!_ptr) {
		_ptr = &_fail_glCallList;
	}
	_glCallList = _ptr;
	_glCallList(list);
}

PFN_GLCALLLIST _glCallList = &_get_glCallList;

static void APIENTRY _fail_glMaterialfv(GLenum face, GLenum pname, const GLfloat * params) {
	const char *_name = "glMaterialfv";
	
	return;
}

static void APIENTRY _get_glMaterialfv(GLenum face, GLenum pname, const GLfloat * params) {
	PFN_GLMATERIALFV _ptr;
	_ptr = (PFN_GLMATERIALFV)_getPublicProcAddress("glMaterialfv");
	if (!_ptr) {
		_ptr = &_fail_glMaterialfv;
	}
	_glMaterialfv = _ptr;
	_glMaterialfv(face, pname, params);
}

PFN_GLMATERIALFV _glMaterialfv = &_get_glMaterialfv;

static void APIENTRY _fail_glEnable(GLenum cap) {
	const char *_name = "glEnable";
	
	return;
}

static void APIENTRY _get_glEnable(GLenum cap) {
	PFN_GLENABLE _ptr;
	_ptr = (PFN_GLENABLE)_getPublicProcAddress("glEnable");
	if (!_ptr) {
		_ptr = &_fail_glEnable;
	}
	_glEnable = _ptr;
	_glEnable(cap);
}

PFN_GLENABLE _glEnable = &_get_glEnable;

static void APIENTRY _fail_glPopMatrix(void) {
	const char *_name = "glPopMatrix";
	
	return;
}

static void APIENTRY _get_glPopMatrix(void) {
	PFN_GLPOPMATRIX _ptr;
	_ptr = (PFN_GLPOPMATRIX)_getPublicProcAddress("glPopMatrix");
	if (!_ptr) {
		_ptr = &_fail_glPopMatrix;
	}
	_glPopMatrix = _ptr;
	_glPopMatrix();
}

PFN_GLPOPMATRIX _glPopMatrix = &_get_glPopMatrix;

static void APIENTRY _fail_glTexEnvi(GLenum target, GLenum pname, GLint param) {
	const char *_name = "glTexEnvi";
	
	return;
}

static void APIENTRY _get_glTexEnvi(GLenum target, GLenum pname, GLint param) {
	PFN_GLTEXENVI _ptr;
	_ptr = (PFN_GLTEXENVI)_getPublicProcAddress("glTexEnvi");
	if (!_ptr) {
		_ptr = &_fail_glTexEnvi;
	}
	_glTexEnvi = _ptr;
	_glTexEnvi(target, pname, param);
}

PFN_GLTEXENVI _glTexEnvi = &_get_glTexEnvi;

static void APIENTRY _fail_glBindTexture(GLenum target, GLuint texture) {
	const char *_name = "glBindTexture";
	
	return;
}

static void APIENTRY _get_glBindTexture(GLenum target, GLuint texture) {
	PFN_GLBINDTEXTURE _ptr;
	_ptr = (PFN_GLBINDTEXTURE)_getPublicProcAddress("glBindTexture");
	if (!_ptr) {
		_ptr = &_fail_glBindTexture;
	}
	_glBindTexture = _ptr;
	_glBindTexture(target, texture);
}

PFN_GLBINDTEXTURE _glBindTexture = &_get_glBindTexture;

static void APIENTRY _fail_glVertex3d(GLdouble x, GLdouble y, GLdouble z) {
	const char *_name = "glVertex3d";
	
	return;
}

static void APIENTRY _get_glVertex3d(GLdouble x, GLdouble y, GLdouble z) {
	PFN_GLVERTEX3D _ptr;
	_ptr = (PFN_GLVERTEX3D)_getPublicProcAddress("glVertex3d");
	if (!_ptr) {
		_ptr = &_fail_glVertex3d;
	}
	_glVertex3d = _ptr;
	_glVertex3d(x, y, z);
}

PFN_GLVERTEX3D _glVertex3d = &_get_glVertex3d;

static void APIENTRY _fail_glTexCoord2d(GLdouble s, GLdouble t) {
	const char *_name = "glTexCoord2d";
	
	return;
}

static void APIENTRY _get_glTexCoord2d(GLdouble s, GLdouble t) {
	PFN_GLTEXCOORD2D _ptr;
	_ptr = (PFN_GLTEXCOORD2D)_getPublicProcAddress("glTexCoord2d");
	if (!_ptr) {
		_ptr = &_fail_glTexCoord2d;
	}
	_glTexCoord2d = _ptr;
	_glTexCoord2d(s, t);
}

PFN_GLTEXCOORD2D _glTexCoord2d = &_get_glTexCoord2d;

static void APIENTRY _fail_glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z) {
	const char *_name = "glRotated";
	
	return;
}

static void APIENTRY _get_glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z) {
	PFN_GLROTATED _ptr;
	_ptr = (PFN_GLROTATED)_getPublicProcAddress("glRotated");
	if (!_ptr) {
		_ptr = &_fail_glRotated;
	}
	_glRotated = _ptr;
	_glRotated(angle, x, y, z);
}

PFN_GLROTATED _glRotated = &_get_glRotated;

static void APIENTRY _fail_glPushAttrib(GLbitfield mask) {
	const char *_name = "glPushAttrib";
	
	return;
}

static void APIENTRY _get_glPushAttrib(GLbitfield mask) {
	PFN_GLPUSHATTRIB _ptr;
	_ptr = (PFN_GLPUSHATTRIB)_getPublicProcAddress("glPushAttrib");
	if (!_ptr) {
		_ptr = &_fail_glPushAttrib;
	}
	_glPushAttrib = _ptr;
	_glPushAttrib(mask);
}

PFN_GLPUSHATTRIB _glPushAttrib = &_get_glPushAttrib;

static void APIENTRY _fail_glPopAttrib(void) {
	const char *_name = "glPopAttrib";
	
	return;
}

static void APIENTRY _get_glPopAttrib(void) {
	PFN_GLPOPATTRIB _ptr;
	_ptr = (PFN_GLPOPATTRIB)_getPublicProcAddress("glPopAttrib");
	if (!_ptr) {
		_ptr = &_fail_glPopAttrib;
	}
	_glPopAttrib = _ptr;
	_glPopAttrib();
}

PFN_GLPOPATTRIB _glPopAttrib = &_get_glPopAttrib;

static void APIENTRY _fail_glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
	const char *_name = "glColor4f";
	
	return;
}

static void APIENTRY _get_glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
	PFN_GLCOLOR4F _ptr;
	_ptr = (PFN_GLCOLOR4F)_getPublicProcAddress("glColor4f");
	if (!_ptr) {
		_ptr = &_fail_glColor4f;
	}
	_glColor4f = _ptr;
	_glColor4f(red, green, blue, alpha);
}

PFN_GLCOLOR4F _glColor4f = &_get_glColor4f;

static void APIENTRY _fail_glVertex2f(GLfloat x, GLfloat y) {
	const char *_name = "glVertex2f";
	
	return;
}

static void APIENTRY _get_glVertex2f(GLfloat x, GLfloat y) {
	PFN_GLVERTEX2F _ptr;
	_ptr = (PFN_GLVERTEX2F)_getPublicProcAddress("glVertex2f");
	if (!_ptr) {
		_ptr = &_fail_glVertex2f;
	}
	_glVertex2f = _ptr;
	_glVertex2f(x, y);
}

PFN_GLVERTEX2F _glVertex2f = &_get_glVertex2f;

static void APIENTRY _fail_glBlendFunc(GLenum sfactor, GLenum dfactor) {
	const char *_name = "glBlendFunc";
	
	return;
}

static void APIENTRY _get_glBlendFunc(GLenum sfactor, GLenum dfactor) {
	PFN_GLBLENDFUNC _ptr;
	_ptr = (PFN_GLBLENDFUNC)_getPublicProcAddress("glBlendFunc");
	if (!_ptr) {
		_ptr = &_fail_glBlendFunc;
	}
	_glBlendFunc = _ptr;
	_glBlendFunc(sfactor, dfactor);
}

PFN_GLBLENDFUNC _glBlendFunc = &_get_glBlendFunc;

static void APIENTRY _fail_glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar) {
	const char *_name = "glFrustum";
	
	return;
}

static void APIENTRY _get_glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar) {
	PFN_GLFRUSTUM _ptr;
	_ptr = (PFN_GLFRUSTUM)_getPublicProcAddress("glFrustum");
	if (!_ptr) {
		_ptr = &_fail_glFrustum;
	}
	_glFrustum = _ptr;
	_glFrustum(left, right, bottom, top, zNear, zFar);
}

PFN_GLFRUSTUM _glFrustum = &_get_glFrustum;

static void APIENTRY _fail_glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
	const char *_name = "glViewport";
	
	return;
}

static void APIENTRY _get_glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
	PFN_GLVIEWPORT _ptr;
	_ptr = (PFN_GLVIEWPORT)_getPublicProcAddress("glViewport");
	if (!_ptr) {
		_ptr = &_fail_glViewport;
	}
	_glViewport = _ptr;
	_glViewport(x, y, width, height);
}

PFN_GLVIEWPORT _glViewport = &_get_glViewport;

static void APIENTRY _fail_glMatrixMode(GLenum mode) {
	const char *_name = "glMatrixMode";
	
	return;
}

static void APIENTRY _get_glMatrixMode(GLenum mode) {
	PFN_GLMATRIXMODE _ptr;
	_ptr = (PFN_GLMATRIXMODE)_getPublicProcAddress("glMatrixMode");
	if (!_ptr) {
		_ptr = &_fail_glMatrixMode;
	}
	_glMatrixMode = _ptr;
	_glMatrixMode(mode);
}

PFN_GLMATRIXMODE _glMatrixMode = &_get_glMatrixMode;

static void APIENTRY _fail_glCullFace(GLenum mode) {
	const char *_name = "glCullFace";
	
	return;
}

static void APIENTRY _get_glCullFace(GLenum mode) {
	PFN_GLCULLFACE _ptr;
	_ptr = (PFN_GLCULLFACE)_getPublicProcAddress("glCullFace");
	if (!_ptr) {
		_ptr = &_fail_glCullFace;
	}
	_glCullFace = _ptr;
	_glCullFace(mode);
}

PFN_GLCULLFACE _glCullFace = &_get_glCullFace;

static void APIENTRY _fail_glPolygonMode(GLenum face, GLenum mode) {
	const char *_name = "glPolygonMode";
	
	return;
}

static void APIENTRY _get_glPolygonMode(GLenum face, GLenum mode) {
	PFN_GLPOLYGONMODE _ptr;
	_ptr = (PFN_GLPOLYGONMODE)_getPublicProcAddress("glPolygonMode");
	if (!_ptr) {
		_ptr = &_fail_glPolygonMode;
	}
	_glPolygonMode = _ptr;
	_glPolygonMode(face, mode);
}

PFN_GLPOLYGONMODE _glPolygonMode = &_get_glPolygonMode;

static void APIENTRY _fail_glScalef(GLfloat x, GLfloat y, GLfloat z) {
	const char *_name = "glScalef";
	
	return;
}

static void APIENTRY _get_glScalef(GLfloat x, GLfloat y, GLfloat z) {
	PFN_GLSCALEF _ptr;
	_ptr = (PFN_GLSCALEF)_getPublicProcAddress("glScalef");
	if (!_ptr) {
		_ptr = &_fail_glScalef;
	}
	_glScalef = _ptr;
	_glScalef(x, y, z);
}

PFN_GLSCALEF _glScalef = &_get_glScalef;

static void APIENTRY _fail_glFogi(GLenum pname, GLint param) {
	const char *_name = "glFogi";
	
	return;
}

static void APIENTRY _get_glFogi(GLenum pname, GLint param) {
	PFN_GLFOGI _ptr;
	_ptr = (PFN_GLFOGI)_getPublicProcAddress("glFogi");
	if (!_ptr) {
		_ptr = &_fail_glFogi;
	}
	_glFogi = _ptr;
	_glFogi(pname, param);
}

PFN_GLFOGI _glFogi = &_get_glFogi;

static void APIENTRY _fail_glHint(GLenum target, GLenum mode) {
	const char *_name = "glHint";
	
	return;
}

static void APIENTRY _get_glHint(GLenum target, GLenum mode) {
	PFN_GLHINT _ptr;
	_ptr = (PFN_GLHINT)_getPublicProcAddress("glHint");
	if (!_ptr) {
		_ptr = &_fail_glHint;
	}
	_glHint = _ptr;
	_glHint(target, mode);
}

PFN_GLHINT _glHint = &_get_glHint;

static void APIENTRY _fail_glFogfv(GLenum pname, const GLfloat * params) {
	const char *_name = "glFogfv";
	
	return;
}

static void APIENTRY _get_glFogfv(GLenum pname, const GLfloat * params) {
	PFN_GLFOGFV _ptr;
	_ptr = (PFN_GLFOGFV)_getPublicProcAddress("glFogfv");
	if (!_ptr) {
		_ptr = &_fail_glFogfv;
	}
	_glFogfv = _ptr;
	_glFogfv(pname, params);
}

PFN_GLFOGFV _glFogfv = &_get_glFogfv;

static void APIENTRY _fail_glFogf(GLenum pname, GLfloat param) {
	const char *_name = "glFogf";
	
	return;
}

static void APIENTRY _get_glFogf(GLenum pname, GLfloat param) {
	PFN_GLFOGF _ptr;
	_ptr = (PFN_GLFOGF)_getPublicProcAddress("glFogf");
	if (!_ptr) {
		_ptr = &_fail_glFogf;
	}
	_glFogf = _ptr;
	_glFogf(pname, param);
}

PFN_GLFOGF _glFogf = &_get_glFogf;

static void APIENTRY _fail_glColorMaterial(GLenum face, GLenum mode) {
	const char *_name = "glColorMaterial";
	
	return;
}

static void APIENTRY _get_glColorMaterial(GLenum face, GLenum mode) {
	PFN_GLCOLORMATERIAL _ptr;
	_ptr = (PFN_GLCOLORMATERIAL)_getPublicProcAddress("glColorMaterial");
	if (!_ptr) {
		_ptr = &_fail_glColorMaterial;
	}
	_glColorMaterial = _ptr;
	_glColorMaterial(face, mode);
}

PFN_GLCOLORMATERIAL _glColorMaterial = &_get_glColorMaterial;

static void APIENTRY _fail_glLightModelfv(GLenum pname, const GLfloat * params) {
	const char *_name = "glLightModelfv";
	
	return;
}

static void APIENTRY _get_glLightModelfv(GLenum pname, const GLfloat * params) {
	PFN_GLLIGHTMODELFV _ptr;
	_ptr = (PFN_GLLIGHTMODELFV)_getPublicProcAddress("glLightModelfv");
	if (!_ptr) {
		_ptr = &_fail_glLightModelfv;
	}
	_glLightModelfv = _ptr;
	_glLightModelfv(pname, params);
}

PFN_GLLIGHTMODELFV _glLightModelfv = &_get_glLightModelfv;

static void APIENTRY _fail_glGetFloatv(GLenum pname, GLfloat * params) {
	const char *_name = "glGetFloatv";

	return;
}

static void APIENTRY _get_glGetFloatv(GLenum pname, GLfloat * params) {
	PFN_GLGETFLOATV _ptr;
	_ptr = (PFN_GLGETFLOATV)_getPublicProcAddress("glGetFloatv");
	if (!_ptr) {
		_ptr = &_fail_glGetFloatv;
	}
	_glGetFloatv = _ptr;
	_glGetFloatv(pname, params);
}

PFN_GLGETFLOATV _glGetFloatv = &_get_glGetFloatv;

static void APIENTRY _fail_glBindBufferARB(GLenum target, GLuint buffer) {
	const char *_name = "glBindBufferARB";
	
	return;
}

static void APIENTRY _get_glBindBufferARB(GLenum target, GLuint buffer) {
	PFN_GLBINDBUFFERARB _ptr;
	_ptr = (PFN_GLBINDBUFFERARB)_getPrivateProcAddress("glBindBufferARB");
	if (!_ptr) {
		_ptr = &_fail_glBindBufferARB;
	}
	_glBindBufferARB = _ptr;
	_glBindBufferARB(target, buffer);
}

PFN_GLBINDBUFFERARB _glBindBufferARB = &_get_glBindBufferARB;

static void APIENTRY _fail_glInterleavedArrays(GLenum format, GLsizei stride, const GLvoid * pointer) {
	const char *_name = "glInterleavedArrays";
	
	return;
}

static void APIENTRY _get_glInterleavedArrays(GLenum format, GLsizei stride, const GLvoid * pointer) {
	PFN_GLINTERLEAVEDARRAYS _ptr;
	_ptr = (PFN_GLINTERLEAVEDARRAYS)_getPublicProcAddress("glInterleavedArrays");
	if (!_ptr) {
		_ptr = &_fail_glInterleavedArrays;
	}
	_glInterleavedArrays = _ptr;
	_glInterleavedArrays(format, stride, pointer);
}

PFN_GLINTERLEAVEDARRAYS _glInterleavedArrays = &_get_glInterleavedArrays;

static void APIENTRY _fail_glDrawArrays(GLenum mode, GLint first, GLsizei count) {
	const char *_name = "glDrawArrays";
	
	return;
}

static void APIENTRY _get_glDrawArrays(GLenum mode, GLint first, GLsizei count) {
	PFN_GLDRAWARRAYS _ptr;
	_ptr = (PFN_GLDRAWARRAYS)_getPublicProcAddress("glDrawArrays");
	if (!_ptr) {
		_ptr = &_fail_glDrawArrays;
	}
	_glDrawArrays = _ptr;
	_glDrawArrays(mode, first, count);
}

PFN_GLDRAWARRAYS _glDrawArrays = &_get_glDrawArrays;

static void APIENTRY _fail_glUseProgram(GLuint program) {
	const char *_name = "glUseProgram";
	
	return;
}

static void APIENTRY _get_glUseProgram(GLuint program) {
	PFN_GLUSEPROGRAM _ptr;
	_ptr = (PFN_GLUSEPROGRAM)_getPrivateProcAddress("glUseProgram");
	if (!_ptr) {
		_ptr = &_fail_glUseProgram;
	}
	_glUseProgram = _ptr;
	_glUseProgram(program);
}

PFN_GLUSEPROGRAM _glUseProgram = &_get_glUseProgram;

static GLint APIENTRY _fail_glGetUniformLocationARB(GLhandleARB programObj, const GLcharARB * name) {
	const char *_name = "glGetUniformLocationARB";
	
	return 0;
}

static GLint APIENTRY _get_glGetUniformLocationARB(GLhandleARB programObj, const GLcharARB * name) {
	PFN_GLGETUNIFORMLOCATIONARB _ptr;
	_ptr = (PFN_GLGETUNIFORMLOCATIONARB)_getPrivateProcAddress("glGetUniformLocationARB");
	if (!_ptr) {
		_ptr = &_fail_glGetUniformLocationARB;
	}
	_glGetUniformLocationARB = _ptr;
	return _glGetUniformLocationARB(programObj, name);
}

PFN_GLGETUNIFORMLOCATIONARB _glGetUniformLocationARB = &_get_glGetUniformLocationARB;

static void APIENTRY _fail_glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
	const char *_name = "glUniform4f";
	
	return;
}

static void APIENTRY _get_glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
	PFN_GLUNIFORM4F _ptr;
	_ptr = (PFN_GLUNIFORM4F)_getPrivateProcAddress("glUniform4f");
	if (!_ptr) {
		_ptr = &_fail_glUniform4f;
	}
	_glUniform4f = _ptr;
	_glUniform4f(location, v0, v1, v2, v3);
}

PFN_GLUNIFORM4F _glUniform4f = &_get_glUniform4f;

static void APIENTRY _fail_glUniform1f(GLint location, GLfloat v0) {
	const char *_name = "glUniform1f";
	
	return;
}

static void APIENTRY _get_glUniform1f(GLint location, GLfloat v0) {
	PFN_GLUNIFORM1F _ptr;
	_ptr = (PFN_GLUNIFORM1F)_getPrivateProcAddress("glUniform1f");
	if (!_ptr) {
		_ptr = &_fail_glUniform1f;
	}
	_glUniform1f = _ptr;
	_glUniform1f(location, v0);
}

PFN_GLUNIFORM1F _glUniform1f = &_get_glUniform1f;

static void APIENTRY _fail_glUniform1i(GLint location, GLint v0) {
	const char *_name = "glUniform1i";
	
	return;
}

static void APIENTRY _get_glUniform1i(GLint location, GLint v0) {
	PFN_GLUNIFORM1I _ptr;
	_ptr = (PFN_GLUNIFORM1I)_getPrivateProcAddress("glUniform1i");
	if (!_ptr) {
		_ptr = &_fail_glUniform1i;
	}
	_glUniform1i = _ptr;
	_glUniform1i(location, v0);
}

PFN_GLUNIFORM1I _glUniform1i = &_get_glUniform1i;

static void APIENTRY _fail_glActiveTextureARB(GLenum texture) {
	const char *_name = "glActiveTextureARB";
	
	return;
}

static void APIENTRY _get_glActiveTextureARB(GLenum texture) {
	PFN_GLACTIVETEXTUREARB _ptr;
	_ptr = (PFN_GLACTIVETEXTUREARB)_getPrivateProcAddress("glActiveTextureARB");
	if (!_ptr) {
		_ptr = &_fail_glActiveTextureARB;
	}
	_glActiveTextureARB = _ptr;
	_glActiveTextureARB(texture);
}

PFN_GLACTIVETEXTUREARB _glActiveTextureARB = &_get_glActiveTextureARB;

static void APIENTRY _fail_glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t) {
	const char *_name = "glMultiTexCoord2fARB";
	
	return;
}

static void APIENTRY _get_glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t) {
	PFN_GLMULTITEXCOORD2FARB _ptr;
	_ptr = (PFN_GLMULTITEXCOORD2FARB)_getPrivateProcAddress("glMultiTexCoord2fARB");
	if (!_ptr) {
		_ptr = &_fail_glMultiTexCoord2fARB;
	}
	_glMultiTexCoord2fARB = _ptr;
	_glMultiTexCoord2fARB(target, s, t);
}

PFN_GLMULTITEXCOORD2FARB _glMultiTexCoord2fARB = &_get_glMultiTexCoord2fARB;

static void APIENTRY _fail_glDepthMask(GLboolean flag) {
	const char *_name = "glDepthMask";
	
	return;
}

static void APIENTRY _get_glDepthMask(GLboolean flag) {
	PFN_GLDEPTHMASK _ptr;
	_ptr = (PFN_GLDEPTHMASK)_getPublicProcAddress("glDepthMask");
	if (!_ptr) {
		_ptr = &_fail_glDepthMask;
	}
	_glDepthMask = _ptr;
	_glDepthMask(flag);
}

PFN_GLDEPTHMASK _glDepthMask = &_get_glDepthMask;

static void APIENTRY _fail_glEnableClientState(GLenum array) {
	const char *_name = "glEnableClientState";
	
	return;
}

static void APIENTRY _get_glEnableClientState(GLenum array) {
	PFN_GLENABLECLIENTSTATE _ptr;
	_ptr = (PFN_GLENABLECLIENTSTATE)_getPublicProcAddress("glEnableClientState");
	if (!_ptr) {
		_ptr = &_fail_glEnableClientState;
	}
	_glEnableClientState = _ptr;
	_glEnableClientState(array);
}

PFN_GLENABLECLIENTSTATE _glEnableClientState = &_get_glEnableClientState;

static void APIENTRY _fail_glPointParameterfvARB(GLenum pname, const GLfloat * params) {
	const char *_name = "glPointParameterfvARB";
	
	return;
}

static void APIENTRY _get_glPointParameterfvARB(GLenum pname, const GLfloat * params) {
	PFN_GLPOINTPARAMETERFVARB _ptr;
	_ptr = (PFN_GLPOINTPARAMETERFVARB)_getPrivateProcAddress("glPointParameterfvARB");
	if (!_ptr) {
		_ptr = &_fail_glPointParameterfvARB;
	}
	_glPointParameterfvARB = _ptr;
	_glPointParameterfvARB(pname, params);
}

PFN_GLPOINTPARAMETERFVARB _glPointParameterfvARB = &_get_glPointParameterfvARB;

static void APIENTRY _fail_glPointParameterfARB(GLenum pname, GLfloat param) {
	const char *_name = "glPointParameterfARB";

	return;
}

static void APIENTRY _get_glPointParameterfARB(GLenum pname, GLfloat param) {
	PFN_GLPOINTPARAMETERFARB _ptr;
	_ptr = (PFN_GLPOINTPARAMETERFARB)_getPrivateProcAddress("glPointParameterfARB");
	if (!_ptr) {
		_ptr = &_fail_glPointParameterfARB;
	}
	_glPointParameterfARB = _ptr;
	_glPointParameterfARB(pname, param);
}

PFN_GLPOINTPARAMETERFARB _glPointParameterfARB = &_get_glPointParameterfARB;

static void APIENTRY _fail_glPointSize(GLfloat size) {
	const char *_name = "glPointSize";
	
	return;
}

static void APIENTRY _get_glPointSize(GLfloat size) {
	PFN_GLPOINTSIZE _ptr;
	_ptr = (PFN_GLPOINTSIZE)_getPublicProcAddress("glPointSize");
	if (!_ptr) {
		_ptr = &_fail_glPointSize;
	}
	_glPointSize = _ptr;
	_glPointSize(size);
}

PFN_GLPOINTSIZE _glPointSize = &_get_glPointSize;

static void APIENTRY _fail_glTexEnvf(GLenum target, GLenum pname, GLfloat param) {
	const char *_name = "glTexEnvf";

	return;
}

static void APIENTRY _get_glTexEnvf(GLenum target, GLenum pname, GLfloat param) {
	PFN_GLTEXENVF _ptr;
	_ptr = (PFN_GLTEXENVF)_getPublicProcAddress("glTexEnvf");
	if (!_ptr) {
		_ptr = &_fail_glTexEnvf;
	}
	_glTexEnvf = _ptr;
	_glTexEnvf(target, pname, param);
}

PFN_GLTEXENVF _glTexEnvf = &_get_glTexEnvf;

static void APIENTRY _fail_glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer) {
	const char *_name = "glColorPointer";

	return;
}

static void APIENTRY _get_glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer) {
	PFN_GLCOLORPOINTER _ptr;
	_ptr = (PFN_GLCOLORPOINTER)_getPublicProcAddress("glColorPointer");
	if (!_ptr) {
		_ptr = &_fail_glColorPointer;
	}
	_glColorPointer = _ptr;
	_glColorPointer(size, type, stride, pointer);
}

PFN_GLCOLORPOINTER _glColorPointer = &_get_glColorPointer;

static void APIENTRY _fail_glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer) {
	const char *_name = "glVertexPointer";

	return;
}

static void APIENTRY _get_glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer) {
	PFN_GLVERTEXPOINTER _ptr;
	_ptr = (PFN_GLVERTEXPOINTER)_getPublicProcAddress("glVertexPointer");
	if (!_ptr) {
		_ptr = &_fail_glVertexPointer;
	}
	_glVertexPointer = _ptr;
	_glVertexPointer(size, type, stride, pointer);
}

PFN_GLVERTEXPOINTER _glVertexPointer = &_get_glVertexPointer;

static void APIENTRY _fail_glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha) {
	const char *_name = "glColor4ub";
	
	return;
}

static void APIENTRY _get_glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha) {
	PFN_GLCOLOR4UB _ptr;
	_ptr = (PFN_GLCOLOR4UB)_getPublicProcAddress("glColor4ub");
	if (!_ptr) {
		_ptr = &_fail_glColor4ub;
	}
	_glColor4ub = _ptr;
	_glColor4ub(red, green, blue, alpha);
}

PFN_GLCOLOR4UB _glColor4ub = &_get_glColor4ub;

static void APIENTRY _fail_glTexCoord2f(GLfloat s, GLfloat t) {
	const char *_name = "glTexCoord2f";
	
	return;
}

static void APIENTRY _get_glTexCoord2f(GLfloat s, GLfloat t) {
	PFN_GLTEXCOORD2F _ptr;
	_ptr = (PFN_GLTEXCOORD2F)_getPublicProcAddress("glTexCoord2f");
	if (!_ptr) {
		_ptr = &_fail_glTexCoord2f;
	}
	_glTexCoord2f = _ptr;
	_glTexCoord2f(s, t);
}

PFN_GLTEXCOORD2F _glTexCoord2f = &_get_glTexCoord2f;

static void APIENTRY _fail_glVertex2i(GLint x, GLint y) {
	const char *_name = "glVertex2i";
	
	return;
}

static void APIENTRY _get_glVertex2i(GLint x, GLint y) {
	PFN_GLVERTEX2I _ptr;
	_ptr = (PFN_GLVERTEX2I)_getPublicProcAddress("glVertex2i");
	if (!_ptr) {
		_ptr = &_fail_glVertex2i;
	}
	_glVertex2i = _ptr;
	_glVertex2i(x, y);
}

PFN_GLVERTEX2I _glVertex2i = &_get_glVertex2i;


/************************************************************************************/
int WINAPI interceptedwglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd)
{
	initialiseVariables();

	/************Just for now. Hard code the value of the screen************/
	widthScreen = 1920;
	heightScreen = 1080;
	/************************/

	//m_framesGL = 0;
	
	currentConfig3DSettings.readConfig3DSettingsFromFile();
	/************************/

	currentOpenGLContext = hdc; //store the current context
	//Log::open("intercept.log");
	Log::print("OK: get interceptedwglChoosePixelFormat function \n");
	//printf("interceptedwglChoosePixelFormat\n");

	if (!_wglChoosePixelFormat)
	{
		MessageBox(NULL, "_wglChoosePixelFormat not supported", "Error! (interceptedwglChoosePixelFormat)", MB_OK);
		return false;
	}

	//PIXELFORMATDESCRIPTOR newppfd = {};
	//newppfd.nSize = sizeof(newppfd);

	////copy all existing values
	//newppfd.cAccumAlphaBits = ppfd->cAccumAlphaBits;
	//newppfd.cAccumBits = ppfd->cAccumBits;
	//newppfd.cAccumBlueBits = ppfd->cAccumBlueBits;
	//newppfd.cAccumGreenBits = ppfd->cAccumGreenBits;
	//newppfd.cAccumRedBits = ppfd->cAccumRedBits;
	//newppfd.cAlphaBits = ppfd->cAlphaBits;
	//newppfd.cAlphaShift = ppfd->cAlphaShift;
	//newppfd.cAuxBuffers = ppfd->cAuxBuffers;
	//newppfd.cBlueBits = ppfd->cBlueBits;
	//newppfd.cBlueShift = ppfd->cBlueShift;
	//newppfd.cColorBits = ppfd->cColorBits;
	//newppfd.cDepthBits = ppfd->cDepthBits;
	//newppfd.cGreenBits = ppfd->cGreenBits;
	//newppfd.cGreenShift = ppfd->cGreenShift;
	//newppfd.cRedBits = ppfd->cRedBits;
	//newppfd.cRedShift = ppfd->cRedShift;
	//newppfd.cStencilBits = ppfd->cStencilBits;
	//newppfd.dwDamageMask = ppfd->dwDamageMask;
	//newppfd.dwFlags = ppfd->dwFlags;
	//newppfd.dwLayerMask = ppfd->dwLayerMask;
	//newppfd.dwVisibleMask = ppfd->dwVisibleMask;
	////change to stereo
	//newppfd.dwFlags |= PFD_STEREO | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;

	currentStereo_ppfd = {};
	ZeroMemory(&currentStereo_ppfd, sizeof(PIXELFORMATDESCRIPTOR));
	currentStereo_ppfd.nSize = sizeof(currentStereo_ppfd);

	//copy all existing values
	currentStereo_ppfd.cAccumAlphaBits = ppfd->cAccumAlphaBits;
	currentStereo_ppfd.cAccumBits = ppfd->cAccumBits;
	currentStereo_ppfd.cAccumBlueBits = ppfd->cAccumBlueBits;
	currentStereo_ppfd.cAccumGreenBits = ppfd->cAccumGreenBits;
	currentStereo_ppfd.cAccumRedBits = ppfd->cAccumRedBits;
	currentStereo_ppfd.cAlphaBits = ppfd->cAlphaBits;
	currentStereo_ppfd.cAlphaShift = ppfd->cAlphaShift;
	currentStereo_ppfd.cAuxBuffers = ppfd->cAuxBuffers;
	currentStereo_ppfd.cBlueBits = ppfd->cBlueBits;
	currentStereo_ppfd.cBlueShift = ppfd->cBlueShift;
	currentStereo_ppfd.cColorBits = ppfd->cColorBits;
	currentStereo_ppfd.cDepthBits = ppfd->cDepthBits;
	currentStereo_ppfd.cGreenBits = ppfd->cGreenBits;
	currentStereo_ppfd.cGreenShift = ppfd->cGreenShift;
	currentStereo_ppfd.cRedBits = ppfd->cRedBits;
	currentStereo_ppfd.cRedShift = ppfd->cRedShift;
	currentStereo_ppfd.cStencilBits = ppfd->cStencilBits;
	currentStereo_ppfd.dwDamageMask = ppfd->dwDamageMask;
	currentStereo_ppfd.dwFlags = ppfd->dwFlags;
	currentStereo_ppfd.dwLayerMask = ppfd->dwLayerMask;
	currentStereo_ppfd.dwVisibleMask = ppfd->dwVisibleMask;
	//change to stereo
	currentStereo_ppfd.dwFlags |= PFD_STEREO | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;
	currentStereo_ppfd.iPixelType = PFD_TYPE_RGBA;
	//currentStereo_ppfd.cColorBits = 24;

	//just for NOW
	g_stereoDetect = true;

	//BOOL bValidPixFormat;
	
	//call the original function
	//bValidPixFormat = _wglChoosePixelFormat(hdc, &newppfd);
	currentStereoPixelFormat = _wglChoosePixelFormat(hdc, &currentStereo_ppfd);

	/*PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	if (DescribePixelFormat(hdc, currentStereoPixelFormat, sizeof(currentStereo_ppfd), &currentStereo_ppfd) == 0)
	return 0;*/

	//HWND     window;        ///< window handle
	//HDC      context;         ///< device context
	//HGLRC    glcontext;       ///< OpenGL resource context
	//int      pixelFormat; ///< OpenGL pixel format

	//GLWindow glWin;
	//glWin.getSuitableStereoWindow(window, context, glcontext, pixelFormat);

	//// attempt to describe the pixel format
	//PIXELFORMATDESCRIPTOR pfd = {};
	//pfd.nSize = sizeof(pfd);
	//pfd.nVersion = 1;
	//if (DescribePixelFormat(hdc, pixelFormat, sizeof(pfd), &pfd) == 0)
	//	return 0;

	//// attempt to set the pixel format
	//if (SetPixelFormat(hdc, pixelFormat, &pfd) != TRUE) return 0;
	
	//_wglChoosePixelFormatARB()

	if (currentStereoPixelFormat == 0)//!bValidPixFormat)
	{
		MessageBox(NULL, "Invalid Pixel Format", "Error! (interceptedwglChoosePixelFormat)", MB_OK);
		return 0;
		//return _wglChoosePixelFormat(hdc, ppfd);
	}
	else
	{
		//// attempt to describe the pixel format
		//PIXELFORMATDESCRIPTOR pfd = {};
		//pfd.nSize = sizeof(pfd);
		//pfd.nVersion = 1;
		//if (DescribePixelFormat(hdc, currentStereoPixelFormat, sizeof(currentStereo_ppfd), &currentStereo_ppfd) == 0)
		//	return 0;
	}
	// attempt to set the pixel format
	//if (_wglSetPixelFormat(hdc, bValidPixFormat, &newppfd) != TRUE) return 0;


	// use textures or renderbuffers?
	//JUST FOR NOW
	//useTexture = true;//false; //true;

	return 1;
}

void WINAPI interceptedglClear(GLbitfield mask)
{
	//Log::open("intercept.log");
	//Log::print("interceptedglClear\n");

	if (!_glClear)
	{
		MessageBox(NULL, "_glClear not supported", "Error! (interceptedglClear)", MB_OK);
		//return false;
		return;
	}

	//g_stereoDetect = isOpenGLStereoAvailable();

	if (g_stereoDetect)
	{

	}
	else
	{
		Log::print("No: you don't have stereo enabled.\n");
	}

	//Log::print("Start initialisation.\n");

	if (!m_initialised)
	{
		//currentConfig3DSettings.readConfig3DSettingsFromFile();

		//Log::print("Begin initialisation.\n");
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &textureSize);

		bool success = false;
				

		if (!glx.load())
			Log::print("error: failed to load GL extensions\n");
		else
		{
			//do {
			//	Log::print("loaded GL extensions\n");

			//	// select standard or multisampled GL texture mode
			//	GLenum textureMode = (m_samplesGL > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

			//	Log::print("generating render buffers\n");
			//	unsigned i = 0;
			//	for (i = 0; i<m_target.size(); ++i) {
			//		// are we using textures or renderbuffers?
			//		if (useTexture) {
			//			// using GL_TEXTURE_2D
			//			glGenTextures(1, &m_target[i].texture);

			//			glBindTexture(GL_TEXTURE_2D, m_target[i].texture);

			//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			//			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			//			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			//			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap generation included in OpenGL v1.4

			//			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthScreen, heightScreen, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			//			glBindTexture(GL_TEXTURE_2D, 0);

			//			if (m_target[i].texture == 0) {
			//				Log::print("error: failed to generate texture ID\n");
			//				break;
			//			}


			//			/*glGenTextures(1, &m_target[i].depthTexture);
			//			glBindTexture(GL_TEXTURE_2D, m_target[i].depthTexture);
			//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//			glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
			//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
			//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			//			//NULL means reserve texture memory, but texels are undefined
			//			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, widthScreen, heightScreen, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


			//			glBindTexture(GL_TEXTURE_2D, 0);
			//			*/
			//		}
			//		else {
			//			// using GL_RENDERBUFFER
			//			glx.glGenRenderbuffers(1, &m_target[i].renderBuffer);
			//			glx.glBindRenderbuffer(GL_RENDERBUFFER, m_target[i].renderBuffer);

			//			glx.glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, widthScreen, heightScreen);

			//			if (m_target[i].renderBuffer == 0) {
			//				Log::print("error: failed to generate render buffer ID\n");
			//				break;
			//			}
			//		}

			//		/*
			//		if (m_target[i].object == 0) {
			//		DWORD error = GetLastError();
			//		Log::print("error: wglDXRegisterObjectNV failed for render target: ");

			//		break;
			//		}
			//		*/
			//		glx.glGenFramebuffers(1, &m_target[i].frameBuffer);

			//		if (m_target[i].frameBuffer == 0) {
			//			Log::print("error: glGenFramebuffers failed\n");
			//			break;
			//		}

			//		glx.glBindFramebuffer(GL_FRAMEBUFFER, m_target[i].frameBuffer);


			//		if (useTexture) {
			//			// using GL_TEXTURE_2D

			//			// important to lock before using glFramebufferTexture2D

			//			// attach colour buffer texture
			//			glx.glFramebufferTexture2D(
			//				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			//				textureMode, m_target[i].texture, 0
			//				);

			//			//glx.glFramebufferTexture2D(
			//			//	GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureMode, m_target[i].depthTexture, 0);


			//		}
			//		else {
			//			// using GL_RENDERBUFFER

			//			// important to lock before using glFramebufferRenderbuffer


			//			// attach colour renderbuffer


			//			glx.glFramebufferRenderbuffer(
			//				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			//				GL_RENDERBUFFER, m_target[i].renderBuffer
			//				);


			//			// this table defines the renderbuffer parameters to be listed
			//			struct {
			//				GLenum name;
			//				const char *text;
			//			} table[] = {
			//					{ GL_RENDERBUFFER_WIDTH, "width" },
			//					{ GL_RENDERBUFFER_HEIGHT, "height" },
			//					{ GL_RENDERBUFFER_INTERNAL_FORMAT, "format" },
			//					{ GL_RENDERBUFFER_RED_SIZE, "red" },
			//					{ GL_RENDERBUFFER_GREEN_SIZE, "green" },
			//					{ GL_RENDERBUFFER_BLUE_SIZE, "blue" },
			//					{ GL_RENDERBUFFER_ALPHA_SIZE, "alpha" },
			//					{ GL_RENDERBUFFER_DEPTH_SIZE, "depth" },
			//					{ GL_RENDERBUFFER_STENCIL_SIZE, "stencil" },
			//					{ 0, 0 }
			//			};



			//			// query and log all the renderbuffer parameters
			//			for (int p = 0; table[p].name != 0; ++p) {
			//				GLint value = 0;
			//				glx.glGetRenderbufferParameteriv(GL_RENDERBUFFER, table[p].name, &value);
			//				Log::print("renderBuffer.") << table[p].text << " = " << value << endl;
			//			}

			//			glx.glBindRenderbuffer(GL_RENDERBUFFER, 0);
			//		}

			//		// log the framebuffer status (should be GL_FRAMEBUFFER_COMPLETE)
			//		GLenum status = glx.glCheckFramebufferStatus(GL_FRAMEBUFFER);
			//		Log::print() << "glCheckFramebufferStatus = " << GLFRAMEBUFFERSTATUStoString(status) << endl;

			//		status = glx.glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
			//		Log::print() << "DRAWING glCheckFramebufferStatus = " << GLFRAMEBUFFERSTATUStoString(status) << endl;

			//		glx.glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//	}

			//	// successful only if all render buffers were created and initialised
			//	success = (i == m_target.size());
			//	Log::print("Eng initialisation.\n");
			//} while (0);
		}
			
		//for testing
		//glx.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_target[0].frameBuffer);
		//glx.glBindFramebuffer(GL_FRAMEBUFFER, m_target[1].frameBuffer);
		//glDrawBuffer(GL_COLOR_ATTACHMENT0);
		//Log::print("Out initialisation.\n");
		// default OpenGL settings
		//glEnable(GL_COLOR_MATERIAL);
		//glDisable(GL_LIGHTING);
		//glDisable(GL_DEPTH_TEST);

		// default viewing system
		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();
		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		//glEnable(GL_DEPTH_TEST);
		m_initialised = true;

		//for testing
		//glDrawBuffer(GL_BACK_RIGHT);

		inLeft = true;

		glDrawBuffer(GL_BACK);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClear(GL_COLOR_BUFFER_BIT );
		glClearColor(0, 0, 0, 1);
		glClearDepth(1.0);

		inLeft = false;
	}
	
	//glEnable(GL_DEPTH_TEST);

	//for testing
	// enable texturing if required
	if (useTexture) {
		//glEnable(GL_TEXTURE_2D);
		//glColor3f(1, 0, 0);
	}
	
	/*_glClear(mask);*/

	//glx.glBindFramebuffer(GL_FRAMEBUFFER, m_target[0].frameBuffer);
	
	//if (!g_stereoDetect)
	//{
	//	glDrawBuffer(GL_BACK);
	//}
	//else 
	//	if (g_clearCount % 2 == 1)
	//{
	//	glDrawBuffer(GL_BACK_LEFT);
		
	//}
	//else
	//{
	//	glDrawBuffer(GL_BACK_RIGHT);
		/*glLineWidth(2.5);
		glColor3f(1.0, 1.0, 0.0);
		glBegin(GL_LINES);
		glVertex3f(-1, -1.0, 0.0);
		glVertex3f(1, 1, 0);
		glEnd();*/
	

	if (!inLeft){
		//glDrawBuffer(GL_BACK);
		//glClearColor(0, 1, 0, 1);
	}
	//else
	//{
	//	if (m_framesGL > 10)
	//	{
	//		std::cout << "Start real stereo \n. ";
	//		//glDrawBuffer(GL_BACK_RIGHT);

	//		inLeft = true;
	//		glDrawBuffer(GL_BACK_RIGHT);
	//		increaseClearCount = false;
	//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//		inLeft = false;
	//		increaseClearCount = true;
	//	}
	//	else
	//	{
	//		glDrawBuffer(GL_BACK);
	//	}
	//	/*glLineWidth(2.5);
	//	glColor3f(1.0, 1.0, 0.0);
	//	glBegin(GL_LINES);
	//	glVertex3f(-1, -1.0, 0.0);
	//	glVertex3f(1, 1, 0);
	//	glEnd();*/
	//	//glDrawBuffer(GL_BACK_RIGHT);
	//	//glClearColor(1, 0, 0, 1);
	//	//glDrawBuffer(GL_BACK_RIGHT);
	//}
	//}
	//if (m_framesGL % 200 == 0)
	//{
	//	glDrawBuffer(GL_BACK_RIGHT);
	//}
	//else
	//{
	//	glDrawBuffer(GL_BACK_LEFT);
	//}
	if (inLeft){
		currentConfig3DSettings.increaseFunctionCall();
	}
	
	_glClear(mask);
	
	// save OpenGL state
	//glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);

	// count the number of glClear calls
	if (increaseClearCount)
	{
		++g_clearCount;
		increaseClearCount = false;
	}
	else
	{
		increaseClearCount = true;
	}

	//glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//glDrawBuffer(GL_BACK_LEFT);

	/*if (!g_stereoDetect)
	{
		glDrawBuffer(GL_BACK);
	}
	else if (g_clearCount < g_clearsPerEye)
	{
		glDrawBuffer(GL_BACK_LEFT);
	}
	else
	{
		glDrawBuffer(GL_BACK_RIGHT);
	}
		*/

	

	return;
}

BOOL WINAPI interceptedwglSetPixelFormat(HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR * ppfd)
{
	//Log::print("OK: get interceptedwglSetPixelFormat function \n");

	if (!_wglSetPixelFormat)
	{
		MessageBox(NULL, "_wglSetPixelFormat not supported", "Error! (interceptedwglSetPixelFormat)", MB_OK);
		return false;
	}

	//return _wglSetPixelFormat(hdc, iPixelFormat, ppfd);
	// attempt to describe the pixel format
	/*PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	if (DescribePixelFormat(hdc, currentStereoPixelFormat, sizeof(currentStereo_ppfd), &currentStereo_ppfd) == 0)
		return 0;*/
	
	currentConfig3DSettings.increaseFunctionCall();

	if (_wglSetPixelFormat(hdc, currentStereoPixelFormat, &currentStereo_ppfd) != TRUE)
	{
		Log::print("_wglSetPixelFormat get error.\n");
		return 0;
	}
	else
	{
		
	}

	return 1;
}

BOOL WINAPI interceptedwglSwapBuffers(HDC hdc)
{
	//Log::open("intercept.log");
	//Log::print("OK: get interceptedwglSwapBuffers function \n");

	// was stereo detected previously?
	bool wasStereo = g_stereoDetect;

	if (!_wglSwapBuffers)
	{
		MessageBox(NULL, "_wglSwapBuffers not supported", "Error! (interceptedwglSwapBuffers)", MB_OK);
		return false;
	}

	currentConfig3DSettings.increaseFunctionCall();

	//m_framesGL++;

	
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//increaseClearCount = false;
	//inLeft = true;
	//glDrawBuffer(GL_BACK_LEFT);
	////glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClear(GL_COLOR_BUFFER_BIT);
	//glClearColor(0, 0, 0, 1);

	///*glLineWidth(2.5);
	//glColor3f(1.0, 1.0, 0.0);
	//glBegin(GL_LINES);
	//glVertex3f(-1, -1.0, 0.0);
	//glVertex3f(1, 1, 0);
	//glEnd();*/

	//inLeft = false;

	//if (g_clearCount % 2 == 1)
	//{
	//	//increaseClearCount = false;
	//	//glDrawBuffer(GL_BACK_LEFT);
	//	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	//glClearColor(1, 0, 1, 1);
	//	//increaseClearCount = true;
	//	//increaseClearCount = true;
	//	//glClear(GL_DEPTH_BUFFER_BIT);
	//	//increaseClearCount = false;
	//	
	//}
	//else
	//{
	//	//glDrawBuffer(GL_BACK_RIGHT);
	//	//increaseClearCount = false;
	//	////glClear(GL_DEPTH_BUFFER_BIT);
	//	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	//glClearColor(0, 1, 1, 1);
	//	////glMatrixMode(GL_MODELVIEW);
	//	////glLoadIdentity();
	//	//glLineWidth(2.5);
	//	//glColor3f(1.0, 1.0, 0.0);
	//	//glBegin(GL_LINES);
	//	//glVertex3f(-1, -1.0, 0.0);
	//	//glVertex3f(1, 1, 0);
	//	//glEnd();
	//	//increaseClearCount = true;
	//}

	//firstForground = true;
	//inLeft = false;

	// call the original function
	_wglSwapBuffers(hdc);
	
	//inLeft = true;
	//glDrawBuffer(GL_BACK_LEFT);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//inLeft = false;
	
	//glDrawBuffer(GL_BACK);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	////glClear(GL_COLOR_BUFFER_BIT);
	//glClearColor(0, 0, 0, 1);
	//glClearDepth(1.0);

	////for testing
	//glx.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_target[0].frameBuffer);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*
	// draw to default framebuffer
	glx.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// are we forced to use blit?
	mustUseBlit = true;

	// save OpenGL state
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);

	// enable texturing if required
	if (useTexture) {
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	}

	// for each eye
	for (int eye = 0; eye<2; ++eye) {
	// get the GL draw buffer identifier for the last rendered frame
	// (i.e. the DX surface we are reading from)
	GLuint drawBuffer = m_target[m_readBuffer].drawBuffer;

	// select the GL draw buffer (GL_BACK or GL_BACK_LEFT or GL_BACK_RIGHT)
	glDrawBuffer(drawBuffer);

	// lock the shared DX/GL render target
	//if (m_target[m_readBuffer].object != 0)  {

	// are we rendering using textures or framebuffer blitting?
	if (!useTexture || mustUseBlit) {
	//-- render using framebuffer blitting
	glx.glBindFramebuffer(GL_READ_FRAMEBUFFER, m_target[m_readBuffer].frameBuffer);

	// blit from the read framebuffer to the display framebuffer
	glx.glBlitFramebuffer(
	0, 0, m_width, m_height,        // source rectangle
	0, m_height, m_width, 0,        // destination: flip the image vertically
	GL_COLOR_BUFFER_BIT,
	GL_LINEAR
	);
	}
	else {
	//-- render using texture

	// bind the texture
	glBindTexture(GL_TEXTURE_2D, m_target[m_readBuffer].texture);

	// build our display list if it doesn't exist already
	if (m_quadListGL == 0) {
	// generate display list
	m_quadListGL = glGenLists(1);

	// draw a quad into the display list
	glNewList(m_quadListGL, GL_COMPILE);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);
	glVertex3f(-1.0f, +1.0f, 0.0f);

	glTexCoord2i(1, 0);
	glVertex3f(+1.0f, +1.0f, 0.0f);

	glTexCoord2i(1, 1);
	glVertex3f(+1.0f, -1.0f, 0.0f);

	glTexCoord2i(0, 1);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glEnd();
	glEndList();
	}

	// draw a large textured quad
	if (m_quadListGL != 0)
	glCallList(m_quadListGL);
	}

	// unlock the shared DX/GL target

	//}
	//else
	//	Log::print() << "unable to lock DX target on paint\n";

	// pick next read buffer
	m_readBuffer = (m_readBuffer + 1) % m_target.size();

	// we are only rendering stereo if we have just rendered the left eye,
	// otherwise this must be a 2D frame and we can just exit the loop
	if (drawBuffer != GL_BACK_LEFT) break;
	}

	// restore OpenGL state
	glPopAttrib();

	// swap the buffers
	//m_window.swapBuffers();
	// call the original function
	_wglSwapBuffers(hdc);


	// signal that we've processed one complete frame
	m_frameDone.signal();

	// in verbose mode, log the point at which GL swap occurs
	if (m_verbose) Log::print("GLSWAP\n");

	// performance statistics are collected in stereo mode
	//if (m_stereoMode) {
	//	// record time-stamp of first/last frame
	//	if (m_framesGL == 0)
	//		m_firstFrameTimeGL = getTime();
	//	else
	//		m_lastFrameTimeGL = getTime();

	//	// count GL frames
	//	++m_framesGL;
	//}



	// detected stereo if there is more than one glClear per frame, and the
	// number of glClear per frame is exactly divisible by two
	g_stereoDetect = (g_clearCount > 1) && ((g_clearCount % 2) == 0);

	// if stereo is enabled, then g_clearsPerEye indicates how many glClear
	// calls are expected per eye, otherwise zero
	if (g_stereoDetect)
	g_clearsPerEye = g_clearCount / 2;
	else
	g_clearsPerEye = 0;

	// reset counter for next time
	g_clearCount = 0;

	*/

	currentConfig3DSettings.increaseFrameGL();
	currentConfig3DSettings.resetCurrentStatusOfEachFrame();
	currentConfig3DSettings.switchCurrentBuffer();

	return 1;
}

void APIENTRY interceptedglRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glRotatef");
		currentConfig3DSettings.switchCurrentBuffer();
	}

	//if (m_framesGL > 10)
	//{
	//	if (firstForground)
	//	{
	//		inLeft = true;

	//		glDrawBuffer(GL_BACK_RIGHT);
	//		increaseClearCount = false;

	//		//glDrawBuffer(GL_BACK_RIGHT);
	//		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//		glClear(GL_COLOR_BUFFER_BIT);
	//		glClearColor(0, 0, 0, 1);
	//		inLeft = false;
	//		increaseClearCount = true;

	//		firstForground = false;
	//	}
	//	else
	//	{
	//		glDrawBuffer(GL_BACK_RIGHT);
	//		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//		glClear(GL_COLOR_BUFFER_BIT);
	//		glClearColor(0, 0, 0, 1);
	//	}
	//	
	//}
	
	_glRotatef(angle, x, y, z);
}

void APIENTRY interceptedglColor3f(GLfloat red, GLfloat green, GLfloat blue)
{
	//std::cout << "interceptedglColor3f ";
	
	currentConfig3DSettings.increaseFunctionCall("glColor3f");

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glColor3f");
		currentConfig3DSettings.switchCurrentBuffer();
	}

	//if (m_framesGL > 10)
	//{
	//	if (firstForground)
	//	{
	//		inLeft = true;

	//		glDrawBuffer(GL_BACK_RIGHT);
	//		increaseClearCount = false;

	//		//glDrawBuffer(GL_BACK_RIGHT);
	//		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//		glClear(GL_COLOR_BUFFER_BIT);
	//		glClearColor(0, 0, 0, 1);
	//		inLeft = false;
	//		increaseClearCount = true;

	//		firstForground = false;
	//	}
	//	else
	//	{
	//		glDrawBuffer(GL_BACK_RIGHT);
	//		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//		//glClear(GL_COLOR_BUFFER_BIT);
	//		//glClearColor(0, 0, 0, 1);
	//	}
	//}
	
	_glColor3f(red, green, blue);
	//inLeft = false;
}
void APIENTRY interceptedglLoadIdentity(void) {

	currentConfig3DSettings.increaseFunctionCall("glLoadIdentity");

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glLoadIdentity");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glLoadIdentity();
}
void APIENTRY interceptedglMultMatrixf(const GLfloat * m)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glMultMatrixf");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glMultMatrixf(m);
}
void APIENTRY interceptedglTranslated(GLdouble x, GLdouble y, GLdouble z) {

	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glTranslated");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glTranslated(x, y, z);
}
void APIENTRY interceptedglLightfv(GLenum light, GLenum pname, const GLfloat * params) {
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glLightfv");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glLightfv(light, pname, params);
}
void APIENTRY interceptedglTranslatef(GLfloat x, GLfloat y, GLfloat z) {
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glTranslatef");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glTranslatef(x, y, z);
}
void APIENTRY interceptedglBegin(GLenum mode) {
	currentConfig3DSettings.increaseFunctionCall("glBegin");

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glBegin");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glBegin(mode);
}
void APIENTRY interceptedglVertex3f(GLfloat x, GLfloat y, GLfloat z) {
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glVertex3f");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glVertex3f(x, y, z);
}
void APIENTRY interceptedglEnd(void) {
	currentConfig3DSettings.increaseFunctionCall("glEnd");

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glEnd");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glEnd();
}
void APIENTRY interceptedglFlush(void) {
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glFlush");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glFlush();
}
/************************************************************************************/
BOOL APIENTRY interceptedwglMakeCurrent(HDC hdc, HGLRC hglrc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglMakeCurrent");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglMakeCurrent(hdc, hglrc);
}
void APIENTRY interceptedglScaled(GLdouble x, GLdouble y, GLdouble z)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glScaled");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glScaled(x, y, z);
}
void APIENTRY interceptedglColor3d(GLdouble red, GLdouble green, GLdouble blue)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glColor3d");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glColor3d(red, green, blue);
}
void APIENTRY interceptedglPushMatrix(void)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glPushMatrix");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glPushMatrix();
}
void APIENTRY interceptedglMultMatrixd(const GLdouble * m)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glMultMatrixd");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glMultMatrixd(m);
}
HGLRC APIENTRY interceptedwglCreateContext(HDC hdc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglCreateContext");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglCreateContext(hdc);
}
BOOL APIENTRY interceptedwglDeleteContext(HGLRC hglrc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDeleteContext");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDeleteContext(hglrc);
}
HGLRC APIENTRY interceptedwglGetCurrentContext(void)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetCurrentContext");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetCurrentContext();
}
BOOL APIENTRY interceptedwglCopyContext(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglCopyContext");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglCopyContext(hglrcSrc, hglrcDst, mask);
}
HDC APIENTRY interceptedwglGetCurrentDC(void)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetCurrentDC");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetCurrentDC();
}
PROC APIENTRY interceptedwglGetDefaultProcAddress(const char * lpszProc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetDefaultProcAddress");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetDefaultProcAddress(lpszProc);
}
BOOL APIENTRY interceptedwglShareLists(HGLRC hglrc1, HGLRC hglrc2)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglShareLists");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglShareLists(hglrc1, hglrc2);
}
HGLRC APIENTRY interceptedwglCreateLayerContext(HDC hdc, int iLayerPlane)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglCreateLayerContext");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglCreateLayerContext(hdc, iLayerPlane);
}
BOOL APIENTRY interceptedwglDescribeLayerPlane(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LAYERPLANEDESCRIPTOR * plpd)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDescribeLayerPlane");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDescribeLayerPlane(hdc, iPixelFormat, iLayerPlane, nBytes, plpd);
}
int APIENTRY interceptedwglSetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, const COLORREF * pcr)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglSetLayerPaletteEntries");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglSetLayerPaletteEntries(hdc, iLayerPlane, iStart, cEntries, pcr);
}
int APIENTRY interceptedwglGetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF * pcr)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetLayerPaletteEntries");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetLayerPaletteEntries(hdc, iLayerPlane, iStart, cEntries, pcr);
}
BOOL APIENTRY interceptedwglRealizeLayerPalette(HDC hdc, int iLayerPlane, BOOL bRealize)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglRealizeLayerPalette");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglRealizeLayerPalette(hdc, iLayerPlane, bRealize);
}
BOOL APIENTRY interceptedwglSwapLayerBuffers(HDC hdc, UINT fuPlanes)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglSwapLayerBuffers");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglSwapLayerBuffers(hdc, fuPlanes);
}
BOOL APIENTRY interceptedwglUseFontBitmapsA(HDC hdc, DWORD first, DWORD count, DWORD listBase)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglUseFontBitmapsA");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglUseFontBitmapsA(hdc, first, count, listBase);
}
BOOL APIENTRY interceptedwglUseFontBitmapsW(HDC hdc, DWORD first, DWORD count, DWORD listBase)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglUseFontBitmapsW");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglUseFontBitmapsW(hdc, first, count, listBase);
}
DWORD APIENTRY interceptedwglSwapMultipleBuffers(UINT n, const WGLSWAP * ps)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglSwapMultipleBuffers");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglSwapMultipleBuffers(n, ps);
}
BOOL APIENTRY interceptedwglUseFontOutlinesA(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, GLYPHMETRICSFLOAT * lpgmf)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglUseFontOutlinesA");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglUseFontOutlinesA(hdc, first, count, listBase, deviation, extrusion, format, lpgmf);
}
BOOL APIENTRY interceptedwglUseFontOutlinesW(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, GLYPHMETRICSFLOAT * lpgmf)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglUseFontOutlinesW");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglUseFontOutlinesW(hdc, first, count, listBase, deviation, extrusion, format, lpgmf);
}
HANDLE APIENTRY interceptedwglCreateBufferRegionARB(HDC hDC, int iLayerPlane, UINT uType)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglCreateBufferRegionARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglCreateBufferRegionARB(hDC, iLayerPlane, uType);
}
void APIENTRY interceptedwglDeleteBufferRegionARB(HANDLE hRegion)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDeleteBufferRegionARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_wglDeleteBufferRegionARB(hRegion);
}
BOOL APIENTRY interceptedwglSaveBufferRegionARB(HANDLE hRegion, int x, int y, int width, int height)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglSaveBufferRegionARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglSaveBufferRegionARB(hRegion, x, y, width, height);
}
BOOL APIENTRY interceptedwglRestoreBufferRegionARB(HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglRestoreBufferRegionARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglRestoreBufferRegionARB(hRegion, x, y, width, height, xSrc, ySrc);
}
const char * APIENTRY interceptedwglGetExtensionsStringARB(HDC hdc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetExtensionsStringARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetExtensionsStringARB(hdc);
}
BOOL APIENTRY interceptedwglGetPixelFormatAttribivARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, int * piValues)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetPixelFormatAttribivARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetPixelFormatAttribivARB(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, piValues);
}
BOOL APIENTRY interceptedwglGetPixelFormatAttribfvARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, FLOAT * pfValues)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetPixelFormatAttribfvARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetPixelFormatAttribfvARB(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, pfValues);
}
BOOL APIENTRY interceptedwglMakeContextCurrentARB(HDC hDrawDC, HDC hReadDC, HGLRC hglrc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglMakeContextCurrentARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglMakeContextCurrentARB(hDrawDC, hReadDC, hglrc);
}
HDC APIENTRY interceptedwglGetCurrentReadDCARB(void)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetCurrentReadDCARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetCurrentReadDCARB();
}
HPBUFFERARB APIENTRY interceptedwglCreatePbufferARB(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int * piAttribList)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglCreatePbufferARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglCreatePbufferARB(hDC, iPixelFormat, iWidth, iHeight, piAttribList);
}
HDC APIENTRY interceptedwglGetPbufferDCARB(HPBUFFERARB hPbuffer)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetPbufferDCARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetPbufferDCARB(hPbuffer);
}
int APIENTRY interceptedwglReleasePbufferDCARB(HPBUFFERARB hPbuffer, HDC hDC)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglReleasePbufferDCARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglReleasePbufferDCARB(hPbuffer, hDC);
}
BOOL APIENTRY interceptedwglDestroyPbufferARB(HPBUFFERARB hPbuffer)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDestroyPbufferARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDestroyPbufferARB(hPbuffer);
}
BOOL APIENTRY interceptedwglQueryPbufferARB(HPBUFFERARB hPbuffer, int iAttribute, int * piValue)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglQueryPbufferARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglQueryPbufferARB(hPbuffer, iAttribute, piValue);
}
BOOL APIENTRY interceptedwglBindTexImageARB(HPBUFFERARB hPbuffer, int iBuffer)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglBindTexImageARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglBindTexImageARB(hPbuffer, iBuffer);
}
BOOL APIENTRY interceptedwglReleaseTexImageARB(HPBUFFERARB hPbuffer, int iBuffer)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglReleaseTexImageARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglReleaseTexImageARB(hPbuffer, iBuffer);
}
BOOL APIENTRY interceptedwglSetPbufferAttribARB(HPBUFFERARB hPbuffer, const int * piAttribList)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglSetPbufferAttribARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglSetPbufferAttribARB(hPbuffer, piAttribList);
}
HGLRC APIENTRY interceptedwglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int * attribList)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglCreateContextAttribsARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglCreateContextAttribsARB(hDC, hShareContext, attribList);
}
GLboolean APIENTRY interceptedwglCreateDisplayColorTableEXT(GLushort id)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglCreateDisplayColorTableEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglCreateDisplayColorTableEXT(id);
}
GLboolean APIENTRY interceptedwglLoadDisplayColorTableEXT(const GLushort * table, GLuint length)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglLoadDisplayColorTableEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglLoadDisplayColorTableEXT(table, length);
}
GLboolean APIENTRY interceptedwglBindDisplayColorTableEXT(GLushort id)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglBindDisplayColorTableEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglBindDisplayColorTableEXT(id);
}
void APIENTRY interceptedwglDestroyDisplayColorTableEXT(GLushort id)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDestroyDisplayColorTableEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_wglDestroyDisplayColorTableEXT(id);
}
const char * APIENTRY interceptedwglGetExtensionsStringEXT(void)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetExtensionsStringEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetExtensionsStringEXT();
}
BOOL APIENTRY interceptedwglMakeContextCurrentEXT(HDC hDrawDC, HDC hReadDC, HGLRC hglrc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglMakeContextCurrentEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglMakeContextCurrentEXT(hDrawDC, hReadDC, hglrc);
}
HDC APIENTRY interceptedwglGetCurrentReadDCEXT(void)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetCurrentReadDCEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetCurrentReadDCEXT();
}
HPBUFFEREXT APIENTRY interceptedwglCreatePbufferEXT(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int * piAttribList)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglCreatePbufferEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglCreatePbufferEXT(hDC, iPixelFormat, iWidth, iHeight, piAttribList);
}
HDC APIENTRY interceptedwglGetPbufferDCEXT(HPBUFFEREXT hPbuffer)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetPbufferDCEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetPbufferDCEXT(hPbuffer);
}
int APIENTRY interceptedwglReleasePbufferDCEXT(HPBUFFEREXT hPbuffer, HDC hDC)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglReleasePbufferDCEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglReleasePbufferDCEXT(hPbuffer, hDC);
}
BOOL APIENTRY interceptedwglDestroyPbufferEXT(HPBUFFEREXT hPbuffer)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDestroyPbufferEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDestroyPbufferEXT(hPbuffer);
}
BOOL APIENTRY interceptedwglQueryPbufferEXT(HPBUFFEREXT hPbuffer, int iAttribute, int * piValue)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglQueryPbufferEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglQueryPbufferEXT(hPbuffer, iAttribute, piValue);
}
BOOL APIENTRY interceptedwglGetPixelFormatAttribivEXT(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, int * piValues)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetPixelFormatAttribivEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetPixelFormatAttribivEXT(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, piValues);
}
BOOL APIENTRY interceptedwglGetPixelFormatAttribfvEXT(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, FLOAT * pfValues)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetPixelFormatAttribfvEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetPixelFormatAttribfvEXT(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, pfValues);
}
BOOL APIENTRY interceptedwglChoosePixelFormatEXT(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglChoosePixelFormatEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglChoosePixelFormatEXT(hdc, piAttribIList, pfAttribFList, nMaxFormats, piFormats, nNumFormats);
}
BOOL APIENTRY interceptedwglSwapIntervalEXT(int interval)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglSwapIntervalEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglSwapIntervalEXT(interval);
}
int APIENTRY interceptedwglGetSwapIntervalEXT(void)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetSwapIntervalEXT");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetSwapIntervalEXT();
}
void * APIENTRY interceptedwglAllocateMemoryNV(GLsizei size, GLfloat readfreq, GLfloat writefreq, GLfloat priority)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglAllocateMemoryNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglAllocateMemoryNV(size, readfreq, writefreq, priority);
}
void APIENTRY interceptedwglFreeMemoryNV(void * pointer)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglFreeMemoryNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_wglFreeMemoryNV(pointer);
}
BOOL APIENTRY interceptedwglGetSyncValuesOML(HDC hdc, INT64 * ust, INT64 * msc, INT64 * sbc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetSyncValuesOML");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetSyncValuesOML(hdc, ust, msc, sbc);
}
BOOL APIENTRY interceptedwglGetMscRateOML(HDC hdc, INT32 * numerator, INT32 * denominator)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetMscRateOML");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetMscRateOML(hdc, numerator, denominator);
}
INT64 APIENTRY interceptedwglSwapBuffersMscOML(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglSwapBuffersMscOML");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglSwapBuffersMscOML(hdc, target_msc, divisor, remainder);
}
INT64 APIENTRY interceptedwglSwapLayerBuffersMscOML(HDC hdc, int fuPlanes, INT64 target_msc, INT64 divisor, INT64 remainder)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglSwapLayerBuffersMscOML");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglSwapLayerBuffersMscOML(hdc, fuPlanes, target_msc, divisor, remainder);
}
BOOL APIENTRY interceptedwglWaitForMscOML(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder, INT64 * ust, INT64 * msc, INT64 * sbc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglWaitForMscOML");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglWaitForMscOML(hdc, target_msc, divisor, remainder, ust, msc, sbc);
}
BOOL APIENTRY interceptedwglWaitForSbcOML(HDC hdc, INT64 target_sbc, INT64 * ust, INT64 * msc, INT64 * sbc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglWaitForSbcOML");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglWaitForSbcOML(hdc, target_sbc, ust, msc, sbc);
}
BOOL APIENTRY interceptedwglGetDigitalVideoParametersI3D(HDC hDC, int iAttribute, int * piValue)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetDigitalVideoParametersI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetDigitalVideoParametersI3D(hDC, iAttribute, piValue);
}
BOOL APIENTRY interceptedwglSetDigitalVideoParametersI3D(HDC hDC, int iAttribute, const int * piValue)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglSetDigitalVideoParametersI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglSetDigitalVideoParametersI3D(hDC, iAttribute, piValue);
}
BOOL APIENTRY interceptedwglGetGammaTableParametersI3D(HDC hDC, int iAttribute, int * piValue)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetGammaTableParametersI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetGammaTableParametersI3D(hDC, iAttribute, piValue);
}
BOOL APIENTRY interceptedwglSetGammaTableParametersI3D(HDC hDC, int iAttribute, const int * piValue)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglSetGammaTableParametersI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglSetGammaTableParametersI3D(hDC, iAttribute, piValue);
}
BOOL APIENTRY interceptedwglGetGammaTableI3D(HDC hDC, int iEntries, USHORT * puRed, USHORT * puGreen, USHORT * puBlue)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetGammaTableI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetGammaTableI3D(hDC, iEntries, puRed, puGreen, puBlue);
}
BOOL APIENTRY interceptedwglSetGammaTableI3D(HDC hDC, int iEntries, const USHORT * puRed, const USHORT * puGreen, const USHORT * puBlue)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglSetGammaTableI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglSetGammaTableI3D(hDC, iEntries, puRed, puGreen, puBlue);
}
BOOL APIENTRY interceptedwglEnableGenlockI3D(HDC hDC)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglEnableGenlockI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglEnableGenlockI3D(hDC);
}
BOOL APIENTRY interceptedwglDisableGenlockI3D(HDC hDC)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDisableGenlockI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDisableGenlockI3D(hDC);
}
BOOL APIENTRY interceptedwglIsEnabledGenlockI3D(HDC hDC, BOOL * pFlag)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglIsEnabledGenlockI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglIsEnabledGenlockI3D(hDC, pFlag);
}
BOOL APIENTRY interceptedwglGenlockSourceI3D(HDC hDC, UINT uSource)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGenlockSourceI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGenlockSourceI3D(hDC, uSource);
}
BOOL APIENTRY interceptedwglGetGenlockSourceI3D(HDC hDC, UINT * uSource)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetGenlockSourceI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetGenlockSourceI3D(hDC, uSource);
}
BOOL APIENTRY interceptedwglGenlockSourceEdgeI3D(HDC hDC, UINT uEdge)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGenlockSourceEdgeI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGenlockSourceEdgeI3D(hDC, uEdge);
}
BOOL APIENTRY interceptedwglGetGenlockSourceEdgeI3D(HDC hDC, UINT * uEdge)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetGenlockSourceEdgeI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetGenlockSourceEdgeI3D(hDC, uEdge);
}
BOOL APIENTRY interceptedwglGenlockSampleRateI3D(HDC hDC, UINT uRate)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGenlockSampleRateI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGenlockSampleRateI3D(hDC, uRate);
}
BOOL APIENTRY interceptedwglGetGenlockSampleRateI3D(HDC hDC, UINT * uRate)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetGenlockSampleRateI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetGenlockSampleRateI3D(hDC, uRate);
}
BOOL APIENTRY interceptedwglGenlockSourceDelayI3D(HDC hDC, UINT uDelay)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGenlockSourceDelayI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGenlockSourceDelayI3D(hDC, uDelay);
}
BOOL APIENTRY interceptedwglGetGenlockSourceDelayI3D(HDC hDC, UINT * uDelay)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetGenlockSourceDelayI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetGenlockSourceDelayI3D(hDC, uDelay);
}
BOOL APIENTRY interceptedwglQueryGenlockMaxSourceDelayI3D(HDC hDC, UINT * uMaxLineDelay, UINT * uMaxPixelDelay)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglQueryGenlockMaxSourceDelayI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglQueryGenlockMaxSourceDelayI3D(hDC, uMaxLineDelay, uMaxPixelDelay);
}
void * APIENTRY interceptedwglCreateImageBufferI3D(HDC hDC, DWORD dwSize, UINT uFlags)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglCreateImageBufferI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglCreateImageBufferI3D(hDC, dwSize, uFlags);
}
BOOL APIENTRY interceptedwglDestroyImageBufferI3D(HDC hDC, void * pAddress)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDestroyImageBufferI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDestroyImageBufferI3D(hDC, pAddress);
}
BOOL APIENTRY interceptedwglAssociateImageBufferEventsI3D(HDC hDC, const HANDLE * pEvent, void * const * pAddress, const DWORD * pSize, UINT count)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglAssociateImageBufferEventsI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglAssociateImageBufferEventsI3D(hDC, pEvent, pAddress, pSize, count);
}
BOOL APIENTRY interceptedwglReleaseImageBufferEventsI3D(HDC hDC, void * const * pAddress, UINT count)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglReleaseImageBufferEventsI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglReleaseImageBufferEventsI3D(hDC, pAddress, count);
}
BOOL APIENTRY interceptedwglEnableFrameLockI3D(void)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglEnableFrameLockI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglEnableFrameLockI3D();
}
BOOL APIENTRY interceptedwglDisableFrameLockI3D(void)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDisableFrameLockI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDisableFrameLockI3D();
}
BOOL APIENTRY interceptedwglIsEnabledFrameLockI3D(BOOL * pFlag)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglIsEnabledFrameLockI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglIsEnabledFrameLockI3D(pFlag);
}
BOOL APIENTRY interceptedwglQueryFrameLockMasterI3D(BOOL * pFlag)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglQueryFrameLockMasterI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglQueryFrameLockMasterI3D(pFlag);
}
BOOL APIENTRY interceptedwglGetFrameUsageI3D(float * pUsage)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetFrameUsageI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetFrameUsageI3D(pUsage);
}
BOOL APIENTRY interceptedwglBeginFrameTrackingI3D(void)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglBeginFrameTrackingI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglBeginFrameTrackingI3D();
}
BOOL APIENTRY interceptedwglEndFrameTrackingI3D(void)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglEndFrameTrackingI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglEndFrameTrackingI3D();
}
BOOL APIENTRY interceptedwglQueryFrameTrackingI3D(DWORD * pFrameCount, DWORD * pMissedFrames, float * pLastMissedUsage)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglQueryFrameTrackingI3D");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglQueryFrameTrackingI3D(pFrameCount, pMissedFrames, pLastMissedUsage);
}
BOOL APIENTRY interceptedwglSetStereoEmitterState3DL(HDC hDC, UINT uState)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglSetStereoEmitterState3DL");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglSetStereoEmitterState3DL(hDC, uState);
}
int APIENTRY interceptedwglEnumerateVideoDevicesNV(HDC hDC, HVIDEOOUTPUTDEVICENV * phDeviceList)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglEnumerateVideoDevicesNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglEnumerateVideoDevicesNV(hDC, phDeviceList);
}
BOOL APIENTRY interceptedwglBindVideoDeviceNV(HDC hDC, unsigned int uVideoSlot, HVIDEOOUTPUTDEVICENV hVideoDevice, const int * piAttribList)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglBindVideoDeviceNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglBindVideoDeviceNV(hDC, uVideoSlot, hVideoDevice, piAttribList);
}
BOOL APIENTRY interceptedwglQueryCurrentContextNV(int iAttribute, int * piValue)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglQueryCurrentContextNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglQueryCurrentContextNV(iAttribute, piValue);
}
BOOL APIENTRY interceptedwglGetVideoDeviceNV(HDC hDC, int numDevices, HPVIDEODEV * hVideoDevice)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetVideoDeviceNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetVideoDeviceNV(hDC, numDevices, hVideoDevice);
}
BOOL APIENTRY interceptedwglReleaseVideoDeviceNV(HPVIDEODEV hVideoDevice)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglReleaseVideoDeviceNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglReleaseVideoDeviceNV(hVideoDevice);
}
BOOL APIENTRY interceptedwglBindVideoImageNV(HPVIDEODEV hVideoDevice, HPBUFFERARB hPbuffer, int iVideoBuffer)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglBindVideoImageNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglBindVideoImageNV(hVideoDevice, hPbuffer, iVideoBuffer);
}
BOOL APIENTRY interceptedwglReleaseVideoImageNV(HPBUFFERARB hPbuffer, int iVideoBuffer)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglReleaseVideoImageNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglReleaseVideoImageNV(hPbuffer, iVideoBuffer);
}
BOOL APIENTRY interceptedwglSendPbufferToVideoNV(HPBUFFERARB hPbuffer, int iBufferType, unsigned long * pulCounterPbuffer, BOOL bBlock)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglSendPbufferToVideoNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglSendPbufferToVideoNV(hPbuffer, iBufferType, pulCounterPbuffer, bBlock);
}
BOOL APIENTRY interceptedwglGetVideoInfoNV(HPVIDEODEV hpVideoDevice, unsigned long * pulCounterOutputPbuffer, unsigned long * pulCounterOutputVideo)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetVideoInfoNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetVideoInfoNV(hpVideoDevice, pulCounterOutputPbuffer, pulCounterOutputVideo);
}
BOOL APIENTRY interceptedwglJoinSwapGroupNV(HDC hDC, GLuint group)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglJoinSwapGroupNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglJoinSwapGroupNV(hDC, group);
}
BOOL APIENTRY interceptedwglBindSwapBarrierNV(GLuint group, GLuint barrier)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglBindSwapBarrierNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglBindSwapBarrierNV(group, barrier);
}
BOOL APIENTRY interceptedwglQuerySwapGroupNV(HDC hDC, GLuint * group, GLuint * barrier)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglQuerySwapGroupNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglQuerySwapGroupNV(hDC, group, barrier);
}
BOOL APIENTRY interceptedwglQueryMaxSwapGroupsNV(HDC hDC, GLuint * maxGroups, GLuint * maxBarriers)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglQueryMaxSwapGroupsNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglQueryMaxSwapGroupsNV(hDC, maxGroups, maxBarriers);
}
BOOL APIENTRY interceptedwglQueryFrameCountNV(HDC hDC, GLuint * count)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglQueryFrameCountNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglQueryFrameCountNV(hDC, count);
}
BOOL APIENTRY interceptedwglResetFrameCountNV(HDC hDC)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglResetFrameCountNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglResetFrameCountNV(hDC);
}
BOOL APIENTRY interceptedwglEnumGpusNV(UINT iGpuIndex, HGPUNV * phGpu)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglEnumGpusNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglEnumGpusNV(iGpuIndex, phGpu);
}
BOOL APIENTRY interceptedwglEnumGpuDevicesNV(HGPUNV hGpu, UINT iDeviceIndex, _GPU_DEVICE * lpGpuDevice)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglEnumGpuDevicesNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglEnumGpuDevicesNV(hGpu, iDeviceIndex, lpGpuDevice);
}
HDC APIENTRY interceptedwglCreateAffinityDCNV(const HGPUNV * phGpuList)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglCreateAffinityDCNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglCreateAffinityDCNV(phGpuList);
}
BOOL APIENTRY interceptedwglEnumGpusFromAffinityDCNV(HDC hAffinityDC, UINT iGpuIndex, HGPUNV * hGpu)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglEnumGpusFromAffinityDCNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglEnumGpusFromAffinityDCNV(hAffinityDC, iGpuIndex, hGpu);
}
BOOL APIENTRY interceptedwglDeleteDCNV(HDC hdc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDeleteDCNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDeleteDCNV(hdc);
}
UINT APIENTRY interceptedwglGetGPUIDsAMD(UINT maxCount, UINT * ids)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetGPUIDsAMD");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetGPUIDsAMD(maxCount, ids);
}
INT APIENTRY interceptedwglGetGPUInfoAMD(UINT id, int property, GLenum dataType, UINT size, void * data)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetGPUInfoAMD");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetGPUInfoAMD(id, property, dataType, size, data);
}
UINT APIENTRY interceptedwglGetContextGPUIDAMD(HGLRC hglrc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetContextGPUIDAMD");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetContextGPUIDAMD(hglrc);
}
HGLRC APIENTRY interceptedwglCreateAssociatedContextAMD(UINT id)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglCreateAssociatedContextAMD");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglCreateAssociatedContextAMD(id);
}
HGLRC APIENTRY interceptedwglCreateAssociatedContextAttribsAMD(UINT id, HGLRC hShareContext, const int * attribList)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglCreateAssociatedContextAttribsAMD");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglCreateAssociatedContextAttribsAMD(id, hShareContext, attribList);
}
BOOL APIENTRY interceptedwglDeleteAssociatedContextAMD(HGLRC hglrc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDeleteAssociatedContextAMD");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDeleteAssociatedContextAMD(hglrc);
}
BOOL APIENTRY interceptedwglMakeAssociatedContextCurrentAMD(HGLRC hglrc)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglMakeAssociatedContextCurrentAMD");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglMakeAssociatedContextCurrentAMD(hglrc);
}
HGLRC APIENTRY interceptedwglGetCurrentAssociatedContextAMD(void)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglGetCurrentAssociatedContextAMD");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglGetCurrentAssociatedContextAMD();
}
void APIENTRY interceptedwglBlitContextFramebufferAMD(HGLRC dstCtx, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglBlitContextFramebufferAMD");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_wglBlitContextFramebufferAMD(dstCtx, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}
BOOL APIENTRY interceptedwglBindVideoCaptureDeviceNV(UINT uVideoSlot, HVIDEOINPUTDEVICENV hDevice)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglBindVideoCaptureDeviceNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglBindVideoCaptureDeviceNV(uVideoSlot, hDevice);
}
UINT APIENTRY interceptedwglEnumerateVideoCaptureDevicesNV(HDC hDc, HVIDEOINPUTDEVICENV * phDeviceList)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglEnumerateVideoCaptureDevicesNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglEnumerateVideoCaptureDevicesNV(hDc, phDeviceList);
}
BOOL APIENTRY interceptedwglLockVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglLockVideoCaptureDeviceNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglLockVideoCaptureDeviceNV(hDc, hDevice);
}
BOOL APIENTRY interceptedwglQueryVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice, int iAttribute, int * piValue)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglQueryVideoCaptureDeviceNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglQueryVideoCaptureDeviceNV(hDc, hDevice, iAttribute, piValue);
}
BOOL APIENTRY interceptedwglReleaseVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglReleaseVideoCaptureDeviceNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglReleaseVideoCaptureDeviceNV(hDc, hDevice);
}
BOOL APIENTRY interceptedwglCopyImageSubDataNV(HGLRC hSrcRC, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, HGLRC hDstRC, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglCopyImageSubDataNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglCopyImageSubDataNV(hSrcRC, srcName, srcTarget, srcLevel, srcX, srcY, srcZ, hDstRC, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, width, height, depth);
}
BOOL APIENTRY interceptedwglDXSetResourceShareHandleNV(void * dxObject, HANDLE shareHandle)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDXSetResourceShareHandleNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDXSetResourceShareHandleNV(dxObject, shareHandle);
}
HANDLE APIENTRY interceptedwglDXOpenDeviceNV(void * dxDevice)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDXOpenDeviceNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDXOpenDeviceNV(dxDevice);
}
BOOL APIENTRY interceptedwglDXCloseDeviceNV(HANDLE hDevice)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDXCloseDeviceNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDXCloseDeviceNV(hDevice);
}
HANDLE APIENTRY interceptedwglDXRegisterObjectNV(HANDLE hDevice, void * dxObject, GLuint name, GLenum type, GLenum access)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDXRegisterObjectNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDXRegisterObjectNV(hDevice, dxObject, name, type, access);
}
BOOL APIENTRY interceptedwglDXUnregisterObjectNV(HANDLE hDevice, HANDLE hObject)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDXUnregisterObjectNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDXUnregisterObjectNV(hDevice, hObject);
}
BOOL APIENTRY interceptedwglDXObjectAccessNV(HANDLE hObject, GLenum access)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDXObjectAccessNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDXObjectAccessNV(hObject, access);
}
BOOL APIENTRY interceptedwglDXLockObjectsNV(HANDLE hDevice, GLint count, HANDLE * hObjects)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDXLockObjectsNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDXLockObjectsNV(hDevice, count, hObjects);
}
BOOL APIENTRY interceptedwglDXUnlockObjectsNV(HANDLE hDevice, GLint count, HANDLE * hObjects)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDXUnlockObjectsNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDXUnlockObjectsNV(hDevice, count, hObjects);
}
BOOL APIENTRY interceptedwglDelayBeforeSwapNV(HDC hDC, GLfloat seconds)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("wglDelayBeforeSwapNV");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _wglDelayBeforeSwapNV(hDC, seconds);
}
/************************************************************************************/
void APIENTRY interceptedglGetIntegerv(GLenum pname, GLint * params)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glGetIntegerv");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glGetIntegerv(pname, params);
}
void APIENTRY interceptedglGetBooleanv(GLenum pname, GLboolean * params)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glGetBooleanv");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glGetBooleanv(pname, params);
}
void APIENTRY interceptedglDisable(GLenum cap)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glDisable");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glDisable(cap);
}
void APIENTRY interceptedglVertex2d(GLdouble x, GLdouble y)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glVertex2d");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glVertex2d(x, y);
}
void APIENTRY interceptedglCallList(GLuint list)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glCallList");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glCallList(list);
}
void APIENTRY interceptedglMaterialfv(GLenum face, GLenum pname, const GLfloat * params)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glMaterialfv");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glMaterialfv(face, pname, params);
}
void APIENTRY interceptedglEnable(GLenum cap)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glEnable");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glEnable(cap);
}
void APIENTRY interceptedglPopMatrix(void)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glPopMatrix");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glPopMatrix();
}
void APIENTRY interceptedglTexEnvi(GLenum target, GLenum pname, GLint param)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glTexEnvi");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glTexEnvi(target, pname, param);
}
void APIENTRY interceptedglVertex3d(GLdouble x, GLdouble y, GLdouble z)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glVertex3d");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glVertex3d(x, y, z);
}
void APIENTRY interceptedglTexCoord2d(GLdouble s, GLdouble t)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glTexCoord2d");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glTexCoord2d(s, t);
}
void APIENTRY interceptedglBindTexture(GLenum target, GLuint texture)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glBindTexture");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glBindTexture(target, texture);
}
void APIENTRY interceptedglRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glRotated");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glRotated(angle, x, y, z);
}
void APIENTRY interceptedglPushAttrib(GLbitfield mask)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glPushAttrib");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glPushAttrib(mask);
}
void APIENTRY interceptedglPopAttrib(void)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glPopAttrib");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glPopAttrib();
}
void APIENTRY interceptedglColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glColor4f");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glColor4f(red, green, blue, alpha);
}
void APIENTRY interceptedglVertex2f(GLfloat x, GLfloat y)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glVertex2f");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glVertex2f(x, y);
}
void APIENTRY interceptedglBlendFunc(GLenum sfactor, GLenum dfactor)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glBlendFunc");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glBlendFunc(sfactor, dfactor);
}
void APIENTRY interceptedglFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glFrustum");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glFrustum(left, right, bottom, top, zNear, zFar);
}
void APIENTRY interceptedglViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glViewport");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glViewport(x, y, width, height);
}
void APIENTRY interceptedglMatrixMode(GLenum mode)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glMatrixMode");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glMatrixMode(mode);
}
void APIENTRY interceptedglPolygonMode(GLenum face, GLenum mode)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glPolygonMode");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glPolygonMode(face, mode);
}
void APIENTRY interceptedglClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glClearColor");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glClearColor(red, green, blue, alpha);
}
void APIENTRY interceptedglScalef(GLfloat x, GLfloat y, GLfloat z)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glScalef");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glScalef(x, y, z);
}
void APIENTRY interceptedglHint(GLenum target, GLenum mode)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glHint");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glHint(target, mode);
}
void APIENTRY interceptedglFogfv(GLenum pname, const GLfloat * params)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glFogfv");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glFogfv(pname, params);
}
void APIENTRY interceptedglFogf(GLenum pname, const GLfloat params)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glFogf");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glFogf(pname, params);
}
void APIENTRY interceptedglColorMaterial(GLenum face, GLenum mode)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glColorMaterial");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glColorMaterial(face, mode);
}
void APIENTRY interceptedglLightModelfv(GLenum pname, const GLfloat * params)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glLightModelfv");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glLightModelfv(pname, params);
}
void APIENTRY interceptedglGetFloatv(GLenum pname, GLfloat * params)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glGetFloatv");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glGetFloatv(pname, params);
}
void APIENTRY interceptedglBindBufferARB(GLenum target, GLuint buffer)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glBindBufferARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glBindBufferARB(target, buffer);
}
void APIENTRY interceptedglInterleavedArrays(GLenum format, GLsizei stride, const GLvoid * pointer)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glInterleavedArrays");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glInterleavedArrays(format, stride, pointer);
}
void APIENTRY interceptedglDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glDrawArrays");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glDrawArrays(mode, first, count);
}
void APIENTRY interceptedglUseProgram(GLuint program)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glUseProgram");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glUseProgram(program);
}
GLint APIENTRY interceptedglGetUniformLocationARB(GLhandleARB programObj, const GLcharARB * name)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glGetUniformLocationARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	return _glGetUniformLocationARB(programObj, name);
}
void APIENTRY interceptedglUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glUniform4f");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glUniform4f(location, v0, v1, v2, v3);
}
void APIENTRY interceptedglUniform1f(GLint location, GLfloat v0)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glUniform1f");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glUniform1f(location, v0);
}
void APIENTRY interceptedglUniform1i(GLint location, GLint v0)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glUniform1i");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glUniform1i(location, v0);
}
void APIENTRY interceptedglActiveTextureARB(GLenum texture)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glActiveTextureARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glActiveTextureARB(texture);
}
void APIENTRY interceptedglMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glMultiTexCoord2fARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glMultiTexCoord2fARB(target, s, t);
}
void APIENTRY interceptedglDepthMask(GLboolean flag)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glDepthMask");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glDepthMask(flag);
}
void APIENTRY interceptedglEnableClientState(GLenum array)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glEnableClientState");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glEnableClientState(array);
}
void APIENTRY interceptedglPointParameterfvARB(GLenum pname, const GLfloat * params)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glPointParameterfvARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glPointParameterfvARB(pname, params);
}
void APIENTRY interceptedglPointParameterfARB(GLenum pname, GLfloat param)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glPointParameterfARB");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glPointParameterfARB(pname, param);
}
void APIENTRY interceptedglPointSize(GLfloat size)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glPointSize");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glPointSize(size);
}
void APIENTRY interceptedglTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glTexEnvf");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glTexEnvf(target, pname, param);
}
void APIENTRY interceptedglColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glColorPointer");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glColorPointer(size, type, stride, pointer);
}
void APIENTRY interceptedglVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glVertexPointer");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glVertexPointer(size, type, stride, pointer);
}
void APIENTRY interceptedglColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glColor4ub");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glColor4ub(red, green, blue, alpha);
}
void APIENTRY interceptedglTexCoord2f(GLfloat s, GLfloat t)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glTexCoord2f");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glTexCoord2f(s, t);
}
void APIENTRY interceptedglVertex2i(GLint x, GLint y)
{
	currentConfig3DSettings.increaseFunctionCall();

	if (currentConfig3DSettings.startInterception())
	{
		currentConfig3DSettings.getDrawingBuffer("glVertex2i");
		currentConfig3DSettings.switchCurrentBuffer();
	}
	_glVertex2i(x, y);
}
/************************************************************************************/
void initialiseVariables()
{
	//m_framesGL = 0;
	m_samplesGL = 0;

	
	m_drawBuffer = 0;
	m_readBuffer = 0;
	
	m_verbose = false;
	m_stereoMode = false;
	//m_clearCount = 0;
	m_clearCountPersist = 0;
	m_firstFrameTimeGL = 0.0;
	m_lastFrameTimeGL = 0.0;
	m_quadListGL = 0;
	m_thread = 0;
	
	m_width = 0;
	m_height = 0;
	m_initialised = false;
}

void sendFrame(GLuint drawBuffer)
{
	// set the OpenGL draw buffer destination
	m_target[m_drawBuffer].drawBuffer = drawBuffer;

	// select next draw buffer
	m_drawBuffer = (m_drawBuffer + 1) % m_target.size();

	// count DX frames
	//if (m_stereoMode) ++m_framesDX;
}//sendFrame
void switchBuffer(GLuint drawBuffer)
{
	glDrawBuffer(drawBuffer);
}
unsigned getSamples()
{
	GLint samples = 0;
	glGetIntegerv(GL_SAMPLES, &samples);
	if (samples < 0) samples = 0;
	return static_cast<unsigned>(samples);
}
/************************************************************************************/