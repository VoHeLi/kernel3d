#include "oxr_api_funcs.h"
#include "openxr/openxr_platform.h"
#include "openxr/openxr_platform_defines.h"
#include "openxr/openxr_reflection.h"

#include <android/log.h>
#include <string>

#include "android_globals.h"
#include "oxr_extension_support.h"
#include "oxr_api_verify.h"
#include "oxr_chain.h"
#include "oxr_objects.h"

#include "mirage_binder.h"

#include <cstdlib>
#include <chrono>
#include <ctime>

#define ARRAY_SIZE(array) sizeof(array)/sizeof(array[0])

//! OpenXR API function @ep{xrInitializeLoaderKHR}
 XRAPI_ATTR XrResult XRAPI_CALL
xrInitializeLoaderKHR(const XrLoaderInitInfoBaseHeaderKHR *loaderInitInfo){
    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "xrInitializeLoaderKHR called!");

    const XrLoaderInitInfoAndroidKHR *initInfoAndroid = (XrLoaderInitInfoAndroidKHR*)loaderInitInfo;

    if (initInfoAndroid == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", "Did not find XrLoaderInitInfoAndroidKHR");

    }
    if (initInfoAndroid->applicationVM == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", "applicationVM must be populated");

    }
    if (initInfoAndroid->applicationContext == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", "applicationContext must be populated");

    }

    android_globals_store_vm_and_context((_JavaVM*)initInfoAndroid->applicationVM, initInfoAndroid->applicationContext);

    //LINK TO MIRAGE SERVICE TODO


    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "xrInitializeLoaderKHR success!");
    return XR_SUCCESS;

}
#define XR_BD_controller_interaction_SPEC_VERSION 1
#define MAKE_EXTENSION_PROPERTIES(mixed_case, all_caps)                                                                \
	{XR_TYPE_EXTENSION_PROPERTIES, NULL, XR_##all_caps##_EXTENSION_NAME, XR_##mixed_case##_SPEC_VERSION},

static const XrExtensionProperties extension_properties[] = {OXR_EXTENSION_SUPPORT_GENERATE(MAKE_EXTENSION_PROPERTIES)};



//! OpenXR API function @ep{xrEnumerateInstanceExtensionProperties}
XRAPI_ATTR XrResult XRAPI_CALL
xrEnumerateInstanceExtensionProperties(const char *layerName,
                                       uint32_t propertyCapacityInput,
                                       uint32_t *propertyCountOutput,
                                       XrExtensionProperties *properties){
    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "xrEnumerateInstanceExtensionProperties called!");

    if (propertyCountOutput == __null) {
        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", "propertyCountOutput null");
        return XR_ERROR_VALIDATION_FAILURE;
    }
    *propertyCountOutput = ARRAY_SIZE(extension_properties);
    if (propertyCapacityInput == 0) { return XR_SUCCESS; }
    if (propertyCapacityInput < ARRAY_SIZE(extension_properties)) {
        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", "propertyCapacityInput insufficiant : %u", propertyCapacityInput);
        return XR_ERROR_SIZE_INSUFFICIENT;
    }
    for (uint32_t i = 0; i < ARRAY_SIZE(extension_properties); i++) {
        (properties)[i] = (extension_properties)[i];
    }

    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "xrEnumerateInstanceExtensionProperties success!");

    return XR_SUCCESS;

}

static XrResult
oxr_check_android_extensions(const XrInstanceCreateInfo *createInfo, const struct oxr_extension_status *extensions)
{
    // We need the XR_KHR_android_create_instance extension.
    if (!extensions->KHR_android_create_instance) {

        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", "xrCreateInstance : (createInfo->enabledExtensionNames) Mandatory platform-specific extension XR_KHR_android_create_instance not specified");

        return XR_ERROR_INITIALIZATION_FAILED;
    }

    {
        // Verify that it exists and is populated.
        XrInstanceCreateInfoAndroidKHR const *createInfoAndroid = OXR_GET_INPUT_FROM_CHAIN(
                createInfo, XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR, XrInstanceCreateInfoAndroidKHR);
        if (createInfoAndroid == NULL) {
            __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", "xrCreateInstance : (createInfo->next...) Did not find XrInstanceCreateInfoAndroidKHR in chain");

            return XR_ERROR_VALIDATION_FAILURE;
        }
        if (createInfoAndroid->applicationVM == NULL) {
            __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", "xrCreateInstance : (createInfo->next...->applicationVM) applicationVM must be populated");
            return XR_ERROR_VALIDATION_FAILURE;
        }
        if (createInfoAndroid->applicationActivity == NULL) {
            __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", "xrCreateInstance : (createInfo->next...->applicationActivity) applicationActivity must be populated");
            return XR_ERROR_VALIDATION_FAILURE;
        }
    }
    return XR_SUCCESS;
}

