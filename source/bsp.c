#include  "../header/bsp_430G2553.h"
#include  "../header/halGPIO.h"

struct fileManager ScriptPtrArr[10] = {{{0}, 'e', 0, 0}, 
                            {{0}, 'e', 0, 0}, 
                            {{0}, 'e', 0, 0}, 
                            {{0}, 'e', 0, 0}, 
                            {{0}, 'e', 0, 0}, 
                            {{0}, 'e', 0, 0}, 
                            {{0}, 'e', 0, 0}, 
                            {{0}, 'e', 0, 0}, 
                            {{0}, 'e', 0, 0},
                            {{0}, 'e', 0, 0}};
unsigned int num_of_files = 0;

//-----------------------------------------------------------------------------  
//           GPIO configuration
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


    // PushButtons Setup
  PBsArrPortSel &= ~0x81;
  PBsArrPortDir &= ~0x81;
  PBsArrIntEn &= ~0x81;              // enable interrupts on PB0 and PB3
  PBsArrIntEdgeSel |= 0x81;  	     // pull-up mode
  PBsArrIntPend &= ~0xFF;            // clear pending interrupts

  
  //SERVO
  ServoSEL             |= BIT4;
  ServoDIR             |= BIT4;
  ServoOUT             &= ~BIT4;

  //ultrasonic
  ultrasonicSEL        |= BIT2 + BIT6;
//   ultrasonicSEL        &= ~BIT7;
//   ultrasonicSEL2       &= ~(BIT6 +BIT7);
  ultrasonicSEL2       &= ~BIT6;
  ultrasonicOUT        &= ~BIT6;
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
//            ADC configuration
//-------------------------------------------------------------------------------------
void ADCconfig(void){
	
	ADC10CTL0 &= ~0x0002;// enc off
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE;   //set ref 0-3v, sample rate= 8*(1/2^20)
}              

//-------------------------------------------------------------------------------------
//            UART configuration
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
             
             
//-------------------------------------------------------------------------------------
//            File mode configuration
//-------------------------------------------------------------------------------------
void FileModeConfig(void){
    int count = -1;
    int idx = 0;

    while(count < 10)
    {
        count++;
        char* status_ptr = (char *) (0x1011 + MetaDataSize*count);
        if(*status_ptr != 's' || *status_ptr != 't'){
            break;
        }
        
    }

    while(idx <= count){
        ScriptPtrArr[idx].filepointer = (char *) (0xF600 + MetaDataSize*idx);
        ScriptPtrArr[idx].fileSize = (unsigned int *) (0x1014 + MetaDataSize*idx);
        ScriptPtrArr[idx].fileStatus = (char *) (0x1011 + MetaDataSize*idx);
        NameFlashToRam(idx);
        idx++;
    }

    while (idx < 10){
        ScriptPtrArr[idx].filepointer = 0;
        ScriptPtrArr[idx].fileSize = 0;
        ScriptPtrArr[idx].fileStatus = 'e';
        ScriptPtrArr[idx].fileName[0] = '\0';
    }
    num_of_files = count;

}

void NameFlashToRam(int idx){
    int i;
    if (ScriptPtrArr[idx].fileStatus == 's' || ScriptPtrArr[idx].fileStatus == 't'){
        char* flash_ptr = (char *) (0x1000 + (idx * MetaDataSize));
                
        for (i = 0; i < 10; i++)
            ScriptPtrArr[idx].fileName[i] = flash_ptr[i];
        
        ScriptPtrArr[idx].fileName[10] = '\0';
    }
        
        
    
}
