
#include "controllers_binding.h"
#include "handtracking_inputs.h"

#include <string>
#include <cmath>
#include <ctime>
#include <chrono>
#include <algorithm>

#include <android/log.h>



void TryRegisterControllerPath(XrPath path, const char* pathStr){
    __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "TryRegisterControllerPath : %p, %s ", path, pathStr);
    if(strcmp(pathStr, handSpacesPathNames[LEFT_HAND]) == 0){
        __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Registering left hand");
        handPaths[LEFT_HAND] = path;
    }
    else if(strcmp(pathStr, handSpacesPathNames[RIGHT_HAND]) == 0){
        __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Registering right hand");
        handPaths[RIGHT_HAND] = path;
    }
    else if(strcmp(pathStr, "/interaction_profiles/bytedance/pico4_controller") == 0){
        __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Registering interaction_profile : %p, %s ", path, pathStr);
        interactionProfilePath = path;
    }
    else{
        for(int i = 0; i < 33; i++){
            if(strcmp(pathStr, controllerPathNames[i]) == 0){
                __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Registering controller path : %d", i);
                controllerPaths[i] = path;
            }
        }
    }
}

void TryRegisterControllerSpace(XrSpace space, XrAction action, XrPath path){ //TODO GET ONLY INTERESTED POSE (ONLY GRIP AND PICO4)

    __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "TryRegisterControllerSpace : %p, %p, %p ", space, path, action);
    if(path == handPaths[LEFT_HAND]){ //Force grip pos and not pointer
        handSpaces[LEFT_HAND].push_back(space);
        __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Registering left hand : %p, %p, %p ", space, path, action);
    }
    else if(path == handPaths[RIGHT_HAND]) { //Force grip pos and not pointer
        handSpaces[RIGHT_HAND].push_back(space);
        __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Registering right hand : %p, %p, %p ", space, path, action);
    }
}

void GetControllerSpacePose(XrTime time, XrSpace space, XrSpace baseSpace, XrSpaceLocation *location){

    //DEBUG
    /*updateHandJoints(time, baseSpace, XR_HAND_LEFT_EXT);

    tryGetPalmPosition(XR_HAND_LEFT_EXT, &location->pose); //FOR NOW*/

    __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "TryGetControllerSpacePose : %p", space);

    for(XrSpace handSpace : handSpaces[LEFT_HAND]){
        __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "HandSpace : %p", handSpace);
    }

    //Update Hand Joints here
    if(std::find(handSpaces[LEFT_HAND].begin(), handSpaces[LEFT_HAND].end(), space) != handSpaces[LEFT_HAND].end()){
        updateHandJoints(time, baseSpace, XR_HAND_LEFT_EXT);

        //We rotate the hand correctly

        XrPosef palmPose;
        tryGetBonePose(XR_HAND_LEFT_EXT, &palmPose, XR_HAND_JOINT_PALM_EXT); //FOR NOW
        glm::quat palmRot = glm::quat(palmPose.orientation.w, palmPose.orientation.x, palmPose.orientation.y, palmPose.orientation.z);
        glm::quat finalRot = palmRot * glm::angleAxis(1.5707963268f, glm::vec3(0.0f,0.0f,-1.0f));
        XrQuaternionf xrFinalRot = XrQuaternionf{
            .w = finalRot.w,
            .x = finalRot.x,
            .y = finalRot.y,
            .z = finalRot.z
        };

        glm::vec3 deltaVec = glm::vec3(0.02f, 0.039f, -0.04f);

        glm::vec3 palmPos = glm::vec3(palmPose.position.x, palmPose.position.y, palmPose.position.z);
        glm::vec3 finalPos = palmPos + finalRot * deltaVec;
        XrVector3f xrFinalPos = XrVector3f{
            .x = finalPos.x,
            .y = finalPos.y,
            .z = finalPos.z
        };

        location->pose.position = xrFinalPos;
        location->pose.orientation = xrFinalRot;

        __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "TryGetControllerSpacePose left : %p", space);
    }
    else if(std::find(handSpaces[RIGHT_HAND].begin(), handSpaces[RIGHT_HAND].end(), space) != handSpaces[RIGHT_HAND].end()){
        updateHandJoints(time, baseSpace, XR_HAND_RIGHT_EXT);
        XrPosef palmPose;


        tryGetBonePose(XR_HAND_RIGHT_EXT, &palmPose, XR_HAND_JOINT_PALM_EXT); //FOR NOW
        glm::quat palmRot = glm::quat(palmPose.orientation.w, palmPose.orientation.x, palmPose.orientation.y, palmPose.orientation.z);
        glm::quat finalRot = palmRot * glm::angleAxis(1.5707963268f, glm::vec3(0.0f,0.0f,1.0f));
        XrQuaternionf xrFinalRot = XrQuaternionf{
                .w = finalRot.w,
                .x = finalRot.x,
                .y = finalRot.y,
                .z = finalRot.z
        };

        glm::vec3 deltaVec = glm::vec3(-0.02f, 0.039f, -0.04f);

        glm::vec3 palmPos = glm::vec3(palmPose.position.x, palmPose.position.y, palmPose.position.z);
        glm::vec3 finalPos = palmPos + finalRot * deltaVec;
        XrVector3f xrFinalPos = XrVector3f{
                .x = finalPos.x,
                .y = finalPos.y,
                .z = finalPos.z
        };

        location->pose.position = xrFinalPos;
        location->pose.orientation = xrFinalRot;


        __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "TryGetControllerSpacePose right : %p", space);
    }
    else return;

    __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "GetControllerSpacePose : %p", space);
    location->locationFlags = XR_SPACE_LOCATION_POSITION_TRACKED_BIT | XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;

    //DEBUG
    /*float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    location->pose.position.x = r;*/
}

