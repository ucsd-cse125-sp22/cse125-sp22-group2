#include "Game.h"
#include <iostream>

void Game::updateDrips(int time, RealNumber makeupLevel) {

    // translate the drips linearly based on makeupLevel
    //float y_offset = sin(time * float(M_PI) / 5000.0f);
    float maximumOffset = 20.0f;
    //float maximumOffset = 0.0f;
    float y_offset = (100.0f - makeupLevel) * maximumOffset / 1000.0f - 1.2; 

    glm::mat4 offset = translate(glm::vec3(0.0f, -y_offset, 0.0f));

    //std::cout << offset[3][1] << std::endl;

    drips->modeltransforms[0] = offset * initial_drip_transform;
}

void Game::updateMakeupStatusBar(int time, RealNumber makeupLevel) {

	// Scale makeup status bar based on the makeup level
	float max_offset = -1.66f; 
	glm::mat4 translate = glm::translate(glm::vec3(max_offset * (1 - (float)makeupLevel / 100.0f), 0.0f, 0.0f));
	makeup_status_bar->modeltransforms[0] = translate * initial_drip_transform;

}

void Game::updateBlowdryerIcon(bool visible) {
    blowdryer_status_icon->visible = visible;
}

void Game::bobCrown(int time) {
    // Change position of the powerup based on time 
    glm::mat4 offset = glm::translate(vec3(0.0f, 0.5f + sin(time * float(M_PI) / 2000.0f) / 9.0f, 0.0f)) * glm::rotate((float)time / 5000.0f, vec3(0.0f, 1.0f, 0.0f));
    loose_crown->modeltransforms[0] = loose_crown_translation * offset * initial_crown_transform;
}

void Game::bobPowerup(int time) {
    // Change position of the powerup based on time 
    glm::mat4 offset = glm::translate(vec3(0.0f, 0.5f + sin(time * float(M_PI) / 1400.0f) / 4.0f, 0.0f)) * glm::rotate((float)time / 800.0f, vec3(0.0f, 1.0f, 0.0f));
    for (int i = 0; i < blowdryer_pickup_node.size(); i++) {
        blowdryer_pickup_node[i]->modeltransforms[0] = blowdryer_translation[i] * offset * initial_blowdryer_transform;
    }
}

// *******************************************
// ************* Gate Animation **************
// *******************************************

void Game::parseGateAnimation() {

	const char* path = "animations/makeup_gate.txt";
	const char* lipstick_path = "animations/lipstick.txt";
	const char* mascara_path = "animations/mascara.txt";
	const char* powder_path = "animations/powder_brush.txt";

	for (int i = 0; i < cse125constants::NUM_MAKEUP_STATIONS; i++) {
		animations["gate_anim" + std::to_string(i)] = new Animation();
		animations["gate_anim" + std::to_string(i)]->readAnimation(path);

		animations["lipstick_anim" + std::to_string(i)] = new Animation(); 
		animations["lipstick_anim" + std::to_string(i)]->readAnimation(lipstick_path);

		animations["mascara_anim" + std::to_string(i)] = new Animation(); 
		animations["mascara_anim" + std::to_string(i)]->readAnimation(mascara_path);

		animations["powder_anim" + std::to_string(i)] = new Animation(); 
		animations["powder_anim" + std::to_string(i)]->readAnimation(powder_path);
	}
}

/* 
 * Function to trigger gate animation. 
 */
void Game::triggerGateAnimation(int gateNum) {
	animations["gate_anim" + std::to_string(gateNum)]->triggerAnimation(true);
	animations["lipstick_anim" + std::to_string(gateNum)]->triggerAnimation(true);
	animations["mascara_anim" + std::to_string(gateNum)]->triggerAnimation(true);
	animations["powder_anim" + std::to_string(gateNum)]->triggerAnimation(true);
}

// *******************************************
// ******** Car Collision Animation **********
// *******************************************

void Game::parseCarCollisionAnimation() {

    //const char* path = "animations/makeup_gate.txt";
    const char* path = "animations/car_collision.txt"; 

    //for (int i = 0; i < cse125constants::NUM_PLAYERS; i++) {
    for (int i = 0; i < 4; i++) {
        animations["collision_anim" + std::to_string(i)] = new Animation();
        animations["collision_anim" + std::to_string(i)]->readAnimation(path); 
    }
    std::cout << "Successfully read in car collision animation" << std::endl;
}

