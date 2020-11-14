cmake_minimum_required(VERSION 3.10)

include(ExternalProject)

project(JunglesOsHelpers)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra)

set(FREERTOS_KERNEL_ROOT_DIR ${CMAKE_BINARY_DIR}/freertos_kernel_repo)

ExternalProject_Add(freertos_kernel_repo
    GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
    GIT_SHALLOW 1
    GIT_PROGRESS 1
    SOURCE_DIR ${FREERTOS_KERNEL_ROOT_DIR}
    CONFIGURE_COMMAND "" 
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    TEST_COMMAND ""
)

set(FREERTOS_KERNEL_PORT_DIR ${FREERTOS_KERNEL_ROOT_DIR}/portable/ThirdParty/GCC/Posix)
set(FREERTOS_KERNEL_SOURCES
    ${FREERTOS_KERNEL_PORT_DIR}/port.c
    ${FREERTOS_KERNEL_ROOT_DIR}/portable/MemMang/heap_3.c
    ${FREERTOS_KERNEL_ROOT_DIR}/tasks.c
    ${FREERTOS_KERNEL_ROOT_DIR}/list.c
    ${FREERTOS_KERNEL_ROOT_DIR}/queue.c
    ${FREERTOS_KERNEL_ROOT_DIR}/timers.c
)
set_source_files_properties(${FREERTOS_KERNEL_SOURCES} PROPERTIES GENERATED TRUE)
add_library(freertos_kernel STATIC ${FREERTOS_KERNEL_SOURCES})
add_dependencies(freertos_kernel freertos_kernel_repo)
target_include_directories(freertos_kernel PUBLIC
    ${CMAKE_SOURCE_DIR}/test/freertos
    ${FREERTOS_KERNEL_PORT_DIR}
    ${FREERTOS_KERNEL_ROOT_DIR}/include
)
target_link_libraries(freertos_kernel pthread)

set(CATCH_BUILD_TESTING OFF CACHE BOOL "Internal Catch2's option to disable Catch2 self-test")
set(BUILD_TESTING OFF CACHE BOOL "Internal Catch2's option to disable Catch2 self-test")

include(FetchContent)
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
target_link_libraries(TestBuild freertos_kernel Catch2::Catch2)

add_executable(NativeTests
    ${CMAKE_SOURCE_DIR}/test/native/main.cpp
    ${CMAKE_SOURCE_DIR}/test/native/test_active_impl.cpp
    ${CMAKE_SOURCE_DIR}/test/generic/test_active.cpp
)
find_package(Threads REQUIRED)
target_link_libraries(NativeTests Catch2::Catch2 Threads::Threads)
