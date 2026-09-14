#ifndef EEPROM_SPI
#define EEPROM_SPI


#include "EmbeddedLib/status.hpp"
#include "EmbeddedLib/devices/gpio_device.hpp"

#include "spi.h"
#include "gpio.h"

#include <cstdint>
#include <vector>


// Datasheet https://www.st.com/resource/en/datasheet/m95320-w.pdf
class EEPROMSPI
{

    public:

        EEPROMSPI(SPI_HandleTypeDef* spi, GPIO_TypeDef* gpio_family, uint16_t gpio_pin);

        bool is_connected();

        void wait_for_WIP();

        status_utils::StatusCode enable_write();

        status_utils::StatusCode write_bytes(uint16_t address, const std::vector<uint8_t>& bytes);

        std::vector<uint8_t> read_bytes(uint16_t address, uint16_t num_bytes);

    private:

        // WRITE command
        static constexpr uint8_t EEPROM_WRITE = 0x02;

        // READ command
        static constexpr uint8_t EEPROM_READ = 0x03;

        // Read Status Register command
        static constexpr uint8_t EEPROM_RDSR = 0x05;

        // Write Enable command
        static constexpr uint8_t EEPROM_WREN = 0x06;

        SPI_HandleTypeDef* m_spi = nullptr;

        GPIODevice m_cs_pin;

        int m_timeout_ms = 100;

        std::vector<uint8_t> m_write_buffer;

        std::vector<uint8_t> m_read_buffer;

        void select();

        void deselect();

        status_utils::StatusCode send_command(uint8_t command);

        std::vector<uint8_t> create_header(uint8_t command, uint16_t address);

        void set_write_buffer(const std::vector<uint8_t>& bytes);

        // status_utils::StatusCode transmit(const std::vector<uint8_t> bytes);

        // std::vector<uint8_t> receive(uint16_t num_bytes);
        

}; // class EEPROMSPI

#endif // EEPROM_SPI