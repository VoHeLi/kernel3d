#include "XrSystemIdDescriptor.h"
#include "XrInstanceDescriptor.h"
#include <new>



XrSystemIdDescriptor::XrSystemIdDescriptor(shared_memory_descriptor *sharedMemoryDescriptor,
                                           XrInstanceDescriptor *instanceDescriptorServerAddr,
                                           XrSystemProperties *systemPropertiesServerAddr,
                                           XrGraphicsRequirementsOpenGLESKHR* graphicsRequirementsOpenGleskhrServerAddr,
                                           XrViewConfigurationType* viewConfigurationTypes, int viewConfigurationsCount,
                                           XrViewConfigurationView* views, int viewsCount,
                                           XrViewConfigurationProperties* viewConfigurationProperties,
                                           XrEnvironmentBlendMode* environmentBlendModes, int environmentBlendModesCount){
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

    //--------------------------------------------------------------------------------
    XrGraphicsRequirementsOpenGLESKHR* g = NEW_SHARED(XrGraphicsRequirementsOpenGLESKHR);
    g->type = XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR;
    g->next = nullptr;
    g->minApiVersionSupported = graphicsRequirementsOpenGleskhrServerAddr->minApiVersionSupported;
    g->maxApiVersionSupported = graphicsRequirementsOpenGleskhrServerAddr->maxApiVersionSupported;

    this->graphicsRequirements = STCM(g, XrGraphicsRequirementsOpenGLESKHR*);

    //--------------------------------------------------------------------------------
    XrViewConfigurationType* configurationType = static_cast<XrViewConfigurationType *>(sharedMemoryDescriptor->memory_allocate(
            viewConfigurationsCount * sizeof(XrViewConfigurationType)));
    memcpy(configurationType, viewConfigurationTypes, viewConfigurationsCount * sizeof(XrViewConfigurationType));

    this->viewConfigurations = STCM(configurationType, XrViewConfigurationType*);
    this->viewConfigurationsCount = viewConfigurationsCount;

    //--------------------------------------------------------------------------------
    XrViewConfigurationView* view = static_cast<XrViewConfigurationView *>(sharedMemoryDescriptor->memory_allocate(
            viewsCount * sizeof(XrViewConfigurationView)));
    memcpy(view, views, viewsCount * sizeof(XrViewConfigurationView));
    this->views = STCM(view, XrViewConfigurationView*);
    this->viewsCount = viewsCount;

    //--------------------------------------------------------------------------------
    XrViewConfigurationProperties* vcp = NEW_SHARED(XrViewConfigurationProperties); //WHY *** I DID THIS MISTAKE, KILL ME, LOOK AT THE DIFF WITH THE PREVIOUS COMMIT!!!
    vcp->type = XR_TYPE_VIEW_CONFIGURATION_PROPERTIES;
    vcp->next = nullptr;
    vcp->viewConfigurationType = viewConfigurationProperties->viewConfigurationType;
    vcp->fovMutable = viewConfigurationProperties->fovMutable;

    this->viewConfigurationProperties = STCM(vcp, XrViewConfigurationProperties*);

    //--------------------------------------------------------------------------------
    XrEnvironmentBlendMode* ebm = static_cast<XrEnvironmentBlendMode *>(sharedMemoryDescriptor->memory_allocate(
            environmentBlendModesCount * sizeof(XrEnvironmentBlendMode)));
    memcpy(ebm, environmentBlendModes, environmentBlendModesCount * sizeof(XrEnvironmentBlendMode));
    this->environmentBlendModes = STCM(ebm, XrEnvironmentBlendMode*);
    this->environmentBlendModesCount = environmentBlendModesCount;

    created = true;
}

XrSystemIdDescriptor::~XrSystemIdDescriptor() {

}
