#include "DuplicateFilter.h"

namespace kvadra {
namespace server {

bool DuplicateFilter::processAndCheck(float x, float y, float z) {
    std::lock_guard<std::mutex> lock(filter_mutex_);

    if (!has_first_packet_) {
        last_x_ = x; last_y_ = y; last_z_ = z;
        has_first_packet_ = true;
        return true;
    }

    if (common::MathUtils::isDuplicate(x, y, z, last_x_, last_y_, last_z_)) {
        return false; 
    }

    last_x_ = x; last_y_ = y; last_z_ = z;
    return true;
}

void DuplicateFilter::reset() {
    std::lock_guard<std::mutex> lock(filter_mutex_);
    has_first_packet_ = false;
}

} // namespace server
} // namespace kvadra