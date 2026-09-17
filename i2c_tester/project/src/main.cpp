#include <iostream>
#include <vector>

#include "CommiFaceLib/protocols/i2c.hpp"


using namespace status_utils;
using namespace std;


int main(int argc, char* argv[])
{
    I2C::init_name("MCP2221", true);

    I2C device(10, 8, 10000);

    // vector<uint8_t> bytes = {100, 9, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

    // bytes.at(1) = bytes.size();

    // StatusCode status = device.transmit_bytes(bytes);

    // if(status == StatusCode::OK)
    //     Logger::info("Good send");
    // else
    //     Logger::error("failed!");

    // auto status = device.write_data<vector<uint8_t>>(100, {});

    // auto bob = device.receive_bytes(4, 10000);

    // uint8_t data[32];

    // auto larry = i2c_read(&device.get_device(), 0, data, 4);

    // Logger::info((int)larry);

    // if(bob.is_OK())
    //     Logger::info("bob");
    // else
    //     Logger::error("shart");

    auto read = device.request_data<double>(101, 500);

    if(read.is_OK())
        Logger::info(read.value);
    else
        Logger::error("shart");

    // vector<uint8_t> bytes = {101, 2};

    // device.transmit_bytes(bytes);

    // auto bob = device.receive_bytes(8, 500);

    // if(bob.is_OK())
    //     Logger::info("yay");
    // else
    //     Logger::error("womp");

    return 0;

} // end of "main(int, char*)"
