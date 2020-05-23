project(tuitube)

include_directories(/usr/local/include /usr/include)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Iinclude")
set(CMAKE_SHARED_LINKER_FLAGS "-L/usr/lib -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now -Llib64 -lcurl -lnghttp2 -lssl -lcrypto -lssl -lcrypto -lz -lcurlpp")

include(FindPkgConfig)
pkg_check_modules(CURLPP REQUIRED curlpp)

add_executable(tuitube src/main.cpp )
target_link_libraries(tuitube ${CURLPP_LDFLAGS})

install(TARGETS ${PROJECT_NAME})
