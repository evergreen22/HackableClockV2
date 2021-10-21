/*
 * Hackable Clock Project 2021-22
 * Timekeeping chip (RTC) driver
 * jajerkins@una.edu, 05/13/21
 */
#ifndef DS1302_H
#define DS1302_H

/* timekeeping chip pins */
#define RTC_CLK        0
#define RTC_DAT        1
#define RTC_CE         2

/* real time clock commands */
#define RTC_WRITE_SECS 0x80
#define RTC_READ_SECS  0x81
#define RTC_WRITE_MINS 0x82
#define RTC_READ_MINS  0x83
#define RTC_WRITE_HRS  0x84
#define RTC_READ_HRS   0x85
#define RTC_WRITE_DAY  0x86
#define RTC_READ_DAY   0x87
#define RTC_WRITE_MTH  0x88
#define RTC_READ_MTH   0x89
#define RTC_WRITE_DOW  0x8A
#define RTC_READ_DOW   0x8B
#define RTC_WRITE_YR   0x8C
#define RTC_READ_YR    0x8D

#define RTC_WRITE_WP   0x8E
#define RTC_WRITE_TCS  0x90

#define RTC_WRITE_RAM  0xFE
#define RTC_READ_RAM   0xFF

void startRTC(void*);
void resetClock(void);

#endif /* DS1302_H */
