#pragma once

#include "shared_memory_descriptor.h"
#include "openxr/openxr.h"

#define XR_ACTION_DESCRIPTOR_SIGNATURE 0xb207cbf69e1dc18e

union ActionState{
    XrActionStateBoolean boolean;
    XrActionStateFloat floatAction;
    XrActionStateVector2f vector2f;
    XrActionStatePose pose;
};

class XrActionDescriptor {
public:
    XrActionDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, const XrActionCreateInfo* createInfo, XrActionDescriptor* parent);
    ~XrActionDescriptor();

    uint64_t signature;
    bool created;
    XrActionDescriptor* nextActionDescriptor;
    XrActionCreateInfo* createInfo;
    uint32_t actionStateCount;
    ActionState* actionState;
};
