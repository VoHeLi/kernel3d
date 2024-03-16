
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>


#include "XrAppLayer.h"
#include "mirage_shared/common_types.h"
#include "GraphicsBackendManager.h"

XrAppLayer::XrAppLayer(int width, int height, AHardwareBuffer *hardwareBuffer1,
                       AHardwareBuffer *hardwareBuffer2, AHardwareBuffer *hardwareBuffer3, int textureType,
                       int arrayLayerCount, uint32_t *currentSwapchainIndexHandle) {
    _width = width;
    _height = height;
    _hardwareBuffer1 = hardwareBuffer1;
    _hardwareBuffer2 = hardwareBuffer2;
    _hardwareBuffer3 = hardwareBuffer3;
    currentTextureIdIndex = 0;
    this->textureType = textureType;
    this->arrayLayerCount = arrayLayerCount;

    _textureId1 = 0;
    _textureId2 = 0;
    _textureId3 = 0;

    this->currentSwapchainIndexHandle = currentSwapchainIndexHandle;
}

XrAppLayer::~XrAppLayer() {

}

void XrAppLayer::PrepareRendering() {

    if(_textureId1 != 0){
        return;
    }

    glGenTextures(1, &_textureId1);
    glGenTextures(1, &_textureId2);
    glGenTextures(1, &_textureId3);


    glBindTexture(DEBUG_TEXTURE_TYPE, _textureId1);
    EGLClientBuffer eglClientBuffer1 = eglGetNativeClientBufferANDROID(_hardwareBuffer1);
    EGLImageKHR eglImageKHR1 = eglCreateImageKHR(eglGetCurrentDisplay(), EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, eglClientBuffer1, nullptr);
    glEGLImageTargetTexture2DOES(DEBUG_TEXTURE_TYPE, eglImageKHR1);

    glBindTexture(DEBUG_TEXTURE_TYPE, _textureId2);
    EGLClientBuffer eglClientBuffer2 = eglGetNativeClientBufferANDROID(_hardwareBuffer2);
    EGLImageKHR eglImageKHR2 = eglCreateImageKHR(eglGetCurrentDisplay(), EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, eglClientBuffer2, nullptr);
    glEGLImageTargetTexture2DOES(DEBUG_TEXTURE_TYPE, eglImageKHR2);

    glBindTexture(DEBUG_TEXTURE_TYPE, _textureId3);
    EGLClientBuffer eglClientBuffer3 = eglGetNativeClientBufferANDROID(_hardwareBuffer3);
    EGLImageKHR eglImageKHR3 = eglCreateImageKHR(eglGetCurrentDisplay(), EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, eglClientBuffer3, nullptr);
    glEGLImageTargetTexture2DOES(DEBUG_TEXTURE_TYPE, eglImageKHR3);

    glBindTexture(DEBUG_TEXTURE_TYPE, 0);
}

