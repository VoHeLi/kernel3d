#pragma once

#include "shared_memory_descriptor.h"
#include "openxr/openxr.h"
#include "XrActionDescriptor.h"

#define XR_ACTION_SET_DESCRIPTOR_SIGNATURE 0x9419d8a669879900

class XrInstanceDescriptor;

class XrActionSetDescriptor {
public:
    XrActionSetDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, const XrActionSetCreateInfo* createInfo, XrActionSetDescriptor* parent);
    ~XrActionSetDescriptor();

    uint64_t signature;
    bool created;
    XrActionSetDescriptor* nextActionSetDescriptor;
    XrActionSetCreateInfo* createInfo;
    XrActionDescriptor* firstActionDescriptor;
};
