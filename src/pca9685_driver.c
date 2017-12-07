/*
 * pca9685_driver.c
 *
 * Created: 11/29/2017 10:34:35 PM
 *  Author: Alex Bennett
 */ 

#include <asf.h>
#include <math.h>
#include "pca9685_driver.h"

pca9685_return_code_t pca9685_init(const pca9685_config_t* config)
{
	// Reset the device
	return pca9685_reset(config);
}

pca9685_return_code_t pca9685_reset(const pca9685_config_t* config)
{
	// Set to MODE1
	return pca9685_write_register(config, PCA9685_MODE1, 0x00);
}

pca9685_return_code_t pca9685_write_register(const pca9685_config_t* config, uint8_t reg_addr, uint8_t data)
{
	// Create data buffer
	uint8_t buf[1] = { data };
		
	// Forward to write register bytes
	return pca9685_write_register_bytes(config, reg_addr, 1, buf);
}

pca9685_return_code_t pca9685_write_register_bytes(const pca9685_config_t* config, uint8_t reg_addr, uint8_t num_bytes, uint8_t* data)
{
	// Create packet
	twihs_packet_t send_packet;
	
	// Set chip address
	send_packet.chip = config->chip_addr;
	
	// Setup register address
	send_packet.addr[0] = reg_addr;
	send_packet.addr_length = 1;
	
	// Setup data
	send_packet.buffer = data;
	send_packet.length = num_bytes;
	
	// Send the packet
	if(twihs_master_write(BOARD_BASE_TWIHS, &send_packet) != TWIHS_SUCCESS)
	{
		return PCA9685_ERROR;
	}
	
	return PCA9685_SUCCESS;
}

pca9685_return_code_t pca9685_read_register(const pca9685_config_t* config, uint8_t reg_addr, uint8_t num_bytes, uint8_t* dest)
{
	// Create packet
	twihs_packet_t receive_packet;
		
	// Set chip address
	receive_packet.chip = config->chip_addr;
		
	// Setup register address
	receive_packet.addr[0] = reg_addr;
	receive_packet.addr_length = 1;
		
	// Setup data return buffer
	receive_packet.buffer = dest;
	receive_packet.length = num_bytes;
		
	// Send the packet
	if(twihs_master_read(BOARD_BASE_TWIHS, &receive_packet) != TWIHS_SUCCESS)
	{
		return PCA9685_ERROR;
	}
		
	return PCA9685_SUCCESS;
}

pca9685_return_code_t pca9685_set_pwm_freq(const pca9685_config_t* config, float freq)
{
	// Create mode buffer
	uint8_t mode_buf[1] = { 0 };
	
	// Correct for overshoot
	freq *= 0.9;
	
	// Create initial prescale value
	float prescale_value = 25000000 / 4096 / freq - 1;
	
	// Create prescale
	uint8_t prescale = floor(prescale_value + 0.5);	
	
	// Grab the current mode
	if(pca9685_read_register(config, PCA9685_MODE1, 1, mode_buf) != PCA9685_SUCCESS) return PCA9685_INIT_ERROR;
	
	// Create the new mode (sleep)
	uint8_t new_mode = (mode_buf[0] & 0x7F) | 0x10;
	
	// Write new mode
	if(pca9685_write_register(config, PCA9685_MODE1, new_mode) != PCA9685_SUCCESS) return PCA9685_INIT_ERROR;
	
	// Set the prescaler
	if(pca9685_write_register(config, PCA9685_PRESCALE, prescale) != PCA9685_SUCCESS) return PCA9685_INIT_ERROR;
	
	// Reset mode
	if(pca9685_write_register(config, PCA9685_MODE1, mode_buf[0]) != PCA9685_SUCCESS) return PCA9685_INIT_ERROR;
	
	// Delay to allow startup
	delay_ms(10);
	
	// Enable auto increment
	if(pca9685_write_register(config, PCA9685_MODE1, mode_buf[0] | 0xA1) != PCA9685_SUCCESS) return PCA9685_INIT_ERROR;
	
	return PCA9685_SUCCESS;	
}

pca9685_return_code_t pca9685_set_raw_pwm(const pca9685_config_t* config, pca9685_channel_t channel, uint16_t on_time, uint16_t off_time)
{
	// Create data packet
	uint8_t data[4] = {
		on_time,
		on_time >> 8,
		off_time,
		off_time >> 8	
	};
	
	// Send data
	if(pca9685_write_register_bytes(config, PCA9685_LED0_ON_L + 4 * channel, 4, data) != PCA9685_SUCCESS) return PCA9685_INIT_ERROR;
	
	return PCA9685_SUCCESS;
}

pca9685_return_code_t pca9685_set_pwm(const pca9685_config_t* config, pca9685_channel_t channel, uint16_t value, bool invert)
{
	// Constrict value
	value = min(value, 4095);
	
	if(invert)
	{
		if(value == 0)
		{
			if(pca9685_set_raw_pwm(config, channel, 4096, 0) != PCA9685_SUCCESS) return PCA9685_ERROR;
		}
		else if(value == 4095)
		{
			if(pca9685_set_raw_pwm(config, channel, 0, 4096) != PCA9685_SUCCESS) return PCA9685_ERROR;
		}
		else
		{
			if(pca9685_set_raw_pwm(config, channel, 0, 4095 - value) != PCA9685_SUCCESS) return PCA9685_ERROR;
		}
	}
	else
	{
		if(value == 0)
		{
			if(pca9685_set_raw_pwm(config, channel, 0, 4096) != PCA9685_SUCCESS) return PCA9685_ERROR;
		}
		else if(value == 4095)
		{
			if(pca9685_set_raw_pwm(config, channel, 4096, 0) != PCA9685_SUCCESS) return PCA9685_ERROR;
		}
		else
		{
			if(pca9685_set_raw_pwm(config, channel, 0, value) != PCA9685_SUCCESS) return PCA9685_ERROR;
		}
	}
	
	return PCA9685_SUCCESS;
}
