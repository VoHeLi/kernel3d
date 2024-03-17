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
#include "mirage_app_server.h"
#include <vector>

#include "mirage_service.h"


std::vector<mirage_app_server*> servers;

extern "C"
JNIEXPORT void JNICALL
Java_com_androx_kernel3d_Kernel3DLauncher_mirageCreateAppListener(JNIEnv *env, jclass clazz) {
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Creating App Listener!");
    mirage_app_server* server = new mirage_app_server();
    servers.push_back(server);

}
extern "C"
JNIEXPORT void JNICALL
Java_com_androx_kernel3d_Kernel3DLauncher_mirageInit(JNIEnv *env, jclass clazz) {

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Initializing Service!");
}


JNIEXPORT void JNICALL
engineUpdateTest(std::vector<XrAppLayer*>* appLayers, XrView* views, XrTime predictedDisplayTime, XrDuration predictedDisplayPeriod) {

//    for(int i = 0; i < servers.size(); i++){
//        servers[i]->debugLog();
//    }

    //TODO PASS IF AN APP IS TOO LONG TO UPDATE, WE SKIP IT UNTIL NEXT FRAME

    //We make a copy, so if begin and end are always called together
    bool* running = (bool*)malloc(sizeof(bool)*servers.size());
    for(int i = 0; i < servers.size(); i++){
        running[i] = servers[i]->_isInitialized;
    }


    for(int i = 0; i < servers.size(); i++){
        if (running[i]) {
            servers[i]->updateBegin(views, predictedDisplayTime, predictedDisplayPeriod);
        }
    }

    for(int i = 0; i < servers.size(); i++){
        if (running[i]) {
            servers[i]->updateEnd();
        }
    }

    free(running);

    if(appLayers == nullptr){
        appLayers = new std::vector<XrAppLayer*>();
    }

    *appLayers = std::vector<XrAppLayer*>();
    for(int i = 0; i < servers.size(); i++){
        if(servers[i]->_appLayer != nullptr)
            (*appLayers).push_back(servers[i]->_appLayer);
    }


}