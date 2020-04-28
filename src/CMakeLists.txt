# cmake_minimum_required(VERSION <specify CMake version here>)
project(tuitube)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_SHARED_LINKER_FLAGS "-L/usr/lib -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now -lcurl -lcurlpp")

include(FindPkgConfig)
pkg_check_modules(CURLPP REQUIRED curlpp)

add_executable(tuitube main.cpp)
target_link_libraries(tuitube ${CURLPP_LDFLAGS})