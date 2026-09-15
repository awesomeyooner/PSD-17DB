#include "core.hpp"

#include "gpio.h"
#include "i2c.h"

#include "EmbeddedLib/devices/gpio_device.hpp"

#include "WireLib/communication/protocols/serial_interface.hpp"

#include "ActionLib/ActionManager.hpp"

#include <vector>


using namespace status_utils;
using namespace std;


GPIODevice led = GPIODevice(GPIOC, GPIO_PIN_1);


vector<uint8_t> read_buffer(64);


void init()
{
    HAL_I2C_EnableListen_IT(&hi2c1);

    // while(!Serial.is_connected())
    // {
    //     HAL_Delay(100);
    // }

    // Serial.println("Starting Program!");

    ActionManager::add(
        Action(0.25).link_callback(
            [](double, double) -> StatusedValue<bool>
            {
                Serial.println("Hello");

                return StatusedValue<bool>(false, StatusCode::OK);
            }
        )
    );

} // end of "init()"


void update()
{
    ActionManager::update();
    
} // end of "update()"


void HAL_I2C_AddrCallback(I2C_HandleTypeDef* hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
    led.set_high();

    // Master wants to transmit data
    // if(TransferDirection == I2C_DIRECTION_TRANSMIT)
    //     HAL_I2C_Slave_Seq_Receive_IT(hi2c, read_buffer.data(), read_buffer.size(), I2C_FIRST_AND_LAST_FRAME);
}


void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef* hi2c)
{
    // uint32_t num_bytes = read_buffer.size() - hi2c->XferCount;
    
    // Serial.println(num_bytes);

    HAL_I2C_EnableListen_IT(hi2c);
}