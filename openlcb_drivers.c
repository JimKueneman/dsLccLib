/*
 * File:   openlcb_drivers.c
 * Author: jimkueneman
 *
 * Created on February 17, 2024, 6:09 AM
 */


#include "xc.h"
#include "openlcb_drivers.h"
#include "openlcb_defines.h"



int IsMultiFrameMsg(uint32_t Identifier) {
    
    if(is_can_control_msg(Identifier)) {
        
    };
    
   if(is_openlcb_msg(Identifier)) {
        
    };
    
    uint16_t x = extract_can_control_nodeid_chunk(Identifier);
   return 0; 
}
