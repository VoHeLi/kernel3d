#pragma once

#include "../openxr/openxr.h"
#include "../openxr/openxr_platform_defines.h"
#include "../engine/gfxwrapper_opengl.h"
#include "shared_memory_descriptor.h"

#include <string.h>

//TODO : Change this to a more unique name per app
#define XR_INSTANCE_SIGNATURE 0xcba6c2e8da5f677b

class XrInstanceDescriptor {
public:
    XrInstanceDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, void* vm, void* clazz, const XrInstanceCreateInfo *createInfo);
    ~XrInstanceDescriptor();

    bool created = false;
    int64_t signature;
    void* vm;
    void* clazz;
    XrInstanceCreateInfo* createInfo;

};



