cmake_minimum_required(VERSION 3.10)

project(poll)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/build/cmake")
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" "/home/sierra/thrift-0.16.0/cmake-build/myinstall/lib/cmake/thrift")

find_package(Thrift REQUIRED)
find_package(Threads REQUIRED)
#target_include_directories("/home/sierra/benchmarks/gen-cpp")
set(pollgencpp_SOURCES
    /home/sierra/benchmarks/gen-cpp/poll.cpp
    /home/sierra/benchmarks/gen-cpp/nullaryfn.cpp
    /home/sierra/benchmarks/gen-cpp/agent.cpp
    /home/sierra/benchmarks/gen-cpp/agent_types.cpp
)
add_library(pollgencpp STATIC ${pollgencpp_SOURCES})

# COMPILE AND LINK POLL
add_executable(poll poll.cpp)
target_link_libraries(poll PUBLIC thrift::thrift)
target_link_libraries(poll PRIVATE Threads::Threads)

target_link_libraries(pollgencpp PUBLIC thrift::thrift)
target_link_libraries(poll PUBLIC pollgencpp)

# COMPILE AND LINK POLL BENCH AGENT
add_executable(poll_bench_agent poll_bench_agent.cpp)
target_link_libraries(poll_bench_agent PUBLIC thrift::thrift)
target_link_libraries(poll_bench_agent PRIVATE Threads::Threads)
target_link_libraries(poll_bench_agent PUBLIC pollgencpp)

# COMPILE AND LINK POLL PARAM BENCH AGENT
add_executable(poll_param_bench_agent poll_param_bench_agent.cpp)
target_link_libraries(poll_param_bench_agent PUBLIC thrift::thrift)
target_link_libraries(poll_param_bench_agent PRIVATE Threads::Threads)
target_link_libraries(poll_param_bench_agent PUBLIC pollgencpp)

# COMPILE AND LINK POLL BENCH AGENT
add_executable(test_poll_bench_agent test_poll_bench_agent.cpp)
target_link_libraries(test_poll_bench_agent PUBLIC thrift::thrift)
target_link_libraries(test_poll_bench_agent PRIVATE Threads::Threads)
target_link_libraries(test_poll_bench_agent PUBLIC pollgencpp)

#add_custom_command(OUTPUT /home/sierra/benchmarks/gen-cpp/poll.cpp
#    /home/sierra/benchmarks/gen-cpp/nullaryfn.cpp
#    /home/sierra/benchmarks/gen-cpp/agent.cpp
#    /home/sierra/benchmarks/gen-cpp/agent_types.cpp
#    COMMAND /home/sierra/thrift-0.16.0/cmake-build/compiler/cpp/bin/thrift --gen cpp -r /home/sierra/benchmarks/agent.thrift
#)