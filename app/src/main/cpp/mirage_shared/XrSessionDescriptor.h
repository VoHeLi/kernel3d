#pragma once

#include "XrInstanceDescriptor.h"

#define XR_SESSION_DESCRIPTOR_SIGNATURE 0x40558631ca921e7b

class XrInstanceDescriptor;

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
    uint64_t referenceSpacesCount;
    //TODO : Add Graphics Bindings support for server
};
