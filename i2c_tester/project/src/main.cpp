#include <iostream>
#include <vector>

#include "CommiFaceLib/protocols/i2c.hpp"


using namespace status_utils;
using namespace std;


int main(int argc, char* argv[])
{
    I2C::init_name("MCP2221", true);

    I2C device(10);

    vector<uint8_t> bytes = {0, 1, 2, 3};

    StatusCode status = device.transmit_bytes(bytes);

    if(status == StatusCode::OK)
        Logger::info("Good send");
    else
        Logger::error("failed!");

    return 0;

} // end of "main(int, char*)"
