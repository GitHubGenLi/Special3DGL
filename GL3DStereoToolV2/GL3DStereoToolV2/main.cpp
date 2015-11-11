#include <windows.h>
#include <mhook-lib/mhook.h>
#include <atlconv.h>
#include "Log.h"
#include <iostream>
#include "opengl\OpenGLProc.h"

#include <stdio.h>

#include "Config3DSettings.h"

using namespace hive;
using namespace std;

//This project has been modified from Quadifier
//-----------------------------------------------------------------------------
//
// Copyright (C) 2012-14 James Ward, University of Hull
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//    1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software.
//
//    2. If you use this software in a product, an acknowledgment in the
//    product documentation is required.
//
//    3. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
//    4. This notice may not be removed or altered from any source
//    distribution.
//
//-----------------------------------------------------------------------------
#include "GLWindow.h"

void initialiseRealOpenGLFunctions()
{

	// attributes for an OpenGL stereo window
	GLWindow::Attributes attributes;
	attributes[WGL_STEREO_ARB] = GL_TRUE;

	// create and destroy a small stereo window: this is here to work around
	// an issue with windows failing to switch to quad-buffer mode (although
	// the window is successfully created and reports a stereo pixel format)
	// note: the window is invisible (not shown) but typically causes the
	// windows desktop to flash briefly (first run only) as stereo activates
	GLWindow window;
	window.create(0, "", 0, 0, 0, 8, 8, 0, 0, DefWindowProc, 0, attributes);
	

	_wglChoosePixelFormat = (PFN_WGLCHOOSEPIXELFORMAT)_getAnyGLFuncAddress("wglChoosePixelFormat");
	_glClear = (PFN_GLCLEAR)_getAnyGLFuncAddress("glClear");
	_wglSwapBuffers = (PFN_WGLSWAPBUFFERS)_getAnyGLFuncAddress("wglSwapBuffers");

	//_wglChoosePixelFormat = &_get_wglChoosePixelFormat; //not working with styles
	//_glClear = &_get_glClear;
	//_wglSwapBuffers = &_get_wglSwapBuffers;

	_wglSetPixelFormat = (PFN_WGLSETPIXELFORMAT)_getPublicProcAddress("wglSetPixelFormat");
	_wglChoosePixelFormatARB = (PFN_WGLCHOOSEPIXELFORMATARB)_getAnyGLFuncAddress("wglChoosePixelFormatARB");

	//part 1
	_glColor3f = (PFN_GLCOLOR3F)_getAnyGLFuncAddress("glColor3f");
	_glRotatef = (PFN_GLROTATEF)_getAnyGLFuncAddress("glRotatef");
	_glLoadIdentity = (PFN_GLLOADIDENTITY)_getAnyGLFuncAddress("glLoadIdentity");
	_glMultMatrixf = (PFN_GLMULTMATRIXF)_getAnyGLFuncAddress("glMultMatrixf");
	_glTranslated = (PFN_GLTRANSLATED)_getAnyGLFuncAddress("glTranslated");
	_glTranslatef = (PFN_GLTRANSLATEF)_getAnyGLFuncAddress("glTranslatef");
	_glBegin = (PFN_GLBEGIN)_getAnyGLFuncAddress("glBegin");
	_glVertex3f = (PFN_GLVERTEX3F)_getAnyGLFuncAddress("glVertex3f");
	_glEnd = (PFN_GLEND)_getAnyGLFuncAddress("glEnd");
	_glFlush = (PFN_GLFLUSH)_getAnyGLFuncAddress("glFlush");

	//part 2
	_wglMakeCurrent = (PFN_WGLMAKECURRENT)_getAnyGLFuncAddress("wglMakeCurrent");
	_glScaled = (PFN_GLSCALED)_getAnyGLFuncAddress("glScaled");
	_glColor3d = (PFN_GLCOLOR3D)_getAnyGLFuncAddress("glColor3d");
	_glPushMatrix = (PFN_GLPUSHMATRIX)_getAnyGLFuncAddress("glPushMatrix");
	_glMultMatrixd = (PFN_GLMULTMATRIXD)_getAnyGLFuncAddress("glMultMatrixd");
	_wglCreateContext = (PFN_WGLCREATECONTEXT)_getAnyGLFuncAddress("wglCreateContext");
	_wglDeleteContext = (PFN_WGLDELETECONTEXT)_getAnyGLFuncAddress("wglDeleteContext");
	_wglGetCurrentContext = (PFN_WGLGETCURRENTCONTEXT)_getAnyGLFuncAddress("wglGetCurrentContext");
	_wglCopyContext = (PFN_WGLCOPYCONTEXT)_getAnyGLFuncAddress("wglCopyContext");
	_wglGetCurrentDC = (PFN_WGLGETCURRENTDC)_getAnyGLFuncAddress("wglGetCurrentDC");
	_wglGetDefaultProcAddress = (PFN_WGLGETDEFAULTPROCADDRESS)_getAnyGLFuncAddress("wglGetDefaultProcAddress");
	_wglShareLists = (PFN_WGLSHARELISTS)_getAnyGLFuncAddress("wglShareLists");
	_wglCreateLayerContext = (PFN_WGLCREATELAYERCONTEXT)_getAnyGLFuncAddress("wglCreateLayerContext");
	_wglDescribeLayerPlane = (PFN_WGLDESCRIBELAYERPLANE)_getAnyGLFuncAddress("wglDescribeLayerPlane");
	_wglSetLayerPaletteEntries = (PFN_WGLSETLAYERPALETTEENTRIES)_getAnyGLFuncAddress("wglSetLayerPaletteEntries");
	_wglGetLayerPaletteEntries = (PFN_WGLGETLAYERPALETTEENTRIES)_getAnyGLFuncAddress("wglGetLayerPaletteEntries");
	_wglRealizeLayerPalette = (PFN_WGLREALIZELAYERPALETTE)_getAnyGLFuncAddress("wglRealizeLayerPalette");
	_wglSwapLayerBuffers = (PFN_WGLSWAPLAYERBUFFERS)_getAnyGLFuncAddress("wglSwapLayerBuffers");
	_wglUseFontBitmapsA = (PFN_WGLUSEFONTBITMAPSA)_getAnyGLFuncAddress("wglUseFontBitmapsA");
	_wglUseFontBitmapsW = (PFN_WGLUSEFONTBITMAPSW)_getAnyGLFuncAddress("wglUseFontBitmapsW");
	_wglSwapMultipleBuffers = (PFN_WGLSWAPMULTIPLEBUFFERS)_getAnyGLFuncAddress("wglSwapMultipleBuffers");
	_wglUseFontOutlinesA = (PFN_WGLUSEFONTOUTLINESA)_getAnyGLFuncAddress("wglUseFontOutlinesA");
	_wglUseFontOutlinesW = (PFN_WGLUSEFONTOUTLINESW)_getAnyGLFuncAddress("wglUseFontOutlinesW");
	_wglCreateBufferRegionARB = (PFN_WGLCREATEBUFFERREGIONARB)_getAnyGLFuncAddress("wglCreateBufferRegionARB");
	_wglDeleteBufferRegionARB = (PFN_WGLDELETEBUFFERREGIONARB)_getAnyGLFuncAddress("wglDeleteBufferRegionARB");
	_wglSaveBufferRegionARB = (PFN_WGLSAVEBUFFERREGIONARB)_getAnyGLFuncAddress("wglSaveBufferRegionARB");
	_wglRestoreBufferRegionARB = (PFN_WGLRESTOREBUFFERREGIONARB)_getAnyGLFuncAddress("wglRestoreBufferRegionARB");
	_wglGetExtensionsStringARB = (PFN_WGLGETEXTENSIONSSTRINGARB)_getAnyGLFuncAddress("wglGetExtensionsStringARB");
	_wglGetPixelFormatAttribivARB = (PFN_WGLGETPIXELFORMATATTRIBIVARB)_getAnyGLFuncAddress("wglGetPixelFormatAttribivARB");
	_wglGetPixelFormatAttribfvARB = (PFN_WGLGETPIXELFORMATATTRIBFVARB)_getAnyGLFuncAddress("wglGetPixelFormatAttribfvARB");
	_wglMakeContextCurrentARB = (PFN_WGLMAKECONTEXTCURRENTARB)_getAnyGLFuncAddress("wglMakeContextCurrentARB");
	_wglGetCurrentReadDCARB = (PFN_WGLGETCURRENTREADDCARB)_getAnyGLFuncAddress("wglGetCurrentReadDCARB");
	_wglCreatePbufferARB = (PFN_WGLCREATEPBUFFERARB)_getAnyGLFuncAddress("wglCreatePbufferARB");
	_wglGetPbufferDCARB = (PFN_WGLGETPBUFFERDCARB)_getAnyGLFuncAddress("wglGetPbufferDCARB");
	_wglReleasePbufferDCARB = (PFN_WGLRELEASEPBUFFERDCARB)_getAnyGLFuncAddress("wglReleasePbufferDCARB");
	_wglDestroyPbufferARB = (PFN_WGLDESTROYPBUFFERARB)_getAnyGLFuncAddress("wglDestroyPbufferARB");
	_wglQueryPbufferARB = (PFN_WGLQUERYPBUFFERARB)_getAnyGLFuncAddress("wglQueryPbufferARB");
	_wglBindTexImageARB = (PFN_WGLBINDTEXIMAGEARB)_getAnyGLFuncAddress("wglBindTexImageARB");
	_wglReleaseTexImageARB = (PFN_WGLRELEASETEXIMAGEARB)_getAnyGLFuncAddress("wglReleaseTexImageARB");
	_wglSetPbufferAttribARB = (PFN_WGLSETPBUFFERATTRIBARB)_getAnyGLFuncAddress("wglSetPbufferAttribARB");
	_wglCreateContextAttribsARB = (PFN_WGLCREATECONTEXTATTRIBSARB)_getAnyGLFuncAddress("wglCreateContextAttribsARB");
	/*_wglCreateDisplayColorTableEXT = (PFN_WGLCREATEDISPLAYCOLORTABLEEXT)_getAnyGLFuncAddress("wglCreateDisplayColorTableEXT");
	_wglLoadDisplayColorTableEXT = (PFN_WGLLOADDISPLAYCOLORTABLEEXT)_getAnyGLFuncAddress("wglLoadDisplayColorTableEXT");
	_wglBindDisplayColorTableEXT = (PFN_WGLBINDDISPLAYCOLORTABLEEXT)_getAnyGLFuncAddress("wglBindDisplayColorTableEXT");
	_wglDestroyDisplayColorTableEXT = (PFN_WGLDESTROYDISPLAYCOLORTABLEEXT)_getAnyGLFuncAddress("wglDestroyDisplayColorTableEXT");
	_wglGetExtensionsStringEXT = (PFN_WGLGETEXTENSIONSSTRINGEXT)_getAnyGLFuncAddress("wglGetExtensionsStringEXT");
	_wglMakeContextCurrentEXT = (PFN_WGLMAKECONTEXTCURRENTEXT)_getAnyGLFuncAddress("wglMakeContextCurrentEXT");
	_wglGetCurrentReadDCEXT = (PFN_WGLGETCURRENTREADDCEXT)_getAnyGLFuncAddress("wglGetCurrentReadDCEXT");
	_wglCreatePbufferEXT = (PFN_WGLCREATEPBUFFEREXT)_getAnyGLFuncAddress("wglCreatePbufferEXT");
	_wglGetPbufferDCEXT = (PFN_WGLGETPBUFFERDCEXT)_getAnyGLFuncAddress("wglGetPbufferDCEXT");
	_wglReleasePbufferDCEXT = (PFN_WGLRELEASEPBUFFERDCEXT)_getAnyGLFuncAddress("wglReleasePbufferDCEXT");
	_wglDestroyPbufferEXT = (PFN_WGLDESTROYPBUFFEREXT)_getAnyGLFuncAddress("wglDestroyPbufferEXT");
	_wglQueryPbufferEXT = (PFN_WGLQUERYPBUFFEREXT)_getAnyGLFuncAddress("wglQueryPbufferEXT");
	_wglGetPixelFormatAttribivEXT = (PFN_WGLGETPIXELFORMATATTRIBIVEXT)_getAnyGLFuncAddress("wglGetPixelFormatAttribivEXT");
	_wglGetPixelFormatAttribfvEXT = (PFN_WGLGETPIXELFORMATATTRIBFVEXT)_getAnyGLFuncAddress("wglGetPixelFormatAttribfvEXT");
	_wglChoosePixelFormatEXT = (PFN_WGLCHOOSEPIXELFORMATEXT)_getAnyGLFuncAddress("wglChoosePixelFormatEXT");
	_wglSwapIntervalEXT = (PFN_WGLSWAPINTERVALEXT)_getAnyGLFuncAddress("wglSwapIntervalEXT");
	_wglGetSwapIntervalEXT = (PFN_WGLGETSWAPINTERVALEXT)_getAnyGLFuncAddress("wglGetSwapIntervalEXT");
	*/
	/*_wglAllocateMemoryNV = (PFN_WGLALLOCATEMEMORYNV)_getAnyGLFuncAddress("wglAllocateMemoryNV");
	_wglFreeMemoryNV = (PFN_WGLFREEMEMORYNV)_getAnyGLFuncAddress("wglFreeMemoryNV");
	*/
	/*_wglGetSyncValuesOML = (PFN_WGLGETSYNCVALUESOML)_getAnyGLFuncAddress("wglGetSyncValuesOML");
	_wglGetMscRateOML = (PFN_WGLGETMSCRATEOML)_getAnyGLFuncAddress("wglGetMscRateOML");
	_wglSwapBuffersMscOML = (PFN_WGLSWAPBUFFERSMSCOML)_getAnyGLFuncAddress("wglSwapBuffersMscOML");
	_wglSwapLayerBuffersMscOML = (PFN_WGLSWAPLAYERBUFFERSMSCOML)_getAnyGLFuncAddress("wglSwapLayerBuffersMscOML");
	_wglWaitForMscOML = (PFN_WGLWAITFORMSCOML)_getAnyGLFuncAddress("wglWaitForMscOML");
	_wglWaitForSbcOML = (PFN_WGLWAITFORSBCOML)_getAnyGLFuncAddress("wglWaitForSbcOML");
	_wglGetDigitalVideoParametersI3D = (PFN_WGLGETDIGITALVIDEOPARAMETERSI3D)_getAnyGLFuncAddress("wglGetDigitalVideoParametersI3D");
	_wglSetDigitalVideoParametersI3D = (PFN_WGLSETDIGITALVIDEOPARAMETERSI3D)_getAnyGLFuncAddress("wglSetDigitalVideoParametersI3D");
	_wglGetGammaTableParametersI3D = (PFN_WGLGETGAMMATABLEPARAMETERSI3D)_getAnyGLFuncAddress("wglGetGammaTableParametersI3D");
	_wglSetGammaTableParametersI3D = (PFN_WGLSETGAMMATABLEPARAMETERSI3D)_getAnyGLFuncAddress("wglSetGammaTableParametersI3D");
	_wglGetGammaTableI3D = (PFN_WGLGETGAMMATABLEI3D)_getAnyGLFuncAddress("wglGetGammaTableI3D");
	_wglSetGammaTableI3D = (PFN_WGLSETGAMMATABLEI3D)_getAnyGLFuncAddress("wglSetGammaTableI3D");
	_wglEnableGenlockI3D = (PFN_WGLENABLEGENLOCKI3D)_getAnyGLFuncAddress("wglEnableGenlockI3D");
	_wglDisableGenlockI3D = (PFN_WGLDISABLEGENLOCKI3D)_getAnyGLFuncAddress("wglDisableGenlockI3D");
	_wglIsEnabledGenlockI3D = (PFN_WGLISENABLEDGENLOCKI3D)_getAnyGLFuncAddress("wglIsEnabledGenlockI3D");
	_wglGenlockSourceI3D = (PFN_WGLGENLOCKSOURCEI3D)_getAnyGLFuncAddress("wglGenlockSourceI3D");
	_wglGetGenlockSourceI3D = (PFN_WGLGETGENLOCKSOURCEI3D)_getAnyGLFuncAddress("wglGetGenlockSourceI3D");
	_wglGenlockSourceEdgeI3D = (PFN_WGLGENLOCKSOURCEEDGEI3D)_getAnyGLFuncAddress("wglGenlockSourceEdgeI3D");
	_wglGetGenlockSourceEdgeI3D = (PFN_WGLGETGENLOCKSOURCEEDGEI3D)_getAnyGLFuncAddress("wglGetGenlockSourceEdgeI3D");
	_wglGenlockSampleRateI3D = (PFN_WGLGENLOCKSAMPLERATEI3D)_getAnyGLFuncAddress("wglGenlockSampleRateI3D");
	_wglGetGenlockSampleRateI3D = (PFN_WGLGETGENLOCKSAMPLERATEI3D)_getAnyGLFuncAddress("wglGetGenlockSampleRateI3D");
	_wglGenlockSourceDelayI3D = (PFN_WGLGENLOCKSOURCEDELAYI3D)_getAnyGLFuncAddress("wglGenlockSourceDelayI3D");
	_wglGetGenlockSourceDelayI3D = (PFN_WGLGETGENLOCKSOURCEDELAYI3D)_getAnyGLFuncAddress("wglGetGenlockSourceDelayI3D");
	_wglQueryGenlockMaxSourceDelayI3D = (PFN_WGLQUERYGENLOCKMAXSOURCEDELAYI3D)_getAnyGLFuncAddress("wglQueryGenlockMaxSourceDelayI3D");
	_wglCreateImageBufferI3D = (PFN_WGLCREATEIMAGEBUFFERI3D)_getAnyGLFuncAddress("wglCreateImageBufferI3D");
	_wglDestroyImageBufferI3D = (PFN_WGLDESTROYIMAGEBUFFERI3D)_getAnyGLFuncAddress("wglDestroyImageBufferI3D");
	_wglAssociateImageBufferEventsI3D = (PFN_WGLASSOCIATEIMAGEBUFFEREVENTSI3D)_getAnyGLFuncAddress("wglAssociateImageBufferEventsI3D");
	_wglReleaseImageBufferEventsI3D = (PFN_WGLRELEASEIMAGEBUFFEREVENTSI3D)_getAnyGLFuncAddress("wglReleaseImageBufferEventsI3D");
	_wglEnableFrameLockI3D = (PFN_WGLENABLEFRAMELOCKI3D)_getAnyGLFuncAddress("wglEnableFrameLockI3D");
	_wglDisableFrameLockI3D = (PFN_WGLDISABLEFRAMELOCKI3D)_getAnyGLFuncAddress("wglDisableFrameLockI3D");
	_wglIsEnabledFrameLockI3D = (PFN_WGLISENABLEDFRAMELOCKI3D)_getAnyGLFuncAddress("wglIsEnabledFrameLockI3D");
	_wglQueryFrameLockMasterI3D = (PFN_WGLQUERYFRAMELOCKMASTERI3D)_getAnyGLFuncAddress("wglQueryFrameLockMasterI3D");
	_wglGetFrameUsageI3D = (PFN_WGLGETFRAMEUSAGEI3D)_getAnyGLFuncAddress("wglGetFrameUsageI3D");
	_wglBeginFrameTrackingI3D = (PFN_WGLBEGINFRAMETRACKINGI3D)_getAnyGLFuncAddress("wglBeginFrameTrackingI3D");
	_wglEndFrameTrackingI3D = (PFN_WGLENDFRAMETRACKINGI3D)_getAnyGLFuncAddress("wglEndFrameTrackingI3D");
	_wglQueryFrameTrackingI3D = (PFN_WGLQUERYFRAMETRACKINGI3D)_getAnyGLFuncAddress("wglQueryFrameTrackingI3D");
	_wglSetStereoEmitterState3DL = (PFN_WGLSETSTEREOEMITTERSTATE3DL)_getAnyGLFuncAddress("wglSetStereoEmitterState3DL");
	*/
	/*_wglEnumerateVideoDevicesNV = (PFN_WGLENUMERATEVIDEODEVICESNV)_getAnyGLFuncAddress("wglEnumerateVideoDevicesNV");
	_wglBindVideoDeviceNV = (PFN_WGLBINDVIDEODEVICENV)_getAnyGLFuncAddress("wglBindVideoDeviceNV");
	_wglQueryCurrentContextNV = (PFN_WGLQUERYCURRENTCONTEXTNV)_getAnyGLFuncAddress("wglQueryCurrentContextNV");
	_wglGetVideoDeviceNV = (PFN_WGLGETVIDEODEVICENV)_getAnyGLFuncAddress("wglGetVideoDeviceNV");
	_wglReleaseVideoDeviceNV = (PFN_WGLRELEASEVIDEODEVICENV)_getAnyGLFuncAddress("wglReleaseVideoDeviceNV");
	_wglBindVideoImageNV = (PFN_WGLBINDVIDEOIMAGENV)_getAnyGLFuncAddress("wglBindVideoImageNV");
	_wglReleaseVideoImageNV = (PFN_WGLRELEASEVIDEOIMAGENV)_getAnyGLFuncAddress("wglReleaseVideoImageNV");
	_wglSendPbufferToVideoNV = (PFN_WGLSENDPBUFFERTOVIDEONV)_getAnyGLFuncAddress("wglSendPbufferToVideoNV");
	_wglGetVideoInfoNV = (PFN_WGLGETVIDEOINFONV)_getAnyGLFuncAddress("wglGetVideoInfoNV");
	_wglJoinSwapGroupNV = (PFN_WGLJOINSWAPGROUPNV)_getAnyGLFuncAddress("wglJoinSwapGroupNV");
	_wglBindSwapBarrierNV = (PFN_WGLBINDSWAPBARRIERNV)_getAnyGLFuncAddress("wglBindSwapBarrierNV");
	_wglQuerySwapGroupNV = (PFN_WGLQUERYSWAPGROUPNV)_getAnyGLFuncAddress("wglQuerySwapGroupNV");
	_wglQueryMaxSwapGroupsNV = (PFN_WGLQUERYMAXSWAPGROUPSNV)_getAnyGLFuncAddress("wglQueryMaxSwapGroupsNV");
	_wglQueryFrameCountNV = (PFN_WGLQUERYFRAMECOUNTNV)_getAnyGLFuncAddress("wglQueryFrameCountNV");
	_wglResetFrameCountNV = (PFN_WGLRESETFRAMECOUNTNV)_getAnyGLFuncAddress("wglResetFrameCountNV");
	_wglEnumGpusNV = (PFN_WGLENUMGPUSNV)_getAnyGLFuncAddress("wglEnumGpusNV");
	_wglEnumGpuDevicesNV = (PFN_WGLENUMGPUDEVICESNV)_getAnyGLFuncAddress("wglEnumGpuDevicesNV");
	_wglCreateAffinityDCNV = (PFN_WGLCREATEAFFINITYDCNV)_getAnyGLFuncAddress("wglCreateAffinityDCNV");
	_wglEnumGpusFromAffinityDCNV = (PFN_WGLENUMGPUSFROMAFFINITYDCNV)_getAnyGLFuncAddress("wglEnumGpusFromAffinityDCNV");
	_wglDeleteDCNV = (PFN_WGLDELETEDCNV)_getAnyGLFuncAddress("wglDeleteDCNV");
	*/
	/*_wglGetGPUIDsAMD = (PFN_WGLGETGPUIDSAMD)_getAnyGLFuncAddress("wglGetGPUIDsAMD");
	_wglGetGPUInfoAMD = (PFN_WGLGETGPUINFOAMD)_getAnyGLFuncAddress("wglGetGPUInfoAMD");
	_wglGetContextGPUIDAMD = (PFN_WGLGETCONTEXTGPUIDAMD)_getAnyGLFuncAddress("wglGetContextGPUIDAMD");
	_wglCreateAssociatedContextAMD = (PFN_WGLCREATEASSOCIATEDCONTEXTAMD)_getAnyGLFuncAddress("wglCreateAssociatedContextAMD");
	_wglCreateAssociatedContextAttribsAMD = (PFN_WGLCREATEASSOCIATEDCONTEXTATTRIBSAMD)_getAnyGLFuncAddress("wglCreateAssociatedContextAttribsAMD");
	_wglDeleteAssociatedContextAMD = (PFN_WGLDELETEASSOCIATEDCONTEXTAMD)_getAnyGLFuncAddress("wglDeleteAssociatedContextAMD");
	_wglMakeAssociatedContextCurrentAMD = (PFN_WGLMAKEASSOCIATEDCONTEXTCURRENTAMD)_getAnyGLFuncAddress("wglMakeAssociatedContextCurrentAMD");
	_wglGetCurrentAssociatedContextAMD = (PFN_WGLGETCURRENTASSOCIATEDCONTEXTAMD)_getAnyGLFuncAddress("wglGetCurrentAssociatedContextAMD");
	_wglBlitContextFramebufferAMD = (PFN_WGLBLITCONTEXTFRAMEBUFFERAMD)_getAnyGLFuncAddress("wglBlitContextFramebufferAMD");
	*/
	/*_wglBindVideoCaptureDeviceNV = (PFN_WGLBINDVIDEOCAPTUREDEVICENV)_getAnyGLFuncAddress("wglBindVideoCaptureDeviceNV");
	_wglEnumerateVideoCaptureDevicesNV = (PFN_WGLENUMERATEVIDEOCAPTUREDEVICESNV)_getAnyGLFuncAddress("wglEnumerateVideoCaptureDevicesNV");
	_wglLockVideoCaptureDeviceNV = (PFN_WGLLOCKVIDEOCAPTUREDEVICENV)_getAnyGLFuncAddress("wglLockVideoCaptureDeviceNV");
	_wglQueryVideoCaptureDeviceNV = (PFN_WGLQUERYVIDEOCAPTUREDEVICENV)_getAnyGLFuncAddress("wglQueryVideoCaptureDeviceNV");
	_wglReleaseVideoCaptureDeviceNV = (PFN_WGLRELEASEVIDEOCAPTUREDEVICENV)_getAnyGLFuncAddress("wglReleaseVideoCaptureDeviceNV");
	_wglCopyImageSubDataNV = (PFN_WGLCOPYIMAGESUBDATANV)_getAnyGLFuncAddress("wglCopyImageSubDataNV");
	_wglDXSetResourceShareHandleNV = (PFN_WGLDXSETRESOURCESHAREHANDLENV)_getAnyGLFuncAddress("wglDXSetResourceShareHandleNV");
	_wglDXOpenDeviceNV = (PFN_WGLDXOPENDEVICENV)_getAnyGLFuncAddress("wglDXOpenDeviceNV");
	_wglDXCloseDeviceNV = (PFN_WGLDXCLOSEDEVICENV)_getAnyGLFuncAddress("wglDXCloseDeviceNV");
	_wglDXRegisterObjectNV = (PFN_WGLDXREGISTEROBJECTNV)_getAnyGLFuncAddress("wglDXRegisterObjectNV");
	_wglDXUnregisterObjectNV = (PFN_WGLDXUNREGISTEROBJECTNV)_getAnyGLFuncAddress("wglDXUnregisterObjectNV");
	_wglDXObjectAccessNV = (PFN_WGLDXOBJECTACCESSNV)_getAnyGLFuncAddress("wglDXObjectAccessNV");
	_wglDXLockObjectsNV = (PFN_WGLDXLOCKOBJECTSNV)_getAnyGLFuncAddress("wglDXLockObjectsNV");
	_wglDXUnlockObjectsNV = (PFN_WGLDXUNLOCKOBJECTSNV)_getAnyGLFuncAddress("wglDXUnlockObjectsNV");
	_wglDelayBeforeSwapNV = (PFN_WGLDELAYBEFORESWAPNV)_getAnyGLFuncAddress("wglDelayBeforeSwapNV");
	*/

	_glGetIntegerv = (PFN_GLGETINTEGERV)_getAnyGLFuncAddress("glGetIntegerv");
	_glGetBooleanv = (PFN_GLGETBOOLEANV)_getAnyGLFuncAddress("glGetBooleanv");
	_glDisable = (PFN_GLDISABLE)_getAnyGLFuncAddress("glDisable");
	_glVertex2d = (PFN_GLVERTEX2D)_getAnyGLFuncAddress("glVertex2d");
	_glCallList = (PFN_GLCALLLIST)_getAnyGLFuncAddress("glCallList");
	_glMaterialfv = (PFN_GLMATERIALFV)_getAnyGLFuncAddress("glMaterialfv");
	_glEnable = (PFN_GLENABLE)_getAnyGLFuncAddress("glEnable");
	_glPopMatrix = (PFN_GLPOPMATRIX)_getAnyGLFuncAddress("glPopMatrix");
	_glTexEnvi = (PFN_GLTEXENVI)_getAnyGLFuncAddress("glTexEnvi");
	_glBindTexture = (PFN_GLBINDTEXTURE)_getAnyGLFuncAddress("glBindTexture");
	_glVertex3d = (PFN_GLVERTEX3D)_getAnyGLFuncAddress("glVertex3d");
	_glTexCoord2d = (PFN_GLTEXCOORD2D)_getAnyGLFuncAddress("glTexCoord2d");
	_glRotated = (PFN_GLROTATED)_getAnyGLFuncAddress("glRotated");
	_glPushAttrib = (PFN_GLPUSHATTRIB)_getAnyGLFuncAddress("glPushAttrib");
	_glPopAttrib = (PFN_GLPOPATTRIB)_getAnyGLFuncAddress("glPopAttrib");
	_glColor4f = (PFN_GLCOLOR4F)_getAnyGLFuncAddress("glColor4f");
	_glVertex2f = (PFN_GLVERTEX2F)_getAnyGLFuncAddress("glVertex2f");
	_glBlendFunc = (PFN_GLBLENDFUNC)_getAnyGLFuncAddress("glBlendFunc");

	_glFrustum = (PFN_GLFRUSTUM)_getAnyGLFuncAddress("glFrustum");
	_glViewport = (PFN_GLVIEWPORT)_getAnyGLFuncAddress("glViewport");
	_glMatrixMode = (PFN_GLMATRIXMODE)_getAnyGLFuncAddress("glMatrixMode");
	_glPolygonMode = (PFN_GLPOLYGONMODE)_getAnyGLFuncAddress("glPolygonMode");
	_glClearColor = (PFN_GLCLEARCOLOR)_getAnyGLFuncAddress("glClearColor");
	_glScalef = (PFN_GLSCALEF)_getAnyGLFuncAddress("glScalef");
	_glHint = (PFN_GLHINT)_getAnyGLFuncAddress("glHint");
	_glFogfv = (PFN_GLFOGFV)_getAnyGLFuncAddress("glFogfv");
	_glFogf = (PFN_GLFOGF)_getAnyGLFuncAddress("glFogf");
	
	_glColorMaterial = (PFN_GLCOLORMATERIAL)_getAnyGLFuncAddress("glColorMaterial");
	_glLightModelfv = (PFN_GLLIGHTMODELFV)_getAnyGLFuncAddress("glLightModelfv");
	_glLightfv = (PFN_GLLIGHTFV)_getAnyGLFuncAddress("glLightfv");
	_glGetFloatv = (PFN_GLGETFLOATV)_getAnyGLFuncAddress("glGetFloatv");
	_glBindBufferARB = (PFN_GLBINDBUFFERARB)_getAnyGLFuncAddress("glBindBufferARB");
	_glInterleavedArrays = (PFN_GLINTERLEAVEDARRAYS)_getAnyGLFuncAddress("glInterleavedArrays");
	_glDrawArrays = (PFN_GLDRAWARRAYS)_getAnyGLFuncAddress("glDrawArrays");
	_glUseProgram = (PFN_GLUSEPROGRAM)_getAnyGLFuncAddress("glUseProgram");
	_glGetUniformLocationARB = (PFN_GLGETUNIFORMLOCATIONARB)_getAnyGLFuncAddress("glGetUniformLocationARB");
	_glUniform4f = (PFN_GLUNIFORM4F)_getAnyGLFuncAddress("glUniform4f");
	_glUniform1f = (PFN_GLUNIFORM1F)_getAnyGLFuncAddress("glUniform1f");
	_glUniform1i = (PFN_GLUNIFORM1I)_getAnyGLFuncAddress("glUniform1i");
	_glActiveTextureARB = (PFN_GLACTIVETEXTUREARB)_getAnyGLFuncAddress("glActiveTextureARB");
	_glMultiTexCoord2fARB = (PFN_GLMULTITEXCOORD2FARB)_getAnyGLFuncAddress("glMultiTexCoord2fARB");
	_glDepthMask = (PFN_GLDEPTHMASK)_getAnyGLFuncAddress("glDepthMask");
	_glEnableClientState = (PFN_GLENABLECLIENTSTATE)_getAnyGLFuncAddress("glEnableClientState");
	_glPointParameterfvARB = (PFN_GLPOINTPARAMETERFVARB)_getAnyGLFuncAddress("glPointParameterfvARB");
	_glPointParameterfARB = (PFN_GLPOINTPARAMETERFARB)_getAnyGLFuncAddress("glPointParameterfARB");
	_glPointSize = (PFN_GLPOINTSIZE)_getAnyGLFuncAddress("glPointSize");
	_glTexEnvf = (PFN_GLTEXENVF)_getAnyGLFuncAddress("glTexEnvf");
	_glColorPointer = (PFN_GLCOLORPOINTER)_getAnyGLFuncAddress("glColorPointer");
	_glVertexPointer = (PFN_GLVERTEXPOINTER)_getAnyGLFuncAddress("glVertexPointer");
	_glColor4ub = (PFN_GLCOLOR4UB)_getAnyGLFuncAddress("glColor4ub");
	_glTexCoord2f = (PFN_GLTEXCOORD2F)_getAnyGLFuncAddress("glTexCoord2f");
	_glVertex2i = (PFN_GLVERTEX2I)_getAnyGLFuncAddress("glVertex2i");
	_glLoadIdentity = (PFN_GLLOADIDENTITY)_getAnyGLFuncAddress("glLoadIdentity");



	window.destroy();
}

