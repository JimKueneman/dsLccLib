/*
 * File:   engine_can_message.c
 * Author: jimkueneman
 *
 * Created on February 7, 2024, 6:03 AM
 * 
 */


#include "xc.h"
#include "openlcb_buffers.h"
#include "mcu_driver.h"
#include "openlcb_defines.h"
#include "debug.h"
#include "openlcb_buffers.h"
#include "can_common_statemachine.h"
#include "stdio.h" // printf
#include "node.h" 
#include "openlcb_utilities.h"
#include "can_buffers.h"


void Initialize_CAN_Incoming_StateMachine() {

    
    
}

openlcb_msg_t* AllocateAndStoreToFIFO(openlcb_msg_buffer_t* list_ptr, uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t can_mti, can_msg_t* ecan_msg, uint8_t payload_start, uint8_t data_size) {

    openlcb_msg_t* result = Allocate_OpenLcb_Msg(source_alias, source_id, dest_alias, dest_id, can_mti, data_size, FALSE);

    if (result) {

        result = Push_OpenLcb_Message(list_ptr, result, FALSE);

        if (result)

            CopyData_CAN_Buffer_To_OpenLcbMsg(result, ecan_msg, payload_start, APPEND_FALSE);

        else {

            // TODO: Send Error  TALKING POINT:  IF THE BUFFERS ARE THE SAME SIZE AS THE NUMBER OF MESSAGE SLOTS THIS CAN NOT FAIL AND NOT POINT IN CHECKING.....

            printf("  fail store: AllocateAndStoreToFIFO\n");
        }

    } else {

        // TODO: Send Error 

        printf("  fail allocate: AllocateAndStoreToFIFO\n");

    }

    return result;

};

openlcb_msg_t* AllocateAndStoreToOpenLcbMsg(inprocess_buffer_t* list_ptr, uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t can_mti, can_msg_t* ecan_msg, uint8_t payload_start, uint8_t data_size) {

    openlcb_msg_t* result = Allocate_OpenLcb_Msg(source_alias, source_id, dest_alias, dest_id, can_mti, data_size, FALSE);

    if (result) {

        result = Insert_OpenLcb_Message(list_ptr, result, FALSE);

        if (result)

            CopyData_CAN_Buffer_To_OpenLcbMsg(result, ecan_msg, payload_start, APPEND_FALSE);

        else {

            // TODO: Send Error   TALKING POINT:  IF THE BUFFERS ARE THE SAME SIZE AS THE NUMBER OF MESSAGE SLOTS THIS CAN NOT FAIL AND NOT POINT IN CHECKING.....

            printf("  fail store: AllocateAndStoreToFIFO\n");
        }

    } else {

        // TODO: Send Error 

        printf("  fail allocate: AllocateAndStoreToFIFO\n");

    }

    return result;

};

openlcb_msg_t* HandleIncoming_CAN_FirstFrame(inprocess_buffer_t* buffer_ptr, uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t mti, can_msg_t* ecan_msg, uint8_t payload_start, uint8_t data_size) {

    openlcb_msg_t* result = Find_OpenLcb_Message_As_Buffer(buffer_ptr, source_alias, source_id, dest_alias, dest_id, mti, FALSE, FALSE);

    if (!result) {

        result = AllocateAndStoreToOpenLcbMsg(buffer_ptr, source_alias, source_id, dest_alias, dest_id, mti, ecan_msg, payload_start, data_size);

        if (!result) {


            // TODO Send Error No Buffer

            printf("  fail first frame: allocate\n");

            result = (void*) 0;
        }

    } else {

        // TODO: Send error, can't interleave the same message from the same node.  No way to resolve them (Streams you can as it has an ID)

        printf("  fail first frame: find\n");

    };

    return result;

}

openlcb_msg_t* HandleIncoming_CAN_MiddleFrame(inprocess_buffer_t* buffer_ptr, uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t mti, can_msg_t* ecan_msg, uint8_t payload_start) {

    openlcb_msg_t* result = Find_OpenLcb_Message_As_Buffer(buffer_ptr, source_alias, source_id, dest_alias, dest_id, mti, FALSE, FALSE);

    if (result) {

        CopyData_CAN_Buffer_To_OpenLcbMsg(result, ecan_msg, payload_start, APPEND_TRUE);

    } else {

        // TODO: Send error, can't interleave the same message from the same node.  No way to resolve them (Streams you can as it has an ID)

        printf("  fail Middle frame\n");
    };

    return result;

}

openlcb_msg_t* HandleIncoming_CAN_LastFrame(inprocess_buffer_t* buffer_ptr, uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t mti, can_msg_t* ecan_msg, uint8_t payload_start) {

    openlcb_msg_t* result = Find_OpenLcb_Message_As_Buffer(buffer_ptr, source_alias, source_id, dest_alias, dest_id, mti, FALSE, TRUE);

    if (result) {

        CopyData_CAN_Buffer_To_OpenLcbMsg(result, ecan_msg, payload_start, APPEND_TRUE);

        if (!Push_OpenLcb_Message(&incoming_openlcb_msg_fifo, result, FALSE)) {

            Release_OpenLcb_Msg(result, FALSE);

            // TODO Send Error

            printf("  fail CAN LastFrame - push\n");

        }

    } else {

        // TODO Send Error

        printf("  fail CAN LastFrame - find\n");

    }

    return result;

}

