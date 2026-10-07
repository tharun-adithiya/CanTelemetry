#include "FrameDecoder.hpp"
#include<arpa/inet.h> //for network to host short (ntohs)

bool FrameDecoder::decodePowertrain(const struct can_frame& frame, PowertrainData& out)
{
    if(frame.can_id!=0x100||frame.can_dlc<5) return false;   //Checks if a frame with another ID is paased. Also checks if the DLC is 5 because the message structure requires at least 5 bytes

    out.rpm=static_cast<uint16_t>((frame.data[0]<<8)|frame.data[1]); // first two bytes hold rpm data. We have to merge them to get the full value.
    out.speed_kmh=frame.data[2];
    out.coolant_temp_c=static_cast<uint16_t>(frame.data[3])-40; //subtracting 40 to measure negative temperature upto -40 deg celcius
    
    //Extracting warning flags with bit masks
    uint8_t flags = frame.data[4];  //The 8 bits representing the flags
    out.check_engine=(flags & 0x01)!=0; //bit 0
    out.transmission_fault=(flags & 0x02)!=0; //bit 1
    out.abs_active= (flags & 0x03)!=0; //bit 2

    return true;
}

bool FrameDecoder::decodeSteeringPedal(const struct can_frame& frame, SteeringPedalData& out)
{
    if(frame.can_id!=0x200||frame.can_dlc<5) return false;
    int16_t raw_angle=static_cast<int16_t>((frame.data[0]<<8)|frame.data[1]); //casting to 16 bit int to preserve the negative 2's component value
    out.steering_angle_deg=static_cast<float>(raw_angle)*0.1f; //0.1 is multiplied to yield the physical floating-point angle
    out.throttle_percent=frame.data[2];
    out.brake_percent=frame.data[3];
    switch (frame.data[4])
    {
        case 0: out .gear='P';break;
        case 1: out .gear='R';break;
        case 2: out .gear='N';break;
        case 3: out .gear='D';break;
        default: out.gear='?';break;
    }
    return true;
}