#include "AudioEngine.h"


bool AudioEngine::errorCheck(const std::string& message, FMOD_RESULT engine)
{
	if (engine != FMOD_OK) {
		std::cerr << message << ": " << engine << " " << FMOD_ErrorString(engine) << std::endl;
		return false;
	}
	return true;
}

std::string loadFile(std::string fileName)
{
	std::string pathPrefix = "../../audio/";
	std::string filePath = pathPrefix + fileName;

	// Check file exists
	std::ifstream f(filePath.c_str());
	if (f.good()) {
		std::cout << "file exists" << std::endl;
	}
	else {
		std::cout << "file DNE" << std::endl;
		exit(1);
	}

	return filePath;
}

void AudioEngine::loadSound(std::string& soundName, bool is3d, bool isLooping, bool isStream)
{
	FMOD_MODE mode = FMOD_DEFAULT;
	mode |= is3d ? FMOD_3D : FMOD_2D;
	mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	mode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	std::string e = "Load: " + soundName;
	FMOD::Sound* fmod_sound = nullptr;
	AudioEngine::errorCheck(e, AudioEngine::system->createSound(loadFile(soundName).c_str(), mode, nullptr, &fmod_sound));

	if (fmod_sound)
	{
		library[soundName] = fmod_sound;
	}
}

void AudioEngine::unloadSound(std::string& soundName)
{
	auto library_iter = library.find(soundName);
	if (library_iter != library.end()) {
		std::string e = "Unload: " + soundName;
		AudioEngine::errorCheck(e, library_iter->second->release());
		library.erase(library_iter);
	}
}

AudioEngine::AudioEngine()
{
	AudioEngine::engine = FMOD::System_Create(&system);
	AudioEngine::engine = system->init(MAX_CHANNELS, FMOD_INIT_NORMAL, 0);

	// Load Sounds 
	AudioEngine::loadSounds();
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