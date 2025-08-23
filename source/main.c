#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;



 void main(void){
  state = state0;  // start in idle state on RESET //////////////////////////////to change
  lpm_mode = mode0;     // start in idle state on RESET

  sysConfig();
  lcd_init();
  lcd_clear();
  servo_Tozero();



  while(1){
	switch(state){
	  case state0:                // goes to sleep
           enterLPM(lpm_mode);
		break;

      case state1:
          servo_sweep(0,180);
          state = state0;
          break;

	  case state2:
	      telemeter();
	      break;

	  case state3:
	      servo_sweep(0,180);
	      state = state0;
		  break;

      case state4:
          servo_sweep(0,180);
          state = state0;

		  break;

      case state5:
          PBsArrIntEn |= 0x81;
          loadScript();
          state = state0;
          PBsArrIntEn &= ~0x81;
          break;

      case state6:
          setMask();
          state = state0;
          break;

      case state7:
          LDRconfig();
          state = state0;
          break;
      case state8:
          playScript();
          break;
	}
  }

}




