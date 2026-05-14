add_library(client_node_a SHARED
    src/NodeA_JNI.cpp
    src/SensorReader.cpp
    src/ClientA_Core.cpp
    src/ResultLogger.cpp
)

add_dependencies(client_node_a accel_proto)

target_include_directories(client_node_a PRIVATE 
    ${CMAKE_BINARY_DIR}/proto/generated
)

find_library(log-lib log)
find_library(android-lib android)

target_link_libraries(client_node_a PRIVATE
    common_lib
    accel_proto
    gRPC::grpc++
    ${log-lib}
    ${android-lib}
    Threads::Threads
)

target_compile_options(client_node_a PRIVATE -Wall -Wextra)