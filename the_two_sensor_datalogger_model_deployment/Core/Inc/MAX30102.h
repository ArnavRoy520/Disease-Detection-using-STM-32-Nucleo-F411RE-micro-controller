/*
 * max30100.h
 *
 *  Created on: May 21, 2024
 *      Author: Arnav Roy
 */

#ifndef INC_MAX30100_H_
#define INC_MAX30100_H_

extern I2C_HandleTypeDef hi2c1;

#define MAX30102_ADDR 0x57
#define MAX30102_I2C_TIMEOUT 1000

#define MAX30102_BYTES_PER_SAMPLE 6
#define MAX30102_SAMPLE_LEN_MAX 32

#define MAX30102_INTERRUPT_STATUS_1 0x00
#define MAX30102_INTERRUPT_STATUS_2 0x01
#define MAX30102_INTERRUPT_ENABLE_1 0x02
#define MAX30102_INTERRUPT_ENABLE_2 0x03
#define MAX30102_INTERRUPT_A_FULL 7
#define MAX30102_INTERRUPT_PPG_RDY 6
#define MAX30102_INTERRUPT_ALC_OVF 5
#define MAX30102_INTERRUPT_DIE_TEMP_RDY 1

#define MAX30102_FIFO_WR_PTR 0x04
#define MAX30102_OVF_COUNTER 0x05
#define MAX30102_FIFO_RD_PTR 0x06

#define MAX30102_FIFO_DATA 0x07

#define MAX30102_FIFO_CONFIG 0x08
#define MAX30102_FIFO_CONFIG_SMP_AVE 5
#define MAX30102_FIFO_CONFIG_ROLL_OVER_EN 4
#define MAX30102_FIFO_CONFIG_FIFO_A_FULL 0

#define MAX30102_MODE_CONFIG 0x09
#define MAX30102_MODE_SHDN 7
#define MAX30102_MODE_RESET 6
#define MAX30102_MODE_MODE 0

#define MAX30102_SPO2_CONFIG 0x0a
#define MAX30102_SPO2_ADC_RGE 5
#define MAX30102_SPO2_SR 2
#define MAX30102_SPO2_LEW_PW 0

#define MAX30102_LED_IR_PA1 0x0c
#define MAX30102_LED_RED_PA2 0x0d

#define MAX30102_MULTI_LED_CTRL_1 0x11
#define MAX30102_MULTI_LED_CTRL_SLOT2 4
#define MAX30102_MULTI_LED_CTRL_SLOT1 0
#define MAX30102_MULTI_LED_CTRL_2 0x12
#define MAX30102_MULTI_LED_CTRL_SLOT4 4
#define MAX30102_MULTI_LED_CTRL_SLOT3 0

#define MAX30102_DIE_TINT 0x1f
#define MAX30102_DIE_TFRAC 0x20
#define MAX30102_DIE_TFRAC_INCREMENT 0.0625f
#define MAX30102_DIE_TEMP_CONFIG 0x21
#define MAX30102_DIE_TEMP_EN 1

#define HR_THRESHOLD 57.5

typedef enum max30102_mode_t
{
    max30102_heart_rate = 0x02,
    max30102_spo2 = 0x03,
    max30102_multi_led = 0x07
} max30102_mode_t;

typedef enum max30102_smp_ave_t
{
    max30102_smp_ave_1,
    max30102_smp_ave_2,
    max30102_smp_ave_4,
    max30102_smp_ave_8,
    max30102_smp_ave_16,
    max30102_smp_ave_32,
} max30102_smp_ave_t;

typedef enum max30102_sr_t
{
    max30102_sr_50,
    max30102_sr_100,
    max30102_sr_200,
    max30102_sr_400,
    max30102_sr_800,
    max30102_sr_1000,
    max30102_sr_1600,
    max30102_sr_3200
} max30102_sr_t;

typedef enum max30102_led_pw_t
{
    max30102_pw_15_bit,
    max30102_pw_16_bit,
    max30102_pw_17_bit,
    max30102_pw_18_bit
} max30102_led_pw_t;

typedef enum max30102_adc_t
{
    max30102_adc_2048,
    max30102_adc_4096,
    max30102_adc_8192,
    max30102_adc_16384
} max30102_adc_t;

