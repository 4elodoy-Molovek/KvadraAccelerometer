#pragma once
#include <android/sensor.h>
#include <android/looper.h>
#include <functional>
#include <thread>
#include <atomic>

namespace kvadra {
namespace client_a {

class SensorReader {
public:
    using SensorCallback = std::function<void(int64_t timestamp, float x, float y, float z)>;

    SensorReader();
    ~SensorReader();

    bool start(SensorCallback callback);
    void stop();

private:
    void loop();

    ASensorManager* sensor_manager_ = nullptr;
    const ASensor* accelerometer_ = nullptr;
    ASensorEventQueue* event_queue_ = nullptr;
    
    SensorCallback callback_;
    std::atomic<bool> active_{false};
    std::thread reader_thread_;
};

} // namespace client_a
} // namespace kvadra