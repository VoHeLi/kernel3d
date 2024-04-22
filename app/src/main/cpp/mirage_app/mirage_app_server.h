
#ifndef ANDROX_KERNEL3D_MIRAGE_APP_SERVER_H
#define ANDROX_KERNEL3D_MIRAGE_APP_SERVER_H

#include "openxr/openxr.h"
#include <jni.h>

#include <android/log.h>

#include <android/sharedmem.h>
#include <android/sharedmem_jni.h>
#include <sys/mman.h>
#include <string>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/ashmem.h>
#include <errno.h>
#include <thread>


#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "mirage_shared/shared_memory_descriptor.h"
#include "engine/OpenXRPlugin.h"
#include "engine/XrAppLayer.h"

#include "controllers/controllers_binding.h"

#define SOCKET_PATH "\0mirage_service_listener" //TODO : Change this to a more unique name per app
#define STC_MEMORY_SIZE 1048576

#define RESOLUTION 1600

#define MIRAGE_VENDOR_ID 0xb2328062
#define SYSTEM_NAME "Mirage System"

class mirage_app_server {

public:
    mirage_app_server();
    ~mirage_app_server();

    XrInstance getInstance();
    bool isAccessible();

    void debugLog();

    void updateBegin(XrView* views, XrTime predictedDisplayTime, XrDuration predictedDisplayPeriod);

    void updateEnd();

    bool _isInitialized;
    XrAppLayer* _appLayer;
private:
    shared_memory_descriptor* sharedMemoryDescriptor;
    int _sharedMemoryFD;
    int _server_fd;
    int _client_fd;
    bool _isAccessible;
    std::thread _serverInitializationThread;
    std::thread _serverDestroyThread;
    XrTime _lastUpdateTime;
    uint64_t _frameIndex;

    //TODO : add frame_counter;

    XrTime getCurrentTimeNanos();
    void initializeServerThread();
    void destroyServerThread();
    void initializeSharedMemory();
    void initializeServerSocket();
    void handleClientInitialization();
    void sendFDToClient(int fd);
    void populateSystemProperties();
    void populateInitialSessionProperties();
    void receiveHardwareBufferFromClient();
    void initControllers();


    void updateControllers();
};




#endif
