#pragma once

#include "../engine/OpenXRPlugin.h"
#include "../openxr/openxr.h"

static XrHandTrackerEXT leftHandTracker;
static XrHandTrackerEXT rightHandTracker;

static XrHandJointLocationEXT leftJointLocations[XR_HAND_JOINT_COUNT_EXT];
static XrHandJointLocationEXT rightJointLocations[XR_HAND_JOINT_COUNT_EXT];

static XrTime lastTime;

static OpenXRPlugin* handTrackingOpenXRPlugin;

void initializeHands(OpenXRPlugin* openXrPlugin);

void destroyHands(); //todo ?

void updateHandJoints(XrTime currentTime, XrSpace baseSpace, XrHandEXT handType);


XrResult tryGetBonePose(XrHandEXT handType, XrPosef* pose, uint32_t bone);

XrTime getCurrentTime();