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

        /**
         * @brief Create a new EEPROM object on the given SPI line and CS pin
         * 
         * @param spi `SPI_HandleTypeDef*` Pointer to the SPI line to use, like `&hspi1` for `SPI1`
         * @param gpio_family `GPIO_TypeDef*` The GPIO Family the CS pin is on, like `GPIOB` for `PBxx`
         * @param gpio_pin `uint16_t` The GPIO pin the CS pin is on, like `GPIO_PIN_11`, for `PB11`
         */
        EEPROMSPI(SPI_HandleTypeDef* spi, GPIO_TypeDef* gpio_family, uint16_t gpio_pin);

        /**
         * @brief Gets if the EEPROM is connected or not by reading the Status Register
         * 
         * @return `true` 
         * @return `false` 
         */
        bool is_connected();

        /**
         * @brief Block until the WIP bit is 0 (write is ready)
         * 
         */
        void wait_for_WIP();

        /**
         * @brief Send the WRITE ENABLE command
         * 
         * @return `status_utils::StatusCode` OK if SPI transmit was successful. FAILED otherwise 
         */
        status_utils::StatusCode enable_write();

        /**
         * @brief Write the given bytes to the address of the EEPROM
         * 
         * @param address `uint16_t` Address of the starting byte
         * @param bytes `const std::vector<uint8_t>&` The data to send
         * @return `status_utils::StatusCode` OK if SPI transmit was successful. FAILED otherwise 
         */
        status_utils::StatusCode write_bytes(uint16_t address, const std::vector<uint8_t>& bytes);

        /**
         * @brief Read a certain amount of bytes starting at the given address
         * 
         * @param address `uint16_t`
         * @param num_bytes `uint16_t`
         * @return `std::vector<uint8_t>` 
         */
        std::vector<uint8_t> read_bytes(uint16_t address, uint16_t num_bytes);

        /**
         * @brief Set the timeout for SPI transactions in milliseconds
         * 
         * @param timeout_ms `int` 
         */
        void set_timeout_ms(int timeout_ms);

        /**
         * @brief Get the timeout for SPI transactions in milliseconds
         * 
         * @return `int` 
         */
        int get_timeout_ms();

    private:

        // WRITE command
        static constexpr uint8_t EEPROM_WRITE = 0x02;

        // READ command
        static constexpr uint8_t EEPROM_READ = 0x03;

        // Read Status Register command
        static constexpr uint8_t EEPROM_RDSR = 0x05;

        // Write Enable command
        static constexpr uint8_t EEPROM_WREN = 0x06;

        // SPI Handle for transactions
        SPI_HandleTypeDef* m_spi = nullptr;

        // Chip Select pin
        GPIODevice m_cs_pin;

        // Default timeout for SPI transactions in milliseconds
        int m_timeout_ms = 100;

        std::vector<uint8_t> m_write_buffer;

        std::vector<uint8_t> m_read_buffer;

        void select();

        void deselect();

        status_utils::StatusCode send_command(uint8_t command);

        std::vector<uint8_t> create_header(uint8_t command, uint16_t address);

        void set_write_buffer(const std::vector<uint8_t>& bytes);
        

}; // class EEPROMSPI

#endif // EEPROM_SPI