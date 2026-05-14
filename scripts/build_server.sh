#!/bin/bash

set -e

echo "[System] Подготовка директории сборки сервера..."
mkdir -p build/server
cd build/server

echo "[System] Генерация Makefile (CMake)..."
cmake ../..

echo "[System] Сборка исполняемого файла..."
make -j$(nproc)

echo "================================================="
echo "Сборка сервера успешно завершена!"
echo "Исполняемый файл находится в: build/server/server_node"
echo "Пример запуска:"
echo "./build/server/server_node 50051 ./certs/ca.crt ./certs/server.key ./certs/server.crt"
echo "================================================="