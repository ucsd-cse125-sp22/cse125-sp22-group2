#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "main.h"
#include "Cube.h"
#include "shader.h"
#include "Camera.h"

class Window
{
private: 
	static bool objInit; 
public:
	// Window Properties 
	static int width; 
	static int height; 

	// Objects to render 
	static Cube* cube; 
	static Cube* floor; 

	// Shader Program 
	static GLuint shaderProgram; 

	// Act as Constructors and Destructors 
	static bool initializeProgram(); 
	static bool initializeObjects();
	static void cleanUp(); 

	// For the Window 
	static GLFWwindow* createWindow(int width, int height); 
	static void resizeCallback(GLFWwindow* window, int width, int height); 

	// update and draw functions 
	static void idleCallback(); 
	static void displayCallback(GLFWwindow*); 

	// helper to reset the camera 
	static void resetCamera(); 

	// Callbacks - for interation 
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods); 
	static void mouseCallback(GLFWwindow* window, int button, int action, int mods); 
	static void cursorCallback(GLFWwindow* window, double currX, double currY); 

};

#endif 
