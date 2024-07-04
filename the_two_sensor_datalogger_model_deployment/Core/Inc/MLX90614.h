#ifndef INC_MLX90614_H_
#define INC_MLX90614_H_

#include "stm32f4xx_hal.h"

extern I2C_HandleTypeDef hi2c2;

// MLX90614 I2C Address
#define MLX90614_ADDR 0x5A

// MLX90614 Commands
#define MLX90614_TA 0x06
#define MLX90614_TOBJ1 0x07
#define MLX90614_TOBJ2 0x08

// Function Declarations
void MLX90614_Init(void);
float MLX90614_ReadTempAmbient(void);
float MLX90614_ReadTempObject1(void);
float MLX90614_ReadTempObject2(void);

// Function Definitions
void MLX90614_Init(void) {
    // No special initialization required for MLX90614
}

float MLX90614_ReadTemp(uint8_t reg) {
    uint8_t buffer[3];
    uint16_t data;
    float temp;

    HAL_I2C_Mem_Read(&hi2c2, MLX90614_ADDR << 1, reg, 1, buffer, 3, HAL_MAX_DELAY);

    data = (buffer[1] << 8) | buffer[0];
    temp = (data * 0.02) - 273.15;

    return temp;
}

float MLX90614_ReadTempAmbient(void) {
    return MLX90614_ReadTemp(MLX90614_TA);
}

float MLX90614_ReadTempObject1(void) {
    return MLX90614_ReadTemp(MLX90614_TOBJ1);
}

float MLX90614_ReadTempObject2(void) {
    return MLX90614_ReadTemp(MLX90614_TOBJ2);
}

#endif /* INC_MLX90614_H_ */
