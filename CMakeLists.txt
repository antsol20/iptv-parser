cmake_minimum_required(VERSION 3.16)
project(iptv-parser)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(iptv-parser ${SRC_FILES})

target_include_directories(iptv-parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/jdbc)
target_link_libraries(iptv-parser libmysqlcppconn-static.a ssl crypto pthread dl resolv)
