#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer

char counterToPrint[5];
// unsigned int objectDist[2][60];
//                                   5   10  15  20  25  30  35  40  45  50
unsigned int lightToDist[2][10] = {{600,650,692,734,776,818,860,902,950,1023},
                                   {540,600,635,670,705,740,775,810,1000,1000}};
int delayS = 500;


//---------------------------------------------------------------------
//            LDR config
//---------------------------------------------------------------------
void LDRconfig(){
    int i;
    int j;
    lcd_clear();
    lcd_home();
    servo_Tolocation(90,750);
    for(i = 0; i < 10; i++){
        lcd_puts("move flashlight");
        lcd_new_line;
        lcd_puts("scan in 3 sec");
        timerA0On(1000);
        lcd_home();
        lcd_new_line;
        lcd_puts("scan in 2 sec");
        timerA0On(1000);
        lcd_home();
        lcd_new_line;
        lcd_puts("scan in 1 sec");
        timerA0On(1000);
        lcd_clear();
        lcd_home();
        lightToDist[0][i] = scanLDR1();
        // lightToDist[1][i] = scanLDR2();
    }
}

void intNumToString(unsigned int num, unsigned int len){
  unsigned int num2 = num;
  int counter = 0;
  int counter2 = len;
  while(num2!=0){
    num2 = diveide(num2,10);
    counter++;
  }
  while(counter2>0){
      counterToPrint[counter2-1] = (modulo(num,10)+'0');
      num = diveide(num,10);
      counter--;
      counter2--;
  }
}

void sendToPC(unsigned int num){
    intNumToString(num,3);
    timerA0On(15);
    TX_to_send[0] = counterToPrint[0];
    TX_to_send[1] = counterToPrint[1];
    TX_to_send[2] = counterToPrint[2];
    enable_send_to_pc();
}
void sendEndSigToPC(){
    timerA0On(15);
    TX_to_send[0] = 'x';
    TX_to_send[1] = 'x';
    TX_to_send[2] = 'x';
    enable_send_to_pc();
}
void sendEndSigScriptToPC(){
    timerA0On(15);
    TX_to_send[0] = 'z';
    TX_to_send[1] = 'z';
    TX_to_send[2] = 'z';
    enable_send_to_pc();
}



//---------------------------------------------------------------------
//            timerA0On
//---------------------------------------------------------------------
void timerA0On(int delay){
  while(delay >500){
      setTA0CCR0(Set0xffff);
      enable_TimerA0();
      enterLPM(0);
      disable_TimerA0();
      delay -= 500;
  }
  if (delay != 0){
      int temp2 = delay;
      delay = delay<<7;          // we did y*128 insted of y*131 = y*0xffff/500
      TA0CCR0 = delay + temp2 + temp2 + temp2;
      enable_TimerA0();
      enterLPM(0);
      disable_TimerA0();
  }
  setTA0CCR0(Sixtymsec);
}
//---------------------------------------------------------------------
//            servo sweep
//---------------------------------------------------------------------
void servo_sweep(unsigned int l, unsigned int r){ //l<r
    servo_Tolocation(l,750);
    unsigned int i = l>>1;
    unsigned int max = r>>1;
    unsigned int tmp,tmp2;

    while(i<90 && i <= max){
        int loc = i<<1;
        servo_Tolocation(loc,250);
        
        switch(state){
            case state1:
                tmp = ultrasonic_measure();
                if( tmp <= maskDist ){
                    sendToPC(i);                    //send angle
                    sendToPC(tmp);                  //send val
                    sendToPC(0);                    //measure = 0 / light = 1
                }
                break;
            case state3:
                tmp = readLDRs();
                if(tmp > 0 ){
                    sendToPC(i);                    //send angle
 //                   sendToPC(ultrasonic_measure()); //send val
                    sendToPC(tmp);
                    sendToPC(1);                    //measure = 0 / light = 1
                }
                break;
            case state4:
                tmp = ultrasonic_measure();
                tmp2 = readLDRs();
                if(tmp2 > 0){
                    sendToPC(i);                    //send angle
                    sendToPC(tmp2);                  //send val
                    sendToPC(1);                    //measure = 0 / light = 1
                }
                else if( tmp <= maskDist ){
                    sendToPC(i);                    //send angle
                    sendToPC(tmp);                  //send val
                    sendToPC(0);                    //measure = 0 / light = 1
                }
                break;
            case state8:
                tmp = ultrasonic_measure();
                if( tmp <= maskDist ){
                    sendToPC(i);                    //send angle
                    sendToPC(tmp);                  //send val
                    sendToPC(0);                    //measure = 0 / light = 1
                }
                break;
        }
        i++;
    }
    servo_Tozero();
    sendEndSigToPC();

}


