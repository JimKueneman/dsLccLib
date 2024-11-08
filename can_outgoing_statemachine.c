/*
 * File:   can_outgoing_statemachine.c
 * Author: jimkueneman
 *
 * Created on November 7, 2024, 2:57 PM
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


openlcb_msg_t* outgoing_openlcb_msg = (void*) 0;
uint16_t outgoing_openlcb_msg_index = 0;
payload_bytes_can_t openlcb_msg_can_data;
uint8_t openlcb_last_frame = 0;

can_msg_t outgoing_can_frame_msg;

void Initialize_CAN_Outgoing_StateMachine() {

    outgoing_can_frame_msg.identifier = 0;

}

uint8_t Send_Raw_CAN_Message(uint8_t tx_channel, can_msg_t* msg, uint8_t block) {

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

uint8_t Outgoing_CAN_Msg_Buffer_Empty() {

    if ((outgoing_can_frame_msg.identifier == 0) && Ecan1TxBufferClear(TX_CHANNEL_CAN_CONTROL)) 
        return TRUE;
    else
        return FALSE;

}

uint8_t Load_Outgoing_CAN_Msg_Buffer(can_msg_t* msg) {

    if (outgoing_can_frame_msg.identifier == 0) {

        outgoing_can_frame_msg = *msg;

        return TRUE;
    }

    return FALSE;

}

uint8_t Outgoing_OpenLcb_Msg_Buffer_Empty() {

    if (outgoing_openlcb_msg && Ecan1TxBufferClear(TX_CHANNEL_OPENLCB_MSG)) {
        
        return FALSE;
        
    } else {
        
        return TRUE;
        
    }

}

uint8_t Load_Outgoing_OpenLcb_Msg_Buffer(openlcb_msg_t* msg) {

    uint8_t result = FALSE;

    if (!outgoing_openlcb_msg) {

        outgoing_openlcb_msg = msg;
        outgoing_openlcb_msg_index = 0;
        openlcb_last_frame = FALSE;
        
        result = TRUE;

    }

    return result;
}


// CAN TX Interrupt will call this if it is a multi-frame message to keep it pumping..

void Statemachine_Outgoing_CAN() {


    if (outgoing_can_frame_msg.identifier != 0) {

        
        Ecan1WriteTxMsgBufId(TX_CHANNEL_CAN_CONTROL, outgoing_can_frame_msg.identifier, TRUE, FALSE);
        Ecan1WriteTxMsgBufData(TX_CHANNEL_CAN_CONTROL, outgoing_can_frame_msg.payload_size, &outgoing_can_frame_msg.payload);
        Ecan1TxBufferSetTransmit(TX_CHANNEL_CAN_CONTROL, TRUE);
        
        outgoing_can_frame_msg.identifier = 0;

    }

    if (!outgoing_openlcb_msg)
        return;

    uint32_t identifier = 0;

    if ((outgoing_openlcb_msg->mti & MASK_DEST_ADDRESS_PRESENT) == MASK_DEST_ADDRESS_PRESENT) { // Addressed Message

        switch (outgoing_openlcb_msg->mti) {

            case MTI_DATAGRAM:
            {

                uint8_t iIndex = 0;

                while ((iIndex < 8) && (outgoing_openlcb_msg_index < outgoing_openlcb_msg->payload_count)) {

                    openlcb_msg_can_data[iIndex] = (*(payload_datagram_t*) outgoing_openlcb_msg->payload_ptr)[outgoing_openlcb_msg_index];
                    iIndex = iIndex + 1;
                    outgoing_openlcb_msg_index = outgoing_openlcb_msg_index + 1;

                }

                if (outgoing_openlcb_msg_index <= 8) {

                    if (outgoing_openlcb_msg->payload_count <= 8) {
                        identifier = RESERVED_TOP_BIT | CAN_OPENLCB_MSG | CAN_FRAME_TYPE_DATAGRAM_ONLY | (outgoing_openlcb_msg->dest_alias << 12) | outgoing_openlcb_msg->source_alias;
                        openlcb_last_frame = TRUE;
                    } else
                        identifier = RESERVED_TOP_BIT | CAN_OPENLCB_MSG | CAN_FRAME_TYPE_DATAGRAM_FIRST | (outgoing_openlcb_msg->dest_alias << 12) | outgoing_openlcb_msg->source_alias;

                } else if (outgoing_openlcb_msg_index < outgoing_openlcb_msg->payload_count)

                    identifier = RESERVED_TOP_BIT | CAN_OPENLCB_MSG | CAN_FRAME_TYPE_DATAGRAM_MIDDLE | (outgoing_openlcb_msg->dest_alias << 12) | outgoing_openlcb_msg->source_alias;

                else {

                    identifier = RESERVED_TOP_BIT | CAN_OPENLCB_MSG | CAN_FRAME_TYPE_DATAGRAM_FINAL | (outgoing_openlcb_msg->dest_alias << 12) | outgoing_openlcb_msg->source_alias;
                    openlcb_last_frame = TRUE;

                }

                if (openlcb_last_frame) {

                    Release_OpenLcb_Msg(outgoing_openlcb_msg, TRUE);
                    // reset for the next message
                    outgoing_openlcb_msg = (void*) 0;
                    outgoing_openlcb_msg_index = 0;
                    openlcb_last_frame = FALSE;

                }


                Ecan1WriteTxMsgBufId(TX_CHANNEL_OPENLCB_MSG, identifier, TRUE, FALSE);
                Ecan1WriteTxMsgBufData(TX_CHANNEL_OPENLCB_MSG, iIndex + 1, &openlcb_msg_can_data);

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


                openlcb_msg_can_data[0] = (outgoing_openlcb_msg->dest_alias >> 8) & 0xFF;
                openlcb_msg_can_data[1] = outgoing_openlcb_msg->dest_alias & 0xFF;

                uint8_t iIndex = 2;

                while ((iIndex <= 6) && (outgoing_openlcb_msg_index < outgoing_openlcb_msg->payload_count)) {

                    if (outgoing_openlcb_msg->state.data_struct_size == ID_DATA_SIZE_BASIC)
                        openlcb_msg_can_data[iIndex] = (*(payload_basic_t*) outgoing_openlcb_msg->payload_ptr)[outgoing_openlcb_msg_index];
                    else
                        openlcb_msg_can_data[iIndex] = (*(payload_stream_snip_t*) outgoing_openlcb_msg->payload_ptr)[outgoing_openlcb_msg_index];

                    iIndex = iIndex + 1;
                    outgoing_openlcb_msg_index = outgoing_openlcb_msg_index + 1;

                }

                if (outgoing_openlcb_msg_index <= 6) {

                    if (outgoing_openlcb_msg->payload_count <= 6) {

                        openlcb_msg_can_data[0] = openlcb_msg_can_data[0] | MULTIFRAME_ONLY;
                        openlcb_last_frame = TRUE;

                    } else
                        openlcb_msg_can_data[0] = openlcb_msg_can_data[0] | MULTIFRAME_FIRST;

                } else if (outgoing_openlcb_msg_index < outgoing_openlcb_msg->payload_count)

                    openlcb_msg_can_data[0] = openlcb_msg_can_data[0] | MULTIFRAME_MIDDLE;

                else {

                    openlcb_msg_can_data[0] = openlcb_msg_can_data[0] | MULTIFRAME_FINAL;
                    openlcb_last_frame = TRUE;

                }

                identifier = RESERVED_TOP_BIT | CAN_OPENLCB_MSG | CAN_FRAME_TYPE_GLOBAL_ADDRESSED | ((uint32_t) (outgoing_openlcb_msg->mti & 0x0FFF) << 12) | outgoing_openlcb_msg->source_alias;


                if (openlcb_last_frame) {

                    Release_OpenLcb_Msg(outgoing_openlcb_msg, TRUE);
                    // reset for the next message
                    outgoing_openlcb_msg = (void*) 0;
                    outgoing_openlcb_msg_index = 0;
                    openlcb_last_frame = FALSE;
                }


                Ecan1WriteTxMsgBufId(TX_CHANNEL_OPENLCB_MSG, identifier, TRUE, FALSE);
                Ecan1WriteTxMsgBufData(TX_CHANNEL_OPENLCB_MSG, iIndex, &openlcb_msg_can_data);

                Ecan1TxBufferSetTransmit(TX_CHANNEL_OPENLCB_MSG, TRUE);

                break;

        }


    } else { // Unaddressed message


        if (outgoing_openlcb_msg->payload_count <= 8) { // single frame

            identifier = RESERVED_TOP_BIT | CAN_OPENLCB_MSG | CAN_FRAME_TYPE_GLOBAL_ADDRESSED | ((uint32_t) (outgoing_openlcb_msg->mti & 0x0FFF) << 12) | outgoing_openlcb_msg->source_alias;

            Ecan1WriteTxMsgBufId(TX_CHANNEL_OPENLCB_MSG, identifier, TRUE, FALSE);
            Ecan1WriteTxMsgBufData(TX_CHANNEL_OPENLCB_MSG, outgoing_openlcb_msg->payload_count, (payload_bytes_can_t*) (outgoing_openlcb_msg->payload_ptr));

            Ecan1TxBufferSetTransmit(TX_CHANNEL_OPENLCB_MSG, TRUE);

            Release_OpenLcb_Msg(outgoing_openlcb_msg, TRUE);
            // reset for the next message
            outgoing_openlcb_msg = (void*) 0;
            outgoing_openlcb_msg_index = 0;

        } else { // multi frame

            // TODO Is there such a thing as a unaddressed multi frame?

        }

    }



    return;
    
}
