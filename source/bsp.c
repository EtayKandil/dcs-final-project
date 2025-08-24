#include  "../header/bsp_430G2553.h"

//-----------------------------------------------------------------------------  
//           GPIO congiguration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
   
  if (CALBC1_1MHZ==0xFF)                    // If calibration constant erased
  {
    while(1);                               // do not load, trap CPU!!
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;
  FCTL2 = FWKEY + FSSEL0+ FN1;              // MCLK/3 for Flash timing gen
  /*
  // PushButtons Setup
  PBsArrPortSel &= ~0x01;
  PBsArrPortDir &= ~0x01;
  PBsArrIntEdgeSel |= 0x01;  	     // pull-up mode
  PBsArrIntPend &= ~0x01;            // clear pending interrupts
  */

  
  //SERVO
  ServoSEL             |= BIT4;
  ServoDIR             |= BIT4;
  ServoOUT             &= ~BIT4;

  //ultrasonic
  ultrasonicSEL        |= BIT2 + BIT6;
  ultrasonicSEL        &= ~BIT7;
  ultrasonicSEL2       &= ~(BIT6 +BIT7);
  ultrasonicOUT        &= ~(BIT6);
  ultrasonicDIR        |= BIT6;
  ultrasonicDIR        &= ~BIT2;
  ultrasonicIN         |= BIT2;


  // LDR Setup
  LDRArrPortIN         |=  (BIT0 +BIT3);
  LDRArrPortSel        &= ~(BIT0 +BIT3);
  LDRArrPortDir        &= ~(BIT0 +BIT3);
   
  //LCD Setup 
   LCD_DATA_WRITE      &= ~0xFF;
   LCD_DATA_DIR        |= 0xF0;
   LCD_DATA_SEL        &= ~0xF0;
   LCD_CTL_SEL         &= ~(BIT1+BIT3+BIT5);
  
  _BIS_SR(GIE);                     // enable interrupts globally
}                             
//------------------------------------------------------------------------------------- 
//            Timers congiguration 
//-------------------------------------------------------------------------------------
void TIMERconfig(void){

    TA1CTL = TASSEL_2 +TACLR+ ID_3;  // timer is off SMCLK /8
	TA0CTL = TASSEL_2+ TACLR +ID_3;  // timer is off SMCLK /8

	TA0CCR0 = Sixtymsec;                // value of 60 msec
    TA0CCR1 = 3;           // value of 22 micro sec


    TA1CCR0 = twentyFivemsec;                // value of 25 msec
} 
//------------------------------------------------------------------------------------- 
//            ADC congiguration 
//-------------------------------------------------------------------------------------
void ADCconfig(void){
	
	ADC10CTL0 &= ~0x0002;// enc off
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE;   //set ref 0-3v, sample rate= 8*(1/2^20)
}              

//-------------------------------------------------------------------------------------
//            UART congiguration
//-------------------------------------------------------------------------------------
void UARTconfig(void){
    UARTArrPortSel       |= BIT1 + BIT2;
    UARTArrPortSel2      |= BIT1 + BIT2;
    //  UARTArrPortDir       |= BIT1 + BIT2; //RXLED + TXLED
    //  UARTArrPortOUT       &= ~(BIT1 + BIT2);

    DCOCTL = 0;                 //select lowest DC0X and mod x setings
    BCSCTL1 = CALBC1_1MHZ;      //set DC0
    DCOCTL = CALDCO_1MHZ;


    UCA0CTL1             |= UCSSEL_2;
    UCA0BR0              = 104;
    UCA0BR1              = 0x00;
    UCA0MCTL             = UCBRS0;
    UCA0CTL1             &= ~UCSWRST;
    IE2                  |= UCA0RXIE;
}
             
             
            
  

// ---------------- Metadata address calculators ----------------
char *meta_name_ptr(unsigned int idx){
    return (char *)(METADATA_BASE + (idx * METADATA_ENTRY_SIZE) + META_OFF_NAME);
}
char *meta_status_ptr(unsigned int idx){
    return (char *)(METADATA_BASE + (idx * METADATA_ENTRY_SIZE) + META_OFF_STATUS);
}
char **meta_data_ptr_ptr(unsigned int idx){
    return (char **)(METADATA_BASE + (idx * METADATA_ENTRY_SIZE) + META_OFF_DATAPTR);
}
unsigned int *meta_size_ptr(unsigned int idx){
    return (unsigned int *)(METADATA_BASE + (idx * METADATA_ENTRY_SIZE) + META_OFF_SIZE);
}

// ---------------- Metadata getters (reads) ----------------
char meta_get_status(unsigned int idx){
    return *meta_status_ptr(idx);
}
unsigned int meta_get_size(unsigned int idx){
    return *meta_size_ptr(idx);
}
char *meta_get_data_ptr(unsigned int idx){
    return *meta_data_ptr_ptr(idx);
}
void meta_get_name(unsigned int idx, char out[11]){
    char *p = meta_name_ptr(idx);
    unsigned int i;
    for(i=0;i<META_NAME_LEN;i++) out[i] = p[i];
    out[META_NAME_LEN] = '\0';
}

// ---------------- Data offset computations ----------------
unsigned int meta_compute_data_offset_until(unsigned int idx){
    unsigned int i;
    unsigned int total = 0;
    for(i = 0; i < idx && i < METADATA_MAX_FILES; i++){
        total += meta_get_size(i);
    }
    return total;
}
char *meta_compute_data_write_ptr(unsigned int idx){
    return (char *)(FILE_DATA_BASE + meta_compute_data_offset_until(idx));
}





