#include  "../header/halGPIO.h"     // private library - HAL layer

//global var
unsigned int start = 0;
unsigned int end = 0;
unsigned int dist = 0;
unsigned int first_time = 1;
int loc_angel;

unsigned char TX_to_send[3] ;
int TXindex = 0;
int maskDist = 100;
char input[61];
int input_slot = 0;
int indexfile = 0;
int state_or_num = 1;
unsigned int LCD_roll = 0; 
int size_left = 0;
unsigned int isScrolling = 0;
unsigned int Status_name_data = 0;
unsigned int memLoad = 0;
unsigned int ptr_mov = 0;


//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	TIMERconfig();
	ADCconfig();
	UARTconfig();
    FileModeConfig();

}

//******************************************************************
// initialize the LCD
//******************************************************************
void lcd_init(){
  
    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
        else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
        LCD_DATA_DIR |= OUTPUT_DATA;
        LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);
        
    DelayMs(15);
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
                LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
        else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots 

    lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x1); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
        enable_interrupts();
        
}
//******************************************************************
// send a command to the LCD
//******************************************************************
void lcd_cmd(unsigned char c){
  
    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
                LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
                LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}
//******************************************************************
// send data to the LCD
//******************************************************************
void lcd_data(unsigned char c){
        
    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
                LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;  
        lcd_strobe();
                LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
                LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
          
    LCD_RS(0);
}
//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){
    unsigned char i;
        for(i=cnt ; i>0 ; i--) asm("NoP"); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
// Delay msec functions
//******************************************************************
void DelayMs(unsigned int cnt){
    unsigned char i;
        for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
// lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
//  asm("NOP");
  asm("nop");
  LCD_EN(0);
}
//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;
	for(i=t; i>0; i--);
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03) 
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}
//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){
  
	while(*s)
		lcd_data(*s++);
}
//******************************************************************
// setTA0CCR0
//******************************************************************

void setTA0CCR0(int set){
    TA0CCR0 = set;
}

//---------------------------------------------------------------------
//            enable Timer ultrasonic
//---------------------------------------------------------------------
void enable_Timer_ultrasonic(){
//   TA0CTL |= MC_1;
//   TA0CCTL1 |= OUTMOD_7;  // pwm for ultrasonic

//   TA1CTL |= MC_1;
//   TA1CCTL1 |= CM_3 + CAP + CCIS_1 + CCIE + SCS;

    TA0CTL |= MC_1;
    TA0CCTL1 |= OUTMOD_7;            // trigger generation on TA0.1
    // Do NOT reset or stop TA1. Only arm CCR1 capture.
    TA1CCTL1 |= CM_3 + CAP + CCIS_1 + CCIE + SCS;
}

//---------------------------------------------------------------------
//            disable TimerA ultrasonic
//---------------------------------------------------------------------
void disable_TimerA_ultrasonic(){
//     enable_TimerA1_servo();
//    TA1CTL &= ~MC_3;
//   TA0CTL &= ~MC_3;
//   TA0CCTL1 &= ~OUTMOD_7;
//    TA1CCTL1 &= ~(CM_3 + CAP + CCIS_1 + CCIE + SCS);

   TA0CTL &= ~MC_3;                 // stop trigger timer
    TA0CCTL1 &= ~OUTMOD_7;
    TA1CCTL1 &= ~(CM_3 + CAP + CCIS_1 + CCIE + SCS);  // disarm capture
    // Leave TA1 running for CCR2 PWM (servo)

}
//---------------------------------------------------------------------
//            Enable TimerA1 for servo
//---------------------------------------------------------------------
void enable_TimerA1_servo(){
//   TA1CTL |= MC_1 + TACLR;
//   TA1CCTL2 = OUTMOD_7;

     TA1CTL |= MC_1;        // no TACLR during measurement
    TA1CCTL2 = OUTMOD_7;
}
//---------------------------------------------------------------------
//            Enable TimerA1 for servo
//---------------------------------------------------------------------
void disable_TimerA1_servo(){
  TA1CTL &= ~MC_3;
  TA1CCTL2 &= ~OUTMOD_7;
}

