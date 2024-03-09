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
#include <sys/socket.h>
#include <linux/un.h>
#include <new>
#include "mirage_shared/XrInstanceDescriptor.h"
#include "mirage_shared/XrReferenceSpaceDescriptor.h"
#include "mirage_shared/common_types.h"

#define SOCKET_PATH "\0mirage_service_listener"

int getFd(const char* filename) {
    struct sockaddr_un server_addr;
    long long fd;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Getting FD from binder...");

    // Création du socket
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "Client Socket error");
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "ERRNO : %d", errno);
        exit(EXIT_FAILURE);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Socket created!");

    // Configuration de l'adresse du serveur
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Connexion au serveur
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "Connect error");
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "ERRNO : %d", errno);
        exit(EXIT_FAILURE);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Connected!");

    // Réception du message

    struct msghdr msg;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    struct iovec iov;
    int data;

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    iov.iov_base = &data;
    iov.iov_len = sizeof(data);

    union {
        char   buf[CMSG_SPACE(sizeof(int))];
        /* Space large enough to hold an 'int' */
        struct cmsghdr align;
    } controlMsg;

    msg.msg_control = controlMsg.buf;
    msg.msg_controllen = sizeof(controlMsg.buf);

    if (recvmsg(client_fd, &msg, 0) == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "RECVMSG error");
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "ERRNO : %d", errno);
        exit(EXIT_FAILURE);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MSG Received : %d", data);


    // Récupérer le descripteur de fichier
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    if (cmsg == NULL || cmsg->cmsg_type != SCM_RIGHTS) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "Invalid CMSG error");
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "ERRNO : %d", errno);
        exit(EXIT_FAILURE);
    }
    memcpy(&fd, CMSG_DATA(cmsg), sizeof(fd));

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Got FD : %d", fd);

    char* lol = (char*) malloc(256);
    recv(client_fd, lol, 256, 0);

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Drained 256 extra bytes of recv queue");

    free(lol);

    // Fermeture du descripteur de fichier
    //close(client_fd);
    //close(fd);

    return fd;
}

//SYSTEM

XrResult initializeMirageAppInstance(void* vm, void* clazz, const XrInstanceCreateInfo *createInfo, XrInstance *out_instance){



    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "InitializeMirageAppInstance");

    int fd = getFd(SOCKET_PATH);

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Got fd : %d", fd);

    size_t memSize = ASharedMemory_getSize(fd);
    char* sharedMemoryBuffer = (char *) mmap(NULL, sizeof(shared_memory_descriptor), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    sharedMemoryDescriptor = new(sharedMemoryBuffer) shared_memory_descriptor(); //(shared_memory_descriptor*) sharedMemoryPtr;

    //Store XRInstance in shared memory
    XrInstanceDescriptor* instanceDescriptor = NEW_SHARED(XrInstanceDescriptor, sharedMemoryDescriptor, vm, clazz, createInfo);
    sharedMemoryDescriptor->set_instance_ptr((void*)instanceDescriptor);
    sharedMemoryDescriptor->memory_init_client();

    *out_instance = (XrInstance)instanceDescriptor;

    cts_instruction instruction = cts_instruction::POPULATE_SYSTEM_PROPERTIES;
    send(client_fd, &instruction, sizeof(cts_instruction), 0);
    cts_instruction response = cts_instruction::NONE;

    recv(client_fd, &response, sizeof(cts_instruction), 0);
    if(response != cts_instruction::POPULATE_SYSTEM_PROPERTIES){
        return XR_ERROR_RUNTIME_FAILURE;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "InitializeMirageAppInstanceEnd");

    return XR_SUCCESS;
    //TODO LATER close(fd);
}

