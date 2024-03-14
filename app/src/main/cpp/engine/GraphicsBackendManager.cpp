#include "GraphicsBackendManager.h"

#include <android/log.h>
#include <vector>
#include <chrono>

#include "gfxwrapper_opengl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"
#include "SpatialObject.h"
#include "../glm/gtc/type_ptr.hpp"
#include "mirage_shared/common_types.h"

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

    glGenFramebuffers(1, &_swapchainFramebuffer);


    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "InitializeDevice : %u", result);
    return result;
}

const XrBaseInStructure* GraphicsBackendManager::GetGraphicsBinding() {
    XrGraphicsBindingOpenGLESAndroidKHR* bindings = (XrGraphicsBindingOpenGLESAndroidKHR*) malloc(1000); //MAGIC NUMBER 49, TODO UNDERSTAND WHY MONADO USES EXTRA SPACE
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

void GraphicsBackendManager::RenderView(const XrCompositionLayerProjectionView& layerView, const XrSwapchainImageBaseHeader* swapchainImage, int64_t swapchainFormat, std::vector<SpatialObject*> sos) {
    //CHECK(layerView.subImage.imageArrayIndex == 0);  // Texture arrays not supported.
    UNUSED_PARM(swapchainFormat);                    // Not used in this function for now.

    glBindFramebuffer(GL_FRAMEBUFFER, _swapchainFramebuffer);

    const uint32_t colorTexture = reinterpret_cast<const XrSwapchainImageOpenGLESKHR*>(swapchainImage)->image;

    glViewport(static_cast<GLint>(layerView.subImage.imageRect.offset.x),
               static_cast<GLint>(layerView.subImage.imageRect.offset.y),
               static_cast<GLsizei>(layerView.subImage.imageRect.extent.width),
               static_cast<GLsizei>(layerView.subImage.imageRect.extent.height));

    //glFrontFace(GL_CW);
    //glCullFace(GL_BACK);
    //glEnable(GL_CULL_FACE);
    //glEnable(GL_DEPTH_TEST);

    const uint32_t depthTexture = GetDepthTexture(colorTexture);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    //TODO TRANSLATE CODE TO ANOTHER CLASS

    XrPosef cameraPos = layerView.pose;

    glm::vec3 position = glm::vec3(cameraPos.position.x, cameraPos.position.y, cameraPos.position.z);
    glm::quat rotation = glm::quat(cameraPos.orientation.w, cameraPos.orientation.x, cameraPos.orientation.y, cameraPos.orientation.z);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    SpatialObject* camObj = new SpatialObject(position, rotation, scale, 0);
    glm::mat4x4 viewMatrix = glm::inverse(camObj->getTransformationMatrix());
    delete camObj;

    //CALCUL DE LA MATRICE DE PROJECTION
    float n = 0.1f;
    float f = 1000.0f;
    float l = tanf(layerView.fov.angleLeft);
    float r = tanf(layerView.fov.angleRight);
    float t = tanf(layerView.fov.angleUp);
    float b = tanf(layerView.fov.angleDown);

    glm::mat4x4 projectionMatrix = glm::mat4x4{
        2*n/(r-l), 0, (r+l)/(r-l), 0,
        0, 2*n/(t-b), (t+b)/(t-b), 0,
        0, 0, -(f+n)/(f-n), -(2*f*n)/(f-n),
        0, 0, -1, 0
    };

    //COLONNE PAR COLONNE, la matrice finale est transposee!!!

    glm::mat4x4 projectionMatrix2 = glm::mat4x4{
            2/(r-l), 0, 0, 0,
            0, 2/(t-b), 0, 0,
            (r+l)/(r-l), (t+b)/(t-b), -(f+n)/(f-n), -1,
            0, 0, -(2*f*n)/(f-n), 0
    };

    //__android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "%.6f %.6f %.6f %.6f", layerView.fov.angleLeft, layerView.fov.angleRight, layerView.fov.angleUp, layerView.fov.angleDown);

    /*glm::mat4x4 testMatrix = glm::mat4x4{
            2/(r-l), 0, 0, 10,
            0, 2/(t-b), 0, 0,
            (r+l)/(r-l), (t+b)/(t-b), 1, 0,
            0, 0, 0, 1
    };*/



    //TODO REMOVE DEBUG

    // Clear swapchain and depth buffer.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //glClearDepthf(1.0f);
    glClear(GL_COLOR_BUFFER_BIT); //debug

    glUseProgram(_program);

    GLint finalMatrixLocation = glGetUniformLocation(_program, "finalMatrix");

    glm::mat4x4 finalMatrix = projectionMatrix2 * viewMatrix;
    glUniformMatrix4fv(finalMatrixLocation, 1, false, glm::value_ptr(finalMatrix));


    for(SpatialObject* so : sos){
        glBindBuffer(GL_ARRAY_BUFFER, _debugVbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glUniformMatrix4fv(finalMatrixLocation, 1, false, glm::value_ptr(finalMatrix*so->getTransformationMatrix()));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(DEBUG_TEXTURE_TYPE, so->_textureId);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableVertexAttribArray(0);
    }



    glUseProgram(0);
    //TODO RENDER REAL Objects

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const uint32_t GraphicsBackendManager::GetDepthTexture(const uint32_t colorTexture) {
    // If a depth-stencil view has already been created for this back-buffer, use it.
    auto depthBufferIt = _colorToDepthMap.find(colorTexture);
    if (depthBufferIt != _colorToDepthMap.end()) {
        return depthBufferIt->second;
    }

    // This back-buffer has no corresponding depth-stencil texture, so create one with matching dimensions.

    GLint width;
    GLint height;
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

    uint32_t depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

    _colorToDepthMap.insert(std::make_pair(colorTexture, depthTexture));

    return depthTexture;
}

static const char* VertexShaderGlsl = R"_(#version 320 es

    in vec3 VertexPos;


    void main() {
       gl_Position = vec4(VertexPos, 1.0);
    }
    )_";

// The version statement has come on first line.
static const char* FragmentShaderGlsl = R"_(#version 320 es

    out lowp vec4 FragColor;

    void main() {
        FragColor = vec4(1,1,1,1);
    }
    )_";


// Vertex shader source code
const char* vertexShaderSource =
        "#version 300 es\n"
        "#extension GL_OES_EGL_image_external : enable\n"
        "#extension GL_OES_texture_3D : enable\n"
        "in vec4 position;\n"
        "out vec2 textureCoords;\n"
        "uniform mat4 finalMatrix;\n"
        "void main() {\n"
        "  gl_Position = finalMatrix*position;"
        "  textureCoords = vec2((position.x+0.5), 1.0-(position.y+0.5)); \n"
        "}\0";

// Fragment shader source code
const char* fragmentShaderSource =
        "#version 300 es\n"
        "#extension GL_OES_EGL_image_external : enable\n"
        "#extension GL_OES_texture_3D : enable\n"
        "precision mediump float;\n"
        "uniform sampler2DArray textureSampler;\n" //Change between this and external samplerExternalOES, sampler2D
        "in vec2 textureCoords;\n"
        "out vec4 fragColor;\n"
        "void main() {\n"
        "  fragColor = vec4(0.0, 0.0, 1.0, 1.0);\n"
        "  fragColor = texture(textureSampler, vec3(textureCoords, 0));\n"
        //"  gl_FragColor = texture2DArray(textureSampler, textureCoords);\n"
        "}\0";

GLuint GraphicsBackendManager::loadTexture(const char* imagePath) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true); // Inverser l'image en Y (OpenGL a l'origine en bas à gauche)
    unsigned char* data = stbi_load(imagePath, &width, &height, &channels, 0);

    if (data) {
        GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }

    return textureID;
}

