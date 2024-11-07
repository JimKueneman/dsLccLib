
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __MCU_DRV__
#define	__MCU_DRV__

#include <xc.h> // include processor files - each processor file is guarded. 
#include "openlcb_buffers.h"
#include "can_statemachine.h"


// Timer -----------------------------------------------------------------------

typedef struct tagTIMEBITS {
  uint8_t _100ms  :1;
  uint8_t _1sec   :1;
  uint8_t _1minute:1;
  uint8_t _1hour  :1;
  uint8_t _1day   :1;
} TIMEBITS;



extern TIMEBITS TIMEbits;

extern void Initialize_MCU_Drv(void);

extern void _100msTimerEnableInterrupt(int8_t enable);
extern void Ecan1EnableInterrupt(int8_t enable);

extern void Ecan1WriteRxAcptFilter(int16_t n, int32_t identifier, uint16_t exide, uint16_t bufPnt, uint16_t maskSel);
extern void Ecan1WriteRxAcptMask(int16_t m, int32_t identifierMask, uint16_t mide, uint16_t exide);
extern void Ecan1DisableRXFilter(int16_t n);

extern uint8_t Ecan1TxBufferClear(uint16_t buf);
extern void Ecan1TxBufferSetTransmit(uint16_t buf, uint8_t do_set);
extern void Ecan1WriteTxMsgBufId(uint16_t buf, int32_t txIdentifier, uint16_t ide, uint16_t remoteTransmit);
extern void Ecan1WriteTxMsgBufData(uint16_t buf, uint16_t dataLength, payload_bytes_can_t* data);
extern void Ecan1WriteTxMsgBufDataWord(uint16_t buf, uint16_t data_length, uint16_t data1, uint16_t data2, uint16_t data3, uint16_t data4);
extern void Ecan1WriteTxMsgBufDataByte(uint16_t buf, uint16_t data_length, uint16_t data1, uint16_t data2, uint16_t data3, uint16_t data4, uint16_t data5, uint16_t data6, uint16_t data7, uint16_t data8);
extern void Ecan1ReadRxMsgBufId(uint16_t buf, can_msg_t *rxData, uint16_t *ide);
extern void Ecan1ReadRxMsgBufData(uint16_t buf, can_msg_t *rxData);


// How full the chips CAN fifo has gotten
extern uint8_t max_can_fifo_depth;




#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

