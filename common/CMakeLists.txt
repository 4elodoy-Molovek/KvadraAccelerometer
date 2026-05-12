add_library(common_lib STATIC
    src/MathUtils.cpp
    src/AuthInterceptor.cpp
)

target_include_directories(common_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(common_lib PUBLIC gRPC::grpc++)