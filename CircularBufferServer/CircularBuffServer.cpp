#include <iostream>
#include <thread>
#include <cerrno>
#include <csignal>
#include <memory>
#include <atomic>
#include <string>

#include <unistd.h>

#include "Logger.h"
#include "SharedTypes.h"
#include "SocketFactory.h"
#include "CircularSPSCQueue.h"

/*
* create companion program
* write to socket and read from consumer socket to verify
*/

std::atomic<bool> stop{false};
//using EntityType = SocketFactory::EntityType;

std::shared_ptr<Logger> logger_(Logger::getLogger());

void sigHandler(int signal) {
	logger_->info_log("SIGINT received, setting stop");
	stop = true;
}

int main(int argc, char* argv[]) {

	std::signal(SIGINT, sigHandler);

	CircularSPSCQueue<uint64_t> lcBuffer(10);

	//Pusher Thread
	/*
	* read from socket
	* push element into circular buffer
	*/
	std::thread producerThread([&lcBuffer]() {
		SocketFactory sockFactory(EntityType::Server, std::string("read"));
		int sockfd = sockFactory.createSocket();
		if (sockfd < 0) {
			logger_->error_log("Producer socket creation failed");
			stop = true;
			return;
		}

/*
		if (!sockFactory.acceptConnection(sockfd)) {
			sockFactory.closeSocket(sockfd);
			logger_->error_log("Producer accept failed");
			stop = true;
			return;
		}
*/
		unsigned char buffer[sizeof(uint64_t)]; // !!! make type dynamic
		// !!! not an atomic operation because of ! (not)
		// Used for consumer thread also
		int flags = MSG_DONTWAIT;
		while (!stop) {
			auto ret = ::recv(sockfd, &buffer, sizeof(buffer), flags);
			if (ret < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
				continue;
			} else if (ret < 0) {
				logger_->error_log("Push read failed (" + std::to_string(errno) + ")");
				stop = true;
			}
			auto val = reinterpret_cast<uint64_t>(buffer); // !!! make type dynamic
			lcBuffer.push(val);
		}

		sockFactory.closeSocket(sockfd);
	});

	// Consumer Thread	
	/*
	* pop element from circular buffer
	* write element to socket
	*/

	std::thread consumerThread([&lcBuffer]() {
		SocketFactory sockFactory(EntityType::Server, std::string("write"));
		int sockfd = sockFactory.createSocket();
		if (sockfd < 0) {
			logger_->error_log("Consumer socket creation failed");
			stop = true;
			return;
		}
/*
		if (!sockFactory.acceptConnection(sockfd)) {
			logger_->error_log("Consumer accept failed");
			sockFactory.closeSocket(sockfd);
			stop = true;
			return;
		}
*/

		uint64_t val; // !!! make this type dynamic
		while (!stop) {
			if (!lcBuffer.pop(val)) {
				continue;
			}
			int flags = 0; // same as write, check options !!!
			if (::send(sockfd, std::addressof(val), sizeof(val), flags) < 0) {
				logger_->error_log("consumer write failed");
				stop = true;
				return;
			}
		}

		sockFactory.closeSocket(sockfd);
	});

	// !!! check if an error occured for threads?
	consumerThread.join();
	producerThread.join();
	logger_->info_log("Server stopped, exiting...");

	return EXIT_SUCCESS;
}