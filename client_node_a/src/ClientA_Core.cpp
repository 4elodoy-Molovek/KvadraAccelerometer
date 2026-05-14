#include "ClientA_Core.h"
#include "AuthInterceptor.h"
#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "KvadraNodeA", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "KvadraNodeA", __VA_ARGS__)

namespace kvadra {
namespace client_a {

ClientA_Core::ClientA_Core(const std::string& target_ip, 
                           const std::string& api_key,
                           const std::string& ca_cert,
                           const std::string& client_cert,
                           const std::string& client_key,
                           const std::string& log_path)
    : api_key_(api_key), logger_(log_path) {
    
    grpc::SslCredentialsOptions ssl_opts;
    ssl_opts.pem_root_certs = ca_cert;
    ssl_opts.pem_cert_chain = client_cert;
    ssl_opts.pem_private_key = client_key;

    auto creds = grpc::SslCredentials(ssl_opts);
    auto channel = grpc::CreateChannel(target_ip, creds);
    stub_ = accelerometer::AccelerometerService::NewStub(channel);
}

ClientA_Core::~ClientA_Core() {
    stop();
}

void ClientA_Core::start() {
    if (active_) return;
    active_ = true;

    context_ = std::make_unique<grpc::ClientContext>();
    common::AuthInterceptor::injectApiKey(*context_, api_key_);

    stream_ = stub_->StreamAccelData(context_.get());
    
    receive_thread_ = std::thread(&ClientA_Core::receiveLoop, this);

    sensor_reader_.start([this](int64_t ts, float x, float y, float z) {
        this->onSensorData(ts, x, y, z);
    });
}

void ClientA_Core::stop() {
    if (!active_) return;
    active_ = false;

    sensor_reader_.stop();
    if (context_) {
        context_->TryCancel();
    }
    
    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }
    LOGI("Узел А полностью остановлен");
}

void ClientA_Core::onSensorData(int64_t timestamp, float x, float y, float z) {
    accelerometer::AccelPacket packet;
    packet.set_timestamp(timestamp);
    packet.set_x(x);
    packet.set_y(y);
    packet.set_z(z);
    
    if (stream_) {
        stream_->Write(packet);
    }
}

void ClientA_Core::receiveLoop() {
    accelerometer::AccelModule module;
    while (stream_->Read(&module)) {
        logger_.log(module.timestamp(), module.module());
        // LOGI("Записан модуль: %f", module.module());
    }
    
    grpc::Status status = stream_->Finish();
    if (!status.ok()) {
        LOGE("Обрыв связи: %s", status.error_message().c_str());
    } else {
        LOGI("Стрим успешно закрыт");
    }
}

} // namespace client_a
} // namespace kvadra