#include <iostream>
#include <stdlib.h>
#include <string>
#include <format>
// OSX systems need their own headers
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
// Use of degrees is deprecated. Use radians for GLM functions
#define GLM_FORCE_RADIANS

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <glm/glm.hpp>
#include <chrono>
#include "Screenshot.h"
#include "Scene.h"
#include "Game.h"
#include "Player.h"
#include "NetworkClient.hpp"

#include "../../../Config.hpp"
#include "../../../Frame.hpp"
#include "../../../Definitions.hpp"
#include "Debug.h"

static const int width = cse125constants::WINDOW_WIDTH;
static const int height = cse125constants::WINDOW_HEIGHT;
static const char* title = "Scene viewer";
static const glm::vec4 background(0.1f, 0.2f, 0.3f, 1.0f);
static Scene scene;
//static ParticleCube* testcube;
//static Player p0, p1, p2, p3;
//static std::vector<Player*> players{ &p0, &p1, &p2, &p3 };
//static Game game(&p0, &p1, &p2, &p3);
//static Game game(cse125constants::NUM_PLAYERS);
static Game game(4);

static std::map<std::string, bool>triggers;

static int lastRenderTime = 0;
static int particleTime = 0;

// Network related variables
int clientId = cse125constants::DEFAULT_CLIENT_ID; // this client's unique id
std::unique_ptr<cse125networkclient::NetworkClient> networkClient;

// Optimization to prevent accessing the scene graph to toggle menu visibility
// if the current visibility is the same as the toggled visibility
bool startScreenVisibility = false;
bool winScreenVisibility = false;

// Game / match flow variables
bool showStartMenu = true;
bool matchInProgress = false;
bool waitingToStartMatch = false;
bool enableSendPlay = true;
float countdownTimeRemaining = cse125constants::DEFAULT_COUNTDOWN_TIME_REMAINING;
int winnerId = cse125constants::DEFAULT_WINNER_ID;

// Time
static std::chrono::time_point<std::chrono::system_clock> startTime;

static int mouseX = 0.0f;
static int mouseY = 0.0f;
static bool mouseLocked = true;

#include "hw3AutoScreenshots.h"

std::string makeMatchEndText(int playerId, int winnerId) {
    std::string matchEndText = "";
    if (playerId == winnerId) {
        matchEndText = "You won the pageant! Press Space to play again!";
    }
    else {
        matchEndText = "Player " + std::to_string(winnerId + 1) + " won the pageant! Better luck next time! Press Space to play again!";
    }
    return matchEndText;
}

void printHelp(){

        /*
        case ' ':
            hw3AutoScreenshots();
            glutPostRedisplay();
            break;
        */
    std::cout << R"(
    Available commands:
      press 'H' to print this message again.
      press Esc to quit.
      press 'O' to save a screenshot.
      press the arrow keys to rotate camera.
      press 'Z' to zoom.
      press 'C' to reset camera.
      press 'L' to turn on/off the lighting.
    
      press 'W' 'A' 'S' 'D' to move. 

      press Spacebar to generate images for hw3 submission.
    
        )";
}

