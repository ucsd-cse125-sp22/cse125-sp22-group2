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

#include "../../Definitions.hpp"
#include "../../Frame.hpp"
#include "Scene.h"
#include "Screenshot.h"

// IMPORTANT: Include Screenshot.h before the boost inclusion to prevent a compiler error

static const int width = 800;
static const int height = 600;
static const char* title = "Scene viewer";
static const glm::vec4 background(0.1f, 0.2f, 0.3f, 1.0f);
static Scene scene;

boost::asio::io_context outgoingContext;
boost::asio::ip::tcp::resolver outgoingResolver(outgoingContext);
boost::asio::ip::tcp::resolver::results_type outgoingEndpoints =
    outgoingResolver.resolve(cse125constants::SERVER_HOST,
                             cse125constants::SERVER_PORT);
boost::asio::ip::tcp::socket outgoingSocket(outgoingContext);

int clientId = cse125constants::DEFAULT_CLIENT_ID; // this client's unique id
int clientFrameCtr = 0;

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

        std::cerr << "sending frame: \n" << &frame << std::endl;

        size_t numWritten = boost::asio::write(
            outgoingSocket, boost::asio::buffer(clientBuffer), writeError);
        if (writeError)
        {
            std::cerr << "Error contacting server, retrying ..." << std::endl;
            continue;
        }
        boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE>
            serverBuffer;
        boost::system::error_code error;
        size_t numRead =
            outgoingSocket.read_some(boost::asio::buffer(serverBuffer), error);

        if (error == boost::asio::error::eof)
        {
            std::cout << "EOF from server:"
                      << std::endl; // Server closed connection
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
            std::cout << "Client id is now " << clientId << std::endl;
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

    size_t numWritten = boost::asio::write(
        outgoingSocket, boost::asio::buffer(clientBuffer), writeError);
    if (writeError)
    {
        std::cerr << "Error sending packet to server, continuing ..."
                  << std::endl;
        std::cerr << writeError << std::endl;
    }

    // receiveDataFromServer();
}

void receiveDataFromServer()
{
    // Wait for server to respond. With multiple clients, this listening step
    // must be moved into a separate, asynchronous thread.
    cse125framing::ServerFrame serverFrame;
    boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;
    boost::system::error_code error;
    size_t numRead =
        outgoingSocket.read_some(boost::asio::buffer(serverBuffer), error);

    if (error == boost::asio::error::eof)
    {
        std::cout << "EOF from server."
                  << std::endl; // Server closed connection
    }
    else if (error)
    {
        std::cerr << "Error reading from server: " << error 
                  << std::endl; // Some other error.
    }
    else
    {
        cse125framing::deserialize(&serverFrame, serverBuffer);

        assert(numRead == cse125framing::SERVER_FRAME_BUFFER_SIZE);
        // std::cout << "numread: " << numRead << std::endl;
        // std::cout << "Received reply from server." << std::endl;
        // std::cout << &serverFrame << std::endl;

        // Use the data. 
        for (int i = 0; i < cse125constants::NUM_PLAYERS; i++)
        {
			const glm::vec3 pos = glm::vec3(serverFrame.players[0].playerPosition);
			const glm::vec3 dir = glm::vec3(serverFrame.players[0].playerDirection);
			scene.node["car" + std::to_string(i)]->modeltransforms[0] = glm::translate(pos)  * glm::scale(vec3(0.5f, 0.5f, 0.5)) * glm::rotate(-1.0f* glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f)) * glm::inverse(glm::lookAt(glm::vec3(0), dir, glm::vec3(0,1,0)));
			// scene.camera->setPosition(pos);
        }
    }
}

void cleanupConnection()
{
    boost::system::error_code errorCode;
    outgoingSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                            errorCode);
    if (errorCode)
    {
        std::cerr << "Error shutting down socket" << std::endl;
    }
    outgoingSocket.close(errorCode);
    if (errorCode)
    {
        std::cerr << "Error closing socket" << std::endl;
    }
}

void printHelp()
{
    std::cout << R"(
    Available commands:
      press 'H' to print this message again.
      press Esc to quit.
      press 'O' to save a screenshot.
      press the arrow keys to rotate camera.
      press 'A'/'Z' to zoom.
      press 'R' to reset camera.
      press 'L' to turn on/off the lighting.
    
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

void keyboard(unsigned char key, int x, int y)

{
    switch (key)
    {
    case 27: // Escape to quit
        cleanupConnection();
        exit(0);
        break;
    case 'h': // print help
        printHelp();
        break;
    case 'o': // save screenshot
        saveScreenShot();
        break;
    case 'r':
        scene.camera->aspect_default = float(glutGet(GLUT_WINDOW_WIDTH)) /
                                       float(glutGet(GLUT_WINDOW_HEIGHT));
        scene.camera->reset();
        glutPostRedisplay();
        break;
    case 'a':
        scene.camera->zoom(0.9f);
        glutPostRedisplay();
        break;
    case 'z':
        scene.camera->zoom(1.1f);
        glutPostRedisplay();
        break;
    case 'l':
        scene.shader->enablelighting = !(scene.shader->enablelighting);
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

void specialKey(int key, int x, int y)
{
    glm::vec3 camera = (scene.camera->target - scene.camera->eye) *
                       glm::vec3(1.0f, 0.0f, 1.0f);
    switch (key)
    {
    case GLUT_KEY_UP: // up
        sendDataToServer(MovementKey::FORWARD, camera);
        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN: // down
        sendDataToServer(MovementKey::BACKWARD, camera);
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT: // right
        sendDataToServer(MovementKey::RIGHT, camera);
        glutPostRedisplay();
        break;
    case GLUT_KEY_LEFT: // left
        sendDataToServer(MovementKey::LEFT, camera);
        glutPostRedisplay();
        break;
    }
}

void idle()
{
    // Gets called anytime there isn't a keyboard event
    // Only connect to server once the client has been registered
    if (clientId != cse125constants::DEFAULT_CLIENT_ID) {
        receiveDataFromServer();
    }
    glutPostRedisplay();
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
    boost::asio::connect(outgoingSocket, outgoingEndpoints);
    // Set this client's id
    requestClientId();

    initialize();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutIdleFunc(idle);
    glutMainLoop();

    return 0; /* ANSI C requires main to return int. */
}
