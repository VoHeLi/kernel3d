// Copyright 2019-2023, Collabora, Ltd.
// SPDX-License-Identifier: BSL-1.0
/*!
 * @file
 * @brief  Macros for generating extension-related tables and code and
 * inspecting Monado's extension support.
 *
 * MOSTLY GENERATED CODE - see below!
 *
 * To add support for a new extension, edit and run generate_oxr_ext_support.py,
 * then run clang-format on this file. Two entire chunks of this file get
 * replaced by that script: comments indicate where they are.
 *
 * @author Ryan Pavlik <ryan.pavlik@collabora.com>
 */

#pragma once

#define RUNTIME_NAME "PICOREUR Runtime"
#define RUNTIME_VERSION XrVersion XR_MAKE_VERSION(0,0,1)
#define VENDOR_ID 0x2982
#define XR_PICO_CONFIGURATION_SPEC_VERSION 1
#define XR_PICO_CONFIGURATION_EXTENSION_NAME "XR_PICO_configuration"


//TODO : find the good VENDOR_ID




// beginning of GENERATED defines - do not modify - used by scripts

/*
 * XR_KHR_android_create_instance
 */

#define OXR_HAVE_KHR_android_create_instance
#define OXR_EXTENSION_SUPPORT_KHR_android_create_instance(_) _(KHR_android_create_instance, KHR_ANDROID_CREATE_INSTANCE)

#define OXR_FEATURE_CONTROLLER_INTERACTION(_) _(BD_controller_interaction, BD_CONTROLLER_INTERACTION)
#define OXR_EXTENSION_SUPPORT_PICO_CONFIGURATION(_) _(PICO_CONFIGURATION, PICO_CONFIGURATION)

/*
 * XR_KHR_android_thread_settings
 */
#if defined(XR_KHR_android_thread_settings) && defined(XR_USE_PLATFORM_ANDROID)
#define OXR_HAVE_KHR_android_thread_settings
#define OXR_EXTENSION_SUPPORT_KHR_android_thread_settings(_) _(KHR_android_thread_settings, KHR_ANDROID_THREAD_SETTINGS)
#else
#define OXR_EXTENSION_SUPPORT_KHR_android_thread_settings(_)
#endif


/*
 * XR_KHR_binding_modification
 */
#if defined(XR_KHR_binding_modification)
#define OXR_HAVE_KHR_binding_modification
#define OXR_EXTENSION_SUPPORT_KHR_binding_modification(_) _(KHR_binding_modification, KHR_BINDING_MODIFICATION)
#else
#define OXR_EXTENSION_SUPPORT_KHR_binding_modification(_)
#endif


/*
 * XR_KHR_composition_layer_cube
 */
#if defined(XR_KHR_composition_layer_cube) && defined(XRT_FEATURE_OPENXR_LAYER_CUBE)
#define OXR_HAVE_KHR_composition_layer_cube
#define OXR_EXTENSION_SUPPORT_KHR_composition_layer_cube(_) _(KHR_composition_layer_cube, KHR_COMPOSITION_LAYER_CUBE)
#else
#define OXR_EXTENSION_SUPPORT_KHR_composition_layer_cube(_)
#endif


/*
 * XR_KHR_composition_layer_cylinder
 */
#if defined(XR_KHR_composition_layer_cylinder) && defined(XRT_FEATURE_OPENXR_LAYER_CYLINDER)
#define OXR_HAVE_KHR_composition_layer_cylinder
#define OXR_EXTENSION_SUPPORT_KHR_composition_layer_cylinder(_)                                                        \
	_(KHR_composition_layer_cylinder, KHR_COMPOSITION_LAYER_CYLINDER)
#else
#define OXR_EXTENSION_SUPPORT_KHR_composition_layer_cylinder(_)
#endif


/*
 * XR_KHR_composition_layer_depth
 */
#if defined(XR_KHR_composition_layer_depth) && defined(XRT_FEATURE_OPENXR_LAYER_DEPTH)
#define OXR_HAVE_KHR_composition_layer_depth
#define OXR_EXTENSION_SUPPORT_KHR_composition_layer_depth(_) _(KHR_composition_layer_depth, KHR_COMPOSITION_LAYER_DEPTH)
#else
#define OXR_EXTENSION_SUPPORT_KHR_composition_layer_depth(_)
#endif