void initialize(void)
{
    printHelp();
    glClearColor(background[0], background[1], background[2],
                 background[3]); // background color
    glViewport(0, 0, width, height);

    // Initialize scene
    scene.init();
    
    // Initialize triggers map 
    triggers["up"] = false; 
    triggers["left"] = false; 
    triggers["down"] = false; 
    triggers["right"] = false; 

    // Set up players
    //for (int i = 0; i < cse125constants::NUM_PLAYERS; i++) {
    for (int i = 0; i < 4; i++) {
        game.players[i]->setPlayer(scene.node["player" + std::to_string(i)]);
        game.players[i]->setCrown(scene.node["crown" + std::to_string(i)]);
        game.players[i]->setBlowdryer(scene.node["blowdryer" + std::to_string(i)]);
    }

    game.init(scene.node["world"], scene.node["UI_root"]);
    game.updateTime(cse125config::MATCH_LENGTH);

    // Initialize time
    startTime = std::chrono::system_clock::now();
    //lastRenderTime = glutGet(GLUT_ELAPSED_TIME);
    lastRenderTime = -50;

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glLineWidth(3.0f);
    glEnable(GL_CULL_FACE);

    // Make the cursor invisible
    glutSetCursor(GLUT_CURSOR_NONE);

}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene.draw(scene.node["world"]);
    scene.updateScreen();

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //scene.draw(scene.node["UI_root"]);
    scene.drawUI();


    // Create the end of match text
    const bool showMatchEndText = winnerId != cse125constants::DEFAULT_WINNER_ID;
    if (showMatchEndText) {
        scene.drawText(countdownTimeRemaining, true, makeMatchEndText(clientId, winnerId));
    }
    else {
        scene.drawText(countdownTimeRemaining, false);
    }

    /*
	std::cout << "car transformation : " << std::endl; 
	glm::mat4 car_transform = scene.node["player0"]->childtransforms[0];
	std::cout << car_transform[0][0] << " " << car_transform[0][1] << " " << car_transform[0][2] << " " << car_transform[0][3] << std::endl;
	std::cout << car_transform[1][0] << " " << car_transform[1][1] << " " << car_transform[1][2] << " " << car_transform[1][3] << std::endl;
	std::cout << car_transform[2][0] << " " << car_transform[2][1] << " " << car_transform[2][2] << " " << car_transform[2][3] << std::endl;
	std::cout << car_transform[3][0] << " " << car_transform[3][1] << " " << car_transform[3][2] << " " << car_transform[3][3] << std::endl;
    */

    glDisable(GL_BLEND);

    //testcube->draw(glm::mat4(1.0f), scene.shader->program);

    glutSwapBuffers();
    glFlush();
}

// Toggles the visibility of the start menu and background
void toggleStartMenuVisibility(const bool& visibility) {
    if (visibility != startScreenVisibility) {
        startScreenVisibility = visibility;
        scene.node["start_menu"]->visible = visibility;
    }
}

void saveScreenShot(const char* filename = "test.png")
{
    int currentwidth = glutGet(GLUT_WINDOW_WIDTH);
    int currentheight = glutGet(GLUT_WINDOW_HEIGHT);
    Screenshot imag = Screenshot(currentwidth, currentheight);
    imag.save(filename);
}

void sendDataToServer(MovementKey movementKey, vec3 cameraDirection)
{
    // Only send data to server if the match is in progress
    if (matchInProgress) {     
        boost::system::error_code error;
        size_t numWritten = networkClient->send(movementKey, cameraDirection, &error);
        if (error) {
            cse125debug::log(LOG_LEVEL_ERROR, "Error sending packet to server\n");
        }
    }
}

void sendPlayToServer() {
    if (enableSendPlay) {
        // Send packet to server indicating client is ready to play
        boost::system::error_code error;
        // Check for a packet from the server indicating that the game is ready to restart
        cse125debug::log(LOG_LEVEL_INFO, "Sending play packet to server...\n");
        networkClient->play(&error);
        if (!error) {
            enableSendPlay = false;
            waitingToStartMatch = true;
            cse125debug::log(LOG_LEVEL_INFO, "Successfully sent play packet to server...\n");
        }
        else {
            cse125debug::log(LOG_LEVEL_ERROR, "Error sending play packet to server\n");
        }
    }
}

cse125framing::ServerFrame* receiveDataFromServer()
{
    cse125framing::ServerFrame* frame = new cse125framing::ServerFrame();
    boost::system::error_code error;
    size_t numRead = networkClient->receive(frame, &error);
    if (error) {
        cse125debug::log(LOG_LEVEL_ERROR, "Error receiving packet from server\n");
    }

    return frame;
}

