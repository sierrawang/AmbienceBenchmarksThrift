cmake_minimum_required(VERSION 3.10)

project(load_balancer)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/build/cmake")
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" "/home/sierra/thrift-0.16.0/cmake-build/myinstall/lib/cmake/thrift")

find_package(Thrift REQUIRED)
find_package(Threads REQUIRED)

set(compute_requestgencpp_SOURCES
    /home/sierra/benchmarks/gen-cpp/compute_request_types.cpp
    /home/sierra/benchmarks/gen-cpp/compute_request_ui.cpp
    /home/sierra/benchmarks/gen-cpp/client.cpp
    /home/sierra/benchmarks/gen-cpp/provider.cpp
    /home/sierra/benchmarks/gen-cpp/resource_db.cpp
    /home/sierra/benchmarks/gen-cpp/user_verification.cpp
    /home/sierra/benchmarks/gen-cpp/time_verification.cpp
    /home/sierra/benchmarks/gen-cpp/price_calculator.cpp
)
add_library(compute_requestgencpp STATIC ${compute_requestgencpp_SOURCES})
target_link_libraries(compute_requestgencpp PUBLIC thrift::thrift)

add_executable(compute_request_ui compute_request_ui.cpp)
target_link_libraries(compute_request_ui PUBLIC thrift::thrift)
target_link_libraries(compute_request_ui PRIVATE Threads::Threads)
target_link_libraries(compute_request_ui PRIVATE compute_requestgencpp)

add_executable(client client.cpp)
target_link_libraries(client PUBLIC thrift::thrift)
target_link_libraries(client PRIVATE Threads::Threads)
target_link_libraries(client PRIVATE compute_requestgencpp)

add_executable(price_calculator price_calculator.cpp)
target_link_libraries(price_calculator PUBLIC thrift::thrift)
target_link_libraries(price_calculator PRIVATE Threads::Threads)
target_link_libraries(price_calculator PRIVATE compute_requestgencpp)

add_executable(user_verification user_verification.cpp)
target_link_libraries(user_verification PUBLIC thrift::thrift)
target_link_libraries(user_verification PRIVATE Threads::Threads)
target_link_libraries(user_verification PRIVATE compute_requestgencpp)

add_executable(time_verification time_verification.cpp)
target_link_libraries(time_verification PUBLIC thrift::thrift)
target_link_libraries(time_verification PRIVATE Threads::Threads)
target_link_libraries(time_verification PRIVATE compute_requestgencpp)

add_executable(provider provider.cpp)
target_link_libraries(provider PUBLIC thrift::thrift)
target_link_libraries(provider PRIVATE Threads::Threads)
target_link_libraries(provider PRIVATE compute_requestgencpp)

add_executable(resource_db resource_db.cpp)
target_link_libraries(resource_db PUBLIC thrift::thrift)
target_link_libraries(resource_db PRIVATE Threads::Threads)
target_link_libraries(resource_db PRIVATE compute_requestgencpp)

add_executable(test_compute_request test_compute_request.cpp)
target_link_libraries(test_compute_request PUBLIC thrift::thrift)
target_link_libraries(test_compute_request PRIVATE Threads::Threads)
target_link_libraries(test_compute_request PRIVATE compute_requestgencpp)