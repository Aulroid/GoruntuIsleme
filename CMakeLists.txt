cmake_minimum_required(VERSION 3.10)
project(GoruntuIsleme)

set(CMAKE_CXX_STANDARD 17)

# Homebrew ile kurulan OpenCV'yi bul
find_package(OpenCV REQUIRED)

add_executable(GoruntuIsleme main.cpp)
target_link_libraries(GoruntuIsleme ${OpenCV_LIBS})