//---------------------------------------------------------------------
//            Enable TimerA0
//---------------------------------------------------------------------
void enable_TimerA0(){
  TA0CTL |= MC_1 +TACLR;
  TA0CCTL0 |= CCIE;
}
//---------------------------------------------------------------------
//            Disable TimerA0
//---------------------------------------------------------------------
void disable_TimerA0(){
  TA0CTL &= ~(MC_1 + TAIE);
  TA0CCTL0 &= ~CCIE;
}
//---------------------------------------------------------------------
//            Enable ADC10
//---------------------------------------------------------------------
void enable_ADC10LDR1(){
  ADC10CTL0 &= ~ENC;
  ADC10CTL1 = INCH_0+ADC10SSEL_3; //A0,SMCLK
  ADC10AE0 |= BIT0;
  ADC10CTL0 |= ENC + ADC10SC;             // Start sampling
}
//---------------------------------------------------------------------
//            Enable ADC10
//---------------------------------------------------------------------
void enable_ADC10LDR2(){
  ADC10CTL0 &= ~ENC;
  ADC10CTL1 = INCH_3+ADC10SSEL_3; //A3,SMCLK
  ADC10AE0 |= BIT3;
  ADC10CTL0 |= ENC + ADC10SC;             // Start sampling
}
//---------------------------------------------------------------------
//            enable_send_to_pc
//---------------------------------------------------------------------
void enable_send_to_pc(){
    TXindex = 0;
    IE2 |= UCA0TXIE;
}
//---------------------------------------------------------------------
//            scan first LDR
//---------------------------------------------------------------------
unsigned int scanLDR1(){
    unsigned int voltage_level;

    enable_ADC10LDR1();
    __bis_SR_register(LPM0_bits + GIE);
    voltage_level = ADC10MEM;
    ADC10CTL0 &= ~ADC10ON;
    return voltage_level;
}
//---------------------------------------------------------------------
//            scan second LDR
//---------------------------------------------------------------------
unsigned int scanLDR2(){
    unsigned int voltage_level;

    enable_ADC10LDR2();
    __bis_SR_register(LPM0_bits + GIE);
    voltage_level = ADC10MEM;
    ADC10CTL0 &= ~ADC10ON;
    return voltage_level;
}
//---------------------------------------------------------------------
//            mul 2 unsigned int
//---------------------------------------------------------------------
unsigned int mul(unsigned int a, unsigned int b) {
    unsigned int result = 0;

    while (b != 0) {
        if (b & 1) {
            result += a;
        }
        a <<= 1;
        b >>= 1;
    }
    return result;
}
//---------------------------------------------------------------------
//            divide 2 unsigned int (a/b)
//---------------------------------------------------------------------
unsigned int diveide(unsigned int a, unsigned int b) {
    unsigned int result = 0;
    if (b == 0) {
        return 0;
    }

    while (a >= b) {
        unsigned int temp = b;
        unsigned int multiplier = 1;
        while ((temp << 1) <= a) {
            temp <<= 1;
            multiplier <<= 1;
        }
        a -= temp;
        result += multiplier;
    }

    return result;
}
//---------------------------------------------------------------------
//            modulo 2 unsigned int (a%b)
//---------------------------------------------------------------------
unsigned int modulo(unsigned int a, unsigned int b) {
    if (b == 0)
        return 0;
    if (a < b)
        return a;

    while (a >= b) {
        unsigned int temp = b;
        while ((temp << 1) <= a) {
            temp <<= 1;
        }
        a -= temp;
    }
    return a;
}

// //-----------------------------------------------------------------------------
// //            loading file name onto flash
// //-----------------------------------------------------------------------------

// void loadNameToMem(){  // load all script (input ,until input_slot-1 ,into memLoad place)

//     char *Flash_ptr;                                                // Flash pointer
//     unsigned int i;
//     if( num_of_files == 10){
//         lcd_puts("10 files loaded");
//         delayMs(500);
//         lcd_clear();
//         return;
//     }

//     Flash_ptr = (char *) (0x1000 + MetaDataSize*num_of_files);              // Initialize Flash pointer
//     FCTL1 = FWKEY + ERASE;                                          // Set Erase bit
//     FCTL3 = FWKEY;                                                  // Clear Lock bit
//     *Flash_ptr = 0;                                                 // Dummy write to erase Flash segment
//     FCTL1 = FWKEY + WRT;                                            // Set WRT bit for write operation
//     for (i=0; i < indexfile-1; i = i+2){      
//         if (Flash_ptr >= TheForbiddenSegment){
//             lcd_puts("No room for name");
//             delayMs(1000);
//             lcd_clear();
//             break;
//         }
        
//         // Write value to flash
//         if(input[i] < 58){
//             char big = input[i];
//             big<<=4;
//             if(input[i+1] < 58)
//                 *Flash_ptr++ = big + input[i+1];
//             else
//                 *Flash_ptr++ = big + input[i+1] + 10;
//         }
//         else{
//             char big = input[i];
//             big<<=4;
//             if(input[i+1] < 58)
//                 *Flash_ptr++ = big + input[i+1];
//             else
//                 *Flash_ptr++ = big + input[i+1]+ 10;
//         }
//     }
//     FCTL1 = FWKEY;                                                  // Clear WRT bit
//     FCTL3 = FWKEY + LOCK;                                           // Set LOCK bit
    
