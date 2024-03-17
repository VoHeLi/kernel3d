#include "XrActionDescriptor.h"
#include <new>
#include <string.h>

XrActionDescriptor::XrActionDescriptor(shared_memory_descriptor *sharedMemoryDescriptor,
                                       const XrActionCreateInfo *createInfo,
                                       XrActionDescriptor *parent) {
    this->signature = XR_ACTION_DESCRIPTOR_SIGNATURE;

    this->createInfo = NEW_SHARED(XrActionCreateInfo);

    this->createInfo->type = createInfo->type;
    this->createInfo->next = nullptr;
    strcpy(this->createInfo->actionName, createInfo->actionName);
    strcpy(this->createInfo->localizedActionName, createInfo->localizedActionName);
    this->createInfo->actionType = createInfo->actionType;

    this->createInfo->countSubactionPaths = createInfo->countSubactionPaths;
    this->createInfo->subactionPaths = NEW_SHARED(XrPath[createInfo->countSubactionPaths]);
    for(int i = 0; i < createInfo->countSubactionPaths; i++){
        ((XrPath*)this->createInfo->subactionPaths)[i] = createInfo->subactionPaths[i];
    }

    this->actionStateCount = createInfo->countSubactionPaths;
    this->actionState = NEW_SHARED(ActionState[createInfo->countSubactionPaths]);
    for(int i = 0; i < createInfo->countSubactionPaths; i++){
        this->actionState[i] = {};
    }

    this->nextActionDescriptor = nullptr;

    if(parent != nullptr){
        parent->nextActionDescriptor = this;
    }

    this->created = true;
}

XrActionDescriptor::~XrActionDescriptor() {

}
