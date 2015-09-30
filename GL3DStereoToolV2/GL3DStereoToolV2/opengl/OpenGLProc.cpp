#include "OpenGLProc.h"

#include <stdio.h>

bool g_stereoDetect = false;
unsigned g_clearsPerEye = 0;
unsigned g_clearCount = 0;
HMODULE _libGlHandle = NULL;
HDC currentOpenGLContext;

/************************************************************************************/
void * _getPublicProcAddress(const char *procName)
{
	if (!_libGlHandle) {
		char szDll[MAX_PATH] = { 0 };

		if (!GetSystemDirectoryA(szDll, MAX_PATH)) {
			return NULL;
		}

		strcat_s(szDll, "\\opengl32.dll");

		_libGlHandle = LoadLibraryA(szDll);
		if (!_libGlHandle) {
			//os::log("apitrace: error: couldn't load %s\n", szDll);
			return NULL;
		}
	}

	return (void *)GetProcAddress(_libGlHandle, procName);
}

void * _getPrivateProcAddress(const char *procName) {
	return (void *)_wglGetProcAddress(procName);
}
/************************************************************************************/
static PROC __stdcall _fail_wglGetProcAddress(const char * lpszProc) {
	const char *_name = "wglGetProcAddress";
	//os::log("error: unavailable function %s\n", _name);
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
static BOOL __stdcall _fail_wglChoosePixelFormatARB(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats) {
	const char *_name = "wglChoosePixelFormatARB";
	/*os::log("error: unavailable function %s\n", _name);
	os::abort();*/
	return 0;
}

static BOOL __stdcall _get_wglChoosePixelFormatARB(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats) {
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
static BOOL __stdcall _fail_wglSwapBuffers(HDC hdc) {
	const char *_name = "wglSwapBuffers";
	//os::log("error: unavailable function %s\n", _name);
	//os::abort();
	return 0;
}

static BOOL __stdcall _get_wglSwapBuffers(HDC hdc) {
	PFN_WGLSWAPBUFFERS _ptr;
	_ptr = (PFN_WGLSWAPBUFFERS)_getPublicProcAddress("wglSwapBuffers");
	if (!_ptr) {
		_ptr = &_fail_wglSwapBuffers;
	}
	_wglSwapBuffers = _ptr;
	return _wglSwapBuffers(hdc);
}

PFN_WGLSWAPBUFFERS _wglSwapBuffers = &_get_wglSwapBuffers;
/************************************************************************************/
static int __stdcall _fail_wglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd) {
	const char *_name = "wglChoosePixelFormat";
	//os::log("error: unavailable function %s\n", _name);
	//os::abort();
	return 0;
}

static int __stdcall _get_wglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd) {
	PFN_WGLCHOOSEPIXELFORMAT _ptr;
	_ptr = (PFN_WGLCHOOSEPIXELFORMAT)_getPublicProcAddress("wglChoosePixelFormat");
	if (!_ptr) {
		_ptr = &_fail_wglChoosePixelFormat;
	}
	_wglChoosePixelFormat = _ptr;
	return _wglChoosePixelFormat(hdc, ppfd);
}

PFN_WGLCHOOSEPIXELFORMAT _wglChoosePixelFormat = &_get_wglChoosePixelFormat;
/************************************************************************************/
static BOOL __stdcall _fail_wglSetPixelFormat(HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR * ppfd) {
	const char *_name = "wglSetPixelFormat";
	/*os::log("error: unavailable function %s\n", _name);
	os::abort();*/
	return 0;
}

static BOOL __stdcall _get_wglSetPixelFormat(HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR * ppfd) {
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
static void WINAPI _fail_glClear(GLbitfield mask) {
	const char *_name = "glClear";
	//os::log("warning: ignoring call to unavailable function %s\n", _name);
	return;
}

static void WINAPI _get_glClear(GLbitfield mask) {
	PFN_GLCLEAR _ptr;
	_ptr = (PFN_GLCLEAR)_getPublicProcAddress("glClear");
	if (!_ptr) {
		_ptr = &_fail_glClear;
	}
	_glClear = _ptr;
	_glClear(mask);
}

PFN_GLCLEAR _glClear = &_get_glClear;
/************************************************************************************/
static void WINAPI _fail_glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
	const char *_name = "glClearColor";
	//os::log("warning: ignoring call to unavailable function %s\n", _name);
	return;
}

static void WINAPI _get_glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
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
static void WINAPI _fail_glClearDepth(GLdouble depth) {
	const char *_name = "glClearDepth";
	//os::log("warning: ignoring call to unavailable function %s\n", _name);
	return;
}

