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


void EEPROM_write_enable()
{
    uint8_t command = EEPROM_WREN;

    select();

    HAL_SPI_Transmit(&hspi2, &command, sizeof(command), HAL_MAX_DELAY);
    
    deselect();
     
} // end of "EEPROM_write_enable()"


// Datasheet Page 23 - 6.5 READ
// Instruction is first, then 16 bit address (MSB First)
void EEPROM_read_bytes(uint16_t address, uint8_t* read_buffer, uint16_t num_bytes)
{
    uint8_t command[3];

    command[0] = EEPROM_READ;

    // MSB (First 8 Bits)
    command[1] = (uint8_t)(address >> 8);

    // LSB (Last 8 Bits)
    command[2] = (uint8_t)(address & 0xFF);

    // Select SPI
    select();

    // Transmit the READ command
    HAL_SPI_Transmit(&hspi2, command, sizeof(command), HAL_MAX_DELAY);

    // Receive however many bytes
    HAL_SPI_Receive(&hspi2, read_buffer, num_bytes, HAL_MAX_DELAY);

    // Deselect SPI
    deselect();

} // end of "EEPROM_read_bytes"


// Datasheet Page 20 - 6.3.4 SRWD Bit
// b0 is the Write In Progress Bit
// It is 1 if Write in Progress
// and 0 when it is ready to be written to
void EEPROM_wait_ready()
{
    uint8_t command = EEPROM_RDSR;
    uint8_t status_reg = 0;

    do 
    {
        // Read the Status Register
        select();
        HAL_SPI_Transmit(&hspi2, &command, sizeof(command), 100);
        HAL_SPI_Receive(&hspi2, &status_reg, sizeof(status_reg), 100);
        deselect();
        
        // Keep looping while WIP bit (b0) is 1 (when write is in progress))
    } 
    while ((status_reg & 0x01) == 0x01); 

} // end of "EEPROM_wait_ready()"


// Datasheet Page 24 - 6.6 WRITE
// Instruction is first, then 16 bit address (MSB First)
void EEPROM_write_bytes(uint16_t address, uint8_t* write_buffer, uint16_t num_bytes)
{
    uint8_t command[3];

    command[0] = EEPROM_WRITE;

    // MSB (First 8 Bits)
    command[1] = (uint8_t)(address >> 8);

    // LSB (Last 8 Bits)
    command[2] = (uint8_t)(address & 0xFF);

    EEPROM_write_enable();
    
    select();

    HAL_SPI_Transmit(&hspi2, command, sizeof(command), HAL_MAX_DELAY);

    HAL_SPI_Transmit(&hspi2, write_buffer, num_bytes, HAL_MAX_DELAY);

    deselect();
    
    // HAL_Delay(5);
    EEPROM_wait_ready();

} // end of "EEPROM_write_bytes()"


// Datasheet Page 20 - 6.3.4 SRWD Bit
// b6 to b4 are guarenteed 0, so if we AND the Status Register
// with 0xFF, then if the result isn't == 0xFF then the EEPROM responded
bool EEPROM_is_connected()
{
    uint8_t command = EEPROM_RDSR;
    uint8_t status_reg = 0xFF;

    select();

    if (HAL_SPI_Transmit(&hspi2, &command, sizeof(EEPROM_RDSR), 100) == HAL_OK)
        HAL_SPI_Receive(&hspi2, &status_reg, sizeof(status_reg), 100);

    deselect();

    // If nothing changed
    // Then the EEPROM didn't respond
    if(status_reg == 0xFF)
        return false;
    
    // If bits b6 -  b4 aren't 0'd
    // Then the EEPROM didn't respond
    // Since Status Register guarentees them to be zero'd
    if((status_reg & 0x70) != 0x00)
        return false;

    return true;

} // end of "EEPROM_is_connected()"


GPIODevice led = GPIODevice(GPIOC, GPIO_PIN_1);


void init()
{
    while(!Serial.is_connected())
    {
        HAL_Delay(100);
    }

    if(!EEPROM_is_connected())
        led.set_high();

    double data = 150.15;

    EEPROM_write_bytes(0, (uint8_t*)&data, sizeof(double));

    // EEPROM_wait_ready();

    double read = 0;

    EEPROM_read_bytes(0, (uint8_t*)&read, sizeof(double));

    HAL_Delay(500);

    Serial.println("Hello World!");
    Serial.println(read, 9);

} // end of "init()"


void update()
{

    
} // end of "update()"
