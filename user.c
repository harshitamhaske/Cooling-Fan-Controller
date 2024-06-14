#include <stdint.h>
#include "os.h"
#include "tm4c123gh6pm.h"
#include "Timer3A.h"
#include "PLL.h"
#include "Motor.h"
#include "Timer2A.h"
#include "LCD.h"
#include "Timer0A.h"
#include "TempShow.h"
#include "SSI2.h"

#define TIMESLICE 400000

uint32_t Count1;   // number of times thread1 loops
uint32_t Count2;   // number of times thread2 loops
uint32_t Count3;   // number of times thread3 loops

extern float temperatureCelsius;
extern uint8_t temperatureCelsius_Update;
void Task1(void){
  Count1 = 0;
  for(;;){
    Count1++;
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0x0E) | (0x01<<1);  // Show red
    TempRead();

  }
}

void Task2(void){
  Count2 = 0;


  for(;;){
   Count2++;
   GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0x0E) | (0x02<<1);  // Show blue
   if(temperatureCelsius_Update==1){
       DisplayTemperature(temperatureCelsius);
       temperatureCelsius_Update=0;     // Empties the mailbox
   }


  }
}

void Task3(void){
  Count3 = 0;
  for(;;){
    Count3++;
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0x0E) | (0x03<<1);  // Show red + blue = purple/magenta
    if (temperatureCelsius >= 27.0) {
                PWM1_3_CMPB_R = 2000;
    }

    else {
            PWM1_3_CMPB_R = 100;
    }
  }
}


int main(void){
  OS_Init();           // initialize, disable interrupts, set PLL to 16 MHz
  SSI2_init();
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20) == 0){}; // allow time for clock to stabilize
  GPIO_PORTF_DIR_R |= 0x0E;             // make PF3-1 out
  GPIO_PORTF_AFSEL_R &= ~0x0E;          // disable alt funct on PF3-1
  GPIO_PORTF_DEN_R |= 0x0E;             // enable digital I/O on PF3-1
  GPIO_PORTF_PCTL_R &= ~0x0000FFF0;     // configure PF3-1 as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x0E;          // disable analog functionality on PF3-1
  OS_AddThreads(&Task1, &Task2, &Task3);
  LED_Mux_Init(5);                      //Multiplexing time
  Motor_Init();
  LCD_init();
  ADC_Init();

  SYSCTL_RCGCPWM_R |= 0x02;        // enable clock to PWM1
  SYSCTL_RCGCGPIO_R |= 0x20;       // enable clock to GPIOF
  SYSCTL_RCGCGPIO_R |= 0x02;       // enable clock to GPIOB

  Timer0A_Wait1ms(1);                     // PWM1 seems to take a while to start
  SYSCTL_RCC_R &= ~0x00100000;     // use system clock for PWM
  PWM1_INVERT_R |= 0x80;           // positive pulse
  PWM1_3_CTL_R = 0;               // disable PWM1_3 during configuration
  PWM1_3_GENB_R = 0x0000080C;     // output high when load and low when match
  PWM1_3_LOAD_R = 3999;           // 4 kHz
  PWM1_3_CTL_R = 1;               // enable PWM1_3
  PWM1_ENABLE_R |= 0x80;           // enable PWM1

  GPIO_PORTF_DIR_R |= 0x08;             // set PORTF 3 pins as output (LED) pin
  GPIO_PORTF_DEN_R |= 0x08;             // set PORTF 3 pins as digital pins
  GPIO_PORTF_AFSEL_R |= 0x08;           // enable alternate function
  GPIO_PORTF_PCTL_R &= ~0x0000F000;     // clear PORTF 3 alternate function
  GPIO_PORTF_PCTL_R |= 0x00005000;      // set PORTF 3 alternate function to PWM

  GPIO_PORTB_DEN_R |= 0x0C;             // PORTB 3 as digital pins
  GPIO_PORTB_DIR_R |= 0x0C;             // set PORTB 3 as output
  GPIO_PORTB_DATA_R |= 0x08;            // enable PORTB 3



  OS_Launch(TIMESLICE); // doesn't return, interrupts enabled in here
  return 0;             // this never executes
}
