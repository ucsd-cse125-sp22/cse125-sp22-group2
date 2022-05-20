/**************************************************
AudioEngine is a class to manage all sounds for the game
*****************************************************/
#include "fmod.hpp"
#include <fmod_errors.h>
#include <../Definitions.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

#define MAX_CHANNELS 32

class AudioEngine
{
public:
	typedef struct {
		bool is3d;
		bool isLooping;
		bool isStream;
	} metadata;

	std::unordered_map<std::string, metadata> audioFiles {
		{ "MenuTheme.wav", {false, true, true}},
		//{ "BattleTheme.wav", {false, true, true}},
		{ "WinTheme.wav", {false, false, true}},
		{ "LoseTheme.wav", {false, false, true}},
		{ "GetCrown.wav", {false, false, false}},
		{ "OneMinuteLeft.wav", {false, false, false}},
		//{ "Horn", {true, false, false}},
		//{ "Makeup.wav", {false, false, false}},
		//{ "Engine", {true, true, false}},
	};

	std::vector<bool> isFile3d{
		false,
		false,
		false,
		false,

	}


	AudioEngine();
	~AudioEngine();

	void update();
	bool errorCheck(const std::string& message, FMOD_RESULT engine);
	std::string loadFile(std::string& fileName);

	void loadSound(std::string& soundName, bool is3d, bool isLooping, bool isStream);
	void unloadSound(std::string& soundName);

	void playEvent(const char* fileName, const vec3& position = { 0,0,0 }, float dB = 0.0f);
	void playMusic(const char* fileName, const vec3& position = { 0,0,0 }, float dB = 0.0f);
	void stopChannel(int channelId);
	void stopEvent(const char* eventName, bool immediate = false);
	void stopAllChannels();
	void setChannel3dPosition(int channelId, const vec3& newPosition);
	void setChannelVolume(int channelId, float db);
	bool isPlaying(int channelId) const;
	bool isEventPlaying(const char* eventName) const;
	float dbToVolume(float db);
	float volumeToDb(float volume);
	FMOD_VECTOR vecToFmodVec(const  vec3& position);


private:
	FMOD::System* system;
	FMOD::Channel* channel;
	FMOD_RESULT engine;
	std::unordered_map<std::string, FMOD::Sound*> library;

};

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

