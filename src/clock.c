/*
 * Hackable Clock Project 2021-22
 * Clock driver (main program file with button interface)
 * jajerkins@una.edu, 05/13/21
 */
#include "simpletools.h"
#include "common.h"
#include "ds1302.h"
#include "max7221.h"
#include "alarm.h"

static void   failBeep(int err);
static ubyte  updateDisplay(void);
static void   showAlarm(void);
static void   showDate(void);
static void   showDOW(void);
static void   setDecimals(ubyte);
static void   doSetup(void);
static int    setTime(void);
static int    setAlarm(void);
static int    setSnoozeInterval(void);
static int    setSnoozeRepeat(void);
static int    setAlarmDuration(void);
static int    setBrightness(void);
static ubyte  setYear(void);
static ubyte  setMonth(void);
static void   setDay(ubyte month, ubyte year);
static ubyte  calcDOW(ubyte month, ubyte day, ubyte year);

int
main(void)
{
    ubyte second;
    int *rtc_cog, *led_cog;

    set_direction(SW_ALRM, PIN_IN);
    set_direction(BTN_A, PIN_IN);
    set_direction(BTN_B, PIN_IN);
    set_direction(BTN_C, PIN_IN);
    set_direction(BTN_D, PIN_IN);

    if ((sem_time = locknew()) == -1)
        failBeep(1);
    if ((sem_date = locknew()) == -1)
        failBeep(2);
    if ((sem_display = locknew()) == -1)
        failBeep(3);

    if ((rtc_cog = cog_run(startRTC, 64)) == NULL)
        failBeep(4);

    if ((led_cog = cog_run(startLED, 16)) == NULL) {
        cog_end(rtc_cog);
        failBeep(5);
    }

    if (cog_run(startAlarm, 40) == NULL) {
        cog_end(rtc_cog);
        cog_end(led_cog);
        failBeep(6);
    }

    /* wait until all drivers (cogs) are started */
    do {
        pause(100);
     } while (!rtc_ok || !led_ok || !alrm_ok);

    second = 60;

    while (1) {
        if (second != sec)
            second = updateDisplay();

        /*
         * the clock setup and reset functions must be disabled
         * when the alarm is set (switch is on).
         * see the alarm driver code for details why
         */
        if (input(BTN_A) == BTN_DOWN && input(BTN_B) == BTN_DOWN && input(SW_ALRM) == SW_OFF)
            doSetup();

        if (input(BTN_A) == BTN_DOWN && input(BTN_D) == BTN_DOWN && input(SW_ALRM) == SW_OFF) {
            resetClock();
            freqout(SPKR_PIN, 500, 2700);
            freqout(SPKR_PIN, 250, 2600);
        }

        if (input(BTN_B) == BTN_DOWN)
            showAlarm();

        if (input(BTN_C) == BTN_DOWN)
            showDate();

        if (input(BTN_D) == BTN_DOWN)
            showDOW();

        pause(100);
    }

    /* NOT REACHED */
}

/*
 * --------------------------------------------------------------------------
 * beep forever when a fatal error has occurred during startup
 * beep code indicates where startup failed
 */
static void
failBeep(int err)
{
    while (1) {
        for (int i=0; i<err; i++) {
            freqout(SPKR_PIN, 250, 2700);
            pause(500);
        }
        pause(2000);
    }
}

/*
 * --------------------------------------------------------------------------
 * update the digit vars used for the LED display with the current time.
 * by design we restrict all the display var changes to this one cog to
 * avoid conflicting settings
 */
static ubyte
updateDisplay(void)
{
    // acquire the time semaphore, store time variables into locals, and release
    // if the alarm is enabled set a var to turn the the leftmost decimal point on
    // if it is after noon (p.m.) set a var to turn the the rightmost decimal point on
    // acquire the display semaphore, update the display variables, and release
    // return the seconds
}

/*
 * --------------------------------------------------------------------------
 * display the alarm time on the display for 5 seconds then restore time
 */
static void
showAlarm(void)
{
    // acquire the display semaphore
    // turn off all the decimal points
    // update the display with the alarm values
    // release the display semaphore
    // pause for 5 seconds
}

/*
 * --------------------------------------------------------------------------
 * display the date on the display for 5 seconds then restore the time
 */
