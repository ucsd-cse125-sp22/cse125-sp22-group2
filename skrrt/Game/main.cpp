#include <iostream>
#include <stdlib.h>
#include <string>
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
#include "Screenshot.h"
#include "Scene.h"
#include "Game.h"
#include "Player.h"

#include "../../Config.hpp"
#include "../../Frame.hpp"
#include "../../Definitions.hpp"
#include "Debug.h"

static const int width = 1600;
static const int height = 1100;
static const char* title = "Scene viewer";
static const glm::vec4 background(0.1f, 0.2f, 0.3f, 1.0f);
static Scene scene;
static Player p0, p1, p2, p3;
static std::vector<Player*> players{ &p0, &p1, &p2, &p3 };
static Game game(&p0, &p1, &p2, &p3);

//static bool triggers[] = { false, false, false, false };
static std::map<std::string, bool>triggers;

static int lastRenderTime = 0;

boost::asio::io_context outgoingContext;
std::unique_ptr<boost::asio::ip::tcp::resolver> outgoingResolver;
boost::asio::ip::tcp::resolver::results_type outgoingEndpoints;
std::unique_ptr<boost::asio::ip::tcp::socket> outgoingSocket;


int clientId = cse125constants::DEFAULT_CLIENT_ID; // this client's unique id
int clientFrameCtr = 0;
static int mouseX = 0.0f;
static int mouseY = 0.0f;

void sendDataToServer(MovementKey movementKey,
                      vec3 cameraDirection);
void receiveDataFromServer();
void cleanupConnection();

#include "hw3AutoScreenshots.h"

// Asks the server to provide this client with its unique id that the client can
// use for all future communication with the server
void requestClientId()
{
    // Set up connection to server for outgoing data

    cse125framing::ClientFrame frame;
    frame.id = cse125constants::DEFAULT_CLIENT_ID;

    // Get this client's id from the server
    while (true)
    {
        // Send a frame with an id of -1
        boost::array<char, cse125framing::CLIENT_FRAME_BUFFER_SIZE>
            clientBuffer;
        cse125framing::serialize(&frame, clientBuffer);
        boost::system::error_code writeError;

        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR)
            std::cerr << "sending frame: \n" << &frame << std::endl;

        size_t numWritten = boost::asio::write(
            *outgoingSocket, boost::asio::buffer(clientBuffer), writeError);
        if (writeError) {
            if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
                std::cerr << "Error contacting server, retrying ..." << std::endl;
            }
            continue;
        }
        boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE>
            serverBuffer;
        boost::system::error_code error;
        //size_t numRead = outgoingSocket->read_some(boost::asio::buffer(serverBuffer), error);
        size_t numRead = boost::asio::read(*outgoingSocket ,boost::asio::buffer(serverBuffer), error);

        if (error == boost::asio::error::eof) {
            if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
                std::cout << "EOF from server:" << std::endl; // Server closed connection
            }
            break;
        }
        else if (error)
        {
            throw boost::system::system_error(error); // Some other error.
        }
        else
        {
            // Parse the id provided by the server
            cse125framing::ServerFrame frame;
            cse125framing::deserialize(&frame, serverBuffer);
            clientId = frame.id;
            // TODO: Account for endianess differences
            if (DEBUG_LEVEL >= LOG_LEVEL_INFO) {
                std::cout << "Client id is now " << clientId << std::endl;
            }
            break;
        }
    }
}

void sendDataToServer(MovementKey movementKey, vec3 cameraDirection)
{
    cse125framing::ClientFrame frame;
    frame.id = clientId;
    frame.ctr = clientFrameCtr++;
    frame.movementKey = movementKey;
    frame.cameraDirection = glm::vec3(cameraDirection);

    boost::array<char, cse125framing::CLIENT_FRAME_BUFFER_SIZE> clientBuffer;
    cse125framing::serialize(&frame, clientBuffer);
    boost::system::error_code writeError;

    // std::cerr << "sending frame: " << std::endl << &frame << std::endl;

    size_t numWritten = boost::asio::write(*outgoingSocket, boost::asio::buffer(clientBuffer), writeError);
    if (writeError) {
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cerr << "Error sending packet to server, continuing ..." << std::endl;
            std::cerr << writeError << std::endl;
        }
    }
}

void receiveDataFromServer()
{
    // Wait for server to respond.
    cse125framing::ServerFrame serverFrame;
    boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;
    boost::system::error_code error;

    //size_t numRead = outgoingSocket->read_some(boost::asio::buffer(serverBuffer), error);
    size_t numRead = boost::asio::read(*outgoingSocket ,boost::asio::buffer(serverBuffer), error);

    if (error == boost::asio::error::eof) {
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cout << "EOF from server." << std::endl; // Server closed connection
        }
    }
    else if (error) {
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cerr << "Error reading from server: " << error << std::endl; // Some other error.
        }
    }
    else
    {
        cse125framing::deserialize(&serverFrame, serverBuffer);

        assert(numRead == cse125framing::SERVER_FRAME_BUFFER_SIZE);
        if (DEBUG_LEVEL >= LOG_LEVEL_FINE) {
            std::cout << "Received reply from server." << std::endl;
            std::cout << numRead << " " << sizeof(cse125framing::ServerFrame) << std::endl;
            std::cout << &serverFrame << std::endl;
        }

        // Use the data. 
        for (int i = 0; i < cse125constants::NUM_PLAYERS; i++)
        {
            const glm::vec3 pos = glm::vec3(serverFrame.players[i].playerPosition);
            const glm::vec3 dir = glm::vec3(serverFrame.players[i].playerDirection);
            players[i]->moveCar(dir, glm::vec3(0.0f, 1.0f, 0.0f), pos);
            players[i]->setCrownStatus(serverFrame.players[i].hasCrown);
        }
        scene.camera->setPosition(glm::vec3(serverFrame.players[clientId].playerPosition));
    }
}

