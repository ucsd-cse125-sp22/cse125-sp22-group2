#include "Debug.h"

void cse125debug::log(int level, const char* msg) {
	if (level >= DEBUG_LEVEL) {
		std::cerr << msg;
	}
}
