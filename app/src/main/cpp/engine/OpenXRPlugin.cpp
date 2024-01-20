#include "OpenXRPlugin.h"

#include <android/log.h>

OpenXRPlugin::OpenXRPlugin(const char* appName, GraphicsBackendManager *graphicsBackendManager) {
    _graphicsBackendManager = graphicsBackendManager;
    _appName = appName;
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
    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "_createInstance Result : %u", result);

    return result;
}

