#pragma once
#include<linux/can.h>
#include<cstdint>
#include<string>

struct PowertrainData  
{
    uint16_t rpm{0};
    uint8_t speed_kmh{0};
    uint16_t coolant_temp_c{0};
    bool check_engine{false};
    bool transmission_fault{false};
    bool abs_active{false};
};
struct SteeringPedalData
{
    float steering_angle_deg{0.0f};
    uint8_t throttle_percent{0};
    uint8_t brake_percent{0};
    char gear{'P'};
};
class FrameDecoder
{
    public:
        static bool decodePowertrain(const struct can_frame& frame, PowertrainData& out); //Outputs the data on successfull CAN frame read
        static bool decodeSteeringPedal(const struct can_frame& frame, SteeringPedalData& out); 
};