#include <math.h>
#include "Countdown.hpp"
#include "../../Config.hpp"

countdown::CountdownStateMachine::CountdownStateMachine()
{
    this->state = CountdownState::PLAY_NO_SOUND;
    this->readySoundPlayed = false;
    this->setSoundPlayed = false;
    this->goSoundPlayed = false;
}

void countdown::CountdownStateMachine::ready()
{
    if (!readySoundPlayed) {
        state = countdown::CountdownState::PLAY_READY_SOUND;
        readySoundPlayed = true;
    }
    else {
        state = countdown::CountdownState::PLAY_NO_SOUND;
    }
}

void countdown::CountdownStateMachine::set()
{
    if (!setSoundPlayed) {
        state = countdown::CountdownState::PLAY_SET_SOUND;
        setSoundPlayed = true;
    }
    else {
        state = countdown::CountdownState::PLAY_NO_SOUND;
    }
}

void countdown::CountdownStateMachine::go()
{
    if (!goSoundPlayed) {
        state = countdown::CountdownState::PLAY_GO_SOUND;
        goSoundPlayed = true;
    }
    else {
        state = countdown::CountdownState::PLAY_NO_SOUND;
    }
}

void countdown::CountdownStateMachine::resetState()
{
    this->state = countdown::CountdownState::PLAY_NO_SOUND;
    this->readySoundPlayed = false;
    this->setSoundPlayed = false;
    this->goSoundPlayed = false;
}

countdown::CountdownState countdown::CountdownStateMachine::getState() const
{
    return state;
}


std::string countdown::makeCountdownText(const float& countdownTimeRemaining, CountdownStateMachine& csm) {
    if (countdownTimeRemaining <= 0.0f) {
        return "";
    }
    std::string countdownText = "";

    // Determine the number of ellipses to display to simulate animation
    // Each 1/3 of a whole number corresponds to one ellipsis
    const int secondsLeft = (int)(countdownTimeRemaining / cse125config::TICK_RATE);
    double secondsInteger = 0.0f;
    float secondsFraction = 1.0f - modf(countdownTimeRemaining / cse125config::TICK_RATE, &secondsInteger);
    std::string ellipses = "";
    if (secondsFraction >= 0.01f) {
        ellipses += ".";
    }
    if (secondsFraction >= 0.33f) {
        ellipses += ".";
    }
    if (secondsFraction >= 0.67f) {
        ellipses += ".";
    }

    switch (secondsLeft) {
    case 2:
        countdownText = "READY " + ellipses;
        csm.ready();
        break;
    case 1:
        countdownText = "SET " + ellipses;
        csm.set();
        break;
    case 0:
        countdownText = "SKRRT!";
        csm.go();
        break;
    default:
        break;
    }
    return countdownText;
}