void HandleIncoming_CAN_LegacySNIP(inprocess_buffer_t* buffer_ptr, uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t mti, can_msg_t* ecan_msg) {

    // Early implementations did not have the multi-frame bits to use... special case


    openlcb_msg_t* openlcb_msg_inprocess = Find_OpenLcb_Message_As_Buffer(buffer_ptr, source_alias, source_id, dest_alias, dest_id, mti, FALSE, FALSE);


    if (!openlcb_msg_inprocess) { // Do we have one in process?

        HandleIncoming_CAN_FirstFrame(buffer_ptr, source_alias, source_id, dest_alias, source_id, mti, ecan_msg, 2, ID_DATA_SIZE_STREAM_SNIP);

    } else { // Yes we have one in process   

        // TODO: we can't tell what is the last frame other than counting how many nulls were in the data stream and look for the 6th null byte

        uint8_t null_count = 0;
        uint8_t next_null_count = 0;

        for (int i = 0; i < openlcb_msg_inprocess->payload_count; i++) {

            if ((*((payload_stream_snip_t*) openlcb_msg_inprocess->payload_ptr))[i] == 0x00) {

                null_count = null_count + 1;

            }

        };

        for (int i = 0; i < ecan_msg->payload_size; i++) {

            if (ecan_msg->payload[i] == 0x00)

                next_null_count = next_null_count + 1;

        };

        if (null_count + next_null_count < 6)

            HandleIncoming_CAN_MiddleFrame(buffer_ptr, source_alias, source_id, dest_alias, dest_id, mti, ecan_msg, 2);
        else

            HandleIncoming_CAN_LastFrame(buffer_ptr, source_alias, source_id, dest_alias, dest_id, mti, ecan_msg, 2);

    };

    return;

}

// Handled from within a CAN Rx Interrupt
//

void HandleIncoming_CAN_Control_Frame(can_msg_t* msg) {

    can_msg_t out_msg;

    if (msg->identifier & MASK_CAN_FRAME_SEQUENCE_NUMBER) {

        // CIDn Frames.  Not sure they are interesting at a small node level
        //   for now we will just swallow them..  We may be able to reply early that
        //   someone is stomping on an allocated Node ID range but it is just as easy to 
        //   wait for the RID frame complain.

        switch (msg->identifier & MASK_CAN_FRAME_SEQUENCE_NUMBER) {

            case CAN_CONTROL_FRAME_CID7:
            case CAN_CONTROL_FRAME_CID6:
            case CAN_CONTROL_FRAME_CID5:
            case CAN_CONTROL_FRAME_CID4:

                printf("CID\n");

                TestForAliasConflict(msg, CAN_CONTROL_FRAME_RID, FALSE);

                break;
            case CAN_CONTROL_FRAME_CID3:

                break;
            case CAN_CONTROL_FRAME_CID2:

                break;
            case CAN_CONTROL_FRAME_CID1:

                break;
        }

    } else {

        switch (msg->identifier & MASK_CAN_VARIABLE_FIELD) {

            case CAN_CONTROL_FRAME_RID: // Reserve ID


                // Node has come on line and is taking this Node ID and Alias
                // Check and make sure it is not a duplicate Node ID and if so we need to follow 6.2.5 in the CAN Transport Spec

                break;
            case CAN_CONTROL_FRAME_AMD: // Alias Map Definition

                // Reply from an Alias Mapping Enquiry (AME) with a Node ID and Alias Mapping
                // Check and make sure it is not a duplicate Node ID and if so we need to follow 6.2.5 in the CAN Transport Spec

                printf("AMD\n");

                TestForAliasConflict(msg, CAN_CONTROL_FRAME_AMR, TRUE);

                break;
            case CAN_CONTROL_FRAME_AME:

                // Someone is requesting we reply with Alias Mapping Definitions for our Node(s)

                for (int iIndex = 0; iIndex < LEN_NODE_ARRAY; iIndex++) {

                    if (nodes.node[iIndex].state.permitted) {

                        if ((msg->payload_size == 0) || (CAN_PayloadToNodeID(&msg->payload) == nodes.node[iIndex].id)) {

                            CopyNodeIDToCANBuffer(&out_msg, nodes.node[iIndex].id);
                            out_msg.identifier = RESERVED_TOP_BIT | CAN_CONTROL_FRAME_AMD | nodes.node[iIndex].alias;

                            Push_CAN_Message(&out_msg, TRUE);

                        }
                    }
                }

                break;
            case CAN_CONTROL_FRAME_AMR:

                // The Alias and Node ID passed are not longer associated and the Alias is effectively "released" and can be reused

                printf("AMr\n");

                break;
            case CAN_CONTROL_FRAME_ERROR_INFO_REPORT_0:
                // Advanced feature for gateways/routers/etc.
                break;
            case CAN_CONTROL_FRAME_ERROR_INFO_REPORT_1:
                // Advanced feature for gateways/routers/etc.
                break;
            case CAN_CONTROL_FRAME_ERROR_INFO_REPORT_2:
                // Advanced feature for gateways/routers/etc.
                break;
            case CAN_CONTROL_FRAME_ERROR_INFO_REPORT_3:
                // Advanced feature for gateways/routers/etc.
                break;

        }

    }

}