void TryRegisterActionSet(XrActionSet actionSet, const char* actionSetName){
    __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Try Registering ActionSet : %p, %s ", actionSet, actionSetName);
    if(strcmp(actionSetName, controllersActionSetName) == 0){
        __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Registering ActionSet : %p, %s ", actionSet, actionSetName);
        controllersActionSet = actionSet;
    }
}

void TryRegisterAction(XrAction action, XrActionSet actionSet, const char* actionName){
    __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Try Registering Action : %p, %s, %p, %p", action, actionName, actionSet, controllersActionSet);

    if(actionSet != controllersActionSet) return;

    for(int i = 0; i < 17; i++){
        if(strcmp(actionName, controllersActionsNames[i]) == 0){
            __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Registering Action : %p, %s, %p ", action, actionName, actionSet);
            controllersActions[i] = action;
        }
    }
}

XrResult GetControllerDefaultBinding(const XrInteractionProfileSuggestedBinding *suggestedBindings){
    if(controllersActionSet == NULL){
        return XR_ERROR_PATH_UNSUPPORTED;
    }

    return XR_SUCCESS;
}

XrResult GetCurrentInteractionProfileBinding(XrInteractionProfileState *interactionProfile){
    if(interactionProfilePath != NULL){
        interactionProfile->interactionProfile = interactionProfilePath;
        return XR_SUCCESS;
    }

    return XR_ERROR_PATH_UNSUPPORTED;
}

//Get actions
void GetControllerActionStateBoolean(const XrActionStateGetInfo *getInfo, XrActionStateBoolean *data){
    XrHandEXT hand = getInfo->subactionPath == handPaths[LEFT_HAND] ? XR_HAND_LEFT_EXT : XR_HAND_RIGHT_EXT;

    uint32_t actionId = 0;
    for(int i = 0; i < 33; i++){
        //__android_log_print(ANDROID_LOG_DEBUG, "TEST_DEBUG_CONTROLLERS", "Action stored : %p", controllersActions[i]);
        if(getInfo->action == controllersActions[i]){
            actionId = i;
            break;
        }
    }

    //__android_log_print(ANDROID_LOG_DEBUG, "TEST_DEBUG_CONTROLLERS", "Action state : %d", hand);

    switch(actionId){
        case TRIGGER_PRESSED:
            UpdateIfTriggerPressed(hand, data);
            break;
        case GRIP_PRESSED:
            UpdateIfGripPressed(hand, data);
            break;
        case PRIMARY_BUTTON:
            UpdateIfPrimaryButtonPressed(hand, data);
            break;
        case SECONDARY_BUTTON:
            UpdateIfSecondaryButtonPressed(hand, data);
            break;

        case TRIGGER_TOUCHED:
            UpdateIfTriggerTouched(hand, data);
            break;

    }
    //UpdateIfTriggerPressed(hand, data);

    data->changedSinceLastSync = XR_TRUE;
    data->lastChangeTime = getCurrentTime();
    data->isActive = XR_TRUE;
}

