#pragma once

#include "../openxr/openxr.h"
#include "../openxr/openxr_platform_defines.h"
#include "../engine/gfxwrapper_opengl.h"
#include "shared_memory_descriptor.h"
#include "XrSystemIdDescriptor.h"
#include "XrPathDescriptor.h"
#include "XrSessionDescriptor.h"

#include <string.h>

//TODO : Change this to a more unique name per app
#define XR_INSTANCE_SIGNATURE 0xcba6c2e8da5f677b

class XrSystemIdDescriptor;
class XrSessionDescriptor;

class XrInstanceDescriptor {
public:
    XrInstanceDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, void* vmClientAddr, void* clazClientAddr, const XrInstanceCreateInfo *createInfoClientAddr);
    ~XrInstanceDescriptor();

    int64_t signature;
    bool created = false;
    void* vm;
    void* clazz;
    XrInstanceCreateInfo* createInfo;
    XrSystemIdDescriptor* systemIdDescriptor;
    XrPathDescriptor* firstPathDescriptor;
    XrSessionDescriptor* firstSessionDescriptor;


};



