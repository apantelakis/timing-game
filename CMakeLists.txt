cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(OTAW)

find_package(SDL3 REQUIRED)

add_executable(OTAW src/main.cpp "src/app.cpp")

target_include_directories(OTAW PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(OTAW PROPERTIES CXX_STANDARD 20)

target_link_libraries(OTAW PRIVATE SDL3::SDL3)