void GetControllerActionStateFloat(const XrActionStateGetInfo *getInfo, XrActionStateFloat *data){
    XrHandEXT hand = getInfo->subactionPath == handPaths[LEFT_HAND] ? XR_HAND_LEFT_EXT : XR_HAND_RIGHT_EXT;

    uint32_t actionId = 0;
    for(int i = 0; i < 33; i++){
        if(getInfo->action == controllersActions[i]){
            actionId = i;
            break;
        }
    }

    switch(actionId){
        case GRIP:
            UpdateIfGripTouched(hand, data);
            break;
    }
    //UpdateIfTriggerPressed(hand, data);

    data->changedSinceLastSync = XR_TRUE;
    data->lastChangeTime = getCurrentTime();
    data->isActive = XR_TRUE;
}

void GetControllerActionStateVector2f(const XrActionStateGetInfo *getInfo, XrActionStateVector2f *data){
    XrHandEXT hand = getInfo->subactionPath == handPaths[LEFT_HAND] ? XR_HAND_LEFT_EXT : XR_HAND_RIGHT_EXT;

    uint32_t actionId = 0;
    for(int i = 0; i < 33; i++){
        if(getInfo->action == controllersActions[i]){
            actionId = i;
            break;
        }
    }

    switch(actionId){
        case THUMBSTICK:
            UpdateJoystickInput(hand, data);
            break;
    }
    //UpdateIfTriggerPressed(hand, data);

    data->changedSinceLastSync = XR_TRUE;
    data->lastChangeTime = getCurrentTime();
    data->isActive = XR_TRUE;
}



//Controllers Binding
bool triggerLeftLastState = false;
bool triggerRightLastState = false;
void UpdateIfTriggerPressed(XrHandEXT hand, XrActionStateBoolean *data){
    XrPosef palmPose;
    XrPosef indexTipPose;
    tryGetBonePose(hand, &palmPose, XR_HAND_JOINT_PALM_EXT);
    tryGetBonePose(hand, &indexTipPose, XR_HAND_JOINT_INDEX_DISTAL_EXT);
    //__android_log_print(ANDROID_LOG_DEBUG, "PICOEMU", "Getting poses : ");

    glm::vec3 palmGlobalPosition = glm::vec3(palmPose.position.x, palmPose.position.y, palmPose.position.z);//GLM_POS(palmPose);
    glm::vec3 indexGlobalPosition = glm::vec3(indexTipPose.position.x, indexTipPose.position.y, indexTipPose.position.z);//GLM_POS(indexTipPose);

    //glm::quat palmRot = glm::quat(palmPose.orientation.w, palmPose.orientation.x, palmPose.orientation.y, palmPose.orientation.z);//GLM_QUAT(palmPose);

    //glm::vec3 indexLocalPos = inverse(palmRot) * (indexGlobalPosition - palmGlobalPosition);

    float distSquare = (palmPose.position.x - indexTipPose.position.x)*(palmPose.position.x - indexTipPose.position.x)
                     + (palmPose.position.y - indexTipPose.position.y)*(palmPose.position.y - indexTipPose.position.y)
                     + (palmPose.position.z - indexTipPose.position.z)*(palmPose.position.z - indexTipPose.position.z);

    bool isActive = distSquare <= 0.005f;//0.03f*0.03f; //TODO CHANGE VALUE

    if(hand == XR_HAND_LEFT_EXT){
        bool changedActive = triggerLeftLastState != isActive;
        triggerLeftLastState = isActive;
        data->changedSinceLastSync = changedActive;
    }
    else{
        bool changedActive = triggerRightLastState != isActive;
        triggerRightLastState = isActive;
        data->changedSinceLastSync = changedActive;
    }


    __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Hand %d : Distance between palm and index : %.6f, palmGlobalPos : %.6f,%.6f,%.6f , indexTipGlobalPos : %.6f,%.6f,%.6f , isActive %d ", hand, distSquare,
                        indexTipPose.position.x, indexTipPose.position.y, indexTipPose.position.z, indexTipPose.position.x, indexTipPose.position.y, indexTipPose.position.z, isActive);


    data->currentState = isActive;

}

