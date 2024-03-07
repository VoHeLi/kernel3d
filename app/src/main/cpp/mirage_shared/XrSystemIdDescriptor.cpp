#include "XrSystemIdDescriptor.h"
#include "XrInstanceDescriptor.h"
#include <new>



XrSystemIdDescriptor::XrSystemIdDescriptor(shared_memory_descriptor *sharedMemoryDescriptor,
                                           XrInstanceDescriptor *instanceDescriptorServerAddr,
                                           XrSystemProperties *systemPropertiesServerAddr,
                                           XrGraphicsRequirementsOpenGLESKHR* graphicsRequirementsOpenGleskhrServerAddr) {
    signature = XR_SYSTEMID_SIGNATURE;

    this->instanceDescriptor = STCM(instanceDescriptorServerAddr, XrInstanceDescriptor*);
    instanceDescriptorServerAddr->systemIdDescriptor = STCM(this, XrSystemIdDescriptor*);

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "Populating system properties at address %p, server_addr : %p", instanceDescriptorServerAddr->systemIdDescriptor, this);

    //DEEP COPY OF systemProperties, next needs to be iterated until null
    XrSystemProperties* s = NEW_SHARED(XrSystemProperties);
    s->type = XR_TYPE_SYSTEM_PROPERTIES;
    s->next = nullptr;
    s->systemId = STCM(s, XrSystemId); //It should be more a value than a pointer, but we simplify here
    strcpy(s->systemName, systemPropertiesServerAddr->systemName);
    s->vendorId = systemPropertiesServerAddr->vendorId;

    s->graphicsProperties = systemPropertiesServerAddr->graphicsProperties;
    s->trackingProperties = systemPropertiesServerAddr->trackingProperties;

    this->systemProperties = STCM(s, XrSystemProperties*);

    //DEEP COPY OF graphicsRequirementsOpenGleskhrServerAddr
    XrGraphicsRequirementsOpenGLESKHR* g = NEW_SHARED(XrGraphicsRequirementsOpenGLESKHR);
    g->type = XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR;
    g->next = nullptr;
    g->minApiVersionSupported = graphicsRequirementsOpenGleskhrServerAddr->minApiVersionSupported;
    g->maxApiVersionSupported = graphicsRequirementsOpenGleskhrServerAddr->maxApiVersionSupported;

    this->graphicsRequirements = STCM(g, XrGraphicsRequirementsOpenGLESKHR*);

    created = true;
}

XrSystemIdDescriptor::~XrSystemIdDescriptor() {

}
