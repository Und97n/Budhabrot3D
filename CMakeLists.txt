cmake_minimum_required(VERSION 3.13)
project(Budhabrot_)

set(CMAKE_CXX_STANDARD 17)

add_executable(Budhabrot_ main.cpp Budhabrot.cpp Budhabrot.h defs.h bmath.cpp)
target_link_libraries(Budhabrot_ SDL)
target_link_libraries(Budhabrot_ pthread)