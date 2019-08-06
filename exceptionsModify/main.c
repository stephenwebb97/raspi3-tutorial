/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "uart.h"
#include "mmu.h"

void go_to_el0(){
    asm volatile ("mov     x2, #(0)"); //The Ret Stack Pointer of EL1h
/*    orr     x2, x2, #(1<<4) //FIQ mask
    orr     x2, x2, #(1<<6) //FIQ mask
    orr     x2, x2, #(1<<7) //IRQ mask
    orr     x2, x2, #(1<<8) //SError mask
    orr     x2, x2, #(1<<9) //Debug mask
*/
    asm volatile ("msr     spsr_el1, x2");
    asm volatile ("adr     x2, el0");
    asm volatile ("msr     elr_el1, x2");
    asm volatile ("eret");
}

void el0(){
    asm volatile ("SVC #0");
    uart_puts("Im In EL0");
    while(1);
}

void main()
{
    // unsigned int r;
    unsigned long el;

    // set up serial console
    uart_init();

    asm volatile ("mrs %0, CurrentEL" : "=r" (el));

    // asm volatile ("mov x0, #0");
    // asm volatile ("msr mair_el1, x0");

    uart_puts("Current EL is: ");
    uart_hex((el>>2)&3);
    uart_puts("\n");

    go_to_el0();

    // mmu_init();
    
    // set up paging
    

    // generate a Data Abort with a bad address access
    // r=*((volatile unsigned int*)0xFFFFFFFFFF000000);
    // make gcc happy about unused variables :-)
    // r++;

    uart_puts("SuccessFully Returned From Handler\n");
    // echo everything back
    while(1) {
        uart_send(uart_getc());
    }
}
