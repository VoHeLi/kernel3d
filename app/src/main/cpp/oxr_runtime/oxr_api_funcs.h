// Copyright 2018-2019, Collabora, Ltd.
// SPDX-License-Identifier: BSL-1.0
/*!
 * @file
 * @brief  Header defining all API functions.
 * @author Jakob Bornecrantz <jakob@collabora.com>
 * @ingroup oxr_api
 */



#pragma once

#include <android/log.h>
#include <jni.h>

#include <jni.h>
#include "../openxr/openxr_platform_defines.h"
#include "openxr/openxr_platform.h"


extern "C" {

/*!
 * @defgroup oxr_api OpenXR entrypoints
 *
 * Gets called from the client application, does most verification and routes
 * calls into @ref oxr_main functions.
 *
 * @ingroup oxr
 * @{
 */



/*
 *
 * oxr_api_negotiate.c
 *
 */

//! OpenXR API function @ep{xrGetInstanceProcAddr}
JNIEXPORT XRAPI_ATTR XrResult XRAPI_CALL
xrGetInstanceProcAddr(XrInstance instance, const char *name, PFN_xrVoidFunction *function);

//! OpenXR API function @ep{xrEnumerateApiLayerProperties}
JNIEXPORT XRAPI_ATTR XrResult XRAPI_CALL
xrEnumerateApiLayerProperties(uint32_t propertyCapacityInput,
                              uint32_t *propertyCountOutput,
                              XrApiLayerProperties *properties);


/*
 *
 * oxr_api_instance.c
 *
 */

//! OpenXR API function @ep{xrInitializeLoaderKHR}
JNIEXPORT XRAPI_ATTR XrResult XRAPI_CALL
xrInitializeLoaderKHR(const XrLoaderInitInfoBaseHeaderKHR *loaderInitInfo);

//! OpenXR API function @ep{xrEnumerateInstanceExtensionProperties}
JNIEXPORT XRAPI_ATTR XrResult XRAPI_CALL
xrEnumerateInstanceExtensionProperties(const char *layerName,
                                       uint32_t propertyCapacityInput,
                                       uint32_t *propertyCountOutput,
                                       XrExtensionProperties *properties);


//! OpenXR API function @ep{xrCreateInstance}
JNIEXPORT XRAPI_ATTR XrResult XRAPI_CALL
xrCreateInstance(const XrInstanceCreateInfo *createInfo, XrInstance *instance);



//! OpenXR API function @ep{xrDestroyInstance}
XRAPI_ATTR XrResult XRAPI_CALL
xrDestroyInstance(XrInstance instance);

//! OpenXR API function @ep{xrGetInstanceProperties}
XRAPI_ATTR XrResult XRAPI_CALL
xrGetInstanceProperties(XrInstance instance, XrInstanceProperties *instanceProperties);

//! OpenXR API function @ep{xrPollEvent}
XRAPI_ATTR XrResult XRAPI_CALL
xrPollEvent(XrInstance instance, XrEventDataBuffer *eventData);

//! OpenXR API function @ep{xrResultToString}
XRAPI_ATTR XrResult XRAPI_CALL
xrResultToString(XrInstance instance, XrResult value, char buffer[XR_MAX_RESULT_STRING_SIZE]);

//! OpenXR API function @ep{xrStructureTypeToString}
XRAPI_ATTR XrResult XRAPI_CALL
xrStructureTypeToString(XrInstance instance, XrStructureType value, char buffer[XR_MAX_STRUCTURE_NAME_SIZE]);

//! OpenXR API function @ep{xrStringToPath}
XRAPI_ATTR XrResult XRAPI_CALL
xrStringToPath(XrInstance instance, const char *pathString, XrPath *path);

//! OpenXR API function @ep{xrPathToString}
XRAPI_ATTR XrResult XRAPI_CALL
xrPathToString(
    XrInstance instance, XrPath path, uint32_t bufferCapacityInput, uint32_t *bufferCountOutput, char *buffer);

/*
//! OpenXR API function @ep{xrConvertTimespecTimeToTimeKHR}
XRAPI_ATTR XrResult XRAPI_CALL
oxr_xrConvertTimespecTimeToTimeKHR(XrInstance instance, const struct timespec *timespecTime, XrTime *time);

//! OpenXR API function @ep{xrConvertTimeToTimespecTimeKHR}
XRAPI_ATTR XrResult XRAPI_CALL
oxr_xrConvertTimeToTimespecTimeKHR(XrInstance instance, XrTime time, struct timespec *timespecTime);
*/


/*
 *
 * oxr_api_system.c
 *
 */

//! OpenXR API function @ep{xrGetSystem}
XRAPI_ATTR XrResult XRAPI_CALL
xrGetSystem(XrInstance instance, const XrSystemGetInfo *getInfo, XrSystemId *systemId);

//! OpenXR API function @ep{xrGetSystemProperties}
XRAPI_ATTR XrResult XRAPI_CALL
xrGetSystemProperties(XrInstance instance, XrSystemId systemId, XrSystemProperties *properties);

//! OpenXR API function @ep{xrEnumerateViewConfigurations}
XRAPI_ATTR XrResult XRAPI_CALL
xrEnumerateViewConfigurations(XrInstance instance,
                                  XrSystemId systemId,
                                  uint32_t viewConfigurationTypeCapacityInput,
                                  uint32_t *viewConfigurationTypeCountOutput,
                                  XrViewConfigurationType *viewConfigurationTypes);

//! OpenXR API function @ep{xrGetViewConfigurationProperties}
XRAPI_ATTR XrResult XRAPI_CALL
xrGetViewConfigurationProperties(XrInstance instance,
                                     XrSystemId systemId,
                                     XrViewConfigurationType viewConfigurationType,
                                     XrViewConfigurationProperties *configurationProperties);

//! OpenXR API function @ep{xrEnumerateViewConfigurationViews}
XRAPI_ATTR XrResult XRAPI_CALL
xrEnumerateViewConfigurationViews(XrInstance instance,
                                      XrSystemId systemId,
                                      XrViewConfigurationType viewConfigurationType,
                                      uint32_t viewCapacityInput,
                                      uint32_t *viewCountOutput,
                                      XrViewConfigurationView *views);

//! OpenXR API function @ep{xrEnumerateEnvironmentBlendModes}
XRAPI_ATTR XrResult XRAPI_CALL
xrEnumerateEnvironmentBlendModes(XrInstance instance,
                                     XrSystemId systemId,
                                     XrViewConfigurationType viewConfigurationType,
                                     uint32_t environmentBlendModeCapacityInput,
                                     uint32_t *environmentBlendModeCountOutput,
                                     XrEnvironmentBlendMode *environmentBlendModes);


//! OpenXR API function @ep{xrGetOpenGLESGraphicsRequirementsKHR}
XRAPI_ATTR XrResult XRAPI_CALL
xrGetOpenGLESGraphicsRequirementsKHR(XrInstance instance,
                                         XrSystemId systemId,
                                         XrGraphicsRequirementsOpenGLESKHR *graphicsRequirements);



/*
 *
 * oxr_api_session.c
 *
 */

//! OpenXR API function @ep{xrCreateSession}
XRAPI_ATTR XrResult XRAPI_CALL
xrCreateSession(XrInstance instance, const XrSessionCreateInfo *createInfo, XrSession *session);

//! OpenXR API function @ep{xrDestroySession}
XRAPI_ATTR XrResult XRAPI_CALL
xrDestroySession(XrSession session);

//! OpenXR API function @ep{xrBeginSession}
XRAPI_ATTR XrResult XRAPI_CALL
xrBeginSession(XrSession session, const XrSessionBeginInfo *beginInfo);

//! OpenXR API function @ep{xrEndSession}
XRAPI_ATTR XrResult XRAPI_CALL
xrEndSession(XrSession session);

//! OpenXR API function @ep{xrWaitFrame}
XRAPI_ATTR XrResult XRAPI_CALL
xrWaitFrame(XrSession session, const XrFrameWaitInfo *frameWaitInfo, XrFrameState *frameState);

//! OpenXR API function @ep{xrBeginFrame}
XRAPI_ATTR XrResult XRAPI_CALL
xrBeginFrame(XrSession session, const XrFrameBeginInfo *frameBeginInfo);

//! OpenXR API function @ep{xrEndFrame}
XRAPI_ATTR XrResult XRAPI_CALL
xrEndFrame(XrSession session, const XrFrameEndInfo *frameEndInfo);

//! OpenXR API function @ep{xrRequestExitSession}
XRAPI_ATTR XrResult XRAPI_CALL
xrRequestExitSession(XrSession session);

//! OpenXR API function @ep{xrLocateViews}
XRAPI_ATTR XrResult XRAPI_CALL
xrLocateViews(XrSession session,
                  const XrViewLocateInfo *viewLocateInfo,
                  XrViewState *viewState,
                  uint32_t viewCapacityInput,
                  uint32_t *viewCountOutput,
                  XrView *views);

#ifdef OXR_HAVE_KHR_visibility_mask
//! OpenXR API function @ep{xrGetVisibilityMaskKHR}
XRAPI_ATTR XrResult XRAPI_CALL
oxr_xrGetVisibilityMaskKHR(XrSession session,
                           XrViewConfigurationType viewConfigurationType,
                           uint32_t viewIndex,
                           XrVisibilityMaskTypeKHR visibilityMaskType,
                           XrVisibilityMaskKHR *visibilityMask);
#endif // OXR_HAVE_KHR_visibility_mask

#ifdef OXR_HAVE_KHR_android_thread_settings
//! OpenXR API function @ep{xrSetAndroidApplicationThreadKHR}
XRAPI_ATTR XrResult XRAPI_CALL
oxr_xrSetAndroidApplicationThreadKHR(XrSession session, XrAndroidThreadTypeKHR threadType, uint32_t threadId);
#endif // OXR_HAVE_KHR_android_thread_settings

#ifdef OXR_HAVE_EXT_performance_settings
//! OpenXR API function @ep{xrPerfSettingsSetPerformanceLevelEXT}
XRAPI_ATTR XrResult XRAPI_CALL
oxr_xrPerfSettingsSetPerformanceLevelEXT(XrSession session,
                                         XrPerfSettingsDomainEXT domain,
                                         XrPerfSettingsLevelEXT level);
#endif // OXR_HAVE_EXT_performance_settings

#ifdef OXR_HAVE_EXT_thermal_query
//! OpenXR API function @ep{xrThermalGetTemperatureTrendEXT}
XRAPI_ATTR XrResult XRAPI_CALL
oxr_xrThermalGetTemperatureTrendEXT(XrSession session,
                                    XrPerfSettingsDomainEXT domain,
                                    XrPerfSettingsNotificationLevelEXT *notificationLevel,
                                    float *tempHeadroom,
                                    float *tempSlope);
#endif // OXR_HAVE_EXT_thermal_query


/*
 *
 * oxr_api_space.c
 *
 */

//! OpenXR API function @ep{xrEnumerateReferenceSpaces}
XRAPI_ATTR XrResult XRAPI_CALL
xrEnumerateReferenceSpaces(XrSession session, //PASS TODO : maybe change reference space to stage?
                               uint32_t spaceCapacityInput,
                               uint32_t *spaceCountOutput,
                               XrReferenceSpaceType *spaces);

//! OpenXR API function @ep{xrGetReferenceSpaceBoundsRect}
XRAPI_ATTR XrResult XRAPI_CALL // PASS TODO: update bounds?
xrGetReferenceSpaceBoundsRect(XrSession session, XrReferenceSpaceType referenceSpaceType, XrExtent2Df *bounds);

//! OpenXR API function @ep{xrCreateReferenceSpace}
XRAPI_ATTR XrResult XRAPI_CALL // PASS TODO: Set origin
xrCreateReferenceSpace(XrSession session, const XrReferenceSpaceCreateInfo *createInfo, XrSpace *space);

//! OpenXR API function @ep{xrLocateSpace}
XRAPI_ATTR XrResult XRAPI_CALL // PASS TODO : see usages
xrLocateSpace(XrSpace space, XrSpace baseSpace, XrTime time, XrSpaceLocation *location);

//! OpenXR API function @ep{xrDestroySpace}
XRAPI_ATTR XrResult XRAPI_CALL // PASS
xrDestroySpace(XrSpace space);

//! OpenXR API function @ep{xrCreateActionSpace}
XRAPI_ATTR XrResult XRAPI_CALL // PASS TODO : Set Origin?
xrCreateActionSpace(XrSession session, const XrActionSpaceCreateInfo *createInfo, XrSpace *space);


/*
 *
 * oxr_api_swapchain.c
 *
 */

//! OpenXR API function @ep{xrEnumerateSwapchainFormats} //PASS TODO CHECK IF THIS CAUSES PROBLEMS
XRAPI_ATTR XrResult XRAPI_CALL
xrEnumerateSwapchainFormats(XrSession session,
                                uint32_t formatCapacityInput,
                                uint32_t *formatCountOutput,
                                int64_t *formats);

//! OpenXR API function @ep{xrCreateSwapchain}
XRAPI_ATTR XrResult XRAPI_CALL
xrCreateSwapchain(XrSession session, const XrSwapchainCreateInfo *createInfo, XrSwapchain *swapchain);

//! OpenXR API function @ep{xrDestroySwapchain}
XRAPI_ATTR XrResult XRAPI_CALL
xrDestroySwapchain(XrSwapchain swapchain);

//! OpenXR API function @ep{xrEnumerateSwapchainImages}
XRAPI_ATTR XrResult XRAPI_CALL
xrEnumerateSwapchainImages(XrSwapchain swapchain,
                               uint32_t imageCapacityInput,
                               uint32_t *imageCountOutput,
                               XrSwapchainImageBaseHeader *images);

//! OpenXR API function @ep{xrAcquireSwapchainImage}
XRAPI_ATTR XrResult XRAPI_CALL
xrAcquireSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageAcquireInfo *acquireInfo, uint32_t *index);

