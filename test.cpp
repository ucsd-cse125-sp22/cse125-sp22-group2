#include <cstdio>
#include <irrKlang.h>

int main(void) {
	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();

	engine->play2D("audio/Skrrt.wav"); 

	engine->drop(); 
}
