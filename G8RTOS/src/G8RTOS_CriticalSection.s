; G8RTOS_CriticalSection.s
; Created: 2022-07-26
; Updated: 2022-07-26
; Contains assembly functions for entering and ending critical sections.

	; Functions Defined
	.def StartCriticalSection, EndCriticalSection

	.thumb		; Set to thumb mode
	.align 2	; Align by 2 bytes (thumb mode uses allignment by 2 or 4)
	.text		; Text section

; Starts a critical section
; 	- Saves the state of the current PRIMASK (I-bit)
; 	- Disables interrupts
; Returns: The current PRIMASK State
StartCriticalSection:
	.asmfunc

	MRS R0, PRIMASK		; Save PRIMASK to R0 (Return Register)
	CPSID I				; Disable Interrupts
	BX LR				; Return

	.endasmfunc

; Ends a critical Section
; 	- Restores the state of the PRIMASK given an input
; Param R0: PRIMASK State to update
EndCriticalSection:
	.asmfunc

	MSR PRIMASK, R0		; Save R0 (Param) to PRIMASK
	BX LR				; Return

	.endasmfunc

; end G8RTOS_CriticalSection.s
