/*
 * Hackable Clock Project 2021-22
 * Common definitions and variables
 * jajerkins@una.edu, 05/13/21
 */
#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

/* user defined types */
typedef unsigned char  ubyte;
typedef signed char    byte;

/* cpu pin directions */
#define PIN_IN    0
#define PIN_OUT   1

/* switch states */
#define SW_OFF    0
#define SW_ON     1

/* button states */
#define BTN_UP    0
#define BTN_DOWN  1

/* switch and button cpu pins*/
#define SW_ALRM   3
#define BTN_A     4
#define BTN_B     5
#define BTN_C     6
#define BTN_D     7

/* Semaphore variables */
volatile byte    sem_time, sem_date, sem_display;

/* RTC global shared variables */
volatile bool    rtc_ok;
volatile ubyte   hr, min, sec, mon, day, yr, dow;
volatile ubyte   set_hr, set_min, set_sec, set_mon, set_day, set_yr, set_dow;
volatile bool    save_time, save_date, save_cfg;
volatile bool    valid_cfg;

/* LED global shared variables */
volatile bool    led_ok;
volatile ubyte   d0, d1, d2, d3;
volatile ubyte   dp0, dp1, dp2, dp3;
volatile ubyte   brightness;

/* ALARM global shared variables */
volatile bool    alrm_ok;
volatile ubyte   alrm_hr, alrm_min, alrm_duration, snooze_interval, snooze_repeat;

#endif /* COMMON_H */
