cmake_minimum_required(VERSION 3.10)

include(ExternalProject)

project(JunglesOsHelpers)

include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra)

FetchContent_Declare(
    cmake_helpers
    GIT_REPOSITORY https://github.com/KKoovalsky/CMakeHelpers.git
    GIT_TAG main
)
FetchContent_MakeAvailable(cmake_helpers)

JunglesHelpers_DownloadAndPopulateFreeRTOSKernel(
    freertos
    18f714f786ddbc137c632d6845c5fa2b175d3cbd
    heap_3
)
set(freertos_port_dir ${FREERTOS_SOURCE_DIR}/portable/ThirdParty/GCC/Posix)
target_include_directories(freertos PUBLIC
    ${CMAKE_SOURCE_DIR}/test/freertos
    ${freertos_port_dir}
)
target_sources(freertos PRIVATE ${freertos_port_dir}/port.c ${freertos_port_dir}/utils/wait_for_event.c)

find_package(Threads REQUIRED)
target_link_libraries(freertos Threads::Threads)

set(CATCH_BUILD_TESTING OFF CACHE BOOL "Internal Catch2's option to disable Catch2 self-test")
set(BUILD_TESTING OFF CACHE BOOL "Internal Catch2's option to disable Catch2 self-test")

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG 0fa133a0c5e065065ef96ac2b6c0284cf5da265d
)
FetchContent_MakeAvailable(Catch2)

set(generic_tests_dir ${CMAKE_CURRENT_LIST_DIR}/test/generic)
set(freertos_tests_dir ${CMAKE_CURRENT_LIST_DIR}/test/freertos)
set(native_tests_dir ${CMAKE_CURRENT_LIST_DIR}/test/native)

add_executable(freertos_helpers_tests
    ${freertos_tests_dir}/main.cpp
    ${freertos_tests_dir}/os_delayed_job_test.cpp
    ${freertos_tests_dir}/test_thread_impl.cpp
    ${freertos_tests_dir}/test_active_impl.cpp
    ${freertos_tests_dir}/test_queue_impl.cpp
    ${generic_tests_dir}/test_active.cpp
    ${generic_tests_dir}/test_thread.cpp
    ${generic_tests_dir}/test_queue.cpp
)
include_directories(${CMAKE_SOURCE_DIR})
target_link_libraries(freertos_helpers_tests freertos Catch2::Catch2)

add_executable(native_helpers_tests
    ${native_tests_dir}/main.cpp
    ${native_tests_dir}/test_active_impl.cpp
    ${generic_tests_dir}/test_active.cpp
)
target_link_libraries(native_helpers_tests Catch2::Catch2 Threads::Threads)
