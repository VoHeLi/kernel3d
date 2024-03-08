
#include "XrPathDescriptor.h"
#include <new>
#include <string>

XrPathDescriptor::XrPathDescriptor(shared_memory_descriptor* sharedMemoryDescriptor, const char *pathString, XrPathDescriptor *parent) {
    this->pathString = NEW_SHARED(char[strlen(pathString)]);
    strcpy((char*)this->pathString, pathString);

    this->nextPathDescriptor = nullptr;

    if(parent != nullptr){
        parent->nextPathDescriptor = this;
    }
    //IF parent is nullptr, then this is the first path descriptor
}

XrPathDescriptor::~XrPathDescriptor() {

}
