#include "XrSessionDescriptor.h"

#include <new>

XrSessionDescriptor::XrSessionDescriptor(shared_memory_descriptor *sharedMemoryDescriptor,
                                         XrInstanceDescriptor *instanceDescriptor,
                                         const XrSessionCreateInfo *createInfo) {
    signature = XR_SESSION_DESCRIPTOR_SIGNATURE;

    this->instanceDescriptor = instanceDescriptor;

    this->createInfo = NEW_SHARED(XrSessionCreateInfo);

    this->createInfo->type = createInfo->type;
    this->createInfo->createFlags = createInfo->createFlags;
    this->createInfo->systemId = createInfo->systemId;

    //TODO: NOTE THAT Graphics Bindings may not be accessible as is in the server
    if(createInfo->next != nullptr){ //WE SUPPORT OPENGL ES ANDROID ONLY
        XrGraphicsBindingOpenGLESAndroidKHR* graphicsBindings = NEW_SHARED(XrGraphicsBindingOpenGLESAndroidKHR);
        XrGraphicsBindingOpenGLESAndroidKHR* appGraphicsBindings = (XrGraphicsBindingOpenGLESAndroidKHR*)createInfo->next;

        graphicsBindings->type = appGraphicsBindings->type;
        graphicsBindings->next = nullptr;
        graphicsBindings->display = appGraphicsBindings->display;
        graphicsBindings->config = appGraphicsBindings->config;
        graphicsBindings->context = appGraphicsBindings->context;

        this->createInfo->next = graphicsBindings;
    } else {
        this->createInfo->next = nullptr;
    }

    this->nextSessionDescriptor = nullptr;

    if(instanceDescriptor->firstSessionDescriptor == nullptr){
        instanceDescriptor->firstSessionDescriptor = this;
    } else {
        XrSessionDescriptor* currentSessionDescriptor = instanceDescriptor->firstSessionDescriptor;
        while(currentSessionDescriptor->nextSessionDescriptor != nullptr){
            currentSessionDescriptor = currentSessionDescriptor->nextSessionDescriptor;
        }
        currentSessionDescriptor->nextSessionDescriptor = this;
    }

    this->referenceSpaces = nullptr;
    this->referenceSpacesCount = 0;
    this->swapchainFormats = nullptr;
    this->swapchainFormatsCount = 0;

    this->firstReferenceSpaceDescriptor = nullptr;
    this->primaryViewConfigurationType = (XrViewConfigurationType)0; //UNINITIALIZED

    this->countActionSets = 0;
    this->actionSets = nullptr;

    this->firstActionSpaceDescriptor = nullptr;
    this->firstSwapchainDescriptor = nullptr;

    this->created = true;
}

XrSessionDescriptor::~XrSessionDescriptor() {

}
