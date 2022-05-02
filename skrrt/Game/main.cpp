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
#include "Game.h"
#include "Player.h"

#include "../../Frame.hpp"
#include "Debug.h"


static const int width = 1600;
static const int height = 1100;
static const char* title = "Scene viewer";
static const glm::vec4 background(0.1f, 0.2f, 0.3f, 1.0f);
static Scene scene;
static Player p0, p1, p2, p3;
static Game game(&p0, &p1, &p2, &p3);

static int lastRenderTime = 0;

boost::asio::io_context outgoingContext;
boost::asio::ip::tcp::resolver outgoingResolver = boost::asio::ip::tcp::resolver(outgoingContext);
boost::asio::ip::tcp::resolver::results_type outgoingEndpoints = outgoingResolver.resolve(cse125constants::SERVER_HOST, cse125constants::SERVER_PORT);
boost::asio::ip::tcp::socket outgoingSocket = boost::asio::ip::tcp::socket(outgoingContext);

int clientId = -1; // this client's unique id
int clientFrameCtr = 0;
static int mouseX = 0.0f;
static int mouseY = 0.0f;

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
        size_t numWritten = boost::asio::write(outgoingSocket, boost::asio::buffer(clientBuffer), writeError);
        if (writeError) {
            if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
                std::cerr << "Error contacting server, retrying ..." << std::endl;
            }
            continue;
        }
        boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;
        boost::system::error_code error;
        size_t numRead = outgoingSocket.read_some(boost::asio::buffer(serverBuffer), error);

        if (error == boost::asio::error::eof) {
            if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
                std::cout << "EOF from server:" << std::endl; // Server closed connection
            }
            break;
        }
        else if (error) {
            throw boost::system::system_error(error); // Some other error.
        }
        else {
            // Parse the id provided by the server
            // TODO: Account for endianess differences
            std::memcpy(&clientId, &serverBuffer, sizeof(int));
            if (DEBUG_LEVEL >= LOG_LEVEL_INFO) {
                std::cout << "Client id is now " << clientId << std::endl;
            }
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
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cerr << "Error sending packet to server, continuing ..." << std::endl;
            std::cerr << writeError << std::endl;
        }
    }
    else {
        if (DEBUG_LEVEL >= LOG_LEVEL_FINE) {
            std::cout << "Successfully sent frame # " << clientFrameCtr << " to server." << std::endl;
        }
    }

    // Wait for server to respond. With multiple clients, this listening step must be
    // moved into a separate, asynchronous thread.
    cse125framing::ServerFrame serverFrame;
    boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;
    boost::system::error_code error;
    //size_t numRead = outgoingSocket.read_some(boost::asio::buffer(serverBuffer), error);

    size_t numRead = boost::asio::read(outgoingSocket, boost::asio::buffer(serverBuffer));


    if (error == boost::asio::error::eof) {
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cout << "EOF from server." << std::endl; // Server closed connection
        }
    }
    else if (error) {
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cerr << "Error reading from server." << std::endl; // Some other error.
        }
    }
    else {
        cse125framing::deserialize(&serverFrame, serverBuffer);
        if (DEBUG_LEVEL >= LOG_LEVEL_FINE) {
            std::cout << "Received reply from server." << std::endl;
            std::cout << numRead << " " << sizeof(cse125framing::ServerFrame) << std::endl;
            std::cout << &serverFrame << std::endl;
        }

        // Use the data
        const glm::vec3 pos = glm::vec3(serverFrame.playerPosition);
        const glm::vec3 dir = glm::vec3(serverFrame.playerDirection);
        scene.node["car1"]->modeltransforms[0] = glm::translate(pos)  * glm::scale(vec3(0.5f, 0.5f, 0.5)) * glm::rotate(-1.0f* glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f)) * glm::inverse(glm::lookAt(glm::vec3(0), dir, glm::vec3(0,1,0)));
        scene.camera->setPosition(pos);
    }
}

void cleanupConnection() {
    boost::system::error_code errorCode;
    outgoingSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, errorCode);
    if (errorCode) {
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cerr << "Error shutting down socket" << std::endl;
        }
    }
    outgoingSocket.close(errorCode);
    if (errorCode) {
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cerr << "Error closing socket" << std::endl;
        }
    }
}

class GraphicsSession : public std::enable_shared_from_this<GraphicsSession>
{
public:
    GraphicsSession(boost::asio::ip::tcp::socket socket)
        : socket(std::move(socket))
    {
    }