void cleanupConnection()
{
    boost::system::error_code errorCode;
    outgoingSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, errorCode);
    if (errorCode) {
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cerr << "Error shutting down socket" << std::endl;
        }
    }
    outgoingSocket->close(errorCode);
    if (errorCode) {
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cerr << "Error closing socket" << std::endl;
        }
    }
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
      press 'R' to reset camera.
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
    for (int i = 0; i < cse125constants::NUM_PLAYERS; i++) {
        players[i]->setPlayer(scene.node["player" + std::to_string(i)]);
        players[i]->setCrown(scene.node["crown" + std::to_string(i)]);
    }

    lastRenderTime = glutGet(GLUT_ELAPSED_TIME);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene.draw();

    glutSwapBuffers();
    glFlush();
}

void saveScreenShot(const char* filename = "test.png")
{
    int currentwidth = glutGet(GLUT_WINDOW_WIDTH);
    int currentheight = glutGet(GLUT_WINDOW_HEIGHT);
    Screenshot imag = Screenshot(currentwidth, currentheight);
    imag.save(filename);
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

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case 27: // Escape to quit
            //cleanupConnection();
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
        case 'r':
            scene.camera -> aspect_default = float(glutGet(GLUT_WINDOW_WIDTH))/float(glutGet(GLUT_WINDOW_HEIGHT));
            scene.camera -> reset();
            glutPostRedisplay();
            break;
        case 'a':
            //handleMoveLeft();
            triggers["left"] = true;
            glutPostRedisplay();
            break;
        case 'd':
            //handleMoveRight();
            triggers["right"] = true;
            glutPostRedisplay();
            break;
        case 'w':
            //handleMoveForward();
            triggers["up"] = true;
            glutPostRedisplay();
            break;
        case 's':
            triggers["down"] = true;
            handleMoveBackward();
            glutPostRedisplay();
            break;
        case 'z':
            //scene.camera -> zoom(1.1f);
            players[0]->setCrownStatus(true);
            std::cout << players[0]->getCrownStatus() << std::endl;
            glutPostRedisplay();
            break;
        case 'x':
            players[0]->setCrownStatus(false);
            std::cout << players[0]->getCrownStatus() << std::endl;
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
        /*
        case ' ':
            hw3AutoScreenshots();
            glutPostRedisplay();
            break;
        */
        default:
            glutPostRedisplay();
            break;
    }
}

void keyboardUp(unsigned char key, int x, int y){
    switch(key){
        case 'a':
            triggers["left"] = false;
            glutPostRedisplay();
            break;
        case 'd':
            triggers["right"] = false;
            glutPostRedisplay();
            break;
        case 'w':
            triggers["up"] = false;
            glutPostRedisplay();
            break;
        case 's':
            triggers["down"] = false;
            glutPostRedisplay();
            break;
        default:
            glutPostRedisplay();
            break;
    }
}

void specialKey(int key, int x, int y) {
    glm::vec3 camera = (scene.camera->target - scene.camera->eye) *
        glm::vec3(1.0f, 0.0f, 1.0f);
    switch (key) {
    case GLUT_KEY_UP: // up
        triggers["up"] = true;
        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN: // down
        triggers["down"] = true;
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT: // right
        triggers["right"] = true;
        glutPostRedisplay();
        break;
    case GLUT_KEY_LEFT: // left
        triggers["left"] = true;
        glutPostRedisplay();
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
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN: // down
            //handleMoveBackward();
            triggers["down"] = false;
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT: // right
            //handleMoveRight();
            triggers["right"] = false;
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT: // left
            //handleMoveLeft();
            triggers["left"] = false;
            glutPostRedisplay();
            break;
    }
}

void idle() {

    /*

    // Update wheel animation
    // Render every half second
    if (time - lastRenderTime > 50) {
        float speed = 5.0f;
        p0.spinWheels(speed);
        p1.spinWheels(speed);
        p2.spinWheels(speed);
        p3.spinWheels(speed);
        glutPostRedisplay();
    }
    */
    int time = glutGet(GLUT_ELAPSED_TIME);
	float speed = 5.0f;
    if (time - lastRenderTime > 50) {
        for (int i = 0; i < cse125constants::NUM_PLAYERS; i++) {
            players[i]->spinWheels(speed);
            players[i]->bobCrown(time);
        }
        glutPostRedisplay();
		lastRenderTime = time;
    }

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

    if (clientId != cse125constants::DEFAULT_CLIENT_ID) {
        receiveDataFromServer();
    }
    glutPostRedisplay();
}

void mouseMovement(int x, int y) {
	int maxDelta = 100;
	int dx = glm::clamp(x - mouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(y - mouseY, -maxDelta, maxDelta);

	mouseX = x;
	mouseY = y;

    if (dx != 0 || dy != 0) {
        scene.camera->rotateRight(dx);
        scene.camera->rotateUp(dy);
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

    // Network setup
    cse125config::initializeConfig("../../config.json");
    outgoingResolver = std::make_unique<boost::asio::ip::tcp::resolver>(outgoingContext);
    outgoingEndpoints = outgoingResolver->resolve(cse125config::SERVER_HOST, cse125config::SERVER_PORT);
    outgoingSocket = std::make_unique<boost::asio::ip::tcp::socket>(outgoingContext);
    boost::asio::connect(*outgoingSocket, outgoingEndpoints);
    requestClientId();
    
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