extern "C"
//! OpenXR API function @ep{xrCreateInstance}
XRAPI_ATTR XrResult XRAPI_CALL
xrCreateInstance(const XrInstanceCreateInfo *createInfo, XrInstance *out_instance){


    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "xrCreateInstance called!");



    XrResult ret;

    if(createInfo == nullptr){
        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", "xrCreateInstance : createInfo nullptr error!");
        return XR_ERROR_VALIDATION_FAILURE;
    }

    if(createInfo->type != XR_TYPE_INSTANCE_CREATE_INFO){
        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", "xrCreateInstance : createInfo nullptr error!");
        return XR_ERROR_VALIDATION_FAILURE;
    }

    const uint32_t major = XR_VERSION_MAJOR(XR_CURRENT_API_VERSION);
    const uint32_t minor = XR_VERSION_MINOR(XR_CURRENT_API_VERSION);
    const uint32_t patch = XR_VERSION_PATCH(XR_CURRENT_API_VERSION);
    (void)patch; // Not used for now.


    if (createInfo->applicationInfo.apiVersion < XR_MAKE_VERSION(major, minor, 0)) {
        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", "xrCreateInstance : Cannot satisfy request for version less than %d.%d.%d", major, minor, 0);

        return XR_ERROR_API_VERSION_UNSUPPORTED;
    }

    /*
     * This is a slight fib, to let us approximately run things between 1.0
     * and 2.0
     */
    if (createInfo->applicationInfo.apiVersion >= XR_MAKE_VERSION(2, 0, 0)) {
        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", "xrCreateInstance : Cannot satisfy request for version: too high");
        return XR_ERROR_API_VERSION_UNSUPPORTED;
    }

    // To be passed into verify and instance creation.
    struct oxr_extension_status extensions;
    std::memset(&extensions, 0, sizeof(extensions));

    /*
     * Check that all extension names are recognized, so oxr_instance_create
     * doesn't need to check for bad extension names.
     *
     * Also fill out the oxr_extension_status struct at the same time.
     */
#define CHECK_EXT_NAME(mixed_case, all_caps)                                                                           \
	if (strcmp(createInfo->enabledExtensionNames[i], XR_##all_caps##_EXTENSION_NAME) == 0) {                       \
		extensions.mixed_case = true;                                                                          \
		continue;                                                                                              \
	}
    for (uint32_t i = 0; i < createInfo->enabledExtensionCount; ++i) {
        OXR_EXTENSION_SUPPORT_GENERATE(CHECK_EXT_NAME)

        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", "xrCreateInstance : (createInfo->enabledExtensionNames[%d]) Unrecognized extension name '%s'", i,
                            createInfo->enabledExtensionNames[i]);

        return XR_ERROR_EXTENSION_NOT_PRESENT;
    }

    ret = oxr_verify_extensions(&extensions);
    if (ret != XR_SUCCESS) {
        return ret;
    }


    ret = oxr_check_android_extensions(createInfo, &extensions);
	if (ret != XR_SUCCESS) {
		return ret;
	}

    oxr_instance *inst = NULL;

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto currentTimeMicros = std::chrono::duration_cast<std::chrono::microseconds>(currentTime.time_since_epoch()).count();
    srand(static_cast<unsigned int>(currentTimeMicros));
    int randomNumber = rand();

    inst = new oxr_instance(randomNumber);

    *out_instance = ((XrInstance)(uint64_t)(intptr_t)(inst));



    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "xrCreateInstance calling mirage!");

    return initializeMirageAppInstance(android_globals_get_vm(), android_globals_get_context());

}

XRAPI_ATTR XrResult XRAPI_CALL
xrDestroyInstance(XrInstance instance)
{
    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "xrDestroyInstance called!");

    if(instance != nullptr){
        free(instance);
    }

    return destroyMirageInstance();
}

XRAPI_ATTR XrResult XRAPI_CALL
xrGetInstanceProperties(XrInstance instance, XrInstanceProperties *instanceProperties)
{
    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "xrGetInstanceProperties called!");

    if(instance == nullptr){
        return XR_ERROR_INSTANCE_LOST;
    }

    strcpy(instanceProperties->runtimeName, RUNTIME_NAME);
    instanceProperties->runtimeVersion = RUNTIME_VERSION;

    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "xrGetInstanceProperties success!");

    return XR_SUCCESS;
}

