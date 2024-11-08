/*
 * File:   can_common_statemachine.c
 * Author: jimkueneman
 *
 * Created on November 7, 2024, 3:12 PM
 */


#include "xc.h"
#include "node.h"
#include "openlcb_buffers.h"
#include "can_buffers.h"
#include "stdio.h"  // printf
#include "openlcb_utilities.h"
#include "openlcb_defines.h"


uint8_t CopyData_CAN_Buffer_To_OpenLcbMsg(openlcb_msg_t* openlcb_msg_ptr, can_msg_t* ecan_msg_ptr, uint8_t start_index, uint8_t append) {

    uint8_t result = 0;

    if (openlcb_msg_ptr && ecan_msg_ptr) {

        int iMsgPayload = 0;

        if (append)
            iMsgPayload = openlcb_msg_ptr->payload_count;
        else
            openlcb_msg_ptr->payload_count = 0;


        for (int iCAN_Payload = start_index; iCAN_Payload < ecan_msg_ptr->payload_size; iCAN_Payload++) {

            switch (openlcb_msg_ptr->state.data_struct_size) {

                case ID_DATA_SIZE_BASIC:

                    if (iMsgPayload < LEN_DATA_BASIC) {

                        (*((payload_basic_t*) openlcb_msg_ptr->payload_ptr)) [iMsgPayload] = ecan_msg_ptr->payload[iCAN_Payload];
                        openlcb_msg_ptr->payload_count = openlcb_msg_ptr->payload_count + 1;
                    };

                    break;
                case ID_DATA_SIZE_DATAGRAM:

                    if (iMsgPayload < LEN_DATA_DATAGRAM) {

                        (*((payload_datagram_t*) openlcb_msg_ptr->payload_ptr))[iMsgPayload] = ecan_msg_ptr->payload[iCAN_Payload];
                        openlcb_msg_ptr->payload_count = openlcb_msg_ptr->payload_count + 1;
                    }

                    break;
                case ID_DATA_SIZE_STREAM_SNIP:

                    if (iMsgPayload < LEN_DATA_STREAM_SNIP) {

                        (*((payload_stream_snip_t*) openlcb_msg_ptr->payload_ptr))[iMsgPayload] = ecan_msg_ptr->payload[iCAN_Payload];
                        openlcb_msg_ptr->payload_count = openlcb_msg_ptr->payload_count + 1;
                    }

                    break;

            };

            iMsgPayload = iMsgPayload + 1;
            result = result + 1;

        }

    };

    return result;
}

void TestForAliasConflict(can_msg_t* msg, uint32_t can_control_msg, uint8_t add_payload_node_id) {

    can_msg_t out_msg;

    for (int iIndex = 0; iIndex < LEN_NODE_ARRAY; iIndex++) {

        if (nodes.node[iIndex].state.permitted) {

            if ((nodes.node[iIndex].alias == (msg->identifier & 0xFFF))) { // Check if we own this Alias
             
                if (add_payload_node_id)
                    
                    CopyNodeIDToCANBuffer(&out_msg, nodes.node[iIndex].id);
                
                else
                    out_msg.payload_size = 0;

                out_msg.identifier = RESERVED_TOP_BIT | can_control_msg | nodes.node[iIndex].alias;

                Push_CAN_Frame_Message(&out_msg, TRUE);

                break;

            }

        }
        
    }

}
