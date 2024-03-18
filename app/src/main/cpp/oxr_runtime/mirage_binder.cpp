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

XrResult pollMirageEvents(XrEventDataBuffer *eventData){


    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "PollMirageEvents");

    XrInstanceDescriptor* instanceDescriptor = (XrInstanceDescriptor*)sharedMemoryDescriptor->get_instance_ptr();

    if(instanceDescriptor == nullptr){
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "InstanceDescriptor is null!");
        return XR_ERROR_VALIDATION_FAILURE;
    }

    XrEventDataBuffer* eventDataBuffer = instanceDescriptor->popEvent(sharedMemoryDescriptor);

    if(eventDataBuffer == nullptr){
        //__android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "No event found!");
        return XR_EVENT_UNAVAILABLE;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Event found : %d", ((XrEventDataSessionStateChanged*)eventDataBuffer)->state);

    *eventData = *eventDataBuffer;
    sharedMemoryDescriptor->memory_free(eventDataBuffer);

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "PollMirageEventsEnd");

    return XR_SUCCESS;
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

        *out_path = (XrPath)1;

        return XR_SUCCESS;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Paths found");

    uint64_t counter = 1;

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

XrResult getMirageSystemProperties(XrSystemId systemId, XrSystemProperties *properties){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Get Mirage System Properties!");

    XrSystemIdDescriptor* systemIdDescriptor = (XrSystemIdDescriptor*) systemId;

    if(systemIdDescriptor == nullptr){
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "SystemIdDescriptor is null!");
        return XR_ERROR_VALIDATION_FAILURE;
    }

    *properties = *systemIdDescriptor->systemProperties;

    return XR_SUCCESS;

}

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
        __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "ViewConfig enum : %d", viewConfigurationTypes[i]);
    }

    *viewConfigurationTypeCountOutput = systemIdDescriptor->viewConfigurationsCount;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateViewConfigurations done!");

    return XR_SUCCESS;
}


XrResult mirageGetViewConfigurationProperties(XrSystemId systemId, XrViewConfigurationType viewConfigurationType, XrViewConfigurationProperties *configurationProperties){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageGetViewConfigurationProperties called!");

    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "ViewConfigProperties : %d", viewConfigurationType);

    XrSystemIdDescriptor* systemIdDescriptor = (XrSystemIdDescriptor*) systemId;

    if(viewConfigurationType != systemIdDescriptor->viewConfigurations[0]){ //TODO : Support more than one view configuration
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "ViewConfigProperties : Not supported!");
        return XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED;
    }

    *configurationProperties = *systemIdDescriptor->viewConfigurationProperties;

    //PRINT CONFIGURATION PROPERTIES
    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Type : %d, Next : %p, ViewConfigType : %d, fovmut : %d", systemIdDescriptor->viewConfigurationProperties->type, systemIdDescriptor->viewConfigurationProperties->next, systemIdDescriptor->viewConfigurationProperties->viewConfigurationType, systemIdDescriptor->viewConfigurationProperties->fovMutable);

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

    //Log the views
    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "ViewConfigViews count: %d", *viewCountOutput);
    for(int i = 0; i < *viewCountOutput; i++){
        __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "ViewConfigViews : %d, %u, %u, %u, %u, %u, %u, %u, %u, %u", views[i].type, views[i].recommendedImageRectWidth, views[i].recommendedImageRectHeight, views[i].maxImageRectWidth, views[i].maxImageRectHeight, views[i].recommendedSwapchainSampleCount, views[i].maxSwapchainSampleCount, views[i].recommendedImageRectWidth, views[i].recommendedImageRectHeight, views[i].maxImageRectWidth, views[i].maxImageRectHeight);
    }

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

XrResult mirageWaitFrame(XrSession session, const XrFrameWaitInfo *frameWaitInfo, XrFrameState *frameState){

    //__android_log_print(ANDROID_LOG_ERROR, "PICOREUR2", "Unimplemented");

    //Send a message to the server to wait for a frame
    cts_instruction instruction = cts_instruction::WAIT_FRAME;
    send(client_fd, &instruction, sizeof(cts_instruction), 0);

    //Receive the same instruction pinged back, when the frame wait has ended
    recv(client_fd, &instruction, sizeof(cts_instruction), 0);
    if(instruction != cts_instruction::WAIT_FRAME){
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "WaitFrame : Instruction error!");
        return XR_ERROR_RUNTIME_FAILURE;
    }

    //Get session descriptor
    XrSessionDescriptor* sessionDescriptor = (XrSessionDescriptor*)session;

    //Retrieve data from the shared memory
    *frameState = *sessionDescriptor->waitFrameState;

    //Print the frame state
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER PICOREUR", "Frame State : Predicted Time : %ld, Predicted delay : %ld, Should render : %d", frameState->predictedDisplayTime, frameState->predictedDisplayPeriod, frameState->shouldRender);

    return XR_SUCCESS;
}

