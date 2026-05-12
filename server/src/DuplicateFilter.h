#pragma once
#include "MathUtils.h"
#include <mutex>

namespace kvadra {
namespace server {

class DuplicateFilter {
public:
    DuplicateFilter() = default;
    
    bool processAndCheck(float x, float y, float z);
    void reset();

private:
    std::mutex filter_mutex_;
    bool has_first_packet_ = false;
    float last_x_ = 0.0f;
    float last_y_ = 0.0f;
    float last_z_ = 0.0f;
};

} // namespace server
} // namespace kvadra