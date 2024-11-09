#include "Logger.h"
#include "Timestamp.h"

#include <iostream>
    // 唯一实例：单例模式
Logger& Logger::instance(){

    static Logger logger;
    return logger;
}
// 设置日志级别
int Logger::setLogLevel(int level) {

    logLevel_ = level;
    return logLevel_;
}

void Logger::log(std::string msg) {

    switch (logLevel_) {
        case INFO:
            std::cout << "[INFO]";
            break;
        case ERROR:
            std::cout << "[ERROR]";
            break;
        case DEBUG:
            std::cout << "[DEBUG]";
            break;
        case FATAL:
            std::cout << "[FATAL]";
            break;
        default:
            std::cout << "[INFO]";
            break;
    }

    std::cout << Timestamp::now().toString() << " : " << msg <<std::endl;
}