XrResult mirageBeginFrame(XrSession session, const XrFrameBeginInfo *frameBeginInfo){
    __android_log_print(ANDROID_LOG_WARN, "PICOREUR2", "Unimplemented");

    //TODO : Implement this later


    return XR_SUCCESS;

}

XrResult mirageEndFrame(XrSession session, const XrFrameEndInfo *frameEndInfo){
    __android_log_print(ANDROID_LOG_WARN, "PICOREUR2", "Unimplemented");

    //Display the frameEndInfo
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER PICOREUR", "Frame End Info : Display Time : %ld, Layer Count : %d", frameEndInfo->displayTime, frameEndInfo->layerCount);

    XrSessionDescriptor* sessionDescriptor = (XrSessionDescriptor*)session;
    XrSwapchainDescriptor* swapchain = (XrSwapchainDescriptor*)sessionDescriptor->firstSwapchainDescriptor;

    /*GLuint texture = ((XrSwapchainDescriptor*)swapchain)->clientTextureIds[0];
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    GLuint fbo;
    glGenBuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_ARRAY, texture, 0);
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0, 0);
    float r = ((float)(rainbow_counter % 800)) / 799.0f;
    float g = ((float)(rainbow_counter % 400)) / 399.0f;
    float b = ((float)(rainbow_counter % 200)) / 199.0f;
    glClearColor(r, g, b, 1.0f);
    rainbow_counter += 1;
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);*/

    glFinish();

    return XR_SUCCESS;
}

XrResult mirageRequestExitSession(XrSession session){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageLocateViews(XrSession session, const XrViewLocateInfo *viewLocateInfo, XrViewState *viewState, uint32_t viewCapacityInput, uint32_t *viewCountOutput, XrView *views){

    //TODO USE viewLocateInfo data for better view location
    __android_log_print(ANDROID_LOG_WARN, "PICOREUR2", "mirageLocateViews : %d, requestedInput : %d", viewLocateInfo->viewConfigurationType, viewCapacityInput);

    viewState->type = XR_TYPE_VIEW_STATE;
    viewState->next = nullptr;


    //You do the same than enumerate
    XrSessionDescriptor* sessionDescriptor = (XrSessionDescriptor*)session;

    if(viewCapacityInput == 0 || views == nullptr){
        *viewCountOutput = sessionDescriptor->viewCount;
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "ViewLocate : Giving size!");
        return XR_SUCCESS;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "ViewLocate : View count : %d, view capacity : %d", sessionDescriptor->viewCount, viewCapacityInput);

    if(viewCapacityInput < sessionDescriptor->viewCount){
        *viewCountOutput = sessionDescriptor->viewCount;
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "ViewLocate : size incorrect!");
        return XR_ERROR_SIZE_INSUFFICIENT;
    }

    for(int i = 0; i < viewCapacityInput; i++){
        views[i] = sessionDescriptor->views[i];
    }

    *viewCountOutput = sessionDescriptor->viewCount;

    viewState->viewStateFlags = XR_VIEW_STATE_ORIENTATION_VALID_BIT | XR_VIEW_STATE_POSITION_VALID_BIT |
                                XR_VIEW_STATE_ORIENTATION_TRACKED_BIT | XR_VIEW_STATE_POSITION_TRACKED_BIT;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageLocateViews done!");

    return XR_SUCCESS;
}

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

XrResult mirageLocateSpace(XrSpace space, XrSpace baseSpace, XrTime time, XrSpaceLocation *location){
    __android_log_print(ANDROID_LOG_ERROR, "PICOREUR2", "Unimplemented");

    *location = XrSpaceLocation{XR_TYPE_SPACE_LOCATION,
                                nullptr, XR_SPACE_LOCATION_ORIENTATION_VALID_BIT | XR_SPACE_LOCATION_POSITION_VALID_BIT,
                                XrPosef{
        XrQuaternionf{0, 0, 0, 1},
        XrVector3f{0, 0, 0}}};

    XrActionSpaceDescriptor* spaceDescriptor = (XrActionSpaceDescriptor*)space;
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Space : %p", spaceDescriptor);
    if(spaceDescriptor != nullptr && spaceDescriptor->signature == XR_ACTION_SPACE_DESCRIPTOR_SIGNATURE){
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Action space found! : %lx", spaceDescriptor->signature);
        *location = XrSpaceLocation{XR_TYPE_SPACE_LOCATION,
                                    nullptr, XR_SPACE_LOCATION_ORIENTATION_VALID_BIT | XR_SPACE_LOCATION_POSITION_VALID_BIT |
                                             XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT | XR_SPACE_LOCATION_POSITION_TRACKED_BIT,
                                    spaceDescriptor->currentPose};

    }



    //TODO USE REAL SPACE LOCATION

    return XR_SUCCESS;

}

