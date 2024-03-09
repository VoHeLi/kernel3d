#pragma once

#include <stdint.h>
#include "XrInstanceDescriptor.h"
#include "openxr/openxr_platform.h"

#define XR_SYSTEMID_SIGNATURE 0xc7c1090589f5b693

class XrInstanceDescriptor;

class XrSystemIdDescriptor {

public:
    XrSystemIdDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, XrInstanceDescriptor* instanceDescriptorServerAddr, XrSystemProperties* systemPropertiesServerAddr, XrGraphicsRequirementsOpenGLESKHR* graphicsRequirementsOpenGleskhrServerAddr, XrViewConfigurationType* viewConfigurationTypes, int viewConfigurationsCount);
    ~XrSystemIdDescriptor();

    int64_t signature;
    bool created = false;
    XrInstanceDescriptor* instanceDescriptor;
    XrSystemProperties* systemProperties;
    XrGraphicsRequirementsOpenGLESKHR* graphicsRequirements;
    XrViewConfigurationType* viewConfigurations;
    uint64_t viewConfigurationsCount;
    //TODO MOVE INTO INSTANCE, IN REALITY XrSystemId is just an id
};