//---------------------------------------------------------------------
//            reset servo
//---------------------------------------------------------------------
void servo_Tozero(){
    // enable_TimerA1_servo();
    // TA1CCR2 = startServoVal;
    // timerA0On(750);
    // disable_TimerA1_servo();
    // enable_TimerA1_servo();
    // TA1CCR2 = startServoVal;
    // timerA0On(750);
    // disable_TimerA1_servo();

    enable_TimerA1_servo();
    TA1CCR2 = startServoVal;
    timerA0On(750);
}


//---------------------------------------------------------------------
//             servo to location
//---------------------------------------------------------------------
void servo_Tolocation(int loc, int sleep){
    int num = 0;
    if(loc>135){
        num = -40 + diveide(loc<<4,9);
    }else if(loc>90){
        num = 50+ diveide(mul(loc,10),9);
    }else if(loc>45){
        num = 30+ diveide(loc<<2,3);
    }else {
        num = 60+ diveide(loc<<1,3);
    }

    // int i;
    // for (i = 0; i < 4; i++){
    //     enable_TimerA1_servo();
    //     TA1CCR2 = num;
    //     timerA0On(sleep);
    //     disable_TimerA1_servo();
    // }

    enable_TimerA1_servo();
    TA1CCR2 = num;
    timerA0On(sleep);        // give time to reach position
    // Do NOT disable here; keep PWM on to hold angle.

}
//---------------------------------------------------------------------
//            ultrasonic measure
//---------------------------------------------------------------------
unsigned int ultrasonic_measure(){
    unsigned int to_return = 0;
    int k = 0;
    enable_Timer_ultrasonic();

    while(k < 4){
        int i = 0;
        unsigned int count = 0;
        while(i < 4){
            count += dist;
            if(dist!=0)
                i++;
        }
        count = count >> 5;
        unsigned int temp = count >> 3;
        temp += 4;
        to_return = to_return + count + temp;
        k++;
    }
    disable_TimerA_ultrasonic();
    to_return = to_return>>2;
    ///////////////////////////////////////////////////////////////
    intNumToString(to_return,5);
    lcd_home();
    lcd_puts(counterToPrint);
    ///////////////////////////////////////////////////////////////
    dist = 0;
    return to_return;

}

//---------------------------------------------------------------------
//            readLDR
//---------------------------------------------------------------------
unsigned int readLDRs(){
    unsigned int output_voltage1;
    unsigned int output_voltage2;

    lcd_clear();
    lcd_home();

    output_voltage1 = scanLDR1();
    // output_voltage2 = scanLDR2();

    if((output_voltage1 == 1023) /*&& (output_voltage2 == 1023)*/)
        return 0;
    unsigned int i = 0,j = 0;
    int notFound1 = 1,notFound2 = 1;
    while(i<10 && notFound1){
        if((lightToDist[0][i] < output_voltage1))
            i++;
        else{
            notFound1 = 0;
        }
    }
    // while(j<10 && notFound2){
    //     if((lightToDist[1][j] < output_voltage2))
    //         j++;
    //     else{
    //         notFound2 = 0;
    //     }
    // }
//////////////////////////////////////////////
    intNumToString(output_voltage1,5);
    lcd_puts(counterToPrint);
    // lcd_puts("-");
    // intNumToString(output_voltage2,5);
    // lcd_puts(counterToPrint);
    lcd_new_line;
    lcd_data(i+'0');
    // lcd_puts("-");
    // lcd_data(j+'0');
////////////////////////////////////////////////
    timerA0On(500);

    if(/*(i==j) &&*/(i != 10))
        return mul(i,5);
    else
        return 0;

}