bool gripLeftLastState = false;
bool gripRightLastState = false;
void UpdateIfGripPressed(XrHandEXT hand, XrActionStateBoolean *data){
    XrPosef palmPose;
    XrPosef indexTipPose;
    tryGetBonePose(hand, &palmPose, XR_HAND_JOINT_PALM_EXT);
    tryGetBonePose(hand, &indexTipPose, XR_HAND_JOINT_MIDDLE_DISTAL_EXT);
    //__android_log_print(ANDROID_LOG_DEBUG, "PICOEMU", "Getting poses : ");

    glm::vec3 palmGlobalPosition = glm::vec3(palmPose.position.x, palmPose.position.y, palmPose.position.z);//GLM_POS(palmPose);
    glm::vec3 indexGlobalPosition = glm::vec3(indexTipPose.position.x, indexTipPose.position.y, indexTipPose.position.z);//GLM_POS(indexTipPose);

    //glm::quat palmRot = glm::quat(palmPose.orientation.w, palmPose.orientation.x, palmPose.orientation.y, palmPose.orientation.z);//GLM_QUAT(palmPose);

    //glm::vec3 indexLocalPos = inverse(palmRot) * (indexGlobalPosition - palmGlobalPosition);

    float distSquare = (palmPose.position.x - indexTipPose.position.x)*(palmPose.position.x - indexTipPose.position.x)
                       + (palmPose.position.y - indexTipPose.position.y)*(palmPose.position.y - indexTipPose.position.y)
                       + (palmPose.position.z - indexTipPose.position.z)*(palmPose.position.z - indexTipPose.position.z);

    bool isActive = distSquare <= 0.005f;//0.03f*0.03f; //TODO CHANGE VALUE

    if(hand == XR_HAND_LEFT_EXT){
        bool changedActive = gripLeftLastState != isActive;
        gripLeftLastState = isActive;
        data->changedSinceLastSync = changedActive;
    }
    else{
        bool changedActive = gripRightLastState != isActive;
        gripRightLastState = isActive;
        data->changedSinceLastSync = changedActive;
    }


    __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Hand %d : Distance between palm and middle : %.6f, palmGlobalPos : %.6f,%.6f,%.6f , middleTipGlobalPos : %.6f,%.6f,%.6f , isActive %d ", hand, distSquare,
                        indexTipPose.position.x, indexTipPose.position.y, indexTipPose.position.z, indexTipPose.position.x, indexTipPose.position.y, indexTipPose.position.z, isActive);


    data->currentState = isActive;

}

bool primaryLeftLastState = false;
bool primaryRightLastState = false;
void UpdateIfPrimaryButtonPressed(XrHandEXT hand, XrActionStateBoolean *data){
    XrPosef thumbDistalPos;
    XrPosef indexProximalPose;
    tryGetBonePose(hand, &thumbDistalPos, XR_HAND_JOINT_THUMB_DISTAL_EXT);
    tryGetBonePose(hand, &indexProximalPose, XR_HAND_JOINT_INDEX_PROXIMAL_EXT);
    //__android_log_print(ANDROID_LOG_DEBUG, "PICOEMU", "Getting poses : ");

    glm::vec3 thumbDistalGlobalPosition = glm::vec3(thumbDistalPos.position.x, thumbDistalPos.position.y, thumbDistalPos.position.z);//GLM_POS(thumbDistalPos);
    glm::vec3 indexMetacarpalGlobalPosition = glm::vec3(indexProximalPose.position.x, indexProximalPose.position.y, indexProximalPose.position.z);//GLM_POS(indexProximalPose);

    //glm::quat palmRot = glm::quat(thumbDistalPos.orientation.w, thumbDistalPos.orientation.x, thumbDistalPos.orientation.y, thumbDistalPos.orientation.z);//GLM_QUAT(thumbDistalPos);

    //glm::vec3 indexLocalPos = inverse(palmRot) * (indexGlobalPosition - palmGlobalPosition);

    float distSquare = (thumbDistalPos.position.x - indexProximalPose.position.x) * (thumbDistalPos.position.x - indexProximalPose.position.x)
                       + (thumbDistalPos.position.y - indexProximalPose.position.y) * (thumbDistalPos.position.y - indexProximalPose.position.y)
                       + (thumbDistalPos.position.z - indexProximalPose.position.z) * (thumbDistalPos.position.z - indexProximalPose.position.z);

    bool isActive = distSquare <= 0.002f;//0.03f*0.03f; //TODO CHANGE VALUE 0.001270 min, 0.004 max

    if(hand == XR_HAND_LEFT_EXT){
        bool changedActive = primaryLeftLastState != isActive;
        primaryLeftLastState = isActive;
        data->changedSinceLastSync = changedActive;
    }
    else{
        bool changedActive = primaryRightLastState != isActive;
        primaryRightLastState = isActive;
        data->changedSinceLastSync = changedActive;
    }


    __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Hand %d : Distance between index proximal and thumb distal : %.6f, first : %.6f,%.6f,%.6f , second : %.6f,%.6f,%.6f , isActive %d", hand, distSquare,
                        indexProximalPose.position.x, indexProximalPose.position.y, indexProximalPose.position.z, indexProximalPose.position.x, indexProximalPose.position.y, indexProximalPose.position.z, isActive);


    data->currentState = isActive;
}

