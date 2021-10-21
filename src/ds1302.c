/*
 * Hackable Clock Project 2021-22
 * Timekeeping chip (RTC) driver
 * jajerkins@una.edu, 05/13/21
 */
#include <stdbool.h>
#include <string.h>

#include "simpletools.h"
#include "common.h"
#include "ds1302.h"

/* ... function prototypes TBD */

void 
startRTC(void* unused)
{
    ubyte config[31];
    /*
     * scratchpad ram is 31 bytes, used to store config
     * first byte is the config valid/invalid flag,
     * remaining 30 bytes may be used to store anything
     */
    ubyte hour, minute, second;
    ubyte month, date, year, weekday;

    rtc_ok = false;

    /* initialize the ds1302 RTC registers */
    set_direction(RTC_CLK, PIN_OUT);
    set_direction(RTC_DAT, PIN_OUT);
    set_direction(RTC_CE, PIN_OUT);

    config[0] = '\0';
    writeRTC(CMD_WRITE_WP, config, 1);

    /* trickle charger on with 1 diode and 2K Ohms */
    config[0] = 0xA5;
    writeRTC(CMD_WRITE_TCS, config, 1);

    /* ... incomplete */

}
