;
; Copyright (C) 2016 ImageCraft Creations Inc. http://imagecraft.com
; Released under MIT License
;
    .text
;
; the Systock hanlder
; saves the rest of the registers to PSP, calls nanoexec_ContextSwitch to select
; a new task, then assigns the new task's SP to PSP and resumes the task
;
_nanoexec_SysTick_Handler::
    mrs R0,PSP
    stmdb R0!,{R4-R11,LR}
    msr PSP,R0              // pass PSP to ContextSwitch
    bl _nanoexec_ContextSwitch

    ldmfd R0!,{R4-R11,LR}   // restore R4-R11 from new task's stack
    msr PSP,R0              // update PSP
    bx  LR



