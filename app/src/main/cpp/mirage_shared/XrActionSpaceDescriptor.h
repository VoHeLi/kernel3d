#pragma once

#include "XrInstanceDescriptor.h"
#include "XrSessionDescriptor.h"

#define XR_ACTION_SPACE_DESCRIPTOR_SIGNATURE 0x14c91d8ce7e9b56d

class XrSessionDescriptor;


class XrActionSpaceDescriptor{
public:
    XrActionSpaceDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, XrSessionDescriptor* sessionDescriptor, const XrActionSpaceCreateInfo *createInfo);
    ~XrActionSpaceDescriptor();

    uint64_t signature;
    bool created;
    XrActionSpaceDescriptor* nextActionSpaceDescriptor;
    XrSessionDescriptor* sessionDescriptor;
    XrActionSpaceCreateInfo* createInfo;
};