typedef enum max30102_multi_led_ctrl_t
{
    max30102_led_off,
    max30102_led_red,
    max30102_led_ir
} max30102_multi_led_ctrl_t;

typedef struct max30102_t
{
    I2C_HandleTypeDef *_ui2c;
    uint32_t _ir_samples[32];
    uint32_t _red_samples[32];
    uint8_t _interrupt_flag;
} max30102_t;

float heartRate, spo2rate;
float temperature_compensation_factor;
uint32_t startTime = 0;
uint32_t currentTime = 0;
uint16_t previousTime = 0;
int peakDetected = 0;
float timeInterval;
uint32_t lastPeakTime = 0;
// Function prototypes
uint8_t MAX30102_Init() {
    uint8_t data;
    // Reset the device
    data = 0x40;
    HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR << 1, MAX30102_MODE_CONFIG, 1, &data, 1, HAL_MAX_DELAY);
//    if(HAL_I2C_IsDeviceReady(&hi2c1,MAX30102_ADDR << 1 , 3, 5)!= HAL_OK){
//    	printf("CONNECTION FAILED!\r\n");
//    }
//    else{
//    	printf("CONNECTION SUCCESSFUL!\r\n");
//    }

    // Set LED pulse amplitude
    data = 0x24;
    HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR << 1, MAX30102_LED_IR_PA1, 1, &data, 1, HAL_MAX_DELAY);
    HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR << 1, MAX30102_LED_RED_PA2, 1, &data, 1, HAL_MAX_DELAY);
//    printf("LED Pulse Amplitude is set\r\n");
    MAX30102_DEFAULT();
//    printf("Default settings");
}
// Reset the device
void MAX30102_Reset(void){
	uint8_t data = 0x40;
	HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR << 1, MAX30102_MODE_CONFIG, 1, &data, 1, HAL_MAX_DELAY);
	MAX30102_DEFAULT();
}

//Set the different mode of the device
void MAX30102_set_Mode(uint8_t data){
	HAL_I2C_Mem_Write(&hi2c1,MAX30102_ADDR << 1, MAX30102_MODE_CONFIG, 1, &data, 1, HAL_MAX_DELAY);
}

void MAX30102_SET_LED_PULSE_AMPLTITUDE(){
	//Changing the brightness level of the LED
	uint8_t data = 0x4F;
	HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR << 1, MAX30102_LED_IR_PA1, 1, &data, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR << 1, MAX30102_LED_RED_PA2, 1, &data, 1, HAL_MAX_DELAY);
}

void MAX30102_DEFAULT(){
	//Trigger the FIFO Configuration
	uint8_t trigger_fifo = 0x01;
	HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR << 1, MAX30102_FIFO_CONFIG, 1, &trigger_fifo, 1, HAL_MAX_DELAY);
	//Changing the Simple Average
	uint8_t data = 0x02;
	HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR << 1, MAX30102_FIFO_CONFIG_SMP_AVE, 1, &data, 1, HAL_MAX_DELAY);
	//Trigger the SPO2 Configuration
	uint8_t trigger_spo2 = 0x01;
	HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR << 1, MAX30102_SPO2_CONFIG, 1, &trigger_spo2, 1, HAL_MAX_DELAY);
	//Changing the Sampling Rate
	uint8_t samp_rate = 0x03;
	HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR << 1, MAX30102_SPO2_SR, 1, &samp_rate, 1, HAL_MAX_DELAY);
	//Changing the Pulse Width
	uint8_t pulse_width = 0x03;
	HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR << 1, MAX30102_SPO2_LEW_PW, 1, &pulse_width, 1, HAL_MAX_DELAY);
	//Changing the ADC Range
	uint8_t adc_range = 0x01;
	HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR << 1, MAX30102_SPO2_ADC_RGE, 1, &adc_range, 1, HAL_MAX_DELAY);
}