/*
 * XR_KHR_composition_layer_equirect
 */
#if defined(XR_KHR_composition_layer_equirect) && defined(XRT_FEATURE_OPENXR_LAYER_EQUIRECT1)
#define OXR_HAVE_KHR_composition_layer_equirect
#define OXR_EXTENSION_SUPPORT_KHR_composition_layer_equirect(_)                                                        \
	_(KHR_composition_layer_equirect, KHR_COMPOSITION_LAYER_EQUIRECT)
#else
#define OXR_EXTENSION_SUPPORT_KHR_composition_layer_equirect(_)
#endif


/*
 * XR_KHR_composition_layer_equirect2
 */
#if defined(XR_KHR_composition_layer_equirect2) && defined(XRT_FEATURE_OPENXR_LAYER_EQUIRECT2)
#define OXR_HAVE_KHR_composition_layer_equirect2
#define OXR_EXTENSION_SUPPORT_KHR_composition_layer_equirect2(_)                                                       \
	_(KHR_composition_layer_equirect2, KHR_COMPOSITION_LAYER_EQUIRECT2)
#else
#define OXR_EXTENSION_SUPPORT_KHR_composition_layer_equirect2(_)
#endif


/*
 * XR_KHR_convert_timespec_time
 */
#if defined(XR_KHR_convert_timespec_time) && defined(XR_USE_TIMESPEC) && (!defined(XR_USE_PLATFORM_WIN32))
#define OXR_HAVE_KHR_convert_timespec_time
#define OXR_EXTENSION_SUPPORT_KHR_convert_timespec_time(_) _(KHR_convert_timespec_time, KHR_CONVERT_TIMESPEC_TIME)
#else
#define OXR_EXTENSION_SUPPORT_KHR_convert_timespec_time(_)
#endif


/*
 * XR_KHR_D3D11_enable
 */
#if defined(XR_KHR_D3D11_enable) && defined(XR_USE_GRAPHICS_API_D3D11)
#define OXR_HAVE_KHR_D3D11_enable
#define OXR_EXTENSION_SUPPORT_KHR_D3D11_enable(_) _(KHR_D3D11_enable, KHR_D3D11_ENABLE)
#else
#define OXR_EXTENSION_SUPPORT_KHR_D3D11_enable(_)
#endif


/*
 * XR_KHR_D3D12_enable
 */
#if defined(XR_KHR_D3D12_enable) && defined(XR_USE_GRAPHICS_API_D3D12)
#define OXR_HAVE_KHR_D3D12_enable
#define OXR_EXTENSION_SUPPORT_KHR_D3D12_enable(_) _(KHR_D3D12_enable, KHR_D3D12_ENABLE)
#else
#define OXR_EXTENSION_SUPPORT_KHR_D3D12_enable(_)
#endif


/*
 * XR_KHR_loader_init
 */
#if defined(XR_KHR_loader_init) && defined(XR_USE_PLATFORM_ANDROID)
#define OXR_HAVE_KHR_loader_init
#define OXR_EXTENSION_SUPPORT_KHR_loader_init(_) _(KHR_loader_init, KHR_LOADER_INIT)
#else
#define OXR_EXTENSION_SUPPORT_KHR_loader_init(_)
#endif


/*
 * XR_KHR_loader_init_android
 */
#if defined(XR_KHR_loader_init_android) && defined(OXR_HAVE_KHR_loader_init) && defined(XR_USE_PLATFORM_ANDROID)
#define OXR_HAVE_KHR_loader_init_android
#define OXR_EXTENSION_SUPPORT_KHR_loader_init_android(_) _(KHR_loader_init_android, KHR_LOADER_INIT_ANDROID)
#else
#define OXR_EXTENSION_SUPPORT_KHR_loader_init_android(_)
#endif


/*
 * XR_KHR_opengl_enable
 */
#if defined(XR_KHR_opengl_enable) && defined(XR_USE_GRAPHICS_API_OPENGL)
#define OXR_HAVE_KHR_opengl_enable
#define OXR_EXTENSION_SUPPORT_KHR_opengl_enable(_) _(KHR_opengl_enable, KHR_OPENGL_ENABLE)
#else
#define OXR_EXTENSION_SUPPORT_KHR_opengl_enable(_)
#endif


/*
 * XR_KHR_opengl_es_enable
 */
