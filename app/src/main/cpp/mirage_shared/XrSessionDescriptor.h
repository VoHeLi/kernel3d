#pragma once

#include "XrInstanceDescriptor.h"
#include "XrReferenceSpaceDescriptor.h"
#include "XrActionSpaceDescriptor.h"
#include "XrSwapchainDescriptor.h"

#define XR_SESSION_DESCRIPTOR_SIGNATURE 0x40558631ca921e7b

class XrInstanceDescriptor;
class XrReferenceSpaceDescriptor;
class XrActionSpaceDescriptor;
class XrSwapchainDescriptor;

class XrSessionDescriptor {
public:

    XrSessionDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, XrInstanceDescriptor* instanceDescriptor, const XrSessionCreateInfo *createInfo);
    ~XrSessionDescriptor();


    uint64_t signature;
    bool created;
    XrInstanceDescriptor* instanceDescriptor;
    XrSessionCreateInfo* createInfo;
    XrSessionDescriptor* nextSessionDescriptor;
    XrReferenceSpaceType* referenceSpaces;
    uint32_t referenceSpacesCount;
    XrReferenceSpaceDescriptor* firstReferenceSpaceDescriptor;
    int64_t* swapchainFormats;
    uint32_t swapchainFormatsCount;
    XrViewConfigurationType primaryViewConfigurationType;
    uint32_t countActionSets;
    XrActionSet* actionSets;
    XrActionSpaceDescriptor* firstActionSpaceDescriptor;
    XrSwapchainDescriptor* firstSwapchainDescriptor;


    //TODO : Add Graphics Bindings support for server
};
