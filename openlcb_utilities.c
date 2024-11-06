/*
 * File:   openlcb_utilities.c
 * Author: jimkueneman
 *
 * Created on November 6, 2024, 5:44 AM
 */


#include "xc.h"
#include "buffers.h"
#include "openlcb_defines.h"

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

    uint64_t result = 0;

    if ((msg->state.data_struct_size == ID_DATA_SIZE_BASIC) && (msg->payload_count == 6)) {

        uint64_t x = ((uint64_t) ((*(payload_basic_t*) msg->payload_ptr)[0]));
        result = result | x << 40;
        x = ((uint64_t) ((*(payload_basic_t*) msg->payload_ptr)[1]));
        result = result | x << 32;
        x = ((uint64_t) ((*(payload_basic_t*) msg->payload_ptr)[2]));
        result = result | x << 24;
        x = ((uint64_t) ((*(payload_basic_t*) msg->payload_ptr)[3]));
        result = result | x << 16;
        x = ((uint64_t) ((*(payload_basic_t*) msg->payload_ptr)[4]));
        result = result | x << 8;
        x = ((uint64_t) ((*(payload_basic_t*) msg->payload_ptr)[5]));
        result = result | x;

    }

    return result;

}

uint64_t CAN_PayloadToNodeID(payload_bytes_can_t* payload) {

    uint64_t result = 0;

    uint64_t x = (uint64_t) (*payload)[0];
    result = result | x << 40;
    x = (uint64_t) (*payload)[1];
    result = result | x << 32;
    x = (uint64_t) (*payload)[2];
    result = result | x << 24;
    x = (uint64_t) (*payload)[3];
    result = result | x << 16;
    x = (uint64_t) (*payload)[4];
    result = result | x << 8;
    x = (uint64_t) (*payload)[5];
    result = result | x;

    return result;

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