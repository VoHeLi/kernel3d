#include "mirage_app_server.h"
#include "mirage_shared/XrInstanceDescriptor.h"
#include <chrono>
#include <ctime>

#include "mirage_shared/common_types.h"

mirage_app_server::mirage_app_server() {
    _isAccessible = false;
    _serverInitializationThread = std::thread(&mirage_app_server::initializeServerThread, this);
}

mirage_app_server::~mirage_app_server() {
    _isAccessible = false;
    _serverDestroyThread = std::thread(&mirage_app_server::destroyServerThread, this);
}

void mirage_app_server::initializeServerThread() {

    initializeSharedMemory();
    initializeServerSocket();
    handleClientInitialization();
    sendFDToClient(_sharedMemoryFD);


    while(!_isInitialized){
        cts_instruction instruction = cts_instruction::NONE;
        if(recv(_client_fd, &instruction, sizeof(cts_instruction), 0) < 0){
            __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "Receiving error");
            __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "ERRNO : %d", errno);
            if(errno == 104){
                __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "The client app has been closed. Exiting server thread.");
                //TODO : DESTROY SERVER THREAD
                break;
            }
            exit(EXIT_FAILURE);
        }


        //__android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Received : %lu", instruction);

        switch(instruction){
            case cts_instruction::POPULATE_SYSTEM_PROPERTIES:
                populateSystemProperties();
                break;
            case cts_instruction::POPULATE_INITIAL_SESSION_PROPERTIES:
                populateInitialSessionProperties();
                break;
            case cts_instruction::SHARE_SWAPCHAIN_AHARDWAREBUFFER:
                receiveHardwareBufferFromClient();
                break;
            default:
                break;
        }

        //PING BACK
        send(_client_fd, &instruction, sizeof(cts_instruction), 0);
    }
}

void mirage_app_server::destroyServerThread() {
    //TODO: Implement this
}

XrInstance mirage_app_server::getInstance() {
    return (XrInstance)sharedMemoryDescriptor->get_instance_ptr();
}

bool mirage_app_server::isAccessible() {
    return _isAccessible;
}

void mirage_app_server::initializeSharedMemory() {
    _sharedMemoryFD = ASharedMemory_create("MirageSharedMemory", sizeof(shared_memory_descriptor));

    char* sharedMemoryPtr = (char *) mmap(NULL, sizeof(shared_memory_descriptor), PROT_WRITE | PROT_READ, MAP_SHARED, _sharedMemoryFD, 0);

    sharedMemoryDescriptor = new(sharedMemoryPtr) shared_memory_descriptor(); //(shared_memory_descriptor*) sharedMemoryPtr;
    sharedMemoryDescriptor->memory_init_server();

//    char* debugStr = (char*)_sharedMemoryDescriptor->memory_allocate(128);
//    strcpy(debugStr, "Hello from server!");

    //_sharedMemoryDescriptor->set_instance_ptr((void*)debugStr);

    //_sharedMemoryDescriptor->memory_free(debugStr);

    ASharedMemory_setProt(_sharedMemoryFD, PROT_READ | PROT_WRITE);
}

void mirage_app_server::initializeServerSocket() {
    _server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    if (bind(_server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "Binding error to %s", SOCKET_PATH);
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "ERRNO : %d", errno);
        exit(EXIT_FAILURE);
    }

    if (listen(_server_fd, 5) == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "Listening error");
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "ERRNO : %d", errno);
        exit(EXIT_FAILURE);
    }


}

void mirage_app_server::handleClientInitialization() {
    _client_fd = accept(_server_fd, NULL, NULL);
    if (_client_fd == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "Accepting error");
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "ERRNO : %d", errno);
        exit(EXIT_FAILURE);
    }

}

