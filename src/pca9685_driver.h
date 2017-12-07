/*
 * pca9685_driver.h
 *
 * Adapted from Adafruit's PWMServoDriver library by Limor Fried (Ladyada).
 * https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
 *
 * Created: 11/24/2017 11:21:06 PM
 *  Author: Alex Bennett
 */ 

#ifndef PCA9685_DRIVER_H_
#define PCA9685_DRIVER_H_

#include "pca9685_register_map.h"

#define PCA9685_ADDR_DEFAULT	0x40

enum pca9685_return_code {
	PCA9685_SUCCESS		= 0,
	PCA9685_ERROR		= 1,
	PCA9685_INIT_ERROR	= 2
} typedef pca9685_return_code_t;

enum pca9685_channel {
	PCA9685_CHANNEL_0	= 0,
	PCA9685_CHANNEL_1	= 1,
	PCA9685_CHANNEL_2	= 2,
	PCA9685_CHANNEL_3	= 3,
	PCA9685_CHANNEL_4	= 4,
	PCA9685_CHANNEL_5	= 5,
	PCA9685_CHANNEL_6	= 6,
	PCA9685_CHANNEL_7	= 7,
	PCA9685_CHANNEL_8	= 8,
	PCA9685_CHANNEL_9	= 9,
	PCA9685_CHANNEL_10	= 10,
	PCA9685_CHANNEL_11	= 11,
	PCA9685_CHANNEL_12	= 12,
	PCA9685_CHANNEL_13	= 13,
	PCA9685_CHANNEL_14	= 14,
	PCA9685_CHANNEL_15	= 15,
} typedef pca9685_channel_t;

struct pca9685_config {
	uint8_t chip_addr;
} typedef pca9685_config_t;

pca9685_return_code_t pca9685_init(const pca9685_config_t* config);
pca9685_return_code_t pca9685_reset(const pca9685_config_t* config);
pca9685_return_code_t pca9685_write_register(const pca9685_config_t* config, uint8_t reg_addr, uint8_t data);
pca9685_return_code_t pca9685_write_register_bytes(const pca9685_config_t* config, uint8_t reg_addr, uint8_t num_bytes, uint8_t* data);
pca9685_return_code_t pca9685_read_register(const pca9685_config_t* config, uint8_t reg_addr, uint8_t num_bytes, uint8_t* dest);
pca9685_return_code_t pca9685_set_pwm_freq(const pca9685_config_t* config, float freq);
pca9685_return_code_t pca9685_set_raw_pwm(const pca9685_config_t* config, pca9685_channel_t channel, uint16_t on_time, uint16_t off_time);
pca9685_return_code_t pca9685_set_pwm(const pca9685_config_t* config, pca9685_channel_t channel, uint16_t value, bool invert);

#endif /* PCA9685_DRIVER_H_ */