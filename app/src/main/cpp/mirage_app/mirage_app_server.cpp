#include "mirage_app_server.h"


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
}

void mirage_app_server::destroyServerThread() {
    //TODO: Implement this
}

XrInstance mirage_app_server::getInstance() {
    //TODO: Implement this
    return nullptr;
}

bool mirage_app_server::isAccessible() {
    return _isAccessible;
}

void mirage_app_server::initializeSharedMemory() {
    _sharedMemoryFD = ASharedMemory_create("serverToClientMemory", STC_MEMORY_SIZE);

    _sharedMemoryPtr = (char *) mmap(NULL, STC_MEMORY_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, _sharedMemoryFD, 0);

    //DEBUG WRITE TO SHARED MEMORY
    strcpy(_sharedMemoryPtr, "This is an example."); //TODO : Remove this line

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