static void WINAPI _get_glClearDepth(GLdouble depth) {
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
static int __stdcall _fail_wglGetPixelFormat(HDC hdc) {
	const char *_name = "wglGetPixelFormat";
	//os::log("error: unavailable function %s\n", _name);
	//os::abort();
	return 0;
}

static int __stdcall _get_wglGetPixelFormat(HDC hdc) {
	PFN_WGLGETPIXELFORMAT _ptr;
	_ptr = (PFN_WGLGETPIXELFORMAT)_getPublicProcAddress("wglGetPixelFormat");
	if (!_ptr) {
		_ptr = &_fail_wglGetPixelFormat;
	}
	_wglGetPixelFormat = _ptr;
	return _wglGetPixelFormat(hdc);
}

PFN_WGLGETPIXELFORMAT _wglGetPixelFormat = &_get_wglGetPixelFormat;
/************************************************************************************/
static int __stdcall _fail_wglDescribePixelFormat(HDC hdc, int iPixelFormat, UINT nBytes, PIXELFORMATDESCRIPTOR * ppfd) {
	const char *_name = "wglDescribePixelFormat";
	//os::log("error: unavailable function %s\n", _name);
	//os::abort();
	return 0;
}

static int __stdcall _get_wglDescribePixelFormat(HDC hdc, int iPixelFormat, UINT nBytes, PIXELFORMATDESCRIPTOR * ppfd) {
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
BOOL WINAPI interceptedwglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd)
{
	currentOpenGLContext = hdc; //store the current context
		
	printf("interceptedwglChoosePixelFormat\n");

	if (!_wglChoosePixelFormat)
	{
		MessageBox(NULL, "_wglChoosePixelFormat not supported", "Error! (interceptedwglChoosePixelFormat)", MB_OK);
		return false;
	}

	PIXELFORMATDESCRIPTOR newppfd ;

	//copy all existing values
	newppfd.cAccumAlphaBits = ppfd->cAccumAlphaBits;
	newppfd.cAccumBits = ppfd->cAccumBits;
	newppfd.cAccumBlueBits = ppfd->cAccumBlueBits;
	newppfd.cAccumGreenBits = ppfd->cAccumGreenBits;
	newppfd.cAccumRedBits = ppfd->cAccumRedBits;
	newppfd.cAlphaBits = ppfd->cAlphaBits;
	newppfd.cAlphaShift = ppfd->cAlphaShift;
	newppfd.cAuxBuffers = ppfd->cAuxBuffers;
	newppfd.cBlueBits = ppfd->cBlueBits;
	newppfd.cBlueShift = ppfd->cBlueShift;
	newppfd.cColorBits = ppfd->cColorBits;
	newppfd.cDepthBits = ppfd->cDepthBits;
	newppfd.cGreenBits = ppfd->cGreenBits;
	newppfd.cGreenShift = ppfd->cGreenShift;
	newppfd.cRedBits = ppfd->cRedBits;
	newppfd.cRedShift = ppfd->cRedShift;
	newppfd.cStencilBits = ppfd->cStencilBits;
	newppfd.dwDamageMask = ppfd->dwDamageMask;
	newppfd.dwFlags = ppfd->dwFlags;
	newppfd.dwLayerMask = ppfd->dwLayerMask;
	newppfd.dwVisibleMask = ppfd->dwVisibleMask;

	//change to stereo
	newppfd.dwFlags |= PFD_STEREO;
	g_stereoDetect = true;

	BOOL bValidPixFormat;
	
	//call the original function
	bValidPixFormat = _wglChoosePixelFormat(hdc, &newppfd);

	if (!bValidPixFormat)
	{
		MessageBox(NULL, "Invalid Pixel Format", "Error! (interceptedwglChoosePixelFormat)", MB_OK);
		return false;
	}

	return 1;
}

BOOL WINAPI interceptedglClear(GLbitfield mask)
{
	if (!_glClear)
	{
		MessageBox(NULL, "_glClear not supported", "Error! (interceptedglClear)", MB_OK);
		return false;
	}

	if (!g_stereoDetect)
		glDrawBuffer(GL_BACK);
	else if (g_clearCount < g_clearsPerEye)
		glDrawBuffer(GL_BACK_LEFT);
	else
		glDrawBuffer(GL_BACK_RIGHT);

	// call the original function
	_glClear(mask);

	// count the number of glClear calls
	++g_clearCount;

	return 1;
}

BOOL WINAPI interceptedwglSwapBuffers(HDC hdc)
{
	if (!_wglSwapBuffers)
	{
		MessageBox(NULL, "_wglSwapBuffers not supported", "Error! (interceptedwglSwapBuffers)", MB_OK);
		return false;
	}

	// call the original function
	_wglSwapBuffers(hdc);

	// was stereo detected previously?
	bool wasStereo = g_stereoDetect;

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

	return 1;
}
/************************************************************************************/