void Statemachine_Incoming_CAN(can_msg_t* msg) {

    uint16_t source_alias = msg->identifier & 0x00000FFF;
    uint16_t dest_alias = (msg->identifier & 0x00FFF000) >> 12; // Make this assumption up front.  Correct for Datagrams (steams?)
    uint16_t can_mti = dest_alias; // These 12 bits are either the Dest Alias (Datagram/Stream?) or the 12 bit CAN MTI (Most other messages)

    // Only handle OpenLCB Messages
    if ((msg->identifier & CAN_OPENLCB_MSG) == CAN_OPENLCB_MSG) {

        switch (msg->identifier & MASK_CAN_FRAME_TYPE) {

            case CAN_FRAME_TYPE_GLOBAL_ADDRESSED:

                if (msg->identifier & MASK_CAN_DEST_ADDRESS_PRESENT) {

                    // TODO:  Eventually we should only process this if the message is addressed to us to save bandwidth internally...

                    // Destination Alias carried in the payload bytes on these messages
                    dest_alias = (uint16_t) (msg->payload[0] << 8) | (uint16_t) msg->payload[1]; // Pull the destination address and embedded framing bit out of the data                   
                    uint8_t framing_bits = dest_alias & 0xF000; // Extract the framing bits of the top nibble
                    dest_alias = dest_alias & 0x0FFF; // Strip off the framing bits from the destination alias

                    switch (framing_bits) {

                        case MULTIFRAME_ONLY:

                            if (can_mti == MTI_SIMPLE_NODE_INFO_REPLY)

                                HandleIncoming_CAN_LegacySNIP(&incoming_openlcb_inprocess_msg_list, source_alias, 0, dest_alias, 0, can_mti, msg);

                            else

                                AllocateAndStoreToFIFO(&incoming_openlcb_msg_fifo, source_alias, 0, dest_alias, 0, can_mti, msg, 0, ID_DATA_SIZE_BASIC);

                            break;

                        case MULTIFRAME_FIRST:

                            // TODO: This could be dangerous if a future message used more than 2 frames.... (larger than ID_DATA_SIZE_BASIC)

                            HandleIncoming_CAN_FirstFrame(&incoming_openlcb_inprocess_msg_list, source_alias, 0, dest_alias, 0, can_mti, msg, 2, ID_DATA_SIZE_BASIC);

                            break;

                        case MULTIFRAME_MIDDLE:

                            HandleIncoming_CAN_MiddleFrame(&incoming_openlcb_inprocess_msg_list, source_alias, 0, dest_alias, 0, can_mti, msg, 2);

                            break;

                        case MULTIFRAME_FINAL:

                            HandleIncoming_CAN_LastFrame(&incoming_openlcb_inprocess_msg_list, source_alias, 0, dest_alias, 0, can_mti, msg, 2);

                            break;
                    }
                } else { // No Destination Address

                    AllocateAndStoreToFIFO(&incoming_openlcb_msg_fifo, source_alias, 0, dest_alias, 0, can_mti, msg, 0, ID_DATA_SIZE_BASIC);

                };

                break;
            case CAN_FRAME_TYPE_DATAGRAM_ONLY:

                AllocateAndStoreToFIFO(&incoming_openlcb_msg_fifo, source_alias, 0, dest_alias, 0, MTI_DATAGRAM, msg, 0, ID_DATA_SIZE_BASIC);
                break;

            case CAN_FRAME_TYPE_DATAGRAM_FIRST:

                HandleIncoming_CAN_FirstFrame(&incoming_openlcb_inprocess_msg_list, source_alias, 0, dest_alias, 0, MTI_DATAGRAM, msg, 0, ID_DATA_SIZE_DATAGRAM);
                break;

            case CAN_FRAME_TYPE_DATAGRAM_MIDDLE:

                HandleIncoming_CAN_MiddleFrame(&incoming_openlcb_inprocess_msg_list, source_alias, 0, dest_alias, 0, MTI_DATAGRAM, msg, 0);
                break;

            case CAN_FRAME_TYPE_DATAGRAM_FINAL:

                HandleIncoming_CAN_LastFrame(&incoming_openlcb_inprocess_msg_list, source_alias, 0, dest_alias, 0, MTI_DATAGRAM, msg, 0);
                break;

            case CAN_FRAME_TYPE_RESERVED:

                break;
            case CAN_FRAME_TYPE_STREAM:

                break;
        }

        // CAN Control Messages
    } else {

        HandleIncoming_CAN_Control_Frame(msg);

    };

};

