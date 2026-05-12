#pragma once
#include <grpcpp/grpcpp.h>
#include <string>

namespace kvadra {
namespace common {

class AuthInterceptor {
public:
    static constexpr const char* API_KEY_HEADER = "x-api-key";

    static void injectApiKey(grpc::ClientContext& context, const std::string& apiKey);

    static grpc::Status validateApiKey(const grpc::ServerContext& context, const std::string& expectedApiKey);
};

} // namespace common
} // namespace kvadra