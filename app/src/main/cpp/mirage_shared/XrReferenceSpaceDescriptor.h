#pragma once

#include "XrInstanceDescriptor.h"
#include "XrSessionDescriptor.h"

#define XR_REFERENCE_SPACE_DESCRIPTOR_SIGNATURE 0xcecbedd5966517ad

class XrSessionDescriptor;


class XrReferenceSpaceDescriptor{
public:
    XrReferenceSpaceDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, XrSessionDescriptor* sessionDescriptor, const XrReferenceSpaceCreateInfo *createInfo);
    ~XrReferenceSpaceDescriptor();

    uint64_t signature;
    bool created;
    XrReferenceSpaceDescriptor* nextReferenceSpaceDescriptor;
    XrSessionDescriptor* sessionDescriptor;
    XrReferenceSpaceCreateInfo* createInfo;
};