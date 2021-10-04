/*
 * Hackable Clock Project 2021-22
 * LED display driver
 * jajerkins@una.edu, 05/13/21
 */

/* ----------------------------- */
/* incomplete common header file */
/* ----------------------------- */

#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

/* user defined types */
typedef unsigned char ubyte;
typedef signed char byte;

/* cpu pin directions */
#define PIN_IN    0
#define PIN_OUT   1

/* switch states */
#define SW_OFF    0
#define SW_ON     1

/* button states */
#define BTN_UP    0
#define BTN_DOWN  1

/* switch and button pins */
#define SW_ALRM   3
#define BTN_A     5
#define BTN_B     6
#define BTN_C     7
#define BTN_D     8

/* Semaphore variables */
volatile byte    sem_display;

/* RTC global shared variables */

/* LED global shared variables */
volatile bool    led_ok;
volatile ubyte   d0, d1, d2, d3;
volatile ubyte   dp0, dp1, dp2, dp3;
volatile ubyte   brightness;

/* ALARM global shared variables */

#endif  /* COMMON_H */
