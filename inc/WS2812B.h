/*
 * WS2812B.h
 *
 * Created on: 13. okt. 2016
 * Author: Matej
 */

#ifndef WS2812B_H_
#define WS2812B_H_

// Number of LEDs in the LED ring
#define LED_RING_NUM 24

// Maximum rx power in mW
#define MAX_RX_POWER_mW 1
/**
 * LED color structure.
 */
typedef struct{
	uint8_t green;
	uint8_t red;
	uint8_t blue;
}WS2812B_color_t;

/**
 * Sends the whole packet to LEDs
 *
 * @param packet Struct for color of LEDs.
 * @param length Number of LEDs.
 * @return None.
 */
extern void WS2812B_send_packet(WS2812B_color_t* packet, uint32_t length);

/**
 * Turn on LEDs according to strength and number of them.
 *
 * @param strength Number of LEDs to turn on.
 * @param length Total number of LEDs.
 * @return None.
 */
extern void WS2812B_level_indicator(uint32_t strength, uint32_t length);

/**
 * Turn on LEDs according to strength and number of them and illuminates special function LED to given color.
 *
 * @param special_LED_color Color of special function LED.
 * @param strength Number of LEDs to turn on.
 * @param length Total number of LEDs.
 * @return None.
 */
extern void WS2812B_level_indicator_wLED(WS2812B_color_t special_LED_color, uint32_t strength, uint32_t length);

/**
 * Calculates the number of LEDs to turn on based on rx_power.
 *
 * @param rx_power Power of received signal.
 * @param led_ring_num Number of LEDs to turn on.
 * @return None.
 */
void koruza_led_ring_calc(uint16_t rx_power, int *led_ring_num);

/**
 * Turn on LEDs according to strength and number of them and there color will be according to specific color scheme.
 * -40 to -30dBm = red
 * -30 to -20dBm = purple
 * -20 to -15dBm = blue
 * -15 to -10dBm = cyan
 * -10 to 0dBm = green
 *
 * @param strength RX signal power in mW, multiplied by 10000.
 * @param length Total number of LEDs.
 * @return None.
 */
void WS2812B_level_indicator_color(uint16_t rx_power, uint32_t length);
#endif /* WS2812B_H_ */
