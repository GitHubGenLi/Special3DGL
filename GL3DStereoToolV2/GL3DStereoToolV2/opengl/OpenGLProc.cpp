#include "OpenGLProc.h"

#include <stdio.h>

bool g_stereoDetect = false;
unsigned g_clearsPerEye = 0;
unsigned g_clearCount = 0;
HMODULE _libGlHandle = NULL;
HDC currentOpenGLContext;

unsigned m_framesGL;    ///< OpenGL frame count

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

bool useTexture;
bool mustUseBlit;
/************************************************************************************/
void * _getPublicProcAddress(const char *procName)
{
	//Log::open("intercept.log");
	Log::print("_getPublicProcAddress\n");

	if (!_libGlHandle) {
		char szDll[MAX_PATH] = { 0 };

		if (!GetSystemDirectoryA(szDll, MAX_PATH)) {
			return NULL;
		}

		//Log::print("System directory: ");
		//Log::print(szDll);
		
		strcat_s(szDll, "\\opengl32.dll");
		Log::print("System directory: ");
		Log::print(szDll);
		
		Log::print("\n");
		
		_libGlHandle = LoadLibraryA(szDll);

		if (!_libGlHandle) {
			Log::print("couldn't load opengl32.dll\n");
			return NULL;
		}

		Log::print("Can get opengl handle.");

	}
	Log::print("Proc name: ");
	Log::print(procName);
	Log::print("\n");

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
	Log::print("_getAnyGLFuncAddress\n");

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

/************************************************************************************/
int WINAPI interceptedwglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd)
{
	initialiseVariables();

	currentOpenGLContext = hdc; //store the current context
	//Log::open("intercept.log");
	Log::print("OK: get interceptedwglChoosePixelFormat function \n");
	//printf("interceptedwglChoosePixelFormat\n");

	if (!_wglChoosePixelFormat)
	{
		MessageBox(NULL, "_wglChoosePixelFormat not supported", "Error! (interceptedwglChoosePixelFormat)", MB_OK);
		return false;
	}

	PIXELFORMATDESCRIPTOR newppfd = {};
	newppfd.nSize = sizeof(newppfd);

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

	GLint textureSize = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &textureSize);
	
	bool success = false;

	// use textures or renderbuffers?
	useTexture = true;

	if (!glx.load())
		Log::print("error: failed to load GL extensions\n");
	else do {
		Log::print("loaded GL extensions\n");

		// select standard or multisampled GL texture mode
		GLenum textureMode = (m_samplesGL > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

		Log::print("generating render buffers\n");
		unsigned i = 0;
		for (i = 0; i<m_target.size(); ++i) {
			// are we using textures or renderbuffers?
			if (useTexture) {
				// using GL_TEXTURE_2D
				glGenTextures(1, &m_target[i].texture);

				if (m_target[i].texture == 0) {
					Log::print("error: failed to generate texture ID\n");
					break;
				}
			}
			else {
				// using GL_RENDERBUFFER
				glx.glGenRenderbuffers(1, &m_target[i].renderBuffer);

				if (m_target[i].renderBuffer == 0) {
					Log::print("error: failed to generate render buffer ID\n");
					break;
				}
			}

			
			if (m_target[i].object == 0) {
				DWORD error = GetLastError();
				Log::print("error: wglDXRegisterObjectNV failed for render target: ");
					
				break;
			}

			glx.glGenFramebuffers(1, &m_target[i].frameBuffer);

			if (m_target[i].frameBuffer == 0) {
				Log::print("error: glGenFramebuffers failed\n");
				break;
			}

			glx.glBindFramebuffer(GL_FRAMEBUFFER, m_target[i].frameBuffer);
			

			if (useTexture) {
				// using GL_TEXTURE_2D

				// important to lock before using glFramebufferTexture2D
				
					// attach colour buffer texture
					glx.glFramebufferTexture2D(
						GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
						textureMode, m_target[i].texture, 0
						);

			}
			else {
				// using GL_RENDERBUFFER

				// important to lock before using glFramebufferRenderbuffer
				
					// attach colour renderbuffer
					glx.glFramebufferRenderbuffer(
						GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
						GL_RENDERBUFFER, m_target[i].renderBuffer
						);

				
				// this table defines the renderbuffer parameters to be listed
				struct {
					GLenum name;
					const char *text;
				} table[] = {
						{ GL_RENDERBUFFER_WIDTH, "width" },
						{ GL_RENDERBUFFER_HEIGHT, "height" },
						{ GL_RENDERBUFFER_INTERNAL_FORMAT, "format" },
						{ GL_RENDERBUFFER_RED_SIZE, "red" },
						{ GL_RENDERBUFFER_GREEN_SIZE, "green" },
						{ GL_RENDERBUFFER_BLUE_SIZE, "blue" },
						{ GL_RENDERBUFFER_ALPHA_SIZE, "alpha" },
						{ GL_RENDERBUFFER_DEPTH_SIZE, "depth" },
						{ GL_RENDERBUFFER_STENCIL_SIZE, "stencil" },
						{ 0, 0 }
				};

				glx.glBindRenderbuffer(GL_RENDERBUFFER, m_target[i].renderBuffer);

				// query and log all the renderbuffer parameters
				for (int p = 0; table[p].name != 0; ++p) {
					GLint value = 0;
					glx.glGetRenderbufferParameteriv(GL_RENDERBUFFER, table[p].name, &value);
					Log::print("renderBuffer.") << table[p].text << " = " << value << endl;
				}

				glx.glBindRenderbuffer(GL_RENDERBUFFER, 0);
			}

			// log the framebuffer status (should be GL_FRAMEBUFFER_COMPLETE)
			GLenum status = glx.glCheckFramebufferStatus(GL_FRAMEBUFFER);
			Log::print() << "glCheckFramebufferStatus = " << GLFRAMEBUFFERSTATUStoString(status) << endl;
		}

		// successful only if all render buffers were created and initialised
		success = (i == m_target.size());
	} while (0);

	// default OpenGL settings
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	// default viewing system
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	

	return 1;
}

void WINAPI interceptedglClear(GLbitfield mask)
{
	//Log::open("intercept.log");
	Log::print("interceptedglClear\n");

	

	if (!_glClear)
	{
		MessageBox(NULL, "_glClear not supported", "Error! (interceptedglClear)", MB_OK);
		//return false;
		return;
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

	return;
}

BOOL WINAPI interceptedwglSwapBuffers(HDC hdc)
{
	//Log::open("intercept.log");
	Log::print("OK: get interceptedwglSwapBuffers function \n");

	// was stereo detected previously?
	bool wasStereo = g_stereoDetect;
	
	if (!_wglSwapBuffers)
	{
		MessageBox(NULL, "_wglSwapBuffers not supported", "Error! (interceptedwglSwapBuffers)", MB_OK);
		return false;
	}


	// draw to default framebuffer
	glx.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// are we using textures?
	

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
		if (m_target[m_readBuffer].object != 0)  {

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
		
		}
		else
			Log::print() << "unable to lock DX target on paint\n";

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

	return 1;
}
/************************************************************************************/
void initialiseVariables()
{
	m_framesGL = 0;
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
unsigned getSamples()
{
	GLint samples = 0;
	glGetIntegerv(GL_SAMPLES, &samples);
	if (samples < 0) samples = 0;
	return static_cast<unsigned>(samples);
}
/************************************************************************************/