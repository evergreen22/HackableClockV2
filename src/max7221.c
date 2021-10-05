/*
 * Hackable Clock Project 2021-22
 * LED display driver
 * jajerkins@una.edu, 05/13/21
 */
#include <stdbool.h>

#include "simpletools.h"
#include "common.h"
#include "max7221.h"

static void  writeLED(ubyte reg, ubyte data);

void 
startLED(void* unused)
{
    ubyte dig0, dig1, dig2, dig3;
    ubyte dsp0, dsp1, dsp2, dsp3;
    ubyte bset;

    led_ok = false;

    /* initialize the max7221 registers */
    set_direction(LED_CLK, PIN_OUT);
    set_direction(LED_CS, PIN_OUT);
    set_direction(LED_DIN, PIN_OUT);

    low(LED_CLK);
    high(LED_CS);
    low(LED_DIN);

    d0 = LED_DIGIT_DASH;
    d1 = LED_DIGIT_DASH;
    d2 = LED_DIGIT_DASH;
    d3 = LED_DIGIT_DASH;

    dp0 = LED_DP_OFF;
    dp1 = LED_DP_OFF;
    dp2 = LED_DP_OFF;
    dp3 = LED_DP_OFF;

    dsp0 = d0;
    dsp1 = d1;
    dsp2 = d2;
    dsp3 = d3;
    bset = 8;

    writeLED(LED_TEST_REG, LED_TEST_ENABLE);
    writeLED(LED_DECODE_REG, LED_DECODE_B_D0_3);
    writeLED(LED_BRIGHTNESS_REG, bset);
    writeLED(LED_SCANLIMIT_REG, LED_SCAN_D3);
    writeLED(LED_SHUTDOWN_REG, LED_SHUTDOWN_DISABLE);
    pause(2500);

    while (!rtc_ok)
        pause(100);
    
    writeLED(LED_TEST_REG, LED_TEST_DISABLE);
    led_ok = true;

    while (1) {
        while (lockset(sem_display)) ;
        dig0 = dp0 | d0;
        dig1 = dp1 | d1;
        dig2 = dp2 | d2;
        dig3 = dp3 | d3;
        lockclr(sem_display);

        if (dig0 != dsp0) {
            dsp0 = dig0;
            writeLED(LED_DIGIT_0_REG, dig0);
        }

        if (dig1 != dsp1) {
            dsp1 = dig1;
            writeLED(LED_DIGIT_1_REG, dig1);
        }

        if (dig2 != dsp2) {
            dsp2 = dig2;
            writeLED(LED_DIGIT_2_REG, dig2);
        }

        if (dig3 != dsp3) {
            dsp3 = dig3;
            writeLED(LED_DIGIT_3_REG, dig3);
        }

        if (bset != brightness) {
            bset = brightness;
            writeLED(LED_BRIGHTNESS_REG, bset);
        }

        pause(100);
    }

    /* NOT REACHED */
}

/*
 * -----------------------------------------------------------------------
 * writes occur msb first - see MAX7221 datasheet for details
 */
static void 
writeLED(ubyte cmd, ubyte data)
{
    low(LED_CS);

    for (ubyte i = 0; i < 8; i++) {
        if (cmd & 0x80)
            high(LED_DIN);
        else
            low(LED_DIN);
        high(LED_CLK);
        cmd = cmd << 1;
        low(LED_CLK);
    }

    for (ubyte i = 0; i < 8; i++) {
        if (data & 0x80)
            high(LED_DIN);
        else
            low(LED_DIN);
        high(LED_CLK);
        data = data << 1;
        low(LED_CLK);
    }

    high(LED_CS);
}
