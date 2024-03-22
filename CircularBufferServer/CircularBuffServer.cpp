#include <iostream>
#include <thread>
#include <cerrno>
#include <csignal>
#include <memory>
#include <atomic>
#include <cstring>
#include <string>
#include <unistd.h>

#include "Logger.h"
#include "SharedTypes.h"
#include "SocketFactory.h"
#include "CircularSPSCQueue.h"

std::atomic<bool> run{true};

std::shared_ptr<Logger> logger_(Logger::getLogger("ServerLogs.log"));

void sigHandler(int signal) {
	logger_->info_log("SIGINT received, stopping... ");
	run = false;
}

int main(int argc, char* argv[]) {

	std::signal(SIGINT, sigHandler);

	CircularSPSCQueue<USED_TYPE> lcBuffer(10);

	//Pusher Thread
	/*
	* read from socket
	* push element into circular buffer
	*/
	std::thread producerThread([&lcBuffer]() {
		Network::SocketFactory sockFactory(Network::EntityType::Server, std::string("read"));
		int sockfd = sockFactory.createSocket();
		if (sockfd < 0) {
			logger_->error_log("Producer socket creation failed");
			run = false;
			return;
		}

		USED_TYPE val = 0;
		int ret = 0;
		int flags = MSG_DONTWAIT;
		while (run) {
			
			ret = sockFactory.readVal(sockfd, val, flags);
			if (ret == 0) {
				continue;
			} else if (ret < 0) {
				run = false;
			}
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
		Network::SocketFactory sockFactory(Network::EntityType::Server, std::string("write"));
		int sockfd = sockFactory.createSocket();
		if (sockfd < 0) {
			logger_->error_log("Consumer socket creation failed");
			run = false;
			return;
		}

		USED_TYPE val;
		while (run) {
			if (!lcBuffer.pop(val)) {
				continue;
			}

			if (sockFactory.sendVal(sockfd, val) < 0) {
				run = false;
			}
		}

		sockFactory.closeSocket(sockfd);
	});

	consumerThread.join();
	producerThread.join();
	logger_->info_log("Server stopped, exiting...");

	return EXIT_SUCCESS;
}