static void
showDate(void)
{
    // acquire the time semaphore, store date vars into locals, release
    // acquire the display semaphore
    // turn off all the decimal points
    // update the display with the month and day values
    // release the display semaphore
    // pause for 2.5 seconds
    // acquire the display semaphore
    // update the display with the year value
    // release the display semaphore
}

/*
 * --------------------------------------------------------------------------
 * display the DOW on the display for 5 seconds then restore the time
 */
static void
showDOW(void)
{
    // acquire the display semaphore
    // turn off all the decimal points
    // update the display with the dow value
    // release the display semaphore
    // pause for 5 seconds
}

/*
 * --------------------------------------------------------------------------
 * set the LED decimal points to the specified value in binary.
 * for example, 3=0011 where dp0=0, dp1=0, dp2=1 dp3=1
 */
static void
setDecimals(ubyte val)
{
    while (lockset(sem_display)) ;
    dp0 = val & 0x08 ? LED_DP_ON : LED_DP_OFF;
    dp1 = val & 0x04 ? LED_DP_ON : LED_DP_OFF;
    dp2 = val & 0x02 ? LED_DP_ON : LED_DP_OFF;
    dp3 = val & 0x01 ? LED_DP_ON : LED_DP_OFF;
    lockclr(sem_display);
}

/*
 * --------------------------------------------------------------------------
 * set clock options
 * BTN_A = exit setup, BTN_B = next option
 * BTN_C = up/increase, BTN_D = down/decrease
 */
static void
doSetup(void)
{
    ubyte btn;

    while (lockset(sem_display)) ;
    d0 = LED_DIGIT_DASH;
    d1 = LED_DIGIT_DASH;
    d2 = LED_DIGIT_DASH;
    d3 = LED_DIGIT_DASH;
    lockclr(sem_display);
    pause(2000);

    /* option 1 - set clock time */
    setDecimals(1);
    btn = setTime();

    /* option 2 - set alarm time */
    if (btn == BTN_B) {
        setDecimals(2);
        btn = setAlarm();
    }

    /* option 3 - set alarm duration (1-240s) */
    if (btn == BTN_B) {
        setDecimals(3);
        btn = setAlarmDuration();
    }

    /* option 4 - set snooze interval (1-30m) */
    if (btn == BTN_B) {
        setDecimals(4);
        btn = setSnoozeInterval();
    }

    /* option 5 - set snooze repeats (0-10) */
    if (btn == BTN_B) {
        setDecimals(5);
        btn = setSnoozeRepeat();
    }

    /* option 6 - set display brightness (1-15) */
    if (btn == BTN_B) {
        setDecimals(6);
        btn = setBrightness();
    }

    /*
     * option 7 - set year (0-99)
     * option 8 - set month (1-12)
     * option 9 - set day (1-31)
     *
     * To avoid input of invalid dates we require the year, month, and day
     * to always be set together. Year and month are set before the day
     * in order to determine the valid days per month and February leap years.
     * The exit button is not allowed when setting the date fields.
     */
    if (btn == BTN_B) {
        ubyte month, year;

        setDecimals(7);
        year = setYear();
        setDecimals(8);
        month = setMonth();
        setDecimals(9);
        setDay(month, year);
    }

   setDecimals(0);
}

/*
 * --------------------------------------------------------------------------
 * Option 1 - set the time, times are set in 24 hour mode (0-23)
 */
static int
setTime(void)
{
    byte new_hr, new_min;
    byte old_hr, old_min;
    int exit_button = -1;

    while (lockset(sem_time)) ;
    new_hr = hr;
    new_min = min;
    lockclr(sem_time);

    old_hr = new_hr;
    old_min = new_min;

    do {
        if (input(BTN_A) == BTN_DOWN)
            exit_button = BTN_A;

        if (input(BTN_B) == BTN_DOWN)
            exit_button = BTN_B;

        if (input(BTN_C) == BTN_DOWN) {
            new_min = (new_min + 1) % 60;
            if (new_min == 0)
                new_hr = (new_hr + 1) % 24;
        }

        if (input(BTN_D) == BTN_DOWN) {
            new_min = new_min - 1;
            if (new_min == -1) {
                new_min = 59;
                new_hr = new_hr == 0 ? 23 : new_hr - 1;
            }
        }

        while (lockset(sem_display)) ;
        if (new_hr > 9)
            d0 = new_hr / 10;
        else
            d0 = LED_DIGIT_BLANK;
        d1 = new_hr % 10;
        d2 = new_min / 10;
        d3 = new_min % 10;
        lockclr(sem_display);

        pause(100);
    } while (exit_button == -1);

    if (old_hr != new_hr || old_min != new_min) {
        while (lockset(sem_time))
        hr = new_hr;
        min = new_min;
        lockclr(sem_time);

        set_hr = new_hr;
        set_min = new_min;
        set_sec = 0;
        save_time = true;
    }

    pause(250);
    return exit_button;
}

