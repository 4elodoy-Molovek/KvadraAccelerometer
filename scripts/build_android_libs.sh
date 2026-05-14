#!/bin/bash

set -e

echo "[System] Сборка клиентских библиотек C++ (Android NDK)..."

if [ -z "$ANDROID_NDK_HOME" ]; then
    echo "Ошибка: Переменная окружения ANDROID_NDK_HOME не задана."
    echo "Пример: export ANDROID_NDK_HOME=/home/$USER/Android/Sdk/ndk/26.x.x"
    exit 1
fi

mkdir -p build_android
cd build_android

echo "[System] Запуск CMake кросс-компиляции (arm64-v8a)..."
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI="arm64-v8a" \
    -DANDROID_PLATFORM=android-30

echo "[System] Компиляция библиотек..."
make -j$(nproc)

echo "[System] Развертывание .so файлов в Android-проект..."
# Путь к папке jniLibs внутри Android проекта
JNI_LIBS_DIR="../android_app/app/src/main/jniLibs/arm64-v8a"
mkdir -p "$JNI_LIBS_DIR"

find . -name "*.so" -exec cp {} "$JNI_LIBS_DIR/" \;

echo "================================================="
echo "Сборка клиентов успешно завершена!"
echo "Библиотеки скопированы в $JNI_LIBS_DIR"
echo "Теперь вы можете открыть проект в Android Studio и собрать APK."
echo "================================================="