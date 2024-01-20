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

    _sessionRunning = true;

    /*LogReferenceSpaces();
    InitializeActions();
    CreateVisualizedSpaces();*/ //TODO Implement Space

    {
        XrReferenceSpaceCreateInfo referenceSpaceCreateInfo =
                {XR_TYPE_REFERENCE_SPACE_CREATE_INFO,
                 nullptr,
                XR_REFERENCE_SPACE_TYPE_LOCAL,
                 {
                         {
                             0.0f,0.0f,0.0f,1.0f
                         },
                         {
                             0.0f,0.0f,0.0f
                         }
                 }
                };
        result = xrCreateReferenceSpace(_session, &referenceSpaceCreateInfo, &_appSpace);

        if(result != XR_SUCCESS){
            __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D", "xrCreateReferenceSpace %d", result);
        }
    }

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

bool OpenXRPlugin::IsSessionRunning() {
    //_sessionRunning = true; //TODO REMOVE
    return _sessionRunning;
}

void OpenXRPlugin::PollEvents(bool *exitRenderLoop, bool *requestRestart) {
    *exitRenderLoop = *requestRestart = false;

    // Process all pending messages.
    while (const XrEventDataBaseHeader* event = TryReadNextEvent()) {
        switch (event->type) {
            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
                const auto& instanceLossPending = *reinterpret_cast<const XrEventDataInstanceLossPending*>(event);
                __android_log_print(ANDROID_LOG_WARN, "Androx Kernel3D", "XrEventDataInstanceLossPending by %lld", instanceLossPending.lossTime);
                *exitRenderLoop = true;
                *requestRestart = true;
                return;
            }
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                auto sessionStateChangedEvent = *reinterpret_cast<const XrEventDataSessionStateChanged*>(event);
                HandleSessionStateChangedEvent(sessionStateChangedEvent, exitRenderLoop, requestRestart);
                break;
            }
            case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
                /*LogActionSourceName(m_input.grabAction, "Grab");
                LogActionSourceName(m_input.quitAction, "Quit");
                LogActionSourceName(m_input.poseAction, "Pose");
                LogActionSourceName(m_input.vibrateAction, "Vibrate");*/
                __android_log_print(ANDROID_LOG_WARN, "Androx Kernel3D", "XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED");
                break;
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
            default: {
                __android_log_print(ANDROID_LOG_VERBOSE, "Androx Kernel3D", "Ignoring event type %d", event->type);
                break;
            }
        }
    }
}

const XrEventDataBaseHeader *OpenXRPlugin::TryReadNextEvent() {
    // It is sufficient to clear the just the XrEventDataBuffer header to
    // XR_TYPE_EVENT_DATA_BUFFER
    XrEventDataBaseHeader* baseHeader = reinterpret_cast<XrEventDataBaseHeader*>(&_eventDataBuffer);
    *baseHeader = {XR_TYPE_EVENT_DATA_BUFFER};
    const XrResult xr = xrPollEvent(_instance, &_eventDataBuffer);
    if (xr == XR_SUCCESS) {
        if (baseHeader->type == XR_TYPE_EVENT_DATA_EVENTS_LOST) {
            const XrEventDataEventsLost* const eventsLost = reinterpret_cast<const XrEventDataEventsLost*>(baseHeader);
            __android_log_print(ANDROID_LOG_WARN, "Androx Kernel3D","%d events lost", eventsLost->lostEventCount);
        }

        return baseHeader;
    }
    if (xr == XR_EVENT_UNAVAILABLE) {
        return nullptr;
    }
    return nullptr;
}

void
OpenXRPlugin::HandleSessionStateChangedEvent(XrEventDataSessionStateChanged stateChangedEvent, bool* exitRenderLoop, bool* requestRestart) {

    const XrSessionState oldState = _sessionState;
    _sessionState = stateChangedEvent.state;

    __android_log_print(ANDROID_LOG_INFO, "Androx Kernel3D","XrEventDataSessionStateChanged: state %d->%d session=%lld time=%lld", oldState,
                                     _sessionState, stateChangedEvent.session, stateChangedEvent.time);


    if ((stateChangedEvent.session != XR_NULL_HANDLE) && (stateChangedEvent.session != _session)) {
        __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D","XrEventDataSessionStateChanged for unknown session");
        return;
    }

    switch (_sessionState) {
        case XR_SESSION_STATE_READY: {
            if(_session == XR_NULL_HANDLE){
                break;
            }
            XrSessionBeginInfo sessionBeginInfo{XR_TYPE_SESSION_BEGIN_INFO};
            sessionBeginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
            XrResult result = xrBeginSession(_session, &sessionBeginInfo);
            if(result < XR_SUCCESS){
                __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D","xrBeginSession not ready ! : %d", result);
            }

            _sessionRunning = true;
            break;
        }
        case XR_SESSION_STATE_STOPPING: {
            if(_session == XR_NULL_HANDLE){
                break;
            }
            _sessionRunning = false;
            XrResult result = xrEndSession(_session);
            if(result < XR_SUCCESS){
                __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D","xrEndSession not ready ! : %d", result);
            }
            break;
        }
        case XR_SESSION_STATE_EXITING: {
            *exitRenderLoop = true;
            // Do not attempt to restart because user closed this session.
            *requestRestart = false;
            break;
        }
        case XR_SESSION_STATE_LOSS_PENDING: {
            *exitRenderLoop = true;
            // Poll for a new instance.
            *requestRestart = true;
            break;
        }
        default:
            break;
    }
}

