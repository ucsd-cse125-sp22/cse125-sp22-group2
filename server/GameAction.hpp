#pragma once
#include <vector>
namespace cse125gameaction {

	// Each of these constants should be a power of 2
	const unsigned int MOVE_RIGHT = 1;
	const unsigned int MOVE_FORWARD = 2;
	const unsigned int MOVE_LEFT = 4;
	const unsigned int MOVE_BACKWARD = 8;
	const std::vector<unsigned int> allActions = {
		MOVE_RIGHT,
		MOVE_FORWARD,
		MOVE_LEFT,
		MOVE_BACKWARD
	};

	struct s {
		unsigned int w : 1;
		unsigned int a : 1;
		unsigned int s : 1;
		unsigned int d : 1;
	};

	class GameAction {
	public:
		GameAction();
		~GameAction();
		void moveRight();
		void moveFoward();
		void moveLeft();
		void moveBackward();
		std::vector<unsigned int> getActions();
	private:
		unsigned char flags;
		void setAction(const unsigned int& action);
		void clearAction(const unsigned int& action);
	};
}