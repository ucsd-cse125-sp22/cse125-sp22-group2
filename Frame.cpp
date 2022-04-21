#include <iostream>
#include <string>
#include "Frame.hpp"

using namespace cse125framing;

std::ostream& cse125framing::operator<<(std::ostream& os, const Action& action)
{
	std::string str;
	switch (action) {
	case Action::JUMP:
		str = "JUMP";
		break;
	case Action::LEAVE_TRAIL:
		str = "LEAVE_TRAIL";
		break;
	case Action::LOSE_CROWN:
		str = "LOSE_CROWN";
		break;
	case Action::MOVE:
		str = "MOVE";
		break;
	case Action::NO_ACTION:
		str = "NO_ACTION";
		break;
	case Action::REDO_MAKEUP:
		str = "REDO_MAKEUP";
		break;
	case Action::TAKE_CROWN:
		str = "TAKE_CROWN";
		break;
	}

	os << str;
	return os;
}
//
//std::ostream& cse125framing::operator<<(std::ostream& os, const Frame* frame)
//{
//	const vec4& v4 = frame->playerPosition;
//	const vec3& v3 = frame->movementDirection;
//	os << "Player position: " << "(" << v4.x << ", " << v4.y << ", " << v4.z <<  ", " << v4.w << ")" << std::endl;
//	os << "Movement direction: " << "(" << v3.x << ", " << v3.y << ", " << v3.z << ")" << std::endl;
//	os << "Makeup level: " << frame->makeupLevel << std::endl;
//	os << "Score: " << frame->score << std::endl;
//	os << "Has crown: " << frame->hasCrown << std::endl;
//	os << "Action: " << frame->action << std::endl;
//
//	return os;
//}

void cse125framing::serialize(const ClientFrame* frame, boost::array<char, CLIENT_FRAME_BUFFER_SIZE>& buffer)
{
	std::memcpy(&buffer, frame, sizeof(ClientFrame));
}

void cse125framing::deserialize(ClientFrame* frame, const boost::array<char, CLIENT_FRAME_BUFFER_SIZE>& buffer)
{
	std::memcpy(frame, &buffer, sizeof(ClientFrame));
}

void cse125framing::serialize(const ServerFrame* frame, boost::array<char, SERVER_FRAME_BUFFER_SIZE>& buffer)
{
	std::memcpy(&buffer, frame, sizeof(ServerFrame));

}

void cse125framing::deserialize(ServerFrame* frame, const boost::array<char, SERVER_FRAME_BUFFER_SIZE>& buffer)
{
	std::memcpy(frame, &buffer, sizeof(ClientFrame));

}
