#pragma once

#include "../openxr/openxr_platform.h"
#include "GraphicsBackendManager.h"

#include <vector>
#include <map>

class OpenXRPlugin {
public:
    OpenXRPlugin(const char* appName, GraphicsBackendManager* graphicsBackendManager);
    ~OpenXRPlugin();
    XrResult InitializeLoaderAndJavaContext(JavaVM* jvm, jobject activity);
    XrResult CreateInstance();
    XrResult InitializeSystemId();
    XrResult InitializeDevice();
    XrResult InitializeSession();
    XrResult CreateSwapchains();
private:
    struct Swapchain {
        XrSwapchain handle;
        int32_t width;
        int32_t height;
    };

    GraphicsBackendManager* _graphicsBackendManager;
    XrInstanceCreateInfoAndroidKHR _instanceCreateInfoAndroid;
    XrInstance _instance;
    const char* _appName;
    XrSystemId _systemId;
    XrSession _session;

    std::vector<XrViewConfigurationView> _configViews;
    std::vector<XrView> _views;
    int64_t _colorSwapchainFormat{-1};
    std::vector<Swapchain> _swapchains;
    std::map<XrSwapchain, std::vector<XrSwapchainImageBaseHeader*>> _swapchainImages;




    XrBaseInStructure* GetInstanceCreateExtension() { return (XrBaseInStructure*)&_instanceCreateInfoAndroid; }
};
