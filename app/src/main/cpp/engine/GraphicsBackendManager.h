#pragma once

#include "../openxr/openxr.h"
#include "../openxr/openxr_platform.h"
#include <EGL/egl.h>
#include <GLES3/gl32.h>

#include <string>

class GraphicsBackendManager {
public:
    GraphicsBackendManager(ANativeWindow* nativeWindow);
    ~GraphicsBackendManager();
    XrResult InitializeDevice(XrInstance instance, XrSystemId systemId);
    const XrBaseInStructure* GetGraphicsBinding();

private:
    ANativeWindow* _nativeWindow;
    EGLDisplay _nativeDisplay;
    EGLSurface _nativeSurface;
    EGLContext _nativeContext;
    EGLConfig _nativeConfig;
    GLint _contextApiMajorVersion;
    GLuint _swapchainFramebuffer;
};