#if defined(XR_KHR_opengl_es_enable) && defined(XR_USE_GRAPHICS_API_OPENGL_ES)
#define OXR_HAVE_KHR_opengl_es_enable
#define OXR_EXTENSION_SUPPORT_KHR_opengl_es_enable(_) _(KHR_opengl_es_enable, KHR_OPENGL_ES_ENABLE)
#else
#define OXR_EXTENSION_SUPPORT_KHR_opengl_es_enable(_)
#endif


/*
 * XR_KHR_swapchain_usage_input_attachment_bit
 */
#if defined(XR_KHR_swapchain_usage_input_attachment_bit)
#define OXR_HAVE_KHR_swapchain_usage_input_attachment_bit
#define OXR_EXTENSION_SUPPORT_KHR_swapchain_usage_input_attachment_bit(_)                                              \
	_(KHR_swapchain_usage_input_attachment_bit, KHR_SWAPCHAIN_USAGE_INPUT_ATTACHMENT_BIT)
#else
#define OXR_EXTENSION_SUPPORT_KHR_swapchain_usage_input_attachment_bit(_)
#endif


/*
 * XR_KHR_visibility_mask
 */
#if defined(XR_KHR_visibility_mask) && defined(XRT_FEATURE_OPENXR_VISIBILITY_MASK)
#define OXR_HAVE_KHR_visibility_mask
#define OXR_EXTENSION_SUPPORT_KHR_visibility_mask(_) _(KHR_visibility_mask, KHR_VISIBILITY_MASK)
#else
#define OXR_EXTENSION_SUPPORT_KHR_visibility_mask(_)
#endif


/*
 * XR_KHR_vulkan_enable
 */
#if defined(XR_KHR_vulkan_enable) && defined(XR_USE_GRAPHICS_API_VULKAN)
#define OXR_HAVE_KHR_vulkan_enable
#define OXR_EXTENSION_SUPPORT_KHR_vulkan_enable(_) _(KHR_vulkan_enable, KHR_VULKAN_ENABLE)
#else
#define OXR_EXTENSION_SUPPORT_KHR_vulkan_enable(_)
#endif


/*
 * XR_KHR_vulkan_enable2
 */
#if defined(XR_KHR_vulkan_enable2) && defined(XR_USE_GRAPHICS_API_VULKAN)
#define OXR_HAVE_KHR_vulkan_enable2
#define OXR_EXTENSION_SUPPORT_KHR_vulkan_enable2(_) _(KHR_vulkan_enable2, KHR_VULKAN_ENABLE2)
#else
#define OXR_EXTENSION_SUPPORT_KHR_vulkan_enable2(_)
#endif


/*
 * XR_KHR_vulkan_swapchain_format_list
 */
#if defined(XR_KHR_vulkan_swapchain_format_list) && defined(XR_USE_GRAPHICS_API_VULKAN)
#define OXR_HAVE_KHR_vulkan_swapchain_format_list
#define OXR_EXTENSION_SUPPORT_KHR_vulkan_swapchain_format_list(_)                                                      \
	_(KHR_vulkan_swapchain_format_list, KHR_VULKAN_SWAPCHAIN_FORMAT_LIST)
#else
#define OXR_EXTENSION_SUPPORT_KHR_vulkan_swapchain_format_list(_)
#endif


/*
 * XR_KHR_win32_convert_performance_counter_time
 */
#if defined(XR_KHR_win32_convert_performance_counter_time) && defined(XR_USE_PLATFORM_WIN32)
#define OXR_HAVE_KHR_win32_convert_performance_counter_time
#define OXR_EXTENSION_SUPPORT_KHR_win32_convert_performance_counter_time(_)                                            \
	_(KHR_win32_convert_performance_counter_time, KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME)
#else
#define OXR_EXTENSION_SUPPORT_KHR_win32_convert_performance_counter_time(_)
#endif


/*
 * XR_EXT_debug_utils
 */
#if defined(XR_EXT_debug_utils) && defined(XRT_FEATURE_OPENXR_DEBUG_UTILS)
#define OXR_HAVE_EXT_debug_utils
#define OXR_EXTENSION_SUPPORT_EXT_debug_utils(_) _(EXT_debug_utils, EXT_DEBUG_UTILS)
#else
#define OXR_EXTENSION_SUPPORT_EXT_debug_utils(_)
#endif


