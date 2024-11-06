/*
 * File:   engine_can_message.c
 * Author: jimkueneman
 *
 * Created on February 7, 2024, 6:03 AM
 * 
 */


#include "xc.h"
#include "buffers.h"
#include "mcu_drv.h"
#include "openlcb_defines.h"
#include "debug.h"
#include "buffers.h"
#include "can_message_handler.h"
#include "stdio.h" // printf
#include "node.h" 
#include "openlcb_utilities.h"


#define STORAGE_PUSH 0  // Treat the list like a FIFO
#define STORAGE_INSERT  1  // Threat the list an array

#define APPEND_TRUE  TRUE
#define APPEND_FALSE FALSE

void Initialize_CAN_MessageHandler() {

}

uint8_t CopyData_CAN_Buffer_To_OpenLcbMsg(openlcb_msg_t* openlcb_msg_ptr, ecan_msg_t* ecan_msg_ptr, uint8_t start_index, uint8_t append) {

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

                        (*((payload_basic_ptr) openlcb_msg_ptr->payload_ptr)) [iMsgPayload] = ecan_msg_ptr->payload[iCAN_Payload];
                        openlcb_msg_ptr->payload_count = openlcb_msg_ptr->payload_count + 1;
                    };

                    break;
                case ID_DATA_SIZE_DATAGRAM:

                    if (iMsgPayload < LEN_DATA_DATAGRAM) {

                        (*((payload_datagram_ptr) openlcb_msg_ptr->payload_ptr))[iMsgPayload] = ecan_msg_ptr->payload[iCAN_Payload];
                        openlcb_msg_ptr->payload_count = openlcb_msg_ptr->payload_count + 1;
                    }

                    break;
                case ID_DATA_SIZE_STREAM_SNIP:

                    if (iMsgPayload < LEN_DATA_STREAM_SNIP) {

                        (*((payload_stream_snip_ptr) openlcb_msg_ptr->payload_ptr))[iMsgPayload] = ecan_msg_ptr->payload[iCAN_Payload];
                        openlcb_msg_ptr->payload_count = openlcb_msg_ptr->payload_count + 1;
                    }

                    break;

            };

            iMsgPayload = iMsgPayload + 1;
            result = result + 1;

        }

    };

    return result;
};

openlcb_msg_t* AllocateAndStoreToList(openlcb_msg_buffer_t* list_ptr, uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t can_mti, ecan_msg_t* ecan_msg, uint8_t payload_start, uint8_t data_size, uint8_t storage_type) {

    openlcb_msg_t* result = Allocate_OpenLcb_Msg(source_alias, source_id, dest_alias, dest_id, can_mti, data_size, FALSE);

    if (result) {

        switch (storage_type) {

            case STORAGE_PUSH:

                result = Push_OpenLcb_Message(list_ptr, result, FALSE);

                break;

            case STORAGE_INSERT:

                result = Insert_OpenLcb_Message(list_ptr, result, FALSE);

                break;
        }

    };

    if (result) {

        CopyData_CAN_Buffer_To_OpenLcbMsg(result, ecan_msg, payload_start, APPEND_FALSE);

    } else {

        // TODO: Send Error 

        printf("  fail allocate and store: allocate\n");

    }

    return result;

};

