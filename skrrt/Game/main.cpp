#include <stdlib.h>
#include <iostream>
#include <string>
// OSX systems need their own headers
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
// Use of degrees is deprecated. Use radians for GLM functions
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "Screenshot.h"
#include "Scene.h"
#include "../../Frame.hpp"



static const int width = 800;
static const int height = 600;
static const char* title = "Scene viewer";
static const glm::vec4 background(0.1f, 0.2f, 0.3f, 1.0f);
static Scene scene;

boost::asio::io_context outgoingContext;
boost::asio::ip::tcp::resolver outgoingResolver = boost::asio::ip::tcp::resolver(outgoingContext);
boost::asio::ip::tcp::resolver::results_type outgoingEndpoints = outgoingResolver.resolve(cse125constants::SERVER_HOST, cse125constants::SERVER_PORT);
boost::asio::ip::tcp::socket outgoingSocket = boost::asio::ip::tcp::socket(outgoingContext);

int clientId = -1; // this client's unique id
int clientFrameCtr = 0;

#include "hw3AutoScreenshots.h"

// Asks the server to provide this client with its unique id that the client can use for all future communication
// with the server
void requestClientId() {
    // Set up connection to server for outgoing data

    cse125framing::ClientFrame frame;
    frame.id = cse125constants::DEFAULT_CLIENT_ID;

    // Get this client's id from the server
    while (true) {
        // Send a frame with an id of -1
        boost::array<char, cse125framing::CLIENT_FRAME_BUFFER_SIZE> clientBuffer;
        cse125framing::serialize(&frame, clientBuffer);
        boost::system::error_code writeError;

        std::cerr << "sending frame: \n" <<  &frame << std::endl;

        size_t numWritten = boost::asio::write(outgoingSocket, boost::asio::buffer(clientBuffer), writeError);
        if (writeError) {
            std::cerr << "Error contacting server, retrying ..." << std::endl;
            continue;
        }
        boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;
        boost::system::error_code error;
        size_t numRead = outgoingSocket.read_some(boost::asio::buffer(serverBuffer), error);

        if (error == boost::asio::error::eof) {
            std::cout << "EOF from server:" << std::endl; // Server closed connection
            break;
        }
        else if (error) {
            throw boost::system::system_error(error); // Some other error.
        }
        else {
            // Parse the id provided by the server
            // TODO: Account for endianess differences
            std::memcpy(&clientId, &serverBuffer, sizeof(int));
            std::cout << "Client id is now " << clientId << std::endl;
            break;
        }
    }
}

void sendDataToServer(cse125framing::MovementKey movementKey, vec3 cameraDirection) {
    cse125framing::ClientFrame frame;
    frame.id = clientId;
    frame.ctr = clientFrameCtr++;
    frame.movementKey = movementKey;
    frame.cameraDirection = glm::vec3(cameraDirection);

    boost::array<char, cse125framing::CLIENT_FRAME_BUFFER_SIZE> clientBuffer;
    cse125framing::serialize(&frame, clientBuffer);
    boost::system::error_code writeError;
    size_t numWritten = boost::asio::write(outgoingSocket, boost::asio::buffer(clientBuffer), writeError);
    if (writeError) {
        std::cerr << "Error sending packet to server, continuing ..." << std::endl;
        std::cerr << writeError << std::endl;
    }
    else {
        std::cout << "Successfully sent frame # " << clientFrameCtr << " to server." << std::endl;
    }

    // Wait for server to respond. With multiple clients, this listening step must be
    // moved into a separate, asynchronous thread.
    cse125framing::ServerFrame serverFrame;
    boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;
    boost::system::error_code error;
    size_t numRead = outgoingSocket.read_some(boost::asio::buffer(serverBuffer), error);

    //size_t numRead = boost::asio::read(outgoingSocket, boost::asio::buffer(serverBuffer));


    if (error == boost::asio::error::eof) {
        std::cout << "EOF from server." << std::endl; // Server closed connection
    }
    else if (error) {
        std::cerr << "Error reading from server." << std::endl; // Some other error.
    }
    else {
        cse125framing::deserialize(&serverFrame, serverBuffer);
        std::cout << "Received reply from server." << std::endl;
        std::cout << numRead << " " << sizeof(cse125framing::ServerFrame) << std::endl;
        std::cout << &serverFrame << std::endl;

        // Use the data
        const glm::vec3 pos = glm::vec3(serverFrame.players[clientId].playerPosition);
        scene.node["car1"]->modeltransforms[0] = glm::translate(pos)  * scene.node["car1"]->modeltransforms[0];
    }
}