void mirage_app_server::sendFDToClient(int fd) {
    struct msghdr msg = {0};
    char buf[CMSG_SPACE(sizeof(int))], dup[256];
    memset(buf, 0, sizeof(buf));
    struct iovec io = { .iov_base = dup, .iov_len = sizeof(dup) };
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    *((int *) CMSG_DATA(cmsg)) = fd;
    msg.msg_controllen = cmsg->cmsg_len;
    if (sendmsg(_client_fd, &msg, 0) < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "Sending error");
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "ERRNO : %d", errno);
        exit(EXIT_FAILURE);
    }
}


void mirage_app_server::populateSystemProperties() {
    //DEBUG VALUES
    //TODO : MAKE IT ADAPTATIVE
    XrSystemProperties systemProperties = {
        .type = XR_TYPE_SYSTEM_PROPERTIES,
        .next = nullptr,
        .systemId = 0,
        .vendorId = MIRAGE_VENDOR_ID,
        .systemName = SYSTEM_NAME,
        .graphicsProperties = {
            .maxSwapchainImageHeight = 1600, //TODO : MAKE IT ADAPTATIVE
            .maxSwapchainImageWidth = 1600, //TODO : MAKE IT ADAPTATIVE
            .maxLayerCount = XR_MIN_COMPOSITION_LAYERS_SUPPORTED,
        },
        .trackingProperties = {
            .orientationTracking = XR_TRUE,
            .positionTracking = XR_TRUE,
        },
    };


    XrGraphicsRequirementsOpenGLESKHR graphicsRequirementsOpenGLESKHR = {
        .type = XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR,
        .next = nullptr,
        .minApiVersionSupported = XR_MAKE_VERSION(2,0,0), //TODO CHANGE
        .maxApiVersionSupported = XR_MAKE_VERSION(3,2,0), //TODO CHANGE
    };



    XrViewConfigurationType viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    int viewConfigurationsCount = 1;

    //Fill views
    //Lynx values
    XrViewConfigurationView viewConfigurationView = {
        .type = XR_TYPE_VIEW_CONFIGURATION_VIEW,
        .next = nullptr,
        .recommendedImageRectWidth = 1600,
        .maxImageRectWidth = 1600,
        .recommendedImageRectHeight = 1600,
        .maxImageRectHeight = 1600,
        .recommendedSwapchainSampleCount = 1,
        .maxSwapchainSampleCount = 1,
    };

    XrViewConfigurationProperties viewConfigurationProperties = {
        .type = XR_TYPE_VIEW_CONFIGURATION_PROPERTIES,
        .next = nullptr,
        .viewConfigurationType = viewConfigurationType,
        .fovMutable = XR_FALSE, //Should not be changed
    };

    XrEnvironmentBlendMode environmentBlendModes[] = {XR_ENVIRONMENT_BLEND_MODE_OPAQUE, XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND}; //Video passthrough


    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Populating system properties");

    NEW_SHARED(XrSystemIdDescriptor, sharedMemoryDescriptor, CTSM(sharedMemoryDescriptor->get_instance_ptr(), XrInstanceDescriptor*),
               &systemProperties, &graphicsRequirementsOpenGLESKHR, &viewConfigurationType, 1,
               &viewConfigurationView, 1, &viewConfigurationProperties,
               environmentBlendModes, 2);


}