    void start()
    {
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        socket.async_read_some(boost::asio::buffer(data, max_length),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    do_write(length);
                }
            });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        boost::asio::async_write(socket, boost::asio::buffer(data, length),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    do_read();
                }
            });
    }

    boost::asio::ip::tcp::socket socket;
    enum { max_length = 1024 };
    char data[max_length];
};

class GraphicsServer
{
public:
    GraphicsServer(boost::asio::io_context& io_context, short port)
        : acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor.async_accept(
            [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<GraphicsSession>(std::move(socket))->start();
                }

                do_accept();
            });
    }

    boost::asio::ip::tcp::acceptor acceptor;
};
void launchServer(short port) {
    boost::asio::io_context io_context;

    GraphicsServer s(io_context, port);

    std::cout << "Before io_context.run()" << std::endl;
    io_context.run();
}

void printHelp(){
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

void initialize(void){
    printHelp();
    glClearColor(background[0], background[1], background[2], background[3]); // background color
    glViewport(0,0,width,height);
    
    // Initialize scene
    scene.init();

    // Set up players
    p0.setPlayer(scene.node["player0"]); 
    p1.setPlayer(scene.node["player1"]); 
    p2.setPlayer(scene.node["player2"]); 
    p3.setPlayer(scene.node["player3"]);

    lastRenderTime = glutGet(GLUT_ELAPSED_TIME);

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

void handleMoveForward() {
    sendDataToServer(cse125framing::MovementKey::FORWARD, scene.camera->forwardVectorXZ());
}

void handleMoveBackward() {
    sendDataToServer(cse125framing::MovementKey::BACKWARD, scene.camera->forwardVectorXZ());
}

void handleMoveRight() {
    sendDataToServer(cse125framing::MovementKey::RIGHT, scene.camera->forwardVectorXZ());

}

void handleMoveLeft() {
    sendDataToServer(cse125framing::MovementKey::LEFT, scene.camera->forwardVectorXZ());
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case 27: // Escape to quit
            //cleanupConnection();
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
            handleMoveLeft();
            std::cout << "left" << std::endl;
            //glm::vec3 t = scene.camera->target - scene.camera->eye

            //p0.moveCar(glm::vec3(scene.camera->), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
            glutPostRedisplay();
            break;
        case 'd':
            handleMoveRight();
            std::cout << "right" << std::endl;
            //p0.moveCar(glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            glutPostRedisplay();
            break;
        case 'w':
            handleMoveForward();
            std::cout << "forward" << std::endl;
            //p0.moveCar(glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            glutPostRedisplay();
            break;
        case 's':
            handleMoveBackward();
            std::cout << "back" << std::endl;
            //p0.moveCar(glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
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
    switch (key) {
        case GLUT_KEY_UP: // up
/*
            scene.camera -> rotateUp(-10.0f);
            //sendDataToServer(cse125framing::MovementKey::FORWARD, camera);
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN: // down
            scene.camera -> rotateUp(10.0f);
            //sendDataToServer(cse125framing::MovementKey::BACKWARD, camera);
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT: // right
            scene.camera -> rotateRight(-10.0f);
            //sendDataToServer(cse125framing::MovementKey::RIGHT, camera);
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT: // left
            scene.camera -> rotateRight(10.0f);
            //sendDataToServer(cse125framing::MovementKey::LEFT, camera);
*/
            handleMoveForward();
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN: // down
            handleMoveBackward();
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT: // right
            handleMoveRight();
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT: // left
            handleMoveLeft();
            glutPostRedisplay();
            break;
    }
}

void idle() {
    // Idle loop for logic 
    // Get's called anytime there isn't a keyboard event
    // Packet receiving stuff 

    int time = glutGet(GLUT_ELAPSED_TIME);

    // Update wheel animation
    // Render every half second
    if (time - lastRenderTime > 50) {
        float speed = 5.0f;
        p0.spinWheels(speed);
        p1.spinWheels(speed);
        p2.spinWheels(speed);
        p3.spinWheels(speed);
        glutPostRedisplay();
        lastRenderTime = time;
    }
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
    /*
    // Specify the port on which the client will listen
    short port = 8000;
    //// Launch a thread in which to run the listener
    bool threadSuccess = true;
    boost::thread listenerThread;
    try {
        listenerThread = boost::thread(launchServer, port);
    }
    catch (std::exception& e)
    { 
		threadSuccess = false;
		std::cerr << "Exception: " << e.what() << "\n";
    }
    */
    
    initialize();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutIdleFunc(idle);
    glutPassiveMotionFunc(mouseMovement);
    glutMotionFunc(mouseMovement);
    
    
    glutMainLoop();
    /*
    if (threadSuccess) {
        listenerThread.join();
    }
    */

 

  
	return 0;   /* ANSI C requires main to return int. */
}
