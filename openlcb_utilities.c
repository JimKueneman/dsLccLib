/*
 * File:   openlcb_utilities.c
 * Author: jimkueneman
 *
 * Created on November 6, 2024, 5:44 AM
 */


#include "xc.h"
#include "buffers.h"
#include "openlcb_defines.h"
#include "debug.h"

void CopyNodeIDToMessage(openlcb_msg_t* msg, uint64_t node_id) {

    if (msg->state.data_struct_size == ID_DATA_SIZE_BASIC) {

        msg->payload_count = 6;

        for (int iIndex = 5; iIndex > -1; iIndex--) {
            (*(payload_basic_t*) (msg->payload_ptr))[iIndex] = node_id & 0xFF;
            node_id = node_id >> 8;
        }

    }

}

void CopyNodeIDToCANBuffer(ecan_msg_t* buffer, uint64_t node_id) {

    buffer->payload_size = 6;

    for (int iIndex = 5; iIndex > -1; iIndex--) {
        buffer->payload[iIndex] = node_id & 0xFF;
        node_id = node_id >> 8;
    }

}

uint64_t MessageDataToNodeID(openlcb_msg_t* msg) {
    
    return ((uint64_t) ((*(payload_basic_t*) msg->payload_ptr)[0]) << 40) | 
           ((uint64_t) ((*(payload_basic_t*) msg->payload_ptr)[1]) << 32) | 
           ((uint64_t) ((*(payload_basic_t*) msg->payload_ptr)[2]) << 24) | 
           ((uint64_t) ((*(payload_basic_t*) msg->payload_ptr)[3]) << 16) | 
           ((uint64_t) ((*(payload_basic_t*) msg->payload_ptr)[4]) << 8)  |
           ((uint64_t) ((*(payload_basic_t*) msg->payload_ptr)[5]));

}

uint64_t CAN_PayloadToNodeID(payload_bytes_can_t* payload) {

    return ((uint64_t) (*payload)[0] << 40) | 
           ((uint64_t) (*payload)[1] << 32) | 
           ((uint64_t) (*payload)[2] << 24) | 
           ((uint64_t) (*payload)[3] << 16) | 
           ((uint64_t) (*payload)[4] << 8)  |
           ((uint64_t) (*payload)[5]);

}

void CAN_CopyBuffers(uint8_t start, uint8_t count, payload_bytes_can_t* payload_source, payload_bytes_can_t* payload_target) {

    for (int iIndex = start; iIndex < count; iIndex++)
        *payload_target[iIndex] = *payload_source[iIndex];

}

uint8_t EqualBuffers(openlcb_msg_t* msg1, openlcb_msg_t* msg2) {

    if ((msg1->payload_count == msg2->payload_count) && (msg1->state.data_struct_size == msg2->state.data_struct_size)) {

        for (int iIndex = 0; iIndex < msg1->payload_count; iIndex++) {
            if ((*(payload_basic_t*) (msg1->payload_ptr))[iIndex] != (*(payload_basic_t*) (msg2->payload_ptr))[iIndex]) {

                return FALSE;

            }
        }

        return TRUE;
    }

    return FALSE;
}