bool secondaryLeftLastState = false;
bool secondaryRightLastState = false;
void UpdateIfSecondaryButtonPressed(XrHandEXT hand, XrActionStateBoolean *data){
    XrPosef thumbDistalPos;
    XrPosef indexProximalPose;
    XrPosef palmPose;

    tryGetBonePose(hand, &thumbDistalPos, XR_HAND_JOINT_THUMB_DISTAL_EXT);
    tryGetBonePose(hand, &indexProximalPose, XR_HAND_JOINT_INDEX_PROXIMAL_EXT);
    tryGetBonePose(hand, &palmPose, XR_HAND_JOINT_PALM_EXT);
    //__android_log_print(ANDROID_LOG_DEBUG, "PICOEMU", "Getting poses : ");

    glm::vec3 thumbDistalGlobalPosition = glm::vec3(thumbDistalPos.position.x, thumbDistalPos.position.y, thumbDistalPos.position.z);//GLM_POS(thumbDistalPos);
    glm::vec3 indexMetacarpalGlobalPosition = glm::vec3(indexProximalPose.position.x, indexProximalPose.position.y, indexProximalPose.position.z);//GLM_POS(indexProximalPose);
    glm::vec3 palmGlobalPosition = glm::vec3(palmPose.position.x, palmPose.position.y, palmPose.position.z);

    glm::quat palmRot = glm::quat(palmPose.orientation.w, palmPose.orientation.x, palmPose.orientation.y, palmPose.orientation.z);//GLM_QUAT(thumbDistalPos);

    glm::vec3 indexLocalPos = inverse(palmRot) * (thumbDistalGlobalPosition - palmGlobalPosition);

    float distSquare = (thumbDistalPos.position.x - indexProximalPose.position.x) * (thumbDistalPos.position.x - indexProximalPose.position.x)
                       + (thumbDistalPos.position.y - indexProximalPose.position.y) * (thumbDistalPos.position.y - indexProximalPose.position.y)
                       + (thumbDistalPos.position.z - indexProximalPose.position.z) * (thumbDistalPos.position.z - indexProximalPose.position.z);

    bool isActive = distSquare >= 0.0035f;//0.03f*0.03f; //TODO CHANGE VALUE 0.001270 min, 0.004 max (0.004 a bit too hard, switched to .0035

    float distanceToExpected = glm::distance(indexLocalPos, SECONDARY_BUTTON_LOC);

    //if(distanceToExpected > SECONDARY_BUTTON_DISTANCE) isActive = false;

    if(hand == XR_HAND_LEFT_EXT){
        bool changedActive = secondaryLeftLastState != isActive;
        secondaryLeftLastState = isActive;
        data->changedSinceLastSync = changedActive;
    }
    else{
        bool changedActive = secondaryRightLastState != isActive;
        secondaryRightLastState = isActive;
        data->changedSinceLastSync = changedActive;
    }


    __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Hand %d : Distance between index proximal and thumb distal : %.6f, first : %.6f,%.6f,%.6f , second : %.6f,%.6f,%.6f , isActive %d", hand, distSquare,
                        indexProximalPose.position.x, indexProximalPose.position.y, indexProximalPose.position.z, indexProximalPose.position.x, indexProximalPose.position.y, indexProximalPose.position.z, isActive);


    data->currentState = isActive;
}


//EACH POSITION
/*
 * MIDDLE : (0.029615,-0.056456,-0.024166)
 * UP : (0.019883,-0.063061,-0.028699)
 * DOWN : (0.047657,-0.043240,-0.013707)
 * LEFT : (-0.043221,-0.055522,-0.028632)
 * RIGHT : (0.008095,-0.071079,-0.026443)
 */
