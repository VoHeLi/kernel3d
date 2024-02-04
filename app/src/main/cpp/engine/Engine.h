#pragma once

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <thread>

class Engine {
public:
    Engine(JNIEnv* jniEnv, jobject activity);
    ~Engine();
    void setNativeWindow(ANativeWindow* window);
    void setResuming(bool resumed);
    void start();

private:
    std::thread _engineThread;

    JavaVM* _vm;
    JNIEnv* _jniActivityEnv;
    JNIEnv* _jniEngineEnv;
    jobject _activity;
    ANativeWindow* _nativeWindow;
    bool _resumed;
    bool _destroyed;

    void engineMain();

    void intentTest(int displayTextureId, int app);

    void updateSurfaceTexture(bool pinching);
};

