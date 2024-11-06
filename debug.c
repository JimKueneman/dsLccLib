/*
 * File:   debug.c
 * Author: jimkueneman
 *
 * Created on February 4, 2024, 2:03 PM
 */


#include "xc.h"
#include "stdio.h"
#include "openlcb_defines.h"
#include "buffers.h"
#include "mcu_drv.h"


uint8_t print_msg = TRUE;


void PrintContentsFIFO(openlcb_msg_buffer_t* fifo) {
    
    for ( int i = 0; i < LEN_OPENLCB_MSG_FIFO; i++) {
        
        if ( fifo->list[i] ) {
            
            printf("index: %d, mti: %04x, source alias: %04x, dest alias: %04x\n", i, fifo->list[i]->mti, fifo->list[i]->source_alias, fifo->list[i]->dest_alias);
           
        }

    }
    
};

void ForceFlushAndFreeFIFO(openlcb_msg_buffer_t* fifo) {
    
    for ( int i = 0; i < LEN_OPENLCB_MSG_FIFO; i++) {
        
        if ( fifo->list[i] ) {
            
            Release_OpenLcb_Msg(fifo->list[i], TRUE);
            
            fifo->list[i] = (void*) 0;
                  
        }

    }
    
}

uint16_t CountFIFO(openlcb_msg_buffer_t* fifo) {
    
    int count = 0;
    for (int i = 0; i < LEN_OPENLCB_MSG_FIFO; i++) {
      if (fifo->list[i]) 
          count = count + 1;
    };
    
    return count;
    
};


void PrintBufferStats() {
          
    printf("OpenLcb Msg Pool: %d\n", pool_openlcb_msg_allocated);
    printf("OpenLcb Msg Pool Max Depth: %d\n", max_pool_openlcb_msg_allocated);
    printf("dsPIC CAN FIFO Buffer Max Depth: %d\n", max_can_fifo_depth);
   
    printf("Incoming Msg FIFO: %d\n", CountFIFO(&incoming_openlcb_msg_fifo));
    printf("Outgoing Msg FIFO: %d\n", CountFIFO(&outgoing_openlcb_msg_fifo));
    printf("Incoming Msg Inprocess FIFO: %d\n", CountFIFO(&incoming_openlcb_inprocess_msg_list));
    printf("Outgoing Msg Inprocess FIFO: %d\n", CountFIFO(&outgoing_openlcb_inprocess_msg_list));
    
};

