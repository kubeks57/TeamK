#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>
#include "leds.h"
#include "spi_comm.h"
#include "sensors/proximity.h"
#include "motors.h"



messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

int main(void)
{

    halInit();
    chSysInit();
    mpu_init();

    messagebus_init(&bus, &bus_lock, &bus_condvar);

    clear_leds();
    spi_comm_start();

    proximity_start(0);
//    calibrate_ir();

    get_prox(0);
    get_prox(1);
    get_prox(2);
    get_prox(3);
    get_prox(4);
    get_prox(5);
    get_prox(6);
    get_prox(7);

    motors_init();
    int motor_left;
    int motor_right;
    left_motor_set_speed(500);
    right_motor_set_speed(-500);

    /* Infinite loop. */
    while (1) {
    	//waits 1 second
        chThdSleepMilliseconds(1000);
//        motor_left = 500;
//        motor_right = 500;
//
//        chThdSleepMilliseconds(1000);
//
//        motor_left = 500;
//        motor_right = 1000;


// LED set-up

//      set_led(LED1, 2);
//      set_led(LED3, 2);
//      set_led(LED5, 2);
//      set_led(LED7, 2);
//
//      set_body_led(2);
//      set_front_led(2);

//      set_rgb_led(LED2, 10, 10, 10);
//      set_rgb_led(LED4, 0, 10, 10);
//      set_rgb_led(LED6, 10, 0, 10);
//      set_rgb_led(LED8, 10, 10, 0);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
