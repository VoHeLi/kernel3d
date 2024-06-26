#include "mirage_app_server.h"
#include "mirage_shared/XrInstanceDescriptor.h"
#include <chrono>
#include <ctime>
#include <time.h>

#include "mirage_shared/common_types.h"

XrTime mirage_app_server::getCurrentTimeNanos(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (XrTime)(ts.tv_sec * 1000000000ULL + ts.tv_nsec);
}

mirage_app_server::mirage_app_server() {
    _isAccessible = false;
    _isInitialized = false;
    _serverInitializationThread = std::thread(&mirage_app_server::initializeServerThread, this);
    _appLayer = nullptr;
}

mirage_app_server::~mirage_app_server() {
    _isAccessible = false;
    _isInitialized = false;
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
            case cts_instruction::WAIT_FRAME:
                //Ping back
                __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Initialization ends, xrWaitFrame() called!");
                _lastUpdateTime = getCurrentTimeNanos();
                _frameIndex = 0;

                initControllers();

                _isInitialized = true;
                return;
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
            .maxSwapchainImageHeight = RESOLUTION, //TODO : MAKE IT ADAPTATIVE
            .maxSwapchainImageWidth = RESOLUTION, //TODO : MAKE IT ADAPTATIVE
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
    XrViewConfigurationView viewConfigurationViews[2] = {
    {
            .type = XR_TYPE_VIEW_CONFIGURATION_VIEW,
            .next = nullptr,
            .recommendedImageRectWidth = RESOLUTION,
            .maxImageRectWidth = RESOLUTION,
            .recommendedImageRectHeight = RESOLUTION,
            .maxImageRectHeight = RESOLUTION,
            .recommendedSwapchainSampleCount = 1,
            .maxSwapchainSampleCount = 1,
        },
    {
            .type = XR_TYPE_VIEW_CONFIGURATION_VIEW,
            .next = nullptr,
            .recommendedImageRectWidth = RESOLUTION,
            .maxImageRectWidth = RESOLUTION,
            .recommendedImageRectHeight = RESOLUTION,
            .maxImageRectHeight = RESOLUTION,
            .recommendedSwapchainSampleCount = 1,
            .maxSwapchainSampleCount = 1,
        }
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
               viewConfigurationViews, 2, &viewConfigurationProperties, //2 here means 2 views!!!
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
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "ALERT : MULTIPLE SESSIONS NOT SURE TO BE SUPPORTED YET!");
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
        .time = getCurrentTimeNanos(), //TODO : GET TIME?
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



    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Test");

    //TODO LINK TO GL TEXTURE
    int textureType = CTSM(swapchainDescriptor->createInfo, XrSwapchainCreateInfo*)->arraySize > 1 ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Test2");

    if(_appLayer == nullptr){
        _appLayer = new XrAppLayer(RESOLUTION, RESOLUTION, hardwareBuffers[0], hardwareBuffers[1], hardwareBuffers[2], textureType, count, CTSM(swapchainDescriptor->currentSwapchainIndexHandle, uint32_t*));

    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Test3");

    debugHardwareBuffer = hardwareBuffers[0]; //DEBUG

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Test4");
    //__android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Received hardware buffer : %p", hardwareBuffers[0]);

    //clear temp swapchain descriptor
    instanceDescriptor->tempSwapchainDescriptor = nullptr; //VERY IMPORTANT
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

void mirage_app_server::updateBegin(XrView* backviews, XrTime predictedDisplayTime, XrDuration predictedDisplayPeriod) {
    //We do the approximation of the time, TODO : USE THE REAL TIME FOR MORE ACCURACY
    XrTime currentTime = getCurrentTimeNanos();
    XrDuration deltaTime = currentTime - _lastUpdateTime;
    _lastUpdateTime = currentTime;

    //Get instance and then the first session and then the frameState
    XrInstanceDescriptor* instanceDescriptor = CTSM(sharedMemoryDescriptor->get_instance_ptr(), XrInstanceDescriptor*);
    XrSessionDescriptor* sessionDescriptor = CTSM(instanceDescriptor->firstSessionDescriptor, XrSessionDescriptor*);
    XrFrameState* frameState = CTSM(sessionDescriptor->waitFrameState, XrFrameState*);

    //We update the frame state
    frameState->predictedDisplayTime = predictedDisplayTime;
    frameState->predictedDisplayPeriod = predictedDisplayPeriod;
    frameState->shouldRender = _frameIndex > 5 ? XR_TRUE : XR_FALSE;

    //Send event to instance
    _frameIndex++;
    if(_frameIndex == 3)
    {
        XrEventDataSessionStateChanged sessionStateChanged = {
            .type = XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED,
            .next = nullptr,
            .session = (XrSession)STCM(sessionDescriptor, XrSessionDescriptor*),
            .state = XR_SESSION_STATE_SYNCHRONIZED,
            .time = currentTime,
        };

        instanceDescriptor->pushEvent(sharedMemoryDescriptor, (XrEventDataBuffer*)&sessionStateChanged);
    }
    else if(_frameIndex == 5)
    {
        XrEventDataSessionStateChanged sessionStateChanged = {
            .type = XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED,
            .next = nullptr,
            .session = (XrSession)STCM(sessionDescriptor, XrSessionDescriptor*),
            .state = XR_SESSION_STATE_VISIBLE,
            .time = currentTime,
        };

        instanceDescriptor->pushEvent(sharedMemoryDescriptor, (XrEventDataBuffer*)&sessionStateChanged);
    }
    else if(_frameIndex == 90) //TODO CHANGE
    {
        XrEventDataSessionStateChanged sessionStateChanged = {
                .type = XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED,
                .next = nullptr,
                .session = (XrSession)STCM(sessionDescriptor, XrSessionDescriptor*),
                .state = XR_SESSION_STATE_FOCUSED,
                .time = currentTime,
        };

        instanceDescriptor->pushEvent(sharedMemoryDescriptor, (XrEventDataBuffer*)&sessionStateChanged);
    }



    //DEBUG SET XRVIEW TO IDENTITY
    sessionDescriptor->viewCount = 2;
    XrView* views = CTSM(sessionDescriptor->views, XrView*);

    if(views == CTSM(nullptr, void*)){
        views = (XrView*)sharedMemoryDescriptor->memory_allocate(sizeof(XrView)*sessionDescriptor->viewCount);
        sessionDescriptor->views = STCM(views, XrView*);
    }


    for(int i = 0; i < sessionDescriptor->viewCount; i++){
        /*views[i].pose.orientation.w = 1;
        views[i].pose.orientation.x = 0;
        views[i].pose.orientation.y = 0;
        views[i].pose.orientation.z = 0;
        views[i].pose.position.x = 0;
        views[i].pose.position.y = 0;
        views[i].pose.position.z = 0;

        views[i].fov.angleLeft = -0.78539816339; //PI/4
        views[i].fov.angleRight = 0.78539816339; //PI/4
        views[i].fov.angleUp = 0.78539816339; //PI/4
        views[i].fov.angleDown = -0.78539816339;  //PI/4*/

        views[i] = backviews[i];
    }

    //Update controllers
    updateControllers();

    //We end the client xrWaitFrame()
    cts_instruction instruction = cts_instruction::WAIT_FRAME;
    send(_client_fd, &instruction, sizeof(cts_instruction), 0);
}

void mirage_app_server::updateEnd() {
    //We wait for the client to end xrWaitFrame()
    cts_instruction instruction = cts_instruction::NONE;
    recv(_client_fd, &instruction, sizeof(cts_instruction), 0);

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Received Instruction : %lu", instruction);

    //Try to receive additional messages without blocking TODO : BETTER WAY TO DO THIS
    while(recv(_client_fd, &instruction, sizeof(cts_instruction), MSG_DONTWAIT) > 0){
        __android_log_print(ANDROID_LOG_WARN, "MIRAGE", "Received Instruction EXTRA : %lu", instruction);
//
//        switch (instruction) {
//            case cts_instruction::WAIT_FRAME:
//                //We do nothing
//                break;
//            case cts_instruction::SHARE_SWAPCHAIN_AHARDWAREBUFFER:
//                receiveHardwareBufferFromClient();
//                send(_client_fd, &instruction, sizeof(cts_instruction), 0); // PING BACK
//                break;
//            default:
//                __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "Unknown instruction received : %d", instruction);
//                break;
//        }
    }



    switch (instruction) {
        case cts_instruction::WAIT_FRAME:
            //We do nothing
            break;
        case cts_instruction::SHARE_SWAPCHAIN_AHARDWAREBUFFER: //Was from a bug, should not be received here
            receiveHardwareBufferFromClient();
            send(_client_fd, &instruction, sizeof(cts_instruction), 0); // PING BACK
            updateEnd(); //RECURSIVE CALL
            break;
        default:
            __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "Unknown instruction received : %d", instruction);
            break;
    }

}


void mirage_app_server::initControllers() {
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Controller paths registering 1");

    XrInstanceDescriptor* instanceDescriptor = CTSM(sharedMemoryDescriptor->get_instance_ptr(), XrInstanceDescriptor*);
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Controller paths registering 2");

    XrPathDescriptor* pathDescriptor = CTSM(instanceDescriptor->firstPathDescriptor, XrPathDescriptor*);
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Controller paths registering 3");

    int counter = 1;

    while(pathDescriptor != CTSM(nullptr, void*) && pathDescriptor != nullptr) {

        TryRegisterControllerPath((XrPath)counter, CTSM(pathDescriptor->pathString, const char*));

        pathDescriptor = CTSM(pathDescriptor->nextPathDescriptor, XrPathDescriptor*);

        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Controller path registered : %d", counter);
        counter++;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Controller paths registered");

    XrSessionDescriptor* sessionDescriptor = CTSM(instanceDescriptor->firstSessionDescriptor, XrSessionDescriptor*);

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Controller test 2");

    //Actions & action sets
    XrActionSetDescriptor* actionSetDescriptor = CTSM(instanceDescriptor->firstActionSetDescriptor, XrActionSetDescriptor*);

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Controller test 3");

    uint64_t setCounter = 1;
    while(actionSetDescriptor != CTSM(nullptr, void*) && actionSetDescriptor != nullptr) {

        TryRegisterActionSet((XrActionSet)setCounter, CTSM(actionSetDescriptor->createInfo, XrActionSetCreateInfo*)->actionSetName);

        XrActionDescriptor* actionDescriptor = CTSM(actionSetDescriptor->firstActionDescriptor, XrActionDescriptor*);
        while(actionDescriptor != CTSM(nullptr, void*)) {
            __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Controller test 3.5");
            TryRegisterAction((XrAction)actionDescriptor, (XrActionSet)setCounter, CTSM(actionDescriptor->createInfo, XrActionCreateInfo*)->actionName);
            __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Controller test 3.6");


            actionDescriptor = CTSM(actionDescriptor->nextActionDescriptor, XrActionDescriptor*);
        }



        setCounter++;

        actionSetDescriptor = CTSM(actionSetDescriptor->nextActionSetDescriptor, XrActionSetDescriptor*);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Controller test 4");


    //Spaces
    XrActionSpaceDescriptor* actionSpaceDescriptor = CTSM(sessionDescriptor->firstActionSpaceDescriptor, XrActionSpaceDescriptor*);

    while(actionSpaceDescriptor != CTSM(nullptr, void*)) {
        TryRegisterControllerSpace((XrSpace)actionSpaceDescriptor, (XrAction)CTSM(actionSpaceDescriptor->createInfo, XrActionSpaceCreateInfo*)->action, (XrPath)CTSM(actionSpaceDescriptor->createInfo, XrActionSpaceCreateInfo*)->subactionPath);

        actionSpaceDescriptor = CTSM(actionSpaceDescriptor->nextActionSpaceDescriptor, XrActionSpaceDescriptor*);
    }

}

void mirage_app_server::updateControllers() {
    XrInstanceDescriptor* instanceDescriptor = CTSM(sharedMemoryDescriptor->get_instance_ptr(), XrInstanceDescriptor*);
    XrSessionDescriptor* sessionDescriptor = CTSM(instanceDescriptor->firstSessionDescriptor, XrSessionDescriptor*);

    //Iterate through action spaces
    XrActionSpaceDescriptor* actionSpaceDescriptor = CTSM(sessionDescriptor->firstActionSpaceDescriptor, XrActionSpaceDescriptor*);
    while(actionSpaceDescriptor != CTSM(nullptr, void*) && actionSpaceDescriptor != nullptr) {

        XrSpaceLocation location = {
            .type = XR_TYPE_SPACE_LOCATION,
            .next = nullptr,
            .locationFlags = XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_VALID_BIT | XR_SPACE_LOCATION_POSITION_TRACKED_BIT | XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT,
            .pose = {
                .orientation = {
                    .x = 0,
                    .y = 0,
                    .z = 0,
                    .w = 1,
                },
                .position = {
                    .x = 0,
                    .y = 2,
                    .z = 0,
                },
            },

        };
        GetControllerSpacePose(getCurrentTimeNanos(), (XrSpace)actionSpaceDescriptor, (XrSpace)CTSM(sessionDescriptor->firstReferenceSpaceDescriptor, XrReferenceSpaceDescriptor*), &location);
        actionSpaceDescriptor->currentPose = location.pose;

        actionSpaceDescriptor = CTSM(actionSpaceDescriptor->nextActionSpaceDescriptor, XrActionSpaceDescriptor*);
    }

    //Iterate though actions
    XrActionSetDescriptor* actionSetDescriptor = CTSM(instanceDescriptor->firstActionSetDescriptor, XrActionSetDescriptor*);
    while(actionSetDescriptor != CTSM(nullptr, void*) && actionSetDescriptor != nullptr) {
        XrActionDescriptor* actionDescriptor = CTSM(actionSetDescriptor->firstActionDescriptor, XrActionDescriptor*);
        while(actionDescriptor != CTSM(nullptr, void*)) {
            switch (CTSM(actionDescriptor->createInfo,XrActionCreateInfo*)->actionType) {
                case XR_ACTION_TYPE_BOOLEAN_INPUT: {
                    if(actionDescriptor->actionStateCount != 2) break;
                    ActionState* actionState = CTSM(actionDescriptor->actionState, ActionState*);

                    XrActionStateGetInfo getInfo = {
                        .type = XR_TYPE_ACTION_STATE_GET_INFO,
                        .next = nullptr,
                        .action = (XrAction)actionDescriptor,
                        .subactionPath = CTSM(CTSM(actionDescriptor->createInfo, XrActionCreateInfo*)->subactionPaths, XrPath*)[0],
                    };
                    GetControllerActionStateBoolean(&getInfo, &(actionState[0]).boolean);

                    XrActionStateGetInfo getInfo2 = {
                            .type = XR_TYPE_ACTION_STATE_GET_INFO,
                            .next = nullptr,
                            .action = (XrAction)actionDescriptor,
                            .subactionPath = CTSM(CTSM(actionDescriptor->createInfo, XrActionCreateInfo*)->subactionPaths, XrPath*)[1],
                    };
                    GetControllerActionStateBoolean(&getInfo2, &(actionState[1]).boolean);
                    break;
                }
                case XR_ACTION_TYPE_FLOAT_INPUT: {
                    if (actionDescriptor->actionStateCount != 2) break;
                    ActionState* actionState = CTSM(actionDescriptor->actionState, ActionState*);

                    XrActionStateGetInfo getInfo = {
                            .type = XR_TYPE_ACTION_STATE_GET_INFO,
                            .next = nullptr,
                            .action = (XrAction) actionDescriptor,
                            .subactionPath = CTSM(CTSM(actionDescriptor->createInfo,
                                                       XrActionCreateInfo * )->subactionPaths,
                                                  XrPath*)[0],
                    };
                    GetControllerActionStateFloat(&getInfo, &(actionState[0]).floatAction);

                    XrActionStateGetInfo getInfo2 = {
                            .type = XR_TYPE_ACTION_STATE_GET_INFO,
                            .next = nullptr,
                            .action = (XrAction) actionDescriptor,
                            .subactionPath = CTSM(CTSM(actionDescriptor->createInfo,
                                                       XrActionCreateInfo * )->subactionPaths,
                                                  XrPath*)[1],
                    };
                    GetControllerActionStateFloat(&getInfo2, &(actionState[1]).floatAction);
                    break;
                }
                case XR_ACTION_TYPE_VECTOR2F_INPUT: {
                    if (actionDescriptor->actionStateCount != 2) break;
                    ActionState* actionState = CTSM(actionDescriptor->actionState, ActionState*);

                    XrActionStateGetInfo getInfo = {
                            .type = XR_TYPE_ACTION_STATE_GET_INFO,
                            .next = nullptr,
                            .action = (XrAction) actionDescriptor,
                            .subactionPath = CTSM(CTSM(actionDescriptor->createInfo,
                                                       XrActionCreateInfo * )->subactionPaths,
                                                  XrPath*)[0],
                    };
                    GetControllerActionStateVector2f(&getInfo, &(actionState[0]).vector2f);

                    XrActionStateGetInfo getInfo2 = {
                            .type = XR_TYPE_ACTION_STATE_GET_INFO,
                            .next = nullptr,
                            .action = (XrAction) actionDescriptor,
                            .subactionPath = CTSM(CTSM(actionDescriptor->createInfo,
                                                       XrActionCreateInfo * )->subactionPaths,
                                                  XrPath*)[1],
                    };
                    GetControllerActionStateVector2f(&getInfo2, &(actionState[1]).vector2f);
                    break;
                }
                default:
                    break;
            }

            actionDescriptor = CTSM(actionDescriptor->nextActionDescriptor, XrActionDescriptor*);
        }

        actionSetDescriptor = CTSM(actionSetDescriptor->nextActionSetDescriptor, XrActionSetDescriptor*);
    }



}





