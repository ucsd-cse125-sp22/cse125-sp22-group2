#include <string>
#include <boost/asio.hpp>

#include "../../../Definitions.hpp"
#include "../../../Frame.hpp"

namespace cse125networkclient {
	class NetworkClient {
	public:
		NetworkClient(std::string host, std::string port);
		~NetworkClient();
		int getId();
		size_t send(MovementKey movementKey, vec3 cameraDirection, boost::system::error_code* errorCode);
		size_t receive(cse125framing::ServerFrame* frame, boost::system::error_code* errorCode);
		void closeConnection();
	private:
		int clientId;
		int clientFrameCtr;
		boost::asio::io_context context;
		std::unique_ptr<boost::asio::ip::tcp::resolver> resolver;
		boost::asio::ip::tcp::resolver::results_type endpoints;
		std::unique_ptr<boost::asio::ip::tcp::socket> socket;
	};
}

