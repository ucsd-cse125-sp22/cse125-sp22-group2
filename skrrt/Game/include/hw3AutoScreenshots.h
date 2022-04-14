/**************************************************
The helper function that generates screenshots for HW2
*****************************************************/
void keyboard(unsigned char key, int x, int y);
void specialKey(int key, int x, int y);
void display(void);
void saveScreenShot(const char* filename);
void hw3AutoScreenshots(){
    scene.camera -> reset();
    scene.shader -> enablelighting = false;
    display();
    saveScreenShot("image-00.png");
    
    keyboard('l',0,0);
    display();
    saveScreenShot("image-01.png");
    
    keyboard('a',0,0);
    keyboard('a',0,0);
    keyboard('a',0,0);
    keyboard('a',0,0);
    keyboard('a',0,0);
    specialKey(GLUT_KEY_RIGHT,0,0);
    specialKey(GLUT_KEY_RIGHT,0,0);
    specialKey(GLUT_KEY_RIGHT,0,0);
    display();
    saveScreenShot("image-02.png");
    
    for (int count = 0; count < 8; count ++){
        keyboard('z',0,0);
    }
    for (int count = 0; count < 19; count ++){
        specialKey(GLUT_KEY_LEFT,0,0);
    }
    for (int count = 0; count < 2; count ++){
        specialKey(GLUT_KEY_DOWN,0,0);
    }
    display();
    saveScreenShot("image-03.png");
    
    scene.camera -> reset();
    scene.shader -> enablelighting = false;
    display();
}