void updatePlayerState(cse125framing::ServerFrame* frame) {
    // Use the data to update the player's game state
    for (int i = 0; i < cse125constants::NUM_PLAYERS; i++)
    {
        const glm::vec3 pos = glm::vec3(frame->players[i].playerPosition);
        const glm::vec3 dir = glm::vec3(frame->players[i].playerDirection);
        const glm::vec3 up = glm::vec3(frame->players[i].playerUp);
        game.players[i]->moveCar(dir, up, pos);
        game.players[i]->setCrownStatus(frame->players[i].hasCrown);
        game.players[i]->setMakeupLevel(frame->players[i].makeupLevel);
        game.players[i]->setPlayerScore(frame->players[i].score);
        game.players[i]->setHasPowerup(frame->players[i].hasPowerup);
        game.players[i]->setUsingPowerup(frame->players[i].powerupActive);
        //std::cout << "makeup level for player " << i << ": " << game.players[i]->getMakeupLevel() << std::endl;
        game.players[i]->setSpeed(frame->players[i].playerSpeed);
        game.players[i]->setInvincibility(frame->players[i].invincible);
        glm::vec3 offsetDir = glm::normalize(glm::cross(dir, up));
        const std::string headlightName = "player" + std::to_string(i) + "Headlight";
        scene.spotLights[headlightName + "0"]->position = vec4(pos + (1.0f * glm::normalize(dir)) + (0.5f * offsetDir), 1.0f);
        scene.spotLights[headlightName + "0"]->direction = dir;
        scene.spotLights[headlightName + "1"]->position = vec4(pos + (1.0f * glm::normalize(dir)) + (-0.5f * offsetDir), 1.0f);
        scene.spotLights[headlightName + "1"]->direction = dir;
    }
    if (!TOP_DOWN_VIEW) {
        scene.camera->setPosition(glm::vec3(frame->players[clientId].playerPosition), true);
    }
}

void updateCrownState(cse125framing::ServerFrame* frame) {
    game.setCrownTransform(glm::translate(frame->crown.crownPosition), frame->crown.crownVisible);
}

void updatePowerupState(cse125framing::ServerFrame* frame) {
    for (int i = 0; i < cse125constants::NUM_POWERUPS; i++) {
        game.setBlowdryerTransform(i, glm::translate(frame->powerup[i].powerupPosition), frame->powerup[i].powerupVisible);
    }
}

void triggerAnimations(const cse125framing::AnimationTrigger& triggers)
{
    // makeup booth animation
    for (int booth = 0; booth < cse125constants::NUM_MAKEUP_STATIONS; booth++)
    {
        if (triggers.makeupBooth[booth])
        {
            // trigger gate animation
            game.triggerGateAnimation(booth);
        }
    }

    // crash animation
    for (int playerId = 0; playerId < cse125constants::NUM_PLAYERS; playerId++)
    {
        if (triggers.playerCrash[playerId])
        {
            // trigger crash animation
            game.triggerCarCollisionAnimation(playerId);
        }
    }
}

void triggerAudio(const cse125framing::AudioTrigger triggers[cse125constants::MAX_NUM_SOUNDS])
{
    using namespace cse125framing;
    using namespace cse125constants;
    for (int i = 0; i < MAX_NUM_SOUNDS; i++)
    {
        AudioTrigger audio = triggers[i];
        vec3 position;
        switch (audio.id)
        {
        case AudioId::COLLISION:
            position = game.computeCamRelative3dPosition(scene.camera->forwardVectorXZ(), game.players[clientId]->getPosition(), audio.position);
            game.triggerFx("Collision.wav", position);
            break;
        case AudioId::MAKEUP:
            position = game.computeCamRelative3dPosition(scene.camera->forwardVectorXZ(), game.players[clientId]->getPosition(), audio.position);
            game.triggerFx("Makeup.wav", position);
            break;
        case AudioId::CROWN_CHANGE:
            game.triggerFx("GetCrown.wav", { 0,0,0 }, -3.0);
            break;
        case AudioId::NO_AUDIO:
        default:
            break;
        }
        // optimization if all audios are at the front (no holes)
        if (audio.id == AudioId::NO_AUDIO)
            break;
    }
}

void handleMoveForward() {
    sendDataToServer(MovementKey::FORWARD, scene.camera->forwardVectorXZ());
}

void handleMoveBackward() {
    sendDataToServer(MovementKey::BACKWARD, scene.camera->forwardVectorXZ());
}

