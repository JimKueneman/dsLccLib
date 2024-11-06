/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __MAINSTATEMACHINE__
#define	__MAINSTATEMACHINE__

#include <xc.h> // include processor files - each processor file is guarded.  
#include "node.h"
#include "buffers.h"

// General boot initialization
#define RUNSTATE_INIT                             0
// Generate the 48 bit seed to create the Alias from
#define RUNSTATE_GENERATE_SEED                    1
// Generate the Alias
#define RUNSTATE_GENERATE_ALIAS                   2
// Send the 4 (CID) messages with the NodeID and suggested Alias
#define RUNSTATE_SEND_CHECK_ID_07                 3
#define RUNSTATE_SEND_CHECK_ID_06                 4
#define RUNSTATE_SEND_CHECK_ID_05                 5   
#define RUNSTATE_SEND_CHECK_ID_04                 6   
// Wait for 200ms to see if anyone objects.  They objection could occur in this or the previous state, 
// if they do then jump back to RUNSTATE_GENERATE_SEED to try again
#define RUNSTATE_WAIT_200ms                       7
// Send the Alias Reserved message (AMR)
#define RUNSTATE_TRANSMIT_RESERVE_ID              8
// At this point the alias is reserved
// To "log in" send the (AMD) message, this sets the node to "Permitted" but still not OpenLCB "Initialized"
#define RUNSTATE_TRANSMIT_ALIAS_MAP_DEFINITION    9
// At this point the CAN specific login is complete, the Alias is reserved and the node is ready to be an OpenLCB Node
#define RUNSTATE_TRANSMIT_INITIALIZATION_COMPLETE 10
// Node is Initialized and can send any message, need to send the events we handle
#define RUNSTATE_TRANSMIT_EVENTS                  11
// Runs the message loop
#define RUNSTATE_RUN                              12
// Error state, detected someone is using our NodeID
#define RUNSTATE_DUPLICATE_NODEID                 13



extern void InitializeStateMachine(void);

extern void RunMainStateMachine(openlcb_msg_t* dispatched_msg);


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