XrResult destroyMirageInstance(){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

bool firstCallMirageEvents = true;
XrResult pollMirageEvents(XrEventDataBuffer *eventData){


    //__android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "PollMirageEvents called!");


    //TODO : IMPLEMENT LATER, WE DON'T SEND EVENTS EXCEPT FOR SESSION READY, ONLY SUPPORT 1 SESSION FOR NOW
    if(firstCallMirageEvents) {
        firstCallMirageEvents = false;

        XrEventDataSessionStateChanged* sessionStateChanged = (XrEventDataSessionStateChanged*)eventData;
        sessionStateChanged->type = XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED;
        sessionStateChanged->next = nullptr;
        sessionStateChanged->session = (XrSession)(((XrInstanceDescriptor*)sharedMemoryDescriptor->get_instance_ptr())->firstSessionDescriptor);
        sessionStateChanged->state = XR_SESSION_STATE_READY;
        sessionStateChanged->time = 0;

        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "PollMirageEvents session ready called!");

        return XR_SUCCESS;
    }

    return XR_EVENT_UNAVAILABLE;

    }

XrResult miragePathToString(XrPath path, uint32_t bufferCapacityInput, uint32_t *bufferCountOutput, char *buffer){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MiragePathToString called!");

    XrPathDescriptor* pathDescriptor = (XrPathDescriptor*) ((XrInstanceDescriptor*)sharedMemoryDescriptor->get_instance_ptr())->firstPathDescriptor;
    if(pathDescriptor == nullptr){
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "Path not found!");
        return XR_ERROR_VALIDATION_FAILURE;
    }

    uint64_t n = (uint64_t)path;
    for(int i = 0; i < n; i++){
        if(pathDescriptor == nullptr){
            __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "Path not found!");
            return XR_ERROR_VALIDATION_FAILURE;
        }

        pathDescriptor = pathDescriptor->nextPathDescriptor;
    }

    if(pathDescriptor == nullptr){
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "Path not found!");
        return XR_ERROR_VALIDATION_FAILURE;
    }

    if(strlen(pathDescriptor->pathString) > bufferCapacityInput){
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "Buffer too small!");
        return XR_ERROR_VALIDATION_FAILURE;
    }

    strcpy(buffer, pathDescriptor->pathString);

    return XR_SUCCESS;
}

XrResult mirageStringToPath(const char *pathString, XrPath *out_path){
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Searching for path : %s", pathString);


    XrPathDescriptor* pathDescriptor = (XrPathDescriptor*) ((XrInstanceDescriptor*)sharedMemoryDescriptor->get_instance_ptr())->firstPathDescriptor;

    if(pathDescriptor == nullptr){
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "No paths found");

        ((XrInstanceDescriptor*)sharedMemoryDescriptor->get_instance_ptr())->firstPathDescriptor = NEW_SHARED(XrPathDescriptor, sharedMemoryDescriptor, pathString, nullptr);

        *out_path = (XrPath)0;

        return XR_SUCCESS;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Paths found");

    uint64_t counter = 0;

    XrPathDescriptor* lastPathDescriptor = pathDescriptor;
    //TODO : Add a limit to the number of paths
    //Check the existing paths
    while(pathDescriptor != nullptr){
        if(strcmp(pathDescriptor->pathString, pathString) == 0){
            *out_path = (XrPath)counter;
            return XR_SUCCESS;
        }
        lastPathDescriptor = pathDescriptor;
        pathDescriptor = pathDescriptor->nextPathDescriptor;
        counter++;
    }


    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Path not found, adding it!");


    lastPathDescriptor->nextPathDescriptor = NEW_SHARED(XrPathDescriptor, sharedMemoryDescriptor, pathString, lastPathDescriptor);

    *out_path = (XrPath)counter;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Path added!");

    return XR_SUCCESS;
}


XrResult getMirageSystem(const XrSystemGetInfo* systemGetInfo, XrSystemId* systemId){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Get Mirage System!");

    if(systemGetInfo == nullptr || systemGetInfo->formFactor != XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY){
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "systemGetInfo is null or form factor is not HMD!");
        return XR_ERROR_VALIDATION_FAILURE;
    }

    XrSystemIdDescriptor* systemIdDescriptor = (XrSystemIdDescriptor*) ((XrInstanceDescriptor*)sharedMemoryDescriptor->get_instance_ptr())->systemIdDescriptor;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Got resolution %u x %u", systemIdDescriptor->systemProperties->graphicsProperties.maxSwapchainImageWidth, systemIdDescriptor->systemProperties->graphicsProperties.maxSwapchainImageHeight);

    //__android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Not crashed yet + %p", systemIdDescriptor);
    *systemId = (XrSystemId)systemIdDescriptor;

    return XR_SUCCESS;
}