//! OpenXR API function @ep{xrWaitSwapchainImage}
XRAPI_ATTR XrResult XRAPI_CALL
xrWaitSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageWaitInfo *waitInfo);

//! OpenXR API function @ep{xrReleaseSwapchainImage}
XRAPI_ATTR XrResult XRAPI_CALL
xrReleaseSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageReleaseInfo *releaseInfo);



/*
 *
 * oxr_api_debug.c
 *
 */

//! OpenXR API function @ep{xrSetDebugUtilsObjectNameEXT}
XRAPI_ATTR XrResult XRAPI_CALL
xrSetDebugUtilsObjectNameEXT(XrInstance instance, const XrDebugUtilsObjectNameInfoEXT *nameInfo);

//! OpenXR API function @ep{xrCreateDebugUtilsMessengerEXT}
XRAPI_ATTR XrResult XRAPI_CALL
xrCreateDebugUtilsMessengerEXT(XrInstance instance,
                                   const XrDebugUtilsMessengerCreateInfoEXT *createInfo,
                                   XrDebugUtilsMessengerEXT *messenger);

//! OpenXR API function @ep{xrDestroyDebugUtilsMessengerEXT}
XRAPI_ATTR XrResult XRAPI_CALL
xrDestroyDebugUtilsMessengerEXT(XrDebugUtilsMessengerEXT messenger);