void Game::triggerCarCollisionAnimation(int playerNum) {

    // Select which car to apply the animation on 
    animations["collision_anim" + std::to_string(playerNum)]->triggerAnimation(true);
}

void Game::updateAnimations() {

    std::map<std::string, Animation*>::iterator it;

    // Call update on all animations in map
    for (it = animations.begin(); it != animations.end(); it++) {
        it->second->updateAnimation();
    }

    // apply all animations to respective objects 
    applyAnimations(); 
}

void Game::applyAnimations() {

    // Gate's arm animation 
    // Obtain the frame's rotation, translation, and scale matrices 
    for (int i = 0; i < cse125constants::NUM_MAKEUP_STATIONS; i++) {
        glm::mat4 new_transformation = animations["gate_anim" + std::to_string(i)]->getCurrentTransform();

		// Apply the transformations to the gate's arm
		makeup_gate_arms[i]->modeltransforms[0] = initial_arm_transforms[i] * new_transformation;

		new_transformation = animations["lipstick_anim" + std::to_string(i)]->getCurrentTransform(); 
		lipsticks[i]->modeltransforms[0] = initial_lipstick_transforms[i] * new_transformation;

		new_transformation = animations["mascara_anim" + std::to_string(i)]->getCurrentTransform(); 
		mascaras[i]->modeltransforms[0] = initial_mascaras_transforms[i] * new_transformation;

		new_transformation = animations["powder_anim" + std::to_string(i)]->getCurrentTransform(); 
		powder_brushes[i]->modeltransforms[0] = initial_powder_transforms[i] * new_transformation;
	}

    // Car collision animations 
    for (int i = 0; i < 4; i++) {
        glm::mat4 new_transformation = animations["collision_anim" + std::to_string(i)]->getCurrentTransform(); 
        players[i]->setPlayerTransform(new_transformation); 
    }

    // *********************************
    // Add future animation applications here 
    // *********************************
}

// *********************************
// AUDIO TRIGGERS
// *********************************
void Game::updateAudio()
{
    // Update Audio Engine
    audioEngine.update();
}

void Game::stopAllSounds()
{
    audioEngine.stopAllChannels();
    for (int i = 0; i < cse125constants::NUM_PLAYERS; i++)
    {
        powerupChannels[i] = 0;
    }
}

int Game::playMusic(const char* musicName, float db)
{
    audioEngine.stopAllChannels();
    int channel = audioEngine.playSound(musicName, { 0,0,0 }, db);
    return channel;
}

int Game::triggerFx(const char* fxName, const vec3& position, float dB)
{
    int channel = audioEngine.playSound(fxName, position, dB);
    return channel;
}

vec3 Game::computeCamRelative3dPosition(const vec3& cameraPos, const vec3& playerPos, const vec3& eventPos)
{
    vec3 distance = eventPos - playerPos;

    // Compute camera angle of rotation from origin
    vec3 origin = { 0, 0, -1 };
    float theta = glm::acos(glm::dot(origin, cameraPos)); 
    // > 180 degree check (left or right half)
    if (cameraPos.x > 0.0)
    {
        // Left Half, pi - 2pi
        theta = 2*M_PI - theta;
    }

    // Rotate original vector
    vec3 eventRelativeToCameraPos = { 0,0,0 };
    eventRelativeToCameraPos.x = glm::cos(theta) * distance.x - glm::sin(theta) * distance.z;
    eventRelativeToCameraPos.z = glm::sin(theta) * distance.x + glm::cos(theta) * distance.z;

    return eventRelativeToCameraPos;
}

void Game::startCarEngines(int clientId, vec3& cameraPos)
{
    vec3 playerPos = players[clientId]->getPosition();
    for (int i = 0; i < cse125constants::NUM_PLAYERS; i++) 
    {
        float db = OTHER_PLAYER_ENGINE_DB;
        vec3 enginePosition = Game::computeCamRelative3dPosition(cameraPos, playerPos, players[i]->getPosition());
        if (i == clientId) 
        {
            db = CLIENT_ENGINE_DB; // turn down player engine volume
        }
        // Start Car Engine
        int idle = Game::triggerFx("EngineIdle.wav", enginePosition, db);
        int accelerate = Game::triggerFx("EngineAccelerate.wav", enginePosition, VOLUME_OFF);
        // Add channel number to carEngineChannels
        carEngineChannels[i] = CarEngine{ idle, accelerate };
        //printf("player: %d | idle: %d accelerate: %d\n", i, idle, accelerate);

        // Update so we can have multiple instances of the same sound
        audioEngine.update();
    }
    carEngineState = true;
}