XrResult mirageDestroySpace(XrSpace space){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

//TODO NEXT
XrResult mirageCreateActionSpace(XrSession session, const XrActionSpaceCreateInfo *createInfo, XrSpace *space){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageCreateActionSpace called!");

    XrActionSpaceDescriptor* actionSpaceDescriptor = NEW_SHARED(XrActionSpaceDescriptor, sharedMemoryDescriptor, (XrSessionDescriptor*)session, createInfo);

    *space = (XrSpace)actionSpaceDescriptor;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageCreateActionSpace done!");

    return XR_SUCCESS;
}


//ACTIONS
//    XrActionSetDescriptor* actionSetDescriptor = NEW_SHARED(XrActionSetDescriptor, sharedMemoryDescriptor, createInfo, nullptr);
XrResult mirageCreateActionSet(const XrActionSetCreateInfo *createInfo, XrActionSet *actionSet){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageCreateActionSet called!");

    //DO IT THE SAME WAY THAN mirageStringToPath

    XrActionSetDescriptor* actionSetDescriptor = (XrActionSetDescriptor*) ((XrInstanceDescriptor*)sharedMemoryDescriptor->get_instance_ptr())->firstActionSetDescriptor;

    if(actionSetDescriptor == nullptr){
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "No action sets found");

        ((XrInstanceDescriptor*)sharedMemoryDescriptor->get_instance_ptr())->firstActionSetDescriptor = NEW_SHARED(XrActionSetDescriptor, sharedMemoryDescriptor, createInfo, nullptr);

        *actionSet = (XrActionSet)1;

        return XR_SUCCESS;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Action sets found");

    uint64_t counter = 1;

    XrActionSetDescriptor* lastActionSetDescriptor = actionSetDescriptor;

    //Check the existing action sets

    while(actionSetDescriptor != nullptr){
        if(strcmp(actionSetDescriptor->createInfo->actionSetName, createInfo->actionSetName) == 0){
            *actionSet = (XrActionSet)counter;
            return XR_SUCCESS;
        }
        lastActionSetDescriptor = actionSetDescriptor;
        actionSetDescriptor = actionSetDescriptor->nextActionSetDescriptor;
        counter++;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Action set not found, adding it!");

    lastActionSetDescriptor->nextActionSetDescriptor = NEW_SHARED(XrActionSetDescriptor, sharedMemoryDescriptor, createInfo, lastActionSetDescriptor);

    *actionSet = (XrActionSet)counter;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Action set added!");

    return XR_SUCCESS;
}

XrResult mirageDestroyActionSet(XrActionSet actionSet){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageCreateAction(XrActionSet actionSet, const XrActionCreateInfo *createInfo, XrAction *action){

    //Similar way of doing it than mirageCreateActionSet

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageCreateAction called!");

    XrActionSetDescriptor* actionSetDescriptor = (XrActionSetDescriptor*) ((XrInstanceDescriptor*)sharedMemoryDescriptor->get_instance_ptr())->firstActionSetDescriptor;

    if(actionSetDescriptor == nullptr){
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_ERROR", "Wrong action set");

        return XR_ERROR_VALIDATION_FAILURE;
    }

    XrActionDescriptor* actionDescriptor = actionSetDescriptor->firstActionDescriptor;

    if(actionDescriptor == nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "No actions found");

        actionSetDescriptor->firstActionDescriptor = NEW_SHARED(
                XrActionDescriptor, sharedMemoryDescriptor, createInfo, nullptr);

        *action = (XrAction) actionSetDescriptor->firstActionDescriptor;

        if(*action == nullptr){
            __android_log_print(ANDROID_LOG_WARN, "MIRAGE_BINDER", "Action value : %p", *action);
        }
        //__android_log_print(ANDROID_LOG_WARN, "MIRAGE_BINDER", "Action value : %p", *action);

        return XR_SUCCESS;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Actions found");

    XrActionDescriptor* lastActionDescriptor = actionDescriptor;

    //Check the existing actions
    while(actionDescriptor != nullptr) {
        if (strcmp(actionDescriptor->createInfo->actionName, createInfo->actionName) == 0) {
            *action = (XrAction) actionDescriptor;
            if(*action == nullptr){
                __android_log_print(ANDROID_LOG_WARN, "MIRAGE_BINDER", "Action alert value : %p", *action);
            }
            return XR_SUCCESS;
        }
        lastActionDescriptor = actionDescriptor;
        actionDescriptor = actionDescriptor->nextActionDescriptor;
    }



    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Action not found, adding it!");

    lastActionDescriptor->nextActionDescriptor = NEW_SHARED(XrActionDescriptor, sharedMemoryDescriptor, createInfo, lastActionDescriptor);

    *action = (XrAction) lastActionDescriptor->nextActionDescriptor;
    if(*action == nullptr){
        __android_log_print(ANDROID_LOG_WARN, "MIRAGE_BINDER", "Action value 2 : %p", *action);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Action added!");

    return XR_SUCCESS;
}

XrResult mirageDestroyAction(XrAction action){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageSuggestInteractionProfileBindings(const XrInteractionProfileSuggestedBinding *suggestedBindings){

    //TODO TEST

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageSuggestInteractionProfileBindings called!");

    //DEEP COPY THE STRUCTURE
    XrInstanceDescriptor* instanceDescriptor = (XrInstanceDescriptor*)sharedMemoryDescriptor->get_instance_ptr();

    XrInteractionProfileSuggestedBinding* suggestedBindingsCopy = NEW_SHARED(XrInteractionProfileSuggestedBinding);
    suggestedBindingsCopy->type = suggestedBindings->type;
    suggestedBindingsCopy->next = nullptr;
    suggestedBindingsCopy->interactionProfile = suggestedBindings->interactionProfile;
    suggestedBindingsCopy->countSuggestedBindings = suggestedBindings->countSuggestedBindings;

    suggestedBindingsCopy->suggestedBindings = (XrActionSuggestedBinding*)malloc(suggestedBindings->countSuggestedBindings * sizeof(XrActionSuggestedBinding));

    for(int i = 0; i < suggestedBindings->countSuggestedBindings; i++){
        ((XrActionSuggestedBinding *)suggestedBindingsCopy->suggestedBindings)[i].action = suggestedBindings->suggestedBindings[i].action;
        ((XrActionSuggestedBinding *)suggestedBindingsCopy->suggestedBindings)[i].binding = suggestedBindings->suggestedBindings[i].binding;
    }

    /*if(instanceDescriptor->interactionProfileSuggestedBindings != nullptr){
        for(int i = 0; i < instanceDescriptor->interactionProfileSuggestedBindings->countSuggestedBindings; i++){
            sharedMemoryDescriptor->memory_free((void*)instanceDescriptor->interactionProfileSuggestedBindings->suggestedBindings);
        }
        sharedMemoryDescriptor->memory_free((void*)instanceDescriptor->interactionProfileSuggestedBindings);
    }*/

    instanceDescriptor->interactionProfileSuggestedBindings = suggestedBindingsCopy;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageSuggestInteractionProfileBindings done!");

    return XR_SUCCESS;
}

XrResult mirageAttachSessionActionSets(XrSession session, const XrSessionActionSetsAttachInfo *bindInfo){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageAttachSessionActionSets called!");

    XrSessionDescriptor* sessionDescriptor = (XrSessionDescriptor*)session;

    if(sessionDescriptor->countActionSets != 0 || sessionDescriptor->actionSets != nullptr){
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "Action sets already attached!");
        return XR_ERROR_ACTIONSETS_ALREADY_ATTACHED;
    }

    sessionDescriptor->countActionSets = bindInfo->countActionSets;
    sessionDescriptor->actionSets = (XrActionSet*)malloc(bindInfo->countActionSets * sizeof(XrActionSet));

    for(int i = 0; i < bindInfo->countActionSets; i++){
        sessionDescriptor->actionSets[i] = bindInfo->actionSets[i];
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageAttachSessionActionSets done!");

    return XR_SUCCESS;
}

XrResult mirageGetCurrentInteractionProfile(XrSession session, XrPath topLevelUserPath, XrInteractionProfileState *interactionProfile){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageGetCurrentInteractionProfile called!");

    //For now just return the suggested bindings
    XrInstanceDescriptor* instanceDescriptor = (XrInstanceDescriptor*)sharedMemoryDescriptor->get_instance_ptr();

    interactionProfile->type = XR_TYPE_INTERACTION_PROFILE_STATE;
    interactionProfile->next = nullptr;
    interactionProfile->interactionProfile = instanceDescriptor->interactionProfileSuggestedBindings->interactionProfile;

    return XR_SUCCESS;
}

XrResult mirageGetActionStateBoolean(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStateBoolean *data){

    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "ActionStateBoolean called!");

    XrActionDescriptor* actionDescriptor = (XrActionDescriptor*)getInfo->action;

    uint32_t subPathIndex = -1;
    for(int i = 0; i < actionDescriptor->createInfo->countSubactionPaths; i++){
        if(actionDescriptor->createInfo->subactionPaths[i] == getInfo->subactionPath){
            subPathIndex = i;
            break;
        }
    }

    if(subPathIndex == -1){
        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR2L", "Subaction path not found!");
        return XR_ERROR_PATH_INVALID;
    }



    //Get the action state
    *data = actionDescriptor->actionState[subPathIndex].boolean;

    data->isActive = XR_TRUE;
    data->changedSinceLastSync = XR_TRUE;

    return XR_SUCCESS;
}

XrResult mirageGetActionStateFloat(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStateFloat *data) {


    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "ActionStateFloat called!");

    XrActionDescriptor *actionDescriptor = (XrActionDescriptor *) getInfo->action;

    uint32_t subPathIndex = -1;
    for (int i = 0; i < actionDescriptor->createInfo->countSubactionPaths; i++) {
        if (actionDescriptor->createInfo->subactionPaths[i] == getInfo->subactionPath) {
            subPathIndex = i;
            break;
        }
    }

    if (subPathIndex == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR2", "Subaction path not found!");
        return XR_ERROR_PATH_INVALID;
    }

    //Get the action state
    *data = actionDescriptor->actionState[subPathIndex].floatAction;

    data->isActive = XR_TRUE;
    data->changedSinceLastSync = XR_TRUE;

    return XR_SUCCESS;
}

XrResult mirageGetActionStateVector2f(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStateVector2f *data){

    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "ActionStateVector2f called!");

    XrActionDescriptor *actionDescriptor = (XrActionDescriptor *) getInfo->action;

    uint32_t subPathIndex = -1;
    for (int i = 0; i < actionDescriptor->createInfo->countSubactionPaths; i++) {
        if (actionDescriptor->createInfo->subactionPaths[i] == getInfo->subactionPath) {
            subPathIndex = i;
            break;
        }
    }

    if (subPathIndex == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR2", "Subaction path not found!");
        return XR_ERROR_PATH_INVALID;
    }

    //Get the action state
    *data = actionDescriptor->actionState[subPathIndex].vector2f;

    data->isActive = XR_TRUE;
    data->changedSinceLastSync = XR_TRUE;

    return XR_SUCCESS;
}

XrResult mirageGetActionStatePose(XrSession session, const XrActionStateGetInfo *getInfo, XrActionStatePose *data){

    __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "ActionStatePose called!");

    XrActionDescriptor *actionDescriptor = (XrActionDescriptor *) getInfo->action;

    uint32_t subPathIndex = -1;
    for (int i = 0; i < actionDescriptor->createInfo->countSubactionPaths; i++) {
        if (actionDescriptor->createInfo->subactionPaths[i] == getInfo->subactionPath) {
            subPathIndex = i;
            break;
        }
    }

    if (subPathIndex == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "PICOREUR2", "Subaction path not found!");
        return XR_ERROR_PATH_INVALID;
    }

    //Get the action state
    *data = actionDescriptor->actionState[subPathIndex].pose;

    return XR_SUCCESS;
}

