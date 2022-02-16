/**
 * @file	main.cpp
 * @brief	Generates main for Catch and defines common setup for the tests.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include "catch2/catch_session.hpp"

#include <algorithm>
#include <iostream>
#include <thread>

#include "FreeRTOS.h"
#include "task.h"

// --------------------------------------------------------------------------------------------------------------------
// The vApplicationGetIdleTaskMemory and vApplicationGetTimerTaskMemory code below is copy-pasted from the FreeRTOS
// documentation page.
// --------------------------------------------------------------------------------------------------------------------

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
extern "C" void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer,
                                              StackType_t** ppxIdleTaskStackBuffer,
                                              uint32_t* pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static – otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task’s
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task’s stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*———————————————————–*/

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
extern "C" void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer,
                                               StackType_t** ppxTimerTaskStackBuffer,
                                               uint32_t* pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static – otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task’s state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task’s stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    static int result{1};

    using program_args = std::pair<int, char**>;
    program_args arg{argc, argv};
    xTaskCreate(
        [](void* arg_pv) {
            auto arg_p{static_cast<program_args*>(arg_pv)};
            auto [argc, argv] = *arg_p;
            std::cout << "TEST FreeRTOS: Running tests." << std::endl;
            result = Catch::Session().run(argc, argv);
            std::cout << "TEST FreeRTOS: Stopping scheduler" << std::endl;
            vTaskEndScheduler();

            std::cout << "TEST FreeRTOS: Scheduler end commissioned, but SHALL NOT GET HERE." << std::endl;
            vTaskDelete(nullptr);
        },
        "TestRunner",
        2048,
        &arg,
        1,
        nullptr);

    std::cout << "TEST FreeRTOS: Starting scheduler" << std::endl;
    vTaskStartScheduler();
    std::cout << "TEST FreeRTOS: Scheduler stopped, finished, leaving." << std::endl;

    return result;
}
