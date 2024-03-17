#pragma once

#include "../openxr/openxr_platform.h"
#include "GraphicsBackendManager.h"
#include "SpatialObject.h"
#include "XrAppLayer.h"

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

    XrResult CreateHandTrackerEXT(XrHandTrackerCreateInfoEXT* createInfo, XrHandTrackerEXT* handTracker);
    XrResult DestroyHandTrackerEXT(XrHandTrackerEXT handTracker);
    XrResult LocateHandJointsEXT(XrHandTrackerEXT handTracker, XrHandJointsLocateInfoEXT* locateInfo, XrHandJointLocationsEXT* jointLocations);

    bool IsSessionRunning();

    void PollEvents(bool *pBoolean, bool *pBoolean1);

    void PollActions();

    void PrepareRendering();
    void RenderFrame(std::vector<SpatialObject*> sos, std::vector<XrAppLayer*> appLayers);

    std::vector<XrView> _views;

    XrFrameState _frameState;
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
    bool _sessionRunning = true;
    XrSpace _appSpace;
    XrTime _currentTime;

    std::vector<XrViewConfigurationView> _configViews;

    int64_t _colorSwapchainFormat{-1};
    std::vector<Swapchain> _swapchains;
    std::map<XrSwapchain, std::vector<XrSwapchainImageBaseHeader*>> _swapchainImages;

    XrEventDataBuffer _eventDataBuffer;
    XrSessionState _sessionState;


    XrBaseInStructure* GetInstanceCreateExtension() { return (XrBaseInStructure*)&_instanceCreateInfoAndroid; }

    const XrEventDataBaseHeader *TryReadNextEvent();

    void HandleSessionStateChangedEvent(XrEventDataSessionStateChanged stateChangedEvent, bool* exitRenderLoop, bool* requestRestart);


    bool RenderLayer(XrTime predictedDisplayTime, std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                     XrCompositionLayerProjection& layer, std::vector<SpatialObject*> sos, std::vector<XrAppLayer*> appLayers);
};