XrResult getMirageSystemProperties(XrSystemId systemId, XrSystemProperties *properties){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageEnumerateViewConfigurations( XrSystemId systemId, uint32_t viewConfigurationTypeCapacityInput, uint32_t *viewConfigurationTypeCountOutput, XrViewConfigurationType *viewConfigurationTypes){


    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateViewConfigurations called!");

    XrSystemIdDescriptor* systemIdDescriptor = (XrSystemIdDescriptor*) systemId;

    if(viewConfigurationTypeCapacityInput == 0 || viewConfigurationTypes == nullptr){
        *viewConfigurationTypeCountOutput = systemIdDescriptor->viewConfigurationsCount;
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "ViewConfig enum : Giving size!");
        return XR_SUCCESS;
    }

    if(viewConfigurationTypeCapacityInput < systemIdDescriptor->viewConfigurationsCount){
        *viewConfigurationTypeCountOutput = systemIdDescriptor->viewConfigurationsCount;
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "ViewConfig enum : Size error!");
        return XR_ERROR_SIZE_INSUFFICIENT;
    }


    for(int i = 0; i < systemIdDescriptor->viewConfigurationsCount; i++){
        viewConfigurationTypes[i] = systemIdDescriptor->viewConfigurations[i];
    }

    *viewConfigurationTypeCountOutput = systemIdDescriptor->viewConfigurationsCount;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateViewConfigurations done!");

    return XR_SUCCESS;
}


XrResult mirageGetViewConfigurationProperties(XrSystemId systemId, XrViewConfigurationType viewConfigurationType, XrViewConfigurationProperties *configurationProperties){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageGetViewConfigurationProperties called!");

    XrSystemIdDescriptor* systemIdDescriptor = (XrSystemIdDescriptor*) systemId;

    if(viewConfigurationType != systemIdDescriptor->viewConfigurations[0]){ //TODO : Support more than one view configuration
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "ViewConfigProperties : Not supported!");
        return XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED;
    }

    *configurationProperties = *systemIdDescriptor->viewConfigurationProperties;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageGetViewConfigurationProperties done!");

    return XR_SUCCESS;

}


XrResult mirageEnumerateViewConfigurationViews(XrSystemId systemId, XrViewConfigurationType viewConfigurationType, uint32_t viewCapacityInput, uint32_t *viewCountOutput, XrViewConfigurationView *views){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateViewConfigurationViews called!");

    XrSystemIdDescriptor* systemIdDescriptor = (XrSystemIdDescriptor*) systemId;

    if(viewConfigurationType != systemIdDescriptor->viewConfigurations[0]){ //TODO : Support more than one view configuration
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "ViewConfigViews enum : Not supported!");
        return XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED;
    }

    if(viewCapacityInput == 0 || views == nullptr){
        *viewCountOutput = systemIdDescriptor->viewsCount;
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "ViewConfigViews enum : Giving size!");
        return XR_SUCCESS;
    }

    if(viewCapacityInput < systemIdDescriptor->viewsCount){
        *viewCountOutput = systemIdDescriptor->viewsCount;
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "ViewConfigViews enum : Size error!");
        return XR_ERROR_SIZE_INSUFFICIENT;
    }

    for(int i = 0; i < systemIdDescriptor->viewsCount; i++){
        views[i] = systemIdDescriptor->views[i];
    }

    *viewCountOutput = systemIdDescriptor->viewsCount;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateViewConfigurationViews done!");

    return XR_SUCCESS;

}

