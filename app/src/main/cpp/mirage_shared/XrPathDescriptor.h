#pragma once

#include "shared_memory_descriptor.h"

#define XR_PATH_DESCRIPTOR_SIGNATURE 0x82727e3fe83b708d

class XrPathDescriptor {
public:
    XrPathDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, const char* pathString, XrPathDescriptor* parent);
    ~XrPathDescriptor();

    uint64_t signature;
    bool created;
    const char* pathString;
    XrPathDescriptor* nextPathDescriptor;
};