// }

// //---------------------------------------------------------------------
// //            loading file data onto flash
// //---------------------------------------------------------------------

// void loadDataToMem(){  // load all script (input ,until input_slot-1 ,into memLoad place)

//     char *Flash_ptr;                                                // Flash pointer
//     unsigned int i;
//     unsigned int size = 0;
//     if( num_of_files == 10){
//         lcd_puts("10 files loaded");
//         delayMs(1000);
//         lcd_clear();
//         return;
//     }


//     for ( i = 0; i < num_of_files; i++)
//         size = size + *((unsigned int *) (0x1014 + MetaDataSize * i));

//     Flash_ptr = (char *) (0xF600 + size);              // Initialize Flash pointer
//     i = 0;
//     size = 0;
//     FCTL1 = FWKEY + ERASE;                                          // Set Erase bit
//     FCTL3 = FWKEY;                                                  // Clear Lock bit
//     *Flash_ptr = 0;                                                 // Dummy write to erase Flash segment
//     FCTL1 = FWKEY + WRT;                                            // Set WRT bit for write operation
//     for (i=0; i < indexfile-1; i = i+2){      
        
//         if (Flash_ptr >= FileEnding){
//             lcd_puts("No room for data");
//             delayMs(500);
//             lcd_clear();
//             break;
//         }
        
//         // Write value to flash
//         if(input[i] < 58){
//             unsigned int big = input[i] - '0';
//             big<<=4;
//             if(input[i+1] < 58)
//                 *Flash_ptr++ = big + input[i+1] - '0';
//             else
//                 *Flash_ptr++ = big + input[i+1] - 'a' + 10;
//         }
//         else{
//             unsigned int big = input[i] - 'a' + 10;
//             big<<=4;
//             if(input[i+1] < 58)
//                 *Flash_ptr++ = big + input[i+1] - '0';
//             else
//                 *Flash_ptr++ = big + input[i+1] - 'a' + 10;
//         }
//         size++;
//     }
//     FCTL1 = FWKEY;                                                  // Clear WRT bit
//     FCTL3 = FWKEY + LOCK;                                           // Set LOCK bit

//     char** file_ptr_ptr = (char **) (0x1012 + MetaDataSize*num_of_files);
//     *file_ptr_ptr =  Flash_ptr - size;
//     unsigned int* size_ptr = (unsigned int *) (0x1014 + MetaDataSize*num_of_files);
//     *size_ptr = size;
//     num_of_files++;
// }

//---------------------------------------------------------------------
//            loading script data onto flash
//---------------------------------------------------------------------
void loadInToMem(){  // load all script (input ,until input_slot-1 ,into memLoad place)
    char *Flash_ptr;                                                // Flash pointer
    unsigned int i;
    unsigned int size_sum = 0;
    unsigned int size = 0;
    for ( i = 0; i < num_of_files; i++)
        size_sum = size_sum + *((unsigned int *) (0x100E + MetaDataSize * i));

    Flash_ptr = (char *) FileStarting + size_sum;                             // Initialize Flash pointer
    size = 0;
    FCTL1 = FWKEY ;                                          // Set Erase bit
    FCTL3 = FWKEY;                                                  // Clear Lock bit
    *Flash_ptr = 0;                                                 // Dummy write to erase Flash segment
    FCTL1 = FWKEY + WRT;                                            // Set WRT bit for write operation
    for (i=0; i < indexfile-1; i = i+2){                            // Write value to flash
        if (Flash_ptr >= FileEnding){
            lcd_puts("No room for data");
            DelayMs(1000);
            lcd_clear();
            break;
        }
        if(input[i] < 58){
            unsigned int big = input[i] - '0';
            big<<=4;
            if(input[i+1] < 58)
                *Flash_ptr++ = big + input[i+1] - '0';
            else
                *Flash_ptr++ = big + input[i+1] - 'a' + 10;
        }
        else{
            unsigned int big = input[i] - 'a' + 10;
            big<<=4;
            if(input[i+1] < 58)
                *Flash_ptr++ = big + input[i+1] - '0';
            else
                *Flash_ptr++ = big + input[i+1] - 'a' + 10;
        }
    }

    FCTL1 = FWKEY;                                                  // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                                           // Set LOCK bit 

    loadNameToMem();
    loadMetadataToMem(indexfile / 2, Flash_ptr);
    num_of_files++;
}

