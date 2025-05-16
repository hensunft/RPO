@echo off
::set ABI=armeabi-v7a
::set ABI=x86
::set ABI=arm64-v8a
set ABI=x86_64

set ANDROID_NDK=C:\Users\User\AppData\Local\Android\Sdk\ndk\27.0.12077973
set TOOL_CHAIN=%ANDROID_NDK%\build\cmake\android.toolchain.cmake
set CMAKE=C:\Users\User\AppData\Local\Android\Sdk\cmake\3.22.1\bin\cmake.exe

:: Добавляем Ninja в PATH
set PATH=%PATH%;C:\Users\User\AppData\Local\Android\Sdk\cmake\3.22.1\bin

:: Создаем и переходим в директорию сборки
if not exist "%ABI%" mkdir "%ABI%"
cd "%ABI%"

%CMAKE% ..\..\spdlog -GNinja -DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=21 -DANDROID_ABI=%ABI% -DCMAKE_TOOLCHAIN_FILE=%TOOL_CHAIN% -DCMAKE_CXX_FLAGS=-DSPDLOG_COMPILED_LIB

%CMAKE% --build .