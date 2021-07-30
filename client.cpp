#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

#include "TCPClient.hpp"
void routine(uint16_t socket_fd, std::string name, int period)
{
    char    buffer[256];
    int     n;

    bzero(buffer, 256);
    std::string msg = gettimestamp();
    msg.append(" ");
    msg.append(name);
    memmove(buffer, msg.c_str(), msg.length());
    n = write(socket_fd, buffer,strlen(buffer));
    if (n < 0) 
    {
        std::cerr << "ERROR writing to socket" << std::endl;
        throw std::exception();
    }
    close(socket_fd);
    socket_fd = -1;
    sleep(period);
}

int main(int ac, char **av)
{
    if (ac != 4)
    {
        std::cerr << "usage: " << av[0] << " name port period\n";
        exit(1);
    }
    TCPClient c(atoi(av[2]));
    c.start(routine, std::string(av[1]), atoi(av[3]));
}