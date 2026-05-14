#include "AccelServiceImpl.h"
#include "AuthInterceptor.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace kvadra {
namespace server {

AccelServiceImpl::AccelServiceImpl(std::string expected_api_key) 
    : api_key_(std::move(expected_api_key)) {}

AccelServiceImpl::~AccelServiceImpl() {
    shutdown();
}

void AccelServiceImpl::shutdown() {
    queue_to_node_b_.shutdown();
    queue_to_node_a_.shutdown();
}

grpc::Status AccelServiceImpl::StreamAccelData(grpc::ServerContext* context,
                                               grpc::ServerReaderWriter<accelerometer::AccelModule, accelerometer::AccelPacket>* stream) {
    auto auth_status = common::AuthInterceptor::validateApiKey(*context, api_key_);
    if (!auth_status.ok()) {
        std::cerr << "[Node A] Ошибка аутентификации.\n";
        return auth_status;
    }

    if (node_a_connected_.exchange(true)) {
        return grpc::Status(grpc::StatusCode::ALREADY_EXISTS, "Node A is already connected.");
    }

    std::cout << "[Node A] Подключен.\n";
    filter_.reset();
    queue_to_node_a_.reset();

    std::atomic<bool> stream_active{true};

    std::thread writer_thread([this, context, stream, &stream_active]() {
        while (!context->IsCancelled() && stream_active.load()) {
            auto module_opt = queue_to_node_a_.pop(std::chrono::milliseconds(200));
            if (module_opt) {
                if (!stream->Write(*module_opt)) break;
            }
        }
    });

    accelerometer::AccelPacket packet;
    while (stream->Read(&packet)) {
        if (packet.version() != 1) {
            std::cerr << "[Server] Пакет отброшен: неподдерживаемая версия протокола (" << packet.version() << ")\n";
            continue;
        }

        if (filter_.processAndCheck(packet.x(), packet.y(), packet.z())) {
            if (node_b_connected_.load()) {
                queue_to_node_b_.push(packet);
            } else {
                // std::cerr << "[Server] Пакет отброшен. Node B не подключен.\n";
            }
        }
    }

    stream_active.store(false);
    writer_thread.join();
    
    node_a_connected_ = false;
    std::cout << "[Node A] Отключен.\n";
    
    return context->IsCancelled() ? grpc::Status::CANCELLED : grpc::Status::OK;
}

grpc::Status AccelServiceImpl::ProcessorStream(grpc::ServerContext* context,
                                               grpc::ServerReaderWriter<accelerometer::AccelPacket, accelerometer::AccelModule>* stream) {
    auto auth_status = common::AuthInterceptor::validateApiKey(*context, api_key_);
    if (!auth_status.ok()) {
        std::cerr << "[Node B] Ошибка аутентификации.\n";
        return auth_status;
    }

    if (node_b_connected_.exchange(true)) {
        return grpc::Status(grpc::StatusCode::ALREADY_EXISTS, "Node B is already connected.");
    }

    std::cout << "[Node B] Подключен.\n";
    queue_to_node_b_.reset();

    std::atomic<bool> stream_active{true};

    std::thread writer_thread([this, context, stream, &stream_active]() {
        while (!context->IsCancelled() && stream_active.load()) {
            auto packet_opt = queue_to_node_b_.pop(std::chrono::milliseconds(200));
            if (packet_opt) {
                if (!stream->Write(*packet_opt)) break;
            }
        }
    });

    accelerometer::AccelModule module;
    while (stream->Read(&module)) {
        if (node_a_connected_.load()) {
            queue_to_node_a_.push(module);
        }
    }

    stream_active.store(false);
    writer_thread.join();
    
    node_b_connected_ = false;
    std::cout << "[Node B] Отключен.\n";

    return context->IsCancelled() ? grpc::Status::CANCELLED : grpc::Status::OK;
}

} // namespace server
} // namespace kvadra