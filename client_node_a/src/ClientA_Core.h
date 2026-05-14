#pragma once
#include "accelerometer.grpc.pb.h"
#include "SensorReader.h"
#include "ResultLogger.h"
#include <grpcpp/grpcpp.h>
#include <string>
#include <memory>

namespace kvadra {
namespace client_a {

class ClientA_Core {
public:
    ClientA_Core(const std::string& target_ip, 
                 const std::string& api_key,
                 const std::string& ca_cert,
                 const std::string& client_cert,
                 const std::string& client_key,
                 const std::string& log_path);
    ~ClientA_Core();

    void start();
    void stop();

private:
    std::unique_ptr<accelerometer::AccelerometerService::Stub> stub_;
    std::string api_key_;
    
    SensorReader sensor_reader_;
    ResultLogger logger_;
    
    std::shared_ptr<grpc::ClientReaderWriter<accelerometer::AccelPacket, accelerometer::AccelModule>> stream_;
    std::unique_ptr<grpc::ClientContext> context_;
    
    std::atomic<bool> active_{false};
    std::thread receive_thread_;
    
    void onSensorData(int64_t timestamp, float x, float y, float z);
    void receiveLoop();
};

} // namespace client_a
} // namespace kvadra