/*
 * Hackable Clock Project 2021-22
 * Alarm driver
 * jajerkins@una.edu, 05/13/21
*/
#include <stdbool.h>

#include "simpletools.h"
#include "common.h"
#include "alarm.h"

static void soundManualAlarm(void);
static void soundAutoAlarm(void);

void
startAlarm(void* unused)
{
    ubyte now_hr, now_min;

    alrm_ok = false;
    set_direction(SPKR_PIN, PIN_OUT);

    while (!rtc_ok)
        pause(100);

    alrm_ok = true;

    while (1) {
        if (input(SW_ALRM) == SW_ON) {
            while (lockset(sem_time)) ;
            now_hr = hr;
            now_min = min;
            lockclr(sem_time);
            if (alrm_hr == now_hr) {
                if (alrm_min == now_min) {
                    /*
                     * now that the alarm is active we won't see changes to
                     * the alarm and snooze settings so the setup and
                     * reset functions (which could change alarm and snooze)
                     * must be inhibited until the alarm/snooze is complete
                     */
                    if (snooze_repeat == 0)
                        soundManualAlarm();
                    else
                        soundAutoAlarm();
                }
            }
        }

        pause(100);
    }

    /* NOT REACHED */
}

/* -----------------------------------------------------------------------
 * sound the alarm for alrm_duration seconds (1-240)
 * manual snooze for snooze_interval minutes
 */
static void
soundManualAlarm(void)
{
/*
    repeat snooze alarms {

        sound a single alarm {
            freqout(SPKR_PIN, 750, 2700);
            pause(250);

            start a new snooze cycle if the A button is pressed

            cancel the alarm if the switch is off

        } until elapsed < alrm_duration and no more snooze alarms

        if the snooze button is pressed, busy wait until the next alarm time

    } until no more snooze alarms
*/

}

/* -----------------------------------------------------------------------
 * sound the alarm for alrm_duration seconds (1-240)
 * automatically snooze for snooze_interval and repeat alarm
 * snooze_repeat is the *TOTAL* number of times the alarm sounds
 */
static void
soundAutoAlarm(void)
{
/*
    repeat snooze alarms {

        sound a single alarm {
            freqout(SPKR_PIN, 750, 2600);
            pause(250);

            cancel the alarm if the switch is off
 
        } until repeats < snooze_repeat and elapsed time < alrm_duration

         busy wait until time to sound the next alarm

    } until repeats < snooze_repeat
*/
}
