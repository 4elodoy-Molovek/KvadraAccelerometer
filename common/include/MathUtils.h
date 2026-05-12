#pragma once

namespace kvadra {
namespace common {

class MathUtils {
public:
    static float calculateModule(float x, float y, float z);

    static bool isDuplicate(float x1, float y1, float z1,
                            float x2, float y2, float z2,
                            float epsilon = 0.0001f);
};

} // namespace common
} // namespace kvadra