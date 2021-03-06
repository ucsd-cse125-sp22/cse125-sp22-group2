#include "AudioEngine.h"
#include <vector>


bool AudioEngine::errorCheck(const std::string& message, FMOD_RESULT engine)
{
    if (engine != FMOD_OK) 
    {
        std::cerr << message << ": " << engine << " " << FMOD_ErrorString(engine) << std::endl;
        return false;
    }
    return true;
}

void AudioEngine::update()
{
    // Clear unused channels
    std::vector<std::unordered_map<int, FMOD::Channel*>::iterator> pStoppedChannels;
    for (auto it = channels.begin(), itEnd = channels.end(); it != itEnd; ++it)
    {
        bool bIsPlaying = false;
        it->second->isPlaying(&bIsPlaying);
        if (!bIsPlaying)
        {
             pStoppedChannels.push_back(it);
        }
    }
    for (auto& it : pStoppedChannels)
    {
         channels.erase(it);
         //std::cerr << "Erasing a channel!" << std::endl;
    }

    // Update FMOD Low Level API
    AudioEngine::system->update();
}

std::string AudioEngine::loadFile(const std::string& fileName)
{
    std::string pathPrefix = "../../audio/";
    std::string filePath = pathPrefix + fileName;

    // Check file exists
    std::ifstream f(filePath.c_str());
    if (!f.good()) 
    {
        std::cerr  << "Cannot find file: " << filePath << std::endl;
        exit(1);
    }

    return filePath;
}

void AudioEngine::loadSound(const std::string& soundName, bool is3d, bool isLooping, bool isStream)
{
    FMOD_MODE mode = FMOD_DEFAULT;
    mode |= is3d ? FMOD_3D : FMOD_2D;
    mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    mode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    std::string e = "Load: " + soundName;
    FMOD::Sound* fmod_sound = nullptr;
    AudioEngine::errorCheck(e, AudioEngine::system->createSound(AudioEngine::loadFile(soundName).c_str(), mode, nullptr, &fmod_sound));

    // Set 3D parameters
    if (is3d) 
    {
        if (soundName == "Collision.wav")
        {
            fmod_sound->set3DMinMaxDistance(MIN_COLLISION_DISTANCE, MAX_3D_DISTANCE);
        }
        else
        {
            fmod_sound->set3DMinMaxDistance(MIN_3D_DISTANCE, MAX_3D_DISTANCE); // atm just car engine sounds
        }
    }

    if (fmod_sound)
    {
        library[soundName] = fmod_sound;
    }
}

void AudioEngine::unloadSound(const std::string& soundName)
{
    auto library_iter = library.find(soundName);
    if (library_iter != library.end()) 
    {
        std::string e = "Unload: " + soundName;
        AudioEngine::errorCheck(e, library_iter->second->release());
        library.erase(library_iter);
    }
}

/* CONSTRUCTOR */

AudioEngine::AudioEngine()
{
    AudioEngine::engine = FMOD::System_Create(&system);
    AudioEngine::engine = system->init(MAX_CHANNELS, FMOD_INIT_NORMAL, 0);
    //AudioEngine::engine = system->set3DSettings(2.0, 2.0, 2.0);

    // Load Sounds 
    for (auto const& audioFile : audioFiles)
    {
        AudioEngine::loadSound(audioFile.first, audioFile.second.is3d, audioFile.second.isLooping, audioFile.second.isStream);
    }
    std::cerr << "Audio Library Loaded!" << std::endl;
}

AudioEngine::~AudioEngine()
{
    // Unload sounds
    while (!library.empty())
    {
        AudioEngine::unloadSound(library.begin()->first);
    }
    std::cerr << "Audio Library Unloaded!" << std::endl;
    AudioEngine::engine = AudioEngine::system->close();
    AudioEngine::engine = AudioEngine::system->release();
}

/* UTILITY FUNCTIONS */

int AudioEngine::playSound(const char* soundName, const vec3& position, float dB)
{
    int channelId = soundCounter++;
    auto soundIter = library.find(soundName);
    if (soundIter != library.end())
    {
        // Play sound
        std::string e = "Play: ";
        e += soundName;
        FMOD::Channel* channel = nullptr;
        AudioEngine::errorCheck(e, AudioEngine::system->playSound(soundIter->second, nullptr, true, &channel));

        // Set channel parameters
        if (channel)
        {
            FMOD_MODE currMode;
            soundIter->second->getMode(&currMode);
            if (currMode & FMOD_3D)
            {
                FMOD_VECTOR fmod_position = AudioEngine::vecToFmodVec(position);
                channel->set3DAttributes(&fmod_position, nullptr);
            }
            channel->setVolume(AudioEngine::dbToVolume(dB));
            channel->setPaused(false);
            // Add channel to channels
            channels[channelId] = channel;
        }
    }
    return channelId;
}

bool AudioEngine::isPlaying(int channelId)
{
    auto channelIter = channels.find(channelId);
    bool bisPlaying = false;
    if (channelIter != channels.end())
    {
        std::string e = "Stopping Channel";
        e += channelId;
        channelIter->second->isPlaying(&bisPlaying);
    }
    return bisPlaying;
}

void AudioEngine::stopChannel(int channelId)
{
    auto channelIter = channels.find(channelId);
    if (channelIter != channels.end())
    {
        std::string e = "Stopping Channel";
        e += channelId;
        AudioEngine::errorCheck(e, channelIter->second->stop());
    }
}

void AudioEngine::stopAllChannels()
{
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        FMOD::Channel* pChannel = nullptr;
        FMOD_RESULT res = AudioEngine::system->getChannel(i, &pChannel);

        if (res == FMOD_OK && pChannel)
        {
            pChannel->stop();
        }
    }
    soundCounter = 0;
}

void AudioEngine::setChannel3dPosition(int channelId, const vec3& position)
{
    auto channelIter = channels.find(channelId);
    if (channelIter != channels.end())
    {
        FMOD_VECTOR fmod_position = AudioEngine::vecToFmodVec(position);
        std::string e = "Set 3D Position: ";
        AudioEngine::errorCheck(e, channelIter->second->set3DAttributes(&fmod_position, NULL));
    }
}

void AudioEngine::setChannelVolume(int channelId, float dB)
{
    auto channelIter = channels.find(channelId);
    if (channelIter != channels.end())
    {
        std::string e = "Set channel volume: ";
        AudioEngine::errorCheck(e, channelIter->second->setVolume(AudioEngine::dbToVolume(dB)));
    }
}

float AudioEngine::getChannelVolume(int channelId)
{
    float volume = 0.0f;
    auto channelIter = channels.find(channelId);
    if (channelIter != channels.end())
    {
        channelIter->second->getVolume(&volume);
    }
    return volume;
}