void Game::stopCarEngines()
{
    for (int i = 0; i < cse125constants::NUM_PLAYERS; i++)
    {
        CarEngine carEngine = carEngineChannels[i];
        audioEngine.stopChannel(carEngine.idle);
        audioEngine.stopChannel(carEngine.accelerate);
    }
    carEngineState = false;
}

float Game::fadeEngine(int channelId, float targetDb)
{
    float currVolume = audioEngine.getChannelVolume(channelId);
    float currDb = audioEngine.volumeToDb(currVolume);
    currDb += (targetDb - currDb) * 0.5f;
    return currDb;
}

void Game::updateCarEngines(int clientId, vec3& cameraPos)
{
    // If Car Engines aren't on
    if (carEngineState)
    {
        vec3 playerPos = players[clientId]->getPosition();
        for (int i = 0; i < cse125constants::NUM_PLAYERS; i++)
        {
            // Interpolate carEngine sound to play
            RealNumber speed = players[i]->getSpeed();
            if (speed >= 0.5f) speed = 0.5f; // Clamp values above 0.5
            CarEngine carEngine = carEngineChannels[i];
            RealNumber c = speed / 0.5; // audio fade coefficient based on speed
            float idleDb = i == clientId ? CLIENT_ENGINE_DB : OTHER_PLAYER_ENGINE_DB;
            if (speed == 0.5)
            {
                audioEngine.setChannelVolume(carEngine.idle, Game::fadeEngine(carEngine.idle, VOLUME_OFF));
                audioEngine.setChannelVolume(carEngine.accelerate, Game::fadeEngine(carEngine.accelerate, idleDb));
            }
            else 
            {
                audioEngine.setChannelVolume(carEngine.idle, Game::fadeEngine(carEngine.idle, idleDb));
                audioEngine.setChannelVolume(carEngine.accelerate, Game::fadeEngine(carEngine.accelerate, VOLUME_OFF));
            }

            // Update Player car engine location
            vec3 enginePosition = Game::computeCamRelative3dPosition(cameraPos, playerPos, players[i]->getPosition());
            audioEngine.setChannel3dPosition(carEngine.idle, enginePosition);
            audioEngine.setChannel3dPosition(carEngine.accelerate, enginePosition);
        }
    }
}

void Game::startUsePowerup(int clientId, vec3& cameraPos)
{
    //std::cerr << "powerup use trigger" << std::endl;
    vec3 playerPos = players[clientId]->getPosition();
    for (int i = 0; i < cse125constants::NUM_PLAYERS; i++)
    {
        std::cout << players[i]->getUsingPowerup() << std::endl;
        if (players[i]->getUsingPowerup() && powerupChannels[i] == 0)
        {
            vec3 powerupPos = Game::computeCamRelative3dPosition(cameraPos, playerPos, players[i]->getPosition());
            float powerupDb = i == clientId ? CLIENT_POWERUP_DB : OTHER_PLAYER_POWERUP_DB;
            //int powerupChannel = triggerFx("BlowDryerUse.wav", powerupPos, powerupDb);
            int powerupChannel = triggerFx("BlowDryerUse.wav", powerupPos);
            powerupChannels[i] = powerupChannel;
            //std::cerr << "player " << i << "starting powerup: [" << powerupChannel << "]" << std::endl;
        }
    }
}

void Game::updateUsePowerup(int clientId, vec3& cameraPos)
{
    vec3 playerPos = players[clientId]->getPosition();
    for (int i = 0; i < cse125constants::NUM_PLAYERS; i++)
    {
        if (players[i]->getUsingPowerup() && powerupChannels[i])
        {
            //std::cerr << "updating powerup: " << powerupChannels[i] << std::endl;
            vec3 powerupPos = Game::computeCamRelative3dPosition(cameraPos, playerPos, players[i]->getPosition());
            audioEngine.setChannel3dPosition(powerupChannels[i], powerupPos);
        }
        else if (powerupChannels[i])
        {
            //std::cerr << "turning off powerup" << powerupChannels[i] << std::endl;
            powerupChannels[i] = 0;
        }
    }
}