#include "CanSocket.hpp"
#include "FrameDecoder.hpp"
#include <iostream>
#include <iomanip>

int main()
{
    //Instatiating and binding
    CanSocket can;
    const std::string iface="vcan0";
    std::cout<<"Attempting to bind "<<iface<<"...\n";
    if(!can.open(iface))
    {
        std::cerr<<"Initialization failed. Ensure vcan0 is active vial 'sudo ip link set up vcan0'\n";
        return 1;
    }
    
    //Reception Loop
    std::cout<<"Listening for CAN frames on "<<iface<<" (Press Ctrl+C to stop)";
    struct can_frame frame; 
    PowertrainData pt;
    SteeringPedalData sp;
    while(can.readFrame(frame)) //Performs a POSIX system call to read canFrame. If no frame, this process will be put to sleep
    {
        if(FrameDecoder::decodePowertrain(frame, pt))
        {
            std::cout<<"\n[ID 0x100 POWERTRAIN] "
                     <<"RPM:"<<pt.rpm<<" | "
                     << "Speed: " << static_cast<int>(pt.speed_kmh) << " km/h | "
                      << "Coolant: " << pt.coolant_temp_c << " °C | "
                      << "Warnings: "
                      << (pt.check_engine ? "[CHECK ENGINE] " : "")
                      << (pt.transmission_fault ? "[TRANS FAULT] " : "")
                      << (pt.abs_active ? "[ABS ACTIVE]" : "None")
                      << "\n";
        } 
        else if (FrameDecoder::decodeSteeringPedal(frame, sp)) {
            std::cout << "\n[0x200 CHASSIS]    "
                      << "Steering: " << std::fixed << std::setprecision(1) << sp.steering_angle_deg << "° | "
                      << "Throttle: " << static_cast<int>(sp.throttle_percent) << "% | "
                      << "Brake: " << static_cast<int>(sp.brake_percent) << "% | "
                      << "Gear: [" << sp.gear << "]\n";
        }

        /*std::cout<<"CAN ID: 0x" << std::hex<<std::uppercase<<std::setw(3)
            <<std::setfill('0')<<frame.can_id<<"| DLC: "<<std::dec
            <<static_cast<int>(frame.can_dlc)<<"| Payload: ";
        //Dumping the payload bytes
        for(int i = 0; i<frame.can_dlc;i++)
        {
            std::cout<<std::hex<<std::setw(2)<< std::setfill('0')
            <<static_cast<int>(frame.data[i])<<" ";
        }
        std::cout<<std::dec<<"\n";*/
    }
    
    return 0;
}
