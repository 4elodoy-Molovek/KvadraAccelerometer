add_library(client_node_b SHARED
    src/NodeB_JNI.cpp
    src/ClientB_Core.cpp
)

add_dependencies(client_node_b accel_proto)

target_include_directories(client_node_b PRIVATE 
    ${CMAKE_BINARY_DIR}/proto/generated
)

find_library(log-lib log)
find_library(android-lib android)

target_link_libraries(client_node_b PRIVATE
    common_lib
    accel_proto
    gRPC::grpc++
    ${log-lib}
    ${android-lib}
    Threads::Threads
)

target_compile_options(client_node_b PRIVATE -Wall -Wextra)