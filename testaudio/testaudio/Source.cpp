#include <fmod.hpp>
#include <iostream>

int main(void) {
	FMOD_RESULT result;
	FMOD::System* system;

	result = FMOD::System_Create(&system);
	if (result != FMOD_OK) {
		std::cout << "error" << std::endl;
		return 0;
	}
	result = system->init(100, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK) {
		std::cout << "error" << std::endl;
		return 0;
	}

	// Load sound
	FMOD::Sound* sound;
	result = system->createSound(".. / audio / Skrrt.wav", FMOD_DEFAULT, 0, &sound);
}