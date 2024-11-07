
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __BUFFERS__
#define	__BUFFERS__

#include <xc.h> // include processor files - each processor file is guarded.  



///////////////////////////// 10/28/2024


#define LEN_DATA_BASIC       16     // most are 8 bytes but a few protocols take 2 frames like Traction
#define LEN_DATA_DATAGRAM    72
#define LEN_DATA_STREAM_SNIP 1024

#define LEN_MAX_DATA         LEN_DATA_STREAM_SNIP

#define ID_DATA_SIZE_BASIC       0x00    // b00 
#define ID_DATA_SIZE_DATAGRAM    0x01    // b01
#define ID_DATA_SIZE_STREAM_SNIP 0x02    // b10
#define ID_DATA_SIZE_RESERVED    0x03    // b11


#define LEN_DATA_SIZE_BASIC_POOL          50  // USER DEFINED
#define LEN_DATA_SIZE_DATAGRAM_POOL       10  // USER DEFINED
#define LEN_DATA_SIZE_STREAM_SNIP_POOL     2  // USER DEFINED
#define LEN_OPENLCB_MSG_POOL    LEN_DATA_SIZE_BASIC_POOL+LEN_DATA_SIZE_DATAGRAM_POOL+LEN_DATA_SIZE_STREAM_SNIP_POOL  

#define LEN_OPENLCB_MSG_INPROCESS_BUFFER  50  // USER DEFINED

#define LEN_OPENLCB_MSG_FIFO  LEN_OPENLCB_MSG_POOL


// Structures of different length depending on the OpenLCB payload size
typedef uint8_t payload_basic_t[LEN_DATA_BASIC];
typedef uint8_t payload_datagram_t[LEN_DATA_DATAGRAM];
typedef uint8_t payload_stream_snip_t[LEN_DATA_STREAM_SNIP];


// ******************************************
// OpenLCB Message 
// ******************************************

typedef struct {
    uint8_t data_struct_size:2;        // defines what type the data_struct is; ID_DATA_SIZE_BASIC, ID_DATA_SIZE_DATAGRAM, ID_DATA_SIZE_STREAM_SNIP
    uint8_t allocated:1;               // message has been allocated and is in use
    uint8_t valid:1;                   // message is complete and ready to be dispatched
} openlcb_msg_state;

typedef struct {
    openlcb_msg_state state;
    uint16_t mti;
    uint16_t source_alias;
    uint16_t dest_alias;
    uint64_t source_id;
    uint64_t dest_id;
    uint16_t payload_count;
    void* payload_ptr;       // depending on state.data_struct_size will be payload_basic_t*, payload_datagram_t* or payload_stream_snip_t*;
} openlcb_msg_t;

// Array of all the openlcb message structures
typedef openlcb_msg_t openlcb_msg_pool_t[LEN_OPENLCB_MSG_POOL];


// fifo to hold pointers to openlcb_msg structures and indexes for the head and tail, note this implies that
// this is a fifo so the messages will be put in and taken out in order

typedef struct {
    openlcb_msg_t* list[LEN_OPENLCB_MSG_FIFO];
    uint16_t head; // Points to the next message to process
    uint16_t tail; // Points to the next message to process
} openlcb_msg_buffer_t;

typedef struct {
    openlcb_msg_t* list[LEN_OPENLCB_MSG_INPROCESS_BUFFER];
} inprocess_buffer_t;

// buffer for complete OpenLCB messages ready to dispatch to internal nodes
extern openlcb_msg_buffer_t incoming_openlcb_msg_fifo;
// buffer for complete OpenLCB messages to be sent out
extern openlcb_msg_buffer_t outgoing_openlcb_msg_fifo;

// buffer for multiframe incoming messages to be assembled and eventually moved to the incoming_openlcb_msg_fifo sructure (or deleted if abandon/error) 
extern inprocess_buffer_t incoming_openlcb_inprocess_msg_list;


extern uint16_t pool_openlcb_msg_allocated;
extern uint16_t max_pool_openlcb_msg_allocated;


/*
 * 
 *   [IN] Nothing
 *   Returns: nothing
 * Call at the beginning of program execution to set the buffers to a known state
 */
extern void Initialize_OpenLcb_Buffers();

