#ifndef hive_Quadifier_h
#define hive_Quadifier_h

#include <array>
#include <set>

//#include "OpenGLProc.h"
#include "Event.h"
#include "Extensions.h"

class GLQuadifier {
public:
	/// Constructor
	GLQuadifier();

	/// Destructor
	virtual ~GLQuadifier();


private:
	

public:
	

private:
	

	bool g_stereoDetect = false;    ///< have we detected incoming stereo frames?
	unsigned g_clearsPerEye = 0;    ///< number of glClear calls per eye
	unsigned g_clearCount = 0;    ///< used to count number of glClear calls


	unsigned m_framesGL;    ///< OpenGL frame count
	
	unsigned m_samplesGL;   ///< OpenGL multisamples (or 0)

	unsigned m_drawBuffer;  ///< buffer to draw to
	unsigned m_readBuffer;  ///< buffer to read from

	unsigned m_width;       ///< display width in pixels
	unsigned m_height;      ///< display height in pixels

	bool m_initialised;     ///< has initialisation completed?

	/// Set of all render targets that have been presented
	std::set< unsigned > m_presentedTargets;

	/// Stores all the details of an individual render target
	struct Target {
		
		HANDLE              object;         ///< Handle of interop object
		GLuint              texture;        ///< OpenGL texture
		GLuint              renderBuffer;   ///< OpenGL renderbuffer
		GLuint              frameBuffer;    ///< OpenGL framebuffer
		GLuint              drawBuffer;     ///< OpenGL draw buffer identifier

		/// Default constructor
		Target()
		{

		}

		/// Clear the contents (free stored data)
		void clear() {
		
		}
	};

	std::tr1::array<Target, 3> m_target; ///< DX/GL targets for rendering

	bool     m_verbose;             ///< Verbose logging

	bool     m_stereoMode;          ///< Stereo mode enable/disable
	bool     m_stereoAvailable;     ///< Is quad-buffer stereo available?
	unsigned m_clearCount;          ///< Number of clears per frame
	unsigned m_clearCountPersist;   ///< Persistent number of clears

	double   m_firstFrameTimeGL;    ///< time-stamp of first GL frame
	double   m_lastFrameTimeGL;     ///< time-stamp of last GL frame

	GLuint   m_quadListGL;          ///< GL display list for textured quad

	uintptr_t m_thread;             ///< Handle of the rendering thread

	//HWND m_sourceWindow;            ///< Window handle of source (Direct3D)

	//HANDLE m_interopGLDX;           ///< Handle for the OpenGL/DX interop

	Event m_frameDone;              ///< Signals when frame is rendered out

	Extensions glx;                 ///< Stores the OpenGL extension functions

	//GLWindow m_window;              ///< The OpenGL output window
};

#endif//hive_Quadifier_h