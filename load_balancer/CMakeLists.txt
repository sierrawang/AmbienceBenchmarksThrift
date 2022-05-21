cmake_minimum_required(VERSION 3.10)

project(load_balancer)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/build/cmake")
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" "/home/sierra/thrift-0.16.0/cmake-build/myinstall/lib/cmake/thrift")

find_package(Thrift REQUIRED)
find_package(Threads REQUIRED)

set(load_balancergencpp_SOURCES
    /home/sierra/benchmarks/gen-cpp/load_balancer_types.cpp
    /home/sierra/benchmarks/gen-cpp/endpoint.cpp
    /home/sierra/benchmarks/gen-cpp/tweet_db.cpp
    /home/sierra/benchmarks/gen-cpp/user_db.cpp
    /home/sierra/benchmarks/gen-cpp/worker.cpp
)
add_library(load_balancergencpp STATIC ${load_balancergencpp_SOURCES})
target_link_libraries(load_balancergencpp PUBLIC thrift::thrift)

add_executable(load_balancer load_balancer.cpp)
target_link_libraries(load_balancer PUBLIC thrift::thrift)
target_link_libraries(load_balancer PRIVATE Threads::Threads)
target_link_libraries(load_balancer PRIVATE load_balancergencpp)

add_executable(user_db user_db.cpp)
target_link_libraries(user_db PUBLIC thrift::thrift)
target_link_libraries(user_db PRIVATE Threads::Threads)
target_link_libraries(user_db PRIVATE load_balancergencpp)

add_executable(tweet_db tweet_db.cpp)
target_link_libraries(tweet_db PUBLIC thrift::thrift)
target_link_libraries(tweet_db PRIVATE Threads::Threads)
target_link_libraries(tweet_db PRIVATE load_balancergencpp)