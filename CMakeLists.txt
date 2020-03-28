cmake_minimum_required(VERSION 3.10)

include(ExternalProject)

project(JunglesOsHelpers)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -Wall)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wall)

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

set(CATCH_ROOT_DIR ${CMAKE_BINARY_DIR}/catch2_repo)
list(APPEND CATCH2_CMAKE_ARGS 
    "-DCATCH_BUILD_TESTING=OFF" "-DCATCH_INSTALL_DOCS=OFF" "-DCATCH_INSTALL_HELPERS=OFF"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CATCH_ROOT_DIR}"
)
ExternalProject_Add(catch2_repo
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    CMAKE_ARGS ${CATCH2_CMAKE_ARGS}
)
add_library(catch2 INTERFACE IMPORTED)
add_dependencies(catch2 catch2_repo)
target_include_directories(catch2 INTERFACE ${CATCH_ROOT_DIR}/include)

add_executable(TestBuild test/freertos/main.cpp test/freertos/os_delayed_job_test.cpp)
include_directories(${CMAKE_SOURCE_DIR})
target_link_libraries(TestBuild freertos_kernel catch2)
