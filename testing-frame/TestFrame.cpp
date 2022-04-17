// Tests for Frame.hpp and Frame.cpp 
// Invokes the test functions from main without the use of a testing framework

#include <string>
#include <boost/array.hpp>
#include "../Frame.hpp"
#include "../Frame.cpp"

void assertExpression(bool expr, std::string msg) {
	if (!expr) {
		std::cerr << "Assertion failed: " << msg << std::endl;
		exit(1);
	}
}

void testFrameSerialization() {
	// Base frame for comparison
	cse125framing::Frame f1;
	f1.action = cse125framing::Action::JUMP;
	f1.hasCrown = true;
	f1.makeupLevel = 10;
	f1.movementDirection = vec3(1, 2, 3);
	f1.playerPosition = vec4(4, 5, 6, 7);
	f1.score = 20;

	// Serialize the frame (corresponds to prearing it for transmission over a network)
	boost::array<char, cse125constants::FRAME_BUFFER_SIZE> buffer;
	cse125framing::serialize(&f1, buffer);

	// Deserialize the frame (corresponds to reading the frame from the network)
	cse125framing::Frame f2;
	cse125framing::deserialize(&f2, buffer);

	// Compare the resulting frame with the original.
	// If this test passes, then the procedure for sending data over the network
	// and retrieving it should be correct.
	assertExpression(f1.action == f2.action, "Action");
	assertExpression(f1.hasCrown == f2.hasCrown, "crown");
	assertExpression(f1.makeupLevel == f2.makeupLevel, "makeup");
	assertExpression(f1.movementDirection == f2.movementDirection, "direction");
	assertExpression(f1.playerPosition == f2.playerPosition, "position");
	assertExpression(f1.score == f2.score, "score");
}

int main(void) {
	testFrameSerialization();
	std::cout << "All tests passed!" << std::endl;
	return 0;
}
