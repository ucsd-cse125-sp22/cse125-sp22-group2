#include <string>
#include <boost/asio.hpp>

#include "../../../Definitions.hpp"
#include "../../../Frame.hpp"

namespace cse125networkclient {
	class NetworkClient {
	public:
		/**
		 * @brief Initializes the client's connection to the server with the given host and port 
		 * 
		 * @param host the host IP of the server
		 * @param port the port number of the server
		 */
		NetworkClient(std::string host, std::string port);
		~NetworkClient();

		/**
		 * @brief Gets the client's id that the server has assigned to it
		 *
		 * @return the id to be used by the graphics client
		 */
		int getId();

		/**
		 * @brief Sends a ClientFrame to the server with the player's input movement key and camera direction
		 *
		 * @param movementKey		the movement key the player pressed
		 * @param cameraDirection	the player's camera direction
		 * @param errorCode			the error (if any) set by the network call
		 * 
		 * @return the number of bytes successfully sent
		 */
		size_t send(MovementKey movementKey, vec3 cameraDirection, boost::system::error_code* errorCode);

		/**
		 * @brief Gets a ServerFrame from the server and initializes the given frame with the response data
		 *
		 * @param frame			the ServerFrame to initialize
		 * @param errorCode		the error (if any) set by the network call
		 *
		 * @return the number of bytes successfully read
		 */
		size_t receive(cse125framing::ServerFrame* frame, boost::system::error_code* errorCode);


		/**
		 * @brief Closes the connection established by this object
		 */
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

