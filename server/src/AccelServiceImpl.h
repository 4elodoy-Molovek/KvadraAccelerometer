#pragma once
#include "accelerometer.grpc.pb.h"
#include "DuplicateFilter.h"
#include "ThreadSafeQueue.h"
#include <grpcpp/grpcpp.h>
#include <atomic>

namespace kvadra {
namespace server {

class AccelServiceImpl final : public accelerometer::AccelerometerService::Service {
public:
    explicit AccelServiceImpl(std::string expected_api_key);
    ~AccelServiceImpl();

    grpc::Status StreamAccelData(grpc::ServerContext* context,
                                 grpc::ServerReaderWriter<accelerometer::AccelModule, accelerometer::AccelPacket>* stream) override;

    grpc::Status ProcessorStream(grpc::ServerContext* context,
                                 grpc::ServerReaderWriter<accelerometer::AccelPacket, accelerometer::AccelModule>* stream) override;

    void shutdown();

private:
    std::string api_key_;
    DuplicateFilter filter_;
    
    ThreadSafeQueue<accelerometer::AccelPacket> queue_to_node_b_;
    ThreadSafeQueue<accelerometer::AccelModule> queue_to_node_a_;

    std::atomic<bool> node_a_connected_{false};
    std::atomic<bool> node_b_connected_{false};
};

} // namespace server
} // namespace kvadra