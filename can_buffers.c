/*
 * File:   can_buffers.c
 * Author: jimkueneman
 *
 * Created on November 7, 2024, 6:09 AM
 */


#include "xc.h"
#include "can_buffers.h"
#include "openlcb_defines.h"
#include "mcu_driver.h"


can_fifo_t outgoing_can_fifo;

uint16_t pool_can_msg_allocated = 0;
uint16_t max_pool_can_msg_allocated = 0;

void Initialize_CAN_Buffers() {

    for (int iIndex = 0; iIndex < LEN_OUTGOING_CAN_BUFFER; iIndex++) {

        outgoing_can_fifo.list[iIndex].identifier = 0;
        outgoing_can_fifo.list[iIndex].payload_size = 0;

        for (int iBuffer = 0; iBuffer < LEN_CAN_PAYLOAD_DATA; iBuffer++)
            outgoing_can_fifo.list[iIndex].payload[iBuffer] = 0;

    }

    outgoing_can_fifo.head = 0;
    outgoing_can_fifo.tail = 0;

}

extern uint8_t Push_CAN_Message(can_msg_t* msg, uint8_t disable_interrupts) {

    uint8_t result = FALSE;

    if (msg) {

        if (disable_interrupts)
            Ecan1EnableInterrupt(FALSE);

        uint16_t next = outgoing_can_fifo.head + 1;
        if (next >= LEN_CAN_PAYLOAD_DATA)
            next = 0;

        if (next != outgoing_can_fifo.tail) {

            // Copy the message
            outgoing_can_fifo.list[outgoing_can_fifo.head] = *msg;

            outgoing_can_fifo.head = next;
            
            pool_can_msg_allocated = pool_can_msg_allocated + 1;
            
            result = TRUE;

        };

        if (disable_interrupts)
            Ecan1EnableInterrupt(TRUE);

    };

    return result;

}

uint8_t Pop_CAN_Message(can_msg_t* msg, uint8_t disable_interrupts) {
    
    uint8_t result = FALSE;

    if (msg) {
 
        if (disable_interrupts)
            Ecan1EnableInterrupt(FALSE);

        // Is there something here?
        if (outgoing_can_fifo.head != outgoing_can_fifo.tail) {

            *msg = outgoing_can_fifo.list[outgoing_can_fifo.tail];
            
            outgoing_can_fifo.tail = outgoing_can_fifo.tail + 1;
            if (outgoing_can_fifo.tail >= LEN_OPENLCB_MSG_FIFO)
                outgoing_can_fifo.tail = 0;
            
            pool_can_msg_allocated = pool_can_msg_allocated - 1;
            
            result = TRUE;
        }

        if (disable_interrupts)
            Ecan1EnableInterrupt(TRUE);

    }

    return result;

}

extern uint8_t Is_CAN_FIFO_Empty(uint8_t disable_interrupts) {
    
    uint8_t result = TRUE;
    
    if (disable_interrupts)
      Ecan1EnableInterrupt(FALSE);
    
    result = outgoing_can_fifo.head == outgoing_can_fifo.tail;
    
    if (disable_interrupts)
        Ecan1EnableInterrupt(TRUE);
    
    return result;

}