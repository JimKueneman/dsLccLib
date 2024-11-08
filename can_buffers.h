/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __CAN_BUFFERS__
#define	__CAN_BUFFERS__

#include <xc.h> // include processor files - each processor file is guarded.  

#define LEN_CAN_PAYLOAD_DATA   8

#define TX_CHANNEL_CAN_CONTROL 0
#define TX_CHANNEL_OPENLCB_MSG 1

#define LEN_OUTGOING_CAN_BUFFER 20   

// Structure for a basic CAN payload
typedef uint8_t payload_bytes_can_t[LEN_CAN_PAYLOAD_DATA];


typedef struct {
    uint32_t identifier; // CAN 29 bit identifier (extended)
    uint8_t payload_size;  // How many bytes are valid
    payload_bytes_can_t payload;    // Payload bytes
} can_msg_t;

typedef struct {
   can_msg_t list[LEN_OUTGOING_CAN_BUFFER];
   uint8_t head;
   uint8_t tail;
} can_fifo_t;


extern can_fifo_t outgoing_can_fifo;


extern uint16_t pool_can_msg_allocated;
extern uint16_t max_pool_can_msg_allocated;

extern void Initialize_CAN_Frame_Buffers();

/*
 * Puts the passed message on the passed FIFO stack
 *     [IN] fifo: the FIFO to operate on
 *     [IN] openlcb_msg: The message to push on the FIFO stack
 *     [IN] disable_interrupts: used to disable the CAN interrupts for resource locking (mutex)
 * 
 * Returns the message passed if it was placed on the FIFO; NULL if there was no space
 */
extern uint8_t Push_CAN_Frame_Message(can_msg_t* msg, uint8_t disable_interrupts);


/*
 * Pulls the first in message on the passed FIFO stack, the msg is removed from the stack
 *     [IN] fifo: the FIFO to operate on
 *     [IN] disable_interrupts: used to disable the CAN interrupts for resource locking (mutex)
 * 
 * Returns: TRUE or FALSE
 */
extern uint8_t Pop_CAN_Frame_Message(can_msg_t* msg, uint8_t disable_interrupts);

extern uint8_t Is_CAN_FIFO_Empty(uint8_t disable_interrupts);

extern void Process_CAN_Frame_Messages();



// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

