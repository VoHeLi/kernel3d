#pragma once

#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"

#include <vector>

class SpatialObject {

public:
    SpatialObject(glm::vec3 position, glm::quat rotation, glm::vec3 scale, int textureId);
    ~SpatialObject();
    glm::vec3 _position;
    glm::quat _rotation;
    glm::vec3 _scale;
    glm::mat4x4 getTransformationMatrix();

//RENDERER STUFF
int _textureId;
    /*static uint64_t _GlobalIdCounter;
    TODO FOR LATER USE
    uint64_t _id;*/


};

