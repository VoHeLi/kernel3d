#include "XrActionSetDescriptor.h"
#include <new>
#include <string.h>

XrActionSetDescriptor::XrActionSetDescriptor(shared_memory_descriptor *sharedMemoryDescriptor,
                                             const XrActionSetCreateInfo* createInfo,
                                             XrActionSetDescriptor *parent) {


    signature = XR_ACTION_SET_DESCRIPTOR_SIGNATURE;

    this->createInfo = NEW_SHARED(XrActionSetCreateInfo);

    this->createInfo->type = createInfo->type;
    this->createInfo->next = nullptr;
    strcpy(this->createInfo->actionSetName, createInfo->actionSetName);
    strcpy(this->createInfo->localizedActionSetName, createInfo->localizedActionSetName);
    this->createInfo->priority = createInfo->priority;

    this->nextActionSetDescriptor = nullptr;

    if(parent != nullptr){
        parent->nextActionSetDescriptor = this;
    }

    this->firstActionDescriptor = nullptr;

    this->created = true;
}


XrActionSetDescriptor::~XrActionSetDescriptor() {

}

