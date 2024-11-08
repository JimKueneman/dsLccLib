/*
 * File:   main_statemachine.c
 * Author: jimkueneman
 *
 * Created on March 2, 2024, 5:07 AM
 */


#include "xc.h"
#include "openlcb_defines.h"
#include "openlcb_statemachine.h"
#include "node.h"
#include "can_outgoing_statemachine.h"
#include "can_common_statemachine.h"
#include "mcu_driver.h"
#include "openlcb_buffers.h"
#include "node_definition.h"
#include "openlcb_utilities.h"
#include "callbacks.h"
#include "debug.h"
#include "stdio.h"  // printf

void Initialize_OpenLcb_StateMachine(void) {

   

}

void HandleSimpleNodeInfoRequest(openlcb_msg_t* dispatched_msg, openlcb_node_t* node) {

    openlcb_msg_t* new_msg;

    if (dispatched_msg->dest_alias == node->alias) {

        new_msg = Allocate_OpenLcb_Msg(dispatched_msg->dest_alias, dispatched_msg->dest_id, dispatched_msg->source_alias, dispatched_msg->source_id, MTI_SIMPLE_NODE_INFO_REPLY, ID_DATA_SIZE_STREAM_SNIP, TRUE);

        if (new_msg) {

            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[0] = 0x01;
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[1] = 'N';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[2] = 'a';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[3] = 'm';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[4] = 'e';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[5] = 0x00;
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[6] = 'M';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[7] = 'o';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[8] = 'd';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[9] = 'e';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[10] = 'l';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[11] = 0x00;
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[12] = '1';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[13] = '.';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[14] = '2';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[15] = 0x00;
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[16] = '0';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[17] = '.';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[18] = '9';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[19] = 0x00;
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[20] = 0x02;
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[21] = 'J';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[22] = 'i';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[23] = 'm';
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[24] = 0x00;
            (*(payload_stream_snip_t*) (new_msg->payload_ptr))[25] = 0x00;

            new_msg->payload_count = 26;


            if (!Push_OpenLcb_Message(&outgoing_openlcb_msg_fifo, new_msg, TRUE)) {

                // TODO: Send Error

            }


        } else {
            // TODO: Send Error
        }

    }
}

void HandleProtocolSupportInquiry(openlcb_msg_t* dispatched_msg, openlcb_node_t* node) {

    openlcb_msg_t* new_msg;

    if (dispatched_msg->dest_alias == node->alias) {

        new_msg = Allocate_OpenLcb_Msg(dispatched_msg->dest_alias, dispatched_msg->dest_id, dispatched_msg->source_alias, dispatched_msg->source_id, MTI_PROTOCOL_SUPPORT_REPLY, ID_DATA_SIZE_BASIC, TRUE);

        if (new_msg) {

            new_msg->payload_count = 3;

            uint32_t supported_protocols = USER_DEFINED_PROTOCOL_SUPPORT;

            for (int iIndex = 2; iIndex >= 0; iIndex--) {
                (*(payload_basic_t*) new_msg->payload_ptr)[iIndex] = supported_protocols & 0xFF;
                supported_protocols = supported_protocols >> 8;
            }


            if (!Push_OpenLcb_Message(&outgoing_openlcb_msg_fifo, new_msg, TRUE)) {

                // TODO: Send Error

            }


        } else {
            // TODO: Send Error
        }

    }

}

void HandleVerifyNodeID(openlcb_msg_t* dispatched_msg, openlcb_node_t* node) {

    openlcb_msg_t* new_msg;

    new_msg = Allocate_OpenLcb_Msg(dispatched_msg->dest_alias, dispatched_msg->dest_id, dispatched_msg->source_alias, dispatched_msg->source_id, MTI_VERIFIED_NODE_ID, ID_DATA_SIZE_BASIC, TRUE);

    if (new_msg) {

        CopyNodeIDToMessage(new_msg, node->id);

        if (!Push_OpenLcb_Message(&outgoing_openlcb_msg_fifo, new_msg, TRUE)) {

            // TODO: Send Error

        }


    } else {
        
        // TODO: Send Error
        
    }

}

void DispatchMsg(openlcb_msg_t* dispatched_msg) {

    openlcb_node_t* nextnode;
    openlcb_node_t* firstnode;


    if (dispatched_msg) {

        nextnode = NextActiveNode();
        firstnode = nextnode;

        if (nextnode) {

            if (nextnode->state.permitted) {

                switch (dispatched_msg->mti) {

                    case MTI_SIMPLE_NODE_INFO_REQUEST:
                    {
                        HandleSimpleNodeInfoRequest(dispatched_msg, nextnode);
                        break;

                    }
                    case MTI_PROTOCOL_SUPPORT_INQUIRY:
                    {
                        HandleProtocolSupportInquiry(dispatched_msg, nextnode);
                        break;

                    }
                    case MTI_VERIFY_NODE_ID_ADDRESSED:
                    {
                        HandleVerifyNodeID(dispatched_msg, nextnode); // Always reply regardless
                        break;
                    }
                    case MTI_VERIFY_NODE_ID_GLOBAL:
                    {
                        if (dispatched_msg->payload_count == 6) {
                            
                            if (MessageDataToNodeID(dispatched_msg) == nextnode->id)
                                HandleVerifyNodeID(dispatched_msg, nextnode);

                        } else
                            HandleVerifyNodeID(dispatched_msg, nextnode);

                        break;
                    }

                }


                nextnode = NextActiveNode();

                if (nextnode == firstnode)
                    return;
            }

        }

    }

}

