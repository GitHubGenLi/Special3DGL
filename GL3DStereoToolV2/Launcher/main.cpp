#include <iostream>
#include "../GL3DStereoToolV2/common/DLLInject.h"//DLLInject.h
#include "../GL3DStereoToolV2/common/GLWindow.h" //GLWindow.h
#include <GL/gl.h>
#include <GL/wglext.h>

using namespace std;

//This project has been modified from Quadifier

// workaround for issue seen on Windows 7 with NVIDIA 
void triggerStereo()
{
    // attributes for an OpenGL stereo window
    //GLWindow::Attributes attributes;
    //attributes[WGL_STEREO_ARB] = GL_TRUE;

    // create and destroy a small stereo window: this is here to work around
    // an issue with windows failing to switch to quad-buffer mode (although
    // the window is successfully created and reports a stereo pixel format)
    // note: the window is invisible (not shown) but typically causes the
    // windows desktop to flash briefly (first run only) as stereo activates
    //GLWindow window;
    //window.create( 0, "", 0, 0, 0, 8, 8, 0, 0, DefWindowProc, 0, attributes );
    //window.destroy();
}

//-----------------------------------------------------------------------------

int main ( int argc, char **argv )
{
    // trigger stereo mode in Windows by creating a temporary stereo window
    triggerStereo();

    // the name of our DLL to be injected
    static const string moduleName( "GL3DStereoToolV2.dll" );

    // have we got an application filename?
    if ( argc > 1 ) {
        // name of the application executable
        string applicationName( argv[1] );

        // build command line string
        string commandLine;
        for (int i=2; i<argc; ++i) {
            if (i > 2) commandLine += " ";
            commandLine += string(argv[i]);
        }

        // attempt to launch the application and inject the DLL
        if ( !hive::createProcessWithDLL(
            applicationName,    // name of executable
            moduleName,         // name of DLL
            commandLine         // command line options
        ) )
            cerr << "error: failed to start " << applicationName << endl;
    } else {
        // display usage instructions
        cerr << "usage: launcher <executable> [options]\n";
    }

    return 0;
}

//-----------------------------------------------------------------------------
