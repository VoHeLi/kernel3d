
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).éé
project("kernel3d")

include_directories(".")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.
add_library(${CMAKE_PROJECT_NAME} SHARED
    # List C/C++ source files with relative paths to this CMakeLists.txt.
    kernel3d.cpp engine/Engine.cpp engine/GraphicsBackendManager.cpp engine/OpenXRPlugin.cpp engine/gfxwrapper_opengl.c engine/SpatialObject.cpp controllers/handtracking_inputs.cpp)

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.

add_library(runtime SHARED
        oxr_runtime/android_globals.cpp oxr_runtime/oxr_api_debug.cpp oxr_runtime/oxr_api_swapchain.cpp oxr_runtime/oxr_api_action.cpp oxr_runtime/oxr_api_space.cpp
        oxr_runtime/oxr_api_session.cpp oxr_runtime/oxr_api_system.cpp oxr_runtime/oxr_path.cpp oxr_runtime/oxr_api_verify.cpp oxr_runtime/oxr_api_instance.cpp
        oxr_runtime/oxr_api_negotiate.cpp oxr_runtime/mirage_binder.cpp)

add_library(mirage SHARED
        mirage_app/mirage_service.cpp mirage_app/mirage_app_server.cpp)

add_library(openxr_loader SHARED IMPORTED)

set_target_properties(openxr_loader PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../jniLibs/arm64-v8a/libopenxr_loader.so)

#add_library(cutils SHARED IMPORTED)
#set_target_properties(cutils PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../jniLibs/arm64-v8a/libcutils.so)

target_link_libraries(${CMAKE_PROJECT_NAME}
    # List libraries link to the target library
    EGL
    GLESv3
    android
    log
    openxr_loader
    )

target_link_libraries(runtime
        android
        log
        EGL
        GLESv3
        #cutils
        )

target_link_libraries(mirage
        android
        log
        EGL
        GLESv3
        #cutils
        )