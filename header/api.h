#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer

extern char counterToPrint[5];

extern void timerA0On(int delay);

extern void servo_sweep(unsigned int l, unsigned int r);
extern void servo_Tozero();
extern void servo_Tolocation(int loc, int sleep);
extern unsigned int ultrasonic_measure();
extern void telemeter();
extern void setMask();
extern void loadScript();
extern void playScript();
extern void inc_lcd(int x);
extern void dec_lcd(int x);
extern void rra_lcd(char x);
extern void set_delay_d(int d);
extern void delete_all_files();

extern void LDRconfig();
extern void sendToPC(unsigned int num);
extern void sendEndSigToPC();
extern void sendEndSigScriptToPC();
extern void sendErrSigScriptToPC();
extern void  delete_all_files();
extern unsigned int readLDRs();

#endif
