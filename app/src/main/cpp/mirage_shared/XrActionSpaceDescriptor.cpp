#include "XrActionSpaceDescriptor.h"

#include <new>

XrActionSpaceDescriptor::XrActionSpaceDescriptor(shared_memory_descriptor *sharedMemoryDescriptor,
                                                 XrSessionDescriptor *sessionDescriptor,
                                                 const XrActionSpaceCreateInfo *createInfo) {

    signature = XR_ACTION_SPACE_DESCRIPTOR_SIGNATURE;
    this->sessionDescriptor = sessionDescriptor;
    this->createInfo = NEW_SHARED(XrActionSpaceCreateInfo);

    this->createInfo->type = createInfo->type;
    this->createInfo->next = nullptr;
    this->createInfo->action = createInfo->action;
    this->createInfo->subactionPath = createInfo->subactionPath;
    this->createInfo->poseInActionSpace = createInfo->poseInActionSpace;

    this->nextActionSpaceDescriptor = nullptr;

    if(sessionDescriptor->firstActionSpaceDescriptor == nullptr){
        sessionDescriptor->firstActionSpaceDescriptor = this;
    } else {
        XrActionSpaceDescriptor* currentReferenceSpaceDescriptor = sessionDescriptor->firstActionSpaceDescriptor;
        while(currentReferenceSpaceDescriptor->nextActionSpaceDescriptor != nullptr){
            currentReferenceSpaceDescriptor = currentReferenceSpaceDescriptor->nextActionSpaceDescriptor;
        }
        currentReferenceSpaceDescriptor->nextActionSpaceDescriptor = this;
    }

    this->created = true;

}

XrActionSpaceDescriptor::~XrActionSpaceDescriptor() {

}
