/*
 * Copyright (C) 2016 ImageCraft Creations Inc. http://imagecraft.com
 * Released under MIT License
 */
#include <stdint.h>
#include <jsapi.h>
#include "_nanoexec.h"

/* This code uses ImageCraft's JumpStart API (JSAPI) but their uses can be replaced with
 * similar calls to set up systick interrupt etc. Nanoexec is also described in our blog
 * http://imagecraft.com/blog
 *
 * Also see nanoexec.h for usage details.
 *
 * Any printf/putchar calls should be removed if your system does not support them. They
 * are used for debug display purpose.
 */
extern void JSAPI_SysTick_Handler(void);
extern void JSAPI_SysTick_MillisecondTimer(void);

#define MAIN_RETURN     0xFFFFFFF9  //Tells the handler to return using the MSP
#define THREAD_RETURN   0xFFFFFFFD  //Tells the handler to return using the PSP

static int current_task;
static TASK_CONTROL_BLOCK task_table[MAX_TASKS];
static unsigned char task0_stack[sizeof (hw_stack_frame_t) + sizeof (sw_stack_frame_t) + 100];

static void (*error_function)(unsigned);
static void InitializeHardwareFrame(int i, void (*p)(int), unsigned arg, unsigned char *stack_addr, unsigned stack_size);
static void del_process(void);
void task0(int);

#define SetPSPtoV(s)    do { unsigned v; v = s; asm("msr PSP,%v"); } while (0)

void nanoexec_Start(void (*errfcn)(unsigned))
    {
    error_function = errfcn;
    InitializeHardwareFrame(0, task0, 0, task0_stack, sizeof (task0_stack));
    SetPSPtoV(task_table[0].sp);
    jsapi_cortex_core.SysTick_Timer(SYSTICK_MILLISECOND);

    // the loop runs until the first context switch. Afterward, the code will never
    // run again
    while (1)
        ;
    }

int nanoexec_NewTask(void (*p)(int), unsigned arg, unsigned char *stack_addr, unsigned stack_size)
    {
    int i;

    for(i = 1; i < MAX_TASKS && task_table[i].flags != 0; i++)
        ;
    if (i == MAX_TASKS)
        return -1;

    InitializeHardwareFrame(i, p, arg, stack_addr, stack_size);
    task_table[i].flags = IN_USE_FLAG | EXEC_FLAG;
    task_table[i].sp -= sizeof(sw_stack_frame_t);
    ((sw_stack_frame_t *)task_table[i].sp)->LR = THREAD_RETURN;

    return i;
    }

/* nanoexec_ContextSwitch selects the next task to run.
 * The argument is the PSP (Processor Stack Pointer) of the last running task.
 * It returns the SP of the newly selected task.
 */
unsigned nanoexec_ContextSwitch(unsigned sp)
    {
    // JSAPI requirement
    JSAPI_SysTick_Handler();

    // Sanity check
    if (sp < task_table[current_task].initial_sp ||
        task_table[current_task].initial_sp + task_table[current_task].stack_size < sp)
        {
        if (error_function)
            error_function((sp & 0xFFFF)| task_table[current_task].initial_sp << 16);
        while (1)
            ;
        }

    task_table[current_task].sp = sp;

    while (1)
        {
        current_task++;
        if (current_task == MAX_TASKS)
            {
            current_task = 0;
            // only need the first run as it was set as MAIN_RETURN at the first interrupt
            ((sw_stack_frame_t *)task_table[0].sp)->LR = THREAD_RETURN;
            break;
            }
        else if (task_table[current_task].flags & EXEC_FLAG)
            break;
        }

    return task_table[current_task].sp;
    }

static void InitializeHardwareFrame(int i, void (*p)(int), unsigned arg, unsigned char *stack_addr, unsigned stack_size)
    {
    TASK_CONTROL_BLOCK *task = &task_table[i];

    // Align the stack at 8 bytes boundary for best performance
    stack_addr = (unsigned char *)((unsigned)stack_addr & ~0x7);
    task->stack_size = stack_size = stack_size & ~0x7;
    task->initial_sp = (unsigned)stack_addr;

    unsigned stack_top = (unsigned)stack_addr + stack_size;

    hw_stack_frame_t *process_frame = (hw_stack_frame_t *)(stack_top - sizeof(hw_stack_frame_t));
    process_frame->r0 = (unsigned)arg;
    process_frame->r1 = 0;
    process_frame->r2 = 0;
    process_frame->r3 = 0;
    process_frame->r12 = 0;
    process_frame->pc = (unsigned)p | 0x1;
    process_frame->lr = (unsigned)del_process | 0x1;
    process_frame->psr = 0x1000000; //default PSR value, T(humb) state
    task->sp = (unsigned)process_frame;

    printf("task %d initial_sp %x sp %x\n", i, task->initial_sp, task->sp);
    }

//This is called when the task returns
static void del_process(void)
    {
    task_table[current_task].flags = 0; //clear the in use and exec flags
    while(1)
        ; //once the context changes, the program will no longer return to this thread
    }

// This is the default task. Body should be replaced by your code (empty forever loop)
void task0(int i)
    {
    while (1)
        {
        putchar('-');
        DelaySecs(5);
        }
    }
