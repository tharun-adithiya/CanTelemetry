#pragma once
#include <string>
#include <linux/can.h>
#include <linux/can/raw.h>

class CanSocket
{
    public:
        CanSocket();
        ~CanSocket();

        //Rule of 5

        //Prevents copies to safely manage thr raw socket file descriptor
        CanSocket(const CanSocket&)=delete;
        CanSocket& operator = (const CanSocket&) = delete;

        //Allows move semantics. Useful to prevent unneccessary deep copies
        CanSocket(CanSocket&& other) noexcept;
        CanSocket& operator = (CanSocket&& other) noexcept;
        
        //Core interface methods
        bool open(const std::string& interface_name);
        void close();
        bool readFrame(struct can_frame& frame);

        private:
            int socket_fd_{-1};
};