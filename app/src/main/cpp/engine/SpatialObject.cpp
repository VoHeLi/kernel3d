#include "SpatialObject.h"

SpatialObject::SpatialObject(glm::vec3 position, glm::quat rotation, glm::vec3 scale, int textureId) {
    _position = position;
    _rotation = rotation;
    _scale = scale;
    _textureId = textureId;
}

SpatialObject::~SpatialObject() {

}

glm::mat4x4 SpatialObject::getTransformationMatrix() {
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), _position);
    glm::mat4 rotationMatrix = glm::mat4_cast(_rotation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), _scale);

    glm::mat4 transformationMatrix = translationMatrix * rotationMatrix * scaleMatrix;

    return transformationMatrix;
}


