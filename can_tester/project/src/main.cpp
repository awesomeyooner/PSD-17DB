#include <iostream>
#include <vector>
#include <cstring>

#include "plib/util/status.hpp"
#include "plib/util/logger.hpp"

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
    int s;

    sockaddr_can addr;
    ifreq ifr;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if(s < 0)
        Logger::error("socket()");

    strcpy(ifr.ifr_name, "can0");

    if(ioctl(s, SIOCGIFINDEX, &ifr) < 0)
        Logger::error("ioctl()");

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if(bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        Logger::error("bind()");

    can_frame frame;

    frame.can_id = 10;
    frame.can_dlc = 8;

    for(int i = 0; i < 8; i++)
    {
        frame.data[i] = i;
    }

    int nbytes = write(s, &frame, sizeof(frame));

    Logger::info("Wrote: ");
    Logger::info(nbytes);

    can_frame read_frame;

    int read_bytes = read(s, &read_frame, sizeof(read_frame));

    Logger::info("Reading: ");

    Logger::info((int)read_frame.can_id);

    for(int i = 0; i < 8; i++)
    {
        Logger::info(read_frame.data[i]);
    }

    return 0;

} // end of "main(int, char*)"