//! OpenXR API function @ep{xrSubmitDebugUtilsMessageEXT}
XRAPI_ATTR XrResult XRAPI_CALL
xrSubmitDebugUtilsMessageEXT(XrInstance instance,
                                 XrDebugUtilsMessageSeverityFlagsEXT messageSeverity,
                                 XrDebugUtilsMessageTypeFlagsEXT messageTypes,
                                 const XrDebugUtilsMessengerCallbackDataEXT *callbackData);

//! OpenXR API function @ep{xrSessionBeginDebugUtilsLabelRegionEXT}
XRAPI_ATTR XrResult XRAPI_CALL
xrSessionBeginDebugUtilsLabelRegionEXT(XrSession session, const XrDebugUtilsLabelEXT *labelInfo);

//! OpenXR API function @ep{xrSessionEndDebugUtilsLabelRegionEXT}
XRAPI_ATTR XrResult XRAPI_CALL
xrSessionEndDebugUtilsLabelRegionEXT(XrSession session);

//! OpenXR API function @ep{xrSessionInsertDebugUtilsLabelEXT}
XRAPI_ATTR XrResult XRAPI_CALL
xrSessionInsertDebugUtilsLabelEXT(XrSession session, const XrDebugUtilsLabelEXT *labelInfo);



/*
 *
 * oxr_api_action.c
 *
 */

