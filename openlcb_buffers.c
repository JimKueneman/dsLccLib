/*
 * File:   buffers.c
 * Author: jimkueneman
 *
 * Created on February 27, 2024, 5:22 AM
 */


#include "xc.h"
#include "openlcb_buffers.h"
#include "mcu_driver.h"
#include "openlcb_defines.h"


#include "stdio.h"  // printf



// list to hold pointers to openlcb_msg structures, since these are waiting for multiframe messages to arrive there is no
// definition of order.  The first one that receives all of its frames will be moved to the correct openlcb_msg_fifo and from there it is 
// sent/received in the correct fifo 


openlcb_msg_buffer_t incoming_openlcb_msg_fifo; // buffer for complete OpenLCB messages ready to dispatch to internal node
openlcb_msg_buffer_t outgoing_openlcb_msg_fifo; // buffer for complete OpenLCB messages to be sent out
inprocess_buffer_t incoming_openlcb_inprocess_msg_list; // buffer for multiframe incoming messages to be assembled and eventually moved to the incoming_openlcb_msg_fifo sructure (or deleted if abandon/error) 

openlcb_msg_pool_t openlcb_msg_pool;

payload_basic_t payload_basic_pool[LEN_DATA_SIZE_BASIC_POOL];
payload_datagram_t payload_datagram_pool[LEN_DATA_SIZE_DATAGRAM_POOL];
payload_stream_snip_t payload_stream_snip_pool[LEN_DATA_SIZE_STREAM_SNIP_POOL];

// Some metrics
uint16_t pool_openlcb_msg_allocated = 0;
uint16_t max_pool_openlcb_msg_allocated = 0;

/*
 * [IN] source_alias
 * [IN] mti
 * [IN] direction: which way this message is going (DIRECTION_RX or DIRECTION_RX);
 * [IN] data_size: size of the data buffer (ID_DATA_SIZE_BASIC, ID_DATA_SIZE_DATAGRAM, ID_DATA_SIZE_STREAM_SNIP)
 */
openlcb_msg_t* Allocate_OpenLcb_Msg(uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t mti, uint8_t buffer_size, uint8_t disable_interrupts) {
    
    if (disable_interrupts)
        Ecan1EnableInterrupt(FALSE);

    int iStart = 0;
    int iEnd = 0;


    // Where to look in the openlcb_msg_pool array for structure that have this length payload buffer

    switch (buffer_size) {

        case ID_DATA_SIZE_BASIC:
            iStart = 0;
            iEnd = LEN_DATA_SIZE_BASIC_POOL;
            break;
        case ID_DATA_SIZE_DATAGRAM:
            iStart = LEN_DATA_SIZE_BASIC_POOL;
            iEnd = (LEN_DATA_SIZE_BASIC_POOL + LEN_DATA_SIZE_DATAGRAM_POOL);
            break;
        case ID_DATA_SIZE_STREAM_SNIP:
            iStart = LEN_DATA_SIZE_BASIC_POOL + LEN_DATA_SIZE_DATAGRAM_POOL;
            iEnd = (LEN_DATA_SIZE_BASIC_POOL + LEN_DATA_SIZE_DATAGRAM_POOL + LEN_DATA_SIZE_STREAM_SNIP_POOL);
            break;

    };

    openlcb_msg_t* new_msg = (void*) 0;

    for (int iIndex = iStart; iIndex < iEnd; iIndex++) {

        if (!openlcb_msg_pool[iIndex].state.allocated) {
           

            openlcb_msg_pool[iIndex].state.allocated = 1;
            openlcb_msg_pool[iIndex].state.data_struct_size = buffer_size;
            openlcb_msg_pool[iIndex].state.valid = FALSE;
            openlcb_msg_pool[iIndex].mti = mti;
            openlcb_msg_pool[iIndex].dest_alias = dest_alias;
            openlcb_msg_pool[iIndex].source_alias = source_alias;
            openlcb_msg_pool[iIndex].dest_id = dest_id;
            openlcb_msg_pool[iIndex].source_id = source_id;


            pool_openlcb_msg_allocated = pool_openlcb_msg_allocated + 1;

            if (max_pool_openlcb_msg_allocated < pool_openlcb_msg_allocated)
                max_pool_openlcb_msg_allocated = pool_openlcb_msg_allocated;

            new_msg = &openlcb_msg_pool[iIndex];
            break;

        }

    };

    if (disable_interrupts)
        Ecan1EnableInterrupt(TRUE);

    return new_msg;

};

