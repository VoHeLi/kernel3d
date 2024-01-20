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

    XrResult result = xrCreateSession(_instance, &createInfo, &_session);

    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "InitializeSession : %d", result);


    /*LogReferenceSpaces();
    InitializeActions();
    CreateVisualizedSpaces();*/ //TODO Implement Space

    /*{
        XrReferenceSpaceCreateInfo referenceSpaceCreateInfo = GetXrReferenceSpaceCreateInfo(m_options->AppSpace);
        CHECK_XRCMD(xrCreateReferenceSpace(m_session, &referenceSpaceCreateInfo, &m_appSpace));
    }*/

    return result;
}

XrResult OpenXRPlugin::CreateSwapchains() {
    // Read graphics properties for preferred swapchain length and logging.
    XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
    XrResult result = xrGetSystemProperties(_instance, _systemId, &systemProperties);

    if(result != XR_SUCCESS){
        __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D", "Swap 1 %d", result);
    }

    // Log system properties.
    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "System Properties: Name=%s VendorId=%d", systemProperties.systemName, systemProperties.vendorId);
    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "System Graphics Properties: MaxWidth=%d MaxHeight=%d MaxLayers=%d",
                                     systemProperties.graphicsProperties.maxSwapchainImageWidth,
                                     systemProperties.graphicsProperties.maxSwapchainImageHeight,
                                     systemProperties.graphicsProperties.maxLayerCount);
    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "System Tracking Properties: OrientationTracking=%s PositionTracking=%s",
                                     systemProperties.trackingProperties.orientationTracking == XR_TRUE ? "True" : "False",
                                     systemProperties.trackingProperties.positionTracking == XR_TRUE ? "True" : "False");


    // Query and cache view configuration views.
    uint32_t viewCount;
    result = xrEnumerateViewConfigurationViews(_instance, _systemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 0, &viewCount, nullptr);
    if(result != XR_SUCCESS){
        __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D", "Swap 2 %d", result);
    }

    _configViews.resize(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
    result = xrEnumerateViewConfigurationViews(_instance, _systemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, viewCount,
                                                  &viewCount, _configViews.data());
    if(result != XR_SUCCESS){
        __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D", "Swap 3 %d", result);
    }
    // Create and cache view buffer for xrLocateViews later.
    _views.resize(viewCount, {XR_TYPE_VIEW});

    // Create the swapchain and get the images.
    if (viewCount > 0) {
        // Select a swapchain format.
        uint32_t swapchainFormatCount;
        result = xrEnumerateSwapchainFormats(_session, 0, &swapchainFormatCount, nullptr);
        if(result != XR_SUCCESS){
            __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D", "Swap 4 %d %p", result, _session);
        }
        std::vector<int64_t> swapchainFormats(swapchainFormatCount);
        result = xrEnumerateSwapchainFormats(_session, (uint32_t)swapchainFormats.size(), &swapchainFormatCount,
                                                swapchainFormats.data());

        if(result != XR_SUCCESS){
            __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D", "Swap 5 %d", result);
        }
        if(swapchainFormatCount != swapchainFormats.size()){
            return XR_ERROR_VALIDATION_FAILURE;
        }

        _colorSwapchainFormat = _graphicsBackendManager->SelectColorSwapchainFormat(swapchainFormats);
        __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "formatCounts : %u", swapchainFormatCount);
        // Print swapchain formats and the selected one.
        {
            std::string swapchainFormatsString;
            for (int64_t format : swapchainFormats) {
                const bool selected = format == _colorSwapchainFormat;
                swapchainFormatsString += " ";
                if (selected) {
                    swapchainFormatsString += "[";
                }
                swapchainFormatsString += std::to_string(format);
                if (selected) {
                    swapchainFormatsString += "]";
                }
            }
            __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "Swapchain Formats: %s", swapchainFormatsString.c_str());
        }

        // Create a swapchain for each view.
        for (uint32_t i = 0; i < viewCount; i++) {
            const XrViewConfigurationView& vp = _configViews[i];
            __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D",
                       "Creating swapchain for view %d with dimensions Width=%d Height=%d SampleCount=%d", i,
                           vp.recommendedImageRectWidth, vp.recommendedImageRectHeight, vp.recommendedSwapchainSampleCount);

            // Create the swapchain.
            XrSwapchainCreateInfo swapchainCreateInfo{XR_TYPE_SWAPCHAIN_CREATE_INFO};
            swapchainCreateInfo.arraySize = 1;
            swapchainCreateInfo.format = _colorSwapchainFormat;
            swapchainCreateInfo.width = vp.recommendedImageRectWidth;
            swapchainCreateInfo.height = vp.recommendedImageRectHeight;
            swapchainCreateInfo.mipCount = 1;
            swapchainCreateInfo.faceCount = 1;
            swapchainCreateInfo.sampleCount = _graphicsBackendManager->GetSupportedSwapchainSampleCount(vp);
            swapchainCreateInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
            Swapchain swapchain;
            swapchain.width = swapchainCreateInfo.width;
            swapchain.height = swapchainCreateInfo.height;
            result = xrCreateSwapchain(_session, &swapchainCreateInfo, &swapchain.handle);
            if(result != XR_SUCCESS){
                __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D", "Swap 6 %d", result);
            }


            _swapchains.push_back(swapchain);

            uint32_t imageCount;
            result =  xrEnumerateSwapchainImages(swapchain.handle, 0, &imageCount, nullptr);
            if(result != XR_SUCCESS){
                __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D", "Swap 7 %d", result);
            }
            // XXX This should really just return XrSwapchainImageBaseHeader*
            std::vector<XrSwapchainImageBaseHeader*> swapchainImages = _graphicsBackendManager->AllocateSwapchainImageStructs(imageCount, swapchainCreateInfo);
            result = xrEnumerateSwapchainImages(swapchain.handle, imageCount, &imageCount, swapchainImages[0]);
            if(result != XR_SUCCESS){
                __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D", "Swap 8 %d", result);
            }

            _swapchainImages.insert(std::make_pair(swapchain.handle, std::move(swapchainImages)));
        }
    }

    return XR_SUCCESS;
}


