/*
 * Copyright (C) 2016 ImageCraft Creations Inc. http://imagecraft.com
 * Released under MIT License
 */
/* NanoExec is a simple preemptive task switcher for Cortex-M (ccurently does not work on M0)
 * You create a "task", which is any C function, by calling the function nanoexec_NewTask().
 * You must supply an array to be used as the task stack. The minimum stack size is #define'd
 * above. The task function should not "return", but if it does, nothing bad will happen.
 * The same function can be used for multiple calls to nanexec_NewTask, and each instance may
 * take a different argument. Per C rules, any local avriables defined within the function are
 * not shared, but any global or static variables are shared.
 *
 * nanoexec_NewTask() returns a task ID, which is a postive integer or -1, if the maximum
 * number of tasks have been created.
 *
 * You may create up to MAX_TASKS number of tasks. Tasks may be created from within another task.
 *
 * Multitasking starts as soon as you invoke nanoexec_Start(). This function DOES NOT RETURN,
 * and execution will never return to your "main" app, and only tasks will be executed. The
 * "errfcn" argument to nanoexec_Start() is for specifying an error function if stack overflow
 * or underflow happens. errfcn will be called with the task ID. errfcn should somehow recovers
 * or reset the system when it is called as the system is corrupted.
 *
 * NanoExec uses the SysTick timer (1ms tick), thus you MUST NOT use the jsapi_cortex_core.SysTick_Timer()
 * function. Instead, YOU MUST use the stm32f4_nanoexec_cortex.s for your vecotr file. A copy is
 * located under c:\iccv8cortex\libsrc.cortex\. This vector file invokes the function
 * nanoexec_SysTick_Handler() to handle the interrupt and task switching.
 *
 * You may however, continue to use the DelayXXX() functions defined in jsapi.h as long as the
 * granularity is 1ms or coarser.
 *
 * To use, use stm32f4_nanoexec_vectors.s for your vector file, as described above, and add
 * "nanoexec" (without the quotes) to your "Project->Build Options->Target->Additional Lib"
 * edit box - multiple libraries (e.g. jsapi-stmf4) can be separated by space
 */
#pragma once

// A basic interrupt frame is 16 registers x 4 = 64 bytes
// Each level of function call is up to 12 registers x 4 = 48 bytes
// plus any local stack space
#define NANOEXEC_MIN_STACKSIZE  200
#define MAX_TASKS               10

void nanoexec_Start(void (*errfcn)(unsigned));
int nanoexec_NewTask(void (*p)(int), unsigned arg, unsigned char *stack_addr, unsigned stack_size);
