#include "SensorReader.h"
#include <android/log.h>
#include <chrono>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "KvadraNodeA", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "KvadraNodeA", __VA_ARGS__)

namespace kvadra {
namespace client_a {

SensorReader::SensorReader() {
    sensor_manager_ = ASensorManager_getInstanceForPackage("com.kvadra.accelerometer");
    if (sensor_manager_) {
        accelerometer_ = ASensorManager_getDefaultSensor(sensor_manager_, ASENSOR_TYPE_ACCELEROMETER);
    }
}

SensorReader::~SensorReader() {
    stop();
}

bool SensorReader::start(SensorCallback callback) {
    if (!accelerometer_) {
        LOGE("Акселерометр не найден на устройстве!");
        return false;
    }
    
    callback_ = callback;
    active_ = true;
    reader_thread_ = std::thread(&SensorReader::loop, this);
    return true;
}

void SensorReader::stop() {
    if (active_) {
        active_ = false;
        if (reader_thread_.joinable()) {
            reader_thread_.join();
        }
    }
}

void SensorReader::loop() {
    ALooper* looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    event_queue_ = ASensorManager_createEventQueue(sensor_manager_, looper, ALOOPER_POLL_CALLBACK, nullptr, nullptr);

    ASensorEventQueue_enableSensor(event_queue_, accelerometer_);
    ASensorEventQueue_setEventRate(event_queue_, accelerometer_, 20000);

    LOGI("Опрос акселерометра (~50Hz)");

    while (active_) {
        int ident;
        int events;
        void* data;
        
        while ((ident = ALooper_pollOnce(100, nullptr, &events, &data)) >= 0) {
            ASensorEvent event;
            while (ASensorEventQueue_getEvents(event_queue_, &event, 1) > 0) {
                if (event.type == ASENSOR_TYPE_ACCELEROMETER && active_) {
                    auto now = std::chrono::system_clock::now();
                    int64_t unix_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
                    
                    callback_(unix_ms, event.acceleration.x, event.acceleration.y, event.acceleration.z);
                }
            }
        }
    }

    ASensorEventQueue_disableSensor(event_queue_, accelerometer_);
    ASensorManager_destroyEventQueue(sensor_manager_, event_queue_);
    LOGI("Опрос акселерометра остановлен");
}

} // namespace client_a
} // namespace kvadra