void RunMainStateMachine(openlcb_msg_t* dispatched_msg) {

    openlcb_node_t* nextnode;
    openlcb_msg_t* openlcb_msg;
    can_msg_t can_msg;


    DispatchMsg(dispatched_msg);

    nextnode = NextActiveNode();

    if (nextnode) {

        switch (nextnode->state.run) {

            case RUNSTATE_INIT:

                nextnode->seed = nextnode->id;
                nextnode->alias = GenerateAlias(nextnode->seed);

                nextnode->state.run = RUNSTATE_GENERATE_ALIAS; // Jump over Generate Seed that only is if we have an Alias conflict and have to jump back
                break;

            case RUNSTATE_GENERATE_SEED:

                nextnode->seed = GenerateNewSeed(nextnode->seed);
                nextnode->state.run = RUNSTATE_GENERATE_ALIAS;

                break;

            case RUNSTATE_GENERATE_ALIAS:

                nextnode->alias = GenerateAlias(nextnode->seed);
               if (AliasChangeCallbackFunc) 
                    AliasChangeCallbackFunc(nextnode->alias, nextnode->id);
                
                nextnode->state.run = RUNSTATE_SEND_CHECK_ID_07;

                break;

            case RUNSTATE_SEND_CHECK_ID_07:

                can_msg.payload_size = 0;
                can_msg.identifier = RESERVED_TOP_BIT | CAN_CONTROL_FRAME_CID7 | (((nextnode->id >> 24) & 0xFFF000) | nextnode->alias); // AA0203040506
                  
                if (Push_CAN_Frame_Message(&can_msg, TRUE))
                    nextnode->state.run = RUNSTATE_SEND_CHECK_ID_06;

                break;

            case RUNSTATE_SEND_CHECK_ID_06:

                can_msg.payload_size = 0;
                can_msg.identifier = RESERVED_TOP_BIT | CAN_CONTROL_FRAME_CID6 | (((nextnode->id >> 12) & 0xFFF000) | nextnode->alias);

                if (Push_CAN_Frame_Message(&can_msg, TRUE))
                    nextnode->state.run = RUNSTATE_SEND_CHECK_ID_05;

                break;

            case RUNSTATE_SEND_CHECK_ID_05:

                can_msg.payload_size = 0;
                can_msg.identifier = RESERVED_TOP_BIT | CAN_CONTROL_FRAME_CID5 | ((nextnode->id & 0xFFF000) | nextnode->alias);

                if (Push_CAN_Frame_Message(&can_msg, TRUE))
                    nextnode->state.run = RUNSTATE_SEND_CHECK_ID_04;

                break;

            case RUNSTATE_SEND_CHECK_ID_04:

                can_msg.payload_size = 0;
                can_msg.identifier = RESERVED_TOP_BIT | CAN_CONTROL_FRAME_CID4 | (((nextnode->id << 12) & 0xFFF000) | nextnode->alias);

                if (Push_CAN_Frame_Message(&can_msg, TRUE))
                    nextnode->state.run = RUNSTATE_WAIT_200ms;

                break;

            case RUNSTATE_WAIT_200ms:

                if (nextnode->timerticks > 3) {

                    nextnode->state.run = RUNSTATE_TRANSMIT_RESERVE_ID;
                }

                break;

            case RUNSTATE_TRANSMIT_RESERVE_ID:


                can_msg.identifier = RESERVED_TOP_BIT | CAN_CONTROL_FRAME_RID | nextnode->alias;
                can_msg.payload_size = 0;

                if (Push_CAN_Frame_Message(&can_msg, TRUE)) {

                    nextnode->state.run = RUNSTATE_TRANSMIT_ALIAS_MAP_DEFINITION;

                }

                break;

            case RUNSTATE_TRANSMIT_ALIAS_MAP_DEFINITION:

                can_msg.identifier = RESERVED_TOP_BIT | CAN_CONTROL_FRAME_AMD | nextnode->alias;
                can_msg.payload_size = 6;


                uint64_t local_id = nextnode->id;
                for (int iIndex = 5; iIndex > -1; iIndex--) {
                    can_msg.payload[iIndex] = local_id & 0xFF;
                    local_id = local_id >> 8;
                }

                if (Push_CAN_Frame_Message(&can_msg, TRUE)) {

                    nextnode->state.permitted = TRUE;
                    nextnode->state.run = RUNSTATE_TRANSMIT_INITIALIZATION_COMPLETE;

                }

                break;


            case RUNSTATE_TRANSMIT_INITIALIZATION_COMPLETE:

                // All OpenLcb transport types will enter this state so use the OpenLcb Message Sending System (independent of CAN at this level)

                openlcb_msg = Allocate_OpenLcb_Msg(nextnode->alias, nextnode->id, 0, 0, MTI_INITIALIZATION_COMPLETE, ID_DATA_SIZE_BASIC, TRUE);

                if (openlcb_msg) {

                    CopyNodeIDToMessage(openlcb_msg, nextnode->id);
                    
                    Push_OpenLcb_Message(&outgoing_openlcb_msg_fifo, openlcb_msg, TRUE);

                    nextnode->state.run = RUNSTATE_TRANSMIT_EVENTS;

                }

                break;

            case RUNSTATE_TRANSMIT_EVENTS:


                nextnode->state.run = RUNSTATE_RUN;

                break;

            case RUNSTATE_RUN:


                break;

            case RUNSTATE_DUPLICATE_NODEID:


                break;


        }

    }

}
