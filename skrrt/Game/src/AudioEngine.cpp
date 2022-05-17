#include "AudioEngine.h"


AudioEngine::AudioEngine()
{
    this.engine = FMOD::System_Create(&system);
	this.engine = system->init(MAX_CHANNELS, FMOD_INIT_NORMAL, 0);
    // Load audio files
    for (audioFileName : this.audioFiles)
    {
        system->createSound()
    }
}

~AudioEngine::AudioEngine()
{
    // Release sounds and engines
    for (sound : this.library)
    {
        engine = sound->second->release();
    }
    this.engine = this.system->close();
    this.engine = this.system->release();
}
bool AudioEngine::errorCheck(const std::string& message, FMOD_Result engine)
{
	if (engine != FMOD_OK) {
		std::cerr << message << ": " << engine << " " << FMOD_ErrorString(engine) << std::endl;
		return false;
	}
	return true;
}

string AudioEngine::loadFile(const char* fileName)
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