XrResult mirageEnumerateEnvironmentBlendModes(XrSystemId systemId, XrViewConfigurationType viewConfigurationType, uint32_t environmentBlendModeCapacityInput,uint32_t *environmentBlendModeCountOutput,XrEnvironmentBlendMode *environmentBlendModes){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateEnvironmentBlendModes called!");

    XrSystemIdDescriptor* systemIdDescriptor = (XrSystemIdDescriptor*) systemId;

    if(viewConfigurationType != systemIdDescriptor->viewConfigurations[0]){ //TODO : Support more than one view configuration
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "EnvironmentBlendModes enum : Not supported!");
        return XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED;
    }

    if(environmentBlendModeCapacityInput == 0 || environmentBlendModes == nullptr){
        *environmentBlendModeCountOutput = systemIdDescriptor->environmentBlendModesCount;
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "EnvironmentBlendModes enum : Giving size!");
        return XR_SUCCESS;
    }


    if(environmentBlendModeCapacityInput < systemIdDescriptor->environmentBlendModesCount){
        *environmentBlendModeCountOutput = systemIdDescriptor->environmentBlendModesCount;
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "EnvironmentBlendModes enum : Size error!");
        return XR_ERROR_SIZE_INSUFFICIENT;
    }

    for(int i = 0; i < systemIdDescriptor->environmentBlendModesCount; i++){
        environmentBlendModes[i] = systemIdDescriptor->environmentBlendModes[i];
    }

    *environmentBlendModeCountOutput = systemIdDescriptor->environmentBlendModesCount;
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateEnvironmentBlendModes done!");

    return XR_SUCCESS;
}

XrResult mirageGetOpenGLESGraphicsRequirementsKHR(XrSystemId systemId, XrGraphicsRequirementsOpenGLESKHR *graphicsRequirements){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageGetOpenGLESGraphicsRequirementsKHR called!");

    XrSystemIdDescriptor* systemIdDescriptor = (XrSystemIdDescriptor*) systemId;

    *graphicsRequirements = *systemIdDescriptor->graphicsRequirements;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageGetOpenGLESGraphicsRequirementsKHR : Minor Ver : %hu, Major Ver : %hu!",
                        XR_VERSION_MAJOR(graphicsRequirements->minApiVersionSupported), XR_VERSION_MAJOR(graphicsRequirements->maxApiVersionSupported));

    return XR_SUCCESS;
}


//SESSION

