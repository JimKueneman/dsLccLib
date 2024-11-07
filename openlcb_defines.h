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
#ifndef __OPENLCB_DEFINES__
#define	__OPENLCB_DEFINES__

#include <xc.h> // include processor files - each processor file is guarded.  


// New 10/19/2024

#define APPEND_TRUE  TRUE
#define APPEND_FALSE FALSE

#define TRUE                             0x01
#define FALSE                            0x00

#define RESERVED_TOP_BIT                 0x10000000
// OpenLCB Message Mask CAN adaptation (MTI is only 12 bits (vs 16) with upper 4 redefined)
#define CAN_OPENLCB_MSG                  0x08000000  // 1 in the 27th bit is OpenLcb; 0 is a CAN frame
// Masks out the 3 Frame Type Bits
#define MASK_CAN_FRAME_SEQUENCE_NUMBER   0x07000000  // if not an OpenLcb message then this is the type of CAN frame it is 
#define MASK_CAN_FRAME_TYPE              MASK_CAN_FRAME_SEQUENCE_NUMBER // if an OpenLcb frame the same 3 bits are the frame type
// Mask out the CAN adaptation 12 bit MTI
#define MASK_CAN_VARIABLE_FIELD          0x00FFF000

#define CAN_FRAME_TYPE_GLOBAL_ADDRESSED  0x01000000
#define CAN_FRAME_TYPE_DATAGRAM_ONLY     0x02000000
#define CAN_FRAME_TYPE_DATAGRAM_FIRST    0x03000000
#define CAN_FRAME_TYPE_DATAGRAM_MIDDLE   0x04000000
#define CAN_FRAME_TYPE_DATAGRAM_FINAL    0x05000000
#define CAN_FRAME_TYPE_RESERVED          0x06000000
#define CAN_FRAME_TYPE_STREAM            0x07000000

#define MTI_INITIALIZATION_COMPLETE         0x0100
#define MTI_INITIALIZATION_COMPLETE_SIMPLE  0x0101   // Only supports Simple Node
#define MTI_VERIFY_NODE_ID_ADDRESSED        0x0488
#define MTI_VERIFY_NODE_ID_GLOBAL           0x0490
#define MTI_VERIFIED_NODE_ID                0x0170
#define MTI_VERIFIED_NODE_ID_SIMPLE         0x0171   // Only supports Simple Node
#define MTI_OPTIONAL_INTERACTION_REJECTED   0x0068
#define MTI_TERMINATE_DO_TO_ERROR           0x00A8
#define MTI_PROTOCOL_SUPPORT_INQUIRY        0x0828
#define MTI_PROTOCOL_SUPPORT_REPLY          0x0668

#define MTI_CONSUMER_IDENTIFY                0x08F4                             // Databytes = EventID
#define MTI_CONSUMER_IDENTIFY_RANGE          0x04A4                            // Databytes = EventID with Mask
#define MTI_CONSUMER_IDENTIFIED_UNKNOWN      0x04C7                            // Databytes = EventID
#define MTI_CONSUMER_IDENTIFIED_SET          0x04C4                            // Databytes = EventID
#define MTI_CONSUMER_IDENTIFIED_CLEAR        0x04C5                            // Databytes = EventID
#define MTI_CONSUMER_IDENTIFIED_RESERVED     0x04C6                            // Databytes = EventID
#define MTI_PRODUCER_IDENDIFY                0x0914                            // Databytes = EventID
#define MTI_PRODUCER_IDENTIFY_RANGE          0x0524                            // Databytes = EventID with Mask
#define MTI_PRODUCER_IDENTIFIED_UNKNOWN      0x0547                            // Databytes = EventID
#define MTI_PRODUCER_IDENTIFIED_SET          0x0544                            // Databytes = EventID
#define MTI_PRODUCER_IDENTIFIED_CLEAR        0x0545                            // Databytes = EventID
#define MTI_PRODUCER_IDENTIFIED_RESERVED     0x0546                            // Databytes = EventID
#define MTI_EVENTS_IDENTIFY_DEST             0x0968                            // Databytes = Destination Alias
#define MTI_EVENTS_IDENTIFY                  0x0970                            //
#define MTI_EVENT_LEARN                      0x0594                            // Databytes = EventID
#define MTI_PC_EVENT_REPORT                  0x05B4                            // Databytes = EventID  (Infamouse PCER)

#define MTI_SIMPLE_NODE_INFO_REQUEST         0x0DE8                            // Databytes = Destination Alias
#define MTI_SIMPLE_NODE_INFO_REPLY           0x0A08                            // Databytes = Destination Alias, ACDI Data

