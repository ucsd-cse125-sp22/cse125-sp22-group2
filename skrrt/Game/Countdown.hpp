#pragma once
#include <string>
namespace countdown {

    enum class CountdownState {
        PLAY_NO_SOUND,
        PLAY_READY_SOUND,
        PLAY_SET_SOUND,
        PLAY_GO_SOUND
    };

    // Used to play different sounds for the countdown
    class CountdownStateMachine {
    public:
        CountdownStateMachine();
        void ready();
        void set();
        void go();
        CountdownState getState() const;
    private:
        bool readySoundPlayed;
        bool setSoundPlayed;
        bool goSoundPlayed;
        CountdownState state;
    };


    std::string makeCountdownText(const float& countdownTimeRemaining, CountdownStateMachine& csm);

}


