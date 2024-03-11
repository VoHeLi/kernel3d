#pragma once

#include "XrInstanceDescriptor.h"
#include "XrSessionDescriptor.h"

#define XR_SWAPCHAIN_DESCRIPTOR_SIGNATURE 0xd76fb262e0c246ee

class XrSessionDescriptor;


class XrSwapchainDescriptor{
public:
    XrSwapchainDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, XrSessionDescriptor* sessionDescriptor, const XrSwapchainCreateInfo *createInfo, AHardwareBuffer* aHardwareBuffer);
    ~XrSwapchainDescriptor();

    uint64_t signature;
    bool created;
    XrSwapchainDescriptor* nextSwapchainDescriptor;
    XrSessionDescriptor* sessionDescriptor;
    XrSwapchainCreateInfo* createInfo;
    AHardwareBuffer* aHardwareBuffer;
};