void OpenXRPlugin::PollActions() {
    //TODO Enable Actions and poll
}

void OpenXRPlugin::RenderFrame() {
    XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
    XrFrameState frameState{XR_TYPE_FRAME_STATE};
    XrResult result = xrWaitFrame(_session, &frameWaitInfo, &frameState);
    if(result < XR_SUCCESS){
        __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D","xrWaitFrame ! : %d", result);
    }

    XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
    result = xrBeginFrame(_session, &frameBeginInfo);
    if(result < XR_SUCCESS){
        __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D","xrWaitFrame ! : %d", result);
    }

    std::vector<XrCompositionLayerBaseHeader*> layers;
    XrCompositionLayerProjection layer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    std::vector<XrCompositionLayerProjectionView> projectionLayerViews;
    if (frameState.shouldRender == XR_TRUE) {
        if (RenderLayer(frameState.predictedDisplayTime, projectionLayerViews, layer)) {
            layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&layer));
        }
    }

    XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};
    frameEndInfo.displayTime = frameState.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND; //TODO CHANGE BLEND MODE
    frameEndInfo.layerCount = (uint32_t)layers.size();
    frameEndInfo.layers = layers.data();
    result = xrEndFrame(_session, &frameEndInfo);
    if(result < XR_SUCCESS){
        __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D","xrWaitFrame ! : %d", result);
    }
}

bool OpenXRPlugin::RenderLayer(XrTime predictedDisplayTime, std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                 XrCompositionLayerProjection& layer) {
    XrResult res;

    XrViewState viewState{XR_TYPE_VIEW_STATE};
    uint32_t viewCapacityInput = (uint32_t)_views.size();
    uint32_t viewCountOutput;

    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    viewLocateInfo.displayTime = predictedDisplayTime;
    viewLocateInfo.space = _appSpace; //TODO app space _appSpace

    res = xrLocateViews(_session, &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, _views.data());
    if(res< XR_SUCCESS){
        __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D","xrLocateViews : %d", res);
    }
    if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
        (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0) {
        return false;  // There is no valid tracking poses for the views.
    }

    bool ok = (viewCountOutput == viewCapacityInput)
    && (viewCountOutput == _configViews.size())
    && (viewCountOutput == _swapchains.size());

    if(!ok){
        __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D","Size error when renderig...");
    }

    projectionLayerViews.resize(viewCountOutput);

    //TODO ADVANCED RENDERING

    // Render view to the appropriate part of the swapchain image.
    for (uint32_t i = 0; i < viewCountOutput; i++) {
        // Each view has a separate swapchain which is acquired, rendered to, and released.
        const Swapchain viewSwapchain = _swapchains[i];

        XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};

        uint32_t swapchainImageIndex;
        res = xrAcquireSwapchainImage(viewSwapchain.handle, &acquireInfo, &swapchainImageIndex);
        if(res < XR_SUCCESS){
            __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D","xrAcquireSwapchainImage : %d", res);
        }

        XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitInfo.timeout = XR_INFINITE_DURATION;
        res = xrWaitSwapchainImage(viewSwapchain.handle, &waitInfo);
        if(res < XR_SUCCESS){
            __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D","xrWaitSwapchainImage : %d", res);
        }

        projectionLayerViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
        projectionLayerViews[i].pose = _views[i].pose;
        projectionLayerViews[i].fov = _views[i].fov;
        projectionLayerViews[i].subImage.swapchain = viewSwapchain.handle;
        projectionLayerViews[i].subImage.imageRect.offset = {0, 0};
        projectionLayerViews[i].subImage.imageRect.extent = {viewSwapchain.width, viewSwapchain.height};

        const XrSwapchainImageBaseHeader* const swapchainImage = _swapchainImages[viewSwapchain.handle][swapchainImageIndex];
        _graphicsBackendManager->RenderView(projectionLayerViews[i], swapchainImage, _colorSwapchainFormat);

        XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        res = xrReleaseSwapchainImage(viewSwapchain.handle, &releaseInfo);
        if(res < XR_SUCCESS){
            __android_log_print(ANDROID_LOG_ERROR, "Androx Kernel3D","xrWaitSwapchainImage : %d", res);
        }
    }

    layer.space = _appSpace;
    layer.layerFlags =
            //m_options->Parsed.EnvironmentBlendMode == XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND //TODO CUSTOMIZE
            true ? XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT | XR_COMPOSITION_LAYER_UNPREMULTIPLIED_ALPHA_BIT
            : 0;
    layer.viewCount = (uint32_t)projectionLayerViews.size();
    layer.views = projectionLayerViews.data();
    return true;
}


