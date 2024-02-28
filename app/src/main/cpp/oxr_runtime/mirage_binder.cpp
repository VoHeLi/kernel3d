#include "mirage_binder.h"
#include "android_globals.h"
#include <android/log.h>

XrResult initializeMirageAppInstance(void* vm, void* clazz){


    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "InitializeMirageAppInstance");

    //Binding to MirageService
    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "Binding to Mirage Service...");

    JNIEnv* env;
    android_globals_get_vm()->GetEnv((void**)&env, JNI_VERSION_1_6);

    jclass contextClass = env->FindClass("android/content/Context");
    jmethodID getSystemServiceMethod = env->GetMethodID(contextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jobject context = (jobject)android_globals_get_context();
    jstring serviceName = env->NewStringUTF("com.androx.kernel3d.PicoreurService");
    jobject service = env->CallObjectMethod(context, getSystemServiceMethod, serviceName);


    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "Bound to Mirage Service!");


    return XR_ERROR_VALIDATION_FAILURE;

}

XrResult destroyMirageInstance(){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult pollMirageEvents(XrEventDataBuffer *eventData){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult miragePathToString(XrPath path, uint32_t bufferCapacityInput, uint32_t *bufferCountOutput, char *buffer){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageStringToPath(const char *pathString, XrPath *out_path){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}


XrResult getMirageSystem(const XrSystemGetInfo* systemGetInfo, XrSystemId* systemId){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult getMirageSystemProperties(XrSystemId systemId, XrSystemProperties *properties){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageEnumerateViewConfigurations( XrSystemId systemId, uint32_t viewConfigurationTypeCapacityInput, uint32_t *viewConfigurationTypeCountOutput, XrViewConfigurationType *viewConfigurationTypes){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageGetViewConfigurationProperties(XrSystemId systemId, XrViewConfigurationType viewConfigurationType, XrViewConfigurationProperties *configurationProperties){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageEnumerateViewConfigurationViews(XrSystemId systemId,XrViewConfigurationType viewConfigurationType, uint32_t viewCapacityInput, uint32_t *viewCountOutput, XrViewConfigurationView *views){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageEnumerateEnvironmentBlendModes(XrSystemId systemId, XrViewConfigurationType viewConfigurationType, uint32_t environmentBlendModeCapacityInput,uint32_t *environmentBlendModeCountOutput,XrEnvironmentBlendMode *environmentBlendModes){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageGetOpenGLESGraphicsRequirementsKHR(XrSystemId systemId, XrGraphicsRequirementsOpenGLESKHR *graphicsRequirements){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}


XrResult mirageCreateSession(const XrSessionCreateInfo *createInfo, XrSession *session){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageDestroySession(XrSession session){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageBeginSession(XrSession session, const XrSessionBeginInfo *beginInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageEndSession(XrSession session){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageWaitFrame(XrSession session, const XrFrameWaitInfo *frameWaitInfo, XrFrameState *frameState){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageBeginFrame(XrSession session, const XrFrameBeginInfo *frameBeginInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageEndFrame(XrSession session, const XrFrameEndInfo *frameEndInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageRequestExitSession(XrSession session){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageLocateViews(XrSession session, const XrViewLocateInfo *viewLocateInfo, XrViewState *viewState, uint32_t viewCapacityInput, uint32_t *viewCountOutput, XrView *views){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}


XrResult mirageEnumerateReferenceSpaces(XrSession session, //PASS TODO : maybe change reference space to stage?
                                        uint32_t spaceCapacityInput,
                                        uint32_t *spaceCountOutput,
                                        XrReferenceSpaceType *spaces){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageGetReferenceSpaceBoundsRect(XrSession session, XrReferenceSpaceType referenceSpaceType, XrExtent2Df *bounds){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageCreateReferenceSpace(XrSession session, const XrReferenceSpaceCreateInfo *createInfo, XrSpace *space){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageLocateSpace(XrSpace space, XrSpace baseSpace, XrTime time, XrSpaceLocation *location){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageDestroySpace(XrSpace space){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageCreateActionSpace(XrSession session, const XrActionSpaceCreateInfo *createInfo, XrSpace *space){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}


//ACTIONS

XrResult mirageCreateActionSet(const XrActionSetCreateInfo *createInfo, XrActionSet *actionSet){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageDestroyActionSet(XrActionSet actionSet){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageCreateAction(XrActionSet actionSet, const XrActionCreateInfo *createInfo, XrAction *action){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageDestroyAction(XrAction action){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageSuggestInteractionProfileBindings(const XrInteractionProfileSuggestedBinding *suggestedBindings){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageAttachSessionActionSets(XrSession session, const XrSessionActionSetsAttachInfo *bindInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageGetCurrentInteractionProfile(XrSession session, XrPath topLevelUserPath, XrInteractionProfileState *interactionProfile){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageGetActionStateBoolean(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStateBoolean *data){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageGetActionStateFloat(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStateFloat *data){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageGetActionStateVector2f(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStateVector2f *data){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageGetActionStatePose(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStatePose *data){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageSyncActions(XrSession session, const XrActionsSyncInfo *syncInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageEnumerateBoundSourcesForAction(XrSession session, const XrBoundSourcesForActionEnumerateInfo *enumerateInfo,
                                              uint32_t sourceCapacityInput, uint32_t *sourceCountOutput, XrPath *sources){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageGetInputSourceLocalizedName(XrSession session, const XrInputSourceLocalizedNameGetInfo *getInfo,
                                           uint32_t bufferCapacityInput, uint32_t *bufferCountOutput, char *buffer){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageApplyHapticFeedback(XrSession session, const XrHapticActionInfo *hapticActionInfo, const XrHapticBaseHeader *hapticEvent){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageStopHapticFeedback(XrSession session, const XrHapticActionInfo *hapticActionInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageCreateHandTrackerEXT(XrSession session, const XrHandTrackerCreateInfoEXT *createInfo, XrHandTrackerEXT *handTracker){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageDestroyHandTrackerEXT(XrHandTrackerEXT handTracker){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageLocateHandJointsEXT(XrHandTrackerEXT handTracker, const XrHandJointsLocateInfoEXT *locateInfo, XrHandJointLocationsEXT *locations){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageApplyForceFeedbackCurlMNDX(XrHandTrackerEXT handTracker, const XrForceFeedbackCurlApplyLocationsMNDX *locations){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageEnumerateDisplayRefreshRatesFB(XrSession session, uint32_t displayRefreshRateCapacityInput,
                                              uint32_t *displayRefreshRateCountOutput, float *displayRefreshRates){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageGetDisplayRefreshRateFB(XrSession session, float *displayRefreshRate){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageRequestDisplayRefreshRateFB(XrSession session, float displayRefreshRate){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}


XrResult mirageEnumerateSwapchainFormats(XrSession session, uint32_t formatCapacityInput, uint32_t *formatCountOutput, int64_t *formats){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageCreateSwapchain(XrSession session, const XrSwapchainCreateInfo *createInfo, XrSwapchain *swapchain){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageDestroySwapchain(XrSwapchain swapchain){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageEnumerateSwapchainImages(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t *imageCountOutput, XrSwapchainImageBaseHeader *images){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageAcquireSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageAcquireInfo *acquireInfo, uint32_t *index){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageWaitSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageWaitInfo *waitInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageReleaseSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageReleaseInfo *releaseInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}


//DEBUG EXT


XrResult mirageSetDebugUtilsObjectNameEXT(const XrDebugUtilsObjectNameInfoEXT *nameInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageCreateDebugUtilsMessengerEXT(
        const XrDebugUtilsMessengerCreateInfoEXT *createInfo,
        XrDebugUtilsMessengerEXT *messenger){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}
XrResult mirageDestroyDebugUtilsMessengerEXT(XrDebugUtilsMessengerEXT messenger){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageSubmitDebugUtilsMessageEXT(
        XrDebugUtilsMessageSeverityFlagsEXT messageSeverity,
        XrDebugUtilsMessageTypeFlagsEXT messageTypes,
        const XrDebugUtilsMessengerCallbackDataEXT *callbackData){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageSessionBeginDebugUtilsLabelRegionEXT(XrSession session, const XrDebugUtilsLabelEXT *labelInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageSessionEndDebugUtilsLabelRegionEXT(XrSession session){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}

XrResult mirageSessionInsertDebugUtilsLabelEXT(XrSession session, const XrDebugUtilsLabelEXT *labelInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}