XrResult mirageSyncActions(XrSession session, const XrActionsSyncInfo *syncInfo){
    __android_log_print(ANDROID_LOG_ERROR, "PICOREUR2", "Unimplemented");

    return XR_SUCCESS;}

XrResult mirageEnumerateBoundSourcesForAction(XrSession session, const XrBoundSourcesForActionEnumerateInfo *enumerateInfo,
                                              uint32_t sourceCapacityInput, uint32_t *sourceCountOutput, XrPath *sources){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageGetInputSourceLocalizedName(XrSession session, const XrInputSourceLocalizedNameGetInfo *getInfo,
                                           uint32_t bufferCapacityInput, uint32_t *bufferCountOutput, char *buffer){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageApplyHapticFeedback(XrSession session, const XrHapticActionInfo *hapticActionInfo, const XrHapticBaseHeader *hapticEvent){

    __android_log_print(ANDROID_LOG_ERROR, "PICOREUR2", "Unimplemented");

    return XR_SUCCESS;}

XrResult mirageStopHapticFeedback(XrSession session, const XrHapticActionInfo *hapticActionInfo){

    __android_log_print(ANDROID_LOG_ERROR, "PICOREUR2", "Unimplemented");

    return XR_SUCCESS;}

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

    //LOG THE OUTPUT FORMATS

//    for(int i = 0; i < *formatCountOutput; i++){
//        __android_log_print(ANDROID_LOG_WARN, "MIRAGE_BINDER PICOREUR", "SwapchainFormat : %d", formats[i]);
//    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateSwapchainFormats done!");

    return XR_SUCCESS;
}

//TODO REMOVE: ---------------------------------------------------------
#include <vector>

std::vector<unsigned char> generateGradient(int width, int height) {
    std::vector<unsigned char> gradientData(width * height * 4); // RGBA format

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Calculer les valeurs R, G, B, A en fonction des coordonnées x et y
            unsigned char r = static_cast<unsigned char>(255 * x / (width - 1));
            unsigned char g = 128;
            unsigned char b = static_cast<unsigned char>(255 * y / (height - 1));
            unsigned char a = 250; // Opacité maximale

            // Insérer les valeurs de couleur dans le tableau de données
            int index = (y * width + x) * 4;
            gradientData[index] = r;
            gradientData[index + 1] = g;
            gradientData[index + 2] = b;
            gradientData[index + 3] = a;
        }
    }

    return gradientData;
}



