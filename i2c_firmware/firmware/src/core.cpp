#include "core.hpp"

#include "gpio.h"


void init()
{

} // end of "init()"


void update()
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_Delay(500);
    
} // end of "update()"
