#pragma once

#include <string>

#include "noncopyable.h"

#define LOG_INFO(logmsgFormat, ...) \
    do { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(INFO); \
        char buf[1024] = {0}; \
        snprintf(buf, sizeof(buf), logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0);

#define LOG_ERROR(logmsgFormat, ...) \
    do { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(ERROR); \
        char buf[1024] = {0}; \
        snprintf(buf, sizeof(buf), logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0);

#define LOG_FATAL(logmsgFormat, ...) \
    do { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(FATAL); \
        char buf[1024] = {0}; \
        snprintf(buf, sizeof(buf), logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
        exit(-1); \
    } while(0);

#ifdef MUDEBUG
    #define LOG_DEBUG(logmsgFormat, ...) \
        do { \
            Logger &logger = Logger::instance(); \
            logger.setLogLevel(LogLevel::INFO); \
            char buf[1024] = {0}; \
            snprintf(buf, sizeof(buf), logmsgFormat, ##__VA_ARGS__); \
            logger.log(buf); \
        } while(0);
#else
    #define LOG_DEBUG(logmsgFormat, ...)
#endif

enum LogLevel {
    INFO,
    ERROR,
    FATAL,
    DEBUG,
};

// Logger class
class Logger : noncopyable {

public:
    // 唯一实例：单例模式
    static Logger& instance();
    // 设置日志级别
    int setLogLevel(int level);
    
    void log(std::string msg);
private:
    int logLevel_;
    Logger(){}
};