/*void UpdateJoystickInput(XrHandEXT hand, XrActionStateVector2f* data){
    XrPosef palmPose;
    XrPosef thumbTipPose;
    tryGetBonePose(hand, &palmPose, XR_HAND_JOINT_PALM_EXT);
    tryGetBonePose(hand, &thumbTipPose, XR_HAND_JOINT_THUMB_DISTAL_EXT);
    //__android_log_print(ANDROID_LOG_DEBUG, "PICOEMU", "Getting poses : ");

    glm::vec3 palmGlobalPosition = glm::vec3(palmPose.position.x, palmPose.position.y, palmPose.position.z);//GLM_POS(palmPose);
    glm::vec3 thumbGlobalPosition = glm::vec3(thumbTipPose.position.x, thumbTipPose.position.y, thumbTipPose.position.z);//GLM_POS(indexTipPose);

    glm::quat palmRot = glm::quat(palmPose.orientation.w, palmPose.orientation.x, palmPose.orientation.y, palmPose.orientation.z);//GLM_QUAT(palmPose);

    glm::vec3 localPalmPosition = glm::inverse(palmRot) * (thumbGlobalPosition - palmGlobalPosition);

    __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Hand %d : Positions , first : %.6f,%.6f,%.6f", hand,
                        localPalmPosition.x, localPalmPosition.y, localPalmPosition.z);

    glm::vec3 normale = glm::triangleNormal(THUMBSTICK_LEFT, THUMBSTICK_RIGHT, THUMBSTICK_UP-THUMBSTICK_DOWN);
    float distanceAuPlan = glm::dot(localPalmPosition - THUMBSTICK_MIDDLE, normale);

    glm::vec3 pointProjete = localPalmPosition - distanceAuPlan * normale;*/


    // Afficher les résultats
    //__android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Distance du point au plan : %.6f", distanceAuPlan);
    /*__android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Coordonnée par rapport à haut : %.6f", coordHaut);
    __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Coordonnée par rapport à bas : %.6f", coordBas);
    __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Coordonnée par rapport à gauche : %.6f", coordGauche);
    __android_log_print(ANDROID_LOG_DEBUG, "PICOR2", "Coordonnée par rapport à droite : %.6f", coordDroite);*/

    //TODO CHECK DISTANCE -0.015
    /*if(abs(distanceAuPlan) > 0.015) return;


    float upDist = glm::distance(localPalmPosition - THUMBSTICK_MIDDLE, THUMBSTICK_UP - THUMBSTICK_MIDDLE);
    float downDist = glm::distance(localPalmPosition - THUMBSTICK_MIDDLE, THUMBSTICK_DOWN - THUMBSTICK_MIDDLE);
    float rightDist = glm::distance(localPalmPosition - THUMBSTICK_MIDDLE, THUMBSTICK_RIGHT - THUMBSTICK_MIDDLE);
    float leftDist = glm::distance(localPalmPosition - THUMBSTICK_MIDDLE, THUMBSTICK_LEFT - THUMBSTICK_MIDDLE);

    float upDirRatio = upDist / (upDist + downDist);
    float y = 2.0f*upDirRatio - 1.0f;

    float rightDirRatio = rightDist / (rightDist + leftDist);
    float x = 2.0f*rightDirRatio - 1.0f;

    if(hand == XR_HAND_LEFT_EXT){
        x = x;
    }

    data->changedSinceLastSync = true;
    data->isActive = true;
    data->currentState = XrVector2f{
        .x = -x,
        .y = -y
    };

}*/

/*void UpdateJoystickInput(XrHandEXT hand, XrActionStateVector2f* data){

    if(hand != XR_HAND_LEFT_EXT) return;

    XrPosef leftPalmPose;
    XrPosef rightPalmPose;
    tryGetBonePose(XR_HAND_LEFT_EXT, &leftPalmPose, XR_HAND_JOINT_PALM_EXT);
    tryGetBonePose(XR_HAND_RIGHT_EXT, &rightPalmPose, XR_HAND_JOINT_PALM_EXT);

    glm::vec3 leftPalmPosition = glm::vec3(leftPalmPose.position.x, leftPalmPose.position.y, leftPalmPose.position.z);//GLM_POS(palmPose);
    glm::vec3 rightPalmPosition = glm::vec3(rightPalmPose.position.x, rightPalmPose.position.y, rightPalmPose.position.z);//GLM_POS(indexTipPose);


    data->changedSinceLastSync = true;
    data->isActive = true;
    data->currentState = XrVector2f{
        .x = 0,
        .y = (glm::distance(leftPalmPosition, rightPalmPosition) < 0.20f) ? 1.0f : 0.0f
    };

}*/


const float joystickSensibility = 10.0f;

