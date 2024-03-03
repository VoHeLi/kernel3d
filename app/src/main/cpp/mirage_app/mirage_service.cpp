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

#include "mirage_utils.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

//#include "ashmem.h"

#define SOCKET_PATH "\0mirage_service_listener"

void mirageServer(){
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Initializing App Listener!");

    int fd = ASharedMemory_create("memory", 128);


    // By default it has PROT_READ | PROT_WRITE | PROT_EXEC.
    char *buffer = (char *) mmap(NULL, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    strcpy(buffer, "This is an example."); // trivially initialize content


    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Giving Info to FD : %d", fd);

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Initializing the server");

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);


    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Binding to %s", SOCKET_PATH);
    // Bind the socket to a file path
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "Binding error to %s", SOCKET_PATH);
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "ERRNO : %d", errno);
        exit(EXIT_FAILURE);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Trying to listen");
    // Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "Listening error");
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "ERRNO : %d", errno);
        exit(EXIT_FAILURE);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Server is listening");


    // Accepte la connexion entrante
    socklen_t client_len = sizeof(struct sockaddr_un);
    int client_fd;
    sockaddr_un client_addr;
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "Accept error");
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "ERRNO : %d", errno);
        exit(EXIT_FAILURE);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Connection accepted!");

    struct cmsghdr cheader = {
            .cmsg_len = CMSG_LEN(sizeof(fd)),
            .cmsg_level = SOL_SOCKET,
            .cmsg_type = SCM_RIGHTS,
    };

    struct msghdr header = {
        .msg_control = &cheader,
        .msg_controllen = sizeof(fd),
    };

    if (sendmsg(client_fd, &header, 0) == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "MIRAGE", "Sendmsg error");
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "ERRNO : %d", errno);
        exit(EXIT_FAILURE);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "MSG Sent!");

}

std::thread appThread;

extern "C"
JNIEXPORT void JNICALL
Java_com_androx_kernel3d_MirageService_mirageCreateAppListener(JNIEnv *env, jclass clazz) {

    appThread = std::thread(mirageServer);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_androx_kernel3d_MirageService_mirageInit(JNIEnv *env, jclass clazz) {
    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Initializing Service!");
}