void handleMoveRight() {
    sendDataToServer(MovementKey::RIGHT, scene.camera->forwardVectorXZ());
}

void handleMoveLeft() {
    sendDataToServer(MovementKey::LEFT, scene.camera->forwardVectorXZ());
}

void handleSpace() {
    // Just pretend I don't handle game logic client-side here
    if (game.players[clientId]->getHasPowerup()) {
        sendDataToServer(MovementKey::SPACE, scene.camera->forwardVectorXZ());
    }
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case 27: // Escape to quit
            networkClient->closeConnection();
            exit(0);
            break;
        case 'h': // print help
            printHelp();
        /*
        case ' ':
            hw3AutoScreenshots();
            glutPostRedisplay();

            break;
        */
            break;
        case 'o': // save screenshot
            saveScreenShot();
            break;
        case 'c':
            scene.camera -> aspect_default = float(glutGet(GLUT_WINDOW_WIDTH))/float(glutGet(GLUT_WINDOW_HEIGHT));
            scene.camera -> reset();
            glutPostRedisplay();
            break;
        case 'A':
        case 'a':
            //handleMoveLeft();
            triggers["left"] = true;
            //glutPostRedisplay();
            break;
        case 'D':
        case 'd':
            //handleMoveRight();
            triggers["right"] = true;
            //glutPostRedisplay();
            break;
        case 'W':
        case 'w':
            //handleMoveForward();
            triggers["up"] = true;
            //glutPostRedisplay();
            break;
        case 'S':
        case 's':
            triggers["down"] = true;
            //handleMoveBackward();
            //glutPostRedisplay();
            break;
        case 'z':
            //scene.camera -> zoom(1.1f);
            game.players[0]->setCrownStatus(true);
            std::cout << game.players[0]->getCrownStatus() << std::endl;
            glutPostRedisplay();
            break;
        case 'x':
            game.players[0]->setCrownStatus(false);
            std::cout << game.players[0]->getCrownStatus() << std::endl;
            glutPostRedisplay();
        /*
        case ' ':
            hw3AutoScreenshots();
            glutPostRedisplay();
            break;
        */
            break;
        case 'l':
            scene.shader -> enablelighting = !(scene.shader -> enablelighting);
            glutPostRedisplay();
            break;
        case 'u': 
            // Test to trigger gate animation 
            game.triggerGateAnimation(0); 
            glutPostRedisplay(); 
            break; 
        case 'i': 
            // Test to trigger gate animation 
            game.triggerGateAnimation(1); 
            glutPostRedisplay(); 
            break; 
        case '1': 
            // Test to trigger car collision animation 
            game.triggerCarCollisionAnimation(0); 
            glutPostRedisplay(); 
            break; 
        case '2': 
            // Test to trigger car collision animation 
            game.triggerCarCollisionAnimation(1); 
            glutPostRedisplay(); 
            break; 
        case '3': 
            // Test to trigger car collision animation 
            game.triggerCarCollisionAnimation(2); 
            glutPostRedisplay(); 
            break; 
        case '4': 
            // Test to trigger car collision animation 
            game.triggerCarCollisionAnimation(3); 
            glutPostRedisplay(); 
            break; 

            /* AUDIO TRIGGERS */
        case ',':
            // Audio Engine
            game.stopAllSounds();
            break;
        case 'm':
            // Audio Engine
            game.playMusic("BattleTheme.wav", -10.0);
            break;
        case 'n':
        {
            // Audio Engine
            vec3 position = game.computeCamRelative3dPosition(scene.camera->forwardVectorXZ(), game.players[clientId]->getPosition(), vec3{ 0,0,0 });
            std::cout << "Camera position relative to the map center: " << position.x << " " << position.y << " " << position.z << std::endl;
            game.triggerFx("Collision.wav", position);
            break;
        }

        case 'p': 
            // Print player0's location 
            std::cout << "Player0's location: " << game.players[0]->getPosition().x << " " << game.players[0]->getPosition().y<< " " << game.players[0]->getPosition().z << std::endl;
            break;

        /*
        case ' ':
            hw3AutoScreenshots();
            glutPostRedisplay();
            break;
        */
        case '`':
            mouseLocked = !mouseLocked;
            if (mouseLocked) {
                glutSetCursor(GLUT_CURSOR_NONE);
            }
            else {
                glutSetCursor(GLUT_CURSOR_INHERIT);
            }
            break;

        // Key for telling server that player is ready for another match
        case 'r':
            sendPlayToServer();         
            break;
        // Key for starting the game initially
        case ' ':
            handleSpace();
            sendPlayToServer();
            break;
        case 'q':
            handleSpace();
            break;
        default:
            //glutPostRedisplay();
            break;
    }
}

