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

static void  saveConfig(void);
static bool  getConfig(ubyte data[]);

static void  writeTime(ubyte hr, ubyte min, ubyte sec);
static void  readTime(ubyte* hr, ubyte* min, ubyte* sec);
static void  writeDate(ubyte month, ubyte day, ubyte yr, ubyte dow);
static void  readDate(ubyte* month, ubyte* day, ubyte* yr, ubyte* dow);

static void  writeRTC(ubyte command, ubyte data[], ubyte datalen);
static void  readRTC(ubyte command, ubyte data[], ubyte datalen);

static ubyte reverseByte(ubyte data);
static ubyte bcd2bin(ubyte data);
static ubyte bin2bcd(ubyte data);

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
    writeRTC(RTC_WRITE_WP, config, 1);

    /* trickle charger on with 1 diode and 2K Ohms */
    config[0] = 0xA5;
    writeRTC(RTC_WRITE_TCS, config, 1);

    if ((valid_cfg = getConfig(config))) {
        alrm_hr = config[1];
        alrm_min = config[2];
        alrm_duration = config[3];
        snooze_interval = config[4];
        snooze_repeat = config[5];
        brightness = config[6];
    } else {
        alrm_hr = 12;
        alrm_min = 0;
        alrm_duration = 30;
        snooze_interval = 10;
        snooze_repeat = 0;
        brightness = 8;
        writeTime(12, 0, 0);
        writeDate(9, 1, 21, 4);
    }

    save_time = false;
    save_date = false;
    save_cfg = false;
    rtc_ok = true;

    while (1) {
        readTime(&hour, &minute, &second);
        while (lockset(sem_time)) ;
        hr = hour;
        min = minute;
        sec = second;
        lockclr(sem_time);

        readDate(&month, &date, &year, &weekday);
        while (lockset(sem_date)) ;
        mon = month;
        day = date;
        yr = year;
        dow = weekday;
        lockclr(sem_date);

        if (save_cfg) {
            saveConfig();
            save_cfg = false;
        }

        if (save_time) {
            writeTime(set_hr, set_min, set_sec);
            save_time = false;
        }

        if (save_date) {
            writeDate(set_mon, set_day, set_yr, set_dow);
            save_date = false;
        }

        pause(100);
    }

    /* NOT REACHED */
}

/*
 * -----------------------------------------------------------------------
 * reset clock to default values and invalidate config (scratchpad ram)
 */
void
resetClock(void)
{
    // set default values for the date and enable save

    // set default values for the time and enable save

    // set default values for the config and enable save

    // unset valid_cfg
}

/*
 * -----------------------------------------------------------------------
 * scratchpad ram is 31 bytes, mark config as valid with first byte = AAh
 * uses burst mode to write all 31 bytes of scratchpad ram
 */
static void
saveConfig(void)
{
    ubyte data[31] = {0};

    if (valid_cfg) {
        data[0] = 0xAA;
        data[1] = alrm_hr;
        data[2] = alrm_min;
        data[3] = alrm_duration;
        data[4] = snooze_interval;
        data[5] = snooze_repeat;
        data[6] = brightness;
    }

    writeRTC(RTC_WRITE_RAM, data, 31);
}

/*
 * -----------------------------------------------------------------------
 * scratchpad ram is 31 bytes, return true if config is valid
 * uses burst mode to read all 31 bytes of scratchpad ram
 */
static bool
getConfig(ubyte data[])
{

}

/*
 * -----------------------------------------------------------------------
 * time values are stored in BCD - see DS1302 datasheet for details
 */
static void
writeTime(ubyte hr, ubyte min, ubyte sec)
{
    ubyte data[1];

    data[0] = bin2bcd(hr) & 0x3F; /* enable 24 hour mode */
    writeRTC(RTC_WRITE_HRS, data, 1);
    data[0] = bin2bcd(min);
    writeRTC(RTC_WRITE_MINS, data, 1);
    data[0] = bin2bcd(sec) & 0x7F;  /* clock halt disabled */
    writeRTC(RTC_WRITE_SECS, data, 1);
}

/*
 * -----------------------------------------------------------------------
 * time values are stored in BCD - see DS1302 datasheet for details
 */
static void
readTime(ubyte* hr, ubyte* min, ubyte* sec)
{
    // use readRTC 3 times [hours, minutes, seconds]
}

/*
 * -----------------------------------------------------------------------
 * date values are stored in BCD - see DS1302 datasheet for details
 */
static void
writeDate(ubyte month, ubyte day, ubyte yr, ubyte dow)
{
    // use writeRTC 4 times [month, day, year, DOW]
}

/*
 * -----------------------------------------------------------------------
 * date values are stored in BCD - see DS1302 datasheet for details
 */
static void
readDate(ubyte * month, ubyte * day, ubyte * yr, ubyte * dow)
{
    // use readRTC 4 times [month, day, year, DOW]
}

/*
 * -----------------------------------------------------------------------
 * reads and writes occur lsb first - see DS1302 datasheet for details
 */
static void
writeRTC(ubyte command, ubyte data[], ubyte datalen)
{
    high(RTC_CE);

    for (ubyte i = 0; i < 8; i++) {
        if (command & 0x01)
            high(RTC_DAT);
        else
            low(RTC_DAT);
        high(RTC_CLK);
        command = command >> 1;
        low(RTC_CLK);
    }

    for (ubyte i = 0; i < datalen; i++) {
        ubyte onebyte = data[i];
        for (ubyte j = 0; j < 8; j++) {
            if (onebyte & 0x01)
                high(RTC_DAT);
            else
                low(RTC_DAT);
            high(RTC_CLK);
            onebyte = onebyte >> 1;
            low(RTC_CLK);
        }
    }

    low(RTC_CE);
}

/*
 * -----------------------------------------------------------------------
 * reads and writes occur lsb first - see DS1302 datasheet for details
 */
static void
readRTC(ubyte command, ubyte data[], ubyte datalen)
{
    // write command bits
    // read data bits
}

/*
 * -----------------------------------------------------------------------
 * reads and writes occur lsb first - see DS1302 datasheet for details
 */
static ubyte
reverseByte(ubyte data)
{
    // reverse the order of the bits in one byte
    // for example, if input data = 1100 1011
    // output data = 1101 0011
}

/*
 * -----------------------------------------------------------------------
 * RTC stores data in bcd, convert a value read from the chip to binary
 */
static ubyte
bcd2bin(ubyte data)
{
    // convert a bcd value to binary
    // for example, if input data = 0010 0100
    // output data = 0001 1000
}

/*
 * -----------------------------------------------------------------------
 * RTC stores data in bcd, convert a value to store on the chip to bcd
 */
static ubyte
bin2bcd(ubyte data)
{
    // convert a binary value to bcd
}
