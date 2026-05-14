#pragma once
#include "accelerometer.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <string>
#include <memory>
#include <thread>
#include <atomic>

namespace kvadra {
namespace client_b {

class ClientB_Core {
public:
    ClientB_Core(const std::string& target_ip, 
                 const std::string& api_key,
                 const std::string& ca_cert,
                 const std::string& client_cert,
                 const std::string& client_key);
    ~ClientB_Core();

    void start();
    void stop();

private:
    std::unique_ptr<::kvadra::accelerometer::AccelerometerService::Stub> stub_;
    std::string api_key_;
    
    std::shared_ptr<grpc::ClientReaderWriter<::kvadra::accelerometer::AccelModule, ::kvadra::accelerometer::AccelPacket>> stream_;
    std::unique_ptr<grpc::ClientContext> context_;
    
    std::atomic<bool> active_{false};
    std::thread processor_thread_;
    
    void processLoop();
};

} // namespace client_b
} // namespace kvadra