/*
 * XR_EXT_dpad_binding
 */
#if defined(XR_EXT_dpad_binding)
#define OXR_HAVE_EXT_dpad_binding
#define OXR_EXTENSION_SUPPORT_EXT_dpad_binding(_) _(EXT_dpad_binding, EXT_DPAD_BINDING)
#else
#define OXR_EXTENSION_SUPPORT_EXT_dpad_binding(_)
#endif


/*
 * XR_EXT_eye_gaze_interaction
 */
#if defined(XR_EXT_eye_gaze_interaction) && defined(XRT_FEATURE_OPENXR_INTERACTION_EXT_EYE_GAZE)
#define OXR_HAVE_EXT_eye_gaze_interaction
#define OXR_EXTENSION_SUPPORT_EXT_eye_gaze_interaction(_) _(EXT_eye_gaze_interaction, EXT_EYE_GAZE_INTERACTION)
#else
#define OXR_EXTENSION_SUPPORT_EXT_eye_gaze_interaction(_)
#endif


/*
 * XR_EXT_hand_interaction
 */
#if defined(XR_EXT_hand_interaction) && defined(XRT_FEATURE_OPENXR_INTERACTION_EXT_HAND)
#define OXR_HAVE_EXT_hand_interaction
#define OXR_EXTENSION_SUPPORT_EXT_hand_interaction(_) _(EXT_hand_interaction, EXT_HAND_INTERACTION)
#else
#define OXR_EXTENSION_SUPPORT_EXT_hand_interaction(_)
#endif


/*
 * XR_EXT_hand_tracking
 */
#if defined(XR_EXT_hand_tracking)
#define OXR_HAVE_EXT_hand_tracking
#define OXR_EXTENSION_SUPPORT_EXT_hand_tracking(_) _(EXT_hand_tracking, EXT_HAND_TRACKING)
#else
#define OXR_EXTENSION_SUPPORT_EXT_hand_tracking(_)
#endif


/*
 * XR_EXT_hp_mixed_reality_controller
 */
#if defined(XR_EXT_hp_mixed_reality_controller) && defined(XRT_FEATURE_OPENXR_INTERACTION_WINMR)
#define OXR_HAVE_EXT_hp_mixed_reality_controller
#define OXR_EXTENSION_SUPPORT_EXT_hp_mixed_reality_controller(_)                                                       \
	_(EXT_hp_mixed_reality_controller, EXT_HP_MIXED_REALITY_CONTROLLER)
#else
#define OXR_EXTENSION_SUPPORT_EXT_hp_mixed_reality_controller(_)
#endif


/*
 * XR_EXT_local_floor
 */
#if defined(XR_EXT_local_floor) && defined(XRT_FEATURE_OPENXR_SPACE_LOCAL_FLOOR)
#define OXR_HAVE_EXT_local_floor
#define OXR_EXTENSION_SUPPORT_EXT_local_floor(_) _(EXT_local_floor, EXT_LOCAL_FLOOR)
#else
#define OXR_EXTENSION_SUPPORT_EXT_local_floor(_)
#endif


/*
 * XR_EXT_palm_pose
 */
#if defined(XR_EXT_palm_pose) && defined(XRT_FEATURE_OPENXR_INTERACTION_EXT_PALM_POSE)
#define OXR_HAVE_EXT_palm_pose
#define OXR_EXTENSION_SUPPORT_EXT_palm_pose(_) _(EXT_palm_pose, EXT_PALM_POSE)
#else
#define OXR_EXTENSION_SUPPORT_EXT_palm_pose(_)
#endif


/*
 * XR_EXT_samsung_odyssey_controller
 */
#if defined(XR_EXT_samsung_odyssey_controller) && defined(XRT_FEATURE_OPENXR_INTERACTION_WINMR)
#define OXR_HAVE_EXT_samsung_odyssey_controller
#define OXR_EXTENSION_SUPPORT_EXT_samsung_odyssey_controller(_)                                                        \
	_(EXT_samsung_odyssey_controller, EXT_SAMSUNG_ODYSSEY_CONTROLLER)
#else
#define OXR_EXTENSION_SUPPORT_EXT_samsung_odyssey_controller(_)
#endif


/*
 * XR_FB_display_refresh_rate
 */
