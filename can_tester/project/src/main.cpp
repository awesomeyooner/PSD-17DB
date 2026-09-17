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


int main(int argc, char* argv[])
{
    int s;

    sockaddr_can addr;
    ifreq ifr;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    strcpy(ifr.ifr_name, "can0");
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    bind(s, (struct sockaddr*)&addr, sizeof(addr));

    can_frame frame;

    frame.can_id = 10;
    frame.can_dlc = 8;

    for(int i = 0; i < 8; i++)
    {
        frame.data[i] = i;
    }


    int nbytes = write(s, &frame, sizeof(can_frame));
    
    Logger::info(nbytes);


    return 0;

} // end of "main(int, char*)"
