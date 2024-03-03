#include "mirage_binder.h"
#include "android_globals.h"
#include <android/log.h>
#include <string.h>
#include <android/sharedmem.h>
#include <android/sharedmem_jni.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

char* readFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(buffer, 1, fileSize, file);
    fclose(file);

    if (bytesRead < fileSize) {
        free(buffer);
        return NULL;
    }

    buffer[bytesRead] = '\0'; // Null-terminate the string
    return buffer;
}


XrResult initializeMirageAppInstance(void* vm, void* clazz){



    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "InitializeMirageAppInstance");

//     __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Binding to Mirage Service...");
//
//    JNIEnv* env;
//    android_globals_get_vm()->GetEnv((void**)&env, JNI_VERSION_1_6);
//
//    jclass contextClass = env->FindClass("android/content/Context");
//    jmethodID getSystemServiceMethod = env->GetMethodID(contextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
//    jobject context = (jobject)android_globals_get_context();
//    jstring serviceName = env->NewStringUTF("com.androx.kernel3d.PicoreurService");
//    jobject service = env->CallObjectMethod(context, getSystemServiceMethod, serviceName);
//
//    sleep(1); //DEBUG
//
//    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Bound to Mirage Service!");

    const char* dataFD = readFromFile("/storage/self/primary/testAppFD.data");

    int fd = atoi(dataFD);

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Got fd : %d", fd);

    size_t memSize = ASharedMemory_getSize(fd);
    char *buffer = (char *) mmap(NULL, 16, PROT_READ, MAP_SHARED, fd, 0);

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "ERRNO : %d", errno);
    //char a = buffer[0];

//    char* testStr = (char*)malloc(65);
//    memcpy(testStr, buffer, 64);
//    testStr[64] = 0;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Reading : %d, %d, %d, %d", buffer[0], buffer[1], buffer[2], buffer[3]);

    //__android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Reading : %s", testStr);

    //kill(5373, SIGKILL);

    //Binding to MirageService
//    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Binding to Mirage Service...");
//
//    JNIEnv* env;
//    android_globals_get_vm()->GetEnv((void**)&env, JNI_VERSION_1_6);
//
//    jclass contextClass = env->FindClass("android/content/Context");
//    jmethodID getSystemServiceMethod = env->GetMethodID(contextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
//    jobject context = (jobject)android_globals_get_context();
//    jstring serviceName = env->NewStringUTF("com.androx.kernel3d.PicoreurService");
//    jobject service = env->CallObjectMethod(context, getSystemServiceMethod, serviceName);
//
//
//    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Bound to Mirage Service!");

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "InitializeMirageAppInstanceEnd");

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