bool leftJoystickActive = false;
glm::vec3 leftJoystickOrigin;
bool rightJoystickActive = false;
glm::vec3 rightJoystickOrigin;
void UpdateJoystickInput(XrHandEXT hand, XrActionStateVector2f* data){
    XrPosef palmPose;
    XrPosef ringTipPose;
    tryGetBonePose(hand, &palmPose, XR_HAND_JOINT_PALM_EXT);
    tryGetBonePose(hand, &ringTipPose, XR_HAND_JOINT_RING_DISTAL_EXT);

    glm::vec3 palmGlobalPosition = glm::vec3(palmPose.position.x, palmPose.position.y, palmPose.position.z);//GLM_POS(palmPose);
    glm::vec3 indexGlobalPosition = glm::vec3(ringTipPose.position.x, ringTipPose.position.y, ringTipPose.position.z);//GLM_POS(indexTipPose);


    float distSquare = (palmPose.position.x - ringTipPose.position.x)*(palmPose.position.x - ringTipPose.position.x)
                       + (palmPose.position.y - ringTipPose.position.y)*(palmPose.position.y - ringTipPose.position.y)
                       + (palmPose.position.z - ringTipPose.position.z)*(palmPose.position.z - ringTipPose.position.z);

    bool isActive = distSquare <= 0.006f;

    glm::vec3 deltaVec = glm::vec3(0.0f,0.0f,0.0f);

    if(hand == XR_HAND_LEFT_EXT){
        if(!isActive){
            leftJoystickActive = false;
            return;
        }

        if(!leftJoystickActive){
            leftJoystickOrigin = palmGlobalPosition;
            leftJoystickActive = true;
        }

        deltaVec = palmGlobalPosition - leftJoystickOrigin;
    }
    else{
        if(!isActive){
            rightJoystickActive = false;
            return;
        }

        if(!rightJoystickActive){
            rightJoystickOrigin = palmGlobalPosition;
            rightJoystickActive = true;
        }

        deltaVec = palmGlobalPosition - rightJoystickOrigin;
    }

    data->changedSinceLastSync = true;
    data->isActive = true;
    data->currentState = XrVector2f{
            .x = joystickSensibility*deltaVec.x,
            .y = -joystickSensibility*deltaVec.z
    };

}

bool triggerTouchedLeftLastState = false;
bool triggerTouchedRightLastState = false;
void UpdateIfTriggerTouched(XrHandEXT hand, XrActionStateBoolean *data){
    XrPosef indexTipPose;
    XrPosef indexDistalPose;
    XrPosef indexMiddlePose;
    XrPosef indexProximalPose;
    tryGetBonePose(hand, &indexTipPose, XR_HAND_JOINT_INDEX_TIP_EXT);
    tryGetBonePose(hand, &indexDistalPose, XR_HAND_JOINT_INDEX_DISTAL_EXT);
    tryGetBonePose(hand, &indexMiddlePose, XR_HAND_JOINT_INDEX_INTERMEDIATE_EXT);
    tryGetBonePose(hand, &indexProximalPose, XR_HAND_JOINT_INDEX_PROXIMAL_EXT);

    glm::vec3 indexTipPos = glm::vec3(indexTipPose.position.x, indexTipPose.position.y, indexTipPose.position.z);
    glm::vec3 indexDistalPos = glm::vec3(indexDistalPose.position.x, indexDistalPose.position.y, indexDistalPose.position.z);
    glm::vec3 indexMiddlePos = glm::vec3(indexMiddlePose.position.x, indexMiddlePose.position.y, indexMiddlePose.position.z);
    glm::vec3 indexProximalPos = glm::vec3(indexProximalPose.position.x, indexProximalPose.position.y, indexProximalPose.position.z);

    glm::vec3 deltaVec1 = glm::normalize(indexTipPos - indexDistalPos);
    glm::vec3 deltaVec2 = glm::normalize(indexDistalPos - indexMiddlePos);
    glm::vec3 deltaVec3 = glm::normalize(indexMiddlePos - indexProximalPos);

    float alinement = 0.5f * (glm::dot(deltaVec1, deltaVec2) + glm::dot(deltaVec2, deltaVec3));

    bool isActive = alinement < 0.8f;

    __android_log_print(ANDROID_LOG_DEBUG, "PICOC2", "Trigger Alinement : %.6f", alinement);

    //isActive = true;

    if(hand == XR_HAND_LEFT_EXT){
        bool changedActive = triggerTouchedLeftLastState != isActive;
        triggerTouchedLeftLastState = isActive;
        data->changedSinceLastSync = changedActive;
    }
    else{
        bool changedActive = triggerTouchedRightLastState != isActive;
        triggerTouchedRightLastState = isActive;
        data->changedSinceLastSync = changedActive;
    }

    data->isActive = XR_TRUE;
    data->currentState = isActive;

}

