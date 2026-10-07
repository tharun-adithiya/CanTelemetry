#include "CanSocket.hpp"
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

CanSocket::CanSocket() : socket_fd_(-1) {}  //Initializing socket id with -1 states that the object currently holds no active OS resource

CanSocket::~CanSocket()
{
    close();  //closing the socket when object moves out of scope
}

CanSocket::CanSocket(CanSocket&& other) noexcept : socket_fd_(other.socket_fd_) //moves other socket id to this socket
{
    other.socket_fd_=-1;  //resets other socket to -1
}

CanSocket& CanSocket :: operator=(CanSocket&& other) noexcept  //Same move operation with move assignment
{
    if(this!=&other)        //prevents self assignment
    {
        close();  //closing to prevent leaking of whatever resource it was holding, before adapting to the new one
        socket_fd_=other.socket_fd_;
        other.socket_fd_=-1;
    }
    return *this;
}

//Initialization pipeline
bool CanSocket::open(const std::string& interface_name)
{
    //Creates raw socket

    socket_fd_=::socket(PF_CAN, SOCK_RAW, CAN_RAW);   //Asks the linux kernel to allocate a network socket endpoint and return a file descriptor int
    if(socket_fd_<0)  //fall back if failed to create a socket
    {
        std::cerr<<"[Error] failed to create a socket: " << strerror(errno) <<"\n";
        return false;
    }

    //Resolves interface name to index

    struct ifreq ifr;   //used to pass network device configuration between user and kernel space
    std::strncpy(ifr.ifr_name, interface_name.c_str(),IFNAMSIZ-1);
    ifr.ifr_name[IFNAMSIZ-1]='\0'; //marking the last character as null termininator
    if(::ioctl(socket_fd_,SIOCGIFINDEX,&ifr)<0){
        std::cerr<<"[Error] ioctl failed to rsolve interface "<<interface_name<<": "<< strerror(errno)<<"\n";
        close();
        return false;
    }

    // binding the socket

    struct sockaddr_can addr; //Address struct specific to CAN sockets
    std::memset(&addr,0,sizeof(addr));
    addr.can_family=AF_CAN;   //Address family CAN
    addr.can_ifindex=ifr.ifr_ifindex;   //Assigning the interface index from the ioctl.
    if(::bind(socket_fd_,reinterpret_cast<struct sockaddr*>(&addr),sizeof(addr))<0){  //hooks our file descriptor to that specific virtual or physical CAN device
        std::cerr<<"[Error] failed to bind to "<<interface_name<<": "<<strerror(errno)<<"\n";
        close();
        return false;
    }
    return true;
}
void CanSocket::close()
{
    if(socket_fd_>=0)
    {
        ::close(socket_fd_); //Releases the file descriptor in the kernel's file table and unbinds the socket
        socket_fd_=-1; 
    }
}
//Ingesting packets
bool CanSocket::readFrame(struct can_frame& frame)
{
    if(socket_fd_<0) return false; //No socket to read from. So, return false
    ssize_t nbytes=::read(socket_fd_,&frame,sizeof(struct can_frame)); //
    if(nbytes<0)
    {
        std::cerr<<"[Error]Read failed: "<<strerror(errno)<<"\n";
        return false;
    }
    return nbytes==sizeof(struct can_frame); //If the number of bytes read is equal to the size of a CAN frame, return true. Otherwise, return false    
}

