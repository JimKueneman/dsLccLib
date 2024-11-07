

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __CAN_OUTGOING_STATEMACHINE__
#define	__CAN_OUTGOING_STATEMACHINE__

#include <xc.h> // include processor files - each processor file is guarded.  
#include "openlcb_buffers.h"
#include "can_buffers.h"
#include "can_common_statemachine.h"

extern void Initialize_CAN_Outgoing_StateMachine();

// Returns if the buffer to break up and send an OpenLcbMessage is clear and is acceptable to 
// call HandleOutgoingCAN_msg to process the next one.
extern uint8_t Outgoing_OpenLcb_Msg_Buffer_Empty();

// Places the OpenLcbMessage that need to be transmitted on the CAN bus in a local storage variable.
// Once this message is loaded it will be freed when done and will be accessed via the CAN TX interrupt
// so give it to this function then forget it.
extern uint8_t Load_Outgoing_OpenLcb_Msg_Buffer(openlcb_msg_t* msg);

// Returns if the buffer to break up and send an OpenLcbMessage is clear and is acceptable to 
// call HandleOutgoingCAN_msg to process the next one.
extern uint8_t Outgoing_CAN_Msg_Buffer_Empty();

// Places the OpenLcbMessage that need to be transmitted on the CAN bus in a local storage variable.
// Once this message is loaded it will be freed when done and will be accessed via the CAN TX interrupt
// so give it to this function then forget it.
extern uint8_t Load_Outgoing_CAN_Msg_Buffer(can_msg_t* msg);

// Takes an outgoing openlcb message and converts it (or breaks it up into multiple messages) to send to the CAN driver to put out on the CAN bus 
// Note there is no reason to call this directly.  It is called vis the LoadOutgoing_CAN_Buffer to kick off the transmission but after that the
// CAN TX interrupt pumps it until it is finished then it is released.
extern uint8_t Statemachine_Outgoing_CAN(uint8_t called_from_interrupt); 


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

