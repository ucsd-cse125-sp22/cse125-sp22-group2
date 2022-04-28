/**************************************************
Camera is a class for a camera object.
*****************************************************/
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#pragma once

class Camera {
public:
    glm::vec3 eye;// position of the eye
    glm::vec3 target;  // look at target
    glm::vec3 up;      // up vector
    float fovy;  // field of view in degrees
    float aspect; // aspect ratio
    float near; // near clipping distance
    float far; // far clipping distance

    const float MAX_Y_ANGLE = 86.0f; // Camera will not rotate above this angle
    const float MIN_Y_ANGLE = -1.0f * MAX_Y_ANGLE; // Camera will not rotate below this angle
    
    // default values for reset
    glm::vec3 eye_default = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 target_default = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up_default = glm::vec3(0.0f, 1.0f, 0.0f);
    float fovy_default = 30.0f;
    float aspect_default = 4.0f/3.0f;
    float near_default = 0.01f;
    float far_default = 100.0f;
    
    glm::mat4 view = glm::mat4(1.0f);   // view matrix
    glm::mat4 proj = glm::mat4(1.0f);   // projection matrix
    
    void rotateRight(const float degrees);
    void rotateUp(const float degrees);
    void zoom(const float factor);
    void computeMatrices(void);
    void reset(void);

    void movePosition(const float distance, const glm::vec3 direction);
    glm::vec3 Camera::forwardVectorXZ();
    glm::vec3 Camera::leftVectorXZ();
};