//PASS ALL TODO EMULATE ACTIONSETS FOR CONTROLLERS

//! OpenXR API function @ep{xrCreateActionSet}
XRAPI_ATTR XrResult XRAPI_CALL
xrCreateActionSet(XrInstance instance, const XrActionSetCreateInfo *createInfo, XrActionSet *actionSet);

//! OpenXR API function @ep{xrDestroyActionSet}
XRAPI_ATTR XrResult XRAPI_CALL
xrDestroyActionSet(XrActionSet actionSet);

//! OpenXR API function @ep{xrCreateAction}
XRAPI_ATTR XrResult XRAPI_CALL
xrCreateAction(XrActionSet actionSet, const XrActionCreateInfo *createInfo, XrAction *action);

//! OpenXR API function @ep{xrDestroyAction}
XRAPI_ATTR XrResult XRAPI_CALL
xrDestroyAction(XrAction action);

//! OpenXR API function @ep{xrSuggestInteractionProfileBindings}
XRAPI_ATTR XrResult XRAPI_CALL
xrSuggestInteractionProfileBindings(XrInstance instance,
                                        const XrInteractionProfileSuggestedBinding *suggestedBindings);

//! OpenXR API function @ep{xrAttachSessionActionSets}
XRAPI_ATTR XrResult XRAPI_CALL
xrAttachSessionActionSets(XrSession session, const XrSessionActionSetsAttachInfo *bindInfo);

