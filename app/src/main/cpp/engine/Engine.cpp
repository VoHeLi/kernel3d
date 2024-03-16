#include "Engine.h"

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "../openxr/openxr_platform.h"

#include <android/log.h>
#include <unistd.h>
#include <android/looper.h>

#include "GraphicsBackendManager.h"
#include "OpenXRPlugin.h"
#include "../controllers/handtracking_inputs.h"
#include "../glm/vec3.hpp"
#include "../glm/geometric.hpp"
#include "SpatialObject.h"

#include "mirage_app/mirage_service.h"
#include "XrAppLayer.h"

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

    initializeHands(openXrPlugin);

    intentTest(graphicsBackendManager->GetDisplayTexture(1), 1);

    glm::vec3 pos = glm::vec3(0, 0,-2.0f);
    glm::quat rot = glm::quat(1,0,0,0);
    glm::vec3 size = glm::vec3(16.0f/9.0f, 1.0f, 1.0f);
    SpatialObject* appDisplay = new SpatialObject(pos, rot,  size, graphicsBackendManager->GetDisplayTexture(1));

    glm::vec3 pos2 = glm::vec3(0, 0,-1.99f);
    glm::quat rot2 = glm::quat(1,0,0,0);
    glm::vec3 size2 = glm::vec3(0.02f, 0.02f, 1.0f);
    SpatialObject* pointerDisplay = new SpatialObject(pos2, rot2,  size2, 0);

    std::vector<SpatialObject*> spatialObjects;
    spatialObjects.push_back(appDisplay);
    spatialObjects.push_back(pointerDisplay);


    /*intentTest(graphicsBackendManager->GetDisplayTexture(2), 2);
    intentTest(graphicsBackendManager->GetDisplayTexture(3), 3);*/


    std::vector<XrAppLayer*> appLayers;
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

        openXrPlugin->PrepareRendering();

        graphicsBackendManager->GetDisplayTexture(1); //Just to trigger the hardware buffer



        //DEBUG
        engineUpdateTest(&appLayers, openXrPlugin->_views.data());

        //Hand tracking test TODO REMOVE
        XrPosef indexTipPose;
        XrPosef thumbTipPose;
        XrPosef palmPose;
        tryGetBonePose(XR_HAND_RIGHT_EXT, &indexTipPose, XR_HAND_JOINT_INDEX_TIP_EXT);
        tryGetBonePose(XR_HAND_RIGHT_EXT, &thumbTipPose, XR_HAND_JOINT_THUMB_TIP_EXT);
        tryGetBonePose(XR_HAND_RIGHT_EXT, &palmPose, XR_HAND_JOINT_PALM_EXT);

        glm::vec3 indexTipPos = glm::vec3(indexTipPose.position.x, indexTipPose.position.y, indexTipPose.position.z);
        glm::vec3 thumbTipPos = glm::vec3(thumbTipPose.position.x, thumbTipPose.position.y, thumbTipPose.position.z);
        glm::vec3 palmPos = glm::vec3(palmPose.position.x, palmPose.position.y, palmPose.position.z);

        float pinchDist = glm::distance(indexTipPos, thumbTipPos);

        bool pinching = pinchDist <= 0.03f;

        //__android_log_print(ANDROID_LOG_DEBUG, "AndroxKernel3D", "PinchDist : %.6f", pinchDist);

        float cx = 4.0f*(palmPos.x-0.15f);
        float cy = 4.0f*(palmPos.y+0.15f);

        pointerDisplay->_position = glm::vec3(cx, cy, -1.99f);

        //appDisplay->_position = thumbTipPos + glm::vec3(0.0f,0.0f, -1.0f);

        updateSurfaceTexture(!pinching, cx, cy);

        openXrPlugin->RenderFrame(spatialObjects, appLayers);
    }

    for(SpatialObject* so : spatialObjects){
        delete so;
    }

    destroyHands();
    delete graphicsBackendManager;
    delete openXrPlugin;

    _vm->DetachCurrentThread();
}

void Engine::intentTest(int displayTextureId, int app) {

    jmethodID launchTestMethod = _jniEngineEnv->GetMethodID(_jniEngineEnv->GetObjectClass(_activity), "intentTest", "(II)V");
    _jniEngineEnv->CallVoidMethod(_activity, launchTestMethod, displayTextureId, app);
}

void Engine::updateSurfaceTexture(bool pinching, float cx, float cy){
    jmethodID updateTestMethod = _jniEngineEnv->GetMethodID(_jniEngineEnv->GetObjectClass(_activity), "updateDisplayTexture", "(ZFF)V");
    _jniEngineEnv->CallVoidMethod(_activity, updateTestMethod, pinching, cx, cy);
}