/*
 * File:   main.c
 * Author: jimkueneman
 *
 * Created on January 23, 2024, 6:26 AM
 */


// DSPIC33EP64GP502 Configuration Bit Settings

// 'C' source line config statements

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config ALTI2C1 = OFF            // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF            // Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins)
#pragma config WDTWIN = WIN25           // Watchdog Window Select bits (WDT Window is 25% of WDT period)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FOSC
#pragma config POSCMD = HS              // Primary Oscillator Mode Select bits (HS Crystal Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSDCMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Oscillator Source Selection (Primary Oscillator with PLL module (XT + PLL, HS + PLL, EC + PLL))
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GCP = OFF                // General Segment Code-Protect bit (General Segment Code protect is Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#include "xc.h"

#include "mcu_driver.h"
#include "stdio.h"
#include "openlcb_buffers.h"
#include "openlcb_defines.h"
#include "openlcb_statemachine.h"
#include "debug.h"
#include "node.h"
#include "openlcb_utilities.h"
#include "callbacks.h"

/*
 * Function:        void ClearIntrflags(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:         Clears all the interrupt flag registers.
 *****************************************************************************/
void ClearIntrflags(void) {
    /* Clear Interrupt Flags */
    IFS0 = 0;
    IFS1 = 0;
    IFS2 = 0;
    IFS3 = 0;
    IFS4 = 0;
}

void HandleTime(void) {

    _100msTimerEnableInterrupt(FALSE);

    if (TIMEbits._100ms) {

        TIMEbits._100ms = 0;
        PORTAbits.RA1 = !PORTAbits.RA1;
    }
    if (TIMEbits._1minute) {

        TIMEbits._1minute = 0;
        //    Mapping1mTimeTick();

    }
    _100msTimerEnableInterrupt(TRUE);
}

void HandleUART(void) {

    /* Check for receive errors */
    if (U1STAbits.FERR == 1) {

    }

    /* Must clear the overrun error to keep UART receiving */
    if (U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0;

    }

    if (U1STAbits.URXDA == 1) {
        U1TXREG = U1RXREG; // Loop back, send the byte back to the UART

        //    AddOpenLCB_CAN_MsgBufferItem(&list_outgoing, )
    }
    return;
}

void AliasAllocatedCallback(uint16_t alias, uint64_t node_id) {

    PrintAliasAndNodeID(alias, node_id);
}

//#define DEBUG

int main(void) {

    openlcb_msg_t* dispatched_msg;

    ClearIntrflags();
    
    Initialize_OpenLcb_Buffers(); 
    Initialize_OpenLcb_StateMachine();
    
    Initialize_CAN_Buffers();
    Initialize_CAN_StateMachine();
    
#ifdef DEBUG
    
    can_msg_t msg;
    
    msg.identifier = 0x19AAAFFF;
    msg.payload_size = 4;
    msg.payload[0] = 0xAA;
    msg.payload[1] = 0xBB;
    msg.payload[2] = 0xCC;
    msg.payload[3] = 0xDD;
    
    Push_CAN_Message(&msg, FALSE);
    
    msg.identifier = 0;
    msg.payload_size = 0;
    msg.payload[0] = 0;
    msg.payload[0] = 0;
    msg.payload[0] = 0;
    msg.payload[0] = 0;
    
    Pop_CAN_Message(&msg, FALSE);
    
#endif
    
    
    
    Initialize_MCU_Drv();
    Initialize_Node();

    TRISAbits.TRISA1 = 0;

    // */ 

    _TRISB4 = 0; // Output
    _RB4 = 0;

    openlcb_node_t* node = AllocateNode(0xAA0203040506);

    //     AllocateNode(0xBB0203040506);
    //     AllocateNode(0xCC0203040506);
    //     AllocateNode(0xDD0203040506);
    //     AllocateNode(0xFF0203040506);

    AliasChangeCallbackFunc = &AliasAllocatedCallback;



    while (1) {



        HandleTime();
        HandleUART();


        dispatched_msg = Pop_OpenLcb_Message(&incoming_openlcb_msg_fifo, TRUE);

        RunMainStateMachine(dispatched_msg);


        if (dispatched_msg) {

            Ecan1EnableInterrupt(FALSE);



            if (print_msg) {
                printf(" Source MTI: [0x%04x] - Source Alias: [0x%04x] - Dest Alias: [0x%04x] ", dispatched_msg->mti, dispatched_msg->source_alias, dispatched_msg->dest_alias);

                printf("[");
                for (int i = 0; i < dispatched_msg->payload_count; i++) {

                    switch (dispatched_msg->state.data_struct_size) {
                        case ID_DATA_SIZE_BASIC:
                            printf("0x%02x ", (*((payload_basic_t*) dispatched_msg->payload_ptr))[i]);
                            break;
                        case ID_DATA_SIZE_DATAGRAM:
                            printf("0x%02x ", (*((payload_datagram_t*) dispatched_msg->payload_ptr))[i]);
                            break;
                        case ID_DATA_SIZE_STREAM_SNIP:
                            printf("0x%02x ", (*((payload_stream_snip_t*) dispatched_msg->payload_ptr))[i]);
                            break;
                    };

                };
                printf("]\n");

            };

            Release_OpenLcb_Msg(dispatched_msg, TRUE);

            Ecan1EnableInterrupt(TRUE);


        };

        //    RunCAN_MsgEngine();

    }

    return 0;
}
