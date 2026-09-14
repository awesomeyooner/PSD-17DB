#include "core.hpp"

#include "gpio.h"
#include "spi.h"

#include <cstdint>

#include "WireLib/communication/protocols/serial_interface.hpp"

#include "EmbeddedLib/devices/gpio_device.hpp"


// https://www.st.com/resource/en/datasheet/m95320-w.pdf
// Data Sheet Page 16 - Instructions (6)
#define EEPROM_WREN  0x06
#define EEPROM_WRITE 0x02
#define EEPROM_READ  0x03
#define EEPROM_RDSR 0x05

#define CS_FAMILY GPIOB
#define CS_PIN GPIO_PIN_12


void select()
{
    HAL_GPIO_WritePin(CS_FAMILY, CS_PIN, GPIO_PIN_RESET);

} // end of "select()"


void deselect()
{
    HAL_GPIO_WritePin(CS_FAMILY, CS_PIN, GPIO_PIN_SET);

} // end of "select()"


void EEPROM_WriteEnable()
{
    uint8_t command = EEPROM_WREN;

    select();

    HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
    
    deselect();
     
} // end of "EEPROM_WriteEnable"


void EEPROM_ReadBuffer(uint16_t address, uint8_t *pBuffer, uint16_t size) {
    uint8_t header[3];
    header[0] = EEPROM_READ;
    header[1] = (uint8_t)(address >> 8);   // Address MSB
    header[2] = (uint8_t)(address & 0xFF);  // Address LSB

    select();
    // Transmit 3-byte command frame header
    HAL_SPI_Transmit(&hspi2, header, 3, HAL_MAX_DELAY);
    // Receive the requested data bytes immediately after
    HAL_SPI_Receive(&hspi2, pBuffer, size, HAL_MAX_DELAY);
    deselect();
}


void EEPROM_WritePage(uint16_t address, uint8_t *pData, uint16_t size) {
    uint8_t header[3];
    header[0] = EEPROM_WRITE;
    header[1] = (uint8_t)(address >> 8);
    header[2] = (uint8_t)(address & 0xFF);

    EEPROM_WriteEnable(); // Must be set before every write command
    
    select();
    HAL_SPI_Transmit(&hspi2, header, 3, HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi2, pData, size, HAL_MAX_DELAY);
    deselect();
    
    HAL_Delay(5); // Wait 5ms for physical EEPROM programming cycle
}


bool EEPROM_is_connected()
{
    uint8_t command = EEPROM_RDSR;
    uint8_t status_reg = 0xFF;

    select();

    if (HAL_SPI_Transmit(&hspi2, &command, 1, 100) == HAL_OK) {
        HAL_SPI_Receive(&hspi2, &status_reg, 1, 100);
    }

    deselect();

    if(status_reg == 0xFF)
        return false;
    
    if((status_reg & 0x70) != 0x00)
        return false;

    return true;
}


bool EEPROM_wait_ready()
{
    uint8_t command = EEPROM_RDSR;
    uint8_t status_reg;

    do {
        select();
        HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
        HAL_SPI_Receive(&hspi2, &status_reg, 1, HAL_MAX_DELAY);
        deselect();
        
        // Loop while Bit 0 (WIP) is 1
    } while ((status_reg & 0x01) == 0x01); 
}


GPIODevice led = GPIODevice(GPIOC, GPIO_PIN_1);


void init()
{
    while(!Serial.is_connected())
    {
        HAL_Delay(100);
    }

    if(!EEPROM_is_connected())
        led.set_high();

    double data = 3.14159;

    EEPROM_WritePage(0, (uint8_t*)&data, sizeof(double));

    // EEPROM_wait_ready();

    double read = 0;

    EEPROM_ReadBuffer(0, (uint8_t*)&read, sizeof(double));

    HAL_Delay(500);

    Serial.println("Hello World!");
    Serial.println(read, 9);

} // end of "init()"


void update()
{

    
} // end of "update()"
