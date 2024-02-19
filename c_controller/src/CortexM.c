// CortexM.c
// Cortex M registers and basic functions used in these labs
// Daniel and Jonathan Valvano
// December 12, 2018
#include <stdint.h>

//******DisableInterrupts************
// sets the I bit in the PRIMASK to disable interrupts
// Inputs: none
// Outputs: none
void DisableInterrupts(void) {
  __asm volatile (
    "    CPSID   I\n"
    // "    bx      lr\n"
  );
}


//******EnableInterrupts************
// clears the I bit in the PRIMASK to enable interrupts
// Inputs: none
// Outputs: none
void EnableInterrupts(void) {
  __asm volatile (
    "    CPSIE   I\n"
    // "    bx      lr\n"
  );
}

//******StartCritical************
// StartCritical saves a copy of PRIMASK and disables interrupts
// Code between StartCritical and EndCritical is run atomically
// Inputs: none
// Outputs: copy of the PRIMASK (I bit) before StartCritical called
long StartCritical(void) {
  long result;
  __asm volatile (
    "    MRS     %0, PRIMASK\n"
    "    CPSID   I\n"
    // "    bx      lr\n"
    : "=r" (result)
  );
  return result;
}


//******EndCritical************
// EndCritical sets PRIMASK with value passed in
// Code between StartCritical and EndCritical is run atomically
// Inputs: PRIMASK (I bit) before StartCritical called
// Outputs: none
void EndCritical(long sr) {
  __asm volatile (
    "    MSR     PRIMASK, R0\n"
    // "    bx      lr\n"
  );
}


//******WaitForInterrupt************
// enters low power sleep mode waiting for interrupt (WFI instruction)
// processor sleeps until next hardware interrupt
// returns after ISR has been run
// Inputs: none
// Outputs: none
void WaitForInterrupt(void) {
  __asm volatile (
    "    WFI\n"
    // "    bx      lr\n"
  );
}

// delay function
// which delays about 6*ulCount cycles
// ulCount=8000 => 1ms = (8000 loops)*(6 cycles/loop)*(20.83 ns/cycle)
void delay(unsigned long ulCount) {
  __asm volatile (
    "loop: subs    %0, #1\n"
    "      bne     loop\n"
    // "      bx      lr\n"
    : : "r" (ulCount)
  );
}

// ------------Clock_Delay1ms------------
// Simple delay function which delays about n milliseconds.
// Inputs: n, number of msec to wait
// Outputs: none
void Clock_Delay1ms(uint32_t n)
{
  while(n)
	{
    delay(8000);   // 1 msec, tuned at 48 MHz
    n--;
  }
}