//---------------------------------------------------------------------
//            telemeter
//---------------------------------------------------------------------
void telemeter(){
    enterLPM(0);
    servo_Tolocation(loc_angel,750);
    state  = state10;
    while(state == state10){
        sendToPC(ultrasonic_measure());
        timerA0On(200);
    }
}
//---------------------------------------------------------------------
//            set mask
//---------------------------------------------------------------------
void setMask(){
    enterLPM(0);
}
//---------------------------------------------------------------------
//            load script into flash
//---------------------------------------------------------------------
void loadScript(){
    enterLPM(0);
    enterLPM(0);
    enterLPM(0);
    if (is_text){
        loadTextToMem();
    }
    else{
        loadInToMem(); // load all script (input ,until input_slot-1 ,into memLoad place)
    }
    
    timerA0On(15);
    TX_to_send[0] = 'x';
    TX_to_send[1] = 'x';
    TX_to_send[2] = '0'+ num_of_files;
    enable_send_to_pc();
    state = state0;
    memLoad = 0;
}
//---------------------------------------------------------------------
//            play script from flash
//---------------------------------------------------------------------
void playScript(){
    enterLPM(0);
    unsigned int size_sum = 0;
    unsigned int i;
    char *Flash_ptr;
    for ( i = 0; i < (memLoad -1); i++)
        size_sum = size_sum + *((unsigned int *) (0x100E + MetaDataSize * i));

    Flash_ptr = (char *) FileStarting + size_sum;
    unsigned int x,command = 1;
    i = 0;
    x = *Flash_ptr;
    if((x<1) || (8<x))
        command = 0;
    lcd_data('a');
    while(i < 10 && command){
        switch(*Flash_ptr++){
            unsigned int tmp = (unsigned int)Flash_ptr;

            case 1:
                inc_lcd(*Flash_ptr++);
                break;
            case 2:
                dec_lcd(*Flash_ptr++);
                break;
            case 3:
                rra_lcd(*Flash_ptr++);
                break;
            case 4:
                set_delay_d(*Flash_ptr++);
                break;
            case 5:
                lcd_clear();
                break;
            case 6:
                servo_Tolocation(*Flash_ptr++,750);
                break;
            case 7:
                servo_sweep(*Flash_ptr++,*Flash_ptr++);
                timerA0On(500);
                break;
            case 8:
                sendEndSigScriptToPC();
                state = state0;
                command = 0;
                break;
            default:
                command = 0;
                state = state0;
                sendEndSigScriptToPC();
                break;
        }
        i++;
    }
    memLoad = 0;
}

//---------------------------------------------------------------------
//            delete all files from Flash
//---------------------------------------------------------------------
void delete_all_files(){

    wipe_info_DCB();
    erase_segments_4_3_2_1();
    num_of_files = 0;
    sendEndSigToPC();
}

static void erase_seg(volatile unsigned char *base){
    FCTL1 = FWKEY | ERASE;             // segment erase
    *base = 0;                         // dummy write starts erase
    while (FCTL3 & BUSY) ;             // wait end of erase
}

void wipe_info_DCB(void){
    FCTL3 = FWKEY;                     // unlock (LOCK=0)
    erase_seg(0x1000); // D
    erase_seg(0x1040); // C
    erase_seg(0x1080); // B
    FCTL3 = FWKEY | LOCK;              // lock (LOCK=1)
}

void erase_segments_4_3_2_1(void){
    FCTL3 = FWKEY;                     // unlock (LOCK=0)
    erase_seg(0xF600);
    erase_seg(0xF800);
    erase_seg(0xFA00);
    erase_seg(0xFC00);
    FCTL3 = FWKEY | LOCK;              // lock (LOCK=1)
}
//---------------------------------------------------------------------
//            inc lcd up to x
//---------------------------------------------------------------------
void inc_lcd(int x){
    int countUp = 0;
    lcd_clear();
    lcd_home();
    while(countUp <= x){
        cursor_off;
        lcd_clear();
        lcd_home();
        intNumToString(countUp,5);
        lcd_puts(counterToPrint);
        countUp++;
        lcd_home();
        timerA0On(delayS);
    }

}
//---------------------------------------------------------------------
//            dec lcd from x to 0
//---------------------------------------------------------------------
void dec_lcd(int x){
    int countDown = x;
    lcd_clear();
    lcd_home();
    while(countDown >= 0){
        cursor_off;
        lcd_clear();
        lcd_home();
        intNumToString(countDown,5);
        lcd_puts(counterToPrint);
        countDown--;
        lcd_home();
        timerA0On(delayS);
    }
}
//---------------------------------------------------------------------
//            rotate right char x
//---------------------------------------------------------------------
void rra_lcd(char x){
    lcd_clear();
    lcd_home();
    cursor_off;
    int index = 0;
    while(index < 32){
        if(index == 16)
            lcd_new_line;
        lcd_clear();
        lcd_data(x);
        timerA0On(delay);
        index++;
    }
}
//---------------------------------------------------------------------
//            set delay d
//---------------------------------------------------------------------
void set_delay_d(int d){
    delayS = mul(d ,10);
}