void Release_OpenLcb_Msg(openlcb_msg_t* openlcb_msg, uint8_t disable_interrupts) {

    if (disable_interrupts)
        Ecan1EnableInterrupt(FALSE);

    if (openlcb_msg) {

        openlcb_msg->state.allocated = 0;
        pool_openlcb_msg_allocated = pool_openlcb_msg_allocated - 1;

    }

    if (disable_interrupts)
        Ecan1EnableInterrupt(TRUE);

};


/*
 * Uses passed array as a raw array buffer and puts the passed message in the first available slot it finds
 *     [IN] buffer_ptr: the array to operate on
 *     [IN] openlcb_msg: The message to push on the FIFO stack
 *     [IN] disable_interrupts: used to disable the CAN interrupts for resource locking (mutex)
 */
openlcb_msg_t* Insert_OpenLcb_Message(inprocess_buffer_t* buffer_ptr, openlcb_msg_t* openlcb_msg_ptr, uint8_t disable_interrupts) {

    openlcb_msg_t* result = (void*) 0;

    if (disable_interrupts)
        Ecan1EnableInterrupt(FALSE);

    for (int iIndex = 0; iIndex < LEN_OPENLCB_MSG_FIFO; iIndex++) {

        if (buffer_ptr->list[iIndex] == (void*) 0) {

            buffer_ptr->list[iIndex] = openlcb_msg_ptr;
            result = openlcb_msg_ptr;
            break;

        }
    }

    if (disable_interrupts)
        Ecan1EnableInterrupt(TRUE);

    return result;

};

openlcb_msg_t* Find_OpenLcb_Message_As_Buffer(inprocess_buffer_t* buffer_ptr, uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t mti, uint8_t disable_interrupts, uint8_t remove) {

    openlcb_msg_t* result = (void*) 0;

    if (disable_interrupts)
        Ecan1EnableInterrupt(FALSE);

    if ((source_alias > 0) && (mti > 0) && buffer_ptr) {

        for (int iIndex = 0; iIndex < LEN_OPENLCB_MSG_FIFO; iIndex++) {

            if ((buffer_ptr->list[iIndex]->mti = mti) &&
                    (buffer_ptr->list[iIndex]->source_alias = source_alias) &&
                    (buffer_ptr->list[iIndex]->dest_alias = dest_alias)) {

                result = buffer_ptr->list[iIndex];

                if (remove) 
                    buffer_ptr->list[iIndex] = (void*) 0;
        

                break;

            };

        };

    };

    if (disable_interrupts)
        Ecan1EnableInterrupt(TRUE);

    return result;

}

/*
 * Uses the passed array as a FIFO and puts the passed message on the passed FIFO stack
 *     [IN] fifo_ptr: the FIFO to operate on
 *     [IN] openlcb_msg: The message to push on the FIFO stack
 *     [IN] disable_interrupts: used to disable the CAN interrupts for resource locking (mutex)
 */
openlcb_msg_t* Push_OpenLcb_Message(openlcb_msg_buffer_t* fifo_ptr, openlcb_msg_t* msg, uint8_t disable_interrupts) {

    openlcb_msg_t* result = (void*) 0;

    if ((fifo_ptr) && (msg)) {

        if (disable_interrupts)
            Ecan1EnableInterrupt(FALSE);

        uint16_t next = fifo_ptr->head + 1;
        if (next >= LEN_OPENLCB_MSG_FIFO)
            next = 0;

        if (next != fifo_ptr->tail) {

            fifo_ptr->list[fifo_ptr->head] = msg;
            fifo_ptr->head = next;

            result = msg;

        };

        if (disable_interrupts)
            Ecan1EnableInterrupt(TRUE);

    };

    return result;

};

/*
 * Pulls the first in message on the passed FIFO stack, the msg is removed from the stack
 *     [IN] fifo_ptr: the FIFO to operate on
 *     [IN] disable_interrupts: used to disable the CAN interrupts for resource locking (mutex)
 * RESULT: 
 *     openlcb_msg that is next in line to be popped
 */