//TODO-------------------------------------------------------------------------------------------

void intermediateGenerateSwapchainImage(const XrSwapchainCreateInfo *createInfo, uint32_t format, uint32_t pixelStride, AHardwareBuffer** hardwareBuffer, GLuint* texture){
    //Create hardware buffer
    AHardwareBuffer_Desc bufferDesc;
    bufferDesc.width = createInfo->width;
    bufferDesc.height = createInfo->height;
    bufferDesc.format = format;
    bufferDesc.layers = 2;//createInfo->arraySize;
    bufferDesc.usage = AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE | AHARDWAREBUFFER_USAGE_GPU_FRAMEBUFFER; //Color + Depth + Stencil
    bufferDesc.stride = createInfo->width * pixelStride;
    bufferDesc.rfu0 = 0;
    bufferDesc.rfu1 = 0;

    //Allocate the hardware buffer
    AHardwareBuffer_allocate(&bufferDesc, hardwareBuffer);

    //Create the swapchain image
    EGLClientBuffer eglClientBuffer = eglGetNativeClientBufferANDROID(*hardwareBuffer);
    EGLImageKHR eglImageKHR = eglCreateImageKHR(eglGetCurrentDisplay(), EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, eglClientBuffer, nullptr);


    //Create OpenGL ES Texture and link it to the hardware buffer
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, *texture);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D_ARRAY, (GLeglImageOES)eglImageKHR);

    //Optional parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//    //TODO : REMOVE THIS DEBUG ----------------------------------------------------------------------------------------------
