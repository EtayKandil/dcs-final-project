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

// File system layout (flash)
// Metadata table lives from 0x1000 upwards. Allow up to 10 entries.
// Entry layout (16 bytes):
//   [0..9]  name (10 bytes)
//   [10]    status ('s' script, 't' text, 'e' empty)
//   [11]    reserved/padding
//   [12..13] data pointer (char*)
//   [14..15] size (unsigned int, bytes)
#define METADATA_BASE         0x1000
#define METADATA_TOTAL_BYTES  192      // reserved space for metadata region
#define METADATA_ENTRY_SIZE   16       // 10 files -> 160 bytes used (32 bytes spare)
#define METADATA_MAX_FILES    10
#define META_OFF_NAME         0
#define META_NAME_LEN         10
#define META_OFF_STATUS       10
#define META_OFF_DATAPTR      12
#define META_OFF_SIZE         14

// Data region: one file per 512B segment, starting at first free segment (per linker map)
// Start at 0xD600 to avoid .text/.const/.cinit; always avoid 0xFE00..0xFFFF (vectors)
#define FILE_DATA_BASE        0xD600
#define FILE_DATA_END         0xFDFF

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
#define PBsArrPort	       P1IN
#define PBsArrIntPend	   P1IFG 
#define PBsArrIntEn	       P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL 
#define PBsArrPortDir      P1DIR 
#define PB0                0x01
#define PB1                0x20
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

// Metadata helpers (addresses and accessors)
char *meta_name_ptr(unsigned int idx);
char *meta_status_ptr(unsigned int idx);
char **meta_data_ptr_ptr(unsigned int idx);
unsigned int *meta_size_ptr(unsigned int idx);

char meta_get_status(unsigned int idx);
unsigned int meta_get_size(unsigned int idx);
char *meta_get_data_ptr(unsigned int idx);
void meta_get_name(unsigned int idx, char out[11]);

// Compute cumulative offset (bytes) of data before idx and where to write data for idx
unsigned int meta_compute_data_offset_until(unsigned int idx);
char *meta_compute_data_write_ptr(unsigned int idx);

// Fixed segment-per-slot helpers
static inline char *slot_segment_base(unsigned int slot /*1..10*/){
	return (char *)(FILE_DATA_BASE + ((slot-1) * 512));
}

#endif
