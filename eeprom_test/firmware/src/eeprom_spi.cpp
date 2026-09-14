#include "eeprom_spi.hpp"


using namespace std;
using namespace status_utils;


EEPROMSPI::EEPROMSPI(SPI_HandleTypeDef* spi, GPIO_TypeDef* gpio_family, uint16_t gpio_pin)
    : m_spi(spi),
    m_cs_pin(gpio_family, gpio_pin){}


// Datasheet Page 20 - 6.3.4 SRWD Bit
// b6 to b4 are guarenteed 0, so if we AND the Status Register
// with 0xFF, then if the result isn't == 0xFF then the EEPROM responded
bool EEPROMSPI::is_connected()
{
    uint8_t status_reg = 0xFF;

    select();

    if (send_command(EEPROM_RDSR) == StatusCode::OK)
        HAL_SPI_Receive(&hspi2, &status_reg, sizeof(status_reg), m_timeout_ms);
    else
        return false;

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

} // end of "is_connected()"


// Datasheet Page 20 - 6.3.4 SRWD Bit
// b0 is the Write In Progress Bit
// It is 1 if Write in Progress
// and 0 when it is ready to be written to
void EEPROMSPI::wait_for_WIP()
{
    uint8_t status_reg = 0;

    do 
    {
        // Read the Status Register
        select();
        send_command(EEPROM_RDSR);
        HAL_SPI_Receive(m_spi, &status_reg, sizeof(status_reg), m_timeout_ms);
        deselect();
        
        // Keep looping while WIP bit (b0) is 1 (when write is in progress))
    } 
    while ((status_reg & 0x01) == 0x01); 

} // end of "wait_for_WIP()"


StatusCode EEPROMSPI::enable_write()
{
    select();

    StatusCode status = send_command(EEPROM_WREN);

    deselect();

    return status;

} // end of "enable_write()"


StatusCode EEPROMSPI::write_bytes(uint16_t address, const vector<uint8_t>& bytes)
{
    set_write_buffer(bytes);

    vector<uint8_t> header = create_header(EEPROM_WRITE, address);

    enable_write();

    select();

    // Transmit WRITE command
    HAL_SPI_Transmit(m_spi, header.data(), header.size(), m_timeout_ms);

    // Transmit actual datra
    HAL_SPI_Transmit(m_spi, m_write_buffer.data(), m_write_buffer.size(), m_timeout_ms);

    deselect();

    // Block until WIP is 0
    wait_for_WIP();

    return StatusCode::OK;

} // end of "write_bytes(uint16_t, const vector<uint8_t>&)"


vector<uint8_t> EEPROMSPI::read_bytes(uint16_t address, uint16_t num_bytes)
{
    vector<uint8_t> header = create_header(EEPROM_READ, address);

    select();

    HAL_SPI_Transmit(m_spi, header.data(), header.size(), m_timeout_ms);

    m_read_buffer.resize(num_bytes);

    HAL_SPI_Receive(m_spi, m_read_buffer.data(), num_bytes, m_timeout_ms);

    deselect();

    return m_read_buffer;

} // end of "read_bytes(uint16_t, uint16_t)"


void EEPROMSPI::set_timeout_ms(int timeout_ms)
{
    m_timeout_ms = timeout_ms;
 
} // end of "set_timeout_ms(int)"


int EEPROMSPI::get_timeout_ms()
{
    return m_timeout_ms;

} // end of "get_timeout_ms()"


// Datasheet Page 9 - 3.4 Chip Select
// Pull CS LOW to select
void EEPROMSPI::select()
{
    m_cs_pin.set_low();

} // end of "select()"


// Datasheet Page 9 - 3.4 Chip Select
// Pull CS HIGH to deselect
void EEPROMSPI::deselect()
{
    m_cs_pin.set_high();

} // end of "deselect()"



StatusCode EEPROMSPI::send_command(uint8_t command)
{
    HAL_StatusTypeDef status = HAL_SPI_Transmit(m_spi, (uint8_t*)command, sizeof(command), m_timeout_ms);

    return status == HAL_OK ? StatusCode::OK : StatusCode::FAILED;

} // end of "send_command(uint8_t)"


vector<uint8_t> EEPROMSPI::create_header(uint8_t command, uint16_t address)
{
    vector<uint8_t> header(3);

    header.at(0) = command;

    // MSB (First 8 Bits)
    header.at(1) = (uint8_t)(address >> 8);

    // LSB (Last 8 Bits)
    header.at(2) = (uint8_t)(address & 0xFF);

    return header;
     
} // end of "create_address_header(uint8_t, uint16_t)"


void EEPROMSPI::set_write_buffer(const vector<uint8_t>& bytes)
{
    m_write_buffer.assign(bytes.begin(), bytes.end());

} // end of "set_write_buffer(const vector<uint8_t>&)"