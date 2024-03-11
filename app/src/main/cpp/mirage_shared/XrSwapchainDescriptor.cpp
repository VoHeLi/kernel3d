#include "XrSwapchainDescriptor.h"

#include <new>

XrSwapchainDescriptor::XrSwapchainDescriptor(shared_memory_descriptor *sharedMemoryDescriptor,
                                             XrSessionDescriptor *sessionDescriptor,
                                             const XrSwapchainCreateInfo *createInfo,
                                             AHardwareBuffer* aHardwareBuffer) {

    signature = XR_SWAPCHAIN_DESCRIPTOR_SIGNATURE;
    this->sessionDescriptor = sessionDescriptor;
    this->createInfo = NEW_SHARED(XrSwapchainCreateInfo);

    this->createInfo->type = createInfo->type;
    this->createInfo->next = nullptr;
    this->createInfo->createFlags = createInfo->createFlags;
    this->createInfo->usageFlags = createInfo->usageFlags;
    this->createInfo->format = createInfo->format;
    this->createInfo->sampleCount = createInfo->sampleCount;
    this->createInfo->width = createInfo->width;
    this->createInfo->height = createInfo->height;
    this->createInfo->faceCount = createInfo->faceCount;
    this->createInfo->arraySize = createInfo->arraySize;
    this->createInfo->mipCount = createInfo->mipCount;



    this->nextSwapchainDescriptor = nullptr;

    if(sessionDescriptor->firstSwapchainDescriptor == nullptr){
        sessionDescriptor->firstSwapchainDescriptor = this;
    } else {
        XrSwapchainDescriptor* currentReferenceSpaceDescriptor = sessionDescriptor->firstSwapchainDescriptor;
        while(currentReferenceSpaceDescriptor->nextSwapchainDescriptor != nullptr){
            currentReferenceSpaceDescriptor = currentReferenceSpaceDescriptor->nextSwapchainDescriptor;
        }
        currentReferenceSpaceDescriptor->nextSwapchainDescriptor = this;
    }

    this->aHardwareBuffer = aHardwareBuffer; //TODO CHANGE

    this->created = true;

}

XrSwapchainDescriptor::~XrSwapchainDescriptor() {


}
