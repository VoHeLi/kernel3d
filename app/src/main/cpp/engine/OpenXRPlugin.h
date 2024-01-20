#pragma once

#include "../openxr/openxr_platform.h"
#include "GraphicsBackendManager.h"

#include <vector>

class OpenXRPlugin {
public:
    OpenXRPlugin(const char* appName, GraphicsBackendManager* graphicsBackendManager);
    XrResult InitializeLoaderAndJavaContext(JavaVM* jvm, jobject activity);
    XrResult CreateInstance();
private:
    GraphicsBackendManager* _graphicsBackendManager;
    XrInstanceCreateInfoAndroidKHR _instanceCreateInfoAndroid;
    XrInstance _instance;
    const char* _appName;


    XrBaseInStructure* GetInstanceCreateExtension() { return (XrBaseInStructure*)&_instanceCreateInfoAndroid; }
    /*AndroidPlatformPlugin(const std::shared_ptr<Options>& , const std::shared_ptr<PlatformData>& data) {
        instanceCreateInfoAndroid = {XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR};
        instanceCreateInfoAndroid.applicationVM = data->applicationVM;
        instanceCreateInfoAndroid.applicationActivity = data->applicationActivity;
    }
    std::vector<std::string> GetInstanceExtensions() const override { return {XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME}; }



    void UpdateOptions(const std::shared_ptr<struct Options>&) override {}

    */
};