float readTemperature(void) {
    uint8_t temp_int, temp_frac;
    int8_t temp_int_signed;
    uint8_t data = 0x01;

    // Trigger temperature measurement
    HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR << 1, MAX30102_DIE_TEMP_CONFIG, 1, &data, 1, HAL_MAX_DELAY);

    // Wait for temperature conversion
    HAL_Delay(30);

    // Read temperature integer and fraction parts
    HAL_I2C_Mem_Read(&hi2c1, MAX30102_ADDR << 1, MAX30102_DIE_TINT, 1, &temp_int, 1, HAL_MAX_DELAY);
    HAL_I2C_Mem_Read(&hi2c1, MAX30102_ADDR << 1, MAX30102_DIE_TFRAC, 1, &temp_frac, 1, HAL_MAX_DELAY);

    temp_int_signed = (int8_t)temp_int;
    return (float)temp_int_signed + ((float)temp_frac * MAX30102_DIE_TFRAC_INCREMENT);
}

uint32_t MAX30102_ReadFIFO() {
    uint8_t data[6];
    uint32_t red, ir;

    // Read 6 bytes from the FIFO
    HAL_I2C_Mem_Read(&hi2c1, MAX30102_ADDR << 1, MAX30102_FIFO_DATA, 1, data, 6, HAL_MAX_DELAY);

    // Combine bytes into 18-bit values
    red = (data[0] << 16) | (data[1] << 8) | data[2];
    ir = (data[3] << 16) | (data[4] << 8) | data[5];

    // Remove the unused bits
    red &= 0x03FFFF;
    ir &= 0x03FFFF;

    return (red << 18) | ir;
}

float MAX30102_ReadHeartRate(uint32_t hrdata) {
	MAX30102_Reset();
	MAX30102_SET_LED_PULSE_AMPLTITUDE();
	uint8_t data_6 = 0x02;
	MAX30102_set_Mode(data_6);
//	MAX30102_SET_LED(max30102_led_ir);
    uint32_t fifo_data = MAX30102_ReadFIFO();
    uint32_t ir = fifo_data & 0x03FFFF; // Extract IR data
    heartRate = (float)ir/1000.0; // Replace with proper conversion
    HAL_Delay(750);
    if (heartRate > HR_THRESHOLD) {
    //	        peakDetected = 1;
    	        currentTime = HAL_GetTick(); // Get the current time in milliseconds
    //	        printf("Current Time: %d\r\n", currentTime);

    	        // Calculate the time interval between consecutive R-wave peaks

    	            timeInterval = currentTime - lastPeakTime;
    	            lastPeakTime = currentTime;
//    	            printf("Difference Time: %f\r\n", timeInterval);

    	            // Calculate heart rate (BPM)
    	            heartRate = (60.0 / (timeInterval / 1000.0));
    //	            heartRate = heartRate + 48;
//    	            printf("Heart Rate: %.2f BPM\r\n", heartRate);
    	    }

    	    if (heartRate < HR_THRESHOLD) {
    	        peakDetected = 0;
    	    }
//    heartRate = 60/heartrate;
    	    heartRate = (int)heartRate;
    return heartRate;
}

float MAX30102_ReadSpO2(uint32_t spo2data) {
	MAX30102_Reset();
	MAX30102_SET_LED_PULSE_AMPLTITUDE();
	uint8_t data = 0x03;
	MAX30102_set_Mode(data);
//	MAX30102_SET_LED(max30102_led_red);
    uint32_t fifo_data = MAX30102_ReadFIFO();
    uint32_t red = (fifo_data >> 18) & 0x03FFFF; // Extract Red data
    uint32_t ir = fifo_data & 0x03FFFF;
    spo2rate = (float)ir / ((float)red+(float)ir)*100; // Replace with proper conversion
    float temp_int_signed = readTemperature();
//    printf(" temperature is: %0.2f   ", temp_int_signed);
    temperature_compensation_factor = ((1.0 + (temp_int_signed - 25.0) * 0.01) * ((float)ir / ((float)red+(float)ir))) * 100 ;  // Example adjustment
    if(temperature_compensation_factor > 90){
    	temperature_compensation_factor += 5;
    	if(temperature_compensation_factor >= 100){
    		temperature_compensation_factor = 100;
    	}
    }

    return temperature_compensation_factor;
//    printf("SPO2 level is %0.2f,   ", temperature_compensation_factor);
//    return spo2rate;
}

#endif /* INC_MAX30100_H_ */