//    std::vector<unsigned char> gradientData = generateGradient(createInfo->width, createInfo->height); //DEBUG
//    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, createInfo->width, createInfo->height, GL_RGBA, GL_UNSIGNED_BYTE, gradientData.data());
//    //TODO : ----------------------------------------------------------------------------------------------------------------

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    glFinish();
}

XrResult mirageCreateSwapchain(XrSession session, const XrSwapchainCreateInfo *createInfo, XrSwapchain *swapchain){

    //TODO : Implement this
    //TODO : Support CUBEMAPS
    //TODO : LINK TO THE SESSION OPENGL ES CONTEXT (It should be ok in this part of the code however)



    //Print createInfo data
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER PICOREUR", "MirageCreateSwapchain called!");

    uint32_t format = 0;
    uint32_t pixelStride = 4;
    switch(createInfo->format){
        case 0x8058: //RGBA8_EXT
            format = AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM;
            pixelStride = 4;
            break;
        case 0x8C43: //SRGB8_ALPHA8_EXT
            format = AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM; //it's the same bytes, but should be used differently on the server
            pixelStride = 4;
            break;
        case 0x81A5: //GL_DEPTH_COMPONENT16
            format = AHARDWAREBUFFER_FORMAT_D16_UNORM;
            pixelStride = 2;
            break;
        case 0x88F0: //GL_DEPTH24_STENCIL8
            format = AHARDWAREBUFFER_FORMAT_D24_UNORM_S8_UINT;
            pixelStride = 4;
            break;
        case 0x8CAC: //GL_DEPTH_COMPONENT32F
            format = AHARDWAREBUFFER_FORMAT_D32_FLOAT;
            pixelStride = 4;
            break;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER PICOREUR", "Format : %d", format);

    if(format == 0){
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "Format not supported!");
        return XR_ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED;
    }


    //IT IS NOT MANDATORY TO ALLOCATE THEM IN SHARED MEMORY
    AHardwareBuffer** hardwareBuffers = (AHardwareBuffer**)sharedMemoryDescriptor->memory_allocate(SWAPCHAIN_IMAGE_COUNT * sizeof(AHardwareBuffer*));
    GLuint* textures = (GLuint*)sharedMemoryDescriptor->memory_allocate(SWAPCHAIN_IMAGE_COUNT * sizeof(GLuint));

    //debug
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER PICOREUR", "Array size : %d", createInfo->arraySize);

    //CREATING 3 TEXTURES FOR THE SWAPCHAIN
    for(int i = 0; i < SWAPCHAIN_IMAGE_COUNT; i++){
        AHardwareBuffer* hardwareBuffer;
        GLuint texture;
        intermediateGenerateSwapchainImage(createInfo, format, pixelStride, &hardwareBuffer, &texture);

        //Debug try glclear
        /*glBindTexture(GL_TEXTURE_2D, texture);
        GLuint fbo;
        glGenBuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
        glBindTexture(GL_TEXTURE_2D, 0);*/

        hardwareBuffers[i] = hardwareBuffer;
        textures[i] = texture;
    }

    //debug
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER PICOREUR", "Hardware buffers and textures created!");

    XrSwapchainDescriptor* swapchainDescriptor = NEW_SHARED(XrSwapchainDescriptor, sharedMemoryDescriptor, (XrSessionDescriptor*)session, createInfo, hardwareBuffers, textures);

    //debug
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER PICOREUR", "Swapchain descriptor created!");


    //TELL THE SERVER THE SWAPCHAIN WHERE THE HARDWARE BUFFERS ARE
    XrInstanceDescriptor* instanceDescriptor = (XrInstanceDescriptor*)sharedMemoryDescriptor->get_instance_ptr();
    if(instanceDescriptor->tempSwapchainDescriptor != nullptr){
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "Swapchain already being created, multithreading on xrCreateSwapchain inside the same instance not supported!");
        return XR_ERROR_RUNTIME_FAILURE;
    }
    instanceDescriptor->tempSwapchainDescriptor = swapchainDescriptor;

    //Send the hardware buffers to the client

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Sending hardware buffers to client!");

    //SEND A BUFFER TO THE CLIENT TO SAY THAT WE ARE SENDING HARDWARE BUFFERS
    cts_instruction instruction = cts_instruction::SHARE_SWAPCHAIN_AHARDWAREBUFFER;
    send(client_fd, &instruction, sizeof(cts_instruction), 0);

    for(int i = 0; i < SWAPCHAIN_IMAGE_COUNT; i++) {
        //Ready to send an hardware buffer

        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "Waiting for client to be ready to receive AHardwareBuffer!");

        recv(client_fd, &instruction, sizeof(cts_instruction), 0);
        if (instruction != cts_instruction::SHARE_SWAPCHAIN_AHARDWAREBUFFER_READY) {
            __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "Ping back error!");
        }

        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER",
                            "Sending AHardwareBuffer to client!");

        AHardwareBuffer_sendHandleToUnixSocket(hardwareBuffers[i], client_fd);
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "AHardwareBuffer sent to server!");
    }

    //CHECK PING BACK, AND SENDING HARDWARE BUFFERS OK
    recv(client_fd, &instruction, sizeof(cts_instruction), 0);

    if(instruction != cts_instruction::SHARE_SWAPCHAIN_AHARDWAREBUFFER){
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "Ping back error!");
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "AHardwareBuffers sent to server");

    //ADD THE SWAPCHAIN TO THE SESSION, USE THE SAME CODE STRUCTURE THAN XRCREATEACTIONSET, a linked list
    XrSessionDescriptor* sessionDescriptor = (XrSessionDescriptor*)session;

    *swapchain = (XrSwapchain)swapchainDescriptor;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageCreateSwapchain done (2)!");

    return XR_SUCCESS;
}