#if defined(XR_FB_display_refresh_rate) && defined(XRT_FEATURE_OPENXR_DISPLAY_REFRESH_RATE)
#define OXR_HAVE_FB_display_refresh_rate
#define OXR_EXTENSION_SUPPORT_FB_display_refresh_rate(_) _(FB_display_refresh_rate, FB_DISPLAY_REFRESH_RATE)
#else
#define OXR_EXTENSION_SUPPORT_FB_display_refresh_rate(_)
#endif


/*
 * XR_ML_ml2_controller_interaction
 */
#if defined(XR_ML_ml2_controller_interaction) && defined(XRT_FEATURE_OPENXR_INTERACTION_ML2)
#define OXR_HAVE_ML_ml2_controller_interaction
#define OXR_EXTENSION_SUPPORT_ML_ml2_controller_interaction(_)                                                         \
	_(ML_ml2_controller_interaction, ML_ML2_CONTROLLER_INTERACTION)
#else
#define OXR_EXTENSION_SUPPORT_ML_ml2_controller_interaction(_)
#endif


/*
 * XR_MND_headless
 */
#if defined(XR_MND_headless) && defined(XRT_FEATURE_OPENXR_HEADLESS)
#define OXR_HAVE_MND_headless
#define OXR_EXTENSION_SUPPORT_MND_headless(_) _(MND_headless, MND_HEADLESS)
#else
#define OXR_EXTENSION_SUPPORT_MND_headless(_)
#endif


/*
 * XR_MND_swapchain_usage_input_attachment_bit
 */
#if defined(XR_MND_swapchain_usage_input_attachment_bit)
#define OXR_HAVE_MND_swapchain_usage_input_attachment_bit
#define OXR_EXTENSION_SUPPORT_MND_swapchain_usage_input_attachment_bit(_)                                              \
	_(MND_swapchain_usage_input_attachment_bit, MND_SWAPCHAIN_USAGE_INPUT_ATTACHMENT_BIT)
#else
#define OXR_EXTENSION_SUPPORT_MND_swapchain_usage_input_attachment_bit(_)
#endif


/*
 * XR_MSFT_hand_interaction
 */
#if defined(XR_MSFT_hand_interaction) && defined(XRT_FEATURE_OPENXR_INTERACTION_MSFT_HAND)
#define OXR_HAVE_MSFT_hand_interaction
#define OXR_EXTENSION_SUPPORT_MSFT_hand_interaction(_) _(MSFT_hand_interaction, MSFT_HAND_INTERACTION)
#else
#define OXR_EXTENSION_SUPPORT_MSFT_hand_interaction(_)
#endif


/*
 * XR_MSFT_unbounded_reference_space
 */
#if defined(XR_MSFT_unbounded_reference_space) && defined(XRT_FEATURE_OPENXR_SPACE_UNBOUNDED)
#define OXR_HAVE_MSFT_unbounded_reference_space
#define OXR_EXTENSION_SUPPORT_MSFT_unbounded_reference_space(_)                                                        \
	_(MSFT_unbounded_reference_space, MSFT_UNBOUNDED_REFERENCE_SPACE)
#else
#define OXR_EXTENSION_SUPPORT_MSFT_unbounded_reference_space(_)
#endif


/*
 * XR_OPPO_controller_interaction
 */
#if defined(XR_OPPO_controller_interaction) && defined(XRT_FEATURE_OPENXR_INTERACTION_OPPO)
#define OXR_HAVE_OPPO_controller_interaction
#define OXR_EXTENSION_SUPPORT_OPPO_controller_interaction(_) _(OPPO_controller_interaction, OPPO_CONTROLLER_INTERACTION)
#else
#define OXR_EXTENSION_SUPPORT_OPPO_controller_interaction(_)
#endif


/*
 * XR_EXTX_overlay
 */
#if defined(XR_EXTX_overlay) && defined(XRT_FEATURE_OPENXR_OVERLAY)
#define OXR_HAVE_EXTX_overlay
#define OXR_EXTENSION_SUPPORT_EXTX_overlay(_) _(EXTX_overlay, EXTX_OVERLAY)
#else
#define OXR_EXTENSION_SUPPORT_EXTX_overlay(_)
#endif


/*
 * XR_HTCX_vive_tracker_interaction
 */