bool gripTouchedLeftLastState = false;
bool gripTouchedRightLastState = false;
void UpdateIfGripTouched(XrHandEXT hand, XrActionStateFloat* data){
    XrPosef middleTipPose;
    XrPosef middleDistalPose;
    XrPosef middleMiddlePose;
    XrPosef middleProximalPose;
    tryGetBonePose(hand, &middleTipPose, XR_HAND_JOINT_MIDDLE_TIP_EXT);
    tryGetBonePose(hand, &middleDistalPose, XR_HAND_JOINT_MIDDLE_DISTAL_EXT);
    tryGetBonePose(hand, &middleMiddlePose, XR_HAND_JOINT_MIDDLE_INTERMEDIATE_EXT);
    tryGetBonePose(hand, &middleProximalPose, XR_HAND_JOINT_MIDDLE_PROXIMAL_EXT);

    glm::vec3 middleTipPos = glm::vec3(middleTipPose.position.x, middleTipPose.position.y, middleTipPose.position.z);
    glm::vec3 middleDistalPos = glm::vec3(middleDistalPose.position.x, middleDistalPose.position.y, middleDistalPose.position.z);
    glm::vec3 middleMiddlePos = glm::vec3(middleMiddlePose.position.x, middleMiddlePose.position.y, middleMiddlePose.position.z);
    glm::vec3 middleProximalPos = glm::vec3(middleProximalPose.position.x, middleProximalPose.position.y, middleProximalPose.position.z);

    glm::vec3 deltaVec1 = glm::normalize(middleTipPos - middleDistalPos);
    glm::vec3 deltaVec2 = glm::normalize(middleDistalPos - middleMiddlePos);
    glm::vec3 deltaVec3 = glm::normalize(middleMiddlePos - middleProximalPos);

    float alinement = 0.5f * (glm::dot(deltaVec1, deltaVec2) + glm::dot(deltaVec2, deltaVec3));

    bool isActive = alinement < 0.8f;

    __android_log_print(ANDROID_LOG_DEBUG, "PICOC2", "Grip Alinement : %.6f", alinement);

    //isActive = true;

    if(hand == XR_HAND_LEFT_EXT){
        bool changedActive = gripTouchedLeftLastState != isActive;
        gripTouchedLeftLastState = isActive;
        data->changedSinceLastSync = changedActive;
    }
    else{
        bool changedActive = gripTouchedRightLastState != isActive;
        gripTouchedRightLastState = isActive;
        data->changedSinceLastSync = changedActive;
    }

    data->isActive = XR_TRUE;
    data->currentState = isActive ? 1.0f : 0.0f;

}

bool joystickTouchedLeftLastState = false;
bool joystickTouchedRightLastState = false;
void UpdateIfJoystickTouched(XrHandEXT hand, XrActionStateBoolean *data){
    XrPosef thumbTipPose;
    XrPosef thumbDistalPose;
    XrPosef thumbMiddlePose;
    tryGetBonePose(hand, &thumbTipPose, XR_HAND_JOINT_THUMB_TIP_EXT);
    tryGetBonePose(hand, &thumbDistalPose, XR_HAND_JOINT_THUMB_DISTAL_EXT);
    tryGetBonePose(hand, &thumbMiddlePose, XR_HAND_JOINT_THUMB_PROXIMAL_EXT);

    glm::vec3 thumbTipPos = glm::vec3(thumbTipPose.position.x, thumbTipPose.position.y, thumbTipPose.position.z);
    glm::vec3 thumbDistalPos = glm::vec3(thumbDistalPose.position.x, thumbDistalPose.position.y, thumbDistalPose.position.z);
    glm::vec3 thumbMiddlePos = glm::vec3(thumbMiddlePose.position.x, thumbMiddlePose.position.y, thumbMiddlePose.position.z);

    glm::vec3 deltaVec1 = glm::normalize(thumbTipPos - thumbDistalPos);
    glm::vec3 deltaVec2 = glm::normalize(thumbDistalPos - thumbMiddlePos);

    float alinement = glm::dot(deltaVec1, deltaVec2);

    bool isActive = alinement < 0.8f;

    __android_log_print(ANDROID_LOG_DEBUG, "PICOC2", "Thumb Alinement : %.6f", alinement);

    //isActive = true;

    if(hand == XR_HAND_LEFT_EXT){
        bool changedActive = triggerTouchedLeftLastState != isActive;
        triggerTouchedLeftLastState = isActive;
        data->changedSinceLastSync = changedActive;
    }
    else{
        bool changedActive = triggerTouchedRightLastState != isActive;
        triggerTouchedRightLastState = isActive;
        data->changedSinceLastSync = changedActive;
    }

    data->isActive = XR_TRUE;
    data->currentState = isActive;

}
