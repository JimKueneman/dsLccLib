
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __NODE__
#define	__NODE__

#include <xc.h> // include processor files - each processor file is guarded.  

#define LEN_NODE_ARRAY 10

typedef struct tagnodestateBITS {
  uint16_t run:6;                    // Run state... limits the number to how many bits here.... 64 possible states.
  uint16_t allocated:1;              // Allocated to be used
  uint16_t permitted:1;              // Has the CAN alias been allocated and the network notified
  uint16_t initalized:1;             // Has the node been logged into the the network
  uint16_t duplicate_id_detected:1;  // Node has detected a duplicated Node ID and has taken itself off line
} nodestateBITS;

typedef struct {
    uint64_t id;
    uint16_t alias;
    uint64_t seed;                     // Seed for generating the alias 
    nodestateBITS state;
    uint16_t timerticks;              // Counts the 100ms timer ticks during the CAN alias allocation, timeouts, etc
} openlcb_node_t; 

typedef struct {
    openlcb_node_t* nodes[LEN_NODE_ARRAY];
    uint16_t count;        // How many are valid in the nodes array
    uint16_t index;        // current node that the state machine is running on  
} active_nodes_t;

typedef struct {
  openlcb_node_t node[LEN_NODE_ARRAY];
  active_nodes_t active;
} openlcb_nodes_t;

typedef openlcb_node_t* active_nodes[LEN_NODE_ARRAY];


extern void Initialize_Node();

extern openlcb_node_t* AllocateNode(uint64_t nodeid);
extern openlcb_node_t* FindNodeByAlias(uint16_t alias);
extern openlcb_node_t* FindNodeByNodeID(uint64_t nodeid);
extern uint64_t GenerateNewSeed(uint64_t start_seed);
extern uint16_t GenerateAlias(uint64_t seed);
extern openlcb_node_t* NextActiveNode();
extern void _100msTimeTickNode();

extern openlcb_nodes_t nodes;
extern uint16_t can_send_timeout;


// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
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

