#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <main.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include "leds.h"
#include "spi_comm.h"
#include "sensors/proximity.h"
#include "motors.h"
#include "sensors/VL53L0X/VL53L0X.h"

#include "epuck1x/uart/e_uart_char.h"
#include "stdio.h"
#include "serial_comm.h"
#include "sensors/battery_level.h"
#include "selector.h"

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

int main()
{
  //Initialisations:

    halInit();
    chSysInit();
    mpu_init();

    messagebus_init(&bus, &bus_lock, &bus_condvar); //initiate the inter process communication bus

    clear_leds(); // turn off LEDs
    spi_comm_start(); // Initiate LEDs

    proximity_start(0); // Initiate proximity sensors

    VL53L0X_start(); // Initiate distance sensor

    motors_init(); // Initialise motors

    serial_start(); // Initialise UART1

    battery_level_start(); // Initialise Battery level


//  In infinite loop:
    while (1) {

    chThdSleepMilliseconds(500); //waits 1 second


    //Get sensors measurements:
     int prox0 = get_prox(0);
     int prox1 = get_prox(1);
     int prox2 = get_prox(2);
     int prox3 = get_prox(3);
     int prox4 = get_prox(4);
     int prox5 = get_prox(5);
     int prox6 = get_prox(6);
     int prox7 = get_prox(7);

    // Selector handler: /////////////////////////////////////////////////////////////////
     int selector = get_selector();

    int Task=1;
    int lastTask=0;
    int state;

    int batt = get_battery_percentage();

    char str[100];
    int str_length;
    int value = 10;
    str_length = sprintf(str, "PProx 0 %d\n",prox0);
    e_send_uart1_char(str, str_length);

    if (get_selector() > 1 && get_selector() < 8 && lastTask != Task){
      Task = 1;
      state = 1;
    }
    else if (get_selector() == 1 && lastTask != Task){
      Task = 3;
      state = 3;
    }
    else if (lastTask != Task){
      Task = 2;
      state = 2;
    }

    switch (state){
    case 0:
      break;

    case 1:
      str_length = sprintf(str, "Task 1 selected\n");
      e_send_uart1_char(str, str_length);
    lastTask = 1;
      str_length = sprintf(str, "Battery state = %d%\n",batt);
      e_send_uart1_char(str, str_length);

      state = 0;
      break;

    case 2:
    str_length = sprintf(str, "Task 2 selected\n");
    e_send_uart1_char(str, str_length);
    lastTask = 2;
      str_length = sprintf(str, "Battery state = %d%\n",batt);
      e_send_uart1_char(str, str_length);

    state = 0;
    break;

    case 3:
      str_length = sprintf(str, "Select task (Knob left - Task 2/Knob right - Task 1)\n");
      e_send_uart1_char(str, str_length);
    lastTask = 3;
      str_length = sprintf(str, "Battery state = %d%\n",batt);
      e_send_uart1_char(str, str_length);

      state=0;
      break;
   }
    /////////////////////////////////////////////////////////////////////////////////////////////

    // No task selected: //////////////////////////////////////////////////////////////////////
    if (Task == 3){
        set_led(LED1, 2); //Toggle Red LEDs
        set_led(LED3, 2);
        set_led(LED5, 2);
        set_led(LED7, 2);
        set_rgb_led(LED2, 0, 0, 0); // RGB LEDs
        set_rgb_led(LED4, 0, 0, 0);
        set_rgb_led(LED6, 0, 0, 0);
        set_rgb_led(LED8, 0, 0, 0);
    }
    ////////////////////////////////////////////////////////////////////////////////////

     //Task 1 handler: ////////////////////////////////////////////////////////////////////////////
   else if (Task == 1){
	   clear_leds();
	   set_rgb_led(LED2, 0, 10, 0); // RGB LEDs
       set_rgb_led(LED4, 0, 10, 0);
       set_rgb_led(LED6, 0, 10, 0);
       set_rgb_led(LED8, 0, 10, 0);
    }
    //////////////////////////////////////////////////////////////////////////////////////

     //Task 2 handler: /////////////////////////////////////////////////////////////////////////////
   else if (Task == 2){
    	clear_leds();
    	set_rgb_led(LED2, 0, 0, 10); // RGB LEDs
       set_rgb_led(LED4, 0, 0, 10);
       set_rgb_led(LED6, 0, 0, 10);
       set_rgb_led(LED8, 0, 0, 10);
    }
    ////////////////////////////////////////////////////////////////////////////////////////

    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}

/* Commands:

LED set-up:

    set_led(LED1, 2); //Red LEDs
    set_led(LED3, 2); // 0 OFF
    set_led(LED5, 2); // 1 ON
    set_led(LED7, 2); // 2 Toggle

    set_body_led(2); // Body LED
    set_front_led(2); // Front LED

    set_rgb_led(LED2, 10, 10, 10); // RGB LEDs
    set_rgb_led(LED4, 0, 10, 10);
    set_rgb_led(LED6, 10, 0, 10);
    set_rgb_led(LED8, 10, 10, 0);

Set motor speed:

    left_motor_set_speed(500);
    right_motor_set_speed(-500);

Get latest motor speed:

    int left_motor_get_desired_speed(void);
    int right_motor_get_desired_speed(void);

Get battery percentage:

    float get_battery_percentage();

Get value from selector knob:

    int get_selector();

Sending to the terminal "hello world":

    char str[100];
    int str_length;
    str_length = sprintf(str, "Hello World\n");
    e_send_uart1_char(str, str_length);

Sending value 10 in the terminal:

    int value = 10;
    str_length = sprintf(str, "Printing number %d!\n",value);
    e_send_uart1_char(str, str_length);

Send a character buffer array buff with length buff_len using UART1 channel:

    e_send_uart1_char(const char * buff, int buff_len);

Return the distance measured in mm:
    uint16_t VL53L0X_get_dist_mm(void);


  int get_calibrated_prox(0);
    int get_calibrated_prox(1);
    int get_calibrated_prox(2);
    int get_calibrated_prox(3);
    int get_calibrated_prox(4);
    int get_calibrated_prox(5);
    int get_calibrated_prox(6);
    int get_calibrated_prox(7);

    int get_ambient_light(0);
    int get_ambient_light(1);
    int get_ambient_light(2);
    int get_ambient_light(3);
    int get_ambient_light(4);
    int get_ambient_light(5);
    int get_ambient_light(6);
    int get_ambient_light(7);

    calibrate_ir(); // not nescessary
 */
