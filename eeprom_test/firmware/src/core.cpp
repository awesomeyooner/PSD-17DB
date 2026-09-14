#include "core.hpp"

#include "gpio.h"
#include "spi.h"

#include <cstdint>

#include "WireLib/communication/protocols/serial_interface.hpp"

#include "EmbeddedLib/devices/gpio_device.hpp"

#include "WireLib/util/byte_converter.hpp"

#include "eeprom_spi.hpp"


GPIODevice led = GPIODevice(GPIOC, GPIO_PIN_1);
EEPROMSPI eeprom = EEPROMSPI(&hspi2, GPIOB, GPIO_PIN_12);


void init()
{
    while(!Serial.is_connected())
    {
        HAL_Delay(100);
    }

    if(!eeprom.is_connected())
        led.set_high();

    double data = 150.15;

    eeprom.write_bytes(0, ByteConverter::to_bytes<double>(data));

    double read = ByteConverter::from_bytes<double>(
        eeprom.read_bytes(0, sizeof(double))
    );

    HAL_Delay(500);

    Serial.println("Hello World!");
    Serial.println(read, 9);

} // end of "init()"


void update()
{

    
} // end of "update()"
