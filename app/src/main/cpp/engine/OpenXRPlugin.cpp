#include "OpenXRPlugin.h"

#include <android/log.h>

OpenXRPlugin::OpenXRPlugin(const char* appName, GraphicsBackendManager *graphicsBackendManager) {
    _graphicsBackendManager = graphicsBackendManager;
    _appName = appName;
}

OpenXRPlugin::~OpenXRPlugin() {
    //TODO

    xrDestroySession(_session);
    xrDestroyInstance(_instance);
}

XrResult OpenXRPlugin::InitializeLoaderAndJavaContext(JavaVM * jvm, jobject activity) {
    PFN_xrInitializeLoaderKHR initializeLoader = nullptr;
    XrResult result = xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction*)(&initializeLoader));

    if (XR_SUCCEEDED(result)) {
        XrLoaderInitInfoAndroidKHR loaderInitInfoAndroid = {XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR};
        loaderInitInfoAndroid.applicationVM = jvm;
        loaderInitInfoAndroid.applicationContext = activity;
        initializeLoader((const XrLoaderInitInfoBaseHeaderKHR*)&loaderInitInfoAndroid);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "After loader");

    _instanceCreateInfoAndroid = {XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR};
    _instanceCreateInfoAndroid.applicationVM = jvm;
    _instanceCreateInfoAndroid.applicationActivity = activity;

    return result;
}

XrResult OpenXRPlugin::CreateInstance() {
    //if(_instance != nullptr) return XR_ERROR_VALIDATION_FAILURE;

    // Create union of extensions required by platform and graphics plugins.
    std::vector<const char*> extensions = {XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME, XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME};

    XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    createInfo.next = GetInstanceCreateExtension();
    createInfo.enabledExtensionCount = (uint32_t)extensions.size();
    createInfo.enabledExtensionNames = extensions.data();

    strcpy(createInfo.applicationInfo.applicationName, _appName);
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "Before create instance");
    XrResult result = xrCreateInstance(&createInfo, &_instance);
    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "_createInstance Result : %d", result);

    return result;
}

XrResult OpenXRPlugin::InitializeSystemId() {
    XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    XrResult result = xrGetSystem(_instance, &systemInfo, &_systemId);

    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "_getSystem Result : %d", result);

    return result;
}

XrResult OpenXRPlugin::InitializeDevice(){
    return _graphicsBackendManager->InitializeDevice(_instance, _systemId);
}

XrResult OpenXRPlugin::InitializeSession() {


    const XrBaseInStructure* structure = _graphicsBackendManager->GetGraphicsBinding();


    XrSessionCreateInfo createInfo = {
            XR_TYPE_SESSION_CREATE_INFO,
            structure,
            0,
            _systemId
    };


    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "Creating Session... : %p, %p, %p, %p, %p, %p",
                        ((XrGraphicsBindingOpenGLESAndroidKHR*)structure)->type,
                        ((XrGraphicsBindingOpenGLESAndroidKHR*)structure)->next,
                        ((XrGraphicsBindingOpenGLESAndroidKHR*)structure)->display,
                        ((XrGraphicsBindingOpenGLESAndroidKHR*)structure)->config,
                        ((XrGraphicsBindingOpenGLESAndroidKHR*)structure)->context,
                        _instance);

    XrSession session;
    XrResult result = xrCreateSession(_instance, &createInfo, &session);

    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "InitializeSession : %d", result);


    /*LogReferenceSpaces();
    InitializeActions();
    CreateVisualizedSpaces();*/

    /*{
        XrReferenceSpaceCreateInfo referenceSpaceCreateInfo = GetXrReferenceSpaceCreateInfo(m_options->AppSpace);
        CHECK_XRCMD(xrCreateReferenceSpace(m_session, &referenceSpaceCreateInfo, &m_appSpace));
    }*/

    return result;
}


