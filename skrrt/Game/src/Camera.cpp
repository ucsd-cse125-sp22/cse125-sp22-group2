#include "Camera.h"
#define _USE_MATH_DEFINES
#include <math.h>

// Quaternion multiplication p * q
glm::vec4 qmultiply(const glm::vec4 p, const glm::vec4 q){
    const float p_re = p.w;
    const float q_re = q.w;
    const glm::vec3 p_im(p.x,p.y,p.z);
    const glm::vec3 q_im(q.x,q.y,q.z);
    float r_re = p_re * q_re - glm::dot(p_im,q_im);
    glm::vec3 r_im = p_re * q_im + q_re * p_im + glm::cross(p_im,q_im);
    glm::vec4 r = glm::vec4(r_im,r_re);
    return r;
}
// Quaternion conjugation
glm::vec4 qconj(const glm::vec4 q){return glm::vec4(-q.x,-q.y,-q.z,q.w);}

glm::mat3 rotation(const float degrees,const glm::vec3 axis){
    const float angle = degrees * (float)M_PI/180.0f; // convert to radians
    const glm::vec3 a = glm::normalize(axis);
    glm::mat3 R;
    glm::vec4 q = glm::vec4( glm::sin(0.5f*angle)*a, glm::cos(0.5f*angle) );
    glm::vec4 ii(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 jj(0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 kk(0.0f, 0.0f, 1.0f, 0.0f);
    R[0] = glm::vec3(qmultiply(q,qmultiply(ii,qconj(q))));
    R[1] = glm::vec3(qmultiply(q,qmultiply(jj,qconj(q))));
    R[2] = glm::vec3(qmultiply(q,qmultiply(kk,qconj(q))));
    return R;
    //return glm::mat3(1.0f);
}

//degrees is not in rad
void Camera::rotateRight(const float degrees){
    glm::vec3 targetToEye = eye - target;
    
    //glm::vec3 axis = glm::cross(targetToEye, up);
    glm::mat3 rotMat = rotation(degrees, glm::vec3(0.0f,1.0f,0.0f));
    glm::vec3 targetToEye_new = targetToEye * rotMat;
    eye = target + targetToEye_new;
    up = up * rotMat;
}

//degrees is not in rad
void Camera::rotateUp(const float degrees){
    glm::vec3 targetToEye = eye - target;

    //make sure we don't go over y pole
    float angleFromXZ = glm::asin(-1.0f * targetToEye.y / glm::length(targetToEye)); // in rad, positive is above xz plane
    angleFromXZ = angleFromXZ * 180.0f/(float)M_PI; // convert to degreees
    float newAngleFromXZ = degrees - angleFromXZ;
    if (newAngleFromXZ > MAX_Y_ANGLE || newAngleFromXZ < MIN_Y_ANGLE) {
        return;
    }
    
    glm::vec3 axis = glm::cross(targetToEye, up);
    glm::mat3 rotMat = rotation(-1.0f * degrees, axis); // invert controls with -1
    glm::vec3 targetToEye_new = targetToEye * rotMat;

    eye = target + targetToEye_new;
    up = up * rotMat;
}
void Camera::zoom(const float factor){
    eye = target + factor * (eye - target);
}

// Returns a normalized vector in the xz plane that represents the forward direction of the camera
glm::vec3 Camera::forwardVectorXZ() {
    glm::vec3 targetToEye = eye - target;
    
    // calculate sideways vector
    glm::vec3 directionToMove = glm::cross(targetToEye, up);
    directionToMove.y = 0.0f; // Only want to move in xz plane, so remove y component.
    
    // vector is now perpendicular to sideways vector
    directionToMove = glm::vec3(-1.0f * directionToMove.z, 0.0f, directionToMove.x);

    directionToMove = glm::normalize(directionToMove);
    return directionToMove;
}

// Returns a normalized vector in the xz plane that represents the left direction of the camera (90 degrees left of forward)
glm::vec3 Camera::leftVectorXZ() {
    glm::vec3 targetToEye = eye - target;
    
    // calculate sideways vector
    glm::vec3 directionToMove = glm::cross(targetToEye, up);
    directionToMove.y = 0.0f; // Only want to move in xz plane, so remove y component.
    
    directionToMove = glm::normalize(directionToMove);
    return directionToMove;
}

void Camera::movePosition(const float distance, const glm::vec3 direction) {
    glm::vec3 delta = direction * distance;

    target += delta;
    eye += delta; 
}

void Camera::setPosition(const glm::vec3 newTarget, bool clamped) {
    glm::vec3 delta = newTarget - target;

    target = newTarget;
    eye += delta; 

    // Potentially use for falling/respawning, needs some tweaking
    //if (clamped && newTarget.y < 0.0f) {
    //    eye.y = std::max(eye.y, -(newTarget.y / 2.0f));
    //    eye.x = (eye.x + newTarget.x) / 2.0f;
    //    eye.z = (eye.z + newTarget.z) / 2.0f;
    //}
}

void Camera::computeMatrices(){
    // Note that glm matrix column majored.
    // That is, A[i] is the ith column of A,
    // and A_{ij} in math notation is A[j][i] in glm notation.
    
    // VIEW MATRIX
    glm::vec3 targetToEye = eye - target;
    // camera basis Cex, Cey, Cez
    glm::vec3 Cez = glm::normalize(targetToEye);
    glm::vec3 Cey = glm::normalize(up - glm::dot(Cez,up)*Cez);
    glm::vec3 Cex = glm::cross(Cey,Cez);
    // camera's model matrix
    glm::mat4 C;
    C[0] = glm::vec4(Cex,0.0f);
    C[1] = glm::vec4(Cey,0.0f);
    C[2] = glm::vec4(Cez,0.0f);
    C[3] = glm::vec4(eye,1.0f);
    // view matrix is the inverse of camera's model matrix
    view = glm::inverse(C);
    
    // PROJECTION MATRIX
    proj = glm::mat4(1.0f);
    float fovy_rad = fovy * (float)M_PI/180.0f;
    float tan = glm::tan(fovy_rad/2.0f);
    proj = glm::mat4(1.0f/(aspect*tan), 0.0f,     0.0f, 0.0f,
                     0.0f,              1.0f/tan, 0.0f, 0.0f,
                     0.0f, 0.0f,    -(farPlane+nearPlane)/(farPlane- nearPlane), -1.0f,
                     0.0f, 0.0f, -2.0f*farPlane* nearPlane /(farPlane- nearPlane), 0.0f);
}

void Camera::reset(){
    eye = eye_default;// position of the eye
    target = target_default;  // look at target
    up = up_default;      // up vector
    fovy = fovy_default;  // field of view in degrees
    aspect = aspect_default; // aspect ratio
    nearPlane = near_default; // near clipping distance
    farPlane = far_default; // far clipping distance
    zoom(3.0f);
}

void Camera::reset(int playerId)
{
    // TODO: Figure out eye positions for each player
    switch (playerId) {
    case 0:     
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    default:
        break;
    }
    eye = eye_default;
    target = target_default;  // look at target
    up = up_default;      // up vector
    fovy = fovy_default;  // field of view in degrees
    aspect = aspect_default; // aspect ratio
    nearPlane = near_default; // near clipping distance
    farPlane = far_default; // far clipping distance
    zoom(3.0f);
}