/*
 * --------------------------------------------------------------------------
 * Option 2 - set the alarm time, times are set in 24 hour mode (0-23)
 */
static int
setAlarm(void)
{
    // set the alarm hour and minutes - this function is similar to setTime()
    // set both valid_cfg and save_cfg to true to save the alarm time
}

/*
 * --------------------------------------------------------------------------
 * Option 3 - set the alarm duration (how long the alarm sounds)
 * valid values are in the range 1 - 240 seconds
 */
static int
setAlarmDuration(void)
{
    // set the alarm duration - this function is similar to setTime()
    // note the range for valid values in the function description
    // set both valid_cfg and save_cfg to true to save the duration
}

/*
 * --------------------------------------------------------------------------
 * Option 4 - set the snooze interval (time between alarms when snoozing)
 * valid values are in the range 1 - 30 minutes
 */
static int
setSnoozeInterval(void)
{
    // set the snooze interval - this function is similar to setTime()
    // note the range for valid values in the function description
    // set both valid_cfg and save_cfg to true to save the snooze interval
}

/*
 * --------------------------------------------------------------------------
 * Option 5 - set the snooze repeats (number of automatic alarm repeats)
 * valid values are in the range 0 - 10 repeats
 */
static int
setSnoozeRepeat(void)
{
    // set the snooze repeat - this function is similar to setTime()
    // note the range for valid values in the function description
    // set both valid_cfg and save_cfg to true to save the snooze repeat
}

/*
 * --------------------------------------------------------------------------
 * Option 6 - set the display brightness
 * valid values are in the range 1 - 15
 */
static int
setBrightness(void)
{
    // set the display brightness - this function is similar to setTime()
    // note the range for valid values in the function description
    // set both valid_cfg and save_cfg to true to save the brightness
}

/*
 * --------------------------------------------------------------------------
 * Option 7 - set the year (no century)
 * valid values are in the range 0 - 99
 */
static ubyte
setYear(void)
{
    // set the year (no century) - this function is similar to setTime()
    // note the range for valid values in the function description
    // return the year value - do not save it yet
}

/*
 * --------------------------------------------------------------------------
 * Option 8 - set the month
 * valid values are in the range 1 - 12
 */
static ubyte
setMonth(void)
{
    // set the month - this function is similar to setTime()
    // note the range for valid values in the function description
    // return the month value - do not save it yet
}

/*
 * --------------------------------------------------------------------------
 * Option 9 - set the day of the month (date)
 * valid values are in the range 1 - 31 depending on the month
 */
static void
setDay(ubyte month, ubyte year)
{
    // set the day - this function is similar to setTime()
    // note the range for valid values in the function description
    // remember, months have different maximum values for the day
    // also remember, leap years affect the maximum day value in February
    // after the day is selected, calculate the day-of-week (dow) by calling calcDOW()
    // store month, day, year, and dow values into the common "set_*" variables (in common.h)
    // set save_cfg to true to save the date values
}

/*
 * --------------------------------------------------------------------------
 * use Zeller's congruence to calculate the day of the week (dow).
 * based on implementation in RFC 3339 (Sun = 0, Mon = 1, ..., Sat = 6)
 * DS1302+ is limited to century = 20, see datasheet
 * DS1302+ dow values are in the range 1...7, see datasheet (ISO 8601)
 */
static ubyte
calcDOW(ubyte m, ubyte d, ubyte y)
{
    // use your favorite web search tool to find RFC 3339
    // return the computed dow (note the range)
}
