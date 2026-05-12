#include "AccelServiceImpl.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <fstream>
#include <string>
#include <csignal>
#include <condition_variable>

std::mutex server_mutex;
std::condition_variable server_cv;
bool shutdown_requested = false;

void signalHandler(int signum) {
    std::cout << "\n[System] Получен сигнал " << signum << ".\n";
    std::lock_guard<std::mutex> lock(server_mutex);
    shutdown_requested = true;
    server_cv.notify_one();
}

std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл сертификата " << filepath << "\n";
        exit(1);
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

int main(int argc, char** argv) {
    if (argc < 5) {
        std::cerr << "Использование: " << argv[0] << " <port> <ca_cert.pem> <server_key.pem> <server_cert.pem>\n";
        return 1;
    }

    std::string port = argv[1];
    std::string ca_cert_path = argv[2];
    std::string key_path = argv[3];
    std::string cert_path = argv[4];
    std::string server_address = "0.0.0.0:" + port;

    const std::string API_KEY = "super-secret-key-2026";

    grpc::SslServerCredentialsOptions ssl_opts;
    ssl_opts.client_certificate_request = GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY;
    ssl_opts.pem_root_certs = readFile(ca_cert_path);

    grpc::SslServerCredentialsOptions::PemKeyCertPair key_cert_pair = {
        readFile(key_path),
        readFile(cert_path)
    };
    ssl_opts.pem_key_cert_pairs.push_back(key_cert_pair);

    auto creds = grpc::SslServerCredentials(ssl_opts);

    kvadra::server::AccelServiceImpl service(API_KEY);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, creds);
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "=================================================\n";
    std::cout << "Сервер запущен\n";
    std::cout << "mTLS активен. Порт: " << port << "\n";
    std::cout << "=================================================\n";

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::unique_lock<std::mutex> lock(server_mutex);
    server_cv.wait(lock, [] { return shutdown_requested; });

    std::cout << "[System] Останавливаем gRPC очереди...\n";
    service.shutdown();
    
    std::cout << "[System] Выключаем gRPC сервер...\n";
    server->Shutdown();
    
    std::cout << "[System] Сервер остановлен.\n";
    return 0;
}