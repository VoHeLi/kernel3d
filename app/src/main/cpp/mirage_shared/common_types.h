#pragma once

#include <GLES2/gl2ext.h>
#include <EGL/eglext.h>
#include "../openxr/openxr_platform_defines.h"

extern "C"{
    GL_APICALL void GL_APIENTRY glEGLImageTargetTexture2DOES (GLenum target, GLeglImageOES image);
    GL_APICALL void GL_APIENTRY glEGLImageTargetRenderbufferStorageOES (GLenum target, GLeglImageOES image);
    GL_APICALL EGLClientBuffer GL_APIENTRY eglGetNativeClientBufferANDROID (const struct AHardwareBuffer *buffer);
    GL_APICALL EGLImageKHR GL_APIENTRY eglCreateImageKHR (EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list);
}


enum cts_instruction : uint64_t  {
    NONE = 0,
    POPULATE_SYSTEM_PROPERTIES = 1,
    POPULATE_INITIAL_SESSION_PROPERTIES = 2,
    SHARE_SWAPCHAIN_AHARDWAREBUFFER = 3,
    SHARE_SWAPCHAIN_AHARDWAREBUFFER_READY = 4,
    WAIT_FRAME = 5,
};