void PrintMtiName(uint16_t mti) {
    switch (mti) {
        case MTI_INITIALIZATION_COMPLETE:
            printf("MTI_INITIALIZATION_COMPLETE");
            break;
        case MTI_INITIALIZATION_COMPLETE_SIMPLE:
            printf("MTI_INITIALIZATION_COMPLETE_SIMPLE");
            break;
        case MTI_VERIFY_NODE_ID_ADDRESSED:
            printf("MTI_VERIFY_NODE_ID_ADDRESSED");
            break;
        case MTI_VERIFY_NODE_ID_GLOBAL:
            printf("MTI_VERIFY_NODE_ID_GLOBAL");
            break;
        case MTI_VERIFIED_NODE_ID:
            printf("MTI_VERIFIED_NODE_ID");
            break;
        case MTI_VERIFIED_NODE_ID_SIMPLE:
            printf("MTI_VERIFIED_NODE_ID_SIMPLE");
            break;
        case MTI_OPTIONAL_INTERACTION_REJECTED:
           printf("MTI_OPTIONAL_INTERACTION_REJECTED");
           break;      
        case MTI_TERMINATE_DO_TO_ERROR:
           printf("MTI_TERMINATE_DO_TO_ERROR");
           break;
        case MTI_PROTOCOL_SUPPORT_INQUIRY:
           printf("MTI_PROTOCOL_SUPPORT_INQUIRY");
           break;      
        case MTI_PROTOCOL_SUPPORT_REPLY:
           printf("MTI_PROTOCOL_SUPPORT_REPLY");
           break;   
        case MTI_CONSUMER_IDENTIFY:
            printf("MTI_CONSUMER_IDENTIFY");
            break;
        case MTI_CONSUMER_IDENTIFY_RANGE:
            printf("MTI_CONSUMER_IDENTIFY_RANGE");
            break;
        case MTI_CONSUMER_IDENTIFIED_UNKNOWN:
            printf("MTI_CONSUMER_IDENTIFIED_UNKNOWN");
            break;
        case MTI_CONSUMER_IDENTIFIED_SET:
            printf("MTI_CONSUMER_IDENTIFIED_SET");
            break;
         case MTI_CONSUMER_IDENTIFIED_CLEAR:
            printf("MTI_CONSUMER_IDENTIFIED_CLEAR");
            break;      
         case MTI_CONSUMER_IDENTIFIED_RESERVED:
            printf("MTI_CONSUMER_IDENTIFIED_RESERVED");
            break;
         case MTI_PRODUCER_IDENDIFY:
            printf("MTI_PRODUCER_IDENDIFY");
            break;      
         case MTI_PRODUCER_IDENTIFY_RANGE:
            printf("MTI_PRODUCER_IDENTIFY_RANGE");
            break;  
        case MTI_PRODUCER_IDENTIFIED_UNKNOWN:
            printf("MTI_PRODUCER_IDENTIFIED_UNKNOWN");
            break;
        case MTI_PRODUCER_IDENTIFIED_SET:
            printf("MTI_PRODUCER_IDENTIFIED_SET");
            break;
        case MTI_PRODUCER_IDENTIFIED_CLEAR:
            printf("MTI_PRODUCER_IDENTIFIED_CLEAR");
            break;
        case MTI_PRODUCER_IDENTIFIED_RESERVED:
            printf("MTI_PRODUCER_IDENTIFIED_RESERVED");
            break;
         case MTI_EVENTS_IDENTIFY_DEST:
            printf("MTI_EVENTS_IDENTIFY_DEST");
            break;      
         case MTI_EVENTS_IDENTIFY:
            printf("MTI_EVENTS_IDENTIFY");
            break;
         case MTI_EVENT_LEARN:
            printf("MTI_EVENT_LEARN");
            break;      
         case MTI_PC_EVENT_REPORT:
            printf("MTI_PC_EVENT_REPORT");
            break;   
        case MTI_SIMPLE_NODE_INFO_REQUEST:
            printf("MTI_SIMPLE_NODE_INFO_REQUEST");
            break;
        case MTI_SIMPLE_NODE_INFO_REPLY:
            printf("MTI_SIMPLE_NODE_INFO_REPLY");
            break;
        case MTI_SIMPLE_TRAIN_INFO_REQUEST:
            printf("MTI_SIMPLE_TRAIN_INFO_REQUEST");
            break;
        case MTI_SIMPLE_TRAIN_INFO_REPLY:
            printf("MTI_SIMPLE_TRAIN_INFO_REPLY");
            break;
        case MTI_TRACTION_PROTOCOL:
            printf("MTI_TRACTION_PROTOCOL");
            break;      
        case MTI_TRACTION_REPLY:
            printf("MTI_TRACTION_REPLY ");
            break;
        case MTI_STREAM_INIT_REQUEST:
            printf("MTI_STREAM_INIT_REQUEST");
            break;      
        case MTI_STREAM_INIT_REPLY:
            printf("MTI_STREAM_INIT_REPLY");
            break;
        case MTI_FRAME_TYPE_CAN_STREAM_SEND:
            printf("MTI_FRAME_TYPE_CAN_STREAM_SEND");
            break;
        case MTI_STREAM_PROCEED:
           printf("MTI_STREAM_PROCEED");
           break;      
        case MTI_STREAM_COMPLETE:
           printf("MTI_STREAM_COMPLETE");
           break;
        case MTI_DATAGRAM_OK_REPLY:
           printf("MTI_DATAGRAM_OK_REPLY");
           break;      
        case MTI_DATAGRAM_REJECTED_REPLY:
           printf("MTI_DATAGRAM_REJECTED_REPLY");
           break;        
    };
};

