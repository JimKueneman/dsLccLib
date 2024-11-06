
/* 
 * File:   
 * Author: Jim Kueneman
 * Comments:
 * Revision history: 
 * 
 *     Handles the incoming and outgoing messages through a CAN interface.  
 * 
 *  The CAN driver calls HandleIncomingCAN_Msg with the CAN Frame that it captured coming in.  
 *  This function deals with the concatenation of multi-frame OpenLcb messages and only passes 
 *  along fully qualified OpenLcb messages.  It places them in the incoming_openlcb_msg_fifo 
 *  structure within the buffer.h file.  Use Pop_OpenLcb_Message in buffer. to remove the next 
 *  one in the FIFO.
 * 
 *  The main message loop needs call 
 *  
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __ENGINE_CAN_MESSAGE__
#define	__ENGINE_CAN_MESSAGE__

#include <xc.h> // include processor files - each processor file is guarded.  
#include "buffers.h"


#define TX_CHANNEL_CAN_CONTROL 0
#define TX_CHANNEL_OPENLCB_MSG 1


extern void Initialize_CAN_MessageHandler();

// Returns if the buffer to break up and send an OpenLcbMessage is clear and is acceptable to 
// call HandleOutgoingCAN_msg to process the next one.
extern uint8_t Outgoing_CAN_BufferEmpty();

// Places the OpenLcbMessage that need to be transmitted on the CAN bus in a local storage variable.
// Once this message is loaded it will be freed when done and will be accessed via the CAN TX interrupt
// so give it to this function then forget it.
extern uint8_t LoadOutgoing_CAN_Buffer(openlcb_msg_t* msg);

// Takes an outgoing openlcb message and converts it (or breaks it up into multiple messages) to send to the CAN driver to put out on the CAN bus 
// Note there is no reason to call this directly.  It is called vis the LoadOutgoing_CAN_Buffer to kick off the transmission but after that the
// CAN TX interrupt pumps it until it is finished then it is released.
extern uint8_t HandleOutgoingCAN_Msg(uint8_t called_from_interrupt); 

// Called on every incoming CAN message from the hardware module driver to decode and process
extern void HandleIncomingCAN_Msg(ecan_msg_t* msg);

extern uint8_t Send_Raw_CAN_Message(uint8_t tx_channel, ecan_msg_t* msg, uint8_t block);

extern uint8_t CopyData_CAN_Buffer_To_OpenLcbMsg(openlcb_msg_t* openlcb_msg_ptr, ecan_msg_t* ecan_msg_ptr, uint8_t start_index, uint8_t append);

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

