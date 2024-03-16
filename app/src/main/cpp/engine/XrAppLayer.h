#pragma once

#include <GLES3/gl32.h>
#include <android/hardware_buffer.h>

#include "mirage_shared/common_types.h"

class XrAppLayer {
public:
    XrAppLayer(int width, int height, AHardwareBuffer* hardwareBuffer1, AHardwareBuffer* hardwareBuffer2, AHardwareBuffer* hardwareBuffer3, int textureType, int arrayLayerCount,
               uint32_t *currentSwapchainIndexHandle);
    ~XrAppLayer();

    void PrepareRendering();


    int _width;
    int _height;
    GLuint _textureId1;
    GLuint _textureId2;
    GLuint _textureId3;

    AHardwareBuffer* _hardwareBuffer1;
    AHardwareBuffer* _hardwareBuffer2;
    AHardwareBuffer* _hardwareBuffer3;

    int currentTextureIdIndex;
    int textureType = GL_TEXTURE_2D_ARRAY;
    int arrayLayerCount = 2;
    uint32_t* currentSwapchainIndexHandle;

};
