cmake_minimum_required(VERSION 2.6)
project(alive_intercept CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
#set(CMAKE_CXX_STANDARD 11)

add_subdirectory(src)
add_subdirectory(ext/intercept)
