#include <iostream>
#include <csignal>
#include <atomic>
#include <cstring>
#include <string>
#include <cerrno>

#include "Logger.h"
#include "SharedTypes.h"
#include "SocketFactory.h"
#include "Timer.h"

std::atomic<bool> run{true};
std::shared_ptr<Logger> logger_(Logger::getLogger("ClientLogs.log"));
const int BUFFER_SIZE = 500;

void sigHandler(int sig) {
    logger_->info_log("SIGINT received, stopping threads...");
    run = false;
};

int main() {
    std::signal(SIGINT, sigHandler);

    Network::SocketFactory writeSockFact(Network::EntityType::Client, std::string("write"), BUFFER_SIZE);
    Network::SocketFactory readSockFact(Network::EntityType::Client, std::string("read"), BUFFER_SIZE);

    auto writeSockfd = writeSockFact.createSocket(50);
    auto readSockfd = readSockFact.createSocket(50);
    if(writeSockfd < 0 || readSockfd < 0) {
        logger_->error_log("Socket creation failed");
        return EXIT_FAILURE;
    }

    USED_TYPE val = 0, readVal = 0;
    std::vector<USED_TYPE> writeBuffer(BUFFER_SIZE), readBuffer(BUFFER_SIZE);
    Timer timer;
    Timer::Time time(0);
    USED_TYPE max = std::numeric_limits<decltype(val)>::max() - writeBuffer.size();
    int readBufferSize = 0;

    while(run && val < max) {
        for(uint32_t i = 0; i < writeBuffer.size(); ++i) {
            writeBuffer[i] = val;
            //logger_->debug_log("writebuffer: "  + std::to_string(i) + " " + std::to_string(val));
            ++val;
        }

        timer.start();
        if(writeSockFact.writeValues(writeSockfd, writeBuffer) < 0) {
            run = false;
            logger_->error_log("Write failed, exiting..");
            continue;
        }

        readBufferSize = readSockFact.readValues(readSockfd, readBuffer);
        if(readBufferSize < 0) {
            run = false;
            logger_->error_log("Read failed, exiting..");
            continue;
        }
      
        for(int i = 0; i < readBufferSize; ++i) {
            if(readVal != readBuffer[i]) {
                logger_->error_log("Values don't match: \n sent: " + std::to_string(readVal)
                                + "\n received: " + std::to_string(readBuffer[i]));
                run = false;
                break;
            }
            ++readVal;
        }

        time += timer.stop();
    }

    readSockFact.closeSocket(readSockfd);
    writeSockFact.closeSocket(writeSockfd);

    logger_->info_log(std::to_string(readVal / time.count()) + " ops/ms");
    logger_->info_log("Client Stopped exiting...");
    return EXIT_SUCCESS;
}