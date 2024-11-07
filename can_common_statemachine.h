

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __CAN_COMMON_STATEMACHINE_
#define	__CAN_COMMON_STATEMACHINE_

#include <xc.h> // include processor files - each processor file is guarded.  

extern uint8_t CopyData_CAN_Buffer_To_OpenLcbMsg(openlcb_msg_t* openlcb_msg_ptr, can_msg_t* ecan_msg_ptr, uint8_t start_index, uint8_t append);
extern void TestForAliasConflict(can_msg_t* msg, uint32_t can_control_msg, uint8_t add_payload_node_id);

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