#define MTI_SIMPLE_TRAIN_INFO_REQUEST        0x0DA8                            // Databytes = Destination Alias
#define MTI_SIMPLE_TRAIN_INFO_REPLY          0x09C8                            // Databytes = Destination Alias, ACDI Data

#define MTI_TRACTION_PROTOCOL                0x05EB                            // Databyte = depends;
#define MTI_TRACTION_REPLY                   0x01E9                            // Databyte = depends

#define MTI_STREAM_INIT_REQUEST              0x0CC8
#define MTI_STREAM_INIT_REPLY                0x0868
#define MTI_FRAME_TYPE_CAN_STREAM_SEND       0xF000
#define MTI_STREAM_PROCEED                   0x0888
#define MTI_STREAM_COMPLETE                  0x08A8

#define MTI_DATAGRAM                         0x1C48
#define MTI_DATAGRAM_OK_REPLY                0x0A28                            // Databytes = Destination Alias
#define MTI_DATAGRAM_REJECTED_REPLY          0x0A48                            // Databytes = Destination Alias, Error Code 


// Data field Masks
#define MASK_MULTIFRAME_BITS             0xF0

#define MULTIFRAME_ONLY                  0x00
#define MULTIFRAME_FIRST                 0x40
#define MULTIFRAME_MIDDLE                0xC0
#define MULTIFRAME_FINAL                 0x80

#define MASK_STREAM_OR_DATAGRAM      0x01000
#define MASK_PRIORITY                0x00C00
#define MASK_SIMPLE_PROTOCOL         0x00010
#define MASK_DEST_ADDRESS_PRESENT    0x00008
#define MASK_EVENT_PRESENT           0x00004
#define MASK_PRIORITY_MODIFIER       0x00003

// OLD

// CAN MTI

#define CAN_CONTROL_FRAME_CID7                  0x07000000                                // First 12 Bits of 48 bit Node ID
#define CAN_CONTROL_FRAME_CID6                  0x06000000                                // 2rd 12 Bits of 48 bit Node ID
#define CAN_CONTROL_FRAME_CID5                  0x05000000                                // 3nd 12 Bits of 48 bit Node ID
#define CAN_CONTROL_FRAME_CID4                  0x04000000                                // Last 12 Bits of 48 bit Node ID
#define CAN_CONTROL_FRAME_CID3                  0x03000000                                // non-OpenLCB Protocol
#define CAN_CONTROL_FRAME_CID2                  0x02000000                                // non-OpenLCB Protocol
#define CAN_CONTROL_FRAME_CID1                  0x01000000                                // non-OpenLCB Protocol

#define CAN_CONTROL_FRAME_RID                   0x00700000                                // Reserve ID
#define CAN_CONTROL_FRAME_AMD                   0x00701000                                // Alias Map Definition
#define CAN_CONTROL_FRAME_AME                   0x00702000                                // Alias Mapping Enquiry
#define CAN_CONTROL_FRAME_AMR                   0x00703000                                // Alias Map Reset Frame
#define CAN_CONTROL_FRAME_ERROR_INFO_REPORT_0   0x00710000
#define CAN_CONTROL_FRAME_ERROR_INFO_REPORT_1   0x00711000
#define CAN_CONTROL_FRAME_ERROR_INFO_REPORT_2   0x00712000
#define CAN_CONTROL_FRAME_ERROR_INFO_REPORT_3   0x00713000

// OpenLCB Message Masks Full MTI (16 bits)
#define MASK_OPENLCB_MSG_BIT             0x08000000 
#define MASK_RESERVED_2                  0x04000000
#define MASK_RESERVED_1                  0x04000000
#define MASK_SPECIAL                     0x02000000




// OpenLCB Message Masks CAN adaptation in the Identifier
#define MASK_CAN_STREAM_OR_DATAGRAM      0x01000000
#define MASK_CAN_PRIORITY                0x00C00000
#define MASK_CAN_SIMPLE_PROTOCOL         0x00010000
#define MASK_CAN_DEST_ADDRESS_PRESENT    0x00008000
#define MASK_CAN_EVENT_PRESENT           0x00004000
#define MASK_CAN_PRIORITY_MODIFIER       0x00003000
#define MASK_CAN_SOURCE_ALIAS            0x00000FFF

// CAN Control Message Masks
#define MASK_CAN_CONTROL_VAR_FIELD       0x07FFF000
#define MASK_CAN_CONTROL_FRAME_SEQUENCE  0x07000000
#define MASK_CAN_CONTROL_NODEID          0x00FFF000


// Protocol Support 

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


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