void keyboardUp(unsigned char key, int x, int y){
    switch(key){
        case 'A':
        case 'a':
            triggers["left"] = false;
            //glutPostRedisplay();
            break;
        case 'D':
        case 'd':
            triggers["right"] = false;
            //glutPostRedisplay();
            break;
        case 'W':
        case 'w':
            triggers["up"] = false;
            //glutPostRedisplay();
            break;
        case 'S':
        case 's':
            triggers["down"] = false;
            //glutPostRedisplay();
            break;
        default:
            //glutPostRedisplay();
            break;
    }
}

void specialKey(int key, int x, int y) {
    glm::vec3 camera = (scene.camera->target - scene.camera->eye) *
        glm::vec3(1.0f, 0.0f, 1.0f);
    switch (key) {
    case GLUT_KEY_UP: // up
        triggers["up"] = true;
        //glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN: // down
        triggers["down"] = true;
        //glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT: // right
        triggers["right"] = true;
        //glutPostRedisplay();
        break;
    case GLUT_KEY_LEFT: // left
        triggers["left"] = true;
        //glutPostRedisplay();
        break;
    }
}

void specialKeyUp(int key, int x, int y){
    glm::vec3 camera = (scene.camera->target - scene.camera->eye) *
        glm::vec3(1.0f, 0.0f, 1.0f);
    switch (key) {
        case GLUT_KEY_UP: // up
            //handleMoveForward();
            triggers["up"] = false;
            //glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN: // down
            //handleMoveBackward();
            triggers["down"] = false;
            //glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT: // right
            //handleMoveRight();
            triggers["right"] = false;
            //glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT: // left
            //handleMoveLeft();
            triggers["left"] = false;
            //glutPostRedisplay();
            break;
    }
}