XrResult mirageCreateSession(const XrSessionCreateInfo *createInfo, XrSession *session){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageCreateSession called!");

    XrSessionDescriptor* sessionDescriptor = NEW_SHARED(XrSessionDescriptor, sharedMemoryDescriptor, (XrInstanceDescriptor*)sharedMemoryDescriptor->get_instance_ptr(), createInfo);

    *session = (XrSession)sessionDescriptor;

    cts_instruction instruction = cts_instruction::POPULATE_INITIAL_SESSION_PROPERTIES;
    send(client_fd, &instruction, sizeof(cts_instruction), 0);
    cts_instruction response = cts_instruction::NONE;

    recv(client_fd, &response, sizeof(cts_instruction), 0);
    if(response != cts_instruction::POPULATE_INITIAL_SESSION_PROPERTIES){
        return XR_ERROR_RUNTIME_FAILURE;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageCreateSession done : %p", sessionDescriptor);

    return XR_SUCCESS;



}

XrResult mirageDestroySession(XrSession session){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageBeginSession(XrSession session, const XrSessionBeginInfo *beginInfo){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageBeginSession called!");

    XrSessionDescriptor* sessionDescriptor = (XrSessionDescriptor*)session;

    sessionDescriptor->primaryViewConfigurationType = beginInfo->primaryViewConfigurationType;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageBeginSession done!");

    return XR_SUCCESS;
}

XrResult mirageEndSession(XrSession session){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageWaitFrame(XrSession session, const XrFrameWaitInfo *frameWaitInfo, XrFrameState *frameState){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageBeginFrame(XrSession session, const XrFrameBeginInfo *frameBeginInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageEndFrame(XrSession session, const XrFrameEndInfo *frameEndInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageRequestExitSession(XrSession session){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageLocateViews(XrSession session, const XrViewLocateInfo *viewLocateInfo, XrViewState *viewState, uint32_t viewCapacityInput, uint32_t *viewCountOutput, XrView *views){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

//SPACE

XrResult mirageEnumerateReferenceSpaces(XrSession session, //PASS TODO : maybe change reference space to stage?
                                        uint32_t spaceCapacityInput,
                                        uint32_t *spaceCountOutput,
                                        XrReferenceSpaceType *spaces){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateReferenceSpaces called!");

    //get session
    XrSessionDescriptor* sessionDescriptor = (XrSessionDescriptor*)session;

    if(spaceCapacityInput == 0 || spaces == nullptr){
        *spaceCountOutput = sessionDescriptor->referenceSpacesCount;
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Giving size!");
        return XR_SUCCESS;
    }

    if(spaceCapacityInput < sessionDescriptor->referenceSpacesCount){
        *spaceCountOutput = sessionDescriptor->referenceSpacesCount;
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Size error!");
        return XR_ERROR_SIZE_INSUFFICIENT;
    }

    for(int i = 0; i < sessionDescriptor->referenceSpacesCount; i++){
        spaces[i] = sessionDescriptor->referenceSpaces[i];
    }

    *spaceCountOutput = sessionDescriptor->referenceSpacesCount;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateReferenceSpaces done!");

    return XR_SUCCESS;
}

XrResult mirageGetReferenceSpaceBoundsRect(XrSession session, XrReferenceSpaceType referenceSpaceType, XrExtent2Df *bounds){


    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageGetReferenceSpaceBoundsRect called!");

    //TODO : Implement this later

    bounds->width = 0;
    bounds->height = 0;

    return XR_SPACE_BOUNDS_UNAVAILABLE;
}

XrResult mirageCreateReferenceSpace(XrSession session, const XrReferenceSpaceCreateInfo *createInfo, XrSpace *space){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageCreateReferenceSpace called!");

    XrReferenceSpaceDescriptor* referenceSpaceDescriptor = NEW_SHARED(XrReferenceSpaceDescriptor, sharedMemoryDescriptor, (XrSessionDescriptor*)session, createInfo);

    *space = (XrSpace)referenceSpaceDescriptor;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageCreateReferenceSpace done!");

    return XR_SUCCESS;

}

XrResult mirageLocateSpace(XrSpace space, XrSpace baseSpace, XrTime time, XrSpaceLocation *location){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageDestroySpace(XrSpace space){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageCreateActionSpace(XrSession session, const XrActionSpaceCreateInfo *createInfo, XrSpace *space){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}


//ACTIONS

XrResult mirageCreateActionSet(const XrActionSetCreateInfo *createInfo, XrActionSet *actionSet){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageDestroyActionSet(XrActionSet actionSet){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageCreateAction(XrActionSet actionSet, const XrActionCreateInfo *createInfo, XrAction *action){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageDestroyAction(XrAction action){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageSuggestInteractionProfileBindings(const XrInteractionProfileSuggestedBinding *suggestedBindings){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageAttachSessionActionSets(XrSession session, const XrSessionActionSetsAttachInfo *bindInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageGetCurrentInteractionProfile(XrSession session, XrPath topLevelUserPath, XrInteractionProfileState *interactionProfile){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageGetActionStateBoolean(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStateBoolean *data){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageGetActionStateFloat(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStateFloat *data){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageGetActionStateVector2f(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStateVector2f *data){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageGetActionStatePose(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStatePose *data){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageSyncActions(XrSession session, const XrActionsSyncInfo *syncInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageEnumerateBoundSourcesForAction(XrSession session, const XrBoundSourcesForActionEnumerateInfo *enumerateInfo,
                                              uint32_t sourceCapacityInput, uint32_t *sourceCountOutput, XrPath *sources){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageGetInputSourceLocalizedName(XrSession session, const XrInputSourceLocalizedNameGetInfo *getInfo,
                                           uint32_t bufferCapacityInput, uint32_t *bufferCountOutput, char *buffer){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageApplyHapticFeedback(XrSession session, const XrHapticActionInfo *hapticActionInfo, const XrHapticBaseHeader *hapticEvent){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageStopHapticFeedback(XrSession session, const XrHapticActionInfo *hapticActionInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageCreateHandTrackerEXT(XrSession session, const XrHandTrackerCreateInfoEXT *createInfo, XrHandTrackerEXT *handTracker){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageDestroyHandTrackerEXT(XrHandTrackerEXT handTracker){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageLocateHandJointsEXT(XrHandTrackerEXT handTracker, const XrHandJointsLocateInfoEXT *locateInfo, XrHandJointLocationsEXT *locations){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageApplyForceFeedbackCurlMNDX(XrHandTrackerEXT handTracker, const XrForceFeedbackCurlApplyLocationsMNDX *locations){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageEnumerateDisplayRefreshRatesFB(XrSession session, uint32_t displayRefreshRateCapacityInput,
                                              uint32_t *displayRefreshRateCountOutput, float *displayRefreshRates){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageGetDisplayRefreshRateFB(XrSession session, float *displayRefreshRate){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageRequestDisplayRefreshRateFB(XrSession session, float displayRefreshRate){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}


//SWAPCHAINS
XrResult mirageEnumerateSwapchainFormats(XrSession session, uint32_t formatCapacityInput, uint32_t *formatCountOutput, int64_t *formats){


    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateSwapchainFormats called!");

    XrSessionDescriptor* sessionDescriptor = (XrSessionDescriptor*)session;

    if(formatCapacityInput == 0 || formats == nullptr){
        *formatCountOutput = sessionDescriptor->swapchainFormatsCount;
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "SwapchainFormat : Giving size!");
        return XR_SUCCESS;
    }

    if(formatCapacityInput < sessionDescriptor->swapchainFormatsCount){
        *formatCountOutput = sessionDescriptor->swapchainFormatsCount;
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "SwapchainFormat error!");
        return XR_ERROR_SIZE_INSUFFICIENT;
    }

    for(int i = 0; i < sessionDescriptor->swapchainFormatsCount; i++){
        formats[i] = sessionDescriptor->swapchainFormats[i];
    }

    *formatCountOutput = sessionDescriptor->swapchainFormatsCount;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateSwapchainFormats done!");

    return XR_SUCCESS;
}

XrResult mirageCreateSwapchain(XrSession session, const XrSwapchainCreateInfo *createInfo, XrSwapchain *swapchain){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageDestroySwapchain(XrSwapchain swapchain){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageEnumerateSwapchainImages(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t *imageCountOutput, XrSwapchainImageBaseHeader *images){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageAcquireSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageAcquireInfo *acquireInfo, uint32_t *index){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageWaitSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageWaitInfo *waitInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageReleaseSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageReleaseInfo *releaseInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}


//DEBUG EXT


XrResult mirageSetDebugUtilsObjectNameEXT(const XrDebugUtilsObjectNameInfoEXT *nameInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageCreateDebugUtilsMessengerEXT(
        const XrDebugUtilsMessengerCreateInfoEXT *createInfo,
        XrDebugUtilsMessengerEXT *messenger){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}
XrResult mirageDestroyDebugUtilsMessengerEXT(XrDebugUtilsMessengerEXT messenger){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageSubmitDebugUtilsMessageEXT(
        XrDebugUtilsMessageSeverityFlagsEXT messageSeverity,
        XrDebugUtilsMessageTypeFlagsEXT messageTypes,
        const XrDebugUtilsMessengerCallbackDataEXT *callbackData){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageSessionBeginDebugUtilsLabelRegionEXT(XrSession session, const XrDebugUtilsLabelEXT *labelInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageSessionEndDebugUtilsLabelRegionEXT(XrSession session){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageSessionInsertDebugUtilsLabelEXT(XrSession session, const XrDebugUtilsLabelEXT *labelInfo){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_VALIDATION_FAILURE;}
