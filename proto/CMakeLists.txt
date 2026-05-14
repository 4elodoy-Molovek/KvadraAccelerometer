set(PROTO_SRC "${CMAKE_CURRENT_SOURCE_DIR}/accelerometer.proto")

set(PROTO_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

add_custom_command(
    OUTPUT "${PROTO_GEN_DIR}/accelerometer.pb.cc"
           "${PROTO_GEN_DIR}/accelerometer.pb.h"
           "${PROTO_GEN_DIR}/accelerometer.grpc.pb.cc"
           "${PROTO_GEN_DIR}/accelerometer.grpc.pb.h"
    COMMAND protobuf::protoc
    ARGS --grpc_out="${PROTO_GEN_DIR}"
         --cpp_out="${PROTO_GEN_DIR}"
         --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
         -I "${CMAKE_CURRENT_SOURCE_DIR}"
         "${PROTO_SRC}"
    DEPENDS "${PROTO_SRC}"
)

add_library(accel_proto
    "${PROTO_GEN_DIR}/accelerometer.pb.cc"
    "${PROTO_GEN_DIR}/accelerometer.grpc.pb.cc"
)

target_include_directories(accel_proto PUBLIC "${PROTO_GEN_DIR}")
target_link_libraries(accel_proto PUBLIC 
    gRPC::grpc++ 
    protobuf::libprotobuf 
    absl::cord 
    absl::cordz_functions 
    absl::cordz_info 
    absl::strings 
    absl::synchronization
)