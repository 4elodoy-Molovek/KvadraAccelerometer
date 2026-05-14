#pragma once
#include <string>
#include <fstream>
#include <mutex>

namespace kvadra {
namespace client_a {

class ResultLogger {
public:
    explicit ResultLogger(const std::string& filepath);
    ~ResultLogger();

    void log(int64_t timestamp, float module);

private:
    std::ofstream file_;
    std::mutex mtx_;
};

} // namespace client_a
} // namespace kvadra