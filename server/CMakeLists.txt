add_executable(server_node
    src/main.cpp
    src/AccelServiceImpl.cpp
    src/DuplicateFilter.cpp
)

target_link_libraries(server_node PRIVATE 
    common_lib 
    accel_proto 
    gRPC::grpc++ 
    gRPC::grpc++_reflection
    Threads::Threads
)

target_compile_options(server_node PRIVATE -Wall -Wextra -Wpedantic)