#include "engine/Engine.h"

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include <android/log.h>

Engine* engine;

extern "C"
JNIEXPORT jint JNICALL
Java_com_androx_kernel3d_Kernel3DLauncher_start(JNIEnv *env, jobject thiz) {
    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "Start");

    engine = new Engine(env, thiz);
    engine->start();

    return 0;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_androx_kernel3d_Kernel3DLauncher_resume(JNIEnv *env, jobject thiz) {
    engine->setResuming(true);

    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "Resume");

    return 0;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_androx_kernel3d_Kernel3DLauncher_pause(JNIEnv *env, jobject thiz) {
    engine->setResuming(false);

    return 0;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_androx_kernel3d_Kernel3DLauncher_stop(JNIEnv *env, jobject thiz) {
    // TODO: implement stop()

    engine->setNativeWindow(nullptr);

    delete engine;
    return 0;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_androx_kernel3d_Kernel3DLauncher_createNativeWindow(JNIEnv *env, jobject thiz,
                                                             jobject surface) {
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);

    engine->setNativeWindow(nativeWindow);

    __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "CreateNativeWindow");

    return 0;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_androx_kernel3d_Kernel3DLauncher_destroyNativeWindow(JNIEnv *env, jobject thiz) {
    engine->setNativeWindow(nullptr);

    return 0;
}