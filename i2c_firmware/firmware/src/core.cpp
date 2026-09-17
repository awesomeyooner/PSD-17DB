#include "core.hpp"

#include "gpio.h"
#include "i2c.h"

#include "EmbeddedLib/devices/gpio_device.hpp"

#include "WireLib/communication/protocols/serial_interface.hpp"
#include "WireLib/communication/protocols/i2c/i2c_interface.hpp"
#include "WireLib/communication/wire_manager.hpp"

#include "ActionLib/ActionManager.hpp"

#include <vector>


using namespace status_utils;
using namespace std;


GPIODevice led = GPIODevice(GPIOC, GPIO_PIN_1);


uint8_t bob[] = {0, 1, 2, 3};


void init()
{
    I2C.set_i2c(&hi2c1);
    I2C.set_max_packet_size(8);
    I2C.set_parse_type(ParseType::PACKET);

    WireManager::attach(I2C);

    RegisterManager::add_command(
        Command<vector<uint8_t>>(
            100, // Register Byte
            [](const vector<uint8_t>& bytes ) -> StatusCode
            {
                ActionManager::add(
                    Action::run_once(
                        [bytes](double)
                        {
                            Serial.println("Received Bytes");

                            for(int i = 0; i < bytes.size(); i++)
                            {
                                Serial.println(bytes.at(i));
                            }    

                            
                        }
                    )
                );

                return StatusCode::OK;
            }
        )
    );

    RegisterManager::add_request(
        Request<double>(
            101, // Register Byte
            []() -> double
            {
                ActionManager::add(
                    Action::run_once(
                        [](double)
                        {
                            Serial.println("Received Bytes");
                        }
                    )
                );

                return System::get_seconds();
            }
        )
    );
    


    // I2C.configure_on_receive(
    //     [](const vector<uint8_t>& bytes) -> StatusCode
    //     {
    //         ActionManager::add(
    //             Action::run_once(
    //                 [bytes](double)
    //                 {
    //                     Serial.println("Received Bytes");

    //                     for(int i = 0; i < bytes.size(); i++)
    //                     {
    //                         Serial.println(bytes.at(i));
    //                     }    
    //                 }
    //             )
    //         );

    //         return StatusCode::OK;
    //     }
    // );


    ActionManager::add(
        Action(0.5).link_callback(
            [](double, double) -> StatusedValue<bool>
            {
                // led.toggle();

                return StatusedValue<bool>(false, StatusCode::OK);
            }
        )
    );

} // end of "init()"


void update()
{
    ActionManager::update();
    
} // end of "update()"