void processAttach()
{
	//bool isOpened = Log::open("intercept.log");
	Log::print("DLL_PROCESS_ATTACH\n");

	printf("processAttach\n");
	
	initialiseRealOpenGLFunctions(); 

	if ((_wglChoosePixelFormat == 0) || !Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglChoosePixelFormat), interceptedwglChoosePixelFormat))
	{
		cerr << "Failed to hook _wglChoosePixelFormat" << endl;
		MessageBox(0, "Failed to hook _wglChoosePixelFormat", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _wglChoosePixelFormat\n");
	}
	if ((_wglSetPixelFormat == 0) || !Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSetPixelFormat), interceptedwglSetPixelFormat))
	{
		cerr << "Failed to hook _wglSetPixelFormat" << endl;
		MessageBox(0, "Failed to hook _wglSetPixelFormat", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _wglSetPixelFormat\n");
	}

	if ((_glClear == 0) || !Mhook_SetHook(reinterpret_cast<PVOID*>(&_glClear), interceptedglClear))
	{
		cerr << "Failed to hook _glClear" << endl;
		MessageBox(0, "Failed to hook _glClear", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glClear\n");
	}
	if ((_wglSwapBuffers == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSwapBuffers), interceptedwglSwapBuffers))
	{
		cerr << "Failed to hook _wglSwapBuffers" << endl;
		MessageBox(0, "Failed to hook _wglSwapBuffers", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _wglSwapBuffers\n");
	}
	if ((_glColor3f == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glColor3f), interceptedglColor3f))
	{
		cerr << "Failed to hook _glColor3f" << endl;
		MessageBox(0, "Failed to hook _glColor3f", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glColor3f\n");
	}
	if ((_glRotatef == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glRotatef), interceptedglRotatef))
	{
		cerr << "Failed to hook _glRotatef" << endl;
		MessageBox(0, "Failed to hook _glRotatef", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glRotatef\n");
	}
	if ((_glLoadIdentity == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glLoadIdentity), interceptedglLoadIdentity))
	{
		cerr << "Failed to hook _glLoadIdentity" << endl;
		MessageBox(0, "Failed to hook _glLoadIdentity", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glLoadIdentity\n");
	}
	if ((_glMultMatrixf == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glMultMatrixf), interceptedglMultMatrixf))
	{
		cerr << "Failed to hook _glMultMatrixf" << endl;
		MessageBox(0, "Failed to hook _glMultMatrixf", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glMultMatrixf\n");
	}
	if ((_glTranslated == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glTranslated), interceptedglTranslated))
	{
		cerr << "Failed to hook _glTranslated" << endl;
		MessageBox(0, "Failed to hook _glTranslated", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glTranslated\n");
	}
	if ((_glTranslatef == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glTranslatef), interceptedglTranslatef))
	{
		cerr << "Failed to hook _glTranslatef" << endl;
		MessageBox(0, "Failed to hook _glTranslatef", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glTranslatef\n");
	}
	if ((_glBegin == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glBegin), interceptedglBegin))
	{
		cerr << "Failed to hook _glBegin" << endl;
		MessageBox(0, "Failed to hook _glBegin", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glBegin\n");
	}
	if ((_glVertex3f == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glVertex3f), interceptedglVertex3f))
	{
		cerr << "Failed to hook _glVertex3f" << endl;
		MessageBox(0, "Failed to hook _glVertex3f", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glVertex3f\n");
	}
	if ((_glEnd == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glEnd), interceptedglEnd))
	{
		cerr << "Failed to hook _glEnd" << endl;
		MessageBox(0, "Failed to hook _glEnd", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glEnd\n");
	}
	if ((_glFlush == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glFlush), interceptedglFlush))
	{
		cerr << "Failed to hook _glFlush" << endl;
		MessageBox(0, "Failed to hook _glFlush", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _glFlush\n");
	}
	if ((_wglMakeCurrent == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglMakeCurrent), interceptedwglMakeCurrent))
	{
		cerr << "Failed to hook _wglMakeCurrent" << endl;
	}
	else
	{
		printf("Hooked _wglMakeCurrent\n");
	}
	if ((_glScaled == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glScaled), interceptedglScaled))
	{
		cerr << "Failed to hook _glScaled" << endl;
	}
	else
	{
		printf("Hooked _glScaled\n");
	}
	if ((_glColor3d == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glColor3d), interceptedglColor3d))
	{
		cerr << "Failed to hook _glColor3d" << endl;
	}
	else
	{
		printf("Hooked _glColor3d\n");
	}
	if ((_glPushMatrix == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glPushMatrix), interceptedglPushMatrix))
	{
		cerr << "Failed to hook _glPushMatrix" << endl;
	}
	else
	{
		printf("Hooked _glPushMatrix\n");
	}
	if ((_glMultMatrixd == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glMultMatrixd), interceptedglMultMatrixd))
	{
		cerr << "Failed to hook _glMultMatrixd" << endl;
	}
	else
	{
		printf("Hooked _glMultMatrixd\n");
	}
	if ((_wglCreateContext == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglCreateContext), interceptedwglCreateContext))
	{
		cerr << "Failed to hook _wglCreateContext" << endl;
	}
	else
	{
		printf("Hooked _wglCreateContext\n");
	}
	if ((_wglDeleteContext == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDeleteContext), interceptedwglDeleteContext))
	{
		cerr << "Failed to hook _wglDeleteContext" << endl;
	}
	else
	{
		printf("Hooked _wglDeleteContext\n");
	}
	if ((_wglGetCurrentContext == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetCurrentContext), interceptedwglGetCurrentContext))
	{
		cerr << "Failed to hook _glLoadIdentity" << endl;
	}
	else
	{
		printf("Hooked _wglGetCurrentContext\n");
	}
	if ((_wglCopyContext == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglCopyContext), interceptedwglCopyContext))
	{
		cerr << "Failed to hook _glLoadIdentity" << endl;
	}
	else
	{
		printf("Hooked _wglCopyContext\n");
	}
	if ((_wglGetCurrentDC == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetCurrentDC), interceptedwglGetCurrentDC))
	{
		cerr << "Failed to hook _wglGetCurrentDC" << endl;
	}
	else
	{
		printf("Hooked _wglGetCurrentDC\n");
	}
	if ((_wglGetDefaultProcAddress == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetDefaultProcAddress), interceptedwglGetDefaultProcAddress))
	{
		cerr << "Failed to hook _wglGetDefaultProcAddress" << endl;
	}
	else
	{
		printf("Hooked _wglGetDefaultProcAddress\n");
	}
	if ((_wglShareLists == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglShareLists), interceptedwglShareLists))
	{
		cerr << "Failed to hook _wglShareLists" << endl;
	}
	else
	{
		printf("Hooked _wglShareLists\n");
	}
	if ((_wglCreateLayerContext == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglCreateLayerContext), interceptedwglCreateLayerContext))
	{
		cerr << "Failed to hook _wglCreateLayerContext" << endl;
	}
	else
	{
		printf("Hooked _wglCreateLayerContext\n");
	}
	if ((_wglDescribeLayerPlane == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDescribeLayerPlane), interceptedwglDescribeLayerPlane))
	{
		cerr << "Failed to hook _wglDescribeLayerPlane" << endl;
	}
	else
	{
		printf("Hooked _wglDescribeLayerPlane\n");
	}
	if ((_wglSetLayerPaletteEntries == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSetLayerPaletteEntries), interceptedwglSetLayerPaletteEntries))
	{
		cerr << "Failed to hook _wglSetLayerPaletteEntries" << endl;
	}
	else
	{
		printf("Hooked _wglSetLayerPaletteEntries\n");
	}
	if ((_wglGetLayerPaletteEntries == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetLayerPaletteEntries), interceptedwglGetLayerPaletteEntries))
	{
		cerr << "Failed to hook _wglGetLayerPaletteEntries" << endl;
	}
	else
	{
		printf("Hooked _wglGetLayerPaletteEntries\n");
	}
	if ((_wglRealizeLayerPalette == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglRealizeLayerPalette), interceptedwglRealizeLayerPalette))
	{
		cerr << "Failed to hook _wglRealizeLayerPalette" << endl;
	}
	else
	{
		printf("Hooked _wglRealizeLayerPalette\n");
	}
	if ((_wglSwapLayerBuffers == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSwapLayerBuffers), interceptedwglSwapLayerBuffers))
	{
		cerr << "Failed to hook _wglSwapLayerBuffers" << endl;
	}
	else
	{
		printf("Hooked _wglSwapLayerBuffers\n");
	}
	if ((_wglUseFontBitmapsA == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglUseFontBitmapsA), interceptedwglUseFontBitmapsA))
	{
		cerr << "Failed to hook _wglUseFontBitmapsA" << endl;
	}
	else
	{
		printf("Hooked _wglUseFontBitmapsA\n");
	}
	if ((_wglUseFontBitmapsW == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglUseFontBitmapsW), interceptedwglUseFontBitmapsW))
	{
		cerr << "Failed to hook _wglUseFontBitmapsW" << endl;
	}
	else
	{
		printf("Hooked _wglUseFontBitmapsW\n");
	}
	if ((_wglSwapMultipleBuffers == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSwapMultipleBuffers), interceptedwglSwapMultipleBuffers))
	{
		cerr << "Failed to hook _wglSwapMultipleBuffers" << endl;
	}
	else
	{
		printf("Hooked _wglSwapMultipleBuffers\n");
	}
	if ((_wglUseFontOutlinesA == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglUseFontOutlinesA), interceptedwglUseFontOutlinesA))
	{
		cerr << "Failed to hook _wglUseFontOutlinesA" << endl;
	}
	else
	{
		printf("Hooked _wglUseFontOutlinesA\n");
	}
	if ((_wglUseFontOutlinesW == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglUseFontOutlinesW), interceptedwglUseFontOutlinesW))
	{
		cerr << "Failed to hook _wglUseFontOutlinesW" << endl;
	}
	else
	{
		printf("Hooked _wglUseFontOutlinesW\n");
	}
	if ((_wglCreateBufferRegionARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglCreateBufferRegionARB), interceptedwglCreateBufferRegionARB))
	{
		cerr << "Failed to hook _wglCreateBufferRegionARB" << endl;
	}
	else
	{
		printf("Hooked _wglCreateBufferRegionARB\n");
	}
	if ((_wglDeleteBufferRegionARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDeleteBufferRegionARB), interceptedwglDeleteBufferRegionARB))
	{
		cerr << "Failed to hook _wglDeleteBufferRegionARB" << endl;
	}
	else
	{
		printf("Hooked _wglDeleteBufferRegionARB\n");
	}
	if ((_wglSaveBufferRegionARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSaveBufferRegionARB), interceptedwglSaveBufferRegionARB))
	{
		cerr << "Failed to hook _wglSaveBufferRegionARB" << endl;
	}
	else
	{
		printf("Hooked _wglSaveBufferRegionARB\n");
	}
	if ((_wglRestoreBufferRegionARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglRestoreBufferRegionARB), interceptedwglRestoreBufferRegionARB))
	{
		cerr << "Failed to hook _wglRestoreBufferRegionARB" << endl;
	}
	else
	{
		printf("Hooked _wglRestoreBufferRegionARB\n");
	}
	if ((_wglGetExtensionsStringARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetExtensionsStringARB), interceptedwglGetExtensionsStringARB))
	{
		cerr << "Failed to hook _wglGetExtensionsStringARB" << endl;
	}
	else
	{
		printf("Hooked _wglGetExtensionsStringARB\n");
	}
	if ((_wglGetPixelFormatAttribivARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetPixelFormatAttribivARB), interceptedwglGetPixelFormatAttribivARB))
	{
		cerr << "Failed to hook _wglGetPixelFormatAttribivARB" << endl;
	}
	else
	{
		printf("Hooked _wglGetPixelFormatAttribivARB\n");
	}
	if ((_wglGetPixelFormatAttribfvARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetPixelFormatAttribfvARB), interceptedwglGetPixelFormatAttribfvARB))
	{
		cerr << "Failed to hook _wglGetPixelFormatAttribfvARB" << endl;
	}
	else
	{
		printf("Hooked _wglGetPixelFormatAttribfvARB\n");
	}
	if ((_wglMakeContextCurrentARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglMakeContextCurrentARB), interceptedwglMakeContextCurrentARB))
	{
		cerr << "Failed to hook _wglMakeContextCurrentARB" << endl;
	}
	else
	{
		printf("Hooked _wglMakeContextCurrentARB\n");
	}
	if ((_wglGetCurrentReadDCARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetCurrentReadDCARB), interceptedwglGetCurrentReadDCARB))
	{
		cerr << "Failed to hook _wglGetCurrentReadDCARB" << endl;
	}
	else
	{
		printf("Hooked _wglGetCurrentReadDCARB\n");
	}
	if ((_wglCreatePbufferARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglCreatePbufferARB), interceptedwglCreatePbufferARB))
	{
		cerr << "Failed to hook _wglCreatePbufferARB" << endl;
	}
	else
	{
		printf("Hooked _wglCreatePbufferARB\n");
	}
	if ((_wglGetPbufferDCARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetPbufferDCARB), interceptedwglGetPbufferDCARB))
	{
		cerr << "Failed to hook _wglGetPbufferDCARB" << endl;
	}
	else
	{
		printf("Hooked _wglGetPbufferDCARB\n");
	}
	if ((_wglReleasePbufferDCARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglReleasePbufferDCARB), interceptedwglReleasePbufferDCARB))
	{
		cerr << "Failed to hook _wglReleasePbufferDCARB" << endl;
	}
	else
	{
		printf("Hooked _wglReleasePbufferDCARB\n");
	}
	if ((_wglDestroyPbufferARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDestroyPbufferARB), interceptedwglDestroyPbufferARB))
	{
		cerr << "Failed to hook _wglDestroyPbufferARB" << endl;
	}
	else
	{
		printf("Hooked _wglDestroyPbufferARB\n");
	}
	if ((_wglQueryPbufferARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglQueryPbufferARB), interceptedwglQueryPbufferARB))
	{
		cerr << "Failed to hook _wglQueryPbufferARB" << endl;
	}
	else
	{
		printf("Hooked _wglQueryPbufferARB\n");
	}
	if ((_wglBindTexImageARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglBindTexImageARB), interceptedwglBindTexImageARB))
	{
		cerr << "Failed to hook _wglBindTexImageARB" << endl;
	}
	else
	{
		printf("Hooked _wglBindTexImageARB\n");
	}
	if ((_wglReleaseTexImageARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglReleaseTexImageARB), interceptedwglReleaseTexImageARB))
	{
		cerr << "Failed to hook _wglReleaseTexImageARB" << endl;
	}
	else
	{
		printf("Hooked _wglReleaseTexImageARB\n");
	}
	if ((_wglSetPbufferAttribARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSetPbufferAttribARB), interceptedwglSetPbufferAttribARB))
	{
		cerr << "Failed to hook _wglSetPbufferAttribARB" << endl;
	}
	else
	{
		printf("Hooked _wglSetPbufferAttribARB\n");
	}
	if ((_wglCreateContextAttribsARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglCreateContextAttribsARB), interceptedwglCreateContextAttribsARB))
	{
		cerr << "Failed to hook _wglCreateContextAttribsARB" << endl;
	}
	else
	{
		printf("Hooked _wglCreateContextAttribsARB\n");
	}
	/*if ((_wglCreateDisplayColorTableEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglCreateDisplayColorTableEXT), interceptedwglCreateDisplayColorTableEXT))
	{
		cerr << "Failed to hook _wglCreateDisplayColorTableEXT" << endl;
	}
	else
	{
		printf("Hooked _wglCreateDisplayColorTableEXT\n");
	}
	if ((_wglLoadDisplayColorTableEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglLoadDisplayColorTableEXT), interceptedwglLoadDisplayColorTableEXT))
	{
		cerr << "Failed to hook _wglLoadDisplayColorTableEXT" << endl;
	}
	else
	{
		printf("Hooked _wglLoadDisplayColorTableEXT\n");
	}
	if ((_wglBindDisplayColorTableEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglBindDisplayColorTableEXT), interceptedwglBindDisplayColorTableEXT))
	{
		cerr << "Failed to hook _wglBindDisplayColorTableEXT" << endl;
	}
	else
	{
		printf("Hooked _wglBindDisplayColorTableEXT\n");
	}
	if ((_wglDestroyDisplayColorTableEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDestroyDisplayColorTableEXT), interceptedwglDestroyDisplayColorTableEXT))
	{
		cerr << "Failed to hook _wglDestroyDisplayColorTableEXT" << endl;
	}
	else
	{
		printf("Hooked _wglDestroyDisplayColorTableEXT\n");
	}
	if ((_wglGetExtensionsStringEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetExtensionsStringEXT), interceptedwglGetExtensionsStringEXT))
	{
		cerr << "Failed to hook _wglGetExtensionsStringEXT" << endl;
	}
	else
	{
		printf("Hooked _wglGetExtensionsStringEXT\n");
	}
	if ((_wglMakeContextCurrentEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglMakeContextCurrentEXT), interceptedwglMakeContextCurrentEXT))
	{
		cerr << "Failed to hook _wglMakeContextCurrentEXT" << endl;
	}
	else
	{
		printf("Hooked _wglMakeContextCurrentEXT\n");
	}
	if ((_wglGetCurrentReadDCEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetCurrentReadDCEXT), interceptedwglGetCurrentReadDCEXT))
	{
		cerr << "Failed to hook _wglGetCurrentReadDCEXT" << endl;
	}
	else
	{
		printf("Hooked _wglGetCurrentReadDCEXT\n");
	}
	if ((_wglCreatePbufferEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglCreatePbufferEXT), interceptedwglCreatePbufferEXT))
	{
		cerr << "Failed to hook _wglCreatePbufferEXT" << endl;
	}
	else
	{
		printf("Hooked _wglCreatePbufferEXT\n");
	}
	if ((_wglGetPbufferDCEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetPbufferDCEXT), interceptedwglGetPbufferDCEXT))
	{
		cerr << "Failed to hook _wglGetPbufferDCEXT" << endl;
	}
	else
	{
		printf("Hooked _wglGetPbufferDCEXT\n");
	}
	if ((_wglReleasePbufferDCEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglReleasePbufferDCEXT), interceptedwglReleasePbufferDCEXT))
	{
		cerr << "Failed to hook _wglReleasePbufferDCEXT" << endl;
	}
	else
	{
		printf("Hooked _wglReleasePbufferDCEXT\n");
	}
	if ((_wglDestroyPbufferEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDestroyPbufferEXT), interceptedwglDestroyPbufferEXT))
	{
		cerr << "Failed to hook _wglDestroyPbufferEXT" << endl;
	}
	else
	{
		printf("Hooked _wglDestroyPbufferEXT\n");
	}
	if ((_wglQueryPbufferEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglQueryPbufferEXT), interceptedwglQueryPbufferEXT))
	{
		cerr << "Failed to hook _wglQueryPbufferEXT" << endl;
	}
	else
	{
		printf("Hooked _wglQueryPbufferEXT\n");
	}
	if ((_wglGetPixelFormatAttribivEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetPixelFormatAttribivEXT), interceptedwglGetPixelFormatAttribivEXT))
	{
		cerr << "Failed to hook _wglGetPixelFormatAttribivEXT" << endl;
	}
	else
	{
		printf("Hooked _wglGetPixelFormatAttribivEXT\n");
	}
	if ((_wglGetPixelFormatAttribfvEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetPixelFormatAttribfvEXT), interceptedwglGetPixelFormatAttribfvEXT))
	{
		cerr << "Failed to hook _wglGetPixelFormatAttribfvEXT" << endl;
	}
	else
	{
		printf("Hooked _wglGetPixelFormatAttribfvEXT\n");
	}
	if ((_wglChoosePixelFormatEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglChoosePixelFormatEXT), interceptedwglChoosePixelFormatEXT))
	{
		cerr << "Failed to hook _wglChoosePixelFormatEXT" << endl;
	}
	else
	{
		printf("Hooked _wglChoosePixelFormatEXT\n");
	}
	if ((_wglSwapIntervalEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSwapIntervalEXT), interceptedwglSwapIntervalEXT))
	{
		cerr << "Failed to hook _wglSwapIntervalEXT" << endl;
	}
	else
	{
		printf("Hooked _wglSwapIntervalEXT\n");
	}
	if ((_wglGetSwapIntervalEXT == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetSwapIntervalEXT), interceptedwglGetSwapIntervalEXT))
	{
		cerr << "Failed to hook _wglGetSwapIntervalEXT" << endl;
	}
	else
	{
		printf("Hooked _wglGetSwapIntervalEXT\n");
	}
	*/
	/*if ((_wglAllocateMemoryNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglAllocateMemoryNV), interceptedwglAllocateMemoryNV))
	{
		cerr << "Failed to hook _wglAllocateMemoryNV" << endl;
	}
	else
	{
		printf("Hooked _wglAllocateMemoryNV\n");
	}
	if ((_wglFreeMemoryNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglFreeMemoryNV), interceptedwglFreeMemoryNV))
	{
		cerr << "Failed to hook _wglFreeMemoryNV" << endl;
	}
	else
	{
		printf("Hooked _wglFreeMemoryNV\n");
	}
	*/
	/*
	if ((_wglGetSyncValuesOML == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetSyncValuesOML), interceptedwglGetSyncValuesOML))
	{
		cerr << "Failed to hook _wglGetSyncValuesOML" << endl;
	}
	else
	{
		printf("Hooked _wglGetSyncValuesOML\n");
	}
	if ((_wglGetMscRateOML == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetMscRateOML), interceptedwglGetMscRateOML))
	{
		cerr << "Failed to hook _wglGetMscRateOML" << endl;
	}
	else
	{
		printf("Hooked _wglGetMscRateOML\n");
	}
	if ((_wglSwapBuffersMscOML == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSwapBuffersMscOML), interceptedwglSwapBuffersMscOML))
	{
		cerr << "Failed to hook _wglSwapBuffersMscOML" << endl;
	}
	else
	{
		printf("Hooked _wglSwapBuffersMscOML\n");
	}
	if ((_wglSwapLayerBuffersMscOML == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSwapLayerBuffersMscOML), interceptedwglSwapLayerBuffersMscOML))
	{
		cerr << "Failed to hook _wglSwapLayerBuffersMscOML" << endl;
	}
	else
	{
		printf("Hooked _wglSwapLayerBuffersMscOML\n");
	}
	if ((_wglWaitForMscOML == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglWaitForMscOML), interceptedwglWaitForMscOML))
	{
		cerr << "Failed to hook _wglWaitForMscOML" << endl;
	}
	else
	{
		printf("Hooked _wglWaitForMscOML\n");
	}
	if ((_wglWaitForSbcOML == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglWaitForSbcOML), interceptedwglWaitForSbcOML))
	{
		cerr << "Failed to hook _wglWaitForSbcOML" << endl;
	}
	else
	{
		printf("Hooked _wglWaitForSbcOML\n");
	}
	if ((_wglGetDigitalVideoParametersI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetDigitalVideoParametersI3D), interceptedwglGetDigitalVideoParametersI3D))
	{
		cerr << "Failed to hook _wglGetDigitalVideoParametersI3D" << endl;
	}
	else
	{
		printf("Hooked _wglGetDigitalVideoParametersI3D\n");
	}
	if ((_wglSetDigitalVideoParametersI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSetDigitalVideoParametersI3D), interceptedwglSetDigitalVideoParametersI3D))
	{
		cerr << "Failed to hook _wglSetDigitalVideoParametersI3D" << endl;
	}
	else
	{
		printf("Hooked _wglSetDigitalVideoParametersI3D\n");
	}
	if ((_wglGetGammaTableParametersI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetGammaTableParametersI3D), interceptedwglGetGammaTableParametersI3D))
	{
		cerr << "Failed to hook _wglGetGammaTableParametersI3D" << endl;
	}
	else
	{
		printf("Hooked _wglGetGammaTableParametersI3D\n");
	}
	if ((_wglSetGammaTableParametersI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSetGammaTableParametersI3D), interceptedwglSetGammaTableParametersI3D))
	{
		cerr << "Failed to hook _wglSetGammaTableParametersI3D" << endl;
	}
	else
	{
		printf("Hooked _wglSetGammaTableParametersI3D\n");
	}
	if ((_wglGetGammaTableI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetGammaTableI3D), interceptedwglGetGammaTableI3D))
	{
		cerr << "Failed to hook _wglGetGammaTableI3D" << endl;
	}
	else
	{
		printf("Hooked _wglGetGammaTableI3D\n");
	}
	if ((_wglSetGammaTableI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSetGammaTableI3D), interceptedwglSetGammaTableI3D))
	{
		cerr << "Failed to hook _wglSetGammaTableI3D" << endl;
	}
	else
	{
		printf("Hooked _wglSetGammaTableI3D\n");
	}
	if ((_wglEnableGenlockI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglEnableGenlockI3D), interceptedwglEnableGenlockI3D))
	{
		cerr << "Failed to hook _wglEnableGenlockI3D" << endl;
	}
	else
	{
		printf("Hooked _wglEnableGenlockI3D\n");
	}
	if ((_wglDisableGenlockI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDisableGenlockI3D), interceptedwglDisableGenlockI3D))
	{
		cerr << "Failed to hook _wglDisableGenlockI3D" << endl;
	}
	else
	{
		printf("Hooked _wglDisableGenlockI3D\n");
	}
	if ((_wglIsEnabledGenlockI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglIsEnabledGenlockI3D), interceptedwglIsEnabledGenlockI3D))
	{
		cerr << "Failed to hook _wglIsEnabledGenlockI3D" << endl;
	}
	else
	{
		printf("Hooked _wglIsEnabledGenlockI3D\n");
	}
	if ((_wglGenlockSourceI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGenlockSourceI3D), interceptedwglGenlockSourceI3D))
	{
		cerr << "Failed to hook _wglGenlockSourceI3D" << endl;
	}
	else
	{
		printf("Hooked _wglGenlockSourceI3D\n");
	}
	if ((_wglGetGenlockSourceI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetGenlockSourceI3D), interceptedwglGetGenlockSourceI3D))
	{
		cerr << "Failed to hook _wglGetGenlockSourceI3D" << endl;
	}
	else
	{
		printf("Hooked _wglGetGenlockSourceI3D\n");
	}
	if ((_wglGenlockSourceEdgeI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGenlockSourceEdgeI3D), interceptedwglGenlockSourceEdgeI3D))
	{
		cerr << "Failed to hook _wglGenlockSourceEdgeI3D" << endl;
	}
	else
	{
		printf("Hooked _wglGenlockSourceEdgeI3D\n");
	}
	if ((_wglGetGenlockSourceEdgeI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetGenlockSourceEdgeI3D), interceptedwglGetGenlockSourceEdgeI3D))
	{
		cerr << "Failed to hook _wglGetGenlockSourceEdgeI3D" << endl;
	}
	else
	{
		printf("Hooked _wglGetGenlockSourceEdgeI3D\n");
	}
	if ((_wglGenlockSampleRateI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGenlockSampleRateI3D), interceptedwglGenlockSampleRateI3D))
	{
		cerr << "Failed to hook _wglGenlockSampleRateI3D" << endl;
	}
	else
	{
		printf("Hooked _wglGenlockSampleRateI3D\n");
	}
	if ((_wglGetGenlockSampleRateI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetGenlockSampleRateI3D), interceptedwglGetGenlockSampleRateI3D))
	{
		cerr << "Failed to hook _wglGetGenlockSampleRateI3D" << endl;
	}
	else
	{
		printf("Hooked _wglGetGenlockSampleRateI3D\n");
	}
	if ((_wglGenlockSourceDelayI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGenlockSourceDelayI3D), interceptedwglGenlockSourceDelayI3D))
	{
		cerr << "Failed to hook _wglGenlockSourceDelayI3D" << endl;
	}
	else
	{
		printf("Hooked _wglGenlockSourceDelayI3D\n");
	}
	if ((_wglGetGenlockSourceDelayI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetGenlockSourceDelayI3D), interceptedwglGetGenlockSourceDelayI3D))
	{
		cerr << "Failed to hook _wglGetGenlockSourceDelayI3D" << endl;
	}
	else
	{
		printf("Hooked _wglGetGenlockSourceDelayI3D\n");
	}
	if ((_wglQueryGenlockMaxSourceDelayI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglQueryGenlockMaxSourceDelayI3D), interceptedwglQueryGenlockMaxSourceDelayI3D))
	{
		cerr << "Failed to hook _wglQueryGenlockMaxSourceDelayI3D" << endl;
	}
	else
	{
		printf("Hooked _wglQueryGenlockMaxSourceDelayI3D\n");
	}
	if ((_wglCreateImageBufferI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglCreateImageBufferI3D), interceptedwglCreateImageBufferI3D))
	{
		cerr << "Failed to hook _wglCreateImageBufferI3D" << endl;
	}
	else
	{
		printf("Hooked _wglCreateImageBufferI3D\n");
	}
	if ((_wglDestroyImageBufferI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDestroyImageBufferI3D), interceptedwglDestroyImageBufferI3D))
	{
		cerr << "Failed to hook _wglDestroyImageBufferI3D" << endl;
	}
	else
	{
		printf("Hooked _wglDestroyImageBufferI3D\n");
	}
	if ((_wglAssociateImageBufferEventsI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglAssociateImageBufferEventsI3D), interceptedwglAssociateImageBufferEventsI3D))
	{
		cerr << "Failed to hook _wglAssociateImageBufferEventsI3D" << endl;
	}
	else
	{
		printf("Hooked _wglAssociateImageBufferEventsI3D\n");
	}
	if ((_wglReleaseImageBufferEventsI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglReleaseImageBufferEventsI3D), interceptedwglReleaseImageBufferEventsI3D))
	{
		cerr << "Failed to hook _wglReleaseImageBufferEventsI3D" << endl;
	}
	else
	{
		printf("Hooked _wglReleaseImageBufferEventsI3D\n");
	}
	if ((_wglEnableFrameLockI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglEnableFrameLockI3D), interceptedwglEnableFrameLockI3D))
	{
		cerr << "Failed to hook _wglEnableFrameLockI3D" << endl;
	}
	else
	{
		printf("Hooked _wglEnableFrameLockI3D\n");
	}
	if ((_wglDisableFrameLockI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDisableFrameLockI3D), interceptedwglDisableFrameLockI3D))
	{
		cerr << "Failed to hook _wglDisableFrameLockI3D" << endl;
	}
	else
	{
		printf("Hooked _wglDisableFrameLockI3D\n");
	}
	if ((_wglIsEnabledFrameLockI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglIsEnabledFrameLockI3D), interceptedwglIsEnabledFrameLockI3D))
	{
		cerr << "Failed to hook _wglIsEnabledFrameLockI3D" << endl;
	}
	else
	{
		printf("Hooked _wglIsEnabledFrameLockI3D\n");
	}
	if ((_wglQueryFrameLockMasterI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglQueryFrameLockMasterI3D), interceptedwglQueryFrameLockMasterI3D))
	{
		cerr << "Failed to hook _wglQueryFrameLockMasterI3D" << endl;
	}
	else
	{
		printf("Hooked _wglQueryFrameLockMasterI3D\n");
	}
	if ((_wglGetFrameUsageI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetFrameUsageI3D), interceptedwglGetFrameUsageI3D))
	{
		cerr << "Failed to hook _wglGetFrameUsageI3D" << endl;
	}
	else
	{
		printf("Hooked _wglGetFrameUsageI3D\n");
	}
	if ((_wglBeginFrameTrackingI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglBeginFrameTrackingI3D), interceptedwglBeginFrameTrackingI3D))
	{
		cerr << "Failed to hook _wglBeginFrameTrackingI3D" << endl;
	}
	else
	{
		printf("Hooked _wglBeginFrameTrackingI3D\n");
	}
	if ((_wglEndFrameTrackingI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglEndFrameTrackingI3D), interceptedwglEndFrameTrackingI3D))
	{
		cerr << "Failed to hook _wglEndFrameTrackingI3D" << endl;
	}
	else
	{
		printf("Hooked _wglEndFrameTrackingI3D\n");
	}
	if ((_wglQueryFrameTrackingI3D == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglQueryFrameTrackingI3D), interceptedwglQueryFrameTrackingI3D))
	{
		cerr << "Failed to hook _wglQueryFrameTrackingI3D" << endl;
	}
	else
	{
		printf("Hooked _wglQueryFrameTrackingI3D\n");
	}
	if ((_wglSetStereoEmitterState3DL == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSetStereoEmitterState3DL), interceptedwglSetStereoEmitterState3DL))
	{
		cerr << "Failed to hook _wglSetStereoEmitterState3DL" << endl;
	}
	else
	{
		printf("Hooked _wglSetStereoEmitterState3DL\n");
	}
	*/
	/*if ((_wglEnumerateVideoDevicesNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglEnumerateVideoDevicesNV), interceptedwglEnumerateVideoDevicesNV))
	{
		cerr << "Failed to hook _wglEnumerateVideoDevicesNV" << endl;
	}
	else
	{
		printf("Hooked _wglEnumerateVideoDevicesNV\n");
	}
	if ((_wglBindVideoDeviceNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglBindVideoDeviceNV), interceptedwglBindVideoDeviceNV))
	{
		cerr << "Failed to hook _wglBindVideoDeviceNV" << endl;
	}
	else
	{
		printf("Hooked _wglBindVideoDeviceNV\n");
	}
	if ((_wglQueryCurrentContextNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglQueryCurrentContextNV), interceptedwglQueryCurrentContextNV))
	{
		cerr << "Failed to hook _wglQueryCurrentContextNV" << endl;
	}
	else
	{
		printf("Hooked _wglQueryCurrentContextNV\n");
	}
	if ((_wglGetVideoDeviceNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetVideoDeviceNV), interceptedwglGetVideoDeviceNV))
	{
		cerr << "Failed to hook _wglGetVideoDeviceNV" << endl;
	}
	else
	{
		printf("Hooked _wglGetVideoDeviceNV\n");
	}
	if ((_wglReleaseVideoDeviceNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglReleaseVideoDeviceNV), interceptedwglReleaseVideoDeviceNV))
	{
		cerr << "Failed to hook _wglReleaseVideoDeviceNV" << endl;
	}
	else
	{
		printf("Hooked _wglReleaseVideoDeviceNV\n");
	}
	if ((_wglBindVideoImageNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglBindVideoImageNV), interceptedwglBindVideoImageNV))
	{
		cerr << "Failed to hook _wglBindVideoImageNV" << endl;
	}
	else
	{
		printf("Hooked _wglBindVideoImageNV\n");
	}
	if ((_wglReleaseVideoImageNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglReleaseVideoImageNV), interceptedwglReleaseVideoImageNV))
	{
		cerr << "Failed to hook _wglReleaseVideoImageNV" << endl;
	}
	else
	{
		printf("Hooked _wglReleaseVideoImageNV\n");
	}
	if ((_wglSendPbufferToVideoNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglSendPbufferToVideoNV), interceptedwglSendPbufferToVideoNV))
	{
		cerr << "Failed to hook _wglSendPbufferToVideoNV" << endl;
	}
	else
	{
		printf("Hooked _wglSendPbufferToVideoNV\n");
	}
	if ((_wglGetVideoInfoNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetVideoInfoNV), interceptedwglGetVideoInfoNV))
	{
		cerr << "Failed to hook _wglGetVideoInfoNV" << endl;
	}
	else
	{
		printf("Hooked _wglGetVideoInfoNV\n");
	}
	if ((_wglJoinSwapGroupNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglJoinSwapGroupNV), interceptedwglJoinSwapGroupNV))
	{
		cerr << "Failed to hook _wglJoinSwapGroupNV" << endl;
	}
	else
	{
		printf("Hooked _wglJoinSwapGroupNV\n");
	}
	if ((_wglBindSwapBarrierNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglBindSwapBarrierNV), interceptedwglBindSwapBarrierNV))
	{
		cerr << "Failed to hook _wglBindSwapBarrierNV" << endl;
	}
	else
	{
		printf("Hooked _wglBindSwapBarrierNV\n");
	}
	if ((_wglQuerySwapGroupNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglQuerySwapGroupNV), interceptedwglQuerySwapGroupNV))
	{
		cerr << "Failed to hook _wglQuerySwapGroupNV" << endl;
	}
	else
	{
		printf("Hooked _wglQuerySwapGroupNV\n");
	}
	if ((_wglQueryMaxSwapGroupsNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglQueryMaxSwapGroupsNV), interceptedwglQueryMaxSwapGroupsNV))
	{
		cerr << "Failed to hook _wglQueryMaxSwapGroupsNV" << endl;
	}
	else
	{
		printf("Hooked _wglQueryMaxSwapGroupsNV\n");
	}
	if ((_wglQueryFrameCountNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglQueryFrameCountNV), interceptedwglQueryFrameCountNV))
	{
		cerr << "Failed to hook _wglQueryFrameCountNV" << endl;
	}
	else
	{
		printf("Hooked _wglQueryFrameCountNV\n");
	}
	if ((_wglResetFrameCountNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglResetFrameCountNV), interceptedwglResetFrameCountNV))
	{
		cerr << "Failed to hook _wglResetFrameCountNV" << endl;
	}
	else
	{
		printf("Hooked _wglResetFrameCountNV\n");
	}
	if ((_wglEnumGpusNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglEnumGpusNV), interceptedwglEnumGpusNV))
	{
		cerr << "Failed to hook _wglEnumGpusNV" << endl;
	}
	else
	{
		printf("Hooked _wglEnumGpusNV\n");
	}
	if ((_wglEnumGpuDevicesNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglEnumGpuDevicesNV), interceptedwglEnumGpuDevicesNV))
	{
		cerr << "Failed to hook _wglEnumGpuDevicesNV" << endl;
	}
	else
	{
		printf("Hooked _wglEnumGpuDevicesNV\n");
	}
	if ((_wglCreateAffinityDCNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglCreateAffinityDCNV), interceptedwglCreateAffinityDCNV))
	{
		cerr << "Failed to hook _wglCreateAffinityDCNV" << endl;
	}
	else
	{
		printf("Hooked _wglCreateAffinityDCNV\n");
	}
	if ((_wglEnumGpusFromAffinityDCNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglEnumGpusFromAffinityDCNV), interceptedwglEnumGpusFromAffinityDCNV))
	{
		cerr << "Failed to hook _wglEnumGpusFromAffinityDCNV" << endl;
	}
	else
	{
		printf("Hooked _wglEnumGpusFromAffinityDCNV\n");
	}
	if ((_wglDeleteDCNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDeleteDCNV), interceptedwglDeleteDCNV))
	{
		cerr << "Failed to hook _wglDeleteDCNV" << endl;
	}
	else
	{
		printf("Hooked _wglDeleteDCNV\n");
	}
	*/
	/*if ((_wglGetGPUIDsAMD == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetGPUIDsAMD), interceptedwglGetGPUIDsAMD))
	{
		cerr << "Failed to hook _wglGetGPUIDsAMD" << endl;
	}
	else
	{
		printf("Hooked _wglGetGPUIDsAMD\n");
	}
	if ((_wglGetGPUInfoAMD == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetGPUInfoAMD), interceptedwglGetGPUInfoAMD))
	{
		cerr << "Failed to hook _wglGetGPUInfoAMD" << endl;
	}
	else
	{
		printf("Hooked _wglGetGPUInfoAMD\n");
	}
	if ((_wglGetContextGPUIDAMD == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetContextGPUIDAMD), interceptedwglGetContextGPUIDAMD))
	{
		cerr << "Failed to hook _wglGetContextGPUIDAMD" << endl;
	}
	else
	{
		printf("Hooked _wglGetContextGPUIDAMD\n");
	}
	if ((_wglCreateAssociatedContextAMD == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglCreateAssociatedContextAMD), interceptedwglCreateAssociatedContextAMD))
	{
		cerr << "Failed to hook _wglCreateAssociatedContextAMD" << endl;
	}
	else
	{
		printf("Hooked _wglCreateAssociatedContextAMD\n");
	}
	if ((_wglCreateAssociatedContextAttribsAMD == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglCreateAssociatedContextAttribsAMD), interceptedwglCreateAssociatedContextAttribsAMD))
	{
		cerr << "Failed to hook _wglCreateAssociatedContextAttribsAMD" << endl;
	}
	else
	{
		printf("Hooked _wglCreateAssociatedContextAttribsAMD\n");
	}
	if ((_wglDeleteAssociatedContextAMD == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDeleteAssociatedContextAMD), interceptedwglDeleteAssociatedContextAMD))
	{
		cerr << "Failed to hook _wglDeleteAssociatedContextAMD" << endl;
	}
	else
	{
		printf("Hooked _wglDeleteAssociatedContextAMD\n");
	}
	if ((_wglMakeAssociatedContextCurrentAMD == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglMakeAssociatedContextCurrentAMD), interceptedwglMakeAssociatedContextCurrentAMD))
	{
		cerr << "Failed to hook _wglMakeAssociatedContextCurrentAMD" << endl;
	}
	else
	{
		printf("Hooked _wglMakeAssociatedContextCurrentAMD\n");
	}
	if ((_wglGetCurrentAssociatedContextAMD == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglGetCurrentAssociatedContextAMD), interceptedwglGetCurrentAssociatedContextAMD))
	{
		cerr << "Failed to hook _wglGetCurrentAssociatedContextAMD" << endl;
	}
	else
	{
		printf("Hooked _wglGetCurrentAssociatedContextAMD\n");
	}
	if ((_wglBlitContextFramebufferAMD == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglBlitContextFramebufferAMD), interceptedwglBlitContextFramebufferAMD))
	{
		cerr << "Failed to hook _wglBlitContextFramebufferAMD" << endl;
	}
	else
	{
		printf("Hooked _wglBlitContextFramebufferAMD\n");
	}
	*/
	/*if ((_wglBindVideoCaptureDeviceNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglBindVideoCaptureDeviceNV), interceptedwglBindVideoCaptureDeviceNV))
	{
		cerr << "Failed to hook _wglBindVideoCaptureDeviceNV" << endl;
	}
	else
	{
		printf("Hooked _wglBindVideoCaptureDeviceNV\n");
	}
	if ((_wglEnumerateVideoCaptureDevicesNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglEnumerateVideoCaptureDevicesNV), interceptedwglEnumerateVideoCaptureDevicesNV))
	{
		cerr << "Failed to hook _wglEnumerateVideoCaptureDevicesNV" << endl;
	}
	else
	{
		printf("Hooked _wglEnumerateVideoCaptureDevicesNV\n");
	}
	if ((_wglLockVideoCaptureDeviceNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglLockVideoCaptureDeviceNV), interceptedwglLockVideoCaptureDeviceNV))
	{
		cerr << "Failed to hook _wglLockVideoCaptureDeviceNV" << endl;
	}
	else
	{
		printf("Hooked _wglLockVideoCaptureDeviceNV\n");
	}
	if ((_wglQueryVideoCaptureDeviceNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglQueryVideoCaptureDeviceNV), interceptedwglQueryVideoCaptureDeviceNV))
	{
		cerr << "Failed to hook _wglQueryVideoCaptureDeviceNV" << endl;
	}
	else
	{
		printf("Hooked _wglQueryVideoCaptureDeviceNV\n");
	}
	if ((_wglReleaseVideoCaptureDeviceNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglReleaseVideoCaptureDeviceNV), interceptedwglReleaseVideoCaptureDeviceNV))
	{
		cerr << "Failed to hook _wglReleaseVideoCaptureDeviceNV" << endl;
	}
	else
	{
		printf("Hooked _wglReleaseVideoCaptureDeviceNV\n");
	}
	if ((_wglCopyImageSubDataNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglCopyImageSubDataNV), interceptedwglCopyImageSubDataNV))
	{
		cerr << "Failed to hook _wglCopyImageSubDataNV" << endl;
	}
	else
	{
		printf("Hooked _wglCopyImageSubDataNV\n");
	}
	if ((_wglDXSetResourceShareHandleNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDXSetResourceShareHandleNV), interceptedwglDXSetResourceShareHandleNV))
	{
		cerr << "Failed to hook _wglDXSetResourceShareHandleNV" << endl;
	}
	else
	{
		printf("Hooked _wglDXSetResourceShareHandleNV\n");
	}
	if ((_wglDXOpenDeviceNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDXOpenDeviceNV), interceptedwglDXOpenDeviceNV))
	{
		cerr << "Failed to hook _wglDXOpenDeviceNV" << endl;
	}
	else
	{
		printf("Hooked _wglDXOpenDeviceNV\n");
	}
	if ((_wglDXCloseDeviceNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDXCloseDeviceNV), interceptedwglDXCloseDeviceNV))
	{
		cerr << "Failed to hook _wglDXCloseDeviceNV" << endl;
	}
	else
	{
		printf("Hooked _wglDXCloseDeviceNV\n");
	}
	if ((_wglDXRegisterObjectNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDXRegisterObjectNV), interceptedwglDXRegisterObjectNV))
	{
		cerr << "Failed to hook _wglDXRegisterObjectNV" << endl;
	}
	else
	{
		printf("Hooked _wglDXRegisterObjectNV\n");
	}
	if ((_wglDXUnregisterObjectNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDXUnregisterObjectNV), interceptedwglDXUnregisterObjectNV))
	{
		cerr << "Failed to hook _wglDXUnregisterObjectNV" << endl;
	}
	else
	{
		printf("Hooked _wglDXUnregisterObjectNV\n");
	}
	if ((_wglDXObjectAccessNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDXObjectAccessNV), interceptedwglDXObjectAccessNV))
	{
		cerr << "Failed to hook _wglDXObjectAccessNV" << endl;
	}
	else
	{
		printf("Hooked _wglDXObjectAccessNV\n");
	}
	if ((_wglDXLockObjectsNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDXLockObjectsNV), interceptedwglDXLockObjectsNV))
	{
		cerr << "Failed to hook _wglDXLockObjectsNV" << endl;
	}
	else
	{
		printf("Hooked _wglDXLockObjectsNV\n");
	}
	if ((_wglDXUnlockObjectsNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDXUnlockObjectsNV), interceptedwglDXUnlockObjectsNV))
	{
		cerr << "Failed to hook _wglDXUnlockObjectsNV" << endl;
	}
	else
	{
		printf("Hooked _wglDXUnlockObjectsNV\n");
	}
	if ((_wglDelayBeforeSwapNV == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglDelayBeforeSwapNV), interceptedwglDelayBeforeSwapNV))
	{
		cerr << "Failed to hook _wglDelayBeforeSwapNV" << endl;
	}
	else
	{
		printf("Hooked _wglDelayBeforeSwapNV\n");
	}
	*/
	if ((_glGetIntegerv == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glGetIntegerv), interceptedglGetIntegerv))
	{
		cerr << "Failed to hook _glGetIntegerv" << endl;
	}
	else
	{
		printf("Hooked _glGetIntegerv\n");
	}
	if ((_glGetBooleanv == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glGetBooleanv), interceptedglGetBooleanv))
	{
		cerr << "Failed to hook _glGetBooleanv" << endl;
	}
	else
	{
		printf("Hooked _glGetBooleanv\n");
	}
	if ((_glDisable == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glDisable), interceptedglDisable))
	{
		cerr << "Failed to hook _glDisable" << endl;
	}
	else
	{
		printf("Hooked _glDisable\n");
	}
	if ((_glVertex2d == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glVertex2d), interceptedglVertex2d))
	{
		cerr << "Failed to hook _glVertex2d" << endl;
	}
	else
	{
		printf("Hooked _glVertex2d\n");
	}
	if ((_glCallList == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glCallList), interceptedglCallList))
	{
		cerr << "Failed to hook _glCallList" << endl;
	}
	else
	{
		printf("Hooked _glCallList\n");
	}
	if ((_glMaterialfv == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glMaterialfv), interceptedglMaterialfv))
	{
		cerr << "Failed to hook _glMaterialfv" << endl;
	}
	else
	{
		printf("Hooked _glMaterialfv\n");
	}
	if ((_glEnable == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glEnable), interceptedglEnable))
	{
		cerr << "Failed to hook _glEnable" << endl;
	}
	else
	{
		printf("Hooked _glEnable\n");
	}
	if ((_glPopMatrix == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glPopMatrix), interceptedglPopMatrix))
	{
		cerr << "Failed to hook _glPopMatrix" << endl;
	}
	else
	{
		printf("Hooked _glPopMatrix\n");
	}
	if ((_glTexEnvi == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glTexEnvi), interceptedglTexEnvi))
	{
		cerr << "Failed to hook _glTexEnvi" << endl;
	}
	else
	{
		printf("Hooked _glTexEnvi\n");
	}
	if ((_glVertex3d == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glVertex3d), interceptedglVertex3d))
	{
		cerr << "Failed to hook _glVertex3d" << endl;
	}
	else
	{
		printf("Hooked _glVertex3d\n");
	}
	if ((_glTexCoord2d == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glTexCoord2d), interceptedglTexCoord2d))
	{
		cerr << "Failed to hook _glTexCoord2d" << endl;
	}
	else
	{
		printf("Hooked _glTexCoord2d\n");
	}
	if ((_glBindTexture == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glBindTexture), interceptedglBindTexture))
	{
		cerr << "Failed to hook _glBindTexture" << endl;
	}
	else
	{
		printf("Hooked _glBindTexture\n");
	}
	if ((_glRotated == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glRotated), interceptedglRotated))
	{
		cerr << "Failed to hook _glRotated" << endl;
	}
	else
	{
		printf("Hooked _glRotated\n");
	}
	if ((_glPushAttrib == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glPushAttrib), interceptedglPushAttrib))
	{
		cerr << "Failed to hook _glPushAttrib" << endl;
	}
	else
	{
		printf("Hooked _glPushAttrib\n");
	}
	if ((_glPopAttrib == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glPopAttrib), interceptedglPopAttrib))
	{
		cerr << "Failed to hook _glPopAttrib" << endl;
	}
	else
	{
		printf("Hooked _glPopAttrib\n");
	}
	if ((_glColor4f == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glColor4f), interceptedglColor4f))
	{
		cerr << "Failed to hook _glColor4f" << endl;
	}
	else
	{
		printf("Hooked _glColor4f\n");
	}
	if ((_glVertex2f == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glVertex2f), interceptedglVertex2f))
	{
		cerr << "Failed to hook _glVertex2f" << endl;
	}
	else
	{
		printf("Hooked _glVertex2f\n");
	}
	if ((_glBlendFunc == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glBlendFunc), interceptedglBlendFunc))
	{
		cerr << "Failed to hook _glBlendFunc" << endl;
	}
	else
	{
		printf("Hooked _glBlendFunc\n");
	}
	if ((_glFrustum == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glFrustum), interceptedglFrustum))
	{
		cerr << "Failed to hook _glFrustum" << endl;
	}
	else
	{
		printf("Hooked _glFrustum\n");
	}
	if ((_glViewport == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glViewport), interceptedglViewport))
	{
		cerr << "Failed to hook _glViewport" << endl;
	}
	else
	{
		printf("Hooked _glViewport\n");
	}
	if ((_glMatrixMode == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glMatrixMode), interceptedglMatrixMode))
	{
		cerr << "Failed to hook _glMatrixMode" << endl;
	}
	else
	{
		printf("Hooked _glMatrixMode\n");
	}
	if ((_glPolygonMode == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glPolygonMode), interceptedglPolygonMode))
	{
		cerr << "Failed to hook _glPolygonMode" << endl;
	}
	else
	{
		printf("Hooked _glPolygonMode\n");
	}
	if ((_glClearColor == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glClearColor), interceptedglClearColor))
	{
		cerr << "Failed to hook _glClearColor" << endl;
	}
	else
	{
		printf("Hooked _glClearColor\n");
	}
	if ((_glScalef == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glScalef), interceptedglScalef))
	{
		cerr << "Failed to hook _glScalef" << endl;
	}
	else
	{
		printf("Hooked _glScalef\n");
	}
	if ((_glHint == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glHint), interceptedglHint))
	{
		cerr << "Failed to hook _glHint" << endl;
	}
	else
	{
		printf("Hooked _glHint\n");
	}
	if ((_glFogfv == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glFogfv), interceptedglFogfv))
	{
		cerr << "Failed to hook _glFogfv" << endl;
	}
	else
	{
		printf("Hooked _glFogfv\n");
	}
	if ((_glFogf == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glFogf), interceptedglFogf))
	{
		cerr << "Failed to hook _glFogf" << endl;
	}
	else
	{
		printf("Hooked _glFogf\n");
	}
	if ((_glColorMaterial == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glColorMaterial), interceptedglColorMaterial))
	{
		cerr << "Failed to hook _glColorMaterial" << endl;
	}
	else
	{
		printf("Hooked _glColorMaterial\n");
	}
	if ((_glLightModelfv == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glLightModelfv), interceptedglLightModelfv))
	{
		cerr << "Failed to hook _glLightModelfv" << endl;
	}
	else
	{
		printf("Hooked _glLightModelfv\n");
	}
	if ((_glLightfv == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glLightfv), interceptedglLightfv))
	{
		cerr << "Failed to hook _glLightfv" << endl;
	}
	else
	{
		printf("Hooked _glLightfv\n");
	}
	if ((_glGetFloatv == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glGetFloatv), interceptedglGetFloatv))
	{
		cerr << "Failed to hook _glGetFloatv" << endl;
	}
	else
	{
		printf("Hooked _glGetFloatv\n");
	}
	if ((_glBindBufferARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glBindBufferARB), interceptedglBindBufferARB))
	{
		cerr << "Failed to hook _glBindBufferARB" << endl;
	}
	else
	{
		printf("Hooked _glBindBufferARB\n");
	}
	if ((_glInterleavedArrays == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glInterleavedArrays), interceptedglInterleavedArrays))
	{
		cerr << "Failed to hook _glInterleavedArrays" << endl;
	}
	else
	{
		printf("Hooked _glInterleavedArrays\n");
	}
	if ((_glDrawArrays == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glDrawArrays), interceptedglDrawArrays))
	{
		cerr << "Failed to hook _glDrawArrays" << endl;
	}
	else
	{
		printf("Hooked _glDrawArrays\n");
	}
	if ((_glUseProgram == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glUseProgram), interceptedglUseProgram))
	{
		cerr << "Failed to hook _glUseProgram" << endl;
	}
	else
	{
		printf("Hooked _glUseProgram\n");
	}
	if ((_glGetUniformLocationARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glGetUniformLocationARB), interceptedglGetUniformLocationARB))
	{
		cerr << "Failed to hook _glGetUniformLocationARB" << endl;
	}
	else
	{
		printf("Hooked _glGetUniformLocationARB\n");
	}
	if ((_glUniform4f == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glUniform4f), interceptedglUniform4f))
	{
		cerr << "Failed to hook _glUniform4f" << endl;
	}
	else
	{
		printf("Hooked _glUniform4f\n");
	}
	if ((_glUniform1f == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glUniform1f), interceptedglUniform1f))
	{
		cerr << "Failed to hook _glUniform1f" << endl;
	}
	else
	{
		printf("Hooked _glUniform1f\n");
	}
	if ((_glUniform1i == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glUniform1i), interceptedglUniform1i))
	{
		cerr << "Failed to hook _glUniform1i" << endl;
	}
	else
	{
		printf("Hooked _glUniform1i\n");
	}
	if ((_glActiveTextureARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glActiveTextureARB), interceptedglActiveTextureARB))
	{
		cerr << "Failed to hook _glActiveTextureARB" << endl;
	}
	else
	{
		printf("Hooked _glActiveTextureARB\n");
	}
	if ((_glMultiTexCoord2fARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glMultiTexCoord2fARB), interceptedglMultiTexCoord2fARB))
	{
		cerr << "Failed to hook _glMultiTexCoord2fARB" << endl;
	}
	else
	{
		printf("Hooked _glMultiTexCoord2fARB\n");
	}
	if ((_glDepthMask == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glDepthMask), interceptedglDepthMask))
	{
		cerr << "Failed to hook _glDepthMask" << endl;
	}
	else
	{
		printf("Hooked _glDepthMask\n");
	}
	if ((_glEnableClientState == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glEnableClientState), interceptedglEnableClientState))
	{
		cerr << "Failed to hook _glEnableClientState" << endl;
	}
	else
	{
		printf("Hooked _glEnableClientState\n");
	}
	if ((_glPointParameterfvARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glPointParameterfvARB), interceptedglPointParameterfvARB))
	{
		cerr << "Failed to hook _glPointParameterfvARB" << endl;
	}
	else
	{
		printf("Hooked _glPointParameterfvARB\n");
	}
	if ((_glPointParameterfARB == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glPointParameterfARB), interceptedglPointParameterfARB))
	{
		cerr << "Failed to hook _glPointParameterfARB" << endl;
	}
	else
	{
		printf("Hooked _glPointParameterfARB\n");
	}
	if ((_glPointSize == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glPointSize), interceptedglPointSize))
	{
		cerr << "Failed to hook _glPointSize" << endl;
	}
	else
	{
		printf("Hooked _glPointSize\n");
	}
	if ((_glTexEnvf == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glTexEnvf), interceptedglTexEnvf))
	{
		cerr << "Failed to hook _glTexEnvf" << endl;
	}
	else
	{
		printf("Hooked _glTexEnvf\n");
	}
	if ((_glColorPointer == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glColorPointer), interceptedglColorPointer))
	{
		cerr << "Failed to hook _glColorPointer" << endl;
	}
	else
	{
		printf("Hooked _glColorPointer\n");
	}
	if ((_glVertexPointer == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glVertexPointer), interceptedglVertexPointer))
	{
		cerr << "Failed to hook _glVertexPointer" << endl;
	}
	else
	{
		printf("Hooked _glVertexPointer\n");
	}
	if ((_glColor4ub == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glColor4ub), interceptedglColor4ub))
	{
		cerr << "Failed to hook _glColor4ub" << endl;
	}
	else
	{
		printf("Hooked _glColor4ub\n");
	}
	if ((_glTexCoord2f == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glTexCoord2f), interceptedglTexCoord2f))
	{
		cerr << "Failed to hook _glTexCoord2f" << endl;
	}
	else
	{
		printf("Hooked _glTexCoord2f\n");
	}
	if ((_glVertex2i == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_glVertex2i), interceptedglVertex2i))
	{
		cerr << "Failed to hook _glVertex2i" << endl;
	}
	else
	{
		printf("Hooked _glVertex2i\n");
	}
}
void processDetach()
{
	printf("processDetach\n");

	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglChoosePixelFormat));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSetPixelFormat));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glClear));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSwapBuffers));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glColor3f));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glRotatef));

	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglMakeCurrent));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glScaled));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glColor3d));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glPushMatrix));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glMultMatrixd));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglCreateContext));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDeleteContext));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetCurrentContext));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglCopyContext));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetCurrentDC));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetDefaultProcAddress));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglShareLists));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglCreateLayerContext));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDescribeLayerPlane));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSetLayerPaletteEntries));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetLayerPaletteEntries));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglRealizeLayerPalette));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSwapLayerBuffers));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglUseFontBitmapsA));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglUseFontBitmapsW));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSwapMultipleBuffers));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglUseFontOutlinesA));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglUseFontOutlinesW));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglCreateBufferRegionARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDeleteBufferRegionARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSaveBufferRegionARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglRestoreBufferRegionARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetExtensionsStringARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetPixelFormatAttribivARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetPixelFormatAttribfvARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglMakeContextCurrentARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetCurrentReadDCARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglCreatePbufferARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetPbufferDCARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglReleasePbufferDCARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDestroyPbufferARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglQueryPbufferARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglBindTexImageARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglReleaseTexImageARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSetPbufferAttribARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglCreateContextAttribsARB));
	/*
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglCreateDisplayColorTableEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglLoadDisplayColorTableEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglBindDisplayColorTableEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDestroyDisplayColorTableEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetExtensionsStringEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglMakeContextCurrentEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetCurrentReadDCEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglCreatePbufferEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetPbufferDCEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglReleasePbufferDCEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDestroyPbufferEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglQueryPbufferEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetPixelFormatAttribivEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetPixelFormatAttribfvEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglChoosePixelFormatEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSwapIntervalEXT));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetSwapIntervalEXT));
	*/
	/*Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglAllocateMemoryNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglFreeMemoryNV));
	*/
	/*Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetSyncValuesOML));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetMscRateOML));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSwapBuffersMscOML));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSwapLayerBuffersMscOML));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglWaitForMscOML));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglWaitForSbcOML));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetDigitalVideoParametersI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSetDigitalVideoParametersI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetGammaTableParametersI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSetGammaTableParametersI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetGammaTableI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSetGammaTableI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglEnableGenlockI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDisableGenlockI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglIsEnabledGenlockI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGenlockSourceI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetGenlockSourceI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGenlockSourceEdgeI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetGenlockSourceEdgeI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGenlockSampleRateI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetGenlockSampleRateI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGenlockSourceDelayI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetGenlockSourceDelayI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglQueryGenlockMaxSourceDelayI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglCreateImageBufferI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDestroyImageBufferI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglAssociateImageBufferEventsI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglReleaseImageBufferEventsI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglEnableFrameLockI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDisableFrameLockI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglIsEnabledFrameLockI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglQueryFrameLockMasterI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetFrameUsageI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglBeginFrameTrackingI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglEndFrameTrackingI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglQueryFrameTrackingI3D));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSetStereoEmitterState3DL));
	*/
	/*Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglEnumerateVideoDevicesNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglBindVideoDeviceNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglQueryCurrentContextNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetVideoDeviceNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglReleaseVideoDeviceNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglBindVideoImageNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglReleaseVideoImageNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglSendPbufferToVideoNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetVideoInfoNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglJoinSwapGroupNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglBindSwapBarrierNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglQuerySwapGroupNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglQueryMaxSwapGroupsNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglQueryFrameCountNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglResetFrameCountNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglEnumGpusNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglEnumGpuDevicesNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglCreateAffinityDCNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglEnumGpusFromAffinityDCNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDeleteDCNV));
	*/
	/*
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetGPUIDsAMD));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetGPUInfoAMD));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetContextGPUIDAMD));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglCreateAssociatedContextAMD));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglCreateAssociatedContextAttribsAMD));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDeleteAssociatedContextAMD));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglMakeAssociatedContextCurrentAMD));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglGetCurrentAssociatedContextAMD));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglBlitContextFramebufferAMD));
	*/
	/*Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglBindVideoCaptureDeviceNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglEnumerateVideoCaptureDevicesNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglLockVideoCaptureDeviceNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglQueryVideoCaptureDeviceNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglReleaseVideoCaptureDeviceNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglCopyImageSubDataNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDXSetResourceShareHandleNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDXOpenDeviceNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDXCloseDeviceNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDXRegisterObjectNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDXUnregisterObjectNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDXObjectAccessNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDXLockObjectsNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDXUnlockObjectsNV));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_wglDelayBeforeSwapNV));
	*/
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glGetIntegerv));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glGetBooleanv));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glDisable));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glVertex2d));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glCallList));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glMaterialfv));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glEnable));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glPopMatrix));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glTexEnvi));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glVertex3d));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glTexCoord2d));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glBindTexture));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glRotated));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glPushAttrib));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glPopAttrib));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glColor4f));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glVertex2f));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glBlendFunc));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glFrustum));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glViewport));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glMatrixMode));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glPolygonMode));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glClearColor));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glScalef));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glHint));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glFogfv));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glFogf));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glColorMaterial));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glLightModelfv));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glLightfv));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glGetFloatv));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glBindBufferARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glInterleavedArrays));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glDrawArrays));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glUseProgram));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glGetUniformLocationARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glUniform4f));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glUniform1f));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glUniform1i));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glActiveTextureARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glMultiTexCoord2fARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glDepthMask));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glEnableClientState));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glPointParameterfvARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glPointParameterfARB));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glPointSize));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glTexEnvf));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glColorPointer));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glVertexPointer));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glColor4ub));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glTexCoord2f));
	Mhook_Unhook(reinterpret_cast<PVOID*>(&_glVertex2i));
}
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	bool isOpened = Log::open("intercept.log");

	if (isOpened)
	{
		printf("Opened\n");
	}

	Config3DSettings config;
	config.readConfig3DSettingsFromFileV2();

	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(instance);
		processAttach();
		break;
	case DLL_PROCESS_DETACH:
		processDetach();
		break;
	}

	return TRUE;
}
