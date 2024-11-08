

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

// Call as often as possible to pump out the message(s) loaded via Load_Outgoing_CAN_Msg_Buffer
extern void Statemachine_Outgoing_CAN(); 


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