void idle() {

    bool render = false;

    int time = (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count();
	float speed = 50.0f;
    if (time - lastRenderTime > 50) {
        //std::cout << time - lastRenderTime << "\n";
        for (int i = 0; i < cse125constants::NUM_PLAYERS; i++) {
            game.players[i]->spinWheels(speed * game.players[i]->getSpeed());
            game.players[i]->bobCrown(time);
            game.players[i]->updateParticles((time - lastRenderTime) / 50.0f);
            //std::cout << (time - lastRenderTime) / 50.0f << "\n";

            scene.scores[i]->updateText(std::to_string((int)game.players[i]->getScore()));
        }
        // Update crown on the map
        game.bobCrown(time);
        // Update powerups on the map
        game.bobPowerup(time);

		// Update drip level based on current player's makeup level 
		RealNumber currentMakeupLevel = game.players[clientId]->getMakeupLevel();
		game.updateDrips(time, currentMakeupLevel);
		game.updateMakeupStatusBar(time, currentMakeupLevel);
        game.updateBlowdryerIcon(game.players[clientId]->getHasPowerup());

        // Update all animations 
        game.updateAnimations(); 

        // Update time 
        scene.game_time->updateText(std::to_string((int)(game.getTime() + 0.5f)));

		lastRenderTime = time;
        render = true;
    }

    // Update Engine Audio Positions
    game.updateCarEngines(clientId, scene.camera->forwardVectorXZ());


    // Handle direction triggers 
    if (triggers["up"]) {
        handleMoveForward(); 
    } 
    if (triggers["down"]) {
        handleMoveBackward();
    }
    if (triggers["left"]) {
        handleMoveLeft();
    }
    if (triggers["right"]) {
        handleMoveRight();
    }

    toggleStartMenuVisibility(showStartMenu);

    // Handle server communication
    const bool connectedToServer = clientId != cse125constants::DEFAULT_CLIENT_ID;
    if (connectedToServer) {
        if (matchInProgress) {
            cse125framing::ServerFrame* frame = receiveDataFromServer();
            triggerAnimations(frame->animations);
            triggerAudio(frame->audio);
            if (frame->matchInProgress) {
                // Use the frame to update the powerups' state
                updatePowerupState(frame);
                // Use the frame to update the crown's state
                updateCrownState(frame);
                // Use the frame to update the player's state
                updatePlayerState(frame);
                game.updateTime(frame->gameTime);
            }
            else {
               cse125debug::log(LOG_LEVEL_INFO, "Match has ended!\n");
               winnerId = frame->winnerId;
               matchInProgress = false;
               enableSendPlay = true;
            }
            // Delete the frame
            delete frame;
        }
        else {
            if (waitingToStartMatch) {
                // Reset the winner id for this new match
                winnerId = cse125constants::DEFAULT_WINNER_ID;
                // Stop showing the start menu
                showStartMenu = false;
                // Display the game time
                game.updateTime(cse125config::MATCH_LENGTH);

                cse125framing::ServerFrame* frame = receiveDataFromServer();    

                if (cse125config::ENABLE_COUNTDOWN) {
                    scene.camera->reset(clientId);
                    updatePlayerState(frame);
                    triggerAnimations(frame->animations);
                    triggerAudio(frame->audio);
                    // Update countdown time
                    countdownTimeRemaining = frame->countdownTimeRemaining;
                    if (countdownTimeRemaining <= 0) {
                        cse125debug::log(LOG_LEVEL_INFO, "Ready to start match!\n");
                        matchInProgress = true;
                        waitingToStartMatch = false;
                        // Play Game Music
                        game.playMusic("BattleTheme.wav", -10.0);
                    }
                }
                else {
                    cse125debug::log(LOG_LEVEL_INFO, "Ready to start match!\n");
                    matchInProgress = true;
                    waitingToStartMatch = false;
                    // Play Game Music
                    game.playMusic("BattleTheme.wav", -10.0);
                    game.startCarEngines(clientId, scene.camera->forwardVectorXZ());
                }                
                
                // Delete the frame
                delete frame;
            }
        }
    }


    if (render) {
        glutPostRedisplay();
    }
}

void mouseMovement(int x, int y) {
	int maxDelta = 100;
	int dx = glm::clamp(x - mouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(y - mouseY, -maxDelta, maxDelta);

    // Set cursor to the center of the screen
    if (mouseLocked) {
        mouseX = width / 2;
        mouseY = height / 2;
        glutWarpPointer(mouseX, mouseY);
    }
    else {
	    mouseX = x;
	    mouseY = y;
    }

    if (dx != 0 || dy != 0) {
        if (!TOP_DOWN_VIEW) {
			scene.camera->rotateRight(dx);
			scene.camera->rotateUp(dy);
        }
        glutPostRedisplay();
    }
}

int main(int argc, char** argv)
{
    // BEGIN CREATE WINDOW
    glutInit(&argc, argv);

#ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA |
                        GLUT_DEPTH);
#else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#endif
    glutInitWindowSize(width, height);
    glutCreateWindow(title);
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    }
#endif
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    // END CREATE WINDOW

    // Read in config file and set variables
    cse125config::initializeConfig("../../config.json");

    // Initialize the network client
    networkClient = std::make_unique<cse125networkclient::NetworkClient>(cse125config::SERVER_HOST, cse125config::SERVER_PORT);
    // Connect to the server and set the client's id
    clientId = networkClient->getId();
    
    // Graphics binding
    initialize();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKey);
    glutSpecialUpFunc(specialKeyUp);
    glutIdleFunc(idle);
    glutPassiveMotionFunc(mouseMovement);
    glutMotionFunc(mouseMovement);  
    glutMainLoop();
    return 0; /* ANSI C requires main to return int. */
}