openlcb_msg_t* HandleIncomingCAN_FirstFrame(openlcb_msg_buffer_t* buffer_ptr, uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t mti, ecan_msg_t* ecan_msg, uint8_t payload_start, uint8_t data_size) {

    openlcb_msg_t* result = Find_OpenLcb_Message_As_Buffer(buffer_ptr, source_alias, source_id, dest_alias, dest_id, mti, FALSE, FALSE);

    if (!result) {

        result = AllocateAndStoreToList(buffer_ptr, source_alias, source_id, dest_alias, dest_id, mti, ecan_msg, payload_start, data_size, STORAGE_INSERT);

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

openlcb_msg_t* HandleIncomingCAN_MiddleFrame(openlcb_msg_buffer_t* buffer_ptr, uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t mti, ecan_msg_t* ecan_msg, uint8_t payload_start) {

    openlcb_msg_t* result = Find_OpenLcb_Message_As_Buffer(buffer_ptr, source_alias, source_id, dest_alias, dest_id, mti, FALSE, FALSE);

    if (result) {

        CopyData_CAN_Buffer_To_OpenLcbMsg(result, ecan_msg, payload_start, APPEND_TRUE);

    } else {

        // TODO: Send error, can't interleave the same message from the same node.  No way to resolve them (Streams you can as it has an ID)

        printf("  fail Middle frame\n");
    };

    return result;

}

openlcb_msg_t* HandleIncomingCAN_LastFrame(openlcb_msg_buffer_t* buffer_ptr, uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t mti, ecan_msg_t* ecan_msg, uint8_t payload_start) {

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

void HandleIncomingLegacySNIP(openlcb_msg_buffer_t* buffer_ptr, uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t mti, ecan_msg_t* ecan_msg) {

    // Early implementations did not have the multi-frame bits to use... special case


    openlcb_msg_t* openlcb_msg_inprocess = Find_OpenLcb_Message_As_Buffer(buffer_ptr, source_alias, source_id, dest_alias, dest_id, mti, FALSE, FALSE);


    if (!openlcb_msg_inprocess) { // Do we have one in process?

        HandleIncomingCAN_FirstFrame(buffer_ptr, source_alias, source_id, dest_alias, source_id, mti, ecan_msg, 2, ID_DATA_SIZE_STREAM_SNIP);

    } else { // Yes we have one in process   

        // TODO: we can't tell what is the last frame other than counting how many nulls were in the data stream and look for the 6th null byte

        uint8_t null_count = 0;
        uint8_t next_null_count = 0;

        for (int i = 0; i < openlcb_msg_inprocess->payload_count; i++) {

            if ((*((payload_stream_snip_ptr) openlcb_msg_inprocess->payload_ptr))[i] == 0x00) {

                null_count = null_count + 1;

            }

        };

        for (int i = 0; i < ecan_msg->payload_size; i++) {

            if (ecan_msg->payload[i] == 0x00)

                next_null_count = next_null_count + 1;

        };

        if (null_count + next_null_count < 6)

            HandleIncomingCAN_MiddleFrame(buffer_ptr, source_alias, source_id, dest_alias, dest_id, mti, ecan_msg, 2);
        else

            HandleIncomingCAN_LastFrame(buffer_ptr, source_alias, source_id, dest_alias, dest_id, mti, ecan_msg, 2);

    };

    return;

}

void TestForAliasConflict(ecan_msg_t* msg, uint32_t can_control_msg, uint8_t add_payload_node_id) {

    ecan_msg_t out_msg;

    for (int iIndex = 0; iIndex < LEN_NODE_ARRAY; iIndex++) {

        if (nodes.node[iIndex].state.permitted) {

            printf("node: %d\n", iIndex);

            if ((nodes.node[iIndex].alias == (msg->identifier & 0xFFF))) { // Check if we own this Alias

                printf("Found\n");
                if (add_payload_node_id)
                    CopyNodeIDToCANBuffer(&out_msg, nodes.node[iIndex].id);
                else
                    out_msg.payload_size = 0;

                out_msg.identifier = RESERVED_TOP_BIT | can_control_msg | nodes.node[iIndex].alias;
                Send_Raw_CAN_Message(TX_CHANNEL_OPENLCB_MSG, &out_msg, TRUE);

                break;

            }

        }
    }

}

// Handled from within a CAN Rx Interrupt
//

void HandleIncoming_CAN_Control_Frame(ecan_msg_t* msg) {

    ecan_msg_t out_msg;

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
                            Send_Raw_CAN_Message(TX_CHANNEL_OPENLCB_MSG, &out_msg, TRUE);

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

void HandleIncomingCAN_Msg(ecan_msg_t* msg) {

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

                                HandleIncomingLegacySNIP(&incoming_openlcb_inprocess_msg_list, source_alias, 0, dest_alias, 0, can_mti, msg);

                            else

                                AllocateAndStoreToList(&incoming_openlcb_msg_fifo, source_alias, 0, dest_alias, 0, can_mti, msg, 0, ID_DATA_SIZE_BASIC, STORAGE_PUSH);

                            break;

                        case MULTIFRAME_FIRST:

                            // TODO: This could be dangerous if a future message used more than 2 frames.... (larger than ID_DATA_SIZE_BASIC)

                            HandleIncomingCAN_FirstFrame(&incoming_openlcb_inprocess_msg_list, source_alias, 0, dest_alias, 0, can_mti, msg, 2, ID_DATA_SIZE_BASIC);

                            break;

                        case MULTIFRAME_MIDDLE:

                            HandleIncomingCAN_MiddleFrame(&incoming_openlcb_inprocess_msg_list, source_alias, 0, dest_alias, 0, can_mti, msg, 2);

                            break;

                        case MULTIFRAME_FINAL:

                            HandleIncomingCAN_LastFrame(&incoming_openlcb_inprocess_msg_list, source_alias, 0, dest_alias, 0, can_mti, msg, 2);

                            break;
                    }
                } else { // No Destination Address

                    AllocateAndStoreToList(&incoming_openlcb_msg_fifo, source_alias, 0, dest_alias, 0, can_mti, msg, 0, ID_DATA_SIZE_BASIC, STORAGE_PUSH);

                };

                break;
            case CAN_FRAME_TYPE_DATAGRAM_ONLY:

                AllocateAndStoreToList(&incoming_openlcb_msg_fifo, source_alias, 0, dest_alias, 0, MTI_DATAGRAM, msg, 0, ID_DATA_SIZE_BASIC, STORAGE_PUSH);
                break;

            case CAN_FRAME_TYPE_DATAGRAM_FIRST:

                HandleIncomingCAN_FirstFrame(&incoming_openlcb_inprocess_msg_list, source_alias, 0, dest_alias, 0, MTI_DATAGRAM, msg, 0, ID_DATA_SIZE_DATAGRAM);
                break;

            case CAN_FRAME_TYPE_DATAGRAM_MIDDLE:

                HandleIncomingCAN_MiddleFrame(&incoming_openlcb_inprocess_msg_list, source_alias, 0, dest_alias, 0, MTI_DATAGRAM, msg, 0);
                break;

            case CAN_FRAME_TYPE_DATAGRAM_FINAL:

                HandleIncomingCAN_LastFrame(&incoming_openlcb_inprocess_msg_list, source_alias, 0, dest_alias, 0, MTI_DATAGRAM, msg, 0);
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

uint8_t Send_Raw_CAN_Message(uint8_t tx_channel, ecan_msg_t* msg, uint8_t block) {

    if (block) {
        can_send_timeout = 0;
        while (!Ecan1TxBufferClear(tx_channel)) {

            if (can_send_timeout >= 1500) {
                Ecan1TxBufferSetTransmit(tx_channel, FALSE);
                can_send_timeout = 0;
                break;

            }

        };

        Ecan1WriteTxMsgBufId(tx_channel, msg->identifier, TRUE, FALSE);
        Ecan1WriteTxMsgBufData(tx_channel, msg->payload_size, &msg->payload);
        Ecan1TxBufferSetTransmit(tx_channel, TRUE);

        return TRUE;

    } else {

        if (Ecan1TxBufferClear(tx_channel)) {

            Ecan1WriteTxMsgBufId(tx_channel, msg->identifier, TRUE, FALSE);
            Ecan1WriteTxMsgBufData(tx_channel, msg->payload_size, &msg->payload);
            Ecan1TxBufferSetTransmit(tx_channel, TRUE);

            return TRUE;
        }

    }

    return FALSE;

}

openlcb_msg_t* outgoing_msg = (void*) 0;
uint16_t outgoing_msg_index;
payload_bytes_can_t can_data;
uint8_t last_frame;

uint8_t Outgoing_CAN_BufferEmpty() {

    if (outgoing_msg) {
        return FALSE;
    } else {
        return TRUE;
    }

}

uint8_t LoadOutgoing_CAN_Buffer(openlcb_msg_t* msg) {

    uint8_t result = FALSE;

    if (!outgoing_msg) {

        outgoing_msg = msg;
        outgoing_msg_index = 0;
        last_frame = FALSE;

        // Kick it off.
        HandleOutgoingCAN_Msg(FALSE);

        result = TRUE;

    }

    return result;
}


// CAN TX Interrupt will call this if it is a multi-frame message to keep it pumping..

uint8_t HandleOutgoingCAN_Msg(uint8_t called_from_interrupt) {

    uint8_t result = FALSE;

    if (!outgoing_msg)
        return result;

    uint32_t identifier = 0;

    if ((outgoing_msg->mti & MASK_DEST_ADDRESS_PRESENT) == MASK_DEST_ADDRESS_PRESENT) { // Addressed Message

        switch (outgoing_msg->mti) {

            case MTI_DATAGRAM:
            {

                uint8_t iIndex = 0;

                while ((iIndex < 8) && (outgoing_msg_index < outgoing_msg->payload_count)) {

                    can_data[iIndex] = (*(payload_datagram_t*) outgoing_msg->payload_ptr)[outgoing_msg_index];
                    iIndex = iIndex + 1;
                    outgoing_msg_index = outgoing_msg_index + 1;

                }

                if (outgoing_msg_index <= 8) {

                    if (outgoing_msg->payload_count <= 8) {
                        identifier = RESERVED_TOP_BIT | CAN_OPENLCB_MSG | CAN_FRAME_TYPE_DATAGRAM_ONLY | (outgoing_msg->dest_alias << 12) | outgoing_msg->source_alias;
                        last_frame = TRUE;
                    } else
                        identifier = RESERVED_TOP_BIT | CAN_OPENLCB_MSG | CAN_FRAME_TYPE_DATAGRAM_FIRST | (outgoing_msg->dest_alias << 12) | outgoing_msg->source_alias;

                } else if (outgoing_msg_index < outgoing_msg->payload_count)

                    identifier = RESERVED_TOP_BIT | CAN_OPENLCB_MSG | CAN_FRAME_TYPE_DATAGRAM_MIDDLE | (outgoing_msg->dest_alias << 12) | outgoing_msg->source_alias;

                else {

                    identifier = RESERVED_TOP_BIT | CAN_OPENLCB_MSG | CAN_FRAME_TYPE_DATAGRAM_FINAL | (outgoing_msg->dest_alias << 12) | outgoing_msg->source_alias;
                    last_frame = TRUE;

                }

                if (last_frame) {

                    Release_OpenLcb_Msg(outgoing_msg, called_from_interrupt);
                    // reset for the next message
                    outgoing_msg = (void*) 0;
                    outgoing_msg_index = 0;
                    last_frame = FALSE;

                }


                Ecan1WriteTxMsgBufId(TX_CHANNEL_OPENLCB_MSG, identifier, TRUE, FALSE);
                Ecan1WriteTxMsgBufData(TX_CHANNEL_OPENLCB_MSG, iIndex + 1, &can_data);

                Ecan1TxBufferSetTransmit(TX_CHANNEL_OPENLCB_MSG, TRUE);

                break;

            }

            case MTI_STREAM_COMPLETE:
            case MTI_STREAM_INIT_REPLY:
            case MTI_STREAM_INIT_REQUEST:
            case MTI_STREAM_PROCEED:
            {

            }

            default:

                // Only the Datagram and Stream carry the dest alias in special places,
                // every other message carries it in the first 2 bytes of the payload reducing
                // the capacity to 6


                can_data[0] = (outgoing_msg->dest_alias >> 8) & 0xFF;
                can_data[1] = outgoing_msg->dest_alias & 0xFF;

                uint8_t iIndex = 2;

                while ((iIndex <= 6) && (outgoing_msg_index < outgoing_msg->payload_count)) {

                    if (outgoing_msg->state.data_struct_size == ID_DATA_SIZE_BASIC)
                        can_data[iIndex] = (*(payload_basic_t*) outgoing_msg->payload_ptr)[outgoing_msg_index];
                    else
                        can_data[iIndex] = (*(payload_stream_snip_t*) outgoing_msg->payload_ptr)[outgoing_msg_index];

                    iIndex = iIndex + 1;
                    outgoing_msg_index = outgoing_msg_index + 1;

                }

                if (outgoing_msg_index <= 6) {

                    if (outgoing_msg->payload_count <= 6) {

                        can_data[0] = can_data[0] | MULTIFRAME_ONLY;
                        last_frame = TRUE;

                    } else
                        can_data[0] = can_data[0] | MULTIFRAME_FIRST;

                } else if (outgoing_msg_index < outgoing_msg->payload_count)

                    can_data[0] = can_data[0] | MULTIFRAME_MIDDLE;

                else {

                    can_data[0] = can_data[0] | MULTIFRAME_FINAL;
                    last_frame = TRUE;

                }

                identifier = RESERVED_TOP_BIT | CAN_OPENLCB_MSG | CAN_FRAME_TYPE_GLOBAL_ADDRESSED | ((uint32_t) (outgoing_msg->mti & 0x0FFF) << 12) | outgoing_msg->source_alias;


                if (last_frame) {

                    Release_OpenLcb_Msg(outgoing_msg, called_from_interrupt);
                    // reset for the next message
                    outgoing_msg = (void*) 0;
                    outgoing_msg_index = 0;
                    last_frame = FALSE;
                }


                Ecan1WriteTxMsgBufId(TX_CHANNEL_OPENLCB_MSG, identifier, TRUE, FALSE);
                Ecan1WriteTxMsgBufData(TX_CHANNEL_OPENLCB_MSG, iIndex, &can_data);

                Ecan1TxBufferSetTransmit(TX_CHANNEL_OPENLCB_MSG, TRUE);

                break;

        }


    } else { // Unaddressed message


        if (outgoing_msg->payload_count <= 8) { // single frame

            identifier = RESERVED_TOP_BIT | CAN_OPENLCB_MSG | CAN_FRAME_TYPE_GLOBAL_ADDRESSED | ((uint32_t) (outgoing_msg->mti & 0x0FFF) << 12) | outgoing_msg->source_alias;

            Ecan1WriteTxMsgBufId(TX_CHANNEL_OPENLCB_MSG, identifier, TRUE, FALSE);
            Ecan1WriteTxMsgBufData(TX_CHANNEL_OPENLCB_MSG, outgoing_msg->payload_count, (payload_bytes_can_t*) (outgoing_msg->payload_ptr));

            Ecan1TxBufferSetTransmit(TX_CHANNEL_OPENLCB_MSG, TRUE);

            Release_OpenLcb_Msg(outgoing_msg, called_from_interrupt);
            // reset for the next message
            outgoing_msg = (void*) 0;
            outgoing_msg_index = 0;

        } else { // multi frame

            // TODO Is there such a thing as a unaddressed multi frame?

        }

    }

    return result;

}

