/*
 * Copyright (C) 2016 ImageCraft Creations Inc. http://imagecraft.com
 * Released under MIT License
 */

// Internal header file for nanoexec. Users should only include "nanoexec.h"
#pragma once
#include "nanoexec.h"

#define EXEC_FLAG   0x1
#define IN_USE_FLAG 0x2

//This defines the stack frame that is saved  by the hardware
typedef struct {
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;
  uint32_t pc;
  uint32_t psr;
} hw_stack_frame_t;

//This defines the stack frame that must be saved by the software
typedef struct {
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t r11;
  unsigned LR;
} sw_stack_frame_t;

typedef struct {
     unsigned sp;       //The task's current stack pointer
     unsigned flags;    //Status flags includes activity status, etc

     // The following are for sanity check at context switching to ensure that the
     // task's stack pointer does not go out of bound
     unsigned initial_sp;
     int stack_size;
} TASK_CONTROL_BLOCK;
