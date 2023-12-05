; G8RTOS_SchedulerASM.s
; Created: 2022-07-26
; Updated: 2022-07-26
; Contains assembly functions for scheduler.

	; Functions Defined
  .def G8RTOS_Start
  .def PendSV_Handler

	; Dependencies
	.ref CurrentlyRunningThread, G8RTOS_Scheduler

	.thumb		; Set to thumb mode
	.align 2	; Align by 2 bytes (thumb mode uses allignment by 2 or 4)
	.text		; Text section

; Need to have the address defined in file
; (label needs to be close enough to asm code to be reached with PC relative addressing)
RunningPtr: .field CurrentlyRunningThread, 32

; G8RTOS_Start
;	Sets the first thread to be the currently running thread
;	Starts the currently running thread by setting Link Register to tcb's Program Counter
G8RTOS_Start:

	.asmfunc

	; Load the address of RunningPtr
  LDR R0, RunningPtr

	; Load the address of the thread control block of the currently running pointer
	LDR R0, [R0]

	; Load the first thread's stack pointer
	LDR SP, [R0]

	; Load LR with the first thread's PC
	LDR LR, [R0, #4]

	;Branches to the first thread
	BX LR

  .endasmfunc

; PendSV_Handler
; - Performs a context switch in G8RTOS
; 	- Saves remaining registers into thread stack
; - Saves current stack pointer to tcb
; - Calls G8RTOS_Scheduler to get new tcb
; - Set stack pointer to new stack pointer from new tcb
; - Pops registers from thread stack
PendSV_Handler:

	.asmfunc

  CPSID I
  PUSH {R4 - R11}

  ; start non-provided pendsv code


  ; Save LR
  MOV R4, LR


  ; Save current stack pointer to RunningPtr tcb
  LDR R1, RunningPtr
  LDR R5, [R1]
  STR SP, [R5]


  ; Call G8RTOS_Scheduler to get new tcb
  BL G8RTOS_Scheduler


  ; Restore LR
  MOV LR, R4


  ; Load the address of RunningPtr
  LDR R0, RunningPtr

  ; Load the address of the tcb of the new currently running thread
  LDR R0, [R0]

  ; Set stack pointer to new stack pointer from new tcb
  LDR SP, [R0]


  ; end non-provided pendsv code

  POP {R4 - R11}
  CPSIE I
  BX LR

	.endasmfunc

	; end of the asm file
	.align
	.end