//---------------------------------------------------------------------
//            loading script name onto flash
//---------------------------------------------------------------------
void loadNameToMem(){  // load all script (input ,until input_slot-1 ,into memLoad place)                                              // Flash pointer
    unsigned int i;

    char* name_ptr = (char *) (0x1000 + MetaDataSize*num_of_files);                             // Initialize Flash pointer
    FCTL1 = FWKEY;                                          // Set Erase bit
    FCTL3 = FWKEY;                                                  // Clear Lock bit
    *name_ptr = 0;                                                 // Dummy write to erase Flash segment
    FCTL1 = FWKEY + WRT;                                            // Set WRT bit for write operation
    for (i=0; i < 19; i = i+2){                            // Write value to flash
        if (name_ptr >= 0x10BE){
            lcd_puts("No room for name");
            DelayMs(1000);
            lcd_clear();
            break;
        }
        if(input[i] < 58){
            unsigned int big = input[i] - '0';
            big<<=4;
            if(input[i+1] < 58)
                *name_ptr++ = big + input[i+1] - '0';
            else
                *name_ptr++ = big + input[i+1] - 'a' + 10;
        }
        else{
            unsigned int big = input[i] - 'a' + 10;
            big<<=4;
            if(input[i+1] < 58)
                *name_ptr++ = big + input[i+1] - '0';
            else
                *name_ptr++ = big + input[i+1] - 'a' + 10;
        }
    }

    FCTL1 = FWKEY;                                                  // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                                           // Set LOCK bit 
}

void loadMetadataToMem(unsigned int size, char* Flash_ptr){  // load all script (input ,until input_slot-1 ,into memLoad place)                                              // Flash pointer

    FCTL1 = FWKEY;                                          // Set Erase bit
    FCTL3 = FWKEY;                                                  // Clear Lock bit
    FCTL1 = FWKEY + WRT;                                            // Set WRT bit for write operation
    
    char** file_ptr_ptr = (char **) (0x100C + MetaDataSize*num_of_files);
    *file_ptr_ptr =  Flash_ptr - size;
    unsigned int* size_ptr = (unsigned int *) (0x100E + MetaDataSize*num_of_files);
    *size_ptr = size;

    char* status_ptr = (char *) (0x100A + MetaDataSize*num_of_files);
    *status_ptr = 's';

    FCTL1 = FWKEY;                                                  // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                                           // Set LOCK bit 
}

//---------------------------------------------------------------------
//            printing scripts onto LCD
//---------------------------------------------------------------------

void showScript(unsigned int idx){
    if (size_left < 16)
        printLcdTop((char *) (0x100C + MetaDataSize*LCD_roll) + 32 * ptr_mov ,size_left);

    else if (size_left < 32)
    {
        printLcdTop((char *) (0x100C + MetaDataSize*LCD_roll) + 32 * ptr_mov ,16);
        printLcdBottom((char *) (0x100C + MetaDataSize*LCD_roll) + 16 + 32 * ptr_mov,size_left - 16);
    }
    else{
        printLcdTop((char *) (0x100C + MetaDataSize*LCD_roll) + 32 * ptr_mov ,16);
        printLcdBottom((char *) (0x100C + MetaDataSize*LCD_roll) + 16 + 32 * ptr_mov,16);
    }
    size_left = size_left - 32;
}

void printLcdTop(char *toPrint, unsigned int n){
    lcd_home();
    int i;
    for(i = 0 ; i<n; i++){
        lcd_data(toPrint[i]);
    }
}
void printLcdBottom(char *toPrint, unsigned int n){
    lcd_new_line;
    int i;
    for(i = 0; i<n; i++){
        lcd_data(toPrint[i]);
        }
}


//*********************************************************************
//            Port2 Interrupt Service Rotine
//*********************************************************************
#pragma vector=PORT2_VECTOR
	__interrupt void PB_handler(void){
   
	delay(debounceVal);

	if(PBsArrIntPend & PB0){
		PBsArrIntPend &= ~PB0;
        lcd_clear();
        printLcdTop((0x1000 + MetaDataSize*LCD_roll),10);
        printLcdBottom((0x1000 + MetaDataSize*((LCD_roll + 1) % (num_of_files))),10);
        LCD_roll ++;
        if(LCD_roll > (num_of_files - 1))
            LCD_roll = 0;
        DelayMs(500);
        isScrolling = 1;
        size_left = 0;
        ptr_mov = 0;
	}
    else if(PBsArrIntPend & PB1){
        PBsArrIntPend &= ~PB1;
        lcd_clear();
        if (isScrolling == 1){
            size_left = *((unsigned int *) (0x100E + MetaDataSize*LCD_roll));
            showScript(LCD_roll);
            isScrolling = 2;
            
        }
        else if (isScrolling == 2 && size_left > 0){
            showScript(LCD_roll);
            isScrolling = 2;
            ptr_mov++;

        }
        else if (isScrolling == 2 && size_left <= 0){
            size_left = *((unsigned int *) (0x100E + MetaDataSize*LCD_roll));
            showScript(LCD_roll);
            ptr_mov = 0;
        }
    }
    //enterLPM(lpm_mode); 
    LPM0_EXIT;     
}




