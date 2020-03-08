cmake_minimum_required (VERSION 2.6)
project (examples)

include_directories(../include/)
include_directories(../libvfuzz-core/src)

add_executable(nlohmann nlohmann.cpp)
set_target_properties(nlohmann PROPERTIES COMPILE_FLAGS -fsanitize-coverage=trace-pc-guard)
set_target_properties(nlohmann PROPERTIES LINK_FLAGS -fsanitize-coverage=trace-pc-guard)
target_link_libraries(nlohmann vfuzz-runtime vfuzz-core)

add_executable(libvfuzz-core-example libvfuzz-core-example.cpp)
target_link_libraries(libvfuzz-core-example vfuzz-core)