//! OpenXR API function @ep{xrGetCurrentInteractionProfile}
XRAPI_ATTR XrResult XRAPI_CALL
xrGetCurrentInteractionProfile(XrSession session,
                                   XrPath topLevelUserPath,
                                   XrInteractionProfileState *interactionProfile);

//! OpenXR API function @ep{xrGetActionStateBoolean}
XRAPI_ATTR XrResult XRAPI_CALL
xrGetActionStateBoolean(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStateBoolean *data);

//! OpenXR API function @ep{xrGetActionStateFloat}
XRAPI_ATTR XrResult XRAPI_CALL
xrGetActionStateFloat(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStateFloat *data);

//! OpenXR API function @ep{xrGetActionStateVector2f}
XRAPI_ATTR XrResult XRAPI_CALL
xrGetActionStateVector2f(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStateVector2f *data);

//! OpenXR API function @ep{xrGetActionStatePose}
XRAPI_ATTR XrResult XRAPI_CALL
xrGetActionStatePose(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStatePose *data);

//! OpenXR API function @ep{xrSyncActions}
XRAPI_ATTR XrResult XRAPI_CALL
xrSyncActions(XrSession session, const XrActionsSyncInfo *syncInfo);

//! OpenXR API function @ep{xrEnumerateBoundSourcesForAction}
XRAPI_ATTR XrResult XRAPI_CALL
xrEnumerateBoundSourcesForAction(XrSession session,
                                     const XrBoundSourcesForActionEnumerateInfo *enumerateInfo,
                                     uint32_t sourceCapacityInput,
                                     uint32_t *sourceCountOutput,
                                     XrPath *sources);

//! OpenXR API function @ep{xrGetInputSourceLocalizedName}
XRAPI_ATTR XrResult XRAPI_CALL
xrGetInputSourceLocalizedName(XrSession session,
                                  const XrInputSourceLocalizedNameGetInfo *getInfo,
                                  uint32_t bufferCapacityInput,
                                  uint32_t *bufferCountOutput,
                                  char *buffer);

//! OpenXR API function @ep{xrApplyHapticFeedback}
XRAPI_ATTR XrResult XRAPI_CALL
xrApplyHapticFeedback(XrSession session,
                          const XrHapticActionInfo *hapticActionInfo,
                          const XrHapticBaseHeader *hapticEvent);

//! OpenXR API function @ep{xrStopHapticFeedback}
XRAPI_ATTR XrResult XRAPI_CALL
xrStopHapticFeedback(XrSession session, const XrHapticActionInfo *hapticActionInfo);

//! OpenXR API function @ep{xrCreateHandTrackerEXT}
XRAPI_ATTR XrResult XRAPI_CALL
xrCreateHandTrackerEXT(XrSession session,
                           const XrHandTrackerCreateInfoEXT *createInfo,
                           XrHandTrackerEXT *handTracker);

//! OpenXR API function @ep{xrDestroyHandTrackerEXT}
XRAPI_ATTR XrResult XRAPI_CALL
xrDestroyHandTrackerEXT(XrHandTrackerEXT handTracker);

//! OpenXR API function @ep{xrLocateHandJointsEXT}
XRAPI_ATTR XrResult XRAPI_CALL
xrLocateHandJointsEXT(XrHandTrackerEXT handTracker,
                          const XrHandJointsLocateInfoEXT *locateInfo,
                          XrHandJointLocationsEXT *locations);

//! OpenXR API function @ep{xrApplyForceFeedbackCurlMNDX}
XRAPI_ATTR XrResult XRAPI_CALL
xrApplyForceFeedbackCurlMNDX(XrHandTrackerEXT handTracker, const XrForceFeedbackCurlApplyLocationsMNDX *locations);


//! OpenXR API function @ep{xrEnumerateDisplayRefreshRatesFB}
XRAPI_ATTR XrResult XRAPI_CALL
xrEnumerateDisplayRefreshRatesFB(XrSession session,
                                     uint32_t displayRefreshRateCapacityInput,
                                     uint32_t *displayRefreshRateCountOutput,
                                     float *displayRefreshRates);

//! OpenXR API function @ep{xrGetDisplayRefreshRateFB}
XRAPI_ATTR XrResult XRAPI_CALL
xrGetDisplayRefreshRateFB(XrSession session, float *displayRefreshRate);

//! OpenXR API function @ep{xrRequestDisplayRefreshRateFB}
XRAPI_ATTR XrResult XRAPI_CALL
xrRequestDisplayRefreshRateFB(XrSession session, float displayRefreshRate);



}