XrResult mirageDestroySwapchain(XrSwapchain swapchain){ __android_log_print(ANDROID_LOG_DEBUG, "PICOREUR2", "Unimplemented"); return XR_ERROR_RUNTIME_FAILURE;}

XrResult mirageEnumerateSwapchainImages(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t *imageCountOutput, XrSwapchainImageBaseHeader *images){

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateSwapchainImages called!");

    //DEBUG

    rainbow_counter = 0;
//    while(rainbow_counter < 10){
//        GLuint texture = ((XrSwapchainDescriptor*)swapchain)->clientTextureIds[0];
//        glBindTexture(GL_TEXTURE_2D, texture);
//        GLuint fbo;
//        glGenBuffers(1, &fbo);
//        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
//        float r = ((float)(rainbow_counter % 8)) / 7.0f;
//        float g = ((float)(rainbow_counter % 4)) / 3.0f;
//        float b = ((float)(rainbow_counter % 2)) / 1.0f;
//        glClearColor(r, g, b, 1.0f);
//        rainbow_counter += 1;
//        glClear(GL_COLOR_BUFFER_BIT);
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        glDeleteFramebuffers(1, &fbo);
//        glBindTexture(GL_TEXTURE_2D, 0);
//
//        glFinish();
//
//        sleep(1);
//
//
//    }



    /*texture = ((XrSwapchainDescriptor*)swapchain)->clientTextureIds[1];
    glBindTexture(GL_TEXTURE_2D, texture);
    glGenBuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    glBindTexture(GL_TEXTURE_2D, 0);*/

    XrSwapchainDescriptor* swapchainDescriptor = (XrSwapchainDescriptor*)swapchain;

    if(imageCapacityInput == 0 || images == nullptr){
        *imageCountOutput = swapchainDescriptor->bufferCount;
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateSwapchainImages Giving size!");
        return XR_SUCCESS;
    }

    if(imageCapacityInput < swapchainDescriptor->bufferCount){
        *imageCountOutput = swapchainDescriptor->bufferCount;
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE_BINDER", "MirageEnumerateSwapchainImages Size error!");
        return XR_ERROR_SIZE_INSUFFICIENT;
    }

    for(int i = 0; i < swapchainDescriptor->bufferCount; i++){
        XrSwapchainImageOpenGLESKHR* image = &(((XrSwapchainImageOpenGLESKHR*)images)[i]);
        image->type = XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR;
        image->next = nullptr;
        image->image = swapchainDescriptor->clientTextureIds[i];
    }

    *imageCountOutput = swapchainDescriptor->bufferCount;

    //LOG DATA
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER PICOREUR", "Should be %d images.", *imageCountOutput);
    for(int i = 0; i < *imageCountOutput; i++){
        __android_log_print(ANDROID_LOG_WARN, "MIRAGE_BINDER PICOREUR", "SwapchainImaged : %d", ((XrSwapchainImageOpenGLESKHR*)images)[i].image);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_BINDER", "MirageEnumerateSwapchainImages done!");

    return XR_SUCCESS;
}

XrResult mirageAcquireSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageAcquireInfo *acquireInfo, uint32_t *index){

    //TODO
    __android_log_print(ANDROID_LOG_ERROR, "PICOREUR2", "Unimplemented");

    XrSwapchainDescriptor* swapchainDescriptor = (XrSwapchainDescriptor*)swapchain;

    __android_log_print(ANDROID_LOG_WARN, "MIRAGE_BINDER PICOREUR", "SwapchainImaged : Acquiring image %d", swapchainDescriptor->clientTextureIds[*swapchainDescriptor->currentSwapchainIndexHandle]);



    *index = *swapchainDescriptor->currentSwapchainIndexHandle;
    *swapchainDescriptor->currentSwapchainIndexHandle = (*swapchainDescriptor->currentSwapchainIndexHandle + 1) % swapchainDescriptor->bufferCount;

    return XR_SUCCESS;
}

XrResult mirageWaitSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageWaitInfo *waitInfo){
    __android_log_print(ANDROID_LOG_ERROR, "PICOREUR2", "Unimplemented");

    return XR_SUCCESS;
}

XrResult mirageReleaseSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageReleaseInfo *releaseInfo){

    //TODO
    __android_log_print(ANDROID_LOG_ERROR, "PICOREUR2", "Unimplemented");

    //currentIndex--;

    return XR_SUCCESS;
}


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