/*
 *   [IN] source_alias: 12 bit alias the message came from
 *   [IN] dest_alias  : 12 bit alias the message was sent to (0 if global message)
 *   [IN] mti         : OpenLCB MTI (this is NOT a 12 bit CAN MTI)
 *   [IN] direction   : direction the message is bound (DIRECTION_RX, DIRECTION_TX)
 *   [IN] data_size: size of the data buffer (ID_DATA_SIZE_BASIC, ID_DATA_SIZE_DATAGRAM, ID_DATA_SIZE_STREAM_SNIP)
 *   [IN] disable_interrupts: used to disable the CAN interrupts for resource locking (mutex)
 *   Returns: pointer to an allocated openlcb_msg structure, null if any buffer could not be allocated
 * Allocates an OpenLCB message and buffer
 */
extern openlcb_msg_t* Allocate_OpenLcb_Msg(uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t mti, uint8_t buffer_size, uint8_t disable_interrupts); 


/*
 *   [IN] openlcb_msg: message to release (with its data structure)
 *   [IN] disable_interrupts: used to disable the CAN interrupts for resource locking (mutex)
 *   Returns: pointer to an openlcb_msg structure with the passed alias(s) and direction, null if any buffer could not be found
 * Releases the message that is passed
 */
extern void Release_OpenLcb_Msg(openlcb_msg_t* openlcb_msg, uint8_t disable_interrupts);


// ****************  FUNCTIONS TO TREAT THE PASSED BUFFER AS A FIFO ****************
 
/*
 * Puts the passed message on the passed FIFO stack
 *     [IN] fifo: the FIFO to operate on
 *     [IN] openlcb_msg: The message to push on the FIFO stack
 *     [IN] disable_interrupts: used to disable the CAN interrupts for resource locking (mutex)
 * 
 * Returns the message passed if it was placed on the FIFO; NULL if there was no space
 */
extern openlcb_msg_t* Push_OpenLcb_Message(openlcb_msg_buffer_t* fifo_ptr, openlcb_msg_t* msg, uint8_t disable_interrupts);


/*
 * Pulls the first in message on the passed FIFO stack, the msg is removed from the stack
 *     [IN] fifo: the FIFO to operate on
 *     [IN] disable_interrupts: used to disable the CAN interrupts for resource locking (mutex)
 * 
 * Returns: message that is next in line to be popped, NULL if empty
 */
extern openlcb_msg_t* Pop_OpenLcb_Message(openlcb_msg_buffer_t* fifo_ptr, uint8_t disable_interrupts);


extern uint8_t Is_OpenLcb_FIFO_Empty(openlcb_msg_buffer_t* fifo_ptr, uint8_t disable_interrupts);


// ***  FUNCTIONS TO TREAT THE PASSED BUFFER AS A LIST OF MESSAGE POINTERS  ****
/*   
 *   Treat the passed buffer as a List so insert the passed message into the first open (non NULL) slot
 * 
 *   [IN] buffer_ptr: buffer to use as the target, note this buffer must not be also used as a FIFO
 *   [IN] openlcb_msg_ptr:   message to insert into the passed list
 *   [IN] disable_interrupts: used to disable the CAN interrupts for resource locking (mutex)
 * 
 *   Returns: passed message if successful, NULL if buffer was full 
 */
extern openlcb_msg_t* Insert_OpenLcb_Message(inprocess_buffer_t* buffer_ptr, openlcb_msg_t* openlcb_msg_ptr, uint8_t disable_interrupts);

/*   
 *   Treat the passed buffer as a List so search all entries that are not NULL
 * 
 *   [IN] source_alias: 12 bit alias the message came from
 *   [IN] dest_alias:   12 bit alias the message came from (0 if not used)
 *   [IN] mti         : OpenLCB MTI (this is NOT a 12 bit CAN MTI)
 *   [IN] disable_interrupts: used to disable the CAN interrupts for resource locking (mutex)
 *   [IN] remove:  removes the messages from the passed list if found (message returned is valid (not destroyed) but no longer has a reference in the list)
 * 
 *   Returns: pointer to an message with matches to the passed alias(s) and mti; returns null if could not be found
 */
extern openlcb_msg_t* Find_OpenLcb_Message_As_Buffer(inprocess_buffer_t* buffer_ptr, uint16_t source_alias, uint64_t source_id, uint16_t dest_alias, uint64_t dest_id, uint16_t mti, uint8_t disable_interrupts, uint8_t remove);


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