#if defined(XR_HTCX_vive_tracker_interaction) && defined(ALWAYS_DISABLED)
#define OXR_HAVE_HTCX_vive_tracker_interaction
#define OXR_EXTENSION_SUPPORT_HTCX_vive_tracker_interaction(_)                                                         \
	_(HTCX_vive_tracker_interaction, HTCX_VIVE_TRACKER_INTERACTION)
#else
#define OXR_EXTENSION_SUPPORT_HTCX_vive_tracker_interaction(_)
#endif


/*
 * XR_MNDX_ball_on_a_stick_controller
 */
#if defined(XR_MNDX_ball_on_a_stick_controller) && defined(XRT_FEATURE_OPENXR_INTERACTION_MNDX)
#define OXR_HAVE_MNDX_ball_on_a_stick_controller
#define OXR_EXTENSION_SUPPORT_MNDX_ball_on_a_stick_controller(_)                                                       \
	_(MNDX_ball_on_a_stick_controller, MNDX_BALL_ON_A_STICK_CONTROLLER)
#else
#define OXR_EXTENSION_SUPPORT_MNDX_ball_on_a_stick_controller(_)
#endif


/*
 * XR_MNDX_egl_enable
 */
#if defined(XR_MNDX_egl_enable) && defined(XR_USE_PLATFORM_EGL) && defined(XR_USE_GRAPHICS_API_OPENGL)
#define OXR_HAVE_MNDX_egl_enable
#define OXR_EXTENSION_SUPPORT_MNDX_egl_enable(_) _(MNDX_egl_enable, MNDX_EGL_ENABLE)
#else
#define OXR_EXTENSION_SUPPORT_MNDX_egl_enable(_)
#endif


/*
 * XR_MNDX_force_feedback_curl
 */
#if defined(XR_MNDX_force_feedback_curl) && defined(XRT_FEATURE_OPENXR_FORCE_FEEDBACK_CURL)
#define OXR_HAVE_MNDX_force_feedback_curl
#define OXR_EXTENSION_SUPPORT_MNDX_force_feedback_curl(_) _(MNDX_force_feedback_curl, MNDX_FORCE_FEEDBACK_CURL)
#else
#define OXR_EXTENSION_SUPPORT_MNDX_force_feedback_curl(_)
#endif


/*
 * XR_MNDX_hydra
 */
#if defined(XR_MNDX_hydra) && defined(XRT_FEATURE_OPENXR_INTERACTION_MNDX)
#define OXR_HAVE_MNDX_hydra
#define OXR_EXTENSION_SUPPORT_MNDX_hydra(_) _(MNDX_hydra, MNDX_HYDRA)
#else
#define OXR_EXTENSION_SUPPORT_MNDX_hydra(_)
#endif


/*
 * XR_MNDX_system_buttons
 */
#if defined(XR_MNDX_system_buttons) && defined(XRT_FEATURE_OPENXR_INTERACTION_MNDX)
#define OXR_HAVE_MNDX_system_buttons
#define OXR_EXTENSION_SUPPORT_MNDX_system_buttons(_) _(MNDX_system_buttons, MNDX_SYSTEM_BUTTONS)
#else
#define OXR_EXTENSION_SUPPORT_MNDX_system_buttons(_)
#endif

// end of GENERATED per-extension defines - do not modify - used by scripts

/*!
 * Call this, passing a macro taking two parameters, to
 * generate tables, code, etc. related to OpenXR extensions.
 * Upon including invoking OXR_EXTENSION_SUPPORT_GENERATE() with some
 * MY_HANDLE_EXTENSION(mixed_case, all_caps), MY_HANDLE_EXTENSION will be called
 * for each extension implemented in Monado and supported in this build:
 *
 * - The first parameter is the name of the extension without the leading XR_
 *   prefix: e.g. `KHR_opengl_enable`
 * - The second parameter is the same text as the first, but in all uppercase,
 *   since this transform cannot be done in the C preprocessor, and some
 *   extension-related entities use this instead of the exact extension name.
 *
 * Implementation note: This macro calls another macro for each extension: that
 * macro is either defined to call your provided macro, or defined to nothing,
 * depending on if the extension is supported in this build.
 *
 * @note Do not edit anything between `clang-format off` and `clang-format on` -
 * it will be replaced next time this file is generated!
 */
