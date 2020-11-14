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
target_link_libraries(freertos pthread)

set(CATCH_BUILD_TESTING OFF CACHE BOOL "Internal Catch2's option to disable Catch2 self-test")
set(BUILD_TESTING OFF CACHE BOOL "Internal Catch2's option to disable Catch2 self-test")

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG 0fa133a0c5e065065ef96ac2b6c0284cf5da265d
)
FetchContent_MakeAvailable(Catch2)

add_executable(TestBuild
    test/freertos/main.cpp
    test/freertos/os_delayed_job_test.cpp
    test/freertos/test_thread_impl.cpp
    test/freertos/test_active_impl.cpp
    test/freertos/test_queue_impl.cpp
    test/generic/test_active.cpp
    test/generic/test_thread.cpp
    test/generic/test_queue.cpp
)
include_directories(${CMAKE_SOURCE_DIR})
target_link_libraries(TestBuild freertos Catch2::Catch2)

add_executable(NativeTests
    ${CMAKE_SOURCE_DIR}/test/native/main.cpp
    ${CMAKE_SOURCE_DIR}/test/native/test_active_impl.cpp
    ${CMAKE_SOURCE_DIR}/test/generic/test_active.cpp
)
find_package(Threads REQUIRED)
target_link_libraries(NativeTests Catch2::Catch2 Threads::Threads)
