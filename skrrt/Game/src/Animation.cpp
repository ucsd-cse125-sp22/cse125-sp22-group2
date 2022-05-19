#include "Animation.h"
#include "Debug.h"

bool Animation::readAnimation(const char* path) {

	if (DEBUG_LEVEL >= LOG_LEVEL_INFO) {
		printf("Loading animation file %s...\n", path);
	}

	FILE* file = fopen(path, "r");

	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? \n");
		getchar();
		return false;
	}

	while (1) {
		char lineHeader[128]; 
		// read the first word of the line 
		int res = fscanf(file, "%s", lineHeader); 
		// if reached end of file, stop parsing 
		if (res == EOF) {
			break;
		}

		// otherwise, continue parsing file

		if (strcmp(lineHeader, "r") == 0) {
			glm::vec3 rotationVector; 
			int frameNumber; 
			fscanf(file, "%d %f %f %f\n", &frameNumber, &rotationVector.x, &rotationVector.y, &rotationVector.z);
			rotationChannel.push_back(rotationVector);
		}
		else if (strcmp(lineHeader, "t") == 0) {
			glm::vec3 translationVector; 
			int frameNumber; 
			fscanf(file, "%d %f %f %f\n", &frameNumber, &translationVector.x, &translationVector.y, &translationVector.z);
			translationChannel.push_back(translationVector); 
		}
		else if (strcmp(lineHeader, "s") == 0) {
			glm::vec3 scaleVector; 
			int frameNumber; 
			fscanf(file, "%d %f %f %f\n", &frameNumber, &scaleVector.x, &scaleVector.y, &scaleVector.z);
			scaleChannel.push_back(scaleVector); 
		}
		else {
			// Comment so eat up the rest of the line 
			char stupidBuffer[1000]; 
			fgets(stupidBuffer, 1000, file); 
		}

	}

	// Find max of three channels to set to animation length
	anim_length = (rotationChannel.size() > translationChannel.size()) ? 
				 ((rotationChannel.size() > scaleChannel.size()) ? rotationChannel.size() : scaleChannel.size()) : 
				 ((translationChannel.size() > scaleChannel.size()) ? translationChannel.size() : scaleChannel.size());

	fclose(file); 

	return true;

}

void Animation::updateAnimation(void) {

	// Check if animation has been triggered 
	if (triggered) {
		count++; 

		// If triggered, increment count 
		if (count % anim_speed == 0) {
			// If count is divisible by animation speed, update the animation to the next frame 
			currentTransform = getTransform(frameNum); 
			frameNum++; 


			// When the animation ends, reset values
			if (frameNum >= anim_length) {
				triggered = false; 
				frameNum = -1; 
				count = 0; 
				currentTransform = glm::mat4(1.0f); 
			}
		}
	}
}

void Animation::triggerAnimation(bool trigger) {
	triggered = trigger; 
	frameNum = 0; 
}

// Get channel values 
glm::mat4& Animation::getRotation(int frameNum) { 
	glm::vec3 rotationVector; 

	// Check if frameNum is within the range of rotation channel, 
	if (frameNum < rotationChannel.size()) {
		rotationVector = rotationChannel[frameNum]; 
	}
	else {
		//std::cerr << "Trying to access rotation frame in animation that is not valid" << std::endl;
		//rotationVector = rotationChannel[0];
		rotationVector = glm::vec3(0.0f); 
	}

	return glm::rotate(rotationVector[0] * float(M_PI)/180.0f, vec3(1.0f, 0.0f,0.0f))
		 * glm::rotate(rotationVector[1] * float(M_PI)/180.0f, vec3(0.0f, 1.0f,0.0f))
		 * glm::rotate(rotationVector[2] * float(M_PI)/180.0f, vec3(0.0f, 0.0f,1.0f));
}

glm::mat4& Animation::getTranslation(int frameNum) {
	glm::vec3 translationVector;

	// Check if frameNum is within the range of translation channel, 
	if (frameNum < translationChannel.size()) {
		translationVector = translationChannel[frameNum];
	}
	else {
		//std::cerr << "Trying to access translation frame in animation that is not valid" << std::endl;
		//translationVector = translationChannel[0];
		translationVector = glm::vec3(0.0f); 
	}

	return glm::translate(translationVector);
}

glm::mat4& Animation::getScale(int frameNum) {
	glm::vec3 scaleVector; 

	// Check if frameNum is within the range of scale channel, 
	if (frameNum < scaleChannel.size()) {
		scaleVector = scaleChannel[frameNum];
	}
	else {
		//std::cerr << "Trying to access scale frame in animation that is not valid" << std::endl;
		//scaleVector = scaleChannel[0];
		scaleVector = glm::vec3(1.0f); 
	}

	return glm::scale(scaleVector);
}