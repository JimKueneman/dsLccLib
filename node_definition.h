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
#ifndef __NODE_DEFINITION__
#define	__NODE_DEFINITION__

#include "openlcb_defines.h"


#define MFG_VERSION 1
#define MFG_NAME "Mustangpeak"
#define MFG_MODEL "GS400"
#define MFG_HARDWARE_VER "1.2.5.6"
#define MFG_SOFTWARE_VER "0.12" "Mustangpeak"
#define USER_VERSION 2
#define USER_NAME "Mustangpeak"
#define USER_DESCRIPTION "This is my node"


/*
#define PSI_SIMPLE                         0x800000
#define PSI_DATAGRAM                       0x400000
#define PSI_STREAM                         0x200000
#define PSI_MEMORY_CONFIGURATION           0x100000
#define PSI_RESERVATION                    0x080000
#define PSI_EVENT_EXCHANGE                 0x040000
#define PSI_IDENTIFICATION                 0x020000
#define PSI_TEACHING_LEARNING              0x010000
#define PSI_REMOTE_BUTTON                  0x008000
#define PSI_ABBREVIATED_DEFAULT_CDI        0x004000
#define PSI_DISPLAY                        0x002000
#define PSI_SIMPLE_NODE_INFORMATION        0x001000
#define PSI_CONFIGURATION_DESCRIPTION_INFO 0x000800
#define PSI_TRAIN_CONTROL                  0x000400
#define PSI_FUNCTION_DESCRIPTION           0x000200
#define PSI_RESERVED_0                     0x000100
#define PSI_RESERVED_1                     0x000080
#define PSI_FUNCTION_CONFIGURATION         0x000040
#define PSI_FIRMWARE_UPGRADE               0x000020
#define PSI_FIRMWARE_UPGRADE_ACTIVE        0x000010
*/

const uint32_t USER_DEFINED_PROTOCOL_SUPPORT = (PSI_DATAGRAM | PSI_MEMORY_CONFIGURATION | PSI_EVENT_EXCHANGE | PSI_ABBREVIATED_DEFAULT_CDI | PSI_SIMPLE_NODE_INFORMATION | PSI_CONFIGURATION_DESCRIPTION_INFO | PSI_TRAIN_CONTROL | PSI_FUNCTION_DESCRIPTION | PSI_FUNCTION_CONFIGURATION);

#include <xc.h> // include processor files - each processor file is guarded.  

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

