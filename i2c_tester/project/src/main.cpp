#include <iostream>

#include "CommiFaceLib/protocols/i2c.hpp"


int main(int argc, char* argv[])
{
    I2C::init_name("MCP2221", true);

    std::cout << "Hello World!" << std::endl;

    return 0;

} // end of "main(int, char*)"
