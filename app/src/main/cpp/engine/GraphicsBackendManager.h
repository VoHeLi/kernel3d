#pragma once

#include "../openxr/openxr.h"
#include "../openxr/openxr_platform.h"
#include <EGL/egl.h>
#include <GLES3/gl32.h>

#include <string>
#include <list>

class GraphicsBackendManager {
public:
    GraphicsBackendManager(ANativeWindow* nativeWindow);
    ~GraphicsBackendManager();
    XrResult InitializeDevice(XrInstance instance, XrSystemId systemId);
    const XrBaseInStructure* GetGraphicsBinding();
    int64_t SelectColorSwapchainFormat(std::vector<int64_t> runtimeFormats);

    uint32_t GetSupportedSwapchainSampleCount(const XrViewConfigurationView &view);

    std::vector<XrSwapchainImageBaseHeader *>
    AllocateSwapchainImageStructs(uint32_t i, XrSwapchainCreateInfo info);

private:
    ANativeWindow* _nativeWindow;
    EGLDisplay _nativeDisplay;
    EGLSurface _nativeSurface;
    EGLContext _nativeContext;
    EGLConfig _nativeConfig;
    GLint _contextApiMajorVersion;
    GLuint _swapchainFramebuffer;
    std::list<std::vector<XrSwapchainImageOpenGLESKHR>> _swapchainImageBuffers;

};