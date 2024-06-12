#include <iostream>
#include <thread>
#include <cerrno>
#include <csignal>
#include <memory>
#include <atomic>
#include <cstring>
#include <string>

#include "Logger.h"
#include "SharedTypes.h"
#include "SocketFactory.h"
#include "CircularSPSCQueue.h"

std::atomic<bool> run{true};

std::shared_ptr<Logger> logger_(Logger::getLogger("ServerLogs.log"));
const int BUFFER_SIZE = 500;

void sigHandler(int signal) {
	logger_->info_log("SIGINT received, stopping... ");
	run = false;
}

int main(int argc, char* argv[]) {

	std::signal(SIGINT, sigHandler);

	CircularSPSCQueue<USED_TYPE> lcBuffer(2048);

	//Pusher Thread
	/*
	* read from socket
	* push element into circular buffer
	*/
	std::thread producerThread([&lcBuffer]() {
		Network::SocketFactory sockFactory(Network::EntityType::Server, std::string("read"), BUFFER_SIZE);
		int sockfd = sockFactory.createSocket(50);
		if (sockfd < 0) {
			logger_->error_log("Producer socket creation failed");
			run = false;
			return;
		}

		//int flags = RWF_NOWAIT;
		std::vector<USED_TYPE> buffer(BUFFER_SIZE);
		int size = 0;
		while (run) {
			
			size = sockFactory.readValues(sockfd, buffer);
			if (size == 0) {
				continue;
			} else if (size < 0) {
				run = false;
			}

			for(int i = 0; i < size; ++i) {
				while(run && !lcBuffer.push(buffer[i]));
			}
		}

		sockFactory.closeSocket(sockfd);
	});

	// Consumer Thread	
	/*
	* pop element from circular buffer
	* write element to socket
	*/

	std::thread consumerThread([&lcBuffer]() {
		Network::SocketFactory sockFactory(Network::EntityType::Server, std::string("write"), BUFFER_SIZE);
		int sockfd = sockFactory.createSocket(50);
		if (sockfd < 0) {
			logger_->error_log("Consumer socket creation failed");
			run = false;
			return;
		}

		USED_TYPE val;
		//int flags = RWF_NOWAIT; 
		std::vector<USED_TYPE> buffer(BUFFER_SIZE);
		uint32_t size = 0;
		while (run) {
			
			while(size != buffer.size()) {
				if (lcBuffer.pop(val)) {
					buffer[size] = val;
					++size;
				} else {
					break;
				}
			}
			if(size != buffer.size()) {
				continue;
			}

			if (sockFactory.writeValues(sockfd, buffer) < 0) {
				run = false;
			} 

			size = 0;
		}

		sockFactory.closeSocket(sockfd);
	});

	consumerThread.join();
	producerThread.join();
	logger_->info_log("Server stopped, exiting...");

	return EXIT_SUCCESS;
}