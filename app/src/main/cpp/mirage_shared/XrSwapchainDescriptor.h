#pragma once

#include "XrInstanceDescriptor.h"
#include "XrSessionDescriptor.h"

#define XR_SWAPCHAIN_DESCRIPTOR_SIGNATURE 0xd76fb26340c246ee

class XrSessionDescriptor;


class XrSwapchainDescriptor{
public:
    XrSwapchainDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, XrSessionDescriptor* sessionDescriptor, const XrSwapchainCreateInfo *createInfo, AHardwareBuffer** clientHardwareBuffers, GLuint* clientTextureIds);
    ~XrSwapchainDescriptor();

    uint64_t signature;
    bool created;
    XrSwapchainDescriptor* nextSwapchainDescriptor;
    XrSessionDescriptor* sessionDescriptor;
    XrSwapchainCreateInfo* createInfo;
    AHardwareBuffer** clientHardwareBuffers;
    AHardwareBuffer** serverHardwareBuffers; //THE ADDRESS IS DIRECTLY STORED IN THE SERVER VIRTUAL SPACE MEMORY
    GLuint* clientTextureIds;
    GLuint* serverTextureIds; //THE ADDRESS IS DIRECTLY STORED IN THE SERVER VIRTUAL SPACE MEMORY
    uint32_t bufferCount;
};