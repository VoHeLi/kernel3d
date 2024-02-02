#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"

#include <vector>

class SpatialObject {

public:
    SpatialObject(glm::vec3 position, glm::quat rotation, glm::vec3 scale);
    ~SpatialObject();

    glm::mat4x4 getTransformationMatrix();

private:
    /*static uint64_t _GlobalIdCounter;
    TODO FOR LATER USE
    uint64_t _id;*/

    glm::vec3 _position;
    glm::quat _rotation;
    glm::vec3 _scale;
};