void PrintCAN1Registers(void) {
    
    printf("C1CTRL1: 0x%x\n", C1CTRL1);
    printf("C1CTRL1: 0x%x\n", C1CTRL2);
    printf("C1VEC: 0x%x\n", C1VEC);
    printf("C1FCTRL: 0x%x\n", C1FCTRL);
    printf("C1FIFO: 0x%x\n", C1FIFO);
    printf("C1INTF: 0x%x\n", C1INTF);
    printf("C1INTE: 0x%x\n", C1INTE);
    printf("C1EC: 0x%x\n", C1EC);
    printf("C1CFG1: 0x%x\n", C1CFG1);
    printf("C1CFG2: 0x%x\n", C1CFG2);
    printf("C1FEN1: 0x%x\n", C1FEN1);
    printf("C1FMSKSEL1: 0x%x\n", C1FMSKSEL1);
    printf("C1FMSKSEL2: 0x%x\n", C1FMSKSEL2);
    
    C1CTRL1bits.WIN = 0;
    
    printf("C1RXFUL1: 0x%x\n", C1RXFUL1);
    printf("C1RXFUL2: 0x%x\n", C1RXFUL2);
    printf("C1RXOVF1: 0x%x\n", C1RXOVF1);
    printf("C1RXOVF2: 0x%x\n", C1RXOVF2);
    printf("C1TR01CON: 0x%x\n", C1TR01CON);
    printf("C11TR23CON: 0x%x\n", C1TR23CON);
    printf("C1TR45CON: 0x%x\n", C1TR45CON);
    printf("C1TR67CON: 0x%x\n", C1TR67CON);
    printf("C1RXD: 0x%x\n", C1RXD);
    printf("C1TXD: 0x%x\n", C1TXD);
    
    C1CTRL1bits.WIN = 1;
    printf("C1BUFPNT1: 0x%x\n", C1BUFPNT1);
    printf("C1BUFPNT2: 0x%x\n", C1BUFPNT2);
    printf("C1BUFPNT3: 0x%x\n", C1BUFPNT3);
    printf("C1BUFPNT4: 0x%x\n", C1BUFPNT4);
    printf("\n");
    
    printf("MASKS\n");
    printf("C1RXM0SID: 0x%x\n", C1RXM0SID);
    printf("C1RXM0EID: 0x%x\n", C1RXM0EID);
    printf("\n");
    printf("C1RXM1SID: 0x%x\n", C1RXM1SID);
    printf("C1RXM1EID: 0x%x\n", C1RXM1EID);
    printf("\n");
    printf("C1RXM2SID: 0x%x\n", C1RXM2SID);
    printf("C1RXM2EID: 0x%x\n", C1RXM2EID);
    printf("\n");printf("\n");
    printf("FILTERS\n");
    printf("C1RXF0SID: 0x%x\n", C1RXF0SID);
    printf("C1RXF0EID: 0x%x\n", C1RXF0EID);
    printf("\n");
    printf("C1RXF1SID: 0x%x\n", C1RXF1SID);
    printf("C1RXF1EID: 0x%x\n", C1RXF1EID);
    printf("\n");
    printf("C1RXF2SID: 0x%x\n", C1RXF2SID);
    printf("C1RXF2EID: 0x%x\n", C1RXF2EID);
    printf(".....\n");
    printf("\n");
    C1CTRL1bits.WIN = 0;
}

void PrintDMA0Registers(void) {
    printf("DMA 0\n");
    printf("DMA0CON: 0x%x\n", DMA0CON);
    printf("DMA0REQ: 0x%x\n", DMA0REQ);
    printf("DMA0STAH: 0x%x\n", DMA0STAH);
    printf("DMA0STAL: 0x%x\n", DMA0STAL);
    printf("DMA0STBH: 0x%x\n", DMA0STBH);
    printf("DMA0STBL: 0x%x\n", DMA0STBL);
    printf("DMA0PAD: 0x%x\n", DMA0PAD);
    printf("DMA0CNT: 0x%x\n", DMA0CNT);
    printf("\n");
}

void PrintDMA1Registers(void) {
    printf("DMA 1\n");
    printf("DMA1CON: 0x%x\n", DMA1CON);
    printf("DMA2REQ: 0x%x\n", DMA1REQ);
    printf("DMA1STAH: 0x%x\n", DMA1STAH);
    printf("DMA1STAL: 0x%x\n", DMA1STAL);
    printf("DMA1STBH: 0x%x\n", DMA1STBH);
    printf("DMA1STBL: 0x%x\n", DMA1STBL);
    printf("DMA1PAD: 0x%x\n", DMA1PAD);
    printf("DMA1CNT: 0x%x\n", DMA1CNT);
    printf("\n");
}

void PrintDMA2Registers(void) {
    printf("DMA 2\n");
    printf("DMA2CON: 0x%x\n", DMA2CON);
    printf("DMA2REQ: 0x%x\n", DMA2REQ);
    printf("DMA2STAH: 0x%x\n", DMA2STAH);
    printf("DMA2STAL: 0x%x\n", DMA2STAL);
    printf("DMA2STBH: 0x%x\n", DMA2STBH);
    printf("DMA2STBL: 0x%x\n", DMA2STBL);
    printf("DMA2PAD: 0x%x\n", DMA2PAD);
    printf("DMA2CNT: 0x%x\n", DMA2CNT);
    printf("\n");
}

void PrintDMA3Registers(void) {
    printf("DMA 3\n");
    printf("DMA3CON: 0x%x\n", DMA3CON);
    printf("DMA3REQ: 0x%x\n", DMA3REQ);
    printf("DMA3STAH: 0x%x\n", DMA3STAH);
    printf("DMA3STAL: 0x%x\n", DMA3STAL);
    printf("DMA3STBH: 0x%x\n", DMA3STBH);
    printf("DMA3STBL: 0x%x\n", DMA3STBL);
    printf("DMA3PAD: 0x%x\n", DMA3PAD);
    printf("DMA3CNT: 0x%x\n", DMA3CNT);
    printf("\n");
}

void PrintDMACommonRegisters(void) {
    printf("DMA Common\n");
    printf("DSADRL: 0x%x\n", DSADRL);
    printf("DSADRH: 0x%x\n", DSADRH);
    printf("DMAPWC: 0x%x\n", DMAPWC);
    printf("DMARQC: 0x%x\n", DMARQC);
    printf("DMALCA: 0x%x\n", DMALCA);
    printf("DMAPPS: 0x%x\n", DMAPPS);
    printf("\n");
}
