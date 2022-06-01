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

#define MIN_3D_DISTANCE 1.0f
#define MAX_3D_DISTANCE 15.0f

#define MIN_COLLISION_DISTANCE 3.0f

#define CLIENT_ENGINE_DB -16.0f
#define OTHER_PLAYER_ENGINE_DB -4.0f

#define VOLUME_OFF -60.0f

class AudioEngine
{
public:
    typedef struct {
        bool is3d;
        bool isLooping;
        bool isStream;
    } metadata;


    const std::unordered_map<std::string, metadata> audioFiles {
        //                  <3d>    <loop>    <stream>
        { "MenuTheme.wav", {false,   true,    true}},
        { "BattleTheme.wav", {false, false,   true}},
        { "WinTheme.wav", {false,    false,   true}},
        { "LoseTheme.wav", {false,   false,   true}},
        { "GetCrown.wav", {false,    false,   false}},
        //{ "Horn.wav", {true, false, false}},
        { "Makeup.wav",   {false,    false,   false}},
        { "EngineIdle.wav",{true,    true,    false}},
        { "EngineAccelerate.wav", {true,true, false}},
        { "Collision.wav", {true,    false,   false}},
    };

    AudioEngine();
    ~AudioEngine();

    void update();
    bool errorCheck(const std::string& message, FMOD_RESULT engine);
    std::string loadFile(const std::string& fileName);

    void loadSound(const std::string& soundName, bool is3d, bool isLooping, bool isStream);
    void unloadSound(const std::string& soundName);

    int playSound(const char* soundName, const vec3& position = { 0,0,0 }, float dB = 0.0f);
    bool isPlaying(int channelId);
    void stopChannel(int channelId);
    void stopAllChannels();
    void setChannel3dPosition(int channelId, const vec3& newPosition);
    void setChannelVolume(int channelId, float db);
    float getChannelVolume(int channelId);

    float dbToVolume(float dB)
    {
        return powf(10.0f, 0.05f * dB);
    }
    float volumeToDb(float volume)
    {
        return 20.0f * log10f(volume);
    }

    FMOD_VECTOR vecToFmodVec(const vec3& position)
    {
        FMOD_VECTOR fVec;
        fVec.x = position.x;
        fVec.y = position.y;
        fVec.z = position.z;
        return fVec;
    }
    vec3 fmodVecToVec3(FMOD_VECTOR position)
    {
        vec3 vec;
        vec.x = position.x;
        vec.y = position.y;
        vec.z = position.z;
        return vec;
    }

    int getChannelsSize()
    {
        return channels.size();
    }

    int getSoundCount()
    {
        return soundCounter;
    }


private:
    FMOD::System* system;
    FMOD_RESULT engine;
    std::unordered_map<std::string, FMOD::Sound*> library;
    std::unordered_map<int, FMOD::Channel*> channels;
    int soundCounter;
};

