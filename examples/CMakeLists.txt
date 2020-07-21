cmake_minimum_required(VERSION 3.8)

project(Beasty-Tepee)
add_executable(tepee-server main.cpp)

target_link_libraries(tepee-server beasty-tepee)