/**************************************************
AudioEngine is a class to manage all sounds for the game
*****************************************************/
#include "fmod.hpp"
#include <fmod_errors.h>
#include <iostream>
#include <fstream>

#define LIBRARY_SIZE 5
#define MAX_CHANNELS 32

class AudioEngine 
{
  public:
    std::array<const car*, LIBRARY_SIZE> audioFiles = {
        "MenuTheme.wav",
        "WinTheme.wav",
        "LoseTheme.wav",
        "GetCrown.wav",
        "OneMinuteLeft.wav",
    }

    AudioEngine();
    ~AudioEngine();

    bool errorCheck(const std::string& message, FMOD_RESULT engine);

    string loadFile(const char* fileName);

    void playFx(const char* fileName);


  private:
    FMOD::System* system;
    FMOD::Channel* channel;
    FMOD_RESULT engine;
    unordered_map<string, FMOD::Sound*> library;

  
}
bool succeededOrWarn(const std::string& message, FMOD_RESULT engine)
{
    if (engine != FMOD_OK) {
        std::cerr << message << ": " << engine << " " << FMOD_ErrorString(engine) << std::endl;
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

int (void) {
    FMOD::System* system;
    FMOD::Sound* sound;
    FMOD::Channel* channel = 0;
    FMOD_RESULT engine;

    engine = FMOD::System_Create(&system);
    engine = system->init(32, FMOD_INIT_NORMAL, 0);

    // Load and play sound
    engine = system->createSound(loadFile("skrrt.wav").c_str(), FMOD_DEFAULT, 0, &sound);
    if (!succeededOrWarn("FMOD: Failed to load sound", engine));

    engine = system->playSound(sound, 0, false, &channel);
    if (!succeededOrWarn("FMOD: Failed to play sound", engine));
    while (true);

    engine = sound->release();
    engine = system->close();
    engine = system->release();

    return 0;
}


