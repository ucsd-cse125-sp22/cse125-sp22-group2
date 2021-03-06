#include <iostream>
#include <string>
#include "Frame.hpp"

std::ostream& cse125framing::operator<<(std::ostream& os, const ClientFrame* frame)
{
	const vec3& camera = frame->cameraDirection;
	os << "Client frame:" << std::endl;
	os << "id: " << frame->id << std::endl;
	os << "ctr: " << frame->ctr << std::endl;
	os << "camera: " << "(" << camera.x << ", " << camera.y << ", " << camera.z << ")" << std::endl;
	return os;
}

std::ostream& cse125framing::operator<<(std::ostream& os, const ServerFrame* frame)
{
	os << "Server frame:" << std::endl;
	os << "id: " << frame->id << std::endl;
	os << "Server frame ctr: " << frame->ctr << std::endl;
	//os << "Position: " << "(" << frame->playerPosition.x << ", " << frame->playerPosition.y << ", " << frame->playerPosition.z << ")" << std::endl;
	//os << "Direction: " << "(" << frame->playerDirection.x << ", " << frame->playerDirection.y << ", " << frame->playerDirection.z << ")" << std::endl;
	return os;
}

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
	std::memcpy(frame, &buffer, sizeof(ServerFrame));
}
