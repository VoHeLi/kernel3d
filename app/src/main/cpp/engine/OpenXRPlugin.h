#pragma once

#include "../openxr/openxr_platform.h"
#include "GraphicsBackendManager.h"

#include <vector>

class OpenXRPlugin {
public:
    OpenXRPlugin(const char* appName, GraphicsBackendManager* graphicsBackendManager);
    ~OpenXRPlugin();
    XrResult InitializeLoaderAndJavaContext(JavaVM* jvm, jobject activity);
    XrResult CreateInstance();
    XrResult InitializeSystemId();
    XrResult InitializeDevice();
    XrResult InitializeSession();
private:
    GraphicsBackendManager* _graphicsBackendManager;
    XrInstanceCreateInfoAndroidKHR _instanceCreateInfoAndroid;
    XrInstance _instance;
    const char* _appName;
    XrSystemId _systemId;
    XrSession _session;

    XrBaseInStructure* GetInstanceCreateExtension() { return (XrBaseInStructure*)&_instanceCreateInfoAndroid; }
};
