#pragma once

#include <memory>
#include <filesystem>
#include <mutex>
#include <fstream>
#include <string>

class Logger {
private:
    std::mutex logger_mutex_;
    std::ofstream log_;
    Logger(std::string& path): log_(path, std::ios::out)  {};

public:
    ~Logger() {};
    Logger(Logger& logger) = delete;
    Logger(Logger&& logger) = delete;
    Logger operator=(Logger& logger) = delete;
    Logger& operator=(Logger&& logger) = delete;

    static std::shared_ptr<Logger>& getLogger(std::string filepath = "logger.log") {
        static std::shared_ptr<Logger> instance_(new Logger(filepath));
        return instance_;
    };

    template <typename t>
    void debug_log(const t& log) {
        std::lock_guard<std::mutex> lock(logger_mutex_);
        log_ << "DEBUG: " << log << std::endl;
    };

    template <typename t>
    void error_log(const t& log) {
        std::lock_guard<std::mutex> lock(logger_mutex_);
        log_ << "ERROR: " << log << std::endl;
    };

    template <typename t>
    void fatal_log(const t& log) {
        std::lock_guard<std::mutex> lock(logger_mutex_);
        log_ << "FATAL: " << log << std::endl;
    };

    template <typename t>
    void warn_log(const t& log) {
        std::lock_guard<std::mutex> lock(logger_mutex_);
        log_ << "WARN: " << log << std::endl;
    };

    template <typename t>
    void info_log(const t& log) {
        std::lock_guard<std::mutex> lock(logger_mutex_);
        log_ << "INFO: " << log << std::endl;
    };
};