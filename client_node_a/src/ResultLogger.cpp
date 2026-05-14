#include "ResultLogger.h"
#include <iostream>

namespace kvadra {
namespace client_a {

ResultLogger::ResultLogger(const std::string& filepath) {
    file_.open(filepath, std::ios::app);
}

ResultLogger::~ResultLogger() {
    if (file_.is_open()) {
        file_.close();
    }
}

void ResultLogger::log(int64_t timestamp, float module) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (file_.is_open()) {
        file_ << timestamp << " : " << module << "\n";
        file_.flush();
    }
}

} // namespace client_a
} // namespace kvadra