openlcb_msg_t* Pop_OpenLcb_Message(openlcb_msg_buffer_t* fifo_ptr, uint8_t disable_interrupts) {

    openlcb_msg_t* result = (void*) 0;

    if (fifo_ptr) {

        if (disable_interrupts)
            Ecan1EnableInterrupt(FALSE);

        // Is there something here?
        if (fifo_ptr->head != fifo_ptr->tail) {

            result = fifo_ptr->list[fifo_ptr->tail];
            fifo_ptr->list[fifo_ptr->tail] = (void*) 0; //  helps with debugging

            fifo_ptr->tail = fifo_ptr->tail + 1;
            if (fifo_ptr->tail >= LEN_OPENLCB_MSG_FIFO)
                fifo_ptr->tail = 0;
        }


        if (disable_interrupts)
            Ecan1EnableInterrupt(TRUE);

    };

    return result;
};

uint8_t Is_OpenLcb_FIFO_Empty(openlcb_msg_buffer_t* fifo_ptr, uint8_t disable_interrupts) {
    
    uint8_t result = TRUE;
    
    if (disable_interrupts)
      Ecan1EnableInterrupt(FALSE);
    
    result = fifo_ptr->head == fifo_ptr->tail;
    
    if (disable_interrupts)
        Ecan1EnableInterrupt(TRUE);
    
    return result;
    
}



void Initialize_OpenLcb_Buffers() {

    for (int iIndex = 0; iIndex < LEN_OPENLCB_MSG_POOL; iIndex++) {
        
        openlcb_msg_pool[iIndex].dest_alias = 0;
        openlcb_msg_pool[iIndex].dest_id = 0;
        openlcb_msg_pool[iIndex].source_alias = 0;
        openlcb_msg_pool[iIndex].source_id = 0;
        openlcb_msg_pool[iIndex].mti = 0;
        openlcb_msg_pool[iIndex].payload_count = 0;
        openlcb_msg_pool[iIndex].payload_ptr = (void*) 0;

        openlcb_msg_pool[iIndex].state.allocated = FALSE;
        openlcb_msg_pool[iIndex].state.data_struct_size = 0;
        openlcb_msg_pool[iIndex].state.valid = FALSE;

 
        // Assign the pointers to the payload structures in the openlcb_msg structure
        // Notice that the datagram and stream/snip are allocated from top down in their respective arrays

        if (iIndex >= (LEN_DATA_SIZE_BASIC_POOL + LEN_DATA_SIZE_DATAGRAM_POOL)) {

            openlcb_msg_pool[iIndex].payload_ptr = &payload_stream_snip_pool[LEN_OPENLCB_MSG_POOL - iIndex - 1];
            openlcb_msg_pool[iIndex].state.data_struct_size = ID_DATA_SIZE_STREAM_SNIP;

            for (int j = 0; j < LEN_DATA_STREAM_SNIP; j++) 
                (*((payload_stream_snip_t*) openlcb_msg_pool[iIndex].payload_ptr))[j] = 0x00;
 

        } else if (iIndex >= (LEN_DATA_SIZE_BASIC_POOL)) {

            openlcb_msg_pool[iIndex].payload_ptr = &payload_datagram_pool[ (LEN_OPENLCB_MSG_POOL - LEN_DATA_SIZE_DATAGRAM_POOL) - iIndex - 1];
            openlcb_msg_pool[iIndex].state.data_struct_size = ID_DATA_SIZE_DATAGRAM;

            for (int j = 0; j < LEN_DATA_DATAGRAM; j++) 
                (*((payload_datagram_t*) openlcb_msg_pool[iIndex].payload_ptr))[j] = 0x00;
       

        } else {

            openlcb_msg_pool[iIndex].payload_ptr = &payload_basic_pool[iIndex];
            openlcb_msg_pool[iIndex].state.data_struct_size = ID_DATA_SIZE_BASIC;

            for (int j = 0; j < LEN_DATA_BASIC; j++) 
                (*((payload_basic_t*) openlcb_msg_pool[iIndex].payload_ptr))[j] = 0x00;
   
        };

        // zero out the buffers holding pointers to the open_lcb message structures initialized above
        incoming_openlcb_msg_fifo.list[iIndex] = (void*) 0;
        outgoing_openlcb_msg_fifo.list[iIndex] = (void*) 0;

    };

    incoming_openlcb_msg_fifo.head = 0;
    incoming_openlcb_msg_fifo.tail = 0;

    outgoing_openlcb_msg_fifo.head = 0;
    outgoing_openlcb_msg_fifo.tail = 0;

};


