#include <iostream>
#include <vector>
#include <cstring>

#include "plib/util/status.hpp"
#include "plib/util/logger.hpp"

#include "CommiFaceLib/protocols/can.hpp"

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>


using namespace status_utils;
using namespace std;

// sudo apt update && sudo apt install can-utils
// sudo ip link set can0 up type can bitrate 500000
// 500kbit/s is standard
// ip link show type can
int main(int argc, char* argv[])
{
    CAN::open("can0", true);

    CAN device = CAN(10);

    StatusCode transmit = device.transmit_bytes({0, 1, 2, 3, 4, 5, 6, 7});

    if(transmit != StatusCode::OK)
        Logger::error("Transmit Failed!");

    StatusedValue<vector<uint8_t>> receive = device.receive_bytes(8);

    if(!receive.is_OK())
        Logger::error("Receive Failed!");

    for(int i = 0; i < receive.value.size(); i++)
    {
        Logger::info(receive.value.at(i));
    }

    CAN::close_socket();

    return 0;

} // end of "main(int, char*)"
