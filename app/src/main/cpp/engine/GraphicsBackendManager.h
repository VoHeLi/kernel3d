#pragma once

#include "../openxr/openxr.h"
#include "../openxr/openxr_platform.h"
#include <EGL/egl.h>
#include <GLES3/gl32.h>

#include <string>
#include <list>
#include <map>

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

    void InitializeResources();

    void RenderView(const XrCompositionLayerProjectionView& layerView, const XrSwapchainImageBaseHeader* swapchainImage, int64_t swapchainFormat);

    int GetDisplayTexture(int appId);

private:
    ANativeWindow* _nativeWindow;
    EGLDisplay _nativeDisplay;
    EGLSurface _nativeSurface;
    EGLContext _nativeContext;
    EGLConfig _nativeConfig;
    GLint _contextApiMajorVersion;
    GLuint _swapchainFramebuffer;
    std::list<std::vector<XrSwapchainImageOpenGLESKHR>> _swapchainImageBuffers;
    std::map<uint32_t, uint32_t> _colorToDepthMap;


    void CheckShader(GLuint shader);
    void CheckProgram(GLuint prog);

    const uint32_t GetDepthTexture(const uint32_t colorTexture);
    GLuint loadTexture(const char* imagePath);
    GLuint createDisplayTexture();

    GLuint _program;
    GLint _vertexAttribCoords;
    GLuint _debugVbo;
    GLuint _debugEbo;
    GLuint _debugVao;

    GLuint _texture1;
    GLuint _texture2;
    GLuint _texture3;

};