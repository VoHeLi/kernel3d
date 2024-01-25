#include "Engine.h"

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "../openxr/openxr_platform.h"

#include <android/log.h>
#include <unistd.h>
#include <android/looper.h>

#include "GraphicsBackendManager.h"
#include "OpenXRPlugin.h"

Engine::Engine(JNIEnv* jniEnv, jobject activity){
    _jniActivityEnv = jniEnv;
    _activity = activity;
    _nativeWindow = nullptr;
    _resumed = false;

    _activity = jniEnv->NewGlobalRef(activity);

    _jniActivityEnv->GetJavaVM(&_vm);

    /*jclass clazz = _jniActivityEnv->GetObjectClass(_appContext);
    jmethodID methodId = _jniActivityEnv->GetMethodID(clazz, "getClass", "()Ljava/lang/Class;");
    jobject classObject = _jniActivityEnv->CallObjectMethod(appContext, methodId);
    jclass classClazz = _jniActivityEnv->GetObjectClass(classObject);
    jmethodID getNameMethodId = _jniActivityEnv->GetMethodID(classClazz, "getName", "()Ljava/lang/String;");
    jstring className = (jstring)_jniActivityEnv->CallObjectMethod(classObject, getNameMethodId);

    const char *name = _jniActivityEnv->GetStringUTFChars(className, NULL);

    // Utilisez android_log_print pour afficher le nom de la classe
    __android_log_print(ANDROID_LOG_INFO, "Androx Kernel3D", "Class Name: %s", name);

    _jniActivityEnv->ReleaseStringUTFChars(className, name);

    //end debug*/
}

Engine::~Engine(){
    _jniActivityEnv->DeleteGlobalRef(_activity);
}

void Engine::setNativeWindow(ANativeWindow *window) {
    _nativeWindow = window;
    _destroyed = _nativeWindow == nullptr;
}

void Engine::setResuming(bool resumed) {
    _resumed = resumed;
}

void Engine::start() {
    _engineThread = std::thread(&Engine::engineMain, this);
}

void Engine::engineMain() {
    while(_nativeWindow == nullptr){
        __android_log_print(ANDROID_LOG_DEBUG, "Androx Kernel3D", "_nativeWindow = %p", _nativeWindow);
        sleep(1);
    }

    _vm->AttachCurrentThread(&_jniEngineEnv, nullptr);

    bool requestRestart = false;
    bool exitRenderLoop = false;

    // Create graphics OpenGL ES Backend (Context, swapchain...) manager...
    GraphicsBackendManager* graphicsBackendManager = new GraphicsBackendManager(_nativeWindow);

    // Initialize the OpenXR program.
    OpenXRPlugin* openXrPlugin = new OpenXRPlugin("Kernel3D", graphicsBackendManager); //TODO CHANGE NAME

    // Initialize the loader for this platform
    openXrPlugin->InitializeLoaderAndJavaContext(_vm, _activity);

    openXrPlugin->CreateInstance();
    openXrPlugin->InitializeSystemId();

    //options->SetEnvironmentBlendMode(program->GetPreferredBlendMode());
    //UpdateOptionsFromSystemProperties(*options);

    //platformPlugin->UpdateOptions(options);
    //graphicsPlugin->UpdateOptions(options);

    openXrPlugin->InitializeDevice();
    openXrPlugin->InitializeSession();
    openXrPlugin->CreateSwapchains();

    graphicsBackendManager->InitializeResources();

    intentTest(graphicsBackendManager->GetDisplayTexture());
    while (!_destroyed) {
        // Read all pending events.
        /*for (;;) {
            int events;
            struct android_poll_source* source;
            // If the timeout is zero, returns immediately without blocking.
            // If the timeout is negative, waits indefinitely until an event appears.
            const int timeoutMilliseconds =
                    (!_resumed && !openXrPlugin->IsSessionRunning() && !_destroyed) ? -1 : 0;
            if (ALooper_pollAll(timeoutMilliseconds, nullptr, &events, (void**)&source) < 0) {
                break;
            }

            // Process this event. TODO CHANGE, but should not be needed, android events...
            /*if (source != nullptr) {
                source->process(app, source);
            }*/
        //}*/

        openXrPlugin->PollEvents(&exitRenderLoop, &requestRestart);
        if (exitRenderLoop) {
            jclass activityClass = _jniEngineEnv->GetObjectClass(_activity);

            // Obtenez la méthode finish
            jmethodID finishMethod = _jniEngineEnv->GetMethodID(activityClass, "finish", "()V");

            // Appelez la méthode finish sur l'objet de l'activité
            _jniEngineEnv->CallVoidMethod(_activity, finishMethod);

            continue;
        }

        if (!openXrPlugin->IsSessionRunning()) {
            // Throttle loop since xrWaitFrame won't be called.
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            continue;
        }

        openXrPlugin->PollActions();
        openXrPlugin->RenderFrame();
    }


    delete graphicsBackendManager;
    delete openXrPlugin;

    _vm->DetachCurrentThread();
}

void Engine::intentTest(int displayTextureId) {

    jmethodID launchTestMethod = _jniEngineEnv->GetMethodID(_jniEngineEnv->GetObjectClass(_activity), "intentTest", "(I)V");
    _jniEngineEnv->CallVoidMethod(_activity, launchTestMethod, displayTextureId);
}
