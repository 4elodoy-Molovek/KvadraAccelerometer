#!/bin/bash

mkdir -p android_app/app/src/main/jniLibs/arm64-v8a/

cp ./build_android/client_node_a/libclient_node_a.so android_app/app/src/main/jniLibs/arm64-v8a/
cp ./build_android/client_node_b/libclient_node_b.so android_app/app/src/main/jniLibs/arm64-v8a/

echo "Библиотеки скопированы в jniLibs"