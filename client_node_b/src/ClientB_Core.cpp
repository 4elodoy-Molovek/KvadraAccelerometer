#include "ClientB_Core.h"
#include "AuthInterceptor.h"
#include "MathUtils.h"
#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "KvadraNodeB", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "KvadraNodeB", __VA_ARGS__)

namespace kvadra {
namespace client_b {

ClientB_Core::ClientB_Core(const std::string& target_ip, 
                           const std::string& api_key,
                           const std::string& ca_cert,
                           const std::string& client_cert,
                           const std::string& client_key)
    : api_key_(api_key) {
    
    grpc::SslCredentialsOptions ssl_opts;
    ssl_opts.pem_root_certs = ca_cert;
    ssl_opts.pem_cert_chain = client_cert;
    ssl_opts.pem_private_key = client_key;

    auto creds = grpc::SslCredentials(ssl_opts);
    auto channel = grpc::CreateChannel(target_ip, creds);
    stub_ = ::kvadra::accelerometer::AccelerometerService::NewStub(channel);
}

ClientB_Core::~ClientB_Core() {
    stop();
}

void ClientB_Core::start() {
    if (active_) return;
    active_ = true;

    context_ = std::make_unique<grpc::ClientContext>();
    common::AuthInterceptor::injectApiKey(*context_, api_key_);

    stream_ = stub_->ProcessorStream(context_.get());
    
    processor_thread_ = std::thread(&ClientB_Core::processLoop, this);
    LOGI("Узел B запущен, ожидаем данные для вычислений...");
}

void ClientB_Core::stop() {
    if (!active_) return;
    active_ = false;

    if (context_) {
        context_->TryCancel();
    }
    
    if (processor_thread_.joinable()) {
        processor_thread_.join();
    }
    LOGI("Узел B полностью остановлен");
}

void ClientB_Core::processLoop() {
    ::kvadra::accelerometer::AccelPacket packet;
    ::kvadra::accelerometer::AccelModule module;

    while (stream_->Read(&packet)) {
        float mod_val = common::MathUtils::calculateModule(packet.x(), packet.y(), packet.z());
        
        module.set_timestamp(packet.timestamp());
        module.set_module(mod_val);

        if (!stream_->Write(module)) {
            LOGE("Ошибка записи в стрим");
            break;
        }
    }
    
    grpc::Status status = stream_->Finish();
    if (!status.ok()) {
        LOGE("Обрыв связи: %s", status.error_message().c_str());
    } else {
        LOGI("Поток вычислений успешно закрыт");
    }
}

} // namespace client_b
} // namespace kvadra