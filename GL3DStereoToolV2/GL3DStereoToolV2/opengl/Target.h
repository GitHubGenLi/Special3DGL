#include <Windows.h>
#include <windef.h>
#include <minwindef.h>
#include <GL/gl.h>

#include <array>
#include <set>

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