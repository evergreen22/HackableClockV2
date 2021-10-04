/*
 * Hackable Clock Project 2021-22
 * LED display driver
 * jajerkins@una.edu, 05/13/21
 */
#ifndef MAX7221_H
#define MAX7221_H

/* display driver chip pins */
#define LED_CLK               25
#define LED_CS                26
#define LED_DIN               27

/* display driver registers */
#define LED_DECODE_REG        0x09
#define LED_BRIGHTNESS_REG    0x0A
#define LED_SCANLIMIT_REG     0x0B
#define LED_SHUTDOWN_REG      0x0C
#define LED_TEST_REG          0x0F

#define LED_DIGIT_0_REG       0x01
#define LED_DIGIT_1_REG       0x02
#define LED_DIGIT_2_REG       0x03
#define LED_DIGIT_3_REG       0x04

/* display driver commands */
#define LED_SHUTDOWN_ENABLE   0x00
#define LED_SHUTDOWN_DISABLE  0x01

#define LED_DECODE_NONE       0x00
#define LED_DECODE_B_D0       0x01
#define LED_DECODE_B_D0_3     0x0F
#define LED_DECODE_B_D0_7     0xFF

#define LED_SCAN_D0           0x00
#define LED_SCAN_D1           0x01
#define LED_SCAN_D2           0x02
#define LED_SCAN_D3           0x03
#define LED_SCAN_D4           0x04
#define LED_SCAN_D5           0x05
#define LED_SCAN_D6           0x06
#define LED_SCAN_D7           0x07

#define LED_TEST_DISABLE      0x00
#define LED_TEST_ENABLE       0x01

/* code B font (0-9 are digits) */
#define LED_DIGIT_DASH        0x0A
#define LED_DIGIT_E           0x0B
#define LED_DIGIT_H           0x0C
#define LED_DIGIT_L           0x0D
#define LED_DIGIT_P           0x0E
#define LED_DIGIT_BLANK       0x0F
#define LED_DP_OFF            0x00
#define LED_DP_ON             0x80

void startLED(void*);

#endif /* MAX7221_H */

