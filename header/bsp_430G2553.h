#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx


#define   debounceVal      250
#define   Sixtymsec        7865
#define   twentyFiveusec   3
#define   twentyFivemsec   3277
#define   startServoVal    60
#define   Set0xffff        65535
#define   Servo_step_size  13
#define FileEnding 0xFDFF
#define FileStarting 0xF600
#define TheForbiddenSegment    0x10BE
#define MetaDataSize        16

// LDR abstraction
#define LDRArrPortIN       P1IN
#define LDRArrPortSel      P1SEL
#define LDRArrPortDir      P1DIR

// UART abstraction
#define UARTArrPortOUT     P1OUT
#define UARTArrPortSel     P1SEL
#define UARTArrPortSel2    P1SEL2
#define UARTArrPortDir     P1DIR

// servoMotor abstraction
#define ServoSEL           P2SEL
#define ServoDIR           P2DIR
#define ServoOUT           P2OUT


// ultrasonic abstraction
#define ultrasonicSEL      P2SEL
#define ultrasonicSEL2     P2SEL2
#define ultrasonicOUT      P2OUT
#define ultrasonicDIR      P2DIR
#define ultrasonicIN       P2IN

// PushButtons abstraction
#define PBsArrPort	       P2IN
#define PBsArrIntPend	   P2IFG 
#define PBsArrIntEn	       P2IE
#define PBsArrIntEdgeSel   P2IES
#define PBsArrPortSel      P2SEL 
#define PBsArrPortDir      P2DIR 
#define PB0                0x01
#define PB1                0x80
#define PB2                0x40
#define PB3                0x80

//    LCD abstraction
#define LCD_DATA_WRITE    P1OUT
#define LCD_DATA_DIR      P1DIR
#define LCD_DATA_SEL      P1SEL
#define LCD_CTL_SEL       P2SEL
#define LCD_DATA_READ     P1IN


extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void ADCconfig(void);
extern void UARTconfig(void);
extern void FileModeConfig(void);

extern unsigned int num_of_files;

#endif
