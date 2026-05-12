#include "AuthInterceptor.h"

namespace kvadra {
namespace common {

void AuthInterceptor::injectApiKey(grpc::ClientContext& context, const std::string& apiKey) {
    context.AddMetadata(API_KEY_HEADER, apiKey);
}

grpc::Status AuthInterceptor::validateApiKey(const grpc::ServerContext& context, const std::string& expectedApiKey) {
    const auto& metadata = context.client_metadata();
    auto it = metadata.find(API_KEY_HEADER);

    if (it != metadata.end()) {
        std::string clientKey(it->second.data(), it->second.length());
        
        if (clientKey == expectedApiKey) {
            return grpc::Status::OK;
        }
        return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid API Key provided.");
    }

    return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Missing API Key in metadata.");
}

} // namespace common
} // namespace kvadra