void mirage_app_server::populateInitialSessionProperties() {
    //Get instance
    XrInstanceDescriptor* instanceDescriptor = CTSM(sharedMemoryDescriptor->get_instance_ptr(), XrInstanceDescriptor*);

    //Check if not null
    if(instanceDescriptor == CTSM(nullptr, void*)) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "Instance is null");
        return;
    }

    //Get session descriptor from instance
    XrSessionDescriptor* sessionDescriptor = CTSM(instanceDescriptor->firstSessionDescriptor, XrSessionDescriptor*);

    //Check if not null
    if(sessionDescriptor == CTSM(nullptr, void*)) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "Session is null");
        return;
    }

    //Get the child until null to fill last session
    while(CTSM(sessionDescriptor->nextSessionDescriptor, XrSessionDescriptor*) != CTSM(nullptr, void*)) {
        sessionDescriptor = CTSM(sessionDescriptor->nextSessionDescriptor, XrSessionDescriptor*);
    }

    //Fill reference spaces
    XrReferenceSpaceType* referenceSpaceType = NEW_SHARED(XrReferenceSpaceType);
    *referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    sessionDescriptor->referenceSpaces = STCM(referenceSpaceType, XrReferenceSpaceType*);
    sessionDescriptor->referenceSpacesCount = 1;

    //Fill swapchain formats


    //TODO Use realtime formats
    //[32856] 35907 33189 35056 36012;
    int64_t* swapchainFormats = static_cast<int64_t *>(sharedMemoryDescriptor->memory_allocate(
            5 * sizeof(int64_t)));
    swapchainFormats[0] = 32856; //0x8058 //RGBA8_EXT
    swapchainFormats[1] = 35907; //0x8C43 //SRGB8_ALPHA8_EXT
    swapchainFormats[2] = 33189; //0x81A5 //GL_DEPTH_COMPONENT16
    swapchainFormats[3] = 35056; //0x88F0 //GL_DEPTH24_STENCIL8
    swapchainFormats[4] = 36012; //0x8CAC //GL_DEPTH_COMPONENT32F

    sessionDescriptor->swapchainFormats = STCM(swapchainFormats, int64_t*);
    sessionDescriptor->swapchainFormatsCount = 5;

    //Push sessionReady event to instance
    XrEventDataSessionStateChanged sessionStateChanged = {
        .type = XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED,
        .next = nullptr,
        .session = (XrSession)STCM(sessionDescriptor, XrSessionDescriptor*),
        .state = XR_SESSION_STATE_READY,
        .time = 0,
    };

    instanceDescriptor->pushEvent(sharedMemoryDescriptor, (XrEventDataBuffer*)&sessionStateChanged);

}


extern AHardwareBuffer* debugHardwareBuffer;

void mirage_app_server::receiveHardwareBufferFromClient() {

    XrInstanceDescriptor* instanceDescriptor = CTSM(sharedMemoryDescriptor->get_instance_ptr(), XrInstanceDescriptor*);

    if(instanceDescriptor == CTSM(nullptr, void*)) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "Instance is null");
        return;
    }

    XrSwapchainDescriptor* swapchainDescriptor = CTSM(instanceDescriptor->tempSwapchainDescriptor, XrSwapchainDescriptor*);
    int count = swapchainDescriptor->bufferCount;

    AHardwareBuffer** hardwareBuffers = (AHardwareBuffer**)sharedMemoryDescriptor->memory_allocate(count * sizeof(AHardwareBuffer*));

    for(int i = 0; i < count; i++){
        //PING TO SAY READY
        cts_instruction instruction = cts_instruction::SHARE_SWAPCHAIN_AHARDWAREBUFFER_READY;
        send(_client_fd, &instruction, sizeof(cts_instruction), 0);

        //RECEIVE HARDWARE BUFFER
        AHardwareBuffer* hardwareBuffer = nullptr;
        AHardwareBuffer_recvHandleFromUnixSocket(_client_fd, &hardwareBuffer);
        hardwareBuffers[i] = hardwareBuffer;

        //DEBUG : PRINT DESC TODO REMOVE
        AHardwareBuffer_Desc desc;
        AHardwareBuffer_describe(hardwareBuffer, &desc);

        __android_log_print(ANDROID_LOG_WARN, "MIRAGE", "Received hardware buffer desc : %d, %d", desc.width, desc.height);
    }

    swapchainDescriptor->serverHardwareBuffers = hardwareBuffers;

    //clear temp swapchain descriptor
    instanceDescriptor->tempSwapchainDescriptor = nullptr; //VERY IMPORTANT

    //TODO LINK TO GL TEXTURE

    debugHardwareBuffer = hardwareBuffers[0]; //DEBUG
    //__android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Received hardware buffer : %p", hardwareBuffers[0]);
}


double lastTime = 0;

