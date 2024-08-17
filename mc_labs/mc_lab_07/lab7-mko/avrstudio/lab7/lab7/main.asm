
.def temp = r16
.def isr_temp1 = r17
.def isr_temp2 = r18



.cseg
.org 0x0000            ; the next instruction has to be written to
                       ; address 0x0000
rjmp setup             ; the reset vector: jump to "main"



.org 0x0028
jmp TIM1_OVF		   ; Timer1 overflow handler



.org 0x0100
setup:
	ldi temp, 0xFF
	out DDRF, temp          ; set ports F and K to be output ports
	sts DDRK, temp
	
	ldi temp, 0x20
	sts DDRH, temp

	ldi temp, 0b00000101
	sts PortL, temp


	; stack is neccessary, because when interrupt occurs, you need to save
	; the address of the instruction you were on to return back to it later
	; this instruction is stored on the stack
	ldi temp, low(RAMEND) 
	out SPL, temp
	ldi r16, high(RAMEND)
	out SPH, temp

	ldi temp, 1			; we can only load values from registers
	sts TIMSK1, temp		; enable interrupts for timer 1
	sei					; enable interrupts globally

	rcall reset_timer

	ldi temp, 0x00
	sts TCCR1A, temp		; we don't use wave generation mode, so leave this at 0 
	ldi temp, 0b100
	sts TCCR1B, temp		; but we do use prescaler, which is 256 in this case

	rjmp loop



loop:
	lds temp, PinL
	andi temp, 0x01
	cpi temp, 0
	breq loop_algo1
	
	lds temp, PinL
	andi temp, 0x04
	cpi temp, 0
	breq loop_algo3
	
	rjmp loop 

loop_algo1:
	rcall run_algo1
	rjmp loop 
loop_algo3:
	rcall run_algo3
	rjmp loop 

           ; jump back to loop

run_algo1:
	ldi temp, 0x01
	sts PortK, temp

	rcall turn_buzzer_on

	rcall reset_timer
	reti

run_algo3:
	ldi temp, 0x81
	out PortF, temp

	rcall turn_buzzer_on

	rcall reset_timer
	reti

turn_buzzer_on:
	lds isr_temp1, PortH
	ori isr_temp1, 0b00100000
	clz
	cln
	clv
	sts PortH, isr_temp1
	reti

turn_buzzer_off:
	lds isr_temp1, PortH
	andi isr_temp1, ~0b00100000
	clz
	cln
	clv
	sts PortH, isr_temp1
	reti

algo1:
	lds isr_temp1, PortK
	cpi isr_temp1, 0x00
	breq algo1_ret

	lsl isr_temp1
	clc
	sts PortK, isr_temp1
algo1_ret:
	reti


algo3:
	in isr_temp1, PortF
	cpi isr_temp1, 0x00
	breq algo3_ret

	mov isr_temp2, isr_temp1
	lsl isr_temp1
	lsr isr_temp2
	andi isr_temp1, 0x0F
	andi isr_temp2, 0xF0
	or isr_temp1, isr_temp2
	clz
	cln
	clv
	clc
	out PortF, isr_temp1
algo3_ret:
	reti


	

reset_timer:
	; set timer initial value to be 0xC350
	ldi isr_temp1, 0xC3
	sts TCNT1L, isr_temp1		; load the low part of timer initial value
	ldi isr_temp1, 0x50
	sts TCNT1H, isr_temp1		; load the high part of timer initial value
	reti
	
	
TIM1_OVF:
	cli							; disable all interrupts for the time of this isr

	rcall algo1
	rcall algo3
	;cbi PortH, 5			; switch buzzer off

	rcall reset_timer			; reset timer
	rcall turn_buzzer_off
	
	sei							; enable back interrupts
	reti						; return from the isr
