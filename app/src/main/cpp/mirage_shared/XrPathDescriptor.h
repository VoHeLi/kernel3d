#pragma once

#include "shared_memory_descriptor.h"

class XrPathDescriptor {
public:
    XrPathDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, const char* pathString, XrPathDescriptor* parent);
    ~XrPathDescriptor();

    const char* pathString;
    XrPathDescriptor* nextPathDescriptor;
};