void cleanupConnection() {
    boost::system::error_code errorCode;
    outgoingSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, errorCode);
    if (errorCode) {
        std::cerr << "Error shutting down socket" << std::endl;
    }
    outgoingSocket.close(errorCode);
    if (errorCode) {
        std::cerr << "Error closing socket" << std::endl;
    }
}

void printHelp(){
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

void initialize(void){
    printHelp();
    glClearColor(background[0], background[1], background[2], background[3]); // background color
    glViewport(0,0,width,height);
    
    // Initialize scene
    scene.init();

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    scene.draw();
    
    glutSwapBuffers();
    glFlush();
    
}

void saveScreenShot(const char* filename = "test.png"){
    int currentwidth = glutGet(GLUT_WINDOW_WIDTH);
    int currentheight = glutGet(GLUT_WINDOW_HEIGHT);
    Screenshot imag = Screenshot(currentwidth,currentheight);
    imag.save(filename);
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
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
            scene.camera -> aspect_default = float(glutGet(GLUT_WINDOW_WIDTH))/float(glutGet(GLUT_WINDOW_HEIGHT));
            scene.camera -> reset();
            glutPostRedisplay();
            break;
        case 'a':
            scene.camera -> zoom(0.9f);
            glutPostRedisplay();
            break;
        case 'z':
            scene.camera -> zoom(1.1f);
            glutPostRedisplay();
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

void specialKey(int key, int x, int y){
    glm::vec3 camera = (scene.camera->target - scene.camera->eye) * glm::vec3(1.0f, 0.0f, 1.0f);
    switch (key) {
        case GLUT_KEY_UP: // up
            //scene.camera -> rotateUp(-10.0f);
            sendDataToServer(cse125framing::MovementKey::FORWARD, camera);
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN: // down
            //scene.camera -> rotateUp(10.0f);
            sendDataToServer(cse125framing::MovementKey::BACKWARD, camera);
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT: // right
            //scene.camera -> rotateRight(-10.0f);
            sendDataToServer(cse125framing::MovementKey::RIGHT, camera);
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT: // left
            //scene.camera -> rotateRight(10.0f);
            sendDataToServer(cse125framing::MovementKey::LEFT, camera);
            glutPostRedisplay();
            break;
    }
}

void idle() {
    // Idle loop for logic 
    // Get's called anytime there isn't a keyboard event
    // Packet receiving stuff 

}

int main(int argc, char** argv)
{
    // BEGIN CREATE WINDOW
    glutInit(&argc, argv);
    
#ifdef __APPLE__
    glutInitDisplayMode( GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#else
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
#endif
    glutInitWindowSize(width, height);
    glutCreateWindow(title);
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    GLenum err = glewInit() ;
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    }
#endif
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    // END CREATE WINDOW
    boost::asio::connect(outgoingSocket, outgoingEndpoints);


    // Set this client's id
    requestClientId();

    // Set up listening socket "server"

    // Specify the port on which the client will listen
    //short port = 8000;
    //// Launch a thread in which to run the listener
    //bool threadSuccess = true;
    //boost::thread listenerThread;
    //try {
    //    listenerThread = boost::thread(launchServer, port);
    //}
    //catch (std::exception& e)
    //{
    //    threadSuccess = false;
    //    std::cerr << "Exception: " << e.what() << "\n";
    //}
    
    initialize();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutIdleFunc(idle);
    
    glutMainLoop();
    //if (threadSuccess) {
    //    listenerThread.join();
    //}

 

  
	return 0;   /* ANSI C requires main to return int. */
}
