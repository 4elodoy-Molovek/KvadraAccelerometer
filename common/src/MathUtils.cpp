#include "MathUtils.h"
#include <cmath>

namespace kvadra {
namespace common {

float MathUtils::calculateModule(float x, float y, float z) {
    return std::sqrt(x * x + y * y + z * z);
}

bool MathUtils::isDuplicate(float x1, float y1, float z1,
                            float x2, float y2, float z2,
                            float epsilon) {
    return std::abs(x1 - x2) < epsilon &&
           std::abs(y1 - y2) < epsilon &&
           std::abs(z1 - z2) < epsilon;
}

} // namespace common
} // namespace kvadra