// clang-format off
#define OXR_EXTENSION_SUPPORT_GENERATE(_) \
    OXR_EXTENSION_SUPPORT_KHR_android_create_instance(_) \
    OXR_EXTENSION_SUPPORT_KHR_android_thread_settings(_) \
    OXR_EXTENSION_SUPPORT_KHR_binding_modification(_) \
    OXR_EXTENSION_SUPPORT_KHR_composition_layer_cube(_) \
    OXR_EXTENSION_SUPPORT_KHR_composition_layer_cylinder(_) \
    OXR_EXTENSION_SUPPORT_KHR_composition_layer_depth(_) \
    OXR_EXTENSION_SUPPORT_KHR_composition_layer_equirect(_) \
    OXR_EXTENSION_SUPPORT_KHR_composition_layer_equirect2(_) \
    OXR_EXTENSION_SUPPORT_KHR_convert_timespec_time(_) \
    OXR_EXTENSION_SUPPORT_KHR_D3D11_enable(_) \
    OXR_EXTENSION_SUPPORT_KHR_D3D12_enable(_) \
    OXR_EXTENSION_SUPPORT_KHR_loader_init(_) \
    OXR_EXTENSION_SUPPORT_KHR_loader_init_android(_) \
    OXR_EXTENSION_SUPPORT_KHR_opengl_enable(_) \
    OXR_EXTENSION_SUPPORT_KHR_opengl_es_enable(_) \
    OXR_EXTENSION_SUPPORT_KHR_swapchain_usage_input_attachment_bit(_) \
    OXR_EXTENSION_SUPPORT_KHR_visibility_mask(_) \
    OXR_EXTENSION_SUPPORT_KHR_vulkan_enable(_) \
    OXR_EXTENSION_SUPPORT_KHR_vulkan_enable2(_) \
    OXR_EXTENSION_SUPPORT_KHR_vulkan_swapchain_format_list(_) \
    OXR_EXTENSION_SUPPORT_KHR_win32_convert_performance_counter_time(_) \
    OXR_EXTENSION_SUPPORT_EXT_debug_utils(_) \
    OXR_EXTENSION_SUPPORT_EXT_dpad_binding(_) \
    OXR_EXTENSION_SUPPORT_EXT_eye_gaze_interaction(_) \
    OXR_EXTENSION_SUPPORT_EXT_hand_interaction(_) \
    OXR_EXTENSION_SUPPORT_EXT_hand_tracking(_) \
    OXR_EXTENSION_SUPPORT_EXT_hp_mixed_reality_controller(_) \
    OXR_EXTENSION_SUPPORT_EXT_local_floor(_) \
    OXR_EXTENSION_SUPPORT_EXT_palm_pose(_) \
    OXR_EXTENSION_SUPPORT_EXT_samsung_odyssey_controller(_) \
    OXR_EXTENSION_SUPPORT_FB_display_refresh_rate(_) \
    OXR_EXTENSION_SUPPORT_ML_ml2_controller_interaction(_) \
    OXR_EXTENSION_SUPPORT_MND_headless(_) \
    OXR_EXTENSION_SUPPORT_MND_swapchain_usage_input_attachment_bit(_) \
    OXR_EXTENSION_SUPPORT_MSFT_hand_interaction(_) \
    OXR_EXTENSION_SUPPORT_MSFT_unbounded_reference_space(_) \
    OXR_EXTENSION_SUPPORT_OPPO_controller_interaction(_) \
    OXR_EXTENSION_SUPPORT_EXTX_overlay(_) \
    OXR_EXTENSION_SUPPORT_HTCX_vive_tracker_interaction(_) \
    OXR_EXTENSION_SUPPORT_MNDX_ball_on_a_stick_controller(_) \
    OXR_EXTENSION_SUPPORT_MNDX_egl_enable(_) \
    OXR_EXTENSION_SUPPORT_MNDX_force_feedback_curl(_) \
    OXR_EXTENSION_SUPPORT_MNDX_hydra(_) \
    OXR_EXTENSION_SUPPORT_MNDX_system_buttons(_)         \
    OXR_FEATURE_CONTROLLER_INTERACTION(_) \
    OXR_EXTENSION_SUPPORT_PICO_CONFIGURATION(_)                               \
// clang-format on


#define MAKE_EXT_STATUS(mixed_case, all_caps) bool mixed_case;
struct oxr_extension_status
{
    OXR_EXTENSION_SUPPORT_GENERATE(MAKE_EXT_STATUS)
};