void mirage_app_server::debugLog() {
    auto currentTime = std::chrono::high_resolution_clock::now();

    // Convertir le temps actuel en secondes sous forme de valeur flottante
    std::chrono::duration<double> duration = currentTime.time_since_epoch();
    double seconds = duration.count();

    if(seconds - lastTime < 5) {
        return;
    }
    lastTime = seconds;

    XrInstanceDescriptor* instanceDescriptor = CTSM(sharedMemoryDescriptor->get_instance_ptr(), XrInstanceDescriptor*);

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_UPDATE", "Getting info on instance : %p, shared memory loc : %p", instanceDescriptor, sharedMemoryDescriptor);

    if(instanceDescriptor == CTSM(nullptr, void*)) {
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_UPDATE", "Instance info : NULL");
        return;
    }


    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_UPDATE", "Instance info : %s", CTSM(CTSM(CTSM(instanceDescriptor->createInfo, XrInstanceCreateInfo*)->enabledExtensionNames, char**)[0], char*));

    if(instanceDescriptor->firstPathDescriptor == CTSM(nullptr, void*)) {
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_UPDATE", "Path info : NULL");
        return;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_UPDATE", "Path info : %p", instanceDescriptor->firstPathDescriptor);

    XrPathDescriptor* pathDescriptor = CTSM(instanceDescriptor->firstPathDescriptor, XrPathDescriptor*);

    while(pathDescriptor != CTSM(nullptr, void*)) {
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_UPDATE", "Path info : %s", CTSM(pathDescriptor->pathString, const char*));
        pathDescriptor = CTSM(pathDescriptor->nextPathDescriptor, XrPathDescriptor*);
    }
    /*XrPathDescriptor* pathDescriptor = CTSM(instanceDescriptor->firstPathDescriptor, XrPathDescriptor*);

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_UPDATE", "Path info : %s", CTSM(pathDescriptor->pathString, const char*));*/

    //Print action sets
    XrActionSetDescriptor* actionSetDescriptor = CTSM(instanceDescriptor->firstActionSetDescriptor, XrActionSetDescriptor*);

    while(actionSetDescriptor != CTSM(nullptr, void*)) {
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_UPDATE", "Action set info : %s", CTSM(actionSetDescriptor->createInfo, XrActionSetCreateInfo*)->actionSetName); //NO CTSM BECAUSE CHAR ARRAY IN STRUCT !!!

        //Print actions
        XrActionDescriptor* actionDescriptor = CTSM(actionSetDescriptor->firstActionDescriptor, XrActionDescriptor*);

        while(actionDescriptor != CTSM(nullptr, void*)) {
            __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_UPDATE", "---> Action info : %s", CTSM(actionDescriptor->createInfo, XrActionCreateInfo*)->actionName); //NO CTSM BECAUSE CHAR ARRAY IN STRUCT !!!
            actionDescriptor = CTSM(actionDescriptor->nextActionDescriptor, XrActionDescriptor*);
        }


        actionSetDescriptor = CTSM(actionSetDescriptor->nextActionSetDescriptor, XrActionSetDescriptor*);
    }



    XrSessionDescriptor* sessionDescriptor = CTSM(instanceDescriptor->firstSessionDescriptor, XrSessionDescriptor*);

    if(sessionDescriptor == CTSM(nullptr, void*)) {
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_UPDATE", "Session info : NULL");
        return;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_UPDATE", "Session OpenGL ES Config ptr info (debug only) : %p", CTSM(CTSM(sessionDescriptor->createInfo, XrSessionCreateInfo*)->next, XrGraphicsBindingOpenGLESAndroidKHR*)->config);

    //Get reference space
    XrReferenceSpaceDescriptor* referenceSpaceDescriptor = CTSM(sessionDescriptor->firstReferenceSpaceDescriptor, XrReferenceSpaceDescriptor*);

    if(referenceSpaceDescriptor == CTSM(nullptr, void*)) {
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_UPDATE", "Reference space info : NULL");
        return;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE_UPDATE", "Reference space info : %f", CTSM(referenceSpaceDescriptor->createInfo, XrReferenceSpaceCreateInfo*)->poseInReferenceSpace.orientation.w);
}








