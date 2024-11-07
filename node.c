/*
 * File:   node.c
 * Author: jimkueneman
 *
 * Created on November 3, 2024, 6:34 AM
 */


#include "xc.h"
#include "node.h"
#include "openlcb_defines.h"
#include "openlcb_statemachine.h"
#include "mcu_driver.h"

openlcb_nodes_t nodes;
uint16_t can_send_timeout;

void Initialize_Node() {

    nodes.active.index = 0;

    for (int iIndex = 0; iIndex < LEN_NODE_ARRAY; iIndex++) {

        nodes.node[iIndex].alias = 0;
        nodes.node[iIndex].id = 0;
        nodes.node[iIndex].seed = 0;
        nodes.node[iIndex].state.run = RUNSTATE_INIT;
        nodes.node[iIndex].state.allocated = FALSE;
        nodes.node[iIndex].state.duplicate_id_detected = FALSE;
        nodes.node[iIndex].state.initalized = FALSE;
        nodes.node[iIndex].state.permitted = FALSE;
        nodes.node[iIndex].timerticks = 0;

        nodes.active.nodes[iIndex] = (void*) 0;

    }

}

openlcb_node_t* NextActiveNode() {
    
    openlcb_node_t* result = (void*) 0;

    if (nodes.active.count > 0) {
        
        result = nodes.active.nodes[nodes.active.index];

        nodes.active.index = nodes.active.index + 1;

        if (nodes.active.index >= nodes.active.count)
            nodes.active.index = 0;     

    }
    
    return result;

}

openlcb_node_t* AllocateNode(uint64_t nodeid) {

    Ecan1EnableInterrupt(FALSE);

    for (int iIndex = 0; iIndex < LEN_NODE_ARRAY; iIndex++) {

        if (!nodes.node[iIndex].state.allocated) {

            // rest of the fields were reset when the node was released

            nodes.node[iIndex].alias = 0;
            nodes.node[iIndex].id = nodeid;
            nodes.node[iIndex].seed = nodeid;
            nodes.node[iIndex].state.allocated = TRUE;

            // Add it to the active nodes list to be included in the message statemachine
            nodes.active.nodes[nodes.active.count] = &nodes.node[iIndex];
            nodes.active.count = nodes.active.count + 1;

            Ecan1EnableInterrupt(TRUE);
            return &nodes.node[iIndex];

        }

    }
    
    Ecan1EnableInterrupt(TRUE);

    return (void*) 0;

}

openlcb_node_t* FindNodeByAlias(uint16_t alias) {

    for (int iIndex = 0; iIndex < nodes.active.count; iIndex++) {

        if ((*(nodes.active.nodes[iIndex])).alias == alias)

            return nodes.active.nodes[iIndex];

    };

    return (void*) 0;

}

openlcb_node_t* FindNodeByNodeID(uint64_t nodeid) {

    for (int iIndex = 0; iIndex < nodes.active.count; iIndex++) {

        if ((*(nodes.active.nodes[iIndex])).id == nodeid)

            return nodes.active.nodes[iIndex];

    };

    return (void*) 0;
}

uint64_t GenerateNewSeed(uint64_t start_seed) {

    uint32_t lfsr1 = start_seed & 0xFFFFFF;
    uint32_t lfsr2 = (start_seed >> 24) & 0xFFFFFF;

    uint32_t temp1 = ((lfsr1 << 9) | ((lfsr2 >> 15) & 0x1FF)) & 0xFFFFFF;
    uint32_t temp2 = (lfsr2 << 9) & 0xFFFFFF;

    lfsr1 = lfsr1 + temp1 + 0x1B0CA3L;
    lfsr2 = lfsr2 + temp2 + 0x7A4BA9L;

    lfsr1 = (lfsr1 & 0xFFFFFF) + ((lfsr2 & 0xFF000000) >> 24);
    lfsr2 = lfsr2 & 0xFFFFFF;

    return ( (uint64_t) lfsr1 << 24) | lfsr2;

}

uint16_t GenerateAlias(uint64_t seed) {

    uint32_t lfsr2 = seed & 0xFFFFFF;
    uint32_t lfsr1 = (seed >> 24) & 0xFFFFFF;

    return ( lfsr1 ^ lfsr2 ^ (lfsr1 >> 12) ^ (lfsr2 >> 12)) & 0x0FFF;

}

void _100msTimeTickNode() {
    
    for (int iIndex = 0; iIndex < nodes.active.count; iIndex++) {
      
      nodes.active.nodes[iIndex]->timerticks = nodes.active.nodes[iIndex]->timerticks + 1;
              
    };
    
    can_send_timeout = can_send_timeout + 1;
}