XRAPI_ATTR XrResult XRAPI_CALL
xrPollEvent(XrInstance instance, XrEventDataBuffer *eventData)
{
    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "xrPollEvent called!");

    if(instance == nullptr){
        return XR_ERROR_INSTANCE_LOST;
    }
    if(eventData == nullptr){
        return XR_ERROR_HANDLE_INVALID;
    }

    return pollMirageEvents(eventData); //TODO : PollEvents()
}

XRAPI_ATTR XrResult XRAPI_CALL
xrResultToString(XrInstance instance, XrResult value, char buffer[XR_MAX_RESULT_STRING_SIZE])
{
    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "xrResultToString called!");

    if(instance == nullptr){
        return XR_ERROR_INSTANCE_LOST;
    }

#define MAKE_RESULT_CASE(VAL, _)                                                                                       \
	case VAL: snprintf(buffer, XR_MAX_RESULT_STRING_SIZE, #VAL); break;

    switch (value) {
        XR_LIST_ENUM_XrResult(MAKE_RESULT_CASE);
        default:
            snprintf(buffer, XR_MAX_RESULT_STRING_SIZE, "XR_UNKNOWN_%s_%d", value < 0 ? "FAILURE" : "SUCCESS",
                     value);
    }

    return XR_SUCCESS;
}

XRAPI_ATTR XrResult XRAPI_CALL
xrStructureTypeToString(XrInstance instance, XrStructureType value, char buffer[XR_MAX_STRUCTURE_NAME_SIZE])
{
    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "xrStructureTypeToString called!");

    if(instance == nullptr){
        return XR_ERROR_INSTANCE_LOST;
    }

#define MAKE_TYPE_CASE(VAL, _)                                                                                         \
	case VAL: snprintf(buffer, XR_MAX_RESULT_STRING_SIZE, #VAL); break;

    switch (value) {
        XR_LIST_ENUM_XrStructureType(MAKE_TYPE_CASE);
        default: snprintf(buffer, XR_MAX_RESULT_STRING_SIZE, "XR_UNKNOWN_STRUCTURE_TYPE_%d", value);
    }

    return XR_SUCCESS;
}

XRAPI_ATTR XrResult XRAPI_CALL //PASS
xrStringToPath(XrInstance instance, const char *pathString, XrPath *out_path)
{
    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "xrStringToPath called!");

    __android_log_print(ANDROID_LOG_DEBUG, "PICOCO", "xrStringToPath : %s", pathString);

    /*if(instance == nullptr){
        return XR_ERROR_INSTANCE_LOST;
    }

    XrResult ret;
    oxr_path path;

    ret = oxr_verify_full_path_c(pathString, "pathString");
    if (ret != XR_SUCCESS) {
        return ret;
    }

    ret = oxr_path_get_or_create(instance, pathString, strlen(pathString), &path);
    if (ret != XR_SUCCESS) {
        return ret;
    }

    *out_path = (XrPath)&path;

    return XR_SUCCESS;*/

    XrResult result = mirageStringToPath(pathString, out_path);
    if(result != XR_SUCCESS) return result;

//    TryRegisterControllerPath(*out_path, pathString);

    return result;
}

XRAPI_ATTR XrResult XRAPI_CALL //PASS
xrPathToString(
        XrInstance instance, XrPath path, uint32_t bufferCapacityInput, uint32_t *bufferCountOutput, char *buffer)
{
    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR", "xrPathToString : xrPathToString called!");

    /*const char *str;
    size_t length;
    XrResult ret;

    if(instance == nullptr){
        return XR_ERROR_INSTANCE_LOST;
    }

    if (path == XR_NULL_PATH) {
        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR", " xrPathToString :(path == XR_NULL_PATH");
        return XR_ERROR_PATH_INVALID;
    }

    return oxr_path_get_string(instance, (oxr_path*)path, &str, &length);

    if (ret != XR_SUCCESS) {
        return ret;
    }

    // Length is the number of valid characters, not including the
    // null termination character (but a extra null byte is always
    // reserved).
    OXR_TWO_CALL_HELPER(&log, bufferCapacityInput, bufferCountOutput, buffer, length + 1, str, XR_SUCCESS);*/

    return miragePathToString(path, bufferCapacityInput, bufferCountOutput, buffer);
}
