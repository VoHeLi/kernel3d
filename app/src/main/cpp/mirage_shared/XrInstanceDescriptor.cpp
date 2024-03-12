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

    this->tempSwapchainDescriptor = nullptr;
    this->firstEventNode = nullptr;


    this->created = true;
}

XrInstanceDescriptor::~XrInstanceDescriptor() {

}


void XrInstanceDescriptor::pushEvent(shared_memory_descriptor* sharedMemoryDescriptor, XrEventDataBuffer *event) {



    XrEventNode* node = NEW_SHARED(XrEventNode);

    //DEEP COPY OF event
    XrEventDataBuffer* e = NEW_SHARED(XrEventDataBuffer);
    *e = *event;

    node->event = STCM(e, XrEventDataBuffer*);
    node->next = nullptr;

    __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "pushEvent: %d", ((XrEventDataSessionStateChanged*)event)->state);

    if(firstEventNode == nullptr || firstEventNode == CTSM(nullptr, void*)){
        firstEventNode = STCM(node, XrEventNode*);
        __android_log_print(ANDROID_LOG_DEBUG, "MIRAGE", "firstEventNode: %d", CTSM(CTSM(firstEventNode, XrEventNode*)->event,XrEventDataSessionStateChanged*)->state);

    } else {
        XrEventNode* currentEventNode = CTSM(firstEventNode, XrEventNode*);
        while(currentEventNode->next != CTSM(nullptr, void*)){
            currentEventNode = CTSM(currentEventNode->next, XrEventNode*);
        }
        currentEventNode->next = STCM(node, XrEventNode*);
    }

}

XrEventDataBuffer *XrInstanceDescriptor::popEvent(shared_memory_descriptor* sharedMemoryDescriptor) {
    //NO CTSM, STCM here, it's from the client




    if(firstEventNode == nullptr){
        return nullptr;
    }

    if(firstEventNode->next == nullptr){
        XrEventDataBuffer* event = firstEventNode->event;

        sharedMemoryDescriptor->memory_free(firstEventNode);

        firstEventNode = nullptr;
        return event;
    }

    XrEventNode* node = firstEventNode;
    while(node->next->next != nullptr){
        node = node->next;
    }

    XrEventDataBuffer* event = node->next->event;
    sharedMemoryDescriptor->memory_free(node->next);
    node->next = nullptr;

    return event;
}
