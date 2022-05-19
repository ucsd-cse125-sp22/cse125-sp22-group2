#define GLM_FORCE_RADIANS
#define _USE_MATH_DEFINES
#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <math.h>

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <cstring>
#include <map>
#include <stack>

#include "../../../Definitions.hpp"

class Animation {
private: 

	std::vector<glm::vec3> rotationChannel; 
	std::vector<glm::vec3> translationChannel; 
	std::vector<glm::vec3> scaleChannel; 

	glm::mat4 currentTransform = glm::mat4(1.0f); 

	bool triggered; 
	int anim_speed = 5; 
	int frameNum; 
	int count; 

	int anim_length = 0; 

	// Get channel values 
	glm::mat4 getRotation(int frameNum);

	glm::mat4 getTranslation(int frameNum);

	glm::mat4 getScale(int frameNum);

	glm::mat4 getTransform(int frameNum) {
		glm::mat4 trans = getTranslation(frameNum) * getScale(frameNum) * getRotation(frameNum); 

		/*
		std::cout << "transformation: " << std::endl; 
		glm::mat4 car_transform = glm::mat4(1.0f) * getRotation(frameNum);
		std::cout << car_transform[0][0] << " " << car_transform[0][1] << " " << car_transform[0][2] << " " << car_transform[0][3] << std::endl;
		std::cout << car_transform[1][0] << " " << car_transform[1][1] << " " << car_transform[1][2] << " " << car_transform[1][3] << std::endl;
		std::cout << car_transform[2][0] << " " << car_transform[2][1] << " " << car_transform[2][2] << " " << car_transform[2][3] << std::endl;
		std::cout << car_transform[3][0] << " " << car_transform[3][1] << " " << car_transform[3][2] << " " << car_transform[3][3] << std::endl;
		*/

		return trans;
	}

public:

	Animation() {
		triggered = false; 
		frameNum = -1;
		count = 0; 
	};

	bool readAnimation(const char* path);

	void triggerAnimation(bool trigger = true); 

	void updateAnimation(); 

	glm::mat4& getCurrentTransform() { return currentTransform; };

};
#endif
