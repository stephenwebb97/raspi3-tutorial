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

/**
 * common exception handler
 */
void exc_handler(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far)
{
    // void * returnAddress = (void *)elr;
    // print out interruption type
    switch(type) {
        case 0: uart_puts("Synchronous"); break;
        case 1: uart_puts("IRQ"); break;
        case 2: uart_puts("FIQ"); break;
        case 3: uart_puts("SError"); break;
    }
    uart_puts(": ");
    // decode exception type (some, not all. See ARM DDI0487B_b chapter D10.2.28)
    switch(esr>>26) {
        case 0b000000: uart_puts("Unknown"); break;
        case 0b000001: uart_puts("Trapped WFI/WFE"); break;
        case 0b001110: uart_puts("Illegal execution"); break;
        case 0b010101: uart_puts("System call"); break;
        case 0b100000: uart_puts("Instruction abort, lower EL"); break;
        case 0b100001: uart_puts("Instruction abort, same EL"); break;
        case 0b100010: uart_puts("Instruction alignment fault"); break;
        case 0b100100: uart_puts("Data abort, lower EL"); break;
        case 0b100101: uart_puts("Data abort, same EL"); break;
        case 0b100110: uart_puts("Stack alignment fault"); break;
        case 0b101100: uart_puts("Floating point"); break;
        default: uart_puts("Unknown Default"); break;
    }
    // decode data abort cause
    if(esr>>26==0b100100 || esr>>26==0b100101) {
        uart_puts(", ");
        switch((esr>>2)&0x3) {
            case 0: uart_puts("Address size fault"); break;
            case 1: uart_puts("Translation fault"); break;
            case 2: uart_puts("Access flag fault"); break;
            case 3: uart_puts("Permission fault"); break;
        }
        switch(esr&0x3) {
            case 0: uart_puts(" at level 0"); break;
            case 1: uart_puts(" at level 1"); break;
            case 2: uart_puts(" at level 2"); break;
            case 3: uart_puts(" at level 3"); break;
        }
    }
    // dump registers
    uart_puts(":\n  ESR_EL1 ");
    uart_hex(esr>>32);
    uart_hex(esr);
    uart_puts(" ELR_EL1 ");
    uart_hex(elr>>32);
    uart_hex(elr);
    uart_puts("\n SPSR_EL1 ");
    uart_hex(spsr>>32);
    uart_hex(spsr);
    uart_puts(" FAR_EL1 ");
    uart_hex(far>>32);
    uart_hex(far);
    uart_puts("\n");

    // ----------------------- FOR TESTING --------------------------- //

    // Skips over the faulting address
    uart_puts("\n");
    uart_hex(elr);
    elr = elr + 1;
    asm volatile ("msr esr_el1, %0"::"r" (elr));
    asm volatile ("mrs %0, esr_el1":"=r" (elr));
    uart_puts("\n");
    uart_hex(elr);
    uart_puts("\n");
    asm volatile ("mov x2, #(1<<2)");
    asm volatile ("msr spsr_el1, x2");
    asm volatile ("eret");
    // no return from exception for now
    while(1);
}

void exc_to_el2(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far)
{
    asm volatile ("HVC #0");
    uart_puts("I have Been Redirected to el2\n");
    asm volatile ("eret");
}

void exc_hyper_handler_lower(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far)
{
    // void * el1_vector_table;

    uart_puts("Hyper Event Lower Handler\n");

    // asm volatile ("msr esr_el1, %0"::"r" (esr));
    // asm volatile ("msr elr_el1, %0"::"r" (elr));
    // asm volatile ("msr spsr_el1, %0"::"r" (spsr));
    // asm volatile ("msr far_el1, %0"::"r" (far));

    // asm volatile ("mov x2, #(1<<2)");
    // asm volatile ("msr spsr_el2, x2");

    // // So It Maps To the correct interupt handler 
    // asm volatile ("mrs %0, vbar_el1":"=r" (el1_vector_table));
    // el1_vector_table = el1_vector_table + (type * 128);
    // asm volatile ("msr elr_el2, %0"::"r" (el1_vector_table));
    asm volatile ("eret");
    while(1);
}

void exc_hyper_handler_current(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far)
{
    while(1);
    
    void * returnAddress;
    void * el1_vector_table;

    uart_puts("Hyper Current Event Handler\n");

    // print out interruption type
    switch(type) {
        case 0: uart_puts("Synchronous"); break;
        case 1: uart_puts("IRQ"); break;
        case 2: uart_puts("FIQ"); break;
        case 3: uart_puts("SError"); break;
    }
    uart_puts(": ");
    // decode exception type (some, not all. See ARM DDI0487B_b chapter D10.2.28)
    switch(esr>>26) {
        case 0b000000: uart_puts("Unknown"); break;
        case 0b000001: uart_puts("Trapped WFI/WFE"); break;
        case 0b001110: uart_puts("Illegal execution"); break;
        case 0b010101: uart_puts("System call"); break;
        case 0b100000: uart_puts("Instruction abort, lower EL"); break;
        case 0b100001: uart_puts("Instruction abort, same EL"); break;
        case 0b100010: uart_puts("Instruction alignment fault"); break;
        case 0b100100: uart_puts("Data abort, lower EL"); break;
        case 0b100101: uart_puts("Data abort, same EL"); break;
        case 0b100110: uart_puts("Stack alignment fault"); break;
        case 0b101100: uart_puts("Floating point"); break;
        default: uart_puts("Unknown Default"); break;
    }
    // decode data abort cause
    if(esr>>26==0b100100 || esr>>26==0b100101) {
        uart_puts(", ");
        switch((esr>>2)&0x3) {
            case 0: uart_puts("Address size fault"); break;
            case 1: uart_puts("Translation fault"); break;
            case 2: uart_puts("Access flag fault"); break;
            case 3: uart_puts("Permission fault"); break;
        }
        switch(esr&0x3) {
            case 0: uart_puts(" at level 0"); break;
            case 1: uart_puts(" at level 1"); break;
            case 2: uart_puts(" at level 2"); break;
            case 3: uart_puts(" at level 3"); break;
        }
    }
    // dump registers
    uart_puts(":\n  ESR_EL1 ");
    uart_hex(esr>>32);
    uart_hex(esr);
    uart_puts(" ELR_EL1 ");
    uart_hex(elr>>32);
    uart_hex(elr);
    uart_puts("\n SPSR_EL1 ");
    uart_hex(spsr>>32);
    uart_hex(spsr);
    uart_puts(" FAR_EL1 ");
    uart_hex(far>>32);
    uart_hex(far);
    uart_puts("\n");


    asm volatile ("msr esr_el1, %0"::"r" (esr));
    asm volatile ("msr elr_el1, %0"::"r" (elr));
    asm volatile ("msr spsr_el1, %0"::"r" (spsr));
    asm volatile ("msr far_el1, %0"::"r" (far));

    asm volatile ("mrs %0, elr_el2":"=r" (returnAddress));

    returnAddress = returnAddress + 4;
    asm volatile ("msr elr_el2, %0"::"r" (returnAddress));

    // asm volatile ("mov x2, #(1<<2)");
    // asm volatile ("msr spsr_el2, x2");

    // So It Maps To the correct interupt handler 
    asm volatile ("mrs %0, vbar_el1":"=r" (el1_vector_table));
    el1_vector_table = el1_vector_table + (type * 128);
    asm volatile ("msr elr_el2, %0"::"r" (el1_vector_table));
    // asm volatile ("eret");

    while(1);

}




