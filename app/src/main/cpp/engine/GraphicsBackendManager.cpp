#include "GraphicsBackendManager.h"

#include <android/log.h>
#include <vector>

#include "gfxwrapper_opengl.h"

GraphicsBackendManager::GraphicsBackendManager(ANativeWindow* nativeWindow) {
    _nativeWindow = nativeWindow;
}

GraphicsBackendManager::~GraphicsBackendManager() {
    eglMakeCurrent(_nativeDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    //eglDestroySurface(eglDisplay, eglSurface);

    //TODO
}

void DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) {
    (void)source;
    (void)type;
    (void)id;
    (void)severity;
    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "GLES Debug: %s", (const char*)message);
}

XrResult GraphicsBackendManager::InitializeDevice(XrInstance instance, XrSystemId systemId){
    // Extension function must be loaded by name
    PFN_xrGetOpenGLESGraphicsRequirementsKHR pfnGetOpenGLESGraphicsRequirementsKHR = nullptr;
    xrGetInstanceProcAddr(instance, "xrGetOpenGLESGraphicsRequirementsKHR",
                                      reinterpret_cast<PFN_xrVoidFunction*>(&pfnGetOpenGLESGraphicsRequirementsKHR));

    XrGraphicsRequirementsOpenGLESKHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR};
    XrResult result = pfnGetOpenGLESGraphicsRequirementsKHR(instance, systemId, &graphicsRequirements);

    ksGpuWindow window;
    ksDriverInstance driverInstance{};
    ksGpuQueueInfo queueInfo{};
    ksGpuSurfaceColorFormat colorFormat{KS_GPU_SURFACE_COLOR_FORMAT_B8G8R8A8};
    ksGpuSurfaceDepthFormat depthFormat{KS_GPU_SURFACE_DEPTH_FORMAT_D24};
    ksGpuSampleCount sampleCount{KS_GPU_SAMPLE_COUNT_1};
    if (!ksGpuWindow_Create(&window, &driverInstance, &queueInfo, 0, colorFormat, depthFormat, sampleCount, 640, 480, true)) {
        __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "Unable to create GL context");
    }

    GLint major = 0;
    GLint minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    const XrVersion desiredApiVersion = XR_MAKE_VERSION(major, minor, 0);
    if (graphicsRequirements.minApiVersionSupported > desiredApiVersion) {
        __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "Runtime does not support desired Graphics API and/or version");
    }

    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "Runtime initialization versions : minSupported : %u.%u, desired : %u.%u",
                        XR_VERSION_MAJOR(graphicsRequirements.minApiVersionSupported), XR_VERSION_MINOR(graphicsRequirements.minApiVersionSupported), major, minor);

    _contextApiMajorVersion = major;

    _nativeDisplay = window.display;
    _nativeConfig = (EGLConfig)0;
    _nativeContext = window.context.context;

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(
            [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
               const void* userParam) {
                DebugMessageCallback(source, type, id, severity, length, message);
            },
            this);

    //InitializeResources(); //TODO

    //test
    //glClearColor(1.0f, 1.0f, 0.0f, 0.5f);
    //glClear(GL_COLOR_BUFFER_BIT);

    //eglSwapBuffers(_nativeDisplay, _nativeSurface);

    glGenFramebuffers(1, &_swapchainFramebuffer);

    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "InitializeDevice : %u", result);
    return result;
}

const XrBaseInStructure* GraphicsBackendManager::GetGraphicsBinding() {
    XrGraphicsBindingOpenGLESAndroidKHR* bindings = (XrGraphicsBindingOpenGLESAndroidKHR*) malloc(101); //MAGIC NUMBER 49, TODO UNDERSTAND WHY MONADO USES EXTRA SPACE
    bindings->type = XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR;
    bindings->display = _nativeDisplay;
    bindings->config = _nativeConfig;
    bindings->context = _nativeContext;

    for(int i = 0; i < 10; i++){
        char data = ((char*)bindings)[i];
        __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "Dump %d : %p", i, data);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "TEST %U : %p, %p, %p", sizeof(XrGraphicsBindingOpenGLESAndroidKHR), _nativeDisplay, _nativeContext, _nativeConfig);

    return (XrBaseInStructure*)bindings; //debug
}

int64_t GraphicsBackendManager::SelectColorSwapchainFormat(std::vector<int64_t> runtimeFormats) {
    std::vector<int64_t> supportedColorSwapchainFormats{GL_RGBA8, GL_RGBA8_SNORM};

    // In OpenGLES 3.0+, the R, G, and B values after blending are converted into the non-linear
    // sRGB automatically.
    if (_contextApiMajorVersion >= 3) {
        supportedColorSwapchainFormats.push_back(GL_SRGB8_ALPHA8);
    }

    auto swapchainFormatIt = std::find_first_of(runtimeFormats.begin(), runtimeFormats.end(),
                                                supportedColorSwapchainFormats.begin(), supportedColorSwapchainFormats.end());
    if (swapchainFormatIt == runtimeFormats.end()) {
        __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "No runtime swapchain format supported for color swapchain");
        return XR_ERROR_VALIDATION_FAILURE;
    }

    return *swapchainFormatIt;
}

uint32_t
GraphicsBackendManager::GetSupportedSwapchainSampleCount(const XrViewConfigurationView &view) {
    return 1;
}

std::vector<XrSwapchainImageBaseHeader *>
GraphicsBackendManager::AllocateSwapchainImageStructs(uint32_t capacity, XrSwapchainCreateInfo info) {
    // Allocate and initialize the buffer of image structs (must be sequential in memory for xrEnumerateSwapchainImages).
    // Return back an array of pointers to each swapchain image struct so the consumer doesn't need to know the type/size.
    std::vector<XrSwapchainImageOpenGLESKHR> swapchainImageBuffer(capacity, {XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR});
    std::vector<XrSwapchainImageBaseHeader*> swapchainImageBase;
    for (XrSwapchainImageOpenGLESKHR& image : swapchainImageBuffer) {
        swapchainImageBase.push_back(reinterpret_cast<XrSwapchainImageBaseHeader*>(&image));
    }

    // Keep the buffer alive by moving it into the list of buffers.
    _swapchainImageBuffers.push_back(std::move(swapchainImageBuffer));

    return swapchainImageBase;
}



/*// Initialize the gl extensions. Note we have to open a window.
    EGLint eglMajor, eglMinor;

    // Initialize EGL
    _nativeDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(_nativeDisplay, &eglMajor, &eglMinor);

    // Configure EGL
    EGLint configAttribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_NONE};

    EGLint numConfigs;

    eglChooseConfig(_nativeDisplay, configAttribs, &_nativeConfig, 1, &numConfigs);

    // Create a surface
    _nativeSurface = eglCreateWindowSurface(_nativeDisplay, _nativeConfig, _nativeWindow, NULL);

    // Create a context
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    _nativeContext = eglCreateContext(_nativeDisplay, _nativeConfig, EGL_NO_CONTEXT, contextAttribs);

    // Make the context current
    eglMakeCurrent(_nativeDisplay, _nativeSurface, _nativeSurface, _nativeContext);

    TODO REMOVE DEBUG, OLD CODE
    */