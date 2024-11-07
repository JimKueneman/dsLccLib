
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
#include "openlcb_buffers.h"
#include "can_buffers.h"
#include "can_common_statemachine.h"


extern void Initialize_CAN_Incoming_StateMachine();


// Called on every incoming CAN message from the hardware module driver to decode and process
extern void Statemachine_Incoming_CAN(can_msg_t* msg);


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

