#pragma once

#include "XrInstanceDescriptor.h"
#include "XrReferenceSpaceDescriptor.h"

#define XR_SESSION_DESCRIPTOR_SIGNATURE 0x40558631ca921e7b

class XrInstanceDescriptor;
class XrReferenceSpaceDescriptor;

class XrSessionDescriptor {
public:

    XrSessionDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, XrInstanceDescriptor* instanceDescriptor, const XrSessionCreateInfo *createInfo);
    ~XrSessionDescriptor();

    uint64_t signature;
    bool created;
    XrInstanceDescriptor* instanceDescriptor;
    XrSessionCreateInfo* createInfo;
    XrSessionDescriptor* nextSessionDescriptor;
    XrReferenceSpaceType* referenceSpaces;
    uint32_t referenceSpacesCount;
    int64_t* swapchainFormats;
    uint32_t swapchainFormatsCount;
    XrReferenceSpaceDescriptor* firstReferenceSpaceDescriptor;
    //TODO : Add Graphics Bindings support for server
};
