
#include "XrReferenceSpaceDescriptor.h"

#include <new>

XrReferenceSpaceDescriptor::XrReferenceSpaceDescriptor(shared_memory_descriptor *sharedMemoryDescriptor,
                                                       XrSessionDescriptor *sessionDescriptor,
                                                       const XrReferenceSpaceCreateInfo *createInfo) {

    signature = XR_REFERENCE_SPACE_DESCRIPTOR_SIGNATURE;
    this->sessionDescriptor = sessionDescriptor;
    this->createInfo = NEW_SHARED(XrReferenceSpaceCreateInfo);

    this->createInfo->type = createInfo->type;
    this->createInfo->poseInReferenceSpace = createInfo->poseInReferenceSpace;
    this->createInfo->referenceSpaceType = createInfo->referenceSpaceType;
    this->createInfo->next = nullptr;

    this->nextReferenceSpaceDescriptor = nullptr;

    if(sessionDescriptor->firstReferenceSpaceDescriptor == nullptr){
        sessionDescriptor->firstReferenceSpaceDescriptor = this;
    } else {
        XrReferenceSpaceDescriptor* currentReferenceSpaceDescriptor = sessionDescriptor->firstReferenceSpaceDescriptor;
        while(currentReferenceSpaceDescriptor->nextReferenceSpaceDescriptor != nullptr){
            currentReferenceSpaceDescriptor = currentReferenceSpaceDescriptor->nextReferenceSpaceDescriptor;
        }
        currentReferenceSpaceDescriptor->nextReferenceSpaceDescriptor = this;
    }

}

XrReferenceSpaceDescriptor::~XrReferenceSpaceDescriptor() {

}
