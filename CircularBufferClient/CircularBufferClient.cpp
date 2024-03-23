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

void sigHandler(int sig) {
    logger_->info_log("SIGINT received, stopping threads...");
    run = false;
};

int main() {
    std::signal(SIGINT, sigHandler);

    Network::SocketFactory writeSockFact(Network::EntityType::Client, std::string("write"));
    Network::SocketFactory readSockFact(Network::EntityType::Client, std::string("read"));

    auto writeSockfd = writeSockFact.createSocket();
    auto readSockfd = readSockFact.createSocket();
    if(writeSockfd < 0 || readSockfd < 0) {
        logger_->error_log("Socket creation failed");
        return EXIT_FAILURE;
    }

    USED_TYPE sent_val = 0, read_val = 0;
    int flags = 0;
    Timer timer;
    Timer::Time time;

    while(run && sent_val < std::numeric_limits<decltype(sent_val)>::max()) {
        timer.start();
        if(writeSockFact.sendVal(writeSockfd, sent_val) < 0) {
            run = false;
            logger_->error_log("Write failed, exiting..");
            continue;
        }

        if (readSockFact.readVal(readSockfd, read_val, flags) < 0) {
            run = false;
            logger_->error_log("Read failed, exiting..");
            continue;
        }
      
        if(sent_val != read_val) {
            logger_->error_log("Values don't match: \n sent: " + std::to_string(sent_val)
                             + "\n received: " + std::to_string(read_val));
            run = false;
        }

        ++sent_val;
        time += timer.stop();
    }

    readSockFact.closeSocket(readSockfd);
    writeSockFact.closeSocket(writeSockfd);

    logger_->info_log(std::to_string(sent_val / time.count()) + "ops/ms");
    logger_->info_log("Client Stoppped exiting...");
    return EXIT_SUCCESS;
}