//*********************************************************************
//            TimerA0 Interrupt Service Rotine
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
  LPM0_EXIT;
}
//*********************************************************************
//            TimerA1 Interrupt Service Rotine
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) TIMER1_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(TA1IV){
  case 0x2:
      if(TA1CCTL1 & CCI){
          // rising edge
          start = TA1CCR1;
          first_time = 0;
      }
      else{
          // Falling edge
          if(!first_time){
              end = TA1CCR1;
              first_time = 1;
              if(end < start){
                  start = twentyFivemsec - start;
                  dist = start + end;
              }
              else{
                  dist = end - start;
              }
              LPM0_EXIT;
          }

      }
    break;
  case 0x4:
    break;
  case 0xa:
    break;
  default:
    break;
  }
  
}



//*********************************************************************
//            ADC10 Interrupt Service Rotine
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC10_VECTOR))) ADC10_ISR (void)
#else
#error Compiler not supported!
#endif
{
  __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}


//---------------------------------------------------------------------
//            USCI A0/B0 Transmit ISR
//---------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  if (TXindex == 3){                // TX over?
      IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
      TXindex = 0;
  }else{
      UCA0TXBUF = TX_to_send[TXindex];
      TXindex++;

  }
}

//---------------------------------------------------------------------
//            USCI A0/B0 Receive ISR
//---------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{

  input[input_slot] = UCA0RXBUF;
  input_slot++;
  indexfile = input_slot;
  if (input[input_slot-1] == '\n')
  {
      input[input_slot-1] = 'k';
      input_slot = 0;
      if(state_or_num){                     //state mode
          switch(input[0]){
              case '1' :
                  state = state1;
                  break;
              case '2' :
                  state = state2;
                  state_or_num = 0;
                  break;
              case '3' :
                  state = state3;
                  break;
              case '4' :
                  state = state4;
                  break;
              case '5' :
                  state = state5;
                  state_or_num = 0;
                  break;
              case '6' :
                  state = state6;
                  state_or_num = 0;
                  break;
              case '7' :
                  state = state7;
                  break;
              case '8' :
                  state = state8;
                  state_or_num = 0;
                  break;
              case '9' :
                  state = state9;
                  break;
              default:
                  break;
          }
      }
      else{
          if(state == state6){
              maskDist = atoi(input);
          }
          else if(state == state2 ||state == state10){
              loc_angel = atoi(input);
          }
          else if(state == state5){

            //   if(Status_name_data == 0){
            //     Status_name_data = 1;
            //     FCTL1 = FWKEY + ERASE;                                          // Set Erase bit
            //     FCTL3 = FWKEY;                                                  // Clear Lock bit
            //     FCTL1 = FWKEY + WRT;

            //     *((char *) (0x100B + MetaDataSize*num_of_files)) = input[0];

            //     FCTL1 = FWKEY;                                                  // Clear WRT bit
            //     FCTL3 = FWKEY + LOCK;                                           // Set LOCK bit
            //   }
            //   else if(Status_name_data == 1){
            //     Status_name_data = 2;
            //     loadNameToMem();
            //   }

            //   else if (Status_name_data == 2 && !memLoad){
            //     memLoad = atoi(input);
            //     Status_name_data = 0;
            //   }
              
              if(!memLoad)
                  memLoad = atoi(input);
          }
          else if(state == state8){
              memLoad = atoi(input);
          }
          state_or_num = 1;
          
      }

    
//---------------------------------------------------------------------
//            Exit from a given LPM
//---------------------------------------------------------------------
      switch(lpm_mode){
          case mode0:
           LPM0_EXIT; // must be called from ISR only
           break;

          case mode1:
           LPM1_EXIT; // must be called from ISR only
           break;

          case mode2:
           LPM2_EXIT; // must be called from ISR only
           break;

                  case mode3:
           LPM3_EXIT; // must be called from ISR only
           break;

                  case mode4:
           LPM4_EXIT; // must be called from ISR only
           break;
      }
  };
};