GLuint GraphicsBackendManager::createDisplayTexture(){
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(DEBUG_TEXTURE_TYPE, textureID);
    //glTexImage2D(DEBUG_TEXTURE_TYPE, 0, GL_RGBA, 1280, 720, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameterf(DEBUG_TEXTURE_TYPE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(DEBUG_TEXTURE_TYPE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(DEBUG_TEXTURE_TYPE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(DEBUG_TEXTURE_TYPE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return textureID;
}

void GraphicsBackendManager::InitializeResources() {
    glEnable(GL_OES_EGL_image_external);
    glEnable(GL_OES_texture_3D);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    _program = glCreateProgram();
    glAttachShader(_program, vertexShader);
    glAttachShader(_program, fragmentShader);
    glLinkProgram(_program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set up VBO for quad
    /*GLfloat vertices[] = {
            -0.27f, -0.48f, -0.5f,
            0.27f, -0.48f, -0.5f,
            -0.27f,  0.48f, -0.5f,
            0.27f,  0.48f, -0.5f
    };*/

    GLfloat vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
            0.5f,  0.5f, 0.0f
    };

    glGenBuffers(1, &_debugVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _debugVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //_texture = loadTexture("/storage/emulated/0/test_texture.jpg");
    _texture1 = createDisplayTexture();
    _texture2 = createDisplayTexture();
    _texture3 = createDisplayTexture();


    GLint textureSamplerLocation = glGetUniformLocation(_program, "textureSampler");

    glUseProgram(_program);
    glUniform1i(textureSamplerLocation, 0);
    glUseProgram(0);

    //TODO, MOVE TO ANOTHER CLASS
    /*
    // Vertices for a quad
    GLfloat vertices[] = {
            // Positions
            -0.5f,  0.5f, 0.0f,   // Top-left
            0.5f,  0.5f, 0.0f,  // Top-right
            0.5f, -0.5f, 0.0f,  // Bottom-right
            -0.5f, -0.5f, 0.0f   // Bottom-left
    };

    // Indices for two triangles forming a quad
    GLuint indices[] = {
            0, 1, 2,  // First triangle
            0, 2, 3   // Second triangle
    };

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VertexShaderGlsl, nullptr);
    glCompileShader(vertexShader);
    CheckShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FragmentShaderGlsl, nullptr);
    glCompileShader(fragmentShader);
    CheckShader(fragmentShader);

    _program = glCreateProgram();
    glAttachShader(_program, vertexShader);
    glAttachShader(_program, fragmentShader);
    glLinkProgram(_program);
    CheckProgram(_program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    _vertexAttribCoords = glGetAttribLocation(_program, "VertexPos");

    glGenBuffers(1, &_debugVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _debugVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &_debugEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _debugEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &_debugVao);
    glBindVertexArray(_debugVao);
    glEnableVertexAttribArray(_vertexAttribCoords);
    glBindBuffer(GL_ARRAY_BUFFER, _debugVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _debugEbo);
    //glVertexAttribPointer(_vertexAttribCoords, 4, GL_FLOAT, GL_FALSE, 3, nullptr);
    glVertexAttribPointer(_vertexAttribCoords, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);*/
}

void GraphicsBackendManager::CheckShader(GLuint shader) {
    GLint r = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
    if (r == GL_FALSE) {
        GLchar msg[4096] = {};
        GLsizei length;
        glGetShaderInfoLog(shader, sizeof(msg), &length, msg);
        __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D", "Compile shader failed: %s", msg);
    }
}

void GraphicsBackendManager::CheckProgram(GLuint prog) {
    GLint r = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &r);
    if (r == GL_FALSE) {
        GLchar msg[4096] = {};
        GLsizei length;
        glGetProgramInfoLog(prog, sizeof(msg), &length, msg);
        __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D", "Link program failed: %s", msg);
    }
}

AHardwareBuffer* debugHardwareBuffer = nullptr;
bool debugHardwareBufferInited = false;

int GraphicsBackendManager::GetDisplayTexture(int id) {
    /*switch(id){
        case 1:
            return _texture1;
            break;
        case 2:
            return _texture2;
            break;
        case 3:
            return _texture3;
            break;
    }*/

    if(!debugHardwareBufferInited && debugHardwareBuffer != nullptr){
        __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "Linking display with hardwarebuffer : %p", debugHardwareBuffer);

        EGLClientBuffer eglClientBuffer = eglGetNativeClientBufferANDROID(debugHardwareBuffer);

        EGLImageKHR eglImageKHR = eglCreateImageKHR(eglGetCurrentDisplay(), EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, eglClientBuffer, nullptr);
        glBindTexture(DEBUG_TEXTURE_TYPE, _texture1);
        glEGLImageTargetTexture2DOES(DEBUG_TEXTURE_TYPE, eglImageKHR);

        glBindTexture(DEBUG_TEXTURE_TYPE, 0);
        __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "Display linked with hardwarebuffer : %p", debugHardwareBuffer);
        debugHardwareBufferInited = true;
    }

    return _texture1;
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