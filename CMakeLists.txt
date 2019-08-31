cmake_minimum_required(VERSION 2.8)
project(BuddhabrotRender)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

File(GLOB SOURCES "src/*.cpp")
add_executable(Buddhabrot ${SOURCES})

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
target_link_libraries(Buddhabrot sfml-graphics sfml-window sfml-system)
