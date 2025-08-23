#ifndef _halGPIO_H_
#define _halGPIO_H_

#include  "../header/bsp_430G2553.h"

#include  "../header/app.h"    		                // private library - APP layer
#include  <stdlib.h>



extern enum FSMstate state;   // global variable
extern enum SYSmode lpm_mode; // global variable

extern unsigned int start;
extern unsigned int end;
extern unsigned int dist;
extern int loc_angel;
extern unsigned char TX_to_send[3];
extern int maskDist;
extern int state_or_num;
extern unsigned int memLoad;
extern int input_slot;
extern unsigned int first_time;

extern unsigned int modulo(unsigned int a, unsigned int b);
extern unsigned int diveide(unsigned int a, unsigned int b);
extern unsigned int mul(unsigned int a, unsigned int b);

struct fileManager{
    char fileName[11]; // file name
    char filestatus; // s - script, t - text, e - empty
    char* filepointer; // pointer to file
    unsigned int fileSize;
} fileManager;

extern unsigned int LCD_roll; // global variable for LCD roll
extern struct fileManager ScriptPtrArr[10]; // array of fileManager

extern void sysConfig(void);
extern void delay(unsigned int);
extern void enterLPM(unsigned char);
extern void enable_interrupts();
extern void disable_interrupts();
extern void setTA0CCR0(int set);

extern void loadInToMem();


extern void disable_TimerA_ultrasonic();
extern void enable_Timer_ultrasonic();
extern void enable_TimerA1_servo();
extern void disable_TimerA1_servo();
extern void enable_TimerA0();
extern void disable_TimerA0();



extern void enable_send_to_pc();

extern void enable_ADC10LDR1();
extern void enable_ADC10LDR1();
extern unsigned int scanLDR1();
extern unsigned int scanLDR2();

extern void writeTimerTimeLCD(int count);




extern void printToRGB(int ToPrint);



//       LCD
extern void lcd_init();
extern void lcd_cmd(unsigned char);
extern void lcd_data(unsigned char);
extern void lcd_strobe();
extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int);
extern void lcd_puts(const char * s);

#ifdef CHECKBUSY
    #define LCD_WAIT lcd_check_busy()
#else
    #define LCD_WAIT DelayMs(5)
#endif

#define LCD_EN(a)   (!a ? (P2OUT&=~BIT1) : (P2OUT|=BIT1)) // P2.1 is lcd enable pin
#define LCD_EN_DIR(a)   (!a ? (P2DIR&=~BIT1) : (P2DIR|=BIT1)) // P2.1 pin direction

#define LCD_RS(a)   (!a ? (P2OUT&=~BIT3) : (P2OUT|=BIT3)) // P2.3 is lcd RS pin
#define LCD_RS_DIR(a)   (!a ? (P2DIR&=~BIT3) : (P2DIR|=BIT3)) // P2.3 pin direction
  
#define LCD_RW(a)   (!a ? (P2OUT&=~BIT5) : (P2OUT|=BIT5)) // P2.5 is lcd RW pin
#define LCD_RW_DIR(a)   (!a ? (P2DIR&=~BIT5) : (P2DIR|=BIT5)) // P2.5 pin direction

#define LCD_DATA_OFFSET 0x04 //data pin selection offset for 4 bit mode, variable range is 0-4, default 0 - Px.0-3, no offset

#define FOURBIT_MODE    0x0
#define EIGHTBIT_MODE   0x1
#define LCD_MODE        FOURBIT_MODE
   
#define OUTPUT_PIN      1
#define INPUT_PIN       0
#define OUTPUT_DATA     (LCD_MODE ? 0xFF : (0x0F << LCD_DATA_OFFSET))
#define INPUT_DATA      0x00

#define LCD_STROBE_READ(value)  LCD_EN(1), \
                asm("nOp"), asm("Nop"), \
                value=LCD_DATA_READ, \
                LCD_EN(0)

#define lcd_cursor(x)       lcd_cmd(((x)&0x7F)|0x80)
#define lcd_clear()         lcd_cmd(0x01)
#define lcd_putchar(x)      lcd_data(x)
#define lcd_goto(x)         lcd_cmd(0x80+(x))
#define lcd_cursor_right()  lcd_cmd(0x14)
#define lcd_cursor_left()   lcd_cmd(0x10)
#define lcd_display_shift() lcd_cmd(0x1C)
#define lcd_home()          lcd_cmd(0x02)
#define cursor_off          lcd_cmd(0x0C)
#define cursor_on           lcd_cmd(0x0F)
#define lcd_function_set    lcd_cmd(0x3C) // 8bit,two lines,5x10 dots
#define lcd_new_line        lcd_cmd(0xC0)

#endif
