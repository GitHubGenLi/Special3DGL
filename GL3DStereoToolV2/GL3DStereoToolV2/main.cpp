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

void initialiseRealOpenGLFunctions()
{
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
	_glColor3d = (PFN_GLCOLOR3D)_getAnyGLFuncAddress("glColor3d");
	_glPushMatrix = (PFN_GLPUSHMATRIX)_getPublicProcAddress("glPushMatrix");
	_glMultMatrixd = (PFN_GLMULTMATRIXD)_getPublicProcAddress("glMultMatrixd");
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
	_wglCreateDisplayColorTableEXT = (PFN_WGLCREATEDISPLAYCOLORTABLEEXT)_getAnyGLFuncAddress("wglCreateDisplayColorTableEXT");
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
	_wglAllocateMemoryNV = (PFN_WGLALLOCATEMEMORYNV)_getAnyGLFuncAddress("wglAllocateMemoryNV");
	_wglFreeMemoryNV = (PFN_WGLFREEMEMORYNV)_getAnyGLFuncAddress("wglFreeMemoryNV");
	_wglGetSyncValuesOML = (PFN_WGLGETSYNCVALUESOML)_getAnyGLFuncAddress("wglGetSyncValuesOML");
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
	_wglEnumerateVideoDevicesNV = (PFN_WGLENUMERATEVIDEODEVICESNV)_getAnyGLFuncAddress("wglEnumerateVideoDevicesNV");
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
	_wglGetGPUIDsAMD = (PFN_WGLGETGPUIDSAMD)_getAnyGLFuncAddress("wglGetGPUIDsAMD");
	_wglGetGPUInfoAMD = (PFN_WGLGETGPUINFOAMD)_getAnyGLFuncAddress("wglGetGPUInfoAMD");
	_wglGetContextGPUIDAMD = (PFN_WGLGETCONTEXTGPUIDAMD)_getAnyGLFuncAddress("wglGetContextGPUIDAMD");
	_wglCreateAssociatedContextAMD = (PFN_WGLCREATEASSOCIATEDCONTEXTAMD)_getAnyGLFuncAddress("wglCreateAssociatedContextAMD");
	_wglCreateAssociatedContextAttribsAMD = (PFN_WGLCREATEASSOCIATEDCONTEXTATTRIBSAMD)_getAnyGLFuncAddress("wglCreateAssociatedContextAttribsAMD");
	_wglDeleteAssociatedContextAMD = (PFN_WGLDELETEASSOCIATEDCONTEXTAMD)_getAnyGLFuncAddress("wglDeleteAssociatedContextAMD");
	_wglMakeAssociatedContextCurrentAMD = (PFN_WGLMAKEASSOCIATEDCONTEXTCURRENTAMD)_getAnyGLFuncAddress("wglMakeAssociatedContextCurrentAMD");
	_wglGetCurrentAssociatedContextAMD = (PFN_WGLGETCURRENTASSOCIATEDCONTEXTAMD)_getAnyGLFuncAddress("wglGetCurrentAssociatedContextAMD");
	_wglBlitContextFramebufferAMD = (PFN_WGLBLITCONTEXTFRAMEBUFFERAMD)_getAnyGLFuncAddress("wglBlitContextFramebufferAMD");
	_wglBindVideoCaptureDeviceNV = (PFN_WGLBINDVIDEOCAPTUREDEVICENV)_getAnyGLFuncAddress("wglBindVideoCaptureDeviceNV");
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
	/*if ((_wglMakeCurrent == 0) ||
		!Mhook_SetHook(reinterpret_cast<PVOID*>(&_wglMakeCurrent), interceptedwglMakeCurrenth))
	{
		cerr << "Failed to hook _wglMakeCurrent" << endl;
		MessageBox(0, "Failed to hook _wglMakeCurrent", "Error", MB_OK);
	}
	else
	{
		printf("Hooked _wglMakeCurrent\n");
	}*/
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
}
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	bool isOpened = Log::open("intercept.log");

	if (isOpened)
	{
		printf("Opened\n");
	}

	//Config3DSettings config;
	//config.readConfig3DSettingsFromFile();

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
