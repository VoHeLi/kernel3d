#include "XrInstanceDescriptor.h"
#include "shared_memory_descriptor.h"

#include <new>

XrInstanceDescriptor::XrInstanceDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, void* vm, void* clazz, const XrInstanceCreateInfo *createInfo) {
    signature = XR_INSTANCE_SIGNATURE;

    this->vm = vm;
    this->clazz = clazz;

    //DEEP COPY OF createInfo
    XrInstanceCreateInfo* c = NEW_SHARED(XrInstanceCreateInfo);
    c->type = createInfo->type;
    c->next = createInfo->next; //SHOULD NOT BE DEFINED IN OPENXR!
    c->createFlags = createInfo->createFlags;
    c->applicationInfo = createInfo->applicationInfo;
    c->enabledApiLayerCount = createInfo->enabledApiLayerCount;
    c->enabledExtensionCount = createInfo->enabledExtensionCount;

    char** enabledApiLayerNames = NEW_SHARED(char*[createInfo->enabledApiLayerCount]);
    for(int i = 0; i < createInfo->enabledApiLayerCount; i++) {
        enabledApiLayerNames[i] = NEW_SHARED(char[strlen(createInfo->enabledApiLayerNames[i])]);
        strcpy(enabledApiLayerNames[i], createInfo->enabledApiLayerNames[i]);
    }
    c->enabledApiLayerNames = enabledApiLayerNames;

    char** enabledExtensionNames = NEW_SHARED(char*[createInfo->enabledExtensionCount]);
    for(int i = 0; i < createInfo->enabledExtensionCount; i++) {
        enabledExtensionNames[i] = NEW_SHARED(char[strlen(createInfo->enabledExtensionNames[i])]);
        strcpy(enabledExtensionNames[i], createInfo->enabledExtensionNames[i]);
    }
    c->enabledExtensionNames = enabledExtensionNames;

    this->createInfo = c;

    this->systemIdDescriptor = nullptr;
    this->firstPathDescriptor = nullptr;
    this->firstSessionDescriptor = nullptr;
    this->firstActionSetDescriptor = nullptr;
    this->interactionProfileSuggestedBindings = nullptr;

    this->created = true;
}

XrInstanceDescriptor::~XrInstanceDescriptor() {

}