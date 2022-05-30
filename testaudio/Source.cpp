#include "fmod.hpp"
#include <fmod_errors.h>
#include <iostream>
#include <fstream>

using namespace std;

bool succeededOrWarn(const std::string& message, FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		std::cerr << message << ": " << result << " " << FMOD_ErrorString(result) << std::endl;
		return false;
	}
	return true;
}

string loadFile(const char* fileName)
{
	string pathPrefix = "../../audio/";
	string filePath = pathPrefix + fileName;

	// Check file exists
	ifstream f(filePath.c_str());
	if (f.good()) {
		cout << "file exists" << endl;
	}
	else {
		cout << "file DNE" << endl;
		exit(1);
	}

	return filePath;
}

int main(void) {
	FMOD::System* system;
	FMOD::Sound* sound;
	FMOD::Channel* channel = 0;
	FMOD_RESULT result;

	result = FMOD::System_Create(&system);
	result = system->init(32, FMOD_INIT_NORMAL, 0);

	// Load and play sound
	result = system->createSound(loadFile("skrrt.wav").c_str(), FMOD_DEFAULT, 0, &sound);
	if (!succeededOrWarn("FMOD: Failed to load sound", result));

	result = system->playSound(sound, 0, false, &channel);
	if (!succeededOrWarn("FMOD: Failed to play sound", result));
	while (true);

	result = sound->release();
	result = system->close();
	result = system->release();

	return 0;
}